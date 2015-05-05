/**************************************************************************//**
 * $Id: lpc12xx_wdt.c 380 2010-08-27 01:46:24Z cnh00813 $
 *
 * @file     lpc12xx_wdt.c
 * @brief    Contains all functions support for WDT firmware library on LPC12xx.
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
/** @addtogroup WDT
 * @{
 */

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_wdt.h"

/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _WDT

/** @defgroup WDT_Private_Macros
  * @{
  */

#define IS_WDT_WINDOW_VALUE(VALUE) (((VALUE) <= 0x00FFFFFF) && ((VALUE) >= 0x100))
#define IS_WDT_WDWARNINT_VALUE(VALUE)   ((VALUE) <= 0x3FF)

#define WDT_IRC_OSC_CLK  12000000UL     /* Internal RC oscillator frequency */

/* Macro defines for WDT Control register  */
#define WDT_WDMOD_WDEN	      ((uint8_t)(1<<0))   /* WDT enable bit */
#define WDT_WDMOD_WDRESET     ((uint8_t)(1<<1))   /* WDT reset enable bit */
#define WDT_WDMOD_WDTOF	      ((uint8_t)(1<<2))   /* WDT time out flag bit */
#define WDT_WDMOD_WDINT	      ((uint8_t)(1<<3))   /* WDT interrupt flag bit */
#define WDT_WDMOD_WDPROTECT   ((uint8_t)(1<<4))   /* WDT update mode bit */
#define WDT_WDMOD_WDLOCKCLK   ((uint8_t)(1<<5))   /* WDT clock lock bit */
#define WDT_WDMOD_WDLOCKDP    ((uint8_t)(1<<6))   /* WDT deep pd disable bit */
#define WDT_WDMOD_WDLOCKEN    ((uint8_t)(1<<7))   /* WDT enable lockout bit */
#define WDT_WDMOD_MASK	      ((uint8_t)(0xFF))   /* WDT mode register mask */

/* Macro defines for WDT Clock Source Selection register */
#define WDT_WDCLKSEL_WDLOCK   ((uint32_t)(1UL<<31))    /*Clock source lock bit*/
#define WDT_WDCLKSEL_MASK     (uint32_t)(0x80000003)   /* register mask */

/* Watchdog timer constant register mask */
#define WDT_WDTC_MASK	      (uint32_t)(0x00FFFFFF) 
/* Watchdog feed sequence register mask */ 
#define WDT_WDFEED_MASK       (uint32_t)(0x000000FF) 
/* Watchdog warning interrupt register mask */ 
#define WDT_WDWARNINT_MASK    (uint32_t)(0x000003FF)  
/* Watchdog window register mask */
#define WDT_WDWINDOW_MASK     (uint32_t)(0x00FFFFFF)  

/**
 * @}
 */

/** @defgroup WDT_Private_Functions
 * @{
 */

/**
  * @brief Update WDT timeout value and feed.
  *
  * @param TimeOut: TimeOut value to be updated.
  * @retval None 
  */ 
void WDT_UpdateTimeOut ( uint32_t TimeOut)
{
    WDT_SetTimeOut(TimeOut);
    WDT_Feed();
}

/**
  * @brief Update WDT timeout clock value and feed.
  *
  * @param TimeOutClk: TimeOut clock value to be updated.
  * @retval None 
  */ 
void WDT_UpdateTimeOutClk ( uint32_t TimeOutClk)
{
    WDT_SetTimeOutClk(TimeOutClk);
    WDT_Feed();
}

/**
  * @brief Set WDT time out value and WDT mode.
  *
  * @param TimeOut: value of time-out for WDT (us). 
  * @retval Status of Set value,  SUCCESS or ERROR.   
  */ 
