/***************************************************************************//**
 * @file main_s1.c
 * @brief This example shows how to use a timer to control the IDAC through the
 * PRS. The IDAC is configured to enable its output based on the signal it
 * receives from PRS channel 0. The timer generates a square waveform signal
 * that it sends to the PRS. This project operates in EM1.
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
#include "em_idac.h"
#include "em_prs.h"
#include "em_timer.h"

// Setting TOP value to 500 so that frequency is around 296 Hz
// frequency = HFPERCLK / (2^(PRESC + 1) x (TOP + 1))
// where HFPERCLK = 19Mhz and PRESC = 6
#define TOP 500

/**************************************************************************//**
 * @brief
 *    Initialize TIMER0
 *
 * @details
 *     Use compare mode and have the PRS follow the output of TIMER0.
 *     Configure TIMER0 to generate a square waveform with
 *     frequency = HFPERCLK / (2^(PRESC + 1) x (TOP + 1))
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize compare mode for TIMER0 on channel 0
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.cofoa = timerOutputActionToggle; // Toggle output on capture overflow
  timerCCInit.mode = timerCCModeCompare;       // Output compare mode
  TIMER_InitCC(TIMER0, 0, &timerCCInit);

  // The PRS channel output will follow capture/compare 0 output
  TIMER0->CC[0].CTRL |= TIMER_CC_CTRL_PRSCONF_LEVEL;

  // Set Top Value
  TIMER_TopSet(TIMER0, TOP);

  // Initialize the TIMER0 module
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.prescale = timerPrescale64; // Set prescale to 6 (i.e. divide by 64)
  TIMER_Init(TIMER0, &timerInit);
}

/**************************************************************************//**
 * @brief
 *    PRS initialization
 *****************************************************************************/
void initPrs(void)
{
  // Enable PRS clock
  CMU_ClockEnable(cmuClock_PRS, true);

  // Select TIMER0 as source and timer CC0 as signal for channel 0
  PRS_SourceSignalSet(0, PRS_CH_CTRL_SOURCESEL_TIMER0,
                         PRS_CH_CTRL_SIGSEL_TIMER0CC0, prsEdgeOff);
}

/**************************************************************************//**
 * @brief
 *    IDAC initialization
 *
 * @note
 *    Don't call IDAC_OutEnable() because we want the output enable to be
 *    controlled through the PRS.
 *****************************************************************************/
void initIdac(void)
{
  // Enable IDAC clock
  CMU_ClockEnable(cmuClock_IDAC0, true);

  // Initialize IDAC
  IDAC_Init_TypeDef init = IDAC_INIT_DEFAULT;
  init.outMode = idacOutputAPORT1XCH10; // Choose output to be on PA2
  init.prsEnable = true;       // Have the PRS act as the enable for IDAC output
  init.prsSel = idacPRSSELCh0; // Choose PRS channel 0
  IDAC_Init(IDAC0, &init);

  // Choose the output current to be 64 microamps
  IDAC_RangeSet(IDAC0, idacCurrentRange3);
  IDAC_StepSet(IDAC0, 31);

  // Enable the IDAC module
  IDAC_Enable(IDAC0, true);
}

/**************************************************************************//**
 * @brief
 *    Use a timer to control the IDAC output enable. The IDAC outputs 64
 *    microamps when enabled.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initIdac();
  initPrs();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}

