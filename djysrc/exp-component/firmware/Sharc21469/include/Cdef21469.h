/************************************************************************
 *
 * Cdef21469.h
 *
 * (c) Copyright 2009 Analog Devices, Inc.  All rights reserved.
 *
 *********************************************************************** */

#ifndef __CDEF21469_H_
#define __CDEF21469_H_

#if 0
Cdef21469.h - SYSTEM AND IOP REGISTER BIT AND ADDRESS DEFINITIONS FOR ADSP-21469

This include file contains macros definitions of the IOP registers which
allow them to be used in C or C++ programs. Each macro is similar to that
in def21369.h, except that the macro is prefixed with 'p' and the relevant
casts are include in the macro definition. They can be used as follows:

    *pSYSTAT = 0x12345678;  /*  Set the SYSTAT register */
    
#endif

#if !defined(__ADI_HAS_MLB__)
# define __ADI_HAS_MLB__ 1
#endif

#if !defined(__ADI_HAS_DTCP__)
# define __ADI_HAS_DTCP__ 0
#endif

/*=========================================================================== */
/*                                                                            */
/*                  I/O Processor Register Address Memory Map                 */
/*                                                                            */
/*============================================================================*/

/* Emulation/Breakpoint Registers */
#define pEEMUIN      ((volatile unsigned int *)0x30020)   /* Emulator Input FIFO */
#define pEEMUSTAT    ((volatile unsigned int *)0x30021)   /* Enhanced Emulation Status Register */
#define pEEMUOUT     ((volatile unsigned int *)0x30022)   /* Emulator Output FIFO */
#define pOSPID       ((volatile unsigned int *)0x30023)   /* Operating System Process ID */
#define pSYSCTL      ((volatile unsigned int *)0x30024)   /* System Control Register */
#define pBRKCTL      ((volatile unsigned int *)0x30025)   /* Hardware Breakpoint Control Register */
#define pREVPID      ((volatile unsigned int *)0x30026)   /* Emulation/Revision ID Register */
#define pPSA1S       ((volatile unsigned int *)0x300a0)   /* Instruction Breakpoint Address Start #1 */
#define pPSA1E       ((volatile unsigned int *)0x300a1)   /* Instruction Breakpoint Address End #1 */
#define pPSA2S       ((volatile unsigned int *)0x300a2)   /* Instruction Breakpoint Address Start #2 */
#define pPSA2E       ((volatile unsigned int *)0x300a3)   /* Instruction Breakpoint Address End #2 */
#define pPSA3S       ((volatile unsigned int *)0x300a4)   /* Instruction Breakpoint Address Start #3 */
#define pPSA3E       ((volatile unsigned int *)0x300a5)   /* Instruction Breakpoint Address End #3 */
#define pPSA4S       ((volatile unsigned int *)0x300a6)   /* Instruction Breakpoint Address Start #4 */
#define pPSA4E       ((volatile unsigned int *)0x300a7)   /* Instruction Breakpoint Address End #4 */
#define pDMA1S       ((volatile unsigned int *)0x300b2)   /* Data Memory Breakpoint Address Start #1 */
#define pDMA1E       ((volatile unsigned int *)0x300b3)   /* Data Memory Breakpoint Address End #1 */
#define pDMA2S       ((volatile unsigned int *)0x300b4)   /* Data Memory Breakpoint Address Start #2 */
#define pDMA2E       ((volatile unsigned int *)0x300b5)   /* Data Memory Breakpoint Address End #2 */
#define pIOAS        ((volatile unsigned int *)0x300b0)   /* I/O Breakpoint Address Start */
#define pIOAE        ((volatile unsigned int *)0x300b1)   /* I/O Breakpoint Address End */
#define pPMDAS       ((volatile unsigned int *)0x300b8)   /* Program Memory Data Breakpoint Address Start #1 */
#define pPMDAE       ((volatile unsigned int *)0x300b9)   /* Program Memory Data Breakpoint Address End #1 */
#define pEMUN        ((volatile unsigned int *)0x300ae)   /* Number of Breakpoints before EMU interrupt */

/* IOP registers for AMI */

#define pAMICTL0    ((volatile unsigned int *)0x1804)    /* Asynchronous Memory Interface Control Register for Bank 1 */
#define pAMICTL1    ((volatile unsigned int *)0x1805)    /* Asynchronous Memory Interface Control Register for Bank 2 */
#define pAMICTL2    ((volatile unsigned int *)0x1806)    /* Asynchronous Memory Interface Control Register for Bank 3 */
#define pAMICTL3    ((volatile unsigned int *)0x1807)    /* Asynchronous Memory Interface Control Register for Bank 4 */
#define pAMISTAT    ((volatile unsigned int *)0x180a)    /* Asynchronous Memory Interface Status Register */

/* DMA address registers */
#define pDMAC0   ((volatile unsigned int *)0x180b)    /* External Port DMA Channel 0 Control Register */
#define pDMAC1   ((volatile unsigned int *)0x180c)    /* External Port DMA Channel 1 Control Register */

#define pEIEP0   ((volatile unsigned int *)0x1820)    /* External Port DMA external index address */
#define pEMEP0   ((volatile unsigned int *)0x1821)    /* External Port DMA external modifier */
#define pECEP0   ((volatile unsigned int *)0x1822)    /* External Port DMA external count */
#define pIIEP0   ((volatile unsigned int *)0x1823)    /* External Port DMA internal index address */
#define pIMEP0   ((volatile unsigned int *)0x1824)    /* External Port DMA internal modifier */
#define pICEP0   ((volatile unsigned int *)0x1825)    /* External Port DMA internal count */
#define pCEP0    ((volatile unsigned int *)0x1825)    /* External Port DMA internal count */
#define pCPEP0   ((volatile unsigned int *)0x1826)    /* External Port DMA chain pointer */
#define pEBEP0   ((volatile unsigned int *)0x1827)    /* External Port DMA external base address */
#define pTPEP0   ((volatile unsigned int *)0x1828)    /* External Port DMA tap pointer */
#define pELEP0   ((volatile unsigned int *)0x1829)    /* External Port DMA external length */
#define pEPTC0   ((volatile unsigned int *)0x182B)    /* External Port DMA delay line tap count */
#define pTCEP0   ((volatile unsigned int *)0x182B)    /* External Port DMA delay line tap count */
#define pDFEP0   ((volatile unsigned int *)0x182c)    /* Data FIFO              this is not mentioned in the list */

#define pEIEP1   ((volatile unsigned int *)0x1830)    /* External Port DMA external index address */
#define pEMEP1   ((volatile unsigned int *)0x1831)    /* External Port DMA external modifier */
#define pECEP1   ((volatile unsigned int *)0x1832)    /* External Port DMA external count */
#define pIIEP1   ((volatile unsigned int *)0x1833)    /* External Port DMA internal index address */
#define pIMEP1   ((volatile unsigned int *)0x1834)    /* External Port DMA internal modifier */
#define pICEP1   ((volatile unsigned int *)0x1835)    /* External Port DMA internal count */
#define pCEP1    ((volatile unsigned int *)0x1835)    /* External Port DMA internal count */
#define pCPEP1   ((volatile unsigned int *)0x1836)    /* External Port DMA chain pointer */
#define pEBEP1   ((volatile unsigned int *)0x1837)    /* External Port DMA external base address */
#define pTPEP1   ((volatile unsigned int *)0x1838)    /* External Port DMA tap pointer */
#define pELEP1   ((volatile unsigned int *)0x1839)    /* External Port DMA external length */
#define pEPTC1   ((volatile unsigned int *)0x183B)    /* External Port DMA delay line tap count */
#define pTCEP1   ((volatile unsigned int *)0x183B)    /* External Port DMA delay line tap count */
#define pDFEP1   ((volatile unsigned int *)0x183c)    /* Data FIFO */

/* SPORT */
#define pSPERRSTAT    ((volatile unsigned int *)0x2300)   /* SPORT Global Interrupt Status Register */

/* Serial Port registers (SP01) */
#define pSPCTL0      ((volatile unsigned int *)0xc00)     /* SPORT 0 control register */
#define pSPCTL1      ((volatile unsigned int *)0xc01)     /* SPORT 1 control register */
#define pDIV0        ((volatile unsigned int *)0xc02)     /* SPORT 0 divisor for transmit/receive SCLK0 and FS0 */
#define pDIV1        ((volatile unsigned int *)0xc03)     /* SPORT 1 divisor for transmit/receive SCLK1 and FS1 */
#define pSPMCTL0     ((volatile unsigned int *)0xc04)     /* SPORT 0 Multichannel Control Register */
#define pSPMCTL1     ((volatile unsigned int *)0xc17)     /* SPORT 1 Multichannel Control Register */

/* MT and MR registers are included here for code compatibility */
#define pMT0CS0      ((volatile unsigned int *)0xc05)     /* SPORT 0 multichannel tx select, channels 31 - 0 */
#define pMT0CS1      ((volatile unsigned int *)0xc06)     /* SPORT 0 multichannel tx select, channels 63 - 32 */
#define pMT0CS2      ((volatile unsigned int *)0xc07)     /* SPORT 0 multichannel tx select, channels 95 - 64 */
#define pMT0CS3      ((volatile unsigned int *)0xc08)     /* SPORT 0 multichannel tx select, channels 127 - 96 */
#define pMR1CS0      ((volatile unsigned int *)0xc09)     /* SPORT 1 multichannel rx select, channels 31 - 0 */
#define pMR1CS1      ((volatile unsigned int *)0xc0A)     /* SPORT 1 multichannel rx select, channels 63 - 32 */
#define pMR1CS2      ((volatile unsigned int *)0xc0B)     /* SPORT 1 multichannel rx select, channels 95 - 64 */
#define pMR1CS3      ((volatile unsigned int *)0xc0C)     /* SPORT 1 multichannel rx select, channels 127 - 96 */
#define pMT0CCS0     ((volatile unsigned int *)0xc0D)     /* SPORT 0 multichannel tx compand select, channels 31 - 0 */
#define pMT0CCS1     ((volatile unsigned int *)0xc0E)     /* SPORT 0 multichannel tx compand select, channels 63 - 32 */
#define pMT0CCS2     ((volatile unsigned int *)0xc0F)     /* SPORT 0 multichannel tx compand select, channels 95 - 64 */
#define pMT0CCS3     ((volatile unsigned int *)0xc10)     /* SPORT 0 multichannel tx compand select, channels 127 - 96 */
#define pMR1CCS0     ((volatile unsigned int *)0xc11)     /* SPORT 1 multichannel rx compand select, channels 31 - 0 */
#define pMR1CCS1     ((volatile unsigned int *)0xc12)     /* SPORT 1 multichannel rx compand select, channels 63 - 32 */
#define pMR1CCS2     ((volatile unsigned int *)0xc13)     /* SPORT 1 multichannel rx compand select, channels 95 - 64 */
#define pMR1CCS3     ((volatile unsigned int *)0xc14)     /* SPORT 1 multichannel rx compand select, channels 127 - 96 */

#define pSPCNT0      ((volatile unsigned int *)0xc15)     /* SPORT Count register - status information for internal clock and fs */
#define pSPCNT1      ((volatile unsigned int *)0xc16)     /* SPORT Count register - status information for internal clock and fs */

#define pSPERRCTL0   ((volatile unsigned int *)0xc18)     /* SPORT 0 Error Interrupt Control Register */
#define pSPERRCTL1   ((volatile unsigned int *)0xc19)     /* SPORT 1 Error Interrupt Control Register */


#define pSPCTLN0	((volatile unsigned int *)0xc1A)	/* Control Register 2 for SP0 */
#define pSPCTLN1	((volatile unsigned int *)0xc1B)	/* Control Register 2 for SP1 */


/* SPORT TDM registers - Naming changed since the direction is programmable now */
#define pSP0CS0       ((volatile unsigned int *)0xc05)    /* SPORT 0 multichannel select, channels 31 - 0 */
#define pSP0CS1       ((volatile unsigned int *)0xc06)    /* SPORT 0 multichannel select, channels 63 - 32 */
#define pSP0CS2       ((volatile unsigned int *)0xc07)    /* SPORT 0 multichannel select, channels 95 - 64 */
#define pSP0CS3       ((volatile unsigned int *)0xc08)    /* SPORT 0 multichannel select, channels 127 - 96 */
#define pSP1CS0       ((volatile unsigned int *)0xc09)    /* SPORT 1 multichannel select, channels 31 - 0 */
#define pSP1CS1       ((volatile unsigned int *)0xc0A)    /* SPORT 1 multichannel select, channels 63 - 32 */
#define pSP1CS2       ((volatile unsigned int *)0xc0B)    /* SPORT 1 multichannel select, channels 95 - 64 */
#define pSP1CS3       ((volatile unsigned int *)0xc0C)    /* SPORT 1 multichannel select, channels 127 - 96 */
#define pSP0CCS0      ((volatile unsigned int *)0xc0D)    /* SPORT 0 multichannel compand select, channels 31 - 0 */
#define pSP0CCS1      ((volatile unsigned int *)0xc0E)    /* SPORT 0 multichannel compand select, channels 63 - 32 */
#define pSP0CCS2      ((volatile unsigned int *)0xc0F)    /* SPORT 0 multichannel compand select, channels 95 - 64 */
#define pSP0CCS3      ((volatile unsigned int *)0xc10)    /* SPORT 0 multichannel compand select, channels 127 - 96 */
#define pSP1CCS0      ((volatile unsigned int *)0xc11)    /* SPORT 1 multichannel compand select, channels 31 - 0 */
#define pSP1CCS1      ((volatile unsigned int *)0xc12)    /* SPORT 1 multichannel compand select, channels 63 - 32 */
#define pSP1CCS2      ((volatile unsigned int *)0xc13)    /* SPORT 1 multichannel compand select, channels 95 - 64 */
#define pSP1CCS3      ((volatile unsigned int *)0xc14)    /* SPORT 1 multichannel compand select, channels 127 - 96 */

