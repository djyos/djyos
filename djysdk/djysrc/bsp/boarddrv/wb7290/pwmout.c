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
// 文件名     ：cpu_peri_pwm.c
// 模块描述: PWM驱动
// 模块版本: V1.00
// 创建人员: zhb
// 创建时间: 8/24.2016
// =============================================================================

#include "string.h"
#include "cpu_peri.h"
#include "pwmout.h"

static const Pin pinPwm[] = {
		{PIO_PD25A_PWMC0_PWML1, PIOD, ID_PIOD, PIO_PERIPH_A, PIO_DEFAULT},
		{PIO_PA15C_PWMC0_PWML3, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_DEFAULT} };
// =========================================================================
// 函数功能：配置PWM
// 输入参数：idx,PWM编号,共有两路，编号为0、1.
//          cycle,周期,单位us
//          duty,占空比,25%的占空比，直接25即可
// 输出参数：无
// 返回值  ：无
// =========================================================================
void PWM_Config(u8 idx,uint32_t cycle,uint32_t duty)
{
	uint32_t temp;

	PIO_Configure(pinPwm,2);

	if(idx > 3)
		return ;
	PMC_EnablePeripheral(ID_PWM0);

	PWM0->PWM_DIS = (0x01<<idx);

	PWM0->PWM_CLK = PWM_CLK_DIVA(75) | PWM_CLK_PREA_CLK_DIV2;
	PWM0->PWM_CH_NUM[idx].PWM_CMR = PWM_CMR_CPRE_CLKA;

	if( (PWM0->PWM_WPSR & PWM_WPSR_WPSWS3) | (PWM0->PWM_WPSR & PWM_WPSR_WPHWS3) )
	{

	}
	{
		temp = (uint32_t)(1 * cycle);
		PWM0->PWM_CH_NUM[idx].PWM_CPRD = PWM_CPRD_CPRD(temp);
	}

	temp = temp * duty / 100;
	PWM0->PWM_CH_NUM[idx].PWM_CDTY = PWM_CDTY_CDTY(temp);

	PWM0->PWM_ENA = (0x01<<idx);
}

