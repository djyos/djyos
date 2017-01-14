#include <cpu_peri_gpio.h>
#include <djyos.h>
#include <stdint.h>
#include <stdio.h>
#include <systime.h>
#include "shell.h"
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
		Djy_EventDelay(500*mS);
		GPIO_SettoLow(GPIO_F, PIN10);
		GPIO_SettoHigh(GPIO_B, PIN7);
		Djy_EventDelay(500*mS);
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
static u8 debug_test=false;

static bool_t __debugOn(char *param)
{
    debug_test = true;
    return true;
}

static bool_t __debugOff(char *param)
{
    debug_test = false;
    return true;
}

static struct ShellCmdTab  gEthRawDebug[] =
{
    {
        "debugon",
        __debugOn,
        "usage:debugon",
        NULL
    },
    {
        "debugoff",
        __debugOff,
        "usage:debugoff",
        NULL
    }
};

#define CN_ETHCAN_DEBUGNUM  ((sizeof(gEthRawDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gEthRawDebugCmdRsc[CN_ETHCAN_DEBUGNUM];

static bool_t shell_test(void)
{
    bool_t result;

    result = Sh_InstallCmd(gEthRawDebug,gEthRawDebugCmdRsc,CN_ETHCAN_DEBUGNUM);

    return result;
}




void djy_main(void)
{
	led_init();

	printf("Run Mode:Iboot.\r\n");

//	iic_test( );

    for(;;)
    {

    	Djy_EventDelay(1000*mS);
    }


}


