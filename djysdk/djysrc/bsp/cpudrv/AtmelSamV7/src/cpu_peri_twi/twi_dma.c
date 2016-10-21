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
// 文件名     ：twi_dma.c
// 模块描述: twi模块，XDMAD与twi驱动中间层
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/11.2015
// =============================================================================

#include "cpu_peri.h"
#include "xdmad.h"
#include "xdmac.h"
#include "twi_dma.h"
#include "xdma_hardware_interface.h"

// =============================================================================
// 功能：配置的DMA回调函数，若TwiChannel中未设置回调函数，则默认调用此函数
// 参数：channel,DMA号，从0-23，共24个通道
//       pArg，TwiDma的DMA控制指针
// 返回：无
// =============================================================================
static void TWID_Rx_Cb(uint32_t channel, TwiDma* pArg)
{

	TwiChannel *pTwidCh = pArg->pRxChannel;
	if (channel != pTwidCh->ChNum)
		return;

	/* Release the DMA channels */
	XDMAD_FreeChannel(pArg->pXdmad, pTwidCh->ChNum);
	pTwidCh->sempaphore = 1;
	__DMB();
}

// =============================================================================
// 功能：配置的DMA回调函数，若TwiChannel中未设置回调函数，则默认调用此函数
// 参数：channel,DMA号，从0-23，共24个通道
//       pArg，TwiDma的DMA控制指针
// 返回：无
// =============================================================================
static void TWID_Tx_Cb(uint32_t channel, TwiDma* pArg)
{
	TwiChannel *pTwidCh = pArg->pTxChannel;
	if (channel != pTwidCh->ChNum)
		return;

	/* Release the DMA channels */
	XDMAD_FreeChannel(pArg->pXdmad, pTwidCh->ChNum);
	pTwidCh->sempaphore = 1;
	__DMB();
}
// =============================================================================
// 功能：配置DMA的参数，例如块大小，通道，源地址，目标地址，中断等
// 参数：pTwid,TwiDma控制块指针
//       pTxCh，发送通道结构体指针
// 返回：0，正常，其他,错误
// =============================================================================
static u8 __Twi_ConfigureRxDma(TwiDma *pTwiXdma, TwiChannel *pRxCh)
{
	uint32_t xdmaCndc, Rhr, xdmaInt;
	sXdmadCfg xdmadRxCfg;

	Rhr = (uint32_t)&(pTwiXdma->pTwihsHw->TWIHS_RHR);
	xdmadRxCfg.mbr_ubc = XDMA_UBC_NVIEW_NDV0 |
						  XDMA_UBC_NDE_FETCH_DIS|
						  XDMA_UBC_NDEN_UPDATED |
						  (uint32_t)pRxCh->BuffSize;

	xdmadRxCfg.mbr_da = (uint32_t)pRxCh->pBuff;
	xdmadRxCfg.mbr_sa = Rhr;

	xdmadRxCfg.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
						   XDMAC_CC_MBSIZE_SINGLE |
						   XDMAC_CC_DSYNC_PER2MEM |
						   XDMAC_CC_CSIZE_CHK_1 |
						   XDMAC_CC_DWIDTH_BYTE |
						   XDMAC_CC_SIF_AHB_IF1 |
						   XDMAC_CC_DIF_AHB_IF0 |
						   XDMAC_CC_SAM_FIXED_AM |
						   XDMAC_CC_DAM_INCREMENTED_AM |
						   XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(
								pTwiXdma->TwiId , XDMAD_TRANSFER_RX ));

	xdmadRxCfg.mbr_bc = 0;
	xdmadRxCfg.mbr_sus = 0;
	xdmadRxCfg.mbr_dus =0;
	xdmaCndc = 0;
	xdmaInt =  (XDMAC_CIE_BIE |
			   XDMAC_CIE_RBIE  |
			   XDMAC_CIE_WBIE );

	if (XDMAD_ConfigureTransfer( pTwiXdma->pXdmad, pRxCh->ChNum,
			&xdmadRxCfg, xdmaCndc, 0, xdmaInt))
	  return 1;
	return 0;
}

