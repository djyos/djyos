/**************************************************************************//**
 * $Id: debug_frmwrk.c 453 2010-09-07 03:32:11Z nxp21428 $
 *
 * @file      debug_frmwrk.c
 * @brief     Contains some utilities that used for debugging through UART
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
/** @addtogroup DEBUG_FRMWRK
 * @{
 */
 
/* Includes ----------------------------------------------------------------- */
/* If this source file built with example, the LPC12xx FW library configuration
 * file in each example directory ("lpc12xx_libcfg.h") must be included,
 * otherwise the default FW library configuration file must be included instead
 */

#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc12xx_libcfg.h"
#else
#include "lpc12xx_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */


#ifdef _DBGFWK
/** @defgroup DEBUG_FRMWRK_Private_Functions
 * @{
 */
static void db_putchar (uint8_t ch);

/**
  * @brief     Puts a character to UART port
  * @param  ch  Character to put. 
  * @retval   None   
  */

static void db_putchar (uint8_t ch)
{
    UART_Send(DEBUG_UART_PORT, &ch, 1, UART_BLOKING_TIMEOUT);
    while (!(UART_GetLineStatus(DEBUG_UART_PORT) & UART_LS_TX_EMPTY_ALL));
}

/**
  * @brief    Initialize Debug frame work through initializing UART port
  * @param None
  * @return  None
  **/
void DB_Init(void)
{
    UART_CFG_Type UARTConfigStruct;
    IOCON_PIO_CFG_Type PIO_mode;


    /*
     * Initialize UART pin connect
     */
    IOCON_StructInit(&PIO_mode);
    PIO_mode.type = DEBUG_UART_RXD;
    IOCON_SetFunc(&PIO_mode);
    PIO_mode.type = DEBUG_UART_TXD;
    IOCON_SetFunc(&PIO_mode);    
    if(DEBUG_UART_PORT == LPC_UART0){ 
        SYS_ResetPeripheral(SYS_PRESETCTRL_UART0_RST,DISABLE);
        SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART0, ENABLE); 
        SYS_SetUART0ClockDiv(1);
    }else{
        SYS_ResetPeripheral(SYS_PRESETCTRL_UART1_RST,DISABLE);
        SYS_ConfigAHBCLK(SYS_AHBCLKCTRL_UART1, ENABLE); 
        SYS_SetUART1ClockDiv(1);
    }
    UART_Init(DEBUG_UART_PORT);

    UART_GetConfig(DEBUG_UART_PORT, &UARTConfigStruct);
    UARTConfigStruct.baudrate = 9600;   //115200;
    UARTConfigStruct.databits = UART_CFG_DATABIT_8;

    // Initialize UART0 peripheral with given to corresponding parameter
    UART_SetConfig(DEBUG_UART_PORT, &UARTConfigStruct);
    UART_ConfigTXD(DEBUG_UART_PORT, ENABLE);
}

/**
  * @brief    Puts a string to UART port
  * @param str  string to put
  * @return  None
  */
void DB_PutChar(uint8_t ch)
{
   db_putchar(ch);
}

/**
  * @brief    Puts a string to UART port
  * @param str  string to put
  * @return  None
  */
void DB_PutStr(const void *str)
{
    uint8_t *s = (uint8_t *) str;

    while (*s){
        db_putchar(*s++);
    }
}

/**
  * @brief    Puts a decimal number to UART port
  * @param decnum   Decimal number (8-bit long)
  * @return  None
  */
void DB_PutDec(uint32_t decnum)
{
    uint8_t     buff[16];
    uint32_t    i = 0;

    if(decnum == 0){
        buff[0] = 0;
        i = 1;
    }
    while(decnum){
        buff[i++] = decnum % 10;
        decnum = decnum / 10;
    }

    while( i > 0 ){
        db_putchar((buff[i-1] + '0'));
        if(i == 0)break;
        i--;
    }
}

/**
  * @brief    Puts a hex number to UART port
  * @param hexnum   Hex number
  * @param type        0:no limit; 8: 8bit; 16: 16bit; 32:32bit 
  * @return None
  */
void DB_PutHex (uint32_t hexnum, uint8_t type)
{
    uint8_t     buff[16];   
    uint32_t    i = 0;
    uint32_t    k = 1;

    db_putchar('0');
    db_putchar('x');

    while(hexnum){
        buff[i++] = hexnum % 16;            
        hexnum = hexnum / 16;
    }
    i--;

    if(type == 8){
       k = 1;
    }else if(type == 16){
       k = 3;
    }else if(type == 32){
       k = 7;
    }else{
       k = i;   
    }   

    while( (k-i) > 0 ){
        db_putchar('0');
        k--;
    }

    while( (i+1) > 0 ){
        if( buff[i] <= 9 ){
            db_putchar(buff[i] + '0');          
        }else{
            db_putchar((buff[i]-10) + 'A');         
        }   
        if(i == 0)break;
        i--;
    }   
}

/**
  * @brief    Puts a hex number to UART port
  * @param str  String to put 
  * @param buf  number buffer
  * @return  None
  */
void DB_Printf (const void *str, uint32_t* buf)
{
    uint8_t *s = (uint8_t *) str;
    uint8_t tmp1,tmp2 = 0;
    
    while (*s){
        tmp1 = *s++;
        if(tmp1 == '%'){
            if(*s == 'd'){
                DB_PutDec(buf[tmp2]);
                tmp2++; 
                *s++;
            }else if(*s == 'x'){
                DB_PutHex(buf[tmp2], 0);
                tmp2++;
                *s++;
            }else if(*s == 'c'){
                db_putchar(buf[tmp2]);
                tmp2++;
                *s++;
            }else{
                db_putchar(tmp1);
            }   
        }else{
           db_putchar(tmp1);
        }
    }   
}

/**
  * @brief   Get a character to UART port
  * @param ch  Character to put
  * @return None
  */
uint8_t DB_GetChar (void)
{
    uint8_t tmp = 0;
    uint8_t len = 0;
    while(len==0)
    {
        len = UART_Receive(DEBUG_UART_PORT, &tmp, 1, UART_BLOKING_TIMEOUT);
    }
    
    return(tmp);
}

/**
 * @}
 */

#endif /* _DEBUG_FRMWRK_ */
/**
 * @}
 */
  
/* --------------------------------- End Of File ------------------------------ */

