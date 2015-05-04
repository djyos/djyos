//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:安全钩子
//作者：lst
//版本：V1.0.0
//文件描述:安全钩子是必须在开机后尽可能快地执行的部分代码
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include "cpu_peri.h"

const ufast_t tg_IntUsed[] =
{
    CN_INT_LINE_UART0,
    CN_INT_LINE_UART1,
    CN_INT_LINE_RTC,
    CN_INT_LINE_WDT,
    CN_INT_LINE_ENET,
    CN_INT_LINE_TIMER0,
    CN_INT_LINE_TIMER1,
    CN_INT_LINE_TIMER2,
    CN_INT_LINE_TIMER3,
    CN_INT_LINE_SSP0,
    CN_INT_LINE_SSP1
};

const ufast_t tg_IntUsedNum = sizeof(tg_IntUsed)/sizeof(ufast_t);

//----配置全部IO口--------------------------------------------------------------
//功能：除必须的外，全部配置成初始态，各功能口由相应的功能模块自行定义。
//参数：无
//返回：无
//------------------------------------------------------------------------------
void gpio_init(void)
{
}

void critical(void)
{
#if 0   //ARM版本认为程序加载过程无误
    //定时器初始化
#if(CN_CFG_SYSTIMER == 1)
    extern void ModuleInstall_HardTimer(void);
    ModuleInstall_HardTimer();
#endif
#if(CN_CFG_WDT == 1)
    //  初始化硬件看门狗
    extern bool_t WDT_LpcInitial(u32 setcycle);
    WDT_LpcInitial(1000);

    // 看门狗组件HAL喂狗
    //  开启中断并注册相关的服务函数
    bool_t WdtHal_BootStart(u32 bootfeedtimes);
    WdtHal_BootStart(20);
#endif
#endif
}
