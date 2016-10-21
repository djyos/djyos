/**************************************************************************//**
 * $Id: lpc12xx_crc.c 435 2010-09-02 13:15:05Z nxp21428 $
 *
 * @file    lpc12xx_crc.c
 * @brief    Contains all functions support for GPIO firmware library on LPC12xx.
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
/** @addtogroup CRC
 * @{
 */

/* Includes ------------------------------------------------------------------- */

/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _CRC
/* Private Variables -------------------------------------------------------- */
/** @defgroup   CRC_Private_Variables
 * @{
 */
static CRC_CFG_Type CRC_configration;

/**
 * @}
 */

/* Public Functions ----------------------------------------------------------- */
/** @addtogroup CRC_Private_Functions
 * @{
 */

/**
  * @brief  CRC initialization function
  *
  * @param  CRC_mode:  pointer to a CRC_CFG_Type structure that 
  *         contains the configuration information for the CRC peripheral.
  * @retval None   
  */
void CRC_Init( CRC_CFG_Type* CRC_mode )
{
    CHECK_PARAM(PARAM_CRC_POLY(CRC_mode->poly));
    CHECK_PARAM(PARAM_CRC_BIT_RVS_WR(CRC_mode->bit_wr));
    CHECK_PARAM(PARAM_CRC_CMPL_WR(CRC_mode->cmpl_wr));
    CHECK_PARAM(PARAM_CRC_CMPL_SUM(CRC_mode->cmpl_sum));
    CHECK_PARAM(PARAM_CRC_BIT_RVS_SUM(CRC_mode->bit_sum));

    LPC_CRC->MODE = ((CRC_mode->poly)|(CRC_mode->bit_wr)|( CRC_mode->cmpl_wr)|\
                   (CRC_mode->bit_sum)|(CRC_mode->cmpl_sum));
    LPC_CRC->SEED = CRC_mode->seed;
    
    CRC_configration.poly = (*CRC_mode).poly;
    CRC_configration.bit_wr = (*CRC_mode).bit_wr;
    CRC_configration.cmpl_wr = (*CRC_mode).cmpl_wr;    
    CRC_configration.cmpl_sum = (*CRC_mode).cmpl_sum;
    CRC_configration.bit_sum = (*CRC_mode).bit_sum;
    CRC_configration.seed = (*CRC_mode).seed;

    return;
}

/*
  * @brief     Get configuration the CRC according to the specified parameters
  * @param  CRC_mode:  pointer to a CRC_CFG_Type structure that 
  *               contains the configuration information for the CRC peripheral.

  * @return    None
  */
void CRC_GetConfig(CRC_CFG_Type *CRC_mode)
{
    *CRC_mode = CRC_configration;
}


/**
  * @brief  Write data to CRC
  *
  * @param  data:  input data. 
  * @param  length:  crc bit length,should be 8,16 or 32.   
  * @retval None   
  */

void CRC_WriteData( uint8_t * data, uint8_t bitlen )
{
    uint16_t * data_word = (uint16_t *)data;
    uint32_t * data_dword = (uint32_t *)data;

    CHECK_PARAM(( bitlen == 8 ) || ( bitlen == 16 ) || ( bitlen == 32 ));

    switch ( bitlen )
    {
        case 32:
        	LPC_CRC->WR_DATA_DWORD = *data_dword;
        	break;
        case 16:
        	LPC_CRC->WR_DATA_WORD = *data_word;
        	break;
        case 8:
       		LPC_CRC->WR_DATA_BYTE = *data;
        	break;
        default:
        	break;
    }
    return;
}

/**
  * @brief  Get CRC Sum value.
  *
  * @param  none.   
  * @retval CRC Sum value   
  */
uint32_t CRC_GetSum( void )
{
    return (LPC_CRC->SUM);
}

/**
  * @brief   Get CRC ID for device validation.
  *
  * @param  none.   
  * @retval CRC ID number 
  */
uint32_t CRC_GetID( void )
{
    return (LPC_CRC->ID);
}

/**
 * @}
 */

#endif /* _CRC */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