uint8_t WDT_SetTimeOut (uint32_t TimeOut)
{
    uint32_t clk_wdt = 0;
    uint32_t tempval = 0;
    uint32_t ClkSrc = 0;
   
    ClkSrc = (LPC_WDT->WDCLKSEL) & WDT_WDCLKSEL_MASK;   
    /* watchdog oscillator as watchdog clock source */
    if (ClkSrc ==WDT_CLKSEL_WDOSC )         
    {
        switch ((LPC_SYSCON->WDTOSCCTRL >> 5) & 0x0F) 
	{
            case 0:  clk_wdt =  400000; break;
            case 1:  clk_wdt =  500000; break;
            case 2:  clk_wdt =  800000; break;
            case 3:  clk_wdt = 1100000; break;
            case 4:  clk_wdt = 1400000; break;
            case 5:  clk_wdt = 1600000; break;
            case 6:  clk_wdt = 1800000; break;
            case 7:  clk_wdt = 2000000; break;
            case 8:  clk_wdt = 2200000; break;
            case 9:  clk_wdt = 2400000; break;
            case 10: clk_wdt = 2600000; break;
            case 11: clk_wdt = 2700000; break;
            case 12: clk_wdt = 2900000; break;
            case 13: clk_wdt = 3100000; break;
            case 14: clk_wdt = 3200000; break;
            case 15: clk_wdt = 3400000; break;
        }
        clk_wdt /= ((LPC_SYSCON->WDTOSCCTRL & 0x1F) << 1) + 2;
    }
    else
   {	
       clk_wdt = WDT_IRC_OSC_CLK;        /* iRC osc. as watchdog clock source */
    	}	
    /*  Calculate TC in WDT  */
    tempval  = (((clk_wdt) / WDT_US_INDEX) * (TimeOut/4));
    /* Check if it valid  */
    if ((tempval >= WDT_TIMEOUT_MIN) && (tempval <= WDT_TIMEOUT_MAX))
    {
        LPC_WDT->TC = tempval;
        return	SUCCESS;
    }
    
    return ERROR;
}


/**
  * @brief Set WDT time out in clock value.
  *
  * @param TimeOutClk: clock value of time-out for WDT. 
  * @retval Status of Set value,  SUCCESS or ERROR.   
  */ 
uint8_t WDT_SetTimeOutClk (uint32_t TimeOutClk)
{
    /* Check if it valid  */
    if ((TimeOutClk >= WDT_TIMEOUT_MIN) && (TimeOutClk <= WDT_TIMEOUT_MAX))
    {
    	LPC_WDT->TC = TimeOutClk;
        return	SUCCESS;
    }
    
    return ERROR;
}

/**
  * @brief Deinitializes the WDT registers to their default reset values.
  *
  * @param None
  * @retval None    
  */
void WDT_DeInit(void)
{
	
    /* Disable clock for WDT */
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1<<15);
}

/**
  * @brief Initializes the WDT peripheral according to the specified 
  *        parameters in the WDT_InitStruct.
  *
  * @param WDT_InitStruct: pointer to a WDT_InitTypeDef structure that 
  *        contains the configuration information for the WDT peripheral.
  * @retval None  
  */
