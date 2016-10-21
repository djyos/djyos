/**************************************************************************//**
 * $Id$
 *
 * @file     lpc12xx_comp.c
 * @brief    Contains all functions support for Comparator firmware library on 
 *           LPC12xx.
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
/** @addtogroup Comparator
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_comp.h"


/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _COMP

/** @defgroup COMP_Private_Macros
  * @{
  */

/* Bit offset in registers */
#define BIT_SYSAHBCLKCTRL_COMP      20
#define BIT_PRESETCTRL_COMP         8

#define BIT_CMP_CMP0_VP_CTRL        8
#define BIT_CMP_CMP0_VM_CTRL        11
#define BIT_CMP_CMP1_VP_CTRL        14
#define BIT_CMP_CMP1_VM_CTRL        17
  
#define COMP0_ENABLE        ((uint32_t) 0x00000001)  // | 
#define COMP0_DISABLE       ((uint32_t) 0xFFFFFFFE)  // &
#define COMP1_ENABLE        ((uint32_t) 0x00000002)  // |
#define COMP1_DISABLE       ((uint32_t) 0xFFFFFFFD)  // &

#define CMP_INTSRC_MASK     ((uint32_t) 0x0000001C) 
#define CMP_SA0_MASK        ((uint32_t) 0x00000040)
#define CMP_SA1_MASK        ((uint32_t) 0x00000080)
#define CMP_INTCLR_MASK     ((uint32_t) 0x00100000)  
#define CMP_STAT0_MASK      ((uint32_t) 0x00200000)
#define CMP_STAT1_MASK      ((uint32_t) 0x00400000)
#define CMP_CMP0_VP_CTRL_MASK   ((uint32_t) 0x00000700)
#define CMP_CMP0_VM_CTRL_MASK   ((uint32_t) 0x00003800)
#define CMP_CMP1_VP_CTRL_MASK   ((uint32_t) 0x0001C000)
#define CMP_CMP1_VM_CTRL_MASK   ((uint32_t) 0x000E0000)

#define VLAD_DISABLE    ((uint8_t) 0xFE)    // &
#define VLAD_ENABLE     ((uint8_t) 0x01)    // |

#define BIT_VLAD_VSEL   1
#define VLAD_VOLTAGE_MASK   ((uint8_t) 0x7E)

/**
 * @}
 */

 
/** @defgroup COMP_Private_Functions
 * @{
 */

/**
  * @brief  Initializes the COMP peripheral.
  *
  * @param  None.
  * @retval None  
  */
void COMP_Init (void)
{  
    /* Comparator reset de-asserted */
    LPC_SYSCON->PRESETCTRL |= (1<<BIT_PRESETCTRL_COMP);  
         
    /* Enable clock to COMP */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<BIT_SYSAHBCLKCTRL_COMP);   
}

/**
  * @brief  Deinitializes the COMP peripheral registers to their default 
  *         reset values.
  *
  * @param  None
  * @retval None    
  */
void COMP_DeInit(void)
{
    /* Reset the SSP peripheral */
	LPC_SYSCON->PRESETCTRL &= ~(1<<BIT_PRESETCTRL_COMP);
	
	/* Disable clock for SSP peripheral */
	LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<BIT_SYSAHBCLKCTRL_COMP);
}

/**
  * @brief  Enables or disables the speicified comparator.
  *
  * @param  COMP_INDEX: Specifies the comparator index. 
  *         This parameter can be one of the following values:
  *             @arg COMP_INDEX_0: comparator 0.
  *             @arg COMP_INDEX_1: comparator 1.  
  * @param  NewState: new state of the specified COMP port. 
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None 
  */
void COMP_Cmd(uint32_t COMP_INDEX, FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_COMP_INDEX(COMP_INDEX));
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));
    
	if (NewState == ENABLE)
	{
	    if (COMP_INDEX == COMP_INDEX_0)
	        LPC_COMP->CMP |= COMP0_ENABLE;  /* Enable comparator 0 */
	    else
	        LPC_COMP->CMP |= COMP1_ENABLE;	/* Enable comparator 1 */       
	}
	else
	{
	    if (COMP_INDEX == COMP_INDEX_0)
	        LPC_COMP->CMP &= COMP0_DISABLE; /* Disable comparator 0 */
	    else
	        LPC_COMP->CMP &= COMP1_DISABLE;	/* Disable comparator 1 */    
	}
}

