
#ifndef __INITCPU_H__
#define __INITCPU_H__

#ifdef __cplusplus
extern "C" {
#endif

//// CP15 R1
#define R1_M    (1<<0)
#define R1_A    (1<<1)
#define R1_C    (1<<2)
#define R1_W    (1<<3)
#define R1_Z    (1<<11)
#define R1_I    (1<<12)
#define R1_V    (1<<13)
#define R1_RR   (1<<14)
#define R1_VE   (1<<24)
#define R1_nF   (1<<30)
#define R1_iA   (1<<31)

//CLK
#define CONFIG_CLK_534_133_66
//#define CONFIG_CLK_400_133_66
//#define CONFIG_CLK_267_133_66

/*
MSYSCLK = MPLL = (Startup_MDIV*Fin)/(Startup_PDIV*(2<<Startup_SDIV))
HCLK = MSYSCLK/HCLK_Ratio = MSYSCLK/((PREDIV+1)*(HCLKDIV+1))
ARMCLK = MSYSCLK/ARMCLK_Ratio = MSYSCLK/((ARMDIV[12]+1)*(ARMDIV[11:9]+1))
 */
#if defined(CONFIG_CLK_400_133_66)/* Output CLK 800MHz 2:4:8*/
#define ARMCLKVAL						400
#define HCLKVAL							133
#define PCLKVAL							66
#define Startup_MDIV					400
#define Startup_PDIV					3
#define Startup_SDIV					1
#define Startup_MPLLSTOP				0
#define Startup_EMDIV					32
#define Startup_EPDIV					1
#define Startup_ESDIV					2
#define Startup_EPLLSTOP				0
#define Startup_ARMCLKdiv				1
#define Startup_PREdiv					0x2
#define Startup_HCLKdiv					0x1
#define Startup_PCLKdiv					1
#define Startup_SSMCdiv					1
#elif defined (CONFIG_CLK_534_133_66) // MPLL = 533, EPLL = 96, HCLK = 133, PCLK = 66, ARMCLK/FCLK = 266
#define ARMCLKVAL						534
#define HCLKVAL							133
#define PCLKVAL							66
#define Startup_MDIV					267
#define Startup_PDIV					3
#define Startup_SDIV					1
#define Startup_MPLLSTOP				0
#define Startup_EMDIV					32
#define Startup_EPDIV					1
#define Startup_ESDIV					2
#define Startup_EPLLSTOP				0
#define Startup_ARMCLKdiv				0
#define Startup_PREdiv					0x1
#define Startup_HCLKdiv					0x1
#define Startup_PCLKdiv					1
#define Startup_SSMCdiv					1
#elif defined (CONFIG_CLK_267_133_66)
#define ARMCLKVAL						267
#define HCLKVAL							133
#define PCLKVAL							66
#define Startup_MDIV					267
#define Startup_PDIV					3
#define Startup_SDIV					1
#define Startup_MPLLSTOP				0
#define Startup_EMDIV					32
#define Startup_EPDIV					1
#define Startup_ESDIV					2
#define Startup_EPLLSTOP				0
#define Startup_ARMCLKdiv				1
#define Startup_PREdiv					0x1
#define Startup_HCLKdiv					0x1
#define Startup_PCLKdiv					1
#define Startup_SSMCdiv					1
#else
# error Must define CONFIG_CLK_534_133_66 or CONFIG_CLK_400_133_66
#endif

#define CLK_DIV_VAL						((Startup_ARMCLKdiv<<9)|(Startup_PREdiv<<4)|(Startup_SSMCdiv<<3)|(Startup_PCLKdiv<<2)|(Startup_HCLKdiv))
#define MPLL_VAL						((Startup_MPLLSTOP<<24)|(Startup_MDIV<<14)|(Startup_PDIV<<5)|(Startup_SDIV))
#define EPLL_VAL						((Startup_EPLLSTOP<<24)|(Startup_EMDIV<<16)|(Startup_EPDIV<<8)|(Startup_ESDIV<<0))//96MHz

//BANKCFG Register: DDR2 SDRAM configure
#define RASBW0		2			//RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
#define RASBW1		2			//RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
#define CASBW0		2			//CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
#define CASBW1		2			//CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
#define ADDRCFG0	1			//addre configure; 00={BA,RAS,CAS}, 01={RAS,BA,CAS}
#define ADDRCFG1	1			//addre configure; 00={BA,RAS,CAS}, 01={RAS,BA,CAS}
#define MEMCFG		1			//Ext.Mem 000=SDR,010=MSDR,100=DDRz,110=MDDR,001=DDR2
#define BW			1			//Bus width 00=32bit,01=16bit

//BANKCON1 Register: DDR2 SDRAM timing control
#define PADLOAD		1
#define BStop		0			//read burst stop control
#define WBUF		1			//write buffer control
#define AP			0			//auto precharge control
#define PWRDN		0			//power down mode
#define DQSDelay	4			//DQS delay

//BANKCON2 Register: DDR2 SDRAM timing control
#define tRAS		5			//Row active time
#define tARFC		13			//Row cycle time
#define CL			3			//CAS latency control
#define tRCD		1			//RAS to CAS delay
#define tRP			1			//Row pre-charge time

//BANKCON3 register: DDR2 SDRAM MRS/EMRS register
#define BA_EMRS1			1	//BA : EMRS
#define DLL_DIS				1
#define DLL_EN				0
#define nDQS_DIS			1
#define RDQS_DIS			0
#define OCD_MODE_EXIT		0
#define OCD_MODE_DEFAULT	7
#define BA_EMRS2			2	//BA: EMRS
#define BA_EMRS3			3	//BA: EMRS
#define DS					0	//Driver strength
#define PASR				0	//PASR
#define BA_MRS				0	//BA : MRS
#define TM					0	//Test Mode - mode register set
#define CL_MRS				3	//CAS Latency
#define DLL_RESET_HIGH		1
#define DLL_RESET_LOW		0
#define REFCYC				1037	//Refresh cycle

#define BANKCFGVAL 	((RASBW0<<17)+(RASBW1<<14)+(CASBW0<<11)+(CASBW1<<8)+(ADDRCFG0<<6)+(ADDRCFG1<<4)+(MEMCFG<<1)+(BW<<0))
#define BANKCON1VAL	((DQSDelay<<28)+(1<<26)+(BStop<<7)+(WBUF<<6)+(AP<<5)+(PWRDN<<4))
#define BANKCON2VAL	((tRAS<<20)+(tARFC<<16)+(CL<<4)+(tRCD<<2)+(tRP<<0))


#if 0
#define	NFCONF		0x4E000000		//NAND Flash configuration
#define	NFCONT		0x4E000004      //NAND Flash control
#define	NFCMD		0x4E000008      //NAND Flash command
#define	NFADDR		0x4E00000C      //NAND Flash address
#define	NFDATA		0x4E000010      //NAND Flash data
#define	NFMECCD0	0x4E000014      //NAND Flash ECC for Main
#define	NFMECCD1	0x4E000018      //NAND Flash ECC for Main
#define	NFSECCD		0x4E00001C	  	//NAND Flash ECC for Spare Area
#define	NFSBLK		0x4E000020		//NAND Flash programmable start block address
#define	NFEBLK		0x4E000024 	    //NAND Flash programmable end block address
#define	NFSTAT		0x4E000028      //NAND Flash operation status
#define	NFECCERR0	0x4E00002C      //NAND Flash ECC Error Status for I/O [7:0]
#define	NFECCERR1	0x4E000030      //NAND Flash ECC Error Status for I/O [15:8]
#define	NFMECC0		0x4E000034      //SLC or MLC NAND Flash ECC status
#define	NFMECC1		0x4E000038	    //SLC or MLC NAND Flash ECC status
#define	NFSECC		0x4E00003C  	//NAND Flash ECC for I/O[15:0]
#define	NFMLCBITPT	0x4E000040  	//NAND Flash 4-bit ECC Error Pattern for data[7:0]
#endif

#ifdef __cplusplus
}
#endif

#endif	/*__INITCPU_H__*/