// =============================================================================
// 功能：配置DMA的参数，例如块大小，通道，源地址，目标地址，中断等
// 参数：pTwid,TwiDma控制块指针
//       pTxCh，发送通道结构体指针
// 返回：0，正常，其他,错误
// =============================================================================
static u8 __Twi_ConfigureTxDma(TwiDma *pTwiXdma, TwiChannel *pTxCh)
{
	uint32_t xdmaCndc, Thr, xdmaInt;
	sXdmadCfg xdmadTxCfg;

	Thr = (uint32_t)&(pTwiXdma->pTwihsHw->TWIHS_THR);
	xdmadTxCfg.mbr_ubc =      XDMA_UBC_NVIEW_NDV0 |
								 XDMA_UBC_NDE_FETCH_DIS|
								 XDMA_UBC_NSEN_UPDATED |
								 (uint32_t)pTxCh->BuffSize;

	xdmadTxCfg.mbr_sa = (uint32_t)pTxCh->pBuff;
	xdmadTxCfg.mbr_da = Thr;
	xdmadTxCfg.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
						 XDMAC_CC_MBSIZE_SINGLE |
						 XDMAC_CC_DSYNC_MEM2PER |
						 XDMAC_CC_CSIZE_CHK_1 |
						 XDMAC_CC_DWIDTH_BYTE|
						 XDMAC_CC_SIF_AHB_IF0 |
						 XDMAC_CC_DIF_AHB_IF1 |
						 XDMAC_CC_SAM_INCREMENTED_AM |
						 XDMAC_CC_DAM_FIXED_AM |
						 XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(
							pTwiXdma->TwiId, XDMAD_TRANSFER_TX ));

	xdmadTxCfg.mbr_bc = 0;
	xdmadTxCfg.mbr_sus = 0;
	xdmadTxCfg.mbr_dus =0;
	xdmaCndc = 0;

	xdmaInt =  (XDMAC_CIE_BIE |
			   XDMAC_CIE_RBIE  |
			   XDMAC_CIE_WBIE );
	if (XDMAD_ConfigureTransfer( pTwiXdma->pXdmad, pTxCh->ChNum,
			&xdmadTxCfg, xdmaCndc, 0, xdmaInt) )
	  return TWIDMA_ERROR;

	return 0;
}

// =============================================================================
// 功能：使能TWID的DMA接收通道，并释放获取的DMA通道
// 参数：pTwid,TwiDma控制块指针
//       pRxCh，发送通道结构体指针
// 返回：0，正常，其他,错误
// =============================================================================
uint32_t TWID_EnableRxChannels(TwiDma *pTwid, TwiChannel *pRxCh)
{
	uint32_t Channel;
	pTwid->pRxChannel = pRxCh;

	/* Allocate a DMA channel for TWI RX. */
	Channel =  XDMAD_AllocateChannel( pTwid->pXdmad, pTwid->TwiId,
			XDMAD_TRANSFER_MEMORY);
	if ( Channel == XDMAD_ALLOC_FAILED )
	{
		return TWIDMA_ERROR;
	}
	pRxCh->ChNum = Channel ;

	 /* Setup callbacks for TWI RX */
	if(pRxCh->callback)
	{
	  XDMAD_SetCallback(pTwid->pXdmad, pRxCh->ChNum,
			(XdmadTransferCallback)pRxCh->callback, pRxCh->pArgument);
	}
	else
	{
		XDMAD_SetCallback(pTwid->pXdmad, pRxCh->ChNum,
				(XdmadTransferCallback)TWID_Rx_Cb, pTwid);
	}

	if (XDMAD_PrepareChannel( pTwid->pXdmad, pRxCh->ChNum ))
		return TWIDMA_ERROR;
	if (__Twi_ConfigureRxDma(pTwid, pRxCh))
		return TWIDMA_ERROR_LOCK;

	return 0;
}

