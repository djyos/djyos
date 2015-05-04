//----------------------------------------------------
//Copyright (C), 2005-2009,  lst.
//版权所有 (C), 2005-2009,   lst.
//所属模块:  CPU初始化
//作者：     lst
//版本：      V1.0.0
//初始版本完成日期：2009-08-05
//文件描述:  cpu时钟初始化
//其他说明:  无
//修订历史:
//2. ...
//1. 日期: 2009-08-05
//   作者: lst
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "cpu_peri.h"

struct scb_reg volatile * const pg_scb_reg  = (struct scb_reg *)0xe000ed00;

#define cn_fcco             (CN_CFG_MCLK* 3)    //  PLL频率(275Mhz~550Mhz)
#define cn_pll_n            1               //  1~256
#define cn_pll_m            ((cn_fcco / CN_CFG_EXTCLK) * (cn_pll_n) / 2)   //6~512

void Init_Cpu(void)
{
    extern   uint32_t   msp_top[ ];
    //flash设为最慢
    pg_sysctrl_reg->flashcfg = ((0x05ul << 12) & (~(0x003f))) | 0x003a;

    __set_PSP(msp_top);
    __set_MSP(msp_top);
    __set_PRIMASK(1);
    __set_FAULTMASK(1);
    __set_CONTROL(0);
    switch(pg_scb_reg->CPUID)
    {
        case cn_revision_r0p0:
            break;    //市场没有版本0的芯片
        case cn_revision_r1p0:
            pg_scb_reg->CCR |= 1<<bo_scb_ccr_stkalign;
            break;
        case cn_revision_r1p1:
            pg_scb_reg->CCR |= 1<<bo_scb_ccr_stkalign;
            break;
        case cn_revision_r2p0:break;    //好像没什么要做的
    }


//外设时钟选择，在此全部设为相同的数值，该值在config-prj.h中定义
#if (CN_CFG_MCLK/4 == CN_CFG_PCLK)          //外设时钟=1/4主时钟
    pg_sysctrl_reg->pclksel0 = 0x00000000;
    pg_sysctrl_reg->pclksel1 = 0x00000000;
#endif

#if (CN_CFG_MCLK/2 == CN_CFG_PCLK)          //外设时钟=1/2主时钟

    pg_sysctrl_reg->pclksel0 = 0xAAAAAAAA;
    pg_sysctrl_reg->pclksel1 = 0xAAAAAAAA;
#endif

#if (CN_CFG_MCLK == CN_CFG_PCLK)          //外设时钟=主时钟

    pg_sysctrl_reg->pclksel0 = 0x55555555;
    pg_sysctrl_reg->pclksel1 = 0x55555555;
#endif
    pg_sysctrl_reg->clkoutcfg = 0;

    //断开PLL，避免调速错误
    if ((pg_sysctrl_reg->pll0stat >> 24) == 1)
    {
        pg_sysctrl_reg->pll0con = 1;        //pll使能，但断开连接
        pg_sysctrl_reg->pll0feed = 0xAA;
        pg_sysctrl_reg->pll0feed = 0x55;
    }
    pg_sysctrl_reg->pll0con = 0;            //pll禁止，且断开连接
    pg_sysctrl_reg->pll0feed = 0xAA;
    pg_sysctrl_reg->pll0feed = 0x55;
    while(pg_sysctrl_reg->pll0stat & (3 << 24)) {    }

     //设置主要时钟
     //  Enable mainOSC,1MHz~20MHz
    pg_sysctrl_reg->scs = (pg_sysctrl_reg->scs & 0x04) | 0x20;

    while ((pg_sysctrl_reg->scs & ((u32)1 << 6)) == 0) {    }

    pg_sysctrl_reg->clksrcsel = 0x01;  //select main OSC as the PLL clock source
    pg_sysctrl_reg->pll0cfg   = (((cn_pll_n - 1) << 16) | (cn_pll_m - 1));
    pg_sysctrl_reg->pll0feed  = 0xAA;
    pg_sysctrl_reg->pll0feed  = 0x55;

    pg_sysctrl_reg->pll0con   = 1;        //pll使能，但断开连接
    pg_sysctrl_reg->pll0feed  = 0xAA;
    pg_sysctrl_reg->pll0feed  = 0x55;

    while ((pg_sysctrl_reg->pll0stat & ((u32)1 << 24)) == 0) {    }

    pg_sysctrl_reg->cclkcfg = (cn_fcco / CN_CFG_MCLK) - 1;

    //  Check lock bit status
    while (((pg_sysctrl_reg->pll0stat & (1ul << 26)) == 0)) {   }

    while (((pg_sysctrl_reg->pll0stat & 0x00007FFF) != (cn_pll_m - 1)) &&
           (((pg_sysctrl_reg->pll0stat & 0x00FF0000) >> 16) != (cn_pll_n - 1)))
    {
    }

    pg_sysctrl_reg->pll0con  = 3;               //使能并连接pll
    pg_sysctrl_reg->pll0feed = 0xAA;
    pg_sysctrl_reg->pll0feed = 0x55;
    //等待直到时钟连接并锁定
    while ((pg_sysctrl_reg->pll0stat & ((u32)1 << 25)) == 0) {   }

// 设置存储器加速模块
#if CN_CFG_MCLK <= 20000000                         //使用1个CPU时钟
    pg_sysctrl_reg->flashcfg = (((u32)0x01 << 12) & (~(0x003f))) | 0x003a;
#endif

#if CN_CFG_MCLK > 20000000 && CN_CFG_MCLK <= 40000000   //使用2个CPU时钟
    pg_sysctrl_reg->flashcfg = (((u32)0x02 << 12) & (~(0x003f))) | 0x003a;
#endif

#if CN_CFG_MCLK > 40000000 && CN_CFG_MCLK <= 60000000   //使用3个CPU时钟
    pg_sysctrl_reg->flashcfg = (((u32)0x03 << 12) & (~(0x003f))) | 0x003a;
#endif

#if CN_CFG_MCLK > 60000000 && CN_CFG_MCLK <= 80000000   //使用4个CPU时钟
    pg_sysctrl_reg->flashcfg = (((u32)0x04 << 12) & (~(0x003f))) | 0x003a;
#endif

#if CN_CFG_MCLK > 80000000 && CN_CFG_MCLK <= 100000000  //使用5个CPU时钟
    pg_sysctrl_reg->flashcfg = (((u32)0x05 << 12) & (~(0x003f))) | 0x003a;
#endif
    Load_Preload();
    return;
}


