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
//所属模块: I2C驱动模块
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
#ifndef __CPU_PERI_SPI_H__
#define __CPU_PERI_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  vu32 rSPICTL;        	// 0x0000
  vu32 rSPIFLG;          // 0x0001
  vu32 rSPISTAT;         // 0x0002
  vu32 rTXSPI;         	// 0x0003
  vu32 rRXSPI;         	// 0x0004
  vu32 rSPIBAUD;         // 0x0005
  vu32 rRXSPI_SHADOW;	// 0x0006
  vu32 RESERVED0[128];
  vu32 rIISPI;        // 0x0000
  vu32 rIMSPI;        // 0x0001
  vu32 rCSPI;         // 0x0002
  vu32 rCPSPI;        // 0x0003
  vu32 rSPIDMAC;      // 0x0004
}tag_SpiReg;

#define g_ptSpi0Reg	(volatile tag_SpiReg *)cn_spi1_baddr
#define g_ptSpi1Reg	(volatile tag_SpiReg *)cn_spi2_baddr
 
//-----------------------------------------------------------------------------
// Public Typedefs
//-----------------------------------------------------------------------------

typedef enum
{
    SPI_CS0 = 0,
    SPI_CS1 = 1,
    SPI_CS2 = 2,
    SPI_CS3 = 3,
}enum_spi_csx;

typedef enum
{
   SPI_MODE_SLAVE = 0,
   SPI_MODE_MASTER
} tagSpiMode;

typedef enum
{
   SPI_SHIFT_MSB = 0,
   SPI_SHIFT_LSB
} tagSpiShiftDir;

typedef struct
{
   tagSpiMode tagMode;			//Slave or Master 
   tagSpiShiftDir tagShiftDir;	//msb or lsb
   u8 polarity;					//Clock Polarity
   u8 phase;					//Clock Phase
   u32 freq;					//SPI BAUD
   u8 char_len;					//word length
} tagSpiConfig;

extern tagSpiConfig	pg_spi_Config;

bool_t Spi_Init(volatile tag_SpiReg * tpSpi, tagSpiConfig *tagpInConfig);
bool_t Spi_ActiveCS(volatile tag_SpiReg * tpSpi,ufast_t ufCsNo);
bool_t Spi_InActiveCS(volatile tag_SpiReg * tpSpi,ufast_t ufCsNo);
u32 Spi_TxRx(volatile tag_SpiReg * const tpSpi,u8 *u8pSrc,u32 u32SrcLen,
                u8 *u8pDst,u32 u32DstLen,u32 u32DstOffset);
                
#ifdef __cplusplus
}
#endif

#endif /*__CPU_PERI_SPI_H__*/

