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
//所属模块: stdlib
//作者：罗侍田
//版本：V1.0.0
//文件描述: 浮点限定
//其他说明:
//修订历史:
//2. ...
//1. 日期  : 2014/12/11
//   修改人: 罗侍田
//   新版本号:
//   修改说明: 新建
//------------------------------------------------------

#ifndef _FLOAT_H___
#define _FLOAT_H___

//以下定义，参见《ISO/IEC 9899:1999 》第27页
#define FLT_RADIX           2
#define DECIMAL_DIG         17

#define FLT_MANT_DIG        24
#define FLT_EPSILON         (1.19209290E-07F)
#define FLT_DIG             6
#define FLT_MIN_EXP         (-125)
#define FLT_MIN             (1.17549435E-38F)
#define FLT_MIN_10_EXP      (-37)
#define FLT_MAX_EXP         128
#define FLT_MAX             (3.40282347E+38F)
#define FLT_MAX_10_EXP      38

#define DBL_MANT_DIG        53
#define DBL_EPSILON         (2.2204460492503131E-16)
#define DBL_DIG             15
#define DBL_MIN_EXP         (-1021)
#define DBL_MIN             (2.2250738585072014E-308)
#define DBL_MIN_10_EXP      (-307)
#define DBL_MAX_EXP         1024
#define DBL_MAX             (1.7976931348623157E+308)
#define DBL_MAX_10_EXP      308

#define LDBL_MANT_DIG       53
#define LDBL_EPSILON        (2.2204460492503131E-16)
#define LDBL_DIG            15
#define LDBL_MIN_EXP        (-1021)
#define LDBL_MIN            (2.2250738585072014E-308)
#define LDBL_MIN_10_EXP     (-307)
#define LDBL_MAX_EXP        1024
#define LDBL_MAX            (1.7976931348623157E+308)
#define LDBL_MAX_10_EXP     308

#endif /* _FLOAT_H___ */

