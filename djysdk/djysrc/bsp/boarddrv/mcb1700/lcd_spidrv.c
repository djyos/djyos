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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：lcd_spidrv.h
// 模块描述: 图形显示驱动通信接口，CPU与驱动芯片通信采用SPI模式，因为该驱动芯片无硬件
//           加速，每一个点都需要发生一次SPI通信，通信量小，采用DJYBUS模块会大大降低
//           通信效率，因此编写该LCD专用SPI通信模块
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 06/10.2014
// =============================================================================
#if 1
#include "cpu_peri.h"
#include "lcd_spidrv.h"

// =============================================================================
// 功能：初始化SPI模块，包括IO和SPI通信参数
// 参数：无
// 返回：无
// =============================================================================
static void __spi_init(void)
{
    // Connect MOSI, MISO, and SCK to SSP peripheral
    LPC_GPIO0->FIODIR    &= ~PIN_CLK;
    LPC_PINCON->PINSEL0  |= (2 << 18) | (2 << 16) | (2 << 14);
    LPC_PINCON->PINMODE0 &= ~0x000FF000;

    // Enable SPI in Master Mode, CPOL=1, CPHA=1
    // Max. 12.5 MBit used for Data Transfer @ 100MHz
    LPC_SSP1->CR0        = 0x01C7;
    LPC_SSP1->CPSR       = 0x02;
    LPC_SSP1->CR1        = 0x02;
}

// =============================================================================
// 功能：传输一个字节的数据到SPI从设备，同时读到一个字节的数据
// 参数：byte,传输的字节
// 返回：读到的字节
// =============================================================================
unsigned char spi_tran (unsigned char byte)
{
    LPC_SSP1->DR = byte;
    while (!(LPC_SSP1->SR & RNE));        // Wait for send to finish
    return (LPC_SSP1->DR);
}

bool_t LCD_TxRx(u8* WrBuf, u32 WrLen, u8* RdBuf, u32 RdLen)
{
    u32 i;
    LCD_CS(0);
    for(i = 0; i < WrLen; i++)
    {
        spi_tran(WrBuf[i]);
    }
    for(i = 0; i < RdLen; i++)
    {
        RdBuf[i] = spi_tran(0);
    }
    LCD_CS(1);
    return true;
}

// =============================================================================
// 功能：SPI硬件初始化，供LCD模块调用
// 参数：无
// 返回：无
// =============================================================================
void LCD_HardInit(void)
{
    // Configure the LCD Control pins
    LPC_PINCON->PINSEL9 &= 0xF0FFFFFF;
    LPC_GPIO4->FIODIR   |= 0x30000000;
    LPC_GPIO4->FIOSET    = 0x20000000;

    // SSEL1 is GPIO output set to high
    LPC_GPIO0->FIODIR   |= 0x00000040;
    LPC_GPIO0->FIOSET    = 0x00000040;
    LPC_PINCON->PINSEL0 &= 0xFFF03FFF;
    LPC_PINCON->PINSEL0 |= 0x000A8000;

    __spi_init();
}
#else

#include "stdint.h"
#include "cpu_peri.h"
#include "spibus.h"
#include "cpu_peri_ssp.h"
#include "lcd_spidrv.h"

//定义IICBUS架构下的IIC设备结构
static struct SPI_Device *ps_LCD_Dev = NULL;
static u32 s_LCD_Timeout = CN_TIMEOUT_FOREVER;


bool_t LCD_TxRx(u8* WrBuf, u32 WrLen, u8* RdBuf, u32 RdLen)
{
    struct SPI_DataFrame spidata;
    spidata.SendBuf = WrBuf;
    spidata.SendLen = WrLen;
    spidata.RecvBuf = RdBuf;
    spidata.RecvLen = RdLen;
    LCD_CS(0);

    SPI_Transfer(ps_LCD_Dev,&spidata,true,s_LCD_Timeout);

    LCD_CS(1);
    return true;
}

static void __LCD_HardDrvInit(void)
{
    // Configure the LCD Control pins
    LPC_PINCON->PINSEL9 &= 0xF0FFFFFF;
    LPC_GPIO4->FIODIR   |= 0x30000000;
    LPC_GPIO4->FIOSET    = 0x20000000;

    // SSEL1 is GPIO output set to high
    LPC_GPIO0->FIODIR   |= 0x00000040;
    LPC_GPIO0->FIOSET    = 0x00000040;
    LPC_PINCON->PINSEL0 &= 0xFFF03FFF;
    LPC_PINCON->PINSEL0 |= 0x000A8000;
}

bool_t LCD_HardInit(void)
{
    bool_t result = false;
    static struct SPI_Device s_LCD_Dev;

    __LCD_HardDrvInit();

    //初始化IIC设备结构体
    s_LCD_Dev.Cs = 0;
    s_LCD_Dev.AutoCs = false;
    s_LCD_Dev.CharLen = 8;
    s_LCD_Dev.Mode = SPI_MODE_3;
    s_LCD_Dev.ShiftDir = SPI_SHIFT_MSB;
    s_LCD_Dev.Freq = 10*1000*1000;      //10M

    //添加LCDCH到IIC0总线
    if(NULL != SPI_DevAdd_s("SSP1","IIC_Dev_LCD",&s_LCD_Dev))
    {
        ps_LCD_Dev = &s_LCD_Dev;
        SPI_BusCtrl(ps_LCD_Dev,CN_SPI_SET_POLL,0,0);
        result = true;
    }

    return result;
}
#endif
