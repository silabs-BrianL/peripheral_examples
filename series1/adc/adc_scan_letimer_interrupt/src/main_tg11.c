/***************************************************************************//**
 * @file main_tg11.c
 * @brief Use the LETIMER to send requests to the ADC to take multiple
 * measurements
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

#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_adc.h"
#include "em_gpio.h"
#include "em_letimer.h"

// Init to max ADC clock for Series 1
#define adcFreq   16000000

// Desired letimer interrupt frequency (in Hz)
#define letimerDesired  1000

#define NUM_INPUTS  2

uint32_t inputs[NUM_INPUTS];

/**************************************************************************//**
 * @brief LETIMER initialization
 *****************************************************************************/
void initLETIMER(void)
{
  LETIMER_Init_TypeDef letimerInit = LETIMER_INIT_DEFAULT;

  // Enable clock to the LE modules interface
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Select LFXO for the LETIMER
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LETIMER0, true);

  // Reload COMP0 on underflow, idle output, and run in repeat mode
  letimerInit.comp0Top  = true;
  letimerInit.ufoa0     = letimerUFOANone;
  letimerInit.repMode   = letimerRepeatFree;

  // Need REP0 != 0 to pulse on underflow
  LETIMER_RepeatSet(LETIMER0, 0, 1);

  // calculate the topValue
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_LETIMER0) / letimerDesired;

  // Compare on wake-up interval count
  LETIMER_CompareSet(LETIMER0, 0, topValue);

  // Initialize and enable LETIMER
  LETIMER_Init(LETIMER0, &letimerInit);

  // Enable LETIMER0 interrupts for COMP0
  LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP0);

  // Enable LETIMER interrupts
  NVIC_ClearPendingIRQ(LETIMER0_IRQn);
  NVIC_EnableIRQ(LETIMER0_IRQn);
}

/**************************************************************************//**
 * @brief ADC initialization
 *****************************************************************************/
void initADC (void)
{
  // Enable clocks required
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_ADC0, true);

  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitScan_TypeDef initScan = ADC_INITSCAN_DEFAULT;

  // Modify init structs
  init.prescale   = ADC_PrescaleCalc(adcFreq, 0);
  init.timebase = ADC_TimebaseCalc(0);

  initScan.diff          = false;       // single ended
  initScan.reference     = adcRef2V5;   // internal 2.5V reference
  initScan.resolution    = adcRes12Bit; // 12-bit resolution
  initScan.acqTime       = adcAcqTime4; // set acquisition time to meet minimum requirements
  initScan.fifoOverwrite = true;        // FIFO overflow overwrites old data

  // Select ADC input. See README for corresponding EXP header pin.
  // *Note that internal channels are unavailable in ADC scan mode
  ADC_ScanSingleEndedInputAdd(&initScan, adcScanInputGroup0, adcPosSelAPORT0XCH2);
  ADC_ScanSingleEndedInputAdd(&initScan, adcScanInputGroup1, adcPosSelAPORT0XCH6);

  // Set scan data valid level (DVL) to 2
  ADC0->SCANCTRLX = (ADC0->SCANCTRLX & ~_ADC_SCANCTRLX_DVL_MASK) | (((NUM_INPUTS - 1) << _ADC_SCANCTRLX_DVL_SHIFT) & _ADC_SCANCTRLX_DVL_MASK);

  // Clear ADC scan FIFO
  ADC0->SCANFIFOCLEAR = ADC_SCANFIFOCLEAR_SCANFIFOCLEAR;

  // Initialize ADC and Scans
  ADC_Init(ADC0, &init);
  ADC_InitScan(ADC0, &initScan);

  // Enable Scan interrupts
  ADC_IntEnable(ADC0, ADC_IEN_SCAN);

  // Enable ADC Interrupts
  NVIC_ClearPendingIRQ(ADC0_IRQn);
  NVIC_EnableIRQ(ADC0_IRQn);
}

/**************************************************************************//**
 * @brief LETIMER Handler
 *****************************************************************************/
void LETIMER0_IRQHandler(void)
{
  // Clear the interrupt flag
  LETIMER_IntClear(LETIMER0, LETIMER_IFC_COMP0);

  // Start next ADC conversion
  ADC_Start(ADC0, adcStartScan);
}

/**************************************************************************//**
 * @brief  ADC Handler
 *****************************************************************************/
void ADC0_IRQHandler(void)
{
  uint32_t data, i, id;

  // Get ADC results
  for(i = 0; i < NUM_INPUTS; i++)
  {
    // Get data from ADC scan
    data = ADC_DataIdScanGet(ADC0, &id);

    // Convert data to mV and store to array
    inputs[i] = data * 2500 / 4096;
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  initADC();

  initLETIMER();

  // Infinite loop
  while(1);
}
