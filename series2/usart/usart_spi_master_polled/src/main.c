/***************************************************************************//**
 * @file main.c
 * @brief This project demonstrates synchronous (SPI) use of the USART in polled
 * master mode. The main loop transmits the specified number of bytes and
 * receives the byte that is shifted in with each outgoing one.
 *
 * The pins used in this example are defined below and are described in the
 * accompanying readme.txt file.
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

// Size of the data buffers
#define BUFLEN  10

// Ports and pins for SPI interface
#define US0MISO_PORT  gpioPortA
#define US0MISO_PIN   5
#define US0MOSI_PORT  gpioPortA
#define US0MOSI_PIN   6
#define US0CLK_PORT   gpioPortC
#define US0CLK_PIN    3
#define US0CS_PORT    gpioPortC
#define US0CS_PIN     1

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure RX pin as an input
  GPIO_PinModeSet(US0MISO_PORT, US0MISO_PIN, gpioModeInput, 0);

  // Configure TX pin as an output
  GPIO_PinModeSet(US0MOSI_PORT, US0MOSI_PIN, gpioModePushPull, 0);

  // Configure CLK pin as an output low (CPOL = 0)
  GPIO_PinModeSet(US0CLK_PORT, US0CLK_PIN, gpioModePushPull, 0);

  // Configure CS pin as an output and drive inactive high
  GPIO_PinModeSet(US0CS_PORT, US0CS_PIN, gpioModePushPull, 1);
}

/**************************************************************************//**
 * @brief
 *    USART0 initialization
 *****************************************************************************/
void initUsart0(void)
{
  // Default asynchronous initializer (master mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_USART0, true);
  init.msbf = true;   // MSB first transmission for SPI compatibility

  /*
   * Route USART0 RX, TX, and CLK to the specified pins.  Note that CS is
   * not controlled by USART0 so there is no write to the corresponding
   * USARTROUTE register to do this.
   */
  GPIO->USARTROUTE[0].RXROUTE = (US0MISO_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (US0MISO_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].TXROUTE = (US0MOSI_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (US0MOSI_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CLKROUTE = (US0CLK_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (US0CLK_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN |    // MISO
                                GPIO_USART_ROUTEEN_TXPEN |    // MOSI
                                GPIO_USART_ROUTEEN_CLKPEN;

  // Configure and enable USART0
  USART_InitSync(USART0, &init);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  uint32_t i;

  // Outgoing data
  uint8_t outbuf[BUFLEN];

  // Incoming data
  uint8_t inbuf[BUFLEN];
  
  // Eliminate unused variable warning
  // (User will observe inbuf in debugger)
  (void)inbuf;

  // Chip errata
  CHIP_Init();

  // Initialize GPIO and USART0
  initGpio();
  initUsart0();

  while (1)
  {
    // Zero incoming buffer and populate outgoing data array
    for (i = 0; i < BUFLEN; i++)
    {
      inbuf[i] = 0;
      outbuf[i] = (uint8_t)i;
    }

    // Assert chip select (drive low)
    GPIO_PinOutClear(US0CS_PORT, US0CS_PIN);

    /*
     * Repeatedly perform single byte SPI transfers (transmission and
     * reception) of the specified length.  USART_SpiTransfer() polls
     * USART_STATUS_TXC for transmission complete, so this function ties
     * up the CPU until the last bit of the byte being transmitted is sent.
     */
    for (i = 0; i < BUFLEN; i++)
      inbuf[i] = USART_SpiTransfer(USART0, outbuf[i]);

    // De-assert chip select upon transfer completion (drive high)
    GPIO_PinOutSet(US0CS_PORT, US0CS_PIN);
  }
}
