/************************************************************************
 *
 * def21469.h
 *
 * Include this file to use definitions for the ADSP-21469. These definitions
 * are the subset of peripherals and registers common to the ADSP-2146x.
 *
 * The header file corresponding to the processor automatically defines the correct
 * macro for the available peripherals. Please use the correct header file for your
 * processor
 *
 * (c) Copyright 2009-2011 Analog Devices, Inc.  All rights reserved.
 * 
 ************************************************************************/

#ifndef __DEF21469_H_
#define __DEF21469_H_

#ifdef _MISRA_RULES
#pragma diag(push)
#pragma diag(suppress:misra_rule_2_4)
#pragma diag(suppress:misra_rule_19_4)
#endif /* _MISRA_RULES */

#if 0

    SYSTEM & IOP REGISTER BIT & ADDRESS DEFINITIONS FOR ADSP-21469
    This include file contains a list of macro "defines" to enable the
    programmer to use symbolic names for the following ADSP-2146x facilities:
     - instruction condition codes
     - system register bit definitions
     - IOP register address memory map
     - *most* IOP control/status register bit definitions

    Here are some example uses:

        bit set mode1 BR0|IRPTEN|ALUSAT;
        ustat1=IIVT|IMDW0 ;
        DM(SYSCTL)=ustat1;

#endif

#define BIT_0   0x00000001
#define BIT_1   0x00000002
#define BIT_2   0x00000004
#define BIT_3   0x00000008
#define BIT_4   0x00000010
#define BIT_5   0x00000020
#define BIT_6   0x00000040
#define BIT_7   0x00000080
#define BIT_8   0x00000100
#define BIT_9   0x00000200
#define BIT_10  0x00000400
#define BIT_11  0x00000800
#define BIT_12  0x00001000
#define BIT_13  0x00002000
#define BIT_14  0x00004000
#define BIT_15  0x00008000
#define BIT_16  0x00010000
#define BIT_17  0x00020000
#define BIT_18  0x00040000
#define BIT_19  0x00080000
#define BIT_20  0x00100000
#define BIT_21  0x00200000
#define BIT_22  0x00400000
#define BIT_23  0x00800000
#define BIT_24  0x01000000
#define BIT_25  0x02000000
#define BIT_26  0x04000000
#define BIT_27  0x08000000
#define BIT_28  0x10000000
#define BIT_29  0x20000000
#define BIT_30  0x40000000
#define BIT_31  0x80000000

/*============================================================================*/
/*                                                                            */
/*                 I/O Processor Register Address Memory Map                  */
/*                                                                            */
/*============================================================================*/

/* Emulation/Breakpoint Registers */
#define EEMUIN      (0x30020)   /* Emulator Input FIFO */
#define EEMUSTAT    (0x30021)   /* Enhanced Emulation Status Register */
#define EEMUOUT     (0x30022)   /* Emulator Output FIFO */
#define OSPID       (0x30023)   /* Operating System Process ID */
#define SYSCTL      (0x30024)   /* System Control Register */
#define BRKCTL      (0x30025)   /* Hardware Breakpoint Control Register */
#define REVPID      (0x30026)   /* Emulation/Revision ID Register */
#define PSA1S       (0x300a0)   /* Instruction Breakpoint Address Start #1 */
#define PSA1E       (0x300a1)   /* Instruction Breakpoint Address End #1 */
#define PSA2S       (0x300a2)   /* Instruction Breakpoint Address Start #2 */
#define PSA2E       (0x300a3)   /* Instruction Breakpoint Address End #2 */
#define PSA3S       (0x300a4)   /* Instruction Breakpoint Address Start #3 */
#define PSA3E       (0x300a5)   /* Instruction Breakpoint Address End #3 */
#define PSA4S       (0x300a6)   /* Instruction Breakpoint Address Start #4 */
#define PSA4E       (0x300a7)   /* Instruction Breakpoint Address End #4 */
#define DMA1S       (0x300b2)   /* Data Memory Breakpoint Address Start #1 */
#define DMA1E       (0x300b3)   /* Data Memory Breakpoint Address End #1 */
#define DMA2S       (0x300b4)   /* Data Memory Breakpoint Address Start #2 */
#define DMA2E       (0x300b5)   /* Data Memory Breakpoint Address End #2 */
#define IOAS        (0x300b0)   /* I/O Breakpoint Address Start */
#define IOAE        (0x300b1)   /* I/O Breakpoint Address End */
#define PMDAS       (0x300b8)   /* Program Memory Data Breakpoint Address Start  */
#define PMDAE       (0x300b9)   /* Program Memory Data Breakpoint Address End  */
#define EMUN        (0x300ae)   /* Number of Breakpoints before EMU interrupt */

/* IOP registers for SDRAM controller */
#define  AMICTL0    (0x1804)   /* Asynchronous Memory Interface Control Register for Bank 1 */
#define  AMICTL1    (0x1805)   /* Asynchronous Memory Interface Control Register for Bank 2 */
#define  AMICTL2    (0x1806)   /* Asynchronous Memory Interface Control Register for Bank 3 */
#define  AMICTL3    (0x1807)   /* Asynchronous Memory Interface Control Register for Bank 4 */
#define  AMISTAT    (0x180a)   /* Asynchronous Memory Interface Status Register */

/*DMA address registers */
#define DMAC0   (0x180b)   /* External Port DMA Channel 0 Control Register */
#define DMAC1   (0x180c)   /* External Port DMA Channel 1 Control Register */

#define EIEP0   (0x1820)   /* External Port DMA external index address */
#define EMEP0   (0x1821)   /* External Port DMA external modifier */
#define ECEP0   (0x1822)   /* External Port DMA external count */
#define IIEP0   (0x1823)   /* External Port DMA internal index address */
#define IMEP0   (0x1824)   /* External Port DMA internal modifier */
#define ICEP0   (0x1825)   /* External Port DMA internal count */
#define CEP0    (0x1825)   /* External Port DMA internal count */
#define CPEP0   (0x1826)   /* External Port DMA chain pointer */
#define EBEP0   (0x1827)   /* External Port DMA external base address */
#define TPEP0   (0x1828)   /* External Port DMA tap pointer */
#define ELEP0   (0x1829)   /* External Port DMA external length */
#define EPTC0   (0x182B)   /* External Port DMA delay line tap count */
#define TCEP0   (0x182B)   /* External Port DMA delay line tap count */
#define DFEP0   (0x182c)   /* Data FIFO */


#define EIEP1   (0x1830)   /* External Port DMA external index address */
#define EMEP1   (0x1831)   /* External Port DMA external modifier */
#define ECEP1   (0x1832)   /* External Port DMA external count */
#define IIEP1   (0x1833)   /* External Port DMA internal index address */
#define IMEP1   (0x1834)   /* External Port DMA internal modifier */
#define ICEP1   (0x1835)   /* External Port DMA internal count */
#define CEP1    (0x1835)   /* External Port DMA internal count */
#define CPEP1   (0x1836)   /* External Port DMA chain pointer */
#define EBEP1   (0x1837)   /* External Port DMA external base address */
#define TPEP1   (0x1838)   /* External Port DMA tap pointer */
#define ELEP1   (0x1839)   /* External Port DMA external length */
#define EPTC1   (0x183B)   /* External Port DMA delay line tap count */
#define TCEP1   (0x183B)   /* External Port DMA delay line tap count */
#define DFEP1   (0x183c)   /* Data FIFO */


/* SPORT */
#define SPERRSTAT    (0x2300)  /* SPORT Global Interrupt Status Register */

/* Serial Port registers (SP01) */
#define SPCTL0      (0xc00)  /* SPORT 0 control register */
#define SPCTL1      (0xc01)  /* SPORT 1 control register */
#define DIV0        (0xc02)  /* SPORT 0 divisor for transmit/receive SCLK0 and FS0 */
#define DIV1        (0xc03)  /* SPORT 1 divisor for transmit/receive SCLK1 and FS1 */
#define SPMCTL0     (0xc04)  /* SPORT 0 Multichannel Control Register */
#define SPMCTL1     (0xc17)  /* SPORT 1 Multichannel Control Register */

/* MT and MR registers are included here for code compatibility */
#define MT0CS0      (0xc05)  /* SPORT 0 multichannel tx select, channels 31 - 0 */
#define MT0CS1      (0xc06)  /* SPORT 0 multichannel tx select, channels 63 - 32 */
#define MT0CS2      (0xc07)  /* SPORT 0 multichannel tx select, channels 95 - 64 */
#define MT0CS3      (0xc08)  /* SPORT 0 multichannel tx select, channels 127 - 96 */
#define MR1CS0      (0xc09)  /* SPORT 1 multichannel rx select, channels 31 - 0 */
#define MR1CS1      (0xc0A)  /* SPORT 1 multichannel rx select, channels 63 - 32 */
#define MR1CS2      (0xc0B)  /* SPORT 1 multichannel rx select, channels 95 - 64 */
#define MR1CS3      (0xc0C)  /* SPORT 1 multichannel rx select, channels 127 - 96 */
#define MT0CCS0     (0xc0D)  /* SPORT 0 multichannel tx compand select, channels 31 - 0 */
#define MT0CCS1     (0xc0E)  /* SPORT 0 multichannel tx compand select, channels 63 - 32 */
#define MT0CCS2     (0xc0F)  /* SPORT 0 multichannel tx compand select, channels 95 - 64 */
#define MT0CCS3     (0xc10)  /* SPORT 0 multichannel tx compand select, channels 127 - 96 */
#define MR1CCS0     (0xc11)  /* SPORT 1 multichannel rx compand select, channels 31 - 0 */
#define MR1CCS1     (0xc12)  /* SPORT 1 multichannel rx compand select, channels 63 - 32 */
#define MR1CCS2     (0xc13)  /* SPORT 1 multichannel rx compand select, channels 95 - 64 */
#define MR1CCS3     (0xc14)  /* SPORT 1 multichannel rx compand select, channels 127 - 96 */

#define SPCNT0      (0xc15)  /* SPORT Count register - status information for internal clock and fs */
#define SPCNT1      (0xc16)  /* SPORT Count register - status information for internal clock and fs */

#define SPERRCTL0   (0xc18)  /* SPORT 0 Error Interrupt Control Register */
#define SPERRCTL1   (0xc19)  /* SPORT 1 Error Interrupt Control Register */

#define SPCTLN0	(0xc1A)		 /* Control Register 2 for SP0 */
#define SPCTLN1	(0xc1B)		 /* Control Register 2 for SP1 */

/* SPORT TDM registers - Naming changed since the direction is programmable now */
#define SP0CS0       (0xc05)  /* SPORT 0 multichannel select, channels 31 - 0 */
#define SP0CS1       (0xc06)  /* SPORT 0 multichannel select, channels 63 - 32 */
#define SP0CS2       (0xc07)  /* SPORT 0 multichannel select, channels 95 - 64 */
#define SP0CS3       (0xc08)  /* SPORT 0 multichannel select, channels 127 - 96 */
#define SP1CS0       (0xc09)  /* SPORT 1 multichannel select, channels 31 - 0 */
#define SP1CS1       (0xc0A)  /* SPORT 1 multichannel select, channels 63 - 32 */
#define SP1CS2       (0xc0B)  /* SPORT 1 multichannel select, channels 95 - 64 */
#define SP1CS3       (0xc0C)  /* SPORT 1 multichannel select, channels 127 - 96 */
#define SP0CCS0      (0xc0D)  /* SPORT 0 multichannel compand select, channels 31 - 0 */
#define SP0CCS1      (0xc0E)  /* SPORT 0 multichannel compand select, channels 63 - 32 */
#define SP0CCS2      (0xc0F)  /* SPORT 0 multichannel compand select, channels 95 - 64 */
#define SP0CCS3      (0xc10)  /* SPORT 0 multichannel compand select, channels 127 - 96 */
#define SP1CCS0      (0xc11)  /* SPORT 1 multichannel compand select, channels 31 - 0 */
#define SP1CCS1      (0xc12)  /* SPORT 1 multichannel compand select, channels 63 - 32 */
#define SP1CCS2      (0xc13)  /* SPORT 1 multichannel compand select, channels 95 - 64 */
#define SP1CCS3      (0xc14)  /* SPORT 1 multichannel compand select, channels 127 - 96 */

#define IISP0A      (0xc40) /* Internal memory DMA address */
#define IMSP0A      (0xc41) /* Internal memory DMA access modifier */
#define CSP0A       (0xc42) /* Contains number of DMA transfers remaining */
#define CPSP0A      (0xc43) /* Points to next DMA parameters */
#define IISP0B      (0xc44) /* Internal memory DMA address */
#define IMSP0B      (0xc45) /* Internal memory DMA access modifier */
#define CSP0B       (0xc46) /* Contains number of DMA transfers remaining */
#define CPSP0B      (0xc47) /* Points to next DMA parameters */
#define IISP1A      (0xc48) /* Internal memory DMA address */
#define IMSP1A      (0xc49) /* Internal memory DMA access modifier */
#define CSP1A       (0xc4A) /* Contains number of DMA transfers remaining */
#define CPSP1A      (0xc4B) /* Points to next DMA parameters */
#define IISP1B      (0xc4C) /* Internal memory DMA address */
#define IMSP1B      (0xc4D) /* Internal memory DMA access modifier */
#define CSP1B       (0xc4E) /* Contains number of DMA transfers remaining */
#define CPSP1B      (0xc4F) /* Points to next DMA parameters */
#define TXSP0A      (0xc60)  /* SPORT 0A transmit data register */
#define RXSP0A      (0xc61)  /* SPORT 0A receive data register */
#define TXSP0B      (0xc62)  /* SPORT 0B transmit data register */
#define RXSP0B      (0xc63)  /* SPORT 0B receive data register */
#define TXSP1A      (0xc64)  /* SPORT 1A transmit data register */
#define RXSP1A      (0xc65)  /* SPORT 1A receive data register */
#define TXSP1B      (0xc66)  /* SPORT 1B transmit data register */
#define RXSP1B      (0xc67)  /* SPORT 1B receive data register */

/* Serial Port registers (SP23) */
#define SPCTL2      (0x400)  /* SPORT 2 control register */
#define SPCTL3      (0x401)  /* SPORT 3 control register */
#define DIV2        (0x402)  /* SPORT 2 divisor for transmit/receive SCLK2 and FS2 */
#define DIV3        (0x403)  /* SPORT 3 divisor for transmit/receive SCLK3 and FS3 */
#define SPMCTL2     (0x404)  /* SPORT 2 Multichannel Control Register */
#define SPMCTL3     (0x417)  /* SPORT 3 Multichannel Control Register */

/* MT and MR registers are included here for code compatibility */
#define MT2CS0      (0x405)  /* SPORT 2 multichannel tx select, channels 31 - 0 */
#define MT2CS1      (0x406)  /* SPORT 2 multichannel tx select, channels 63 - 32 */
#define MT2CS2      (0x407)  /* SPORT 2 multichannel tx select, channels 95 - 64 */
#define MT2CS3      (0x408)  /* SPORT 2 multichannel tx select, channels 127 - 96 */
#define MR3CS0      (0x409)  /* SPORT 3 multichannel rx select, channels 31 - 0 */
#define MR3CS1      (0x40A)  /* SPORT 3 multichannel rx select, channels 63 - 32 */
#define MR3CS2      (0x40B)  /* SPORT 3 multichannel rx select, channels 95 - 64 */
#define MR3CS3      (0x40C)  /* SPORT 3 multichannel rx select, channels 127 - 96 */
#define MT2CCS0     (0x40D)  /* SPORT 2 multichannel tx compand select, channels 31 - 0 */
#define MT2CCS1     (0x40E)  /* SPORT 2 multichannel tx compand select, channels 63 - 32 */
#define MT2CCS2     (0x40F)  /* SPORT 2 multichannel tx compand select, channels 95 - 64 */
#define MT2CCS3     (0x410)  /* SPORT 2 multichannel tx compand select, channels 127 - 96 */
#define MR3CCS0     (0x411)  /* SPORT 3 multichannel rx compand select, channels 31 - 0 */
#define MR3CCS1     (0x412)  /* SPORT 3 multichannel rx compand select, channels 63 - 32 */
#define MR3CCS2     (0x413)  /* SPORT 3 multichannel rx compand select, channels 95 - 64 */
#define MR3CCS3     (0x414)  /* SPORT 3 multichannel rx compand select, channels 127 - 96 */

#define SPCNT2      (0x415)  /* SPORT Count register - status information for internal clock and fs */
#define SPCNT3      (0x416)  /* SPORT Count register - status information for internal clock and fs */

#define SPERRCTL2   (0x418)  /* SPORT 2 Error Interrupt Control Register */
#define SPERRCTL3   (0x419)  /* SPORT 3 Error Interrupt Control Register */

#define SPCTLN2	 (0x41A)	/* Control Register 2 for SP2 */
#define SPCTLN3	 (0x41B)	/* Control Register 2 for SP3 */


/* SPORT TDM registers - Naming changed since the direction is programmable now */
#define SP2CS0       (0x405)  /* SPORT 2 multichannel select, channels 31 - 0 */
#define SP2CS1       (0x406)  /* SPORT 2 multichannel select, channels 63 - 32 */
#define SP2CS2       (0x407)  /* SPORT 2 multichannel select, channels 95 - 64 */
#define SP2CS3       (0x408)  /* SPORT 2 multichannel select, channels 127 - 96 */
#define SP3CS0       (0x409)  /* SPORT 3 multichannel select, channels 31 - 0 */
#define SP3CS1       (0x40A)  /* SPORT 3 multichannel select, channels 63 - 32 */
#define SP3CS2       (0x40B)  /* SPORT 3 multichannel select, channels 95 - 64 */
#define SP3CS3       (0x40C)  /* SPORT 3 multichannel select, channels 127 - 96 */
#define SP2CCS0      (0x40D)  /* SPORT 2 multichannel compand select, channels 31 - 0 */
#define SP2CCS1      (0x40E)  /* SPORT 2 multichannel compand select, channels 63 - 32 */
#define SP2CCS2      (0x40F)  /* SPORT 2 multichannel compand select, channels 95 - 64 */
#define SP2CCS3      (0x410)  /* SPORT 2 multichannel compand select, channels 127 - 96 */
#define SP3CCS0      (0x411)  /* SPORT 3 multichannel compand select, channels 31 - 0 */
#define SP3CCS1      (0x412)  /* SPORT 3 multichannel compand select, channels 63 - 32 */
#define SP3CCS2      (0x413)  /* SPORT 3 multichannel compand select, channels 95 - 64 */
#define SP3CCS3      (0x414)  /* SPORT 3 multichannel compand select, channels 127 - 96 */

#define IISP2A      (0x440)  /* Internal memory DMA address */
#define IMSP2A      (0x441)  /* Internal memory DMA access modifier */
#define CSP2A       (0x442)  /* Contains number of DMA transfers remaining */
#define CPSP2A      (0x443)  /* Points to next DMA parameters */
#define IISP2B      (0x444)  /* Internal memory DMA address */
#define IMSP2B      (0x445)  /* Internal memory DMA access modifier */
#define CSP2B       (0x446)  /* Contains number of DMA transfers remaining */
#define CPSP2B      (0x447)  /* Points to next DMA parameters */
#define IISP3A      (0x448)  /* Internal memory DMA address */
#define IMSP3A      (0x449)  /* Internal memory DMA access modifier */
#define CSP3A       (0x44A)  /* Contains number of DMA transfers remaining */
#define CPSP3A      (0x44B)  /* Points to next DMA parameters */
#define IISP3B      (0x44C)  /* Internal memory DMA address */
#define IMSP3B      (0x44D)  /* Internal memory DMA access modifier */
#define CSP3B       (0x44E)  /* Contains number of DMA transfers remaining */
#define CPSP3B      (0x44F)  /* Points to next DMA parameters */
#define TXSP2A      (0x460)  /* SPORT 2A transmit data register */
#define RXSP2A      (0x461)  /* SPORT 2A receive data register */
#define TXSP2B      (0x462)  /* SPORT 2B transmit data register */
#define RXSP2B      (0x463)  /* SPORT 2B receive data register */
#define TXSP3A      (0x464)  /* SPORT 3A transmit data register */
#define RXSP3A      (0x465)  /* SPORT 3A receive data register */
#define TXSP3B      (0x466)  /* SPORT 3B transmit data register */
#define RXSP3B      (0x467)  /* SPORT 3B receive data register */

/* Serial Port registers (SP45) */
#define SPCTL4      (0x800)  /* SPORT 4 control register */
#define SPCTL5      (0x801)  /* SPORT 5 control register */
#define DIV4        (0x802)  /* SPORT 4 divisor for transmit/receive SCLK4 and FS4 */
#define DIV5        (0x803)  /* SPORT 5 divisor for transmit/receive SCLK5 and FS5 */
#define SPMCTL4     (0x804)  /* SPORT 4 Multichannel Control Register */
#define SPMCTL5     (0x817)  /* SPORT 5 Multichannel Control Register */

/* MT and MR registers are included here for code compatibility */
#define MT4CS0      (0x805)  /* SPORT 4 multichannel tx select, channels 31 - 0 */
#define MT4CS1      (0x806)  /* SPORT 4 multichannel tx select, channels 63 - 32 */
#define MT4CS2      (0x807)  /* SPORT 4 multichannel tx select, channels 95 - 64 */
#define MT4CS3      (0x808)  /* SPORT 4 multichannel tx select, channels 127 - 96 */
#define MR5CS0      (0x809)  /* SPORT 5 multichannel rx select, channels 31 - 0 */
#define MR5CS1      (0x80A)  /* SPORT 5 multichannel rx select, channels 63 - 32 */
#define MR5CS2      (0x80B)  /* SPORT 5 multichannel rx select, channels 95 - 64 */
#define MR5CS3      (0x80C)  /* SPORT 5 multichannel rx select, channels 127 - 96 */
#define MT4CCS0     (0x80D)  /* SPORT 4 multichannel tx compand select, channels 31 - 0 */
#define MT4CCS1     (0x80E)  /* SPORT 4 multichannel tx compand select, channels 63 - 32 */
#define MT4CCS2     (0x80F)  /* SPORT 4 multichannel tx compand select, channels 95 - 64 */
#define MT4CCS3     (0x810)  /* SPORT 4 multichannel tx compand select, channels 127 - 96 */
#define MR5CCS0     (0x811)  /* SPORT 5 multichannel rx compand select, channels 31 - 0 */
#define MR5CCS1     (0x812)  /* SPORT 5 multichannel rx compand select, channels 63 - 32 */
#define MR5CCS2     (0x813)  /* SPORT 5 multichannel rx compand select, channels 95 - 64 */
#define MR5CCS3     (0x814)  /* SPORT 5 multichannel rx compand select, channels 127 - 96 */

#define SPCNT4      (0x815)  /* SPORT Count register - status information for internal clock and fs */
#define SPCNT5      (0x816)  /* SPORT Count register - status information for internal clock and fs */

#define SPERRCTL4   (0x818)  /* SPORT 4 Error Interrupt Control Register */
#define SPERRCTL5   (0x819)  /* SPORT 5 Error Interrupt Control Register */

#define SPCTLN4	 (0x81A)	 /* Control Register 2 for SP4 */
#define SPCTLN5	 (0x81B)	 /* Control Register 2 for SP5 */


/* SPORT TDM registers - Naming changed since the direction is programmable now */
#define SP4CS0       (0x805)  /* SPORT 4 multichannel select, channels 31 - 0 */
#define SP4CS1       (0x806)  /* SPORT 4 multichannel select, channels 63 - 32 */
#define SP4CS2       (0x807)  /* SPORT 4 multichannel select, channels 95 - 64 */
#define SP4CS3       (0x808)  /* SPORT 4 multichannel select, channels 127 - 96 */
#define SP5CS0       (0x809)  /* SPORT 5 multichannel select, channels 31 - 0 */
#define SP5CS1       (0x80A)  /* SPORT 5 multichannel select, channels 63 - 32 */
#define SP5CS2       (0x80B)  /* SPORT 5 multichannel select, channels 95 - 64 */
#define SP5CS3       (0x80C)  /* SPORT 5 multichannel select, channels 127 - 96 */
#define SP4CCS0      (0x80D)  /* SPORT 4 multichannel compand select, channels 31 - 0 */
#define SP4CCS1      (0x80E)  /* SPORT 4 multichannel compand select, channels 63 - 32 */
#define SP4CCS2      (0x80F)  /* SPORT 4 multichannel compand select, channels 95 - 64 */
#define SP4CCS3      (0x810)  /* SPORT 4 multichannel compand select, channels 127 - 96 */
#define SP5CCS0      (0x811)  /* SPORT 5 multichannel compand select, channels 31 - 0 */
#define SP5CCS1      (0x812)  /* SPORT 5 multichannel compand select, channels 63 - 32 */
#define SP5CCS2      (0x813)  /* SPORT 5 multichannel compand select, channels 95 - 64 */
#define SP5CCS3      (0x814)  /* SPORT 5 multichannel compand select, channels 127 - 96 */

#define IISP4A      (0x840)  /* Internal memory DMA address */
#define IMSP4A      (0x841)  /* Internal memory DMA access modifier */
#define CSP4A       (0x842)  /* Contains number of DMA transfers remaining */
#define CPSP4A      (0x843)  /* Points to next DMA parameters */
#define IISP4B      (0x844)  /* Internal memory DMA address */
#define IMSP4B      (0x845)  /* Internal memory DMA access modifier */
#define CSP4B       (0x846)  /* Contains number of DMA transfers remaining */
#define CPSP4B      (0x847)  /* Points to next DMA parameters */
#define IISP5A      (0x848)  /* Internal memory DMA address */
#define IMSP5A      (0x849)  /* Internal memory DMA access modifier */
#define CSP5A       (0x84A)  /* Contains number of DMA transfers remaining */
#define CPSP5A      (0x84B)  /* Points to next DMA parameters */
#define IISP5B      (0x84C)  /* Internal memory DMA address */
#define IMSP5B      (0x84D)  /* Internal memory DMA access modifier */
#define CSP5B       (0x84E)  /* Contains number of DMA transfers remaining */
#define CPSP5B      (0x84F)  /* Points to next DMA parameters */
#define TXSP4A      (0x860)  /* SPORT 4A transmit data register */
#define RXSP4A      (0x861)  /* SPORT 4A receive data register */
#define TXSP4B      (0x862)  /* SPORT 4B transmit data register */
#define RXSP4B      (0x863)  /* SPORT 4B receive data register */
#define TXSP5A      (0x864)  /* SPORT 5A transmit data register */
#define RXSP5A      (0x865)  /* SPORT 5A receive data register */
#define TXSP5B      (0x866)  /* SPORT 5B transmit data register */
#define RXSP5B      (0x867)  /* SPORT 5B receive data register */

/* SP67 */
#define SPCTL6      (0x4800)  /* SPORT 6 control register */
#define SPCTL7      (0x4801)  /* SPORT 7 control register */
#define DIV6        (0x4802)  /* SPORT 6 divisor for transmit/receive SCLK4 and FS4 */
#define DIV7        (0x4803)  /* SPORT 7 divisor for transmit/receive SCLK5 and FS5 */
#define SPMCTL6     (0x4804)  /* SPORT 6 Multichannel Control Register */
#define SPMCTL7     (0x4817)  /* SPORT 7 Multichannel Control Register */

/* MT and MR registers are included here for code compatibility */
#define MT6CS0      (0x4805)  /* SPORT 6 multichannel tx select, channels 31 - 0 */
#define MT6CS1      (0x4806)  /* SPORT 6 multichannel tx select, channels 63 - 32 */
#define MT6CS2      (0x4807)  /* SPORT 6 multichannel tx select, channels 95 - 64 */
#define MT6CS3      (0x4808)  /* SPORT 6 multichannel tx select, channels 127 - 96 */
#define MR7CS0      (0x4809)  /* SPORT 7 multichannel rx select, channels 31 - 0 */
#define MR7CS1      (0x480A)  /* SPORT 7 multichannel rx select, channels 63 - 32 */
#define MR7CS2      (0x480B)  /* SPORT 7 multichannel rx select, channels 95 - 64 */
#define MR7CS3      (0x480C)  /* SPORT 7 multichannel rx select, channels 127 - 96 */
#define MT6CCS0     (0x480D)  /* SPORT 6 multichannel tx compand select, channels 31 - 0 */
#define MT6CCS1     (0x480E)  /* SPORT 6 multichannel tx compand select, channels 63 - 32 */
#define MT6CCS2     (0x480F)  /* SPORT 6 multichannel tx compand select, channels 95 - 64 */
#define MT6CCS3     (0x4810)  /* SPORT 6 multichannel tx compand select, channels 127 - 96 */
#define MR7CCS0     (0x4811)  /* SPORT 7 multichannel rx compand select, channels 31 - 0 */
#define MR7CCS1     (0x4812)  /* SPORT 7 multichannel rx compand select, channels 63 - 32 */
#define MR7CCS2     (0x4813)  /* SPORT 7 multichannel rx compand select, channels 95 - 64 */
#define MR7CCS3     (0x4814)  /* SPORT 7 multichannel rx compand select, channels 127 - 96 */

#define SPCNT6      (0x4815)  /* SPORT Count register - status information for internal clock and fs */
#define SPCNT7      (0x4816)  /* SPORT Count register - status information for internal clock and fs */

#define SPERRCTL6   (0x4818)  /* SPORT 6 Error Interrupt Control Register */
#define SPERRCTL7   (0x4819)  /* SPORT 7 Error Interrupt Control Register */

#define SPCTLN6	 	(0x481A)  /* Control Register 2 for SP6 */
#define SPCTLN7	 	(0x481B)  /* Control Register 2 for SP7 */


/* SPORT TDM registers - Naming changed since the direction is programmable now */
#define SP6CS0       (0x4805)  /* SPORT 6 multichannel select, channels 31 - 0 */
#define SP6CS1       (0x4806)  /* SPORT 6 multichannel select, channels 63 - 32 */
#define SP6CS2       (0x4807)  /* SPORT 6 multichannel select, channels 95 - 64 */
#define SP6CS3       (0x4808)  /* SPORT 6 multichannel select, channels 127 - 96 */
#define SP7CS0       (0x4809)  /* SPORT 7 multichannel select, channels 31 - 0 */
#define SP7CS1       (0x480A)  /* SPORT 7 multichannel select, channels 63 - 32 */
#define SP7CS2       (0x480B)  /* SPORT 7 multichannel select, channels 95 - 64 */
#define SP7CS3       (0x480C)  /* SPORT 7 multichannel select, channels 127 - 96 */
#define SP6CCS0      (0x480D)  /* SPORT 6 multichannel compand select, channels 31 - 0 */
#define SP6CCS1      (0x480E)  /* SPORT 6 multichannel compand select, channels 63 - 32 */
#define SP6CCS2      (0x480F)  /* SPORT 6 multichannel compand select, channels 95 - 64 */
#define SP6CCS3      (0x4810)  /* SPORT 6 multichannel compand select, channels 127 - 96 */
#define SP7CCS0      (0x4811)  /* SPORT 7 multichannel compand select, channels 31 - 0 */
#define SP7CCS1      (0x4812)  /* SPORT 7 multichannel compand select, channels 63 - 32 */
#define SP7CCS2      (0x4813)  /* SPORT 7 multichannel compand select, channels 95 - 64 */
#define SP7CCS3      (0x4814)  /* SPORT 7 multichannel compand select, channels 127 - 96 */

#define IISP6A      (0x4840)  /* Internal memory DMA address */
#define IMSP6A      (0x4841)  /* Internal memory DMA access modifier */
#define CSP6A       (0x4842)  /* Contains number of DMA transfers remaining */
#define CPSP6A      (0x4843)  /* Points to next DMA parameters */
#define IISP6B      (0x4844)  /* Internal memory DMA address */
#define IMSP6B      (0x4845)  /* Internal memory DMA access modifier */
#define CSP6B       (0x4846)  /* Contains number of DMA transfers remaining */
#define CPSP6B      (0x4847)  /* Points to next DMA parameters */
#define IISP7A      (0x4848)  /* Internal memory DMA address */
#define IMSP7A      (0x4849)  /* Internal memory DMA access modifier */
#define CSP7A       (0x484A)  /* Contains number of DMA transfers remaining */
#define CPSP7A      (0x484B)  /* Points to next DMA parameters */
#define IISP7B      (0x484C)  /* Internal memory DMA address */
#define IMSP7B      (0x484D)  /* Internal memory DMA access modifier */
#define CSP7B       (0x484E)  /* Contains number of DMA transfers remaining */
#define CPSP7B      (0x484F)  /* Points to next DMA parameters */
#define TXSP6A      (0x4860)  /* SPORT 4A transmit data register */
#define RXSP6A      (0x4861)  /* SPORT 4A receive data register */
#define TXSP6B      (0x4862)  /* SPORT 4B transmit data register */
#define RXSP6B      (0x4863)  /* SPORT 4B receive data register */
#define TXSP7A      (0x4864)  /* SPORT 5A transmit data register */
#define RXSP7A      (0x4865)  /* SPORT 5A receive data register */
#define TXSP7B      (0x4866)  /* SPORT 5B transmit data register */
#define RXSP7B      (0x4867)  /* SPORT 5B receive data register */

/* SPI Registers */
#define SPICTL          (0x1000)   /* SPI Control Register */
#define SPIFLG          (0x1001)   /* SPI Flag register */
#define SPISTAT         (0x1002)   /* SPI Status register */
#define TXSPI           (0x1003)   /* SPI transmit data register */
#define RXSPI           (0x1004)   /* SPI receive data register */
#define SPIBAUD         (0x1005)   /* SPI baud setup register */
#define RXSPI_SHADOW    (0x1006)   /* SPI receive data shadow register */
#define IISPI           (0x1080)   /* Internal memory DMA address */
#define IMSPI           (0x1081)   /* Internal memory DMA access modifier */
#define CSPI            (0x1082)   /* Contains number of DMA transfers remaining */
#define CPSPI           (0x1083)   /* Points to next DMA parameters */
#define SPIDMAC         (0x1084)   /* SPI DMA control register */
#define SPINTST         (0x1085)   /* Apps based change */

/* SPIB Registers: This SPI port is routed through the DAI */
#define SPICTLB         (0x2800)   /* SPIB Control Register */
#define SPIFLGB         (0x2801)   /* SPIB Flag register */
#define SPISTATB        (0x2802)   /* SPIB Status register */
#define TXSPIB          (0x2803)   /* SPIB transmit data register */
#define RXSPIB          (0x2804)   /* SPIB receive data register */
#define SPIBAUDB        (0x2805)   /* SPIB baud setup register */
#define RXSPIB_SHADOW   (0x2806)   /* SPIB receive data shadow register */
#define IISPIB          (0x2880)   /* Internal memory DMA address */
#define IMSPIB          (0x2881)   /* Internal memory DMA access modifier */
#define CSPIB           (0x2882)   /* Contains number of DMA transfers remaining */
#define CPSPIB          (0x2883)   /* Points to next DMA parameters */
#define SPIDMACB        (0x2884)   /* SPIB DMA control register */
#define SPINTSTB       	(0x2885)   /* apps based change */
/* Timer Registers */
#define TMSTAT      (0x1400)   /* GP Timer Status Register */
                               /* TMxSTAT all address the same register (TMSTAT) */
