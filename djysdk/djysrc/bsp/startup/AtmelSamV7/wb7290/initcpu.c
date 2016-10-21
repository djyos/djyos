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
//所属模块:  CPU初始化
//作者：     lst
//版本：      V1.0.0
//初始版本完成日期：2013-05-27
//文件描述:  cpu时钟初始化
//其他说明:  无
//修订历史:
//2. ...
//1. 日期: 2014-01-20
//   作者: lst
//   新版本号：V1.0.0
//   修改说明: 本版是初版,用于验证目的
//		下一版需增加判断时钟是否已经起振的代码
//------------------------------------------------------
#include "stdint.h"
#include "cpu_peri.h"
#include "hard-exp.h"
#include "arch_feature.h"
#include <samv7/samv71.h>
#include "core_cmFunc.h"
#include "IAP.h"
#include "cfg/Iboot_config.h"
#include "cpu_peri.h"

#ifndef __CHECK_DEVICE_DEFINES
#define __CHECK_DEVICE_DEFINES
#endif

#include "core_cm7.h"

extern   uint32_t   msp_top[ ];
extern void __set_PSP(uint32_t topOfProcStack);
extern void __set_PRIMASK(uint32_t priMask);
extern void __set_FAULTMASK(uint32_t faultMask);
extern void __set_CONTROL(uint32_t control);

extern void SysClk_Init(void);
extern void IAP_SelectLoadProgam(void);
extern char g_cIbootFlag[];

struct ScbReg volatile * const startup_scb_reg
                        = (struct ScbReg *)0xe000ed00;
void Startup_NMI(void)
{
    while(1);
}
void Startup_Hardfault(void)
{
    while(1);
}
void Init_Cpu(void);
const u32 gc_u32StartupExpTable[4] =
{
    (u32)msp_top,
    (u32)Init_Cpu,
    (u32) Startup_NMI,
    (u32) Startup_Hardfault
};
void Init_Cpu(void)
{
	__set_PSP((uint32_t)msp_top);
	__set_PRIMASK(1);
	__set_FAULTMASK(1);
	__set_CONTROL(0);

    #if (CN_CPU_OPTIONAL_FPU == 1)
        startup_scb_reg->CPACR = (3UL << 20)|(3UL << 22);    //使能FPU
        startup_scb_reg->FPCCR = (1UL << 31);                //关闭lazy stacking
    #endif
    switch(startup_scb_reg->CPUID)
    {
    }
    //关狗
    WDT->WDT_MR = (1<<15);

    SysClk_Init();				//系统主时钟、外设时钟、PLL初始化


//    SDRAM_Init();				//SDRAM初始化
#if (CN_CPU_OPTIONAL_CACHE == 1)
	SCB_EnableICache();			//指令、数据cache使能
	SCB_EnableDCache();
#endif

	IAP_SelectLoadProgam();
}



void IAP_GpioPinInit(void)
{
	u32 count;
	const Pin iap_pin={PIO_PA28,PIOA,ID_PIOA,PIO_INPUT,PIO_DEFAULT};
	if ( (PMC->PMC_PCSR0 & ((uint32_t)1 << ID_PIOA)) != ((uint32_t)1 << ID_PIOA) )
	{
		PMC->PMC_PCER0 = 1 << ID_PIOA ;
	}
	Pio *Iap_pin=((Pio    *)0x400E0E00U);   //PIOA
	Iap_pin->PIO_IDR = PIO_PA28;
	Iap_pin->PIO_PUDR = PIO_PA28;
	Iap_pin->PIO_IFDR=PIO_PA28;
	Iap_pin->PIO_ODR=PIO_PA28;
	Iap_pin->PIO_PER=PIO_PA28;
	for(count=0;count<16000000;count++)
	{
	}
}
bool_t IAP_IsForceIboot(void)
{
	const Pin iap_pin={PIO_PA28,PIOA,ID_PIOA,PIO_INPUT,PIO_DEFAULT};
	unsigned int reg ;
	if ( (iap_pin.type == PIO_OUTPUT_0) || (iap_pin.type == PIO_OUTPUT_1) )
	{
		reg = iap_pin.pio->PIO_ODSR ;
	}
	else
	{
		reg = iap_pin.pio->PIO_PDSR ;
	}
	if ( (reg & ((1u << 28))) == 0 )
	{
		return false ;
	}
	else
	{
		return true ;
	}
}
bool_t IAP_IsRamIbootFlag(void)
{
	u8 i;
	char bootflag[8]="RunIboot";
	for(i=0;i<8;i++)
	{
		if(g_cIbootFlag[i]!=bootflag[i])
		{
			return false;
		}
	}
	return true;
}
