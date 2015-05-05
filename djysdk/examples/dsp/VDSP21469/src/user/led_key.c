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
#include <math.h>
#include "os_inc.h"
#include "led_key.h"

/*******************************************************************
*  function prototypes
*******************************************************************/
//void SW8_IRQ1_handler(int sig_int);
//void SW9_IRQ2_handler(int sig_int);
//void SW10_SW11_DAI_handler(int sig_int);
void Delay(const int n);

void SW8_IRQ1_handler(void);
void SW9_IRQ2_handler(void);
void SW10_DAI_handler(void);
void SW11_DAI_handler(void);
void UARTisr();			//function for receive interrupt
void xmitUARTmessage(char*, int); // function to transmit the welcome message
ufast_t __int_engine_dai(void);
ufast_t __int_engine_dpi(void);

/*******************************************************************
*  global variables
*******************************************************************/
static int g_nNumLEDs = 8;

extern bool g_bIsTestChosen;
extern int g_nSelectedTest;
extern volatile int g_nPostState;
extern bool g_bLoopOnTest;


int g_LEDs[] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8};

u8 const cn_int_leading_zero[]=
{
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
}; 


/*******************************************************************
*   Function:    Init_LEDs
*   Description: This function configures PORTH for LEDs.
*******************************************************************/
void Init_LEDs(void)
{	
	/* setting the SRU and route so that flag pins connect to DPI pin buffers */
	
	/* use flags 4 to 15 only, flags 0 to 3 not available on the DPI */

	SRU(FLAG6_O,DPI_PB06_I);	/* connect Flag6 output to DPI_PB06 input (LED1) */
	SRU(FLAG7_O,DPI_PB13_I);	/* connect Flag7 output to DPI_PB13 input (LED2) */
	SRU(FLAG4_O,DPI_PB14_I);	/* connect Flag4 output to DPI_PB14 input (LED3) */

	SRU(LOW,DAI_PB03_I);	    /* connect Input LOW to LED4 */
	SRU(LOW,DAI_PB04_I);    	/* connect Input LOW to LED5 */
	SRU(LOW,DAI_PB15_I);		/* connect Input LOW to LED6 */
	SRU(LOW,DAI_PB16_I);		/* connect Input LOW to LED7 */
	SRU(LOW,DAI_PB17_I);		/* connect Input LOW to LED8 */

	/* enabling the buffer using the following sequence: high -> output, low -> input */

	SRU(HIGH,DPI_PBEN06_I);		/* LED1 */
	SRU(HIGH,DPI_PBEN13_I);		/* LED2 */
	SRU(HIGH,DPI_PBEN14_I);		/* LED3 */
	SRU(HIGH,PBEN03_I);			/* LED4 */
	SRU(HIGH,PBEN04_I);			/* LED5 */
	SRU(HIGH,PBEN15_I);			/* LED6 */
	SRU(HIGH,PBEN16_I);			/* LED7 */
	SRU(HIGH,PBEN17_I);			/* LED8 */
	
	/* setting flag pins as outputs */
	sysreg_bit_set( sysreg_FLAGS, (FLG3O|FLG4O|FLG5O|FLG6O|FLG7O|FLG8O) );
	
	/* clearing flag pins */
	sysreg_bit_clr( sysreg_FLAGS, (FLG3|FLG4|FLG5|FLG6|FLG7|FLG8) );
}

/*******************************************************************
*   Function:    Init_PushButtons
*   Description: This function configures PORTH and interrupts
*					for push buttons.
*******************************************************************/
void Init_PushButtons(void)
{
	/* pin assignments in SRU_PIN3 (Group D) */

	SRU(LOW,DAI_PB19_I);	/* assign pin buffer 19 low so it is an input */
	SRU(LOW,DAI_PB20_I);	/* assign pin buffer 20 low so it is an input */

	/* route MISCA signals in SRU_EXT_MISCA (Group E) */

    SRU(DAI_PB19_O,MISCA1_I);	/* route so that DAI pin buffer 19 connects to MISCA1 */
    SRU(DAI_PB20_O,MISCA2_I);	/* route so that DAI pin buffer 20 connects to MISCA2 */

    /* pin buffer disable in SRU_PINEN0 (Group F) */
	
    SRU(LOW,PBEN19_I);	/* assign pin 19 low so it is an input */
	SRU(LOW,PBEN20_I);	/* assign pin 20 low so it is an input */
    

}


