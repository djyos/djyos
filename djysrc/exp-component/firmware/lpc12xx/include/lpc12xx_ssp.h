/**************************************************************************//**
 * $Id: lpc12xx_ssp.h 530 2010-09-15 06:40:32Z cnh20509 $
 *
 * @file     lpc12xx_ssp.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for SSP firmware library on LPC12xx.
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
/** @defgroup SSP
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_SSP_H
#define __LPC12xx_SSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"

/* Public Types ------------------------------------------------------------- */
/** @defgroup SSP_Public_Types
 * @{
 */

/**
 * @brief SSP Init structure definition
 */
typedef struct {
    
	uint16_t DataSize; 		/*!< Specifies the SSP data size. 
	                             This parameter can be a value of @ref SSP_data_size */
                                 
	uint16_t FrameFormat;	/*!< Specifies the SSP frame format.	                             
							     This parameter can be a value of @ref SSP_frame_format */

	uint16_t CPOL;			/*!< Specifies the SSP clock polarity.
							     This parameter can be a value of @ref SSP_clock_polarity */
															
	uint16_t CPHA;			/*!< Specifies the SSP clock phase.
							     This parameter can be a value of @ref SSP_clock_phase */
    							     
	uint8_t Mode;			/*!< Specifies whether the SSP works in master or slave mode.
							     This parameter can be a value of @ref SSP_mode */     

    uint32_t ClockRate;     /*!< Specifies the SSP clock rate. */
                   	
} SSP_InitTypeDef;

/**
 * @}
 */
 

/* Public Macros -------------------------------------------------------------- */
/** @defgroup SSP_Public_Macros
 * @{
 */

    
/** @defgroup SSP_data_size 
  * @{
  */

#define SSP_DATASIZE_4      ((uint16_t) 0x0003)
#define SSP_DATASIZE_5		((uint16_t) 0x0004)
#define SSP_DATASIZE_6		((uint16_t) 0x0005)
#define SSP_DATASIZE_7		((uint16_t) 0x0006)
#define SSP_DATASIZE_8		((uint16_t) 0x0007)
#define SSP_DATASIZE_9		((uint16_t) 0x0008) 
#define SSP_DATASIZE_10		((uint16_t) 0x0009)
#define SSP_DATASIZE_11		((uint16_t) 0x000A) 
#define SSP_DATASIZE_12		((uint16_t) 0x000B) 
#define SSP_DATASIZE_13		((uint16_t) 0x000C)
#define SSP_DATASIZE_14		((uint16_t) 0x000D) 
#define SSP_DATASIZE_15		((uint16_t) 0x000E)
#define SSP_DATASIZE_16		((uint16_t) 0x000F)
#define PARAM_SSP_DATASIZE(DATASIZE) (((DATASIZE) == SSP_DATASIZE_4) || ((DATASIZE) == SSP_DATASIZE_5) || \
                                      ((DATASIZE) == SSP_DATASIZE_6) || ((DATASIZE) == SSP_DATASIZE_7) || \
                                      ((DATASIZE) == SSP_DATASIZE_8) || ((DATASIZE) == SSP_DATASIZE_9) || \
                                      ((DATASIZE) == SSP_DATASIZE_10) || ((DATASIZE) == SSP_DATASIZE_11) || \
                                      ((DATASIZE) == SSP_DATASIZE_12) || ((DATASIZE) == SSP_DATASIZE_13) || \
                                      ((DATASIZE) == SSP_DATASIZE_14) || ((DATASIZE) == SSP_DATASIZE_15) || \
                                      ((DATASIZE) == SSP_DATASIZE_16))
/**
  * @}
  */   
  
  
/** @defgroup SSP_frame_format
  * @{
  */

#define SSP_FRAMEFORMAT_SPI                ((uint16_t) (0x0000))
#define SSP_FRAMEFORMAT_SSI                ((uint16_t) (0x0010))
#define SSP_FRAMEFORMAT_MICROWIRE          ((uint16_t) (0x0020))
#define PARAM_SSP_FRAMEFORMAT(SSP_FRAMEFORMAT) (((SSP_FRAMEFORMAT) == SSP_FRAMEFORMAT_SPI) || \
                                                ((SSP_FRAMEFORMAT) == SSP_FRAMEFORMAT_SSI) || \
                                                ((SSP_FRAMEFORMAT) == SSP_FRAMEFORMAT_MICROWIRE))

/**
  * @}
  */     


/** @defgroup SSP_clock_polarity 
  * @{
  */

#define SSP_CPOL_LOW                    ((uint16_t) (0x0000))
#define SSP_CPOL_HIGH                   ((uint16_t) (0x0040))
#define PARAM_SSP_CPOL(CPOL) (((CPOL) == SSP_CPOL_LOW) || \
                              ((CPOL) == SSP_CPOL_HIGH))
                           
/**
  * @}
  */
  
  
/** @defgroup SSP_clock_phase 
  * @{
  */

