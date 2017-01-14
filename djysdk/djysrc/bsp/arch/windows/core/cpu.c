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
//3. 日期:2013-2-7
//   作者:  罗侍田.
//   新版本号：1.0.1
//   修改说明: 修正了__asm_reset_switch函数的一处错误，该bug由裴乐提交
//1. 日期: 2012-10-20
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include <windows.h>
#include <tchar.h>
//#include <process.h>
#include "align.h"
#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>
#include "int.h"
#include "hard-exp.h"
#include "string.h"
#include "arch_feature.h"
#include "cpu.h"
#include "djyos.h"
//#pragma comment(lib,"Winmm.lib")
//#pragma comment (lib,"ntdll_32.lib")       // Copy From DDK  64位系统改为ntdll_64.lib

// Windows上仿真堆,其定义与嵌入式环境不太一样,嵌入式环境中的定义,请参考官方发布
// 的代码任意一个*.lds文件.
struct WinHeap
{
    u32 CessionNum;                //该堆由两段内存组成
    u32 HeapAlign;                 //该堆上分配的内存的对齐尺寸，0表示使用系统对齐
    u32 property;                  //0=专用堆,1=专用堆,如果系统只有唯一一个堆,则只能是通用堆
    u32 HeapBottom;                //段基址，须符合对齐要求
    u32 HeapTop;                   //段地址上限（不含），须符合对齐要求
    u32 PageSize;                  //页尺寸=32bytes
    char  name[5];                   //堆的名字,须是“sys”
};

struct WinHeap pHeapList;

//struct WinHeap *pHeapList;             //在脚本中定义

extern ufast_t ufg_int_number;

extern u32  g_s64OsTicks;             //操作系统运行ticks数
u32 g_u32CycleSpeed = 100; //for(i=j;i>0;i--);每循环纳秒数*1.024
s64 s64g_freq;
extern struct EventECB  *g_ptEventReady;      //就绪队列头
extern struct EventECB  *g_ptEventRunning;    //当前正在执行的事件
u8 DjyosHeap[0x1000000];   //16M内存
u32 WINAPI win_engine( LPVOID lpParameter );

u32 WINAPI win32_switch( LPVOID lpParameter );

HINSTANCE hInstance = NULL;

extern HANDLE win32_int_event;
extern HANDLE win32_int_engine;
HANDLE old_id,new_id;

u32 switch_context(ptu32_t line)
{
    SuspendThread(old_id);
    ResumeThread(new_id);
    Int_ContactAsynSignal();
    return 0;
}

ptu32_t Heap_StaticModuleInit(ptu32_t para);
void Sys_Start(void);

void main(int argc, char *argv[])
{
    MSG msg;
    LARGE_INTEGER litmp;

    SetProcessAffinityMask(GetCurrentProcess(), 1);
    hInstance = GetModuleHandle(NULL);

    pHeapList.CessionNum = 1;
    pHeapList.HeapAlign = 0;
    pHeapList.property = 0;
    pHeapList.HeapBottom = (u32)DjyosHeap;
    pHeapList.HeapTop = pHeapList.HeapBottom + sizeof(DjyosHeap);
    pHeapList.PageSize = 128;
    strcpy(pHeapList.name,"sys");
    pHeapList.name[4] = '\0';
//  a = CreateDirectory(_TEXT("djyfs"),NULL);
//  a = CreateDirectory(_TEXT("djyfs\\sys"),NULL);
    Heap_StaticModuleInit(0);
    Int_Init();
    QueryPerformanceFrequency(&litmp);
    s64g_freq = litmp.QuadPart;
    Int_Register(cn_int_line_switch_context);
    Int_IsrConnect(cn_int_line_switch_context,switch_context);
    Int_SettoAsynSignal(cn_int_line_switch_context); //tick中断被设为异步信号
    Int_RestoreAsynLine(cn_int_line_switch_context);
//    printf("VC 编译器版本：%d\n\r",_MSC_VER);
    Sys_Start();
// 主消息循环:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return ;
}


ATOM RegisterWinClass(HINSTANCE hInstance,WNDPROC *func,TCHAR *title)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)func;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = NULL;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = title;
    wcex.hIconSm        = NULL;

    return RegisterClassEx(&wcex);


}

