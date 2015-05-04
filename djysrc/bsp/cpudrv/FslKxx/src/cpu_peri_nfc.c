// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_nfc.c
// 模块描述: NFC模块的底层函数，主要初始化NFC模块，并操作NFC控制器读取数据等
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 02/08.2014
// =============================================================================

#include "cpu_peri.h"
#include "string.h"

// 若没有对NFC进行配置，则使用默认配置
#ifndef NFC_CONFIG
#define NFC_CE 			NFC_CE0			// 片选使用CE0
#define DATA_WIDTH 		NFC_16BIT		// 数据宽度16比特
#define NFCDIV          0x05			// 时钟分频
#define NFCFRAC         0x00			// 时钟分频
#define PAGE_SIZE 		PAGE_2K			// 页大小
#define ECC_SIZE  		PAGE_64			// ECC大小
#endif

#define NFC_BASE_PTR NFC_BASE

// =============================================================================
// 功能：NFC模块的初始化，包括时钟、MPU、时钟分频、IO配置和寄存器的初始化，对于时钟、
//       片选、数据宽度和页大小、ECC数据大小，应根据具体的NAND芯片而定，本源代码中使
//       用默认宏定义的方式，调用者可根据具体芯片修改或自己重新定义
// 参数：无
// 返回：无
// =============================================================================
void NFC_ModuleInit(void)
{
    /* Enable the NFC clock gate in the SIM */
    SIM->SCGC3 |= SIM_SCGC3_NFC_MASK;

    /* Disable the MPU to allow NFC to access memory */
    MPU->CESR &= 0xFFFFFFFE;

    /* Set the NFC clock divder according to defines in nand.h */
    SIM->CLKDIV4 |= SIM_CLKDIV4_NFCDIV(NFCDIV) |SIM_CLKDIV4_NFCFRAC(NFCFRAC);

    /* Enable the chip select in the NFC and enable the pad for the CS */
    if(NFC_CE == NFC_CE0)
    {
        NFC->RAR |= (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
        PORTC->PCR[17] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;  /* Enable NFC_CE0 pad */
    }
    else /* (NFC_CE == NFC_CE1) */
    {
        NFC->RAR |= (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
        PORTC->PCR[18] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK;  /* Enable NFC_CE0 pad */
    }

    /* Enable all of the NFC control signal pads */
    PORTC->PCR[16] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK; /* Enable the NFC_RB pad */
    PORTD->PCR[8] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK; /* Enable the NFC_CLE pad */
    PORTD->PCR[9] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK; /* Enable the NFC_ALE pad */
    PORTD->PCR[10] =  PORT_PCR_MUX(6) | PORT_PCR_DSE_MASK; /* Enable the NFC_RE pad */
    PORTC->PCR[11] =  PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_WE pad */

    /* Configure bit width in NFC and enable the NFC_IO pins based on the
     * defined port size in nand.h.*/

    if (DATA_WIDTH == NFC_16BIT)
    {
    	NFC->CFG |= NFC_CFG_BITWIDTH_MASK;

        PORTB->PCR[20] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA15 pad */
    	PORTB->PCR[21] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA14 pad */
    	PORTB->PCR[22] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA13 pad */
    	PORTB->PCR[23] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA12 pad */
    	PORTC->PCR[0] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA11 pad */
    	PORTC->PCR[1] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA10 pad */
    	PORTC->PCR[2] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA9 pad */
    	PORTC->PCR[4] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA8 pad */
    	PORTC->PCR[5] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA7 pad */
    	PORTC->PCR[6] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA6 pad */
    	PORTC->PCR[7] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA5 pad */
    	PORTC->PCR[8] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA4 pad */
    	PORTC->PCR[9] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA3 pad */
    	PORTC->PCR[10] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA2 pad */
    	PORTD->PCR[4] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA1 pad */
    	PORTD->PCR[5] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA0 pad */

        /* Set the sector size in the NFC to match the memory */
        NFC->SECSZ = (PAGE_SIZE|(ECC_SIZE+1));
    }
    else /* (DATA_WIDTH == NFC_8BIT) */
    {
    	NFC->CFG &= ~NFC_CFG_BITWIDTH_MASK;

    	PORTC->PCR[5] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA7 pad */
    	PORTC->PCR[6] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA6 pad */
    	PORTC->PCR[7] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA5 pad */
    	PORTC->PCR[8] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA4 pad */
    	PORTC->PCR[9] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA3 pad */
    	PORTC->PCR[10] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA2 pad */
    	PORTD->PCR[4] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA1 pad */
    	PORTD->PCR[5] = PORT_PCR_MUX(5) | PORT_PCR_DSE_MASK; /* Enable the NFC_DATA0 pad */

        /* Set the sector size in the NFC to match the memory */
        NFC->SECSZ = (PAGE_SIZE | ECC_SIZE);
    }

    /* Configure NFC for EDO mode if defined in nand.h */
    #if defined(NFC_EDO)
        NFC->CFG |= NFC_CFG_FAST_MASK;
    #endif

    /* Disable auto address and buff num increment */
    NFC->CFG &= ~(NFC_CFG_AIAD_MASK | NFC_CFG_AIBN_MASK);

    /* Set NFC to STOP if a write error is detected */
    NFC->CFG |= NFC_CFG_STOPWERR_MASK;

    /* Disable row address increment */
    NFC->RAI = 0;

    /* Disable swap */
    NFC->SWAP = 0;
}

// =============================================================================
// 功能：判断本次操作是否完成，即判断R/B#引脚是否已经拉高，若为低，则表示未完成
// 参数：无
// 返回：true,本次操作已经完成;false,未完成
// =============================================================================
bool_t NFC_OptionCompleted(void)
{
	return !(NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);
}

// =============================================================================
// 功能：将NFC相应的片选信号配置为有效，即关闭与某片选上的NAND芯片通信
// 参数：CsNo,片选号，0或1
// 返回：无
// =============================================================================
void NFC_CsActive(u8 CsNo)
{
    if(CsNo == NFC_CE0)
    {
        NFC->RAR |= (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
    }
    else
    {
        NFC->RAR |= (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
    }
}

// =============================================================================
// 功能：将NFC相应的片选信号配置为无效，即关闭与某片选上的NAND芯片通信
// 参数：CsNo,片选号，0或1
// 返回：无
// =============================================================================
void NFC_CsInactive(u8 CsNo)
{
    if(CsNo == NFC_CE0)
    {
        NFC->RAR &= ~(NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
    }
    else
    {
        NFC->RAR &= ~(NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
    }
}

// =============================================================================
// 功能：发送复位命令到NAND芯片，复位芯片，函数返回后，复位时序已经完成
// 参数：无
// 返回：无
// =============================================================================
void NFC_Reset(void)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );

    /* Write the NFC_CMD2 register with the command byte and code for a reset */
    NFC->CMD2 = NFC_CMD2_BYTE1(RESET_CMD_BYTE) | NFC_CMD2_CODE(NFC_RESET_CMD_CODE);

    /* Set Start Bit to send reset to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);
}

// =============================================================================
// 功能：读Flash的ID，返回的ID号芯片驱动可根据具体情况获取有用的信息
// 参数：FlashBufNo,使用的NFC发送数据的BUFFER号，从0-3
//       FlashID0,ID号的高8字节，输出参数
//       FlashID1,ID号的低8字节，输出参数
// 返回：无
// =============================================================================
void NFC_FlashIDRead(u8 FlashBufNo, u32* FlashID0, u32* FlashID1)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );

    /* Write the NFC_CMD2 register with the command byte and code for a reset */
    NFC->CMD2 = NFC_CMD2_BYTE1(READ_ID_CMD_BYTE)
              | NFC_CMD2_CODE(NFC_READ_ID_CMD_CODE)
              | NFC_CMD2_BUFNO(FlashBufNo);

    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);

    /* Load the flash ID values into fls_id0 and fls_id1 */
    if(FlashID0 != NULL)
    {
    	*((u32 *)(FlashID0)) = NFC->SR1;
    }
    if(FlashID1 != NULL)
    {
    	*((u32 *)(FlashID1)) = NFC->SR2;
    }
}

// =============================================================================
// 功能：读flash的状态，状态信息反映了本次操作的结果，如写或擦除等是否成功
// 参数：无
// 返回：Status,状态值，是一个8比特的数值，数值意义根据NAND芯片而定
// =============================================================================
u8 NFC_FlashStatusRead(void)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );

    /* Write the NFC_CMD2 register with the command byte and code for a reset */
    NFC->CMD2 = NFC_CMD2_BYTE1(READ_STATUS_CMD_BYTE)
              | NFC_CMD2_CODE(NFC_READ_STATUS_CODE)
              | NFC_CMD2_BUFNO(0);

    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);

    /* Load the flash ID values into fls_id0 and fls_id1 */

    return (u8)NFC->SR2;
}