#define TM0STAT     (0x1400)   /* GP Timer 0 Status register */
#define TM0CTL      (0x1401)   /* GP Timer 0 Control register */
#define TM0CNT      (0x1402)   /* GP Timer 0 Count register */
#define TM0PRD      (0x1403)   /* GP Timer 0 Period register */
#define TM0W        (0x1404)   /* GP Timer 0 Width register */
#define TM1STAT     (0x1408)   /* GP Timer 1 Status register */
#define TM1CTL      (0x1409)   /* GP Timer 1 Control register */
#define TM1CNT      (0x140a)   /* GP Timer 1 Count register */
#define TM1PRD      (0x140b)   /* GP Timer 1 Period register */
#define TM1W        (0x140c)   /* GP Timer 1 Width register */

/* POWER MGT Registers */
#define PMCTL       (0x2000)   /* Power management control register */
#define PMCTL1      (0x2001)   /* Extended Power management control register */
#define ROMID       (0x20FF)
#define RUNRSTCTL   (0x2100)

/* Peripheral Interrupt priority control register */
#define PICR0       (0x2200)
#define PICR1       (0x2201)
#define PICR2       (0x2202)
#define PICR3       (0x2203)

/* DAI Registers */

/* DMA Parameter Registers */
#define IDP_DMA_I0  (0x2400)   /* IDP DMA Channel 0 Index Register */
#define IDP_DMA_I1  (0x2401)   /* IDP DMA Channel 1 Index Register */
#define IDP_DMA_I2  (0x2402)   /* IDP DMA Channel 2 Index Register */
#define IDP_DMA_I3  (0x2403)   /* IDP DMA Channel 3 Index Register */
#define IDP_DMA_I4  (0x2404)   /* IDP DMA Channel 4 Index Register */
#define IDP_DMA_I5  (0x2405)   /* IDP DMA Channel 5 Index Register */
#define IDP_DMA_I6  (0x2406)   /* IDP DMA Channel 6 Index Register */
#define IDP_DMA_I7  (0x2407)   /* IDP DMA Channel 7 Index Register */
#define IDP_DMA_I0A (0x2408)   /* IDP DMA Channel 0 Index A Register for Ping Pong DMA */
#define IDP_DMA_I1A (0x2409)   /* IDP DMA Channel 1 Index A Register for Ping Pong DMA */
#define IDP_DMA_I2A (0x240a)   /* IDP DMA Channel 2 Index A Register for Ping Pong DMA */
#define IDP_DMA_I3A (0x240b)   /* IDP DMA Channel 3 Index A Register for Ping Pong DMA */
#define IDP_DMA_I4A (0x240c)   /* IDP DMA Channel 4 Index A Register for Ping Pong DMA */
#define IDP_DMA_I5A (0x240d)   /* IDP DMA Channel 5 Index A Register for Ping Pong DMA */
#define IDP_DMA_I6A (0x240e)   /* IDP DMA Channel 6 Index A Register for Ping Pong DMA */
#define IDP_DMA_I7A (0x240f)   /* IDP DMA Channel 7 Index A Register for Ping Pong DMA */
#define IDP_DMA_I0B (0x2418)   /* IDP DMA Channel 0 Index B Register for Ping Pong DMA */
#define IDP_DMA_I1B (0x2419)   /* IDP DMA Channel 1 Index B Register for Ping Pong DMA */
#define IDP_DMA_I2B (0x241a)   /* IDP DMA Channel 2 Index B Register for Ping Pong DMA */
#define IDP_DMA_I3B (0x241b)   /* IDP DMA Channel 3 Index B Register for Ping Pong DMA */
#define IDP_DMA_I4B (0x241c)   /* IDP DMA Channel 4 Index B Register for Ping Pong DMA */
#define IDP_DMA_I5B (0x241d)   /* IDP DMA Channel 5 Index B Register for Ping Pong DMA */
#define IDP_DMA_I6B (0x241e)   /* IDP DMA Channel 6 Index B Register for Ping Pong DMA */
#define IDP_DMA_I7B (0x241f)   /* IDP DMA Channel 7 Index B Register for Ping Pong DMA */
#define IDP_DMA_M0  (0x2410)   /* IDP DMA Channel 0 Modify Register */
#define IDP_DMA_M1  (0x2411)   /* IDP DMA Channel 1 Modify Register */
#define IDP_DMA_M2  (0x2412)   /* IDP DMA Channel 2 Modify Register */
#define IDP_DMA_M3  (0x2413)   /* IDP DMA Channel 3 Modify Register */
#define IDP_DMA_M4  (0x2414)   /* IDP DMA Channel 4 Modify Register */
#define IDP_DMA_M5  (0x2415)   /* IDP DMA Channel 5 Modify Register */
#define IDP_DMA_M6  (0x2416)   /* IDP DMA Channel 6 Modify Register */
#define IDP_DMA_M7  (0x2417)   /* IDP DMA Channel 7 Modify Register */
#define IDP_DMA_C0  (0x2420)   /* IDP DMA Channel 0 Count Register */
#define IDP_DMA_C1  (0x2421)   /* IDP DMA Channel 1 Count Register */
#define IDP_DMA_C2  (0x2422)   /* IDP DMA Channel 2 Count Register */
#define IDP_DMA_C3  (0x2423)   /* IDP DMA Channel 3 Count Register */
#define IDP_DMA_C4  (0x2424)   /* IDP DMA Channel 4 Count Register */
#define IDP_DMA_C5  (0x2425)   /* IDP DMA Channel 5 Count Register */
#define IDP_DMA_C6  (0x2426)   /* IDP DMA Channel 6 Count Register */
#define IDP_DMA_C7  (0x2427)   /* IDP DMA Channel 7 Count Register */
#define IDP_DMA_PC0 (0x2428)   /* IDP DMA Channel 0 Ping Pong Count Register */
#define IDP_DMA_PC1 (0x2429)   /* IDP DMA Channel 1 Ping Pong Count Register */
#define IDP_DMA_PC2 (0x242a)   /* IDP DMA Channel 2 Ping Pong Count Register */
#define IDP_DMA_PC3 (0x242b)   /* IDP DMA Channel 3 Ping Pong Count Register */
#define IDP_DMA_PC4 (0x242c)   /* IDP DMA Channel 4 Ping Pong Count Register */
#define IDP_DMA_PC5 (0x242d)   /* IDP DMA Channel 5 Ping Pong Count Register */
#define IDP_DMA_PC6 (0x242e)   /* IDP DMA Channel 6 Ping Pong Count Register */
#define IDP_DMA_PC7 (0x242f)   /* IDP DMA Channel 7 Ping Pong Count Register */

/* SRU Registers */
#define SRU_CLK0    (0x2430)   /* SRU Clock Control Register 0 */
#define SRU_CLK1    (0x2431)   /* SRU Clock Control Register 1 */
#define SRU_CLK2    (0x2432)   /* SRU Clock Control Register 2 */
#define SRU_CLK3    (0x2433)   /* SRU Clock Control Register 3 */
#define SRU_CLK4    (0x2434)   /* SRU Clock Control Register 4 */
#define SRU_CLK5    (0x2435)   /* SRU Clock Control Register 4 */

#define SRU_DAT0    (0x2440)   /* SRU Data Control Register 0 */
#define SRU_DAT1    (0x2441)   /* SRU Data Control Register 1 */
#define SRU_DAT2    (0x2442)   /* SRU Data Control Register 2 */
#define SRU_DAT3    (0x2443)   /* SRU Data Control Register 3 */
#define SRU_DAT4    (0x2444)   /* SRU Data Control Register 4 */
#define SRU_DAT5    (0x2445)   /* SRU Data Control Register 5 */
#define SRU_DAT6    (0x2446)   /* SRU Data Control Register 5 */

#define SRU_FS0     (0x2450)   /* SRU FS Control Register 0 */
#define SRU_FS1     (0x2451)   /* SRU FS Control Register 1 */
#define SRU_FS2     (0x2452)   /* SRU FS Control Register 2 */
#define SRU_FS3     (0x2453)   /* SRU FS Control Register 3 */
#define SRU_FS4     (0x2454)   /* SRU FS Control Register 3 */

#define SRU_PIN0    (0x2460)   /* SRU Pin Control Register 0 */
#define SRU_PIN1    (0x2461)   /* SRU Pin Control Register 1 */
#define SRU_PIN2    (0x2462)   /* SRU Pin Control Register 2 */
#define SRU_PIN3    (0x2463)   /* SRU Pin Control Register 3 */
#define SRU_PIN4    (0x2464)   /* SRU Pin Control Register 4 */

#define SRU_EXT_MISCA   (0x2470)   /* SRU External Misc. A Control Register */
#define SRU_EXT_MISCB   (0x2471)   /* SRU External Misc. B Control Register */

#define SRU_PBEN0  (0x2478)   /* SRU Pin Enable Register 0 */
#define SRU_PBEN1  (0x2479)   /* SRU Pin Enable Register 1 */
#define SRU_PBEN2  (0x247A)   /* SRU Pin Enable Register 2 */
#define SRU_PBEN3  (0x247B)   /* SRU Pin Enable Register 3 */

#define DAI_IRPTL_FE    (0x2480)   /* DAI Falling Edge Interrupt Latch Register */
#define DAI_IMASK_FE    (0x2480)   /* DAI Falling Edge Interrupt Latch Register */
#define DAI_IRPTL_RE    (0x2481)   /* DAI Rising Edge Interrupt Latch Register */
#define DAI_IMASK_RE    (0x2481)   /* DAI Rising Edge Interrupt Latch Register */
#define DAI_IRPTL_PRI   (0x2484)   /* DAI Interrupt Priority Register */
#define DAI_IRPTL_H     (0x2488)   /* DAI High Priority Interrupt Latch Register */
#define DAI_IRPTL_L     (0x2489)   /* DAI Low Priority Interrupt Latch Register */
#define DAI_IRPTL_HS    (0x248C)   /* Shadow DAI High Priority Interrupt Latch Register */
#define DAI_IRPTL_LS    (0x248D)   /* Shadow DAI Low Priority Interrupt Latch Register */


/* Sample Rate Converter registers */
#define SRCCTL0     (0x2490)   /* SRC0 Control register */
#define SRCCTL1     (0x2491)   /* SRC1 Control register */
#define SRCMUTE     (0x2492)   /* SRC Mute register */
#define SRCRAT0     (0x2498)   /* SRC0 Output to Input Ratio */
#define SRCRAT1     (0x2499)   /* SRC1 Output to Input Ratio */


#define IDP_CTL     (0x24B0)   /* IDP Control Register 0 */
#define IDP_CTL0    (0x24B0)   /* IDP Control Register 0 */
#define IDP_PP_CTL  (0x24B1)   /* IDP Parallel Port Control Register */
#define IDP_CTL1    (0x24B2)   /* IDP Control Register 1 */
#define IDP_CTL2    (0x24B3)   /* IDP Control Register 2 */

#define DAI_STAT     (0x24B8)   /* DAI Status Register */
#define DAI_STAT0    (0x24B8)   /* DAI Status Register */
#define DAI_PIN_STAT (0x24B9)   /* DAI Pin Buffer Status Register */
#define DAI_STAT1    (0x24BA)   /* DAI Status Register */

#define IDP_FIFO    (0x24D0)   /* IDP FIFO packing mode register */

/* DPI */
#define SRU2_INPUT0             (0x1C00)
#define SRU2_INPUT1             (0x1C01)
#define SRU2_INPUT2             (0x1C02)
#define SRU2_INPUT3             (0x1C03)
#define SRU2_INPUT4             (0x1C04)
#define SRU2_INPUT5             (0x1C05)
#define SRU2_PIN0               (0x1C10)
#define SRU2_PIN1               (0x1C11)
#define SRU2_PIN2               (0x1C12)
#define SRU2_PBEN0              (0x1C20)
#define SRU2_PBEN1              (0x1C21)
#define SRU2_PBEN2              (0x1C22)

#define DPI_PIN_STAT            (0x1C31)
#define DPI_IRPTL               (0x1C32)
#define DPI_IRPTL_SH            (0x1C33)
#define DPI_IRPTL_FE            (0x1C34)
#define DPI_IMASK_FE            (0x1C34)
#define DPI_IRPTL_RE            (0x1C35)
#define DPI_IMASK_RE            (0x1C35)

/* DTCP module */

#if defined(__ADSP21462__) || defined(__ADSP21465__)

#define DTCPCTL     (0x2C00)
#define DTCP_M0     (0x2C0E)
#define DTCP_M1     (0x2C0F)
#define DTCP_I0     (0x2C10)
#define DTCP_I1     (0x2C11)
#define DTCP_C0     (0x2C16)
#define DTCP_C1     (0x2C17)

#define M6DR        (0x2C20)
#define M6KHR       (0x2C21)
#define M6KLR       (0x2C22)
#define M6CR        (0x2C23)
#define M6ESR       (0x2C24)
#define M6SR        (0x2C25)

#endif

/* PCG */
#define PCG_CTLA0    (0x24C0)  /* Precision Clock A Control Register 0 */
#define PCG_CTLA1    (0x24C1)  /* Precision Clock A Control Register 1 */
#define PCG_CTLB0    (0x24C2)  /* Precision Clock B Control Register 0 */
#define PCG_CTLB1    (0x24C3)  /* Precision Clock B Control Register 1 */
#define PCG_PW       (0x24C4)  /* Precision Clock Pulse Width Control Register */
#define PCG_SYNC     (0x24C5)  /* Precision Clock Frame Sync Synchronization */
#define PCG_PW1      (0x24C4)  /* Precision Clock Pulse Width Control Register */
#define PCG_SYNC1    (0x24C5)  /* Precision Clock Frame Sync Synchronization */

#define PCG_CTLC0    (0x24C6)  /* Precision Clock A Control Register 0 */
#define PCG_CTLC1    (0x24C7)  /* Precision Clock A Control Register 1 */
#define PCG_CTLD0    (0x24C8)  /* Precision Clock B Control Register 0 */
#define PCG_CTLD1    (0x24C9)  /* Precision Clock B Control Register 1 */
#define PCG_PW2      (0x24CA)  /* Precision Clock Pulse Width Control Register */
#define PCG_SYNC2    (0x24CB)  /* Precision Clock Frame Sync Synchronization */


/* MTM registers */
#define MTMCTL (0x2c01)   /* Memory-to-Memory DMA Control Register */
#define IIMTMW (0x2c10)   /* MTM DMA Destination Index Register */
#define IIMTMR (0x2c11)   /* MTM DMA Source Index Register */
#define IMMTMW (0x2c0e)   /* MTM DMA Destination Modify Register */
#define IMMTMR (0x2c0f)   /* MTM DMA Source Modigy Register */
#define CMTMW  (0x2c16)   /* MTM DMA Destination Count Register */
#define CMTMR  (0x2c17)   /* MTM DMA Source Count Register */

/* PWM */
#define PWMGCTL     (0x3800)   /* PWM Global Control Register */
#define PWMGSTAT    (0x3801)   /* PWM Global Status Register */

#define PWMCTL0     (0x3000)   /* PWM Control */
#define PWMSTAT0    (0x3001)   /* PWM Status */
#define PWMPERIOD0  (0x3002)   /* PWM Period Register */
#define PWMDT0      (0x3003)   /* PWM Dead Time Register */
#define PWMA0       (0x3005)   /* PWM Channel A Duty Control */
#define PWMB0       (0x3006)   /* PWM Channel B Duty Control */
#define PWMSEG0     (0x3008)   /* PWM Output Enable */
#define PWMAL0      (0x300A)   /* PWM Channel AL Duty Control */
#define PWMBL0      (0x300B)   /* PWM Channel BL Duty Control */
#define PWMDBG0     (0x300E)   /* PWM Debug Status */
#define PWMPOL0     (0x300F)   /* PWM Output polarity select */

#define PWMCTL1     (0x3010)   /* PWM Control */
#define PWMSTAT1    (0x3011)   /* PWM Status */
#define PWMPERIOD1  (0x3012)   /* PWM Period Register */
#define PWMDT1      (0x3013)   /* PWM Dead Time Register */
#define PWMA1       (0x3015)   /* PWM Channel A Duty Control */
#define PWMB1       (0x3016)   /* PWM Channel B Duty Control */
#define PWMSEG1     (0x3018)   /* PWM Output Enable */
#define PWMAL1      (0x301A)   /* PWM Channel AL Duty Control */
#define PWMBL1      (0x301B)   /* PWM Channel BL Duty Control */
#define PWMDBG1     (0x301E)   /* PWM Debug Status */
#define PWMPOL1     (0x301F)   /* PWM Output polarity select */

#define PWMCTL2     (0x3400)   /* PWM Control */
#define PWMSTAT2    (0x3401)   /* PWM Status */
#define PWMPERIOD2  (0x3402)   /* PWM Period Register */
#define PWMDT2      (0x3403)   /* PWM Dead Time Register */
#define PWMA2       (0x3405)   /* PWM Channel A Duty Control */
#define PWMB2       (0x3406)   /* PWM Channel B Duty Control */
#define PWMSEG2     (0x3408)   /* PWM Output Enable */
#define PWMAL2      (0x340A)   /* PWM Channel AL Duty Control */
#define PWMBL2      (0x340B)   /* PWM Channel BL Duty Control */
#define PWMDBG2     (0x340E)   /* PWM Debug Status */
#define PWMPOL2     (0x340F)   /* PWM Output polarity select */

#define PWMCTL3     (0x3410)   /* PWM Control */
#define PWMSTAT3    (0x3411)   /* PWM Status */
#define PWMPERIOD3  (0x3412)   /* PWM Period Register */
#define PWMDT3      (0x3413)   /* PWM Dead Time Register */
#define PWMA3       (0x3415)   /* PWM Channel A Duty Control */
#define PWMB3       (0x3416)   /* PWM Channel B Duty Control */
#define PWMSEG3     (0x3418)   /* PWM Output Enable */
#define PWMAL3      (0x341A)   /* PWM Channel AL Duty Control */
#define PWMBL3      (0x341B)   /* PWM Channel BL Duty Control */
#define PWMDBG3     (0x341E)   /* PWM Debug Status */
#define PWMPOL3     (0x341F)   /* PWM Output polarity select */

/* Registers for UART0 */
#define UART0THR                (0x3c00)   /* Transmit Holding Register */
#define UART0RBR                (0x3c00)   /* Receive Buffer Register */
#define UART0DLL                (0x3c00)   /* Divisor Latch Low Byte */
#define UART0IER                (0x3c01)   /* Interrupt Enable Register */
#define UART0DLH                (0x3c01)   /* Divisor Latch High Byte */
#define UART0IIR                (0x3c02)   /* Interrupt Identification Register */
#define UART0LCR                (0x3c03)   /* Line Control Register */
#define UART0MODE               (0x3c04)   /* Mode Register */
#define UART0LSR                (0x3c05)   /* Line Status Register */
#define UART0SCR                (0x3c07)   /* Scratch Register */
#define UART0RBRSH              (0x3c08)   /* Read Buffer Shadow Register */
#define UART0IIRSH              (0x3c09)   /* Interrupt Identification Shadow Register */
#define UART0LSRSH              (0x3c0a)   /* Line Status Shadow Register */

#define IIUART0RX               (0x3e00)   /* Internal Memory address for DMA access with UART Receiver */
#define IMUART0RX               (0x3e01)   /* Internal Memory modifier for DMA access with UART Receiver */
#define CUART0RX                (0x3e02)   /* Word Count for DMA access with UART Receiver */
#define CPUART0RX               (0x3e03)   /* Chain Point for DMA access with UART Receiver */
#define UART0RXCTL              (0x3e04)   /* UART Receiver control register */
#define UART0RXSTAT             (0x3e05)   /* UART Receiver status register */

#define IIUART0TX               (0x3f00)   /* Internal Memory address for DMA access with UART Transmitter */
#define IMUART0TX               (0x3f01)   /* Internal Memory modifier for DMA access with UART Transmitter */
#define CUART0TX                (0x3f02)   /* Word Count for DMA access with UART Transmitter */
#define CPUART0TX               (0x3f03)   /* Chain Point for DMA access with UART Transmitter */
#define UART0TXCTL              (0x3f04)   /* UART Transmitter control register */
#define UART0TXSTAT             (0x3f05)   /* UART Transmitter status register */


/* TWI Registers */
#define TWIDIV                  (0x4400) /* TWI Serial Clock Divider */
#define TWIMITR                 (0x4404) /* TWI Master Internal Time Reference */
#define TWISCTL                 (0x4408) /* TWI Slave Mode Control Register */
#define TWISSTAT                (0x440C) /* TWI Slave Mode Status Register */
#define TWISADDR                (0x4410) /* TWI Slave Mode Address Comparison Register */
#define TWIMCTL                 (0x4414) /* TWI Master Mode Control Register */
#define TWIMSTAT                (0x4418) /* TWI Master Mode Status Register */
#define TWIMADDR                (0x441C) /* TWI Master Mode Address Register */
#define TWIIRPTL                (0x4420) /* TWI Interrupt Latch Register */
#define TWIIMASK                (0x4424) /* TwI Interrupt Mask Register */
#define TWIFIFOCTL              (0x4428) /* TWI FIFO Control Register */
#define TWIFIFOSTAT             (0x442C) /* TWI FIFO Status Register */
#define TXTWI8                  (0x4480) /* TWI 8-Bit Transmit FIFO Register */
#define TXTWI16                 (0x4484) /* TWI 16-Bit Transmit FIFO Register */
#define RXTWI8                  (0x4488) /* TWI 8-Bit Receive FIFO Register */
#define RXTWI16                 (0x448C) /* TWI 16-Bit Receive FIFO Register */


/* DDR2 controller registers */

#define DDR2CTL0            (0x1812)
#define DDR2CTL1            (0x1813)
#define DDR2CTL2            (0x1814)
#define DDR2CTL3            (0x1815)
#define DDR2CTL4            (0x1816)
#define DDR2CTL5            (0x1817)
#define DDR2RRC          (0x181D)
#define DDR2STAT0           (0x181E)
#define DDR2STAT1        (0x1840)   /* DDR2 bank active/idle status */
#define DDR2_DTAP        (0x181F)
#define DDR2PADCTL0         (0x1841)   /* Phoenix DDR2 Data, DQS, CLK, DDR2-pad control register */
#define DDR2PADCTL1         (0x1842)   /* Phoenix DDR2 Address,control, DDR2-pad control register */
#define DLL0CTL0                         (0x1850)
#define DLL0CTL1                         (0x1851)
#define DLL1CTL0                         (0x1855)
#define DLL1CTL1                         (0x1856)
#define DLL0STAT0                        (0x1853)
#define DLL0STAT1                        (0x1854)
#define DLL1STAT0                        (0x1858)
#define DLL1STAT1                        (0x1859)

/* FIR accelerator registers */

#define FIRCTL1					(0x5000)   /* FIR global control register */
#define FIRDMASTAT				(0x5001)   /* DMA status reg */
#define FIRMACSTAT				(0x5002)   /*/MAC status register */
#define FIRDEBUGCTL				(0x5004)   /* Debug control register*/
#define FIRDBGADDR				(0x5005)   /* Debug addres register*/
#define FIRDBGWRDATA			(0x5006)   /* Debug data write register*/
#define FIRDBGRDDATA			(0x5007)  /*  Debug data read register */
#define FIRCTL2					(0x5010)   /* Channel Control register */
#define IIFIR					(0x5011)   /* Input data Index register */
#define IMFIR					(0x5012)   /* Input data modifier register */
#define ICFIR					(0x5013)   /* Input data count/buffer length register */
#define IBFIR					(0x5014)   /* Input data base register */
#define OIFIR					(0x5015)   /* Output data Index register */
#define OMFIR					(0x5016)   /* Output data modifier register */
#define OCFIR					(0x5017)   /* Output data count/buffer length register */
#define OBFIR					(0x5018)   /* Output data base register */
#define CIFIR					(0x5019)   /* Coefficient Index register */
#define CMFIR					(0x501A)   /* Coefficient modifier register */
#define CCFIR					(0x501B)   /* Coefficient count/buffer length register */
#define CPFIR					(0x501C)   /* Chain Pointer Register */


/* IIR accelerator registers */


#define IIRCTL1					(0x5200)   /* IIR global control register */
#define IIRDMASTAT				(0x5201)   /* DMA status reg */
#define IIRMACSTAT				(0x5202)   /* MAC status register */
#define IIRDEBUGCTL				(0x5203)   /* IIR Debug Control register */
#define IIRDBGADDR				(0x5204)  /* IIR debug address register */
#define IIRDBGWRDATA_L			(0x5205)  /* Debug data Write register LS 32 bits */
#define IIRDBGWRDATA_H			(0x5206)  /* Debug data Write register MS 8 bits */
#define IIRDBGRDDATA_L			(0x5207)  /* Debug data Read register  LS 32 bits */
#define IIRDBGRDDATA_H			(0x5208)  /* Debug data Read register  MS 8 bits */
#define IIRCTL2					(0x5210)   /* Channel Control register */
#define IIIIR					(0x5211)   /* Input data Index register */
#define IMIIR					(0x5212)   /* Input data modifier register */
#define ICIIR					(0x5213)   /* Input data count/buffer length register */
#define IBIIR					(0x5214)   /* Input data base register */
#define OIIIR					(0x5215)   /* Output data Index register */
#define OMIIR					(0x5216)   /* Output data modifier register */
#define OCIIR					(0x5217)   /* Output data count/buffer length register */
#define OBIIR					(0x5218)   /* Output data base register */
#define CIIIR					(0x5219)   /* Coefficient Index register */
#define CMIIR					(0x521A)   /* Coefficient modifier register */
#define CCIIR					(0x521B)   /* Coefficient count/buffer length register */
#define CPIIR					(0x521C)   /* Chain Pointer Register */


/* FFT accelerator registers */

#define FFTCTL1                 (0x5300)    /* global control register */
#define FFTMACSTAT              (0x5302)    /* multiplier status register */
#define FFTDADDR                (0x5303)    /* debug address register */
#define FFTDDATA                (0x5304)    /* debug data register */
#define FFTCTL2                 (0x530C)    /* mode control register */
#define IIFFT                   (0x5310)    /* input index register */
#define IMFFT                   (0x5311)    /* input modifier register */
#define ICFFT                   (0x5312)    /* input count register */
#define ILFFT                   (0x5313)    /* input data length register */
#define IBFFT                   (0x5314)    /* input data Base register */
#define CPIFFT                  (0x5315)    /* input Chain pointer register */
#define OIFFT                   (0x5318)    /* Output Index register */
#define OMFFT                   (0x5319)    /* Output Modify register */
#define OCFFT                   (0x531A)    /* Output Count register */
#define OLFFT                   (0x531B)    /* Output data length registr*/
#define OBFFT                   (0x531C)    /* Output data base register */
#define CPOFFT                  (0x531D)    /* Output chain pointer register */
#define FFTDMASTAT              (0x5320)    /* DMA status register */
#define FFTSHDMASTAT            (0x5321)    /* Shadow DMA status register */

 


/* link ports registers */
/* link port 0 registers */
#define LCTL0					(0x4c00) 
#define LSTAT0					(0x4c01)
#define LSTAT0_SHADOW			(0x4c03)
#define TXLB0					(0x4c08)
#define TXLB0_IN_SHADOW			(0x4c0d)
#define TXLB0_OUT_SHADOW		(0x4c0c)
#define RXLB0					(0x4c09)
#define RXLB0_IN_SHADOW			(0x4c0b)
#define RXLB0_OUT_SHADOW		(0x4c0a)
#define IILB0   				(0x4c18)	   /* Internal index memory register */
#define IMLB0   				(0x4c19)	   /* Internal memory  access modifier */
#define CLB0    				(0x4c1a)	   /* Contains number of DMA transfers remaining */
#define CPLB0   				(0x4c1b)	   /* Points to next DMA parameters */


/* link port 1 registers */
#define LCTL1					(0x4c20) 
#define LSTAT1					(0x4c21)
#define LSTAT1_SHADOW			(0x4c23)
#define TXLB1					(0x4c28)
#define TXLB1_IN_SHADOW			(0x4c2d)
#define TXLB1_OUT_SHADOW		(0x4c2c)
#define RXLB1					(0x4c29)
#define RXLB1_IN_SHADOW			(0x4c2b)
#define RXLB1_OUT_SHADOW		(0x4c2a)
#define IILB1   				(0x4c38)	   /* Internal index memory register */
#define IMLB1   				(0x4c39)	   /* Internal memory  access modifier */
#define CLB1    				(0x4c3a)	   /* Contains number of DMA transfers remaining */
#define CPLB1   				(0x4c3b)	   /* Points to next DMA parameters */



#if !defined(__ADSP21467__)

/* MLB device registers */


