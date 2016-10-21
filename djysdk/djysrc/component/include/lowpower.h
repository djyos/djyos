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
//所属模块: 低功耗模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:实现低功耗
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-06-24
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __LOWPOWER_H__
#define __LOWPOWER_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "djyos.h"

//休眠算法:
//1.启动后,默认执行L0级低功耗.该模式完全不影响程序运行.
//2.应用程序不可以主动调用函数进入低功耗的函数.
//3.应用程序可以调用api提升低功耗级别.
//4.应用程序可以调用SaveLPL-RestoreLPL函数对暂停高级低功耗模式,返回到默认的L0模式.
//5.低功耗模式统一从系统服务事件为入口

//低功耗级别定义:
//特别注意,根据硬件能力,有些级别在某些硬件下可能会重叠,例如在stm32的cortex-m系列
//mcu中,L0和L1级是等同的(用户的回调函数可能不一样).
//低功耗控制是高级功能,在编写回调函数前,必须非常了解自己的硬件系统.
//L0.每次系统服务事件执行,执行栈检查后,进入最浅的休眠,一旦有中断,典型是tick中断,
//   立即唤醒继续执行。
//L1.每次系统服务事件执行,执行栈检查后,进入深度睡眠,保持所有内存,tick时钟正常运行,
//   一旦有中断,典型是tick中断,立即唤醒继续执行,唤醒所需时间可能长于L0。
//   注:对许多CPU来说,L0和L1可能是相同的,但有些外设也许有区别
//L2.每次系统服务事件执行,执行栈检查后,进入深度睡眠,保持内存,时钟停止,仅中断能
//   唤醒CPU,唤醒后,处理中断,然后继续运行,直到下一次进入低功耗状态。
//L3.每次系统服务事件执行,执行栈检查后,进入深度睡眠,内存掉电,时钟停止,CPU运行
//   状态保存到栈中,调用回调函数,把全部内存数据保存到非易失存储器中.复位唤醒
//   CPU后,重新启动,把保存到非易失存储器中的内容恢复到RAM,然后继续运行。
//L4.每次系统服务事件执行,执行栈检查后,进入深度睡眠,内存掉电,时钟停止,调用回调
//   函数,把关键数据保存到非易失存储器中,中断唤醒CPU后,重新启动并加载运行。

#define CN_SLEEP_NORMAL    0xff    //休眠方式:正常
#define CN_SLEEP_ERROR     0       //休眠方式:错误
#define CN_SLEEP_L0        1       //休眠方式:L0
#define CN_SLEEP_L1        2       //休眠方式:L1
#define CN_SLEEP_L2        3       //休眠方式:L2
#define CN_SLEEP_L3        4       //休眠方式:L3
#define CN_SLEEP_L4        5       //休眠方式:L4

struct LowPowerCtrl
{
    u32 SleepLevel;                             //当前低功耗级别
    u32 DisableCounter;     //禁止低功耗次数计数,0才可以进入L1及以上级别低功耗
    u32 (*EntrySleepReCall)(u32 SleepLevel);      //进入低功耗状态前的回调函数
    u32 (*ExitSleepReCall)(u32 SleepLevel);       //从低功耗状态唤醒后的回调函数

};
//以下函数须在bsp中实现,根据具体硬件,如果该函数跟板有关,则在bsp\boarddrv中实现,
//如果跟cpu有关,则在bsp\cpudrv目录提供,移植新系统时灵活处置.
//跟低功耗相关的硬件初始化
bool_t __LP_BSP_HardInit(void);
// 程序boot后调用本函数可知是从L3还是从L4唤醒,或者是正常启动.
// 返回值是为CN_WAKEUP_NORMAL、CN_WAKEUP_ERROR、 CN_WAKEUP_L3、CN_WAKEUP_L4之一。
// 特别注意:如果硬件在重新上电或复位时,没有自动清除低功耗状态启动标识,则无论
// 调用本函数返回什么,函数返回前,启动模式都被改为 CN_WAKEUP_NORMAL.
// 本函数必须加载到startup中.
u32 __LP_BSP_GetSleepLevel(void);
//把SleepLevel保存到专用硬件或者flash中,只有L3和L4需要.
bool_t __LP_BSP_SaveSleepLevel(u32 SleepLevel);    //返回true=成功,false=失败
//从非易失存储器中恢复RAM,L3级低功耗使用,如果本函数返回失败,系统按normal方式启动
bool_t __LP_BSP_RestoreRamL3(void);     //返回true=成功恢复,false=失败
//把RAM存储到非易失存储器中,L4级低功耗使用,如果返回false,系统将不进入休眠
bool_t __LP_BSP_SaveRamL3(void);     //返回true=成功恢复,false=失败
//从非易失存储器中恢复关键数据,L4级低功耗使用,如果本函数返回失败,系统按normal方式启动

void __LP_BSP_EntrySleepL0(void);
void __LP_BSP_EntrySleepL1(void);
void __LP_BSP_EntrySleepL2(void);
void __LP_BSP_EntrySleepL3(void);
void __LP_BSP_EntrySleepL4(void);

//在cps.S中实现,进入L3级低功耗前调用,保存上下文。
void __LP_BSP_AsmSaveReg(struct ThreadVm *running_vm,
                            bool_t (*SaveRamL3)(void),
                            void (*EntrySleepL3)(void));

//禁止sleep,使DisableCounter+1
u32 LP_DisableSleep(void);
//使能sleep,使DisableCounter-1,达到0则允许sleep.在禁止状态下,只允许CN_SLEEP_L0
//级低功耗
u32 LP_EnableSleep(void);
// 设置进入休眠时的休眠模式,调用本函数后,并不立即休眠,而是要等次系统服务事件执
// 行,且休眠没有被禁止时,才会进入相应的休眠模式
u32 LP_SetSleepLevel(u32 Level);
//安装低功耗组件
ptu32_t ModuleInstall_LowPower (u32 (*EntrySleepReCall)(u32 SleepLevel),
                                u32 (*ExitSleepReCall)(u32 SleepLevel));

#ifdef __cplusplus
}
#endif

#endif //__LOWPOWER_H__



