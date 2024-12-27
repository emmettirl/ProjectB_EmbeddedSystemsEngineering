# ProjectB_EmbeddedSystemsEngineering

## Question 1
Within t.c, the function "IRQ_handler()" (Interrupt Request Handler) controls the flow of the program when an interrupt occurs.


There is a function in timer.c called "timer_handler()", which is called when an interrupt occurs. 
The function checks two registers to determine the source of the interrupt.
VIC_STATUS (Vectored Interrupt Controller) and SIC_STATUS (Secondary Interrupt Controller) indicate which interrupt is active, with each bit being a different interrupt source.
The function then calls the appropriate handler function based on the interrupt source.
The handlers called are "timer_handler()", uart0_handler(), uart1_handler(), vid_handler() and sic_handler(). 
It then clears the interrupt. 


## Question 2