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
//所属模块: BSP模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 板件特性配置文件。
//        CPU型号:STM32F103ZE
//        板件型号:QH_1
//        生产企业:爱好者团体
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-10-31
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../cpu-optional.h"

#define CN_CFG_TICK_US 1000  //tick间隔，以us为单位。
#define CN_CFG_TICK_HZ 1000  //内核时钟频率，单位为hz。

//与时钟振荡器相关的配置,本组配置
#define Mhz 1000000
#define CN_CFG_MCLK (72*Mhz)  //主频，内核要用，必须定义
#define CN_CFG_FCLK CN_CFG_MCLK  //cm3自由运行外设时钟
#define CN_CFG_AHBCLK CN_CFG_MCLK  //总线时钟
#define CN_CFG_PCLK2 CN_CFG_MCLK  //高速外设时钟
#define CN_CFG_PCLK1 (CN_CFG_MCLK/2)  //低速外设时钟
#define CN_CFG_EXTCLK 8000000  //外部时钟=8M
#define CN_CFG_FINE_US 0x0000038e  //1/72M,tick输入时钟周期，以uS为单位，32位定点数整数、小数各占16位，这也限制了ticks最长不超过65535uS
#define CN_CFG_FINE_HZ CN_CFG_MCLK  //tick输入时钟频率，是CN_CFG_FINE_US的倒数，单位Hz。

//内核相关配置
#define CN_CFG_DYNAMIC_MEM 1  //是否支持动态内存分配，即使不支持，也允许使用malloc-free分配内存，
                              //但使用有差别，详见《……用户手册》内存分配章节

//CPU架构相关配置,可选才在这里配置,例如大小端,是可选的,在这里配置,
//而CPU字长固定,故字长在BSP的arch_feature.h文件中定义
//存储器大小端的配置
#define CN_CFG_LITTLE_ENDIAN        0
#define CN_CFG_BIG_ENDIAN           1
#define CN_CFG_BYTE_ORDER          CN_CFG_LITTLE_ENDIAN

#define CN_LCD_XSIZE 240
#define CN_LCD_YSIZE 320

//定义CPU型号,编译ST公司的固件库时需要----for cube 库
//在stm32f1xx.h文件中须包含本头文件
//#define STM32F100xB     //STM32F100C4, STM32F100R4, STM32F100C6, STM32F100R6, STM32F100C8, STM32F100R8, STM32F100V8, STM32F100CB, STM32F100RB and STM32F100VB
//#define STM32F100xE     //STM32F100RC, STM32F100VC, STM32F100ZC, STM32F100RD, STM32F100VD, STM32F100ZD, STM32F100RE, STM32F100VE and STM32F100ZE
//#define STM32F101x6     //STM32F101C4, STM32F101R4, STM32F101T4, STM32F101C6, STM32F101R6 and STM32F101T6 Devices
//#define STM32F101xB     //STM32F101C8, STM32F101R8, STM32F101T8, STM32F101V8, STM32F101CB, STM32F101RB, STM32F101TB and STM32F101VB
//#define STM32F101xE     //STM32F101RC, STM32F101VC, STM32F101ZC, STM32F101RD, STM32F101VD, STM32F101ZD, STM32F101RE, STM32F101VE and STM32F101ZE
//#define STM32F101xG     //STM32F101RF, STM32F101VF, STM32F101ZF, STM32F101RG, STM32F101VG and STM32F101ZG
//#define STM32F102x6     //STM32F102C4, STM32F102R4, STM32F102C6 and STM32F102R6
//#define STM32F102xB     //STM32F102C8, STM32F102R8, STM32F102CB and STM32F102RB
//#define STM32F103x6     //STM32F103C4, STM32F103R4, STM32F103T4, STM32F103C6, STM32F103R6 and STM32F103T6
//#define STM32F103xB     //STM32F103C8, STM32F103R8, STM32F103T8, STM32F103V8, STM32F103CB, STM32F103RB, STM32F103TB and STM32F103VB
#define STM32F103xE     //STM32F103RC, STM32F103VC, STM32F103ZC, STM32F103RD, STM32F103VD, STM32F103ZD, STM32F103RE, STM32F103VE and STM32F103ZE
//#define STM32F103xG     //STM32F103RF, STM32F103VF, STM32F103ZF, STM32F103RG, STM32F103VG and STM32F103ZG
//#define STM32F105xC     //STM32F105R8, STM32F105V8, STM32F105RB, STM32F105VB, STM32F105RC and STM32F105VC
//#define STM32F107xC     //STM32F107RB, STM32F107VB, STM32F107RC and STM32F107VC

//定义CPU型号,编译ST公司的固件库时需要----for 传统 库
//在stm32f10x.h文件中须包含本头文件
//#define STM32F10X_LD      //!< STM32F10X_LD: STM32 Low density devices
//#define STM32F10X_LD_VL   //!< STM32F10X_LD_VL: STM32 Low density Value Line devices
//#define STM32F10X_MD      //!< STM32F10X_MD: STM32 Medium density devices
//#define STM32F10X_MD_VL   //!< STM32F10X_MD_VL: STM32 Medium density Value Line devices
#define STM32F10X_HD      //!< STM32F10X_HD: STM32 High density devices
//#define STM32F10X_HD_VL   //!< STM32F10X_HD_VL: STM32 High density value line devices
//#define STM32F10X_XL      //!< STM32F10X_XL: STM32 XL-density devices
//#define STM32F10X_CL      //!< STM32F10X_CL: STM32 Connectivity line devices

#ifdef __cplusplus
}
#endif
#endif // __BOARD_CONFIG_H__



