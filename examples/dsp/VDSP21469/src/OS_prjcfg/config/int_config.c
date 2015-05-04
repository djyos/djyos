//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:工程配置
//作者：lst
//版本：V1.0.0
//文件描述: 本文件罗列了具体工程所启用的中断，对使用了的每个中断，都会分配一个
//      struct int_line结构。这对于内存比较充足的系统来说，不成为负担，对这类系
//      统，把系统所有中断线都加上就可以了。
//          但对于内存非常有限且中断线众多的单片机来说，不管该中断是否被使用，都
//      分配一个struct int_line结构的话，内存表示亚历山大。实际工程中所用到的中
//      断线往往不多，在这里只把使用到了的中断线罗列一下就好。
//其他说明:
//修订历史:
//2. ...
//1. 日期: 20130907
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "os_inc.h"
#include "cpu_peri.h"
const ufast_t tg_int_used[] =
{
//internal chip interrupt---81,also means the interrupt number

 cn_int_line_IICDI     ,  /* Exception: Illegal Input Condition Detected */
 cn_int_line_SOVFI     ,  /* Exception: Stack overflow */
 cn_int_line_TMZHI     ,  /* Timer = 0 (high priority) */
 cn_int_line_SPERRI    ,  /* Exception: SPORT Error interrupt */
 cn_int_line_BKPI      ,  /* Exception: Hardware Breakpoint interrupt */
 cn_int_line_IRQ2I     ,  /* IRQ2- asserted */
 cn_int_line_IRQ1I     ,  /* IRQ1- asserted */
 cn_int_line_IRQ0I     ,  /* IRQ0- asserted */
 cn_int_line_DAII	   ,  /* Programmable Interrupt 0  (DAII) */
 cn_int_line_SPII      ,  /* Programmable Interrupt 1  (default: SPI Interrupt) */
 cn_int_line_GPTMR0I   ,  /* Programmable Interrupt 2  (default: General purpose IOP Timer0 Interrupt) */
 cn_int_line_SP1I      ,  /* Programmable Interrupt 3  (default: SPORT 1 Interrupt) */
 cn_int_line_SP3I      ,  /* Programmable Interrupt 4  (default: SPORT 3 Interrupt) */
 cn_int_line_SP5I      ,  /* Programmable Interrupt 5  (default: SPORT 5 Interrupt) */
 cn_int_line_SP0I      ,  /* Programmable Interrupt 6  (default: SPORT 0 Interrupt) */
 cn_int_line_SP2I      ,  /* Programmable Interrupt 7  (default: SPORT 2 Interrupt) */
 cn_int_line_SP4I      ,  /* Programmable Interrupt 8  (default: SPORT 4 Interrupt) */
 cn_int_line_EPDM0I    ,  /* Programmable Interrupt 9  (default: External port DMA0 Interrupt) */
 cn_int_line_GPTMR1I   ,  /* Programmable Interrupt 10 (default: General purpose IOP Timer1 Interrupt) */
 cn_int_line_LP0I      ,  /* Programmable Interrupt 11 (default: LinkPort 0 Interrupt) */
 cn_int_line_UARTI     ,  /* Programmable Interrupt 12 (default: UART Tx&Rx Interrupt) */
 cn_int_line_EPDM1I    ,  /* Programmable Interrupt 13 (default: External port DMA1 Interrupt) */
 cn_int_line_DPII	   ,  /* Programmable Interrupt 14 (DPII) */
 cn_int_line_MTMI      ,  /* Programmable Interrupt 15 (default: MTMDMA Interrupt) */
 cn_int_line_LP1I      ,  /* Programmable Interrupt 16 (default: LinkPort 1 Interrupt) */
 cn_int_line_P17I      ,  /* Programmable Interrupt 17 (reserve for USER define) */
 cn_int_line_TWII      ,  /* Programmable Interrupt 18 (default: I2C Interrupt) */
 cn_int_line_CB7I      ,  /* Exception: Circ. buffer 7 overflow */
 cn_int_line_CB15I     ,  /* Exception: Circ. buffer 15 overflow */
 cn_int_line_TMZLI     ,  /* Timer = 0 (low priority) */
 cn_int_line_FIXI      ,  /* Exception: Fixed-pt. overflow */
 cn_int_line_FLTOI     ,  /* Exception: fltg-pt. overflow */
 cn_int_line_FLTUI     ,  /* Exception: fltg-pt. underflow */
 cn_int_line_FLTII     ,  /* Exception: fltg-pt. invalid */
 cn_int_line_EMULI     ,  /* Exception: emulator interrupt low priority */
 cn_int_line_SFT0I     ,  /* user software int 0 */
 cn_int_line_SFT1I     ,  /* user software int 1 */
 cn_int_line_SFT2I     ,  /* user software int 2 */
 cn_int_line_SFT3I     ,  /* user software int 3 */
};

const ufast_t ufg_int_used_num = sizeof(tg_int_used)/sizeof(ufast_t);
//定义中断线控制数据结构
struct tagIntLine tg_int_table[sizeof(tg_int_used)/sizeof(ufast_t)];


