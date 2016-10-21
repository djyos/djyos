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
// 模块描述: timer的硬件接口
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 1:53:42 PM/Mar 4, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注：该文件重在留给使用定时器的任务一个统一的接口，实现还是在timer_driver.c文件
#include "timer_hard.h"
#include "stddef.h"
static struct TimerChip   s_tTimerHardChip = {NULL};//注册的芯片驱动
// =============================================================================
// 函数功能：TimerHard_RegisterChip
//          注册定时器芯片到系统定时器模块
// 输入参数:timerchip,定时器芯片
// 输出参数：无
// 返回值  ：true 成功 false失败，本层实现
// =============================================================================
bool_t  TimerHard_RegisterChip(struct TimerChip *timerchip)
{
    if(NULL != timerchip)
    {
        //不能整体赋值，此时memcpy没有加载，整体赋值会间接调用memcpy。
        s_tTimerHardChip.chipname  = timerchip->chipname;
        s_tTimerHardChip.TimerHardAlloc = timerchip->TimerHardAlloc;
        s_tTimerHardChip.TimerHardFree =  timerchip->TimerHardFree;
        s_tTimerHardChip.TimerHardGetFreq = timerchip->TimerHardGetFreq;
        s_tTimerHardChip.TimerHardCtrl = timerchip->TimerHardCtrl;
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能：TimerHard_UnRegisterChip
//          定时器芯片注销
// 输入参数：无
// 输出参数：无
// 返回值  ：true 成功 false失败
// 说明    :目前而言是没有用，主要是register之后必然有unregister，本层实现
// =============================================================================
bool_t TimerHard_UnRegisterChip(void)
{
    s_tTimerHardChip.TimerHardAlloc = NULL;
    s_tTimerHardChip.TimerHardFree = NULL;
    s_tTimerHardChip.TimerHardGetFreq = NULL;
    s_tTimerHardChip.TimerHardCtrl = NULL;
    return true;
}
// =============================================================================
// 函数功能：TimerHard_Alloc
//          HAL定时器分配
// 输入参数：
//     timerisr,分配的定时器的中断服务函数,中断中调用
// 输出参数：无
// 返回值  ：-1 = 分配不成功，否则返回定时器句柄，该句柄的结构由定时器芯片自己定义
// 说明    :（一般而言刚分配的定时器，其计时停止，中断禁止）依赖芯片驱动实现
//        刚开始分配的定时器应该是各种属性都关闭的，因此属性必须自己重新设置
//        默认：停止计数，异步中断，reload,中断禁止
// =============================================================================
ptu32_t TimerHard_Alloc(fntTimerIsr timerisr)
{
    if(NULL == s_tTimerHardChip.TimerHardAlloc)
    {
        return 0;
    }
    else
    {
        return s_tTimerHardChip.TimerHardAlloc(timerisr);
    }
}

// =============================================================================
// 函数功能：TimerHard_Free
//          HAL定时器释放
// 输入参数：timerhandle，待释放定时器
// 输出参数：无
// 返回值  ：true 成功 false失败 
// 说明    : 依赖芯片驱动实现
// =============================================================================
bool_t  TimerHard_Free(ptu32_t timerhandle)
{
    if(NULL == s_tTimerHardChip.TimerHardFree)
    {
        return false;
    }
    else
    {
        return s_tTimerHardChip.TimerHardFree(timerhandle);
    }
}

// =============================================================================
// 函数功能：去定时器时钟源频率
// 输入参数：timerhandle，待释放定时器
// 输出参数：无
// 返回值  ：时钟源频率，Hz 
// 说明    : 依赖芯片驱动实现
// =============================================================================
u32  TimerHard_GetFreq(ptu32_t timerhandle)
{
    if(NULL == s_tTimerHardChip.TimerHardGetFreq)
    {
        return false;
    }
    else
    {
        return s_tTimerHardChip.TimerHardGetFreq(timerhandle);
    }
}

// =============================================================================
// 函数功能：TimerHard_Ctrl
//          操作硬件定时器
// 输入参数：timerhard，待设置定时器， 
//          ctrlcmd, 操作命令，参见fnTimerHardCtrl类型声明的注释
// 输出参数：inoutpara，输入输出参数，根据不同的情况而定
// 返回值  ：true 操作成功 false操作失败 
// 说明：ctrlcmd对应的inoutpara的属性定义说明
// =============================================================================
bool_t TimerHard_Ctrl(ptu32_t timerhandle, \
                      enum TimerCmdCode ctrlcmd, \
                      ptu32_t inoutpara)
{
    bool_t result;
    if(NULL == s_tTimerHardChip.TimerHardCtrl)
    {
        result = false;
    }
    else
    {
        result = s_tTimerHardChip.TimerHardCtrl(timerhandle, ctrlcmd, inoutpara);
    }
    return result;
}
