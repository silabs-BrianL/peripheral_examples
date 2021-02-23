timer_pulse_capture

This project demonstrates single pulse capture using the TIMER module. The HFXO
is configured to run the HFPER clock. TIMER is then initialized for input
capture on Compare/Capture channel 0 for interrupts on every edge. The GPIO Pin
specified below is to be connected to a periodic signal or pulse generator. The
two edges captured (one falling and one rising) are read from the CCV register.

Note: The range of frequencies this program can measure accurately is limited
due to dropout at higher frequencies, input setup time, and the HPERCLK frequency
selected for the timer peripheral source. The minimum measurable pulse width is 
~10X HPERCLK period.

================================================================================

Peripherals Used:
TIMER - CC0
HFXO
 - 24 MHz for HG, ZG
 - 32 MHz for G, TG
 - 38.4 MHz for radio boards (FG, BG, MG) (1, 12, 13, 14)
 - 40 MHz for PG1, PG12
 - 48 MHz for WG, LG, GG, TG11
 - 50 MHz for GG11

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Connect a periodic signal to GPIO pin specified below
3. Go into debug mode and click run
4. View the firstEdge and secondEdge global variables in the watch window

================================================================================

Listed below are the port and pin mappings for working with this example.

Board:  Silicon Labs EFM32G Starter Kit (Gxxx_STK)
Device: EFM32G890F128
PD1 - TIM0_CC0 #3 (Expansion Header Pin 6)

Board:  Silicon Labs EFM32ZG Starter Kit (STK3200)
Device: EFM32ZG222F32
PD6 - TIM1_CC0 #4 (Expansion Header Pin 6)

Board:  Silicon Labs EFM32HG Starter Kit (SLSTK3400A)
Device: EFM32HG322F64
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32WG Starter Kit (STK3800)
Device: EFM32WG990F256
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32GG Starter Kit (STK3700)
Device: EFM32GG990F1024
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32LG Starter Kit (STK3600)
Device: EFM32LG990F256
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32TG Starter Kit (STK3800)
Device: EFM32TG840F32
PD6 - TIM1_CC0 #4 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC10 - TIM0_CC0 #15 (Expansion Header Pin 16)

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A)
Device: EFR32BG1P232F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A)
Device: EFR32BG12P332F1024GL125
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG13 Starter Kit (BRD4104A)
Device: EFR32BG13P632F512GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A)
Device: EFR32FG1P133F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A)
Device: EFR32FG12P433F1024GL125
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG13 Starter Kit (BRD4256A)
Device: EFR32FG13P233F512GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG14 Starter Kit (BRD4257A)
Device: EFR32FG14P233F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A)
Device: EFR32MG1P232F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG12P Starter Kit (BRD4161A)
Device: EFR32MG12P432F1024GL125
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG13P Starter Kit (BRD4159A)
Device: EFR32MG13P632F512GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG14 Starter Kit (BRD4169B)
Device: EFR32MG14P733F256GM48
PC10 - TIM0_CC0 #15 (Expansion Header Pin 15)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PC13 - TIM1_CC0 #0 (Breakout Pads Pin 23)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PC13 - TIM1_CC0 #0 (Expansion Header Pin 9)

