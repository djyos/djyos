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
// 文件名     ：cpu-ad.c
// 模块描述: SR3200的CPU采集数据
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 1/11.2016
// =============================================================================

#include "stdint.h"
#include "stddef.h"
#include "cpu_peri.h"
#include "lpc17xx_adc.h"

#define ADC_CLK_RATE			200000   //10K的采样率
#define ACD_TIME_OUT 			1000	//超时时间，us

#define PWR10_CHANNEL			ADC_CHANNEL_2
#define P24V10_CHANNEL			ADC_CHANNEL_3
#define LOCK10_CHANNEL    		ADC_CHANNEL_4

extern void Board_AdcGpioInit(void);
// =============================================================================
// 功能：板级的CPU的ADC模块初始化及相应的通道配置，gpio初始化
// 参数: 无
// 返回：无
// =============================================================================
void PwrSamp_Init(void)
{
	Board_AdcGpioInit();

	ADC_Init( LPC_ADC, ADC_CLK_RATE);                	/*设置 ADC的时钟*/
	ADC_ChannelCmd( LPC_ADC, PWR10_CHANNEL, ENABLE);    /* 使能ADC通道  */
	ADC_ChannelCmd( LPC_ADC, P24V10_CHANNEL, ENABLE);    /* 使能ADC通道  */
	ADC_ChannelCmd( LPC_ADC, LOCK10_CHANNEL, ENABLE);    /* 使能ADC通道  */

//	ADC_BurstCmd(LPC_ADC, ENABLE);
}

// =============================================================================
// 功能：获取CPU AD采样的值，开启一次AD采样，并查询采样是否完成，如果完成，则获取采样值，
//      否则返回不成功；测试结果大概为400us采样能够完成
// 参数: 缓冲区，用户提供的用于存储获取缓冲区的数组，只有指定的通道的数据有效
// 返回：true,ADC采集成功，false，采样失败
// =============================================================================
bool_t PwrSamp_GetValue(u32 *Buf)
{
	u32 TimeOut = 1000;
	FlagStatus Status = RESET;
	bool_t Result = false;

	if(NULL == Buf)
		return false;

	//清标志位
	ADC_ChannelGetStatus(LPC_ADC,PWR10_CHANNEL,ADC_DATA_BURST);
	ADC_ChannelGetStatus(LPC_ADC,P24V10_CHANNEL,ADC_DATA_BURST);
	ADC_ChannelGetStatus(LPC_ADC,LOCK10_CHANNEL,ADC_DATA_BURST);

	ADC_BurstCmd(LPC_ADC, ENABLE);

	//等待最新的一次转换完成
	while(1)
	{
		if(LPC_ADC->ADSTAT & (1<<(LOCK10_CHANNEL + 8)))
			Status = SET;
		if( (Status == SET) || (TimeOut == 0))
			break;
		Djy_DelayUs(1);
		TimeOut -= 1;
	}

	//返回数据
	if(Status == SET)
	{
		Buf[PWR10_CHANNEL] = ADC_ChannelGetData(LPC_ADC,PWR10_CHANNEL);
		Buf[P24V10_CHANNEL] = ADC_ChannelGetData(LPC_ADC,P24V10_CHANNEL);
		Buf[LOCK10_CHANNEL] = ADC_ChannelGetData(LPC_ADC,LOCK10_CHANNEL);
		Result = true;
	}
	else
	{
		Result = false;
	}
	ADC_BurstCmd(LPC_ADC, DISABLE);
	return Result;
}