#define MLB_DCCR				(0x4100)   /* MLB Control register */
#define MLB_SSCR				(0x4101)   /* MLB System Status Configuration Register */
#define MLB_SDCR				(0x4102)   /* MLB System Data configuration Register */
#define MLB_SMCR				(0x4103)   /* MLB System Interrupt Mask Register */
#define MLB_VCCR				(0x4107)   /* MLB Version Control Configuration Register(contains the IP version) */
#define MLB_SBCR				(0x4108)   /* MLB Synchronous Base Address Register */
#define MLB_ABCR				(0x4109)   /* MLB Asynchronous Base Address Register */
#define MLB_CBCR				(0x410A)   /* MLB Control Base Address Register */
#define MLB_IBCR				(0x410B)   /* MLB Isochronous Base Address Register */
#define MLB_CICR				(0x410C)   /* MLB Channel Interrupt Status Register */
#define MLB_CECR0				(0x4110)   /* MLB Channel 0 Control Register */
#define MLB_CSCR0				(0x4111)   /* MLB Channel 0 Status Register */
#define MLB_CCBCR0				(0x4112)   /* MLB Channel 0 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR0				(0x4113)   /* MLB Channel 0 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR0				(0x41A0)   /* MLB Channel 0 Local Channel Buffer Control Register */
#define MLB_CECR1				(0x4114)   /* MLB Channel 1 Control Register */
#define MLB_CSCR1				(0x4115)   /* MLB Channel 1 Status Register */
#define MLB_CCBCR1				(0x4116)   /* MLB Channel 1 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR1				(0x4117)   /* MLB Channel 1 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR1				(0x41A1)   /* MLB Channel 1 Local Channel Buffer Control Register */
#define MLB_CECR2				(0x4118)   /* MLB Channel 2 Control Register */
#define MLB_CSCR2				(0x4119)   /* MLB Channel 2 Status Register */
#define MLB_CCBCR2				(0x411A)   /* MLB Channel 2 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR2				(0x411B)   /* MLB Channel 2 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR2				(0x41A2)   /* MLB Channel 2 Local Channel Buffer Control Register */
#define MLB_CECR3				(0x411C)   /* MLB Channel 3 Control Register  */
#define MLB_CSCR3				(0x411D)   /* MLB Channel 3 Status Register */
#define MLB_CCBCR3				(0x411E)   /* MLB Channel 3 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR3				(0x411F)   /* MLB Channel 3 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR3				(0x41A3)   /* MLB Channel 3 Local Channel Buffer Control Register */
#define MLB_CECR4				(0x4120)   /* MLB Channel 4 Control Register */
#define MLB_CSCR4				(0x4121)   /* MLB Channel 4 Status Register */
#define MLB_CCBCR4				(0x4122)   /* MLB Channel 4 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR4				(0x4123)   /* MLB Channel 4 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR4				(0x41A4)   /* MLB Channel 4 Local Channel Buffer Control Register */
#define MLB_CECR5				(0x4124)   /* MLB Channel 5 Control Register */
#define MLB_CSCR5				(0x4125)   /* MLB Channel 5 Status Register */
#define MLB_CCBCR5				(0x4126)   /* MLB Channel 5 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR5				(0x4127)   /* MLB Channel 5 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR5				(0x41A5)   /* MLB Channel 5 Local Channel Buffer Control Register */
#define MLB_CECR6				(0x4128)   /* MLB Channel 6 Control Register */
#define MLB_CSCR6				(0x4129)   /* MLB Channel 6 Status Register */
#define MLB_CCBCR6				(0x412A)   /* MLB Channel 6 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR6				(0x412B)   /* MLB Channel 6 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR6				(0x41A6)   /* MLB Channel 6 Local Channel Buffer Control Register */
#define MLB_CECR7				(0x412C)   /* MLB Channel 7 Control Register */
#define MLB_CSCR7				(0x412D)   /* MLB Channel 7 Status Register */
#define MLB_CCBCR7				(0x412E)   /* MLB Channel 7 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR7				(0x412F)   /* MLB Channel 7 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR7				(0x41A7)   /* MLB Channel 7 Local Channel Buffer Control Register */
#define MLB_CECR8				(0x4130)   /* MLB Channel 8 Control Register */
#define MLB_CSCR8				(0x4131)   /* MLB Channel 8 Status Register */
#define MLB_CCBCR8				(0x4132)   /* MLB Channel 8 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR8				(0x4133)   /* MLB Channel 8 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR8				(0x41A8)   /* MLB Channel 8 Local Channel Buffer Control Register */
#define MLB_CECR9				(0x4134)   /* MLB Channel 9 Control Register */
#define MLB_CSCR9				(0x4135)   /* MLB Channel 9 Status Register */
#define MLB_CCBCR9				(0x4136)   /* MLB Channel 9 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR9				(0x4137)   /* MLB Channel 9 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR9				(0x41A9)   /* MLB Channel 9 Local Channel Buffer Control Register */
#define MLB_CECR10				(0x4138)   /* MLB Channel 10  Control Register */
#define MLB_CSCR10				(0x4139)   /* MLB Channel 10  Status Register */
#define MLB_CCBCR10				(0x413A)   /* MLB Channel 10  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR10				(0x413B)   /* MLB Channel 10  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR10				(0x41AA)   /* MLB Channel 10  Local Channel Buffer Control Register */
#define MLB_CECR11				(0x413C)   /* MLB Channel 11  Control Register */
#define MLB_CSCR11				(0x413D)   /* MLB Channel 11  Status Register */
#define MLB_CCBCR11				(0x413E)   /* MLB Channel 11  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR11				(0x413F)   /* MLB Channel 11  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR11				(0x41AB)   /* MLB Channel 11  Local Channel Buffer Control Register */
#define MLB_CECR12				(0x4140)   /* MLB Channel 12  Control Register */
#define MLB_CSCR12				(0x4141)   /* MLB Channel 12  Status Register */
#define MLB_CCBCR12				(0x4142)   /* MLB Channel 12  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR12				(0x4143)   /* MLB Channel 12  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR12				(0x41AC)   /* MLB Channel 12  Local Channel Buffer Control Register */
#define MLB_CECR13				(0x4144)   /* MLB Channel 13  Control Register */
#define MLB_CSCR13				(0x4145)   /* MLB Channel 13  Status Register */
#define MLB_CCBCR13				(0x4146)   /* MLB Channel 13  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR13				(0x4147)   /* MLB Channel 13  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR13				(0x41AD)   /* MLB Channel 13  Local Channel Buffer Control Register */
#define MLB_CECR14				(0x4148)   /* MLB Channel 14  Control Register */
#define MLB_CSCR14				(0x4149)   /* MLB Channel 14  Status Register */
#define MLB_CCBCR14				(0x414A)   /* MLB Channel 14  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR14				(0x414B)   /* MLB Channel 14  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR14				(0x41AE)   /* MLB Channel 14  Local Channel Buffer Control Register */
#define MLB_CECR15				(0x414C)   /* MLB Channel 15  Control Register */
#define MLB_CSCR15				(0x414D)   /* MLB Channel 15  Status Register */
#define MLB_CCBCR15				(0x414E)   /* MLB Channel 15  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR15				(0x414F)   /* MLB Channel 15  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR15				(0x41AF)   /* MLB Channel 15  Local Channel Buffer Control Register */
#define MLB_CECR16				(0x4150)   /* MLB Channel 16  Control Register */
#define MLB_CSCR16				(0x4151)   /* MLB Channel 16  Status Register */
#define MLB_CCBCR16				(0x4152)   /* MLB Channel 16  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR16				(0x4153)   /* MLB Channel 16  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR16				(0x41B0)   /* MLB Channel 16  Local Channel Buffer Control Register */
#define MLB_CECR17				(0x4154)   /* MLB Channel 17  Control Register */
#define MLB_CSCR17				(0x4155)   /* MLB Channel 17  Status Register */
#define MLB_CCBCR17				(0x4156)   /* MLB Channel 17  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR17				(0x4157)   /* MLB Channel 17  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR17				(0x41B1)   /* MLB Channel 17  Local Channel Buffer Control Register */
#define MLB_CECR18				(0x4158)   /* MLB Channel 18  Control Register */
#define MLB_CSCR18				(0x4159)   /* MLB Channel 18  Status Register */
#define MLB_CCBCR18				(0x415A)   /* MLB Channel 18  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR18				(0x415B)   /* MLB Channel 18  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR18				(0x41B2)   /* MLB Channel 18  Local Channel Buffer Control Register */
#define MLB_CECR19				(0x415C)   /* MLB Channel 19 Control Register */
#define MLB_CSCR19				(0x415D)   /* MLB Channel 19 Status Register */
#define MLB_CCBCR19				(0x415E)   /* MLB Channel 19 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR19				(0x415F)   /* MLB Channel 19 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR19				(0x41B3)   /* MLB Channel 19 Local Channel Buffer Control Register */
#define MLB_CECR20				(0x4160)   /* MLB Channel 20  Control Register */
#define MLB_CSCR20				(0x4161)   /* MLB Channel 20  Status Register */
#define MLB_CCBCR20				(0x4162)   /* MLB Channel 20  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR20				(0x4163)   /* MLB Channel 20  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR20				(0x41B4)   /* MLB Channel 20  Local Channel Buffer Control Register */
#define MLB_CECR21				(0x4164)   /* MLB Channel 21  Control Register */
#define MLB_CSCR21				(0x4165)   /* MLB Channel 21  Status Register */
#define MLB_CCBCR21				(0x4166)   /* MLB Channel 21  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR21				(0x4167)   /* MLB Channel 21  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR21				(0x41B5)   /* MLB Channel 21  Local Channel Buffer Control Register */
#define MLB_CECR22				(0x4168)   /* MLB Channel 22  Control Register */
#define MLB_CSCR22				(0x4169)   /* MLB Channel 22  Status Register */
#define MLB_CCBCR22				(0x416A)   /* MLB Channel 22  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR22				(0x416B)   /* MLB Channel 22  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR22				(0x41B6)   /* MLB Channel 22  Local Channel Buffer Control Register */
#define MLB_CECR23				(0x416C)   /* MLB Channel 23  Control Register */
#define MLB_CSCR23				(0x416D)   /* MLB Channel 23  Status Register */
#define MLB_CCBCR23				(0x416E)   /* MLB Channel 23  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR23				(0x416F)   /* MLB Channel 23  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR23				(0x41B7)   /* MLB Channel 23  Local Channel Buffer Control Register */
#define MLB_CECR24				(0x4170)   /* MLB Channel 24  Control Register */
#define MLB_CSCR24				(0x4171)   /* MLB Channel 24  Status Register */
#define MLB_CCBCR24				(0x4172)   /* MLB Channel 24  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR24				(0x4173)   /* MLB Channel 24  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR24				(0x41B8)   /* MLB Channel 24  Local Channel Buffer Control Register */
#define MLB_CECR25				(0x4174)   /* MLB Channel 25  Control Register */
#define MLB_CSCR25				(0x4175)   /* MLB Channel 25  Status Register */
#define MLB_CCBCR25				(0x4176)   /* MLB Channel 25  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR25				(0x4177)   /* MLB Channel 25  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR25				(0x41B9)   /* MLB Channel 25  Local Channel Buffer Control Register */
#define MLB_CECR26				(0x4178)   /* MLB Channel 26  Control Register */
#define MLB_CSCR26	 			(0x4179)   /* MLB Channel 26  Status Register */
#define MLB_CCBCR26				(0x417A)   /* MLB Channel 26  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR26				(0x417B)   /* MLB Channel 26  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR26				(0x41BA)   /* MLB Channel 26  Local Channel Buffer Control Register */
#define MLB_CECR27				(0x417C)   /* MLB Channel 27  Control Register */
#define MLB_CSCR27				(0x417D)   /* MLB Channel 27  Status Register */
#define MLB_CCBCR27				(0x417E)   /* MLB Channel 27  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR27				(0x417F)   /* MLB Channel 27  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR27				(0x41BB)   /* MLB Channel 27  Local Channel Buffer Control Register */
#define MLB_CECR28				(0x4180)   /* MLB Channel 28  Control Register */
#define MLB_CSCR28				(0x4181)   /* MLB Channel 28  Status Register */
#define MLB_CCBCR28				(0x4182)   /* MLB Channel 28  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR28				(0x4183)   /* MLB Channel 28  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR28				(0x41BC)   /* MLB Channel 28  Local Channel Buffer Control Register */
#define MLB_CECR29				(0x4184)   /* MLB Channel 29  Control Register */
#define MLB_CSCR29				(0x4185)   /* MLB Channel 29  Status Register */
#define MLB_CCBCR29				(0x4186)   /* MLB Channel 29  Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR29				(0x4187)   /* MLB Channel 29  Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR29				(0x41BD)   /* MLB Channel 29  Local Channel Buffer Control Register */
#define MLB_CECR30				(0x4188)   /* MLB Channel 30 Control Register */
#define MLB_CSCR30				(0x4189)   /* MLB Channel 30 Status Register */
#define MLB_CCBCR30				(0x418A)   /* MLB Channel 30 Current Buffer Register(RX buffer in IO mode) */
#define MLB_CNBCR30				(0x418B)   /* MLB Channel 30 Next Buffer Register (TX buffer in IO mode) */
#define MLB_LCBCR30				(0x41BE)   /* MLB Channel 30 Local Channel Buffer Control Register */

#endif /* __ADSP21467__ */

/*============================================================================ */
/*                System Register bit definitions                              */
/*============================================================================ */
/* MODE1 and MMASK registers */
#define BR8         (BIT_0)         /* Bit  0: Bit-reverse for I8 */
#define BR0         (BIT_1)         /* Bit  1: Bit-reverse for I0 (uses DMS0- only ) */
#define SRCU        (BIT_2)         /* Bit  2: Alt. register select for comp. units */
#define SRD1H       (BIT_3)         /* Bit  3: DAG1 alt. register select (7-4) */
#define SRD1L       (BIT_4)         /* Bit  4: DAG1 alt. register select (3-0) */
#define SRD2H       (BIT_5)         /* Bit  5: DAG2 alt. register select (15-12) */
#define SRD2L       (BIT_6)         /* Bit  6: DAG2 alt. register select (11-8) */
#define SRRFH       (BIT_7)         /* Bit  7: Register file alt. select for R(15-8) */
#define SRRFL       (BIT_10)        /* Bit 10: Register file alt. select for R(7-0) */
#define NESTM       (BIT_11)        /* Bit 11: Interrupt nesting enable */
#define IRPTEN      (BIT_12)        /* Bit 12: Global interrupt enable */
#define ALUSAT      (BIT_13)        /* Bit 13: Enable ALU fixed-pt. saturation */
#define SSE         (BIT_14)        /* Bit 14: Enable short word sign extension */
#define TRUNCATE    (BIT_15)        /* Bit 15: 1=fltg-pt. truncation 0=Rnd to nearest */
#define RND32       (BIT_16)        /* Bit 16: 1=32-bit fltg-pt.rounding 0=40-bit rnd */
#define CSEL        (BIT_17|BIT_18) /* Bit 17-18: CSelect: Bus Mastership */
#define PEYEN       (BIT_21)        /* Bit 21: Processing Element Y enable */
#define SIMD        (BIT_21)        /* Bit 21: Enable SIMD Mode */
#define BDCST9      (BIT_22)        /* Bit 22: Load Broadcast for I9 */
#define BDCST1      (BIT_23)        /* Bit 23: Load Broadcast for I1 */
#define CBUFEN      (BIT_24)        /* Bit 23: Circular Buffer Enable */

/* MODE2 register */
#define IRQ0E   (BIT_0)    /* Bit  0: IRQ0- 1=edge sens. 0=level sens. */
#define IRQ1E   (BIT_1)    /* Bit  1: IRQ1- 1=edge sens. 0=level sens. */
#define IRQ2E   (BIT_2)    /* Bit  2: IRQ2- 1=edge sens. 0=level sens. */
#define CADIS   (BIT_4)    /* Bit  4: Cache disable */
#define TIMEN   (BIT_5)    /* Bit  5: Timer enable */
#define EXTCADIS (BIT_6)    /* Bit  6: Cache disable for external execution */
#define CAFRZ   (BIT_19)   /* Bit 19: Cache freeze */
#define IIRAE   (BIT_20)   /* Bit 20: Illegal IOP Register Access Enable */
#define U64MAE  (BIT_21)   /* Bit 21: Unaligned 64-bit Memory Access Enable */

/* ASTATx and ASTATy registers */

#ifdef SUPPORT_DEPRECATED_USAGE
/* Several of these (AV, AC, MV, SV, SZ) are assembler-reserved keywords,
   so this style is now deprecated.  If these are defined, the assembler-
   reserved keywords are still available in lowercase, e.g.,
           IF sz JUMP LABEL1.
*/
#define AZ    (BIT_0)  /* Bit  0: ALU result zero or fltg-pt. underflow */
#define AV    (BIT_1)  /* Bit  1: ALU overflow */
#define AN    (BIT_2)  /* Bit  2: ALU result negative */
#define AC    (BIT_3)  /* Bit  3: ALU fixed-pt. carry */
#define AS    (BIT_4)  /* Bit  4: ALU X input sign (ABS and MANT ops) */
#define AI    (BIT_5)  /* Bit  5: ALU fltg-pt. invalid operation */
#define MN    (BIT_6)  /* Bit  6: Multiplier result negative */
#define MV    (BIT_7)  /* Bit  7: Multiplier overflow */
#define MU    (BIT_8)  /* Bit  8: Multiplier fltg-pt. underflow */
#define MI    (BIT_9)  /* Bit  9: Multiplier fltg-pt. invalid operation */
#define AF    (BIT_10) /* Bit 10: ALU fltg-pt. operation */
#define SV    (BIT_11) /* Bit 11: Shifter overflow */
#define SZ    (BIT_12) /* Bit 12: Shifter result zero */
#define SS    (BIT_13) /* Bit 13: Shifter input sign */
#define SF    (BIT_14) /* Bit 14: This is for bit-FIFO logic*/
#define BTF   (BIT_18) /* Bit 18: Bit test flag for system registers */
#define CACC0 (BIT_24) /* Bit 24: Compare Accumulation Bit 0 */
#define CACC1 (BIT_25) /* Bit 25: Compare Accumulation Bit 1 */
#define CACC2 (BIT_26) /* Bit 26: Compare Accumulation Bit 2 */
#define CACC3 (BIT_27) /* Bit 27: Compare Accumulation Bit 3 */
#define CACC4 (BIT_28) /* Bit 28: Compare Accumulation Bit 4 */
#define CACC5 (BIT_29) /* Bit 29: Compare Accumulation Bit 5 */
#define CACC6 (BIT_30) /* Bit 30: Compare Accumulation Bit 6 */
#define CACC7 (BIT_31) /* Bit 31: Compare Accumulation Bit 7 */

#endif

#define ASTAT_AZ      (BIT_0)  /* Bit  0: ALU result zero or fltg-pt. u'flow */
#define ASTAT_AV      (BIT_1)  /* Bit  1: ALU overflow */
#define ASTAT_AN      (BIT_2)  /* Bit  2: ALU result negative */
#define ASTAT_AC      (BIT_3)  /* Bit  3: ALU fixed-pt. carry */
#define ASTAT_AS      (BIT_4)  /* Bit  4: ALU X input sign(ABS and MANT ops) */
#define ASTAT_AI      (BIT_5)  /* Bit  5: ALU fltg-pt. invalid operation */
#define ASTAT_MN      (BIT_6)  /* Bit  6: Multiplier result negative */
#define ASTAT_MV      (BIT_7)  /* Bit  7: Multiplier overflow */
#define ASTAT_MU      (BIT_8)  /* Bit  8: Multiplier fltg-pt. underflow */
#define ASTAT_MI      (BIT_9)  /* Bit  9: Multiplier fltg-pt. invalid op. */
#define ASTAT_AF      (BIT_10) /* Bit 10: ALU fltg-pt. operation */
#define ASTAT_SV      (BIT_11) /* Bit 11: Shifter overflow */
#define ASTAT_SZ      (BIT_12) /* Bit 12: Shifter result zero */
#define ASTAT_SS      (BIT_13) /* Bit 13: Shifter input sign */
#define ASTAT_SF      (BIT_14)  
#define ASTAT_BTF     (BIT_18) /* Bit 18: Bit test flag for system registers */
#define ASTAT_CACC0   (BIT_24) /* Bit 24: Compare Accumulation Bit 0 */
#define ASTAT_CACC1   (BIT_25) /* Bit 25: Compare Accumulation Bit 1 */
#define ASTAT_CACC2   (BIT_26) /* Bit 26: Compare Accumulation Bit 2 */
#define ASTAT_CACC3   (BIT_27) /* Bit 27: Compare Accumulation Bit 3 */
#define ASTAT_CACC4   (BIT_28) /* Bit 28: Compare Accumulation Bit 4 */
#define ASTAT_CACC5   (BIT_29) /* Bit 29: Compare Accumulation Bit 5 */
#define ASTAT_CACC6   (BIT_30) /* Bit 30: Compare Accumulation Bit 6 */
#define ASTAT_CACC7   (BIT_31) /* Bit 31: Compare Accumulation Bit 7 */

/* STKYx and STKYy registers */
/* bits 0 to 9 in both STKYx and STKYY, bits 17 to 26 in STKYx only */
#define AUS     (BIT_0) /* Bit  0: ALU fltg-pt. underflow */
#define AVS     (BIT_1) /* Bit  1: ALU fltg-pt. overflow */
#define AOS     (BIT_2) /* Bit  2: ALU fixed-pt. overflow */
#define AIS     (BIT_5) /* Bit  5: ALU fltg-pt. invalid operation */
#define MOS     (BIT_6) /* Bit  6: Multiplier fixed-pt. overflow */
#define MVS     (BIT_7) /* Bit  7: Multiplier fltg-pt. overflow */
#define MUS     (BIT_8) /* Bit  8: Multiplier fltg-pt. underflow */
#define MIS     (BIT_9) /* Bit  9: Multiplier fltg-pt. invalid operation */
/* STKYx register *ONLY* */
#define CB7S    (BIT_17) /* Bit 17: DAG1 circular buffer 7 overflow */
#define CB15S   (BIT_18) /* Bit 18: DAG2 circular buffer 15 overflow */
#define IIRA    (BIT_19) /* Bit 19: Illegal IOP Register Access */
#define U64MA   (BIT_20) /* Bit 20: Unaligned 64-bit Memory Access */
#define PCFL    (BIT_21) /* Bit 21: PC stack full */
#define PCEM    (BIT_22) /* Bit 22: PC stack empty */
#define SSOV    (BIT_23) /* Bit 23: Status stack overflow (MODE1 and ASTAT) */
#define SSEM    (BIT_24) /* Bit 24: Status stack empty */
#define LSOV    (BIT_25) /* Bit 25: Loop stack overflow */
#define LSEM    (BIT_26) /* Bit 26: Loop stack empty */

/* IRPTL and IMASK and IMASKP registers */
#define EMUI    (BIT_0)  /* Bit  0 - Emulator Interrupt */
#define RSTI    (BIT_1)  /* Bit  1 - Reset */
#define IICDI   (BIT_2)  /* Bit  2 - Illegal Input Condition Detected */
#define SOVFI   (BIT_3)  /* Bit  3 - Stack overflow */
#define TMZHI   (BIT_4)  /* Bit  4 - Timer = 0 (high priority) */
#define SPERRI  (BIT_5)  /* Bit  5 - SPORT Error interrupt */
#define BKPI    (BIT_6)  /* Bit  6 - Hardware Breakpoint interrupt */
#define IRQ2I   (BIT_8)  /* Bit  8 - IRQ2- asserted */
#define IRQ1I   (BIT_9)  /* Bit  9 - IRQ1- asserted */
#define IRQ0I   (BIT_10) /* Bit 10 - IRQ0- asserted */
#define P0I     (BIT_11) /* Bit 11 - Programmable Interrupt 0 (default: DAI High Priority interrupt) */
#define P1I     (BIT_12) /* Bit 12 - Programmable Interrupt 1 (default: SPI interrupt) */
#define P2I     (BIT_13) /* Bit 13 - Programmable Interrupt 2 (default: General purpose IOP Timer0) */
#define P3I     (BIT_14) /* Bit 14 - Programmable Interrupt 3 (default: SPORT 1 Interrupt) */
#define P4I     (BIT_15) /* Bit 15 - Programmable Interrupt 4 (default: SPORT 3 Interrupt) */
#define P5I     (BIT_16) /* Bit 16 - Programmable Interrupt 5 (default: SPORT 5 Interrupt) */
#define P14I    (BIT_17) /* Bit 17 - Programmable Interrupt 14 (default: DPI Interrupt) */
#define P15I    (BIT_18) /* Bit 18 - Programmable Interrupt 15 (default: MTMDMA Interrupt) */
#define P16I    (BIT_19) /* Bit 19 - Programmable Interrupt 16 (default: SPORT 6 Interrupt) */
#define CB7I    (BIT_20) /* Bit 20 - Circ. buffer 7 overflow */
#define CB15I   (BIT_21) /* Bit 21 - Circ. buffer 15 overflow */
#define TMZLI   (BIT_22) /* Bit 22 - Timer = 0 (low priority) */
#define FIXI    (BIT_23) /* Bit 23 - Fixed-pt. overflow */
#define FLTOI   (BIT_24) /* Bit 24 - fltg-pt. overflow */
#define FLTUI   (BIT_25) /* Bit 25 - fltg-pt. underflow */
#define FLTII   (BIT_26) /* Bit 26 - fltg-pt. invalid */
#define EMULI   (BIT_27) /* Bit 27 - emulator interrupt low priority */
#define SFT0I   (BIT_28) /* Bit 28 - user software int 0 */
#define SFT1I   (BIT_29) /* Bit 29 - user software int 1 */
#define SFT2I   (BIT_30) /* Bit 30 - user software int 2 */
#define SFT3I   (BIT_31) /* Bit 31 - user software int 3 */

/* IRPTL and IMASK and IMASKP registers default values for peripheral interrupts */

#define DAIHI   (BIT_11) /* Bit 11 - DAI interrupt */
#define SPIHI   (BIT_12) /* Bit 12 - SPI High priority interrupt */
#define SPII    (BIT_12) /* Bit 12 - SPI High priority interrupt */
#define GPTMR0I (BIT_13) /* Bit 13 - General purpose IOP Timer0 */
#define SP1I    (BIT_14) /* Bit 14 - SPORT 1 Interrupt */
#define SP3I    (BIT_15) /* Bit 15 - SPORT 3 Interrupt */
#define SP5I    (BIT_16) /* Bit 16 - SPORT 5 Interrupt */
#define DPII    (BIT_17) /* Bit 17 - DPI Interrupt */
#define MTMI    (BIT_18) /* Bit 18 - MTM Interrupt */
#define SP6I    (BIT_19) /* Bit 19 - SPORT 6 Interrupt */


/* LIRPTL register */
#define P6I         (BIT_0)    /* Bit 0 - Programmable Interrupt 6 (default: SPORT 0 interrupt) */
#define P7I         (BIT_1)    /* Bit 1 - Programmable Interrupt 7 (default: SPORT 2 interrupt) */
#define P8I         (BIT_2)    /* Bit 2 - Programmable Interrupt 8 (default: SPORT 4 interrupt) */
#define P9I         (BIT_3)    /* Bit 3 - Programmable Interrupt 9 (default: EPDMA 0 Interrupt) */
#define P10I        (BIT_4)    /* Bit 4 - Programmable Interrupt 10 (default: General Purpose  Timer1 Interrupt) */
#define P11I        (BIT_5)    /* Bit 5 - Programmable Interrupt 10 (default: SPORT 7 interrupt) */
#define P12I        (BIT_6)    /* Bit 6 - Programmable Interrupt 12 (default: DAI Low Priority Interrupt) */
#define P13I        (BIT_7)    /* Bit 7 - Programmable Interrupt 13 (default: EPDMA 1 interrupt) */
#define P17I        (BIT_8)    /* Bit 8 - Programmable Interrupt 17 (no peripheral is connected by default) */
#define P18I        (BIT_9)    /* Bit 9 - Programmable Interrupt 18 (default: SPIB interrupt) */
#define P6IMSK      (BIT_10)   /* Bit 10 - Programmable Interrupt 6 Mask */
#define P7IMSK      (BIT_11)   /* Bit 11 - Programmable Interrupt 7 Mask */
#define P8IMSK      (BIT_12)   /* Bit 12 - Programmable Interrupt 8 Mask */
#define P9IMSK      (BIT_13)   /* Bit 13 - Programmable Interrupt 9 Mask */
#define P10IMSK     (BIT_14)   /* Bit 14 - Programmable Interrupt 10 Mask */
#define P11IMSK     (BIT_15)   /* Bit 15 - Programmable Interrupt 11 Mask */
#define P12IMSK     (BIT_16)   /* Bit 16 - Programmable Interrupt 12 Mask */
#define P13IMSK     (BIT_17)   /* Bit 17 - Programmable Interrupt 13 Mask */
#define P17IMSK     (BIT_18)   /* Bit 18 - Programmable Interrupt 17 Mask */
#define P18IMSK     (BIT_19)   /* Bit 19 - Programmable Interrupt 18 Mask */
#define P6IMSKP     (BIT_20)   /* Bit 20 - Programmable Interrupt 6 Mask Pointer */
#define P7IMSKP     (BIT_21)   /* Bit 21 - Programmable Interrupt 7 Mask Pointer */
#define P8IMSKP     (BIT_22)   /* Bit 22 - Programmable Interrupt 8 Mask Pointer */
#define P9IMSKP     (BIT_23)   /* Bit 23 - Programmable Interrupt 9 Mask Pointer */
#define P10IMSKP    (BIT_24)   /* Bit 24 - Programmable Interrupt 10 Mask Pointer */
#define P11IMSKP    (BIT_25)   /* Bit 25 - Programmable Interrupt 11 Mask Pointer */
#define P12IMSKP    (BIT_26)   /* Bit 26 - Programmable Interrupt 12 Mask Pointer */
#define P13IMSKP    (BIT_27)   /* Bit 26 - Programmable Interrupt 13 Mask Pointer */
#define P17IMSKP    (BIT_28)   /* Bit 28 - Programmable Interrupt 17 Mask Pointer */
#define P18IMSKP    (BIT_29)   /* Bit 29 - Programmable Interrupt 18 Mask Pointer */

/* LIRPTL register default peripheral interrupts */
#define SP0I        (BIT_0)    /* Bit 0 - SPORT 0 interrupt */
#define SP2I        (BIT_1)    /* Bit 1 - SPORT 2 interrupt */
#define SP4I        (BIT_2)    /* Bit 2 - SPORT 4 interrupt */
#define EP0I        (BIT_3)    /* Bit 3 - External Port 0 Interrupt */
#define GPTMR1I     (BIT_4)    /* Bit 4 - General Purpose IOP Timer 1 Interrupt */
#define SP7I        (BIT_5)    /* Bit 5 - SPORT 7 interrupt */
#define DAILI       (BIT_6)    /* Bit 6 - DAI Low Priority Interrupt */
#define EP1I        (BIT_7)    /* Bit 7 - External Port 1 Interrupt */
#define GPTMR2I     (BIT_8)    /* Bit 8 - General Purpose Interrupt */
#define SPILI       (BIT_9)    /* Bit 9 - SPI Low Priority Interrupt */
#define SPIBI       (BIT_9)    /* Bit 9 - SPI Low Priority Interrupt */
#define SP0IMSK     (BIT_10)   /* Bit 10 - SPORT0 Interrupt Mask */
#define SP2IMSK     (BIT_11)   /* Bit 11 - SPORT2 Interrupt Mask */
#define SP4IMSK     (BIT_12)   /* Bit 12 - SPORT4 Interrupt Mask */
#define EP0IMSK     (BIT_13)   /* Bit 13 - External Port 0 Interrupt Mask */
#define GPTMR1IMSK  (BIT_14)   /* Bit 14 - General Purpose IOP Timer 1 Interrupt Mask */
#define SP7IMSK     (BIT_15)   /* Bit 15 - SPORT 7 interrupt Mask */
#define DAILIMSK    (BIT_16)   /* Bit 16 - DAI Low Priority Interrupt Mask */
#define EP1IMSK     (BIT_17)   /* Bit 17 - External Port 1 Interrupt */
#define GPTMR2IMSK  (BIT_18)   /* Bit 18 - General Purpose Interrupt Mask */
#define SPILIMSK    (BIT_19)   /* Bit 19 - SPI Low Priority Interrupt Mask */
#define SPIBIMSK    (BIT_19)   /* Bit 19 - SPI Low Priority Interrupt Mask */
#define SP0IMSKP    (BIT_20)   /* Bit 20 - SPORT0 Interrupt Mask Pointer */
#define SP2IMSKP    (BIT_21)   /* Bit 21 - SPORT2 Interrupt Mask Pointer */
#define SP4IMSKP    (BIT_22)   /* Bit 22 - SPORT4 Interrupt Mask Pointer */
#define EP0IMSKP    (BIT_23)   /* Bit 23 - External Port 0 Interrupt Mask Pointer */
#define GPTMR1IMSKP (BIT_24)   /* Bit 24 - General Purpose IOP Timer 1 Interrupt Mask Pointer */
#define SP7IMSKP    (BIT_25)   /* Bit 25 - SPORT 7 interrupt Mask Pointer */
#define DAILIMSKP   (BIT_26)   /* Bit 26 - DAI Low Priority Interrupt Mask Pointer */
#define EP1IMSKP    (BIT_27)   /* Bit 17 - External Port 1 Interrupt Mask Pointer */
#define GPTMR2IMSKP (BIT_28)   /* Bit 28 - General Purpose IOP Timer 2 Interrupt Mask Pointer */
#define SPILIMSKP   (BIT_29)   /* Bit 29 - SPI Low Priority Interrupt Mask Pointer */
#define SPIBIMSKP   (BIT_29)   /* Bit 29 - SPI Low Priority Interrupt Mask Pointer */


/* FLAGS register */
/* FLAGS register - programmable I/O status macro definitions */
#define FLG0    (BIT_0)    /* FLAG0 value (Low = '0', High = '1') */
#define FLG1    (BIT_2)    /* FLAG1 value (Low = '0', High = '1') */
#define FLG2    (BIT_4)    /* FLAG2 value (Low = '0', High = '1') */
#define FLG3    (BIT_6)    /* FLAG3 value (Low = '0', High = '1') */
#define FLG4    (BIT_8)    /* FLAG4 value (Low = '0', High = '1') */
#define FLG5    (BIT_10)   /* FLAG5 value (Low = '0', High = '1') */
#define FLG6    (BIT_12)   /* FLAG6 value (Low = '0', High = '1') */
#define FLG7    (BIT_14)   /* FLAG7 value (Low = '0', High = '1') */
#define FLG8    (BIT_16)   /* FLAG8 value (Low = '0', High = '1') */
#define FLG9    (BIT_18)   /* FLAG9 value (Low = '0', High = '1') */
#define FLG10   (BIT_20)   /* FLAG10 value (Low = '0', High = '1') */
#define FLG11   (BIT_22)   /* FLAG11 value (Low = '0', High = '1') */
#define FLG12   (BIT_24)   /* FLAG12 value (Low = '0', High = '1') */
#define FLG13   (BIT_26)   /* FLAG13 value (Low = '0', High = '1') */
#define FLG14   (BIT_28)   /* FLAG14 value (Low = '0', High = '1') */
#define FLG15   (BIT_30)   /* FLAG15 value (Low = '0', High = '1') */
/* FLAGS register - programmable I/O control macro definitions */
#define FLG0O   (BIT_1)    /* FLAG0 control ('0' = flag input, '1' = flag output) */
#define FLG1O   (BIT_3)    /* FLAG1 control ('0' = flag input, '1' = flag output) */
#define FLG2O   (BIT_5)    /* FLAG2 control ('0' = flag input, '1' = flag output) */
#define FLG3O   (BIT_7)    /* FLAG3 control ('0' = flag input, '1' = flag output) */
#define FLG4O   (BIT_9)    /* FLAG4 control ('0' = flag input, '1' = flag output) */
#define FLG5O   (BIT_11)   /* FLAG5 control ('0' = flag input, '1' = flag output) */
#define FLG6O   (BIT_13)   /* FLAG6 control ('0' = flag input, '1' = flag output) */
#define FLG7O   (BIT_15)   /* FLAG7 control ('0' = flag input, '1' = flag output) */
#define FLG8O   (BIT_17)   /* FLAG8 control ('0' = flag input, '1' = flag output) */
#define FLG9O   (BIT_19)   /* FLAG9 control ('0' = flag input, '1' = flag output) */
#define FLG10O  (BIT_21)   /* FLAG10 control ('0' = flag input, '1' = flag output) */
#define FLG11O  (BIT_23)   /* FLAG11 control ('0' = flag input, '1' = flag output) */
#define FLG12O  (BIT_25)   /* FLAG12 control ('0' = flag input, '1' = flag output) */
#define FLG13O  (BIT_27)   /* FLAG13 control ('0' = flag input, '1' = flag output) */
#define FLG14O  (BIT_29)   /* FLAG14 control ('0' = flag input, '1' = flag output) */
#define FLG15O  (BIT_31)   /* FLAG15 control ('0' = flag input, '1' = flag output) */


/*============================================================================ */
/*                                                                             */
/*                   IOP Control/Status Register Bit Definitions               */
/*                                                                             */
/*============================================================================ */
/* EEMUSTAT Register */
#define STATPA          (BIT_0)    /* (Read Only) Program Memory Breakpoint Status */
#define STATDA0         (BIT_1)    /* (Read Only) Data Memory Breakpoint #0 Status */
#define STATDA1         (BIT_2)    /* (Read Only) Data Memory Breakpoint #1 Status */
#define STATIA0         (BIT_3)    /* (Read Only) Instruction Breakpoint #0 Status */
#define STATIA1         (BIT_4)    /* (Read Only) Instruction Breakpoint #1 Status */
#define STATIA2         (BIT_5)    /* (Read Only) Instruction Breakpoint #2 Status */
#define STATIA3         (BIT_6)    /* (Read Only) Instruction Breakpoint #3 Status */
#define STATIO0         (BIT_7)    /* (Read Only) Peripheral DMA Breakpoint Status */
#define EEMUOUTIRQEN    (BIT_9)    /* EEMUOUT Interrupt Enable */
#define EEMUOUTRDY      (BIT_10)   /* (Read Only) EEMUOUT Valid Data Status */
#define EEMUOUTFULL     (BIT_11)   /* (Read Only) EEMUOUT FIFO Full Status */
#define EEMUINFULL      (BIT_12)   /* (Read Only) EEMUIN FIFO Full Status */
#define EEMUENS         (BIT_13)   /* (Read Only) Enhanced Emulation Feature Enable Status */
#define OSPIDENS        (BIT_14)   /* (Read Only) Operating System Process ID Enable Status */
#define EEMUINENS       (BIT_15)   /* (Read Only) EEMUIN Interrupt Enable Status */
#define STATIO1         (BIT_16)   /* (Read Only) I/O Memory Breakpoint #0 Status */

/* RUNRSTCTL Register */
#define PM_RUN_RST_PINEN 0x01      /* (Read/Write) Configure the CLKOUT pin as RUNRST input */
#define PM_RUNRST_EN 0x02          /* (Read/Write) Enable the RUNRST functionality */

/* SYSCTL Register */
#define SRST            (BIT_0)                /* Soft Reset */
#define IIVT            (BIT_2)                /* Internal Interrupt Vector Table */
#define DCPR            (BIT_7)                /* Select rotating priority for bus arbitration */
#define IMDW0           (BIT_9)                /* Internal memory block 0, extended data (48 bit) */
#define IMDW1           (BIT_10)               /* Internal memory block 1, extended data (48 bit) */
#define IMDW2           (BIT_11)               /* Internal memory block 2, extended data (48 bit) */
#define IMDW3           (BIT_12)               /* Internal memory block 3, extended data (48 bit) */
#define SY_GPO9           (BIT_14)
#define SY_GPO10            (BIT_15)

#define IRQ0EN          (BIT_16)               /* Selects IRQ0 interrupt mode (set) or Flag 0 mode (clear) */
#define IRQ1EN          (BIT_17)               /* Selects IRQ1 interrupt mode (set) or Flag 1 mode (clear) */
#define IRQ2EN          (BIT_18)               /* Selects IRQ2 interrupt mode (set) or Flag 2 mode (clear) */
#define TMREXPEN        (BIT_19)               /* Selects timer-expired  mode (set) or Flag 3 mode (clear) */
#define MSEN            (BIT_20)               /* Enables FLAG2 and 3 (IRQ2 and TIMEXP) as MS2 and 3 */
#define EPDATA32        (0x00000000)           /* Data [31:0] pins are data lines */
#define NODATA          (BIT_21|BIT_22)        /* Data [31:0] pins are FLAGS/PWM */
#define PDAPFLAGS       (BIT_21|BIT_23)        /* Data [31:8] pins are PDAP, Data [7:0] are flags */
#define TRISTATE        (BIT_21|BIT_22|BIT_23) /* TRISTATE Data [31:0] PINS */
#define PWM0EN          (BIT_24)               /* Enables Data[19:16] as PWM3-0 */
#define PWM1EN          (BIT_25)               /* Enables Data[23:20] as PWM7-4 */
#define PWM2EN          (BIT_26)               /* Enables Data[27:24] as PWM11-8 */
#define PWM3EN          (BIT_27)               /* Enables Data[31:28] as PWM15-12 */
#define FSYNC           (BIT_28)               /* Force Synchronization on the Shared Memory Bus */
#define BUSLK           (BIT_29)               /* Lock the Shared Memory Bus */

