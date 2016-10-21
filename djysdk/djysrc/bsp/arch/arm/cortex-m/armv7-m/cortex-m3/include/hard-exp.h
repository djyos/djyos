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
//文件描述: cm3异常处理模块
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-08-30
//   作者: lst
//   新版本号: V1.0.0
//   修改说明:
//------------------------------------------------------
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "board-config.h"
#include "exp.h"

#define CN_EXP_TYPE_HARD_FAULT      (CN_EXP_TYPE_HARD_START+0)
#define CN_EXP_TYPE_MEMMANAGE_FAULT (CN_EXP_TYPE_HARD_START+1)
#define CN_EXP_TYPE_BUS_FAULT       (CN_EXP_TYPE_HARD_START+2)
#define CN_EXP_TYPE_USAGE_FAULT     (CN_EXP_TYPE_HARD_START+3)
#define CN_EXP_TYPE_DBG_FAULT       (CN_EXP_TYPE_HARD_START+4)

//系统异常表
#define CN_SYSVECT_RESET                    1
#define CN_SYSVECT_NMI                      2
#define CN_SYSVECT_HARD_FAULT               3
#define CN_SYSVECT_MEMMANAGE_FAULT          4
#define CN_SYSVECT_BUS_FAULT                5
#define CN_SYSVECT_USAGE_FAULT              6
#define CN_SYSVECT_SVC                      11
#define CN_SYSVECT_DEBUG                    12
#define CN_SYSVECT_PENDSV                   14
#define CN_SYSVECT_SYSTICK                  15

//异常解码名称和标志
#define CN_HARDEXP_DECODERNAME          "HARDEXP_DECODER"
#define CN_SYS_EXP_CPUINFO_VALIDFLAG    ((u32)(0x87654321))

//定义DEBUG FAULT状态位
#define CN_EXP_DFSR_HALTED                  (0x01 << 0)
#define CN_EXP_DFSR_BKPT                    (0x01 << 1)
#define CN_EXP_DFSR_DWTTRAP                 (0x01 << 2)
#define CN_EXP_DFSR_VCATCH                  (0x01 << 3)
#define CN_EXP_DFSR_EXTERNAL                (0x01 << 4)

//定义HARD FAULT状态位
#define CN_EXP_HFSR_VECTBL                  (0x01 << 1)
#define CN_EXP_HFSR_FORCED                  (0x01 << 30)
#define CN_EXP_HFSR_DEBUGEVT                (0x01 << 31)

//定义USAGE FAULT状态位
#define CN_EXP_UFSR_UNDEFINSTR              (0x01 << 0)
#define CN_EXP_UFSR_INVSTATE                (0x01 << 1)
#define CN_EXP_UFSR_INVPC                   (0x01 << 2)
#define CN_EXP_UFSR_NOCP                    (0x01 << 3)
#define CN_EXP_UFSR_UNALIGNED               (0x01 << 8)
#define CN_EXP_UFSR_DIVBYZERO               (0x01 << 9)

//定义BUS FAULT状态位
#define CN_EXP_BFSR_IBUSERR                 (0x01 << 0)
#define CN_EXP_BFSR_PRECISERR               (0x01 << 1)
#define CN_EXP_BFSR_IMPRECISERR             (0x01 << 2)
#define CN_EXP_BFSR_UNSTKERR                (0x01 << 3)
#define CN_EXP_BFSR_STKERR                  (0x01 << 4)
#define CN_EXP_BFSR_BFARVALID               (0x01 << 7)

//定义MEM FAULT状态位
#define CN_EXP_MFSR_IACCVIOL                (0x01 << 0)
#define CN_EXP_MFSR_DACCVIOL                (0x01 << 1)
#define CN_EXP_MFSR_MUNSTKERR               (0x01 << 3)
#define CN_EXP_MFSR_MSTKERR                 (0x01 << 4)
#define CN_EXP_MFSR_MMARVALID               (0x01 << 7)


//该异常信息是属于CPU 的CORE 信息，属于CORE所有，和具体CPU型号无关
struct SysExpCoreInfo
{
    u32 spr_sp;
    u32 ger_r4;
    u32 ger_r5;
    u32 ger_r6;
    u32 ger_r7;
    u32 ger_r8;
    u32 ger_r9;
    u32 ger_r10;
    u32 ger_r11;    //r4-r11属于现在栈也保存
    u32 ger_r0;
    u32 ger_r1;
    u32 ger_r2;
    u32 ger_r3;
    u32 ger_r12;
    u32 spr_lr;
    u32 spr_pc;
    u32 spr_xpsr;   //r0-r3,r12-r15是自动大压栈
};

struct SysExpNvicInfo
{
    u32 bfar;       //bus fault address register
    u32 mmar;       //memory manage fault address register
    u32 dfsr;       //debug fault state register
    u32 hfsr;       //hard fault state register
    u16 ufsr;       //usage fault state register
    u8  bfsr;       //bus fault state register
    u8  mfsr;       //memory manage fault state register
};

struct SysExceptionInfo
{
    u32 SysExpType;                     //异常信息类型
    u32 SysExpCpuFlag;                 //该异常信息的有效编码
    struct SysExpCoreInfo CoreInfo;
    struct SysExpNvicInfo NvicInfo;
};

extern u32 g_u32ExpTable[16];
void HardExp_Init(void);
void HardExp_ConnectSystick(void (*tick)(u32 inc_ticks));

#ifdef __cplusplus
}
#endif
#endif // _EXCEPTION_H_

