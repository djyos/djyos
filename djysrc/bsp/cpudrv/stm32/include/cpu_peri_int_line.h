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
//所属模块:中断模块
//作者：lst
//版本：V1.0.0
//文件描述: 定义中断号
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009--9-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_INT_LINE_H__
#define __CPU_PERI_INT_LINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CN_INT_LINE_WWDG            (0x00)  //Window WatchDog Interrupt
#define CN_INT_LINE_PVD             (0x01)  //PVD through EXTI Line detection Interrupt
#define CN_INT_LINE_TAMPER          (0x02)  //Tamper Interrupt
#define CN_INT_LINE_RTC             (0x03)  //RTC global Interrupt
#define CN_INT_LINE_FLASH           (0x04)  //FLASH global Interrupt
#define CN_INT_LINE_RCC             (0x05)  //RCC global Interrupt
#define CN_INT_LINE_EXTI0           (0x06)  //EXTI Line0 Interrupt
#define CN_INT_LINE_EXTI1           (0x07)  //EXTI Line1 Interrupt
#define CN_INT_LINE_EXTI2           (0x08)  //EXTI Line2 Interrupt
#define CN_INT_LINE_EXTI3           (0x09)  //EXTI Line3 Interrupt
#define CN_INT_LINE_EXTI4           (0x0A)  //EXTI Line4 Interrupt
#define CN_INT_LINE_DMA1_Ch1        (0x0B)  //DMA1 Channel 1 global Interrupt
#define CN_INT_LINE_DMA1_Ch2        (0x0C)  //DMA1 Channel 2 global Interrupt
#define CN_INT_LINE_DMA1_Ch3        (0x0D)  //DMA1 Channel 3 global Interrupt
#define CN_INT_LINE_DMA1_Ch4        (0x0E)  //DMA1 Channel 4 global Interrupt
#define CN_INT_LINE_DMA1_Ch5        (0x0F)  //DMA1 Channel 5 global Interrupt
#define CN_INT_LINE_DMA1_Ch6        (0x10)  //DMA1 Channel 6 global Interrupt
#define CN_INT_LINE_DMA1_Ch7        (0x11)  //DMA1 Channel 7 global Interrupt
#define CN_INT_LINE_ADC1_2          (0x12)  //ADC1 et ADC2 global Interrupt
#define CN_INT_LINE_USB_HP_CAN_TX   (0x13)  //USB High Priority or CAN TX Interrupts
#define CN_INT_LINE_USB_LP_CAN_RX0  (0x14)  //USB Low Priority or CAN RX0 Interrupts
#define CN_INT_LINE_CAN_RX1         (0x15)  //CAN RX1 Interrupt
#define CN_INT_LINE_CAN_SCE         (0x16)  //CAN SCE Interrupt
#define CN_INT_LINE_EXTI9_5         (0x17)  //External Line[9:5] Interrupts
#define CN_INT_LINE_TIM1_BRK        (0x18)  //TIM1 Break Interrupt
#define CN_INT_LINE_TIM1_UP         (0x19)  //TIM1 Update Interrupt
#define CN_INT_LINE_TIM1_TRG_COM    (0x1A)  //TIM1 Trigger and Commutation Interrupt
#define CN_INT_LINE_TIM1_CC         (0x1B)  //TIM1 Capture Compare Interrupt
#define CN_INT_LINE_TIM2            (0x1C)  //TIM2 global Interrupt
#define CN_INT_LINE_TIM3            (0x1D)  //TIM3 global Interrupt
#define CN_INT_LINE_TIM4            (0x1E)  //TIM4 global Interrupt
#define CN_INT_LINE_I2C1_EV         (0x1F)  //I2C1 Event Interrupt
#define CN_INT_LINE_I2C1_ER         (0x20)  //I2C1 Error Interrupt
#define CN_INT_LINE_I2C2_EV         (0x21)  //I2C2 Event Interrupt
#define CN_INT_LINE_I2C2_ER         (0x22)  //I2C2 Error Interrupt
#define CN_INT_LINE_SPI1            (0x23)  //SPI1 global Interrupt
#define CN_INT_LINE_SPI2            (0x24)  //SPI2 global Interrupt
#define CN_INT_LINE_USART1          (0x25)  //USART1 global Interrupt
#define CN_INT_LINE_USART2          (0x26)  //USART2 global Interrupt
#define CN_INT_LINE_USART3          (0x27)  //USART3 global Interrupt
#define CN_INT_LINE_EXTI15_10       (0x28)  //External Line[15:10] Interrupts
#define CN_INT_LINE_RTCAlarm        (0x29)  //RTC Alarm through EXTI Line Interrupt
#define CN_INT_LINE_USBWakeUp       (0x2A)  //USB WakeUp from suspend through EXTI Line Interrupt
#define CN_INT_LINE_TIM8_BRK        (0x2B)  //TIM8 Break Interrupt
#define CN_INT_LINE_TIM8_UP         (0x2C)  //TIM8 Update Interrupt
#define CN_INT_LINE_TIM8_TRG_COM    (0x2D)  //TIM8 Trigger and Commutation Interrupt
#define CN_INT_LINE_TIM8_CC         (0x2E)  //TIM8 Capture Compare Interrupt
#define CN_INT_LINE_ADC3            (0x2F)  //ADC3 global Interrupt
#define CN_INT_LINE_FSMC            (0x30)  //FSMC global Interrupt
#define CN_INT_LINE_SDIO            (0x31)  //SDIO global Interrupt
#define CN_INT_LINE_TIM5            (0x32)  //TIM5 global Interrupt
#define CN_INT_LINE_SPI3            (0x33)  //SPI3 global Interrupt
#define CN_INT_LINE_UART4           (0x34)  //UART4 global Interrupt
#define CN_INT_LINE_UART5           (0x35)  //UART5 global Interrupt
#define CN_INT_LINE_TIM6            (0x36)  //TIM6 global Interrupt
#define CN_INT_LINE_TIM7            (0x37)  //TIM7 global Interrupt
#define CN_INT_LINE_DMA2_Ch1        (0x38)  //DMA2 Channel 1 global Interrupt
#define CN_INT_LINE_DMA2_Ch2        (0x39)  //DMA2 Channel 2 global Interrupt
#define CN_INT_LINE_DMA2_Ch3        (0x3A)  //DMA2 Channel 3 global Interrupt
#define CN_INT_LINE_DMA2_Ch4_5      (0x3B)  //DMA2 Channel 4 and DMA2 Channel 5 global Interrupt
#define CN_INT_LINE_LAST            (0x3B)

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_INT_LINE_H__

