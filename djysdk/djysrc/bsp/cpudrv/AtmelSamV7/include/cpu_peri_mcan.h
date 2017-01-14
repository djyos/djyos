//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_mcan.h
// 模块版本: V1.00
// 创建人员: zhb
// 创建时间: 05/12.2016
// =============================================================================

#ifndef __CPU_PERI_MCAN_H__
#define __CPU_PERI_MCAN_H__

/*------------------------------------------------------------------------------
 *         Headers
 *------------------------------------------------------------------------------*/
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif
/*------------------------------------------------------------------------------
 *         Global functions
 *------------------------------------------------------------------------------*/
typedef struct MailboxInfoTag
{
	uint32_t   id;
	uint32_t   length;
	uint32_t   timestamp;
} MailboxInfoType;


typedef struct MailBox8Tag
{
	MailboxInfoType info;
	uint8_t         data[8];
} Mailbox8Type;

typedef struct MailBox12Tag
{
	MailboxInfoType info;
	uint8_t         data[12];
} Mailbox12Type;

typedef struct MailBox16Tag
{
	MailboxInfoType info;
	uint8_t         data[16];
} Mailbox16Type;

typedef struct MailBox20Tag
{
	MailboxInfoType info;
	uint8_t         data[20];
} Mailbox20Type;

typedef struct MailBox24Tag
{
	MailboxInfoType info;
	uint8_t         data[24];
} Mailbox24Type;

typedef struct MailBox32Tag
{
	MailboxInfoType info;
	uint8_t         data[32];
} Mailbox32ype;

typedef struct MailBox48Tag
{
	MailboxInfoType info;
	uint8_t         data[48];
} Mailbox48Type;

typedef struct MailBox64Tag
{
	MailboxInfoType info;
	uint8_t         data[64];
} Mailbox64Type;



typedef struct MCan_MsgRamPntrsTag
{
	uint32_t * pStdFilts;
	uint32_t * pExtFilts;
	uint32_t * pRxFifo0;
	uint32_t * pRxFifo1;
	uint32_t * pRxDedBuf;
	uint32_t * pTxEvtFifo;
	uint32_t * pTxDedBuf;
	uint32_t * pTxFifoQ;
} MCan_MsgRamPntrs;

typedef struct MCan_ConfigTag
{
	Mcan            * pMCan;
	uint32_t          bitTiming;
	uint32_t          fastBitTiming;
	uint32_t          nmbrStdFilts;
	uint32_t          nmbrExtFilts;
	uint32_t          nmbrFifo0Elmts;
	uint32_t          nmbrFifo1Elmts;
	uint32_t          nmbrRxDedBufElmts;
	uint32_t          nmbrTxEvtFifoElmts;
	uint32_t          nmbrTxDedBufElmts;
	uint32_t          nmbrTxFifoQElmts;
	uint32_t          rxFifo0ElmtSize;
	uint32_t          rxFifo1ElmtSize;
	uint32_t          rxBufElmtSize;
	// Element sizes and data sizes (encoded element size)
	uint32_t          txBufElmtSize;
	// Element size and data size (encoded element size)
	MCan_MsgRamPntrs  msgRam;
} MCan_ConfigType;

extern const MCan_ConfigType mcan0Config;
extern const MCan_ConfigType mcan1Config;

//__STATIC_INLINE uint32_t MCAN_IsTxComplete(
//	const MCan_ConfigType * mcanConfig )
//{
//	Mcan * mcan = mcanConfig->pMCan;
//	return ( mcan->MCAN_IR & MCAN_IR_TC );
//}

//__STATIC_INLINE void MCAN_ClearTxComplete(
//	const MCan_ConfigType * mcanConfig )
//{
//	Mcan * mcan = mcanConfig->pMCan;
//	mcan->MCAN_IR = MCAN_IR_TC;
//}

__STATIC_INLINE uint32_t MCAN_IsMessageStoredToRxDedBuffer(
	const MCan_ConfigType * mcanConfig )
{
  Mcan * mcan = mcanConfig->pMCan;

  return ( mcan->MCAN_IR & MCAN_IR_DRX );
}

__STATIC_INLINE void MCAN_ClearMessageStoredToRxBuffer(
	const MCan_ConfigType * mcanConfig )
{
	Mcan * mcan = mcanConfig->pMCan;
	mcan->MCAN_IR = MCAN_IR_DRX;
}