#define SSP_CPHA_FIRST                 ((uint16_t) (0x0000))
#define SSP_CPHA_SECOND                ((uint16_t) (0x0080))
#define PARAM_SSP_CPHA(CPHA) (((CPHA) == SSP_CPHA_FIRST) || \
                              ((CPHA) == SSP_CPHA_SECOND))
                           
/**
  * @}
  */
  
/** @defgroup SSP_mode 
  * @{
  */

#define SSP_MODE_MASTER                 ((uint8_t) (0x00))
#define SSP_MODE_SLAVE                  ((uint8_t) (0x04))
#define PARAM_SSP_MODE(MODE) (((MODE) == SSP_MODE_MASTER) || \
                              ((MODE) == SSP_MODE_SLAVE))
/**
  * @}
  */
  

/** @defgroup SSP_interrupt_type
  * @{
  */

#define SSP_INT_RXOVERRUN           ((uint8_t) (0x01))
#define SSP_INT_RXTIMEOUT           ((uint8_t) (0x02))
#define SSP_INT_RXHALFFULL          ((uint8_t) (0x04))
#define SSP_INT_TXHALFEMPTY         ((uint8_t) (0x08))
#define PARAM_SSP_GET_INT(TYPE)     (((TYPE) == SSP_INT_RXOVERRUN) || \
                                     ((TYPE) == SSP_INT_RXTIMEOUT) || \
                                     ((TYPE) == SSP_INT_RXHALFFULL) || \
                                     ((TYPE) == SSP_INT_TXHALFEMPTY))
#define PARAM_SSP_CONFIG_INT(TYPE)   ((((TYPE) & (uint8_t)0xF0) == 0x00) && ((TYPE) != 0x00))                                                                        
#define PARAM_SSP_CLEAR_INT(TYPE)   (((TYPE) == SSP_INT_RXOVERRUN) || \
                                     ((TYPE) == SSP_INT_RXTIMEOUT))                                     

/**
  * @}
  */


/** @defgroup SSP_status 
  * @{
  */

#define SSP_STATUS_TFE      ((uint8_t) 0x01)   /*!< TX FIFO empty. */
#define SSP_STATUS_TNF      ((uint8_t) 0x02)   /*!< TX FIFO not full.*/
#define SSP_STATUS_RNE      ((uint8_t) 0x04)   /*!< RX FIFO not empty. */
#define SSP_STATUS_RFF      ((uint8_t) 0x08)   /*!< RX FIFO full.*/
#define SSP_STATUS_BSY      ((uint8_t) 0x10)   /*!< SSP busy (during process of Tx or Rx.) */
#define PARAM_SSP_GET_STATUS(STATUS) (((STATUS) == SSP_STATUS_TFE) || ((STATUS) == SSP_STATUS_TNF) || \
                                      ((STATUS) == SSP_STATUS_RNE) || ((STATUS) == SSP_STATUS_RFF) || \
                                      ((STATUS) == SSP_STATUS_BSY))
/**
  * @}
  */


/** @defgroup SSP_DMA_transfer_requests 
  * @{
  */

#define SSP_DMAREQ_TX               ((uint16_t) 0x0002)
#define SSP_DMAREQ_RX               ((uint16_t) 0x0001)
#define PARAM_SSP_DMAREQ(DMAREQ) ((((DMAREQ) & (uint16_t)0xFFFC) == 0x00) && ((DMAREQ) != 0x00))

/**
  * @}
  */

/**
  * @}
  */ 

/* Public Functions ----------------------------------------------------------- */
/** @defgroup SSP_Public_Functions
 * @{
 */
 
void SSP_ClearIntPendingBit(uint8_t SSP_INT);
void SSP_Cmd(FunctionalState NewState);
void SSP_DataSizeConfig(uint16_t SSP_DATASIZE);
void SSP_DeInit(void);
uint32_t SSP_GetIntStatusReg(void);
void SSP_DMACmd(uint16_t SSP_DMAReq, FunctionalState NewState);
IntStatus SSP_GetIntStatus (uint8_t SSP_INT);
IntStatus SSP_GetRawIntStatus(uint8_t SSP_INT);
FlagStatus SSP_GetStatus (uint8_t SSP_STATUS);
void SSP_Init(SSP_InitTypeDef *SSP_InitStruct);
void SSP_IntConfig(uint32_t IntType, FunctionalState NewState);
void SSP_LoopbackCmd(FunctionalState NewState);
uint16_t SSP_ReceiveData(void);
void SSP_SendData(uint16_t Data);
void SSP_SetClockRate (uint32_t SSP_ClockRate);
void SSP_SlaveOutputCmd(FunctionalState NewState);
void SSP_StructInit(SSP_InitTypeDef* SSP_InitStruct);


/**
 * @}
 */
 
#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_SSP_H */

/**
 * @}
 */
 
/* --------------------------------- End Of File ------------------------------ */



