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
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
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
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_systime.c
// 模块描述: 系统上电时间模块，用一个RTT的32位定时器计时系统上电时钟
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/15.2015
// =============================================================================

#include "cpu_peri.h"
#include "systime.h"

#define SYSTIME_TC   	(TC2)			//使用16位定时器TC2的通道0作为systime
#define SYSTIME_TC_CH 	(0)

#define SYSTIME_FRE		(CN_CFG_MCLK/ (128*2))	//即1.171875MHz
#define SYSTIME_CYCLE	(65536)	//周期为最大，即0xFFFF,最大T= SYSTIME_FRE * 0xFFFF
// =============================================================================
// 功能：获取16位定时器当前值
// 参数：无
// 返回：16位的定时器时间值
// =============================================================================
u32 Systime_GetTime(void)
{
    return SYSTIME_TC->TC_CHANNEL[SYSTIME_TC_CH].TC_CV;
}

// =============================================================================
// 功能：系统时钟初始化，使用16位的时钟计数器作为系统时钟
// 参数：无
// 返回：无
// =============================================================================
static void SysTime_TcConfig(void)
{
	//hemin modified 20160613
	//TC2时钟源改成TC_CMR_TCCLKS_TIMER_CLOCK4，即MCK/128 = 150M/128

    PMC_EnablePeripheral(ID_TC6);//ID_TC6 - ID_TC8 对应TC2的通道0-2

    //配置TC相应通道的参数，禁止中断，配置比较器，周期
    SYSTIME_TC->TC_CHANNEL[SYSTIME_TC_CH].TC_CCR = TC_CCR_CLKDIS;
    SYSTIME_TC->TC_CHANNEL[SYSTIME_TC_CH].TC_IDR = 0xFFFFFFFF;
    SYSTIME_TC->TC_CHANNEL[SYSTIME_TC_CH].TC_SR;
    SYSTIME_TC->TC_CHANNEL[SYSTIME_TC_CH].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK4;

    //使能定时器
    SYSTIME_TC->TC_CHANNEL[SYSTIME_TC_CH].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}
// =============================================================================
// 功能：系统时钟初始化，注册系统时钟，并初始化RTT闹钟中断
// 参数：para
// 返回：true,成功;false,失败
// =============================================================================
bool_t MoudleInit_Systime(ptu32_t para)
{
    //初始化TC控制器
	SysTime_TcConfig();

	//注册系统时钟函数
    SysTimeConnect(Systime_GetTime,NULL,SYSTIME_FRE,SYSTIME_CYCLE);
    return true;
}


