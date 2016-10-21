/**************************************************************************//**
 * $Id: lpc12xx_i2c.h 583 2010-09-25 13:48:34Z cnh20446 $
 *
 * @file     lpc12xx_i2c.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for I2C firmware library on LPC12xx.
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
 
/* Peripheral group ----------------------------------------------------------- */
/** @defgroup I2C
 * @ingroup LPC1200CMSIS_FwLib_Drivers
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_I2C_H
#define __LPC12xx_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"

/* Public Types ------------------------------------------------------------- */
/** @defgroup I2C_Public_Types
 * @{
 */

/**
 * @brief I2C Init structure definition
 */
typedef struct {
    
	uint8_t Mode;			/*!< Specifies whether the I2C works in master or slave mode.
							     This parameter can be a value of @ref I2C_mode */     

    uint32_t ClockRate;     /*!< Specifies the I2C clock rate. */

	uint32_t SlaveAddress;  /*!< Specifies the address in I2C slave mode. */
	
	uint8_t InterruptMode;	/*!< Specifies I2C works in I2C interrupt mode or polling mode. */

} I2C_InitTypeDef;

/**
 * @}
 */
 

/* Public Macros -------------------------------------------------------------- */
/** @defgroup I2C_Public_Macros
 * @{
 */

/** @defgroup I2C_mode 
  * @{
  */
#define I2C_MASTER		((uint8_t) (0x01))
#define I2C_SLAVE		((uint8_t) (0x02))
#define PARAM_I2C_MODE(MODE) 		(((MODE) == I2C_MASTER) || \
									((MODE) == I2C_SLAVE))

/**
  * @}
  */

/** @defgroup I2C_interrupt_mode 
  * @{
  */
#define I2C_POLLING_MODE		((uint8_t) (0x00))
#define I2C_INTERRUPT_MODE		((uint8_t) (0x01))
#define PARAM_I2C_INTERRUPT_MODE(MODE) 		(((MODE) == I2C_INTERRUPT_MODE) || \
											((MODE) == I2C_POLLING_MODE))

/**
  * @}
  */

/** @defgroup I2C_control_set
  * @{
  */
#define I2C_I2CONSET_AA			((uint8_t)(0x04))	/*!< Assert acknowledge flag */
#define I2C_I2CONSET_SI			((uint8_t)(0x08)) 	/*!< I2C interrupt flag */
#define I2C_I2CONSET_STO			((uint8_t)(0x10)) 	/*!< STOP flag */
#define I2C_I2CONSET_STA			((uint8_t)(0x20)) 	/*!< START flag */
#define I2C_I2CONSET_I2EN			((uint8_t)(0x40)) 	/*!< I2C interface enable */
#define PARAM_I2C_I2CONSET(I2C_I2CONSET) 		((((I2C_I2CONSET) & (uint8_t)0x83) == 0x00) && ((I2C_I2CONSET) != 0x00))

/**
  * @}
  */
  
/** @defgroup I2C_control_clear
  * @{
  */

#define I2C_I2CONCLR_AAC			((uint8_t)(0x04))  	/*!< Assert acknowledge Clear bit */
#define I2C_I2CONCLR_SIC			((uint8_t)(0x08))	/*!< I2C interrupt Clear bit */
#define I2C_I2CONCLR_STAC			((uint8_t)(0x20))	/*!< START flag Clear bit */
#define I2C_I2CONCLR_I2ENC			((uint8_t)(0x40))	/*!< I2C interface Disable bit */
#define PARAM_I2C_I2CONCLR(I2C_I2CONCLR) 		((((I2C_I2CONCLR) & (uint8_t)0x93) == 0x00) && ((I2C_I2CONCLR) != 0x00))

/**
  * @}
  */
  
/** @defgroup I2C_monitor_mode_control
  * @{
  */

#define I2C_I2MMCTRL_MM_ENA			((1<<0))		/*!< Monitor mode enable */
#define I2C_I2MMCTRL_ENA_SCL		((1<<1))		/*!< SCL output enable */
#define I2C_I2MMCTRL_MATCH_ALL		((1<<2))		/*!< Select interrupt register match */
#define PARAM_I2C_I2MMCTRL(I2C_I2MMCTRL) 		((((I2C_I2MMCTRL) & (uint8_t)0xF9) == 0x00) && ((I2C_I2MMCTRL) != 0x00))
									
/**
  * @}
  */
  
/** @defgroup I2C_slave_general_call
  * @{
  */

#define I2C_I2ADR_GC				((1<<0))		/*!< General Call enable bit */

/**
  * @}
  */

/** @defgroup I2C_mask
  * @{
  */

#define I2C_I2MASK_MASK(n)			((n&0xFE))		/*!< I2C Mask Register mask field */

/**
  * @}
  */

/** @defgroup I2C_master_transmit_mode_status
  * @{
  */

