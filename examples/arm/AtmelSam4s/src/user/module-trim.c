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
#include "os.h"
#include "cpu_peri.h"
#include "uart.h"

ptu32_t djy_main(void);

ptu32_t __djy_main(void)
{
    djy_main();
    return 0;
}

const u32 gc_u32CfgTimeZone = 8;     //本地时区为东8区(北京)

const char *gc_pCfgDefaultCharsetName = "gb2312";  //默认字符集
const char *gc_pCfgDefaultFontName = "gb2312_8_16_1616";  //默认字体

const u32 gc_u32CfgGuiCmdDeep = 1024;   //gkernel命令缓冲区深度(字节数,非命令条数)
const u32 gc_u32CfgGuiStsDeep = 256;    //gkernel状态缓冲区深度(字节数,非命令条数)

const u32 gc_u32CfgDeviceLimit = 5;     //定义设备数量。
const u32 gc_u32CfgLockLimit = 30;      //定义锁数量。用户调用semp_create和mutex_create创建的锁，不包括内核用到的锁。
const u32 gc_u32CfgEventLimit = 15;     //事件数量
const u32 gc_u32CfgEvttLimit = 15;      //总事件类型数
const u32 gc_u32CfgWdtLimit = 15;       //允许养狗数量
const u32 gc_u32CfgPipeLimit = 1;       //管道数量
const u32 gc_u32CfgMemPoolLimit = 10;    //允许建立10个内存池
const u32 gc_u32CfgStdinDeviceLimit = 2;
const u32 gc_u32CfgMainStackLen = 1024;

const u32 gc_u32CfgOpenedfileLimit = 16;
const u32 gc_u32CfgTimerLimit = 5;

const char *gc_pCfgStddevName = "UART0";  //标准输入输出设备
const char *gc_pCfgWorkPath = "sys:\\";   //工作路径设置