/**
  * @brief  Configures the interrupt source for both comparators.
  *  
  * @param  COMP_INTSRC: Specifies the interrupt source. 
  *         This parameter can be one of the following values:
  *             @arg COMP_INTSRC_LEVEL: level trigger.
  *             @arg COMP_INTSRC_RISINGEDGE: rising edge.
  *             @arg COMP_INTSRC_FALLINGEDGE: rising edge.
  *             @arg COMP_INTSRC_DOUBLEEDGE: double edge.  
  * @retval None 
  */
void COMP_IntConfig (uint32_t COMP_INTSRC)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_COMP_INTSRC(COMP_INTSRC));
    
    /* Clear Bit CMMPIS, CMPIEV and CMPBE. */
    LPC_COMP->CMP &= ~CMP_INTSRC_MASK;
    
    /* Config interrupt source */
    LPC_COMP->CMP |= COMP_INTSRC;      
}

/**
  * @brief  Selects the async or sync output of the speicified comparator.
  *
  * @param  COMP_INDEX: Specifies the comparator index. 
  *         This parameter can be one of the following values:
  *             @arg COMP_INDEX_0: comparator 0.
  *             @arg COMP_INDEX_1: comparator 1.  
  * @param  COMP_OUTPUT: Specifies the output. 
  *         This parameter can be one of the following values:
  *             @arg COMP_OUTPUT_ASYNC: async output.
  *             @arg COMP_OUTPUT_SYNC:  sync output.    
  * @retval None 
  *
  */
void COMP_OutputConfig (uint32_t COMP_INDEX, uint32_t COMP_OUTPUT)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_COMP_INDEX(COMP_INDEX));    
    CHECK_PARAM(PARAM_COMP_OUTPUT(COMP_OUTPUT));
    
    if (COMP_INDEX == COMP_INDEX_0)
    {
        if (COMP_OUTPUT == COMP_OUTPUT_ASYNC)
            LPC_COMP->CMP &= ~CMP_SA0_MASK;
        else
            LPC_COMP->CMP |= CMP_SA0_MASK;
    }
    else
    {
        if (COMP_OUTPUT == COMP_OUTPUT_ASYNC)
            LPC_COMP->CMP &= ~CMP_SA1_MASK;
        else
            LPC_COMP->CMP |= CMP_SA1_MASK;
    }       
}


/**
  * @brief  Clears interrupt pending bit in COMP peripheral.
  *
  * @param  None
  * @retval None
  */
void COMP_ClearIntPendingBit (void)
{    
    /* Clear interrupt pending */  
	LPC_COMP->CMP |= CMP_INTCLR_MASK;
}

/**
  * @brief  Checks whether the output status of the specified comparator is set 
  *         or not.
  *
  * @param  COMP_INDEX: Specifies the comparator. 
  *         This parameter can be one of the following values:
  *             @arg COMP_INDEX_0: comparator 0.
  *             @arg COMP_INDEX_1: comparator 1.   
  * @retval New State of the output status (SET or RESET). 
  */
FlagStatus COMP_GetOutputStatus (uint32_t COMP_INDEX) 
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_COMP_INDEX(COMP_INDEX));
    
    /* Check the output status of the specified comparator */
    if (COMP_INDEX == COMP_INDEX_0)
	    return (((LPC_COMP->CMP & CMP_STAT0_MASK) == (uint32_t)RESET) ? RESET : SET);
	else
	    return (((LPC_COMP->CMP & CMP_STAT1_MASK) == (uint32_t)RESET) ? RESET : SET);
	    
}

/**
  * @brief  Selects the input channel for COMP0 positive or negative port.
  *
  * @param  COMP_INPORT: Specifies the input port. 
  *         This parameter can be one of the following values:
  *             @arg COMP_INPORT_POSITIVE: positive port.
  *             @arg COMP_INPORT_NEGATIVE: negative port. 
  * @param  COMP0_INCHANNEL: Specifies the input channel: 
  *         This parameter can be one of the following values:
  *             @arg COMP0_INCHANNEL_VLD: voltage ladder output.
  *             @arg COMP0_INCHANNEL_P0IN0: ACMP0_I0, PIO0_19.   
  *             @arg COMP0_INCHANNEL_P0IN1: ACMP0_I1, PIO0_20.
  *             @arg COMP0_INCHANNEL_P0IN2: ACMP0_I2, PIO0_21.
  *             @arg COMP0_INCHANNEL_P0IN3: ACMP0_I3, PIO0_22.
  *             @arg COMP0_INCHANNEL_P1IN0: ACMP1_I0, PIO0_23.
  *             @arg COMP0_INCHANNEL_P1IN1: ACMP1_I1, PIO0_24.          
  *             @arg COMP0_INCHANNEL_BODBG: BOD 0.9V band gap.  
  * @retval None.
  */
