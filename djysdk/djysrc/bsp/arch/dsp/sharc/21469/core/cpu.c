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
//所属模块: 调度器
//作者：lst
//版本：V1.0.1
//文件描述: 调度器中与CPU直接相关的代码。
//其他说明:
//修订历史:
//2. 日期: 2009-04-24
//   作者: lst
//   新版本号: V1.0.1
//   修改说明: 删除了一些为dlsp版本准备的东西
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "board-config.h"
#include "stdlib.h"
#include "align.h"
#include "cpu_peri.h"
#include "int.h"
#include "string.h"

#include <stdio.h>
#include <def21489.h>
#include <signal.h>
#include <21489.h>

extern "asm" unsigned int ldf_heap_space;
extern "asm" unsigned int ldf_heap_end;
extern   u32 asm_get_tick(void);

u32 *sys_heap_bottom_cnt = &ldf_heap_space;
u32 *sys_heap_top_cnt = &ldf_heap_end;


extern s64  g_s64OsTicks;             //操作系统运行ticks数
u32 g_u32CycleSpeed;
static volatile ufast_t uf_delay_counter;
u32 volatile  core_time;


void SetDelayHandler(int sig_int);

u32 __DjyIsrTick(ptu32_t line);
//----创建线程-----------------------------------------------------------------
//功能：为事件类型创建伪虚拟机，初始化上下文环境，安装执行函数，构成完整线程
//参数：evtt_id，待创建的线程所服务的事件类型id
//返回：新创建的线程的虚拟机指针
//注: 移植敏感函数
//-----------------------------------------------------------------------------
struct ThreadVm *__CreateThread(struct EventType *evtt,u32 *stack_size)
{
    struct ThreadVm  *result;
    ptu32_t  len;

    //计算虚拟机栈:线程+最大单个api需求的栈
    len = evtt->stack_size;
    //栈顶需要对齐，malloc函数能保证栈底是对齐的，对齐长度可以使栈顶对齐
    len = align_up_sys(len);
    result=(struct ThreadVm  *)__MallocStack(len);
    *stack_size = len;
    if(result==NULL)
    {
        Djy_SaveLastError(EN_MEM_TRIED);   //内存不足，返回错误
        return result;
    }
    memset(result,'d',len);
    result->stack_top = (u32*)((ptu32_t)result+len); //栈顶地址，移植敏感
    result->next = NULL;
    result->stack_size = len - sizeof(struct ThreadVm); //保存栈深度
    result->host_vm = NULL;
    //复位虚拟机并重置线程
    __asm_reset_thread(evtt->thread_routine,result);
    return result;
}

//----set the instruction delay constant varaible-------------------------------
//function:set the instruction delay to constant, which makes the djy_delay_10us
//         realy got an 10us delay,no matter what compiler and options
//para:null
//return:null
//remarks:sensitive to replant
//        use the ticktimer to test the time
//------------------------------------------------------------------------------
void __DjySetDelay(void)
{
   u32 counter,temp, narrow_cfg_mclk,clksum;
    u32 u32_fors=10000;
    volatile uint32_t u32loops;
    clksum=(u32)(CN_CFG_MCLK/1000);    //1000uS
    atom_high_t high_atom;
    high_atom = Int_HighAtomStart();//dont be interrupt by something
    Int_IsrConnect(cn_int_line_TMZLI,SetDelayHandler);  //将中断ISR与中断线相关联
    Int_RestoreAsynLine(cn_int_line_TMZLI);
     while(1)
    {
      timer_set(clksum,clksum);                        //初始计数值和自动加载值均设为clksum
      timer_on();                                      //开启内核时钟
      uf_delay_counter=0;
      for(u32loops=u32_fors;u32loops>0;u32loops--);    //进入for循环
      counter=timer_off();                             //关闭内核时钟
    if(uf_delay_counter!=0)             //若在for循环过程中产生了定时中断，则重新设置clksum，
                                       //直至for循环运行过程不发生定时中断
    {
        temp = (u32)(uf_delay_counter+1) * clksum;
            if(temp < 65536)
            {
                clksum = (u32)temp;
            }
            else
            {
                clksum = 65535;
                u32_fors = u32_fors / (temp / 65535 +1);
            }
        }
        else
        break;
    }
    counter = clksum - counter;    //取实际脉冲数。
    narrow_cfg_mclk=CN_CFG_MCLK/1000000;
    g_u32CycleSpeed = (uint64_t)((counter * 1000) /(u32_fors*narrow_cfg_mclk));//防溢出，用64位
    Int_HighAtomEnd(high_atom);
    return;
}