/* BRKCTL Register */
#define PA1MODE     (BIT_0|BIT_1)    /* PA1 Triggering Mode */
#define DA1MODE     (BIT_2|BIT_3)    /* DA1 Triggering Mode */
#define DA2MODE     (BIT_4|BIT_5)    /* DA2 Triggering Mode */
#define IO1MODE     (BIT_6|BIT_7)    /* IO1 Triggering Mode */
#define NEGPA1      (BIT_10)         /* Negate Program Memory Address Breakpoint #1 */
#define NEGDA1      (BIT_11)         /* Negate Data Memory Address Breakpoint #1 */
#define NEGDA2      (BIT_12)         /* Negate Data Memory Address Breakpoint #2 */
#define NEGIA1      (BIT_13)         /* Negate Instruction Address Breakpoint #1 */
#define NEGIA2      (BIT_14)         /* Negate Instruction Address Breakpoint #2 */
#define NEGIA3      (BIT_15)         /* Negate Instruction Address Breakpoint #3 */
#define NEGIA4      (BIT_16)         /* Negate Instruction Address Breakpoint #4 */
#define NEGIO1      (BIT_17)         /* Negate DMA Address Breakpoint */

#define ENBPA       (BIT_19)         /* Enable Program Memory Address Breakpoints */
#define ENBDA       (BIT_20)         /* Enable Data Memory Address Breakpoints */
#define ENBIA       (BIT_21)         /* Enable Instruction Address Breakpoints */
#define ENBEP       (BIT_23)         /* Enable External Address Breakpoints */
#define ANDBKP      (BIT_24)         /* AND the Composite Breakpoints */
#define UMODE       (BIT_25)         /* User Mode Breakpoint Functionality Enable */
#define ENIOD1      (BIT_26)  	/* EP DMA Breakpoint Enable */
#define ENIOD0     (BIT_27)  	/* Peripheral DMA Breakpoint Enable */



/* REVPID */
#define PROCID  (BIT_0|BIT_1|BIT_2|BIT_3) /* Read Processor ID Code */
#define SIREV   (BIT_4|BIT_5|BIT_6|BIT_7) /* Read Silicon Revision */

/* DDR2 registers */
/* DDR2CTL0 bit definitions */
#define DIS_DDR2CTL  (BIT_0)        /* DIS_DDR2CTL bit is used to enable or disable the DDR2C */
#define DIS_DDR2CLK1 (BIT_1)        /* the clock 1(DIS_DDR2CLK1 bit)can be frozen during Self-Refresh mode */
#define DDR2BC4     (BIT_2)        /* 4-bank device */
#define DDR2BC8     (BIT_3)        /* 8-bank device */
#define DIS_DDR2CKE  (BIT_4)        /* The DIS_DDCKE bit if set disable the external DDCKE signal */
#define DDR2CAW8    (0x00000000)   /* the Bank column address width for 8-bits is 000 */
#define DDR2CAW9    (BIT_5)        /* the Bank column address width for 9-bits is 001 */
#define DDR2CAW10   (BIT_6)        /* the Bank column address width for 10-bits is 010 */
#define DDR2CAW11   (BIT_5|BIT_6)  /* the Bank column address width for 11-bits is 011 */
#define DDR2CAW12   (BIT_7)        /* the Bank column address width for 12-bits is 100 */
#define SH_DLL_DIS  (BIT_8)        /* Disable or enable the DLL0 and DLL1 */
#define DDR2RAW8    (0x00000000)   /* the Row address width for 8-bits is 000 */
#define DDR2RAW9    (BIT_9)        /* the Row address width for 9-bits is 001 */
#define DDR2RAW10   (BIT_10)       /* the Row address width for 10-bits is 010 */
#define DDR2RAW11   (BIT_9|BIT_10) /* the Row address width for 11-bits is 011 */
#define DDR2RAW12   (BIT_11)       /* the Row address width for 12-bits is 100 */
#define DDR2RAW13   (BIT_9|BIT_11) /* the Row address width for 13-bits is 101 */
#define DDR2RAW14   (BIT_10|BIT_11)       /* the Row address width for 14-bits is 110 */
#define DDR2RAW15   (BIT_9|BIT_10|BIT_11) /* the Row address width for 15-bits is 111 */
#define FEMR2      (BIT_12)       /* Force EMR2 bit if set when DDRs in precharged state will initiate an extended mode register2 write to DDRs. */
#define FDLLCAL      (BIT_13)       /* Force DDR2 DLL calibration */
#define DDR2ADDRMODE (BIT_14)       /* DDR2 address mode (page or bank interleaving) */
#define DDR2PSS     (BIT_15)       /* DDR2 Power-up Sequence Start enable */
#define DDR2WDTHx16  (BIT_16)
#define FEMR3      (BIT_17)       /* Force EMR3 Force EMR3 bit if set when DDRs in precharged state will initiate an extended mode register3 write to DDRs */
#define DDR2SRF     (BIT_18)       /* DDR2 self refresh command enable  */
#define DDR2ORF     (BIT_19)       /* DDR2 optional refresh mode */
#define FARF       (BIT_20)       /* forces auto-refresh */
#define FPC        (BIT_21)       /* forces pre-charge */
#define FLMR       (BIT_22)       /* Force LMR bit if set when DDRs in precharged state will initiate a mode register write to DDRs */
#define FEMR       (BIT_23)       /* Force EMR bit if set when DDRs in precharged state will initiate an extended mode register write to DDRs */
#define DDR2BUF     (BIT_24)       /* The DDBUF bit is used to enable or disable external buffer timing */
#define SREF_EXIT   (BIT_25)       /* Self-Refresh exit */
#define DDR2OPT     (BIT_27)       /* The DDROPT bit enable the read optimization mode */
#define DDR2MODIFY0  (0x00000000)  /* DDR2 read modifier value in optimization mode */
#define DDR2MODIFY1 (BIT_28)
#define DDR2MODIFY2 (BIT_29)
#define DDR2MODIFY3 (BIT_29|BIT_28)
#define DDR2MODIFY4 (BIT_30)
#define DDR2MODIFY5 (BIT_30|BIT_28)
#define DDR2MODIFY6 (BIT_30|BIT_29)
#define DDR2MODIFY7 (BIT_30|BIT_29|BIT_28)
#define DDR2MODIFY8  (BIT_31)
#define DDR2MODIFY9  (BIT_31|BIT_28)
#define DDR2MODIFY10 (BIT_31|BIT_28)
#define DDR2MODIFY11 (BIT_31|BIT_29|BIT_28)
#define DDR2MODIFY12 (BIT_31|BIT_30)
#define DDR2MODIFY13 (BIT_31|BIT_30|BIT_28)
#define DDR2MODIFY14 (BIT_31|BIT_30|BIT_29) 
#define DDR2MODIFY15 (BIT_31|BIT_30|BIT_29|BIT_28)


/* DDR2CTL1 bit definitions */

#define DDR2TRAS1    (BIT_0)   
#define DDR2TRAS2    (BIT_1)   
#define DDR2TRAS3    (BIT_0|BIT_1)   
#define DDR2TRAS4    (BIT_2)   
#define DDR2TRAS5    (BIT_0|BIT_2)   
#define DDR2TRAS6    (BIT_1|BIT_2)   
#define DDR2TRAS7    (BIT_0|BIT_1|BIT_2)   
#define DDR2TRAS8    (BIT_3)   
#define DDR2TRAS9    (BIT_0|BIT_3)   
#define DDR2TRAS10   (BIT_1|BIT_3)   
#define DDR2TRAS11   (BIT_3|BIT_1|BIT_0)   
#define DDR2TRAS12   (BIT_3|BIT_2)   
#define DDR2TRAS13   (BIT_3|BIT_2|BIT_0)   
#define DDR2TRAS14   (BIT_3|BIT_2|BIT_1)   
#define DDR2TRAS15   (BIT_3|BIT_2|BIT_1|BIT_0)   
#define DDR2TRAS16   (BIT_4)   
#define DDR2TRAS17   (BIT_4|BIT_0)   
#define DDR2TRAS18   (BIT_4|BIT_1)   
#define DDR2TRAS19   (BIT_4|BIT_1|BIT_0)   
#define DDR2TRAS20   (BIT_4|BIT_2)   
#define DDR2TRAS21    (BIT_4|BIT_0|BIT_2)   
#define DDR2TRAS22    (BIT_4|BIT_1|BIT_2)   
#define DDR2TRAS23    (BIT_4|BIT_0|BIT_1|BIT_2)   
#define DDR2TRAS24    (BIT_4|BIT_3)   
#define DDR2TRAS25    (BIT_4|BIT_0|BIT_3)   
#define DDR2TRAS26   (BIT_4|BIT_1|BIT_3)   
#define DDR2TRAS27   (BIT_4|BIT_3|BIT_1|BIT_0)   
#define DDR2TRAS28   (BIT_4|BIT_3|BIT_2)   
#define DDR2TRAS29   (BIT_4|BIT_3|BIT_2|BIT_0)   
#define DDR2TRAS30   (BIT_4|BIT_3|BIT_2|BIT_1)   
#define DDR2TRAS31   (BIT_4|BIT_3|BIT_2|BIT_1|BIT_0)   

#define DDR2TRP1     (BIT_5)  
#define DDR2TRP2     (BIT_6)   
#define DDR2TRP3     (BIT_6|BIT_5)  
#define DDR2TRP4     (BIT_7)
#define DDR2TRP5     (BIT_7|BIT_5) 
#define DDR2TRP6     (BIT_7|BIT_6)
#define DDR2TRP7     (BIT_7|BIT_6|BIT_5) 
#define DDR2TRP8     (BIT_8)
#define DDR2TRP9     (BIT_8|BIT_5)  
#define DDR2TRP10     (BIT_8|BIT_6)   
#define DDR2TRP11    (BIT_8|BIT_6|BIT_5)  
#define DDR2TRP12     (BIT_8|BIT_7)
#define DDR2TRP13    (BIT_8|BIT_7|BIT_5) 
#define DDR2TRP14    (BIT_8|BIT_7|BIT_6)
#define DDR2TRP15    (BIT_8|BIT_7|BIT_6|BIT_5) 

#define DDR2TWTR1     (BIT_9)
#define DDR2TWTR2     (BIT_10)
#define DDR2TWTR3     (BIT_9|BIT_10)
#define DDR2TWTR4     (BIT_11)
#define DDR2TWTR5     (BIT_9|BIT_11)
#define DDR2TWTR6     (BIT_10|BIT_11)
#define DDR2TWTR7     (BIT_9|BIT_10|BIT_11)

#define DDR2TRCD1     (BIT_12)
#define DDR2TRCD2     (BIT_13)
#define DDR2TRCD3     (BIT_12|BIT_13)
#define DDR2TRCD4     (BIT_14)
#define DDR2TRCD5     (BIT_14|BIT_12)
#define DDR2TRCD6     (BIT_13|BIT_14)
#define DDR2TRCD7     (BIT_13|BIT_12|BIT_14)
#define DDR2TRCD8     (BIT_15)
#define DDR2TRCD9     (BIT_15|BIT_12)
#define DDR2TRCD10    (BIT_15|BIT_13)
#define DDR2TRCD11    (BIT_15|BIT_12|BIT_13)
#define DDR2TRCD12    (BIT_15|BIT_14)
#define DDR2TRCD13    (BIT_15|BIT_14|BIT_12)
#define DDR2TRCD14    (BIT_15|BIT_13|BIT_14)
#define DDR2TRCD15    (BIT_15|BIT_13|BIT_12|BIT_14)

#define DDR2TMRD1     (BIT_16)
#define DDR2TMRD2     (BIT_17)
#define DDR2TMRD3     (BIT_16|BIT_17)
#define DDR2TMRD4     (BIT_18)
#define DDR2TMRD5     (BIT_16|BIT_18)
#define DDR2TMRD6     (BIT_17|BIT_18)
#define DDR2TMRD7     (BIT_16|BIT_17|BIT_18)

#define DDR2TRTP1     (BIT_19)
#define DDR2TRTP2     (BIT_20)
#define DDR2TRTP3     (BIT_20|BIT_19)
#define DDR2TRTP4     (BIT_21)
#define DDR2TRTP5     (BIT_19|BIT_21)
#define DDR2TRTP6     (BIT_21|BIT_20)
#define DDR2TRTP7     (BIT_19|BIT_20|BIT_21)


#define DDR2TRRD1     (BIT_22)
#define DDR2TRRD2     (BIT_23)
#define DDR2TRRD3     (BIT_22|BIT_23)
#define DDR2TRRD4     (BIT_24)
#define DDR2TRRD5     (BIT_22|BIT_24)
#define DDR2TRRD6     (BIT_23|BIT_24)
#define DDR2TRRD7     (BIT_22|BIT_23|BIT_24)


#define DDR2TFAW1    (BIT_25)   
#define DDR2TFAW2    (BIT_26)   
#define DDR2TFAW3    (BIT_25|BIT_26)   
#define DDR2TFAW4    (BIT_27)   
#define DDR2TFAW5    (BIT_25|BIT_27)   
#define DDR2TFAW6    (BIT_26|BIT_27)   
#define DDR2TFAW7    (BIT_25|BIT_26|BIT_27)   
#define DDR2TFAW8    (BIT_28)   
#define DDR2TFAW9    (BIT_25|BIT_28)   
#define DDR2TFAW10   (BIT_26|BIT_28)   
#define DDR2TFAW11   (BIT_28|BIT_26|BIT_25)   
#define DDR2TFAW12   (BIT_28|BIT_27)   
#define DDR2TFAW13   (BIT_28|BIT_27|BIT_25)   
#define DDR2TFAW14   (BIT_28|BIT_27|BIT_26)   
#define DDR2TFAW15   (BIT_28|BIT_27|BIT_26|BIT_25)   
#define DDR2TFAW16   (BIT_29)   
#define DDR2TFAW17   (BIT_29|BIT_25)   
#define DDR2TFAW18   (BIT_29|BIT_26)   
#define DDR2TFAW19   (BIT_29|BIT_26|BIT_25)   
#define DDR2TFAW20   (BIT_29|BIT_27)   
#define DDR2TFAW21   (BIT_29|BIT_25|BIT_27)   
#define DDR2TFAW22   (BIT_29|BIT_26|BIT_27)   
#define DDR2TFAW23   (BIT_29|BIT_25|BIT_26|BIT_27)   
#define DDR2TFAW24   (BIT_29|BIT_28)   
#define DDR2TFAW25   (BIT_29|BIT_25|BIT_28)   
#define DDR2TFAW26   (BIT_29|BIT_26|BIT_28)   
#define DDR2TFAW27   (BIT_29|BIT_28|BIT_26|BIT_25)   
#define DDR2TFAW28   (BIT_29|BIT_28|BIT_27)   
#define DDR2TFAW29   (BIT_29|BIT_28|BIT_27|BIT_25)   
#define DDR2TFAW30   (BIT_29|BIT_28|BIT_27|BIT_26)   
#define DDR2TFAW31   (BIT_29|BIT_28|BIT_27|BIT_26|BIT_25)   





/* DDR2CTL2 bit definitions */

#define DDR2BL4       (BIT_1)

#define DDR2CAS1      (BIT_4)
#define DDR2CAS2      (BIT_5)
#define DDR2CAS3      (BIT_4|BIT_5)
#define DDR2CAS4      (BIT_6)
#define DDR2CAS5      (BIT_4|BIT_6)
#define DDR2CAS6      (BIT_5|BIT_6)
#define DDR2CAS7      (BIT_4|BIT_5|BIT_6)

#define DDR2DLLRST    (BIT_8)

#define DDR2TWR2      (BIT_9)  /* time Write recovery */
#define DDR2TWR3      (BIT_10)
#define DDR2TWR4      (BIT_9|BIT_10)
#define DDR2TWR5      (BIT_11)
#define DDR2TWR6      (BIT_9|BIT_11)
#define DDR2TWR7      (BIT_10|BIT_11)

#define DDR2MR    (0) 

/* DDR2CTL3 bit definitions */

#define DDR2DLLEN    (BIT_0) /* DLL enable */
#define DDR2OPDS      (BIT_1) /* drive strength reduced if bit is set */
#define DDR2AL0       (00000000)
#define DDR2AL1       (BIT_3) 
#define DDR2AL2       (BIT_4) 
#define DDR2AL3       (BIT_3|BIT_4) 
#define DDR2AL4       (BIT_5) 
#define DDR2AL5       (BIT_3|BIT_5) 

#define DDR2ODTDIS    (00000000)
#define DDR2ODT75     (BIT_2)
#define DDR2ODT150     (BIT_6)
#define DDR2ODT50     (BIT_2|BIT_6)

#define DDR2DQSDIS    (BIT_10)
#define DDR2OBDIS     (BIT_12)
#define DDR2EXTMR1    (BIT_14)

/* DDR2CTL4  bit definitions  */

#define DDR2EXTMR2    (BIT_15)

/* DDR2CTL4  bit definitions  */

#define DDR2EXTMR3    (BIT_14|BIT_15)

/* DDR2 status registers DDR2STAT0 */

#define DDR2CI        (BIT_0)   /* DDR2 controller is idle  */
#define DDR2SRA       (BIT_1)   /* the self-refresh is active */
#define DDR2PUA       (BIT_2)   /* DDR2 in power-up sequencing mode */
#define DDR2RS        (BIT_3)   /* reset value is 1, no power up occured since last reset */
#define DDR2MSE       (BIT_4)   /* DDR2 access error bit */
#define DDR2PD        (BIT_6)   /* If this bit is set, DDR2 is in powerdown mode */
#define DDR2DLLCAL     (BIT_7)   /* If bit is set, DDR2 DLL is in calibration sequence */
#define DDR2DLLCALDONE (BIT_8)  /* Bit set means DDR2 DLL has been calibrated since Reset */

/* DDR2 status register DDR2STAT1 */

/* DLL0CTL0 bit definitions */
/* DLL0CTL0 is a reserved register */

/* DLL0CTL1 bit definitions */

#define DDR2DLL0RESET (BIT_9)
#define DDR2DAT0RESET  (BIT_10)
#define DDR2CAL0RESET (BIT_11)

/* DLL1CTL0 bit definitions */
/* DLL1CTL0 is a reserved register */

/* DLL1CTL1 bit definitions */

#define DDR2DLL1RESET (BIT_9)
#define DDR2DAT1RESET  (BIT_10)
#define DDR2CAL1RESET (BIT_11)

/* DLL0STAT0 bit definitions */

/* DLL0STAT1 bit definitions */

 
#define EPCTL (0x1801) /* External Port Global Control Register */

/* EPCTL register bits */
#define B0SD       (BIT_0)
#define B1SD       (BIT_1)
#define B2SD       (BIT_2)
#define B3SD       (BIT_3)
#define EPBRSPDMA  (0x00000)     /* Priority order (high - low) is SPORT, external port DMA, core */
#define EPBRDMA    (BIT_4)       /* Priority order (high - low) is external port DMA, SPORT, core */
#define EPBRCORE   (BIT_5)       /* Highest priority is core. SPORT and EP DMA are in rotating priority */
#define EPBRROT    (BIT_4|BIT_5) /* Rotating priority (default) */
#if 0
   DMAPRCH1 has been removed as DMA channel 1 can never get high priority.
   Only DMA channel 0 gets high priority for fixed priority operation. In
   rotating priority both channels get equal priority.
#define DMAPRCH1   (BIT_6) 
#endif
#define DMAPRCH0   (BIT_7)
#define DMAPRROT   (BIT_6|BIT_7)
#define NOFRZDMA   (0x00000)
#define FRZDMA4    (BIT_8)
#define FRZDMA8    (BIT_9)
#define FRZDMA16   (BIT_8|BIT_9)
#define FRZDMA32   (BIT_10)
#define FRZDMAPG   (BIT_8|BIT_10)
#define NOFRZCR    (0x00000)
#define FRZCR4     (BIT_12)
#define FRZCR8     (BIT_13)
#define FRZCR16    (BIT_12|BIT_13)
#define FRZCR32    (BIT_14)
#define FRZCRPG    (BIT_12|BIT_14)
#define NOFRZSP    (0x00000)
#define FRZSP4     (BIT_19)
#define FRZSP8     (BIT_20)
#define FRZSP16    (BIT_19|BIT_20)
#define FRZSP32    (BIT_21)
#define FRZSPPG    (BIT_19|BIT_21)




/************************************************************/
/*  Bits of the AMICTL register                             */
/************************************************************/
#define AMIEN   (BIT_0)
#define BW8     (0x00000)
#define PKDIS   (BIT_3)
#define MSWF    (BIT_4)
#define ACKEN   (BIT_5)
#define WS1     (BIT_6)
#define WS2     (BIT_7)
#define WS3     (BIT_6|BIT_7)
#define WS4     (BIT_8)
#define WS5     (BIT_6|BIT_8)
#define WS6     (BIT_7|BIT_8)
#define WS7     (BIT_6|BIT_7|BIT_8)
#define WS8     (BIT_9)
#define WS9     (BIT_6|BIT_9)
#define WS10    (BIT_7|BIT_9)
#define WS11    (BIT_6|BIT_7|BIT_9)
#define WS12    (BIT_9|BIT_8)
#define WS13    (BIT_6|BIT_8|BIT_9)
#define WS14    (BIT_7|BIT_8|BIT_9)
#define WS15    (BIT_6|BIT_7|BIT_8|BIT_9)
#define WS16    (BIT_10)
#define WS17    (BIT_6|BIT_10)
#define WS18    (BIT_7|BIT_10)
#define WS19    (BIT_6|BIT_7|BIT_10)
#define WS20    (BIT_10|BIT_8)
#define WS21    (BIT_6|BIT_8|BIT_10)
#define WS22    (BIT_7|BIT_8|BIT_10)
#define WS23    (BIT_6|BIT_7|BIT_8|BIT_10)
#define WS24    (BIT_9|BIT_10)
#define WS25    (BIT_6|BIT_9|BIT_10)
#define WS26    (BIT_7|BIT_9|BIT_10)
#define WS27    (BIT_6|BIT_7|BIT_9|BIT_10)
#define WS28    (BIT_8|BIT_9|BIT_10)
#define WS29    (BIT_6|BIT_8|BIT_9|BIT_10)
#define WS30    (BIT_7|BIT_8|BIT_9|BIT_10)
#define WS31    (BIT_6|BIT_7|BIT_8|BIT_9|BIT_10)
#define HC1     (BIT_11)
#define HC2     (BIT_12)
#define HC3     (BIT_11|BIT_12)
#define HC4     (BIT_13)
#define HC5     (BIT_13|BIT_11)
#define HC6     (BIT_13)|(BIT_12)
#define HC7     (BIT_13|BIT_12|BIT_11) 
#define IC1     (BIT_14)
#define IC2     (BIT_15)
#define IC3     (BIT_14|BIT_15)
#define IC4     (BIT_16)
#define IC5     (BIT_14|BIT_16)
#define IC6     (BIT_15|BIT_16)
#define IC7     (BIT_14|BIT_15|BIT_16)
#define AMIFLSH (BIT_17)
#define RHC1    (BIT_18)
#define RHC2    (BIT_19)
#define RHC3    (BIT_18|BIT_19)
#define RHC4    (BIT_20)
#define RHC5    (BIT_20|BIT_18)
#define RHC6    (BIT_20|BIT_19)
#define RHC7    (BIT_20|BIT_18|BIT_19) 
#define PREDIS  (BIT_21)

/* AMISTAT */
#define AMIMS   (BIT_0)
#define AMIS    (BIT_1)
#define AMITXS  (BIT_2)
#define AMIRXS  (BIT_3)

/* DMAC register control bits */
#define DEN         (BIT_0)
#define TRAN        (BIT_1)
#define CHEN        (BIT_2)
#define DLEN        (BIT_3)
#define CBEN        (BIT_4)
#define DFLSH       (BIT_5)
#define WRBEN 		(BIT_7)
#define OFCEN	    (BIT_8)
#define TLEN		(BIT_9)
#define INTIRT      (BIT_12)
#define DFS         (BIT_17|BIT_16)
#define DMAS        (BIT_20)
#define CHS         (BIT_21)
#define TLS         (BIT_22)
#define WBS		    (BIT_23)
#define EXTS		(BIT_24)
#define DIRS		(BIT_25)

/* SPICTL register */
#define TIMOD1  (BIT_0)        /* Use core writes for transfers */
#define TIMOD2  (BIT_1)        /* Use DMA for transfers */
#define SENDZ   (BIT_2)        /* Send zero when SPITX buffer is empty (set) or send last word (clear) */
#define GM      (BIT_3)        /* Overwrite with incoming data when SPIRX is full (set) or discard new data (clear) */
#define ISSEN   (BIT_4)        /* enable Input Slave Select */
#define DMISO   (BIT_5)        /* Disable MISO pin for broadcasts */
#define WL8     (0x00000000)   /* SPI Word Length = 8 */
#define WL16    (BIT_7)        /* SPI Word Length = 16 */
#define WL32    (BIT_8)        /* SPI Word Length = 32 */
#define MSBF    (BIT_9)        /* Send data most significant byte first (set) or LSB first (clear) */
#define CPHASE  (BIT_10)       /* Selects transfer format of SPI clock and SPIDSx control */
#define CLKPL   (BIT_11)       /* Clock polarity - 0=act.high (low when idle), 1=act.low (high when idle) */
#define SPIMS   (BIT_12)       /* Configure SPI as a Master (set) or Slave (clear) */
#define OPD     (BIT_13)       /* Open Drain Output enable */
#define SPIEN   (BIT_14)       /* SPI Port Enable */
#define PACKEN  (BIT_15)       /* Packing enable - 0 = no packing, 1 = 8-/16-bit packing */
#define SGN     (BIT_16)       /* Sign extend data */
#define SMLS    (BIT_17)       /* Seamless transfer enable */
#define TXFLSH  (BIT_18)       /* SPITX buffer flushed */
#define RXFLSH  (BIT_19)       /* SPITX buffer flushed */

/* SPI Control Register for SPI port B that is routed through the DAI (SPICTLB) */
/* SPICTLB[19:0] are the same as SPICTL[19:0] */
#define ILPBK   (BIT_20)   /* Internal loopback enable (set) */
#define AUTOSDS (BIT_21)   /* when cph=1 device sel is controlled by SPI hardware */
#define WTWDEN  (BIT_22)



/* SPISTAT register */
#define SPIF    (BIT_0) /* SPI transmit or receive transfer complete */
#define MME     (BIT_1) /* Multimaster error */
#define TUNF    (BIT_2) /* TXSPI transmission error (underflow) */
#define TXS     (BIT_3) /* TXSPI data buffer status */
#define ROVF    (BIT_4) /* RXSPI reception error (overflow) */
#define RXS     (BIT_5) /* RXSPI data buffer status */
#define TXCOL   (BIT_6) /* TXSPI transmit collision */

/* SPISTATB register */
/* SPISTATB[6:0] are the same as SPISTAT[6:0] */
#define SPIFE   (BIT_7) /* SPI tranaction is complete on the external interface */

/* SPIFLG register - only in effect when SPI port enabled in master mode. Otherwise */
/* FLG0-FLG3 controlled by FLAGS register */
#define DS0EN (BIT_0) /* Enable SRU2 output for SPI device-select-0 when CPHASE=0 */
#define DS1EN (BIT_1) /* Enable SRU2 output for SPI device-select-1 when CPHASE=0 */
#define DS2EN (BIT_2) /* Enable SRU2 output for SPI device-select-2 when CPHASE=0 */
#define DS3EN (BIT_3) /* Enable SRU2 output for SPI device-select-3 when CPHASE=0 */
#define ISSS (BIT_7) /* Reflects status of input slave select pin (dsp as slave) */
#define SPIFLG0 (BIT_8) /* If SPICTL:CPHASE=1, SW must manually control slave select signals */
#define SPIFLG1 (BIT_9) /* using these SPIFLGx bits. If CPHASE=0, device-selects are */
#define SPIFLG2 (BIT_10)/* automatically controlled by the SPI hardware, and these SPIDSx */
#define SPIFLG3 (BIT_11)/* bits are ignored. */

/* SPIDMAC Register */
#define SPIDEN      (BIT_0) /* DMA Enable */
#define SPIRCV      (BIT_1) /* DMA Read when set, Transmit when cleared. */
#define INTEN       (BIT_2) /* Enable Interrupt when DMA transfer complete. */
#define INTETC      (BIT_3) /* interrupt on the external transfer completion */
#define SPICHEN     (BIT_4) /* SPI DMA Chaining enable */
#define FIFOFLSH    (BIT_7) /* clear the 4-deep DMA FIFO (seperate from SPIRX and SPITX buffers) */
#define INTERR      (BIT_8) /* Enable interrupt on error */
#define SPIOVF      (BIT_9) /* Receive Overflow error (data rx'd when rx buffer full) */
#define SPIUNF      (BIT_10)/* Transmit Overflow error */
#define SPIMME      (BIT_11)/* Multi-master error */
#define SPIS0       (BIT_12)/* SPI FIFO buffer status Bit 0 */
#define SPIS1       (BIT_13)/* SPI FIFO buffer status Bit 1 */
#define SPIERRS     (BIT_14)/* DMA error status */
#define SPIDMAS     (BIT_15)/* DMA transfer status */
#define SPICHS      (BIT_16)/* DMA Chaining status */

/* SPORT Registers */
/* SPERRSTAT */
#define SP0_DERRA   (BIT_0)
#define SP0_DERRB   (BIT_1)
#define SP1_DERRA   (BIT_2)
#define SP1_DERRB   (BIT_3)
#define SP2_DERRA   (BIT_4)
#define SP2_DERRB   (BIT_5)
#define SP3_DERRA   (BIT_6)
#define SP3_DERRB   (BIT_7)
#define SP4_DERRA   (BIT_8)
#define SP4_DERRB   (BIT_9)
#define SP5_DERRA   (BIT_10)
#define SP5_DERRB   (BIT_11)
#define SP6_DERRA   (BIT_12)
#define SP6_DERRB   (BIT_13)
#define SP7_DERRA   (BIT_14)
#define SP7_DERRB   (BIT_15)
#define SP0_FSERR   (BIT_16)
#define SP1_FSERR   (BIT_17)
#define SP2_FSERR   (BIT_18)
#define SP3_FSERR   (BIT_19)
#define SP4_FSERR   (BIT_20)
#define SP5_FSERR   (BIT_21)
#define SP6_FSERR   (BIT_22)
#define SP7_FSERR   (BIT_23)

/* SPERRCTL */
#define DERRA_EN        (BIT_0)
#define DERRB_EN        (BIT_1)
#define FSERR_EN        (BIT_2)
#define DERRA_STAT      (BIT_4)
#define DERRB_STAT      (BIT_5)
#define FSERR_STAT      (BIT_6)

