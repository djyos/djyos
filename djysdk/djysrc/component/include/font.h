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

//所属模块: 字体驱动
//作者：lst
//版本：V1.0.0
//文件描述: 字体资源头文件
//其他说明:
//修订历史:
//    2. ...
//    1. 日期:2009-11-21
//       作者:lst
//       新版本号：
//       修改说明:初始版本
//------------------------------------------------------
#ifndef __FONT_H__
#define __FONT_H__
#include "stdint.h"
#include "gkernel.h"
#ifdef __cplusplus
extern "C" {
#endif

struct RectBitmap;
#define CN_FONT_RSC_TREE            "font"

/* 字体类型 */
#define CN_FONT_TYPE_DOT            1       // 点阵字体
#define CN_FONT_TYPE_TRUETYPE       2       //矢量字体

struct FontRsc
{
    struct  Object node;

    s32 MaxWidth;       //最宽字符的宽度,纵向显示时可用作为竖行宽
    s32 MaxHeight;      //最高字符的高度
    u32 Attr;

    bool_t (*LoadFont)(void *zk_addr);      //加载字体

    void (*UnloadFont)(void);      //卸载字体

// 获取ucs4编码为charcode字符的显示点阵，把点阵填充到font_bitmap中，调用者应该提
// 供font_bitmap所需内存。
// 如果font_bitmap参数中的bm_bits参数为NULL，则本函数退化为查询函数。在
// font_bitmap参数中返回图像参数，利用它可以计算bm_bits所需内存尺寸，这在一个文
// 档中使用多种尺寸的文字时，特别有用。
// resv,保留参数。
// 返回: true = 成功执行，false=不支持字符
    bool_t (*GetBitmap)(u32 charcode, u32 size, u32 resv,
                        struct RectBitmap *font_bitmap);
    s32 (*GetCharWidth)(u32 Charcode);      //获取某字符宽度
    s32 (*GetCharHeight)(u32 CharCode);     //获取某字符高度
};

bool_t Font_InstallFont(struct FontRsc *font,const char *name);

ptu32_t ModuleInstall_Font(ptu32_t para);

struct FontRsc* Font_GetCurFont(void);
struct FontRsc* Font_SetCurFont(struct FontRsc* font);
struct FontRsc* Font_SearchFont(const char* name);
s32 Font_GetFontLineHeight(struct FontRsc* font);
s32 Font_GetFontLineWidth(struct FontRsc* font);
s32 Font_GetFontAttr(struct FontRsc* font);

#ifdef __cplusplus
}
#endif

#endif //__FONT_H__