#define pIISP0A      ((volatile unsigned int *)0xc40) /* Internal memory DMA address */
#define pIMSP0A      ((volatile unsigned int *)0xc41) /* Internal memory DMA access modifier */
#define pCSP0A       ((volatile unsigned int *)0xc42) /* Contains number of DMA transfers remaining */
#define pCPSP0A      ((volatile unsigned int *)0xc43) /* Points to next DMA parameters */
#define pIISP0B      ((volatile unsigned int *)0xc44) /* Internal memory DMA address */
#define pIMSP0B      ((volatile unsigned int *)0xc45) /* Internal memory DMA access modifier */
#define pCSP0B       ((volatile unsigned int *)0xc46) /* Contains number of DMA transfers remaining */
#define pCPSP0B      ((volatile unsigned int *)0xc47) /* Points to next DMA parameters */
#define pIISP1A      ((volatile unsigned int *)0xc48) /* Internal memory DMA address */
#define pIMSP1A      ((volatile unsigned int *)0xc49) /* Internal memory DMA access modifier */
#define pCSP1A       ((volatile unsigned int *)0xc4A) /* Contains number of DMA transfers remaining */
#define pCPSP1A      ((volatile unsigned int *)0xc4B) /* Points to next DMA parameters */
#define pIISP1B      ((volatile unsigned int *)0xc4C) /* Internal memory DMA address */
#define pIMSP1B      ((volatile unsigned int *)0xc4D) /* Internal memory DMA access modifier */
#define pCSP1B       ((volatile unsigned int *)0xc4E) /* Contains number of DMA transfers remaining */
#define pCPSP1B      ((volatile unsigned int *)0xc4F) /* Points to next DMA parameters */
#define pTXSP0A      ((volatile unsigned int *)0xc60) /* SPORT 0A transmit data register */
#define pRXSP0A      ((volatile unsigned int *)0xc61) /* SPORT 0A receive data register */
#define pTXSP0B      ((volatile unsigned int *)0xc62) /* SPORT 0B transmit data register */
#define pRXSP0B      ((volatile unsigned int *)0xc63) /* SPORT 0B receive data register */
#define pTXSP1A      ((volatile unsigned int *)0xc64) /* SPORT 1A transmit data register */
#define pRXSP1A      ((volatile unsigned int *)0xc65) /* SPORT 1A receive data register */
#define pTXSP1B      ((volatile unsigned int *)0xc66) /* SPORT 1B transmit data register */
#define pRXSP1B      ((volatile unsigned int *)0xc67) /* SPORT 1B receive data register */

/* Serial Port registers (SP23) */

#define pSPCTL2      ((volatile unsigned int *)0x400) /* SPORT 2 control register */
#define pSPCTL3      ((volatile unsigned int *)0x401) /* SPORT 3 control register */
#define pDIV2        ((volatile unsigned int *)0x402) /* SPORT 2 divisor for transmit/receive SCLK2 and FS2 */
#define pDIV3        ((volatile unsigned int *)0x403) /* SPORT 3 divisor for transmit/receive SCLK3 and FS3 */
#define pSPMCTL2     ((volatile unsigned int *)0x404) /* SPORT 2 Multichannel Control Register */
#define pSPMCTL3     ((volatile unsigned int *)0x417) /* SPORT 3 Multichannel Control Register */

/* MT and MR registers are included here for code compatibility */
#define pMT2CS0      ((volatile unsigned int *)0x405) /* SPORT 2 multichannel tx select, channels 31 - 0 */
#define pMT2CS1      ((volatile unsigned int *)0x406) /* SPORT 2 multichannel tx select, channels 63 - 32 */
#define pMT2CS2      ((volatile unsigned int *)0x407) /* SPORT 2 multichannel tx select, channels 95 - 64 */
#define pMT2CS3      ((volatile unsigned int *)0x408) /* SPORT 2 multichannel tx select, channels 127 - 96 */
#define pMR3CS0      ((volatile unsigned int *)0x409) /* SPORT 3 multichannel rx select, channels 31 - 0 */
#define pMR3CS1      ((volatile unsigned int *)0x40A) /* SPORT 3 multichannel rx select, channels 63 - 32 */
#define pMR3CS2      ((volatile unsigned int *)0x40B) /* SPORT 3 multichannel rx select, channels 95 - 64 */
#define pMR3CS3      ((volatile unsigned int *)0x40C) /* SPORT 3 multichannel rx select, channels 127 - 96 */
#define pMT2CCS0     ((volatile unsigned int *)0x40D) /* SPORT 2 multichannel tx compand select, channels 31 - 0 */
#define pMT2CCS1     ((volatile unsigned int *)0x40E) /* SPORT 2 multichannel tx compand select, channels 63 - 32 */
#define pMT2CCS2     ((volatile unsigned int *)0x40F) /* SPORT 2 multichannel tx compand select, channels 95 - 64 */
#define pMT2CCS3     ((volatile unsigned int *)0x410) /* SPORT 2 multichannel tx compand select, channels 127 - 96 */
#define pMR3CCS0     ((volatile unsigned int *)0x411) /* SPORT 3 multichannel rx compand select, channels 31 - 0 */
#define pMR3CCS1     ((volatile unsigned int *)0x412) /* SPORT 3 multichannel rx compand select, channels 63 - 32 */
#define pMR3CCS2     ((volatile unsigned int *)0x413) /* SPORT 3 multichannel rx compand select, channels 95 - 64 */
#define pMR3CCS3     ((volatile unsigned int *)0x414) /* SPORT 3 multichannel rx compand select, channels 127 - 96 */

#define pSPCNT2      ((volatile unsigned int *)0x415) /* SPORT Count register - status information for internal clock and fs */
#define pSPCNT3      ((volatile unsigned int *)0x416) /* SPORT Count register - status information for internal clock and fs */

#define pSPERRCTL2   ((volatile unsigned int *)0x418) /* SPORT 2 Error Interrupt Control Register */
#define pSPERRCTL3   ((volatile unsigned int *)0x419) /* SPORT 3 Error Interrupt Control Register */

#define pSPCTLN2	 ((volatile unsigned int *)0x41A)	/* Control Register 2 for SP2 */
#define pSPCTLN3	 ((volatile unsigned int *)0x41B)	/* Control Register 2 for SP3 */


/* SPORT TDM registers - Naming changed since the direction is programmable now */
#define pSP2CS0       ((volatile unsigned int *)0x405)    /* SPORT 2 multichannel select, channels 31 - 0 */
#define pSP2CS1       ((volatile unsigned int *)0x406)    /* SPORT 2 multichannel select, channels 63 - 32 */
#define pSP2CS2       ((volatile unsigned int *)0x407)    /* SPORT 2 multichannel select, channels 95 - 64 */
#define pSP2CS3       ((volatile unsigned int *)0x408)    /* SPORT 2 multichannel select, channels 127 - 96 */
#define pSP3CS0       ((volatile unsigned int *)0x409)    /* SPORT 3 multichannel select, channels 31 - 0 */
#define pSP3CS1       ((volatile unsigned int *)0x40A)    /* SPORT 3 multichannel select, channels 63 - 32 */
#define pSP3CS2       ((volatile unsigned int *)0x40B)    /* SPORT 3 multichannel select, channels 95 - 64 */
#define pSP3CS3       ((volatile unsigned int *)0x40C)    /* SPORT 3 multichannel select, channels 127 - 96 */
#define pSP2CCS0      ((volatile unsigned int *)0x40D)    /* SPORT 2 multichannel compand select, channels 31 - 0 */
#define pSP2CCS1      ((volatile unsigned int *)0x40E)    /* SPORT 2 multichannel compand select, channels 63 - 32 */
#define pSP2CCS2      ((volatile unsigned int *)0x40F)    /* SPORT 2 multichannel compand select, channels 95 - 64 */
#define pSP2CCS3      ((volatile unsigned int *)0x410)    /* SPORT 2 multichannel compand select, channels 127 - 96 */
#define pSP3CCS0      ((volatile unsigned int *)0x411)    /* SPORT 3 multichannel compand select, channels 31 - 0 */
#define pSP3CCS1      ((volatile unsigned int *)0x412)    /* SPORT 3 multichannel compand select, channels 63 - 32 */
#define pSP3CCS2      ((volatile unsigned int *)0x413)    /* SPORT 3 multichannel compand select, channels 95 - 64 */
#define pSP3CCS3      ((volatile unsigned int *)0x414)    /* SPORT 3 multichannel compand select, channels 127 - 96 */

#define pIISP2A      ((volatile unsigned int *)0x440) /* Internal memory DMA address */
#define pIMSP2A      ((volatile unsigned int *)0x441) /* Internal memory DMA access modifier */
#define pCSP2A       ((volatile unsigned int *)0x442) /* Contains number of DMA transfers remaining */
#define pCPSP2A      ((volatile unsigned int *)0x443) /* Points to next DMA parameters */
#define pIISP2B      ((volatile unsigned int *)0x444) /* Internal memory DMA address */
#define pIMSP2B      ((volatile unsigned int *)0x445) /* Internal memory DMA access modifier */
#define pCSP2B       ((volatile unsigned int *)0x446) /* Contains number of DMA transfers remaining */
#define pCPSP2B      ((volatile unsigned int *)0x447) /* Points to next DMA parameters */
#define pIISP3A      ((volatile unsigned int *)0x448) /* Internal memory DMA address */
#define pIMSP3A      ((volatile unsigned int *)0x449) /* Internal memory DMA access modifier */
#define pCSP3A       ((volatile unsigned int *)0x44A) /* Contains number of DMA transfers remaining */
#define pCPSP3A      ((volatile unsigned int *)0x44B) /* Points to next DMA parameters */
#define pIISP3B      ((volatile unsigned int *)0x44C) /* Internal memory DMA address */
#define pIMSP3B      ((volatile unsigned int *)0x44D) /* Internal memory DMA access modifier */
#define pCSP3B       ((volatile unsigned int *)0x44E) /* Contains number of DMA transfers remaining */
#define pCPSP3B      ((volatile unsigned int *)0x44F) /* Points to next DMA parameters */
#define pTXSP2A      ((volatile unsigned int *)0x460) /* SPORT 2A transmit data register */
#define pRXSP2A      ((volatile unsigned int *)0x461) /* SPORT 2A receive data register */
#define pTXSP2B      ((volatile unsigned int *)0x462) /* SPORT 2B transmit data register */
#define pRXSP2B      ((volatile unsigned int *)0x463) /* SPORT 2B receive data register */
#define pTXSP3A      ((volatile unsigned int *)0x464) /* SPORT 3A transmit data register */
#define pRXSP3A      ((volatile unsigned int *)0x465) /* SPORT 3A receive data register */
#define pTXSP3B      ((volatile unsigned int *)0x466) /* SPORT 3B transmit data register */
#define pRXSP3B      ((volatile unsigned int *)0x467) /* SPORT 3B receive data register */

/* Serial Port registers (SP45) */


#define pSPCTL4      ((volatile unsigned int *)0x800)     /* SPORT 4 control register */
#define pSPCTL5      ((volatile unsigned int *)0x801)     /* SPORT 5 control register */
#define pDIV4        ((volatile unsigned int *)0x802)     /* SPORT 4 divisor for transmit/receive SCLK4 and FS4 */
#define pDIV5        ((volatile unsigned int *)0x803)     /* SPORT 5 divisor for transmit/receive SCLK5 and FS5 */
#define pSPMCTL4     ((volatile unsigned int *)0x804)     /* SPORT 4 Multichannel Control Register */

