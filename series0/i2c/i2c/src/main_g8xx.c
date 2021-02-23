/***************************************************************************//**
 * @file main_g8xx.c
 * @brief This project demonstrates both master and slave configurations of the
 * EFM32 I2C peripheral. Two EFM32 I2C modules are connected and set up to both
 * transmit (master mode) and receive (slave mode) between each other using a
 * common I2C bus.
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
#include "em_i2c.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "bsp.h"
// Defines
#define CORE_FREQUENCY              14000000
#define RTC_MIN_TIMEOUT                32000
#define I2C_ADDRESS                     0xE2
#define I2C_RXBUFFER_SIZE                 10

// Buffers++
uint8_t i2c_txBuffer[] = "Gecko";
uint8_t i2c_txBufferSize = sizeof(i2c_txBuffer);
uint8_t i2c_rxBuffer[I2C_RXBUFFER_SIZE];
uint8_t i2c_rxBufferIndex;

// Transmission flags
volatile bool i2c_rxInProgress;
volatile bool i2c_startTx;

/**************************************************************************//**
 * @brief  Starting oscillators and enabling clocks
 *****************************************************************************/
void initCMU(void)
{
  // Enabling clock to the I2C, GPIO, LE
  CMU_ClockEnable(cmuClock_I2C0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_HFLE, true);

  // Starting LFXO and waiting until it is stable
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
}

/**************************************************************************//**
 * @brief GPIO initialization
 *****************************************************************************/
void initGPIO(void)
{
  // Configure PB0 as input and int
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  // Configure LED0 and LED1 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);


  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
 * @brief  enables I2C slave interrupts
 *****************************************************************************/
void enableI2cSlaveInterrupts(void)
{
  I2C_IntClear(I2C0, I2C_IFC_ADDR | I2C_IF_RXDATAV | I2C_IFC_SSTOP);
  I2C_IntEnable(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  NVIC_EnableIRQ(I2C0_IRQn);
}

/**************************************************************************//**
 * @brief  disables I2C interrupts
 *****************************************************************************/
void disableI2cInterrupts(void)
{
  NVIC_DisableIRQ(I2C0_IRQn);
  I2C_IntDisable(I2C0, I2C_IEN_ADDR | I2C_IEN_RXDATAV | I2C_IEN_SSTOP);
  I2C_IntClear(I2C0, I2C_IFC_ADDR | I2C_IF_RXDATAV | I2C_IFC_SSTOP);
}

/**************************************************************************//**
 * @brief  Setup I2C
 *****************************************************************************/
void initI2C(void)
{
  // Using default settings
  I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;
  // Use ~400khz SCK
  i2cInit.freq = I2C_FREQ_FAST_MAX;

  // Using PD6 (SDA) and PD7 (SCL)
  GPIO_PinModeSet(gpioPortD, 6, gpioModeWiredAndPullUpFilter, 1);
  GPIO_PinModeSet(gpioPortD, 7, gpioModeWiredAndPullUpFilter, 1);

  // Enable pins at location 15 as specified in datasheet
  I2C0->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN;
  I2C0->ROUTE = (I2C0->ROUTE & (~_I2C_ROUTE_LOCATION_MASK)) | I2C_ROUTE_LOCATION_LOC1;

  // Initializing the I2C
  I2C_Init(I2C0, &i2cInit);

  // Setting the status flags and index
  i2c_rxInProgress = false;
  i2c_startTx = false;
  i2c_rxBufferIndex = 0;

  // Setting up to enable slave mode
  I2C0->SADDR = I2C_ADDRESS;
  I2C0->CTRL |= I2C_CTRL_SLAVE | I2C_CTRL_AUTOACK | I2C_CTRL_AUTOSN;
  enableI2cSlaveInterrupts();
}

/**************************************************************************//**
 * @brief  Transmitting I2C data. Will busy-wait until the transfer is complete.
 *****************************************************************************/
void performI2CTransfer(void)
{
  // Transfer structure
  I2C_TransferSeq_TypeDef i2cTransfer;
  I2C_TransferReturn_TypeDef result;

  // Setting LED to indicate transfer
  GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);

  // Initializing I2C transfer
  i2cTransfer.addr          = I2C_ADDRESS;
  i2cTransfer.flags         = I2C_FLAG_WRITE;
  i2cTransfer.buf[0].data   = i2c_txBuffer;
  i2cTransfer.buf[0].len    = i2c_txBufferSize;
  i2cTransfer.buf[1].data   = i2c_rxBuffer;
  i2cTransfer.buf[1].len    = I2C_RXBUFFER_SIZE;
  result = I2C_TransferInit(I2C0, &i2cTransfer);

  // Sending data
  while (result == i2cTransferInProgress)
  {
    result = I2C_Transfer(I2C0);
  }

  // Clearing pin to indicate end of transfer
  GPIO_PinOutClear(gpioPortC, 1);
  enableI2cSlaveInterrupts();
}

/**************************************************************************//**
 * @brief  Receiving I2C data. Along with the I2C interrupt, it will keep the
  EFM32 in EM1 while the data is received.
 *****************************************************************************/
void receiveI2CData(void)
{
  while(i2c_rxInProgress)
  {
    EMU_EnterEM1();
  }
}

/**************************************************************************//**
 * @brief I2C Interrupt Handler.
 *        The interrupt table is in assembly startup file startup_efm32.s
 *****************************************************************************/
void I2C0_IRQHandler(void)
{
  int status;

  status = I2C0->IF;

  if (status & I2C_IF_ADDR)
  {
    // Address Match
    // Indicating that reception is started
    i2c_rxInProgress = true;
    I2C0->RXDATA;
    i2c_rxBufferIndex = 0;

    I2C_IntClear(I2C0, I2C_IFC_ADDR);

  } else if (status & I2C_IF_RXDATAV)
  {
    // Data received
    i2c_rxBuffer[i2c_rxBufferIndex] = I2C0->RXDATA;
    i2c_rxBufferIndex++;
  }

  if(status & I2C_IEN_SSTOP){
    // Stop received, reception is ended
    I2C_IntClear(I2C0, I2C_IFC_SSTOP);
    i2c_rxInProgress = false;
    i2c_rxBufferIndex = 0;
  }
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler
 ******************************************************************************/

void GPIO_ODD_IRQHandler(void)
{
  // Clear pending
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  // If RX is not in progress, a new transfer is started
  if (!i2c_rxInProgress)
  {
    disableI2cInterrupts();
	  i2c_startTx = true;
	}
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Configuring clocks in the Clock Management Unit (CMU)
  initCMU();
  
  // Initializations
  initGPIO();

  // Setting up i2c
  initI2C();

  //LED ON to indicate operating
  GPIO_PinOutSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);

  while (1)
  {
    if(i2c_rxInProgress)
    {
       // Receiving data
       receiveI2CData();
    }else if (i2c_startTx)
    {
       // Transmitting data
       performI2CTransfer();
       // Transmission complete
       i2c_startTx = false;
    }

    // Forever enter EM2. The RTC or I2C will wake up the EFM32

  }
}