/* SPMCTL7-0 registers */
#define MCEA    (BIT_0) /* Multichannel Mode Enable for channel A */
#define MFD0    (0x00000000)   /* no frame delay, multichannel FS pulse in same SCLK cycle as first data bit */
#define MFD1    (BIT_1) /* multichannel mode 1 cycle frame sync delay */
#define MFD2    (BIT_2) /* multichannel mode 2 cycle frame sync delay */
#define MFD3    (BIT_1|BIT_2)   /* multichannel mode 3 cycle frame sync delay */
#define MFD4    (BIT_3)  /* multichannel mode 4 cycle frame sync delay */
#define MFD5    (BIT_1|BIT_3)   /* multichannel mode 5 cycle frame sync delay */
#define MFD6    (BIT_2|BIT_3)   /* multichannel mode 6 cycle frame sync delay */
#define MFD7    (BIT_1|BIT_2|BIT_3) /* multichannel mode 7 cycle frame sync delay */
#define MFD8    (BIT_4) /* multichannel mode 8 cycle frame sync delay */
#define MFD9    (BIT_1|BIT_4)   /* multichannel mode 9 cycle frame sync delay */
#define MFD10   (BIT_2|BIT_4)   /* multichannel mode 10 cycle frame sync delay */
#define MFD11   (BIT_1|BIT_2|BIT_4) /* multichannel mode 11 cycle frame sync delay */
#define MFD12   (BIT_3|BIT_4)   /* multichannel mode 12 cycle frame sync delay */
#define MFD13   (BIT_1|BIT_3|BIT_4) /* multichannel mode 13 cycle frame sync delay */
#define MFD14   (BIT_2|BIT_3|BIT_4) /* multichannel mode 14 cycle frame sync delay */
#define MFD15   (BIT_1|BIT_2|BIT_3|BIT_4)   /* multichannel mode 15 cycle frame sync delay */
#define NCH     (0x00000FE0)   /* Number of MCM channels - 1 */
#define NCH0    (0x00000000)   /* 1 Channel */
#define NCH1    (BIT_5) /* 2 Channels */
#define NCH2    (BIT_6) /* 3 Channels */
#define NCH3    (BIT_5|BIT_6)   /* 4 Channels */
#define NCH4    (BIT_7) /* 5 Channels */
#define NCH5    (BIT_5|BIT_7)   /* 6 Channels */
#define NCH6    (BIT_6|BIT_7)   /* 7 Channels */
#define NCH7    (BIT_5|BIT_6|BIT_7) /* 8 Channels */
#define NCH8    (BIT_8) /* 9 Channels */
#define NCH9    (BIT_5|BIT_8)   /* 10 Channels */
#define NCH10   (BIT_6|BIT_8)   /* 11 Channels */
#define NCH11   (BIT_5|BIT_6|BIT_8) /* 12 Channels */
#define NCH12   (BIT_7|BIT_8)   /* 13 Channels */
#define NCH13   (BIT_5|BIT_7|BIT_8) /* 14 Channels */
#define NCH14   (BIT_6|BIT_7|BIT_8) /* 15 Channels */
#define NCH15   (BIT_5|BIT_6|BIT_7|BIT_8)   /* 16 Channels */
#define NCH16   (BIT_9) /* 17 Channels */
#define NCH17   (BIT_5|BIT_9)   /* 18 Channels */
#define NCH18   (BIT_6|BIT_9)   /* 19 Channels */
#define NCH19   (BIT_5|BIT_6|BIT_9) /* 20 Channels */
#define NCH20   (BIT_7|BIT_9)   /* 21 Channels */
#define NCH21   (BIT_5|BIT_7|BIT_9) /* 22 Channels */
#define NCH22   (BIT_6|BIT_7|BIT_9) /* 23 Channels */
#define NCH23   (BIT_5|BIT_6|BIT_7|BIT_9)   /* 24 Channels */
#define NCH24   (BIT_8|BIT_9)   /* 25 Channels */
#define NCH25   (BIT_5|BIT_8|BIT_9) /* 26 Channels */
#define NCH26   (BIT_6|BIT_8|BIT_9) /* 27 Channels */
#define NCH27   (BIT_5|BIT_6|BIT_8|BIT_9)   /* 28 Channels */
#define NCH28   (BIT_7|BIT_8|BIT_9) /* 29 Channels */
#define NCH29   (BIT_5|BIT_7|BIT_8|BIT_9)   /* 30 Channels */
#define NCH30   (BIT_6|BIT_7|BIT_8|BIT_9)   /* 31 Channels */
#define NCH31   (BIT_5|BIT_6|BIT_7|BIT_8|BIT_9) /* 32 Channels */
#define NCH32   (BIT_10)   /* 33 Channels */
#define NCH33   (BIT_5|BIT_10)  /* 34 Channels */
#define NCH34   (BIT_6|BIT_10)  /* 35 Channels */
#define NCH35   (BIT_5|BIT_6|BIT_10)   /* 36 Channels */
#define NCH36   (BIT_7|BIT_10)  /* 37 Channels */
#define NCH37   (BIT_5|BIT_7|BIT_10)   /* 38 Channels */
#define NCH38   (BIT_6|BIT_7|BIT_10)   /* 39 Channels */
#define NCH39   (BIT_5|BIT_6|BIT_7|BIT_10)  /* 40 Channels */
#define NCH40   (BIT_8|BIT_10)  /* 41 Channels */
#define NCH41   (BIT_5|BIT_8|BIT_10)   /* 42 Channels */
#define NCH42   (BIT_6|BIT_8|BIT_10)   /* 43 Channels */
#define NCH43   (BIT_5|BIT_6|BIT_8|BIT_10)  /* 44 Channels */
#define NCH44   (BIT_7|BIT_8|BIT_10)   /* 45 Channels */
#define NCH45   (BIT_5|BIT_7|BIT_8|BIT_10)  /* 46 Channels */
#define NCH46   (BIT_6|BIT_7|BIT_8|BIT_10)  /* 47 Channels */
#define NCH47   (BIT_5|BIT_6|BIT_7|BIT_8|BIT_10)   /* 48 Channels */
#define NCH48   (BIT_9|BIT_10)  /* 49 Channels */
#define NCH49   (BIT_5|BIT_9|BIT_10)   /* 50 Channels */
#define NCH50   (BIT_6|BIT_9|BIT_10)   /* 51 Channels */
#define NCH51   (BIT_5|BIT_6|BIT_9|BIT_10)  /* 52 Channels */
#define NCH52   (BIT_7|BIT_9|BIT_10)   /* 53 Channels */
#define NCH53   (BIT_5|BIT_7|BIT_9|BIT_10)  /* 54 Channels */
#define NCH54   (BIT_6|BIT_7|BIT_9|BIT_10)  /* 55 Channels */
#define NCH55   (BIT_5|BIT_6|BIT_7|BIT_9|BIT_10)   /* 56 Channels */
#define NCH56   (BIT_8|BIT_9|BIT_10)   /* 57 Channels */
#define NCH57   (BIT_5|BIT_8|BIT_9|BIT_10)  /* 58 Channels */
#define NCH58   (BIT_6|BIT_8|BIT_9|BIT_10)  /* 59 Channels */
#define NCH59   (BIT_5|BIT_6|BIT_8|BIT_9|BIT_10)   /* 60 Channels */
#define NCH60   (BIT_7|BIT_8|BIT_9|BIT_10)  /* 61 Channels */
#define NCH61   (BIT_5|BIT_7|BIT_8|BIT_9|BIT_10)   /* 62 Channels */
#define NCH62   (BIT_6|BIT_7|BIT_8|BIT_9|BIT_10) /* 63 Channels */
#define NCH63   (BIT_5|BIT_6|BIT_7|BIT_8|BIT_9|BIT_10)  /* 64 Channels */
#define NCH64   (BIT_11)   /* 65 Channels */
#define NCH65   (BIT_5|BIT_11)  /* 66 Channels */
#define NCH66   (BIT_6|BIT_11)  /* 67 Channels */
#define NCH67   (BIT_5|BIT_6|BIT_11)   /* 68 Channels */
#define NCH68   (BIT_7|BIT_11)  /* 69 Channels */
#define NCH69   (BIT_5|BIT_7|BIT_11)   /* 70 Channels */
#define NCH70   (BIT_6|BIT_7|BIT_11)   /* 71 Channels */
#define NCH71   (BIT_5|BIT_6|BIT_7|BIT_11)  /* 72 Channels */
#define NCH72   (BIT_8|BIT_11)  /* 73 Channels */
#define NCH73   (BIT_5|BIT_8|BIT_11)   /* 74 Channels */
#define NCH74   (BIT_6|BIT_8|BIT_11)   /* 75 Channels */
#define NCH75   (BIT_5|BIT_6|BIT_8|BIT_11)  /* 76 Channels */
#define NCH76   (BIT_7|BIT_8|BIT_11)   /* 77 Channels */
#define NCH77   (BIT_5|BIT_7|BIT_8|BIT_11)  /* 78 Channels */
#define NCH78   (BIT_6|BIT_7|BIT_8|BIT_11)  /* 79 Channels */
#define NCH79   (BIT_5|BIT_6|BIT_7|BIT_8|BIT_11)   /* 80 Channels */
#define NCH80   (BIT_9|BIT_11)  /* 81 Channels */
#define NCH81   (BIT_5|BIT_9|BIT_11)   /* 82 Channels */
#define NCH82   (BIT_6|BIT_9|BIT_11)   /* 83 Channels */
#define NCH83   (BIT_5|BIT_6|BIT_9|BIT_11)  /* 84 Channels */
#define NCH84   (BIT_7|BIT_9|BIT_11)   /* 85 Channels */
#define NCH85   (BIT_5|BIT_7|BIT_9|BIT_11)  /* 86 Channels */
#define NCH86   (BIT_6|BIT_7|BIT_9|BIT_11)  /* 87 Channels */
#define NCH87   (BIT_5|BIT_6|BIT_7|BIT_9|BIT_11)   /* 88 Channels */
#define NCH88   (BIT_8|BIT_9|BIT_11)   /* 89 Channels */
#define NCH89   (BIT_5|BIT_8|BIT_9|BIT_11)  /* 90 Channels */
#define NCH90   (BIT_6|BIT_8|BIT_9|BIT_11)  /* 91 Channels */
#define NCH91   (BIT_5|BIT_6|BIT_8|BIT_9|BIT_11)   /* 92 Channels */
#define NCH92   (BIT_7|BIT_8|BIT_9|BIT_11)  /* 93 Channels */
#define NCH93   (BIT_5|BIT_7|BIT_8|BIT_9|BIT_11)   /* 94 Channels */
#define NCH94   (BIT_6|BIT_7|BIT_8|BIT_9|BIT_11)   /* 95 Channels */
#define NCH95   (BIT_5|BIT_6|BIT_7|BIT_8|BIT_9|BIT_11)  /* 96 Channels */
#define NCH96   (BIT_10|BIT_11) /* 97 Channel */
#define NCH97   (BIT_5|BIT_10|BIT_11)   /* 98 Channels */
#define NCH98   (BIT_6|BIT_10|BIT_11)   /* 99 Channels */
#define NCH99   (BIT_5|BIT_6|BIT_10|BIT_11) /* 100 Channels */
#define NCH100  (BIT_7|BIT_10|BIT_11)   /* 101 Channels */
#define NCH101  (BIT_5|BIT_7|BIT_10|BIT_11) /* 102 Channels */
#define NCH102  (BIT_6|BIT_7|BIT_10|BIT_11) /* 103 Channels */
#define NCH103  (BIT_5|BIT_6|BIT_7|BIT_10|BIT_11)   /* 104 Channels */
#define NCH104  (BIT_8|BIT_10|BIT_11)   /* 105 Channels */
#define NCH105  (BIT_5|BIT_8|BIT_10|BIT_11) /* 106 Channels */
#define NCH106  (BIT_6|BIT_8|BIT_10|BIT_11) /* 107 Channels */
#define NCH107  (BIT_5|BIT_6|BIT_8|BIT_10|BIT_11)   /* 108 Channels */
#define NCH108  (BIT_7|BIT_8|BIT_10|BIT_11) /* 109 Channels */
#define NCH109  (BIT_5|BIT_7|BIT_8|BIT_10|BIT_11)   /* 110 Channels */
#define NCH110  (BIT_6|BIT_7|BIT_8|BIT_10|BIT_11)   /* 111 Channels */
#define NCH111  (BIT_5|BIT_6|BIT_7|BIT_8|BIT_10|BIT_11) /* 112 Channels */
#define NCH112  (BIT_9|BIT_10|BIT_11)   /* 113 Channels */
#define NCH113  (BIT_5|BIT_9|BIT_10|BIT_11) /* 114 Channels */
#define NCH114  (BIT_6|BIT_9|BIT_10|BIT_11) /* 115 Channels */
#define NCH115  (BIT_5|BIT_6|BIT_9|BIT_10|BIT_11)   /* 116 Channels */
#define NCH116  (BIT_7|BIT_9|BIT_10|BIT_11) /* 117 Channels */
#define NCH117  (BIT_5|BIT_7|BIT_9|BIT_10|BIT_11)   /* 118 Channels */
#define NCH118  (BIT_6|BIT_7|BIT_9|BIT_10|BIT_11)   /* 119 Channels */
#define NCH119  (BIT_5|BIT_6|BIT_7|BIT_9|BIT_10|BIT_11) /* 120 Channels */
#define NCH120  (BIT_8|BIT_9|BIT_10|BIT_11) /* 121 Channels */
#define NCH121  (BIT_5|BIT_8|BIT_9|BIT_10|BIT_11)   /* 122 Channels */
#define NCH122  (BIT_6|BIT_8|BIT_9|BIT_10|BIT_11)   /* 123 Channels */
#define NCH123  (BIT_5|BIT_6|BIT_8|BIT_9|BIT_10|BIT_11) /* 124 Channels */
#define NCH124  (BIT_7|BIT_8|BIT_9|BIT_10|BIT_11)   /* 125 Channels */
#define NCH125  (BIT_5|BIT_7|BIT_8|BIT_9|BIT_10|BIT_11) /* 126 Channels */
#define NCH126  (BIT_6|BIT_7|BIT_8|BIT_9|BIT_10|BIT_11) /* 127 Channels */
#define NCH127  (BIT_5|BIT_6|BIT_7|BIT_8|BIT_9|BIT_10|BIT_11)   /* 128 Channels */
#define SPL     (BIT_12)  /* SPORT 0&2 or SPORT 1&3 Internal Loopback Mode */
#define CHNL    (0x007F0000)   /* Current Channel Status (read-only) */
#define CHNL0   (0x00000000)   /* 1 Channel */
#define CHNL1   (BIT_16)   /* 2 Channels */
#define CHNL2   (BIT_17)   /* 3 Channels */
#define CHNL3   (BIT_16|BIT_17) /* 4 Channels */
#define CHNL4   (BIT_18)   /* 5 Channels */
#define CHNL5   (BIT_16|BIT_18) /* 6 Channels */
#define CHNL6   (BIT_17|BIT_18) /* 7 Channels */
#define CHNL7   (BIT_16|BIT_17|BIT_18)  /* 8 Channels */
#define CHNL8   (BIT_19)   /* 9 Channels */
#define CHNL9   (BIT_16|BIT_19) /* 10 Channels */
#define CHNL10  (BIT_17|BIT_19) /* 11 Channels */
#define CHNL11  (BIT_16|BIT_17|BIT_19)  /* 12 Channels */
#define CHNL12  (BIT_18|BIT_19) /* 13 Channels */
#define CHNL13  (BIT_16|BIT_18|BIT_19)  /* 14 Channels */
#define CHNL14  (BIT_17|BIT_18|BIT_19)  /* 15 Channels */
#define CHNL15  (BIT_16|BIT_17|BIT_18|BIT_19)   /* 16 Channels */
#define CHNL16  (BIT_20)   /* 17 Channels */
#define CHNL17  (BIT_16|BIT_20) /* 18 Channels */
#define CHNL18  (BIT_17|BIT_20) /* 19 Channels */
#define CHNL19  (BIT_16|BIT_17|BIT_20)  /* 20 Channels */
#define CHNL20  (BIT_18|BIT_20) /* 21 Channels */
#define CHNL21  (BIT_16|BIT_18|BIT_20)  /* 22 Channels */
#define CHNL22  (BIT_17|BIT_18|BIT_20)  /* 23 Channels */
#define CHNL23  (BIT_16|BIT_17|BIT_18|BIT_20)   /* 24 Channels */
#define CHNL24  (BIT_19|BIT_20) /* 25 Channels */
#define CHNL25  (BIT_16|BIT_19|BIT_20)  /* 26 Channels */
#define CHNL26  (BIT_17|BIT_19|BIT_20)  /* 27 Channels */
#define CHNL27  (BIT_16|BIT_17|BIT_19|BIT_20)   /* 28 Channels */
#define CHNL28  (BIT_18|BIT_19|BIT_20)  /* 29 Channels */
#define CHNL29  (BIT_16|BIT_18|BIT_19|BIT_20)   /* 30 Channels */
#define CHNL30  (BIT_17|BIT_18|BIT_19|BIT_20)   /* 31 Channels */
#define CHNL31  (BIT_16|BIT_17|BIT_18|BIT_19|BIT_20)   /* 32 Channels */
#define CHNL32  (BIT_21)   /* 33 Channels */
#define CHNL33  (BIT_16|BIT_21) /* 34 Channels */
#define CHNL34  (BIT_17|BIT_21) /* 35 Channels */
#define CHNL35  (BIT_16|BIT_17|BIT_21)  /* 36 Channels */
#define CHNL36  (BIT_18|BIT_21) /* 37 Channels */
#define CHNL37  (BIT_16|BIT_18|BIT_21)  /* 38 Channels */
#define CHNL38  (BIT_17|BIT_18|BIT_21)  /* 39 Channels */
#define CHNL39  (BIT_16|BIT_17|BIT_18|BIT_21)   /* 40 Channels */
#define CHNL40  (BIT_19|BIT_21) /* 41 Channels */
#define CHNL41  (BIT_16|BIT_19|BIT_21)  /* 42 Channels */
#define CHNL42  (BIT_17|BIT_19|BIT_21)  /* 43 Channels */
#define CHNL43  (BIT_16|BIT_17|BIT_19|BIT_21)   /* 44 Channels */
#define CHNL44  (BIT_18|BIT_19|BIT_21)  /* 45 Channels */
#define CHNL45  (BIT_16|BIT_18|BIT_19|BIT_21)   /* 46 Channels */
#define CHNL46  (BIT_17|BIT_18|BIT_19|BIT_21)   /* 47 Channels */
#define CHNL47  (BIT_16|BIT_17|BIT_18|BIT_19|BIT_21)   /* 48 Channels */
#define CHNL48  (BIT_20|BIT_21) /* 49 Channels */
#define CHNL49  (BIT_16|BIT_20|BIT_21)  /* 50 Channels */
#define CHNL50  (BIT_17|BIT_20|BIT_21)  /* 51 Channels */
#define CHNL51  (BIT_16|BIT_17|BIT_20|BIT_21)   /* 52 Channels */
#define CHNL52  (BIT_18|BIT_20|BIT_21)  /* 53 Channels */
#define CHNL53  (BIT_16|BIT_18|BIT_20|BIT_21)   /* 54 Channels */
#define CHNL54  (BIT_17|BIT_18|BIT_20|BIT_21)   /* 55 Channels */
#define CHNL55  (BIT_16|BIT_17|BIT_18|BIT_20|BIT_21)   /* 56 Channels */
#define CHNL56  (BIT_19|BIT_20|BIT_21)  /* 57 Channels */
#define CHNL57  (BIT_16|BIT_19|BIT_20|BIT_21)   /* 58 Channels */
#define CHNL58  (BIT_17|BIT_19|BIT_20|BIT_21)   /* 59 Channels */
#define CHNL59  (BIT_16|BIT_17|BIT_19|BIT_20|BIT_21)   /* 60 Channels */
#define CHNL60  (BIT_18|BIT_19|BIT_20|BIT_21)   /* 61 Channels */
#define CHNL61  (BIT_16|BIT_18|BIT_19|BIT_20|BIT_21)   /* 62 Channels */
#define CHNL62  (BIT_17|BIT_18|BIT_19|BIT_20|BIT_21) /* 63 Channels */
#define CHNL63  (BIT_16|BIT_17|BIT_18|BIT_19|BIT_20|BIT_21) /* 64 Channels */
#define CHNL64  (BIT_22)   /* 65 Channels */
#define CHNL65  (BIT_16|BIT_22) /* 66 Channels */
#define CHNL66  (BIT_17|BIT_22) /* 67 Channels */
#define CHNL67  (BIT_16|BIT_17|BIT_22)  /* 68 Channels */
#define CHNL68  (BIT_18|BIT_22) /* 69 Channels */
#define CHNL69  (BIT_16|BIT_18|BIT_22)  /* 70 Channels */
#define CHNL70  (BIT_17|BIT_18|BIT_22)  /* 71 Channels */
#define CHNL71  (BIT_16|BIT_17|BIT_18|BIT_22)   /* 72 Channels */
#define CHNL72  (BIT_19|BIT_22) /* 73 Channels */
#define CHNL73  (BIT_16|BIT_19|BIT_22)  /* 74 Channels */
#define CHNL74  (BIT_17|BIT_19|BIT_22)  /* 75 Channels */
#define CHNL75  (BIT_16|BIT_17|BIT_19|BIT_22)   /* 76 Channels */
#define CHNL76  (BIT_18|BIT_19|BIT_22)  /* 77 Channels */
#define CHNL77  (BIT_16|BIT_18|BIT_19|BIT_22)   /* 78 Channels */
#define CHNL78  (BIT_17|BIT_18|BIT_19|BIT_22)   /* 79 Channels */
#define CHNL79  (BIT_16|BIT_17|BIT_18|BIT_19|BIT_22)   /* 80 Channels */
#define CHNL80  (BIT_20|BIT_22) /* 81 Channels */
#define CHNL81  (BIT_16|BIT_20|BIT_22)  /* 82 Channels */
#define CHNL82  (BIT_17|BIT_20|BIT_22)  /* 83 Channels */
#define CHNL83  (BIT_16|BIT_17|BIT_20|BIT_22)   /* 84 Channels */
#define CHNL84  (BIT_18|BIT_20|BIT_22)  /* 85 Channels */
#define CHNL85  (BIT_16|BIT_18|BIT_20|BIT_22)   /* 86 Channels */
#define CHNL86  (BIT_17|BIT_18|BIT_20|BIT_22)   /* 87 Channels */
#define CHNL87  (BIT_16|BIT_17|BIT_18|BIT_20|BIT_22)   /* 88 Channels */
#define CHNL88  (BIT_19|BIT_20|BIT_22)  /* 89 Channels */
#define CHNL89  (BIT_16|BIT_19|BIT_20|BIT_22)   /* 90 Channels */
#define CHNL90  (BIT_17|BIT_19|BIT_20|BIT_22)   /* 91 Channels */
#define CHNL91  (BIT_16|BIT_17|BIT_19|BIT_20|BIT_22)   /* 92 Channels */
#define CHNL92  (BIT_18|BIT_19|BIT_20|BIT_22)   /* 93 Channels */
#define CHNL93  (BIT_16|BIT_18|BIT_19|BIT_20|BIT_22)   /* 94 Channels */
#define CHNL94  (BIT_17|BIT_18|BIT_19|BIT_20|BIT_22)   /* 95 Channels */
#define CHNL95  (BIT_16|BIT_17|BIT_18|BIT_19|BIT_20|BIT_22) /* 96 Channels */
#define CHNL96  (BIT_21|BIT_22) /* 97 Channel */
#define CHNL97  (BIT_16|BIT_21|BIT_22)  /* 98 Channels */
#define CHNL98  (BIT_17|BIT_21|BIT_22)  /* 99 Channels */
#define CHNL99  (BIT_16|BIT_17|BIT_21|BIT_22)   /* 100 Channels */
#define CHNL100 (BIT_18|BIT_21|BIT_22)  /* 101 Channels */
#define CHNL101 (BIT_16|BIT_18|BIT_21|BIT_22)   /* 102 Channels */
#define CHNL102 (BIT_17|BIT_18|BIT_21|BIT_22)   /* 103 Channels */
#define CHNL103 (BIT_16|BIT_17|BIT_18|BIT_21|BIT_22)   /* 104 Channels */
#define CHNL104 (BIT_19|BIT_21|BIT_22)  /* 105 Channels */
#define CHNL105 (BIT_16|BIT_19|BIT_21|BIT_22)   /* 106 Channels */
#define CHNL106 (BIT_17|BIT_19|BIT_21|BIT_22)   /* 107 Channels */
#define CHNL107 (BIT_16|BIT_17|BIT_19|BIT_21|BIT_22)   /* 108 Channels */
#define CHNL108 (BIT_18|BIT_19|BIT_21|BIT_22)   /* 109 Channels */
#define CHNL109 (BIT_16|BIT_18|BIT_19|BIT_21|BIT_22)   /* 110 Channels */
#define CHNL110 (BIT_17|BIT_18|BIT_19|BIT_21|BIT_22)   /* 111 Channels */
#define CHNL111 (BIT_16|BIT_17|BIT_18|BIT_19|BIT_21|BIT_22) /* 112 Channels */
#define CHNL112 (BIT_20|BIT_21|BIT_22)  /* 113 Channels */
#define CHNL113 (BIT_16|BIT_20|BIT_21|BIT_22)   /* 114 Channels */
#define CHNL114 (BIT_17|BIT_20|BIT_21|BIT_22)   /* 115 Channels */
#define CHNL115 (BIT_16|BIT_17|BIT_20|BIT_21|BIT_22)   /* 116 Channels */
#define CHNL116 (BIT_18|BIT_20|BIT_21|BIT_22)   /* 117 Channels */
#define CHNL117 (BIT_16|BIT_18|BIT_20|BIT_21|BIT_22)   /* 118 Channels */
#define CHNL118 (BIT_17|BIT_18|BIT_20|BIT_21|BIT_22)   /* 119 Channels */
#define CHNL119 (BIT_16|BIT_17|BIT_18|BIT_20|BIT_21|BIT_22) /* 120 Channels */
#define CHNL120 (BIT_19|BIT_20|BIT_21|BIT_22)   /* 121 Channels */
#define CHNL121 (BIT_16|BIT_19|BIT_20|BIT_21|BIT_22)   /* 122 Channels */
#define CHNL122 (BIT_17|BIT_19|BIT_20|BIT_21|BIT_22)   /* 123 Channels */
#define CHNL123 (BIT_16|BIT_17|BIT_19|BIT_20|BIT_21|BIT_22) /* 124 Channels */
#define CHNL124 (BIT_18|BIT_19|BIT_20|BIT_21|BIT_22)   /* 125 Channels */
#define CHNL125 (BIT_16|BIT_18|BIT_19|BIT_20|BIT_21|BIT_22) /* 126 Channels */
#define CHNL126 (BIT_17|BIT_18|BIT_19|BIT_20|BIT_21|BIT_22) /* 127 Channels */
#define CHNL127 (BIT_16|BIT_17|BIT_18|BIT_19|BIT_20|BIT_21|BIT_22)  /* 128 Channels */
#define MCEB    (BIT_23)   /* Multichannel Mode Enable for channel B */
#define DMAS0A  (BIT_24)  /* SPORT0 Channel A status */
#define DMAS0B  (BIT_25)  /* SPORT0 Channel B status */
#define DMAS1A  (BIT_26)  /* SPORT1 Channel A status */
#define DMAS1B  (BIT_27)  /* SPORT1 Channel B status */
#define DMACHS0A    (BIT_28)  /* SPORT0 Channel A status */
#define DMACHS0B    (BIT_29)  /* SPORT0 Channel B status */
#define DMACHS1A    (BIT_30)  /* SPORT1 Channel A status */
#define DMACHS1B    (BIT_31)  /* SPORT1 Channel B status */
#define DMAS2A  (BIT_24)  /* SPORT2 Channel A status */
#define DMAS2B  (BIT_25)  /* SPORT2 Channel B status */
#define DMAS3A  (BIT_26)  /* SPORT3 Channel A status */
#define DMAS3B  (BIT_27)  /* SPORT3 Channel B status */
#define DMACHS2A    (BIT_28)  /* SPORT2 Channel A status */
#define DMACHS2B    (BIT_29)  /* SPORT2 Channel B status */
#define DMACHS3A    (BIT_30)  /* SPORT3 Channel A status */
#define DMACHS3B    (BIT_31)  /* SPORT3 Channel B status */
#define DMAS4A  (BIT_24)  /* SPORT4 Channel A status */
#define DMAS4B  (BIT_25)  /* SPORT4 Channel B status */
#define DMAS5A  (BIT_26)  /* SPORT5 Channel A status */
#define DMAS5B  (BIT_27)  /* SPORT5 Channel B status */
#define DMACHS4A    (BIT_28)  /* SPORT4 Channel A status */
#define DMACHS4B    (BIT_29)  /* SPORT4 Channel B status */
#define DMACHS5A    (BIT_30)  /* SPORT5 Channel A status */
#define DMACHS5B    (BIT_31)  /* SPORT5 Channel B status */
#define DMAS6A  (BIT_24)  /* SPORT6 Channel A status */
#define DMAS6B  (BIT_25)  /* SPORT6 Channel B status */
#define DMAS7A  (BIT_26)  /* SPORT7 Channel A status */
#define DMAS7B  (BIT_27)  /* SPORT7 Channel B status */
#define DMACHS6A    (BIT_28)  /* SPORT6 Channel A status */
#define DMACHS6B    (BIT_29)  /* SPORT6 Channel B status */
#define DMACHS7A    (BIT_30)  /* SPORT7 Channel A status */
#define DMACHS7B    (BIT_31)  /* SPORT7 Channel B status */

/* SPCTL0, SPCTL1, SPCTL2, SPCTL3, SPCTL4, and SPCTL5 registers */
#define SPEN_A  (BIT_0)  /* SPORT enable primary A channel */
#define DTYPE0  (0x00000000)   /* right justify, fill unused MSBs with 0s */
#define DTYPE1  (BIT_1) /* right justify, sign-extend into unused MSBs */
#define DTYPE2  (BIT_2) /* compand using mu law */
#define DTYPE3  (BIT_1|BIT_2)   /* compand using a law */
#define LSBF    (BIT_3) /* MSB or LSB first */
#define SLEN3   (BIT_5) /* serial length 3 */
#define SLEN4   (BIT_4|BIT_5)   /* serial length 4 */
#define SLEN5   (BIT_6) /* serial length 5 */
#define SLEN6   (BIT_4|BIT_6)   /* serial length 6 */
#define SLEN7   (BIT_5|BIT_6)   /* serial length 7 */
#define SLEN8   (BIT_4|BIT_5|BIT_6) /* serial length 8 */
#define SLEN9   (BIT_7) /* serial length 9 */
#define SLEN10  (BIT_4|BIT_7)   /* serial length 10 */
#define SLEN11  (BIT_5|BIT_7)   /* serial length 11 */
#define SLEN12  (BIT_4|BIT_5|BIT_7) /* serial length 12 */
#define SLEN13  (BIT_6|BIT_7)   /* serial length 13 */
#define SLEN14  (BIT_4|BIT_6|BIT_7) /* serial length 14 */
#define SLEN15  (BIT_5|BIT_6|BIT_7) /* serial length 15 */
#define SLEN16  (BIT_4|BIT_5|BIT_6|BIT_7)   /* serial length 16 */
#define SLEN17  (BIT_8) /* serial length 17 */
#define SLEN18  (BIT_4|BIT_8)   /* serial length 18 */
#define SLEN19  (BIT_5|BIT_8)   /* serial length 19 */
#define SLEN20  (BIT_4|BIT_5|BIT_8) /* serial length 20 */
#define SLEN21  (BIT_6|BIT_8)   /* serial length 21 */
#define SLEN22  (BIT_4|BIT_6|BIT_8) /* serial length 22 */
#define SLEN23  (BIT_5|BIT_6|BIT_8) /* serial length 23 */
#define SLEN24  (BIT_4|BIT_5|BIT_6|BIT_8)   /* serial length 24 */
#define SLEN25  (BIT_7|BIT_8)   /* serial length 25 */
#define SLEN26  (BIT_4|BIT_7|BIT_8) /* serial length 26 */
#define SLEN27  (BIT_5|BIT_7|BIT_8) /* serial length 27 */
#define SLEN28  (BIT_4|BIT_5|BIT_7|BIT_8)   /* serial length 28 */
#define SLEN29  (BIT_6|BIT_7|BIT_8) /* serial length 29 */
#define SLEN30  (BIT_4|BIT_6|BIT_7|BIT_8)   /* serial length 30 */
#define SLEN31  (BIT_5|BIT_6|BIT_7|BIT_8)   /* serial length 31 */
#define SLEN32  (BIT_4|BIT_5|BIT_6|BIT_7|BIT_8) /* serial length 32 */
#define PACK    (BIT_9) /* 16-to-32 data packing */
#define MSTR    (BIT_10)   /* I2S Mode only... TX/RX is master or slave */
#define ICLK    (BIT_10)   /* internally ('1') or externally ('0') generated transmit or recieve SCLKx */
#define OPMODE  (BIT_11)   /* I2S mode enable ('1') or DSP Serial Mode/Multichannel mode ('0') */
#define CKRE    (BIT_12)   /* Clock edge for data and frame sync sampling (rx) or driving (tx) */
#define FSR     (BIT_13)   /* transmit or receive frame sync (FSx) required */
#define IFS     (BIT_14)   /* internally generated transmit or receive frame sync (FSx) */
#define IMFS    (BIT_14)   /* internally generated multichannel frame sync (FSx) */
#define IRFS    (BIT_14)   /* internally generated receive FS0 or FS1 in multichannel mode */
#define DITFS   (BIT_15)   /* (I2S & DSP serial modes only) Data Independent 'tx' FSx when DDIR bit = 1 */
#define DIFS    (BIT_15)   /* (I2S & DSP serial modes only) Data Independent FSx */
#define LFS     (BIT_16)   /* Active Low transmit or receive frame sync (FSx) */
#define LRFS    (BIT_16)  /* SPORT0 and SPORT1 active low TDM frame sync FS0/FS1 in MC mode */
#define LTDV    (BIT_16)   /* (MC Mode only) SPORT2/SPORT3 tx data valid ena in TDM mode - TDV2/TDV3 alternate pin config */
#define LMFS    (BIT_16)   /* (MC Mode only) active low TDM frame sync */
#define L_FIRST (BIT_16)   /* (I2S Mode Only) transmit left channel first ('1'), or right channel first ('0') */
#define LAFS    (BIT_17)   /* (DSP Serial Mode only) Late (vs early) frame sync FSx */
#define SDEN_A  (BIT_18)  /* SPORT TXnA/RXnA DMA enable primary A channel */
#define SCHEN_A (BIT_19)  /* SPORT TXnA/RXnA DMA chaining enable primary A channel */
#define SDEN_B  (BIT_20)  /* SPORT TXnB/RXnB DMA enable primary B channel */
#define SCHEN_B (BIT_21)  /* SPORT TXnB/RXnB DMA chaining enable primary B channel */
#define FS_BOTH (BIT_22)   /* (DSP Serial & I2S modes only) Issue FSx only if data is in both TXnA & TXnB regs */
#define BHD     (BIT_23)   /* Ignore a core hang */
#define SPEN_B  (BIT_24)  /* SPORTx secondary B channel enable */
#define SPTRAN  (BIT_25)  /* SPORT data buffer data dirrection '1' = transmitter, '0' = receiver */
#define DERR_B  (BIT_26)  /* SPORTx secondary B overflow/underflow error status in DSP serial & I2S modes (read-only) */
#define TUVF_B  (BIT_26)  /* SPORTx secondary B overflow/underflow error status in Multichannel mode (read-only) */
#define DXS0_B  (BIT_27)  /* SPORTx secondary B data buffer status in DSP Serial & I2S modes (read-only) */
#define DXS1_B  (BIT_28)  /* SPORTx secondary B data buffer status in DSP Serial & I2S modes (read-only) */
#define TXS0_B  (BIT_27)  /* SPORTx secondary B data buffer status in Multichannel mode (read-only) */
#define TXS1_B  (BIT_28)  /* SPORTx secondary B data buffer status in Multichannel mode (read-only) */
#define DERR_A  (BIT_29)  /* SPORTx primary A overflow/underflow error status in DSP Serial & I2S modes (read-only) */
#define TUVF_A  (BIT_29)  /* SPORT2 and SPORT3 TX2A/TX3A underflow status in MC mode (read-only, sticky) */
#define ROVF_A  (BIT_29)  /* SPORT0 and SPORT1 RX0A/RX1A overflow status in MC mode (read-only, sticky) */
#define DXS0_A  (BIT_30)  /* SPORTx primary A data buffer status in DSP serial and I2S modes (read-only) */
#define DXS1_A  (BIT_31)  /* SPORTx primary A data buffer status in DSP serial and I2S modes (read-only) */
#define RXS0_A  (BIT_30)  /* SPORT0 and SPORT1 RX0A/RX1A data buffer status in MC mode (read-only) */
#define RXS1_A  (BIT_31)  /* SPORT0 and SPORT1 RX0A/RX1A data buffer status in MC mode (read-only) */
#define TXS0_A  (BIT_30)  /* SPORT2 and SPORT3 TX2A/TX3A data buffer status in MC mode (read-only) */
#define TXS1_A  (BIT_31)  /* SPORT 2 and SPORT3 TX2A/TX3A data buffer status in MC mode (read-only) */


/* SPCTLN0,SPCTLN1,SPCTLN2,SPCTLN3,SPCTLN4,SPCTLN5,SPCTLN6,SPCTLN7*/

#define I2SEFE     (BIT_0)   /* generate the last LRCLK if configured as I2S master (valid only for DMA), if the I2S_EFE bit is set   */
#define ETDINTEN   (BIT_1)   /* External Transfer Done Interrupt for TX DMA */
#define FSED       (BIT_2)   /* Frame sync edge detection in TDM mode */


/* GP Timer Status register TM0STAT, TM1STAT */
#define TIM0IRQ (BIT_0) /* Clear Timer 0 interrupt */
#define TIM1IRQ (BIT_1) /* Clear Timer 1 interrupt */
#define TIM0OVF (BIT_4) /* Timer 0 Overflow error */
#define TIM1OVF (BIT_5) /* Timer 1 Overflow error */
#define TIM0EN  (BIT_8) /* Enable Timer 0 */
#define TIM0DIS (BIT_9) /* Disable Timer 0 */
#define TIM1EN  (BIT_10) /* Enable Timer 1 */
#define TIM1DIS (BIT_11) /* Disable Timer 1 */

/* GP Timer Control register TM0CTL, TM1CTL */
#define TIMODEPWM   (BIT_0) /* Timer mode - PWM_OUT mode */
#define TIMODEW     (BIT_1) /* Timer mode - WIDTH_CAP mode */
#define TIMODEEXT   (BIT_0|BIT_1)   /* Timer mode - EXT_CLK mode */
#define PULSE       (BIT_2) /* Pulse edge select */
#define PRDCNT      (BIT_3) /* Period Count */
#define IRQEN       (BIT_4) /* Interrupt enable */