// =============================================================================
// 功能：使能TWID的DMA发送通道，并释放获取的DMA通道
// 参数：pTwid,TwiDma控制块指针
//       pTxCh，发送通道结构体指针
// 返回：0，正常，其他,错误
// =============================================================================
uint32_t TWID_EnableTxChannels( TwiDma *pTwid, TwiChannel *pTxCh)
{

	uint32_t Channel;

	assert(pTxCh);

	/* Init USART Tx Channel. */
	pTwid->pTxChannel = pTxCh;

	 /* Allocate a DMA channel for USART0/1 TX. */
	Channel =  XDMAD_AllocateChannel( pTwid->pXdmad, XDMAD_TRANSFER_MEMORY,
			pTwid->TwiId);
	if ( Channel == XDMAD_ALLOC_FAILED ) {
	  return TWIDMA_ERROR;
	}
	pTxCh->ChNum = Channel;
	/* Setup callbacks for USART0/1 TX */
	if(pTwid->pTxChannel->callback) {
	  XDMAD_SetCallback(pTwid->pXdmad, pTxCh->ChNum,
			(XdmadTransferCallback)pTxCh->callback, pTxCh->pArgument);
	} else {
	  XDMAD_SetCallback(pTwid->pXdmad, pTxCh->ChNum,
			  (XdmadTransferCallback)TWID_Tx_Cb, pTwid);
	}

	if ( XDMAD_PrepareChannel( pTwid->pXdmad, pTxCh->ChNum ))
		return TWIDMA_ERROR;

	if (__Twi_ConfigureTxDma(pTwid , pTwid->pTxChannel))
		return TWIDMA_ERROR_LOCK;

	return 0;
}

// =============================================================================
// 功能：禁止TWID的DMA接收通道，并释放获取的DMA通道
// 参数：pTwid,TwiDma控制块指针
//       pTxCh，发送通道结构体指针
// 返回：0
// =============================================================================
uint32_t TWID_DisableRxChannels( TwiDma *pTwid, TwiChannel *pRxCh)
{
	assert(pRxCh);

	XDMAD_StopTransfer(pTwid->pXdmad, pRxCh->ChNum);

	XDMAD_SetCallback(pTwid->pXdmad, pRxCh->ChNum, NULL, NULL);
	 /* Free allocated DMA channel for USART TX. */
	if(XDMAD_FreeChannel( pTwid->pXdmad, pRxCh->ChNum) != XDMAD_OK) {
		return TWIDMA_ERROR;
	}

	memory_barrier();
	return 0;
}

// =============================================================================
// 功能：禁止TWID的DMA发送通道，并释放获取的DMA通道
// 参数：pTwid,TwiDma控制块指针
//       pTxCh，发送通道结构体指针
// 返回：0
// =============================================================================
uint32_t TWID_DisableTxChannels( TwiDma *pTwid, TwiChannel *pTxCh)
{
	assert(pTxCh);

	XDMAD_StopTransfer(pTwid->pXdmad, pTxCh->ChNum);

	XDMAD_SetCallback(pTwid->pXdmad, pTxCh->ChNum, NULL, NULL);
	/* Free allocated DMA channel for USART TX. */
	if(XDMAD_FreeChannel( pTwid->pXdmad, pTxCh->ChNum) != XDMAD_OK) {
		return TWIDMA_ERROR;
	}

	memory_barrier();
	return 0;
}

