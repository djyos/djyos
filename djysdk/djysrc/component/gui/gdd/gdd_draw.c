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
//文件描述: 绘图管理层
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-11-10
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include    <gui/gdd/gdd_private.h>

/*============================================================================*/

static  bool_t    BeginDraw(HDC hdc)
{
    return TRUE;
}

static  void    EndDraw(HDC hdc)
{

}


static  void    __LPtoDP(HDC hdc,POINT *pt,s32 count)
{
    RECT rc;
    switch(hdc->DCType)
    {
        case    DC_TYPE_PAINT:
        case    DC_TYPE_CLIENT:
            GK_ApiGetArea(hdc->pGkWin, &rc);
            while(count--)
            {
                pt->x +=(hdc->hwnd->CliRect.left - rc.left);
                pt->y +=(hdc->hwnd->CliRect.top - rc.top);
                pt++;
            }
            break;
            ////

        case    DC_TYPE_WINDOW:
            while(count--)
            {
                //pt->x +=(hdc->hwnd->CliRect.left - hdc->hwnd->WinRect.left);
                //pt->y +=(hdc->hwnd->CliRect.top - hdc->hwnd->WinRect.top);
                //pt++;
            }
            break;

        default:
            break;
            ////
    }
}
/*============================================================================*/

u32 AlphaBlendColor(u32 bk_c,u32 fr_c,u8 alpha)
{
    u8 r1,g1,b1;
    u8 r2,g2,b2;

    r1 =bk_c>>16;
    g1 =bk_c>>8;
    b1 =bk_c;

    r2 =fr_c>>16;
    g2 =fr_c>>8;
    b2 =fr_c;


    r1 =(((r2-r1)*alpha)>>8) + r1;
    g1 =(((g2-g1)*alpha)>>8) + g1;
    b1 =(((b2-b1)*alpha)>>8) + b1;

    return   RGB(r1,g1,b1);

}
/*============================================================================*/

void    UpdateDisplay(u32 timeout)
{
    GK_ApiSyncShow(timeout);
}

/*============================================================================*/
/*============================================================================*/

void    __InitDC(DC *pdc,struct GkWinRsc *gk_win,HWND hwnd,s32 dc_type)
{

    pdc->pGkWin     =gk_win;
    pdc->hwnd       =hwnd;
    pdc->DCType     =dc_type;
    pdc->pFontDef   =Font_GetCurFont();
    pdc->pFont      =pdc->pFontDef;
    pdc->pCharset   =Charset_NlsGetCurCharset();

    pdc->CurX       =0;
    pdc->CurY       =0;
    pdc->DrawColor  =hwnd->DrawColor;
    pdc->FillColor  =hwnd->FillColor;
    pdc->TextColor  =hwnd->TextColor;
    pdc->SyncTime   =100*mS;
    pdc->RopCode    =(struct RopGroup){ 0, 0, 0, CN_R2_COPYPEN, 0, 0, 0  };

}

bool_t  DeleteDC(HDC hdc)
{
    if(hdc != NULL)
    {
//        GUI_DeleteDefFont(hdc->pFontDef);
        free(hdc);
        return TRUE;
    }
    return FALSE;
}


//----设置当前光栅码-------------------------------------------------------------
//描述: 略.
//参数：hdc: 绘图上下文句柄.
//     RopCode: 新的光栅码.
//返回：旧的光栅码.
//------------------------------------------------------------------------------
struct RopGroup SetRopCode(HDC hdc,struct RopGroup RopCode)
{
    struct RopGroup old=(struct RopGroup){ 0, 0, 0, CN_R2_COPYPEN, 0, 0, 0  };
    if(hdc!=NULL)
    {
        old =hdc->RopCode;
        hdc->RopCode =RopCode;
    }
    return old;
}

//----获得当前光栅码-------------------------------------------------------------
//描述: 略.
//参数：hdc: 绘图上下文句柄.
//返回：当前光栅码.
//------------------------------------------------------------------------------
struct RopGroup GetRopCode(HDC hdc)
{
    struct RopGroup old=(struct RopGroup){ 0, 0, 0, CN_R2_COPYPEN, 0, 0, 0  };
    if(hdc!=NULL)
    {
        old =hdc->RopCode;
    }
    return old;

}

//----设置当前坐标位置-----------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性.
//参数：hdc: 绘图上下文句柄.
//      x,y: 新的坐标位置.
//     old_pt: 输出旧的坐标位置,如果该参数为NULL,刚不输出旧的坐标位置.
//返回：无.
//------------------------------------------------------------------------------
static  void    __MoveTo(HDC hdc,s32 x,s32 y,POINT *old_pt)
{

    if(old_pt!=NULL)
    {
        old_pt->x =hdc->CurX;
        old_pt->y =hdc->CurY;
    }

    hdc->CurX =x;
    hdc->CurY =y;
}

//----设置当前坐标位置-----------------------------------------------------------
//描述: 当前坐标位置会影响LineTo画线的起始位置.
//参数：hdc: 绘图上下文句柄.
//      x,y: 新的坐标位置.
//     old_pt: 输出旧的坐标位置,如果该参数为NULL,刚不输出旧的坐标位置.
//返回：无.
//------------------------------------------------------------------------------
void    MoveTo(HDC hdc,s32 x,s32 y,POINT *old_pt)
{
    if(hdc!=NULL)
    {
        __MoveTo(hdc,x,y,old_pt);
    }
}

//----设置当前绘制颜色-----------------------------------------------------------
//描述: 当前绘制颜色会影响所有绘制类函数,如DrawRect,DrawCircle...
//参数：hdc: 绘图上下文句柄.
//      color: 新的绘制颜色.
//返回：旧的绘制颜色.
//------------------------------------------------------------------------------
u32 SetDrawColor(HDC hdc,u32 color)
{
    u32 old;

    if(hdc!=NULL)
    {
        old =hdc->DrawColor;
        hdc->DrawColor = color;
    }
    else
    {
        old =0;
    }
    return old;
}

