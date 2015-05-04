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
//所属模块:操作系统运行环境配置
//作者：lst
//版本：V1.0.0
//文件描述:配置本工程所使用到的中断源
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "int.h"
#include "cpu_peri.h"

//此表配置工程中用到的中断源,每多使用一个中断源,内核需要多占一个struct tagIntLine
//结构(32字节)的内存.对于内存丰厚的系统,建议在把cpu_peri_int_line.h文件中列
//出的中断源全部copy到这里,内存匮乏的系统则只把用到的中断源列出即可
const ufast_t tg_IntUsed[] =
{
    CN_INT_LINE_WWDG,            // (0) - Window WatchDog Interrupt
    CN_INT_LINE_PVD,             // (1) - PVD through EXTI Line detection Interrupt
    CN_INT_LINE_TAMPER,          // (2) - Tamper Interrupt
    CN_INT_LINE_RTC,             // (3) - RTC global Interrupt
    CN_INT_LINE_FLASH,           // (4) - FLASH global Interrupt
    CN_INT_LINE_RCC,             // (5) - RCC global Interrupt
    CN_INT_LINE_EXTI0,           // (6) - EXTI Line0 Interrupt
    CN_INT_LINE_EXTI1,           // (7) - EXTI Line1 Interrupt
    CN_INT_LINE_EXTI2,           // (8) - EXTI Line2 Interrupt
    CN_INT_LINE_EXTI3,           // (9) - EXTI Line3 Interrupt
    CN_INT_LINE_EXTI4,           // (0) - EXTI Line4 Interrupt
    CN_INT_LINE_DMA1_Ch1,        // (11) - DMA1 Channel 1 global Interrupt
    CN_INT_LINE_DMA1_Ch2,        // (12) - DMA1 Channel 2 global Interrupt
    CN_INT_LINE_DMA1_Ch3,        // (13) - DMA1 Channel 3 global Interrupt
    CN_INT_LINE_DMA1_Ch4,        // (14) - DMA1 Channel 4 global Interrupt
    CN_INT_LINE_DMA1_Ch5,        // (15) - DMA1 Channel 5 global Interrupt
    CN_INT_LINE_DMA1_Ch6,        // (16) - DMA1 Channel 6 global Interrupt
    CN_INT_LINE_DMA1_Ch7,        // (17) - DMA1 Channel 7 global Interrupt
    CN_INT_LINE_ADC1_2,          // (18) - ADC1 et ADC2 global Interrupt
    CN_INT_LINE_USB_HP_CAN_TX,   // (19) - USB High Priority or CAN TX Interrupts
    CN_INT_LINE_USB_LP_CAN_RX0,  // (20) - USB Low Priority or CAN RX0 Interrupts
    CN_INT_LINE_CAN_RX1,         // (21) - CAN RX1 Interrupt
    CN_INT_LINE_CAN_SCE,         // (22) - CAN SCE Interrupt
    CN_INT_LINE_EXTI9_5,         // (23) - External Line[9:5] Interrupts
    CN_INT_LINE_TIM1_BRK,        // (24) - TIM1 Break Interrupt
    CN_INT_LINE_TIM1_UP,         // (25) - TIM1 Update Interrupt
    CN_INT_LINE_TIM1_TRG_COM,    // (26) - TIM1 Trigger and Commutation Interrupt
    CN_INT_LINE_TIM1_CC,         // (27) - TIM1 Capture Compare Interrupt
    CN_INT_LINE_TIM2,            // (28) - TIM2 global Interrupt
    CN_INT_LINE_TIM3,            // (29) - TIM3 global Interrupt
    CN_INT_LINE_TIM4,            // (30) - TIM4 global Interrupt
    CN_INT_LINE_I2C1_EV,         // (31) - I2C1 Event Interrupt
    CN_INT_LINE_I2C1_ER,         // (32) - I2C1 Error Interrupt
    CN_INT_LINE_I2C2_EV,         // (33) - I2C2 Event Interrupt
    CN_INT_LINE_I2C2_ER,         // (34) - I2C2 Error Interrupt
    CN_INT_LINE_SPI1,            // (35) - SPI1 global Interrupt
    CN_INT_LINE_SPI2,            // (36) - SPI2 global Interrupt
    CN_INT_LINE_USART1,          // (37) - USART1 global Interrupt
    CN_INT_LINE_USART2,          // (38) - USART2 global Interrupt
    CN_INT_LINE_USART3,          // (39) - USART3 global Interrupt
    CN_INT_LINE_EXTI15_10,       // (40) - External Line[15:10] Interrupts
    CN_INT_LINE_RTCAlarm,        // (41) - RTC Alarm through EXTI Line Interrupt
    CN_INT_LINE_USBWakeUp,       // (42) - USB WakeUp from suspend through EXTI Line Interrupt
    CN_INT_LINE_TIM8_BRK,        // (43) - TIM8 Break Interrupt
    CN_INT_LINE_TIM8_UP,         // (44) - TIM8 Update Interrupt
    CN_INT_LINE_TIM8_TRG_COM,    // (45) - TIM8 Trigger and Commutation Interrupt
    CN_INT_LINE_TIM8_CC,         // (46) - TIM8 Capture Compare Interrupt
    CN_INT_LINE_ADC3,            // (47) - ADC3 global Interrupt
    CN_INT_LINE_FSMC,            // (48) - FSMC global Interrupt
    CN_INT_LINE_SDIO,            // (49) - SDIO global Interrupt
    CN_INT_LINE_TIM5,            // (50) - TIM5 global Interrupt
    CN_INT_LINE_SPI3,            // (51) - SPI3 global Interrupt
    CN_INT_LINE_UART4,           // (52) - UART4 global Interrupt
    CN_INT_LINE_UART5,           // (53) - UART5 global Interrupt
    CN_INT_LINE_TIM6,            // (54) - TIM6 global Interrupt
    CN_INT_LINE_TIM7,            // (55) - TIM7 global Interrupt
    CN_INT_LINE_DMA2_Ch1,        // (56) - DMA2 Channel 1 global Interrupt
    CN_INT_LINE_DMA2_Ch2,        // (57) - DMA2 Channel 2 global Interrupt
    CN_INT_LINE_DMA2_Ch3,        // (58) - DMA2 Channel 3 global Interrupt
    CN_INT_LINE_DMA2_Ch4_5,      // (59) - DMA2 Channel 4 and DMA2 Channel 5 global Interrupt
};

const ufast_t tg_IntUsedNum = sizeof(tg_IntUsed)/sizeof(ufast_t);

