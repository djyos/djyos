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
//所属模块:平台无关数据类型定义
//作者：lst
//版本：V1.0.0
//文件描述: 内核配置头文件
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-10-16
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __STDINT_H__
#define __STDINT_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned  long long     uint64_t;
typedef signed    long long     sint64_t;
typedef unsigned  int           uint32_t;
typedef int                     sint32_t;
typedef int                     int32_t;
typedef unsigned  short         uint16_t;
typedef short int               sint16_t;
typedef short int               int16_t;
typedef unsigned  char          uint8_t;
typedef signed  char            sint8_t;
typedef signed  char            int8_t;
typedef uint64_t                u64;
typedef sint64_t                s64;
typedef uint32_t                u32;
typedef sint32_t                s32;
typedef uint16_t                u16;
typedef sint16_t                s16;
typedef uint8_t                 u8;
typedef sint8_t                 s8;
typedef uint64_t const          uc64;
typedef sint64_t const          sc64;
typedef uint32_t const          uc32;
typedef sint32_t const          sc32;
typedef uint16_t const          uc16;
typedef sint16_t const          sc16;
typedef uint8_t  const          uc8;
typedef sint8_t  const          sc8;
typedef unsigned char           ucpu_char;
typedef char                    scpu_char;

typedef volatile uint64_t       vu64;
typedef volatile sint64_t       vs64;
typedef volatile uint32_t       vu32;
typedef volatile sint32_t       vs32;
typedef volatile uint16_t       vu16;
typedef volatile sint16_t       vs16;
typedef volatile uint8_t        vu8;
typedef volatile sint8_t        vs8;
typedef volatile uint64_t const vuc64;
typedef volatile sint64_t const vsc64;
typedef volatile uint32_t const vuc32;
typedef volatile sint32_t const vsc32;
typedef volatile uint16_t const vuc16;
typedef volatile sint16_t const vsc16;
typedef volatile uint8_t  const vuc8;
typedef volatile sint8_t  const vsc8;


//ptu32_t类型与cpu的寻址范围有关，也与CPU的结构有关，如果该系统中指针长度小于或
//等于32位，则ptu32_t被定义为32位，否则与指针等长，它的用途有二:
//1.用于内存分配的指针限定,或者用于描述内存长度。
//2.用于可能需要转换成指针使用的整形数据，比如struct pan_device结构中的
//  ptu32_t  private_tag
//一般来说ptu32_t与ucpu_t相同,但也有意外,尤其是8位机和16位机中,例如51机的ucpu_t
//是8位的,但寻址范围是16位的,ptu32_t=16位.80c296单片机的ucpu_t是16位的，寻址范围
//是24位的，ptu32_t=32位。
typedef uint32_t                ptu32_t;

typedef uint32_t                ucpu_t;

typedef sint32_t                scpu_t;

typedef u8                      ufast_t;

typedef s8                      sfast_t;

typedef u64                     align_type;     //arm中，u64能满足所有对齐要求

#ifndef false
  #define false (ufast_t)0
#endif
#ifndef true
  #define true  (ufast_t)1
#endif

typedef ufast_t                 bool_t;

//typedef s64             time_t;

#define CN_LIMIT_UINT64         0xffffffffffffffff
#define CN_LIMIT_UINT32         0xffffffff
#define CN_LIMIT_PTU32          0xffffffff
#define CN_LIMIT_UINT16         0xffff
#define CN_LIMIT_UINT8          0xff
#define CN_LIMIT_SINT64         0x7fffffffffffffff
#define CN_LIMIT_SINT32         0x7fffffff
#define CN_LIMIT_SINT16         0x7fff
#define CN_LIMIT_SINT8          0x7f

#define CN_MIN_UINT64           0
#define CN_MIN_PTU32            0
#define CN_MIN_UINT32           0
#define CN_MIN_UINT16           0
#define CN_MIN_UINT8            0
#define CN_MIN_SINT64           0x8000000000000000
#define CN_MIN_SINT32           0x80000000
#define CN_MIN_SINT16           0x8000
#define CN_MIN_SINT8            0x80

#ifdef __cplusplus
}
#endif
#endif // __STDINT_H_


