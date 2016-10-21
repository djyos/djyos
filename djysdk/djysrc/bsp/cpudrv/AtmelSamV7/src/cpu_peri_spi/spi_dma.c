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
// 文件名     ：spi_dma.c
// 模块描述: SPI模块，XDMAD与spi驱动中间层
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/11.2015
// =============================================================================
#include "cpu_peri.h"
#include "xdmac.h"
#include "xdmad.h"
#include "spi_dma.h"


static void SPID_CacheInvalid(SpiDma *pSpid)
{
	u32 addr,size;

	addr = (u32)(pSpid->pTxChannel->pBuff);
	size = pSpid->pTxChannel->BuffSize;
	SCB_CleanInvalidateDCache_by_Addr((u32*)addr,size);

	addr = (u32)(pSpid->pRxChannel->pBuff);
	size = pSpid->pRxChannel->BuffSize;
	SCB_CleanInvalidateDCache_by_Addr((u32*)addr,size);
}
// =============================================================================
// 功能：配置的DMA回调函数，若TwiChannel中未设置回调函数，则默认调用此函数
// 参数：channel,DMA号，从0-23，共24个通道
//       pArg，TwiDma的DMA控制指针
// 返回：无
// =============================================================================
static void SPID_RxTx_Cb(uint32_t channel, SpiDma* pArg)
{
	if ( (channel != pArg->pRxChannel->ChNum)
			&& (channel != pArg->pTxChannel->ChNum) )
		return;

	/* Release the DMA channels */
	XDMAD_FreeChannel(pArg->pXdmad, pArg->pRxChannel->ChNum);
	XDMAD_FreeChannel(pArg->pXdmad, pArg->pTxChannel->ChNum);

	SPID_CacheInvalid(pArg);
//	SCB_CleanInvalidateDCache();
	/* Release the dataflash semaphore */
	pArg->sempaphore = 1;
}

// =============================================================================
// 功能：配置的DMA回调函数，若TwiChannel中未设置回调函数，则默认调用此函数
// 参数：channel,DMA号，从0-23，共24个通道
//       pArg，TwiDma的DMA控制指针
// 返回：无
// =============================================================================
static uint8_t __Spid_LinkListConfig(SpiDma *pSpid)
{
	sXdmadCfg xdmadRxCfg,xdmadTxCfg;
	uint32_t xdmaCndc, xdmaInt;
	uint32_t spiId;
	if ((unsigned int)pSpid->pSpiReg == (unsigned int)SPI0 ) spiId = ID_SPI0;
	if ((unsigned int)pSpid->pSpiReg == (unsigned int)SPI1 ) spiId = ID_SPI1;

	/* Setup TX  */

	xdmadTxCfg.mbr_sa = (uint32_t)pSpid->pTxChannel->pBuff;

	xdmadTxCfg.mbr_da = (uint32_t)&pSpid->pSpiReg->SPI_TDR;

	xdmadTxCfg.mbr_ubc =  XDMA_UBC_NVIEW_NDV0 |
		XDMA_UBC_NDE_FETCH_DIS|
		XDMA_UBC_NSEN_UPDATED | pSpid->pTxChannel->BuffSize;

	xdmadTxCfg.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
		XDMAC_CC_MBSIZE_SINGLE |
		XDMAC_CC_DSYNC_MEM2PER |
		XDMAC_CC_CSIZE_CHK_1 |
		XDMAC_CC_DWIDTH_BYTE|
		XDMAC_CC_SIF_AHB_IF0 |
		XDMAC_CC_DIF_AHB_IF1 |
		XDMAC_CC_SAM_INCREMENTED_AM |
		XDMAC_CC_DAM_FIXED_AM |
		XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(  spiId, XDMAD_TRANSFER_TX ));


	xdmadTxCfg.mbr_bc = 0;
	xdmadTxCfg.mbr_sus = 0;
	xdmadTxCfg.mbr_dus =0;

	/* Setup RX Link List */

	xdmadRxCfg.mbr_ubc = XDMA_UBC_NVIEW_NDV0 |
		XDMA_UBC_NDE_FETCH_DIS|
		XDMA_UBC_NDEN_UPDATED | pSpid->pRxChannel->BuffSize;

	xdmadRxCfg.mbr_da = (uint32_t)pSpid->pRxChannel->pBuff;

	xdmadRxCfg.mbr_sa = (uint32_t)&pSpid->pSpiReg->SPI_RDR;
	xdmadRxCfg.mbr_cfg = XDMAC_CC_TYPE_PER_TRAN |
		XDMAC_CC_MBSIZE_SINGLE |
		XDMAC_CC_DSYNC_PER2MEM |
		XDMAC_CC_CSIZE_CHK_1 |
		XDMAC_CC_DWIDTH_BYTE|
		XDMAC_CC_SIF_AHB_IF1 |
		XDMAC_CC_DIF_AHB_IF0 |
		XDMAC_CC_SAM_FIXED_AM |
		XDMAC_CC_DAM_INCREMENTED_AM |
		XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(  spiId, XDMAD_TRANSFER_RX ));


	xdmadRxCfg.mbr_bc = 0;
	xdmadRxCfg.mbr_sus = 0;
	xdmadRxCfg.mbr_dus =0;

	xdmaCndc = 0;

	/* Put all interrupts on for non LLI list setup of DMA */
	  xdmaInt =  (XDMAC_CIE_BIE   |
				   XDMAC_CIE_DIE   |
				   XDMAC_CIE_FIE   |
				   XDMAC_CIE_RBIE  |
				   XDMAC_CIE_WBIE  |
				   XDMAC_CIE_ROIE);

	if (XDMAD_ConfigureTransfer( pSpid->pXdmad, pSpid->pRxChannel->ChNum,
			&xdmadRxCfg, xdmaCndc, 0, xdmaInt))
		return SPID_ERROR;

	if (XDMAD_ConfigureTransfer( pSpid->pXdmad, pSpid->pTxChannel->ChNum,
			&xdmadTxCfg, xdmaCndc, 0, xdmaInt))
		return SPID_ERROR;
	return 0;
}

