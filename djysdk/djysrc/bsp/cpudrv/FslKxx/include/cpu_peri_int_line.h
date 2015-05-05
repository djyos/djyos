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
//作者:lst
//版本：V1.0.0
//文件描述: 定义中断号
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-06-06
//   作者: hm
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_INT_LINE_H__
#define __CPU_PERI_INT_LINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CN_INT_LINE_DMA0_DMA16                  (0x00)
#define CN_INT_LINE_DMA1_DMA17                  (0x01)
#define CN_INT_LINE_DMA2_DMA18                  (0x02)
#define CN_INT_LINE_DMA3_DMA19                  (0x03)
#define CN_INT_LINE_DMA4_DMA20                  (0x04)
#define CN_INT_LINE_DMA5_DMA21                  (0x05)
#define CN_INT_LINE_DMA6_DMA22                  (0x06)
#define CN_INT_LINE_DMA7_DMA23                  (0x07)
#define CN_INT_LINE_DMA8_DMA24                  (0x08)
#define CN_INT_LINE_DMA9_DMA25                  (0x09)
#define CN_INT_LINE_DMA10_DMA26                 (0x0A)
#define CN_INT_LINE_DMA11_DMA27                 (0x0B)
#define CN_INT_LINE_DMA12_DMA28                 (0x0C)
#define CN_INT_LINE_DMA13_DMA29                 (0x0D)
#define CN_INT_LINE_DMA14_DMA30                 (0x0E)
#define CN_INT_LINE_DMA15_DMA31                 (0x0F)
#define CN_INT_LINE_DMA_Error                   (0X10)
#define CN_INT_LINE_MCM                         (0X11)
#define CN_INT_LINE_FTFE                        (0x12)
#define CN_INT_LINE_Read_Collision              (0x13)
#define CN_INT_LINE_LVD_LVW                     (0x14)
#define CN_INT_LINE_LLW                         (0x15)
#define CN_INT_LINE_WDT                         (0x16)
#define CN_INT_LINE_RNG                         (0x17)
#define CN_INT_LINE_I2C0                        (0x18)
#define CN_INT_LINE_I2C1                        (0x19)
#define CN_INT_LINE_SPI0                        (0x1a)
#define CN_INT_LINE_SPI1                        (0x1b)
#define CN_INT_LINE_SPI2                        (0x1c)
#define CN_INT_LINE_CAN0_ORed_Message_buffer    (0x1d)
#define CN_INT_LINE_CAN0_Bus_Off                (0x1e)
#define CN_INT_LINE_CAN0_Error                  (0x1f)
#define CN_INT_LINE_CAN0_Tx_Warning             (0x20)
#define CN_INT_LINE_CAN0_Rx_Warning             (0x21)
#define CN_INT_LINE_CAN0_Wake_Up                (0x22)
#define CN_INT_LINE_I2S0_Tx                     (0x23)
#define CN_INT_LINE_I2S0_Rx                     (0x24)
#define CN_INT_LINE_CAN1_ORed_Message_buffer    (0x25)
#define CN_INT_LINE_CAN1_Bus_Off                (0x26)
#define CN_INT_LINE_CAN1_Error                  (0x26)
#define CN_INT_LINE_CAN1_Tx_Warning             (0x28)
#define CN_INT_LINE_CAN1_Rx_Warning             (0x29)
#define CN_INT_LINE_CAN1_Wake_Up                (0x2A)
#define CN_INT_LINE_Reserved59                  (0x2B)
#define CN_INT_LINE_UART0_LON                   (0x2C)
#define CN_INT_LINE_UART0_RX_TX                 (0x2D)
#define CN_INT_LINE_UART0_ERR                   (0x2E)
#define CN_INT_LINE_UART1_RX_TX                 (0x2F)
#define CN_INT_LINE_UART1_ERR                   (0x30)
#define CN_INT_LINE_UART2_RX_TX                 (0x31)
#define CN_INT_LINE_UART2_ERR                   (0x32)
#define CN_INT_LINE_UART3_RX_TX                 (0x33)
#define CN_INT_LINE_UART3_ERR                   (0x34)
#define CN_INT_LINE_UART4_RX_TX                 (0x35)
#define CN_INT_LINE_UART4_ERR                   (0x36)
#define CN_INT_LINE_UART5_RX_TX                 (0x37)
#define CN_INT_LINE_UART5_ERR                   (0x38)
#define CN_INT_LINE_ADC0                        (0x39)
#define CN_INT_LINE_ADC1                        (0x3A)
#define CN_INT_LINE_CMP0                        (0x3B)
#define CN_INT_LINE_CMP1                        (0x3C)
#define CN_INT_LINE_CMP2                        (0x3D)
#define CN_INT_LINE_FTM0                        (0x3E)
#define CN_INT_LINE_FTM1                        (0x3F)
#define CN_INT_LINE_FTM2                        (0x40)
#define CN_INT_LINE_CMT                         (0x41)
#define CN_INT_LINE_RTC                         (0x42)
#define CN_INT_LINE_RTC_SEC                     (0x43)
#define CN_INT_LINE_PIT0                        (0x44)
#define CN_INT_LINE_PIT1                        (0x45)
#define CN_INT_LINE_PIT2                        (0x46)
#define CN_INT_LINE_PIT3                        (0x47)
#define CN_INT_LINE_PDB0                        (0x48)
#define CN_INT_LINE_USB0                        (0x49)
#define CN_INT_LINE_USBDCD                      (0x4A)
#define CN_INT_LINE_ENET_1588_Timer             (0x4B)
#define CN_INT_LINE_ENET_TX                     (0x4C)
#define CN_INT_LINE_ENET_RX                     (0x4D)
#define CN_INT_LINE_ENET_ERR                    (0x4E)
#define CN_INT_LINE_Reserved95                  (0x4F)
#define CN_INT_LINE_SDHC                        (0x50)
#define CN_INT_LINE_DAC0                        (0x51)
#define CN_INT_LINE_DAC1                        (0x52)
#define CN_INT_LINE_TSI0                        (0x53)
#define CN_INT_LINE_MCG                         (0x54)
#define CN_INT_LINE_LPTimer                     (0x55)
#define CN_INT_LINE_Reserved102                 (0x56)
#define CN_INT_LINE_PORTA                       (0x57)
#define CN_INT_LINE_PORTB                       (0x58)
#define CN_INT_LINE_PORTC                       (0x59)
#define CN_INT_LINE_PORTD                       (0x5A)
#define CN_INT_LINE_PORTE                       (0x5B)
#define CN_INT_LINE_PORTF                       (0x5C)
#define CN_INT_LINE_DDR                         (0x5D)
#define CN_INT_LINE_SWI                         (0x5E)
#define CN_INT_LINE_NFC                         (0x5F)
#define CN_INT_LINE_USBHS                       (0x60)
#define CN_INT_LINE_LCD                         (0x61)
#define CN_INT_LINE_CMP3                        (0x62)
#define CN_INT_LINE_Tamper                      (0x63)
#define CN_INT_LINE_Reserved116                 (0x64)
#define CN_INT_LINE_FTM3                        (0x65)
#define CN_INT_LINE_ADC2                        (0x66)
#define CN_INT_LINE_ADC3                        (0x67)
#define CN_INT_LINE_I2S1_Tx                     (0x68)
#define CN_INT_LINE_I2S1_Rx                     (0x69)


#define CN_INT_LINE_LAST                        (0x69)

#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_INT_LINE_H__

