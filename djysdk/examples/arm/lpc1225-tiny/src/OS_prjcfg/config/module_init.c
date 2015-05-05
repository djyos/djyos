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
#include "os_inc.h"
#include "cpu_peri.h"

ptu32_t djy_main(void);

ptu32_t __djy_main(void)
{
    djy_main();
    return 0;
}
//----组件初始化2-----------------------------------------------------------
//功能：可选组件初始化函数共有3个:
//      1、sys_module_init，在sys_init.c中，参见该函数注释
//      2、prj_module_init，在这里初始化的模块，需要调用跟具体工程相关的代码，例
//         如键盘模块，除了调用module_init_keyboard函数外，还要调用跟硬件相关的
//         module_init_keyboard_hard函数。
//      前两步初始化时，块相联分配方式还没有初始化，驱动中如果用到动态分配，使用
//      的是准静态分配，关于准静态分配的说明，参见mems.c文件。
//      3、run_module_init，参见该函数注释。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void prj_module_init(void)
{
    uint16_t evtt_main;

#if CN_CFG_KEYBOARD == 1
    Keyboard_ModuleInit(0);
#endif

#if CN_CFG_DJYFS == 1
    Djyfs_ModuleInit(0);
#if CN_CFG_DJYFS_FLASH == 1
    DFFSD_ModuleInit();
#endif
#endif

#if(CN_CFG_UART0_USED == 1)
    module_init_uart0(0);
#endif
#if(CN_CFG_UART1_USED == 1)
    module_init_uart1(0);
#endif
    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
                                __djy_main,CN_MAIN_STACK,"main function");
    //事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0,0);
    return ;
}

//----组件初始化3-----------------------------------------------------------
//功能：可选组件初始化函数共有3个:
//      1、sys_module_init，在sys_init.c中，参见该函数注释
//      2、prj_module_init，参见该函数注释。
//      3、run_module_init，本函数在djy_main函数开始处调用，需要操作系统的事件
//          调度开始后，或者需要块相联内存分配初始化后才能初始化的模块。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void run_module_init(void)
{

}

