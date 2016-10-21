/*
 * stdio_my.c
 *
 *  Created on: 2015年4月27日
 *      Author: zhangqf
 */
#include <stdarg.h>
#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdint.h"
#include "stddef.h"
#include "systime.h"        //todo : stdout标准化后删掉。
#include "driver.h"         //todo : stdout标准化后删掉。
#include "char_term.h"

#define CN_VNC_IOBUFLEN   0x100
extern s32 Uart_SendServiceDirectly(char *str);
extern s32 djy_vsprintf(char *buf, const char *fmt, va_list args,\
        s32 (*strputfunc)(char *str));


void rfbLogPerror(const char *fmt, ...)
{
    va_list args;
    char printbuffer[CN_VNC_IOBUFLEN];

    va_start (args, fmt);
    djy_vsprintf(printbuffer, fmt, args,(s32 (*)(char *str))Uart_SendServiceDirectly);
    va_end (args);

    Uart_SendServiceDirectly(printbuffer);
    printk("\n\r");

    return ;
}
void rfbLog(const char *fmt, ...)
{
    va_list args;
    char printbuffer[CN_VNC_IOBUFLEN];

    va_start (args, fmt);

    djy_vsprintf(printbuffer, fmt, args,(s32 (*)(char *str))Uart_SendServiceDirectly);

    va_end (args);

    Uart_SendServiceDirectly(printbuffer);
    printk("\n\r");
    return ;
}
void rfbErr(const char *fmt, ...)
{
    va_list args;
    char printbuffer[CN_VNC_IOBUFLEN];
    va_start (args, fmt);
    djy_vsprintf(printbuffer, fmt, args,(s32 (*)(char *str))Uart_SendServiceDirectly);
    va_end (args);
    Uart_SendServiceDirectly(printbuffer);
    printk("\n\r");
    return ;
}

