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
#ifndef _STACK_DEFINE_H
#define _STACK_DEFINE_H

//attention: this defines only for the powerpc p1020
// in the e500 core, for the ABI protocal, it's an empty decrement stack, so
// all the defines following is under this imagination

#define  _NORMAL_TOP_OFFSET       16
#define  _IRQ_TOP_OFFSET          32
#define  _VM_STACK_FROM_TOP       -4
#define  _IRQ_COUNTER_FROM_TOP    -8

#define CUR_STACK_SIZE  304
//GENERAL REGISTER FROM THE STACK TOP--number 31
#define R0_OFFSET    16
#define R2_OFFSET    24
#define R3_OFFSET    32
#define R4_OFFSET    40
#define R5_OFFSET    48
#define R6_OFFSET    56
#define R7_OFFSET    64
#define R8_OFFSET    72
#define R9_OFFSET    80
#define R10_OFFSET   88
#define R11_OFFSET   96
#define R12_OFFSET   104
#define R13_OFFSET   112
#define R14_OFFSET   120
#define R15_OFFSET   128
#define R16_OFFSET   136
#define R17_OFFSET    144
#define R18_OFFSET    152
#define R19_OFFSET    160
#define R20_OFFSET    168
#define R21_OFFSET    176
#define R22_OFFSET    184
#define R23_OFFSET    192
#define R24_OFFSET    200
#define R25_OFFSET    208
#define R26_OFFSET    216
#define R27_OFFSET    224
#define R28_OFFSET    232
#define R29_OFFSET    240
#define R30_OFFSET    248
#define R31_OFFSET    256
//SPECIAL REGISTER OFFSET FROM THE STACK TOP--number 9
#define CR_OFFSET     268
#define XER_OFFSET    272
#define CTR_OFFSET    276

#define LR_OFFSET     284
#define MSR_OFFSET    288

#define PC_OFFSET     300

#endif  //  _STACK_DEFINE_H
