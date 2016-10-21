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
//所属模块:
//作者：罗侍田
//版本：V1.0.0
//文件描述:
//其他说明:
//修订历史:
//2. ...
//1. 日期  : 2014/12/17
//   修改人: 罗侍田
//   新版本号:
//   修改说明: 新建
//------------------------------------------------------

#ifndef __STDDEF_H__
#define __STDDEF_H__

#ifdef  __cplusplus
extern "C"{
#endif

#include "stdint.h"

typedef ptu32_t ptrdiff_t;
typedef u32 size_t;     //todo,这里必须跟编译器的设置一致,不然memcpy等有问题。
typedef size_t off_t;
typedef s32 ssize_t;
typedef s32 mode_t;
void NULL_func(void);

#ifndef true
  #define true  1
#endif
#ifndef false
  #define false (ufast_t)0
#endif
#ifndef FALSE
    #define FALSE             0
#endif
#ifndef TRUE
    #define TRUE              1
#endif

typedef ufast_t         bool_t;

#ifndef __cplusplus  /* wchar_t is a builtin type for C++ */
  #if !defined(__STRICT_ANSI__)
  /* unconditional in non-strict C for consistency of debug info */
    #if defined(__WCHAR32)
      typedef u32 wchar_t; /* also in <stdlib.h> and <inttypes.h> */
      typedef u32 wint_t;  /* also in <stdlib.h> and <inttypes.h> */
    #else
      typedef u16 wchar_t; /* also in <stdlib.h> and <inttypes.h> */
      typedef u16 wint_t;  /* also in <stdlib.h> and <inttypes.h> */
    #endif
  #elif !defined(__wchar_t)
    #define __wchar_t 1
    #if defined(__WCHAR32)
      typedef u32 wchar_t; /* also in <stdlib.h> and <inttypes.h> */
      typedef u32 wint_t;  /* also in <stdlib.h> and <inttypes.h> */
    #else
      typedef u16 wchar_t; /* also in <stdlib.h> and <inttypes.h> */
      typedef u16 wint_t;  /* also in <stdlib.h> and <inttypes.h> */
    #endif
   /*
    * An integral type whose range of values can represent distinct codes for
    * all members of the largest extended character set specified among the
    * supported locales; the null character shall have the code value zero and
    * each member of the basic character set shall have a code value when used
    * as the lone character in an integer character constant.
    */
  #endif
#else
#if defined(__WCHAR32)
  typedef   u32 wint_t;
#else
  typedef   u16 wint_t;
#endif
#endif

#ifndef __cplusplus
#ifndef NULL
    #define     NULL ((void*) 0)
#endif
#else   /* C++ */
#define NULL 0
#endif

typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    unsigned char __wchb[4];
  } __value;    /* Value so far.  */
} _mbstate_t;

   /* null pointer constant. */

  /* EDG uses __INTADDR__ to avoid errors when strict */
//lst 有__INTADDR__ 和__CLIBNS, 在minGW下无法编译
//  #define offsetof(t, memb) ((__CLIBNS size_t)__INTADDR__(&(((t *)0)->memb)))
  #define offsetof(t, memb) ((size_t)(&(((t *)0)->memb)))


#ifdef  __cplusplus
}
#endif

#endif

/* end of stddef.h */

