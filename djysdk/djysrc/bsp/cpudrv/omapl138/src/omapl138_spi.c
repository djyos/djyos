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
/*========================================================*
 * 文件模块说明:                                                 *
 * omapl138的spi口驱动文件，提供对spi口初始化和输入输出驱动      *
 * 文件版本: v1.00                                               *
 * 开发人员: lst,kongds,huangcw                              *
 * 创建时间: 2011.06.01                                          *
 * Copyright(c) 2011-2011  深圳鹏瑞软件有限公司             *
 *========================================================*
 * 程序修改记录(最新的放在最前面):                               *
 *  <版本号> <修改日期>, <修改人员>: <修改功能概述>              *
 *========================================================*
* 例如：
*  V1.02 2004-07-14, 张三: 接收函数增加对报文长度有效性校核
*
*/
#include "stdint.h"
#include "cpu_peri.h"

// =========================================================================
// 函数功能：spi口初始化,本驱动中，cs由spi模块按gpio的output方式访问，故cs一律设
//              置为gpio输出功能。根据l138的引脚管理特点，pinmux中的设置优先于
//              spi模块中的设置，如果在pinmux中把spi0的scs0引脚设置为GP2[14]的
//              话，该引脚将由系统gpio模块控制，spi模块对它的所有设置和操作均无
//              效。因此，配置spi之前，应该先在pinmux中把该引脚配置为spi的cs功
//              能，ti的文档有误，pinmux中只能把这些引脚分配给spi使用，而不能指
//              定其为cs还是gpio。是否作为cs使用，由spi模块决定
// 输入参数：tpSpi，被操作的spi控制结构的地址
//           tagpInConfig，配置结构，包含配置信息
// 输出参数：无
// 返回值  ：true=成功，false=失败
// =========================================================================
bool_t Spi_Init(volatile tagSpiReg * tpSpi, tagSpiConfig* tagpInConfig)
{
    u32 u32Prescaler;
    if(tpSpi == g_ptSpi0Reg)
        Cpucfg_EnableLPSC(cn_PSC0_SPI0);
    else if(tpSpi == g_ptSpi1Reg)
        Cpucfg_EnableLPSC(cn_PSC1_SPI1);
    else
        return false;
    if (tagpInConfig != NULL)
    {

      // reset tpSpi port.
      tpSpi->GCR0 = 0;

      tpSpi->GCR0 |= CN_SPI_RESET;

      // config master/slave mode.
      if (SPI_MODE_MASTER == tagpInConfig->tagMode)
      {
         // set clkmod and master for master mode.
         tpSpi->GCR1 = CN_SPI_CLKMOD | CN_SPI_MASTER;
      }
      else if (SPI_MODE_SLAVE == tagpInConfig->tagMode)
      {
         // clear spigcr1 for slave mode.
         tpSpi->GCR1 = 0;
      }
      else
      {
         goto error_exit;
      }

      // config pin options.
      switch (tagpInConfig->tagPinOption)
      {
         case SPI_3PIN:
            // enable tpSpi SOMI, SIMO, and CLK.
            tpSpi->PC0 = SOMI | SIMO | CLK;
            break;

         case SPI_4PIN_CS:
            // enable tpSpi SOMI, SIMO, CLK, and set cs[0~7] as gpio.
            tpSpi->PC0 = SOMI | SIMO | CLK;
            tpSpi->PC0 &= ~all_cs;        //所有片选脚设为gpio
            tpSpi->PC4 = all_cs;          //拉高所有片选
            tpSpi->PC1 |= all_cs;         //所有片选脚设为输出
            break;

         case SPI_4PIN_EN:
            // enable tpSpi SOMI, SIMO, CLK, and ENA.
            tpSpi->PC0 = SOMI | SIMO | CLK | ENA;
            break;

         case SPI_5PIN:
            // enable tpSpi SOMI, SIMO, CLK, SCS0, ENA and set cs[0~7] as gpio.
            tpSpi->PC0 = SOMI | SIMO | CLK | ENA;
            tpSpi->PC0 &= ~all_cs;        //所有片选脚设为gpio
            tpSpi->PC4 = all_cs;          //拉高所有片选
            tpSpi->PC1 |= all_cs;         //所有片选脚设为输出
            break;

         default:
             goto error_exit;
      }

      // config tpSpi direction, polarity, and phase.
      tpSpi->FMT0 = 0;

      if (SPI_SHIFT_LSB == tagpInConfig->tagShiftDir)
      {
         tpSpi->FMT0 |= SHIFTDIR;
      }

      if (tagpInConfig->polarity)
      {
         tpSpi->FMT0 |= POLARITY;
      }

      if (tagpInConfig->phase)
      {
         tpSpi->FMT0 |= PHASE;
      }

      // set the u32Prescaler and character length.
      u32Prescaler = (((CN_CFG_PLL0_SYSCLK2 / tagpInConfig->freq) - 1) & 0xFF);
      //u32Prescaler = 0x80;
      tpSpi->FMT0 |= u32Prescaler << SHIFT_PRESCALE;

      tpSpi->FMT0 |= (tagpInConfig->char_len&0x1f)<< SHIFT_CHARLEN;

      tpSpi->DELAY = (8 << 24) | (8 << 16);

      // disable interrupts.
      tpSpi->INT = 0x00;
      tpSpi->LVL = 0x00;

      // enable tpSpi.
      tpSpi->GCR1 |= CN_SPI_ENABLE;

    }
    return true;

error_exit:
    if(tpSpi == g_ptSpi0Reg)
        Cpucfg_DisableLPSC(cn_PSC0_SPI0);
    else if(tpSpi == g_ptSpi1Reg)
        Cpucfg_DisableLPSC(cn_PSC1_SPI1);
    return false;
}