__STATIC_INLINE uint32_t MCAN_IsMessageStoredToRxFifo0(
	const MCan_ConfigType * mcanConfig )
{
	Mcan * mcan = mcanConfig->pMCan;
	return ( mcan->MCAN_IR & MCAN_IR_RF0N );
}

__STATIC_INLINE void MCAN_ClearMessageStoredToRxFifo0(
	const MCan_ConfigType * mcanConfig )
{
	Mcan * mcan = mcanConfig->pMCan;
	mcan->MCAN_IR = MCAN_IR_RF0N;
}

__STATIC_INLINE uint32_t MCAN_IsMessageStoredToRxFifo1(
	const MCan_ConfigType * mcanConfig )
{
	Mcan * mcan = mcanConfig->pMCan;
	return ( mcan->MCAN_IR & MCAN_IR_RF1N );
}

__STATIC_INLINE void MCAN_ClearMessageStoredToRxFifo1(
	const MCan_ConfigType * mcanConfig )
{
	Mcan * mcan = mcanConfig->pMCan;
	mcan->MCAN_IR = MCAN_IR_RF1N;
}



/** MCAN1 pin Transmit Data (TXD) */
#define PIN_MCAN1_TXD {PIO_PD12B_CANTX1, PIOD, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT}
/** MCAN1 pin Receive Data (RXD) */
#define PIN_MCAN1_RXD {PIO_PC12C_CANRX1, PIOC, ID_PIOC, PIO_PERIPH_C, PIO_DEFAULT}

#define MCAN_PROG_CLK_PRESCALER       30  /* /1 to /256 */
#define MCAN_PROG_CLK_SELECT          PMC_PCK_CSS_MCK
#define MCAN_PROG_CLK_FREQ_HZ \
		( (float) 150000000 / (float) MCAN_PROG_CLK_PRESCALER )

#define MCAN1_BIT_RATE_BPS            125000
#define MCAN1_PROP_SEG                2
#define MCAN1_PHASE_SEG1              22
#define MCAN1_PHASE_SEG2              13
#define MCAN1_SYNC_JUMP               4

#define MCAN1_FAST_BIT_RATE_BPS       2000000
#define MCAN1_FAST_PROP_SEG           2
#define MCAN1_FAST_PHASE_SEG1         4
#define MCAN1_FAST_PHASE_SEG2         4
#define MCAN1_FAST_SYNC_JUMP          2

#define MCAN1_NMBR_STD_FLTS           0   /* 128 max filters */
#define MCAN1_NMBR_EXT_FLTS           0   /* 64 max filters */
#define MCAN1_NMBR_RX_FIFO0_ELMTS     12  /* # of elements, 64 elements max */
#define MCAN1_NMBR_RX_FIFO1_ELMTS     0   /* # of elements, 64 elements max */
#define MCAN1_NMBR_RX_DED_BUF_ELMTS   8   /* # of elements, 64 elements max */
#define MCAN1_NMBR_TX_EVT_FIFO_ELMTS  0   /* # of elements, 32 elements max */
#define MCAN1_NMBR_TX_DED_BUF_ELMTS   8   /* # of elements, 32 elements max */
#define MCAN1_NMBR_TX_FIFO_Q_ELMTS    0   /* # of elements, 32 elements max */
#define MCAN1_RX_FIFO0_ELMT_SZ        8   /* 8, 12, 16, 20, 24, 32, 48, 64 bytes */
#define MCAN1_RX_FIFO1_ELMT_SZ        8   /* 8, 12, 16, 20, 24, 32, 48, 64 bytes */
#define MCAN1_RX_BUF_ELMT_SZ          64  /* 8, 12, 16, 20, 24, 32, 48, 64 bytes */
#define MCAN1_TX_BUF_ELMT_SZ          32  /* 8, 12, 16, 20, 24, 32, 48, 64 bytes */




uint32_t CAN_WriteData(uint8_t byChip, uint8_t* txBuf, uint32_t len);
uint32_t CAN_ReadData(uint8_t byChip, uint8_t* rxBuf, uint32_t len, uint32_t *pRd);
bool_t CAN_Main(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MCAN_ */