/* Power management control register (PMCTL) */
#define PLLM1   (BIT_0)        /* PLL Multiplier 1 */
#define PLLM2   (BIT_1)        /* PLL Mulitplier 2 */
#define PLLM3   (BIT_0|BIT_1)   /* PLL Multiplier 3 */
#define PLLM4   (BIT_2) /* PLL Multiplier 4 */
#define PLLM5   (BIT_0|BIT_2)   /* PLL Multiplier 5 */
#define PLLM6   (BIT_1|BIT_2)   /* PLL Multiplier 6 */
#define PLLM7   (BIT_0|BIT_1|BIT_2) /* PLL Multiplier 7 */
#define PLLM8   (BIT_3) /* PLL Multiplier 8 */
#define PLLM9   (BIT_0|BIT_3)   /* PLL Multiplier 9 */
#define PLLM10  (BIT_1|BIT_3)   /* PLL Multiplier 10 */
#define PLLM11  (BIT_0|BIT_1|BIT_3) /* PLL Multiplier 11 */
#define PLLM12  (BIT_2|BIT_3)   /* PLL Multiplier 12 */
#define PLLM13  (BIT_0|BIT_2|BIT_3) /* PLL Multiplier 13 */
#define PLLM14  (BIT_1|BIT_2|BIT_3) /* PLL Multiplier 14 */
#define PLLM15  (BIT_0|BIT_1|BIT_2|BIT_3)   /* PLL Multiplier 15 */
#define PLLM16  (BIT_4) /* PLL Multiplier 16 */
#define PLLM17  (BIT_0|BIT_4)   /* PLL Multiplier 17 */
#define PLLM18  (BIT_1|BIT_4)   /* PLL Multiplier 18 */
#define PLLM19  (BIT_0|BIT_1|BIT_4) /* PLL Multiplier 19 */
#define PLLM20  (BIT_2|BIT_4)   /* PLL Multiplier 20 */
#define PLLM21  (BIT_0|BIT_2|BIT_4) /* PLL Multiplier 21 */
#define PLLM22  (BIT_1|BIT_2|BIT_4) /* PLL Multiplier 22 */
#define PLLM23  (BIT_0|BIT_1|BIT_2|BIT_4)   /* PLL Multiplier 23 */
#define PLLM24  (BIT_3|BIT_4)   /* PLL Multiplier 24 */
#define PLLM25  (BIT_0|BIT_3|BIT_4) /* PLL Multiplier 25 */
#define PLLM26  (BIT_1|BIT_3|BIT_4) /* PLL Multiplier 26 */
#define PLLM27  (BIT_0|BIT_1|BIT_3|BIT_4)   /* PLL Multiplier 27 */
#define PLLM28  (BIT_2|BIT_3|BIT_4) /* PLL Multiplier 28 */
#define PLLM29  (BIT_0|BIT_2|BIT_3|BIT_4)   /* PLL Multiplier 29 */
#define PLLM30  (BIT_1|BIT_2|BIT_3|BIT_4)   /* PLL Multiplier 30 */
#define PLLM31  (BIT_0|BIT_1|BIT_2|BIT_3|BIT_4) /* PLL Multiplier 31 */
#define PLLM32  (BIT_5) /* PLL Multiplier 32 */
#define PLLM33  (BIT_0|BIT_5)   /* PLL Multiplier 33 */
#define PLLM34  (BIT_1|BIT_5)   /* PLL Multiplier 34 */
#define PLLM35  (BIT_0|BIT_1|BIT_5) /* PLL Multiplier 35 */
#define PLLM36  (BIT_2|BIT_5)   /* PLL Multiplier 36 */
#define PLLM37  (BIT_0|BIT_2|BIT_5) /* PLL Multiplier 37 */
#define PLLM38  (BIT_1|BIT_2|BIT_5) /* PLL Multiplier 38 */
#define PLLM39  (BIT_0|BIT_1|BIT_2|BIT_5)   /* PLL Multiplier 39 */
#define PLLM40  (BIT_3|BIT_5)   /* PLL Multiplier 40 */
#define PLLM41  (BIT_0|BIT_3|BIT_5) /* PLL Multiplier 41 */
#define PLLM42  (BIT_1|BIT_3|BIT_5) /* PLL Multiplier 42 */
#define PLLM43  (BIT_0|BIT_1|BIT_3|BIT_5)   /* PLL Multiplier 43 */
#define PLLM44  (BIT_2|BIT_3|BIT_5) /* PLL Multiplier 44 */
#define PLLM45  (BIT_0|BIT_2|BIT_3|BIT_5)   /* PLL Multiplier 45 */
#define PLLM46  (BIT_1|BIT_2|BIT_3|BIT_5)   /* PLL Multiplier 46 */
#define PLLM47  (BIT_0|BIT_1|BIT_2|BIT_3|BIT_5) /* PLL Multiplier 47 */
#define PLLM48  (BIT_4|BIT_5)   /* PLL Multiplier 48 */
#define PLLM49  (BIT_0|BIT_4|BIT_5) /* PLL Multiplier 49 */
#define PLLM50  (BIT_1|BIT_4|BIT_5) /* PLL Multiplier 50 */
#define PLLM51  (BIT_0|BIT_1|BIT_4|BIT_5)   /* PLL Multiplier 51 */
#define PLLM52  (BIT_2|BIT_4|BIT_5) /* PLL Multiplier 52 */
#define PLLM53  (BIT_0|BIT_2|BIT_4|BIT_5)   /* PLL Multiplier 53 */
#define PLLM54  (BIT_1|BIT_2|BIT_4|BIT_5)   /* PLL Multiplier 54 */
#define PLLM55  (BIT_0|BIT_1|BIT_2|BIT_4|BIT_5) /* PLL Multiplier 55 */
#define PLLM56  (BIT_3|BIT_4|BIT_5) /* PLL Multiplier 56 */
#define PLLM57  (BIT_0|BIT_3|BIT_4|BIT_5)   /* PLL Multiplier 57 */
#define PLLM58  (BIT_1|BIT_3|BIT_4|BIT_5)   /* PLL Multiplier 58 */
#define PLLM59  (BIT_0|BIT_1|BIT_3|BIT_4|BIT_5) /* PLL Multiplier 59 */
#define PLLM60  (BIT_2|BIT_3|BIT_4|BIT_5)   /* PLL Multiplier 60 */
#define PLLM61  (BIT_0|BIT_2|BIT_3|BIT_4|BIT_5) /* PLL Multiplier 61 */
#define PLLM62  (BIT_1|BIT_2|BIT_3|BIT_4|BIT_5) /* PLL Multiplier 62 */
#define PLLM63  (BIT_0|BIT_1|BIT_2|BIT_3|BIT_4|BIT_5)   /* PLL Multiplier 63 */
#define PLLM64  (0x00000000)   /* PLL Multiplier 64 */
#define PLLD2   (0x00000000)   /* PLL Divisor 2 */
#define PLLD4   (BIT_6)        /* PLL Divisor 4 */
#define PLLD8   (BIT_7)        /* PLL Divisor 8 */
#define PLLD16   (BIT_6|BIT_7)   /* PLL Divisor 16 */
#define INDIV   (BIT_8)        /* Input divider */
#define DIVEN   (BIT_9)        /* Enable PLL Divisor */
#define CLKOUTEN (BIT_12)      /* Mux select for CLKOUT/RESETOUT */
#define PLLBP    (BIT_15)      /* PLL Bypass mode indiciation */
#define CRAT0    (BIT_16)      /* PLL Clock ratio (Bit 0) */
#define CRAT1    (BIT_17)      /* PLL Clock ratio (Bit 1) */ 

#define DDR2CKR2   (0x00000000)   /* CK - DDR2 DRAM CLK ratio: 2 */
#define DDR2CKR3   (BIT_19)      /* CK - DDR2 DRAM CLK ratio: 3.0 */
#define DDR2CKR4   (BIT_20)      /* CK - DDR2 DRAM CLK ratio: 4.0 */
#define LPCKR2     (0x00000000)
#define LPCKR2_5   (BIT_21)
#define LPCKR3     (BIT_22)
#define LPCKR4     (BIT_21|BIT_22)
#define MLBSYSCKR4 (BIT_23)

/* Extended Power management control register (PMCTL1) */

#define UART0OFF (BIT_0)      /* Shutdown clock to UART0 */
#define TWIOFF   (BIT_1)      /* Shutdown clock to TWI */
#define PWMOFF   (BIT_2)      /* Shutdown clock to PWM */
#define DAIOFF   (BIT_4)      /* Shutdown clock to SRC */
#define EPOFF    (BIT_5)      /* Shutdown clock to External Port (SDRAM) */
#define SP01OFF  (BIT_6)      /* Shutdown clock to SPORT 01 */
#define SP23OFF  (BIT_7)      /* Shutdown clock to SPORT 23 */
#define SP45OFF  (BIT_8)      /* Shutdown clock to SPORT 45 */
#define SP67OFF  (BIT_9)      /* Shutdown clock to SPORT 67 */
#define SPIOFF   (BIT_10)      /* Shutdown clock to SPI */
#define TMROFF   (BIT_11)      /* Shutdown clock to Timer */
#define LP0OFF   (BIT_12)      /* Shutdown clock to link port 0 */
#define LP1OFF   (BIT_13)      /* Shutdown clock to link port 1 */
#define ACCOFF   (BIT_16)      /* Shutdown clock to FIR/IIR/FFT hardware accelerators */
#define FIRACCSEL (0x00000000)     /* Select FIR accelerator */
#define IIRACCSEL (BIT_17)     /* Select IIR accelerator */
#define FFTACCSEL (BIT_18) /* Select FFT accelerator */
#define MLBOFF (BIT_19)      /* Shutdown clock to MLB */


/* DAI Interrupt Latch Register (DAI_IRPTL_FE, DAI_IRPTL_RE, DAI_IRPTL_PRI, */
/*                                  DAI_IRPTL_H, DAI_IRPTL_L) */
#define DIR_VALID_INT       (BIT_0)   /* SPDIF RX Validity interrupt */
#define DIR_LOCK_INT        (BIT_1)   /* SPDIF RX Lock interrupt */
#define DIR_NOSTREAM_INT    (BIT_2)   /* SPDIF RX No Stream interrupt */
#define DIR_CRCERROR_INT    (BIT_3)   /* SPDIF RX CRC Error interrupt */
#define DIR_NOAUDIO_INT     (BIT_4)   /* SPDIF RX NO AUDIO interrupt */
#define DIR_EMPHASIS_INT    (BIT_5)   /* SPDIF RX EMPHASIS interrupt */
#define DIR_ERROR_INT       (BIT_6)   /* SPDIF RX Parity OR Biphase Error interrupt */
#define DIR_STATCNG_INT     (BIT_7)   /* SPDIF RX Status Change interrupt */
#define IDP_FIFO_GTN_INT    (BIT_8)   /* IDP FIFO Samples Exceeded Interrupt (Not FE) */
#define IDP_FIFO_OVR_INT    (BIT_9)   /* IDP FIFO Overflow Interrupt (Not FE) */
#define IDP_DMA0_INT        (BIT_10)  /* IDP DMA0 Interrupt (Not FE) */
#define IDP_DMA1_INT        (BIT_11)  /* IDP DMA1 Interrupt (Not FE) */
#define IDP_DMA2_INT        (BIT_12)  /* IDP DMA2 Interrupt (Not FE) */
#define IDP_DMA3_INT        (BIT_13)  /* IDP DMA3 Interrupt (Not FE) */
#define IDP_DMA4_INT        (BIT_14)  /* IDP DMA4 Interrupt (Not FE) */
#define IDP_DMA5_INT        (BIT_15)  /* IDP DMA5 Interrupt (Not FE) */
#define IDP_DMA6_INT        (BIT_16)  /* IDP DMA6 Interrupt (Not FE) */
#define IDP_DMA7_INT        (BIT_17)  /* IDP DMA7 Interrupt (Not FE) */
#define SRC0_MUTE_INT       (BIT_18)  /* SRC0 MUTE OUT interrupt */
#define SRC1_MUTE_INT       (BIT_19)  /* SRC1 MUTE OUT interrupt */
#define SRC2_MUTE_INT       (BIT_20)  /* SRC2 MUTE OUT interrupt */
#define SRC3_MUTE_INT       (BIT_21)  /* SRC3 MUTE OUT interrupt */
#define SRU_EXTMISCB0_INT   (BIT_22)  /* EXT_MISCB_0 Interrupt */
#define SRU_EXTMISCB1_INT   (BIT_23)  /* EXT_MISCB_1 Interrupt */
#define SRU_EXTMISCB2_INT   (BIT_24)  /* EXT_MISCB_2 Interrupt */
#define SRU_EXTMISCB3_INT   (BIT_25)  /* EXT_MISCB_3 Interrupt */
#define SRU_EXTMISCB4_INT   (BIT_26)  /* EXT_MISCB_4 Interrupt */
#define SRU_EXTMISCB5_INT   (BIT_27)  /* EXT_MISCB_5 Interrupt */
#define SRU_EXTMISCA0_INT   (BIT_28)  /* EXT_MISCA_0 Interrupt */
#define SRU_EXTMISCA1_INT   (BIT_29)  /* EXT_MISCA_1 Interrupt */
#define SRU_EXTMISCA2_INT   (BIT_30)  /* EXT_MISCA_2 Interrupt */
#define SRU_EXTMISCA3_INT   (BIT_31)  /* EXT_MISCA_3 Interrupt */

/* Aliases of SRU_EXTMISC*_INT (above) */
#define DAI_INT_22 (BIT_22) /* EXT_MISCB_0 Interrupt */
#define DAI_INT_23 (BIT_23) /* EXT_MISCB_1 Interrupt */
#define DAI_INT_24 (BIT_24) /* EXT_MISCB_2 Interrupt */
#define DAI_INT_25 (BIT_25) /* EXT_MISCB_3 Interrupt */
#define DAI_INT_26 (BIT_26) /* EXT_MISCB_4 Interrupt */
#define DAI_INT_27 (BIT_27) /* EXT_MISCB_5 Interrupt */
#define DAI_INT_28 (BIT_28) /* EXT_MISCA_0 Interrupt */
#define DAI_INT_29 (BIT_29) /* EXT_MISCA_1 Interrupt */
#define DAI_INT_30 (BIT_30) /* EXT_MISCA_2 Interrupt */
#define DAI_INT_31 (BIT_31) /* EXT_MISCA_3 Interrupt */

/* IDP Control Register 0 (IDP_CTL0) */
#define IDP_BHD     (BIT_4)  /* Buffer Hang Disable for IDP */
#define IDP_DMA_EN  (BIT_5)  /* Global Enable for IDP DMA */
#define IDP_CLROVR  (BIT_6)  /* Clear IDP Overflow */
#define IDP_EN      (BIT_7)  /* Global IDP Enable */

/* IDP PDAP Control Register (IDP_PP_CTL) */
#define IDP_P01_PDAPMASK (BIT_0)     /* Mask PDAP Bit 1 */
#define IDP_P02_PDAPMASK (BIT_1)     /* Mask PDAP Bit 2 */
#define IDP_P03_PDAPMASK (BIT_2)     /* Mask PDAP Bit 3 */
#define IDP_P04_PDAPMASK (BIT_3)     /* Mask PDAP Bit 4 */
#define IDP_P05_PDAPMASK (BIT_4)     /* Mask PDAP Bit 5 */
#define IDP_P06_PDAPMASK (BIT_5)     /* Mask PDAP Bit 6 */
#define IDP_P07_PDAPMASK (BIT_6)     /* Mask PDAP Bit 7 */
#define IDP_P08_PDAPMASK (BIT_7)     /* Mask PDAP Bit 8 */
#define IDP_P09_PDAPMASK (BIT_8)     /* Mask PDAP Bit 9 */
#define IDP_P10_PDAPMASK (BIT_9)     /* Mask PDAP Bit 10 */
#define IDP_P11_PDAPMASK (BIT_10)    /* Mask PDAP Bit 11 */
#define IDP_P12_PDAPMASK (BIT_11)    /* Mask PDAP Bit 12 */
#define IDP_P13_PDAPMASK (BIT_12)    /* Mask PDAP Bit 13 */
#define IDP_P14_PDAPMASK (BIT_13)    /* Mask PDAP Bit 14 */
#define IDP_P15_PDAPMASK (BIT_14)    /* Mask PDAP Bit 15 */
#define IDP_P16_PDAPMASK (BIT_15)    /* Mask PDAP Bit 16 */
#define IDP_P17_PDAPMASK (BIT_16)    /* Mask PDAP Bit 17 */
#define IDP_P18_PDAPMASK (BIT_17)    /* Mask PDAP Bit 18 */
#define IDP_P19_PDAPMASK (BIT_18)    /* Mask PDAP Bit 19 */
#define IDP_P20_PDAPMASK (BIT_19)    /* Mask PDAP Bit 20 */
#define IDP_PDAPMASK     (0x000FFFFF)     /* Mask all PDAP Bits */
#define IDP_PP_SELECT    (BIT_26)         /* Select Parallel Port Pins for PDAP */
#define IDP_PDAP_PACKING0 (0x00000000)    /* Packing Mode 0 */
#define IDP_PDAP_PACKING1 (BIT_27)        /* Packing Mode 1 */
#define IDP_PDAP_PACKING2 (BIT_28)        /* Packing Mode 2 */
#define IDP_PDAP_PACKING3 (BIT_27|BIT_28) /* Packing Mode 3 */
#define IDP_PDAP_CLKEDGE  (BIT_29)        /* Select Clock Edge */
#define IDP_PDAP_RESET    (BIT_30)        /* PDAP Reset */
#define IDP_PDAP_EN       (BIT_31)        /* Enable PDAP on IDP Channel 0 */

/*IDP Control Register 1 (IDP_CTL1) */
#define IDP_EN0 (BIT_0) /*Enable for IDP Channel 0 */
#define IDP_EN1 (BIT_1) /*Enable for IDP Channel 1 */
#define IDP_EN2 (BIT_2) /*Enable for IDP Channel 2 */
#define IDP_EN3 (BIT_3) /*Enable for IDP Channel 3 */
#define IDP_EN4 (BIT_4) /*Enable for IDP Channel 4 */
#define IDP_EN5 (BIT_5) /*Enable for IDP Channel 5 */
#define IDP_EN6 (BIT_6) /*Enable for IDP Channel 6 */
#define IDP_EN7 (BIT_7) /*Enable for IDP Channel 7 */
#define IDP_DMA_EN0 (BIT_8) /*Enable DMA for IDP Channel 0 */
#define IDP_DMA_EN1 (BIT_9) /*Enable DMA for IDP Channel 1 */
#define IDP_DMA_EN2 (BIT_10) /*Enable DMA for IDP Channel 2 */
#define IDP_DMA_EN3 (BIT_11) /*Enable DMA for IDP Channel 3 */
#define IDP_DMA_EN4 (BIT_12) /*Enable DMA for IDP Channel 4 */
#define IDP_DMA_EN5 (BIT_13) /*Enable DMA for IDP Channel 5 */
#define IDP_DMA_EN6 (BIT_14) /*Enable DMA for IDP Channel 6 */
#define IDP_DMA_EN7 (BIT_15) /*Enable DMA for IDP Channel 7 */
#define IDP_PING0   (BIT_16) /*Enable Pin Pong DMA for IDP Channel 0 */
#define IDP_PING1   (BIT_17) /*Enable Pin Pong DMA for IDP Channel 1 */
#define IDP_PING2   (BIT_18) /*Enable Pin Pong DMA for IDP Channel 2 */
#define IDP_PING3   (BIT_19) /*Enable Pin Pong DMA for IDP Channel 3 */
#define IDP_PING4   (BIT_20) /*Enable Pin Pong DMA for IDP Channel 4 */
#define IDP_PING5   (BIT_21) /*Enable Pin Pong DMA for IDP Channel 5 */
#define IDP_PING6   (BIT_22) /*Enable Pin Pong DMA for IDP Channel 6 */
#define IDP_PING7   (BIT_23) /*Enable Pin Pong DMA for IDP Channel 7 */
#define IDP_INTEN   (BIT_24) /* enable for independent channel sync */
#define IDP_FFCLR   (BIT_31) /*Clear IDP FIFP (set) */


/* IDP Control register 2 (IDP_CTL2) */
#define IDP_FAE0    (BIT_0)   /* First active edge for channel 0 */
#define IDP_FAE1    (BIT_1)   /* First active edge for channel 1 */
#define IDP_FAE2    (BIT_2)   /* First active edge for channel 2 */
#define IDP_FAE3    (BIT_3)   /* First active edge for channel 3 */
#define IDP_FAE4    (BIT_4)   /* First active edge for channel 4 */
#define IDP_FAE5    (BIT_5)   /* First active edge for channel 5 */
#define IDP_FAE6    (BIT_6)   /* First active edge for channel 6 */
#define IDP_FAE7    (BIT_7)   /* First active edge for channel 7 */

/* DAI Pin Buffer Status Register (DAI_PIN_STAT) */
#define DAI_PB01 (BIT_0)    /* Status of Pin 1 */
#define DAI_PB02 (BIT_1)    /* Status of Pin 2 */
#define DAI_PB03 (BIT_2)    /* Status of Pin 3 */
#define DAI_PB04 (BIT_3)    /* Status of Pin 4 */
#define DAI_PB05 (BIT_4)    /* Status of Pin 5 */
#define DAI_PB06 (BIT_5)    /* Status of Pin 6 */
#define DAI_PB07 (BIT_6)    /* Status of Pin 7 */
#define DAI_PB08 (BIT_7)    /* Status of Pin 8 */
#define DAI_PB09 (BIT_8)    /* Status of Pin 9 */
#define DAI_PB10 (BIT_9)    /* Status of Pin 10 */
#define DAI_PB11 (BIT_10)   /* Status of Pin 11 */
#define DAI_PB12 (BIT_11)   /* Status of Pin 12 */
#define DAI_PB13 (BIT_12)   /* Status of Pin 13 */
#define DAI_PB14 (BIT_13)   /* Status of Pin 14 */
#define DAI_PB15 (BIT_14)   /* Status of Pin 15 */
#define DAI_PB16 (BIT_15)   /* Status of Pin 16 */
#define DAI_PB17 (BIT_16)   /* Status of Pin 17 */
#define DAI_PB18 (BIT_17)   /* Status of Pin 18 */
#define DAI_PB19 (BIT_18)   /* Status of Pin 19 */
#define DAI_PB20 (BIT_19)   /* Status of Pin 20 */

/* DAI Status Register (DAI_STAT0) */
#define SRU_PING0_STAT (BIT_0)          /* Ping Pong DMA status Channel 0 */
#define SRU_PING1_STAT (BIT_1)          /* Ping Pong DMA status Channel 1 */
#define SRU_PING2_STAT (BIT_2)          /* Ping Pong DMA status Channel 2 */
#define SRU_PING3_STAT (BIT_3)          /* Ping Pong DMA status Channel 3 */
#define SRU_PING4_STAT (BIT_4)          /* Ping Pong DMA status Channel 4 */
#define SRU_PING5_STAT (BIT_5)          /* Ping Pong DMA status Channel 5 */
#define SRU_PING6_STAT (BIT_6)          /* Ping Pong DMA status Channel 6 */
#define SRU_PING7_STAT (BIT_7)          /* Ping Pong DMA status Channel 7 */
#define SRU_OVF0       (BIT_8)          /* Sticky Overflow status */
#define SRU_OVF1       (BIT_9)          /* Sticky Overflow status */
#define SRU_OVF2       (BIT_10)         /* Sticky Overflow status */
#define SRU_OVF3       (BIT_11          /* Sticky Overflow status */
#define SRU_OVF4       (BIT_12)         /* Sticky Overflow status */
#define SRU_OVF5       (BIT_13)         /* Sticky Overflow status */
#define SRU_OVF6       (BIT_14)         /* Sticky Overflow status */
#define SRU_OVF7       (BIT_15)         /* Sticky Overflow status */
#define IDP_DMA0_STAT (BIT_17)             /* IDP DMA0 Status */
#define IDP_DMA1_STAT (BIT_18)             /* IDP DMA1 Status */
#define IDP_DMA2_STAT (BIT_19)             /* IDP DMA2 Status */
#define IDP_DMA3_STAT (BIT_20)             /* IDP DMA3 Status */
#define IDP_DMA4_STAT (BIT_21)             /* IDP DMA4 Status */
#define IDP_DMA5_STAT (BIT_22)             /* IDP DMA5 Status */
#define IDP_DMA6_STAT (BIT_23)             /* IDP DMA6 Status */
#define IDP_DMA7_STAT (BIT_24)             /* IDP DMA7 Status */
#define IDP_FIFOSZ (BIT_28|BIT_29|BIT_30|BIT_31)   /* IDP FIFO size */

/* DAI_STAT1 8-bit register */
#define FFWRINDEX (BIT_0|BIT_1|BIT_2|BIT_3)
#define FFRDINDEX (BIT_4|BIT_5|BIT_6|BIT_7)


/* DPI Pin Buffer Status Register (DPI_PIN_STAT) */
#define DPI_PB01 (BIT_0)    /* Status of Pin 1 */
#define DPI_PB02 (BIT_1)    /* Status of Pin 2 */
#define DPI_PB03 (BIT_2)    /* Status of Pin 3 */
#define DPI_PB04 (BIT_3)    /* Status of Pin 4 */
#define DPI_PB05 (BIT_4)    /* Status of Pin 5 */
#define DPI_PB06 (BIT_5)    /* Status of Pin 6 */
#define DPI_PB07 (BIT_6)    /* Status of Pin 7 */
#define DPI_PB08 (BIT_7)    /* Status of Pin 8 */
#define DPI_PB09 (BIT_8)    /* Status of Pin 9 */
#define DPI_PB10 (BIT_9)    /* Status of Pin 10 */
#define DPI_PB11 (BIT_10)   /* Status of Pin 11 */
#define DPI_PB12 (BIT_11)   /* Status of Pin 12 */
#define DPI_PB13 (BIT_12)   /* Status of Pin 13 */
#define DPI_PB14 (BIT_13)   /* Status of Pin 14 */

/* DPI Interrupt Latch Register (DPI_IRPTL, DPI_IRPTL_FE, DPI_IRPTL_RE) */
#define UART0_TX_INT        (BIT_0)
#define UART0_RX_INT        (BIT_2)
#define TWI_INT             (BIT_4)
#define EXT_MISC_0_INT      (BIT_5)
#define EXT_MISC_1_INT      (BIT_6)
#define EXT_MISC_2_INT      (BIT_7)
#define EXT_MISC_3_INT      (BIT_8)
#define EXT_MISC_4_INT      (BIT_9)
#define EXT_MISC_5_INT      (BIT_10)
#define EXT_MISC_6_INT      (BIT_11)
#define EXT_MISC_7_INT      (BIT_12)
#define EXT_MISC_8_INT      (BIT_13)

/* Aliases of EXT_MISC_*_INT (above) */
#define DPI_INT_5 (BIT_5)
#define DPI_INT_6 (BIT_6)
#define DPI_INT_7 (BIT_7)
#define DPI_INT_8 (BIT_8)
#define DPI_INT_9 (BIT_9)
#define DPI_INT_10 (BIT_10)
#define DPI_INT_11 (BIT_11)
#define DPI_INT_12 (BIT_12)
#define DPI_INT_13 (BIT_13)

/* Precision Clock Control A0 Register (PCG_CTLA0) */
#define FSADIV (0x000FFFFF) /*FS A Divisor */
#define FSAPHASE_HI (0x3FF00000)    /*FS A Phase Offset */
#define ENFSA   (BIT_30)        /*Enable FS A */
#define ENCLKA  (BIT_31)        /*Enable CLK A */

/* Precision Clock Control A1 Register (PCG_CTLA1) */
#define CLKADIV (0x000FFFFF)        /*CLK A Divisor */
#define FSAPHASE_LO (0x3FF00000)    /*CLK A Phase Offset */
#define FSASOURCE   (BIT_30)        /*External FS A Source */
#define CLKASOURCE  (BIT_31)        /*External CLK A Source */

/* Precision Clock Control B0 Register (PCG_CTLB0) */
#define FSBDIV (0x000FFFFF) /*FS B Divisor */
#define FSBPHASE_HI (0x3FF00000)    /*FS B Phase Offset */
#define ENFSB   (BIT_30)        /*Enable FS B */
#define ENCLKB  (BIT_31)        /*Enable CLK B */

/* Precision Clock Control B1 Register (PCG_CTLB1) */
#define CLKBDIV (0x000FFFFF)        /*CLK B Divisor */
#define FSBPHASE_LO (0x3FF00000)    /*CLK B Phase Offset */
#define FSBSOURCE   (BIT_30)        /*External FS B Source */
#define CLKBSOURCE  (BIT_31)        /*External CLK B Source */

/* Precision Clock Pulse Width Control Register (PCG_PW) */
#define STROBEA (BIT_0)         /*Set FS A to Strobe */
#define INVFSA  (BIT_1)         /*Invert FS A */
#define PWFSA   (0x0000FFFF)        /*FS A Pulse Width */
#define STROBEB (BIT_16)            /*Set FS B to Strobe */
#define INVFSB  (BIT_17)            /*Invert FS B */
#define PWFSB   (0xFFFF0000)        /*FS B Pulse Width */

/* Precision Clock Frame Sync Synchronization (PCG_SYNC) */
#define FSA_SYNC        (BIT_0)  /*Enable synchronization of FSA with external LRCLK */
#define CLKA_SYNC       (BIT_1)  /*Enable synchronization of CLKA with external LRCLK */
#define CLKA_SOURCE_IOP (BIT_2)
#define FSA_SOURCE_IOP  (BIT_3)
#define FSB_SYNC        (BIT_16) /*Enable synchronization of FSB with external LRCLK */
#define CLKB_SYNC       (BIT_17) /*Enable synchronization of CLKB with external LRCLK */
#define CLKB_SOURCE_IOP (BIT_18)
#define FSB_SOURCE_IOP  (BIT_19)

/* Precision Clock Control C0 Register (PCG_CTLC0) */
#define FSCDIV      (0x000FFFFF) /* FS C Divisor */
#define FSCPHASE_HI (0x3FF00000) /* FS C Phase Offset */
#define ENFSC       (BIT_30)    /* Enable FS C */
#define ENCLKC      (BIT_31)    /* Enable CLK C */

/* Precision Clock Control C1 Register (PCG_CTLC1) */
#define CLKCDIV     (0x000FFFFF)   /* CLK C Divisor */
#define FSCPHASE_LO (0x3FF00000)   /* CLK C Phase Offset */
#define FSCSOURCE   (BIT_30)       /* External FS C Source */
#define CLKCSOURCE  (BIT_31)       /* External CLK C Source */

/* Precision Clock Control D0 Register (PCG_CTLD0) */
#define FSDDIV      (0x000FFFFF) /* FS D Divisor */
#define FSDPHASE_HI (0x3FF00000) /* FS D Phase Offset */
#define ENFSD       (BIT_30)    /* Enable FS D */
#define ENCLKD      (BIT_31)    /* Enable CLK D */

/* Precision Clock Control D1 Register (PCG_CTLD1) */
#define CLKDDIV     (0x000FFFFF)   /* CLK D Divisor */
#define FSDPHASE_LO (0x3FF00000)   /* CLK D Phase Offset */
#define FSDSOURCE   (BIT_30)       /* External FS D Source */
#define CLKDSOURCE  (BIT_31)       /* External CLK D Source */

/* Precision Clock Pulse Width Control Register (PCG_PW2) */
#define STROBEC (BIT_0)        /* Set FS C to Strobe */
#define INVFSC  (BIT_1)        /* Invert FS C */
#define PWFSC   (0x0000FFFF)   /* FS C Pulse Width */
#define STROBED (BIT_16)       /* Set FS D to Strobe */
#define INVFSD  (BIT_17)       /* Invert FS D */
#define PWFSD   (0xFFFF0000)   /* FS D Pulse Width */

/* Precision Clock Frame Sync Synchronization (PCG_SYNC2) */
#define FSC_SYNC        (BIT_0)  /* Enable synchronization of FSC with external LRCLK */
#define CLKC_SYNC       (BIT_1)  /* Enable synchronization of CLKC with external LRCLK */
#define CLKC_SOURCE_IOP (BIT_2)
#define FSC_SOURCE_IOP  (BIT_3)
#define FSD_SYNC        (BIT_16) /* Enable synchronization of FSD with external LRCLK */
#define CLKD_SYNC       (BIT_17) /* Enable synchronization of CLKD with external LRCLK */
#define CLKD_SOURCE_IOP (BIT_18)
#define FSD_SOURCE_IOP  (BIT_19)



/* Priority Interrupt Control Registers */
/* PICR0 */
#define P0I0    (BIT_0) /* Programmable Interrupt 0 Bit Field */
#define P0I1    (BIT_1)
#define P0I2    (BIT_2)
#define P0I3    (BIT_3)
#define P0I4    (BIT_4)
#define P1I0    (BIT_5) /* Programmable Interrupt 1 Bit Field */
#define P1I1    (BIT_6)
#define P1I2    (BIT_7)
#define P1I3    (BIT_8)
#define P1I4    (BIT_9)
#define P2I0    (BIT_10) /* Programmable Interrupt 2 Bit Field */
#define P2I1    (BIT_11)
#define P2I2    (BIT_12)
#define P2I3    (BIT_13)
#define P2I4    (BIT_14)
#define P3I0    (BIT_15) /* Programmable Interrupt 3 Bit Field */
#define P3I1    (BIT_16)
#define P3I2    (BIT_17)
#define P3I3    (BIT_18)
#define P3I4    (BIT_19)
#define P4I0    (BIT_20) /* Programmable Interrupt 4 Bit Field */
#define P4I1    (BIT_21)
#define P4I2    (BIT_22)
#define P4I3    (BIT_23)
#define P4I4    (BIT_24)
#define P5I0    (BIT_25) /* Programmable Interrupt 5 Bit Field */
#define P5I1    (BIT_26)
#define P5I2    (BIT_27)
#define P5I3    (BIT_28)
#define P5I4    (BIT_29)

/* PICR1 */
#define P6I0    (BIT_0) /* Programmable Interrupt 6 Bit Field */
#define P6I1    (BIT_1)
#define P6I2    (BIT_2)
#define P6I3    (BIT_3)
#define P6I4    (BIT_4)
#define P7I0    (BIT_5) /* Programmable Interrupt 7 Bit Field */
#define P7I1    (BIT_6)
#define P7I2    (BIT_7)
#define P7I3    (BIT_8)
#define P7I4    (BIT_9)
#define P8I0    (BIT_10) /* Programmable Interrupt 8 Bit Field */
#define P8I1    (BIT_11)
#define P8I2    (BIT_12)
#define P8I3    (BIT_13)
#define P8I4    (BIT_14)
#define P9I0    (BIT_15) /* Programmable Interrupt 9 Bit Field */
#define P9I1    (BIT_16)
#define P9I2    (BIT_17)
#define P9I3    (BIT_18)
#define P9I4    (BIT_19)
#define P10I0   (BIT_20) /* Programmable Interrupt 10 Bit Field */
#define P10I1   (BIT_21)
#define P10I2   (BIT_22)
#define P10I3   (BIT_23)
#define P10I4   (BIT_24)
#define P11I0   (BIT_25) /* Programmable Interrupt 11 Bit Field */
#define P11I1   (BIT_26)
#define P11I2   (BIT_27)
#define P11I3   (BIT_28)
#define P11I4   (BIT_29)