//----获得当前绘制颜色-----------------------------------------------------------
//描述: 略.
//参数：hdc: 绘图上下文句柄.
//返回：当前绘制颜色.
//------------------------------------------------------------------------------
u32 GetDrawColor(HDC hdc)
{
    u32 old;

    if(hdc!=NULL)
    {
        old =hdc->DrawColor;
    }
    else
    {
        old =0;
    }
    return old;
}

//----设置当前填充颜色-----------------------------------------------------------
//描述: 当前填充颜色会影响所有填充类函数,如FillRect,FillCircle...
//参数：hdc: 绘图上下文句柄.
//      color: 新的填充颜色.
//返回：旧的绘制颜色.
//------------------------------------------------------------------------------
u32 SetFillColor(HDC hdc,u32 color)
{
    u32 old;

    if(hdc!=NULL)
    {
        old =hdc->FillColor;
        hdc->FillColor = color;
    }
    else
    {
        old =0;
    }
    return old;
}

//----获得当前填充颜色-----------------------------------------------------------
//描述: 略.
//参数：hdc: 绘图上下文句柄.
//返回：当前填充颜色.
//------------------------------------------------------------------------------
u32 GetFillColor(HDC hdc)
{
    u32 old;

    if(hdc!=NULL)
    {
        old =hdc->FillColor;
    }
    else
    {
        old =0;
    }
    return old;
}

//----设置当前文字颜色-----------------------------------------------------------
//描述: 当前文字颜色会影响所有文字绘制类函数,如TextOut,DrawText...
//参数：hdc: 绘图上下文句柄.
//      color: 新的文字颜色.
//返回：旧的文字颜色.
//------------------------------------------------------------------------------
u32 SetTextColor(HDC hdc,u32 color)
{
    u32 old;

    if(hdc!=NULL)
    {
        old =hdc->TextColor;
        hdc->TextColor = color;
    }
    else
    {
        old =0;
    }
    return old;
}

//----获得当前文字颜色-----------------------------------------------------------
//描述: 略.
//参数：hdc: 绘图上下文句柄.
//返回：当前文字颜色.
//------------------------------------------------------------------------------
u32 GetTextColor(HDC hdc)
{
    u32 old;

    if(hdc!=NULL)
    {
        old =hdc->TextColor;
    }
    else
    {
        old =0;
    }
    return old;
}

//----设置当前同步时间-----------------------------------------------------------
//描述: 略
//参数：hdc: 绘图上下文句柄.
//      sync_time: 新的同步时间.
//返回：旧的同步时间.
//------------------------------------------------------------------------------
u32 SetSyncTime(HDC hdc,u32 sync_time)
{
    u32 old;

    if(hdc!=NULL)
    {
        old =hdc->SyncTime;
        hdc->SyncTime = sync_time;
    }
    else
    {
        old =0;
    }
    return old;
}

//----获得当前同步时间-----------------------------------------------------------
//描述: 略
//参数：hdc: 绘图上下文句柄.
//返回：当前同步时间.
//------------------------------------------------------------------------------
u32 GetSyncTime(HDC hdc)
{
    u32 old;

    if(hdc!=NULL)
    {
        old =hdc->SyncTime;
    }
    else
    {
        old =0;
    }
    return old;
}

//----设置当前字体---------------------------------------------------------------
//描述: 略
//参数：hdc: 绘图上下文句柄.
//      hFont: 当前字体句柄.
//返回：旧的字体句柄.
//------------------------------------------------------------------------------
HFONT   SetFont(HDC hdc,HFONT hFont)
{
    HFONT old;

    if(hdc!=NULL)
    {
        old =hdc->pFont;
        hdc->pFont = hFont;
    }
    else
    {
        old =NULL;
    }
    return old;

}

//----获得当前字体---------------------------------------------------------------
//描述: 略
//参数：hdc: 绘图上下文句柄.
//返回：当前字体句柄.
//------------------------------------------------------------------------------
HFONT   GetFont(HDC hdc)
{
    HFONT old;

    if(hdc!=NULL)
    {
        old =hdc->pFont;
    }
    else
    {
        old =NULL;
    }
    return old;

}

//----设置指定位置像素颜色-------------------------------------------------------
//描述: 略
//参数：hdc: 绘图上下文句柄.
//      x,y: 坐标.
//      color:颜色值.
//返回：无.
//------------------------------------------------------------------------------
void SetPixel(HDC hdc,s32 x,s32 y,u32 color)
{
    POINT pt;
    if(BeginDraw(hdc))
    {
        pt.x =x;
        pt.y =y;
        __LPtoDP(hdc,&pt,1);
        GK_ApiSetPixel(hdc->pGkWin,pt.x,pt.y,color,hdc->RopCode.Rop2Mode,hdc->SyncTime);
        EndDraw(hdc);
    }
}

//----画线----------------------------------------------------------------------
//描述: 绘制单个像素宽的任意直线,使用DrawColor作为颜色值.
//参数：hdc: 绘图上下文句柄.
//      x0,y0: 起始坐标.
//      x1,y1: 结束坐标.
//返回：无.
//------------------------------------------------------------------------------
void    DrawLine(HDC hdc,s32 x0,s32 y0,s32 x1,s32 y1)
{
    POINT pt[2];

    if(BeginDraw(hdc))
    {
        pt[0].x =x0;
        pt[0].y =y0;
        pt[1].x =x1;
        pt[1].y =y1;
        __LPtoDP(hdc,pt,2);

        GK_ApiLinetoIe(hdc->pGkWin,pt[0].x,pt[0].y,pt[1].x,pt[1].y,
                hdc->DrawColor,hdc->RopCode.Rop2Mode,hdc->SyncTime);
    }
}