/* MT and MR registers are included here for code compatibility */
#define pMT4CS0      ((volatile unsigned int *)0x805)     /* SPORT 4 multichannel tx select, channels 31 - 0 */
#define pMT4CS1      ((volatile unsigned int *)0x806)     /* SPORT 4 multichannel tx select, channels 63 - 32 */
#define pMT4CS2      ((volatile unsigned int *)0x807)     /* SPORT 4 multichannel tx select, channels 95 - 64 */
#define pMT4CS3      ((volatile unsigned int *)0x808)     /* SPORT 4 multichannel tx select, channels 127 - 96 */
#define pMR5CS0      ((volatile unsigned int *)0x809)     /* SPORT 5 multichannel rx select, channels 31 - 0 */
#define pMR5CS1      ((volatile unsigned int *)0x80A)     /* SPORT 5 multichannel rx select, channels 63 - 32 */
#define pMR5CS2      ((volatile unsigned int *)0x80B)     /* SPORT 5 multichannel rx select, channels 95 - 64 */
#define pMR5CS3      ((volatile unsigned int *)0x80C)     /* SPORT 5 multichannel rx select, channels 127 - 96 */
#define pMT4CCS0     ((volatile unsigned int *)0x80D)     /* SPORT 4 multichannel tx compand select, channels 31 - 0 */
#define pMT4CCS1     ((volatile unsigned int *)0x80E)     /* SPORT 4 multichannel tx compand select, channels 63 - 32 */
#define pMT4CCS2     ((volatile unsigned int *)0x80F)     /* SPORT 4 multichannel tx compand select, channels 95 - 64 */
#define pMT4CCS3     ((volatile unsigned int *)0x810)     /* SPORT 4 multichannel tx compand select, channels 127 - 96 */
#define pMR5CCS0     ((volatile unsigned int *)0x811)     /* SPORT 5 multichannel rx compand select, channels 31 - 0 */
#define pMR5CCS1     ((volatile unsigned int *)0x812)     /* SPORT 5 multichannel rx compand select, channels 63 - 32 */
#define pMR5CCS2     ((volatile unsigned int *)0x813)     /* SPORT 5 multichannel rx compand select, channels 95 - 64 */
#define pMR5CCS3     ((volatile unsigned int *)0x814)     /* SPORT 5 multichannel rx compand select, channels 127 - 96 */

#define pSPCNT4      ((volatile unsigned int *)0x815)     /* SPORT Count register - status information for internal clock and fs */
#define pSPCNT5      ((volatile unsigned int *)0x816)     /* SPORT Count register - status information for internal clock and fs */
#define pSPMCTL5     ((volatile unsigned int *)0x817)     /* SPORT 5 Multichannel Control Register */

#define pSPERRCTL4   ((volatile unsigned int *)0x818)     /* SPORT 4 Error Interrupt Control Register */
#define pSPERRCTL5   ((volatile unsigned int *)0x819)     /* SPORT 5 Error Interrupt Control Register */

#define pSPCTLN4	 ((volatile unsigned int *)0x81A)	/* Control Register 2 for SP4 */
#define pSPCTLN5	 ((volatile unsigned int *)0x81B)	/* Control Register 2 for SP5 */

/* SPORT TDM registers - Naming changed since the direction is programmable now */
#define pSP4CS0       ((volatile unsigned int *)0x805)    /* SPORT 4 multichannel select, channels 31 - 0 */
#define pSP4CS1       ((volatile unsigned int *)0x806)    /* SPORT 4 multichannel select, channels 63 - 32 */
#define pSP4CS2       ((volatile unsigned int *)0x807)    /* SPORT 4 multichannel select, channels 95 - 64 */
#define pSP4CS3       ((volatile unsigned int *)0x808)    /* SPORT 4 multichannel select, channels 127 - 96 */
#define pSP5CS0       ((volatile unsigned int *)0x809)    /* SPORT 5 multichannel select, channels 31 - 0 */
#define pSP5CS1       ((volatile unsigned int *)0x80A)    /* SPORT 5 multichannel select, channels 63 - 32 */
#define pSP5CS2       ((volatile unsigned int *)0x80B)    /* SPORT 5 multichannel select, channels 95 - 64 */
#define pSP5CS3       ((volatile unsigned int *)0x80C)    /* SPORT 5 multichannel select, channels 127 - 96 */
#define pSP4CCS0      ((volatile unsigned int *)0x80D)    /* SPORT 4 multichannel compand select, channels 31 - 0 */
#define pSP4CCS1      ((volatile unsigned int *)0x80E)    /* SPORT 4 multichannel compand select, channels 63 - 32 */
#define pSP4CCS2      ((volatile unsigned int *)0x80F)    /* SPORT 4 multichannel compand select, channels 95 - 64 */
#define pSP4CCS3      ((volatile unsigned int *)0x810)    /* SPORT 4 multichannel compand select, channels 127 - 96 */
#define pSP5CCS0      ((volatile unsigned int *)0x811)    /* SPORT 5 multichannel compand select, channels 31 - 0 */
#define pSP5CCS1      ((volatile unsigned int *)0x812)    /* SPORT 5 multichannel compand select, channels 63 - 32 */
#define pSP5CCS2      ((volatile unsigned int *)0x813)    /* SPORT 5 multichannel compand select, channels 95 - 64 */
#define pSP5CCS3      ((volatile unsigned int *)0x814)    /* SPORT 5 multichannel compand select, channels 127 - 96 */

#define pIISP4A      ((volatile unsigned int *)0x840) /* Internal memory DMA address */
#define pIMSP4A      ((volatile unsigned int *)0x841) /* Internal memory DMA access modifier */
#define pCSP4A       ((volatile unsigned int *)0x842) /* Contains number of DMA transfers remaining */
#define pCPSP4A      ((volatile unsigned int *)0x843) /* Points to next DMA parameters */
#define pIISP4B      ((volatile unsigned int *)0x844) /* Internal memory DMA address */
#define pIMSP4B      ((volatile unsigned int *)0x845) /* Internal memory DMA access modifier */
#define pCSP4B       ((volatile unsigned int *)0x846) /* Contains number of DMA transfers remaining */
#define pCPSP4B      ((volatile unsigned int *)0x847) /* Points to next DMA parameters */
#define pIISP5A      ((volatile unsigned int *)0x848) /* Internal memory DMA address */
#define pIMSP5A      ((volatile unsigned int *)0x849) /* Internal memory DMA access modifier */
#define pCSP5A       ((volatile unsigned int *)0x84A) /* Contains number of DMA transfers remaining */
#define pCPSP5A      ((volatile unsigned int *)0x84B) /* Points to next DMA parameters */
#define pIISP5B      ((volatile unsigned int *)0x84C) /* Internal memory DMA address */
#define pIMSP5B      ((volatile unsigned int *)0x84D) /* Internal memory DMA access modifier */
#define pCSP5B       ((volatile unsigned int *)0x84E) /* Contains number of DMA transfers remaining */
#define pCPSP5B      ((volatile unsigned int *)0x84F) /* Points to next DMA parameters */
#define pTXSP4A      ((volatile unsigned int *)0x860) /* SPORT 4A transmit data register */
#define pRXSP4A      ((volatile unsigned int *)0x861) /* SPORT 4A receive data register */
#define pTXSP4B      ((volatile unsigned int *)0x862) /* SPORT 4B transmit data register */
#define pRXSP4B      ((volatile unsigned int *)0x863) /* SPORT 4B receive data register */
#define pTXSP5A      ((volatile unsigned int *)0x864) /* SPORT 5A transmit data register */
#define pRXSP5A      ((volatile unsigned int *)0x865) /* SPORT 5A receive data register */
#define pTXSP5B      ((volatile unsigned int *)0x866) /* SPORT 5B transmit data register */
#define pRXSP5B      ((volatile unsigned int *)0x867) /* SPORT 5B receive data register */

/* SP67 */

#define pSPCTL6      ((volatile unsigned int *)0x4800)     /* SPORT 6 control register */
#define pSPCTL7      ((volatile unsigned int *)0x4801)     /* SPORT 7 control register */
#define pDIV6        ((volatile unsigned int *)0x4802)     /* SPORT 6 divisor for transmit/receive SCLK4 and FS4 */
#define pDIV7        ((volatile unsigned int *)0x4803)     /* SPORT 7 divisor for transmit/receive SCLK5 and FS5 */
#define pSPMCTL6     ((volatile unsigned int *)0x4804)     /* SPORT 6 Multichannel Control Register */

/* MT and MR registers are included here for code compatibility */
#define pMT6CS0      ((volatile unsigned int *)0x4805)     /* SPORT 6 multichannel tx select, channels 31 - 0 */
#define pMT6CS1      ((volatile unsigned int *)0x4806)     /* SPORT 6 multichannel tx select, channels 63 - 32 */
#define pMT6CS2      ((volatile unsigned int *)0x4807)     /* SPORT 6 multichannel tx select, channels 95 - 64 */
#define pMT6CS3      ((volatile unsigned int *)0x4808)     /* SPORT 6 multichannel tx select, channels 127 - 96 */
#define pMR7CS0      ((volatile unsigned int *)0x4809)     /* SPORT 7 multichannel rx select, channels 31 - 0 */
#define pMR7CS1      ((volatile unsigned int *)0x480A)     /* SPORT 7 multichannel rx select, channels 63 - 32 */
#define pMR7CS2      ((volatile unsigned int *)0x480B)     /* SPORT 7 multichannel rx select, channels 95 - 64 */
#define pMR7CS3      ((volatile unsigned int *)0x480C)     /* SPORT 7 multichannel rx select, channels 127 - 96 */
#define pMT6CCS0     ((volatile unsigned int *)0x480D)     /* SPORT 6 multichannel tx compand select, channels 31 - 0 */
#define pMT6CCS1     ((volatile unsigned int *)0x480E)     /* SPORT 6 multichannel tx compand select, channels 63 - 32 */
#define pMT6CCS2     ((volatile unsigned int *)0x480F)     /* SPORT 6 multichannel tx compand select, channels 95 - 64 */
#define pMT6CCS3     ((volatile unsigned int *)0x4810)     /* SPORT 6 multichannel tx compand select, channels 127 - 96 */
#define pMR7CCS0     ((volatile unsigned int *)0x4811)     /* SPORT 7 multichannel rx compand select, channels 31 - 0 */
#define pMR7CCS1     ((volatile unsigned int *)0x4812)     /* SPORT 7 multichannel rx compand select, channels 63 - 32 */
#define pMR7CCS2     ((volatile unsigned int *)0x4813)     /* SPORT 7 multichannel rx compand select, channels 95 - 64 */
#define pMR7CCS3     ((volatile unsigned int *)0x4814)     /* SPORT 7 multichannel rx compand select, channels 127 - 96 */

#define pSPCNT6      ((volatile unsigned int *)0x4815)     /* SPORT Count register - status information for internal clock and fs */
#define pSPCNT7      ((volatile unsigned int *)0x4816)     /* SPORT Count register - status information for internal clock and fs */
#define pSPMCTL7     ((volatile unsigned int *)0x4817)     /* SPORT 7 Multichannel Control Register */
#define pSPERRCTL6   ((volatile unsigned int *)0x4818)     /* SPORT 6 Error Interrupt Control Register */
#define pSPERRCTL7   ((volatile unsigned int *)0x4819)     /* SPORT 7 Error Interrupt Control Register */

#define pSPCTLN6	 ((volatile unsigned int *)0x481A)	/* Control Register 2 for SP6 */
#define pSPCTLN7	 ((volatile unsigned int *)0x481B)	/* Control Register 2 for SP7 */


/* SPORT TDM registers - Naming changed since the direction is programmable now */
#define pSP6CS0       ((volatile unsigned int *)0x4805)    /* SPORT 6 multichannel select, channels 31 - 0 */
#define pSP6CS1       ((volatile unsigned int *)0x4806)    /* SPORT 6 multichannel select, channels 63 - 32 */
#define pSP6CS2       ((volatile unsigned int *)0x4807)    /* SPORT 6 multichannel select, channels 95 - 64 */
#define pSP6CS3       ((volatile unsigned int *)0x4808)    /* SPORT 6 multichannel select, channels 127 - 96 */
#define pSP7CS0       ((volatile unsigned int *)0x4809)    /* SPORT 7 multichannel select, channels 31 - 0 */
#define pSP7CS1       ((volatile unsigned int *)0x480A)    /* SPORT 7 multichannel select, channels 63 - 32 */
#define pSP7CS2       ((volatile unsigned int *)0x480B)    /* SPORT 7 multichannel select, channels 95 - 64 */
#define pSP7CS3       ((volatile unsigned int *)0x480C)    /* SPORT 7 multichannel select, channels 127 - 96 */
#define pSP6CCS0      ((volatile unsigned int *)0x480D)    /* SPORT 6 multichannel compand select, channels 31 - 0 */
#define pSP6CCS1      ((volatile unsigned int *)0x480E)    /* SPORT 6 multichannel compand select, channels 63 - 32 */
#define pSP6CCS2      ((volatile unsigned int *)0x480F)    /* SPORT 6 multichannel compand select, channels 95 - 64 */
#define pSP6CCS3      ((volatile unsigned int *)0x4810)    /* SPORT 6 multichannel compand select, channels 127 - 96 */
#define pSP7CCS0      ((volatile unsigned int *)0x4811)    /* SPORT 7 multichannel compand select, channels 31 - 0 */
#define pSP7CCS1      ((volatile unsigned int *)0x4812)    /* SPORT 7 multichannel compand select, channels 63 - 32 */
#define pSP7CCS2      ((volatile unsigned int *)0x4813)    /* SPORT 7 multichannel compand select, channels 95 - 64 */
#define pSP7CCS3      ((volatile unsigned int *)0x4814)    /* SPORT 7 multichannel compand select, channels 127 - 96 */

