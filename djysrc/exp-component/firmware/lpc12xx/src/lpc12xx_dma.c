/**************************************************************************//**
 * $Id: lpc12xx_dma.c 510 2010-09-14 02:57:20Z cnh82208 $
 *
 * @file     lpc12xx_dma.c
 * @brief    Contains all functions support for DMA firmware library on LPC12xx.
 * @version  1.0
 * @date     26. Sep. 2010
 * @author   NXP MCU Team
 *
 * @note
 * Copyright (C) 2010 NXP Semiconductors(NXP). All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 ******************************************************************************/
  
/* Peripheral group --------------------------------------------------------- */
/** @addtogroup DMA
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_dma.h"


/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _DMA


/** @defgroup DMA_Private_Functions
 * @{
 */
 
/**
  * @brief  Initialize DMA controller.
  *
  * @param  InitData: Initial data for DMA controller.  
  * @retval None.
  */
void DMA_Init(DMA_InitTypeDef* InitData)
{
	LPC_SYSCON->PRESETCTRL &= ~(1 << 10);
	LPC_SYSCON->PRESETCTRL |= (1 << 10);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 17);

	InitData->CtrlBasePionter &= 0xFFFFFF00;
	InitData->AccessControl &= 0x01;
	InitData->BufferControl &= 0x01;
	LPC_DMA->CTRL_BASE_PTR = InitData->CtrlBasePionter;
	LPC_DMA->IRQ_ERR_ENABLE = InitData->EnableErrINT;
	LPC_DMA->CFG = InitData->AccessControl << 6 | 
				   InitData->BufferControl << 5 | 0x01; 
	while (!(LPC_DMA->STATUS & 0x01)); 
	LPC_DMA->CHNL_ENABLE_CLR = 0x01FFFFF;
	LPC_DMA->ERR_CLR = 0x01;   
}

/**
  * @brief  Initialize DMA channel.
  *
  * @param  InitData: Initial data for DMA Channel.  
  * @retval None.
  */
void DMA_ChannelInit(DMA_ChannelInitTypeDef* InitData)
{	
	if(InitData->ChannelNum > 18) return;
	InitData->UseBurst &= 0x01;
	InitData->PriAlter &= 0x01;
	InitData->Priority &= 0x01;
	InitData->INTEnable &= 0x01;

	if((InitData->ChannelNum == DMA_CHANNEL_4) || (InitData->ChannelNum == DMA_CHANNEL_5))
		if(InitData->UseBurst)
			LPC_DMA->CHNL_USEBURST_SET |= 0x01 << InitData->ChannelNum;
		else LPC_DMA->CHNL_USEBURST_CLR |= 0x01 << InitData->ChannelNum;
	if(InitData->PriAlter)
		LPC_DMA->CHNL_PRI_ALT_SET |= 0x01 << InitData->ChannelNum;
	else LPC_DMA->CHNL_PRI_ALT_CLR |= 0x01 << InitData->ChannelNum;
	if(InitData->Priority) LPC_DMA->CHNL_PRIORITY_SET |= 
									  0x01 << InitData->ChannelNum;
	else LPC_DMA->CHNL_PRIORITY_CLR |= 0x01 << InitData->ChannelNum;
	if(InitData->INTEnable)
		LPC_DMA->CHNL_IRQ_ENABLE |= 0x01 << InitData->ChannelNum;
	else 
		LPC_DMA->CHNL_IRQ_ENABLE &= ~(0x01 << InitData->ChannelNum);
}

/**
  * @brief  Config DMA channel.
  *
  * @param  ConfigData: Configuration data for DMA Channel.  
  * @retval None.
  */
