/**************************************************************************//**
 * $Id: debug_frmwrk.h 453 2010-09-07 03:32:11Z nxp21428 $
 *
 * @file     debug_frmwrk.h
 * @brief    Contains some utilities that used for debugging through UART
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
/** @defgroup DEBUG_FRMWRK
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEBUG_FRMWRK_H_
#define DEBUG_FRMWRK_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "lpc12xx_uart.h"

/* Public Macros -------------------------------------------------------------- */
/** @defgroup DEBUG_FRMWRK_Public_Macros
 * @{
 */

#ifdef _USED_UART0_DEBUG_LOC0
#define DEBUG_UART_PORT LPC_UART0
#define DEBUG_UART_RXD  IOCON_UART_RXD0_LOC0
#define DEBUG_UART_TXD  IOCON_UART_TXD0_LOC0
#else
#ifdef _USED_UART0_DEBUG_LOC1
#define DEBUG_UART_PORT LPC_UART0
#define DEBUG_UART_RXD  IOCON_UART_RXD0_LOC1
#define DEBUG_UART_TXD  IOCON_UART_TXD0_LOC1
#else
#ifdef _USED_UART1_DEBUG_LOC1
#define DEBUG_UART_PORT LPC_UART1
#define DEBUG_UART_RXD  IOCON_UART_RXD1_LOC1
#define DEBUG_UART_TXD  IOCON_UART_TXD1_LOC1
#else 
#ifdef _USED_UART1_DEBUG_LOC2
#define DEBUG_UART_PORT LPC_UART1
#define DEBUG_UART_RXD  IOCON_UART_RXD1_LOC2
#define DEBUG_UART_TXD  IOCON_UART_TXD1_LOC2
#else
#define DEBUG_UART_PORT LPC_UART1
#define DEBUG_UART_RXD  IOCON_UART_RXD1_LOC0
#define DEBUG_UART_TXD  IOCON_UART_TXD1_LOC0
#endif
#endif
#endif
#endif

/**
  * @}
  */ 

/* Public Functions ----------------------------------------------------------- */
/** @defgroup DEBUG_FRMWRK_Public_Functions
 * @{
 */

void DB_Init(void);
void DB_PutChar(uint8_t ch);
void DB_PutStr(const void *str);
void DB_PutDec(uint32_t decnum);
void DB_PutHex (uint32_t hexnum, uint8_t type);
void DB_Printf (const void *str, uint32_t* buf);
uint8_t DB_GetChar (void);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_FRMWRK_H_ */
/**
 * @}
 */
 
/* --------------------------------- End Of File ------------------------------ */


