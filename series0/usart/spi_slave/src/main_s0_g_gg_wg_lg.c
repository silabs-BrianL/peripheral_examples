/***************************************************************************//**
 * @file main_s0_g_gg_wg_lg.c
 * @brief Demonstrates USART1 as SPI slave.
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
#include "em_gpio.h"
#include "em_usart.h"

#define RX_BUFFER_SIZE   10
#define TX_BUFFER_SIZE   RX_BUFFER_SIZE

uint8_t RxBuffer[RX_BUFFER_SIZE];
uint8_t RxBufferIndex = 0;

uint8_t TxBuffer[TX_BUFFER_SIZE] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9};
uint8_t TxBufferIndex = 0;

/**************************************************************************//**
 * @brief USART1 TX IRQ Handler
 *****************************************************************************/
void USART1_TX_IRQHandler(void)
{
  // Send and receive incoming data
  USART1->TXDATA = (uint32_t)TxBuffer[TxBufferIndex];
  TxBufferIndex++;

  // Stop sending once we've gone through the whole TxBuffer
  if (TxBufferIndex == TX_BUFFER_SIZE)
  {
    TxBufferIndex = 0;
  }
}

/**************************************************************************//**
 * @brief USART1 RX IRQ Handler
 *****************************************************************************/
void USART1_RX_IRQHandler(void)
{
  // Read data
  RxBuffer[RxBufferIndex] = USART_RxDataGet(USART1);
  RxBufferIndex++;

  if (RxBufferIndex == RX_BUFFER_SIZE)
  {
    // Putting a break point here to view the full RxBuffer,
    // The RxBuffer should be: 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09
    RxBufferIndex = 0;
  }
}

/**************************************************************************//**
 * @brief Initialize USART1
 *****************************************************************************/
void initUSART1 (void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART1, true);

  // Configure GPIO mode
  GPIO_PinModeSet(gpioPortD, 2, gpioModeInput, 1);    // US1_CLK is input
  GPIO_PinModeSet(gpioPortD, 3, gpioModeInput, 1);    // US1_CS is input
  GPIO_PinModeSet(gpioPortD, 0, gpioModeInput, 1);    // US1_TX (MOSI) is input
  GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 1); // US1_RX (MISO) is push pull

  // Start with default config, then modify as necessary
  USART_InitSync_TypeDef config = USART_INITSYNC_DEFAULT;
  config.master    = false;
  config.clockMode = usartClockMode0; // clock idle low, sample on rising/first edge
  config.msbf      = true;            // send MSB first
  config.enable    = usartDisable;    // making sure to keep USART disabled until we've set everything up
  USART_InitSync(USART1, &config);

  // Set USART pin locations
  USART1->ROUTE = USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_LOCATION_LOC1;

  // Clear RX buffer and shift register
  USART1->CMD |= USART_CMD_CLEARRX;

  // Enable
  USART_Enable(USART1, usartEnable);

  // Enable USART1 RX interrupts
  USART_IntClear(USART1, USART_IF_RXDATAV);
  USART_IntEnable(USART1, USART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(USART1_RX_IRQn);
  NVIC_EnableIRQ(USART1_RX_IRQn);

  // Enabling TX interrupts to transfer whenever
  // there is room in the transmit buffer
  // This should immediately trigger to load the first byte of our TX buffer
  USART_IntClear(USART1, USART_IF_TXBL);
  USART_IntEnable(USART1, USART_IF_TXBL);
  NVIC_ClearPendingIRQ(USART1_TX_IRQn);
  NVIC_EnableIRQ(USART1_TX_IRQn);
}

/**************************************************************************//**
 * @brief Main function
 *****************************************************************************/
int main(void)
{
  // Initialize chip
  CHIP_Init();

  // Initialize USART1 as SPI slave
  initUSART1();

  //packets will begin coming in as soon as the master code starts
  while(1);
}
