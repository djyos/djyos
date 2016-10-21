//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------

//所属模块: 字符编码驱动
//作者：mjh
//版本：V1.0.0
//文件描述: 字符编码资源初始化
//其他说明:
//修订历史:
//    2. ...
//    1. 日期:2011-05-17
//       作者:mjh
//       新版本号：
//       修改说明:初始版本
//------------------------------------------------------
#ifndef __CHARSET_H__
#define __CHARSET_H__

#include "stdint.h"
#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CN_CHAR_ENCODING_RSC_TREE       "charset encoding"

#define CN_NLS_CHARSET_ASCII            "ascii"
#define CN_NLS_CHARSET_GB2312           "gb2312"
#define CN_NLS_CHARSET_UTF8             "utf8"
#define CN_NLS_CHARSET_CYRIL            "cyril"     //西里尔字符集

#define CN_NLS_CHARSET_DEFAULT          CN_NLS_CHARSET_GB2312

/* 多字节字符编码资源 */
struct Charset
{
    struct Object node;

    u16 max_len;        // 单个字符的最大字节数
    u16 EOC_Size;       // 字符串结束符的字节数

//----取多字节字符-------------------------------------------------------------
//功能: 从多字节字符集中取一个多字节字符，不转换为ucs4字符。
//参数: mbs, 指向待转换的多字节字符字节序列的指针(由调用函数判断s的合法性)
//      n，最大检测长度，-1表示检测全串。
//返回: 0: mbs是NULL指针。
//      -1:mbs指向的不是合法多字节字符。
//      其他：被转换的字符长度（字节数）,注意，串结束符也是合法字符
//说明：返回的多字节字符，即mbs指向的字符。
    s32 (*GetOneMb)(const char* mbs, s32 n);

//----多字节字符转为ucs4字符---------------------------------------------------
//功能: 把一个多字节字符转换为ucs4字符。
//参数: pwc，保存转换结果的指针，若为NULL,则不返回转换结果
//      mbs, 指向待转换的多字节字符字节序列的指针(由调用函数判断s的合法性)
//      n，最大检测长度，-1表示检测全串。
//返回: 0: mbs是NULL指针或者mbs指向空串。
//      -1:mbs指向的不是合法多字节字符。
//      其他：被转换的字符长度（字节数）
//说明：
//      此函数是C Run-time库的mblen及mbtowc服务函数。
    s32 (*MbToUcs4)(u32* pwc, const char* mbs, s32 n);

//----ucs4字符转为多字节字符---------------------------------------------------
//功能: 把一个ucs4字符转换为多字节字符。
//参数: mb，保存转换结果的指针(由调用函数判断s的合法性)
//      wc，待转换的字符
//返回：-1,如果wc不能对应一个有效的多字节字符，
//      字节数，返回对应的多字节字符的字节数。
//说明：
//      此函数是C Run-time库的wctomb服务函数。
    s32 (*Ucs4ToMb)(char* mb, u32 wc);

//----多字节字符串转为ucs4串---------------------------------------------------
//功能: 把一个多字节字符串转换为ucs4字符串
//参数: pwcs，保存转换结果的指针，缓冲区由调用方提供，若空，则本函数转变为只计算
//          保存转换结果所需的字节数
//      mbs，保存多字节字符的缓冲区指针
//      n，最大检测长度，-1表示检测全串。
//返回: 0: mbs是NULL指针
//      -1:结束条件到达前，有不能转换的字符
//      其他:得到的字符数，=n表示源串是不包含串结束符'\0'。
//-----------------------------------------------------------------------------
    s32 (*MbsToUcs4s)(u32* pwcs, const char* mbs, s32 n);

//----ucs4字符串转为多字节字符串-----------------------------------------------
//功能: 把一个ucs4字符串转换为多字节字符串。
//参数: mbs，保存转换结果的指针，缓冲区由调用方提供，若空，则本函数转变为只计算
//          保存转换结果所需的字节数
//      pwcs，待转换的字符
//      n，最大检测长度，-1表示检测全串。注意ucs4的结束符是连续4个0x00.
//返回: 0: pwcs是NULL指针
//      -1:结束条件到达前，有不能转换的字符
//      其他:写入mbs缓冲区的字节数，含串结束符'\0'
//-----------------------------------------------------------------------------
    s32 (*Ucs4sToMbs)(char* mbs, const u32* pwcs, s32 n);
};


ptu32_t ModuleInstall_Charset(ptu32_t para);
bool_t  Charset_NlsInstallCharset(struct Charset *encoding,const char* name);
struct Charset* Charset_NlsGetCurCharset(void);
struct Charset* Charset_NlsSetCurCharset(struct Charset* encoding);
struct Charset* Charset_NlsSearchCharset(const char* name);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // __CHARSET_H__

