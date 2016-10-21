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
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: cm3异常处理模块
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-08-30
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明:
//------------------------------------------------------
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "board-config.h"

extern u8 usr_top[];
extern u8 usr_bottom[];
extern u8 svc_top[];
extern u8 svc_bottom[];
extern u8 irq_top[];
extern u8 irq_bottom[];
extern u8 fiq_top[];
extern u8 fiq_bottom[];
extern u8 undef_top[];
extern u8 undef_bottom[];
extern u8 abort_top[];
extern u8 abort_bottom[];

extern void (* engine_irq)(ufast_t ufl_line);


//异常解码名称和标志
#define CN_HARDEXP_DECODERNAME			"HARDEXP_DECODER"
#define CN_SYS_EXP_CPUINFO_VALIDFLAG    ((u32)(0x87654321))

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
	u32 ger_r11;	//r4-r11属于现在栈也保存

	u32 ger_r0;
	u32 ger_r1;
	u32 ger_r2;
	u32 ger_r3;
	u32 ger_r12;
	u32 spr_lr;
	u32 spr_pc;
	u32 spr_xpsr;	//r0-r3,r12-r15是自动大压栈
};

struct SysExpRegInfo
{
	u32 fpu_s16;
	u32 fpu_s17;
	u32 fpu_s18;
	u32 fpu_s19;
	u32 fpu_s20;
	u32 fpu_s21;
	u32 fpu_s22;
	u32 fpu_s23;
	u32 fpu_s24;
	u32 fpu_s25;
	u32 fpu_s26;
	u32 fpu_s27;
	u32 fpu_s28;
	u32 fpu_s29;
	u32 fpu_s30;
	u32 fpu_s31;
	struct SysExpCoreInfo CoreInfo;
	u32 fpu_s0;
	u32 fpu_s1;
	u32 fpu_s2;
	u32 fpu_s3;
	u32 fpu_s4;
	u32 fpu_s5;
	u32 fpu_s6;
	u32 fpu_s7;
	u32 fpu_s8;
	u32 fpu_s9;
	u32 fpu_s10;
	u32 fpu_s11;
	u32 fpu_s12;
	u32 fpu_s13;
	u32 fpu_s14;
	u32 fpu_s15;
	u32 fpu_fpscr;
	u32 fpu_res;
};

enum SysExpType
{
	CN_EXP_TYPE_MEM_FAULT = 0,
	CN_EXP_TYPE_BUS_FAULT,
	CN_EXP_TYPE_USG_FAULT,
	CN_EXP_TYPE_HARD_FAULT,
	CN_EXP_TYPE_DBG_FAULT
};

struct SysExceptionInfo
{
	u32 SysExpType;						//异常信息类型
	u32 SysExpCpuFlag;                 //该异常信息的有效编码
	struct SysExpRegInfo ExpRegInfo;
	//struct SysExpNvicInfo NvicInfo;
};

extern u32 g_u32ExpTable[16];
void Exp_Init(void);
void Exp_ConnectSystick(void (*tick)(u32 inc_ticks));

#ifdef __cplusplus
}
#endif

#endif // _EXCEPTION_H_