void DMA_ControlDataConfig(DMA_ControlDataConfigTypeDef *ConfigData)
{
	if(ConfigData->ChannelNum > 18) return;
	ConfigData->PriAltStructure &= 0x01;
	ConfigData->CycleMode &= 0x03;
	//ConfigData->NextBurst &= 0x01;
	ConfigData->TransferNum &= 0x7FF;
	ConfigData->ArbitrateNum &= 0x0F;
	ConfigData->AHBReadProtection &= 0x07;
	ConfigData->AHBWriteProtection &= 0x07;
	ConfigData->SrcDataWidth &= 0x03;
	ConfigData->SrcDataIncSize &= 0x03;
	ConfigData->DstDataWidth &= 0x03;
	ConfigData->DstDataIncSize &= 0x03;
	if(ConfigData->PriAltStructure)
	{
		if(ConfigData->DMAType == DMA_M2P)
		{
			switch(ConfigData->ChannelNum)
			{
				case DMA_CHANNEL_0:
				case DMA_CHANNEL_2:
				case DMA_CHANNEL_4:
					*(uint32_t *)
						(LPC_DMA->ALT_CTRL_BASE_PTR + (ConfigData->ChannelNum << 4)) =	
							ConfigData->SrcAddr + ((0x01 << ConfigData->SrcDataWidth) * ConfigData->TransferNum)
							 - (0x01 << ConfigData->SrcDataWidth);
					*(uint32_t *)
						(LPC_DMA->ALT_CTRL_BASE_PTR + (ConfigData->ChannelNum << 4) + 4) =
							ConfigData->DstAddr;
				break;
			}
		}
		else if(ConfigData->DMAType == DMA_P2M) 
		{
			switch(ConfigData->ChannelNum)
			{
			case DMA_CHANNEL_1:
			case DMA_CHANNEL_3:
			case DMA_CHANNEL_5:
			case DMA_CHANNEL_6:
				*(uint32_t *)
					(LPC_DMA->ALT_CTRL_BASE_PTR + (ConfigData->ChannelNum << 4)) =	
						ConfigData->SrcAddr;
				*(uint32_t *)
					(LPC_DMA->ALT_CTRL_BASE_PTR + (ConfigData->ChannelNum << 4) + 4) =
						ConfigData->DstAddr + ((0x01 << ConfigData->DstDataWidth) * ConfigData->TransferNum)
						 - (0x01 << ConfigData->DstDataWidth);
				break;
			}
		}
		else
		{
			*(uint32_t *)
				(LPC_DMA->ALT_CTRL_BASE_PTR + (ConfigData->ChannelNum << 4)) =	
					ConfigData->SrcAddr + ((0x01 << ConfigData->SrcDataWidth) * ConfigData->TransferNum)
					 - (0x01 << ConfigData->SrcDataWidth);
			*(uint32_t *)
				(LPC_DMA->ALT_CTRL_BASE_PTR + (ConfigData->ChannelNum << 4) + 4) =
					ConfigData->DstAddr + ((0x01 << ConfigData->DstDataWidth) * ConfigData->TransferNum)
					 - (0x01 << ConfigData->DstDataWidth);
		}
		*(uint32_t *)
			(LPC_DMA->ALT_CTRL_BASE_PTR + (ConfigData->ChannelNum << 4) + 8) =
				ConfigData->CycleMode | 
				ConfigData->NextBurst << 3 | 
				(ConfigData->TransferNum - 1) << 4 |
				(ConfigData->ArbitrateNum) << 14 | 
				ConfigData->AHBReadProtection << 18 |
				ConfigData->AHBWriteProtection << 21 | 
				ConfigData->SrcDataWidth << 24 |
				ConfigData->SrcDataIncSize << 26 | 
				ConfigData->DstDataWidth << 28 | 
				ConfigData->DstDataIncSize << 30;
	}
	else
	{
		if(ConfigData->DMAType == DMA_M2P)
		{
			switch(ConfigData->ChannelNum)
			{
				case DMA_CHANNEL_0:
				case DMA_CHANNEL_2:
				case DMA_CHANNEL_4:
					*(uint32_t *)
						(LPC_DMA->CTRL_BASE_PTR + (ConfigData->ChannelNum << 4)) =	
							ConfigData->SrcAddr + ((0x01 << ConfigData->SrcDataWidth) * ConfigData->TransferNum)
							 - (0x01 << ConfigData->SrcDataWidth);
					*(uint32_t *)
						(LPC_DMA->CTRL_BASE_PTR + (ConfigData->ChannelNum << 4) + 4) =
							ConfigData->DstAddr;
				break;
			}
		}
		else if(ConfigData->DMAType == DMA_P2M) 
		{
			switch(ConfigData->ChannelNum)
			{
			case DMA_CHANNEL_1:
			case DMA_CHANNEL_3:
			case DMA_CHANNEL_5:
			case DMA_CHANNEL_6:
				*(uint32_t *)
					(LPC_DMA->CTRL_BASE_PTR + (ConfigData->ChannelNum << 4)) =	
						ConfigData->SrcAddr;
				*(uint32_t *)
					(LPC_DMA->CTRL_BASE_PTR + (ConfigData->ChannelNum << 4) + 4) =
						ConfigData->DstAddr + ((0x01 << ConfigData->DstDataWidth) * ConfigData->TransferNum)
						 - (0x01 << ConfigData->DstDataWidth);
				break;
			}
		}
		else
		{
			*(uint32_t *)
				(LPC_DMA->CTRL_BASE_PTR + (ConfigData->ChannelNum << 4)) =	
					ConfigData->SrcAddr + ((0x01 << ConfigData->SrcDataWidth) * ConfigData->TransferNum)
					 - (0x01 << ConfigData->SrcDataWidth);
			*(uint32_t *)
				(LPC_DMA->CTRL_BASE_PTR + (ConfigData->ChannelNum << 4) + 4) =
					ConfigData->DstAddr + ((0x01 << ConfigData->DstDataWidth) * ConfigData->TransferNum)
					 - (0x01 << ConfigData->DstDataWidth);
		}
		*(uint32_t *)
			(LPC_DMA->CTRL_BASE_PTR + (ConfigData->ChannelNum << 4) + 8) =
				ConfigData->CycleMode | 
				ConfigData->NextBurst << 3 | 
				(ConfigData->TransferNum - 1) << 4 |
				(ConfigData->ArbitrateNum) << 14 | 
				ConfigData->AHBReadProtection << 18 |
				ConfigData->AHBWriteProtection << 21 | 
				ConfigData->SrcDataWidth << 24 |
				ConfigData->SrcDataIncSize << 26 | 
				ConfigData->DstDataWidth << 28 | 
				ConfigData->DstDataIncSize << 30;
	}	     
}

