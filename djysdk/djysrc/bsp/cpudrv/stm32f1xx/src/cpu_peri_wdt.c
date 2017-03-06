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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_wdt.c
// 模块描述: WDT模块，使用了WDT组件管理看门狗
// 模块版本: V1.10
// 创建人员: hm
// 创建时间: 28/08.2014
// =============================================================================

#include "stdint.h"
#include "stddef.h"
#include "systime.h"
#include "wdt_hal.h"
#include "cpu_peri.h"


#define CN_WDT_WDTCYCLE  5000*mS//200*1000      //单位为微秒
#define CN_WWDG_WDTCYCLE  50*mS                 //WWDG最大为58mS
#define CN_WDT_DOGNAME   "STM32IWDG"
#define CN_WWDG_DOGNAME   "STM32WWDG"

#define CN_IWDG_BASE  0x40003000
#define CN_WWDG_BASE  0x40002C00
#define STM_IWDG      ((tagIWDGReg*)CN_IWDG_BASE)
#define STM_WWDG      ((tagWWDGReg*)CN_WWDG_BASE)

#define CN_PRE_VALUE   5
static u16 s_prevalue[]={4,8,16,32,64,128,256,256};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ------------------------ STM32 独立开门狗 -----------------------------------
// ----------------------------------------------------------------------------
// =============================================================================
// 函数功能:使能开门狗
// 输入参数:
// 返回值  :true成功false失败
// =============================================================================
bool_t IWDG_Stm32Enable(void)
{
    STM_IWDG->IWDG_KR = 0xCCCC;
    return true;
}

// =============================================================================
// 函数功能:IWDG_Stm32WdtFeed
//          IWDG_Stm32WdtFeed的喂狗函数
// 输入参数:
// 返回值  :true成功false失败
// =============================================================================
bool_t IWDG_Stm32WdtFeed(void)
{
    STM_IWDG->IWDG_KR = 0xAAAA;
    return true;
}

// =============================================================================
// 函数功能:IWDG_PreSet,IWDG时钟源为内部RC，为40KHz
//          IWDG_PreSet设置时钟分步系数
// 输入参数:
// 返回值  :true成功false失败
// =============================================================================
bool_t IWDG_Stm32PreSet(u8 Value)
{
    //配置IWDG模式
    if(!(STM_IWDG->IWDG_SR & IWDG_SR_PVU_MASK))
    {
        STM_IWDG->IWDG_KR = 0x5555;
        STM_IWDG->IWDG_PR = IWDG_PR_PR & Value;
        //wait finish
        while((STM_IWDG->IWDG_SR & IWDG_SR_PVU_MASK));
        return true;
    }
    else
        return false;
}

// =============================================================================
// 函数功能:IWDG_Stm32ReloadSet,设置重装载值
//          IWDG_Stm32ReloadSet硬件看门狗初始化
// 输入参数:
// 返回值  :true成功false失败
// =============================================================================
bool_t IWDG_Stm32ReloadSet(u16 value)
{
    if(!(STM_IWDG->IWDG_SR & IWDG_SR_RVU_MASK))
    {
        STM_IWDG->IWDG_KR = 0x5555;
        STM_IWDG->IWDG_RLR = IWDG_RLR_RL & value;
        //wait finish
        while((STM_IWDG->IWDG_SR & IWDG_SR_RVU_MASK));
        return true;
    }
    else
        return false;
}

// =============================================================================
// 函数功能:IWDG_Stm32Initial
//          IWDG_Stm32Initial看门狗注册
// 输入参数:
// 返回值  :true成功false失败
// 说明：IWDG不可用，配置完PR和RL后，STM_IWDG->IWDG_SR相应位总是为1
// =============================================================================
bool_t IWDG_Stm32Initial(u32 setcycle)
{
    bool_t result;
    pg_rcc_reg->CSR |= (1<<0);//turn on LSI
    while(!(pg_rcc_reg->CSR & (1<<1)));

    IWDG_Stm32PreSet(CN_PRE_VALUE);     //配置时钟预分频，时钟源为40KHZ
    //配置重装载值，即喂狗后，装载值,配置为5分频
    IWDG_Stm32ReloadSet((40/s_prevalue[CN_PRE_VALUE]) * (CN_WDT_WDTCYCLE/mS));

    result = WdtHal_RegisterWdtChip(CN_WDT_DOGNAME,CN_WDT_WDTCYCLE,\
                                    IWDG_Stm32WdtFeed,NULL,NULL);

    IWDG_Stm32Enable();
    return result;
}

bool_t WWDG_STM32WdtFeed(void)
{
    STM_WWDG->WWDG_CR |= 0x7F;
    return true;
}

//PCLK1=36M时钟，CK计时器时钟(PCLK1除以4096)除以8，为1.09，即WWDG计数器每
//1.09mS计数一次，WWDG看门狗最大喂狗时间为58mS
bool_t WWDG_STM32Init(u32 setcycle)
{
    bool_t result;
    pg_rcc_reg->APB1ENR |= (1<<11);//enable wwdg
    STM_WWDG->WWDG_CR &= ~(1<<7);//WDGA
    STM_WWDG->WWDG_CFR |= (3<<7);//WDGTB0,1
    WWDG_STM32WdtFeed();

    result = WdtHal_RegisterWdtChip(CN_WWDG_DOGNAME,CN_WWDG_WDTCYCLE,\
                            WWDG_STM32WdtFeed,NULL,NULL);
    STM_WWDG->WWDG_CR |= (1<<7);//使能看门狗
    return result;
}
