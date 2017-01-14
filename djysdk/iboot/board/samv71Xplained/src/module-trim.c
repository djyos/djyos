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
#include "stdint.h"
#include "stdio.h"
#include "driver.h"
#include "cpu_peri.h"
#include "cpu_peri_uart.h"
#include "uartctrl.h"
#include "gkernel.h"
#include "djyos.h"
#include "core_config.h"
#include <cfg/io_config.h>
#include "timer.h"
#include "lowpower.h"
#include "list.h"
#include "..\heap\heap-in.h"
#include "ymodem.h"

extern ptu32_t ModuleInstall_DebugInfo(ptu32_t para);

extern ptu32_t ModuleInstall_IAP(void);

u32 QH_EntrySleepReCall(u32 SleepLevel);
u32 QH_ExitSleepReCall(u32 SleepLevel);

ptu32_t djy_main(void);

ptu32_t __djy_main(void)
{
    djy_main();
    return 0;
}

//----操作系统运行参数配置-----------------------------------------------------
//功能: 配置操作系统运行参数,例如锁的数量/事件数量等.使用文本文件配置时使用。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Sys_ConfigRunParam(char *CfgfileName)
{
}

//----操作系统内核组件配置-----------------------------------------------------
//功能：配置和初始化可选功能组件，在用户工程目录中必须实现本函数，在内核初始化
//      阶段调用。
//      本函数实现内核裁剪功能，例如只要注释掉
//          ModuleInstall_Multiplex(0);
//      这一行，多路复用组件就被裁剪掉了。
//      用户可从example中copy本文件，把不要的组件注释掉，强烈建议，不要删除,也
//      不要修改调用顺序。可以把用户模块的初始化代码也加入到本函数中,建议跟在
//      系统模块初始化后面.
//      有些组件有依赖关系,裁剪时,注意阅读注释.
//参数：无
//返回：无
//-----------------------------------------------------------------------------

void Sys_ModuleInit(void)
{
    uint16_t evtt_main;

    Board_GpioInit();
    MoudleInit_Systime(0);
    //初始化直接输入和输出的硬件，为stdio.c中定义的 PutStrDirect、GetCharDirect
    //两个指针赋值，也可以只为PutStrDirect赋值，以支持printk。
    //这是来自bsp的函数，一般是串口驱动,BSP没提供的话，就不要调用，会导致应用程序编译不通过。
    Stdio_KnlInOutInit( 0 );
   //shell模块,依赖:无
    ModuleInstall_Sh(0);
    //设备驱动模块
    ModuleInstall_Driver(0);
    //多路复用模块,提供类似Linux的epoll、select的功能
    ModuleInstall_Multiplex(0);
    //消息队列模块
    ModuleInstall_MsgQ(0);

    //提供在shell上输出内核信息的功能,依赖:shell模块
    ModuleInstall_DebugInfo(0);

    //异常监视模块,依赖:shell模块
    ModuleInstall_Exp(0);

    //ModuleInstall_NAND("nand", 1, 0);
    ModuleInstall_FileSystem();
    //flash文件系统初始化,依赖:文件系统模块,shell模块

    ModuleInstall_UART(CN_USART0);
    ModuleInstall_UART(CN_USART1);
    ModuleInstall_UART(CN_UART3);

    //打开IO设备,不同的板件,这部分可能区别比较大,不影响printk函数。
    //此后,printf和scanf将使用stdin/out输出和输入。
    //依赖: 若stdin/out/err是文件,则依赖文件系统
    //      若是设备,则依赖设备驱动
    //      同时,还依赖用来输出信息的设施,例如串口,LCD等
    OpenStdin(gc_pCfgStdinName);
    OpenStdout(gc_pCfgStdoutName);
    OpenStderr(gc_pCfgStderrName);
    //一下三个函数，如果stdin、stdout、stderr使用不同的串口或其他IO通道，则要
    //分别设定参数。
    Driver_CtrlDevice(ToDev(stdin),CN_UART_SET_BAUD,115200,0);
    Driver_CtrlDevice(ToDev(stdin),CN_UART_START,0,0);

	ModuleInstall_Ymodem(0);
	Ymodem_PathSet("/iboot");
	ModuleInstall_IAP();

    ModuleInstall_DjyBus(0);
    ModuleInstall_IICBus(0);

    //日历时钟模块
    ModuleInstall_TM(0);
    ModuleInstall_CpuRtc(0);

    ModuleInstall_Timer(CN_TIMER_SOURCE_HARD);
//    extern ptu32_t ModuleInstall_NetStaticIP(ptu32_t para);
//    ModuleInstall_NetStaticIP(0);

    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                __djy_main,NULL,gc_u32CfgMainStackLen,
                                "main function");
    //事件的两个参数暂设为0,如果用shell启动,可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0);

//    ModuleInstall_LowPower(QH_EntrySleepReCall,QH_ExitSleepReCall);

    //用户自己程序的初始化模块,建议从这里开始调用
#if CN_CFG_DYNAMIC_MEM == 1     //CN_CFG_DYNAMIC_MEM与编译libOS时的设置务必保持一致
    Heap_DynamicModuleInit(0);  //自此malloc函数执行块相联算法
#endif
    //至此,初始化时使用的栈,已经被系统回收,本函数就此结束,否则会死得很难看

    return ;
}