//----操作系统运行参数配置-----------------------------------------------------
//功能: 配置操作系统运行参数,例如锁的数量/事件数量等.使用文本文件配置时使用。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Sys_ConfigRunParam(char *CfgfileName)
{
}
void Stddev_KnlInOutInit(u32 para)  //本函数暂未实现
{

}
//----组件初始化2-----------------------------------------------------------
//功能：可选组件初始化函数共有3个:
//      1、sys_module_init，在sys_init.c中，参见该函数注释
//      2、prj_module_init，在这里初始化的模块，需要调用跟具体工程相关的代码，例
//          如键盘模块，除了调用module_init_keyboard函数外，还要调用跟硬件相关的
//          module_init_keyboard_hard函数。
//      前两步初始化时，块相联分配方式还没有初始化，驱动中如果用到动态分配，使用
//      的是准静态分配，关于准静态分配的说明，参见mems.c文件。
//      3、run_module_init，参见该函数注释。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void Sys_ModuleInit(void)
{
	tagDevHandle char_term_hdl;
	u32 stdinout;
	uint16_t evtt_main;

	//初始化PutChark,Putsk,GetChark,GetsK四个内核级输入输出函数,此后,printk
	//可以正常使用.printf和scanf将调用前述4个函数执行输入输出
	Stddev_KnlInOutInit( 0 );
	//shell模块,依赖:无
	Sh_MoudleInit(0);
	//文件系统模块,依赖:shell
	//    Djyfs_ModuleInit(0);
	//设备驱动模块
	Driver_ModuleInit(0);
	//多路复用模块,提供类似Linux的epoll、select的功能
	Multiplex_ModuleInit(0);

	//提供在shell上输出内核信息的功能,依赖:shell模块
	Debug_InfoInit(0);
	//异常监视模块,依赖:shell模块
	Exp_ModuleInit(0);
	//安装异常信息解析器,解析异常模块记录的二进制数据
	Exp_InfoDecoderInit( );

	UART_ModuleInit(CN_UART0);
	UART_ModuleInit(CN_UART1);

	//标准IO初始化,不影响printk函数,此后,printf和scanf将使用stdin/out输出和输
	//入,如果裁掉,将一直用PutChark,Putsk,GetChark,GetsK执行IO
	//依赖: 若stdin/out/err是文件,则依赖文件系统
	//      若是设备,则依赖设备驱动
	//      同时,还依赖用来输出信息的设施,例如串口,LCD等
	Stddev_ModuleInit( 0 );

	//打开IO设备,不同的板件,这部分可能区别比较大.
	stdinout = Driver_FindDevice(gc_pCfgStddevName);
	char_term_hdl = Driver_OpenDeviceAlias(stdinout,O_RDWR,0);
	if(char_term_hdl != NULL)
	{
		Driver_CtrlDevice(char_term_hdl,CN_UART_START,0,0);
		//设置串口波特率为115200，
		Driver_CtrlDevice(char_term_hdl,CN_UART_SET_BAUD,115200,0);
		CharTerm_ModuleInit((ptu32_t)stdinout);
		Driver_CloseDevice(char_term_hdl);
	}

	//djybus模块
	DjyBus_ModuleInit(0);
	//IIC总线模块,依赖:djybus
	ModuleInit_IICBus(0);
	//SPI总线模块,依赖:djybus
	ModuleInit_SPIBus(0);
	SPI_Init();
	IIC0_Init();
//	IIC1_Init();

	//日历时钟模块
//	TM_ModuleInit(0);
	//使用硬件RTC,注释掉则使用tick做RTC计时,依赖:日历时钟模块
//	RTC_ModuleInit(0);

	//定时器组件
//	module_init_timersoft(0);

	//键盘输入模块
	//    Keyboard_ModuleInit(0);
	//键盘输入驱动,依赖:键盘输入模块
	//    module_init_keyboard_hard(0);

	//字符集模块
//	Charset_ModuleInit(0);
	//gb2312字符编码,依赖:字符集模块
//	Charset_Gb2312ModuleInit(0);
	//ascii字符集,注意,gb2312包含了ascii,初始化了gb2312后,无须本模块
	//依赖:字符集模块
	//    Charset_AsciiModuleInit(0);
	//初始化utf8字符集
	//    Charset_Utf8ModuleInit(0);
	//国际化字符集支持,依赖所有字符集模块以及具体字符集初始化
//	Charset_NlsModuleInit("C");

//	Font_ModuleInit(0);                 //字体模块
	//8*8点阵的ascii字体依赖:字体模块
	//    Font_Ascii8x8FontModuleInit(0);
	//6*12点阵的ascii字体依赖:字体模块
	//    Font_Ascii6x12FontModuleInit(0);
	//从数组安装GB2312点阵字体,包含了8*16的ascii字体.依赖:字体模块
	//    Font_Gb2312_816_1616_ArrayModuleInit(0);
	//从文件安装GB2312点阵字体,包含了8*16的ascii字体.依赖:字体模块,文件系统
	//    Font_Gb2312_816_1616_FileModuleInit("sys:\\gb2312_1616");
	//8*16 ascii字体初始化,包含高128字节,依赖:字体模块
	//注:如果安装了GB2312,无须再安装
//	Font_Ascii8x16FontModuleInit(0);


	//看门狗模块,如果启动了加载时喂硬件狗,看门狗软件模块从此开始接管硬件狗.
//	module_init_wdt(0);

	//事件的两个参数暂设为0,如果用shell启动,可用来采集shell命令行参数
	evtt_main = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,__djy_main,
						   NULL,gc_u32CfgMainStackLen,"main function");
	//事件的两个参数暂设为0，如果用shell启动，可用来采集shell命令行参数
	Djy_EventPop(evtt_main,NULL,0,NULL,0,200);

	Heap_DynamicModuleInit(0);//自此malloc函数执行块相联算法
	//至此,初始化时使用的栈,已经被系统回收,本函数就此结束,否则会死得很难看
	return ;
}


