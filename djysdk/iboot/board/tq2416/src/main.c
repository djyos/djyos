


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


ptu32_t djy_main(void)
{

	IAP_LoadAPPFromFile();

	g_bRunModeFlag=0x12345678;
	printf("Run Mode:Iboot.\r\n");
	Led1_Init();
	while(1)
	{
		Led1_On();
		Djy_EventDelay(200000);
		Led1_Off();
		Djy_EventDelay(200000);
	}
}