void    DrawDottedLine(HDC hdc,s32 x0,s32 y0,s32 x1,s32 y1)
{
    POINT pt[2];
    u32 i,num,dot;
    s8 temp=1;
    if(BeginDraw(hdc))
    {
        if(x0==x1)
        {
            dot=y1-y0;
            if(dot==0)
                return;
            else if(dot<0)
            {
                dot=-dot;
                temp=-1;
            }
            else
            {
            }
            if(dot%2!=0)
                dot=dot+1;
            num=dot/2;
            for(i=1;i<=num;i++)
            {
                 pt[0].x =x0;
                 pt[0].y =y0+2*temp*(i-1);
                 pt[1].x =x1;
                 pt[1].y =y0+2*temp*(i);
                 __LPtoDP(hdc,pt,2);
                GK_ApiLinetoIe(hdc->pGkWin,pt[0].x,pt[0].y,pt[1].x,pt[1].y,
                        hdc->DrawColor,hdc->RopCode.Rop2Mode,hdc->SyncTime);
            }
        }
        else if(y0==y1)
        {
          dot=x1-x0;
          if(dot==0)
              return;
          else if(dot<0)
          {
              dot=-dot;
              temp=-1;
          }
          else
          {
          }
          if(dot%2!=0)
                dot=dot+1;
          num=dot/2;
          for(i=1;i<=num;i++)
          {
             pt[0].x =x0+2*temp*(i-1);
             pt[0].y =y0;
             pt[1].x =x0+2*temp*(i);
             pt[1].y =y1;
             __LPtoDP(hdc,pt,2);
            GK_ApiLinetoIe(hdc->pGkWin,pt[0].x,pt[0].y,pt[1].x,pt[1].y,
                    hdc->DrawColor,hdc->RopCode.Rop2Mode,hdc->SyncTime);
          }
        }
        else
        {
        }
    }
}
//----画线----------------------------------------------------------------------
//描述: 使用指定颜色绘制单个像素宽的任意直线.
//参数：hdc: 绘图上下文句柄.
//      x0,y0: 起始坐标.
//      x1,y1: 结束坐标.
//返回：无.
//------------------------------------------------------------------------------
void    DrawLineEx(HDC hdc,s32 x0,s32 y0,s32 x1,s32 y1,u32 color)
{
    POINT pt[2];

    if(BeginDraw(hdc))
    {
        pt[0].x =x0;
        pt[0].y =y0;
        pt[1].x =x1;
        pt[1].y =y1;
        __LPtoDP(hdc,pt,2);

        GK_ApiLinetoIe(hdc->pGkWin,pt[0].x,pt[0].y,pt[1].x,pt[1].y,
                color,hdc->RopCode.Rop2Mode,hdc->SyncTime);
    }
}

//----使用当前位置画线-----------------------------------------------------------
//描述: 绘制单个像素宽的任意直线,使用DrawColor作为颜色值,使用当前位置作为起始坐标,
//     绘制完成后,并将结束坐标更新为当前坐标位置.
//参数：hdc: 绘图上下文句柄.
//      x,y: 结束坐标.
//返回：无.
//------------------------------------------------------------------------------
void    DrawLineTo(HDC hdc,s32 x,s32 y)
{
    POINT pt[2];

    if(BeginDraw(hdc))
    {
        pt[0].x =hdc->CurX;
        pt[0].y =hdc->CurY;
        pt[1].x =x;
        pt[1].y =y;

        __LPtoDP(hdc,pt,2);

        GK_ApiLinetoIe(hdc->pGkWin,pt[0].x,pt[0].y,pt[1].x,pt[1].y,
                hdc->DrawColor,hdc->RopCode.Rop2Mode,hdc->SyncTime);

        __MoveTo(hdc,x,y,NULL);
        EndDraw(hdc);
    }
}

//----在指定位置绘制字符串-------------------------------------------------------
//描述: 在指定位置绘制字符串,使用TextColor作为颜色值,支持回车与换行符.
//参数：hdc: 绘图上下文句柄.
//      x,y: 坐标位置.
//      text: 字符串.
//      count: 要绘制的字符数量,该参数小于0时,将绘制整个字符串.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    TextOut(HDC hdc,s32 x,s32 y,const char *text,s32 count)
{
        POINT pt;

        if(BeginDraw(hdc))
        {
            pt.x =x;
            pt.y =y;
            __LPtoDP(hdc,&pt,1);

            x =pt.x;
            y =pt.y;

//            if(count<0)
//            {
//                count = strlen(text);
//            }

            if(text!=NULL)
            {
               GK_ApiDrawText(hdc->pGkWin,hdc->pFont,hdc->pCharset,
                                      x,y,text,count,hdc->TextColor,
                                      hdc->RopCode.Rop2Mode,hdc->SyncTime);

            }

            EndDraw(hdc);
            return TRUE;
        }

        return FALSE;
}

//----计算字符串所占像素宽度-----------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数合法性.
//参数：ft: 使用的字体句柄
//     text: 字符串.
//     count: 要计算的字符数量,该参数小于0时,将计算整个字符串所占像素宽度.
//返回：字符串像素宽度.
//------------------------------------------------------------------------------
static  s32 __GetTextWidth(HDC hdc,const char *text,s32 count)
{
    s32 width;
    struct Charset *Set;
    struct FontRsc *Font;
    s32 len, loop;
    u32 wc;

    width=0;
    Set = hdc->pCharset;
    if(Set!=NULL)
    {
        Font = hdc->pFont;
        if(Font!=NULL)
        {
            for(loop = 0; (loop < count) || (count == -1);loop++)
            {
                len= Set->MbToUcs4(&wc, text, -1);
                if(len == -1)
                { // 无效字符
                    text++;
                }
                else if(len == 0)
                {
                    break;
                }
                else
                { // 有效字符
                    text += len;
                    width += Font->GetCharWidth(wc);
                }
            }
        }
    }
    return width;
}

