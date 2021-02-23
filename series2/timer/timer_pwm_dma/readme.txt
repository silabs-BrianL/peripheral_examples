timer_pwm_dma

This project demonstrates DMA driven pulse width modulation using the TIMER
module. TIMER0 is initialized for PWM on Compare/Capture channel 0 which is
routed to the GPIO Pin specified below. In PWM mode, overflow events
set the output pin, while compare events clear the pin. Thus the overflow value
is set to output the desired signal frequency, while the CCV is set to control 
the duty cycle. The DMA is set to loop through a buffer of duty cycles
incrementing from 0 to 100%, and writes to the CCVB on each overflow event.

================================================================================

Peripherals Used:
TIMER0 - EM01GRPACLK/PCLK (19 MHz)
LDMA

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Use an oscilloscope to view a 1 kHz signal with continuosly varying duty
   cycle on the GPIO pin specified below

================================================================================

Listed below are the port and pin mappings for working with this example.

Board: Silicon Labs EFR32xG21 2.4 GHz 10 dBm Board (BRD4181A) 
       + Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG21A010F1024IM32
PA6 - TIM0_CC0 (Expansion Header Pin 14)

Board:  Silicon Labs EFR32xG22 Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG22C224F512IM40
PA6 - TIM0_CC0 (Expansion Header Pin 14)