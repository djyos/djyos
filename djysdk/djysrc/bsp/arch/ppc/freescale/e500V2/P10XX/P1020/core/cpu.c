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
// 模块描述: 为DJYOS准备CPU级的功能：线程的切换等
// 模块版本: V1.00(初始化版本)
// 创建人员:zhangqf_cyg
// 创建时间:2013-7-26--上午10:15:29
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注：该文件的对外接口都是移植敏感函数
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"

#include "os.h"

#include "config-prj.h"
#include "E500v2RegBits.h"
extern s64  g_s64OsTicks;             //操作系统运行ticks数
u32 g_u32CycleSpeed; //for(i=j;i>0;i--);每循环纳秒数*1.024
void Exp_ConnectSystick(void (*tick)(void));
extern void __asm_init_Dtimer(u32 value_tick);
extern void __asm_get_tb_value(u32 *value_h32, u32 *value_l32);
extern void __asm_reset_the_tb(void);
extern ptu32_t __asm_reset_thread(ptu32_t (*thread_routine)(void),
                           struct  tagThreadVm  *vm);


// =============================================================================
// 函数功能:__CreateThread
//          创建线程
// 输入参数:evtt，线程关联的evtt
// 输出参数:stack_size，创建的VM的stack的大小
// 返回值  :
// 说明    :将evtt中的内容恢复到创建的VM当中
// =============================================================================
struct  tagThreadVm *__CreateThread(struct  tagEventType *evtt,u32 *stack_size)
{
    struct  tagThreadVm  *result;
    ptu32_t  len;

    //计算虚拟机栈:线程+最大单个api需求的栈
    len = evtt->stack_size+CN_KERNEL_STACK+sizeof(struct  tagThreadVm);
    //栈顶需要对齐，malloc函数能保证栈底是对齐的，对齐长度可以使栈顶对齐
    len = align_up_sys(len);
    result=(struct  tagThreadVm  *)(__MallocStack(len));
    *stack_size = len;
    if(result==NULL)
    {
        Djy_SaveLastError(EN_MEM_TRIED);   //内存不足，返回错误
        return result;
    }
#if CN_CFG_STACK_FILL != 0
    memset(result,CN_CFG_STACK_FILL,len);
#endif
    result->stack_top = (u32*)((ptu32_t)result+len); //栈顶地址，移植敏感
    result->next = NULL;
    result->stack_size = len - sizeof(struct tagThreadVm); //保存栈深度
    result->host_vm = NULL;
    //复位虚拟机并重置线程
    __asm_reset_thread(evtt->thread_routine,result);
    return result;
}

