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
//所属模块: CPU相关低功耗功能
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: cpu相关的低功耗管理,实现lowpower.h文件中规定bsp需要提供的函数,在这
//          里实现CPU相关的函数, 跟板件相关的,可到boarddrv目录下去找
//          使用了stm32的42个备份寄存器中最后一个.
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

//stm32上的低功耗Level定义(参考lowpower.h文件中的定义)
//L0:即执行wfe指令,无其他操作.与L1一样,进入的是处理器的睡眠模式.
//L1:wfe指令,内核执行的操作与L0一致,但用户的回调函数可能停掉更多时钟
//L2:即进入stm32的停止模式,保留SRAM和寄存器值,退出后继续运行
//L3/L4:进入stm32的待机模式,SRAM和寄存器值都丢失只有备份寄存器维持供电.区别在于
//退出后,L3进入时要保存SRAM和寄存器,退出时要恢复并从退出处继续执行.

#include "stdint.h"
#include "lowpower.h"
#include "cpu_peri.h"

#define Stm32SleepModel4    0x789a

//----初始化低功耗硬件--------------------------------------------------------
//功能: 初始化低功耗管理硬件,如果不需要初始化,可以直接return true.因为stm32低功耗
//      管理需要使用备份区保存参数, 故需要打开备份模块电源和使能备份区访问.
//参数: 无
//返回: 无
//----------------------------------------------------------------------------
bool_t __LP_BSP_HardInit(void)
{
    RCC->APB1ENR |=RCC_APB1ENR_BKPEN|RCC_APB1ENR_PWREN;//PWR模块使能
    HAL_PWR_EnableBkUpAccess();//后备区使能
    return true;
}

//----读取休眠级别------------------------------------------------------------
//功能: 当个系统从休眠唤醒并重新,调用本函数可以获得唤醒前的休眠状态,返回值是
//      在lowpower.h文件中定义的CN_SLEEP_L3、CN_SLEEP_L4、CN_SLEEP_NORMAL之一,
//      CN_SLEEP_NORMAL表示上电或者硬复位,或者软件指令复位.注意,返回值不会是
//      CN_SLEEP_L0~2之一.
//参数: 无
//返回: CN_SLEEP_L3、CN_SLEEP_L4、CN_SLEEP_NORMAL之一
//----------------------------------------------------------------------------
u32 __LP_BSP_GetSleepLevel(void)
{
    u32 bkt_DR;
    RTC_HandleTypeDef RTC_Handler;  //RTC句柄
    RTC_Handler.Instance=RTC;
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_WU+PWR_FLAG_SB)& PWR_FLAG_WU)
    {
        bkt_DR = HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR2);//todo
//        bkt_DR = Stm32SleepModel4;
        if(bkt_DR == Stm32SleepModel4)
            return CN_SLEEP_L4;
        else
            return CN_SLEEP_L3;
    }
    else
        return CN_SLEEP_NORMAL;
}

//----保存休眠级别-------------------------------------------------------------
//功能: 把即将进入的休眠级别保存到相应的硬件中,注意,只有L3和L4需要.
//参数: SleepLevel, 即将进入的低功耗级别
//返回true=成功,false=失败
//-----------------------------------------------------------------------------
bool_t __LP_BSP_SaveSleepLevel(u32 SleepLevel)
{
    RTC_HandleTypeDef RTC_Handler;  //RTC句柄
    RTC_Handler.Instance=RTC;

    if((SleepLevel!= CN_SLEEP_L3) && (SleepLevel!= CN_SLEEP_L4))
        return false;
    HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR2,SleepLevel);//todo
    return true;

}

//----进入L0级低功耗-----------------------------------------------------------
//功能: 进入L0级低功耗状态,函数在lowpower.h中声明,供lowpower.c文件调用
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void __LP_BSP_EntrySleepL0(void)
{
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
}

//----进入L1级低功耗-----------------------------------------------------------
//功能: 进入L1级低功耗状态,函数在lowpower.h中声明,供lowpower.c文件调用.在stm32中
//      L0和L1其实是一样的,但用户的回调函数可能不一样.
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void __LP_BSP_EntrySleepL1(void)
{
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFE);
}

//----进入L2级低功耗-----------------------------------------------------------
//功能: 进入L2级低功耗状态,函数在lowpower.h中声明,供lowpower.c文件调用
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void __LP_BSP_EntrySleepL2(void)
{
    //禁止中断
    //清所有外部中断标志和RTC闹钟标志
    EXTI->PR=0xFFFFF;
    CLEAR_BIT(RTC->CRL, RTC_FLAG_ALRAF);
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);
}

//----进入L3级低功耗-----------------------------------------------------------
//功能: 进入L3级低功耗状态,函数在lowpower.h中声明,供lowpower.c文件调用
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void __LP_BSP_EntrySleepL3(void)
{
    HAL_PWR_EnterSTANDBYMode();
}

//----进入L4级低功耗-----------------------------------------------------------
//功能: 进入L4级低功耗状态,函数在lowpower.h中声明,供lowpower.c文件调用,stm32中,
//      L3和L4在cpudrv方面,是一致的.
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void __LP_BSP_EntrySleepL4(void)
{
    HAL_PWR_EnterSTANDBYMode( );
}

