
//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合以下二条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、此三条件表列，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、此三条件表列，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
/*
 * app_main.c
 *
 *  Created on: 2014-5-28
 *      Author: Administrator
 */
#if 0
//#include "os_inc.h"
#include "cpu_peri.h"
#include "printf.h"
#include "string.h"
#include "string.h"

#define GPIO_PIN_MASK            0x1Fu
#define GPIO_PIN_NO(x)              (((1)<<(x & GPIO_PIN_MASK)))

ptu32_t Led1Task(void)
{
    /* make sure port A clock is on */
//      SIM_SCGC5 = SIM_SCGC5_PORTA_MASK ;

    /* Set PTA10 (connected to LED) for GPIO functionality  */
//      PORTA_PCR29=(0|PORT_PCR_MUX(1));
//      PORTA_PCR28=(0|PORT_PCR_MUX(1));
	PORT_MuxConfig(PORT_PORT_A,PORT_PIN(28),PORT_PINMUX_GPIO);
	PORT_MuxConfig(PORT_PORT_A,PORT_PIN(29),PORT_PINMUX_GPIO);

    /* Change PTA10 to output  */
//      GPIOA_PDDR=GPIO_PDDR_PDD(GPIO_PIN_NO(29) | GPIO_PIN_NO(28));
    GPIO_PinConfig(GPIO_PORT_A,GPIO_PIN(28),GPIO_FUNC_OUTPUT);
    GPIO_PinConfig(GPIO_PORT_A,GPIO_PIN(29),GPIO_FUNC_OUTPUT);

	while(1)
	{
		//Toggle the green LED on PTC9
//		GPIOA_PTOR|=GPIO_PDOR_PDO(GPIO_PIN_NO(28));
		GPIO_PinToggle(GPIO_PORT_A,GPIO_PIN(28));
		Djy_EventDelay(1000000);
	}
	return 0;
}

void LedTest(void)
{
	uint16_t evtt_led;
	evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
                                Led1Task,NULL,0x100,"led1 blink");

    Djy_EventPop(evtt_led,NULL,0,NULL,0,0,0);
}

ptu32_t port_int(ufast_t intline)
{
	GPIO_PinSet(GPIO_PORT_A,GPIO_PIN(29));
	Djy_EventDelay(1000000);

	PORT_IntFlagClear(PORT_PORT_E,PORT_PIN(26));
	return 0;
}

void GPIO_IntInit(void)
{
	//sw2
	PORT_IntConfig(PORT_PORT_E,PORT_PIN(26),IQC_INT_RISING_EDGE,PORT_PULL_UP);
	PORT_MuxConfig(PORT_PORT_E,PORT_PIN(26),PORT_PINMUX_GPIO);

    Int_SetClearType(CN_INT_LINE_PORTE,CN_INT_CLEAR_PRE);
    Int_IsrConnect(CN_INT_LINE_PORTE,port_int);
    Int_SettoAsynSignal(CN_INT_LINE_PORTE);
    Int_ClearLine(CN_INT_LINE_PORTE);
    Int_RestoreAsynLine(CN_INT_LINE_PORTE);
}

#include "timer.h"
TIMERSOFT_HANDLE timerhl;
void test_timer_isr(void)
{
	djy_printk("timer test\r\n");
}
void test_timer_isr0(void)
{
	djy_printk("timer test0\r\n");
}
void Timer_Test(void)
{
	timerhl = TimerSoft_Create("testtimer",2000*mS,test_timer_isr);
	TimerSoft_Ctrl(timerhl,EN_TIMER_STARTCOUNT,0);
	timerhl = TimerSoft_Create("testtimer0",3000*mS,test_timer_isr0);
	TimerSoft_Ctrl(timerhl,EN_TIMER_STARTCOUNT,0);
}
bool_t ddr_test(char *param)
{
	char *str_addr,*next_param;
	u32 addr,data=0;

	str_addr = Sh_GetWord(param,&next_param);
	if(str_addr == NULL)
	{
		djy_printk("参数错误！\r\n");
		return false;
	}
	addr = atol(str_addr);

	*((u32*)addr) = 0xaabb5566;
	data = *((u32*)addr);

	djy_printk("addr = 0x%8x, data = 0x%8x \r\n",addr,data);
	return true;
}

bool_t gui_test(void)
{
//    __lcd_hard_init();
//    __lcd_power_enable(0,1);
//    __lcd_envid_of(1);
//    color_demo_data16();
}