//----计算字符串所占矩形大小-----------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数合法性.
//参数：ft: 使用的字体句柄
//     text: 字符串.
//     count: 要计算的字符数量,该参数小于0时,将计算整个字符串所占像素宽度.
//     rect: 调整后的矩形.
//     flag: 字符绘制标记.
//返回：无.
//------------------------------------------------------------------------------
void AdjustTextRect(HDC hdc,const char *text,s32 count, RECT *prc,u32 flag)
{
    s32 Height;
    s32 i;

    Height = Font_GetFontLineHeight(hdc->pFont);
    if(1)
    {
        switch(flag&DT_ALIGN_V_MASK)
        {
            case    DT_VCENTER:
                    prc->top += ((RectH(prc) - Height)>>1);
                    prc->bottom = prc->top + Height;
                    break;
                    ////
            case    DT_TOP:
                    break;
                    ////
            case    DT_BOTTOM:
                    prc->top += (RectH(prc) - Height);
                    prc->bottom =prc->top + Height;
                    break;
                    ////
            default:
                    break;

        }

        switch(flag&DT_ALIGN_H_MASK)
        {
            case    DT_CENTER:
                    i =__GetTextWidth(hdc,text,count);
                    i =(RectW(prc)-i)>>1;
                    prc->left  += i;
                    prc->right -= i;
                    break;
                    ////
            case    DT_LEFT:
                    prc->left  += 1;
                    break;
                    ////
            case    DT_RIGHT:
                    i =__GetTextWidth(hdc,text,count);
                    prc->left = prc->right-i-1;
                    break;
                    ////
            default:
                    break;

        }


    }
}

//----计算一个字符串有多少行-----------------------------------------------------
//描述: 略
//参数：str: 字符串指针
//返回：字符串行数.
//------------------------------------------------------------------------------
s32  GetStrLineCount(const char *str)
{
    s32 count,linenum = 0;
    char *line,*linenext;
    count=0;
    line = str;
    while(1)
    {
        linenext = mbstrchr(line, "\n", &count);
        if(linenext != NULL)
        {
            linenum++;
            line = linenext+1;
        }
        else
        {
            if(count != 0)
                linenum++;
            break;
        }
    }
    return linenum;
}

//----在矩形范围内绘制字符串---------------------------------------------------
//描述: 在指定矩形范围内绘制字符串,使用TextColor作为颜色值,支持回车与换行符,
//      该函数可以指定是否绘制字符串边框和背景,以前指定对齐方式的组合.
//参数：hdc: 绘图上下文句柄.
//      x,y: 坐标位置.
//      count: 要绘制的字符数量,该参数小于0时,将绘制整个字符串.
//      prc: 要绘制的所在矩形位置和大小.
//      flag: 绘制标志参数，DT_VCENTER 族常量
//返回：TRUE:成功; FALSE:失败.
//-----------------------------------------------------------------------------
bool_t    DrawText(HDC hdc,const char *text,s32 count,const RECT *prc,u32 flag)
{

    RECT rc,rc0;
    s32 line_count,charnum;

    if(NULL==prc)
    {
        return FALSE;
    }

    if(NULL==text)
    {
        return FALSE;
    }

    line_count =GetStrLineCount(text);

    if(BeginDraw(hdc))
    {
        if(line_count<=1)
        {
            //单行
            CopyRect(&rc0,prc);
            CopyRect(&rc,prc);
            InflateRect(&rc,-1,-1);
            AdjustTextRect(hdc,text,count,&rc,flag);

            if(flag&DT_BORDER)
            {
                DrawRect(hdc,&rc0);
                InflateRect(&rc0,-1,-1);
            }

            if(flag&DT_BKGND)
            {
                FillRect(hdc,&rc0);
            }

           TextOut(hdc,rc.left,rc.top,text,count);

        }
        else
        {
            //多行
            s32 x0,y0,i,line_h;
            char *p0,*p1;

            line_h = Font_GetFontLineHeight(hdc->pFont);

            CopyRect(&rc0,prc);
            CopyRect(&rc,prc);
            InflateRect(&rc,-1,-1);

            if(flag&DT_BORDER)
            {
                DrawRect(hdc,&rc0);
                InflateRect(&rc0,-1,-1);
            }

            if(flag&DT_BKGND)
            {
                FillRect(hdc,&rc0);
            }

            y0 =rc.top;
            switch(flag&DT_ALIGN_V_MASK)
            {
                case    DT_VCENTER://(line_count-1)*line_h)为行间距
                    y0 += ((RectH(&rc)-(line_count*line_h)-((line_count-1)*line_h))/2);
                        break;
                        ////
                case    DT_TOP:
                        break;
                        ////
                case    DT_BOTTOM://(line_count-1)*line_h)为行间距
                        y0 += (RectH(&rc) - (line_count*line_h)-((line_count-1)*line_h));
                        break;
                        ////
                default:
                        break;

            }

            p0 =(char*)text;
            while(p0!=NULL)
            {
                p1 = mbstrchr(p0,"\n",&charnum);

                if(p1 == p0)    //遇到连续换行符
                {
                    p0++;       //换行符肯定是1个字节的
                }
                else
                {
                    x0 =rc.left;
                    switch(flag&DT_ALIGN_H_MASK)
                    {
                        case    DT_CENTER:
                                x0 =(RectW(&rc)-__GetTextWidth(hdc,p0,charnum))>>1;
                                break;
                                ////
                        case    DT_LEFT:
                                x0  += 1;
                                break;
                                ////
                        case    DT_RIGHT:
                                x0 =(RectW(&rc)-__GetTextWidth(hdc,p0,charnum));
                                break;
                                ////
                        default:
                                break;

                    }

                    TextOut(hdc,x0,y0,p0,charnum);

                    if(p1==NULL)
                    {   //最后一行显示完后,则退出
                        break;
                    }

                    p0 = p1++;      //跳过换行符，肯定是1个字节
                }
                y0 += line_h;
            }
        }

        EndDraw(hdc);
        return TRUE;
    }

    return FALSE;

}