#define pIISP6A      ((volatile unsigned int *)0x4840) /* Internal memory DMA address */
#define pIMSP6A      ((volatile unsigned int *)0x4841) /* Internal memory DMA access modifier */
#define pCSP6A       ((volatile unsigned int *)0x4842) /* Contains number of DMA transfers remaining */
#define pCPSP6A      ((volatile unsigned int *)0x4843) /* Points to next DMA parameters */
#define pIISP6B      ((volatile unsigned int *)0x4844) /* Internal memory DMA address */
#define pIMSP6B      ((volatile unsigned int *)0x4845) /* Internal memory DMA access modifier */
#define pCSP6B       ((volatile unsigned int *)0x4846) /* Contains number of DMA transfers remaining */
#define pCPSP6B      ((volatile unsigned int *)0x4847) /* Points to next DMA parameters */
#define pIISP7A      ((volatile unsigned int *)0x4848) /* Internal memory DMA address */
#define pIMSP7A      ((volatile unsigned int *)0x4849) /* Internal memory DMA access modifier */
#define pCSP7A       ((volatile unsigned int *)0x484A) /* Contains number of DMA transfers remaining */
#define pCPSP7A      ((volatile unsigned int *)0x484B) /* Points to next DMA parameters */
#define pIISP7B      ((volatile unsigned int *)0x484C) /* Internal memory DMA address */
#define pIMSP7B      ((volatile unsigned int *)0x484D) /* Internal memory DMA access modifier */
#define pCSP7B       ((volatile unsigned int *)0x484E) /* Contains number of DMA transfers remaining */
#define pCPSP7B      ((volatile unsigned int *)0x484F) /* Points to next DMA parameters */
#define pTXSP6A      ((volatile unsigned int *)0x4860) /* SPORT 6A transmit data register */
#define pRXSP6A      ((volatile unsigned int *)0x4861) /* SPORT 6A receive data register */
#define pTXSP6B      ((volatile unsigned int *)0x4862) /* SPORT 6B transmit data register */
#define pRXSP6B      ((volatile unsigned int *)0x4863) /* SPORT 6B receive data register */
#define pTXSP7A      ((volatile unsigned int *)0x4864) /* SPORT 7A transmit data register */
#define pRXSP7A      ((volatile unsigned int *)0x4865) /* SPORT 7A receive data register */
#define pTXSP7B      ((volatile unsigned int *)0x4866) /* SPORT 7B transmit data register */
#define pRXSP7B      ((volatile unsigned int *)0x4867) /* SPORT 7B receive data register */

/* SPI Registers */


#define pSPICTL          ((volatile unsigned int *)0x1000)    /* SPI Control Register */
#define pSPIFLG          ((volatile unsigned int *)0x1001)    /* SPI Flag register */
#define pSPISTAT         ((volatile unsigned int *)0x1002)    /* SPI Status register */
#define pTXSPI           ((volatile unsigned int *)0x1003)    /* SPI transmit data register */
#define pRXSPI           ((volatile unsigned int *)0x1004)    /* SPI receive data register */
#define pSPIBAUD         ((volatile unsigned int *)0x1005)    /* SPI baud setup register */
#define pRXSPI_SHADOW    ((volatile unsigned int *)0x1006)    /* SPI receive data shadow register */
#define pIISPI           ((volatile unsigned int *)0x1080)    /* Internal memory DMA address */
#define pIMSPI           ((volatile unsigned int *)0x1081)    /* Internal memory DMA access modifier */
#define pCSPI            ((volatile unsigned int *)0x1082)    /* Contains number of DMA transfers remaining */
#define pCPSPI           ((volatile unsigned int *)0x1083)    /* Points to next DMA parameters */
#define pSPIDMAC         ((volatile unsigned int *)0x1084)    /* SPI DMA control register */
#define pSPINTST         ((volatile unsigned int *)0x1085)  /* Apps based change */

/* SPIB Registers: This SPI port is routed through the DAI */
#define pSPICTLB         ((volatile unsigned int *)0x2800)    /* SPIB Control Register */
#define pSPIFLGB         ((volatile unsigned int *)0x2801)    /* SPIB Flag register */
#define pSPISTATB        ((volatile unsigned int *)0x2802)    /* SPIB Status register */
#define pTXSPIB          ((volatile unsigned int *)0x2803)    /* SPIB transmit data register */
#define pRXSPIB          ((volatile unsigned int *)0x2804)    /* SPIB receive data register */
#define pSPIBAUDB        ((volatile unsigned int *)0x2805)    /* SPIB baud setup register */
#define pRXSPIB_SHADOW   ((volatile unsigned int *)0x2806)    /* SPIB receive data shadow register */
#define pIISPIB          ((volatile unsigned int *)0x2880)    /* Internal memory DMA address */
#define pIMSPIB          ((volatile unsigned int *)0x2881)    /* Internal memory DMA access modifier */
#define pCSPIB           ((volatile unsigned int *)0x2882)    /* Contains number of DMA transfers remaining */
#define pCPSPIB          ((volatile unsigned int *)0x2883)    /* Points to next DMA parameters */
#define pSPIDMACB        ((volatile unsigned int *)0x2884)    /* SPIB DMA control register */

#define pSPINTSTB        ((volatile unsigned int *)0x2885)    /* apps based change */


/* Timer Registers */
#define pTMSTAT      ((volatile unsigned int *)0x1400)    /* GP Timer Status Register */
                                /* TMxSTAT all address the same register (TMSTAT) */
#define pTM0STAT     ((volatile unsigned int *)0x1400)    /* GP Timer 0 Status register */
#define pTM0CTL      ((volatile unsigned int *)0x1401)    /* GP Timer 0 Control register */
#define pTM0CNT      ((volatile unsigned int *)0x1402)    /* GP Timer 0 Count register */
#define pTM0PRD      ((volatile unsigned int *)0x1403)    /* GP Timer 0 Period register */
#define pTM0W        ((volatile unsigned int *)0x1404)    /* GP Timer 0 Width register */
#define pTM1STAT     ((volatile unsigned int *)0x1408)    /* GP Timer 1 Status register */
#define pTM1CTL      ((volatile unsigned int *)0x1409)    /* GP Timer 1 Control register */
#define pTM1CNT      ((volatile unsigned int *)0x140a)    /* GP Timer 1 Count register */
#define pTM1PRD      ((volatile unsigned int *)0x140b)    /* GP Timer 1 Period register */
#define pTM1W        ((volatile unsigned int *)0x140c)    /* GP Timer 1 Width register */

/* POWER MGT Registers */
#define pPMCTL       ((volatile unsigned int *)0x2000)    /* Power management control register */
#define pPMCTL1      ((volatile unsigned int *)0x2001)    /* Extended Power management control register */
#define pROMID       ((volatile unsigned int *)0x20FF)
#define pRUNRSTCTL    ((volatile unsigned int *)0x2100)

/* Peripheral Interrupt priority control register */
#define pPICR0       ((volatile unsigned int *)0x2200)
#define pPICR1       ((volatile unsigned int *)0x2201)
#define pPICR2       ((volatile unsigned int *)0x2202)
#define pPICR3       ((volatile unsigned int *)0x2203)

/* DAI Registers */

/* DMA Parameter Registers */
#define pIDP_DMA_I0  ((volatile unsigned int *)0x2400)    /* IDP DMA Channel 0 Index Register */
#define pIDP_DMA_I1  ((volatile unsigned int *)0x2401)    /* IDP DMA Channel 1 Index Register */
#define pIDP_DMA_I2  ((volatile unsigned int *)0x2402)    /* IDP DMA Channel 2 Index Register */
#define pIDP_DMA_I3  ((volatile unsigned int *)0x2403)    /* IDP DMA Channel 3 Index Register */
#define pIDP_DMA_I4  ((volatile unsigned int *)0x2404)    /* IDP DMA Channel 4 Index Register */
#define pIDP_DMA_I5  ((volatile unsigned int *)0x2405)    /* IDP DMA Channel 5 Index Register */
#define pIDP_DMA_I6  ((volatile unsigned int *)0x2406)    /* IDP DMA Channel 6 Index Register */
#define pIDP_DMA_I7  ((volatile unsigned int *)0x2407)    /* IDP DMA Channel 7 Index Register */
#define pIDP_DMA_I0A ((volatile unsigned int *)0x2408)    /* IDP DMA Channel 0 Index A Register for Ping Pong DMA */
#define pIDP_DMA_I1A ((volatile unsigned int *)0x2409)    /* IDP DMA Channel 1 Index A Register for Ping Pong DMA */
#define pIDP_DMA_I2A ((volatile unsigned int *)0x240a)    /* IDP DMA Channel 2 Index A Register for Ping Pong DMA */
#define pIDP_DMA_I3A ((volatile unsigned int *)0x240b)    /* IDP DMA Channel 3 Index A Register for Ping Pong DMA */
#define pIDP_DMA_I4A ((volatile unsigned int *)0x240c)    /* IDP DMA Channel 4 Index A Register for Ping Pong DMA */
#define pIDP_DMA_I5A ((volatile unsigned int *)0x240d)    /* IDP DMA Channel 5 Index A Register for Ping Pong DMA */
#define pIDP_DMA_I6A ((volatile unsigned int *)0x240e)    /* IDP DMA Channel 6 Index A Register for Ping Pong DMA */
#define pIDP_DMA_I7A ((volatile unsigned int *)0x240f)    /* IDP DMA Channel 7 Index A Register for Ping Pong DMA */
#define pIDP_DMA_I0B ((volatile unsigned int *)0x2418)    /* IDP DMA Channel 0 Index B Register for Ping Pong DMA */
#define pIDP_DMA_I1B ((volatile unsigned int *)0x2419)    /* IDP DMA Channel 1 Index B Register for Ping Pong DMA */
#define pIDP_DMA_I2B ((volatile unsigned int *)0x241a)    /* IDP DMA Channel 2 Index B Register for Ping Pong DMA */
#define pIDP_DMA_I3B ((volatile unsigned int *)0x241b)    /* IDP DMA Channel 3 Index B Register for Ping Pong DMA */
#define pIDP_DMA_I4B ((volatile unsigned int *)0x241c)    /* IDP DMA Channel 4 Index B Register for Ping Pong DMA */
#define pIDP_DMA_I5B ((volatile unsigned int *)0x241d)    /* IDP DMA Channel 5 Index B Register for Ping Pong DMA */
#define pIDP_DMA_I6B ((volatile unsigned int *)0x241e)    /* IDP DMA Channel 6 Index B Register for Ping Pong DMA */
#define pIDP_DMA_I7B ((volatile unsigned int *)0x241f)    /* IDP DMA Channel 7 Index B Register for Ping Pong DMA */
#define pIDP_DMA_M0  ((volatile unsigned int *)0x2410)    /* IDP DMA Channel 0 Modify Register */
#define pIDP_DMA_M1  ((volatile unsigned int *)0x2411)    /* IDP DMA Channel 1 Modify Register */
#define pIDP_DMA_M2  ((volatile unsigned int *)0x2412)    /* IDP DMA Channel 2 Modify Register */
#define pIDP_DMA_M3  ((volatile unsigned int *)0x2413)    /* IDP DMA Channel 3 Modify Register */
#define pIDP_DMA_M4  ((volatile unsigned int *)0x2414)    /* IDP DMA Channel 4 Modify Register */
#define pIDP_DMA_M5  ((volatile unsigned int *)0x2415)    /* IDP DMA Channel 5 Modify Register */
#define pIDP_DMA_M6  ((volatile unsigned int *)0x2416)    /* IDP DMA Channel 6 Modify Register */
#define pIDP_DMA_M7  ((volatile unsigned int *)0x2417)    /* IDP DMA Channel 7 Modify Register */
#define pIDP_DMA_C0  ((volatile unsigned int *)0x2420)    /* IDP DMA Channel 0 Count Register */
#define pIDP_DMA_C1  ((volatile unsigned int *)0x2421)    /* IDP DMA Channel 1 Count Register */
#define pIDP_DMA_C2  ((volatile unsigned int *)0x2422)    /* IDP DMA Channel 2 Count Register */
#define pIDP_DMA_C3  ((volatile unsigned int *)0x2423)    /* IDP DMA Channel 3 Count Register */
#define pIDP_DMA_C4  ((volatile unsigned int *)0x2424)    /* IDP DMA Channel 4 Count Register */
#define pIDP_DMA_C5  ((volatile unsigned int *)0x2425)    /* IDP DMA Channel 5 Count Register */
#define pIDP_DMA_C6  ((volatile unsigned int *)0x2426)    /* IDP DMA Channel 6 Count Register */
#define pIDP_DMA_C7  ((volatile unsigned int *)0x2427)    /* IDP DMA Channel 7 Count Register */
#define pIDP_DMA_PC0 ((volatile unsigned int *)0x2428)    /* IDP DMA Channel 0 Ping Pong Count Register */
#define pIDP_DMA_PC1 ((volatile unsigned int *)0x2429)    /* IDP DMA Channel 1 Ping Pong Count Register */
#define pIDP_DMA_PC2 ((volatile unsigned int *)0x242a)    /* IDP DMA Channel 2 Ping Pong Count Register */
#define pIDP_DMA_PC3 ((volatile unsigned int *)0x242b)    /* IDP DMA Channel 3 Ping Pong Count Register */
#define pIDP_DMA_PC4 ((volatile unsigned int *)0x242c)    /* IDP DMA Channel 4 Ping Pong Count Register */
#define pIDP_DMA_PC5 ((volatile unsigned int *)0x242d)    /* IDP DMA Channel 5 Ping Pong Count Register */
#define pIDP_DMA_PC6 ((volatile unsigned int *)0x242e)    /* IDP DMA Channel 6 Ping Pong Count Register */
#define pIDP_DMA_PC7 ((volatile unsigned int *)0x242f)    /* IDP DMA Channel 7 Ping Pong Count Register */





