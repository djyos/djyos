/**************************************************************************//**
 * $Id: lpc12xx_dma.h 508 2010-09-14 02:46:50Z cnh82208 $
 *
 * @file     lpc12xx_dma.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for DMA firmware library on LPC12xx.
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
/** @defgroup DMA
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_DMA_H
#define __LPC12xx_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"

/* Public Macros -------------------------------------------------------------- */
/** @defgroup DMA_Public_Macros
 * @{
 */


/** @defgroup DMA_InitTypeDef 
  * @{
  */
/**  user access */
#define DMA_USERACCESS			0
/**  privileged access */
#define DMA_PRIVILEGEACCESS		1
/**  non-bufferable */
#define DMA_NONEBUFFERED		0
/**  bufferable */
#define DMA_BUFFERED			1
/**
  * @}
  */ 


/** @defgroup DMA_CHN_Macro_Defines 
  * @{
  */  

#define DMA_CHANNEL_0			0
#define DMA_CHANNEL_1			1
#define DMA_CHANNEL_2			2
#define DMA_CHANNEL_3			3
#define DMA_CHANNEL_4			4
#define DMA_CHANNEL_5			5
#define DMA_CHANNEL_6			6
#define DMA_CHANNEL_7			7
#define DMA_CHANNEL_8			8
#define DMA_CHANNEL_9			9
#define DMA_CHANNEL_10			10
#define DMA_CHANNEL_11			11
#define DMA_CHANNEL_12			12
#define DMA_CHANNEL_13			13
#define DMA_CHANNEL_14			14
#define DMA_CHANNEL_15			15
#define DMA_CHANNEL_16			16
#define DMA_CHANNEL_17			17
#define DMA_CHANNEL_18			18


/**
  * @}
  */ 


/** @defgroup DMA_ChannelInitTypeDef 
  * @{
  */
/**  disable the single DMA request input */
#define DMA_CHANNELUSEBURST		1
/**  eable the single DMA request input */
#define DMA_CHANNELNOTUSEBURST	0
/**  use the primary data structure */
#define DMA_PRIDATASTRUCTURE	0
/**  use the alternate data structure */
#define DMA_ALTDATASTRUCTURE	1
/**  use a high priority level */
#define DMA_HIGHPRIORITY		1
/**  use the default priority level */
#define DMA_DEFAULTPRIORITY		0
/**
  * @}
  */ 


/** @defgroup DMA_ControlDataConfigTypeDef 
  * @{
  */
#define DMA_M2M					0
#define DMA_M2P					1
#define DMA_P2M					2
 /**  DMA cycle operating mode is: stop */
#define DMA_STOPMODE			0
/**  DMA cycle operating mode is: basic */
#define DMA_BASICMODE			1
/**  DMA cycle operating mode is: auto-request */
#define DMA_AUTOREQUEST			2
/**  DMA cycle operating mode is: ping-pong */
#define DMA_PINGPONGMODE		3
/**  DMA arbitrate number */
#define DMA_ARBITRATENUM_1		0
#define DMA_ARBITRATENUM_2		1
#define DMA_ARBITRATENUM_4		2
#define DMA_ARBITRATENUM_8		3
#define DMA_ARBITRATENUM_16		4
#define DMA_ARBITRATENUM_32		5
#define DMA_ARBITRATENUM_64		6
#define DMA_ARBITRATENUM_128	7
#define DMA_ARBITRATENUM_256	8
#define DMA_ARBITRATENUM_512	9
#define DMA_ARBITRATENUM_1024	10
/**  data width is: byte */
#define DMA_BYTELEN				0
/**  data width is: half-word */
#define DMA_HALFWORDLEN			1
/**  data width is: word */
#define DMA_WORDLEN				2
/**  DMA data address increments */
#define DMA_DATA_INC_BYTE		0
#define DMA_DATA_INC_HALFWORD	1
#define DMA_DATA_INC_WORD		2
#define DMA_DATA_INC_NO			3
/**
  * @}
  */

/**
  * @}
  */
  
/* Public Types ------------------------------------------------------------- */
/** @defgroup DMA_Public_Types
 * @{
 */

 /**
 * @brief DMA configuration structure definition
 */
   
typedef struct
{
	uint32_t	CtrlBasePionter;		/*!< Pointer to the base address of the primary data structure. 
	                             		It must point to a location in the chip¡¯s SRAM */
	uint8_t		AccessControl;			/*!< Channel protection access. Should be: 
	                             		DMA_USERACCESS or DMA_PRIVILEGEACCESS */
	uint8_t		BufferControl; 			/*!< Channel protection buffer control. Should be: 
	                             		DMA_NONEBUFFERED or DMA_BUFFERED */
	uint8_t		EnableErrINT;			/*!< Enable or disable DMA error interrupt. Should be: 
	                             		ENABLE or DISABLE */
}DMA_InitTypeDef;	   