/*******************************************************************
*   Function:    LED_Bar
*   Description: Display a blinking LED bar
*******************************************************************/
void LED_Bar(const int iSpeed)
{
	int n,j;

	for( n = 0; n <= g_nNumLEDs-1; n++ )
	{
		ClearSet_LED(g_LEDs[n], LED_TOGGLE);
		Delay(iSpeed);
	}
}


/*******************************************************************
*   Function:    LED_Bar_Reverse
*   Description: Display a blinking LED bar in reverse
*******************************************************************/
void LED_Bar_Reverse(const int iSpeed)
{
	int n,j;

	for( n = g_nNumLEDs-1; n >= 0; n-- )
	{
		ClearSet_LED(g_LEDs[n], LED_TOGGLE);
		Delay(iSpeed);
	}
}

/*******************************************************************
*   Function:    LED_Bar_BacknForth
*   Description: Display a blinking LED bar that goes back and forth.
*******************************************************************/
void LED_Bar_BacknForth(const int iSpeed)
{
	static unsigned int state = 0; /* remember the state */

	if ( (state % 4) == 0 )
	{
		ClearSet_LED(LED1, 1);
		ClearSet_LED(LED2, 0);
		ClearSet_LED(LED3, 0);
	}
	else if ( (state % 4) == 1 )
	{
		ClearSet_LED(LED1, 0);
		ClearSet_LED(LED2, 1);
		ClearSet_LED(LED3, 0);
	}
	else if ( (state % 4) == 2 )
	{
		ClearSet_LED(LED1, 0);
		ClearSet_LED(LED2, 0);
		ClearSet_LED(LED3, 1);
	}
	else
	{
		ClearSet_LED(LED1, 0);
		ClearSet_LED(LED2, 1);
		ClearSet_LED(LED3, 0);
	}

	Delay(iSpeed);
	state++;
}


/*******************************************************************
*   Function:    Strobe_LEDs
*   Description: This strobes the LEDs based on the pattern provide
*				 with a specified delay until the condition is met.
*				 If the condition is NULL, we strobe once and exit.
*******************************************************************/
void Strobe_LEDs(	unsigned int pattern_a, unsigned int pattern_b,
					unsigned int delay, bool *pbDone)
{
	int cnt = 0;
	volatile bool b = false;

	/* use a copy in case pbDone is NULL */
	if ( NULL != pbDone )
		b = *pbDone;

	while (!b)
	{
		for(cnt = delay; cnt > 0; cnt--)
		{
			ClearSet_LED_Bank( pattern_a );
			Delay(cnt);				/* delay decreasing */

			ClearSet_LED_Bank( pattern_b );
			Delay(delay - cnt);		/* delay increasing */
		}

		/* check it again */
		if ( NULL != pbDone )
		{
			b = *pbDone;
			if (b)
				break;		/* if not NULL see if condition has now been met */
		}
		else
			b = true;	/* after one time break out if NULL */

		for(cnt = 0; cnt < delay; cnt++)
		{
			ClearSet_LED_Bank( pattern_a );
			Delay(cnt);				/* delay increasing */

			ClearSet_LED_Bank( pattern_b );
			Delay(delay - cnt);		/* delay decreasing */
		}
	}
}


/*******************************************************************
*   Function:    Blink_LED
*   Description: Blink various LEDs
*******************************************************************/
void Blink_LED(const int iSpeed,unsigned char iNum1,unsigned char iNum2)
{
	unsigned char n;

	for( n = iNum1-1; n <= iNum2-1; n++ )
	{
		ClearSet_LED(g_LEDs[n], LED_TOGGLE);
		Delay(iSpeed);
	}
}
/*
void Blink_LED_12(const int iSpeed)
{
	int n, j;

	for( n = 0; n <= 1; n++ )
	{
		ClearSet_LED(g_LEDs[n], LED_TOGGLE);
		Delay(iSpeed);
	}
}
void Blink_LED_45(const int iSpeed)
{
	int n, j;

	for( n = 3; n <= 4; n++ )
	{
		ClearSet_LED(g_LEDs[n], LED_TOGGLE);
		Delay(iSpeed);
	}
}
void Blink_LED_78(const int iSpeed)
{
	int n, j;

	for( n = 6; n <= 7; n++ )
	{
		ClearSet_LED(g_LEDs[n], LED_TOGGLE);
		Delay(iSpeed);
	}
}*/

