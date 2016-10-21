/**************************************************************************//**
 * $Id: lpc12xx_adc.c 539 2010-09-15 08:16:40Z cnh20509 $
 *
 * @file     lpc12xx_adc.c
 * @brief    Contains all functions support for ADC firmware library on LPC12xx.
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
/** @addtogroup ADC
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_adc.h"


/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _ADC

/** @defgroup ADC_Private_Macros
  * @{
  */

/* bit offset */
#define BIT_PDRUNCFG_ADC            4
#define BIT_SYSAHBCLKCTRL_ADC       14

#define BIT_ADCR_CLKDIV             8
#define BIT_ADCR_BURST              16
#define BIT_ADCR_PDN                21

/* CLKDIV bits mask in Reg. ADCR. */
#define CR_CLKDIV_MASK          ((uint32_t) 0x0000FF00)
/* START bits mask in Reg. ADCR. */
#define CR_START_MASK           ((uint32_t) 0x07000000)

/* max ADC clock feq. */
#define ADC_CLOCKRATE_MAX       (9000000UL)


#define SystemCoreClock 36000000
/**
 * @}
 */

 
/** @defgroup ADC_Private_Functions
 * @{
 */

/**
  * @brief  Enables or disables the Burst mode.
  *             
  * @param  NewState: new state of the burst mode.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_BurstCmd (FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState)); 
    
    if (NewState == ENABLE)
    {
        LPC_ADC->CR |= (1 << BIT_ADCR_BURST);   /* burst mode */
    }
    else
    {   
        LPC_ADC->CR &= ~(1 << BIT_ADCR_BURST);  /* software controlled */
    }
        
}

/**
  * @brief  Enables or disables the specified ADC channel.
  *
  * @param  ADC_CHANNEL: specifies ADC channle to be enabled or disabled. 
  *         This parameter can be one of the following values:
  *             @arg ADC_CHANNEL_0: ADC channel 0
  *             @arg ADC_CHANNEL_1: ADC channel 1
  *             @arg ADC_CHANNEL_2: ADC channel 2
  *             @arg ADC_CHANNEL_3: ADC channel 3
  *             @arg ADC_CHANNEL_4: ADC channel 4
  *             @arg ADC_CHANNEL_5: ADC channel 5
  *             @arg ADC_CHANNEL_6: ADC channel 6
  *             @arg ADC_CHANNEL_7: ADC channel 7              
  * @param  NewState: new state of the selected ADC channel.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_ChannelCmd (uint32_t ADC_CHANNEL, FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_ADC_CHANNEL(ADC_CHANNEL));
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState)); 
    
    if (NewState == ENABLE)
    {
        LPC_ADC->CR |= (1<<ADC_CHANNEL);
    }
    else
    {
        LPC_ADC->CR &= ~(1<<ADC_CHANNEL);
    }        
}

/**
  * @brief  Deinitializes the ADC peripheral.
  *
  * @param  None
  * @retval None    
  */
void ADC_DeInit(void)
{
    /* Put ADC on power down mode */
    LPC_ADC->CR &= ~ (1<<BIT_ADCR_PDN);
    
    /* Disable AHB clock to the ADC. */
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<BIT_SYSAHBCLKCTRL_ADC);  
    
    /* Enable Power down bit to the ADC block. */  
    LPC_SYSCON->PDRUNCFG |= 1<<BIT_PDRUNCFG_ADC;  
}

/**
  * @brief  Specified the start conversion on a falling or rising edge.
  *
  * @param  ADC_EDGE: specifies the edge type. 
  *         This parameter can be one of the following values:
  *             @arg ADC_EDGE_RISING
  *             @arg ADC_EDGE_FALLING
  * @retval None
  */
void ADC_EdgeConfig(uint32_t ADC_EDGE)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_ADC_EDGE(ADC_EDGE));
      
    if (ADC_EDGE == ADC_EDGE_RISING)
        LPC_ADC->CR &= ADC_EDGE_RISING;
    else
        LPC_ADC->CR |= ADC_EDGE_FALLING;
}

/**
  * @brief  Get the specified ADC channel conversion result.
  *
  * @param  ADC_CHANNEL: specifies ADC channle value. 
  *         This parameter can be one of the following values:
  *             @arg ADC_CHANNEL_0: ADC channel 0
  *             @arg ADC_CHANNEL_1: ADC channel 1
  *             @arg ADC_CHANNEL_2: ADC channel 2
  *             @arg ADC_CHANNEL_3: ADC channel 3
  *             @arg ADC_CHANNEL_4: ADC channel 4
  *             @arg ADC_CHANNEL_5: ADC channel 5
  *             @arg ADC_CHANNEL_6: ADC channel 6
  *             @arg ADC_CHANNEL_7: ADC channel 7              
  * @retval ADC converstion result.
  */
uint32_t ADC_GetData (uint8_t ADC_CHANNEL)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_ADC_CHANNEL(ADC_CHANNEL));

    return ((LPC_ADC->DR[ADC_CHANNEL] >> 6) & 0x3FF);         
} 

/**
  * @brief  Checks whether the specified ADC flag is set or not.
  *
  * @param  ADC_CHANNEL: specifies ADC channle value. 
  *         This parameter can be one of the following values:
  *             @arg ADC_CHANNEL_0: ADC channel 0
  *             @arg ADC_CHANNEL_1: ADC channel 1
  *             @arg ADC_CHANNEL_2: ADC channel 2
  *             @arg ADC_CHANNEL_3: ADC channel 3
  *             @arg ADC_CHANNEL_4: ADC channel 4
  *             @arg ADC_CHANNEL_5: ADC channel 5
  *             @arg ADC_CHANNEL_6: ADC channel 6
  *             @arg ADC_CHANNEL_7: ADC channel 7   
  * @param  ADC_FLAG: specifies the ADC flag to check. 
  *         This parameter can be one of the following values:
  *             @arg ADC_FLAG_OVERRUN: Overrun bit in burst mode.
  *             @arg ADC_FLAG_DONE: Done bit.
  * @retval New State of specified ADC flag (SET or RESET).
  */
