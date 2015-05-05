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
//所属模块：时钟模块
//作者：林谦
//版本：V0.1.0
//文件描述：DJYOS与硬件RTC连接文件
//其他说明：
//修订历史：
//2. 日期：2012-06-26
//   作者：Merlin
//   新版本号：
//   修改说明：事件初始化在djyos/clock.c中进行，本文件用于操作运行平台中的RTC硬件
//1. 日期: 2012-05-02
//   作者：林谦
//   新版本号：V0.1.0
//   修改说明：原始版本
//------------------------------------------------------
#include "stddef.h"
#include "time.h"
#include "stm32f10x.h"
#include "cpu_peri.h"

// =============================================================================
// 功能：RTC硬件寄存器配置
// 参数：无
// 返回：无
// =============================================================================
void RTC_Configuration(void)
{
    // 使用RTC及BKP的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);    // 使能对RTC及BKP的寄存器的访问
    BKP_DeInit();   // 复位BKP到初始化状态
    RCC_LSEConfig(RCC_LSE_ON);  // 配置外部低频振荡器
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) // 等待初始化完成
    {}

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); // 选择外部低速时钟
    RCC_RTCCLKCmd(ENABLE);  // 使能RTC的时钟
    RTC_WaitForSynchro();   // 等待RTC寄存器与APB时钟同步
    RTC_WaitForLastTask();  // 等待最后一次RTC写寄存器完成

    RTC_ITConfig(RTC_IT_SEC, DISABLE);  // 禁止RTC的秒中断
    RTC_WaitForLastTask();  // 等待最后一次RTC写寄存器完成

    // 设置RTC的分频值
    /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
    RTC_SetPrescaler(32767);
    RTC_WaitForLastTask();  // 等待最后一次RTC写寄存器完成
}

// =============================================================================
// 功能：获取RTC时间，获取的时间是S，得到微秒要乘以1000000
// 参数：time,时间，单位为微秒
// 返回：true,成功;false,失败
// =============================================================================
bool_t RTC_TimeGet(s64 *time)
{
    *time = 1000000 * RTC_GetCounter();
    RTC_WaitForLastTask();

    return true;
}

// =============================================================================
// 功能：更新RTC时间，RTC里面的时间是S，因此要除以1000000
// 参数：time,时间，单位为微秒
// 返回：true,成功;false,失败
// =============================================================================
bool_t RTC_TimeUpdate(s64 time)
{
    u32 time_s;

    time_s = (u32)(time/1000000);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);    // 使能对RTC及BKP的寄存器的访问
    RTC_WaitForSynchro();

    RTC_SetCounter((u32)time_s);
    RTC_WaitForLastTask();

    return true;
}

// =============================================================================
// 功能：RTC模块初始化，包括注册接口函数和配置RTC
// 参数：para,暂时无用
// 返回：true,成功;false,失败
// =============================================================================
ptu32_t ModuleInstall_RTC(ptu32_t para)
{
    // 查看备份寄存器，如果不是0xA5A5的话，则表示从未初始化过RTC（包括在断电之前）
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        RTC_Configuration();    // 配置RTC
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);   // 标志RTC已经初始化过
    }

    RTC_WaitForSynchro();

    Rtc_RegisterDev(RTC_TimeGet,RTC_TimeUpdate);
    return true;
}
