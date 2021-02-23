/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the use of the WTIMER module for interrupt
 * based input capture. After button 0 is pressed, the PRS routes this signal to
 * the timer to indicate that an input capture needs to occur. An interrupt then
 * occurs because the Compare/Capture channel interrupt flag is set. The 
 * captured timer value is then stored in the user's buffer.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_prs.h"
#include "bsp.h"

// Buffer size
#define BUFFER_SIZE 8

// Buffer to hold input capture values
// Note: needs to be volatile or else the compiler will optimize it out
static volatile uint32_t buffer[BUFFER_SIZE];

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *
 * @details
 *    Since the external interrupts are asynchronous, they are sensitive to
 *    noise. As a result, the filter option is enabled for the button 0 GPIO
 *    pin.
 *
 * @note
 *    Furthermore, the button needs to be pulled up to prevent unintended
 *    triggers to the PRS. During testing, not pulling up the button resulted in
 *    triggering a capture event before the button was even pressed.
 *
 * @note
 *    We need to configure an external interrupt using GPIO_ExtIntConfig()
 *    since the GPIO_EXTIPSEL[3:0] bits tell the PRS which port to listen to.
 *    PRS_SourceSignalSet() tells the PRS it needs to route an incoming GPIO
 *    signal on a specific pin, but it doesn't tell the PRS which port to listen
 *    to. Thus, we use GPIO_ExtIntConfig() to trigger an interrupt flag, but we
 *    don't actually need an interrupt handler so we won't enable GPIO
 *    interrupts and won't enable either edge.
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure button 0 as input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Select button 0 as the interrupt source (configure as disabled since using PRS)
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, false, false, false);
}

/**************************************************************************//**
 * @brief
 *    PRS initialization
 *
 * @details
 *    prsEdgeOff is chosen because GPIO produces a level signal and the timer
 *    CC0 input can accept either a pulse or level. Thus, we do not need the PRS
 *    module to generate a pulse from the GPIO level signal (we can just leave
 *    it as it is). See the PRS chapter in the reference manual for further
 *    details on Producers and Consumers.
 *****************************************************************************/
void initPrs(void)
{
  // Enable PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Select GPIO as source and button 0 GPIO pin as signal for PRS channel 0
  // Note that the PRS Channel Source Select splits LOWER (0-7) and HIGHER (8-15) GPIO pins
  if (BSP_GPIO_PB0_PIN < 8) {
    PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOL, BSP_GPIO_PB0_PIN, prsEdgeOff);
  } else {
    PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, (uint32_t)(BSP_GPIO_PB0_PIN - 8), prsEdgeOff);
  }
}

/**************************************************************************//**
 * @brief
 *    WTIMER initialization
 *
 * @note
 *    The timer prescale value is unnecessary. It's simply there to more easily
 *    show that the timer values are being captured.
 *****************************************************************************/
void initWtimer(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  // Configure WTIMER0 Compare/Capture for input capture of PRS channel 0
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.prsSel = timerPRSSELCh0;
  timerCCInit.edge = timerEdgeFalling;
  timerCCInit.mode = timerCCModeCapture;
  timerCCInit.prsInput = true;
  TIMER_InitCC(WTIMER0, 0, &timerCCInit);

  // Initialize timer with largest prescale setting
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale1024;
  TIMER_Init(WTIMER0, &timerInit);

  // Enable WTIMER0 interrupts for Capture/Compare on channel 0
  TIMER_IntEnable(WTIMER0, TIMER_IEN_CC0);
  NVIC_EnableIRQ(WTIMER0_IRQn);
}

/**************************************************************************//**
 * @brief
 *    WTIMER0 handler
 *****************************************************************************/
void WTIMER0_IRQHandler(void)
{
  static uint32_t i = 0;

  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(WTIMER0);
  TIMER_IntClear(WTIMER0, flags);

  // Check for capture event on channel 0
  if (flags & TIMER_IF_CC0) {

    // Record input capture value
    buffer[i] = TIMER_CaptureGet(WTIMER0, 0);

    // Increment index and have it wrap around
    i = (i + 1) % BUFFER_SIZE;
  }
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initializations
  initGpio();
  initPrs();
  initWtimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

