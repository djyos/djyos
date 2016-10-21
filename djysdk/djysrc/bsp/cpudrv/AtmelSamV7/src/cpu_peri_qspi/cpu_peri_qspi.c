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
// 这份授权条款，在使用者符合以下二条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、此三条件表列，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、此三条件表列，以及下述
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
// 文件名     ：cpu_peri_spi.c
// 模块描述: SPI模块底层硬件驱动模块
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 9/12.2014
// =============================================================================


#include "string.h"
#include "cpu_peri.h"
#include "spibus.h"
#include "xdmac.h"
#include "xdmad.h"
#include "spi_dma.h"

// =============================================================================
#define tagQSpiReg  Qspi
#define sQspi QSPI

static struct SPI_CB s_QSPI_CB;

// =============================================================================
// 功能：时钟源为MCK，即150M分频，      QSCK BAUD = PCLK / (SCBR + 1)
// 参数：reg，寄存器基址
//      spiclk，欲配置的时钟速度，单位为Hz
// 返回：无
// =============================================================================
static void __QSPI_SetClk(volatile tagQSpiReg *Reg,u32 Fre)
{
    Reg->QSPI_SCR &= ~QSPI_SCR_SCBR_Msk;
    Reg->QSPI_SCR |= QSPI_SCR_SCBR(CN_CFG_MCLK/2/Fre - 1);
}

// =============================================================================
// 功能：SPI控制寄存器参数配置，如PHA和CPOL、时钟等，根据各种寄存器而异
// 参数：tpSPI,SPI控制器基址
//       ptr,参数指针
// 返回：无
// =============================================================================
static void __QSPI_Config(volatile tagQSpiReg *Reg,tagSpiConfig *ptr,u8 cs)
{
    if(cs > CN_CS_MAX)
        return;
    __QSPI_SetClk(Reg,ptr->Freq);

    //set the PHA
    if(ptr->Mode & SPI_CPHA)
    {
        Reg->QSPI_SCR |= QSPI_SCR_CPHA;
    }
    else
    {
        Reg->QSPI_SCR &= ~QSPI_SCR_CPHA;
    }
    //set the cpol
    if(ptr->Mode & SPI_CPOL)
    {
        Reg->QSPI_SCR |= QSPI_SCR_CPOL;
    }
    else
    {
        Reg->QSPI_SCR &= ~QSPI_SCR_CPOL;
    }

    if(ptr->CharLen <= 16 && ptr->CharLen >= 4)
    {
        Reg->QSPI_MR &= ~QSPI_MR_NBBITS(0xF);
        Reg->QSPI_MR |= QSPI_MR_NBBITS(ptr->CharLen - 8);
    }
}

// =============================================================================
// 功能: SPI默认硬件初始化配置，主要是时钟配置和GPIO写保护引脚配置
// 参数: RegBaseAddr,寄存器基址
// 返回: 无
// =============================================================================
static void __QSPI_HardConfig(u32 BaseAddr)
{
    tagQSpiReg *Reg;
    u32 Fre;

    Reg = (tagQSpiReg *)BaseAddr;

    Reg->QSPI_CR = QSPI_CR_QSPIDIS|QSPI_CR_SWRST;           //复位控制器
    //SPI Mode
    Reg->QSPI_MR = QSPI_MR_CSMODE(1)|QSPI_MR_DLYCS(128)| QSPI_MR_DLYBCT(64);
//    Reg->QSPI_SCR |= QSPI_SCR_DLYBS(32);

    //默认使用4M波特率
    Fre = 4*1000*1000;
    __QSPI_SetClk(Reg,Fre);
    //使用DMA方式
//    __QSPI_DmaConfig(BaseAddr);

    Reg->QSPI_CR = QSPI_CR_QSPIEN;
}

// =============================================================================
// 功能: SPI中断配置函数
// 参数: IntLine,中断线
// 返回: 无
// =============================================================================
//static void __QSPI_IntConfig(u8 IntLine)
//{
//    //中断线的初始化
//    u32 SPI_ISR(ufast_t IntLine);
//    Int_SetClearType(IntLine,CN_INT_CLEAR_PRE);
//    Int_Register(IntLine);
//    Int_IsrConnect(IntLine,SPI_ISR);
//    Int_SettoAsynSignal(IntLine);
//    Int_ClearLine(IntLine);
//    Int_RestoreAsynLine(IntLine);
//}

// =============================================================================
// 功能：SPI片选使能，使片选有效,若控制器有具体的配置片选寄存器，可直接返回
// 参数：Reg，本模块内即SPI寄存器基址
//       cs,片选线
// 返回：无
// 说明：SPI控制器上只引出了一个CS，若需要接多个CS，则必须应用程序自己控制CS
// =============================================================================
static bool_t __QSPI_BusCsActive(tagQSpiReg *Reg, u8 cs)
{
//  s_ChipSelect = cs;
//    Reg->QSPI_TDR |= SPI_TDR_PCS(cs);
//    Reg->QSPI_MR  &= ~SPI_MR_PCS(0x0F);
//    Reg->QSPI_MR  |= SPI_MR_PCS(cs);
    return true;
}

// =============================================================================
// 功能：SPI片选失能，使片选无效
// 参数：Reg，本模块内即SPI寄存器基址
//       cs,片选线
// 返回：无
// =============================================================================
static bool_t __QSPI_BusCsInActive(tagQSpiReg *Reg, u8 cs)
{
    Reg->QSPI_CR = QSPI_CR_LASTXFER| QSPI_CR_QSPIDIS;       //模式里面配置发了，配置这里就能拉高片选
    return false;
}