//----创建线程-----------------------------------------------------------------
//功能：为事件类型创建伪虚拟机，初始化上下文环境，安装执行函数，构成完整线程
//参数：evtt_id，待创建的线程所服务的事件类型id
//返回：新创建的线程的虚拟机指针
//注: 移植敏感函数
//-----------------------------------------------------------------------------
struct ThreadVm *__CreateThread(struct EventType *evtt,u32 *stack_size)
{
    struct ThreadVm  *result;
    //stack_size和cn_kernel_stack在windows版本中并没有用到，windows的线程栈是自己维护
    //的，这样写比较接近实际cpu平台。
    *stack_size = evtt->stack_size+sizeof(struct ThreadVm);
    result=(struct ThreadVm  *)__MallocStack(*stack_size);
    if(NULL == result)
    {
        return result;
    }
    else
    {
        memset(result,0,sizeof(struct ThreadVm));
    }
    result->stack_top = CreateThread( NULL, 0, win_engine, evtt->thread_routine,
                                        CREATE_SUSPENDED, NULL );
//    SetThreadAffinityMask( result->stack_top, 1 );

    result->stack = 0;
    result->next = NULL;
    result->stack_size = 0; //保存栈深度
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
struct ThreadVm *__CreateStaticThread(struct EventType *evtt,void *Stack,
                                    u32 StackSize)
{
    struct ThreadVm  *result;

    result = (struct ThreadVm  *)align_up_sys(Stack);

    memset(Stack, 'd', StackSize-((ptu32_t)result - (ptu32_t)Stack));

    //看实际分配了多少内存，djyos内存分配使用块相联策略，如果分配的内存量大于
    //申请量，可以保证其实际尺寸是对齐的。之所以注释掉，是因为当len大于申请量时，
    //对齐只是实际结果，而不是内存管理的规定动作，如果不注释掉，就要求内存管理
    //模块必须提供对齐的结果，对模块独立性是不利的。
//    len = M_CheckSize(result);

    result->stack_top = CreateThread( NULL, 0, win_engine, evtt->thread_routine,
                                        CREATE_SUSPENDED, NULL );
    result->stack = 0;
    result->next = NULL;
    result->stack_size = 0; //保存栈深度
    result->host_vm = NULL;
    //复位线程并重置线程
    __asm_reset_thread(evtt->thread_routine,result);
    return result;
}

//----重置线程-----------------------------------------------------------------
//功能:  初始化线程的栈，并挂上线程执行函数，新创建线程时将会调用本函数
//参数:  thread_routine，被重置的线程函数
//      vm，线程虚拟机指针
//返回:  初始化结束后的当前栈指针
//函数原型:void * __asm_reset_thread(void (*thread_routine)(void),
//                                   struct ThreadVm  *vm);
//-----------------------------------------------------------------------------
void * __asm_reset_thread(ptu32_t (*thread_routine)(void),struct ThreadVm  *vm)
{
    return NULL;
}

//----复位老线程，切换到新线程-------------------------------------------------
//功能:  把old_vm的上下文重新初始化到新创建的状态，然后切换到新线程的
//      上下文中继续执行。
//当一个在常驻内存的虚拟机中处理的事件完成后，不能删除该虚拟机，必须复位
//      该虚拟机，然后切换到其他就绪线程，这时候就需要调用本函数。因此本函数在
//      被复位的线程上下文里面执行。
//参数:  thread_routine，被重置的线程函数
//      new_vm，目标线程
//      old_vm，被复位线程
//返回:  无
//函数原型:void __asm_reset_switch(void (*thread_routine)(void),
//                           struct ThreadVm *new_vm,struct ThreadVm *old_vm);
//-----------------------------------------------------------------------------
void __asm_reset_switch(ptu32_t (*thread_routine)(void),
                           struct ThreadVm *new_vm,struct ThreadVm *old_vm)
{
    Int_HalfEnableAsynSignal();
    old_vm->stack_top = CreateThread( NULL, 0, win_engine, thread_routine,
                                        CREATE_SUSPENDED, NULL );
    old_vm->stack = 0;
    old_vm->next = NULL;
    old_vm->stack_size = 0; //保存栈深度
    old_vm->host_vm = NULL;

