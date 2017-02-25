/**************************************************************************//**
 * $Id: lpc12xx_i2c.c 584 2010-09-25 13:49:30Z cnh20446 $
 *
 * @file     lpc12xx_i2c.c
 * @brief    Contains all functions support for I2C firmware library on LPC12xx.
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
/** @addtogroup I2C
 * @{
 */

/* Includes ------------------------------------------------------------------- */
#include "lpc12xx_i2c.h"

/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _I2C

/** @defgroup 	I2C_Private_Functions
 * @{
 */

/**
  * @brief  Get I2C Status Byte.
  *
  *	@param	
  * @retval The value of the status byte.  
  */
uint8_t I2C_GetI2CStatus( void )
{
	return (LPC_I2C->STAT);
}

/**
  * @brief  Read the I2C_I2CONSET bit.
  *
  *	@param  I2C_I2CONSET: specifies the bits will be read.
  *         This parameter can be one of the following values:
  *				@arg I2C_I2CONSET_AA: Assert acknowledge flag
  *				@arg I2C_I2CONSET_SI: I2C interrupt flag
  *				@arg I2C_I2CONSET_STO: STOP flag
  *  			@arg I2C_I2CONSET_STA: START flag
  *				@arg I2C_I2CONSET_I2EN: I2C interface enable
  * @retval The I2C_CONSET bit value.  
  */
uint8_t I2C_ReadFlag( uint8_t I2C_I2CONSET )
{
  	CHECK_PARAM(PARAM_I2C_I2CONSET(I2C_I2CONSET));

	return(LPC_I2C->CONSET & I2C_I2CONSET);      /* retuen flag */
}

/**
  * @brief  Set the I2C_I2CONSET bit.
  *
  *	@param  I2C_I2CONSET: specifies the bits will be set.
  *         This parameter can be one of the following values:
  *				@arg I2C_I2CONSET_AA: Assert acknowledge flag
  *				@arg I2C_I2CONSET_SI: I2C interrupt flag
  *				@arg I2C_I2CONSET_STO: STOP flag
  *  			@arg I2C_I2CONSET_STA: START flag
  *				@arg I2C_I2CONSET_I2EN: I2C interface enable
  * @retval None  
  */
void I2C_SetFlag( uint8_t I2C_I2CONSET )
{
  	CHECK_PARAM(PARAM_I2C_I2CONSET(I2C_I2CONSET));

	LPC_I2C->CONSET = I2C_I2CONSET;      /* Set flag */
}

/**
  * @brief  Clear the I2C_I2CONCLR bit.
  *
  *	@param  I2C_I2CONCLR: specifies the bits will be clear.
  *         This parameter can be one of the following values:
  *				@arg I2C_I2CONSET_AA: Assert acknowledge flag
  *				@arg I2C_I2CONSET_SI: I2C interrupt flag
  *  			@arg I2C_I2CONSET_STA: START flag
  *				@arg I2C_I2CONSET_I2EN: I2C interface enable
  * @retval  
  */
void I2C_ClearFlag( uint8_t I2C_I2CONCLR )
{
	CHECK_PARAM(PARAM_I2C_I2CONCLR(I2C_I2CONCLR));

	LPC_I2C->CONCLR = I2C_I2CONCLR;      /* Clear flag */
}

/**
  * @brief  
  *
  *	@param  DataByte: specifies the data byte will be sent.
  * @retval None 
  */
void I2C_SendByte( uint8_t DataByte )
{
	LPC_I2C->DAT = DataByte; 
}
/**
  * @brief  
  *
  *	@param	
  * @retval The byte read from DAT register.  
  */

uint8_t I2C_GetByte( void )
{
	return(LPC_I2C->DAT);
}

/**
  * @brief  Setup clock rate for I2C peripheral.
  *
  *	@param	TargetClock: speed of I2C bus(bps).
  * @retval None  
  */
void I2C_SetClock (uint32_t TargetClock)
{
	uint32_t temp;

	temp = SystemCoreClock  / TargetClock;

	/* Set the I2C clock value to register */
	LPC_I2C->SCLH = (uint32_t)(temp / 2);
	LPC_I2C->SCLL = (uint32_t)(temp - LPC_I2C->SCLH);
}

/**
  * @brief  De-initializes the I2C peripheral registers to their default reset values.
  *
  * @param  None
  * @retval None  
  */
void I2C_DeInit(void)
{
	LPC_I2C->CONCLR = I2C_I2CONCLR_I2ENC;		/*!< Disable I2C control */
	LPC_SYSCON->SYSAHBCLKCTRL  =  LPC_SYSCON->SYSAHBCLKCTRL & (~(1<<5));	/*!< Disable power for I2C module */
}

