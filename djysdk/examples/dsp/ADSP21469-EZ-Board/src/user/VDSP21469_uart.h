/*******************************************************************************/
/*                                                                             */
/*   (C) Copyright 2008 - Analog Devices, Inc.  All rights reserved.           */
/*                                                                             */
/*    FILE:     uart.h ( )					  				                   */
/*                                                                             */
/*    CHANGES:  1.00.0  - initial release    								   */
/*																			   */
/*******************************************************************************/

#ifndef _VDSP21469_UART_H_
#define _VDSP21469_UART_H_



/* function prototypes */
void initDPI(void);		//makes UART0 signal connections to DPI pins
void initUART(void);	//function for initializing the UART
void UARTisr();			//function for receive interrupt
void xmitUARTmessage(char*, int); // function to transmit the welcome message

#endif /* _UART_H_ */
