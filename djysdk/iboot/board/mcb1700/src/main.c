
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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: main.c
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 9:50:10 AM/Aug 15, 2014
// =============================================================================
#include "djyos.h"
#include "gkernel.h"
#include "cpu_peri.h"
#include "systime.h"

const unsigned long led_mask[] = { 1UL<<28, 1UL<<29, 1UL<<31, 1UL<< 2,
                                   1UL<< 3, 1UL<< 4, 1UL<< 5, 1UL<< 6 };

u16 ledofftask;
u16 ledontask;
u8 flag = 0;
void LED_Init (void)
{
    LPC_SC->PCONP     |= (1 << 15);            // enable power to GPIO & IOCON

    LPC_GPIO1->FIODIR |= 0xB0000000;           // LEDs on PORT1 are output
    LPC_GPIO2->FIODIR |= 0x0000007C;           // LEDs on PORT2 are output
}

/*----------------------------------------------------------------------------
  Function that turns on requested LED
 *----------------------------------------------------------------------------*/
void LED_On (unsigned int num)
{
  if (num < 3) LPC_GPIO1->FIOPIN |=  led_mask[num];
  else         LPC_GPIO2->FIOPIN |=  led_mask[num];
}

/*----------------------------------------------------------------------------
  Function that turns off requested LED
 *----------------------------------------------------------------------------*/
void LED_Off (unsigned int num)
{
  if (num < 3) LPC_GPIO1->FIOPIN &= ~led_mask[num];
  else         LPC_GPIO2->FIOPIN &= ~led_mask[num];
}

ptu32_t LedAllOn(void)
{
    u8 i;
    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            LED_On(i);
            Djy_EventDelay(100*mS);
        }
        Djy_EventPop(ledofftask,NULL,0,NULL,0,200);
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
    return 0;
}
ptu32_t LedAllOff(void)
{
    u8 i;
    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            LED_Off(i);
            Djy_EventDelay(100*mS);
        }
        Djy_EventPop(ledontask,NULL,0,NULL,0,200);
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
    return 0;
}

void Led_Task(void)
{
    LED_Init();

    ledontask = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                    LedAllOn,NULL,0x300,"LedAllOn function");
    if(ledontask == CN_EVTT_ID_INVALID)
    {
        printk("ledontask create failed!\r\n");
    }

    ledofftask = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                    LedAllOff,NULL,0x300,"LedAllOff function");
    if(ledofftask == CN_EVTT_ID_INVALID)
    {
        printk("ledofftask create failed!\r\n");
    }
    Djy_EventPop(ledontask,NULL,0,NULL,0,200);

}
extern u32 g_bRunModeFlag;
void djy_main(void)
{
	Led_Task();
	g_bRunModeFlag=0x12345678;
	printf("Run Mode:Iboot.\r\n");
//	Timer_Test();
//	mcb1700_display();
    while(1)
    {
    	Djy_EventDelay(500*mS);
//    	GDD_Demo();
    }
}