/* PICR2 */
#define P12I0   (BIT_0) /* Programmable Interrupt 12 Bit Field */
#define P12I1   (BIT_1)
#define P12I2   (BIT_2)
#define P12I3   (BIT_3)
#define P12I4   (BIT_4)
#define P13I0   (BIT_5) /* Programmable Interrupt 13 Bit Field */
#define P13I1   (BIT_6)
#define P13I2   (BIT_7)
#define P13I3   (BIT_8)
#define P13I4   (BIT_9)
#define P14I0   (BIT_10) /* Programmable Interrupt 14 Bit Field */
#define P14I1   (BIT_11)
#define P14I2   (BIT_12)
#define P14I3   (BIT_13)
#define P14I4   (BIT_14)
#define P15I0   (BIT_15) /* Programmable Interrupt 15 Bit Field */
#define P15I1   (BIT_16)
#define P15I2   (BIT_17)
#define P15I3   (BIT_18)
#define P15I4   (BIT_19)
#define P16I0   (BIT_20) /* Programmable Interrupt 16 Bit Field */
#define P16I1   (BIT_21)
#define P16I2   (BIT_22)
#define P16I3   (BIT_23)
#define P16I4   (BIT_24)
#define P17I0   (BIT_25) /* Programmable Interrupt 17 Bit Field */
#define P17I1   (BIT_26)
#define P17I2   (BIT_27)
#define P17I3   (BIT_28)
#define P17I4   (BIT_29)

/* PICR3 */
#define P18I0   (BIT_0) /* Programmable Interrupt 18 Bit Field */
#define P18I1   (BIT_1)
#define P18I2   (BIT_2)
#define P18I3   (BIT_3)
#define P18I4   (BIT_4)

/* MTMDMA Control Register (MTMCTL) */
#define MTMEN       (BIT_0)     /*MTMDMA Enable */
#define MTMFLUSH    (BIT_1)     /*MTMDMA FIFO Flush */
#define MTMRS       (BIT_30)    /*MTMDMA Read DMA status */
#define MTMWS       (BIT_31)    /*MTMDMA Write DMA status */

/*PWM Global Control Register (PWMGCTL) */
#define PWM_EN0     (BIT_0) /* PWM group 0 enable */
#define PWM_DIS0    (BIT_1) /* PWM group 0 disable */
#define PWM_EN1     (BIT_2) /* PWM group 1 enable */
#define PWM_DIS1    (BIT_3) /* PWM group 1 disable */
#define PWM_EN2     (BIT_4) /* PWM group 2 enable */
#define PWM_DIS2    (BIT_5) /* PWM group 2 disable */
#define PWM_EN3     (BIT_6) /* PWM group 3 enable */
#define PWM_DIS3    (BIT_7) /* PWM group 3 disable */
#define PWM_SYNCEN0     (BIT_8) /* PWM group 0 enable */
#define PWM_SYNCDIS0    (BIT_9) /* PWM group 0 disable */
#define PWM_SYNCEN1     (BIT_10) /* PWM group 1 enable */
#define PWM_SYNCDIS1    (BIT_11) /* PWM group 1 disable */
#define PWM_SYNCEN2     (BIT_12) /* PWM group 2 enable */
#define PWM_SYNCDIS2    (BIT_13) /* PWM group 2 disable */
#define PWM_SYNCEN3     (BIT_14) /* PWM group 3 enable */
#define PWM_SYNCDIS3    (BIT_15) /* PWM group 3 disable */

/*PWM Global Status Register (PWMGSTAT) */
#define PWM_STAT0   (BIT_0) /*PWM group 0 period completion status (W1C) */
#define PWM_STAT1   (BIT_1) /*PWM group 1 period completion status (W1C) */
#define PWM_STAT2   (BIT_2) /*PWM group 2 period completion status (W1C) */
#define PWM_STAT3   (BIT_3) /*PWM group 3 period completion status (W1C) */


/*PWM Control Register (PWMCTL0, PWMCTL1, PWMCTL2, PWMCTL3) */
#define PWM_ALIGN   (BIT_0) /* PWM Align Mode (edge=0, center=1) */
#define PWM_PAIR    (BIT_1) /* PWM Pair Mode (set) */
#define PWM_UPDATE  (BIT_2) /* PWM Update Mode (0=single udpate, 1=double update) */
#define PWM_IRQEN   (BIT_5) /* PWM Enable PWM interrupt */

/*PWM Status Register (PWMSTAT0, PWMSTAT1, PWMSTAT2, PWMSTAT3) */
#define PWM_PHASE   (BIT_0) /* PWM Phase Status (1st half=0, 2nd half=1) */
#define PWM_PAIRSTAT    (BIT_2) /* PWM Paired Mode Status (inactive=0, active=1) */

/*PWM Output Disable (PWMSEG0, PWMSEG1, PWMSEG2, PWMSEG3) */
#define PWM_BH      (BIT_0) /*BH disable (set) enable (clear) */
#define PWM_BL      (BIT_1) /*BL disable (set) enable (clear) */
#define PWM_AH      (BIT_2) /*AH disable (set) enable (clear) */
#define PWM_AL      (BIT_3) /*AL disable (set) enable (clear) */

/*PWM Debug Status (PWMDBG0, PWMDBG1, PWMDBG2, PWMDBG3) */
#define PWM_STATAL  (BIT_0) /*AL stat */
#define PWM_STATAH  (BIT_1) /*AH stat */
#define PWM_STATBL  (BIT_2) /*BL stat */
#define PWM_STATBH  (BIT_3) /*BH stat */

/*PWM Polarity Select Register (PWMPOL0, PWMPOL1, PWMPOL2, PWMPOL3) */
#define PWM_POL1AL  (BIT_0) /*Write to set AL Polarity 1 */
#define PWM_POL0AL  (BIT_1) /*Write to set AL Polarity 0 */
#define PWM_POL1AH  (BIT_2) /*Write to set AH Polarity 1 */
#define PWM_POL0AH  (BIT_3) /*Write to set AH Polarity 0 */
#define PWM_POL1BL  (BIT_4) /*Write to set BL Polarity 1 */
#define PWM_POL0BL  (BIT_5) /*Write to set BL Polarity 0 */
#define PWM_POL1BH  (BIT_6) /*Write to set BH Polarity 1 */
#define PWM_POL0BH  (BIT_7) /*Write to set BH Polarity 0 */

/* TWI Registers */
/* TWIMITR - Master Internal Time Register */
#define TWIEN       (BIT_7)
#define PRESCALE    (BIT_0|BIT_1|BIT_2|BIT_3|BIT_4|BIT_5|BIT_6)

/* TWISCTL */
#define TWISEN     (BIT_0)
#define TWISLEN    (BIT_1)
#define TWIDVAL    (BIT_2)
#define TWINAK     (BIT_3)
#define TWIGCE     (BIT_4)

/* TWISSTAT */
#define TWISDIR    (BIT_0)
#define TWIGC      (BIT_1)

/* TWIMCTL */
#define TWIMEN     (BIT_0)
#define TWIMLEN    (BIT_1)
#define TWIMDIR    (BIT_2)
#define TWIFAST    (BIT_3)
#define TWISTOP    (BIT_4)
#define TWIRSTART  (BIT_5)
#define TWIDCNT    (BIT_6|BIT_7|BIT_8|BIT_9|BIT_10|BIT_11|BIT_12|BIT_13)
#define TWISDAOVR  (BIT_14)
#define TWISCLOVR  (BIT_15)

/* TWIMSTAT */
#define TWIMPROG   (BIT_0)
#define TWILOST    (BIT_1)
#define TWIANAK    (BIT_2)
#define TWIDNAK    (BIT_3)
#define TWIRERR    (BIT_4)
#define TWIWERR    (BIT_5)
#define TWISDASEN  (BIT_6)
#define TWISCLSEN  (BIT_7)
#define TWIBUSY    (BIT_8)

/* TWIIMASK/TWIIRPTL */
#define TWISINIT   (BIT_0)
#define TWISCOMP   (BIT_1)
#define TWISERR    (BIT_2)
#define TWISOVF    (BIT_3)
#define TWIMCOM    (BIT_4)
#define TWIMERR    (BIT_5)
#define TWITXINT   (BIT_6)
#define TWIRXINT   (BIT_7)

/* TWIFIFOCTL */
#define TWITXFLUSH    (BIT_0)
#define TWIRXFLUSH    (BIT_1)
#define TWITXINT2     (BIT_2)
#define TWIRXINT2     (BIT_3)
#define TWIBHD        (BIT_4)

/* TWIFIFOSTAT */
#define TWITXS      (BIT_0|BIT_1)
#define TWIRXS      (BIT_2|BIT_3)

/* UART registers */
/* UARTIER */
#define UARTRBFIE   (BIT_0)
#define UARTTBEIE   (BIT_1)
#define UARTLSIE    (BIT_2)
#define UARTTXFIE   (BIT_3)
#define UARTADIE    (BIT_4)

/* UARTIIR */
#define UARTNOINT  (BIT_0)
#define UARTISTAT  (BIT_1|BIT_2|BIT_3)
#define UARTRBFI   (BIT_2)
#define UARTTBEI   (BIT_1)
#define UARTLSI    (BIT_1|BIT_2)
#define UARTTXFI   (0x00000000)
#define UARTADI    (BIT_3)

/* UARTLCR */
#define UARTWLS5    (0x00000000)
#define UARTWLS6    (BIT_0)
#define UARTWLS7    (BIT_1)
#define UARTWLS8    (BIT_0|BIT_1)
#define UARTSTB     (BIT_2)
#define UARTPEN     (BIT_3)
#define UARTEPS     (BIT_4)
#define UARTSP      (BIT_5)
#define UARTSB      (BIT_6)
#define UARTDLAB    (BIT_7)

/* UARTMODE */
#define UARTPACK    (BIT_0)
#define UARTPKSYN   (BIT_1)
#define UARTTX9     (BIT_2)
#define UARTRX9     (BIT_3)
#define UARTAEN     (BIT_4)
#define UARTPST0    (BIT_5)
#define UARTPST1    (BIT_6)

/* UARTLSR */
#define UARTDR      (BIT_0)
#define UARTOE      (BIT_1)
#define UARTPE      (BIT_2)
#define UARTFE      (BIT_3)
#define UARTBI      (BIT_4)
#define UARTTHRE    (BIT_5)
#define UARTTEMT    (BIT_6)
#define UARTRX9D    (BIT_7)

/* UARTTXCTL/UARTRXCTL */
#define UARTEN     (BIT_0)
#define UARTDEN    (BIT_1)
#define UARTCHEN   (BIT_2)

/* UARTTXSTAT/UARTRXSTAT */
#define UARTERRIRQ   (BIT_0)
#define UARTDMASTAT  (BIT_1)
#define UARTCHSTAT   (BIT_2)


/* SPDIF Transmit registers */
#define DITCTL      (0x24A0)   /* Digital Interface Transmit Control Register */

/* SPDIF Channel Status registers */
#define DITCHANA0   (0x24A1)
#define DITCHANA1   (0x24D4)
#define DITCHANA2   (0x24D5)
#define DITCHANA3   (0x24D6)
#define DITCHANA4   (0x24D7)
#define DITCHANA5   (0x24D8)
#define DITCHANB0   (0x24A2)
#define DITCHANB1   (0x24DA)
#define DITCHANB2   (0x24DB)
#define DITCHANB3   (0x24DC)
#define DITCHANB4   (0x24DD)
#define DITCHANB5   (0x24DE)

/* SPDIF User bit Status registers */
#define DITUSRBITA0 (0x24E0)
#define DITUSRBITA1 (0x24E1)
#define DITUSRBITA2 (0x24E2)
#define DITUSRBITA3 (0x24E3)
#define DITUSRBITA4 (0x24E4)
#define DITUSRBITA5 (0x24E5)

#define DITUSRBITB0 (0x24E8)
#define DITUSRBITB1 (0x24E9)
#define DITUSRBITB2 (0x24EA)
#define DITUSRBITB3 (0x24EB)
#define DITUSRBITB4 (0x24EC)
#define DITUSRBITB5 (0x24ED)
 
#define DITUSRUPD   (0x24EF)   /* Write to this when above user bits registers are updated */

/* SPDIF Receiver registers */
#define DIRCTL      (0x24A8)   /* Digital Interface Receiver Control Register */
#define DIRSTAT     (0x24A9)   /* Digital Interface Receiver Status */
#define DIRCHANL    (0x24AA)   /* Digital Interface Receiver Left Channel Status */
#define DIRCHANR    (0x24AB)   /* Digital Interface Receiver Right Channel Status */

/* SPDIF Transmit Control Register (DITCTL) */
#define DIT_EN       (BIT_0)  /* Enables SPDIF transmitter and resets control register (set) */
#define DIT_MUTE     (BIT_1)  /* Mutes serial data output maintaing clocks (digital black) */
#define DIT_FREQ0    (BIT_2)  /* Oversampling Multiplier: 256 x Fs(00), 384 x Fs(01) */
#define DIT_FREQ1    (BIT_3)  /* 512 x Fs(10), 768 x Fs(11) */
#define DIT_SCDF     (BIT_4)  /* Enable single channel double frequency mode (set) */
#define DIT_SCDF_LR  (BIT_5)  /* SCDF channel mode L=0, R=1 */
#define DIT_SMODEIN0 (BIT_6)  /* Serial Input Format: Left justified (000), */
#define DIT_SMODEIN1 (BIT_7)  /* I2S (001), Right justified 24-bit (100), */
#define DIT_SMODEIN2 (BIT_8)  /* 20-bit (110), 18-bit (110), 16-bit (111) */
#define DIT_AUTO     (BIT_9)  /* Automatically generate block start information */
#define DIT_VALIDL   (BIT_10)       /* Validity Bit used with Left Channel Status Buffer */
#define DIT_VALIDR   (BIT_11)       /* Validity Bit used with Right Channel Status Buffer */
#define DIT_BLKSTART (BIT_12) /* Current word is block start in Auto block start mode */
#define DIT_USRPEND  (BIT_13)  /* This bit is high if user bits buffer has been written but not transmitted fully */
#define DIT_EXT_SYNCEN (BIT_15)

/* SPDIF TX Input Data Formats */
#define DIT_IN_LJ     0
#define DIT_IN_I2S    DIT_SMODEIN0
#define DIT_IN_TDM    DIT_SMODEIN1
#define DIT_IN_RJ24   DIT_SMODEIN2
#define DIT_IN_RJ20   (DIT_SMODEIN0 | DIT_SMODEIN2)
#define DIT_IN_RJ18   (DIT_SMODEIN1 | DIT_SMODEIN2)
#define DIT_IN_RJ16   (DIT_SMODEIN0 | DIT_SMODEIN1 | DIT_SMODEIN2)

#define DIT_FREQ256   0
#define DIT_FREQ384   DIT_FREQ0

/* SPDIF Recieve Control Register (DIRCTL) */
#define DIR_BIPHASE0    (BIT_0) /* Parity Piphase Error Control: No action(00), */
#define DIR_BIPHASE1    (BIT_1) /* Last valid sample held(01), invalid sample zeroed(10) */
#define DIR_LOCK0   (BIT_2) /* Lock error control: No action900), Last valid sample held(01), */
#define DIR_LOCK1   (BIT_3) /* Zeros after last valid sample(10), soft mute of last valid sample(11) */
#define DIR_SCDF_LR (BIT_4) /* SCDF channel mode L=0, R=1 */
#define DIR_SCDF    (BIT_5) /* SCDF enable */
#define DIR_MUTE    (BIT_6) /* Mute serial data out (digital black) */
#define DIR_PLLDIS  (BIT_7) /* Disable PLL (set) */
#define DIR_RESET   (BIT_9) /* Reset */

/* SPDIF Recieve Status (DIRSTAT) -read only */
#define DIR_NOAUDIOL    (BIT_0) /* Non-audio subframe mode, left channel per SMPTE 337M */
#define DIR_NOAUDIOR    (BIT_1) /* Non-audio subframe mode, right channel per SMPTE 337M */
#define DIR_NOAUDIOLR   (BIT_2) /* Non-audio frame mode, left and right channel */
#define DIR_VALID   (BIT_3) /* OR'd value of left and right channel */
#define DIR_LOCK    (BIT_4) /* Receiver lock signal */
#define DIR_NOSTREAM    (BIT_5) /* Stream is disconnected */
#define DIR_PARITYERROR (BIT_6) /* Parity Error */
#define DIR_BIPHASEERROR (BIT_7) /* Biphase Error */
#define DIR_B0CHANL0   (BIT_16) /* Channel Status byte 0 for subframe A */
#define DIR_B0CHANL1   (BIT_17)
#define DIR_B0CHANL2   (BIT_18)
#define DIR_B0CHANL3   (BIT_19)
#define DIR_B0CHANL4   (BIT_20)
#define DIR_B0CHANL5   (BIT_21)
#define DIR_B0CHANL6   (BIT_22)
#define DIR_B0CHANL7   (BIT_23)
#define DIR_B0CHANR0   (BIT_24) /* Channel Status byte 0 for subframe B */
#define DIR_B0CHANR1   (BIT_25)
#define DIR_B0CHANR2   (BIT_26)
#define DIR_B0CHANR3   (BIT_27)
#define DIR_B0CHANR4   (BIT_28)
#define DIR_B0CHANR5   (BIT_29)
#define DIR_B0CHANR6   (BIT_30)
#define DIR_B0CHANR7   (BIT_31)


/*Sample Rate Converter Control Register 0 (SRCCTL0) */
#define SRC0_HARD_MUTE  (BIT_0)  /* SRC0 Hard Mute enable */
#define SRC0_AUTO_MUTE  (BIT_1)  /* SRC0 Automatic Hard Mute enable */
#define SRC0_SMODEIN0   (BIT_2)  /* SRC0 Serial Input Format: Left Justified(000), */
#define SRC0_SMODEIN1   (BIT_3)  /* I2S(001), TDM(010), Right Justified 24-bits(100), */
#define SRC0_SMODEIN2   (BIT_4)  /* 20 bits(101), 18-bits(110), 16-bits(111) */
#define SRC0_BYPASS (BIT_5)  /* SRC0 Bypass Mode */
#define SRC0_DEEMPHASIS0 (BIT_6)  /* SRC0 filter mode is set at no deemphasis(000), */
#define SRC0_DEEMPHASIS1 (BIT_7)  /* 33kHz(01), 44.1kHz(10), 48kHz(11) */
#define SRC0_SOFTMUTE   (BIT_8)  /* SRC0 Soft Mute enable */
#define SRC0_DITHER (BIT_9)  /* SRC0 Dither enable */
#define SRC0_SMODEOUT0  (BIT_10) /* SRC0 Serial Output Format: Left Justified(00), */
#define SRC0_SMODEOUT1  (BIT_11) /* I2S(01), TDM(10), Right Justified(11) */
#define SRC0_LENOUT0    (BIT_12) /* SRC0 Output Word Length: 24-bit(00), */
#define SRC0_LENOUT1    (BIT_13) /* 20-bit(01), 18-bit(10), 16-bit(11) */
#define SRC0_MPHASE (BIT_14) /* SRC0 Enable Matched Phase Mode */
#define SRC0_ENABLE  (BIT_15) /* SRC0 Enable */
#define SRC1_HARD_MUTE  (BIT_16)  /* SRC1 Hard Mute enable */
#define SRC1_AUTO_MUTE  (BIT_17)    /* SRC1 Automatic Hard Mute enable */
#define SRC1_SMODEIN0   (BIT_18)  /* SRC1 Serial Input Format: Left Justified(000), */
#define SRC1_SMODEIN1   (BIT_19)  /* I2S(001), TDM(010), Right Justified 24-bits(100), */
#define SRC1_SMODEIN2   (BIT_20)  /* 20 bits(101), 18-bits(110), 16-bits(111) */
#define SRC1_BYPASS (BIT_21)  /* SRC1 Bypass Mode */
#define SRC1_DEEMPHASIS0 (BIT_22)  /* SRC1 filter mode is set at no deemphasis(000), */
#define SRC1_DEEMPHASIS1 (BIT_23)  /* 33kHz(01), 44.1kHz(10), 48kHz(11) */
#define SRC1_SOFTMUTE   (BIT_24)  /* SRC1 Soft Mute enable */
#define SRC1_DITHER (BIT_25) /* SRC1 Dither enable */
#define SRC1_SMODEOUT0  (BIT_26) /* SRC1 Serial Output Format: Left Justified(00), */
#define SRC1_SMODEOUT1  (BIT_27) /* I2S(01), TDM(10), Right Justified(11) */
#define SRC1_LENOUT0    (BIT_28) /* SRC1 Output Word Length: 24-bit(00), */
#define SRC1_LENOUT1    (BIT_29) /* 20-bit(01), 18-bit(10), 16-bit(11) */
#define SRC1_MPHASE (BIT_30) /* SRC1 Enable Matched Phase Mode */
#define SRC1_ENABLE  (BIT_31) /* SRC1 Enable */

/*Sample Rate Converter Control Register 1 (SRCCTL1) */
#define SRC2_HARD_MUTE  (BIT_0)  /* SRC2 Hard Mute enable */
#define SRC2_AUTO_MUTE  (BIT_1)  /* SRC2 Automatic Hard Mute enable */
#define SRC2_SMODEIN0   (BIT_2)  /* SRC2 Serial Input Format: Left Justified(000), */
#define SRC2_SMODEIN1   (BIT_3)  /* I2S(001), TDM(010), Right Justified 24-bits(100), */
#define SRC2_SMODEIN2   (BIT_4)  /* 20 bits(101), 18-bits(110), 16-bits(111) */
#define SRC2_BYPASS (BIT_5)  /* SRC2 Bypass Mode */
#define SRC2_DEEMPHASIS0 (BIT_6)  /* SRC2 filter mode is set at no deemphasis(000), */
#define SRC2_DEEMPHASIS1 (BIT_7)  /* 33kHz(01), 44.1kHz(10), 48kHz(11) */
#define SRC2_SOFTMUTE   (BIT_8)  /* SRC2 Soft Mute enable */
#define SRC2_DITHER (BIT_9)  /* SRC2 Dither enable */
#define SRC2_SMODEOUT0  (BIT_10) /* SRC2 Serial Output Format: Left Justified(00), */
#define SRC2_SMODEOUT1  (BIT_11) /* I2S(01), TDM(10), Right Justified(11) */
#define SRC2_LENOUT0    (BIT_12) /* SRC2 Output Word Length: 24-bit(00), */
#define SRC2_LENOUT1    (BIT_13) /* 20-bit(01), 18-bit(10), 16-bit(11) */
#define SRC2_MPHASE (BIT_14) /* SRC2 Enable Matched Phase Mode */
#define SRC2_ENABLE  (BIT_15) /* SRC2 Enable */
#define SRC3_HARD_MUTE  (BIT_16)  /* SRC3 Hard Mute enable */
#define SRC3_AUTO_MUTE  (BIT_17)    /* SRC1 Automatic Hard Mute enable */
#define SRC3_SMODEIN0   (BIT_18)  /* SRC3 Serial Input Format: Left Justified(000), */
#define SRC3_SMODEIN1   (BIT_19)  /* I2S(001), TDM(010), Right Justified 24-bits(100), */
#define SRC3_SMODEIN2   (BIT_20)  /* 20 bits(101), 18-bits(110), 16-bits(111) */
#define SRC3_BYPASS (BIT_21)  /* SRC3 Bypass Mode */
#define SRC3_DEEMPHASIS0 (BIT_22)  /* SRC3 filter mode is set at no deemphasis(000), */
#define SRC3_DEEMPHASIS1 (BIT_23)  /* 33kHz(01), 44.1kHz(10), 48kHz(11) */
#define SRC3_SOFTMUTE   (BIT_24)  /* SRC3 Soft Mute enable */
#define SRC3_DITHER (BIT_25) /* SRC3 Dither enable */
#define SRC3_SMODEOUT0  (BIT_26) /* SRC3 Serial Output Format: Left Justified(00), */
#define SRC3_SMODEOUT1  (BIT_27) /* I2S(01), TDM(10), Right Justified(11) */
#define SRC3_LENOUT0    (BIT_28) /* SRC3 Output Word Length: 24-bit(00), */
#define SRC3_LENOUT1    (BIT_29) /* 20-bit(01), 18-bit(10), 16-bit(11) */
#define SRC3_MPHASE (BIT_30) /* SRC3 Enable Matched Phase Mode */
#define SRC3_ENABLE  (BIT_31) /* SRC3 Enable */

/* SRC0 Input Data Formats */
#define SRC0_IN_LJ     0
#define SRC0_IN_I2S    SRC0_SMODEIN0
#define SRC0_IN_TDM    SRC0_SMODEIN1
#define SRC0_IN_RJ24   SRC0_SMODEIN2
#define SRC0_IN_RJ20   (SRC0_SMODEIN0|SRC0_SMODEIN2)
#define SRC0_IN_RJ18   (SRC0_SMODEIN1|SRC0_SMODEIN2)
#define SRC0_IN_RJ16   (SRC0_SMODEIN0|SRC0_SMODEIN1|SRC0_SMODEIN2)

/* SRC1 Input Data Formats */
#define SRC1_IN_LJ     0
#define SRC1_IN_I2S    SRC1_SMODEIN0
#define SRC1_IN_TDM    SRC1_SMODEIN1
#define SRC1_IN_RJ24   SRC1_SMODEIN2
#define SRC1_IN_RJ20   (SRC1_SMODEIN0|SRC1_SMODEIN2)
#define SRC1_IN_RJ18   (SRC1_SMODEIN1|SRC1_SMODEIN2)
#define SRC1_IN_RJ16   (SRC1_SMODEIN0|SRC1_SMODEIN1|SRC1_SMODEIN2)

/* SRC2 Input Data Formats */
#define SRC2_IN_LJ     0
#define SRC2_IN_I2S    SRC2_SMODEIN0
#define SRC2_IN_TDM    SRC2_SMODEIN1
#define SRC2_IN_RJ24   SRC2_SMODEIN2
#define SRC2_IN_RJ20   (SRC2_SMODEIN0|SRC2_SMODEIN2)
#define SRC2_IN_RJ18   (SRC2_SMODEIN1|SRC2_SMODEIN2)
#define SRC2_IN_RJ16   (SRC2_SMODEIN0|SRC2_SMODEIN1|SRC2_SMODEIN2)

/* SRC3 Input Data Formats */
#define SRC3_IN_LJ     0
#define SRC3_IN_I2S    SRC3_SMODEIN0
#define SRC3_IN_TDM    SRC3_SMODEIN1
#define SRC3_IN_RJ24   SRC3_SMODEIN2
#define SRC3_IN_RJ20   (SRC3_SMODEIN0|SRC3_SMODEIN2)
#define SRC3_IN_RJ18   (SRC3_SMODEIN1|SRC3_SMODEIN2)
#define SRC3_IN_RJ16   (SRC3_SMODEIN0|SRC3_SMODEIN1|SRC3_SMODEIN2)

/* SRC0 Output Data Formats */
#define SRC0_OUT_LJ     0
#define SRC0_OUT_I2S    SRC0_SMODEOUT0
#define SRC0_OUT_TDM    SRC0_SMODEOUT1
#define SRC0_OUT_RJ     (SRC0_SMODEOUT0|SRC0_SMODEOUT1)

#define SRC0_OUT_24      0
#define SRC0_OUT_20      SRC0_LENOUT0
#define SRC0_OUT_18      SRC0_LENOUT1
#define SRC0_OUT_16      (SRC0_LENOUT0|SRC0_LENOUT1)

/* SRC1 Output Data Formats */
#define SRC1_OUT_LJ     0
#define SRC1_OUT_I2S    SRC1_SMODEOUT0
#define SRC1_OUT_TDM    SRC1_SMODEOUT1
#define SRC1_OUT_RJ     (SRC1_SMODEOUT0|SRC1_SMODEOUT1)

#define SRC1_OUT_24      0
#define SRC1_OUT_20      SRC1_LENOUT0
#define SRC1_OUT_18      SRC1_LENOUT1
#define SRC1_OUT_16      (SRC1_LENOUT0|SRC1_LENOUT1)

/* SRC2 Output Data Formats */
#define SRC2_OUT_LJ     0
#define SRC2_OUT_I2S    SRC2_SMODEOUT0
#define SRC2_OUT_TDM    SRC2_SMODEOUT1
#define SRC2_OUT_RJ     (SRC2_SMODEOUT0|SRC2_SMODEOUT1)

#define SRC2_OUT_24      0
#define SRC2_OUT_20      SRC2_LENOUT0
#define SRC2_OUT_18      SRC2_LENOUT1
#define SRC2_OUT_16      (SRC2_LENOUT0|SRC2_LENOUT1)

/* SRC3 Output Data Formats */
#define SRC3_OUT_LJ     0
#define SRC3_OUT_I2S    SRC3_SMODEOUT0
#define SRC3_OUT_TDM    SRC3_SMODEOUT1
#define SRC3_OUT_RJ     (SRC3_SMODEOUT0|SRC3_SMODEOUT1)

#define SRC3_OUT_24      0
#define SRC3_OUT_20      SRC3_LENOUT0
#define SRC3_OUT_18      SRC3_LENOUT1
#define SRC3_OUT_16      (SRC3_LENOUT0|SRC3_LENOUT1)

/* SRC0 Deemphasis Settings */
#define SRC0_DENO       0
#define SRC0_DE33       SRC0_DEEMPHASIS0
#define SRC0_DE44       SRC0_DEEMPHASIS1
#define SRC0_DE48       (SRC0_DEEMPHASIS0|SRC0_DEEMPHASIS1)

/* SRC1 Deemphasis Settings */
#define SRC1_DENO       0
#define SRC1_DE33       SRC1_DEEMPHASIS0
#define SRC1_DE44       SRC1_DEEMPHASIS1
#define SRC1_DE48       (SRC1_DEEMPHASIS0|SRC1_DEEMPHASIS1)

/* SRC2 Deemphasis Settings */
#define SRC2_DENO       0
#define SRC2_DE33       SRC2_DEEMPHASIS0
#define SRC2_DE44       SRC2_DEEMPHASIS1
#define SRC2_DE48       (SRC2_DEEMPHASIS0|SRC2_DEEMPHASIS1)

/* SRC3 Deemphasis Settings */
#define SRC3_DENO       0
#define SRC3_DE33       SRC3_DEEMPHASIS0
#define SRC3_DE44       SRC3_DEEMPHASIS1
#define SRC3_DE48       (SRC3_DEEMPHASIS0|SRC3_DEEMPHASIS1)

/* Sample Rate Converter Ratio 0 (SRCRAT0) */

#define SRC0_RATIO0 (BIT_0) /* SRC0 Ratio field is from bits [14:0] */
#define SRC0_RATIO1 (BIT_1)
#define SRC0_RATIO2 (BIT_2)
#define SRC0_RATIO3 (BIT_3)
#define SRC0_RATIO4 (BIT_4)
#define SRC0_RATIO5 (BIT_5)
#define SRC0_RATIO6 (BIT_6)
#define SRC0_RATIO7 (BIT_7)
#define SRC0_RATIO8 (BIT_8)
#define SRC0_RATIO9 (BIT_9)
#define SRC0_RATIO10    (BIT_10)
#define SRC0_RATIO11    (BIT_11)
#define SRC0_RATIO12    (BIT_12)
#define SRC0_RATIO13    (BIT_13)
#define SRC0_RATIO14    (BIT_14)
#define SRC0_MUTEOUT    (BIT_15) /* SRC0 mute output enable */
#define SRC1_RATIO0 (BIT_16) /* SRC1 Ratio field is from bits [14:0] */
#define SRC1_RATIO1 (BIT_17)
#define SRC1_RATIO2 (BIT_18)
#define SRC1_RATIO3 (BIT_19)
#define SRC1_RATIO4 (BIT_20)
#define SRC1_RATIO5 (BIT_21)
#define SRC1_RATIO6 (BIT_22)
#define SRC1_RATIO7 (BIT_23)
#define SRC1_RATIO8 (BIT_24)
#define SRC1_RATIO9 (BIT_25)
#define SRC1_RATIO10    (BIT_26)
#define SRC1_RATIO11    (BIT_27)
#define SRC1_RATIO12    (BIT_28)
#define SRC1_RATIO13    (BIT_29)
#define SRC1_RATIO14    (BIT_30)
#define SRC1_MUTEOUT    (BIT_31) /*SRC1 mute output enable */

/* Sample Rate Converter Ratio 1 (SRCRAT1) */
#define SRC2_RATIO0 (BIT_0) /* SRC2 Ratio field is from bits [14:0] */
#define SRC2_RATIO1 (BIT_1)
#define SRC2_RATIO2 (BIT_2)
#define SRC2_RATIO3 (BIT_3)
#define SRC2_RATIO4 (BIT_4)
#define SRC2_RATIO5 (BIT_5)
#define SRC2_RATIO6 (BIT_6)
#define SRC2_RATIO7 (BIT_7)
#define SRC2_RATIO8 (BIT_8)
#define SRC2_RATIO9 (BIT_9)
#define SRC2_RATIO10    (BIT_10)
#define SRC2_RATIO11    (BIT_11)
#define SRC2_RATIO12    (BIT_12)
#define SRC2_RATIO13    (BIT_13)
#define SRC2_RATIO14    (BIT_14)
#define SRC2_MUTEOUT    (BIT_15) /* SRC2 mute output enable */
#define SRC3_RATIO0 (BIT_16) /* SRC3 Ratio field is from bits [14:0] */
#define SRC3_RATIO1 (BIT_17)
#define SRC3_RATIO2 (BIT_18)
#define SRC3_RATIO3 (BIT_19)
#define SRC3_RATIO4 (BIT_20)
#define SRC3_RATIO5 (BIT_21)
#define SRC3_RATIO6 (BIT_22)
#define SRC3_RATIO7 (BIT_23)
#define SRC3_RATIO8 (BIT_24)
#define SRC3_RATIO9 (BIT_25)
#define SRC3_RATIO10    (BIT_26)
#define SRC3_RATIO11    (BIT_27)
#define SRC3_RATIO12    (BIT_28)
#define SRC3_RATIO13    (BIT_29)
#define SRC3_RATIO14    (BIT_30)
#define SRC3_MUTEOUT    (BIT_31) /* SRC3 mute output enable */

/* Sample Rate Converter MUTE Register(SRCMUTE) */
#define SRC0_MUTE_EN    (BIT_0) /* Connects SRC0 Mute input and ouptut (cleared) */
#define SRC1_MUTE_EN    (BIT_1) /* Connects SRC1 Mute input and ouptut (cleared) */
#define SRC2_MUTE_EN    (BIT_2) /* Connects SRC2 Mute input and ouptut (cleared) */
#define SRC3_MUTE_EN    (BIT_3) /* Connects SRC3 Mute input and ouptut (cleared) */