/**
  * @brief  Enable/Disable DMA channel.
  *
  * @param  ChannelNum: Number of DMA channel which will be enabled.    
  *         This parameter can be one of the following values:
  *             DMA_ChANNEL_0: 
  *             DMA_ChANNEL_1:
  *             DMA_ChANNEL_2:
  *             DMA_ChANNEL_3:
  *             DMA_ChANNEL_4:
  *             DMA_ChANNEL_5:
  *             DMA_ChANNEL_6:
  *             DMA_ChANNEL_7:
  *             DMA_ChANNEL_8: 
  *             DMA_ChANNEL_9:
  *             DMA_ChANNEL_10:
  *             DMA_ChANNEL_11:
  *             DMA_ChANNEL_12:
  *             DMA_ChANNEL_13:
  *             DMA_ChANNEL_14:
  *             DMA_ChANNEL_15:
  *             DMA_ChANNEL_16:
  *             DMA_ChANNEL_17:
  *             DMA_ChANNEL_18:
  * @param  NewState:
  *         This parameter can be one of the following values:
  *             ENABLE:
  *             DISABLE:
  * @retval None.
  */
void DMA_ChannelCmd(uint8_t ChannelNum, FunctionalState NewState)
{ 
	if(ChannelNum > 18) return;
	if(NewState == ENABLE) LPC_DMA->CHNL_ENABLE_SET |= 0x01 << ChannelNum;
	else LPC_DMA->CHNL_ENABLE_CLR |= 0x01 << ChannelNum;    
}

/**
  * @brief  Generate software DMA request.
  *
  * @param  ChannelNum:     
  *         This parameter can be one of the following values:
  *             DMA_ChANNEL_0: 
  *             DMA_ChANNEL_1:
  *             DMA_ChANNEL_2:
  *             DMA_ChANNEL_3:
  *             DMA_ChANNEL_4:
  *             DMA_ChANNEL_5:
  *             DMA_ChANNEL_6:
  *             DMA_ChANNEL_7:
  *             DMA_ChANNEL_8: 
  *             DMA_ChANNEL_9:
  *             DMA_ChANNEL_10:
  *             DMA_ChANNEL_11:
  *             DMA_ChANNEL_12:
  *             DMA_ChANNEL_13:
  *             DMA_ChANNEL_14:
  *             DMA_ChANNEL_15:
  *             DMA_ChANNEL_16:
  *             DMA_ChANNEL_17:
  *             DMA_ChANNEL_18:
  * @retval None.
  */
void DMA_ChannelSWRequest(uint8_t ChannelNum)
{ 
	if(ChannelNum > 18) return;
	LPC_DMA->CHNL_SW_REQUEST |= 0x01 << ChannelNum;    
}