void WDT_Init (WDT_InitTypeDef* WDT_InitStruct)
{
    uint32_t regVal;
	
    CHECK_PARAM(PARAM_WDT_CLKSEL_OPT(WDT_InitStruct->ClkSel));
    CHECK_PARAM(PARAM_WDT_MODE_OPT(WDT_InitStruct->Mode));
    CHECK_PARAM(IS_WDT_WDWARNINT_VALUE(WDT_InitStruct->WarningCompareValue));
    CHECK_PARAM(IS_WDT_WINDOW_VALUE(WDT_InitStruct->WindowValue));

    /* Select WDT clock source */
    LPC_WDT->WDCLKSEL &=  ~WDT_WDCLKSEL_MASK;	
    LPC_WDT->WDCLKSEL = WDT_InitStruct->ClkSel;   

    switch(WDT_InitStruct->ClkSel)
    {
	/* make sure IRC clock powered */
	case WDT_CLKSEL_IRCOSC:
            LPC_SYSCON->PDRUNCFG &= ~(0x1<<0);     
    	    break;
	/* make sure WD osc clock powered */
	case WDT_CLKSEL_WDOSC:
            LPC_SYSCON->PDRUNCFG &= ~(0x1<<6);     
      	    break;
    }
    /* Enable WDT clock */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<15);
    
    /* Config WDT mode */
    if (WDT_InitStruct->Mode == WDT_MODE_RESET)
    {
    	LPC_WDT->MOD |= WDT_WDMOD_WDRESET;
    }

    /* config timerout value  */
    if (WDT_InitStruct->TimeOutOption == WDT_TIMEOUT_TICKVAL)
    {
      	WDT_SetTimeOutClk(WDT_InitStruct->TimeOutValue);

    }
    else
    {
      	WDT_SetTimeOut(WDT_InitStruct->TimeOutValue);
    }

    WDT_Feed();	

    /* config warning interrupt compare value  */
    LPC_WDT->WARNINT = (WDT_InitStruct->WarningCompareValue)&WDT_WDWARNINT_MASK;

    /* config  clock lock function */
    if (WDT_InitStruct->LockClk)	
    {
        regVal = LPC_WDT->MOD;
        regVal |= WDT_WDMOD_WDLOCKCLK;
        LPC_WDT->MOD = regVal;
    }
    /* config WDT deep power-down mode disable or not */
    if (WDT_InitStruct->LockDP)	
    {
        regVal = LPC_WDT->MOD;
        regVal |= WDT_WDMOD_WDLOCKDP;
        LPC_WDT->MOD = regVal;
    }
    /* config  WDEN lock or not */
    if (WDT_InitStruct->LockWDEN)	
    {
        regVal = LPC_WDT->MOD;
        regVal |= WDT_WDMOD_WDLOCKEN;
        LPC_WDT->MOD = regVal;
    }
    /* config  clock source lock or not */
    if (WDT_InitStruct->LockClkSel)	
    {
        regVal = LPC_WDT->WDCLKSEL;
        regVal |= WDT_WDCLKSEL_WDLOCK;
        LPC_WDT->WDCLKSEL = regVal;
    }
    /* config  WDT update mode */
     if (WDT_InitStruct->SetProtect)	
    {
        regVal = LPC_WDT->MOD;
        regVal |= WDT_WDMOD_WDPROTECT;
        LPC_WDT->MOD = regVal;
    }

}

/**
  * @brief Fills each WDT_InitStruct member with its default value.
  *
  * @param WDT_InitStruct : pointer to a WDT_InitTypeDef structure 
  *        which will be initialized.
  * @retval None
  */
void WDT_StructInit(WDT_InitTypeDef* WDT_InitStruct)
{
    /* Reset WDT init structure parameters values */
    WDT_InitStruct->ClkSel= WDT_CLKSEL_IRCOSC;
    WDT_InitStruct->Mode= WDT_MODE_INT_ONLY;
    WDT_InitStruct->TimeOutOption= WDT_TIMEOUT_TICKVAL;
    WDT_InitStruct->TimeOutValue= WDT_TIMEOUT_MAX;
    WDT_InitStruct->WarningCompareValue= 0;
    WDT_InitStruct->WindowValue= 0xFFFFFF;

    WDT_InitStruct->LockClk=0;
    WDT_InitStruct->LockDP=0;
    WDT_InitStruct->LockWDEN=0;
    WDT_InitStruct->LockClkSel=0;
    WDT_InitStruct->SetProtect=0;
}



/**
  * @brief Start WDT activity.
  *
  * @param None
  * @retval None   
  */ 
void WDT_Start(void)
{
    /* Enable watchdog  */
    LPC_WDT->MOD |= WDT_WDMOD_WDEN;
    WDT_Feed();
}

/**
  * @brief Stop WDT activity.
  *
  * @param None
  * @retval None   
  */ 
void WDT_Stop(void)
{
    /* Stop watchdog  */
    LPC_WDT->MOD &= ~WDT_WDMOD_WDEN;
    WDT_Feed();
}

/**
  * @brief Setting Watchdog update mode to protect.
  *
  * @param None
  * @retval None 
  *
  * After set protect, The watchdog reload value (WDTC) can be changed only
  * after the counter is below the value of WDWARNINT and WDWINDOW.  
  */ 
void WDT_SetProtect(void)
{
    LPC_WDT->MOD |= WDT_WDMOD_WDPROTECT;

}

/**
  * @brief enable Watchdog clock lock.
  *
  * @param None
  * @retval None 
  *
  * The bit can be written to only once to set it.
  * When this bit has been set, it can only be cleared through resetting chip.  
  * Before setting clock lock bit, the user must enable either WD osc. or IRC or both in 
  * all three power configuration register. 
  */ 
void WDT_SetLockClk(void)
{
    LPC_WDT->MOD |= WDT_WDMOD_WDLOCKCLK;
}