/* FIRCTL1 bit definitions */
#define FIR_EN          (BIT_0)   /* this is to enable/run the FIR accelerator */
#define FIR_CH1         0x00000000   /* channels  */
#define FIR_CH2         (BIT_1)   /* this is the channels  */
#define FIR_CH3         (BIT_2)   /* this is the channels  */
#define FIR_CH4         (BIT_2|BIT_1)   /* this is the channels  */
#define FIR_CH5         (BIT_3)   /* this is the channels  */
#define FIR_CH6         (BIT_1|BIT_3)   /* this is the channels  */
#define FIR_CH7         (BIT_2|BIT_3)   /* this is the channels  */
#define FIR_CH8         (BIT_1|BIT_2|BIT_3)
#define FIR_CH9         (BIT_4)
#define FIR_CH10         (BIT_1|BIT_4)   /* this is the channels  */
#define FIR_CH11         (BIT_2|BIT_4)   /* this is the channels  */
#define FIR_CH12         (BIT_2|BIT_1|BIT_4)   /* this is the channels  */
#define FIR_CH13         (BIT_3|BIT_4)   /* this is the channels  */
#define FIR_CH14         (BIT_1|BIT_3|BIT_4)   /* this is the channels  */
#define FIR_CH15         (BIT_2|BIT_3|BIT_4)   /* this is the channels  */
#define FIR_CH16         (BIT_1|BIT_2|BIT_3|BIT_4)
#define FIR_CH17         (BIT_5)
#define FIR_CH18         (BIT_1|BIT_5)   /* this is the channels  */
#define FIR_CH19         (BIT_2|BIT_5)   /* this is the channels  */
#define FIR_CH20         (BIT_2|BIT_1|BIT_5)   /* this is the channels  */
#define FIR_CH21         (BIT_3|BIT_5)   /* this is the channels  */
#define FIR_CH22         (BIT_1|BIT_3|BIT_5)   /* this is the channels  */
#define FIR_CH23         (BIT_2|BIT_3|BIT_5)   /* this is the channels  */
#define FIR_CH24         (BIT_1|BIT_2|BIT_3|BIT_5)
#define FIR_CH25         (BIT_4|BIT_5)
#define FIR_CH26         (BIT_1|BIT_4|BIT_5)   /* this is the channels  */
#define FIR_CH27         (BIT_2|BIT_4|BIT_5)   /* this is the channels  */
#define FIR_CH28         (BIT_2|BIT_1|BIT_4|BIT_5)   /* this is the channels  */
#define FIR_CH29         (BIT_3|BIT_4|BIT_5)   /* this is the channels  */
#define FIR_CH30         (BIT_1|BIT_3|BIT_4|BIT_5)   /* this is the channels  */
#define FIR_CH31         (BIT_2|BIT_3|BIT_4|BIT_5)   /* this is the channels  */
#define FIR_CH32         (BIT_1|BIT_2|BIT_3|BIT_4|BIT_5)

#define FIR_DMAEN        (BIT_8)
#define FIR_CAI          (BIT_9)
#define FIR_CCINTR       (BIT_11)
#define FIR_FXD          (BIT_12)
#define FIR_TC           (BIT_13)

#define FIR_RND0         0x00000000   /* IEEE round to nearest (even) */
#define FIR_RND1         (BIT_14)   /* IEEE round to zero */
#define FIR_RND2         (BIT_15)   /* IEEE round to +ve infinity */
#define FIR_RND3         (BIT_14|BIT_15)   /* IEEE round to -ve infinity */
#define FIR_RND4         (BIT_16)   /* Round to nearest Up */
#define FIR_RND5         (BIT_16|BIT_14)   /* Round away from zero */

/* FIRCTL2 bit definitions */

#define FIR_RATIO1       (0x00000000)
#define FIR_RATIO2       (BIT_25)
#define FIR_RATIO3 (BIT_26)
#define FIR_RATIO4 (BIT_26|BIT_25)
#define FIR_RATIO5 (BIT_27)
#define FIR_RATIO6       (BIT_27|BIT_25)
#define FIR_RATIO7 (BIT_27|BIT_26)
#define FIR_RATIO8 (BIT_27|BIT_26|BIT_25)

#define FIR_SRCEN        (BIT_29)
#define FIR_UPSAMP       (BIT_30)

/* FIR_DMASTAT bit definition */

#define FIR_DMACPLD      (BIT_0)
#define FIR_DMACLD       (BIT_1)
#define FIR_DMADLD       (BIT_2)
#define FIR_DMAPPGS      (BIT_3)
#define FIR_DMAWRBK      (BIT_4)
#define FIR_DMAWDONE      (BIT_5)
#define FIR_DMAACDONE     (BIT_6)

/* The FIR_MACSTAT bit defintions */

#define FIR_MACMRZ0      (BIT_0)
#define FIR_MACMRI0      (BIT_1)
#define FIR_MACMINV0     (BIT_2)
#define FIR_MACARZ0      (BIT_3)
#define FIR_MACARI0      (BIT_4)
#define FIR_MACAINV0     (BIT_5)
#define FIR_MACMRZ1      (BIT_6)
#define FIR_MACMRI1      (BIT_7)
#define FIR_MACMINV1     (BIT_8)
#define FIR_MACARZ1      (BIT_9)
#define FIR_MACARI1      (BIT_10)
#define FIR_MACAINV1     (BIT_11)
#define FIR_MACMRZ2      (BIT_12)
#define FIR_MACMRI2      (BIT_13)
#define FIR_MACMINV2     (BIT_14)
#define FIR_MACARZ2      (BIT_15)
#define FIR_MACARI2      (BIT_16)
#define FIR_MACAINV2     (BIT_17)
#define FIR_MACMRZ3      (BIT_18)
#define FIR_MACMRI3      (BIT_19)
#define FIR_MACMINV3     (BIT_20)
#define FIR_MACARZ3      (BIT_21)
#define FIR_MACARI3      (BIT_22)
#define FIR_MACAINV3     (BIT_23)


/* FIRDEBUGCTL/IIRDEBUGCTL bit definitions */

#define FIR_DBGMODE			(BIT_0)
#define FIR_HLD				(BIT_1)
#define FIR_RUN				(BIT_2)
#define FIR_DBGMEM			(BIT_4)
#define FIR_ADRINC			(BIT_5)


/* IIRCTL1 bit definitions */

#define IIR_EN          (BIT_0)   /* this is to enable/run the FIR accelerator */
#define IIR_CH1         0x00000000   /* channels  */
#define IIR_CH2         (BIT_1)   /* this is the channels  */
#define IIR_CH3         (BIT_2)   /* this is the channels  */
#define IIR_CH4         (BIT_2|BIT_1)   /* this is the channels  */
#define IIR_CH5         (BIT_3)   /* this is the channels  */
#define IIR_CH6         (BIT_1|BIT_3)   /* this is the channels  */
#define IIR_CH7         (BIT_2|BIT_3)   /* this is the channels  */
#define IIR_CH8         (BIT_1|BIT_2|BIT_3)
#define IIR_CH9         (BIT_4)
#define IIR_CH10         (BIT_1|BIT_4)   /* this is the channels  */
#define IIR_CH11         (BIT_2|BIT_4)   /* this is the channels  */
#define IIR_CH12         (BIT_2|BIT_1|BIT_4)   /* this is the channels  */
#define IIR_CH13         (BIT_3|BIT_4)   /* this is the channels  */
#define IIR_CH14         (BIT_1|BIT_3|BIT_4)   /* this is the channels  */
#define IIR_CH15         (BIT_2|BIT_3|BIT_4)   /* this is the channels  */
#define IIR_CH16         (BIT_1|BIT_2|BIT_3|BIT_4)
#define IIR_CH17         (BIT_5)
#define IIR_CH18         (BIT_1|BIT_5)   /* this is the channels  */
#define IIR_CH19         (BIT_2|BIT_5)   /* this is the channels  */
#define IIR_CH20         (BIT_2|BIT_1|BIT_5)   /* this is the channels  */
#define IIR_CH21         (BIT_3|BIT_5)   /* this is the channels  */
#define IIR_CH22         (BIT_1|BIT_3|BIT_5)   /* this is the channels  */
#define IIR_CH23         (BIT_2|BIT_3|BIT_5)   /* this is the channels  */
#define IIR_CH24         (BIT_1|BIT_2|BIT_3|BIT_5)
#define IIR_CH25         (BIT_4|BIT_5)
#define IIR_CH26         (BIT_1|BIT_4|BIT_5)   /* this is the channels  */
#define IIR_CH27         (BIT_2|BIT_4|BIT_5)   /* this is the channels  */
#define IIR_CH28         (BIT_2|BIT_1|BIT_4|BIT_5)   /* this is the channels  */
#define IIR_CH29         (BIT_3|BIT_4|BIT_5)   /* this is the channels  */
#define IIR_CH30         (BIT_1|BIT_3|BIT_4|BIT_5)   /* this is the channels  */
#define IIR_CH31         (BIT_2|BIT_3|BIT_4|BIT_5)   /* this is the channels  */
#define IIR_CH32         (BIT_1|BIT_2|BIT_3|BIT_4|BIT_5)

#define IIR_DMAEN        (BIT_8)
#define IIR_CAI          (BIT_9)
#define IIR_SS           (BIT_10)
#define IIR_CCINTR       (BIT_11)
#define IIR_FORTYBIT     (BIT_12)

#define IIR_RND0         0x00000000   /* IEEE round to nearest (even) */
#define IIR_RND1         (BIT_14)   /* IEEE round to zero */
#define IIR_RND2         (BIT_15)   /* IEEE round to +ve infinity */
#define IIR_RND3         (BIT_14|BIT_15)   /* IEEE round to -ve infinity */
#define IIR_RND4         (BIT_16)   /* Round to nearest Up */
#define IIR_RND5         (BIT_16|BIT_14)   /* Round away from zero */

/* IIRCTL2 bit definitions */

#define IIR_BIQUADS1     (0x00000000)
#define IIR_BIQUADS2     (BIT_0)
#define IIR_BIQUADS3     (BIT_1)
#define IIR_BIQUADS4     (BIT_0|BIT_1)
#define IIR_BIQUADS5     (BIT_2)
#define IIR_BIQUADS6     (BIT_0|BIT_2)
#define IIR_BIQUADS7     (BIT_1|BIT_2)
#define IIR_BIQUADS8     (BIT_0|BIT_1|BIT_2)
#define IIR_BIQUADS9     (BIT_3)
#define IIR_BIQUADS10     (BIT_0|BIT_3)
#define IIR_BIQUADS11     (BIT_1|BIT_3)
#define IIR_BIQUADS12    (BIT_0|BIT_1|BIT_3)


/* IIR_DMASTAT bit definitions */

#define IIR_DMACPL       (BIT_0)
#define IIR_DMACnDkLD    (BIT_1)
#define IIR_DMAPPGS      (BIT_2)
#define IIR_DMAWRBK      (BIT_3)
#define IIR_DMASVDk      (BIT_4)
#define IIR_DMAWDONE     (BIT_5)
#define IIR_DMAACDONE    (BIT_6)
#define IIR_DMACURCHNL  /* 5-bits for this */

/* IIR_DMASTAT  bit definitions */

#define IIR_MACMRZ       (BIT_0)
#define IIR_MACMRI       (BIT_1)
#define IIR_MACMINV      (BIT_2)
#define IIR_MACARZ       (BIT_3)
#define IIR_MACARI       (BIT_4)
#define IIR_MACAINV      (BIT_5)

/* IIRDEBUGCTL bit definitions */

#define IIR_DBGMODE			(BIT_0)
#define IIR_HLD				(BIT_1)
#define IIR_RUN				(BIT_2)
#define IIR_DBGMEM			(BIT_4)
#define IIR_ADRINC			(BIT_5)

/* FFTCTL1 bit definitions */
#define FFT_RST        (BIT_0)
#define FFT_EN         (BIT_1)
#define FFT_START      (BIT_2)
#define FFT_DMAEN      (BIT_3)
#define FFT_DBG        (BIT_6)


/* FFTMACSTAT bit definitions */
#define FFT_NAN        (BIT_0)
#define FFT_DENORM     (BIT_1)
#define FFT_OVR        (BIT_2)
#define FFT_UDR        (BIT_3)


/* FFTDADDR code definitions */
#define FFT_RDDATA       0x00000000
#define FFT_WRDATA       (BIT_12)
#define FFT_RDCOEFF      (BIT_11)
#define FFT_WRCOEFF      (BIT_11|BIT_12)
#define FFT_RDCACHADDR   (BIT_10)
#define FFT_WRCACHADDR   (BIT_10|BIT_12)
#define FFT_RDCACHDATA   (BIT_6|BIT_10)
#define FFT_WRCACHDATA   (BIT_6|BIT_10|BIT_12)  
#define FFT_RDSTATE      (BIT_10|BIT_11) 
#define FFT_WRSTATE      (BIT_10|BIT_11|BIT_12)

/* FFTCTL2 bit definitions */
#define FFT_RPT          (BIT_0)
#define FFT_CPACKIN      (BIT_1)
#define FFT_CPACKOUT     (BIT_2)
#define FFT_LOG2VDIM_4   (BIT_5)
#define FFT_LOG2VDIM_5   (BIT_3|BIT_5)
#define FFT_LOG2VDIM_6   (BIT_4|BIT_5)
#define FFT_LOG2VDIM_7   (BIT_3|BIT_4|BIT_5)
#define FFT_LOG2VDIM_8   (BIT_6)
#define VDIMDIV16_1      (BIT_7)
#define VDIMDIV16_2      (BIT_8)
#define VDIMDIV16_4      (BIT_9)
#define VDIMDIV16_8      (BIT_10)
#define VDIMDIV16_16     (BIT_11)
#define FFT_LOG2HDIM_4   (BIT_14)
#define FFT_LOG2HDIM_5   (BIT_12|BIT_14)
#define FFT_LOG2HDIM_6   (BIT_13|BIT_14)
#define FFT_LOG2HDIM_7   (BIT_12|BIT_13|BIT_14)
#define FFT_LOG2HDIM_8   (BIT_15)
#define HDIMDIV16_1      (BIT_16)
#define HDIMDIV16_2      (BIT_17)
#define HDIMDIV16_4      (BIT_18)
#define HDIMDIV16_8      (BIT_19)
#define HDIMDIV16_16     (BIT_20)
#define NOVER256_1       (BIT_21)
#define NOVER256_2       (BIT_22)
#define NOVER256_4       (BIT_23)
#define NOVER256_8       (BIT_24)
#define NOVER256_16      (BIT_25)
#define NOVER256_32      (BIT_26)
#define NOVER256_64      (BIT_27)
#define NOVER256_128     (BIT_28)

/* FFTDMASTAT and FFTSHDMASTAT  bit definitions */
#define ICPLD            (BIT_0)  
#define IDMASTAT         (BIT_1)
#define IDMACHIRPT       (BIT_2)
#define OCPLD            (BIT_3)
#define ODMASTAT         (BIT_4)
#define ODMACHIRPT       (BIT_5)



#if !defined(__ADSP21467__)

/* MLB Bit-Definitions */
/* MLB_DCCR bit definitions */

#define MLB_MDE          (BIT_31)  /* MLB Enable */
#define MLB_LBM          (BIT_30)  /* Loop-Back Mode enable */
#define MLB_CLK256       (0x00000000)  /* 256Fs MLB Clock Select */
#define MLB_CLK1024      (BIT_29)  /* 1024Fs MLB Clock Select */
#define MLB_CLK512       (BIT_28)  /* 512Fs MLB Clock Select */
#define MLB_M5PS         (BIT_27)  /* MLB 5-pin select */
#define MLB_MLK          (BIT_26)  /* MLB Lock. When set, indicates that MLB port is synchronized to the incoming MLB frame */
#define MLB_MLE          (BIT_25)  /* MLB Little-Endian Mode */
#define MLB_MHRE         (BIT_24)  /* MLB Hardware REset Enable. Enables hardware to automatically reset the MLB physical and link layer logic upon the reception of MLBReset (FEh) system command*/
#define MLB_MRS          (BIT_23)  /* MLB software reset. When set, resets the MLB physical and link layer logic. Hardware clears this bit automatically*/

/* MLB_SSCR bit definitions */

#define MLB_SDR          (BIT_0)  /* System detects Reset Command */
#define MLB_SDNL	 (BIT_1)  /* System detects Network Lock */
#define MLB_SDNU	 (BIT_2)  /* System detects Network Unlock */
#define MLB_SDCS	 (BIT_3)  /* System detects Channel Scan */
#define MLB_SDSC	 (BIT_4)  /* System detects Subcommand */
#define MLB_SDML	 (BIT_5)  /* System detects MLB Lock */
#define MLB_SDMU	 (BIT_6)  /* System detects MLB Unlock */
#define MLB_SSRE 	 (BIT_7)  /* System detects Request Enable */

#if 0
MLB_SDCR  This register allows system software to receive control information from the MLB controller. 
SDCR is update once per frame by the hardware during the MLB system channel. 
This register is loaded with the data from the MLBDAT_IN during the system channel quadlet. 
System software must read SDCR before the start of the next MLB frame to prevent the current data from being lost.
#endif

/* MLB_SMCR  bit definitions */

#define MLB_SMR          (BIT_0)  /* System Masks Reset */
#define MLB_SMNL	 (BIT_1)  /* System Masks Network Lock */
#define MLB_SMNU	 (BIT_2)  /* System Masks Network Unlock */
#define MLB_SMCS	 (BIT_3)  /* System Masks Channel Scan */ 
#define MLB_SMSC	 (BIT_4)  /* System Masks Subcommand */
#define MLB_SMML	 (BIT_5)  /* System Masks MLB Lock */
#define MLB_SMMU	 (BIT_6)  /* System Masks MLB Unlock */


/* MLB_SBCR Register Description:  The base address registers MLB_SBCR holds the base address of the system memory buffers of all synchronous channels in the device. 
Bits [20:16] hold the base address for RX channels and bits [4:0] holds the base address for TX channels */

/* MLB_ABCR Register Description : The base address registers MLB_ABCR holds the base address of the system memory buffers of all asynchronous channels in the device.
Bits [20:16] hold the base address for RX channels and bits [4:0] holds the base address for TX channels */

/* MLB_CBCR Register Description : The base address registers MLB_CBCR holds the base address of the system memory buffers of all control channels in the device. 
Bits [20:16] hold the base address for RX channels and bits [4:0] holds the base address for TX channels */

/* MLB_IBCR Register Description : The base address registers MLB_IBCR holds the base address of the system memory buffers of all isochronous channels in the device. 
Bits [20:16] hold the base address for RX channels and bits [4:0] holds the base address for TX channels */

#if 0
MLB_CICR Register Description : The Channel interrupt status register reflects the channel interrupt status of the individual logical channels. 
These bits are set by hardware when a channel interrupt is generated. 
The channel interrupt bits are sticky and can only be reset by software.
To clear a particular bit in CICR, software must clear all of the unmasked status bits in the corresponding CSCRn registers
#endif

/* MLB_CECRn  For Asynchronous and Control Channels : bit definitions */
#define MLBAC_CE         (BIT_31)	/* Channel n Enable */
#define MLBAC_CTRAN      (BIT_30)	/* Channel n Transmit select */	
#define MLBAC_CHSYNC      (0x00000000)	/* Synchronous - Channel n Type Select */
#define MLBAC_CHISO      (BIT_28)	/* Isochronous - Channel n Type Select */
#define MLBAC_CHASYNC    (BIT_29)	/* Asynchronous - Channel n Type Select */
#define MLBAC_CHCTRL     (BIT_28|BIT_29)/* Control - Channel n Type Select */
#define MLBAC_PCE        (BIT_27)	/* Packet Count Enable. Software can set this bit to enable the Rx packet counter. This is valid for asynchronous and control Rx channels in IO mode*/
#define MLBAC_MDSPPDMA   (0x00000000)  /* ping-pong DMA mode (default) */
#define MLBAC_MDSCIRDMA  (BIT_25)     /* Circular DMA  */
#define MLBAC_MDSIO      (BIT_26)     /* IO mode enable */
#define MLBAC_MAASK0     (BIT_16)     /* Mask protocol error. When set, masks protocol error channel interrupts for this logical channel. This bit valid for all RX channel types. This is valid for asynchronous and control TX channels only. */
#define MLBAC_MAASK1     (BIT_17)     /* Mask detect break. When set, masks detect break channel interrupt for this logical channel. This bit is valid for Asynchronous and Control channels only. */
#define MLBAC_MAASK2IO     (BIT_18)   /* IO mode : Masks receive service request. When set, masks RX channel service request interrupts for this logical channel. */
#define MLBAC_MAASK2DMA     (BIT_18)   /* DMA mode: Mask Buffer Done .When set, masks buffer done channel interrupts for this logical channel. */
#define MLBAC_MAASK3IO     (BIT_19)   /* IO mode: Masks transmit service request. When set, masks TX channel service request interrupts for this logical channel. */
#define MLBAC_MAASK3DMA    (BIT_19)   /* DMA mode: Mask Buffer Start .When set, masks buffer start channel interrupts for this logical channel. */
#define MLBAC_MAASK4       (BIT_20)   /* Mask Buffer error. When set, masks buffer error channel interrupts for this logical channel. */
#define MLBAC_MAASK6       (BIT_22)   /* Mask lost frame synchronization. When set, masks Lost Frame Synchronization channel interrupts for this logical channel. */
#define MLBAC_PCTH0        (0x00000000) /* Packet Count Threshold : IO Mode. Software can program this field with the number of packets to receive before generating an RX packet count service request.This service request is generated independant of and in addition to other service requests generated via the standard buffer threshold mechanism  */
#define MLBAC_PCTH1        (BIT_8)
#define MLBAC_PCTH2        (BIT_9)
#define MLBAC_PCTH3        (BIT_8|BIT_9)
#define MLBAC_PCTH4        (BIT_10)
#define MLBAC_PCTH5        (BIT_8|BIT_10)
#define MLBAC_PCTH6        (BIT_9|BIT_10)
#define MLBAC_PCTH7        (BIT_8|BIT_9|BIT_10)
#define MLBAC_PCTH8        (BIT_11)
#define MLBAC_PCTH9        (BIT_8|BIT_11)
#define MLBAC_PCTH10        (BIT_9|BIT_11)
#define MLBAC_PCTH11        (BIT_8|BIT_9|BIT_11)
#define MLBAC_PCTH12        (BIT_10|BIT_11)
#define MLBAC_PCTH13        (BIT_8|BIT_10|BIT_11)
#define MLBAC_PCTH14        (BIT_10|BIT_11|BIT_9)
#define MLBAC_PCTH15        (BIT_8|BIT_9|BIT_10|BIT_11)
#define MLBAC_PCTH16        (BIT_12)
#define MLBAC_PCTH17        (BIT_8|BIT_12)
#define MLBAC_PCTH18        (BIT_9|BIT_12)
#define MLBAC_PCTH19        (BIT_8|BIT_9|BIT_12)
#define MLBAC_PCTH20        (BIT_10|BIT_12)
#define MLBAC_PCTH21        (BIT_8|BIT_10|BIT_12)
#define MLBAC_PCTH22        (BIT_9|BIT_10|BIT_12)
#define MLBAC_PCTH23        (BIT_8|BIT_9|BIT_10|BIT_12)
#define MLBAC_PCTH24        (BIT_11|BIT_12)
#define MLBAC_PCTH25        (BIT_8|BIT_11|BIT_12)
#define MLBAC_PCTH26        (BIT_9|BIT_11|BIT_12)
#define MLBAC_PCTH27        (BIT_8|BIT_9|BIT_11|BIT_12)
#define MLBAC_PCTH28        (BIT_10|BIT_11|BIT_12)
#define MLBAC_PCTH29        (BIT_8|BIT_10|BIT_11|BIT_12)
#define MLBAC_PCTH30        (BIT_10|BIT_11|BIT_9|BIT_12)
#define MLBAC_PCTH31        (BIT_8|BIT_9|BIT_10|BIT_11|BIT_12)

/* MLB_CECRn for Isochronous Channels : bit definitions */

#define MLBISO_CE           (BIT_31)	/* Channel n Enable */
#define MLBISO_CTRAN        (BIT_30)	/* Channel n Transmit Select */
#define MLBISO_CHSYNC       (0x00000000)/* Synchronous - Channel n Type Select */
#define MLBISO_CHISO        (BIT_28)	/* Isochronous - Channel n Type Select */
#define MLBISO_CHASYNC      (BIT_29)	/* Asynchronous - Channel n Type Select */
#define MLBISO_CHCTRL       (BIT_28|BIT_29)/* Control - Channel n Type Select */
#define MLBISO_FCE          (BIT_27)   /* Flow Control Enable. When set, allows an isochronous Rx channel to generate the ReceiverBusy(10h) response */
#define MLBISO_MDSPPDMA   (0x00000000)   /* ping-pong DMA mode (default) */
#define MLBISO_MDSCIRDMA  (BIT_25)   /* Circular DMA  */
#define MLBISO_MDSIO      (BIT_26)   /* IO mode enable */
#define MLBISO_MAASK0     (BIT_16)   /* Mask protocol error. When set, masks protocol error channel interrupts for this logical channel. This bit valid for all RX channel types. This is valid for asynchronous and control TX channels only. */
#define MLBISO_MAASK1     (BIT_17)   /* Mask detect break. When set, masks detect break channel interrupt for this logical channel. This bit is valid for Asynchronous and Control channels only. */
#define MLBISO_MAASK2IO     (BIT_18)   /* IO mode : Masks receive service request. When set, masks RX channel service request interrupts for this logical channel. */
#define MLBISO_MAASK2DMA     (BIT_18)   /* DMA mode: Mask Buffer Done .When set, masks buffer done channel interrupts for this logical channel. */
#define MLBISO_MAASK3IO     (BIT_19)   /* IO mode: Masks transmit service request. When set, masks TX channel service request interrupts for this logical channel. */
#define MLBISO_MAASK3DMA    (BIT_19)   /* DMA mode: Mask Buffer Start .When set, masks buffer start channel interrupts for this logical channel. */
#define MLBISO_MAASK4       (BIT_20)   /* Mask Buffer error. When set, masks buffer error channel interrupts for this logical channel. */
#define MLBISO_MAASK6       (BIT_22)   /* Mask lost frame synchronization. When set, masks Lost Frame Synchronization channel interrupts for this logical channel. */

/* MLB_CECRn for Synchronous Channels: Bit definitions */

#define MLBSY_CE            (BIT_31)      /* Channel n Enable */
#define MLBSY_CTRAN         (BIT_30)      /* Channel n Transmit Select */
#define MLBSY_CHSYNC        (0x00000000)  /* Synchronous - Channel n Type Select */
#define MLBSY_CHISO         (BIT_28)      /* Isochronous - Channel n Type Select */
#define MLBSY_CHASYNC       (BIT_29)      /* Asynchronous - Channel n Type Select */ 
#define MLBSY_CHCTRL        (BIT_28|BIT_29)  /* Control - Channel n Type Select */
#define MLBSY_FSE           (BIT_27)      /* Frame Synchronization Enable. When set, enables streaming channel frame synchronization for this logical synchronous channel*/
#define MLBSY_MDSPPDMA   (0x00000000)   /* ping-pong DMA mode (default) */
#define MLBSY_MDSCIRDMA  (BIT_25)   /* Circular DMA  */
#define MLBSY_MDSIO      (BIT_26)   /* IO mode enable */
#define MLBSY_MAASK0     (BIT_16)   /* Mask protocol error. When set, masks protocol error channel interrupts for this logical channel. This bit valid for all RX channel types. This is valid for asynchronous and control TX channels only. */
#define MLBSY_MAASK1     (BIT_17)   /* Mask detect break. When set, masks detect break channel interrupt for this logical channel. This bit is valid for Asynchronous and Control channels only. */
#define MLBSY_MAASK2IO     (BIT_18)   /* IO mode : Masks receive service request. When set, masks RX channel service request interrupts for this logical channel. */
#define MLBSY_MAASK2DMA     (BIT_18)   /* DMA mode: Mask Buffer Done .When set, masks buffer done channel interrupts for this logical channel. */
#define MLBSY_MAASK3IO     (BIT_19)   /* IO mode: Masks transmit service request. When set, masks TX channel service request interrupts for this logical channel. */
#define MLBSY_MAASK3DMA    (BIT_19)   /* DMA mode: Mask Buffer Start .When set, masks buffer start channel interrupts for this logical channel. */
#define MLBSY_MAASK4       (BIT_20)   /* Mask Buffer error. When set, masks buffer error channel interrupts for this logical channel. */
#define MLBSY_MAASK6       (BIT_22)   /* Mask lost frame synchronization. When set, masks Lost Frame Synchronization channel interrupts for this logical channel. */
#define MLBSY_FSCD          (BIT_15)      /* Frame Synchronization Channel Disable. When set, disables this logical channel when lost frame synchronization occurs */
#define MLBSY_FSPC0         (0x00000000)  /*Frame Synchronization Physical Channel Count defines the number of physical channels expected to match this logical channel's ChannelAddress each MLB frame */
#define MLBSY_FSPC1        (BIT_8)
#define MLBSY_FSPC2        (BIT_9)
#define MLBSY_FSPC3        (BIT_8|BIT_9)
#define MLBSY_FSPC4        (BIT_10)
#define MLBSY_FSPC5        (BIT_8|BIT_10)
#define MLBSY_FSPC6        (BIT_9|BIT_10)
#define MLBSY_FSPC7        (BIT_8|BIT_9|BIT_10)
#define MLBSY_FSPC8        (BIT_11)
#define MLBSY_FSPC9        (BIT_8|BIT_11)
#define MLBSY_FSPC10        (BIT_9|BIT_11)
#define MLBSY_FSPC11        (BIT_8|BIT_9|BIT_11)
#define MLBSY_FSPC12        (BIT_10|BIT_11)
#define MLBSY_FSPC13        (BIT_8|BIT_10|BIT_11)
#define MLBSY_FSPC14        (BIT_10|BIT_11|BIT_9)
#define MLBSY_FSPC15        (BIT_8|BIT_9|BIT_10|BIT_11)
#define MLBSY_FSPC16        (BIT_12)
#define MLBSY_FSPC17        (BIT_8|BIT_12)
#define MLBSY_FSPC18        (BIT_9|BIT_12)
#define MLBSY_FSPC19        (BIT_8|BIT_9|BIT_12)
#define MLBSY_FSPC20        (BIT_10|BIT_12)
#define MLBSY_FSPC21        (BIT_8|BIT_10|BIT_12)
#define MLBSY_FSPC22        (BIT_9|BIT_10|BIT_12)
#define MLBSY_FSPC23        (BIT_8|BIT_9|BIT_10|BIT_12)
#define MLBSY_FSPC24        (BIT_11|BIT_12)
#define MLBSY_FSPC25        (BIT_8|BIT_11|BIT_12)
#define MLBSY_FSPC26        (BIT_9|BIT_11|BIT_12)
#define MLBSY_FSPC27        (BIT_8|BIT_9|BIT_11|BIT_12)
#define MLBSY_FSPC28        (BIT_10|BIT_11|BIT_12)
#define MLBSY_FSPC29        (BIT_8|BIT_10|BIT_11|BIT_12)
#define MLBSY_FSPC30        (BIT_10|BIT_11|BIT_9|BIT_12)
#define MLBSY_FSPC31        (BIT_8|BIT_9|BIT_10|BIT_11|BIT_12)




/* MLB_CSCRn Register Description: Bit definitions */

#define MLB_BM     	  	 (BIT_31)          /* Channel n Buffer Empty */
#define MLB_BF     	  	 (BIT_30)	   /* Channel n Buffer Full */	
#define MLB_IVB0   	   	 (0x00000000)      /* 0 Isochronous Valid Bytes */
#define MLB_IVB1   	   	 (BIT_18)          /* 1 Isochronous Valid Bytes */
#define MLB_IVB2  	         (BIT_19)          /* 2 Isochronous Valid Bytes */
#define MLB_IVB3   	   	 (BIT_18|BIT_19)   /* 3 Isochronous Valid Bytes */      
#define MLB_GIRB  	  	 (BIT_17)          /* Generate Isochoronous Receive Break for isochronous channels */
#define MLB_GBREAK  	  	 (BIT_17)	   /* Generate Break for Asynchronous and Control Channels */
#define MLB_RDYPPDMA	 	 (BIT_16)          /* Next Buffer Ready for ping-pong DMA */
#define MLB_RDYCIRDMA	  	 (BIT_16)          /* Next Buffer Ready for circular buffer DMA */

/* If RDY bit is set before processing of the Current Buffer is complete,status for the Current buffer will be reported using STS bits [11:8]
(status bits for the previous buffer), and STS bits [3:0](status bits for the Current buffer) will not be updated. The STS bits [3:0]are only 
updated when processing of the current buffer is complete and the RDY bit has not yet been set */

/* A 'Done' status is always generated when the processing of a buffer has finished, even if a 'Break' or 'Error' condition was detected during 
the packet processing.If 'Break' or 'Error' occured,the 'Done' status bits will be set in addition to the 'Break' or 'Error' status bits */

#define MLB_PBS	         (BIT_11)	   /* DMA-Previous Buffer Start         - Channel n Status */
#define MLB_PBD		 (BIT_10)	   /* DMA-Previous Buffer Done          - Channel n Status */
#define MLB_PBDB         (BIT_9)	   /* DMA-Previous Buffer Detect Break  - Channel n Status */
#define MLB_RXPS	 (BIT_9)	   /* IO-Receiver Packet Start          - Channel n Status*/
#define MLB_RXPA         (BIT_8)	   /* IO-Receive Packet Abort           - Channel n Status */
#define MLB_PBPERR       (BIT_8)	   /* DMA-Previous Buffer Protocol Error- Channel n Status */
#define MLB_LOFS         (BIT_6)	   /* Lost Frame Synchronization        - Channel n Status */
#define MLB_HBERR        (BIT_5)	   /* DMA-Host Bus Error                - Channel n Status */
#define MLB_BERR         (BIT_4)	   /* Buffer Error for sync Rx/Tx and isochronous Rx channels - Channel n Status */
#define MLB_CBS          (BIT_3)	   /* DMA-Current Buffer Start          - Channel n Status */
#define MLB_TXSRQ        (BIT_3)	   /* IO-Transmit Service Request       - Channel n Status */
#define MLB_RXSRQ        (BIT_2)	   /* IO-Receive Service Request        - Channel n Status */
#define MLB_CBD          (BIT_2)	   /* DMA-Current Buffer Done           - Channel n Status */
#define MLB_CBDB         (BIT_1)	   /* Current Buffer Detect Break for async and control channels - Channel n Status */
#define MLB_CBPERR       (BIT_0)	   /* Current Buffer Protocol Error for other channels - Channel n Status */

#endif /* __ADSP21467__ */

/* LCTLn Register Description : bit definitons */

#define LEN         (BIT_0)
#define LDEN        (BIT_1)
#define LCHEN       (BIT_2)
#define LTRAN       (BIT_3)
#define LP_BHD      (BIT_7)
#define LTRQ_MSK    (BIT_8)
#define LRRQ_MSK    (BIT_9)
#define DMACH_IRPT_MSK    (BIT_10)
#define LPIT_MSK          (BIT_11)
#define EXTTXFR_DONE_MSK  (BIT_12)

/* LSTATn Register Descritption: bit definitions */

#define LTRQ         (BIT_0)
#define LRRQ         (BIT_1)
#define DMACH_IRPT   (BIT_2)
#define LPIT         (BIT_3)
#define EXTTXFR_DONE  (BIT_4)
#define FFST_EMPTY    (0x00000000)
#define FFST_1WORD    (BIT_6)
#define FFST_FULL     (BIT_6|BIT_5)
#define LERR          (BIT_7)
#define LPBS          (BIT_8)



/* DTCP module */

#if defined(__ADSP21462__) || defined(__ADSP21465__)

#define DTCPOFF (BIT_3)  /* DTCP Clock Shutdown  - only on ROM parts */ 

#endif




#ifdef _MISRA_RULES
#pragma diag(pop)
#endif /* _MISRA_RULES */

#endif /* __DEF21469_H__ */