FlagStatus ADC_GetStatus(uint8_t ADC_CHANNEL, uint32_t ADC_FLAG)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_ADC_CHANNEL(ADC_CHANNEL));
    CHECK_PARAM(PARAM_ADC_FLAG(ADC_FLAG));
    
    return ( (uint32_t)(LPC_ADC->DR[ADC_CHANNEL] & ADC_FLAG) ? SET : RESET);    
}

/**
  * @brief  Initializes the ADC peripheral.
  *
  * @param  ADC_ClockRate: Specify the clock rate (in Hz) for A/D converter,
  *                         should be no more than 9000000Hz.
  * @retval None 
  *
  * Note: Due to the integer division, the actual ADC clock rate might be not
  * same with the value (ADC_ClockRate) use specified.
  *
  * Actural ADC conversion rate is ADC_ClockRate / 36.
  * ADC will be on operational mode after ADC_Init().
  */
void ADC_Init(uint32_t ADC_ClockRate)
{ 
    uint32_t  adc_pclk;
    uint8_t clk_div;

    /* check the parameters */
    CHECK_PARAM (ADC_ClockRate <= ADC_CLOCKRATE_MAX);
    
    /* Disable Power down bit to the ADC block. */  
    LPC_SYSCON->PDRUNCFG &= ~(1<<BIT_PDRUNCFG_ADC);  
      
    /* Enable AHB clock to the ADC. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<BIT_SYSAHBCLKCTRL_ADC); 
    
    /* Clear register */
    LPC_ADC->CR = 0x0;
    
    /* The ADC clock is derived from the system clock. */
    adc_pclk = SystemCoreClock / LPC_SYSCON->SYSAHBCLKDIV;
    
    /* Calculate clock divider. */
    clk_div = (uint8_t)(adc_pclk / ADC_ClockRate); 
    
    /* Configure CLKDIV and put ADC in operational mode. */
    LPC_ADC->CR = (((clk_div-1) << BIT_ADCR_CLKDIV) & CR_CLKDIV_MASK ) | (1<<BIT_ADCR_PDN);
}

/**
  * @brief  Specifies which ADC channels generate interrupt(s).
  *
  * @param  ADC_INT: specifies the interrupt source. 
  *         This parameter can be one of the following values:
  *             @arg ADC_INT_CHANNEL0
  *             @arg ADC_INT_CHANNEL1
  *             @arg ADC_INT_CHANNEL2
  *             @arg ADC_INT_CHANNEL3
  *             @arg ADC_INT_CHANNEL4
  *             @arg ADC_INT_CHANNEL5
  *             @arg ADC_INT_CHANNEL6
  *             @arg ADC_INT_CHANNEL7
  *             @arg ADC_INT_GLOBAL              
  * @retval None
  */
void ADC_IntConfig (uint16_t ADC_INT, FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_ADC_INT(ADC_INT));
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState)); 
      
    if (NewState == ENABLE)            
        LPC_ADC->INTEN |= (1<<ADC_INT);
    else
        LPC_ADC->INTEN &= ~(1<<ADC_INT);
}

/**
  * @brief  Enables or disables the PowerDown mode.
  *             
  * @param  NewState: new state of the power down mode.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_PowerDownCmd (FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState)); 
    
    if (NewState == DISABLE)
    {
        LPC_ADC->CR |= (1 << BIT_ADCR_PDN);     /* Operational mode */
    }
    else
    {
        LPC_ADC->CR &= ~(1 << BIT_ADCR_PDN);    /* Power down mode */
    }        
}

/**
  * @brief  Start the ADC with sepecified mode.
  *
  * @param  ADC_START: specifies ADC start mode. 
  *         This parameter can be one of the following values:
  *             @arg ADC_START_CONTINUOUS: Continuous mode
  *             @arg ADC_START_NOW: Start conversion now
  *             @arg ADC_START_ON_CT16B0_CAP0: Start conversion when the edge selected by 
  *                                            bit 27 occurs on PIO0_2/SSEL/CT16B0_CAP0.
  *             @arg ADC_START_ON_CT32B0_CAP0: Start conversion when the edge selected by 
  *                                            bit 27 occurs on PIO1_5/DIR/CT32B0_CAP0.
  *             @arg ADC_START_ON_CT32B0_MAT0: Start conversion when the edge selected by 
  *                                            bit 27 occurs on CT32B0_MAT0.
  *             @arg ADC_START_ON_CT32B0_MAT1: Start conversion when the edge selected by 
  *                                            bit 27 occurs on CT32B0_MAT1.
  *             @arg ADC_START_ON_CT16B0_MAT0: Start conversion when the edge selected by 
  *                                            bit 27 occurs on CT16B0_MAT0.
  *             @arg ADC_START_ON_CT16B0_MAT1: Start conversion when the edge selected by                              
  *                                            bit 27 occurs on CT16B0_MAT1.              
  * @retval None
  */
void ADC_StartCmd (uint32_t ADC_START)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_ADC_START(ADC_START));
    
    LPC_ADC->CR &= ~CR_START_MASK;
    LPC_ADC->CR |= ADC_START;        
}


/**
 * @}
 */

#endif // _ADC

/**
 * @}
 */
  
/* --------------------------------- End Of File ------------------------------ */
