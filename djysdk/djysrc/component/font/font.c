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
//文件描述: 字体资源初始化
//其他说明:
//修订历史:
//    2. ...
//    1. 日期:2009-11-21
//       作者:lst
//       新版本号：
//       修改说明:初始版本
//------------------------------------------------------
#include "stdint.h"
#include "stdio.h"
#include "object.h"
#include "font.h"
#include "ascii8x8.h"
#include "ascii8x16.h"
#include "gb2312_16.h"
#include "string.h"
#include <cfg/local_config.h>

static struct FontRsc *s_ptCurFont;


//----获取字体资源-------------------------------------------------------------
//功能: 根据字体名称获取字体资源，"C"是默认字体的代号。
//参数: 字体名称
//返回: 匹配的字体，若未有匹配编码，使用默认的ASCII编码
//-----------------------------------------------------------------------------
struct FontRsc* Font_GetLocFont(const char* font_name)
{
    struct FontRsc* font;

    if(strcmp(font_name, "C") == 0){

        font = Font_SearchFont(gc_pCfgDefaultFontName);

    }else
    {
        font = Font_SearchFont(font_name);
    }

    return font;
}

//----安装字体-----------------------------------------------------------------
//功能: 把新字体安装到系统资源队列中
//参数: font，新增的字体资源指针
//      name，新增字体名
//返回: true
//-----------------------------------------------------------------------------
bool_t Font_InstallFont(struct FontRsc *font,const char* name)
{
    struct Object *rsc;
    rsc = OBJ_SearchTree(CN_FONT_RSC_TREE);
    if(rsc == NULL)
        return false;       //字体资源根结点未创建

    OBJ_AddChild(rsc,&font->node,sizeof(struct FontRsc),RSC_FONT,name);
    if(s_ptCurFont == NULL)
        s_ptCurFont = font;

    return true;
}

//----字体驱动模块初始化-------------------------------------------------------
//功能: 初始化字体驱动模块
//参数: para: 无效
//返回: 1=成功，0=失败
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Font(ptu32_t para)
{
    static struct Object font_root_rsc;    // 字体资源

    s_ptCurFont = NULL;
    // 添加字体资源根节点
    if(OBJ_AddTree(&font_root_rsc,
                   sizeof(struct Object),RSC_RSCNODE,
                   CN_FONT_RSC_TREE))
    {
        return 1;
    }else
    {
        printf("install font module fail\r\n");
        return 0;
    }
}

//----获取当前字体---------------------------------------------------------
//功能: 获取当前使用的字体
//返回: 当前字体
//-----------------------------------------------------------------------------
struct FontRsc* Font_GetCurFont(void)
{
    return s_ptCurFont;
}

//----设定当前字体---------------------------------------------------------
//功能: 把新字体设为当前使用的字体，新字体必须事先安装到系统中，如果
//      字体树资源节点未创建，本函数什么都不做，返回原来的当前字体。
//参数: font，指定的字体。
//返回: NULL，设定失败
//      设定之前的字体。
//-----------------------------------------------------------------------------
struct FontRsc* Font_SetCurFont(struct FontRsc* font)
{
    struct Object *rsc;
    char *Name;
    if(font == NULL)
        return NULL;
    rsc = OBJ_SearchTree(CN_FONT_RSC_TREE);
    if(rsc == NULL)
        return NULL;       //字体资源树未创建
    Name = OBJ_Name(&font->node);
    rsc = OBJ_SearchChild(rsc,(const char*)Name);
    if(rsc != NULL)
    {
        s_ptCurFont = (struct FontRsc*)rsc;
    }
    return s_ptCurFont;
}

//----搜索字体资源---------------------------------------------------------
//功能: 根据名称搜索字体资源
//参数: name，指定的字体名称
//返回: NULL，无此字体资源
//      要找的字体资源
//-----------------------------------------------------------------------------
struct FontRsc* Font_SearchFont(const char* name)
{
    struct Object *rsc;

    rsc = OBJ_SearchTree(CN_FONT_RSC_TREE);
    if(rsc == NULL)
        return NULL;       //字体资源树未创建

    return (struct FontRsc*)OBJ_SearchChild(rsc, name);
}

//----取字体点阵行高-----------------------------------------------------------
//功能: 获取字体中字符的点阵行高,即该字体中最高的那个字符的高度.
//参数: font, 被查询的字体.
//返回: 该字库最高的那个字符的高度(像素值)
//-----------------------------------------------------------------------------
s32 Font_GetFontLineHeight(struct FontRsc* font)
{
    if(font == NULL)
        return 0;
    return font->MaxHeight;
}

//----取字体点阵竖行宽---------------------------------------------------------
//功能: 获取字体中字符的点阵竖行宽度,即该字体中最宽的那个字符的宽度.
//参数: font, 被查询的字体.
//返回: 该字库最宽的那个字符的宽度(像素值)
//-----------------------------------------------------------------------------
s32 Font_GetFontLineWidth(struct FontRsc* font)
{
    if(font == NULL)
        return 0;
    return font->MaxWidth;
}

//----取字体属性字-------------------------------------------------------------
//功能: 获取字体的属性字
//参数: font, 被查询的字体.
//返回: 字体属性字,font.c模块并不解析该属性字
//-----------------------------------------------------------------------------
s32 Font_GetFontAttr(struct FontRsc* font)
{
    if(font == NULL)
        return 0;
    return font->Attr;
}

