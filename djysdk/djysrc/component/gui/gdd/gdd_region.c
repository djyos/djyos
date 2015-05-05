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

//所属模块: GDD
//作者:  LiuWei.
//版本：V1.0.0
//文件描述: 矩形运算函数集
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-11-10
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include    "gdd.h"
#include    "./include/gdd_private.h"

int RectW(const RECT *prc)
{
    return (prc->right - prc->left);
}

int RectH(const RECT *prc)
{
    return (prc->bottom - prc->top);
}

void    _SetRect(RECT *prc,int x,int y,int w,int h)
{
    prc->left   = x;
    prc->top    = y;
    prc->right  = x+w;
    prc->bottom = y+h;

}

void    SetRect(RECT *prc,int x,int y,int w,int h)
{
    if(prc!=NULL)
    {
        _SetRect(prc,x,y,w,h);
    }
}

void    SetRectEmpty(RECT *prc)
{
    if(prc!=NULL)
    {
        prc->left   = 0;
        prc->top    = 0;
        prc->right  = 0;
        prc->bottom = 0;
    }
}

BOOL    IsRectEmpty(const RECT *prc)
{
    if(prc!=NULL)
    {
        if((prc->right - prc->left)<=0)
        {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}
/*========================================================================================*/

void    _CopyRect(RECT *dst,const RECT *src)
{
    dst->left =src->left;
    dst->top =src->top;
    dst->right =src->right;
    dst->bottom =src->bottom;
}

BOOL    CopyRect(RECT *dst,const RECT *src)
{
    if(NULL!=dst)
    if(NULL!=src)
    {
        _CopyRect(dst,src);
        return TRUE;
    }
    return FALSE;
}

/*============================================================================*/

void    _OffsetRect(RECT *prc,int dx,int dy)
{
    prc->left += dx;
    prc->top  += dy;
    prc->right += dx;
    prc->bottom  += dy;
}

BOOL    OffsetRect(RECT *prc,int dx,int dy)
{
    if(NULL!=prc)
    {
        _OffsetRect(prc,dx,dy);
        return TRUE;
    }
    return FALSE;
}

/*============================================================================*/

void    _InflateRect(RECT *prc,int dx,int dy)
{
    prc->left   -= dx;
    prc->top    -= dy;
    prc->right  += dx;
    prc->bottom += dy;
}

BOOL    InflateRect(RECT *prc,int dx,int dy)
{
    if(NULL!=prc)
    {
        _InflateRect(prc,dx,dy);
        return TRUE;
    }
    return FALSE;
}

/*============================================================================*/

void    _InflateRectEx(RECT *prc,int l,int t,int r,int b)
{
    prc->left   -= l;
    prc->top    -= t;
    prc->right  += r;
    prc->bottom += b;
}

BOOL    InflateRectEx(RECT *prc,int l,int t,int r,int b)
{
    if(NULL!=prc)
    {
        _InflateRectEx(prc,l,t,r,b);
        return TRUE;
    }
    return FALSE;
}

/*============================================================================*/

BOOL    _PtInRect(const RECT *prc,const POINT *pt)
{
    if(pt->x < prc->left)   return FALSE;
    if(pt->x >= prc->right) return FALSE;

    if(pt->y < prc->top)        return FALSE;
    if(pt->y >= prc->bottom)    return FALSE;
    return TRUE;

}

BOOL    PtInRect(const RECT *prc,const POINT *pt)
{
    if(NULL!=prc)
    if(NULL!=pt)
    {
        return _PtInRect(prc,pt);
    }
    return FALSE;
}

/*============================================================================*/

BOOL    MakeProgressRect(RECT *dst,const RECT *src,u32 Range,u32 Val,EN_PB_MODE mode)
{
    s32 a0,a1;

    if(NULL == dst)
    {
        return FALSE;
    }

    if(NULL == src)
    {
        return FALSE;
    }

    //防止数据过大产生溢出
    if(Range>0x0FFFFFFF)
    {
        Range >>= 16;
        Val >>= 16;
    }
    else if(Range>0x00FFFFFF)
    {
        Range >>= 12;
        Val >>= 12;
    }
    else if(Range>0x000FFFFF)
    {
        Range >>= 8;
        Val >>= 8;
    }

    //防止除数为0而引发异常
    if(Range==0)
    {
        Range=1;
        Val=1;
    }

    if(Val>Range)
    {
        Val=Range;
    }

    switch(mode)
    {
        case    PBM_LEFT:
                a0 =(Val*RectW(src))/Range;
                a1 =RectW(src)-a0;

                dst[0].left =src->left;
                dst[0].top =src->top;
                dst[0].right =src->left+a0;
                dst[0].bottom =src->bottom;

                dst[1].left =src->left+a0;
                dst[1].top =src->top;
                dst[1].right =src->right;
                dst[1].bottom =src->bottom;
                return TRUE;
                ////
        case    PBM_RIGHT:
                a0 =(Val*RectW(src))/Range;
                a1 =RectW(src)-a0;

                dst[0].left =src->left+a1;
                dst[0].top =src->top;
                dst[0].right =src->right;
                dst[0].bottom =src->bottom;

                dst[1].left =src->left;
                dst[1].top =src->top;
                dst[1].right =src->left+a1;
                dst[1].bottom =src->bottom;
                return TRUE;
                ////
        case    PBM_TOP:
                a0 =(Val*RectH(src))/Range;
                a1 =RectH(src)-a0;

                dst[0].left =src->left;
                dst[0].top =src->top;
                dst[0].right =src->right;
                dst[0].bottom =src->top+a0;

                dst[1].left =src->left;
                dst[1].top =src->top+a0;
                dst[1].right =src->right;
                dst[1].bottom =src->bottom;
                return TRUE;
                ////

        case    PBM_BOTTOM:
                a0 =(Val*RectH(src))/Range;
                a1 =RectH(src)-a0;

                dst[0].left =src->left;
                dst[0].top =src->top+a1;
                dst[0].right =src->right;
                dst[0].bottom =src->bottom;

                dst[1].left =src->left;
                dst[1].top =src->top;
                dst[1].right =src->right;
                dst[1].bottom =src->top+a1;
                return TRUE;
                ////
        default:
                return FALSE;



    }
}

/*============================================================================*/

