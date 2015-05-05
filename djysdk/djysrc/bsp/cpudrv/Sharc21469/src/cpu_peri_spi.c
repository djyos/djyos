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
//所属模块: SPI驱动模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: spi口驱动文件，提供对spi口初始化和输入输出驱动
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-10-06
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "cpu_peri_spi.h"

#include <cdef21469.h>
#include <def21469.h>
#include <sru.h>
#include <sysreg.h>

//SPI default setting
 tagSpiConfig   pg_spi_Config = {   SPI_MODE_MASTER,    //Master Mode
                                    SPI_SHIFT_MSB,      //MSBF
                                    1,                  //CLKPL = 1
                                    1,                  //CPHASE = 1
                                    100000,             //SPI BAUD = 100k
                                    8};                 //char_len = 8bits

// ----初始化SPI模块---------------------------------------------
//功能：目前，仅支持测试了SPI1为Master，无DMA功能，在MODE3下工作正常
//      设置参数采用默认设置，仅SPI波特率可以由应用程序通过tagspiConfig.spi_clk来修改
//参数：SPIx            SPI模块选择
//      *tagpInConfig   SPI参数配置结构体tagspiConfig
//返回：true=成功，false=失败
// --------------------------------------------------------------
bool_t Spi_Init(volatile tag_SpiReg * tpSpi, tagSpiConfig *tagpInConfig)
{
    u32 spi_div16=0;

    if((tpSpi != g_ptSpi0Reg)&&(tpSpi != g_ptSpi1Reg))//SPI1未测试
        return false;

    if (tagpInConfig != NULL)
    {
            spi_div16 = (CN_CFG_PCLK1/(tagpInConfig->freq*4))&0x7fff;

            tpSpi->rSPIDMAC = 0;            //no DMA
            tpSpi->rSPIBAUD = spi_div16<<1; //SPI Baud
            tpSpi->rSPIFLG = 0xF80;         //reset value

            //配置说明：
            //模式3，无DMA，CS片选位由IO控制，不使用SPI模块的自动片选功能
            //TIMOD=01，SPIMS-Master，WL8-8位数据，MSBF-MSB位优先
            //OPD-开漏模式，CLKPL-Clock Polarity，CPHASE-Clock Phase
            tpSpi->rSPICTL = SPIEN|SENDZ|TIMOD1;

            if(tagpInConfig->tagMode == SPI_MODE_MASTER)
                tpSpi->rSPICTL |= SPIMS;

            if(tagpInConfig->tagShiftDir == SPI_SHIFT_MSB)
                tpSpi->rSPICTL |= MSBF;

            if(tagpInConfig->polarity)
                tpSpi->rSPICTL |= CLKPL;

            if(tagpInConfig->phase)
                tpSpi->rSPICTL |= CPHASE;

            if(tagpInConfig->char_len == 8)
                tpSpi->rSPICTL |= WL8;
            else if(tagpInConfig->char_len == 16)
                tpSpi->rSPICTL |= WL16;
            else if(tagpInConfig->char_len == 32)
                tpSpi->rSPICTL |= WL32;
            else
                return false;

            return true;
    }
    return false;
}

// ----SPI模块查忙-----------------------------------------------
//功能：查SPIF位
//参数：SPIx    SPI模块选择
//返回：true=成功，false=失败
// --------------------------------------------------------------
bool_t SPI_WaitFor(volatile tag_SpiReg * tpSpi)
{
    int nTimeout = 10000;
    // status updates can be delayed up to 10 cycles
    // so wait at least 10 cycles before even
    // checking them
    int n;

    if((tpSpi != g_ptSpi0Reg)&&(tpSpi != g_ptSpi1Reg))
        return false;

    // make sure nothing is waiting to be sent
    while( !(SPIF & tpSpi->rSPISTAT) )
    {
        if( nTimeout-- < 0 )
        {
            return false;
        }
    }

    return true;

}
// ----SPI模块查写状态-------------------------------------------
//功能：确认写数据处于就绪状态
//参数：SPIx    SPI模块选择
//返回：true=成功，false=失败
// --------------------------------------------------------------
bool_t SPI_CheckTxReady(volatile tag_SpiReg * tpSpi)
{
    int nTimeout = 10000;
    // status updates can be delayed up to 10 cycles
    // so wait at least 10 cycles before even
    // checking them
    int n;

    if((tpSpi != g_ptSpi0Reg)&&(tpSpi != g_ptSpi1Reg))
        return false;

    // make sure nothing is waiting to be sent
    while( (TXS & tpSpi->rSPISTAT) )
    {
        if( nTimeout-- < 0 )
        {
            return false;
        }
    }

    return true;

}
// ----SPI模块查读状态-------------------------------------------
//功能：确认硬件缓存区有数据可读
//参数：SPIx    SPI模块选择
//返回：true=成功，false=失败
// --------------------------------------------------------------
bool_t SPI_CheckRxReady(volatile tag_SpiReg * tpSpi)
{
    int nTimeout = 10000;
    // status updates can be delayed up to 10 cycles
    // so wait at least 10 cycles before even
    // checking them
    int n;

    if((tpSpi != g_ptSpi0Reg)&&(tpSpi != g_ptSpi1Reg))
        return false;

    // make sure something is waiting to be read
    while( !(RXS & tpSpi->rSPISTAT) )
    {
        if( nTimeout-- < 0 )
        {
            return false;
        }
    }

    return true;

}

