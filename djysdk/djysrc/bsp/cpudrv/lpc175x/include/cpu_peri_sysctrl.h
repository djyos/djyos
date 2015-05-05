//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:中断模块
//作者：lst
//版本：V1.0.0
//文件描述: 定义中断号
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009--9-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_SYSCTRL_H__
#define __CPU_PERI_SYSCTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

struct st_sysctrl_reg
{
    vu32 flashcfg;               // flash accelerator module           
    u32 reserved0[31];
    vu32 pll0con;                // clocking and power control         
    vu32 pll0cfg;
    vu32 pll0stat;
    vu32 pll0feed;
    u32 reserved1[4];
    vu32 pll1con;
    vu32 pll1cfg;
    vu32 pll1stat;
    vu32 pll1feed;
    u32 reserved2[4];
    vu32 pcon;
    vu32 pconp;
    u32 reserved3[15];
    vu32 cclkcfg;
    vu32 usbclkcfg;
    vu32 clksrcsel;
    u32 reserved4[12];
    vu32 extint;                 // external interrupts                
    u32 reserved5;
    vu32 extmode;
    vu32 extpolar;
    u32 reserved6[12];
    vu32 rsid;                   // reset                              
    u32 reserved7[7];
    vu32 scs;                    // syscon miscellaneous registers     
    vu32 irctrim;                // clock dividers                     
    vu32 pclksel0;
    vu32 pclksel1;
    u32 reserved8[4];
    vu32 usbintst;               // usb device/otg interrupt register  
    u32 reserved9;
    vu32 clkoutcfg;              // clock output configuration         
 };

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_SYSCTRL_H__