// =============================================================================
// 功能：暂时停止DMA接收，中断及其他配置仍然保持不变
// 参数：pTwid，twi dma指针
// 返回: 0
// =============================================================================
uint32_t TWID_RcvPause( TwiDma *pTwid)
{
	u32 addr,size;

	addr = (u32)(pTwid->pRxChannel->pBuff);
	size = pTwid->pRxChannel->BuffSize;
	SCB_CleanInvalidateDCache_by_Addr((u32*)addr,size);
//	SCB_CleanInvalidateDCache();
	pTwid->pRxChannel->sempaphore=1;
	__DMB();
	/* Start DMA 0(RX) && 1(TX) */
	if (XDMAD_StopTransfer( pTwid->pXdmad, pTwid->pRxChannel->ChNum ))
		return TWIDMA_ERROR_LOCK;

	return 0;
}

// =============================================================================
// 功能：暂时停止DMA接收，中断及其他配置仍然保持不变
// 参数：pTwid，twi dma指针
// 返回: 0
// =============================================================================
uint32_t TWID_SendPause( TwiDma *pTwid)
{
	u32 addr,size;

	addr = (u32)(pTwid->pTxChannel->pBuff);
	size = pTwid->pTxChannel->BuffSize;
	SCB_CleanInvalidateDCache_by_Addr((u32 *)addr,size);
//	SCB_CleanInvalidateDCache();
	pTwid->pTxChannel->sempaphore=1;
	__DMB();
	/* Start DMA 0(RX) && 1(TX) */
	if (XDMAD_StopTransfer( pTwid->pXdmad, pTwid->pTxChannel->ChNum ))
		return TWIDMA_ERROR_LOCK;

	return 0;
}

// =============================================================================
// 功能：TWI的发送数据DMA使能，即开始使用DMA发送TWI数据，需清cache
// 参数：pTwid,TwiDma控制块指针
// 返回：0
// =============================================================================
uint32_t TWID_SendData( TwiDma *pTwid)
{
	u32 addr,size;

	addr = (u32)(pTwid->pTxChannel->pBuff);
	size = pTwid->pTxChannel->BuffSize;
	SCB_CleanInvalidateDCache_by_Addr((u32*)addr,size);
//	SCB_CleanInvalidateDCache();
	pTwid->pTxChannel->sempaphore=0;
	__DMB();
	if (XDMAD_StartTransfer( pTwid->pXdmad, pTwid->pTxChannel->ChNum ))
		return TWIDMA_ERROR_LOCK;

	return 0;
}

// =============================================================================
// 功能：TWI的接收数据DMA使能，即开始使用DMA接收TWI数据，需清cache
// 参数：pTwid,TwiDma控制块指针
// 返回：0
// =============================================================================
uint32_t TWID_RcvData( TwiDma *pTwid)
{
	u32 addr,size;

	addr = (u32)(pTwid->pRxChannel->pBuff);
	size = pTwid->pRxChannel->BuffSize;
	SCB_CleanInvalidateDCache_by_Addr((u32*)addr,size);
//	SCB_CleanInvalidateDCache();
	pTwid->pRxChannel->sempaphore=0;
	__DMB();
	if (XDMAD_StartTransfer( pTwid->pXdmad, pTwid->pRxChannel->ChNum ))
		return TWIDMA_ERROR_LOCK;

	return 0;
}

// =============================================================================
// 功能：配置TWI的DMA功能，主要是初始化XDMAC的控制器
// 参数：pTwid,TwiDma控制块指针
//       TwiId,TWI外设ID号
// 返回：0
// =============================================================================
uint32_t TWID_Configure( TwiDma *pTwid ,uint8_t TwiId)
{
	/* Initialize the UART structure */
	pTwid->TwiId  = TwiId;

	if (TwiId == ID_TWIHS0)
		pTwid->pTwihsHw = TWIHS0;
	if (TwiId == ID_TWIHS1)
		pTwid->pTwihsHw = TWIHS1;
	if (TwiId == ID_TWIHS2)
		pTwid->pTwihsHw = TWIHS2;

	pTwid->pXdmad->pXdmacs = XDMAC;

	/* Driver initialize */
	XDMAD_Initialize(  pTwid->pXdmad, 0 );
	return 0;
}

