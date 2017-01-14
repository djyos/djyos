#include <cpu_peri_gpio.h>
#include <djyos.h>
#include <stdint.h>
#include <stdio.h>
#include <systime.h>

#include "../../../../../djysrc/bsp/arch/arm/arm32_stdint.h"



// =============================================================================
// 功能: LED TEST
// 参数:无
// 返回: 无
// =============================================================================
ptu32_t Led_Test(void)
{
	for(;;)
	{
		GPIO_SettoHigh(GPIO_F, PIN10);
		GPIO_SettoLow(GPIO_B, PIN7);
		Djy_EventDelay(125*mS);
		GPIO_SettoLow(GPIO_F, PIN10);
		GPIO_SettoHigh(GPIO_B, PIN7);
		Djy_EventDelay(125*mS);
	}
	return 0;
}
// =============================================================================
// 功能:led初始化
// 参数:无
// 返回: 无
// =============================================================================
void led_init(void)
{
	u16 evtt_led;
	GPIO_PowerOn(GPIO_F);
	GPIO_PowerOn(GPIO_B);
	GPIO_CfgPinFunc(GPIO_F,PIN10,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);
    GPIO_CfgPinFunc(GPIO_B,PIN7 ,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_NONE);

    evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
    						Led_Test,NULL,1000,"Led_Test");
    //事件的两个参数暂设为0,如果用shell启动,可用来采集shell命令行参数
    if(evtt_led!=CN_EVTT_ID_INVALID)
    	Djy_EventPop(evtt_led,NULL,0,NULL,0,0);

}

// =============================================================================
// 功能: 主函数
// 参数:无
// 返回: 无
// =============================================================================





void djy_main(void)
{
	led_init();

	printf("Run Mode:App.\r\n");
    for(;;)
    {
    	Djy_EventDelay(1000*mS);
    }


}


