

#include "stddef.h"
#include <stdint.h>
#include <stdio.h>
#include <djyos.h>



extern u32 g_bRunModeFlag;

void Led1_Init(void)
{
	volatile u32 *gpbcon = (u32*)0x56000010;
	*gpbcon &= ~(3<<10);
	*gpbcon |= (1<<10);
}

void Led1_On(void)
{
	volatile u32 *gpbdat = (u32*)0x56000014;
	*gpbdat &= ~(1<<5);
}

void Led1_Off(void)
{
	volatile u32 *gpbdat = (u32*)0x56000014;
	*gpbdat |= (1<<5);
}

#include <IAP.h>
#include <cfg/Iboot_config.h>
// iboot needed function below
// iboot 所需要用到的引脚初始化
void IAP_GpioPinInit(void)
{
	//tq2416 的key1在上电时用于是否强制进入iboot
	//若按下key1再上电，则强制进入iboot模式
	volatile u32 *gpfcon = (u32*)0x56000050;

	*gpfcon &= ~(3<<14);
	*gpfcon |= (0<<14);			//input
}
// 是否有硬件强制（如某个跳线）进入iboot
bool_t IAP_IsForceIboot(void)
{
	volatile u32 *gpfdat = (u32*)0x56000054;
	u32 delay = 1000,i;

	IAP_GpioPinInit();
	for(i = 0; i < delay; i++)
	{

	}

	if(!(*gpfdat & (1<<7)))		//按下时，为低电平，不按下为高电平
	{
		return true;
	}
	return false;
}

ptu32_t djy_main(void)
{
	IAP_LoadAPPFromFile();

	extern void Sh_GetStatus(char *param);
	extern void Sh_GetRunMode(char *param);
	Sh_GetRunMode(NULL);
	Sh_GetStatus(NULL);
	printf(">>\r\n");
	Led1_Init();
	while(1)
	{
		Led1_On();
		Djy_EventDelay(1000000);
		Led1_Off();
		Djy_EventDelay(1000000);
	}
}

