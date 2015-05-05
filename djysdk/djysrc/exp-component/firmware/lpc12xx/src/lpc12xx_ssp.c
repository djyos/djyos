/**************************************************************************//**
 * $Id: lpc12xx_ssp.c 531 2010-09-15 06:40:48Z cnh20509 $
 *
 * @file     lpc12xx_ssp.c
 * @brief    Contains all functions support for SSP firmware library on LPC12xx.
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
/** @addtogroup SSP
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_ssp.h"


/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _SSP

/** @defgroup SSP_Private_Macros
  * @{
  */
  
/* SSP SSE mask */  
#define CR1_SSP_ENABLE      ((uint8_t) 0x02)
#define CR1_SSP_DISABLE     ((uint8_t) 0xFD)

/* SSP SOD mask */
#define CR1_SOD_ENABLE      ((uint8_t) 0xF7)  
#define CR1_SOD_DISABLE     ((uint8_t) 0x08)

/* SSP LBM mask */
#define CR1_LBM_ENABLE      ((uint8_t) 0x01)
#define CR1_LBM_DISABLE     ((uint8_t) 0xFE)

/**
 * @}
 */

 
/** @defgroup SSP_Private_Functions
 * @{
 */

/**
  * @brief  Setup bit clock rate for SSP device.
  *
  * @param  SSP_ClockRate: target SSP bit clock rate (unit: Hz). 
  * @retval None   
  */ 
void SSP_SetClockRate (uint32_t SSP_ClockRate)
{
    uint32_t prescale, cr0_div, cmp_clk, ssp_clk;

    /* The SSP clock is derived from the main clock. */
    ssp_clk = cn_mclk / LPC_SYSCON->SSPCLKDIV;
    
	/* Find closest divider to get at or under the target frequency.
	   Use smallest prescale possible and rely on the divider to get
	   the closest target frequency */
	cr0_div = 0;
	cmp_clk = 0xFFFFFFFF;
	prescale = 2;
	while (cmp_clk > SSP_ClockRate)
	{
		cmp_clk = ssp_clk / ((cr0_div + 1) * prescale);
		if (cmp_clk > SSP_ClockRate)
		{
			cr0_div++;
			if (cr0_div > 0xFF)
			{
				cr0_div = 0;
				prescale += 2;
			}
		}
	}

    /* Write computed prescaler and divider back to register */
    LPC_SSP->CR0 &= 0x00FF;
    LPC_SSP->CR0 |= cr0_div << 8;
    LPC_SSP->CPSR = prescale & 0xFF;
}

/**
  * @brief  Deinitializes the SSP peripheral registers to their default 
  *         reset values.
  *
  * @param  None
  * @retval None    
  */
void SSP_DeInit(void)
{
    /* Reset the SSP peripheral */
	LPC_SYSCON->PRESETCTRL &= ~(1<<0);
	
	/* Disable clock for SSP peripheral */
	LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<11);
}

/**
  * @brief  Initializes the SSP peripheral according to the specified 
  *         parameters in the SSP_InitStruct.
  *
  * @param  SSP_InitStruct: pointer to a SSP_InitStruct structure that 
  *         contains the configuration information for the SSP peripheral.
  * @retval None  
  */
void SSP_Init(SSP_InitTypeDef *SSP_InitStruct)
{  
    /* check the parameters */
    CHECK_PARAM(PARAM_SSP_DATASIZE(SSP_InitStruct->DataSize));    
    CHECK_PARAM(PARAM_SSP_FRAMEFORMAT(SSP_InitStruct->FrameFormat));
    CHECK_PARAM(PARAM_SSP_CPOL(SSP_InitStruct->CPOL));
    CHECK_PARAM(PARAM_SSP_CPHA(SSP_InitStruct->CPHA));
    CHECK_PARAM(PARAM_SSP_MODE(SSP_InitStruct->Mode));

    /* Enable SSP clock and de-assert reset */
    LPC_SYSCON->PRESETCTRL |= (1<<0);
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11);
  
    /* SSP CR0 configuration */
    LPC_SSP->CR0 = SSP_InitStruct->DataSize | SSP_InitStruct->FrameFormat | 
                   SSP_InitStruct->CPOL | SSP_InitStruct->CPHA;;
       
    /* SSP CR1 configuration */
    LPC_SSP->CR1 = SSP_InitStruct->Mode;
    
    /* SSP clock configuration */
    SSP_SetClockRate (SSP_InitStruct->ClockRate);
}

/**
  * @brief  Fills each SSP_InitStruct member with its default value.
  *
  * @param  SSP_InitStruct : pointer to a SSP_InitStruct structure which will be initialized.
  * @retval None
  */
void SSP_StructInit(SSP_InitTypeDef* SSP_InitStruct)
{
    /* Reset SSP init structure parameters values */
    SSP_InitStruct->DataSize = SSP_DATASIZE_8;
    SSP_InitStruct->FrameFormat = SSP_FRAMEFORMAT_SPI;
    SSP_InitStruct->CPOL = SSP_CPOL_LOW;
    SSP_InitStruct->CPHA = SSP_CPHA_FIRST;
    SSP_InitStruct->Mode = SSP_MODE_MASTER;
    SSP_InitStruct->ClockRate = 1500000;
}