/* SRU Registers */
#define pSRU_CLK0    ((volatile unsigned int *)0x2430)    /* SRU Clock Control Register 0 */
#define pSRU_CLK1    ((volatile unsigned int *)0x2431)    /* SRU Clock Control Register 1 */
#define pSRU_CLK2    ((volatile unsigned int *)0x2432)    /* SRU Clock Control Register 2 */
#define pSRU_CLK3    ((volatile unsigned int *)0x2433)    /* SRU Clock Control Register 3 */
#define pSRU_CLK4    ((volatile unsigned int *)0x2434)    /* SRU Clock Control Register 4 */
#define pSRU_CLK5    ((volatile unsigned int *)0x2435)    /* SRU Clock Control Register 4 */

#define pSRU_DAT0    ((volatile unsigned int *)0x2440)    /* SRU Data Control Register 0 */
#define pSRU_DAT1    ((volatile unsigned int *)0x2441)    /* SRU Data Control Register 1 */
#define pSRU_DAT2    ((volatile unsigned int *)0x2442)    /* SRU Data Control Register 2 */
#define pSRU_DAT3    ((volatile unsigned int *)0x2443)    /* SRU Data Control Register 3 */
#define pSRU_DAT4    ((volatile unsigned int *)0x2444)    /* SRU Data Control Register 4 */
#define pSRU_DAT5    ((volatile unsigned int *)0x2445)    /* SRU Data Control Register 5 */
#define pSRU_DAT6    ((volatile unsigned int *)0x2446)    /* SRU Data Control Register 5 */

#define pSRU_FS0     ((volatile unsigned int *)0x2450)    /* SRU FS Control Register 0 */
#define pSRU_FS1     ((volatile unsigned int *)0x2451)    /* SRU FS Control Register 1 */
#define pSRU_FS2     ((volatile unsigned int *)0x2452)    /* SRU FS Control Register 2 */
#define pSRU_FS3     ((volatile unsigned int *)0x2453)    /* SRU FS Control Register 3 */
#define pSRU_FS4     ((volatile unsigned int *)0x2454)    /* SRU FS Control Register 3 */

#define pSRU_PIN0    ((volatile unsigned int *)0x2460)    /* SRU Pin Control Register 0 */
#define pSRU_PIN1    ((volatile unsigned int *)0x2461)    /* SRU Pin Control Register 1 */
#define pSRU_PIN2    ((volatile unsigned int *)0x2462)    /* SRU Pin Control Register 2 */
#define pSRU_PIN3    ((volatile unsigned int *)0x2463)    /* SRU Pin Control Register 3 */
#define pSRU_PIN4    ((volatile unsigned int *)0x2464)    /* SRU Pin Control Register 4 */

#define pSRU_EXT_MISCA   ((volatile unsigned int *)0x2470)    /* SRU External Misc. A Control Register */
#define pSRU_EXT_MISCB   ((volatile unsigned int *)0x2471)    /* SRU External Misc. B Control Register */

#define pSRU_PBEN0  ((volatile unsigned int *)0x2478)    /* SRU Pin Enable Register 0 */
#define pSRU_PBEN1  ((volatile unsigned int *)0x2479)    /* SRU Pin Enable Register 1 */
#define pSRU_PBEN2  ((volatile unsigned int *)0x247A)    /* SRU Pin Enable Register 2 */
#define pSRU_PBEN3  ((volatile unsigned int *)0x247B)    /* SRU Pin Enable Register 3 */

#define pDAI_IRPTL_FE    ((volatile unsigned int *)0x2480)   /* DAI Falling Edge Interrupt Latch Register */
#define pDAI_IMASK_FE    ((volatile unsigned int *)0x2480)   /* DAI Falling Edge Interrupt Latch Register */
#define pDAI_IRPTL_RE    ((volatile unsigned int *)0x2481)   /* DAI Rising Edge Interrupt Latch Register */
#define pDAI_IMASK_RE    ((volatile unsigned int *)0x2481)   /* DAI Rising Edge Interrupt Latch Register */
#define pDAI_IRPTL_PRI   ((volatile unsigned int *)0x2484)   /* DAI Interrupt Priority Register */
#define pDAI_IRPTL_H     ((volatile unsigned int *)0x2488)   /* DAI High Priority Interrupt Latch Register */
#define pDAI_IRPTL_L     ((volatile unsigned int *)0x2489)   /* DAI Low Priority Interrupt Latch Register */
#define pDAI_IRPTL_HS    ((volatile unsigned int *)0x248C)   /* Shadow DAI High Priority Interrupt Latch Register */
#define pDAI_IRPTL_LS    ((volatile unsigned int *)0x248D)   /* Shadow DAI Low Priority Interrupt Latch Register */


/* Sample Rate Converter registers */
#define pSRCCTL0     ((volatile unsigned int *)0x2490)    /* SRC0 Control register */
#define pSRCCTL1     ((volatile unsigned int *)0x2491)    /* SRC1 Control register */
#define pSRCMUTE     ((volatile unsigned int *)0x2492)    /* SRC Mute register */
#define pSRCRAT0     ((volatile unsigned int *)0x2498)    /* SRC0 Output to Input Ratio */
#define pSRCRAT1     ((volatile unsigned int *)0x2499)    /* SRC1 Output to Input Ratio */


#define pIDP_CTL     ((volatile unsigned int *)0x24B0)    /* IDP Control Register 0 */
#define pIDP_CTL0    ((volatile unsigned int *)0x24B0)    /* IDP Control Register 0 */
#define pIDP_PP_CTL  ((volatile unsigned int *)0x24B1)    /* IDP Parallel Port Control Register */
#define pIDP_CTL1    ((volatile unsigned int *)0x24B2)    /* IDP Control Register 1 */
#define pIDP_CTL2    ((volatile unsigned int *)0x24B3)    /* IDP Control Register 2 (contains framesync edge synchronization Control bits) */

#define pDAI_STAT     ((volatile unsigned int *)0x24B8)   /* DAI Status Register */
#define pDAI_STAT0    ((volatile unsigned int *)0x24B8)   /* DAI Status Register */
#define pDAI_PIN_STAT ((volatile unsigned int *)0x24B9)   /* DAI Pin Buffer Status Register */
#define pDAI_STAT1    ((volatile unsigned int *)0x24BA)   /* DAI Status Register */



#define pIDP_FIFO    ((volatile unsigned int *)0x24D0)    /* IDP FIFO packing mode register */


/* DPI */

#define pSRU2_INPUT0             ((volatile unsigned int *)0x1C00)
#define pSRU2_INPUT1             ((volatile unsigned int *)0x1C01)
#define pSRU2_INPUT2             ((volatile unsigned int *)0x1C02)
#define pSRU2_INPUT3             ((volatile unsigned int *)0x1C03)
#define pSRU2_INPUT4             ((volatile unsigned int *)0x1C04)
#define pSRU2_INPUT5             ((volatile unsigned int *)0x1C05)
#define pSRU2_PIN0               ((volatile unsigned int *)0x1C10)
#define pSRU2_PIN1               ((volatile unsigned int *)0x1C11)
#define pSRU2_PIN2               ((volatile unsigned int *)0x1C12)
#define pSRU2_PBEN0              ((volatile unsigned int *)0x1C20)
#define pSRU2_PBEN1              ((volatile unsigned int *)0x1C21)
#define pSRU2_PBEN2              ((volatile unsigned int *)0x1C22)

#define pDPI_PIN_STAT            ((volatile unsigned int *)0x1C31)
#define pDPI_IRPTL               ((volatile unsigned int *)0x1C32)
#define pDPI_IRPTL_SH            ((volatile unsigned int *)0x1C33)
#define pDPI_IRPTL_FE            ((volatile unsigned int *)0x1C34)
#define pDPI_IMASK_FE            ((volatile unsigned int *)0x1C34)
#define pDPI_IRPTL_RE            ((volatile unsigned int *)0x1C35)
#define pDPI_IMASK_RE            ((volatile unsigned int *)0x1C35)


/* DTCP module */

#if __ADI_HAS_DTCP__

#define pDTCPCTL    	((volatile unsigned int *)0x2C00)  
#define pDTCP_M0     ((volatile unsigned int *)0x2C0E)
#define pDTCP_M1     ((volatile unsigned int *)0x2C0F)
#define pDTCP_I0     ((volatile unsigned int *)0x2C10)
#define pDTCP_I1     ((volatile unsigned int *)0x2C11)
#define pDTCP_C0     ((volatile unsigned int *)0x2C16)
#define pDTCP_C1     ((volatile unsigned int *)0x2C17)

#define pM6DR        ((volatile unsigned int *)0x2C20)  
#define pM6KHR       ((volatile unsigned int *)0x2C21)
#define pM6KLR       ((volatile unsigned int *)0x2C22)
#define pM6CR        ((volatile unsigned int *)0x2C23)
#define pM6ESR       ((volatile unsigned int *)0x2C24)
#define pM6SR        ((volatile unsigned int *)0x2C25)

#endif  /* __ADI_HAS_DTCP__ */



/* SPDIF Transmit registers */
#define pDITCTL      ((volatile unsigned int *)0x24A0)    /* Digital Interface Transmit Control Register */

/* Channel Status buffer */
#define pDITCHANA0   ((volatile unsigned int *)0x24A1)
#define pDITCHANA1   ((volatile unsigned int *)0x24D4)
#define pDITCHANA2   ((volatile unsigned int *)0x24D5)
#define pDITCHANA3   ((volatile unsigned int *)0x24D6)
#define pDITCHANA4   ((volatile unsigned int *)0x24D7)
#define pDITCHANA5   ((volatile unsigned int *)0x24D8)
#define pDITCHANB0   ((volatile unsigned int *)0x24A2)
#define pDITCHANB1   ((volatile unsigned int *)0x24DA)
#define pDITCHANB2   ((volatile unsigned int *)0x24DB)
#define pDITCHANB3   ((volatile unsigned int *)0x24DC)
#define pDITCHANB4   ((volatile unsigned int *)0x24DD)
#define pDITCHANB5   ((volatile unsigned int *)0x24DE)

/* User bit buffers */
#define pDITUSRBITA0 ((volatile unsigned int *)0x24E0)
#define pDITUSRBITA1 ((volatile unsigned int *)0x24E1)
#define pDITUSRBITA2 ((volatile unsigned int *)0x24E2)
#define pDITUSRBITA3 ((volatile unsigned int *)0x24E3)
#define pDITUSRBITA4 ((volatile unsigned int *)0x24E4)
#define pDITUSRBITA5 ((volatile unsigned int *)0x24E5)

#define pDITUSRBITB0 ((volatile unsigned int *)0x24E8)
#define pDITUSRBITB1 ((volatile unsigned int *)0x24E9)
#define pDITUSRBITB2 ((volatile unsigned int *)0x24EA)
#define pDITUSRBITB3 ((volatile unsigned int *)0x24EB)
#define pDITUSRBITB4 ((volatile unsigned int *)0x24EC)
#define pDITUSRBITB5 ((volatile unsigned int *)0x24ED)

#define pDITUSRUPD   ((volatile unsigned int *)0x24EF)    /* Write to this when above user bits registers are updated */


/* SPDIF Receiver registers */
#define pDIRCTL      ((volatile unsigned int *)0x24A8)    /* Digital Interface Receiver Control Register */
#define pDIRSTAT     ((volatile unsigned int *)0x24A9)    /* Digital Interface Receiver Status */
#define pDIRCHANL    ((volatile unsigned int *)0x24AA)    /* Digital Interface Receiver Left Channel Status */
#define pDIRCHANR    ((volatile unsigned int *)0x24AB)    /* Digital Interface Receiver Right Channel Status */



/* PCG */
#define pPCG_CTLA0    ((volatile unsigned int *)0x24C0)  /* Precision Clock A Control Register 0 */
#define pPCG_CTLA1    ((volatile unsigned int *)0x24C1)  /* Precision Clock A Control Register 1 */
#define pPCG_CTLB0    ((volatile unsigned int *)0x24C2)  /* Precision Clock B Control Register 0 */
#define pPCG_CTLB1    ((volatile unsigned int *)0x24C3)  /* Precision Clock B Control Register 1 */
#define pPCG_PW       ((volatile unsigned int *)0x24C4)  /* Precision Clock Pulse Width Control Register */
#define pPCG_SYNC     ((volatile unsigned int *)0x24C5)  /* Precision Clock Frame Sync Synchronization */
#define pPCG_PW1      ((volatile unsigned int *)0x24C4)  /* Precision Clock Pulse Width Control Register */
#define pPCG_SYNC1    ((volatile unsigned int *)0x24C5)  /* Precision Clock Frame Sync Synchronization */