//----绘制矩形------------------------------------------------------------------
//描述: 使用DrawColor绘制一个空心矩形.
//参数：hdc: 绘图上下文句柄.
//      prc: 矩形参数.
//返回：无.
//------------------------------------------------------------------------------
void    DrawRect(HDC hdc,const RECT *prc)
{
    s32 x0,y0;
    s32 x1,y1;
    RECT rc;

    if(prc != NULL)
    {
        CopyRect(&rc,prc);
        if(BeginDraw(hdc))
        {
            __LPtoDP(hdc,(POINT*)&rc,2);

            x0 =rc.left;
            y0 =rc.top;
            x1 =rc.right-1;
            y1 =rc.bottom-1;

            //Left
            GK_ApiLinetoIe(hdc->pGkWin,x0,y0,x0,y1,hdc->DrawColor,hdc->RopCode.Rop2Mode,hdc->SyncTime);
            //Top
            GK_ApiLinetoIe(hdc->pGkWin,x0,y0,x1,y0,hdc->DrawColor,hdc->RopCode.Rop2Mode,hdc->SyncTime);
            //Right
            GK_ApiLinetoIe(hdc->pGkWin,x1,y0,x1,y1,hdc->DrawColor,hdc->RopCode.Rop2Mode,hdc->SyncTime);
            //Bottom
            GK_ApiLinetoIe(hdc->pGkWin,x0,y1,x1,y1,hdc->DrawColor,hdc->RopCode.Rop2Mode,hdc->SyncTime);

            EndDraw(hdc);
        }
    }

}

//----填充矩形------------------------------------------------------------------
//描述: 使用FillColor填充一个实心矩形.
//参数：hdc: 绘图上下文句柄.
//      prc: 矩形参数.
//返回：无.
//------------------------------------------------------------------------------
void    FillRect(HDC hdc,const RECT *prc)
{
    RECT rc;

    if(hdc!=NULL)
    {
        if(prc!=NULL)
        {
            CopyRect(&rc,prc);
            if(BeginDraw(hdc))
            {
                __LPtoDP(hdc,(POINT*)&rc,2);

                GK_ApiFillRect(hdc->pGkWin,&rc,hdc->FillColor,hdc->FillColor,
                            CN_FILLRECT_MODE_N,hdc->SyncTime);
                EndDraw(hdc);
            }
        }
    }

}
//----填充矩形------------------------------------------------------------------
//描述: 使用指定颜色填充一个实心矩形.
//参数：hdc: 绘图上下文句柄.
//      prc: 矩形参数.
//返回：无.
//------------------------------------------------------------------------------
void    FillRectEx(HDC hdc,const RECT *prc,u32 color)
{
    RECT rc;

    if(hdc!=NULL)
    {
        if(prc!=NULL)
        {
            CopyRect(&rc,prc);
            if(BeginDraw(hdc))
            {
                __LPtoDP(hdc,(POINT*)&rc,2);

                GK_ApiFillRect(hdc->pGkWin,&rc,color,color,
                            CN_FILLRECT_MODE_N,hdc->SyncTime);
                EndDraw(hdc);
            }
        }
    }

}

//----渐变填充矩形------------------------------------------------------------------
//描述: 使用Color1作为起始色,Color2作为结束色,渐变填充矩形.
//参数：hdc: 绘图上下文句柄.
//      prc: 矩形参数.
//      Color1: 超始颜色.
//      Color2: 结束颜色.
//      mode: 渐变递增模式,可以是以下值:
//            CN_FILLRECT_MODE_LR:丛左到右填充
//            CN_FILLRECT_MODE_UD:丛上到下
//            CN_FILLRECT_MODE_LU2RD:丛左上到右下
//            CN_FILLRECT_MODE_RU2LD:丛右上到左下
//返回：无.
//------------------------------------------------------------------------------
void    GradientFillRect(HDC hdc,const RECT *prc,u32 Color1,u32 Color2,u32 mode)
{
    struct Rectangle gk_rc;
    RECT rc;

    if(hdc!=NULL)
    {
        if(prc!=NULL)
        {
            if(BeginDraw(hdc))
            {
                CopyRect(&rc,prc);

                __LPtoDP(hdc,(POINT*)&rc,2);

                gk_rc.left = rc.left;
                gk_rc.top = rc.top;
                gk_rc.right = rc.right;
                gk_rc.bottom = rc.bottom;

                switch(mode)
                {
                    case CN_FILLRECT_MODE_LR:
                            mode =CN_FILLRECT_MODE_LR;
                            break;
                    case CN_FILLRECT_MODE_UD:
                            mode =CN_FILLRECT_MODE_UD;
                            break;
                    case CN_FILLRECT_MODE_LU2RD:
                            mode =CN_FILLRECT_MODE_LU2RD;
                            break;
                    case CN_FILLRECT_MODE_RU2LD:
                            mode =CN_FILLRECT_MODE_RU2LD;
                            break;
                    default:
                            mode =CN_FILLRECT_MODE_N;
                            break;

                }
                GK_ApiFillRect(hdc->pGkWin,&gk_rc,Color1,Color2,
                            mode,hdc->SyncTime);
                EndDraw(hdc);
            }
        }
    }

}

