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
// 文件名     ：cpu_peri_port.h
// 模块描述: 端口模块配置，端口中断配置和使能
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 07/06.2014
// =============================================================================

#ifndef CPU_PERI_PORT_H_
#define CPU_PERI_PORT_H_

//宏定义：port口
#define PORT_PORT_A     		0
#define PORT_PORT_B     		1
#define PORT_PORT_C     		2
#define PORT_PORT_D     		3
#define PORT_PORT_E     		4
#define PORT_PORT_F     		5
#define PORT_PORT_MAX     		5

//宏定义：引脚号
#define PORT_PIN(x)				x
#define PORT_PIN_MAX			31

//宏定义：引脚多功能利用选择
#define PORT_PINMUX_ALTDIS		0	//Analog
#define PORT_PINMUX_GPIO        1
#define PORT_PINMUX_ALT2        2	//chip specific
#define PORT_PINMUX_ALT3        3	//chip specific
#define PORT_PINMUX_ALT4        4	//chip specific
#define PORT_PINMUX_ALT5        5	//chip specific
#define PORT_PINMUX_ALT6        6	//chip specific
#define PORT_PINMUX_ALT7        7	//chip specific / JTAG / NMI

//宏定义：定义中断触发边沿
#define IQC_INTDMA_DISABLE      0	//禁止中断/DMA请求
#define IQC_DMA_RISING_EDGE		1	//DMA请求上升沿触发
#define IQC_DMA_FALING_EDGE     2	//DMA请求下降沿触发
#define IQC_DMA_EIGHER_EDGE     3	//DMA请求任何边沿触发
#define IQC_INT_LEVEL_LOW		8	//INT水平低电平触发
#define IQC_INT_RISING_EDGE     9	//INT上升沿触发
#define IQC_INT_FALING_EDGE     10	//INT下降沿触发
#define IQC_INT_EIGHER_EDGE     11	//INT任何边沿触发
#define IQC_INT_LEVEL_HIGH      12	//INT水平高电平触发

//宏定义：内部上下拉
#define PORT_PULL_NONE          0
#define PORT_PULL_UP            1
#define PORT_PULL_DOWN          2

void PORT_MuxConfig(u32 port,u32 pin,u8 mux);
void PORT_IntConfig(u32 port,u32 pin,u8 trig,u8 pull);
void PORT_IntDisable(u32 port,u32 pin);
void PORT_IntFlagClear(u32 port, u32 pin);
u8   PORT_IntStatusGet(u32 port,u32 pin);


#endif /* CPU_PERI_PORT_H_ */
