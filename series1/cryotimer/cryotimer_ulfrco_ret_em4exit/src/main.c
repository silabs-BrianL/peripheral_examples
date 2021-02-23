/***************************************************************************//**
 * @file main.c
 * @brief This project shows how to use the Cryotimer in EM4 with the ULFRCO and
 * GPIO retention where the retention is enabled upon entering EM4 and disabled
 * upon exiting EM4. This project shows the program execution by using LEDs. For
 * the exact program flow, please see the readme.txt.
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
#include "em_gpio.h"
#include "em_cryotimer.h"
#include "em_rmu.h"
#include "bsp.h"

// Note: change this to one of the defined periods in em_cryotimer.h
// Wakeup events occur every 2048 prescaled clock cycles
#define CRYOTIMER_PERIOD    cryotimerPeriod_2k

// Note: change this to one of the defined prescalers in em_cryotimer.h
// The clock is divided by one
#define CRYOTIMER_PRESCALE  cryotimerPresc_1

/**************************************************************************//**
 * @brief
 *    Cryotimer interrupt service routine
 *
 * @note
 *    The barrier below is make sure the interrupt flags get cleared before
 *    continuing. This ensures that the interrupt is not retriggered before
 *    exiting the Handler. A barrier like this is generally required when the
 *    peripheral clock is much slower than the CPU core clock.
 *
 * @note
 *    Despite the note above, the barrier is just a precaution. The code
 *    might work without it as well.
 *****************************************************************************/
void CRYOTIMER_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = CRYOTIMER_IntGet();
  CRYOTIMER_IntClear(flags);

  // Put a barrier here to ensure interrupts are not retriggered. See note above
  __DSB();
}

/**************************************************************************//**
 * @brief
 *    Even GPIO pin handler
 *****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = GPIO_IntGet();
  GPIO_IntClear(flags);
}

/**************************************************************************//**
 * @brief
 *    Odd GPIO pin handler
 *****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  // Acknowledge the interrupt
  uint32_t flags = GPIO_IntGet();
  GPIO_IntClear(flags);
}

/**************************************************************************//**
 * @brief
 *    Initialize Cryotimer
 *
 * @details
 *    The Cryotimer is reset in order to clear the CNT register. This register
 *    still holds its value from before the EM4 reset and therefore gives
 *    incorrect timing if not cleared. The timer will be started later in the
 *    main().
 *
 * @note
 *    No need to enable the ULFRCO since it is always on and cannot be shut off
 *    under software control. The ULFRCO is used in this example because it is
 *    the only oscillator capable of running in EM3.
 *****************************************************************************/
void initCryotimer(void)
{
  // Enable cryotimer clock
  CMU_ClockEnable(cmuClock_CRYOTIMER, true);

  // Initialize cryotimer
  CRYOTIMER_Init_TypeDef init = CRYOTIMER_INIT_DEFAULT;
  init.osc       = cryotimerOscULFRCO; // Use the ULFRCO
  init.em4Wakeup = true;               // Enable EM4 wakeup upon triggering a Cryotimer interrupt
  init.presc     = CRYOTIMER_PRESCALE; // Set the prescaler
  init.period    = CRYOTIMER_PERIOD;   // Set when wakeup events occur
  init.enable    = false;              // Reset the Cryotimer and don't start the timer
  CRYOTIMER_Init(&init);

  // Enable Cryotimer interrupts
  CRYOTIMER_IntEnable(CRYOTIMER_IEN_PERIOD);
  NVIC_EnableIRQ(CRYOTIMER_IRQn);
}

/**************************************************************************//**
 * @brief
 *    Initialize EM4 mode
 *
 * @note
 *    The ULFRCO must be retained in EM4 because it is the oscillator being used
 *    to power the Cryotimer. Without retaining its settings, there will be
 *    nothing to wake us up from EM4. In EM4 Shutoff mode, the ULFRCO is
 *    automatically disabled, so we must explicitly set the ULFRCO to be
 *    retained.
 *****************************************************************************/
void initEm4(void)
{
  // Configure EM4 behavior
  EMU_EM4Init_TypeDef init = EMU_EM4INIT_DEFAULT;
  init.em4State = emuEM4Shutoff; // emuEM4Hibernate also works
  init.retainUlfrco = true; // Make sure the ULFRCO is still powering the Cryotimer in EM4
  init.pinRetentionMode = emuPinRetentionEm4Exit; // Retain GPIO registers until exit from EM4
  EMU_EM4Init(&init);
}

/**************************************************************************//**
 * @brief
 *    Initialize the GPIO clock and set button 0 as an input
 *
 * @details
 *    The buttons are active low and therefore the code triggers on a falling
 *    edge.
 *
 * @details
 *    The only reason this program uses button 0 is to show the user the state
 *    of the LEDs and therefore show that the reset cause is being
 *    appropriately detected
 *****************************************************************************/
void initGpio(void)
{
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set Button 0 as an input with pull-up and filter enabled
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);

  // Enable GPIO_EVEN interrupt
  if ((BSP_GPIO_PB0_PIN & 0x01) == 0) {
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
  }
  // Enable GPIO_ODD interrupt
  else {
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
  }
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    The only reason this program chooses to wait in EM3 is to act as a delay
 *    and show the user the state of the LEDs.
 *
 * @note
 *    It is good practice to always call EMU_UnlatchPinRetention() when
 *    dealing with EM4 (even if you aren't using retention for the GPIO
 *    pins). The reason is because the registers will have contradictory values
 *    to the latched values upon waking up from EM4. To ensure a consistent
 *    reset state, the unlatch command should be given after properly
 *    reconfiguring these latched registers so that the new values can be
 *    re-latched upon entering EM4. Another good reason to always call
 *    EMU_UnlatchPinRetention() is because of the "EM4H I/O Retention Cannot Be
 *    Disabled" Errata (see the errata for your particular board).
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Get the reset cause and also clear the reset cause register because it can
  // only be cleared by software.
  uint32_t resetCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear();

  // Set button 0 as an input and enable GPIO interrupts
  initGpio();

  // Determine the reset cause
  if ((resetCause & RMU_RSTCAUSE_PORST)           // Reset from powering on the board
        || (resetCause & RMU_RSTCAUSE_EXTRST)     // Reset from an external pin
        || (resetCause & RMU_RSTCAUSE_SYSREQRST)) // Reset from a system request (e.g. from the debugger)
  {
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0); // Turn LED1 off
  }
  // If reset was caused by waking up from EM4
  else if (resetCause & RMU_RSTCAUSE_EM4RST) {
    GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 1); // Turn LED1 on
    EMU_UnlatchPinRetention(); // Unlatch the registers that were latched upon entering EM4
  }

  // Initialize the Cryotimer and EM4 settings
  initCryotimer();
  initEm4();

  // Go into EM3 and wait for the user to push button 0
  // At this point, one of two cases could occur
  // 1) Both LEDs will be off (this case only occurs once upon initially resetting the board)
  // 2) Only LED1 will be on (this case occurs after waking up from EM4)
  EMU_EnterEM3(false);

  // Turn LED0 on and LED1 off
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 1);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

  // Start the Cryotimer
  CRYOTIMER_Enable(true);

  // Go into EM4 and wait for Cryotimer wakeup.
  // At this point, LED0 will still be on since there is GPIO retention.
  // After waking up from EM4 though, LED0 will turn off because GPIO retention
  // was configured to only last while still in EM4.
  EMU_EnterEM4();
}