//----填充立体效果的矩形------------------------------------------------------------------
//描述: 使用Color1作为起始色(左,上边界),Color2作为结束色(右,下边界),填充一个立体效果的矩形.
//参数：hdc: 绘图上下文句柄.
//      prc: 矩形参数.
//      Color1: 超始颜色.
//      Color2: 结束颜色.
//返回：无.
//------------------------------------------------------------------------------
void    Fill3DRect(HDC hdc,const RECT *prc,u32 Color1,u32 Color2)
{
    u32 c;
    RECT rc;

    if(hdc!=NULL)
    if(prc!=NULL)
    {
        CopyRect(&rc,prc);
        c=SetDrawColor(hdc,Color1);
        DrawLine(hdc,0,0,0,RectH(&rc)-1); //L
        DrawLine(hdc,0,0,RectW(&rc)-1,0); //U

        SetDrawColor(hdc,Color2);
        DrawLine(hdc,RectW(&rc)-1,0,RectW(&rc)-1,RectH(&rc)-1); //R
        DrawLine(hdc,0,RectH(&rc)-1,RectW(&rc)-1,RectH(&rc)-1); //D
        SetDrawColor(hdc,c);

        c=SetFillColor(hdc,AlphaBlendColor(Color1,Color2,128));

        InflateRect(&rc,-1,-1);
        FillRect(hdc,&rc);
        SetFillColor(hdc,c);
    }

}

//----画圆------------------------------------------------------------------
//描述: 使用DrawColor画一个空心圆.
//参数：hdc: 绘图上下文句柄.
//      cx,cy: 圆的中心坐标
//      r: 圆的半径
//返回：无.
//------------------------------------------------------------------------------
void    DrawCircle(HDC hdc,s32 cx,s32 cy,s32 r)
{
     s32 x = 0;
     s32 y = r;
     s32 delta = 2*(1-r);
     s32 direction;
     u32 color;
     ////

     if(hdc!=NULL)
     {
         color =GetDrawColor(hdc);
         while (y >= 0)
         {

            SetPixel(hdc,cx+x, cy+y, color);
            SetPixel(hdc,cx-x, cy+y, color);
            SetPixel(hdc,cx-x, cy-y, color);
            SetPixel(hdc,cx+x, cy-y, color);
            ////

            if (delta < 0)
            {
                 if ((2*(delta+y)-1) < 0)
                 {
                  direction = 1;
                 }
                 else
                 {
                  direction = 2;
                 }
             }
             else if(delta > 0)
             {
                 if ((2*(delta-x)-1) <= 0)
                 {
                  direction = 2;
                 }
                 else
                 {
                  direction = 3;
                 }
             }
             else
             {
                   direction=2;
             }

             ////

             switch(direction)
             {

              case 1:
                     x++;
                      delta += (2*x+1);
                      break;

              case 2:
                      x++;
                      y--;
                      delta += 2*(x-y+1);
                       break;

              case 3:
                      y--;
                      delta += (-2*y+1);
                       break;

             }

         }
    }
}

//----填充圆------------------------------------------------------------------
//描述: 使用FillColor填充一个实心圆.
//参数：hdc: 绘图上下文句柄.
//      cx,cy: 圆的中心坐标
//      r: 圆的半径
//返回：无.
//------------------------------------------------------------------------------
void    FillCircle(HDC hdc,s32 cx,s32 cy,s32 r)
{
      s32 i;
      s32 imax = ((s32)((s32)r*707))/1000+1;
      s32 sqmax = (s32)r*(s32)r+(s32)r/2;
      s32 x=r;
      u32 color_bk;

      color_bk =SetDrawColor(hdc,GetFillColor(hdc));

      DrawLine(hdc,cx-r,cy,cx+r+1,cy);
      for (i=1; i<= imax; i++)
      {
        if ((i*i+x*x) >sqmax)
        {

          if (x>imax)
          {
            DrawLine(hdc,cx-i+1,cy+x, cx+i,cy+x);
               DrawLine(hdc,cx-i+1,cy-x, cx+i,cy-x);
          }
          x--;
        }

        DrawLine(hdc,cx-x,cy+i, cx+x+1,cy+i);
        DrawLine(hdc,cx-x,cy-i, cx+x+1,cy-i);
      }

      SetDrawColor(hdc,color_bk);
}

//----画椭圆------------------------------------------------------------------
//描述: 使用DrawColor画一个空心椭圆.
//参数：hdc: 绘图上下文句柄.
//      cx,cy: 椭圆的中心坐标
//      rx: 椭圆的水平半径
//      ry: 椭圆的垂直半径
//返回：无.
//------------------------------------------------------------------------------
void     DrawEllipse(HDC hdc,s32 cx, s32 cy, s32 rx, s32 ry)
{
    s32 OutConst, Sum, SumY;
    s32 x,y;
    s32 xOld;
    u32 _rx = rx;
    u32 _ry = ry;

    OutConst =   _rx*_rx*_ry*_ry
               +(_rx*_rx*_ry>>1);
    xOld = x = rx;

     for(y=0; y<=ry; y++)
     {
        if(y==ry)
        {
          x=0;
        }
        else
        {
          SumY =((s32)(rx*rx))*((s32)(y*y));
          while (Sum = SumY + ((s32)(ry*ry))*((s32)(x*x)),
                 (x>0) && (Sum>OutConst)) x--;
        }

        if(y)
        {
          DrawLine(hdc,cx-xOld,cy-y+1,cx-x,cy-y);
          DrawLine(hdc,cx-xOld,cy+y-1,cx-x,cy+y);
          DrawLine(hdc,cx+xOld,cy-y+1,cx+x,cy-y);
          DrawLine(hdc,cx+xOld,cy+y-1,cx+x,cy+y);
        }
        xOld = x;
     }

}