bool_t ddr2_test(void)
{
	u32 i,j = 0x08000000,addr;
	u8 result = 0;

	for(i = j; i < j + 0x7FF000;)
	{
		addr = i;
		*((u32*)addr) = 0xaabb5566;
		if( 0xaabb5566 != *((u32*)addr))
		{
			djy_printk("ddr2 test error!\r\n");
			result = 1;
			break;
		}
		i += 4;
	}

	if(result == 0)
		djy_printk("ddr2 test sucessed !\r\n");

	return true;
}

bool_t nand_erase(char *param)
{
	char *erase_block,*next_param;
	u32 block;

	erase_block = Sh_GetWord(param,&next_param);
	if(erase_block == NULL)
	{
		djy_printk("参数错误！\r\n");
		return false;
	}
	block = atol(erase_block);
	erase_block_nand(block);
	djy_printk("擦除完成！\r\n");
	return true;
}


extern bool_t CRT_Test(void);
extern bool_t nand_test(void);
extern bool_t nand_reset(void);

extern bool_t test_flash_file(void);
bool_t create_ptt(void)
{
	static created = 0;
	if(created == 0)
	{
		test_flash_file();
		created = 1;
	}
}
//------------------------------------------------------------------------------
//monitor测试表
//------------------------------------------------------------------------------
struct tagShellCmdTab  shell_monitordebug_table[] =
{
    {
    		"ddr_test",
    		ddr_test,
    		"ddr测试",
    		NULL
    },
    {
    		"gui_test",
    		gui_test,
    		"gui测试",
    		NULL
    },
    {
    		"crt_test",
    		CRT_Test,
    		"crt测试",
    		NULL
    },
    {
    		"nand_test",
    		nand_test,
    		"nand测试",
    		NULL
    },
    {
    		"nand_erase",
    		nand_erase,
    		"nand擦除",
    		NULL
    },
    {
    		"create_ptt",
    		create_ptt,
    		"创建文件柜",
    		NULL
    }

};
#define cn_monitor_test_num  ((sizeof(shell_monitordebug_table))/(sizeof(struct tagShellCmdTab)))
static struct tagShellCmdRsc tg_monitorshell_cmd_rsc[cn_monitor_test_num];

ptu32_t MainTestEntry(void)
{
	LedTest();
	djy_printk("FreeScale KENITIS Kxx Serials Board!\n\r");

//	CRT_Init();

	Sh_InstallCmd(shell_monitordebug_table,tg_monitorshell_cmd_rsc,
			cn_monitor_test_num);
//	extern void gui_app(void);
//	gui_app();
//	while(1)
//	{
//		Djy_EventDelay(1000000);
//	}
	return 0;
}
#else
#include "cpu_peri.h"

#define GPIO_PIN_MASK            0x1Fu
#define GPIO_PIN_NO(x)              (((1)<<(x & GPIO_PIN_MASK)))

ptu32_t Led1Task(void)
{
    /* make sure port A clock is on */
//      SIM_SCGC5 = SIM_SCGC5_PORTA_MASK ;

    /* Set PTA10 (connected to LED) for GPIO functionality  */
//      PORTA_PCR29=(0|PORT_PCR_MUX(1));
//      PORTA_PCR28=(0|PORT_PCR_MUX(1));
	PORT_MuxConfig(PORT_PORT_A,PORT_PIN(28),PORT_PINMUX_GPIO);
	PORT_MuxConfig(PORT_PORT_A,PORT_PIN(29),PORT_PINMUX_GPIO);

    /* Change PTA10 to output  */
//      GPIOA_PDDR=GPIO_PDDR_PDD(GPIO_PIN_NO(29) | GPIO_PIN_NO(28));
    GPIO_PinConfig(GPIO_PORT_A,GPIO_PIN(28),GPIO_FUNC_OUTPUT);
    GPIO_PinConfig(GPIO_PORT_A,GPIO_PIN(29),GPIO_FUNC_OUTPUT);

	while(1)
	{
		//Toggle the green LED on PTC9
//		GPIOA_PTOR|=GPIO_PDOR_PDO(GPIO_PIN_NO(28));
		GPIO_PinToggle(GPIO_PORT_A,GPIO_PIN(28));
		Djy_EventDelay(1000000);
	}
	return 0;
}

void LedTest(void)
{
	uint16_t evtt_led;
	evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                Led1Task,NULL,0x40,"led1 blink");

    Djy_EventPop(evtt_led,NULL,0,NULL,0,0);
}
ptu32_t MainTestEntry(void)
{
	LedTest();
//	gui_app();
}
#endif