// =========================================================================
// 函数功能：设置片选有效
// 输入参数：tpSpi，被操作的spi控制结构的地址
//           ufCsNo，片选号
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Spi_ActiveCS(volatile tagSpiReg * const tagpSpi,ufast_t ufCsNo)
{
    if(tagpSpi != NULL)
        tagpSpi->PC5 = 1<<ufCsNo;                       //拉低片选
    return;
}

// =========================================================================
// 函数名称：Spi_InActiveCS
// 函数功能：设置片选无效
// 输入参数：tpSpi，被操作的spi控制结构的地址
//           ufCsNo，片选号
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Spi_InActiveCS(volatile tagSpiReg * const tpSpi,ufast_t ufCsNo)
{
    if(tpSpi != NULL)
        tpSpi->PC4 = 1<<ufCsNo;                         //拉高片选
    return;
}

// =========================================================================
// 函数名称：Spi_TxRx
// 函数功能：从spi口发送数据，同时接收数据。因spi总线特性，收发是同时进行的。
// 输入参数：tpSpi，被操作的spi控制结构的地址
//           ufCsNo，片选号
//           u8pSrc，源缓冲区地址，可为NULL
//           u32SrcLen，源缓冲区长度，即需要发送的数据长度
//           u32DstLen，接收缓冲区长度，也就是欲接收的数据量。
//           u32DstOffset，接收偏移，即发送一定数量数据后，才开始接收，这对需要
//              先发命令给从器件，再从从器件读数据的方案非常有用，因为发命令时收
//              到的数据是必须丢弃的。
// 输出参数：u8pDst，接收缓冲区地址，可为NULL
// 返回值  ：收到的数据量
// =========================================================================
u32 Spi_TxRx(volatile tagSpiReg * const tpSpi,u8 *u8pSrc,u32 u32SrcLen,
                u8 *u8pDst,u32 u32DstLen,u32 u32DstOffset)
{
   u32 u32Sent=0,u32Received=0;

    if(((u8pSrc==NULL) && (u32SrcLen != 0))
                || ((u8pDst==NULL) && (u32DstLen != 0)))
        return 0;
    if (tpSpi != NULL)
    {
        tpSpi->BUF;                               //清空接收寄存器
        while((u32Sent < u32SrcLen)
                    || (u32Received < (u32DstLen + u32DstOffset) ))
        {
            while (tpSpi->BUF & TXFULL) {}            //等待发送寄存器空
            if(u32Sent < u32SrcLen)
            {
                tpSpi->DAT0 = u8pSrc[u32Sent];              //发送数据
                while(tpSpi->BUF & RXEMPTY) {}    //等待接收有效
                u32Sent++;
            }else
            {
                tpSpi->DAT0 = 0;                  //发送空数据，用于产生接收时钟
                while(tpSpi->BUF & RXEMPTY) {}    //等待接收有效
            }
            if(u32DstLen != 0)
            {
                //需要接收数据
                if((u32Received >= u32DstOffset)
                            &&(u32Received <(u32DstLen+u32DstOffset)))
                {
                    u8pDst[u32Received - u32DstOffset] = (u8)tpSpi->BUF;
                }
                u32Received++;
            }
        }
    }

    return (u32Received - u32DstOffset);
}

