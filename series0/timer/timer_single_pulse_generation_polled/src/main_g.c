/***************************************************************************//**
 * @file main_g.c
 * @brief This project demonstrates polled pulse generation via output compare.
 * The GPIO pin specified in the readme.txt is configured for output and
 * generates a single 1 ms pulse.
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

// Compare values set to generate a 1 ms pulse with default clock and prescale
#define COMPARE_VALUE1 65535
#define COMPARE_VALUE2 14000

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO and clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PD1 as output
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Configure TIMER0 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle;
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // Set route to Location 3 and enable
  // TIM1_CC0 #3 is PD1
  TIMER0->ROUTE |= (TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC3);

  // Set first CCV value to trigger rising edge
  TIMER_CompareSet(TIMER0, 0, COMPARE_VALUE1);

  // Set buffered value to next be loaded into the CCV after overflow event
  TIMER_CompareBufSet(TIMER0, 0, COMPARE_VALUE2);

  // Initialize and start timer with default settings
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  TIMER_Init(TIMER0, &timerInit);

  // Safely enable TIMER0 CC0 interrupt flag
  TIMER_IntClear(TIMER0, TIMER_IFC_CC0);
  NVIC_DisableIRQ(TIMER0_IRQn);
  TIMER_IntEnable(TIMER0, TIMER_IEN_CC0);
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
  initGpio();
  initTimer();

  // Wait for rising edge
  while(!(TIMER_IntGet(TIMER0) & TIMER_IF_CC0));
  TIMER_IntClear(TIMER0, TIMER_IFC_CC0);

  // Disable timer after falling edge
  while(!(TIMER_IntGet(TIMER0) & TIMER_IF_CC0));
  TIMER_IntClear(TIMER0, TIMER_IFC_CC0);
  TIMER_Enable(TIMER0, false);

  while(1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}