#define pPCG_CTLC0    ((volatile unsigned int *)0x24C6)  /* Precision Clock A Control Register 0 */
#define pPCG_CTLC1    ((volatile unsigned int *)0x24C7)  /* Precision Clock A Control Register 1 */
#define pPCG_CTLD0    ((volatile unsigned int *)0x24C8)  /* Precision Clock B Control Register 0 */
#define pPCG_CTLD1    ((volatile unsigned int *)0x24C9)  /* Precision Clock B Control Register 1 */
#define pPCG_PW2      ((volatile unsigned int *)0x24CA)  /* Precision Clock Pulse Width Control Register */
#define pPCG_SYNC2    ((volatile unsigned int *)0x24CB)  /* Precision Clock Frame Sync Synchronization */

/* MTM registers */
#define pMTMCTL ((volatile unsigned int *)0x2c01)   /* Memory-to-Memory DMA Control Register */
#define pIIMTMW ((volatile unsigned int *)0x2c10)   /* MTM DMA Destination Index Register */
#define pIIMTMR ((volatile unsigned int *)0x2c11)   /* MTM DMA Source Index Register */
#define pIMMTMW ((volatile unsigned int *)0x2c0e)   /* MTM DMA Destination Modify Register */
#define pIMMTMR ((volatile unsigned int *)0x2c0f)   /* MTM DMA Source Modigy Register */
#define pCMTMW  ((volatile unsigned int *)0x2c16)   /* MTM DMA Destination Count Register */
#define pCMTMR  ((volatile unsigned int *)0x2c17)   /* MTM DMA Source Count Register */

/* PWM */
#define pPWMGCTL     ((volatile unsigned int *)0x3800)    /* PWM Global Control Register */
#define pPWMGSTAT    ((volatile unsigned int *)0x3801)    /* PWM Global Status Register */

#define pPWMCTL0     ((volatile unsigned int *)0x3000)    /* PWM Control */
#define pPWMSTAT0    ((volatile unsigned int *)0x3001)    /* PWM Status */
#define pPWMPERIOD0  ((volatile unsigned int *)0x3002)    /* PWM Period Register */
#define pPWMDT0      ((volatile unsigned int *)0x3003)    /* PWM Dead Time Register */
#define pPWMA0       ((volatile unsigned int *)0x3005)    /* PWM Channel A Duty Control */
#define pPWMB0       ((volatile unsigned int *)0x3006)    /* PWM Channel B Duty Control */
#define pPWMSEG0     ((volatile unsigned int *)0x3008)    /* PWM Output Enable */
#define pPWMAL0      ((volatile unsigned int *)0x300A)    /* PWM Channel AL Duty Control */
#define pPWMBL0      ((volatile unsigned int *)0x300B)    /* PWM Channel BL Duty Control */
#define pPWMDBG0     ((volatile unsigned int *)0x300E)    /* PWM Debug Status */
#define pPWMPOL0     ((volatile unsigned int *)0x300F)    /* PWM Output polarity select */

#define pPWMCTL1     ((volatile unsigned int *)0x3010)    /* PWM Control */
#define pPWMSTAT1    ((volatile unsigned int *)0x3011)    /* PWM Status */
#define pPWMPERIOD1  ((volatile unsigned int *)0x3012)    /* PWM Period Register */
#define pPWMDT1      ((volatile unsigned int *)0x3013)    /* PWM Dead Time Register */
#define pPWMA1       ((volatile unsigned int *)0x3015)    /* PWM Channel A Duty Control */
#define pPWMB1       ((volatile unsigned int *)0x3016)    /* PWM Channel B Duty Control */
#define pPWMSEG1     ((volatile unsigned int *)0x3018)    /* PWM Output Enable */
#define pPWMAL1      ((volatile unsigned int *)0x301A)    /* PWM Channel AL Duty Control */
#define pPWMBL1      ((volatile unsigned int *)0x301B)    /* PWM Channel BL Duty Control */
#define pPWMDBG1     ((volatile unsigned int *)0x301E)    /* PWM Debug Status */
#define pPWMPOL1     ((volatile unsigned int *)0x301F)    /* PWM Output polarity select */

#define pPWMCTL2     ((volatile unsigned int *)0x3400)    /* PWM Control */
#define pPWMSTAT2    ((volatile unsigned int *)0x3401)    /* PWM Status */
#define pPWMPERIOD2  ((volatile unsigned int *)0x3402)    /* PWM Period Register */
#define pPWMDT2      ((volatile unsigned int *)0x3403)    /* PWM Dead Time Register */
#define pPWMA2       ((volatile unsigned int *)0x3405)    /* PWM Channel A Duty Control */
#define pPWMB2       ((volatile unsigned int *)0x3406)    /* PWM Channel B Duty Control */
#define pPWMSEG2     ((volatile unsigned int *)0x3408)    /* PWM Output Enable */
#define pPWMAL2      ((volatile unsigned int *)0x340A)    /* PWM Channel AL Duty Control */
#define pPWMBL2      ((volatile unsigned int *)0x340B)    /* PWM Channel BL Duty Control */
#define pPWMDBG2     ((volatile unsigned int *)0x340E)    /* PWM Debug Status */
#define pPWMPOL2     ((volatile unsigned int *)0x340F)    /* PWM Output polarity select */

#define pPWMCTL3     ((volatile unsigned int *)0x3410)    /* PWM Control */
#define pPWMSTAT3    ((volatile unsigned int *)0x3411)    /* PWM Status */
#define pPWMPERIOD3  ((volatile unsigned int *)0x3412)    /* PWM Period Register */
#define pPWMDT3      ((volatile unsigned int *)0x3413)    /* PWM Dead Time Register */
#define pPWMA3       ((volatile unsigned int *)0x3415)    /* PWM Channel A Duty Control */
#define pPWMB3       ((volatile unsigned int *)0x3416)    /* PWM Channel B Duty Control */
#define pPWMSEG3     ((volatile unsigned int *)0x3418)    /* PWM Output Enable */
#define pPWMAL3      ((volatile unsigned int *)0x341A)    /* PWM Channel AL Duty Control */
#define pPWMBL3      ((volatile unsigned int *)0x341B)    /* PWM Channel BL Duty Control */
#define pPWMDBG3     ((volatile unsigned int *)0x341E)    /* PWM Debug Status */
#define pPWMPOL3     ((volatile unsigned int *)0x341F)    /* PWM Output polarity select */

/* Registers for UART0 */
#define pUART0THR                ((volatile unsigned int *)0x3c00)    /* Transmit Holding Register */
#define pUART0RBR                ((volatile unsigned int *)0x3c00)    /* Receive Buffer Register */
#define pUART0DLL                ((volatile unsigned int *)0x3c00)    /* Divisor Latch Low Byte */
#define pUART0IER                ((volatile unsigned int *)0x3c01)    /* Interrupt Enable Register */
#define pUART0DLH                ((volatile unsigned int *)0x3c01)    /* Divisor Latch High Byte */
#define pUART0IIR                ((volatile unsigned int *)0x3c02)    /* Interrupt Identification Register */
#define pUART0LCR                ((volatile unsigned int *)0x3c03)    /* Line Control Register */
#define pUART0MODE               ((volatile unsigned int *)0x3c04)    /* Mode Register */
#define pUART0LSR                ((volatile unsigned int *)0x3c05)    /* Line Status Register */
#define pUART0SCR                ((volatile unsigned int *)0x3c07)    /* Scratch Register */
#define pUART0RBRSH              ((volatile unsigned int *)0x3c08)    /* Read Buffer Shadow Register */
#define pUART0IIRSH              ((volatile unsigned int *)0x3c09)    /* Interrupt Identification Shadow Register */
#define pUART0LSRSH              ((volatile unsigned int *)0x3c0a)    /* Line Status Shadow Register */

#define pIIUART0RX               ((volatile unsigned int *)0x3e00)    /* Internal Memory address for DMA access with UART Receiver */
#define pIMUART0RX               ((volatile unsigned int *)0x3e01)    /* Internal Memory modifier for DMA access with UART Receiver */
#define pCUART0RX                ((volatile unsigned int *)0x3e02)    /* Word Count for DMA access with UART Receiver */
#define pCPUART0RX               ((volatile unsigned int *)0x3e03)    /* Chain Point for DMA access with UART Receiver */
#define pUART0RXCTL              ((volatile unsigned int *)0x3e04)    /* UART Receiver control register */
#define pUART0RXSTAT             ((volatile unsigned int *)0x3e05)    /* UART Receiver status register */

#define pIIUART0TX               ((volatile unsigned int *)0x3f00)    /* Internal Memory address for DMA access with UART Transmitter */
#define pIMUART0TX               ((volatile unsigned int *)0x3f01)    /* Internal Memory modifier for DMA access with UART Transmitter */
#define pCUART0TX                ((volatile unsigned int *)0x3f02)    /* Word Count for DMA access with UART Transmitter */
#define pCPUART0TX               ((volatile unsigned int *)0x3f03)    /* Chain Point for DMA access with UART Transmitter */
#define pUART0TXCTL              ((volatile unsigned int *)0x3f04)    /* UART Transmitter control register */
#define pUART0TXSTAT             ((volatile unsigned int *)0x3f05)    /* UART Transmitter status register */

/* TWI Registers */
#define pTWIDIV                  ((volatile unsigned int *)0x4400) /* TWI Serial Clock Divider */
#define pTWIMITR                 ((volatile unsigned int *)0x4404) /* TWI Master Internal Time Reference */
#define pTWISCTL                 ((volatile unsigned int *)0x4408) /* TWI Slave Mode Control Register */
#define pTWISSTAT                ((volatile unsigned int *)0x440C) /* TWI Slave Mode Status Register */
#define pTWISADDR                ((volatile unsigned int *)0x4410) /* TWI Slave Mode Address Comparison Register */
#define pTWIMCTL                 ((volatile unsigned int *)0x4414) /* TWI Master Mode Control Register */
#define pTWIMSTAT                ((volatile unsigned int *)0x4418) /* TWI Master Mode Status Register */
#define pTWIMADDR                ((volatile unsigned int *)0x441C) /* TWI Master Mode Address Register */
#define pTWIIRPTL                ((volatile unsigned int *)0x4420) /* TWI Interrupt Latch Register */
#define pTWIIMASK                ((volatile unsigned int *)0x4424) /* TwI Interrupt Mask Register */
#define pTWIFIFOCTL              ((volatile unsigned int *)0x4428) /* TWI FIFO Control Register */
#define pTWIFIFOSTAT             ((volatile unsigned int *)0x442C) /* TWI FIFO Status Register */
#define pTXTWI8                  ((volatile unsigned int *)0x4480) /* TWI 8-Bit Transmit FIFO Register */
#define pTXTWI16                 ((volatile unsigned int *)0x4484) /* TWI 16-Bit Transmit FIFO Register */
#define pRXTWI8                  ((volatile unsigned int *)0x4488) /* TWI 8-Bit Receive FIFO Register */
#define pRXTWI16                 ((volatile unsigned int *)0x448C) /* TWI 16-Bit Receive FIFO Register */


/* DDR2 controller registers. */

#define pDDR2CTL0      ((volatile unsigned int *)0x1812)
#define pDDR2CTL1      ((volatile unsigned int *)0x1813)
#define pDDR2CTL2      ((volatile unsigned int *)0x1814)
#define pDDR2CTL3      ((volatile unsigned int *)0x1815)
#define pDDR2CTL4      ((volatile unsigned int *)0x1816)
#define pDDR2CTL5      ((volatile unsigned int *)0x1817)
#define pDDR2RRC       ((volatile unsigned int *)0x181D)
#define pDDR2STAT0      ((volatile unsigned int *)0x181E)
#define pDDR2STAT1      ((volatile unsigned int *)0x1840)   /* DDR2 bank active/idle status */
#define pDDR2_DTAP      ((volatile unsigned int *)0x181F)
#define pDDR2PADCTL0    ((volatile unsigned int *)0x1841)   /* Phoenix DDR2 Data, DQS, CLK, DDR2-pad control register */
#define pDDR2PADCTL1    ((volatile unsigned int *)0x1842)   /* Phoenix DDR2 Address,control, DDR2-pad control register */
#define pDLL0CTL0       ((volatile unsigned int *)0x1850)
#define pDLL0CTL1       ((volatile unsigned int *)0x1851)
#define pDLL1CTL0       ((volatile unsigned int *)0x1855)
#define pDLL1CTL1       ((volatile unsigned int *)0x1856)
#define pDLL0STAT0      ((volatile unsigned int *)0x1853)
#define pDLL0STAT1      ((volatile unsigned int *)0x1854)
#define pDLL1STAT0      ((volatile unsigned int *)0x1858)
#define pDLL1STAT1      ((volatile unsigned int *)0x1859)


/* FIR accelerator registers		 */


