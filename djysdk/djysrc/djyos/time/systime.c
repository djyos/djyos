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
//所属模块:时钟模块
//作者:  王建忠
//版本：V1.0.0
//文件描述: 为系统提供时钟日历
//其他说明:
//修订历史:
// 1. 日期: 2012-5-1
//   作者:  罗侍田
//   新版本号: V1.1.0
//   修改说明: 大幅度修改
// 1. 日期: 2011-10-28
//   作者:  王建忠
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "int.h"
#include "systime.h"

s64 __DjyGetTime(void);
static sysrunningtime  fnSysRunningTime = NULL;
extern s64  g_s64OsTicks;             //操作系统运行ticks数

// =============================================================================
// 函数功能：SysTimerConnect
//        为系统提供运行时间
// 输入参数：sysrunningtime
// 输出参数：
// 返回值  ：
// 说明：该函数务必在系统初始化的时候调用，务必不能在系统启动后再调用，除非你和当前系统的运行时间做了同步
// =============================================================================
bool_t SysTimerConnect(sysrunningtime runtime)
{
    if(NULL != runtime)
    {
        fnSysRunningTime = runtime;
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能:DjyGetTime
//       获取系统运行时间
// 输入参数：空
// 输出参数：
// 返回值  ：获取的系统运行时间
// 说明：当有注册时间系统时，采用时间系统的；否则使用默认的时间系统（BSP移植的时候使用的）
// =============================================================================
s64 DjyGetTime(void)
{
    if(NULL != fnSysRunningTime)
    {
        return fnSysRunningTime();
    }
    else
    {
        return __DjyGetTime();
    }
}

//----读取当前ticks-------------------------------------------------------------
//功能：读取操作系统时钟ticks数
//      g_s64OsTicks 为64位变量，非64位系统中，读取 g_s64OsTicks 需要超过1个
//      周期,需要使用原子操作。
//参数：无
//返回：当前时钟
//-----------------------------------------------------------------------------
s64 DjyGetTimeTick(void)
{
    s64 time;
#if (64 > CN_CPU_BITS)
    //若处理器字长不是64位,需要多个周期才能读取os_ticks,该过程不能被时钟中断打断.
    atom_low_t atom_low;
    atom_low = Int_LowAtomStart();
#endif

    time = g_s64OsTicks;

#if (64 > CN_CPU_BITS)
    //若处理器字长不是64位,需要多个周期才能读取os_ticks,该过程不能被时钟中断打断.
    Int_LowAtomEnd(atom_low);
#endif
    return time;
}

