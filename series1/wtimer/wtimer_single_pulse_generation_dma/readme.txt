wtimer_single_pulse_generation_dma

This project demonstrates the generation of a single pulse using output compare
and the LDMA. WTIMER is initialized for output compare on Compare/Capture
channel 0 which is routed to the GPIO Pin specified below. The timer is
configured in one-shot mode and an interrupt triggers after the first compare to
load the second compare value to the CCV. The LDMA is configured to transfer a
single word to the CCV register once the first compare event occurs. The values
loaded into the CCV are such that a 1ms pulse is generated after a 1 second
delay.

================================================================================

Peripherals Used:
WTIMER0 - HFPERCLK 19 MHz
LDMA

================================================================================

How To Test:
1. Build the project and download it to the Starter Kit
2. Measure the GPIO pin specified below
3. A 1 ms pulse should be generated after 1 second

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