//----静态创建线程-------------------------------------------------------------
//功能：为事件类型创建线程，初始化上下文环境，安装执行函数，构成完整线程
//参数：evtt_id，待创建的线程所服务的事件类型id
//返回：新创建的线程指针
//注: 移植敏感函数
//-----------------------------------------------------------------------------
struct  tagThreadVm *__CreateStaticThread(struct  tagEventType *evtt,void *Stack,
                                    u32 StackSize)
{
    struct  tagThreadVm  *result;

    result = (struct  tagThreadVm  *)align_up_sys(Stack);

#if CN_CFG_STACK_FILL != 0
    memset(Stack,CN_CFG_STACK_FILL,StackSize);
#endif

    //看实际分配了多少内存，djyos内存分配使用块相联策略，如果分配的内存量大于
    //申请量，可以保证其实际尺寸是对齐的。之所以注释掉，是因为当len大于申请量时，
    //对齐只是实际结果，而不是内存管理的规定动作，如果不注释掉，就要求内存管理
    //模块必须提供对齐的结果，对模块独立性是不利的。
//    len = M_CheckSize(result);
    result->stack_top = (u32 *)(align_down_sys((ptu32_t)Stack+StackSize)); //栈顶地址，移植敏感
    result->next = NULL;
    result->stack_size = (ptu32_t)(result->stack_top) - (ptu32_t)result
                            - sizeof(struct  tagThreadVm);       //保存栈深度
    result->host_vm = NULL;
    //复位线程并重置线程
    __asm_reset_thread(evtt->thread_routine,result);
    return result;
}
// =============================================================================
// 函数功能:__DjySetDelay
//          测试for循环速度
// 输入参数:无
// 输出参数:无
// 返回值  :
// 说明    :
// =============================================================================
void __DjySetDelay(void)
{
    uint32_t counter_h,counter_l;
    volatile uint32_t u32loops;
    atom_high_t high_atom;
    //maybe we need to close the int key
    high_atom = Int_HighAtomStart();//dont be interrupt by something
    __asm_reset_the_tb();

    for(u32loops=0;u32loops<10240;u32loops++);

    __asm_get_tb_value(&counter_h, &counter_l);

    Int_HighAtomEnd(high_atom);
    //actually, the counter_l is enough

    //counter is the time run during the u32_fors loops
    //use 64 bits to avoid overflow
    g_u32CycleSpeed = counter_l*cfg_core_tb_clk_time;//ns

    g_u32CycleSpeed = g_u32CycleSpeed/10000;//每次需要的纳秒数

    return;
}
// =============================================================================
// 函数功能:__DjyIsrTick
//          tick的isr
// 输入参数:无
// 输出参数:无
// 返回值  :
// 说明    :该函数主要是用来完成delay队列的计算
// =============================================================================
void __DjyIsrTick(void)
{
    //todo: 优化时加上实际ticks数
    Djy_IsrTick( 1 );
}

// =============================================================================
// 函数功能:__DjyInitTick
//          tick的初始化
// 输入参数:无
// 输出参数:无
// 返回值  :
// 说明    :tick使用的是我们的递减计数器（E500V2）
// =============================================================================
void __DjyInitTick(void)
{
    //here timer is not in the external interrupt, on the other hand it will
    //send an timer exception.
    //better to the asm to complete this
    //compute the value that used for the tick time
    u32 counter_value;

    //使用的是CORE implete clk, ccb/8
    counter_value =cfg_core_tb_clk/CN_CFG_TICK_HZ;//compute the count number

    Exp_ConnectSystick(__DjyIsrTick);//connect to the tick server

    __asm_init_Dtimer(counter_value);

    return;

    //at the same time we need to init the tick timer
}
//=============================================================================
//功能:读取当前时间(uS),从计算机启动以来经历的us数，64位，默认不会溢出
//     g_s64OsTicks 为64位变量，非64位系统中，读取 g_s64OsTicks 需要超过1个
//     周期,需要使用原子操作。
//     该时间为TBUTBL的组成的S64时间，该时间从上电开始计算，比TICK更精确
//参数:无
//输出:无
//返回:64位时间
//说明: 这是一个桩函数,被rtc.c文件的 DjyGetTime 函数调用
// =============================================================================
s64 __DjyGetTime(void)
{
//PPC use the TB as the 64 bits time, so it could lasts many many years when
//it turns back
    u32 time_high;
    u32 time_low;
    u64 time_need;
    u32 time_div;
    
    __asm_get_tb_value(&time_high, &time_low);

    time_need = ((u64)time_high<<32)|(time_low);
    time_div = cfg_core_tb_clk/1000/1000;

    time_need = time_need/time_div;
    
    return time_need;
}


// =============================================================================
// 函数功能:LockSysCode
//          系统代码段写保护
// 输入参数:无
// 输出参数:无
// 返回值  :
// 说明    :P1020的前64MB为代码段，在此做写保护
// =============================================================================
void LockSysCode(void)
{
	u32 property;
	//protect the mem code from overwriting
	property = __AsmGetTlbMas3Pro(CN_DDR_CODEADDR);
	property = property &(~MAS3_SW);
	__AsmSetTlbMas3Pro(property, CN_DDR_CODEADDR);

	return;
}