// =============================================================================
// 功能：擦除指定的块，擦除的块包括数据段和ECC段
// 参数：RowAddr,块号即是行地址
// 返回：无
// =============================================================================
void NFC_BlockErase(u32 RowAddr)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );

    /* Disable ECC during block erase */
    NFC->CFG &= ~NFC_CFG_ECCMODE(0x7);

    /* Make sure the column address is cleared - not needed for block erase */
    NFC->CAR = 0x0;

    /* Set the chip select to use */
    if(NFC_CE == NFC_CE0)
    {
        NFC->RAR = (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
     }
    else /* (NFC_CE == NFC_CE1) */
    {
        NFC->RAR = (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
    }

    /* Set the row address */
    NFC->RAR |= RowAddr;

    /* Write the NFC_CMD2 register with the command byte and code for an erase */
    NFC->CMD2 = (NFC_CMD2_BYTE1(BLOCK_ERASE_CMD_BYTE1)
              | NFC_CMD2_CODE(NFC_BLOCK_ERASE_CMD_CODE) );

    /* Write the NFC_CMD1 register with the command byte2 and byte3 for an erase */
    NFC->CMD1 = NFC_CMD1_BYTE2(BLOCK_ERASE_CMD_BYTE2)
                | NFC_CMD1_BYTE3(READ_STATUS_CMD_BYTE);

    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);
}