/*******************************************************************
*   Function:    ClearSet_LED_Bank
*   Description: Clear or sets the group of LEDs based on specified state.
*******************************************************************/
void ClearSet_LED_Bank(const int iState)
{
	int n;
	int nTempState = iState;


	for( n = 0; n <= g_nNumLEDs-1; n++ )
	{
		ClearSet_LED(g_LEDs[n], (nTempState & 0x3) );
		nTempState >>= 2;
	}
}


/*******************************************************************
*   Function:    ClearSet_LED
*   Description: Clear or set a particular LED (not a group of LEDs).
*******************************************************************/
void ClearSet_LED(const int led, const int bState)
{
	/* if bState is 0 (LED_OFF) we clear the LED,
	   if bState is 1 (LED_ON) we set the LED,
	   else we toggle the LED */

	switch( led )
	{
		case LED1:
		{
			if( LED_OFF == bState ) 	{ sysreg_bit_clr(sysreg_FLAGS, FLG6); }
			else if( LED_ON == bState )	{ sysreg_bit_set(sysreg_FLAGS, FLG6); }
			else	/* toggle */		{ sysreg_bit_tgl(sysreg_FLAGS, FLG6); }
		}
		break;

		case LED2:
		{
			if( LED_OFF == bState ) 	{ sysreg_bit_clr(sysreg_FLAGS, FLG7); }
			else if( LED_ON == bState )	{ sysreg_bit_set(sysreg_FLAGS, FLG7); }
			else	/* toggle */		{ sysreg_bit_tgl(sysreg_FLAGS, FLG7); }
		}
		break;

		case LED3:
		{
			if( LED_OFF == bState ) 	{ sysreg_bit_clr(sysreg_FLAGS, FLG4); }
			else if( LED_ON == bState )	{ sysreg_bit_set(sysreg_FLAGS, FLG4); }
			else	/* toggle */		{ sysreg_bit_tgl(sysreg_FLAGS, FLG4); }
		}
		break;
		
		case LED4:
		{
			if( LED_OFF == bState ) 	{ SRU(LOW,DAI_PB03_I); }
			else if( LED_ON == bState )	{ SRU(HIGH,DAI_PB03_I); }
			else /* toggle */ 			{ (*pSRU_PIN0) ^= 0x00004000; }
		}
		break;
		
		case LED5:
		{
			if( LED_OFF == bState ) 	{ SRU(LOW,DAI_PB04_I); }
			else if( LED_ON == bState )	{ SRU(HIGH,DAI_PB04_I); }
			else /* toggle */ 			{ (*pSRU_PIN0) ^= 0x00200000; }
		}
		break;
		
		case LED6:
		{
			if( LED_OFF == bState ) 	{ SRU(LOW,DAI_PB15_I); }
			else if( LED_ON == bState )	{ SRU(HIGH,DAI_PB15_I); }
			else /* toggle */ 			{ (*pSRU_PIN3) ^= 0x00004000; }
		}
		break;
		
		case LED7:
		{
			if( LED_OFF == bState ) 	{ SRU(LOW,DAI_PB16_I); }
			else if( LED_ON == bState )	{ SRU(HIGH,DAI_PB16_I); }
			else /* toggle */ 			{ (*pSRU_PIN3) ^= 0x00200000; }
		}
		break;
		
		case LED8:
		{
			if( LED_OFF == bState ) 	{ SRU(LOW,DAI_PB17_I); }
			else if( LED_ON == bState )	{ SRU(HIGH,DAI_PB17_I); }
			else /* toggle */ 			{ (*pSRU_PIN4) ^= 0x00000001; }
		}
		break;
	}
}


/*******************************************************************
*   Function:    Delay
*   Description: executes a specified number of nops
*******************************************************************/

void Delay(const int n)
{	
	int nDelay,mDelay;
	for( nDelay = 0; nDelay < n; nDelay++)
	{
		NOP;
	}
}



/*******************************************************************
*   Function:    SW8_IRQ1_handler
*   Description: Called each time SW8 is pressed.
*******************************************************************/
extern ptu32_t __djy_main(void);
extern struct tagSemaphoreLCB *thread_change_test1;
extern struct tagSemaphoreLCB *thread_change_test2;
extern struct tagSemaphoreLCB *thread_change_test3;
extern load_application_parallel();
/*
volatile bool second_stage_loading=false;

void SW8_IRQ1_handler(void)
{
	
#ifdef Monitor_version
	xmitUARTmessage("IRQ1 answer \n\r",sizeof("IRQ1 answer \n\r")); 
	NOP;
	second_stage_loading=true;
#endif

#ifdef Release_version
	second_stage_loading=true;
	 // Load an application from parallel FLASH
#endif

}
*/

