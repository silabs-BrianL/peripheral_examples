/***************************************************************************//**
 * @file main_gg11_tg11.c
 * @brief This project demonstrates the generation of a single pulse using
 * output compare. The TIMER module is configured for output compare such that a
 * 1ms pulse is generated on the GPIO pin specified in the readme.txt after a 1
 * second delay.
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

// Note: change this to change the number of seconds to delay the pulse going high
#define NUM_SEC_DELAY 1

// Note: change this to change the timer prescale
#define TIMER_PRESCALE timerPrescale1024

// Note: change this to change the pulse width (in units of ms)
#define PULSE_WIDTH 1

// Compare values for outputting the rising and falling edge
static uint32_t compareValue1;
static uint32_t compareValue2;

/**************************************************************************//**
 * @brief
 *    WTIMER0 handler
 *****************************************************************************/
void WTIMER0_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(WTIMER0);
  TIMER_IntClear(WTIMER0, flags);

  // Load compare register with second compare value
  TIMER_CompareSet(WTIMER0, 0, compareValue2);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PC1 as output
  GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initWtimer(void)
{
  // Enable clock for WTIMER0 module
  CMU_ClockEnable(cmuClock_WTIMER0, true);

  // Configure WTIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle; // Toggle output on compare match
  TIMER_InitCC(WTIMER0, 0, &timerCCInit);

  // Set route to Location 7 and enable
  // WTIM0_CC0 #0 is PC1
  WTIMER0->ROUTELOC0 |=  TIMER_ROUTELOC0_CC0LOC_LOC7;
  WTIMER0->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;
  
  // Initialize and start timer with highest prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.enable = false;
  timerInit.prescale = TIMER_PRESCALE;
  timerInit.oneShot = true; // Generate only one pulse
  TIMER_Init(WTIMER0, &timerInit);

  // Set the first compare value
  compareValue1 = CMU_ClockFreqGet(cmuClock_WTIMER0)
                    * NUM_SEC_DELAY
                    / (1 << TIMER_PRESCALE);
  TIMER_CompareSet(WTIMER0, 0, compareValue1);

  // Set the second compare value (don't actually use it, just set the global so
  // that it can be used by the handler later)
  compareValue2 = (CMU_ClockFreqGet(cmuClock_WTIMER0)
                    * PULSE_WIDTH
                    / 1000
                    / (1 << TIMER_PRESCALE))
                    + compareValue1;

  // Enable WTIMER0 interrupts
  TIMER_IntEnable(WTIMER0, TIMER_IEN_CC0);
  NVIC_EnableIRQ(WTIMER0_IRQn);

  // Enable the TIMER
  TIMER_Enable(WTIMER0, true);
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
  initWtimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

