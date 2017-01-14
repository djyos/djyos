#include "stdint.h"
#include "stdio.h"
#include "hmi-input.h"
#include "assert.h"
#include "systime.h"
#include "gkernel.h"
#include "cpu_peri_gpio.h"
#include "time.h"
#include "djyos.h"
#include "timer.h"
#include "lowpower.h"
#include "cpu_peri.h"
#include "lock.h"



//LDE define
#define LED1_ON   GPIO_SettoLow(CN_GPIO_C, (1<<9))
#define LED1_OFF  GPIO_SettoHigh(CN_GPIO_C,  (1<<9))
#define LED2_ON   GPIO_SettoLow(CN_GPIO_C, (1<<8))
#define LED2_OFF  GPIO_SettoHigh(CN_GPIO_C,  (1<<8))
#define LED3_ON   GPIO_SettoLow(CN_GPIO_C, (1<<4))
#define LED3_OFF  GPIO_SettoHigh(CN_GPIO_C,  (1<<4))
#define LED4_ON   GPIO_SettoLow(CN_GPIO_C, (1<<6))
#define LED4_OFF  GPIO_SettoHigh(CN_GPIO_C,  (1<<6))

//----LED_init-----------------------------------------------------------------
//功能：使能gpio端口设置引脚模式
//参数：void
//返回：void
//-----------------------------------------------------------------------------
void led_init(void)
{
    GPIO_PowerOn(CN_GPIO_C);
    GPIO_CfgPinFunc(CN_GPIO_C, 6, CN_GPIO_MODE_GPIO_OUT_PP_2Mhz);
    GPIO_CfgPinFunc(CN_GPIO_C, 4, CN_GPIO_MODE_GPIO_OUT_PP_2Mhz);
    GPIO_CfgPinFunc(CN_GPIO_C, 8, CN_GPIO_MODE_GPIO_OUT_PP_2Mhz);
    GPIO_CfgPinFunc(CN_GPIO_C, 9, CN_GPIO_MODE_GPIO_OUT_PP_2Mhz);
    LED1_OFF; LED2_OFF; LED3_OFF; LED4_OFF;
}

ptu32_t djy_main(void)
{
	printf("Run Mode:APP.\r\n");

	led_init();
	gdd_test();
	while(1)
	{
		Djy_EventDelay(500*mS);
		LED2_ON;
		Djy_EventDelay(500*mS);
		LED2_OFF;
    }

	return 0;
}