//----tick service routine------------------------------------------------------
//function:when the tick comes,will goto this exeption to do something
//para:null
//return:null
//remarks:sensitive to replant
//        this use the fixed timer
//        do something to the delay queque,if permiss shedule,the shedule
//        as you know,it is just soso, just do as usual,on the other hand we
//        use asm file to save the current stack,so the function does not compled
//        all the task that tick isr need
//        ppc use the timer except to do the tick ,so no need line
//        compatibal with th old while use line
//-----------------------------------------------------------------------------

void SetDelayHandler(ptu32_t sig_int)
{
    //todo: 优化时加上实际ticks数
    uf_delay_counter++;
    //Djy_IsrTick( 1 )
}




u32 __DjyIsrTick(ptu32_t line)
{
    Djy_IsrTick(1);
    return 0;
}

//----init the tick-------------------------------------------------------------
//function:init the tick,we will use the decrement timer to the tick
//         which means that you can use anytime for the tick
//para:null
//return:null
//remarks:sensitive to replant
//        set the tick isr to corresponding place in the exception table, then
//        set the tick timer to appropriate value, the enable it including the
//        the timer interrupt, just soso
//        as you know,it is just soso, but annoyance that use the asm
//-----------------------------------------------------------------------------
void __DjyInitTick(void)
{
    Int_Register(cn_int_line_TMZLI);
    Int_CutLine(cn_int_line_TMZLI);
    Int_IsrConnect(cn_int_line_TMZLI,__DjyIsrTick);  //将中断ISR与中断线相关联
    Int_RestoreAsynLine(cn_int_line_TMZLI);
    timer_set(CN_CFG_MCLK/1000,CN_CFG_MCLK/1000);     //定时1ms
    timer_on();
    Int_RestoreAsynLine(cn_int_line_TMZLI);
    return;

}

//----读取当前时间(uS)---------------------------------------------------------
//功能：读取当前时间(uS),从计算机启动以来经历的us数，64位，默认不会溢出
//      g_s64OsTicks 为64位变量，非64位系统中，读取 g_s64OsTicks 需要超过1个
//      周期,需要使用原子操作。
//参数：无
//返回：当前时钟
//说明: 这是一个桩函数,被rtc.c文件的 DjyGetSysTime 函数调用
//-----------------------------------------------------------------------------
s64 __DjyGetSysTime(void)
{
    s64 time;
    u32 narrow_cfg_mclk;
    atom_low_t atom_low;
    atom_low = Int_LowAtomStart();
   //此处调用cpu.s中汇编函数_asm_get_tick获得当前TCOUNT值与加载值之差
     core_time=asm_get_tick();
     narrow_cfg_mclk=CN_CFG_MCLK/1000000;
     time = g_s64OsTicks*CN_CFG_TICK_US + (u32)core_time/narrow_cfg_mclk;
     Int_LowAtomEnd(atom_low);
     return time;
}

// =============================================================================
// 函数功能：djy_poweron_time
//          获取上电开始到现在的时间
// 输入参数:
// 输出参数：time，获取的时间，单位是微秒
// 返回值  ：true成功，false失败（一般的是由于无法实现）
// =============================================================================
bool_t DjyPoweronTime(s64 *time)
{
    *time = DjyGetSysTime();
    return true;
}