/**
  * @brief  Diable channel from generationg DMA requests.
  *
  * @param  ChannelNum:     
  *         This parameter can be one of the following values:
  *             DMA_ChANNEL_0: 
  *             DMA_ChANNEL_1:
  *             DMA_ChANNEL_2:
  *             DMA_ChANNEL_3:
  *             DMA_ChANNEL_4:
  *             DMA_ChANNEL_5:
  *             DMA_ChANNEL_6:
  *             DMA_ChANNEL_7:
  *             DMA_ChANNEL_8: 
  *             DMA_ChANNEL_9:
  *             DMA_ChANNEL_10:
  *             DMA_ChANNEL_11:
  *             DMA_ChANNEL_12:
  *             DMA_ChANNEL_13:
  *             DMA_ChANNEL_14:
  *             DMA_ChANNEL_15:
  *             DMA_ChANNEL_16:
  *             DMA_ChANNEL_17:
  *             DMA_ChANNEL_18:
  * @retval None.
  */
void DMA_ChannelRequestMask(uint8_t ChannelNum)
{
	if(ChannelNum > 18) return;
	LPC_DMA->CHNL_REQ_MASK_SET |= 0x01 << ChannelNum;     
}

/**
  * @brief  Get the status of which data structure
  *         is in use for the corresponding DMA channel.
  *
  * @param  None.
  * @retval SET bit means the corresponding channel is using the 
  *			primary data structure.
  *			CLR bit means the corresponding channel is using the 
  *			alternate data structure.
  */
uint32_t DMA_ChannelGetPriAltStatus(void)
{
	return(LPC_DMA->CHNL_PRI_ALT_SET & 0x01FFFFF);
}

/**
  * @brief  Diable DMA controller.
  *
  * @param  None.
  * @retval None.
  */

void DMA_Cmd(FunctionalState NewState)
{
	if(NewState != ENABLE) LPC_DMA->CFG &= 0x60;
	else LPC_DMA->CFG |= ~0x60;
}

/**
  * @brief  Get DMA controller status.
  *
  * @param[in]	 None. 
  * @param[out]  DMAStatus: DMA controller status.  
  * @retval None.
  */
void DMA_GetStatus(DMA_STATUS_Type* DMAStatus)
{
	DMAStatus->ControllerStatus = LPC_DMA->STATUS & 0x01;
	DMAStatus->ControlStateMachine = (LPC_DMA->STATUS >> 4) & 0xFF;
	DMAStatus->Channels = ((LPC_DMA->STATUS >> 16) & 0x07) + 1;
}

/**
  * @brief  Get the channel interrupt status.
  *
  * @param  None.
  * @retval Channels which transfer have been completed.
  */
uint32_t DMA_GetChannelIntStatusReg(void)
{
	return(LPC_DMA->CHNL_IRQ_STATUS & 0x1FFFFF);
}

/**
  * @brief  Clear the DMA done status.
  *
  * @param  ChannelNum: Number of DMA channel which DMA done status will be clear.    
  *         This parameter can be one of the following values:
  *             DMA_ChANNEL_0: 
  *             DMA_ChANNEL_1:
  *             DMA_ChANNEL_2:
  *             DMA_ChANNEL_3:
  *             DMA_ChANNEL_4:
  *             DMA_ChANNEL_5:
  *             DMA_ChANNEL_6:
  *             DMA_ChANNEL_7:
  *             DMA_ChANNEL_8: 
  *             DMA_ChANNEL_9:
  *             DMA_ChANNEL_10:
  *             DMA_ChANNEL_11:
  *             DMA_ChANNEL_12:
  *             DMA_ChANNEL_13:
  *             DMA_ChANNEL_14:
  *             DMA_ChANNEL_15:
  *             DMA_ChANNEL_16:
  *             DMA_ChANNEL_17:
  *             DMA_ChANNEL_18:
  * @retval None.
  */
void DMA_ClrIntPending(uint8_t ChannelNum)
{
	ChannelNum &= 0x07;
	LPC_DMA->CHNL_IRQ_STATUS |= 0x01 << ChannelNum;
}

/**
  * @brief  Set the dma_err signal to LOW.
  *
  * @param  None.
  * @retval None.
  */
void DMA_ClrBusErr(void)
{
	LPC_DMA->ERR_CLR = 0x01;
}  

/**
 * @}
 */

#endif // _DMA

/**
 * @}
 */
  
/* --------------------------------- End Of File ------------------------------ */
