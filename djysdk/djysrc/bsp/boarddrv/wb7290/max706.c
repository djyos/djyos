// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：brdwdt.c
// 模块描述: SR3200的看门狗，包括初始化和喂狗函数
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/04.2015
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "djyos.h"
#include "cpu_peri.h"
#include "int.h"

// BOARD WDT
// =============================================================================
static const Pin wdt_pin[] = {
		{PIO_PA26, PIOA, ID_PIOA, PIO_OUTPUT_1,    PIO_DEFAULT}
};
#define WDT_FEED_CYCLE  (1600*1000) //1.6S
static u32 sBootDogFeedTime = 0;
// =============================================================================
// 功能：板上看门狗喂狗函数,max706的喂狗时间是1.6s,根据手册，喂狗是边沿触发
// 参数：无
// 返回：无
// =============================================================================
bool_t BrdWdt_FeedDog(void)
{
    if( PIO_Get(wdt_pin) )
    {
        PIO_Clear(wdt_pin);
    }
    else
    {
        PIO_Set(wdt_pin);
    }

    return true;
}

bool_t BrdBoot_FeedEnd(void);
u32 RttFeedDog_Isr(ptu32_t intline)
{
	static u8 cnt = 0;

	if(RTT->RTT_SR & RTT_SR_RTTINC)
	{
		BrdWdt_FeedDog();
		if(cnt * 1000*1000 > sBootDogFeedTime)
		{
			BrdBoot_FeedEnd();
		}
		else
		{
			cnt ++;
		}
	}
	Int_ClearLine(intline);
	return 1;
}
bool_t BrdBoot_FeedStart(u32 bootfeedtime)
{
	u8 irqline = CN_INT_LINE_RTT;

	sBootDogFeedTime = bootfeedtime;

	PMC_EnablePeripheral(ID_RTT);
	RTT->RTT_MR = (32768 |  RTT_MR_RTTRST | RTT_MR_RTTINCIEN);

    Int_Register(irqline);
    Int_IsrConnect(irqline,RttFeedDog_Isr);
    Int_SettoReal(irqline);
    Int_ClearLine(irqline);
    Int_ContactLine(irqline);
	return true;
}

bool_t BrdBoot_FeedEnd(void)
{
	u8 irqline = CN_INT_LINE_RTT;

	RTT->RTT_MR = RTT_MR_RTTDIS;

    Int_CutLine(irqline);
    Int_IsrDisConnect(irqline);
    Int_UnRegister(irqline);
    PMC_DisablePeripheral(ID_RTT);
	return true;
}
// =============================================================================
// 功能：板上看门狗芯片初始化
// 参数：无
// 返回：无
// =============================================================================
void ModuleInstall_BrdWdt(void)
{
	PIO_Configure(wdt_pin, PIO_LISTSIZE(wdt_pin));

    BrdWdt_FeedDog();

    WdtHal_RegisterWdtChip("Wdt_MAX706",WDT_FEED_CYCLE,BrdWdt_FeedDog,
    		BrdBoot_FeedStart,BrdBoot_FeedEnd);
}







