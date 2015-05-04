/*******************************************************************
Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software
you agree to the terms of the associated Analog Devices License Agreement.

Project Name:  	Power_On_Self_Test

Hardware:		ADSP-21469 EZ-Board

Description:	This examples configures pushbutton and LED tests on the EZ-Board.
*******************************************************************/

#include <cdef21469.h>
#include <def21469.h>
#include <sru.h>
#include <signal.h>
#include <sysreg.h>
#include <string.h>
	
#include "VDSP21469_uart.h"
#include "led_key.h"

/*******************************************************************
*  function prototypes
*******************************************************************/
void initDPI(void);		//makes UART0 signal connections to DPI pins
void initUART(void);	//function for initializing the UART


/*******************************************************************
*  global variables
*******************************************************************/



void initDPI()
{
	
  SRU2(UART0_TX_O,DPI_PB09_I); // UART transmit signal is connected to DPI pin 9
  SRU2(HIGH,DPI_PBEN09_I);
  SRU2(DPI_PB10_O,UART0_RX_I); // connect the pin buffer output signal to the UART0 receive
  SRU2(LOW,DPI_PB10_I);
  SRU2(LOW,DPI_PBEN10_I);      // disables DPI pin10 as input
}

void initUART()
{
	/* Sets the Baud rate for UART0 */	
	*pUART0LCR = UARTDLAB;  //enables access to Divisor register to set bauda rate
	*pUART0DLL = 0xb8;      //0x5b8 = 1464 for divisor value and gives a baud rate of 9600 for core clock 262.144MHz
    *pUART0DLH = 0x05; 
  
    /* Configures UART0 LCR */ 
    *pUART0LCR = UARTWLS8; 				// word length 8
                				        // parity enable None parity
 				                        // One stop bits
               
               
    *pUART0RXCTL = UARTEN;       //enables UART0 in receive mode

    *pUART0TXCTL = UARTEN;       //enables UART0 in core driven mode
  
}


