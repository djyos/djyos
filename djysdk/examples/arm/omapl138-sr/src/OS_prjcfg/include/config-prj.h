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
// 这份授权条款，在使用者符合以下二条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、此三条件表列，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、此三条件表列，以及下述
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

#ifndef __CONFIG_PRJ_H_
#define __CONFIG_PRJ_H_

#ifdef __cplusplus
extern "C" {
#endif


/*____与硬件相关的配置____*/
#define Mhz 1000000
#define CN_CFG_PLL0_SYSCLK1 (400*Mhz)
#define CN_CFG_PLL0_SYSCLK2 (CN_CFG_PLL0_SYSCLK1/2)
#define CN_CFG_PLL0_SYSCLK4 (CN_CFG_PLL0_SYSCLK1/4)
#define CN_CFG_PLL0_SYSCLK6 (CN_CFG_PLL0_SYSCLK1)
#define CN_CFG_PLL0_SYSCLK3 (100*Mhz)
#define CN_CFG_PLL0_SYSCLK7 (50*Mhz)
#define CN_CFG_PLL1_SYSCLK1 (150*Mhz)
#define CN_CFG_PLL1_SYSCLK2 (150*Mhz)
#define CN_CFG_MCLK CN_CFG_PLL0_SYSCLK1  //主频
#define CN_CFG_AUX_CLK (25*Mhz)  //timer、usb、iic、rtc、mcasp时钟
#define CN_CFG_TIMER_CLK CN_CFG_AUX_CLK  //定时器时钟源
#define CN_CFG_DDRII_CLK 150  //以M为单位，omapl138最高支持150M

/*____定义tick参数____*/
#define CN_CFG_TICK_US 1000  //操作系统内核时钟脉冲长度，以us为单位。
#define CN_CFG_TICK_HZ 1000  //内核时钟频率，单位为hz。
#define CN_CFG_FINE_US 0x00000a3d  //40nS,精密时钟脉冲长度，以uS为单位，32位定点数整数、小数各占16位，这也限制了ticks最长不超过65535uS
#define CN_CFG_FINE_HZ (25*Mhz)  //内核精密时钟频率，是cn_fine_ns的倒数。

/*____时间相关设置____*/
#define CN_CFG_TIME_MODULE 1  //是否包含时间模块
#define CN_CFG_TIME_ZONE (+8)  //本地时区
#define CN_CFG_TIME_SRC_TICK 0  //内核嘀嗒脉冲做走时基准
#define CN_CFG_TIME_SRC_RTC 1  //硬件rtc做走时基准
#define CN_CFG_TIME_SRC _tick  //选择走时基准

/*____cpu运行模式定义____*/
#define CN_MODE_TINY 0  //tiny模式
#define CN_MODE_SI 1  //单映像模式
#define CN_MODE_DLSP 2  //动态加载单进程模式
#define CN_MODE_MP 3  //多进程模式，只有mmu=true才可选择此模式
#define CN_CFG_RUN_MODE CN_MODE_SI

/*____字符集，支持gui kernel，本组配置才有意义____*/
#define CN_CFG_CHARSET_GB2312 1  //gb2312编码
#define CN_CFG_CHARSET_ASCII 0  //gb2312已经包含了ascii编码，无须再支持
#define CN_CFG_CHARSET_UTF8 0  //unicode最流行的一种编码方式
#define CN_CFG_CHARSET_DEFAULT "gb2312"  //此名字须与C文件中定义的字符集名字相同

/*____字体，支持gui kernel，本组配置才有意义____*/
#define CN_CFG_GB2312_8x16_1616 1  //包含8*16点阵的ascii字符和16*16点阵的宋体汉字
#define CN_CFG_ASCII_8x8 1  //包含8*8点阵的ascii字符
#define CN_CFG_ASCII_8x16 0  //包含8*16点阵的ascii字符gb2312_816_1616中已经包含，无须单独定义
#define CN_CFG_FONT_DEFAULT "gb2312_8_16_1616"  //此名字须与C文件中使用的字体资源名字相同,

/*____gui kernel配置____*/
#define CN_CFG_GUI_KERNEL 1  //gui kernel模块
#define CN_CFG_DOWN_CHUNNEL_DEEP 1024  //控制命令缓存深度，字节数
#define CN_CFG_GK_UP_CHUNNEL_DEEP 256  //gui kernel消息缓冲区长度，字节数
#define CN_CFG_GUI_PATTERN_MODE 0  //模式位图尺寸，0=固定8*8位图，1=任意bitmap（V1.0.0不支持）

/*____数量控制____*/
#define CN_CFG_DEVICE_LIMIT 20  //定义设备数量。
#define CN_CFG_LOCKS_LIMIT 20  //定义程序中可用的自由锁的数量。自由锁指用户调用semp_create和mutex_create创建的锁，不包括内核用到的锁。
#define CN_CFG_EVENTS_LIMIT 100  //事件数量
#define CN_CFG_EVTTS_LIMIT 32  //总事件类型数
#define CN_CFG_PARAS_LIMIT (CN_CFG_EVENTS_LIMIT+CN_CFG_EVTTS_LIMIT)  //事件参数数量，义了一个参数
#define CN_CFG_WDTS_LIMIT 5  //允许养狗数量
#define CN_CFG_PIPES_LIMIT 5  //管道数量

/*____与内存管理相关的常量____*/
#define CN_CFG_VIRTUAL_MEMORY 0  //2440硬件虽然支持虚拟内存，但SI版本不打算支持，设为0
#define CN_CFG_PHY_MEMORY 0  //物理内存总量，支持虚拟内存才有意义。
#define CN_CFG_MMU_USED 1  //使用MMU
#define CN_CFG_CACHE_USED 1  //使能Cache
#define CN_CFG_PAGE_SIZE 0x1000  //基页尺寸为4Kbytes
#define CN_CFG_PAGE_SIZE_SUFFIX_ZERO 12  //页尺寸后缀0的个数
#define CN_CFG_MEM_RECYCLE true  //支持内存回收----配置工具
#define CN_CFG_MEM_POOLS (10)  //允许建立10个内存池
#define CN_CFG_SYSSVC_STACK 1024  //系统服务事件栈容量
#define CN_CFG_DYNAMIC_MEM 1  //是否支持高级内存分配，即使不支持，也允许使用malloc-free分配内存，但必须严格成对调用，否则会内存泄漏
#define CN_CFG_STACK_FILL 'd'  //栈填充值，设为0则不填充
#define CN_CFG_MAIN_STACK 1024  //main函数所需栈容量

/*____文件系统相关配置____*/
#define CN_CFG_DJYFS 1  //是否包含文件系统
#define CN_CFG_DJYFS_FLASH 0  //是否包含flash文件系统的驱动
#define CN_CFG_FAT_FILESYS 0  //是否包含flash文件系统的驱动
#define CN_CFG_DJYFS_RAM 0  //是否包含ram文件系统
#define CN_CFG_OPENED_FSNODE_LIMIT 16  //可同时打开的文件数量，注意多级路径的每一级都算一个文件，但相同的路径不重复算

/*____组件配置____*/
#define CN_CFG_PAN_DRIVER 1  //是否包含泛设备驱动模块
#define CN_CFG_DEBUG_INFO 1  //调试信息收集与输出模块
#define CN_CFG_SHELL 1  //是否包含shell模块
#define CN_CFG_PIPE 1  //是否包含管道通信模块
#define CN_CFG_WDT 0  //是否包含看门狗驱动
#define CN_CFG_NUDE_TERMINAL 0  //nude terminal是一个在启动时提供最基本的人机对话功能的模块
#define CN_CFG_DJYIP 0  //djyip 模块

/*____标准输入模块配置____*/
#define CN_CFG_KEYBOARD 1  //是否包含键盘驱动
#define CN_CFG_SINGLE_TOUCH 0  //是否支持单点触摸屏
#define CN_CFG_MUTI_TOUCH 0  //是否支持多点触摸屏
#define CN_CFG_MUTI_KEYBOARD 0  //0=不支持多键盘，即使系统拥有多键盘，所有键盘也不加区分地使用
#define CN_CFG_MUTI_MOUSE_2D 0  //0=不支持多鼠标，即使系统拥有多个鼠标，所有鼠标也共享同一个鼠标指针。
#define CN_CFG_MUTI_MOUSE_3D 0  //0=不支持多3d鼠标，即使系统拥有多个3d鼠标，所有鼠标也共享同一个鼠标指针。
#define CN_CFG_STDIN_DEVICE "uart1"  //标准输入设备
#define CN_CFG_STDIN_DEVICE_LIMIT 3  //可支持的输入设备数量

/*____外设配置____*/
#define CN_CFG_UART0_USED 0
#define CN_CFG_UART1_USED 1
#define CN_CFG_UART2_USED 0

/*____工作路径设置____*/
#define CN_CFG_WORK_PATH "sys:\\"  //工作路径设置用户不设置的话，set_work_path就是空字符

#ifdef __cplusplus
}
#endif
#endif