    ResumeThread((HANDLE )new_vm->stack_top);
    Int_ContactAsynSignal();
    _endthreadex(0);
}


void __asm_start_thread(struct ThreadVm  *new_vm)
{
    ResumeThread(win32_int_engine);
    __asm_turnto_context(new_vm);
}

//----切入上下文---------------------------------------------------------------
//功能:  不保存原上下文，直接切入新的上下文执行
//参数:  new_sp，新上下文的栈指针
//返回:  无
//函数原型: void __asm_turnto_context(struct ThreadVm  *new_vm);
//说明:  当事件完成,就没有必要保存旧事件的上下文,直接切换到新事件即可.
//-----------------------------------------------------------------------------
void __asm_turnto_context(struct ThreadVm  *new_vm)
{
    Int_HalfEnableAsynSignal();
    ResumeThread((HANDLE )new_vm->stack_top);
    Int_ContactAsynSignal();
}

//----上下文切换---------------------------------------------------------------
//功能:  保存当前线程的上下文，切换到新线程的上下文中继续执行。
//参数:  new_sp，新上下文的栈指针
//参数:  old_sp，旧上下文的栈指针的指针，即&vm->stack。无需提供旧上下文栈指针，
//               sp寄存器的当前值就是
//返回:  无
//函数原型: void __asm_switch_context(struct ThreadVm *new_vm,struct ThreadVm *old_vm);
//-----------------------------------------------------------------------------
void __asm_switch_context(struct ThreadVm *new_vm,struct ThreadVm *old_vm)
{
    old_id = (HANDLE )old_vm->stack_top;
    new_id = (HANDLE )new_vm->stack_top;
    ufg_int_number = cn_int_line_switch_context;
    SetEvent(win32_int_event);

}

//----中断中的上下文切换-------------------------------------------------------
//功能:  保存被中断线程的上下文，切换到新线程的上下文中继续执行。本函数虽然在中
//      断服务函数(非用户用int_isr_connect函数连接的中断服务函数)中，但在ARM
//      中，却运行在svc态
//参数:  new_sp，切换目标虚拟机
//参数:  old_sp，被中断线程虚拟机
//返回:  无
//函数原型: void __asm_switch_context_int(struct ThreadVm *new_vm,struct ThreadVm *old_vm);
//-----------------------------------------------------------------------------
void __asm_switch_context_int(struct ThreadVm *new_vm,struct ThreadVm *old_vm)
{
    SuspendThread((HANDLE )old_vm->stack_top);
    ResumeThread((HANDLE )new_vm->stack_top);
}

void __Djy_VmEngine(ptu32_t (*thread_routine)(void));

u32 WINAPI win_engine( LPVOID lpParameter )
{
    void (*thread_routine)(void);

    thread_routine = (void (*)(void)) lpParameter;
    __Djy_VmEngine(thread_routine);
    return 0;
}



static volatile ufast_t uf_delay_counter;
u32 SetDelayIsr(ufast_t timer_int_line)
{
    uf_delay_counter++;
    return 0;
}

//----设置指令延时常数---------------------------------------------------------
//功能: 设置指令延时常数,使y_delay_10us函数的延时常数=10uS，不管用何种编译器和
//      编译优化选项
//参数：无
//返回: 无
//备注: 本函数移植敏感
//-----------------------------------------------------------------------------
void __DjySetDelay(void)
{
//x86 用QueryPerformanceFrequency函数和QueryPerformanceCounter函数测量10000个for
//循环时间，然后计算。这两函数的使用范例：
#if 0
在进行定时之前，先调用QueryPerformanceFrequency()函数获得机器内部定时器的时钟频率，
然后在需要严格定时的事件发生之前和发生之后分别调用QueryPerformanceCounter()函数，
利用两次获得的计数之差及时钟频率，计算出事件经 历的精确时间。下列代码实现1ms的
精确定时：
#endif

}

void CALLBACK TimerCallBack(UINT uTimerID, UINT uMsg, DWORD dwUser,
                            DWORD dw1, DWORD dw2)
{
    Int_TapLine(cn_int_line_timer_event);
}

u32 __DjyIsrTick(ptu32_t line)
{
    Djy_IsrTick(1);
    return 0;
}

//----初始化tick---------------------------------------------------------------
//功能: 初始化定时器,并连接tick中断函数,启动定时器.
//参数: 无
//返回: 无
//备注: 本函数是移植敏感函数.
//-----------------------------------------------------------------------------
void __DjyInitTick(void)
{
    Int_Register(cn_int_line_timer_event);
    Int_IsrConnect(cn_int_line_timer_event,__DjyIsrTick);
    Int_SettoAsynSignal(cn_int_line_timer_event); //tick中断被设为异步信号
    Int_RestoreAsynLine(cn_int_line_timer_event);
    timeSetEvent(CN_CFG_TICK_US/mS,CN_CFG_TICK_US/mS,&TimerCallBack,0,TIME_PERIODIC);
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
    LARGE_INTEGER litmp;
    s64 cnt;
    s64 time;
    atom_low_t atom_low;
    atom_low = Int_LowAtomStart();

    QueryPerformanceCounter(&litmp);
    cnt = litmp.QuadPart;
    time = (u32)(cnt*1000000/s64g_freq);
    Int_LowAtomEnd(atom_low);
    return time;// (((u32)CN_CFG_TICK_US*1000/cn_fine_ns)-pg_timer_reg->TCNTO3) *cn_fine_ns;
}

void __asm_reset_cpu(void)
{
}
void __asm_reset_cpu_hot(void)
{
}
void restart_system(void)
{
}
void reboot(void)
{
}
void reset(void)
{
}