// =============================================================================
// 功能：SPI总线控制回调函数，被上层调用，目前只实现对SPI时钟频率配置
// 参数：specific_flag,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：无
// =============================================================================
static s32 __QSPI_BusCtrl(tagQSpiReg *Reg,u32 cmd,ptu32_t data1,ptu32_t data2)
{
    s32 result = 1;
    if( ((u32)Reg !=  (u32)sQspi))
        return 0;

    switch(cmd)
    {
    case CN_SPI_SET_CLK:
        __QSPI_SetClk(Reg,data2);
        break;
    case CN_SPI_CS_CONFIG:
        __QSPI_Config(Reg,(tagSpiConfig *)data1,data2);
        break;
    case CN_SPI_SET_AUTO_CS_EN:
        // 硬件上如果有自动产生CS的配置
        break;
    case CN_SPI_SET_AUTO_CS_DIS:
        break;
    default:
        break;
    }
    return result;
}

// =============================================================================
// 功能：读SPI接收寄存器，轮询方式,被__QSPI_TxRxPoll调用
// 参数：Reg,寄存器基址
// 返回：读到的数据
// =============================================================================
static u32 __QSPI_Read( tagQSpiReg * Reg )
{
    while ( (Reg->QSPI_SR & QSPI_SR_RDRF) == 0 ) ;

    return Reg->QSPI_RDR & 0xFFFF ;
}

// =============================================================================
// 功能：写SPI发送寄存器，轮询方式,被__QSPI_TxRxPoll调用
// 参数：Reg,寄存器基址，dwNpcs,片选；wData，写数据
// 返回：无
// =============================================================================
static void __QSPI_Write( tagQSpiReg * Reg, uint32_t dwNpcs, uint16_t wData )
{
    /* Send data */
    while ( (Reg->QSPI_SR & QSPI_SR_TXEMPTY) == 0 ) ;
    Reg->QSPI_TDR = wData;
}

// =============================================================================
// 功能：轮询方式读写数据
// 参数：Reg,寄存器基址，srcAddr,数据源指针；wrSize，写数据量，destAddr，读到的数据
//       地址指针；rdSize，读数据量，recvoff,读偏移，暂未用
// 返回：true,正确；false,错误
// =============================================================================
static bool_t __QSPI_TxRxPoll(tagQSpiReg *Reg,u8 *srcAddr,u32 wrSize,
        u8 *destAddr, u32 rdSize,u32 recvoff,u8 cs)
{
    u32 i;
    if(!srcAddr)
        return false;

    Reg->QSPI_CR = QSPI_CR_QSPIEN;
    i = Reg->QSPI_RDR;
    for (i=0;i<(wrSize+rdSize);i++)
    {
        if (i<wrSize)
        {
            __QSPI_Write(Reg,0x0,srcAddr[i]);
            __QSPI_Read(Reg);
        }
        else if((i>=wrSize)&&(i<(wrSize+rdSize)))
        {
          __QSPI_Write(Reg,0x0,0);
          if(destAddr)
            destAddr[i-wrSize] = __QSPI_Read(Reg);
        }
    }
//    Reg->QSPI_CR |= QSPI_CR_LASTXFER;

    return true;
}

// =============================================================================
// 功能：SPI底层驱动的初始化，完成整个SPI总线的初始化，其主要工作如下：
//       1.初始化总线控制块SPI_CB，回调函数和缓冲区的初始化赋值；
//       2.默认的硬件初始化，如GPIO或SPI寄存器等；
//       3.中断初始化，完成读写中断配置；
//       4.调用SPIBusAdd或SPIBusAdd_r增加总线结点；
// 参数：无
// 返回：初始化成功返回1，失败返回0
// 说明：此函数只支持QSPI用作普通的SPI时，挂接到spibus上面的用途
// =============================================================================
bool_t ModuleInstall_QSPI(ptu32_t para)
{
    struct SPI_Param QSPI_Config;
    u8 PerId;

    QSPI_Config.BusName             = "QSPI";
    QSPI_Config.SPIBuf          = NULL;//(u8*)&s_SPI0_Buf;
//  IntLine = CN_INT_LINE_QSPI;
    PerId   = ID_QSPI;

    QSPI_Config.SPIBufLen        = 0;//SPI_BUF_LEN;
    QSPI_Config.SpecificFlag     = (ptu32_t)sQspi;//(ptu32_t)tg_SPI_Reg[port];
    QSPI_Config.MultiCSRegFlag   = false;//true;
    QSPI_Config.pTransferPoll    = (TransferPoll)__QSPI_TxRxPoll;
    QSPI_Config.pTransferTxRx    = NULL;
    QSPI_Config.pCsActive        = (CsActiveFunc)__QSPI_BusCsActive;
    QSPI_Config.pCsInActive      = (CsInActiveFunc)__QSPI_BusCsInActive;
    QSPI_Config.pBusCtrl         = (SPIBusCtrlFunc)__QSPI_BusCtrl;

    __QSPI_HardConfig(QSPI_Config.SpecificFlag);
//    __QSPI_IntConfig(IntLine);
    PMC_EnablePeripheral(PerId);

    if(NULL == SPI_BusAdd_s(&s_QSPI_CB,&QSPI_Config))
        return 0;
    return 1;
}



