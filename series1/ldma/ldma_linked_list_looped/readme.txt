ldma_linked_list_looped

In this example, the LDMA transfers 4 character strings from one software
array to another.  It switches between A and B four times, then transfers 
C, like so:
A, B, A, B, A, B, A, B, C

The transfer is requested by software at the end of the LDMA initialization.

If you wanted to have the transfers run automatically without interrupts, 
you can remove from initLdma() the following lines:

	/* Enable interrupts */
	descLink[0].xfer.doneIfs = true;
	descLink[1].xfer.doneIfs = true;
	descLink[2].xfer.doneIfs = true;

	/* Disable automatic triggers */
	descLink[0].xfer.structReq = false;
	descLink[1].xfer.structReq = false;
	descLink[2].xfer.structReq = false;
	
	... (Do not remove the lines between these)
	
	/* Request next transfer */
	LDMA->SWREQ |= LDMA_CH_MASK;
  
Also remove from LDMA_IRQHandler() the following lines:

	/* Request first transfer */
	LDMA->SWREQ |= LDMA_CH_MASK;

This example is based on the Reference Manual Section 7.4.4:
Chapter: LDMA
Section: Examples
Subsection: Example #4

How to Test:
1) Update the kit's firmware from the Simplicity Launcher (if necessary)
2) Build the project and download the Starter Kit
3) Open Simplicity Debugger and add "dstBuffer" to the Expressions window
4) Add a breakpoint at the beginning of LDMA_IRQHandler()
5) Run the debugger.  It should halt inside the interrupt subroutine with
the first descriptor complete (this can be seen in the Expressions window)
6) Resume the program.  The debugger should halt inside the interrupt 
subroutine again, after the next descriptor has completed.  You can do
this 7 more times, then the LDMA tranfer will be complete.


