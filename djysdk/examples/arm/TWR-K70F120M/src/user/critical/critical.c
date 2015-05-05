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
//所属模块:安全钩子
//作者：lst
//版本：V1.0.0
//文件描述:安全钩子是必须在开机后尽可能快地执行的部分代码
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "cpu_peri.h"

const ufast_t tg_IntUsed[] =
{
	CN_INT_LINE_PORTA,			//(0x57)
	CN_INT_LINE_PORTB,			//(0x58)
	CN_INT_LINE_PORTC,			//(0x59)
	CN_INT_LINE_PORTD,          //(0x5A)
	CN_INT_LINE_PORTE,          //(0x5B)
	CN_INT_LINE_PORTF,			//(0x5C)
	CN_INT_LINE_RTC_SEC,
	CN_INT_LINE_ENET_TX,
	CN_INT_LINE_ENET_RX,
	CN_INT_LINE_ENET_ERR,
	CN_INT_LINE_UART2_ERR,
	CN_INT_LINE_UART2_RX_TX,
	CN_INT_LINE_UART0_ERR,
	CN_INT_LINE_UART0_RX_TX,
	CN_INT_LINE_PIT0,
	CN_INT_LINE_PIT1,
	CN_INT_LINE_PIT2,
	CN_INT_LINE_PIT3,
	CN_INT_LINE_I2C0,
	CN_INT_LINE_I2C1,
	CN_INT_LINE_PORTB
};

const ufast_t tg_IntUsedNum = sizeof(tg_IntUsed)/sizeof(ufast_t);


//----配置全部IO口--------------------------------------------------------------
//功能：除必须的外，全部配置成初始态，各功能口由相应的功能模块自行定义。
//参数：无
//返回：无
//------------------------------------------------------------------------------
void gpio_init(void)
{
}

void critical(void)
{
#if 0 //ARM版本认为程序加载过程无误
    //定时器初始化
#if(CN_CFG_SYSTIMER == 1)
    extern void Timer_ModuleInit(void);
    Timer_ModuleInit();
#endif
#if(CN_CFG_WDT == 1)
	//  初始化硬件看门狗
	extern bool_t WDT_FslInit(u32 setcycle);
	WDT_FslInit(1000);

	// 看门狗组件HAL喂狗
	//  开启中断并注册相关的服务函数
	bool_t WdtHal_BootStart(u32 bootfeedtimes);
	WdtHal_BootStart(20);
#endif
#endif
}


