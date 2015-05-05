// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_nfc.h
// 模块描述: NFC模块的头文件，定义了NFC相关的操作宏
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 02/08.2014
// =============================================================================

#ifndef CPU_PERI_NFC_H_
#define CPU_PERI_NFC_H_

/* Defines for NAND command values */
#define NFC_READ_PAGE_CMD_CODE        	    0x7EF8
#define NFC_PROGRAM_PAGE_CMD_CODE           0x7FD8
#define NFC_DMA_PROGRAM_PAGE_CMD_CODE       0xFFC0
#define NFC_BLOCK_ERASE_CMD_CODE      	    0x4ED8
#define NFC_READ_ID_CMD_CODE          	    0x4804
#define NFC_RESET_CMD_CODE            	    0x4040
#define NFC_READ_STATUS_CODE                0x4880

#define PAGE_READ_CMD_BYTE1       	    	0x00
#define PAGE_READ_CMD_BYTE2       	    	0x30
#define PROGRAM_PAGE_CMD_BYTE1    	    	0x80
#define PROGRAM_PAGE_CMD_BYTE2    	    	0x10
#define READ_STATUS_CMD_BYTE      	    	0x70
#define BLOCK_ERASE_CMD_BYTE1     	    	0x60
#define BLOCK_ERASE_CMD_BYTE2     	    	0xD0

#define READ_ID_CMD_BYTE                    0x90
#define READ_STATUS_CMD_BYTE                0x70
#define RESET_CMD_BYTE                      0xFF

/* Defines for NFC chip selects and port size */
#define NFC_CE0                       		0
#define NFC_CE1                       		1
#define NFC_8BIT		      				0
#define NFC_16BIT		      				1

/* Defines for page sizes - used for page size and ECC page size */
#define PAGE_4K              				0x1000
#define PAGE_2K              				0x0800
#define PAGE_1K             				0x0400
#define PAGE_512							0x0200
#define PAGE_128             				0x0080
#define PAGE_64              				0x0040
#define PAGE_32              				0x0020
#define PAGE_16 							0x0010

// NFC接口API函数
void NFC_ModuleInit(void);
void NFC_Reset(void);
bool_t NFC_OptionCompleted(void);
void NFC_CsActive(u8 CsNo);
void NFC_CsInactive(u8 CsNo);
void NFC_FlashIDRead(u8 FlashBufNo, u32* FlashID0, u32* FlashID1);
void NFC_BlockErase(u32 BlockNo);
void NFC_PageProgram(u8 FlashBufNo, u32 RowAddr, u16 ColAddr);
void NFC_PageRead(u8 FlashBufNo, u32 RowAddr, u16 ColAddr);
u32 NFC_ReadBuffer(u8* pDstBuf,u32 offset,u32 len);
u32 NFC_WriteBuffer(u8* pSrcBuf,u32 offset,u32 len);
u8 NFC_FlashStatusRead(void);

#endif /* CPU_PERI_NFC_H_ */
