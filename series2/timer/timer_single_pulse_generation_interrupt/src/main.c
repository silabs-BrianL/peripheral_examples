/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates the generation of a single pulse using
 * output compare. The TIMER module is configured for output compare such that a
 * 1 second pulse is generated on the GPIO pin specified in the readme.txt after
 * a 1 second delay.
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

// Note: change this to change the pulse width (in units of s)
#define PULSE_WIDTH 1

// Compare values for outputting the rising and falling edge
static uint32_t compareValue1;
static uint32_t compareValue2;

/**************************************************************************//**
 * @brief
 *    TIMER0 handler
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = TIMER_IntGet(TIMER0);
  TIMER_IntClear(TIMER0, flags);

  // Load compare register with second compare value
  TIMER_CompareSet(TIMER0, 0, compareValue2);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Configure PA6 as output
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    CMU initialization
 *****************************************************************************/
void initCmu(void)
{
  // Enable clock to GPIO and TIMER0
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  uint32_t timerFreq = 0;
  // Initialize the timer
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  // Configure TIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;

  timerInit.enable = false;
  timerInit.prescale = timerPrescale1024;
  timerInit.oneShot = true; // Generate only one pulse
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle; // Toggle output on compare match

  TIMER_Init(TIMER0, &timerInit);

  // Route Timer0 CC0 output to PA6
  GPIO->TIMERROUTE[0].ROUTEEN  = GPIO_TIMER_ROUTEEN_CC0PEN;
  GPIO->TIMERROUTE[0].CC0ROUTE = (gpioPortA << _GPIO_TIMER_CC0ROUTE_PORT_SHIFT)
                    | (6 << _GPIO_TIMER_CC0ROUTE_PIN_SHIFT);

  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  timerFreq = CMU_ClockFreqGet(cmuClock_TIMER0) / (timerInit.prescale + 1);

  // Set the first compare value
  compareValue1 = timerFreq * NUM_SEC_DELAY;

  // Set compare value to the first compare value
  TIMER_CompareSet(TIMER0, 0, compareValue1);

  // Set the second compare value (don't actually use it, just set the global so
  // that it can be used by the handler later)
  compareValue2 = (timerFreq * PULSE_WIDTH) + compareValue1;

  // Enable TIMER0 interrupts
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
  NVIC_EnableIRQ(TIMER0_IRQn);

  // Enable the TIMER
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initializations
  initCmu();
  initGpio();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