// ----SPI模块片选拉低-------------------------------------------
//功能：目前仅提供SPI1片选，适用于WB9260板
//参数：SPIx    SPI模块选择
//      tgCsSel 片选信号
//返回：true=成功，false=失败
// --------------------------------------------------------------
bool_t Spi_ActiveCS(volatile tag_SpiReg * tpSpi,ufast_t ufCsNo)
{
    if(tpSpi == g_ptSpi0Reg)
    {
        if( true != SPI_WaitFor(tpSpi) )
        {
            return false;
        }

        switch (ufCsNo)
        {
           case SPI_CS0:
              sysreg_bit_clr( sysreg_FLAGS, FLG4 ); //拉低ss0线 FLASH
              break;
           case SPI_CS1:
              sysreg_bit_clr( sysreg_FLAGS, FLG5 ); //拉低ss1线 FLASH
              break;
           case SPI_CS2:
              sysreg_bit_clr( sysreg_FLAGS, FLG6 ); //拉低ss2线 RTC
              break;
           default:
              return false;
        }
        return true;
    }
    else if(tpSpi == g_ptSpi1Reg)
    {
        return false;
    }
    else
        return false;

}

// ----SPI模块片选拉高-------------------------------------------
//功能：目前仅提供SPI1片选，适用于WB9260板
//参数：SPIx    SPI模块选择
//      tgCsSel 片选信号
//返回：true=成功，false=失败
// --------------------------------------------------------------
bool_t Spi_InActiveCS(volatile tag_SpiReg * tpSpi,ufast_t ufCsNo)
{
    if(tpSpi == g_ptSpi0Reg)
    {
        if( true != SPI_WaitFor(tpSpi) )
        {
            return false;
        }

        switch(ufCsNo)
        {
           case SPI_CS0:
              sysreg_bit_set( sysreg_FLAGS, FLG4 );    //拉高ss0线  FLASH
              break;
           case SPI_CS1:
              sysreg_bit_set( sysreg_FLAGS, FLG5 );    //拉高ss1线  FLASH
              break;
           case SPI_CS2:
              sysreg_bit_set( sysreg_FLAGS, FLG6 );    //拉高ss2线  RTC
              break;
           default:
              return false;
        }
    }
    else if(tpSpi == g_ptSpi1Reg)
    {
        return false;
    }
    else
        return false;

    return true;
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
u32 Spi_TxRx(volatile tag_SpiReg * const tpSpi,u8 *u8pSrc,u32 u32SrcLen,
                u8 *u8pDst,u32 u32DstLen,u32 u32DstOffset)
{
    u8 temp_data;
    u32 u32Sent=0,u32Received=0;

    if((tpSpi != g_ptSpi0Reg)&&(tpSpi != g_ptSpi1Reg))
        return false;

    if(((u8pSrc==NULL) && (u32SrcLen != 0))
                || ((u8pDst==NULL) && (u32DstLen != 0)))
        return false;

    if( true != SPI_WaitFor(tpSpi) )
    {
        return false;
    }

    while((u32Sent < u32SrcLen)
                    || (u32Received < (u32DstLen + u32DstOffset) ))
    {
        if( true != SPI_CheckTxReady(tpSpi) )
            return false;

        if(u32Sent < u32SrcLen)
        {
            //发送有效数据
            tpSpi->rTXSPI = u8pSrc[u32Sent];
            u32Sent++;
        }
        else
            tpSpi->rTXSPI = 0xFF;

        if( true != SPI_CheckRxReady(tpSpi) )
            return false;

        temp_data = tpSpi->rRXSPI;

        if(u32DstLen != 0)
        {
            if((u32Received >= u32DstOffset)
                        &&(u32Received <(u32DstLen+u32DstOffset)))
            {
                //接收有效数据
                u8pDst[u32Received - u32DstOffset] = temp_data;
            }
            u32Received++;
        }
    }

    return (u32Received - u32DstOffset);
}
