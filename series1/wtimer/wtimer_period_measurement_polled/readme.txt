wtimer_period_measurement_polled

This project demonstrates polled period measurement using the WTIMER module. 
TIMER is initialized for input capture on Compare/Capture channel 0 for falling 
edge capture. The program waits for a falling edge and then the captured edge is 
read from CC0 and compared with the previously captured edge to calculate the 
period. The measured period is stored in the measuredPeriod global variable in 
units of microseconds.

Example: for a 1 kHz input signal, the measuredPeriod variable will equal 1000.

Note: only the following boards have a WTIMER module
 - PG12 
 - TG11
 - GG11
 - radio boards series 12, 13, 14

================================================================================

Peripherals Used:
WTIMER0 - HFPERCLK 19 MHz

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Connect a periodic signal to the GPIO pin specified below
3. Go into debug mode and click run
4. View the measuredPeriod global variable in the debugger

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 16)

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG12P432F1024GL125
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PC10 - WTIM0_CC0 #30 (Expansion Header Pin 15)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC1  - WTIM0_CC0 #7 (Expansion Header Pin 15)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC1  - WTIM0_CC0 #7 (Expansion Header Pin 5)

