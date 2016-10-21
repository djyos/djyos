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
#include "cpu_peri_gpio.h"


#define CLRDOG_PORT		(0)		//CLRDOG_L p0.30
#define CLRDOG_PIN		(30)

#define WDT_FEED_CYCLE  (1600*1000)	//1.6S

// =============================================================================
// 功能：板上看门狗喂狗函数,max706的喂狗时间是1.6s,根据手册，喂狗是边沿触发
// 参数：无
// 返回：无
// =============================================================================
bool_t BrdWdt_FeedDog(void)
{
//	printk("wdtR\r\n");
    if( GPIO_DataGet( CLRDOG_PORT) & ( 1<<CLRDOG_PIN) )
    {
        GPIO_SetToLow( CLRDOG_PORT, ( 1<<CLRDOG_PIN));
    }
    else
    {
        GPIO_SetToHigh( CLRDOG_PORT, ( 1<<CLRDOG_PIN));
    }

    return true;
}

// =============================================================================
// 功能：板上看门狗芯片初始化
// 参数：无
// 返回：无
// =============================================================================
void BrdWdt_ModuleInit(void)
{
	GPIO_CfgPinFunc(CLRDOG_PORT,CLRDOG_PIN,0);
	GPIO_CfgPinMode(CLRDOG_PORT,CLRDOG_PIN,CN_GPIO_MODE_OD_PULLUP);
	GPIO_SetToOut(CLRDOG_PORT,(1 << CLRDOG_PIN));			//配置为输出

	// LPC17xx 该p0.29和p0.30配置为相同输出
	GPIO_CfgPinFunc(0,29,0);
	GPIO_CfgPinMode(CLRDOG_PORT,CLRDOG_PIN,CN_GPIO_MODE_OD_PULLUP);
	GPIO_SetToOut(0,(1 << 29));			//配置为输出

	BrdWdt_FeedDog();

	WdtHal_RegisterWdtChip("board_Wdt",WDT_FEED_CYCLE,BrdWdt_FeedDog,NULL,NULL);
}