#define I2C_I2STAT_M_TX_START				((0x08)) 		/*!< A start condition has been transmitted */
#define I2C_I2STAT_M_TX_RESTART				((0x10))	 	/*!< A repeat start condition has been transmitted */
#define I2C_I2STAT_M_TX_SLAW_ACK			((0x18))	 	/*!< SLA+W has been transmitted, ACK has been received */
#define I2C_I2STAT_M_TX_SLAW_NACK			((0x20))	  	/*!< SLA+W has been transmitted, NACK has been received */
#define I2C_I2STAT_M_TX_DAT_ACK				((0x28))		/*!< Data has been transmitted, ACK has been received */
#define I2C_I2STAT_M_TX_DAT_NACK			((0x30))	 	/*!< Data has been transmitted, NACK has been received */
#define I2C_I2STAT_M_TX_ARB_LOST			((0x38))	 	/*!< Arbitration lost in SLA+R/W or Data bytes */

/**
  * @}
  */

/** @defgroup I2C_master_receive_mode_status
  * @{
  */  

#define I2C_I2STAT_M_RX_START				((0x08))		/*!< A start condition has been transmitted */
#define I2C_I2STAT_M_RX_RESTART				((0x10))		/*!< A repeat start condition has been transmitted */
#define I2C_I2STAT_M_RX_ARB_LOST			((0x38))		/*!< Arbitration lost */
#define I2C_I2STAT_M_RX_SLAR_ACK			((0x40))		/*!< SLA+R has been transmitted, ACK has been received */
#define I2C_I2STAT_M_RX_SLAR_NACK			((0x48))   		/*!< SLA+R has been transmitted, NACK has been received */
#define I2C_I2STAT_M_RX_DAT_ACK				((0x50))	  	/*!< Data has been received, ACK has been returned */
#define I2C_I2STAT_M_RX_DAT_NACK			((0x58))	 	/*!< Data has been received, NACK has been return */

/**
  * @}
  */

/** @defgroup I2C_slave_receive_mode_status
  * @{
  */  

#define I2C_I2STAT_S_RX_SLAW_ACK			((0x60))		/*!< Own slave address has been received, ACK has been returned */
#define I2C_I2STAT_S_RX_ARB_LOST_M_SLA		((0x68))		/*!< Arbitration lost in SLA+R/W as master */
#define I2C_I2STAT_S_RX_GENCALL_ACK			((0x70))		/*!< General call address has been received, ACK has been returned */
#define I2C_I2STAT_S_RX_ARB_LOST_M_GENCALL	((0x78))		/*!< Arbitration lost in SLA+R/W (GENERAL CALL) as master */
#define I2C_I2STAT_S_RX_PRE_SLA_DAT_ACK		((0x80))		/*!< Previously addressed with own SLV address;
 															 * Data has been received, ACK has been return */
#define I2C_I2STAT_S_RX_PRE_SLA_DAT_NACK	((0x88))		/*!< Previously addressed with own SLA;
															 * Data has been received and NOT ACK has been return */
#define I2C_I2STAT_S_RX_PRE_GENCALL_DAT_ACK		((0x90))	/*!< Previously addressed with General Call;
 															 * Data has been received and ACK has been return */
#define I2C_I2STAT_S_RX_PRE_GENCALL_DAT_NACK	((0x98))	/*!< Previously addressed with General Call;
															 * Data has been received and NOT ACK has been return */
#define I2C_I2STAT_S_RX_STA_STO_SLVREC_SLVTRX	((0xA0))	/*!< A STOP condition or repeated START condition has
															 * been received while still addressed as SLV/REC
															 * (Slave Receive) or SLV/TRX (Slave Transmit) */

/**
  * @}
  */
  

/** @defgroup I2C_slave_transmit_mode_status
  * @{
  */

#define I2C_I2STAT_S_TX_SLAR_ACK			((0xA8))		/*!< Own SLA+R has been received, ACK has been returned */	
#define I2C_I2STAT_S_TX_ARB_LOST_M_SLA		((0xB0))		/*!< Arbitration lost in SLA+R/W as master */
#define I2C_I2STAT_S_TX_DAT_ACK				((0xB8))		/*!< Data has been transmitted, ACK has been received */
#define I2C_I2STAT_S_TX_DAT_NACK			((0xC0))	  	/*!< Data has been transmitted, NACK has been received */
#define I2C_I2STAT_S_TX_LAST_DAT_ACK		((0xC8))		/*!< Last data byte in I2DAT has been transmitted (AA = 0);
															 ACK has been received */
#define I2C_SLAVE_TIME_OUT					0x10000UL		/*!< Time out in case of using I2C slave mode */

/**
  * @}
  */
  
/**
 * @}
 */

/* Public Functions ----------------------------------------------------------- */
/** @defgroup I2C_Public_Functions
 * @{
 */

uint8_t I2C_GetI2CStatus( void );
uint8_t I2C_ReadFlag( uint8_t I2C_I2CONSET );
void I2C_SetFlag( uint8_t I2C_I2CONSET );
void I2C_ClearFlag( uint8_t I2CONCLR );
void I2C_SendByte( uint8_t DataByte );
uint8_t I2C_GetByte( void );
void I2C_DeInit(void);
void I2C_Init( I2C_InitTypeDef* I2C_InitStuct );
void I2C_MonitorModeConfig(uint32_t I2C_I2MMCTRL, FunctionalState NewState);
uint8_t I2C_MonitorGetDatabuffer(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_I2C_H */

/**
 * @}
 */


/* --------------------------------- End Of File ------------------------------ */