/**
  * @brief  Initializes the i2c peripheral with specified parameter.
  *
  * @param  i2C_InitStruct: pointer to a I2C_InitStruct structure that 
  *         contains the configuration information for the I2C peripheral.
  * @retval None
  */
void I2C_Init(I2C_InitTypeDef* I2C_InitStuct)
{
	CHECK_PARAM(PARAM_I2C_MODE(I2C_InitStuct->Mode));
	CHECK_PARAM(PARAM_I2C_INTERRUPT_MODE(I2C_InitStuct->InterruptMode));

	/* Enable I2C clock and de-assert reset */
    LPC_SYSCON->PRESETCTRL |= (0x1<<1);
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);

	/*  I2C I/O config */    
    LPC_IOCON->PIO0_10 &= ~0x3F;	/*  I2C I/O config */
    LPC_IOCON->PIO0_10 |= 0x02;		/* I2C SCL */
    LPC_IOCON->PIO0_11 &= ~0x3F;
    LPC_IOCON->PIO0_11 |= 0x02;		/* I2C SDA */
    
	/*--- Clear flags ---*/
	LPC_I2C->CONCLR = I2C_I2CONCLR_AAC | I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC | I2C_I2CONCLR_I2ENC;    

	/*--- Enable Ture Open Drain mode ---*/
	LPC_IOCON->PIO0_10 |= (0x1<<10);
	LPC_IOCON->PIO0_11 |= (0x1<<10);

	/*--- Set Clock rate ---*/
	I2C_SetClock( I2C_InitStuct -> ClockRate );

	if ( I2C_InitStuct -> Mode == I2C_SLAVE )
	{
		LPC_I2C->ADR0 = I2C_InitStuct -> SlaveAddress;
	}    

	/* Enable the I2C Interrupt */
	if (I2C_InitStuct -> InterruptMode == I2C_INTERRUPT_MODE )
	{
		NVIC_EnableIRQ(I2C_IRQn);
	}
	else if (I2C_InitStuct -> InterruptMode == I2C_POLLING_MODE ) 	
	{
		NVIC_DisableIRQ(I2C_IRQn);		/* Disable the I2C Interrupt */
	}

	if ( I2C_InitStuct -> Mode == I2C_MASTER )
	{
		LPC_I2C->CONSET = I2C_I2CONSET_I2EN;
	} 
	else if (I2C_InitStuct -> Mode == I2C_SLAVE)
	{
		LPC_I2C->CONSET = I2C_I2CONSET_I2EN | I2C_I2CONSET_SI;
	}
}

/**
  * @brief  Configures functionality in I2C monitor mode.
  *
  *	@param	I2C_I2MMCTRL: Monitor mode configuration type.
  *         This parameter can be one of the following values:
  * 			@arg I2C_I2MMCTRL_MM_ENA: Enable monitor mode.
  * 			@arg I2C_I2MMCTRL_ENA_SCL: I2C module can 'stretch'
  * 					the clock line (hold it low) until it has had time to
  * 					respond to an I2C interrupt.
  *				@arg I2C_I2MMCTRL_MATCH_ALL: When this bit is set to '1'
  * 					and the I2C is in monitor mode, an interrupt will be
  * 					generated on ANY address received.
  *	@param	NewState: New State of this function, should be:
  * 			@arg ENABLE: Enable this function.
  * 			@arg DISABLE: Disable this function.
  * @retval None  
  */
void I2C_MonitorModeConfig(uint32_t I2C_I2MMCTRL, FunctionalState NewState)
{
	CHECK_PARAM(PARAM_I2C_I2MMCTRL(I2C_I2MMCTRL));

	if (NewState == ENABLE)
	{
		LPC_I2C->MMCTRL |= I2C_I2MMCTRL;
	}
	else
	{
		LPC_I2C->MMCTRL &= (~I2C_I2MMCTRL);
	}
}

/**
  * @brief  Get data from I2C data buffer in monitor mode.
  *
  * @param  None
  * @retval The value read from DATA_BUFFER.  
  *
  * Note:	In monitor mode, the I2C module may lose the ability to stretch
  * 		the clock (stall the bus) if the ENA_SCL bit is not set. This means that
  * 		the processor will have a limited amount of time to read the contents of
  * 		the data received on the bus. If the processor reads the I2DAT shift
  * 		register, as it ordinarily would, it could have only one bit-time to
  * 		respond to the interrupt before the received data is overwritten by
  * 		new data.
  */
uint8_t I2C_MonitorGetDatabuffer(void)
{
	return ((uint8_t)(LPC_I2C->DATA_BUFFER));
}



/**
 * @}
 */

#endif // _I2C

/**
 * @}
 */
/* --------------------------------- End Of File ------------------------------ */