void COMP0_InputConfig (uint32_t COMP_INPORT, uint32_t COMP0_INCHANNEL)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_COMP_INPORT(COMP_INPORT));    
    CHECK_PARAM(PARAM_COMP0_INCHANNEL(COMP0_INCHANNEL));
    
    if (COMP_INPORT == COMP_INPORT_POSITIVE)
    {
        LPC_COMP->CMP |= (COMP0_INCHANNEL << BIT_CMP_CMP0_VP_CTRL) & CMP_CMP0_VP_CTRL_MASK;
    }
    else
    {
        LPC_COMP->CMP |= (COMP0_INCHANNEL << BIT_CMP_CMP0_VM_CTRL) & CMP_CMP0_VM_CTRL_MASK;
    }
    
}

/**
  * @brief  Selects the input channel for COMP1 positive or negative port.
  *
  * @param  COMP_INPORT: Specifies the input port. 
  *         This parameter can be one of the following values:
  *             @arg COMP_INPORT_POSITIVE: positive port.
  *             @arg COMP_INPORT_NEGATIVE: negative port. 
  * @param  COMP0_INCHANNEL: Specifies the input channel: 
  *         This parameter can be one of the following values:
  *             @arg COMP1_INCHANNEL_VLD: voltage ladder output.
  *             @arg COMP1_INCHANNEL_P1IN0: ACMP1_I0, PIO0_23.   
  *             @arg COMP1_INCHANNEL_P1IN1: ACMP1_I1, PIO0_24.
  *             @arg COMP1_INCHANNEL_P1IN2: ACMP1_I2, PIO0_25.
  *             @arg COMP1_INCHANNEL_P1IN3: ACMP1_I3, PIO0_26.
  *             @arg COMP1_INCHANNEL_P0IN0: ACMP0_I0, PIO0_19.
  *             @arg COMP1_INCHANNEL_P0IN1: ACMP0_I1, PIO0_20.          
  *             @arg COMP1_INCHANNEL_BODBG: BOD 0.9V band gap.   
  * @retval None.
  */
void COMP1_InputConfig (uint32_t COMP_INPORT, uint32_t COMP1_INCHANNEL)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_COMP_INPORT(COMP_INPORT));    
    CHECK_PARAM(PARAM_COMP1_INCHANNEL(COMP1_INCHANNEL));
    
    if (COMP_INPORT == COMP_INPORT_POSITIVE)
    {
        LPC_COMP->CMP |= (COMP1_INCHANNEL << BIT_CMP_CMP1_VP_CTRL) & CMP_CMP1_VP_CTRL_MASK;
    }
    else
    {
        LPC_COMP->CMP |= (COMP1_INCHANNEL << BIT_CMP_CMP1_VM_CTRL) & CMP_CMP1_VM_CTRL_MASK;
    }
    
}

/**
  * @brief  Enables or disables the voltage ladder.
  *  
  * @param  NewState: new state of the voltage ladder. 
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None 
  */
void COMP_VolLadderCmd (FunctionalState NewState)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_FUNCTIONALSTATE(NewState));
    
    if (NewState == ENABLE)
    {
        LPC_COMP->VLAD |= VLAD_ENABLE;   /* Enalbes voltage ladder */
    }
    else
    {
        LPC_COMP->VLAD &= VLAD_DISABLE;  /* Disables voltage ladder */
    }
}

/**
  * @brief  Configures the voltage ladder.
  *  
  * @param  COMP_VLADREF: Specfies the reference voltage. 
  *         This parameter can be one of the following values:
  *         @arg COMP_VLADREF_CMP: VREF_CMP pin
  *         @arg COMP_VLADREF_VDD: Vdd(3V3) pin  
  * @param  MValue: Calculate the voltage ladder value:
  *         voltage = Vref * MValue / 31, (MValue=0, 1, ..., 31).   
  * @retval None 
  */
void COMP_VolLadderConfig (uint8_t COMP_VLADREF, uint8_t MValue)
{
    /* Check the parameters */
    CHECK_PARAM(PARAM_COMP_VLADREF(COMP_VLADREF));
    
    /* Clear VSEL and VLADREF */
    LPC_COMP->VLAD &= ~VLAD_VOLTAGE_MASK;
    
    LPC_COMP->VLAD |= (COMP_VLADREF) | ((MValue&0x1F) << BIT_VLAD_VSEL);        
}


/**
 * @}
 */

#endif // _COMP

/**
 * @}
 */
  
/* --------------------------------- End Of File ------------------------------ */
