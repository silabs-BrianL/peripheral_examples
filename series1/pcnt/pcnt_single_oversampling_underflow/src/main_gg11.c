/***************************************************************************//**
 * @file main_gg11.c
 * @brief This project demonstrates the single input oversampling mode of the
 * pulse counter with interrupts. LFACLK is used as the pulse counter clock in 
 * this example. The program generates an interrupt whenever the pulse counter
 * goes below zero. In this example, each press of Push Button PB0 will decrease
 * the counter value by 1. The initial value of the counter and the reload value
 * from the top register is set by the user.
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
#include "em_prs.h"

#include "bsp.h"

#include <stdint.h>
#include <stdbool.h>

/* global variable for LED toggling */
uint32_t setflag = 1;

/***************************************************************************//**
 * @brief PCNT0 interrupt handler
 *        This function acknowledges the interrupt and toggles LED0
 ******************************************************************************/        
void PCNT0_IRQHandler(void)
{
  /* Acknowledge interrupt */	
  PCNT_IntClear(PCNT0, PCNT_IFC_UF);
  if(setflag == 1){
    GPIO_PinOutClear(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);	// set led0, note in GG11 clear = set
    setflag = 0;                                                // clear flag, clear led0 on next cycle
  }
  else{
    GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);      // clear led0, note in GG11 set = clear
    setflag = 1;                                                // set flag, set led0 on next cycle
  }
}

/***************************************************************************//**
 * @brief PCNT0 setup
 *        This function sets up PCNT0 with oversampling single mode.
 *        Counter and top value default to 5
 *        Event triggering when counting down
 ******************************************************************************/        
static void setupPcnt(void)
{
  PCNT_Init_TypeDef pcntInit = PCNT_INIT_DEFAULT;
  PCNT_Filter_TypeDef pcntFilterInit = PCNT_FILTER_DEFAULT;

  CMU_ClockEnable(cmuClock_PCNT0, true);
  pcntInit.mode      = pcntModeOvsSingle;
  pcntInit.counter   = 5;                        // Initial counter value, interrupt at every 6 BTN0 press
  pcntInit.top       = 5;                        // Interrupt at every 6 BTN0 press
  pcntInit.s1CntDir  = false;                    // S1 does not affect counter direction, default count up
  pcntInit.countDown = true;                     // counting direction down
  pcntInit.cntEvent  = pcntCntEventDown;         // event only triggering when counting down
  pcntInit.s0PRS     = pcntPRSCh0;
  pcntInit.filter    = true;                     // Filter GPIO to remove glitching LED behavior

  /* Use max filter len for GPIO push button */
  pcntFilterInit.filtLen = _PCNT_OVSCFG_FILTLEN_MASK;

  /* Enable the PCNT0 */
  PCNT_Init(PCNT0, &pcntInit);

  /* Filter configuration */
  PCNT_FilterConfiguration(PCNT0, &pcntFilterInit, true);

  /* Enable PRS0 for PCNT0 */
  PCNT_PRSInputEnable(PCNT0, pcntPRSInputS0, true);

  /* Enable underflow interrupt for PCNT0 */
  PCNT_IntEnable(PCNT0, PCNT_IEN_UF);
}

/***************************************************************************//**
 * @brief PRS setup
 *        This function sets up GPIO PRS pin 8 which links BTN0 to PCNT0 PRS0
 ******************************************************************************/        
static void setupPrs(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);

  /* Set up GPIO PRS pin 8 */
  PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN0);
}

/***************************************************************************//**
 * @brief GPIO setup
 *        This function configures BTN0 as input and enables external interrupt
 *        for BTN0
 ******************************************************************************/        
static void setupGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize LED driver */
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);

  /* Configure pin I/O - BTN0 */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 1);

  /* Configure BTN0 for external interrupt */
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, 8, false, false, false);
}

/***************************************************************************//**
 * @brief NVIC setup
 *        This function enables PCNT0 interrupt requests in the
 *        interrupt controller
 ******************************************************************************/        
static void setupNvic(void)
{
  /* Clear PCNT0 pending interrupt */
  NVIC_ClearPendingIRQ(PCNT0_IRQn);

  /* Enable PCNT0 interrupt in the interrupt controller */
  NVIC_EnableIRQ(PCNT0_IRQn);
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator with kit specific parameters */
  EMU_DCDCInit(&dcdcInit);

  /* Use LFRCO as LFA clock for LETIMER and PCNT */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockEnable(cmuClock_HFLE, true);

  /* GPIO Initialization */
  setupGpio();

  /* PCNT Initialization */
  setupPcnt();

  /* PRS Initialization */
  setupPrs();

  /* NVIC Initialization */
  setupNvic();

  /* Enter EM2 forever */
  while (true) {
    EMU_EnterEM2(false);
  }
}

