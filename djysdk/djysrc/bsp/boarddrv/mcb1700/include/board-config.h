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
//        CPU型号:LPC1759
//        板件型号:MCB1700
//        生产企业:KEIL
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

#define CN_CFG_TICK_US 1000  //操作系统内核时钟脉冲长度，以us为单位。*
#define CN_CFG_TICK_HZ 1000  //内核时钟频率，单位为hz。

//与时钟振荡器相关的配置,本组配置
#define Mhz 1000000
#define CN_CFG_EXTCLK (12*Mhz)  //晶振频率
#define CN_CFG_CCLK (100*Mhz)  //特注:nxp的cpu要求主频必须是晶振的整数倍
#define CFG_CCLK (CN_CCLK/2)  //外设时钟?须是主频的1/1、1/2、1/4
#define CN_CFG_MCLK CN_CFG_CCLK  //主频，内核要用，必须定义
#define CN_CFG_FCLK CN_CFG_MCLK  //cm3自由运行外设时钟
#define CN_CFG_FINE_US 0x028F  //1/100M,精密时钟脉冲长度?以uS为单位?32位定点数?整数小数各占16位?这也限制了ticks最长不超过65535uS
#define CN_CFG_FINE_HZ CN_MCLK  //精密时钟频率?是cn_fine_ns的倒数。

//内核相关配置
#define CN_CFG_DYNAMIC_MEM 1  //是否支持动态内存分配，即使不支持，也允许使用malloc-free分配内存，
                              //但使用有差别，详见《……用户手册》内存分配章节

//CPU架构相关配置,可选或者可能可选的才在这里配置,例如大小端,是可选的,在这里配置,
//而CPU字长固定,故字长在BSP的arch.h文件中定义
//存储器大小端的配置
#define CN_CFG_LITTLE_ENDIAN        0
#define CN_CFG_BIG_ENDIAN           1
#define CN_CFG_BYTE_ORDER          CN_CFG_LITTLE_ENDIAN

#define CN_LCD_XSIZE 240
#define CN_LCD_YSIZE 320

#ifdef __cplusplus
}
#endif
#endif // __BOARD_CONFIG_H__



