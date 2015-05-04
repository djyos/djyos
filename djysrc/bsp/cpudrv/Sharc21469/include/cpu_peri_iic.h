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
//文件描述: I2C口驱动文件，提供对I2C口初始化和输入输出驱动
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-10-06
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_I2C_H__
#define __CPU_PERI_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif


//error code
#define ERR_NO_ERROR                    0
#define ERR_INVALID_PARAMETER           1
#define ERR_TIMEOUT                     2
#define ERR_WRITE_TIMEOUT               3
#define ERR_ACK_TIMEOUT                 4

//offset of reg
typedef struct
{
  vu8 rTWIDIV;         // 0x0000
  vu8 RESERVED0[3];
  vu8 rTWIMITR;        // 0x0004
  vu8 RESERVED1[3];
  vu8 rTWISCTL;        // 0x0008
  vu8 RESERVED2[3];
  vu8 rTWISSTAT;       // 0x000C
  vu8 RESERVED3[3];
  vu8 rTWISADDR;       // 0x0010
  vu8 RESERVED4[3];
  vu8 rTWIMCTL;        // 0x0014
  vu8 RESERVED5[3];
  vu8 rTWIMSTAT;       // 0x0018
  vu8 RESERVED6[3];
  vu8 rTWIMADDR;       // 0x001C
  vu8 RESERVED7[3];
//  vu8 rTWIIRPTL;	   // 0x0020
  vu8 RESERVED8[4];
  vu8 rTWIIMASK;	   // 0x0024
  vu8 RESERVED9[3];
  vu8 rTWIFIFOCTL;	   // 0x0028
  vu8 RESERVED10[3];
  vu8 rTWIFIFOSTAT;	   // 0x002C
}tagI2CReg;

#define rTWIIRPTL 	((volatile unsigned int *)CN_IIC0_BADDR+0x0020)
//fifo regs
#define rTXTWI8 	((volatile unsigned int *)CN_IIC0_BADDR+0x0080)
#define rTXTWI16 	((volatile unsigned int *)CN_IIC0_BADDR+0x0084)
#define rRXTWI8 	((volatile unsigned int *)CN_IIC0_BADDR+0x0088)
#define rRXTWI16 	((volatile unsigned int *)CN_IIC0_BADDR+0x008c)

typedef enum
{
    I2C_CLOCK_RATE_50K,
    I2C_CLOCK_RATE_100K,
    I2C_CLOCK_RATE_200K,
    I2C_CLOCK_RATE_400K
}enum_I2C_Clock;//设置IIC总线的时钟


bool_t IIC0_Init(void);




#ifdef __cplusplus
}
#endif

#endif /*__CPU_PERI_SPI_H__*/

