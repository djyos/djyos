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
//所属模块:堆管理模块
//作者：lst
//版本：V1.0.0
//文件描述: 内核配置头文件
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-10-31
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __ARCH_H_
#define __ARCH_H_

#include "arch_stdint.h"
#include "board-config.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef bool_t atom_high_t;
typedef bool_t atom_low_t;

//线程上下文中，用来保存寄存器所需要的栈，windows版本不关心
#define CN_REGISTER_STACK     0

//定义一些与对齐相关的宏
#define CN_MEM_ALIGN            1    //如果目标系统没有对齐要求，改为0
#define CN_ALIGN_SIZE           8    //arm(含cm3)要求8字节对齐

#if !defined(__MAVERICK__)
/* ARM traditionally used big-endian words; and within those words the
   byte ordering was big or little endian depending upon the target.
   Modern floating-point formats are naturally ordered; in this case
   __VFP_FP__ will be defined, even if soft-float.  */
#ifdef __VFP_FP__
# ifdef __ARMEL__
#  define __IEEE_LITTLE_ENDIAN
# else
#  define __IEEE_BIG_ENDIAN
# endif
#else
# define __IEEE_BIG_ENDIAN
# ifdef __ARMEL__
#  define __IEEE_BYTES_LITTLE_ENDIAN
# endif
#endif
#endif


//存储器大小端的配置
#define CN_CFG_LITTLE_ENDIAN       0
#define CN_CFG_BIG_ENDIAN          1
#define CN_CFG_BYTE_ORDER          CN_CFG_LITTLE_ENDIAN

//与CPU字长相关的配置
#define CN_CPU_BITS             32  //处理器字长
#define CN_PTR_BITS             32  //CPU地址位数，移植敏感
#define CN_CPU_BITS_SUFFIX_ZERO 5
#define CN_BYTE_BITS            8   //字节位宽



//以下是CPU可以用一条指令处理的数据类型，多用于需要保持数据完整性的操作，以及
//需要快速的操作。
//数据完整性和原子操作的异同:
//数据完整性是指在任何时候都能读到正确值的数据，比如下列语句:
//  uint32_t    b32;
//  b32++;  //b32的原值是0x0000ffff
//在32位risc机上，执行过程为：
//取b32地址到寄存器0-->取b32的数值到寄存器1-->寄存器1加1-->把寄存器1写入b32.
//上述过程无论在哪里被中断或者被高优先级线程打断，在中断或高优先级线程中读b32，
//要么得到0x0000ffff，要么得到0x00010000,这两都可以认为是正确数据.
//而在16位机上，执行过程是
//1、取0x0000到寄存器r0，取0xffff到寄存器r1
//2、执行加操作，使r0=0x0001,r1=0x0000
//3、把r0写入b32的高16位。
//4、把r1写入b32的低16位。
//如果在第3和第4步骤之间被中断打断，在中断里读b32，将得到错误的0x00001ffff。
//那么原子操作呢?就是整个执行过程不被任何中断或者高优先级线程打断，还看上面代码，
//如果b32被定义为原子变量，则无论在16位还是32位机上，b32都可以得到正确的值。目
//前，djyos并不支持原子变量，只能使用原子操作实现原子变量的效果。

#define CN_LIMIT_UCPU       CN_LIMIT_UINT32
#define CN_MIN_UCPU         0

#define CN_LIMIT_SCPU       CN_LIMIT_SINT32
#define CN_MIN_SCPU         CN_MIN_SINT32
//以下是快速数据类型，一般来说，CPU类型是快速数据类型，但有些系统存储器宽度小于
//CPU字长，导致从存储器读一个cpu字长的数需要多个存储器周期，另外,16位和32位cpu
//中,字节操作也并不一定最快.对于有些对字长没有要求的数据类型来说，使用与存储器
//宽度等长的变量也许是最快速的。配置16位SDRAM的44b0x快速类型是16位的.

#define CN_LIMIT_UFAST      CN_LIMIT_UINT8
#define CN_MIN_UFAST        0

#define CN_LIMIT_SFAST      CN_LIMIT_SINT8
#define CN_MIN_SFAST        CN_MIN_SINT8

//下面定义cpu系统的栈类型
#define CN_FULL_DOWN_STACK  0   //向下生长的满栈
#define CN_EMPTY_DOWN_STACK 1   //向下生长的空栈
#define CN_FULL_UP_STACK    2   //向上生长的满栈
#define CN_EMPTY_UP_STACK   3   //向上生长的空栈
#define CN_STACK_TYPE       CN_FULL_DOWN_STACK

//定义一些与对齐相关的宏
#define CN_MEM_ALIGN           1           //如果目标系统没有对齐要求，改为0
#define CN_ALIGN_SIZE          8           //arm(含cm3)要求8字节对齐

#ifdef __cplusplus
}
#endif
#endif // __ARCH_H_


