spi_slave

This example demonstrates USART as a SPI slave.
Data received on the MOSI pin is processed by an interrupt.
Once moved to a buffer the same interrupt moves data from
the TxBuffer into the TXDATA register which will send data 
to the master on the next transfer.

Note: The TXDATA register for the slave must be preloaded
before the first transfer. Due to USART being synchronous,
whenever the master sends data to the slave, the slave
will send whatever is in it's TXDATA register back. If the first
value is not preloaded the first byte back to the master
on the first transfer will always be junk. 

How to connect the master board to slave board:
Connect master CS to slave CS
Connect master SCLK to slave SCLK
Connect master MOSI to slave MOSI
Connect master MISO to slave MISO

How To Test:
1. Build the project and download to the Starter Kit
2. Build spi_master project and download to a Starter Kit
3. Place breakpoint in slave's USART1_RX_IRQHandler() per comments
4. Run spi_slave 
5. Run spi_master 
6. After hitting the breakpoint, observe RxBuffer[] in IDE variables/expressions window. 
   RxBuffer[] should contain 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09.

Peripherals Used:
HFRCO  - 19 MHz
USART1 - Synchronous (SPI) mode, CLK = 1 MHz


Board:  Silicon Labs EFM32BG1P Starter Kit (BRD4100A) + 
        Wireless Starter Kit Mainboard
Device: EFM32BG1P232F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32BG13 Starter Kit (BRD4104A) + 
        Wireless Starter Kit Mainboard
Device: EFR32BG13P632F512GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board:  Silicon Labs EFM32FG1P Starter Kit (BRD4250A) + 
        Wireless Starter Kit Mainboard
Device: EFM32FG1P133F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32FG13 Starter Kit (BRD4256A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG13P233F512GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32FG14 Starter Kit(BRD4257A) + 
        Wireless Starter Kit Mainboard
Device: EFR32FG14P233F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board:  Silicon Labs EFM32MG1P Starter Kit (BRD4151A) + 
        Wireless Starter Kit Mainboard
Device: EFM32MG1P232F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10

Board: Silicon Labs EFR32MG13 Starter Kit (BRD4159A) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG13P632F512GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10


Board: Silicon Labs EFR32MG14 Starter Kit(BRD4169B) + 
        Wireless Starter Kit Mainboard
Device: EFR32MG14P733F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10


Board:  Silicon Labs EFM32PG1 Starter Kit (SLSTK3401A)
Device: EFM32PG1B200F256GM48
PC6 - USART1 MOSI - EXP Header Pin 4
PC7 - USART1 MISO - EXP Header Pin 6
PC8 - USART1 CLK  - EXP Header Pin 8
PC9 - USART1 CS   - EXP Header Pin 10