/********************************************************************************************
**
**  File:   initPLL.c

*********************************************************************************************/
#include <def21469.h>
#include <cdef21469.h>

//#define DDR2TRFC29 (0x1D << 26)
#define DDR2TRFC29 (0x1D << 21)  // RFC bits are available on bits 21:28 of the DD2CTL4
#define RDIV	(0x6CD)
#define dll_lock_period 3000  // 512 DDCLK cycles

/*
** The following definition is a good programming practice to follow,
** in order to prevent the compiler from optimizing out any instructions
** that it may think are non-essential.
** It also makes code MISRA rule 2.1 compliant.
*/
#define NOP asm volatile("nop;")


void initPLL_DDR2(){

/********************************************************************************************/

int i, pmctlsetting;



    pmctlsetting= *pPMCTL;
    pmctlsetting &= ~(0xFF); //Clear

	// Set INDIV bit in PMCTL register
	pmctlsetting= *pPMCTL;
	pmctlsetting|= INDIV;
	*pPMCTL= pmctlsetting;

	// Program PLL multiplier to same value as CLK_CFGx pins/previously programmed value in software……
	*pPMCTL= pmctlsetting;

	// then place PLL in bypass mode
	pmctlsetting|= PLLBP;
	*pPMCTL= pmctlsetting;

    //Wait for recommended number of cycles
    for (i=0; i<4096; i++)
          NOP;

	// Bring PLL out of bypass mode by clearing PLLBP bit
    *pPMCTL ^= PLLBP;

	for (i=0; i<16; i++)
          NOP;

	pmctlsetting = *pPMCTL;
	// Clear the previous PLL multiplier
	pmctlsetting &= ~PLLM63;
	// Clear the INDIV bit
	pmctlsetting &= ~INDIV;
	// or set the INDIV bit
	// *pMCTL |= INDIV;
	*pPMCTL= pmctlsetting;
	
// Core clock = (25MHz * 2* 18) /(2 * 1) = 450 MHz
// DDR2 clock = 450 / 2 = 225 MHz.
    pmctlsetting= DDR2CKR2|PLLM18|DIVEN|LPCKR3|MLBSYSCKR4;
    *pPMCTL= pmctlsetting;
    pmctlsetting|= PLLBP;
    pmctlsetting^= DIVEN;
    *pPMCTL= pmctlsetting;



    //Wait for around 4096 cycles for the pll to lock.
    for (i=0; i<4096; i++)
          NOP;

    *pPMCTL ^= PLLBP;       //Clear Bypass Mode
    *pPMCTL |= (CLKOUTEN);  //and start clkout


   // Wait 16 cycles after pll has emerged from bypass

   for (i =0; i<16; i++)
          NOP;



//tRCD - 15ns
//tRP - 15ns
//tRC - 60ns
//tRRD - 10ns
//tRASmin - 45ns
//tRFC - 127.5ns
//tFAW - 50ns
//tRTP - 7.5ns
//tWR - 15ns
//tWTR - 7.5ns
// tREFI - 7.8ns

//For the case, tDDR2CLK = 1/225 MHz = 4.4ns
// CAS Latency = 4
// tRCD = 15 / 4.4 = 4(3.4)
// tRP = 15 / 4.4 = 4(3.4)
// tRC = 60 / 4.4 = 14(13.6)
// tRRD = 10/4.4 = 3(2.27)
// tRAS = 40/ 4.4 = 9(9.0)
// tRFC = 127.5/4.4 = 29(28.9) = 0x1D
// tFAW = 50/4.4 = 12(11.36)
// tRTP = 7.5 / 4.4 = 2(1.70)
// tWR = 15 / 4.4 = 4(3.4)
// tWTR = 7.5 / 4.4 = 2(1.70)

// RDIV = ((f DDCLK X t REFI)) - (t RAS + t RP)
// RDIV = (225 X 10^6 x 7.8 x 10^-6) - (10 + 4)
// RDIV = 1741 = 0x6CD






/********************************************************************/
/* On chip DLL reset. (Not required if frequency is not changed) */
/* DLL locks to new frequency after this reset */
/********************************************************************/
	*pDLL0CTL1 |= 0xE00;
	*pDLL1CTL1 |= 0xE00;
	// Wait for DLL to get reset
    for (i=0; i<8; i++)
          NOP;
/********************************************************************/
/* Wait until DLL locks to new frequency */
/********************************************************************/
	// Wait for a fixed period of time





    for (i=0; i<dll_lock_period; i++)
          NOP;

	/* OR */
	// Wait until the lock status bit in both the DLL status registers get set
/********************************************************************/
/* Enable external bank(s) in DDR2 mode */
/********************************************************************/

	*pEPCTL= 0x0;
	*pEPCTL = B0SD | EPBRROT | DMAPRROT | NOFRZDMA | NOFRZCR | NOFRZSP ;

/********************************************************************/
/* Program the control registers */
/********************************************************************/



	*pDDR2RRC = DDR2TRFC29 | RDIV;
	*pDDR2CTL1 = DDR2TFAW12 | DDR2TRRD3 | DDR2TRTP2 | DDR2TRCD4 | DDR2TWTR2 | DDR2TRP4 | DDR2TRAS10 ;
	*pDDR2CTL2 = DDR2MR | DDR2TWR4 | DDR2CAS4 | DDR2BL4 ;
	*pDDR2CTL3 = DDR2EXTMR1 | DDR2AL0 | DDR2ODTDIS | DDR2DLLEN;
	*pDDR2CTL4 = DDR2EXTMR2;
	*pDDR2CTL5 = DDR2EXTMR3;
	*pDDR2CTL0 = DDR2PSS | DDR2RAW13 | DDR2CAW10 | DDR2BC8 | DDR2WDTHx16 | DDR2OPT | DDR2MODIFY1;

/********************************************************************/
/* Wait for DLL calibration to complete ( On chip DLL) */
/********************************************************************/
	//while(!(DDR2STAT0 & DDR2DLLCALDONE))
	while(!(*pDDR2STAT0 & DDR2DLLCALDONE))
		NOP;

	NOP;
	NOP;
	NOP;
	NOP;




    //SRAM Settings
    *pSYSCTL |= EPDATA32;
	*pAMICTL1 = AMIEN | BW8 | WS31 ;

}