typedef struct
{
	uint8_t		ChannelNum;				/*!< DMA channel number. */
	uint8_t		UseBurst;				/*!< Disables dma_sreq[C] from generating DMA requests. Should be: 
	                             		DMA_CHANNELUSEBURST or DMA_CHANNELNOTUSEBURST */
	uint8_t		PriAlter;				/*!< This channel will use primary or alternate data structure. Should be: 
	                             		DMA_PRIDATASTRUCTURE or DMA_CHANNELNOTUSEBURST */ 
	uint8_t		Priority;				/*!< configure DMA channel priority level. Should be: 
	                             		DMA_HIGHPRIORITY or DMA_ALTDATASTRUCTURE */							  
	uint8_t		INTEnable;				/*!< Enable or disable DMA channel interrupt. Should be: 
	                             		ENABLE or DISABLE */
}DMA_ChannelInitTypeDef;

typedef struct
{	
	uint8_t		ChannelNum;				/*!< DMA channel number. */
	uint8_t		DMAType;				/*!< Should be: DMA_M2M, DMA_M2P or DMA_P2M */
	uint8_t		PriAltStructure;		/*!< Should be: DMA_PRIDATASTRUCTURE or DMA_ALTDATASTRUCTURE */
	uint32_t	DstAddr;
	uint32_t	SrcAddr;
	uint8_t		CycleMode;				/*!< The operating mode of the DMA cycle. Should be: 
	                             		DMA_STOPMODE, DMA_BASICMODE, DMA_AUTOREQUEST or DMA_PINGPONGMODE */
	uint8_t		NextBurst;				/*!< Should be: ENABLE or DISABLE */
	uint16_t	TransferNum;			/*!< the total number of DMA transfers that the DMA cycle contains */
	uint8_t		ArbitrateNum;			/*!< how many DMA transfers can occur before the controller re-arbitrates */
	uint8_t		AHBReadProtection;		/*!< control AHB Lite access protection when the controller reads the source data */
	uint8_t		AHBWriteProtection;		/*!< control AHB Lite access protection when the controller writes the source data */
	uint8_t		SrcDataWidth; 			/*!< source data width. Should be: 
	                             		DMA_BYTELEN,DMA_HALFWORDLEN or DMA_WORDLEN */
	uint8_t		SrcDataIncSize;			/*!< source address increment. Should be: 
	                             		DMA_DATA_INC_BYTE,DMA_DATA_INC_HALFWORD, DMA_DATA_INC_WORD or DMA_DATA_INC_NO */
	uint8_t		DstDataWidth;			/*!< destination data width. Should be: 
	                             		DMA_BYTELEN,DMA_HALFWORDLEN or DMA_WORDLEN */
	uint8_t		DstDataIncSize;			/*!< destination address increment. Should be: 
	                             		DMA_DATA_INC_BYTE,DMA_DATA_INC_HALFWORD, DMA_DATA_INC_WORD or DMA_DATA_INC_NO */	
}DMA_ControlDataConfigTypeDef;

typedef struct
{
	uint8_t		ControllerStatus;		/*!< DMA controller status: enable or disable. */
	uint8_t		ControlStateMachine;	/*!< DMA controller state machine status. */
	uint8_t		Channels;				
}DMA_STATUS_Type;

/**
 * @}
 */



/* Public Functions ----------------------------------------------------------- */
/** @defgroup DMA_Public_Functions
 * @{
 */
void DMA_Init(DMA_InitTypeDef* InitData);
void DMA_ChannelInit(DMA_ChannelInitTypeDef* InitData);
void DMA_ControlDataConfig(DMA_ControlDataConfigTypeDef *ConfigData);
void DMA_ChannelCmd(uint8_t ChannelNum, FunctionalState NewState);
void DMA_ChannelSWRequest(uint8_t ChannelNum);
void DMA_ChannelRequestMask(uint8_t ChannelNum);
uint32_t DMA_ChannelGetPriAltStatus(void);
void DMA_Cmd(FunctionalState NewState);
void DMA_GetStatus(DMA_STATUS_Type* DMAStatus);
uint32_t DMA_GetChannelIntStatusReg(void);
void DMA_ClrIntPending(uint8_t ChannelNum);
void DMA_ClrBusErr(void);


/**
 * @}
 */ 

#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_DMA_H */

/**
 * @}
 */
 
/* --------------------------------- End Of File ------------------------------ */