/*******************************************************************
*   Function:    SW9_IRQ2_handler
*   Description: Called each time SW9 is pressed.
*******************************************************************/
void SW9_IRQ2_handler(void)
{
	xmitUARTmessage("IRQ2 answer \n\r",sizeof("IRQ2 answer \n\r")); 
	NOP;
}

/*******************************************************************
*   Function:    SW10_SW11_DAI_handler
*   Description: Called each time SW10 or SW11 is pressed.
*******************************************************************/


void SW10_DAI_handler(void)
{
/*	uint16_t evtt_main;
    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
                                __djy_main,CN_CFG_MAIN_STACK,"main function");
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0,190);
    */
//    Lock_SempPost(thread_change_test1);
		
	xmitUARTmessage("SW10 answer \n\r",sizeof("SW10 answer \n\r")); 
	NOP;
}

void SW11_DAI_handler(void)
{	
	Lock_SempPost(thread_change_test1);
//	xmitUARTmessage("SW11 answer \n\r",sizeof("SW11 answer \n\r")); 
	NOP;

}


void UARTisr()
{
 int value;
 
 value = *pUART0RBR;
 
 /* echoes back the value on to the hyperterminal screen*/
 
  
  while ((*pUART0LSR & UARTTHRE) == 0)
  {;
  }
 
  *pUART0THR = value;
  
  /* poll to ensure UART has completed the transfer */
  while ((*pUART0LSR & UARTTEMT) == 0)
   {;
   } 
   
}

void xmitUARTmessage(char*xmit, int SIZE)
{
	int i;
	
	/* loop to transmit source array in core driven mode */   
  for (i=0; i<SIZE; i++)
  {
    // Wait for the UART transmitter to be ready
    do { 
    	;}
    while ((*pUART0LSR & UARTTHRE) == 0);
    
    //Transmit a byte
    *pUART0THR = xmit[i]; 
   }
  
/* poll to ensure UART has completed the transfer */
  while ((*pUART0LSR & UARTTEMT) == 0)
   {;
   } 

}


//----DAI中断引擎--------------------------------------------------------------
//功能：DAI中断引擎
//参数：无
//返回：ufast_t dai_line
//-----------------------------------------------------------------------------
ufast_t __int_engine_dai( void )
{		
	ufast_t dai_line;
	u8 int_cont=0;
	
	dai_line = (*pDAI_IRPTL_HS);	// read the DAIHI Shadow register
	
	int_cont = *pDAI_IRPTL_H;		//read to clear
    int_cont = *pDAI_IRPTL_L;		//read to clear
	
	int_cont = cn_int_leading_zero[(dai_line>>8)&0x000000ff];
	if(int_cont == 8)// all zero
		{
			int_cont = cn_int_leading_zero[(dai_line>>16)&0x000000ff];
			if(int_cont == 8)// all zero
				{
					// last chance
					int_cont = cn_int_leading_zero[(dai_line>>24)&0x000000ff];
					dai_line = 31-int_cont;
				}
			else// stop search	
				{
					dai_line = 23-int_cont;
				}
		}
	else// stop search
		{
			dai_line = 15-int_cont;
		}
	dai_line += cn_int_line_dai_base;			// caculate the real int num
	
	if(dai_line==cn_int_line_EXTMISCA1) SW10_DAI_handler();
	else if(dai_line==cn_int_line_EXTMISCA2) SW11_DAI_handler();

	return dai_line;
	
}

//----DPI中断引擎--------------------------------------------------------------
//功能：DAI中断引擎
//参数：无
//返回：ufast_t dpi_line
//-----------------------------------------------------------------------------
ufast_t __int_engine_dpi( void )
{		
	ufast_t dpi_line;
	u8 int_cont=0;

	dpi_line = (*pDPI_IRPTL_SH);	// read the DPII Shadow register
	
	int_cont = *pDPI_IRPTL;			//read to clear
	
	int_cont = cn_int_leading_zero[dpi_line&0x000000ff];
	if(int_cont == 8)// all zero
		{
			// last chance
			int_cont = cn_int_leading_zero[(dpi_line>>8)&0x000000ff];
			dpi_line = 15-int_cont;
		}
	else// stop search
		{
			dpi_line = 7-int_cont;
		}
	dpi_line += cn_int_line_dpi_base;		// caculate the real int num

	return dpi_line;

}