/**
  * @brief disable Watchdog enter into deep power down.
  *
  * @param None
  * @retval None 
  *
  * The bit can be written to only once to set it.
  * When this bit has been set, it can only be cleared through resetting chip.  
  */ 
void WDT_SetLockDP(void)
{
    LPC_WDT->MOD |= WDT_WDMOD_WDLOCKDP;
}

/**
  * @brief Lockout Watchdog enable.
  *        When set, any subsequent writes to WDEN bit will be blocked.
  * @param None
  * @retval None 
  *
  * The bit can be written to only once to set it.
  * When this bit has been set, it can only be cleared through resetting chip.  
  */ 
void WDT_SetLockWDEN(void)
{
    LPC_WDT->MOD |= WDT_WDMOD_WDLOCKEN;
}

/**
  * @brief Lock CLOCK source of watchdog timer.
  *        When set, WDCLKSEL register can not be modified.
  * @paramNone 
  * @retval None 
  *
  * The bit can not be cleared by software.
  */ 
void WDT_SetLockClkSel(void)
{
    LPC_WDT->WDCLKSEL |= WDT_WDCLKSEL_WDLOCK;

}

/**
  * @brief Setting Watchdog warning interrupt register value.
  *
  * @param CompareValue: warning interrupt compare value.
  * @retval None 
  *
  * Note: The value should be <=0x3FF. if a larger value, only the bottom 
  * 10bits are valid.
  */ 
void WDT_SetWarningIntValue(WDT_InitTypeDef* WDT_InitStruct)
{
    CHECK_PARAM(IS_WDT_WDWARNINT_VALUE(WDT_InitStruct->WarningCompareValue));
    LPC_WDT->WARNINT = (WDT_InitStruct->WarningCompareValue) & WDT_WDWARNINT_MASK;
}

/**
  * @brief Setting Watchdog window register value.
  *
  * @param WindowValue: WDT window value.
  * @retval None 
  *
  * Note: The WDWINDOW value should be <=0xFFFFFF. Value of WDWINDOW below 
  * 0x100 will make it impossible to ever feed the watchdog successfully. 
  */ 
void WDT_SetWindowValue(WDT_InitTypeDef* WDT_InitStruct)
{
    CHECK_PARAM(IS_WDT_WINDOW_VALUE(WDT_InitStruct->WindowValue));
    LPC_WDT->WINDOW = (WDT_InitStruct->WindowValue) & WDT_WDWINDOW_MASK;
}

/**
  * @brief Get the current value of WDT counter.
  *
  * @param None
  * @retval current value of WDT counter. 
  */ 
uint32_t WDT_GetWDTV(void)
{
    return LPC_WDT->TV;
}

/**
  * @brief Get WDT interrupt status.
  *
  * @param None
  * @retval Interrupt flag status of WDT. 
  */ 
IntStatus WDT_GetIntStatus (void)
{
    return (((LPC_WDT->MOD & WDT_WDMOD_WDINT) >>3) ? SET : RESET);
}


/**
  * @brief Get WDT Time out flag.
  *
  * @param None
  * @retval Time out flag status of WDT. 
  */ 
FlagStatus WDT_GetTimeOutFlag (void)
{
    return (((LPC_WDT->MOD & WDT_WDMOD_WDTOF) >>2) ? SET : RESET);
}

/**
  * @brief Clear WDT interrupt flag.
  *
  * @param None
  * @retval None 
  */ 
void WDT_ClrIntPendingBit (void)
{
    LPC_WDT->MOD |=WDT_WDMOD_WDINT;

}

/**
  * @brief Clear WDT Time out flag.
  *
  * @param None
  * @retval None 
  */ 
void WDT_ClrTimeOutFlag (void)
{
    LPC_WDT->MOD &=~WDT_WDMOD_WDTOF;
}



/**
  * @brief After set WDTEN, call this function to start Watchdog or reload 
  *        the Watchdog timer.
  *
  * @param None
  * @retval None 
  */ 
void WDT_Feed (void)
{
    /*  Disable irq interrupt */
    __disable_irq();
    LPC_WDT->FEED = 0xAA;
    LPC_WDT->FEED = 0x55;
    /* Then enable irq interrupt */
    __enable_irq();
}

/**
 * @}
 */

#endif // _WDT

/**
 * @}
 */
  
/* --------------------------------- End Of File ------------------------------ */
