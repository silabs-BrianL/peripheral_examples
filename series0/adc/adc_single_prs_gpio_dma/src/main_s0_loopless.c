/***************************************************************************//**
 * @file main_s0_loopless.c
 * @brief Use the ADC to take a nonblocking measurements in EM1. The PRS
 * redirects GPIO signals to start ADC single conversions. The DMA moves
 * completed conversions to a SW buffer
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
#include "em_gpio.h"
#include "em_adc.h"
#include "em_prs.h"
#include "em_dma.h"
#include "bsp.h"

#include "dmactrl.h"

// Change this to set number of samples.
#define ADC_BUFFER_SIZE   4

#define ADC_FREQ          13000000

#define DMA_CHANNEL       0
#define PRS_CHANNEL       0

// Buffer for ADC single and scan conversion
uint32_t adcBuffer[ADC_BUFFER_SIZE];

DMA_CB_TypeDef dmacb;

/**************************************************************************//**
 * @brief DMA Callback function
 *****************************************************************************/
void dmaCallback(unsigned int channel, bool primary, void *user)
{
  // Insert transfer complete functionality here
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable clock for GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set push button 0 to input
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);

  // Enable push button 0 to generate PRS interrupt signals
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, false, false);

  // Enable and configure PRS
  CMU_ClockEnable(cmuClock_PRS, true);

  if (BSP_GPIO_PB0_PIN > 7){
    PRS_SourceSignalSet(PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOH, (uint32_t)(BSP_GPIO_PB0_PIN - 8), prsEdgePos);
  }
  else {
    PRS_SourceSignalSet(PRS_CHANNEL, PRS_CH_CTRL_SOURCESEL_GPIOL, BSP_GPIO_PB0_PIN, prsEdgePos);
  }
}

/**************************************************************************//**
 * @brief DMA initialization
 *****************************************************************************/
void initDma(void)
{
  // Enable DMA clock
  CMU_ClockEnable(cmuClock_DMA, true);

  // Basic DMA configuration
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;

  // Configure general DMA issues
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;

  DMA_Init(&dmaInit);

  // Setup callback function
  dmacb.cbFunc  = dmaCallback;
  dmacb.userPtr = NULL;

  // Configure DMA channel used
  chnlCfg.highPri   = false;
  chnlCfg.enableInt = true;
  chnlCfg.select    = DMAREQ_ADC0_SINGLE;
  chnlCfg.cb        = &dmacb;
  DMA_CfgChannel(DMA_CHANNEL, &chnlCfg);

  descrCfg.dstInc  = dmaDataInc4;
  descrCfg.srcInc  = dmaDataIncNone;
  descrCfg.size    = dmaDataSize4;
  descrCfg.arbRate = dmaArbitrate1;
  descrCfg.hprot   = 0;
  DMA_CfgDescr(DMA_CHANNEL, true, &descrCfg);

  // Start DMA
  DMA_ActivateBasic(DMA_CHANNEL,  // Channel
      true,                       // Is primary
      false,                      // Don't use bursts
      adcBuffer,                  // Destination
      (void *)&(ADC0->SINGLEDATA), // Source
      ADC_BUFFER_SIZE - 1);       // Transfer size
}

/**************************************************************************//**
 * @brief ADC initialization
 *****************************************************************************/
void initAdc(void)
{
  // Declare init structs
  ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;

  // Enable ADC clock
  CMU_ClockEnable(cmuClock_ADC0, true);
  CMU_ClockEnable(cmuClock_HFPER, true);

  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(ADC_FREQ, 0);

  // Add external ADC input to scan. See README for corresponding EXP header pin.
  initSingle.input = adcSingleInputCh7;

  // Basic ADC scan configuration
  initSingle.diff =       false;        // single-ended
  initSingle.reference  = adcRef2V5;    // 2.5V reference
  initSingle.resolution = adcRes12Bit;  // 12-bit resolution

  // Enable PRS trigger and select channel 0
  initSingle.prsEnable = true;
  initSingle.prsSel = (ADC_PRSSEL_TypeDef) PRS_CHANNEL;

  // Initialize ADC
  ADC_Init(ADC0, &init);
  ADC_InitSingle(ADC0, &initSingle);
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  CHIP_Init();

  // Set up GPIO to trigger ADC via PRS
  initGpio();
  // Setup ADC to perform conversions
  initAdc();
  // Setup DMA to move ADC results to user memory
  initDma();

  // Infinite loop
  while(1)
  {
    // Enter EM1 until next ADC interrupt
    EMU_EnterEM1();
  }

}
