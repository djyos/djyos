/*****************************************************************************
 * VDSP21469.c
 *****************************************************************************/
#include <cdef21469.h>
#include <sysreg.h>
#include <SRU.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include "os_inc.h"
#include "led_key.h"
#include "uart.h"


#define NOP asm volatile("nop;")

volatile bool gb_sw8_pushed = false;
volatile bool gb_sw9_pushed = false;


unsigned char welcomemessage[] = {"Welcome to connect SHARC-ADSP-21469... \n\r"}; 
bool_t bool_value;


atom_high_t atom_high_test;
atom_low_t atom_low_test;

int main( void )
{
/****************UART SET*******************/

	*pPICR2 &= ~(0x1F); //Sets the UART0 receive interrupt to P12

	*pPICR2 |= (0x13<<0); 
	
	initPLL_DDR2();

	*pUART0LCR=0;
	
    *pUART0IER   = UARTRBFIE;    // enables UART0 receive interrupt
    
	
/****************SYS INIT*******************/
	initDPI();
	initUART();
	Init_LEDs();
	Init_PushButtons();
	
	sysreg_bit_clr(sysreg_MODE1, IRPTEN ); /* disable global interrupts */
	/* unmask individual interrupts */
	(*pDAI_IRPTL_PRI) = (SRU_EXTMISCA1_INT | SRU_EXTMISCA2_INT);
	
	/* make sure interrupts latch on the rising edge */
    (*pDAI_IRPTL_RE) = (SRU_EXTMISCA1_INT | SRU_EXTMISCA2_INT);

  	/* setup the IRQ pins to use the pushbuttons */
	(*pSYSCTL) |= (IRQ1EN | IRQ2EN);
	
	sysreg_bit_set(sysreg_MODE2, (IRQ1E | IRQ2E) );
	
	/* enable IRQ interrupts and make DAI interrupts high priority */
	sysreg_bit_set(sysreg_IMASK, (IRQ1I | IRQ2I | DAIHI) );
	sysreg_bit_clr(sysreg_IMASKP, (IRQ1I | IRQ2I | DAIHI) );
	sysreg_bit_set(sysreg_LIRPTL, P12IMSK );
	sysreg_bit_clr(sysreg_LIRPTL, P12IMSKP );
	
	sysreg_bit_clr(sysreg_IRPTL, (IRQ1I | IRQ2I) );
	sysreg_bit_clr(sysreg_LIRPTL, P12I );

	/* register the interrut handlers */
	Int_Init();
	
    Int_IsrConnect(cn_int_line_IRQ2I,SW9_IRQ2_handler);
    Int_RestoreRealLine(cn_int_line_IRQ2I);
    
    Int_IsrConnect(cn_int_line_IRQ1I,SW8_IRQ1_handler);
    Int_RestoreAsynLine(cn_int_line_IRQ1I);
    
    Int_IsrConnect(cn_int_line_DAII,__int_engine_dai);
    Int_RestoreAsynLine(cn_int_line_DAII);
    
	Int_IsrConnect(cn_int_line_P12I,UARTisr);
    Int_RestoreAsynLine(cn_int_line_P12I);
    
    Int_CutTrunk();
    Int_ContactTrunk();
    	
	sysreg_bit_clr(sysreg_IRPTL, (IRQ1I | IRQ2I) );
	NOP;NOP;
	sysreg_bit_clr(sysreg_LIRPTL, P12I );
    NOP;NOP;
	sysreg_bit_set(sysreg_MODE1, NESTM ); /* enable interrupts nest */
	sysreg_bit_set(sysreg_MODE1, IRPTEN ); /* enable global interrupts */
	
// 测试通过	
//	atom_high_test=Int_HighAtomStart();
//	Int_HighAtomEnd(atom_high_test);
//	atom_low_test=Int_LowAtomStart();
//	Int_LowAtomEnd(atom_low_test);
// 测试通过	
	Int_TapLine(cn_int_line_IRQ2I);
//	Int_TapLine(cn_int_line_DAII);
//	Int_TapLine(cn_int_line_P12I);
	Int_TapLine(cn_int_line_IRQ1I);
// 测试通过	
//  bool_value=Int_QueryLine(cn_int_line_IRQ2I);
//  bool_value=Int_QueryLine(cn_int_line_IRQ1I);
//  bool_value=Int_QueryLine(cn_int_line_DAII);
//  bool_value=Int_QueryLine(cn_int_line_P12I);
// 测试通过	
//  Int_CutLine(cn_int_line_SFT3I);
//  Int_CutLine(cn_int_line_P17I);
//  Int_CutLine(cn_int_line_IRQ2I);
    
/****************UART TEST******************/

    xmitUARTmessage(welcomemessage,sizeof(welcomemessage));   
	xmitUARTmessage("CYG Copyright \n\r",sizeof("CYG Copyright \n\r")); 
	
	while(1)
	{
		Blink_LED(10000000);
//		Int_TapLine(cn_int_line_IRQ2I);
//		xmitUARTmessage("CYG Copyright \n\r",sizeof("CYG Copyright \n\r")); 
//		Int_TapLine(cn_int_line_DAII);
	}
	
	/* Begin adding your custom code here */
	return 0;
}