/**
  * @brief  Enables or disables the specified SSP peripheral.
  *
  * @param  NewState: new state of the SSP peripheral. 
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None 
  */
void SSP_Cmd(FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));
    
	if (NewState == ENABLE)
	{
		LPC_SSP->CR1 |= CR1_SSP_ENABLE;
	}
	else
	{
		LPC_SSP->CR1 &= CR1_SSP_DISABLE;
	}
}


/**
  * @brief  Enables or disables the Loop Back mode in SSP peripheral.
  *
  * @param  NewState: new state of the Loop Back mode (ENABLE or DISABLE). 
  * @retval None 
  *  
  * If enabled, serial input is taken from the serial output (MOSI or MISO)
  * rather than the serial input pin (MISO or MOSI respectively).
  */
void SSP_LoopbackCmd(FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));
    
	if (NewState == ENABLE)
	{
	    /* Enable the loop back mode. */
		LPC_SSP->CR1 |= CR1_LBM_ENABLE;
	}
	else
	{
	    /* Disable the loop back mode. */
		LPC_SSP->CR1 &= CR1_LBM_DISABLE;
	}
}

/**
  * @brief  Enables or disables the SSP slave output function.
  *
  * @param  NewState: new state of the SSP slave output (ENABLE or DISABLE). 
  * @retval None   
  *
  * If disabled, it blocks the SSP controller from driving the 
  * transmit data line (MISO).
  *
  * Note: This function is available only when SSP is in Slave mode.
  */
void SSP_SlaveOutputCmd(FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));
	
	if (NewState == ENABLE)
	{
	    /* Enable the slave output */
		LPC_SSP->CR1 &= CR1_SOD_ENABLE; 
	}
	else
	{
	    /* Disable the slave output */
		LPC_SSP->CR1 |= CR1_SOD_DISABLE;    
	}
}

/**
  * @brief  Transmits a Data through the SSP peripheral.
  *
  * @param  Data : Data to be transmitted (must be 16 or 8-bit long,
  * 		  	   this depends on SSP data size configured).
  * @retval None
  */
void SSP_SendData(uint16_t Data)
{
    /* Write in the DR register the data to be sent */
	LPC_SSP->DR = Data;
}

/**
  * @brief  Returns the most recent received data by the SSP peripheral. 
  *
  * @param  None.
  * @retval The value of the received data (16-bit long).
  */
uint16_t SSP_ReceiveData(void)
{
    /* Return the data in the DR register */
	return ((uint16_t) (LPC_SSP->DR));
}

/**
  * @brief  Checks whether the specified SSP flag is set or not.
  *
  * @param  SSP_STATUS: specifies the SSP flag to check. 
  *         This parameter can be one of the following values:
  *             @arg SSP_STATUS_TFE: TX FIFO empty flag.
  *             @arg SSP_STATUS_TNF: TX FIFO not empty flag.
  *             @arg SSP_STATUS_RNE: RX buffer not empty flag.
  *             @arg SSP_STATUS_RFF: RX buffer empty flag.
  *             @arg SSP_STATUS_BSY: SSP busy flag.
  * @retval New State of specified SSP status flag (SET or RESET).
  */
FlagStatus SSP_GetStatus (uint8_t SSP_STATUS)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_SSP_GET_STATUS(SSP_STATUS));
      
    /* Check the status of the specified SSP flag */
	return (((LPC_SSP->SR & SSP_STATUS) == (uint8_t)RESET) ? RESET : SET);
}

/**
  * @brief  Configures the data size for the SSP.
  *
  * @param  SSP_DATASIZE: specifies the SSP data size.
  *         This parameter can be one of the following values:
  *             @arg SSP_DATASIZE_4: Set data size to 4bit
  *             ...
  *             @arg SSP_DATASIZE_16: Set data size to 16bit
  * @retval None
  */
void SSP_DataSizeConfig(uint16_t SSP_DATASIZE)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_SSP_DATASIZE(SSP_DATASIZE));
    
    /* Clear DSS bit */
    LPC_SSP->CR0 &= (uint16_t)~SSP_DATASIZE_16;
    /* Set new DSS bit value */
    LPC_SSP->CR0 |= SSP_DATASIZE;
}

/**
  * @brief  Enables or disables the specified SSP interrupts.
  *
  * @param  SSP_INT: specifies the SSP interrupt source to be enabled or disabled. 
  *         This parameter can be one or combination of the following values:
  *             @arg SSP_INT_RXOVERRUN: Receive Overrun interrupt mask
  *             @arg SSP_INT_RXTIMEOUT: Receive TimeOut interrupt mask
  *             @arg SSP_INT_RXHALFFULL: Rx FIFO is at least half full interrupt mask
  *             @arg SSP_INT_TXHALFEMPTY: Tx FIFO is at least half empty interrupt mask  
  * @param  NewState: new state of the specified SSP interrupt.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SSP_IntConfig(uint32_t SSP_INT, FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));
    CHECK_PARAM(PARAM_SSP_CONFIG_INT(SSP_INT));
      
	if (NewState == ENABLE)
	{
	    /* Enable the selected SSP interrupt */
		LPC_SSP->IMSC |= SSP_INT;
	}
	else
	{
	    /* Disable the selected SSP interrupt */
		LPC_SSP->IMSC &= (~SSP_INT) & 0x0F;
	}
}

