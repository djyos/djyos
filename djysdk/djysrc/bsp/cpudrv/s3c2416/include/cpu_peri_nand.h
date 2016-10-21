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
//作者：djyos团队
//版本：V1.0.0
//文件描述: s3c2440 nand控制器寄存器定义
//其他说明:
//修订历史:
// 1. 日期: 2011-11-29
//   作者: djyos团队
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __CPU_PERI_NAND__
#define __CPU_PERI_NAND__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NFC_REG
{
    vu32 NFCONF;        //NAND Flash configuration
    vu32 NFCONT;        //NAND Flash control

    vu8  NFCMD;         //NAND Flash command
    vu8  rsv0[3];

    vu8  NFADDR;        //NAND Flash address
    vu8  rsv1[3];

    vu8  NFDATA;        //NAND Flash data
    vu8  rsv2[3];

    vu32 NFMECCD0;      //NAND Flash ECC for Main
    vu32 NFMECCD1;      //NAND Flash ECC for Main
    vu32 NFSECCD;       //NAND Flash ECC for Spare Area
    vu32 NFSBLK;        //NAND Flash programmable start block address
    vu32 NFEBLK;        //NAND Flash programmable end block address
    vu32 NFSTAT;        //NAND Flash operation status
    vu32 NFECCERR0;     //NAND Flash ECC Error Status for I/O [7:0]
    vu32 NFECCERR1;     //NAND Flash ECC Error Status for I/O [15:8]
    vu32 NFMECC0;       //SLC or MLC NAND Flash ECC status
    vu32 NFMECC1;       //SLC or MLC NAND Flash ECC status
    vu32 NFSECC;        //NAND Flash ECC for I/O[15:0]
    vu32 NFMLCBITPT;    //NAND Flash 4-bit ECC Error Pattern for data[7:0]
};

#define NFC_REG_BASE    ((struct NFC_REG*)0x4E000000)

/*
#define NFCONF      0x4E000000      //NAND Flash configuration
#define NFCONT      0x4E000004      //NAND Flash control
#define NFCMD       0x4E000008      //NAND Flash command
#define NFADDR      0x4E00000C      //NAND Flash address
#define NFDATA      0x4E000010      //NAND Flash data
#define NFMECCD0    0x4E000014      //NAND Flash ECC for Main
#define NFMECCD1    0x4E000018      //NAND Flash ECC for Main
#define NFSECCD     0x4E00001C      //NAND Flash ECC for Spare Area
#define NFSBLK      0x4E000020      //NAND Flash programmable start block address
#define NFEBLK      0x4E000024      //NAND Flash programmable end block address
#define NFSTAT      0x4E000028      //NAND Flash operation status
#define NFECCERR0   0x4E00002C      //NAND Flash ECC Error Status for I/O [7:0]
#define NFECCERR1   0x4E000030      //NAND Flash ECC Error Status for I/O [15:8]
#define NFMECC0     0x4E000034      //SLC or MLC NAND Flash ECC status
#define NFMECC1     0x4E000038      //SLC or MLC NAND Flash ECC status
#define NFSECC      0x4E00003C      //NAND Flash ECC for I/O[15:0]
#define NFMLCBITPT  0x4E000040      //NAND Flash 4-bit ECC Error Pattern for data[7:0]
*/
#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_NAND__