// =============================================================================
// 功能：获取DMA通道号，并配置回调函数
// 参数：pSpid，TwiDma的DMA控制指针
// 返回：0，正确，其他，错误
// =============================================================================
static uint8_t __Spid_ChannelConfig(SpiDma *pSpid)
{
	uint32_t spiDmaTxChannel,spiDmaRxChannel;
	/* Driver initialize */
	XDMAD_Initialize(  pSpid->pXdmad, 0 );

//	XDMAD_FreeChannel( pSpid->pXdmad, spiDmaTxChannel);
//	XDMAD_FreeChannel( pSpid->pXdmad, spiDmaRxChannel);

	/* Allocate a DMA channel for SPI0/1 TX. */
	spiDmaTxChannel = XDMAD_AllocateChannel( pSpid->pXdmad,
			XDMAD_TRANSFER_MEMORY, pSpid->SpiId);
	if ( spiDmaTxChannel == XDMAD_ALLOC_FAILED ) {
		return SPID_ERROR;
	}
	/* Allocate a DMA channel for SPI0/1 RX. */
	spiDmaRxChannel =
		XDMAD_AllocateChannel( pSpid->pXdmad, pSpid->SpiId, XDMAD_TRANSFER_MEMORY);
	if ( spiDmaRxChannel == XDMAD_ALLOC_FAILED ) {
		return SPID_ERROR;
	}

	/* Setup callbacks for SPI0/1 RX */
	if(pSpid->pRxChannel->callback == NULL)
	{
		XDMAD_SetCallback(pSpid->pXdmad, spiDmaRxChannel,
			(XdmadTransferCallback)SPID_RxTx_Cb, pSpid);
	}
	else
	{
		XDMAD_SetCallback(pSpid->pXdmad, spiDmaRxChannel,
			(XdmadTransferCallback)pSpid->pRxChannel->callback, pSpid);
	}

	if (XDMAD_PrepareChannel( pSpid->pXdmad, spiDmaRxChannel ))
		return SPID_ERROR;

	/* Setup callbacks for SPI0/1 TX (ignored) */
	if(pSpid->pTxChannel->callback == NULL)
	{
		XDMAD_SetCallback(pSpid->pXdmad, spiDmaTxChannel,
				(XdmadTransferCallback)SPID_RxTx_Cb, pSpid);
	}
	else
	{
		XDMAD_SetCallback(pSpid->pXdmad, spiDmaTxChannel,
			(XdmadTransferCallback)pSpid->pTxChannel->callback, pSpid);
	}
	if ( XDMAD_PrepareChannel( pSpid->pXdmad, spiDmaTxChannel ))
		return SPID_ERROR;

	pSpid->pRxChannel->ChNum = spiDmaRxChannel;
	pSpid->pTxChannel->ChNum = spiDmaTxChannel;
	return 0;
}

