// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_hal.c
// 模块描述: HAL需要使用到的接口函数
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 2/20.2017
// =============================================================================

#include <stdio.h>
#include <stddef.h>
#include "cpu_peri.h"
#include <os.h>
#include <board-config.h>
#include <stm32f7xx_hal_tim.h>


//==============================================================================
//HAL_RCC会用到此量，因为DJYOS使用自己的时钟初始化过程，因此，在使用HAL库的时候，有些HAL
//内部会调用该量，因此需此处赋值
//例如ETH网络驱动中，stm32f7xx_hal_eth.c中会调用
uint32_t SystemCoreClock = CN_CFG_MCLK;			//HAL_RCC会用到此量

//HAL里面会调用该函数作时间操作，因此，若用到HAL库，则需要提供该函数提供延时
//return ms
uint32_t HAL_GetTick(void)
{
	static u32 sTicks = 0;
	static u16 sTimCntLast = 0;
	u16 TimCntCurrent;
	u32 TimMs;
	TIM_HandleTypeDef TimHandle;

	TimHandle.Instance = TIM6;
	TimCntCurrent = (__HAL_TIM_GET_COUNTER(&TimHandle))&0xFFFF;//0.5ms
    if( sTimCntLast > TimCntCurrent)
    {
        sTicks += 32768;    //时钟分辨率0.5mS，16bit是32768mS
    }
    sTimCntLast = TimCntCurrent;
	return sTicks + (TimCntCurrent>>1);
}

void HAL_SuspendTick(void)
{
	TIM_HandleTypeDef TimHandle;
	TimHandle.Instance = TIM6;
	HAL_TIM_Base_Stop(&TimHandle);
}
void HAL_ResumeTick(void)
{
	TIM_HandleTypeDef TimHandle;
	TimHandle.Instance = TIM6;
	HAL_TIM_Base_Start(&TimHandle);
}


//此处用TIM6
void HAL_TickInit(void)
{
	u32 uwPrescalerValue;
	TIM_HandleTypeDef TimHandle;

	__HAL_RCC_TIM6_CLK_ENABLE();

	uwPrescalerValue = ((CN_CFG_MCLK/4) / 1000) - 1;	//Counter Clock = 2K
	TimHandle.Instance = TIM6;
	TimHandle.Init.Period        = 0xFFFFFFFF;
	TimHandle.Init.Prescaler     = uwPrescalerValue;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode   = TIM_COUNTERMODE_UP;
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

	HAL_TIM_Base_DeInit(&TimHandle);
	HAL_TIM_Base_Init(&TimHandle);
	HAL_TIM_Base_Start(&TimHandle);
}


