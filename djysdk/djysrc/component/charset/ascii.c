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

//所属模块: ASCII字符编码驱动
//作者：mjh
//版本：V1.0.0
//文件描述: ASCII字符编码解析
//其他说明:
//修订历史:
//    2. ...
//    1. 日期:2011-05-17
//       作者:mjh
//       新版本号：
//       修改说明:初始版本
//------------------------------------------------------
#include "stdint.h"
#include "stdio.h"
#include "stddef.h"
#include "ascii.h"
#include "charset.h"
#include "gkernel.h"
s32 AsciiMbToUcs4(u32* pwc, const char* s, s32 n);
s32 AsciiMbsToUcs4s(u32* pwcs, const char* mbs, s32 n);
s32 AsciiUcs4ToMb(char* s, s32 wc);
s32 AsciiUcs4sToMbs(char* mbs, const u32* pwcs, s32 n);

// 注释参照 charset.h-> struct Charset -> GetOneMb
s32 AsciiGetOneMb(const char* mbs,s32 n)
{
    if (mbs == NULL)
    {
        return 0;
    }
    else if(*mbs >= 0x80)       //串结束符也是合法字符
        return -1;
    else
        return 1;
}

// 注释参照 charset.h-> struct Charset -> MbToUcs4
s32 AsciiMbToUcs4(u32* pwc, const char* mbs,s32 n)
{
    u8 c;
    s32 result;
    if (mbs == NULL)
    {
        return 0;
    }
    c = *mbs;

    if(c == 0)
    {
        result = 0;
    }else if(c < 0x80)
    {
        result = 1;
    }
    else
        result = -1;
    if(pwc != NULL)
        *pwc = c;
    return result;
}

// 注释参照 charset.h-> struct Charset -> MbsToUcs4s
//todo：扩展ascii码怎么算?
s32 AsciiMbsToUcs4s(u32* pwcs, const char* mbs, s32 n)
{
    s32 wcn,len;
    if(mbs == NULL)
        return 0;
    if(n != -1)
        len = n;
    else
        len = CN_LIMIT_SINT32;
    for(wcn = 0; wcn < len; wcn++)
    {
        if( (mbs[wcn] == 0) || (mbs[wcn] >= 0x80) )
        {
            if(pwcs != NULL)
                pwcs[wcn] = (u32)0;
            return wcn;
        }
        else
        {
            if(pwcs != NULL)
                pwcs[wcn] = (u32)mbs[wcn];
        }
    }
    return wcn;
}

// 注释参照 charset.h-> struct Charset -> Ucs4ToMb
s32 AsciiUcs4ToMb(char* mbs, s32 wc)
{

    if(wc < 0x80){

        *mbs = wc;
        return 1;
    }

    return -1;
}

// 注释参照 charset.h-> struct Charset -> Ucs4sToMbs
s32 AsciiUcs4sToMbs(char* mbs, const u32* pwcs, s32 n)
{
    s32 asciin,len;
    if(pwcs == NULL)
        return 0;
    if(n != -1)
        len = n;
    else
        len = CN_LIMIT_SINT32;
    for(asciin = 0; asciin < len; asciin++)
    {
        if( (pwcs[asciin] == 0) || (pwcs[asciin] >= 0x80) )
        {
            if(mbs != NULL)
                mbs[asciin] = '\0';
            break;
        }
        else
        {
            if(mbs != NULL)
                mbs[asciin] = (u8)pwcs[asciin];
        }
    }
    return asciin;
}

//----安装ascii字符集----------------------------------------------------------
//功能: 安装ascii字符集，当系统使用西方字符界面时，使用这个字符集。特别注意，
//      gb2312已经包含了英文字符集，使用中文或中英文混合界面的，不需要安装ascii
//      字符集。但是，由于GB2312的字库只包含了全角的英文字符，故还需要安装ascii
//      的字体资源，尺寸(8*8、8*16)可选。
//参数: 无意义
//返回: 1=成功，0=失败
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_CharsetAscii(ptu32_t para)
{
    static struct Charset encoding;

    encoding.max_len = 1;
    encoding.EOC_Size = 1;
    encoding.GetOneMb = AsciiGetOneMb;
    encoding.MbToUcs4 = AsciiMbToUcs4;
    encoding.Ucs4ToMb = AsciiUcs4ToMb;
    encoding.MbsToUcs4s = AsciiMbsToUcs4s;
    encoding.Ucs4sToMbs = AsciiUcs4sToMbs;
    if( Charset_NlsInstallCharset(&encoding, CN_NLS_CHARSET_ASCII))
    {
        printf("ASCII encoding install sucess\r\n");
        return 1;
    }else
    {
        Djy_SaveLastError(EN_GK_CHARSET_INSTALL_ERROR);
        printf("ASCII encoding install fail\n\r");
        return 0;
    }
}