#define pFIRCTL1        ((volatile unsigned int *)0x5000)	/* /FIR global control register */
#define pFIRDMASTAT     ((volatile unsigned int *)0x5001)	/* /DMA status reg */
#define pFIRMACSTAT     ((volatile unsigned int *)0x5002)	/* /MAC status register */
#define pFIRDEBUGCTL     ((volatile unsigned int *)0x5004)   /* Debug control register*/
#define pFIRDBGADDR      ((volatile unsigned int *)0x5005)   /* Debug addres register*/
#define pFIRDBGWRDATA    ((volatile unsigned int *)0x5006)   /* Debug data write register*/
#define pFIRDBGRDDATA    ((volatile unsigned int *)0x5007)  /*  Debug data read register */
#define pFIRCTL2        ((volatile unsigned int *)0x5010)	/* /Channel Control register */
#define pIIFIR          ((volatile unsigned int *)0x5011)	/* /Input data Index register */
#define pIMFIR          ((volatile unsigned int *)0x5012)	/* Input data modifier register */
#define pICFIR          ((volatile unsigned int *)0x5013)	/* Input data count/buffer length register */
#define pIBFIR          ((volatile unsigned int *)0x5014)	/* Input data base register */
#define pOIFIR          ((volatile unsigned int *)0x5015)	/* Output data Index register */
#define pOMFIR          ((volatile unsigned int *)0x5016)	/* Output data modifier register */
#define pOCFIR          ((volatile unsigned int *)0x5017)	/* Output data count/buffer length register */
#define pOBFIR          ((volatile unsigned int *)0x5018)	/* Output data base register */
#define pCIFIR          ((volatile unsigned int *)0x5019)	/* Coefficient Index register */
#define pCMFIR          ((volatile unsigned int *)0x501A)	/* Coefficient modifier register */
#define pCCFIR          ((volatile unsigned int *)0x501B)	/* Coefficient count/buffer length register */
#define pCPFIR          ((volatile unsigned int *)0x501C)	/* Chain Pointer Register */


/* IIR accelerator registers */


#define pIIRCTL1     ((volatile unsigned int *)0x5200)	/* /IIR global control register */
#define pIIRDMASTAT  ((volatile unsigned int *)0x5201)	/* /DMA status reg */
#define pIIRMACSTAT  ((volatile unsigned int *)0x5202)	/* /MAC status register */
#define pIIRDEBUGCTL     ((volatile unsigned int *)0x5203)   /* IIR Debug Control register */
#define pIIRDBGADDR      ((volatile unsigned int *)0x5204)  /* IIR debug address register */
#define pIIRDBGWRDATA_L  ((volatile unsigned int *)0x5205)  /* Debug data Write register LS 32 bits */
#define pIIRDBGWRDATA_H  ((volatile unsigned int *)0x5206)  /* Debug data Write register MS 8 bits */
#define pIIRDBGRDDATA_L  ((volatile unsigned int *)0x5207)  /* Debug data Read register  LS 32 bits */
#define pIIRDBGRDDATA_H  ((volatile unsigned int *)0x5208)  /* Debug data Read register  MS 8 bits */

#define pIIRCTL2     ((volatile unsigned int *)0x5210)	/* /Channel Control register */
#define pIIIIR       ((volatile unsigned int *)0x5211)	/* /Input data Index register */
#define pIMIIR       ((volatile unsigned int *)0x5212)	/* Input data modifier register */
#define pICIIR       ((volatile unsigned int *)0x5213)	/* Input data count/buffer length register */
#define pIBIIR       ((volatile unsigned int *)0x5214)	/* Input data base register */
#define pOIIIR       ((volatile unsigned int *)0x5215)	/* Output data Index register */
#define pOMIIR       ((volatile unsigned int *)0x5216)	/* Output data modifier register */
#define pOCIIR       ((volatile unsigned int *)0x5217)	/* Output data count/buffer length register */
#define pOBIIR       ((volatile unsigned int *)0x5218)	/* Output data base register */
#define pCIIIR       ((volatile unsigned int *)0x5219)	/* Coefficient Index register */
#define pCMIIR       ((volatile unsigned int *)0x521A)	/* Coefficient modifier register */
#define pCCIIR       ((volatile unsigned int *)0x521B)	/* Coefficient count/buffer length register */
#define pCPIIR       ((volatile unsigned int *)0x521C)	/* Chain Pointer Register */

/* FFT accelerator registers */

#define pFFTCTL1       ((volatile unsigned int *)0x5300)    /* global control register */
#define pFFTMACSTAT    ((volatile unsigned int *)0x5302)    /* multiplier status register */
#define pFFTDADDR      ((volatile unsigned int *)0x5303)    /* debug address register */
#define pFFTDDATA      ((volatile unsigned int *)0x5304)    /* debug data register */
#define pFFTCTL2       ((volatile unsigned int *)0x530C)    /* mode control register */
#define pIIFFT         ((volatile unsigned int *)0x5310)    /* input index register */
#define pIMFFT         ((volatile unsigned int *)0x5311)    /* input modifier register */
#define pICFFT         ((volatile unsigned int *)0x5312)    /* input count register */
#define pILFFT         ((volatile unsigned int *)0x5313)    /* input data length register */
#define pIBFFT         ((volatile unsigned int *)0x5314)    /* input data Base register */
#define pCPIFFT        ((volatile unsigned int *)0x5315)    /* input Chain pointer register */
#define pOIFFT         ((volatile unsigned int *)0x5318)    /* Output Index register */
#define pOMFFT         ((volatile unsigned int *)0x5319)    /* Output Modify register */
#define pOCFFT         ((volatile unsigned int *)0x531A)    /* Output Count register */
#define pOLFFT         ((volatile unsigned int *)0x531B)    /* Output data length registr*/
#define pOBFFT         ((volatile unsigned int *)0x531C)    /* Output data base register */
#define pCPOFFT        ((volatile unsigned int *)0x531D)    /* Output chain pointer register */
#define pFFTDMASTAT    ((volatile unsigned int *)0x5320)    /* DMA status register */
#define pFFTSHDMASTAT  ((volatile unsigned int *)0x5321)    /* Shadow DMA status register */


/* link ports registers */
/* link port 0 registers */
#define pLCTL0					((volatile unsigned int *) 0x4c00) 
#define pLSTAT0					((volatile unsigned int *) 0x4c01)
#define pLSTAT0_SHADOW			((volatile unsigned int *) 0x4c03)
#define pTXLB0					((volatile unsigned int *) 0x4c08)
#define pTXLB0_IN_SHADOW		((volatile unsigned int *) 0x4c0d)
#define pTXLB0_OUT_SHADOW		((volatile unsigned int *) 0x4c0c)
#define pRXLB0					((volatile unsigned int *) 0x4c09)
#define pRXLB0_IN_SHADOW		((volatile unsigned int *) 0x4c0b)
#define pRXLB0_OUT_SHADOW		((volatile unsigned int *) 0x4c0a)
#define pIILB0   				((volatile unsigned int *) 0x4c18)	   /* Internal index memory register	 */
#define pIMLB0   				((volatile unsigned int *) 0x4c19)	   /* Internal memory  access modifier   	      	 */
#define pCLB0    				((volatile unsigned int *) 0x4c1a)	   /* Contains number of DMA transfers remaining */
#define pCPLB0   				((volatile unsigned int *) 0x4c1b)	   /* Points to next DMA parameters		      	 */


/* link port 1 registers */
#define pLCTL1              ((volatile unsigned int *) 0x4c20)
#define pLSTAT1             ((volatile unsigned int *) 0x4c21)
#define pLSTAT1_SHADOW      ((volatile unsigned int *) 0x4c23)
#define pTXLB1              ((volatile unsigned int *) 0x4c28)
#define pTXLB1_IN_SHADOW    ((volatile unsigned int *) 0x4c2d)
#define pTXLB1_OUT_SHADOW   ((volatile unsigned int *) 0x4c2c)
#define pRXLB1              ((volatile unsigned int *) 0x4c29)
#define pRXLB1_IN_SHADOW    ((volatile unsigned int *) 0x4c2b)
#define pRXLB1_OUT_SHADOW   ((volatile unsigned int *) 0x4c2a)
#define pIILB1              ((volatile unsigned int *) 0x4c38)	   /* Internal index memory register	 */
#define pIMLB1              ((volatile unsigned int *) 0x4c39)	   /* Internal memory  access modifier   	      	 */
#define pCLB1               ((volatile unsigned int *) 0x4c3a)	   /* Contains number of DMA transfers remaining */
#define pCPLB1              ((volatile unsigned int *) 0x4c3b)	   /* Points to next DMA parameters		      	 */


#if __ADI_HAS_MLB__

/* MLB device registers */

