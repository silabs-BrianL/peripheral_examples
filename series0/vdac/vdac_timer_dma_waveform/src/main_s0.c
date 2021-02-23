/***************************************************************************//**
 * @file main_s0.c
 * @brief This project uses the DAC and TIMER0 to output a 32 point sine wave at
 * a particular frequency (10 kHz by default). This project operates in EM1.
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
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_dac.h"
#include "em_timer.h"
#include "em_dma.h"
#include "dmactrl.h"

// 32 point sine table
#define SINE_TABLE_SIZE 32
static const uint16_t sineTable[SINE_TABLE_SIZE] = {
  2048 , 2447 , 2831 , 3185 , 3495 , 3750 , 3939 , 4056 ,
  4095 , 4056 , 3939 , 3750 , 3495 , 3185 , 2831 , 2447 ,
  2048 , 1648 , 1264 , 910  , 600  , 345  , 156  , 39   ,
  0    , 39   , 156  , 345  , 600  , 910  , 1264 , 1648 ,
};

// Note: change this to determine the frequency of the sine wave
#define WAVEFORM_FREQ 10000

// The timer needs to run at SINE_TABLE_SIZE times faster than the desired
// waveform frequency because it needs to output SINE_TABLE_SIZE points in that
// same amount of time
#define TIMER0_FREQ (WAVEFORM_FREQ * SINE_TABLE_SIZE)

/**************************************************************************//**
 * @brief
 *    DAC initialization
 *****************************************************************************/
void initDac(void)
{
  // Enable the DAC clock
  CMU_ClockEnable(cmuClock_DAC0, true);

  // Initialize the DAC
  DAC_Init_TypeDef init = DAC_INIT_DEFAULT;
  init.prescale = DAC_PrescaleCalc(1000000, 0); // Use a 1 MHz DAC
  init.reference = dacRef1V25; // Set reference to internal 1.25V low noise
  DAC_Init(DAC0, &init);

  // Initialize DAC channel 0
  DAC_InitChannel_TypeDef initChannel0 = DAC_INITCHANNEL_DEFAULT;
  DAC_InitChannel(DAC0, &initChannel0, 0);

  // Enable DAC channel 0
  DAC_Enable(DAC0, 0, true);
}

/**************************************************************************//**
 * @brief
 *    Timer initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER0 module
  CMU_ClockEnable(cmuClock_TIMER0, true);

  // Initialize TIMER0
  TIMER_Init_TypeDef init = TIMER_INIT_DEFAULT;
  init.enable = false;
  TIMER_Init(TIMER0, &init);

  // Set top (reload) value for the timer
  // Note: the timer runs off of the HFPER clock
  uint32_t topValue = CMU_ClockFreqGet(cmuClock_HFPER) / TIMER0_FREQ;
  TIMER_TopBufSet(TIMER0, topValue);

  // Automatically clear the DMA request
  TIMER0->CTRL |= TIMER_CTRL_DMACLRACT;

  // Enable TIMER0
  TIMER_Enable(TIMER0, true);
}

/**************************************************************************//**
 * @brief
 *    Initialize the DMA module
 *
 * @details
 *    Always use dmaControlBlock to make sure that the control block is properly
 *    aligned. Tell the DMA module to trigger when the TIMER0 overflows (hits
 *    its TOP value and wraps around). Don't trigger an interrupt upon since we
 *    don't need it. This example chose to use channel 0
 *
 * @note
 *    The descriptor object needs to at least have static scope persistence so
 *    that the reference to the object is valid beyond its first use in
 *    initialization. This is because this code loops back to the same
 *    descriptor after every dma transfer. If the reference isn't valid anymore,
 *    then all dma transfers after the first one will fail.
 ******************************************************************************/
void initDma(void)
{
  // Initializing the DMA
  DMA_Init_TypeDef init;
  init.hprot = 0; // Access level/protection not an issue
  init.controlBlock = dmaControlBlock; // Make sure control block is properly aligned
  DMA_Init(&init);

  // Channel configuration
  DMA_CfgChannel_TypeDef channelConfig;
  channelConfig.highPri   = false; // Set high priority for the channel
  channelConfig.enableInt = false; // Interrupt not needed
  channelConfig.select    = DMAREQ_TIMER0_UFOF; // Select DMA trigger
  channelConfig.cb        = NULL;               // No callback because no interrupt
  uint32_t channelNum     = 0;
  DMA_CfgChannel(channelNum, &channelConfig);

  // Channel descriptor configuration
  static DMA_CfgDescr_TypeDef descriptorConfig;
  descriptorConfig.dstInc  = dmaDataIncNone; // Destination doesn't move
  descriptorConfig.srcInc  = dmaDataInc2;    // Source moves 2 bytes each transfer
  descriptorConfig.size    = dmaDataSize2;   // Transfer 2 bytes each time
  descriptorConfig.arbRate = dmaArbitrate1;  // Arbitrate after every DMA transfer
  descriptorConfig.hprot   = 0;              // Access level/protection not an issue
  bool isPrimaryDescriptor = true;
  DMA_CfgDescr(channelNum, isPrimaryDescriptor, &descriptorConfig);

  // Configure loop transfer mode
  DMA_CfgLoop_TypeDef loopConfig;
  loopConfig.enable  = true; // Enable looping
  loopConfig.nMinus1 = SINE_TABLE_SIZE - 1; // Reload value for number of DMA transfers minus 1
  DMA_CfgLoop(channelNum, &loopConfig);

  // Activate basic DMA cycle (used for memory-peripheral transfers)
  bool isUseBurst = false;
  DMA_ActivateBasic(channelNum,
                    isPrimaryDescriptor,
                    isUseBurst,
                    (void *) &DAC0->CH0DATA, // Destination address to transfer to
                    (void *) &sineTable[0],  // Source address to transfer from
                    SINE_TABLE_SIZE-1);      // Number of DMA transfers minus 1
}

/**************************************************************************//**
 * @brief
 *    Ouput a sine wave to DAC channel 0
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Initialization
  initDac();
  initDma();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}