// =============================================================================
// 功能：编程指定的页，在调用该函数前，编程的数据应该已经写入到数据缓冲区中
// 参数：FlashBufNo,使用的NFC发送数据的BUFFER号，从0-3
//       RowAddr,行地址，即所属的页号
//       ColAddr,列地址，即页内地址
// 返回：无
// =============================================================================
void NFC_PageProgram(u8 FlashBufNo, u32 RowAddr, u16 ColAddr)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );

    /* Make sure ECC is enabled before reading */
    NFC->CFG &= ~NFC_CFG_ECCMODE(0x7);

    /* Set the chip select to use */
    if(NFC_CE == NFC_CE0)
    {
        NFC->RAR = (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
     }
    else /* (NFC_CE == NFC_CE1) */
    {
        NFC->RAR = (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
    }

    /* Set the row address */
    NFC->RAR |= RowAddr;

    /* Set the column address */
    NFC->CAR = ColAddr;

    /* Write the NFC_CMD2 register with the command byte and code for an erase */
    NFC->CMD2 = (NFC_CMD2_BYTE1(PROGRAM_PAGE_CMD_BYTE1)
              | NFC_CMD2_CODE(NFC_PROGRAM_PAGE_CMD_CODE)
              | NFC_CMD2_BUFNO(FlashBufNo));

    /* Write the NFC_CMD1 register with the command byte2 and byte3 for an erase */
    NFC->CMD1 = NFC_CMD1_BYTE2(PROGRAM_PAGE_CMD_BYTE2)
                | NFC_CMD1_BYTE3(READ_STATUS_CMD_BYTE);

    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);
}

// =============================================================================
// 功能：读取指定页数据，在函数返回后，调用者可读取缓冲区中的数据
// 参数：FlashBufNo,使用的NFC发送数据的BUFFER号，从0-3
//       RowAddr,行地址，即所属的页号
//       ColAddr,列地址，即页内地址
// 返回：无
// =============================================================================
void NFC_PageRead(u8 FlashBufNo, u32 RowAddr, u16 ColAddr)
{
    /* Clear all status and error bits in the NFC_ISR register */
    NFC->ISR |= ( NFC_ISR_WERRCLR_MASK
                | NFC_ISR_DONECLR_MASK
                | NFC_ISR_IDLECLR_MASK );

    /* Make sure ECC is enabled before reading */
    NFC->CFG &= ~NFC_CFG_ECCMODE(0x7);

    /* Set the chip select to use */
    if(NFC_CE == NFC_CE0)
    {
        NFC->RAR = (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK);
     }
    else /* (NFC_CE == NFC_CE1) */
    {
        NFC->RAR = (NFC_RAR_CS1_MASK | NFC_RAR_RB1_MASK);
    }

    /* Set the row address */
    NFC->RAR |= RowAddr;

    /* Set the column address */
    NFC->CAR = ColAddr;

    /* Write the NFC_CMD2 register with the command byte and code for an erase */
    NFC->CMD2 = (NFC_CMD2_BYTE1(PAGE_READ_CMD_BYTE1)
              | NFC_CMD2_CODE(NFC_READ_PAGE_CMD_CODE)
              | NFC_CMD2_BUFNO(FlashBufNo) );

    /* Write the NFC_CMD1 register with the command byte2 and byte3 for an erase */
    NFC->CMD1 = NFC_CMD1_BYTE2(PAGE_READ_CMD_BYTE2)
                | NFC_CMD1_BYTE3(READ_STATUS_CMD_BYTE);

    /* Set Start Bit to send command to the NAND flash */
    NFC->CMD2 |= NFC_CMD2_BUSY_START_MASK;

    /* Wait for start/busy bit to clear indicating command is done */
    while (NFC->CMD2 & NFC_CMD2_BUSY_START_MASK);
}

// =============================================================================
// 功能：将数据从NADN控制器的缓冲区中读出
// 参数：BufferNo,使用的NFC发送数据的BUFFER号，从0-3
//       pDstBuf,目标缓冲区地址
//       offset,所在BUFFER的偏移量
//       len,读取的字节数
// 返回：len,读到的字节数
// =============================================================================
u32 NFC_ReadBuffer(u8* pDstBuf,u32 offset,u32 len)
{
	u32 result = 0;
	if(len > 0)
	{
		memcpy(pDstBuf,(u8 *)(NFC_BASE_PTR+offset),len);
		result = len;
	}

	return result;
}

// =============================================================================
// 功能：将数据存放到指定NAND控制器的BUFFER缓冲区
// 参数：BufferNo,使用的NFC发送数据的BUFFER号，从0-3
//       pDstBuf,源数据地址
//       offset,所在BUFFER的偏移量
//       len,字节数
// 返回：len,字节数
// =============================================================================
u32 NFC_WriteBuffer(u8* pSrcBuf,u32 offset,u32 len)
{
	u32 result = 0;
	if(len > 0)
	{
		memcpy((u8 *)(NFC_BASE_PTR+offset),pSrcBuf,len);
		result = len;
	}

	return result;
}