//----填充椭圆------------------------------------------------------------------
//描述: 使用FillColor填充一个实心椭圆.
//参数：hdc: 绘图上下文句柄.
//      cx,cy: 椭圆的中心坐标
//      rx: 椭圆的水平半径
//      ry: 椭圆的垂直半径
//返回：无.
//------------------------------------------------------------------------------
void FillEllipse(HDC hdc,s32 cx, s32 cy, s32 rx, s32 ry)
{
    s32 OutConst, Sum, SumY;
    s32 x,y;
    u32 _rx = rx;
    u32 _ry = ry;
    u32 color_bk;

    color_bk =SetDrawColor(hdc,GetFillColor(hdc));

    OutConst = _rx*_rx*_ry*_ry
                +(_rx*_rx*_ry>>1);
     x = rx;
     for (y=0; y<=ry; y++)
     {
          SumY =((s32)(rx*rx))*((s32)(y*y));

          while (Sum = SumY + ((s32)(ry*ry))*((s32)(x*x)),
                 (x>0) && (Sum>OutConst))
          {
             x--;
          }
          DrawLine(hdc,cx-x, cy+y, cx+x,cy+y);

          if(y)
          {
             DrawLine(hdc,cx-x, cy-y, cx+x,cy-y);
          }
    }

    SetDrawColor(hdc,color_bk);

}

//----绘制扇形------------------------------------------------------------------
//描述: 使用DrawColor绘制扇形.
//参数：hdc: 绘图上下文句柄.
//      xCenter,yCenter: 扇形的中心坐标
//      radius: 扇形半径
//      angle1: 扇形起始点角度
//      angle2: 扇形结束点角度
//返回：无.
//------------------------------------------------------------------------------
void    DrawSector(HDC hdc, s32 xCenter, s32 yCenter, s32 radius,s32 angle1,s32 angle2)
{
  s32 x, y, d,c1,c2,c,step=0;
  s32 quarter1,quarter2,quarter3,quarter4;
  u32 color;

  if(radius<=0)
  {
      return;
  }

  x = xCenter;
  y = yCenter + radius;
  d = 3 - 2 * radius;
  c1=(s32)(angle1*radius*3.14159/180);
  c2=(s32)(angle2*radius*3.14159/180);
  quarter1=(s32)(radius*3.14159/2);
  quarter2=(s32)(radius*3.14159);
  quarter3=(s32)(radius*3.14159*3/2);
  quarter4=(s32)(radius*3.14159*2);

  color =GetDrawColor(hdc);
  while(1)
  {
      c=quarter4-step;
    if(c>=c1 && c<=c2)    SetPixel(hdc,xCenter + (y - yCenter), yCenter + (x - xCenter ),color );

    if(step>=c1 && step<=c2) SetPixel(hdc,xCenter + (y - yCenter), yCenter - (x - xCenter),color );

    c=quarter2+step;
    if(c>=c1 && c<=c2) SetPixel(hdc, xCenter - (y - yCenter), yCenter + (x - xCenter ),color );

    c=quarter2-step;
    if(c>=c1 && c<=c2) SetPixel(hdc, xCenter - (y - yCenter), yCenter - (x - xCenter),color );

    if (  x - xCenter  >=  y - yCenter  ) break;

    c=quarter3+step;
    if(c>=c1 && c<=c2) SetPixel(hdc, x, y,color );

    c=quarter1-step;
    if(c>=c1 && c<=c2) SetPixel(hdc,x, yCenter - (y - yCenter),color );

    c=quarter3-step;
    if(c>=c1 && c<=c2) SetPixel(hdc, xCenter - (x - xCenter), y,color );

    c= quarter1+step;
    if(c>=c1 && c<=c2) SetPixel(hdc, xCenter - (x - xCenter), yCenter - (y - yCenter),color );

    if ( d < 0 )
    { d = d + ((x - xCenter) << 2) + 6;
    }
    else
    { d = d + (((x - xCenter) - (y - yCenter)) << 2 ) + 10;
      y--;
    }
    x++;
    step++;
  }

}

//----填充扇形------------------------------------------------------------------
//描述: 使用FillColor填充扇形.
//参数：hdc: 绘图上下文句柄.
//      xCenter,yCenter: 扇形的中心坐标
//      radius: 扇形半径
//      angle1: 扇形起始点角度
//      angle2: 扇形结束点角度
//返回：无.
//------------------------------------------------------------------------------
void    FillSector(HDC hdc, s32 xCenter, s32 yCenter, s32 radius,s32 angle1,s32 angle2)
{
  s32 x, y, d,c1,c2,c,step=0;
  s32 quarter1,quarter2,quarter3,quarter4;
  u32 color;

  if(radius<=0)
  {
      return;
  }

  x = xCenter;
  y = yCenter + radius;
  d = 3 - 2 * radius;
  c1=(s32)(angle1*radius*3.14159/180);
  c2=(s32)(angle2*radius*3.14159/180);
  quarter1=(s32)(radius*3.14159/2);
  quarter2=(s32)(radius*3.14159);
  quarter3=(s32)(radius*3.14159*3/2);
  quarter4=(s32)(radius*3.14159*2);
  color =GetFillColor(hdc);

  while(1)
  {
      c=quarter4-step;
    if(c>=c1 && c<=c2)    DrawLineEx(hdc,xCenter + (y - yCenter), yCenter + (x - xCenter ) ,xCenter,yCenter ,color);

    if(step>=c1 && step<=c2) DrawLineEx(hdc,xCenter + (y - yCenter), yCenter - (x - xCenter),xCenter,yCenter ,color);

    c=quarter2+step;
    if(c>=c1 && c<=c2) DrawLineEx(hdc, xCenter - (y - yCenter), yCenter + (x - xCenter ),xCenter,yCenter ,color);

    c=quarter2-step;
    if(c>=c1 && c<=c2) DrawLineEx(hdc, xCenter - (y - yCenter), yCenter - (x - xCenter),xCenter,yCenter,color);

    if (  x - xCenter  >=  y - yCenter  ) break;

    c=quarter3+step;
    if(c>=c1 && c<=c2) DrawLineEx(hdc, x, y,xCenter,yCenter ,color);

    c=quarter1-step;
    if(c>=c1 && c<=c2) DrawLineEx(hdc,x, yCenter - (y - yCenter),xCenter,yCenter ,color);

    c=quarter3-step;
    if(c>=c1 && c<=c2) DrawLineEx(hdc, xCenter - (x - xCenter), y ,xCenter,yCenter,color);

    c= quarter1+step;
    if(c>=c1 && c<=c2) DrawLineEx(hdc, xCenter - (x - xCenter), yCenter - (y - yCenter) ,xCenter,yCenter,color);

    if ( d < 0 )
    { d = d + ((x - xCenter) << 2) + 6;
    }
    else
    { d = d + (((x - xCenter) - (y - yCenter)) << 2 ) + 10;
      y--;
    }
    x++;
    step++;
  }
}

