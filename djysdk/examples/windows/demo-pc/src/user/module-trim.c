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
#include "board-config.h"
#include "core_config.h"
#include <cfg\IO_config.h>
#include "driver.h"
#include "cpu_peri.h"
#include "uartctrl.h"
#include "gkernel.h"
#include "djyos.h"
#include "timer.h"
#include "lowpower.h"
#include "list.h"
#include "..\heap\heap-in.h"
extern ptu32_t ModuleInstall_DebugInfo(ptu32_t para);

//罗列GDD组件输入设备的名称
static  const char *pGddInputDev[]={

    "sim touch",
    "sim keyboard",
    NULL
};

void    ModuleInstall_GDD(struct GkWinRsc *desktop,const char *InputDevName[]);
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
//---------------------------------------------------------------------------
void Sys_ModuleInit(void)
{
    static struct GkWinRsc desktop;
    struct DisplayRsc *lcd_windows;
    uint16_t evtt_main;
#if 1
    //初始化直接输入和输出的硬件，为stdio.c中定义的 PutStrDirect、GetCharDirect
    //两个指针赋值
    //这是来自bsp的函数，一般是串口驱动,BSP没提供的话，就不要调用，会导致应用程序编译不通过。
 //   Stdio_KnlInOutInit( 0 );
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

    //文件系统模块,依赖:shell
    ModuleInstall_FileSystem();
    //flash文件系统初始化,依赖:文件系统模块,shell模块
//    ModuleInstall_DFFSD(0);
    //nand flash驱动,依赖:flash文件系统模块
//    ModuleInstall_WinFs(NULL);
    //设置工作路径,依赖:文件系统,且相关路径存在.
//    Djyfs_SetWorkPath(gc_pCfgWorkPath);

    ModuleInstall_Cmd(0);

    //打开IO设备,不同的板件,这部分可能区别比较大,不影响printk函数。
    //此后,printf和scanf将使用stdin/out输出和输入。
    //依赖: 若stdin/out/err是文件,则依赖文件系统
    //      若是设备,则依赖设备驱动
    //      同时,还依赖用来输出信息的设施,例如串口,LCD等
    OpenStdin(gc_pCfgStdinName);
    OpenStdout(gc_pCfgStdoutName);
    OpenStderr(gc_pCfgStderrName);
    ModuleInstall_HmiIn( 0 );

    //djybus模块
    ModuleInstall_DjyBus(0);
    //IIC总线模块,依赖:djybus
    ModuleInstall_IICBus(0);
    //SPI总线模块,依赖:djybus
    ModuleInstall_SPIBus(0);

    //日历时钟模块
    ModuleInstall_TM(0);
    //使用硬件RTC,注释掉则使用tick做RTC计时,依赖:日历时钟模块
//    ModuleInstall_RTC(0);

    //定时器组件
    extern ptu32_t ModuleInstall_Timer(ptu32_t para);
    ModuleInstall_Timer(CN_TIMER_SOURCE_TICK);

   //网络协议栈组件
    extern  ptu32_t ModuleInstall_DjyIp(ptu32_t para);
    //协议栈组件初始化
//    ModuleInstall_DjyIp(0);

    //键盘输入模块
    ModuleInstall_KeyBoard(0);
    //键盘输入驱动,依赖:键盘输入模块
    ModuleInstall_KeyBoardWindows(0);

    //字符集模块
    ModuleInstall_Charset(0);
    //gb2312字符编码,依赖:字符集模块
    ModuleInstall_CharsetGb2312(0);
    //ascii字符集,注意,gb2312包含了ascii,初始化了gb2312后,无须本模块
    //依赖:字符集模块
//    ModuleInstall_CharsetAscii(0);
    //初始化utf8字符集
//    ModuleInstall_CharsetUtf8(0);
    //国际化字符集支持,依赖所有字符集模块以及具体字符集初始化
    ModuleInstall_CharsetNls("C");


    ModuleInstall_Font(0);                 //字体模块

    //8*8点阵的ascii字体依赖:字体模块
//    ModuleInstall_FontAscii8x8Font(0);
    //6*12点阵的ascii字体依赖:字体模块
//    ModuleInstall_FontAscii6x12Font(0);
    //从数组安装GB2312点阵字体,包含了8*16的ascii字体.依赖:字体模块
    ModuleInstall_FontGb2312_816_1616_Array(0);
    //从文件安装GB2312点阵字体,包含了8*16的ascii字体.依赖:字体模块,文件系统
//    ModuleInstall_FontGb2312_816_1616_File("sys:\\gb2312_1616");
    //8*16 ascii字体初始化,包含高128字节,依赖:字体模块
    //注:如果安装了GB2312,无须再安装
    ModuleInstall_FonAscii8x16Font(0);

    ModuleInstall_GK(0);           //gkernel模块
    //lcd驱动初始化,如果用系统堆的话,第二个参数用NULL
    //堆的名字,是在lds文件中命名的,注意不要搞错.
    //依赖: gkernel模块
    lcd_windows = (struct DisplayRsc*)ModuleInstall_Lcd("sim_display",NULL);

    GK_ApiCreateDesktop(lcd_windows,&desktop,0,0,
                        CN_COLOR_RED+CN_COLOR_GREEN,CN_WINBUF_BUF,0,0);

    //触摸屏模块,依赖:gkernel模块和显示器驱动
    ModuleInstall_Touch(0);

    //依赖:触摸屏模块,宿主显示器驱动,以及所依赖的硬件,例如qh_1的IIC驱动.
    //     如果矫正数据存在文件中,还依赖文件系统.
    ModuleInstall_TouchWindows(&desktop,"sim touch");

    //看门狗模块,如果启动了加载时喂狗,看门狗软件模块从此开始接管硬件狗.
    extern ptu32_t ModuleInstall_Wdt(ptu32_t para);
//    ModuleInstall_Wdt(0);

    //GDD组件初始化
    ModuleInstall_GDD(&desktop,pGddInputDev);
#endif

    evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                __djy_main,NULL,gc_u32CfgMainStackLen,
                                "main function");
    //事件的两个参数暂设为0?如果用shell启动?可用来采集shell命令行参数
    Djy_EventPop(evtt_main,NULL,0,NULL,0,0);

//    ModuleInstall_LowPower(QH_EntrySleepReCall,QH_ExitSleepReCall);
    //用户自己程序的初始化模块,建议从这里开始调用
#if CN_CFG_DYNAMIC_MEM == 1     //CN_CFG_DYNAMIC_MEM与编译libOS时的设置务必保持一致
    Heap_DynamicModuleInit(0);  //自此malloc函数执行块相联算法
#endif
    //至此,初始化时使用的栈,已经被系统回收,本函数就此结束,否则会死得很难看

    return ;
}


