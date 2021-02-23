adc_scan_interrupt

This project demonstrates using the ADC0 peripheral to take single-ended
analog measurements across different channels. In this example, single-ended
inputs are selected from both APORTnX and APORTnY buses. For single-ended inputs 
selected from the APORTnY bus, the ADC performs a negative single-ended
conversion and automatically inverts the result. ADC interrupts handle 
completed conversions and store the results in a software array.

How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Build the project and download to the Starter Kit
3. Open the Simplicity Debugger and add "inputs" to the Expressions window
4. Observe the measured voltage on the pin in mV in the expressions window
and how it respond to different voltage values on the corresponding EXP header
pin (see below)

Peripherals Used:
HFPER   - 19 MHz
ADC     - 16 MHz, 12-bit resolution, 2.5V internal reference


Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 16)

Board:  Silicon Labs EFM32PG12 Starter Kit (SLSTK3402A)
Device: EFM32PG12B500F1024GL125
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 3)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 16)

Board:  Silicon Labs EFR32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG1P232F256GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG12P Starter Kit (BRD4103A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG12P332F1024GL125
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32BG13 Radio Board (SLWRB4104A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG1P133F256GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG12P Starter Kit (BRD4253A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG12P433F1024GL125
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG13 Radio Board (SLWRB4256A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32FG14 Radio Board (SLWRB4257A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48
PA2 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 3)
PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFR32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG1P232F256GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG12 Radio Board (SLWRB4161A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG12P432F1024GL125
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 13)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG13 Radio Board (SLWRB4159A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48
PC9  - ADC0 Port 2X Channel 9  (Expansion Header Pin 10)
PC10 - ADC0 Port 2Y Channel 10 (Expansion Header Pin 15)

Board:  Silicon Labs EFR32MG14 Radio Board (SLWRB4169A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
PA2 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 3)
PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)

Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A)
Device: EFM32GG11B820F2048GL192
PE10 - ADC0 Port 4Y Channel 10 (Expansion Header Pin 4)
PE11 - ADC0 Port 4X Channel 11 (Expansion Header Pin 6)

Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A)
Device: EFM32TG11B520F128GM80
PD2 - ADC0 Port 0X Channel 2 (Expansion Header Pin 7)
PD6 - ADC0 Port 0X Channel 6 (Expansion Header Pin 16)