#define pMLB_DCCR				((volatile unsigned int *) 0x4100)	/* MLB Control register */
#define pMLB_SSCR				((volatile unsigned int *) 0x4101)	/* MLB System Status Configuration Register */
#define pMLB_SDCR				((volatile unsigned int *) 0x4102)	/* MLB System Data configuration Register */
#define pMLB_SMCR				((volatile unsigned int *) 0x4103)	/* MLB System Interrupt Mask Register */
#define pMLB_VCCR				((volatile unsigned int *) 0x4107)	/* MLB Version Control Configuration Register(contains the IP version) */
#define pMLB_SBCR				((volatile unsigned int *) 0x4108)	/* MLB Synchronous Base Address Register */
#define pMLB_ABCR				((volatile unsigned int *) 0x4109)	/* MLB Asynchronous Base Address Register */
#define pMLB_CBCR				((volatile unsigned int *) 0x410A)	/* MLB Control Base Address Register */
#define pMLB_IBCR				((volatile unsigned int *) 0x410B)	/* MLB Isochronous Base Address Register */
#define pMLB_CICR				((volatile unsigned int *) 0x410C)	/* MLB Channel Interrupt Status Register */
#define pMLB_CECR0				((volatile unsigned int *) 0x4110)	/* MLB Channel 0 Control Register */
#define pMLB_CSCR0				((volatile unsigned int *) 0x4111)	/* MLB Channel 0 Status Register */
#define pMLB_CCBCR0				((volatile unsigned int *) 0x4112)	/* MLB Channel 0 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR0				((volatile unsigned int *) 0x4113)	/* MLB Channel 0 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR0				((volatile unsigned int *) 0x41A0)	/* MLB Channel 0 Local Channel Buffer Control Register */
#define pMLB_CECR1				((volatile unsigned int *) 0x4114)  /* MLB Channel 1 Control Register */
#define pMLB_CSCR1				((volatile unsigned int *) 0x4115)	/* MLB Channel 1 Status Register */
#define pMLB_CCBCR1				((volatile unsigned int *) 0x4116)	/* MLB Channel 1 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR1				((volatile unsigned int *) 0x4117)	/* MLB Channel 1 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR1				((volatile unsigned int *) 0x41A1)	/* MLB Channel 1 Local Channel Buffer Control Register */
#define pMLB_CECR2				((volatile unsigned int *) 0x4118)	/* MLB Channel 2 Control Register */
#define pMLB_CSCR2				((volatile unsigned int *) 0x4119)	/* MLB Channel 2 Status Register */
#define pMLB_CCBCR2				((volatile unsigned int *) 0x411A)	/* MLB Channel 2 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR2				((volatile unsigned int *) 0x411B)	/* MLB Channel 2 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR2				((volatile unsigned int *) 0x41A2)	/* MLB Channel 2 Local Channel Buffer Control Register */
#define pMLB_CECR3				((volatile unsigned int *) 0x411C)	/* MLB Channel 3 Control Register */
#define pMLB_CSCR3				((volatile unsigned int *) 0x411D)	/* MLB Channel 3 Status Register */
#define pMLB_CCBCR3				((volatile unsigned int *) 0x411E)	/* MLB Channel 3 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR3				((volatile unsigned int *) 0x411F)	/* MLB Channel 3 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR3				((volatile unsigned int *) 0x41A3)	/* MLB Channel 3 Local Channel Buffer Control Register */
#define pMLB_CECR4				((volatile unsigned int *) 0x4120)	/* MLB Channel 4 Control Register */
#define pMLB_CSCR4				((volatile unsigned int *) 0x4121)	/* MLB Channel 4 Status Register */
#define pMLB_CCBCR4				((volatile unsigned int *) 0x4122)	/* MLB Channel 4 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR4				((volatile unsigned int *) 0x4123)	/* MLB Channel 4 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR4				((volatile unsigned int *) 0x41A4)	/* MLB Channel 4 Local Channel Buffer Control Register */
#define pMLB_CECR5				((volatile unsigned int *) 0x4124)	/* MLB Channel 5 Control Register */
#define pMLB_CSCR5				((volatile unsigned int *) 0x4125)	/* MLB Channel 5 Status Register */
#define pMLB_CCBCR5				((volatile unsigned int *) 0x4126)	/* MLB Channel 5 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR5				((volatile unsigned int *) 0x4127)	/* MLB Channel 5 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR5				((volatile unsigned int *) 0x41A5)	/* MLB Channel 5 Local Channel Buffer Control Register */
#define pMLB_CECR6				((volatile unsigned int *) 0x4128)	/* MLB Channel 6 Control Register */
#define pMLB_CSCR6				((volatile unsigned int *) 0x4129)	/* MLB Channel 6 Status Register */
#define pMLB_CCBCR6				((volatile unsigned int *) 0x412A)	/* MLB Channel 6 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR6				((volatile unsigned int *) 0x412B)	/* MLB Channel 6 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR6				((volatile unsigned int *) 0x41A6)	/* MLB Channel 6 Local Channel Buffer Control Register */
#define pMLB_CECR7				((volatile unsigned int *) 0x412C)	/* MLB Channel 7 Control Register */
#define pMLB_CSCR7				((volatile unsigned int *) 0x412D)	/* MLB Channel 7 Status Register */
#define pMLB_CCBCR7				((volatile unsigned int *) 0x412E)	/* MLB Channel 7 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR7				((volatile unsigned int *) 0x412F)	/* MLB Channel 7 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR7				((volatile unsigned int *) 0x41A7)	/* MLB Channel 7 Local Channel Buffer Control Register */
#define pMLB_CECR8				((volatile unsigned int *) 0x4130)	/* MLB Channel 8 Control Register */
#define pMLB_CSCR8				((volatile unsigned int *) 0x4131)	/* MLB Channel 8 Status Register */
#define pMLB_CCBCR8				((volatile unsigned int *) 0x4132)	/* MLB Channel 8 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR8				((volatile unsigned int *) 0x4133)	/* MLB Channel 8 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR8				((volatile unsigned int *) 0x41A8)	/* MLB Channel 8 Local Channel Buffer Control Register */
#define pMLB_CECR9				((volatile unsigned int *) 0x4134)	/* MLB Channel 9 Control Register */
#define pMLB_CSCR9				((volatile unsigned int *) 0x4135)	/* MLB Channel 9 Status Register */
#define pMLB_CCBCR9				((volatile unsigned int *) 0x4136)	/* MLB Channel 9 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR9				((volatile unsigned int *) 0x4137)	/* MLB Channel 9 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR9				((volatile unsigned int *) 0x41A9)	/* MLB Channel 9 Local Channel Buffer Control Register */
#define pMLB_CECR10				((volatile unsigned int *) 0x4138)	/* MLB Channel 10  Control Register */
#define pMLB_CSCR10				((volatile unsigned int *) 0x4139)	/* MLB Channel 10  Status Register */
#define pMLB_CCBCR10			((volatile unsigned int *) 0x413A)	/* MLB Channel 10  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR10			((volatile unsigned int *) 0x413B)	/* MLB Channel 10  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR10			((volatile unsigned int *) 0x41AA)	/* MLB Channel 10  Local Channel Buffer Control Register */
#define pMLB_CECR11				((volatile unsigned int *) 0x413C)	/* MLB Channel 11  Control Register */
#define pMLB_CSCR11				((volatile unsigned int *) 0x413D)	/* MLB Channel 11  Status Register */
#define pMLB_CCBCR11			((volatile unsigned int *) 0x413E)	/* MLB Channel 11  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR11			((volatile unsigned int *) 0x413F)	/* MLB Channel 11  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR11			((volatile unsigned int *) 0x41AB)	/* MLB Channel 11  Local Channel Buffer Control Register */
#define pMLB_CECR12				((volatile unsigned int *) 0x4140)	/* MLB Channel 12  Control Register */
#define pMLB_CSCR12				((volatile unsigned int *) 0x4141)	/* MLB Channel 12  Status Register */
#define pMLB_CCBCR12			((volatile unsigned int *) 0x4142)	/* MLB Channel 12  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR12			((volatile unsigned int *) 0x4143)	/* MLB Channel 12  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR12			((volatile unsigned int *) 0x41AC)	/* MLB Channel 12  Local Channel Buffer Control Register */
#define pMLB_CECR13				((volatile unsigned int *) 0x4144)	/* MLB Channel 13  Control Register */
#define pMLB_CSCR13				((volatile unsigned int *) 0x4145)	/* MLB Channel 13  Status Register */
#define pMLB_CCBCR13			((volatile unsigned int *) 0x4146)	/* MLB Channel 13  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR13			((volatile unsigned int *) 0x4147)	/* MLB Channel 13  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR13			((volatile unsigned int *) 0x41AD)	/* MLB Channel 13  Local Channel Buffer Control Register */
#define pMLB_CECR14				((volatile unsigned int *) 0x4148)	/* MLB Channel 14  Control Register */
#define pMLB_CSCR14				((volatile unsigned int *) 0x4149)	/* MLB Channel 14  Status Register */
#define pMLB_CCBCR14			((volatile unsigned int *) 0x414A)	/* MLB Channel 14  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR14			((volatile unsigned int *) 0x414B)	/* MLB Channel 14  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR14			((volatile unsigned int *) 0x41AE)	/* MLB Channel 14  Local Channel Buffer Control Register */
#define pMLB_CECR15				((volatile unsigned int *) 0x414C)	/* MLB Channel 15  Control Register */
#define pMLB_CSCR15				((volatile unsigned int *) 0x414D)	/* MLB Channel 15  Status Register */
#define pMLB_CCBCR15			((volatile unsigned int *) 0x414E)	/* MLB Channel 15  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR15			((volatile unsigned int *) 0x414F)	/* MLB Channel 15  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR15			((volatile unsigned int *) 0x41AF)	/* MLB Channel 15  Local Channel Buffer Control Register */
#define pMLB_CECR16				((volatile unsigned int *) 0x4150)	/* MLB Channel 16  Control Register */
#define pMLB_CSCR16				((volatile unsigned int *) 0x4151)	/* MLB Channel 16  Status Register */
#define pMLB_CCBCR16			((volatile unsigned int *) 0x4152)	/* MLB Channel 16  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR16			((volatile unsigned int *) 0x4153)	/* MLB Channel 16  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR16			((volatile unsigned int *) 0x41B0)	/* MLB Channel 16  Local Channel Buffer Control Register */
#define pMLB_CECR17				((volatile unsigned int *) 0x4154)	/* MLB Channel 17  Control Register */
#define pMLB_CSCR17				((volatile unsigned int *) 0x4155)	/* MLB Channel 17  Status Register */
#define pMLB_CCBCR17			((volatile unsigned int *) 0x4156)	/* MLB Channel 17  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR17			((volatile unsigned int *) 0x4157)	/* MLB Channel 17  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR17			((volatile unsigned int *) 0x41B1)	/* MLB Channel 17  Local Channel Buffer Control Register */
#define pMLB_CECR18				((volatile unsigned int *) 0x4158)	/* MLB Channel 18  Control Register */
#define pMLB_CSCR18				((volatile unsigned int *) 0x4159)	/* MLB Channel 18  Status Register */
#define pMLB_CCBCR18			((volatile unsigned int *) 0x415A)	/* MLB Channel 18  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR18			((volatile unsigned int *) 0x415B)	/* MLB Channel 18  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR18			((volatile unsigned int *) 0x41B2)	/* MLB Channel 18  Local Channel Buffer Control Register */
#define pMLB_CECR19				((volatile unsigned int *) 0x415C)	/* MLB Channel 19 Control Register */
#define pMLB_CSCR19				((volatile unsigned int *) 0x415D)	/* MLB Channel 19 Status Register */
#define pMLB_CCBCR19			((volatile unsigned int *) 0x415E)	/* MLB Channel 19 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR19			((volatile unsigned int *) 0x415F)	/* MLB Channel 19 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR19			((volatile unsigned int *) 0x41B3)	/* MLB Channel 19 Local Channel Buffer Control Register */
#define pMLB_CECR20				((volatile unsigned int *) 0x4160)	/* MLB Channel 20  Control Register */
#define pMLB_CSCR20				((volatile unsigned int *) 0x4161)	/* MLB Channel 20  Status Register */
#define pMLB_CCBCR20			((volatile unsigned int *) 0x4162)	/* MLB Channel 20  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR20			((volatile unsigned int *) 0x4163)	/* MLB Channel 20  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR20			((volatile unsigned int *) 0x41B4)	/* MLB Channel 20  Local Channel Buffer Control Register */
#define pMLB_CECR21				((volatile unsigned int *) 0x4164)	/* MLB Channel 21  Control Register */
#define pMLB_CSCR21				((volatile unsigned int *) 0x4165)	/* MLB Channel 21  Status Register */
#define pMLB_CCBCR21			((volatile unsigned int *) 0x4166)	/* MLB Channel 21  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR21			((volatile unsigned int *) 0x4167)	/* MLB Channel 21  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR21			((volatile unsigned int *) 0x41B5)	/* MLB Channel 21  Local Channel Buffer Control Register */
#define pMLB_CECR22				((volatile unsigned int *) 0x4168)	/* MLB Channel 22  Control Register */
#define pMLB_CSCR22				((volatile unsigned int *) 0x4169)	/* MLB Channel 22  Status Register */
#define pMLB_CCBCR22			((volatile unsigned int *) 0x416A)	/* MLB Channel 22  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR22			((volatile unsigned int *) 0x416B)	/* MLB Channel 22  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR22			((volatile unsigned int *) 0x41B6)	/* MLB Channel 22  Local Channel Buffer Control Register */
#define pMLB_CECR23				((volatile unsigned int *) 0x416C)	/* MLB Channel 23  Control Register */
#define pMLB_CSCR23				((volatile unsigned int *) 0x416D)	/* MLB Channel 23  Status Register */
#define pMLB_CCBCR23			((volatile unsigned int *) 0x416E)	/* MLB Channel 23  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR23			((volatile unsigned int *) 0x416F)	/* MLB Channel 23  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR23			((volatile unsigned int *) 0x41B7)	/* MLB Channel 23  Local Channel Buffer Control Register */
#define pMLB_CECR24				((volatile unsigned int *) 0x4170)	/* MLB Channel 24  Control Register */
#define pMLB_CSCR24				((volatile unsigned int *) 0x4171)	/* MLB Channel 24  Status Register */
#define pMLB_CCBCR24			((volatile unsigned int *) 0x4172)	/* MLB Channel 24  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR24			((volatile unsigned int *) 0x4173)	/* MLB Channel 24  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR24			((volatile unsigned int *) 0x41B8)	/* MLB Channel 24  Local Channel Buffer Control Register */
#define pMLB_CECR25				((volatile unsigned int *) 0x4174)	/* MLB Channel 25  Control Register */
#define pMLB_CSCR25				((volatile unsigned int *) 0x4175)	/* MLB Channel 25  Status Register */
#define pMLB_CCBCR25			((volatile unsigned int *) 0x4176)	/* MLB Channel 25  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR25			((volatile unsigned int *) 0x4177)	/* MLB Channel 25  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR25			((volatile unsigned int *) 0x41B9)	/* MLB Channel 25  Local Channel Buffer Control Register */
#define pMLB_CECR26				((volatile unsigned int *) 0x4178)	/* MLB Channel 26  Control Register */
#define pMLB_CSCR26	 			((volatile unsigned int *) 0x4179)	 /* MLB Channel 26  Status Register */
#define pMLB_CCBCR26			((volatile unsigned int *) 0x417A)	/* MLB Channel 26  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR26			((volatile unsigned int *) 0x417B)	/* MLB Channel 26  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR26			((volatile unsigned int *) 0x41BA)	/* MLB Channel 26  Local Channel Buffer Control Register */
#define pMLB_CECR27				((volatile unsigned int *) 0x417C)	/* MLB Channel 27  Control Register */
#define pMLB_CSCR27				((volatile unsigned int *) 0x417D)	/* MLB Channel 27  Status Register */
#define pMLB_CCBCR27			((volatile unsigned int *) 0x417E)	/* MLB Channel 27  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR27			((volatile unsigned int *) 0x417F)	/* MLB Channel 27  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR27			((volatile unsigned int *) 0x41BB)	/* MLB Channel 27  Local Channel Buffer Control Register */
#define pMLB_CECR28				((volatile unsigned int *) 0x4180)	/* MLB Channel 28  Control Register */
#define pMLB_CSCR28				((volatile unsigned int *) 0x4181)	/* MLB Channel 28  Status Register */
#define pMLB_CCBCR28			((volatile unsigned int *) 0x4182)	/* MLB Channel 28  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR28			((volatile unsigned int *) 0x4183)	/* MLB Channel 28  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR28			((volatile unsigned int *) 0x41BC)	/* MLB Channel 28  Local Channel Buffer Control Register */
#define pMLB_CECR29				((volatile unsigned int *) 0x4184)	/* MLB Channel 29  Control Register */
#define pMLB_CSCR29				((volatile unsigned int *) 0x4185)	/* MLB Channel 29  Status Register */
#define pMLB_CCBCR29			((volatile unsigned int *) 0x4186)	/* MLB Channel 29  Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR29			((volatile unsigned int *) 0x4187)	/* MLB Channel 29  Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR29			((volatile unsigned int *) 0x41BD)	/* MLB Channel 29  Local Channel Buffer Control Register */
#define pMLB_CECR30				((volatile unsigned int *) 0x4188)	/* MLB Channel 30 Control Register */
#define pMLB_CSCR30				((volatile unsigned int *) 0x4189)	/* MLB Channel 30 Status Register */
#define pMLB_CCBCR30			((volatile unsigned int *) 0x418A)	/* MLB Channel 30 Current Buffer Register(RX buffer in IO mode) */
#define pMLB_CNBCR30			((volatile unsigned int *) 0x418B)	/* MLB Channel 30 Next Buffer Register (TX buffer in IO mode) */
#define pMLB_LCBCR30			((volatile unsigned int *) 0x41BE)	/* MLB Channel 30 Local Channel Buffer Control Register */

#endif /* __ADI_HAS_MLB__ */

#define pEPCTL ((volatile unsigned int *)0x1801) /* External Port Global Control Register */
		
#endif /* __CDEF21469_H__ */
