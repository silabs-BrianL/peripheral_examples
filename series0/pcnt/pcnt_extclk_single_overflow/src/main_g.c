/***************************************************************************//**
 * @file main_g.c
 * @brief This project demonstrates the single external clock input oversampling
 *  mode of the pulse counter with interrupts. In this example, Push Button PB0
 * is the clock source and each clock cycle is generated by the press and
 * release of the button. The program generates an interrupt whenever the pulse
 * counter detects the number of pulses that went above a threshold set by user.
 * This source file is for Gecko Gxxx devices.
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

#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_pcnt.h"

#include "bsp.h"

#include <stdint.h>
#include <stdbool.h>

/***************************************************************************//**
 * @brief PCNT0 interrupt handler
 *        This function acknowleges the interrupt request and toggles LED0
 ******************************************************************************/ 
void PCNT0_IRQHandler(void)
{
  /* Acknowledge interrupt */	
  PCNT_IntClear(PCNT0, PCNT_IFC_OF);

  /* Toggle LED0 */
  GPIO_PinOutToggle(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
}

/***************************************************************************//**
 * @brief GPIO odd interrupt handler
 *        This function acknowledges odd pin number interrupt and
 *        toggles PC5 output
 ******************************************************************************/ 
void GPIO_ODD_IRQHandler(void)
{
  /* Acknowledge all odd number pin interrupt */
  GPIO_IntClear(0xAAAA);

  /* Toggle PC5 */
  GPIO_PinOutToggle(gpioPortC, 5);
}

/***************************************************************************//**
 * @brief Initialize PCNT
 *        This function initializes pulse counter 0 and set up the
 *        external single mode. PCNT0 overflow interrupt is configured 
 *        in this function also
 ******************************************************************************/ 
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode     = pcntModeExtSingle;        // Use External Clock source for S0
  pcntInit.top      = 5;                        // Overflow at every PC5 toggle
  pcntInit.counter  = 0;

  /* Init PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Enable Interrupt */
  PCNT_IntEnable(PCNT0, PCNT_IEN_OF);

  /* route PCNT0 to location 1 where PC0 is */
  PCNT0 -> ROUTE = PCNT_ROUTE_LOCATION_LOC1;
}

/***************************************************************************//**
 * @brief Initialize GPIO
 *        This function initializes push button PB0 and enables
 *        external interrupt for PRS functionality. 
 *        This function also initializes PC5 for output
 ******************************************************************************/ 
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);

  /* Configure pin I/O - PE0 */
  GPIO_PinModeSet(gpioPortE, 0, gpioModeInputPull, 0);

  /* Configure pin input for BTN0 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  /* Configure pin output for PC5 */
  GPIO_PinModeSet(gpioPortC, 5, gpioModePushPull, 0);

  /* Configure interrupt for BTN0 */
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 0, 1, true);
}

/***************************************************************************//**
 * @ brief Initialize NVIC
 *         This function enables PCNT0 interrupt request in the
 *         interrupt controller
 ******************************************************************************/ 
static void setupNvic(void)
{
  /* Clear PCNT0 pending interrupt */
  NVIC_ClearPendingIRQ(PCNT0_IRQn);

  /* Clear odd GPIO pending interrupt */
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);

  /* Enable PCNT0 interrupt in the interrupt controller */
  NVIC_EnableIRQ(PCNT0_IRQn);

  /* Enable odd GPIO interrupt in the interrupt controller */
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Use LFRCO as LFA clock for LETIMER and PCNT */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_HFLE, true);

  /* Initialize GPIO */
  setupGpio();

  /* Initialize PCNT */
  setupPcnt();

  /* Initialize NVIC */
  setupNvic();

  /* Enter EM3 forever */
  while (true) {
    EMU_EnterEM3(false);
  }
}