// =============================================================================
// 功能：配置SPI的DMA中断使能，先关闭所有中断，并配置相关中断使能
// 参数：pXdmac，iChannel，IntEn，XDMAC控制器寄存器，DMA通道和中断msk位
// 返回：无
// =============================================================================
void SPID_DmaIntEnable(Xdmac *pXdmac,u8 iChannel,u32 IntEn)
{
	XDMAC_DisableChannelIt (pXdmac, iChannel, 0xFF);
	XDMAC_EnableChannelIt (pXdmac,iChannel, IntEn );
}

// =============================================================================
// 功能：配置SPI的DMA中断失能，关闭所有中断
// 参数：pXdmac，iChannel，IntEn，XDMAC控制器寄存器，DMA通道号
// 返回：无
// =============================================================================
void SPID_DmaIntDisable(Xdmac *pXdmac,u8 iChannel)
{
	XDMAC_DisableChannelIt (pXdmac, iChannel, 0xFF);
}

// =============================================================================
// 功能：配置SPI的DMA功能，主要是初始化XDMAC的控制器和接收，发送通道号并配置相关参数
// 参数：pSpid,SpiDma控制块指针
// 返回：0
// =============================================================================
uint32_t SPID_Configure(SpiDma *pSpid)
{

	if(__Spid_ChannelConfig(pSpid))
		return SPID_ERROR_LOCK;

	if(__Spid_LinkListConfig(pSpid))
		return SPID_ERROR_LOCK;

	return 0;
}

void SPID_FastConfig(Xdmac *pXdmac,u8 iChannel,u32 sa,u32 da,u32 len)
{
	XDMAC_SetSourceAddr(pXdmac, iChannel, sa);
	XDMAC_SetDestinationAddr(pXdmac, iChannel, da);
	XDMAC_SetMicroblockControl(pXdmac, iChannel, len);
}

// =============================================================================
// 功能：SPI的接收数据DMA使能，即开始使用DMA接收发送SPI数据，需清cache
// 参数：pSpid,SpiDma控制块指针
// 返回：0
// =============================================================================
uint32_t SPID_RxTxStart(SpiDma *pSpid)
{
	SPID_CacheInvalid(pSpid);
//	SCB_CleanInvalidateDCache();
	pSpid->sempaphore = 0;
	__DMB();

	/* Start DMA 0(RX) && 1(TX) */
	if (XDMAD_StartTransfer( pSpid->pXdmad, pSpid->pRxChannel->ChNum ))
		return SPID_ERROR_LOCK;

	if (XDMAD_StartTransfer( pSpid->pXdmad, pSpid->pTxChannel->ChNum ))
		return SPID_ERROR_LOCK;

	return 0;
}

// =============================================================================
// 功能：暂停SPI的DMA通道接收和发送
// 参数：pSpid,SpiDma控制块指针
// 返回：0
// =============================================================================
uint32_t SPID_RxTxPause( SpiDma *pSpid)
{
	SPID_CacheInvalid(pSpid);

	assert(pSpid);

	pSpid->sempaphore=1;
	__DMB();
	/* Start DMA 0(RX) && 1(TX) */
	if (XDMAD_StopTransfer( pSpid->pXdmad, pSpid->pRxChannel->ChNum ))
		return SPID_ERROR_LOCK;

	if (XDMAD_StopTransfer( pSpid->pXdmad, pSpid->pTxChannel->ChNum ))
		return SPID_ERROR_LOCK;

	return 0;
}

// =============================================================================
// 功能：禁止SPI的DMA通道接收和发送，并释放DMA通道
// 参数：pSpid,SpiDma控制块指针
// 返回：0，正确返回，其他，错误
// =============================================================================
uint32_t SPID_DisableDma(SpiDma* pSpi)
{
	/* Disable the SPI Peripheral */
	PMC_DisablePeripheral ( pSpi->SpiId );

	XDMAD_StopTransfer(pSpi->pXdmad, pSpi->pRxChannel->ChNum);
	XDMAD_StopTransfer(pSpi->pXdmad, pSpi->pTxChannel->ChNum);

	XDMAD_SetCallback(pSpi->pXdmad,pSpi->pRxChannel->ChNum,NULL,NULL);
	XDMAD_SetCallback(pSpi->pXdmad,pSpi->pTxChannel->ChNum,NULL,NULL);

	if(XDMAD_FreeChannel( pSpi->pXdmad, pSpi->pRxChannel->ChNum) != XDMAD_OK)
	{
		return SPID_ERROR;
	}

	memory_barrier();

	SPID_CacheInvalid(pSpi);
//	SCB_CleanInvalidateDCache();
	/* Release the dataflash semaphore */
	pSpi->sempaphore = 1;

	/* Invoke the callback associated with the current command */
	pSpi->pRxChannel->callback = NULL;
	pSpi->pTxChannel->callback = NULL;

	return 0;
}