/**
  * @brief  Check whether the specified Raw interrupt status flag is set or not.
  *
  * @param  SSP_INT: specifies the SSP interrupt source to check. 
  *         This parameter can be one of the following values:
  *             @arg SSP_INT_RXOVERRUN: Receive Overrun interrupt
  *             @arg SSP_INT_RXTIMEOUT:  Receive TimeOut interrupt
  *             @arg SSP_INT_RXHALFFULL:  Rx FIFO is at least half full interrupt
  *             @arg SSP_INT_TXHALFEMPTY:  Tx FIFO is at least half empty interrupt
  * @retval New State of specified interrupt status flag (SET or RESET).
  *
  * Note: Enabling/Disabling specified interrupt in SSP peripheral does not
  * 		effect to Raw Interrupt Status flag.
  */
IntStatus SSP_GetRawIntStatus(uint8_t SSP_INT)
{
    /* Check the parameters */
    CHECK_PARAM (PARAM_SSP_GET_INT(SSP_INT));
    
    /* Check the raw status of the specified SSP interrupt */
	return ((LPC_SSP->RIS & SSP_INT) ? SET : RESET);
}

/**
  * @brief  Get Interrupt Status register (MIS) value
  *
  * @param  None
  * @retval Interrupt Status register (MIS) value.
  *         This parameter can be any combination of the following values:
  *             @arg SSP_INT_RXOVERRUN: Receive Overrun interrupt
  *             @arg SSP_INT_RXTIMEOUT:  Receive TimeOut interrupt
  *             @arg SSP_INT_RXHALFFULL:  Rx FIFO is at least half full interrupt
  *             @arg SSP_INT_TXHALFEMPTY:  Tx FIFO is at least half empty interrupt            
  */
uint32_t SSP_GetIntStatusReg(void)
{
	return (LPC_SSP->MIS);
}

/**
  * @brief  Checks whether the specified SSP interrupt has occurred or not.
  *
  * @param  SSP_INT: specifies the SSP interrupt source to check. 
  *         This parameter can be one of the following values:
  *             @arg SSP_INT_RXOVERRUN: Receive Overrun interrupt
  *             @arg SSP_INT_RXTIMEOUT:  Receive TimeOut interrupt
  *             @arg SSP_INT_RXHALFFULL:  Rx FIFO is at least half full interrupt
  *             @arg SSP_INT_TXHALFEMPTY:  Tx FIFO is at least half empty interrupt
  * @retval New State of specified interrupt status flag (SET or RESET).
  *
  * Note: Enabling/Disabling specified interrupt in SSP peripheral effects
  * 			to Interrupt Status flag.
  */
IntStatus SSP_GetIntStatus (uint8_t SSP_INT)
{
    /* Check the parameters */
    CHECK_PARAM (PARAM_SSP_GET_INT(SSP_INT));
  
    /* Check the status of the specified SSP interrupt */
	return ((LPC_SSP->MIS & SSP_INT) ? SET : RESET);
}

/**
  * @brief  Clear specified interrupt pending bit in SSP peripheral.
  *
  * @param  SSP_INT: specifies the SSP interrupt pending bit to clear.
  *         This parameter can be one of the following values:
  *             @arg SSP_INT_RXOVERRUN: clears the RXOVERRUN interrupt.
  *             @arg SSP_INT_RXTIMEOUT: clears the RXTIMEOUT interrupt.
  * @retval None
  *
  * Note: This function clears only RXOVERRUN and RXTIMEOUT intetrrupt pending bit.
  */
void SSP_ClearIntPendingBit(uint8_t SSP_INT)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_SSP_CLEAR_INT(SSP_INT));
    
    /* Clear specified interrupt pending */  
	LPC_SSP->ICR = SSP_INT;
}

/**
  * @brief  Enables or disables the SSP DMA interface.
  *
  * @param  SSP_DMAReq: specifies the SSP DMA transfer request to be enabled or disabled. 
  *         This parameter can be any combination of the following values:
  *             @arg SSP_DMAREQ_TX: Tx buffer DMA transfer request
  *             @arg SSP_DMAREQ_RX: Rx buffer DMA transfer request
  * @param  NewState: new state of the selected SSP DMA transfer request.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SSP_DMACmd(uint16_t SSP_DMAReq, FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_SSP_DMAREQ(SSP_DMAReq));
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));  
  
    if (NewState != DISABLE)
    {
        /* Enable the selected SSP DMA requests */
        LPC_SSP->DMACR |= SSP_DMAReq;
    }
    else
    {
        /* Disable the selected SSP DMA requests */
        LPC_SSP->DMACR &= (uint16_t)~SSP_DMAReq;
    }
}

/**
 * @}
 */

#endif // _SSP

/**
 * @}
 */
  
/* --------------------------------- End Of File ------------------------------ */
