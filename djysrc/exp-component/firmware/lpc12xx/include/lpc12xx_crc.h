/**************************************************************************//**
 * $Id: lpc12xx_crc.h 546 2010-09-16 03:26:59Z cnh20509 $
 *
 * @file     lpc12xx_crc.h
 * @brief    Contains all macro definitions and function prototypes
 *              support for CRC firmware library on lpc12xx.
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
/** @defgroup CRC
 * @ingroup LPC1200CMSIS_FwLib_Drivers
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __LPC12xx_CRC_H
#define __LPC12xx_CRC_H

/* Includes ------------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"


#ifdef __cplusplus
extern "C"
{
#endif


/* Public Types --------------------------------------------------------------- */
/** @defgroup CRC_Public_Types
 * @{
 */
typedef struct {
    uint8_t     poly;       /* !< CRC polynomial  
                                                 This parameter can be a value of @ref CRC_polynomial_type */
    uint8_t     bit_wr;     /* !< CRC bit order reverse   
                                                 This parameter can be a value of @ref CRC_bit_order */
    uint8_t     cmpl_wr;    /* !< CRC data complement   
                                                 This parameter can be a value of @ref CRC_data_complement */
    uint8_t     bit_sum;    /* !< CRC bit sum   
                                                 This parameter can be a value of @ref CRC_bit_sum */
    uint8_t     cmpl_sum;   /* !< CRC sum complement   
                                                 This parameter can be a value of @ref CRC_sum_complement */ 
    uint32_t    seed;       /* !< seed value */
} CRC_CFG_Type;

/**
  * @}
  */

/* Public Functions ----------------------------------------------------------- */
/** @defgroup CRC_Public_Functions
 * @{
 */

/* CRC style ------------------------------- */
void CRC_Init( CRC_CFG_Type* CRC_mode );
void CRC_GetConfig(CRC_CFG_Type *CRC_mode);
void CRC_WriteData( uint8_t * data, uint8_t bitlen );
uint32_t CRC_GetSum( void );
uint32_t CRC_GetID( void );

/**
 * @}
 */


/* Public Macros -------------------------------------------------------------- */
/** @defgroup CRC_Public_Macros
  * @{
  */

/* @brief CRC_polynomial_type definition
  * @{
  */

#define CRC_POLY_CCITT ((uint8_t)0x00)   /* CRC32 */
#define CRC_POLY_16    ((uint8_t)0x01)   /* Crc_16 */
#define CRC_POLY_32    ((uint8_t)0x02)   /* CRC-CCITT */
#define PARAM_CRC_POLY(type)    (( type == CRC_POLY_32)|| \
                                 ( type == CRC_POLY_16)|| \
                                 ( type == CRC_POLY_CCITT))
/**
  * @}
  */   
  
/*@brief CRC_bit_order definitions 
  * @{
  */
#define CRC_BIT_RVS_WR_NO    ((uint8_t)0x00)  
#define CRC_BIT_RVS_WR_EN    ((uint8_t)0x04)  
#define PARAM_CRC_BIT_RVS_WR(type)    (( type == CRC_BIT_RVS_WR_NO)|| \
                                       ( type == CRC_BIT_RVS_WR_EN))
/**
  * @}
  */   
  
/*@brief CRC_data_complement definitions 
  * @{
  */
#define CRC_CMPL_WR_NO    ((uint8_t)0x00)  
#define CRC_CMPL_WR_EN    ((uint8_t)0x08)  
#define PARAM_CRC_CMPL_WR(type)   (( type == CRC_CMPL_WR_NO)|| \
                                   ( type == CRC_CMPL_WR_EN))
/**
  * @}
  */   
  
/*@brief CRC_bit_sum definitions 
  * @{
  */

#define CRC_BIT_RVS_SUM_NO    ((uint8_t)0x00)   
#define CRC_BIT_RVS_SUM_EN    ((uint8_t)0x10)  
#define PARAM_CRC_BIT_RVS_SUM(type)   (( type == CRC_BIT_RVS_SUM_NO)|| \
                                       ( type == CRC_BIT_RVS_SUM_EN))

/**
  * @}
  */   
  
/*@brief CRC_sum_complement definitions 
  * @{
  */

#define CRC_CMPL_SUM_NO    ((uint8_t)0x00)   
#define CRC_CMPL_SUM_EN    ((uint8_t)0x20)  
#define PARAM_CRC_CMPL_SUM(type)      (( type == CRC_CMPL_SUM_NO)|| \
                                       ( type == CRC_CMPL_SUM_EN))
/**
  * @}
  */ 

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* __LPC12xx_CRC_H */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
