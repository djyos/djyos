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
//所属模块: CPU外设定义
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 包含CPU所有外设的定义，
//      1、不包含内核相关的固定的设备，比如m3的nvic等，所有m3内核均一致。
//      2、不包含内核相关的可选配但固定的定义，比如m3的mpu部件，不一定所有m3内核
//         的芯片都有，但只有有无的区别，不存在变化。
//         以上两部分在相应的内核目录下定义。
//      3、包含内核相关的可变部件定义
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_H__
#define __CPU_PERI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "cpu_peri_dma.h"
#include "cpu_peri_gpio.h"
#include "cpu_peri_inflash.h"
#include "cpu_peri_int_line.h"
#include "cpu_peri_uart.h"
#include "cpu_peri_rcc.h"
#include "cpu_peri_rtc.h"
#include "cpu_peri_wdt.h"
#include "cpu_peri_timer.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_conf.h"

extern struct DMA_StReg volatile * const pg_dma1_reg;
extern struct DMA_StReg volatile * const pg_dma2_reg;

//dma1的各通道寄存器
extern struct DMA_StChannelReg volatile * const pg_dma1_channel1_reg;
extern struct DMA_StChannelReg volatile * const pg_dma1_channel2_reg;
extern struct DMA_StChannelReg volatile * const pg_dma1_channel3_reg;
extern struct DMA_StChannelReg volatile * const pg_dma1_channel4_reg;
extern struct DMA_StChannelReg volatile * const pg_dma1_channel5_reg;
extern struct DMA_StChannelReg volatile * const pg_dma1_channel6_reg;
extern struct DMA_StChannelReg volatile * const pg_dma1_channel7_reg;
//dma2的各通道寄存器
extern struct DMA_StChannelReg volatile * const pg_dma2_channel1_reg;
extern struct DMA_StChannelReg volatile * const pg_dma2_channel2_reg;
extern struct DMA_StChannelReg volatile * const pg_dma2_channel3_reg;
extern struct DMA_StChannelReg volatile * const pg_dma2_channel4_reg;
extern struct DMA_StChannelReg volatile * const pg_dma2_channel5_reg;

//gpio相关寄存器定义
extern struct GPIO_StReg volatile * const pg_gpio_rega;
extern struct GPIO_StReg volatile * const pg_gpio_regb;
extern struct GPIO_StReg volatile * const pg_gpio_regc;
extern struct GPIO_StReg volatile * const pg_gpio_regd;
extern struct GPIO_StReg volatile * const pg_gpio_rege;
extern struct GPIO_StReg volatile * const pg_gpio_regf;
extern struct GPIO_StReg volatile * const pg_gpio_regg;
extern struct GPIO_StReg volatile * const pg_gpio_reg[];
extern struct AFIO_StReg volatile * const pg_afio_reg;

//串口相关寄存器
extern struct st_usart_reg volatile * const pg_uart1_reg;
extern struct st_usart_reg volatile * const pg_uart2_reg;
extern struct st_usart_reg volatile * const pg_uart3_reg;
extern struct st_usart_reg volatile * const pg_uart4_reg;
extern struct st_usart_reg volatile * const pg_uart5_reg;

//rcc(复位和时钟控制寄存器)
extern struct st_rcc_reg volatile * const pg_rcc_reg;

//片内flash控制寄存器
extern struct st_inflash_fpec_reg volatile * const pg_inflash_fpec_reg;
extern struct st_inflash_ob_reg volatile * const pg_inflash_ob_reg;
//IIC
extern struct I2C_StReg volatile * const pg_iic1_reg;
extern struct I2C_StReg volatile * const pg_iic2_reg;

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_H__