//----绘制3阶Bezier线------------------------------------------------------------------
//描述: 按指定坐标点绘制连续的贝塞尔线
//参数：hdc: 绘图上下文句柄.
//      pt: 线段坐标点
//      cnt: 坐标点数量
//返回：无.
//------------------------------------------------------------------------------
void DrawBezier3(HDC hdc,const POINT *pt,s32 cnt)
{
    float t,t1,t2,xt,yt,step;
    s32 x0,y0;
    ////

    if(cnt <= 0)
    {
        return;
    }

    if(pt == NULL)
    {
        return;
    }

    x0     =pt[0].x;
    y0     =pt[0].y;
    step   =1.0/cnt;

    for (t=0;t<=1;t+=step)
    {
        yt =1-t;
        t1 =yt*yt;
        t2 =3*yt*t;

        xt =pt[0].x*t1*yt+pt[1].x*t2*yt+pt[2].x*t2*t+pt[3].x*t*t*t;
        yt =pt[0].y*yt*t1+pt[1].y*t2*yt+pt[2].y*t2*t+pt[3].y*t*t*t;

        DrawLine(hdc,x0,y0,xt,yt);

        x0 =xt;
        y0 =yt;

    }

}


//----绘制曲线------------------------------------------------------------------
//描述: 使用DrawColor绘制曲线.
//参数：hdc: 绘图上下文句柄.
//      pt: 曲线坐标点.
//      count: 坐标点数量.
//返回：无.
//------------------------------------------------------------------------------
void    DrawPolyLine(HDC hdc,const POINT *pt,s32 count)
{
    s32 i;

    if(count>1)
    {
        for(i=1;i<count;i++)
        {
            DrawLine(hdc,pt[i-1].x,pt[i-1].y,pt[i].x,pt[i].y);
        }
    }
}

//----绘制组合框------------------------------------------------------------------
//描述: 使用DrawColor绘制组合框边框,使用TextColor绘制组合框文字.
//参数：hdc: 绘图上下文句柄.
//      prc: 组合框矩形参数.
//      Text:组合框文字.
//返回：无.
//------------------------------------------------------------------------------

void    DrawGroupBox(HDC hdc,const RECT *prc,const char *Text)
{
    s32 i,text_w,text_h,text_offset;
    u32 old_color;

    POINT pt[6];
    RECT rc;

    rc.left =0;
    rc.top =0;
    rc.right =0;
    rc.bottom =0;
    AdjustTextRect(hdc,Text,-1,&rc,DT_CENTER|DT_VCENTER);

    text_w =RectW(&rc);
    text_h =RectH(&rc);
    text_offset =8;

    pt[0].x =prc->left+text_offset;
    pt[0].y =prc->top+(RectH(&rc)>>1);

    pt[1].x =pt[0].x-text_offset;
    pt[1].y =pt[0].y;

    pt[2].x =pt[1].x;
    pt[2].y =prc->top+RectH(prc)-1;

    pt[3].x =pt[2].x+RectW(prc)-1;
    pt[3].y =pt[2].y;

    pt[4].x =pt[3].x;
    pt[4].y =pt[0].y;

    pt[5].x =pt[0].x+text_w;
    pt[5].y =pt[0].y;

    ////Bottom
    for(i=0;i<6;i++)
    {
        pt[i].x+=1;
        pt[i].y+=1;
    }

    old_color=GetDrawColor(hdc);
    SetDrawColor(hdc,AlphaBlendColor(RGB(255,255,255),old_color,128));
    DrawPolyLine(hdc,pt,6);

    ////Top
    for(i=0;i<6;i++)
    {
        pt[i].x-=1;
        pt[i].y-=1;
    }
    SetDrawColor(hdc,old_color);
    DrawPolyLine(hdc,pt,6);

    SetRect(&rc,pt[0].x+1,prc->top,text_w,text_h);
    DrawText(hdc,Text,-1,&rc,DT_CENTER|DT_VCENTER);


}
//----绘制位图------------------------------------------------------------------
//描述:
//参数：hdc: 绘图上下文句柄.
//返回：无.
//------------------------------------------------------------------------------
bool_t    DrawBitmap(HDC hdc,s32 x,s32 y,struct RectBitmap *bitmap,u32 HyalineColor,struct RopGroup RopCode)
{
    POINT pt;
    if(BeginDraw(hdc))
    {
        pt.x =x;
        pt.y =y;
        __LPtoDP(hdc,&pt,1);

        GK_ApiDrawBitMap(hdc->pGkWin,bitmap,pt.x,pt.y,HyalineColor,RopCode,hdc->SyncTime);
        EndDraw(hdc);
        return true;
    }
    return false;
}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/


