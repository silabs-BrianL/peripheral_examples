/***************************************************************************//**
 * @file main.c
 * @brief LDMA Ping-Pong Example
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
#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_ldma.h"


/* DMA channel used for the examples */
#define LDMA_CHANNEL        0
#define LDMA_CH_MASK        1 << LDMA_CHANNEL

/* Ping-Pong buffer size and constant for Ping-Pong transfer */
#define PP_BUFFER_SIZE      8

/* Descriptor linked list for LDMA transfer */
LDMA_Descriptor_t descLink[2];

/* Buffer for Ping-Pong transfer */
uint16_t srcBuffer[PP_BUFFER_SIZE];

uint16_t pingBuffer[PP_BUFFER_SIZE];
uint16_t pongBuffer[PP_BUFFER_SIZE];

/***************************************************************************//**
 * @brief
 *   LDMA IRQ handler.
 ******************************************************************************/
void LDMA_IRQHandler( void )
{
  uint32_t pending, i;

  /* Read interrupt source */
  pending = LDMA_IntGet();

  /* Clear interrupts */
  LDMA_IntClear(pending);

  /* Check for LDMA error */
  if ( pending & LDMA_IF_ERROR ){
    /* Loop here to enable the debugger to see what has happened */
    while (1);
  }

  /* Increment source buffer */
  for (i = 0; i < PP_BUFFER_SIZE; i++){
    srcBuffer[i]++;
  }

  /* Request next transfer */
  LDMA->SWREQ |= LDMA_CH_MASK;
}

/***************************************************************************//**
 * @brief
 *   Descriptor linked list example.
 ******************************************************************************/
void initLdma(void)
{
  uint32_t i;

  /* Initialize Ping-Pong buffers */
  for (i = 0; i < PP_BUFFER_SIZE; i++){
    srcBuffer[i] = 1;
    pingBuffer[i] = 0;
    pongBuffer[i] = 0;
  }

  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init( &init );

  /* Use peripheral transfer configuration macro */
  LDMA_TransferCfg_t periTransferTx = LDMA_TRANSFER_CFG_MEMORY();

  /* LINK descriptor macros for Ping-Pong transfer */
  descLink[0] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_HALF(&srcBuffer, &pingBuffer, PP_BUFFER_SIZE, 1);
  descLink[1] = (LDMA_Descriptor_t)LDMA_DESCRIPTOR_LINKREL_M2M_HALF(&srcBuffer, &pongBuffer, PP_BUFFER_SIZE, -1);

  /* Enable interrupts */
  descLink[0].xfer.doneIfs = true;
  descLink[1].xfer.doneIfs = true;

  /* Disable automatic transfers */
  descLink[0].xfer.structReq = false;
  descLink[1].xfer.structReq = false;

  LDMA_StartTransfer(LDMA_CHANNEL, (void*)&periTransferTx, (void*)&descLink);

  /* Software request to start transfer */
  LDMA->SWREQ |= LDMA_CH_MASK;
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator if available */
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  /* Initialize LDMA */
  initLdma();

  while (1)
  {
    EMU_EnterEM1();
  }
}
