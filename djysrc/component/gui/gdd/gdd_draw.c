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
#include    "gdd.h"
#include    "./include/gdd_private.h"
/*============================================================================*/



static  BOOL    BeginDraw(HDC hdc)
{
    return TRUE;
}

static  void    EndDraw(HDC hdc)
{

}


static  void    _LPtoDP(HDC hdc,POINT *pt,int count)
{

    switch(hdc->DCType)
    {
        case    DC_TYPE_PAINT:
        case    DC_TYPE_CLIENT:
                while(count--)
                {
                    pt->x +=(hdc->hwnd->CliRect.left - hdc->hwnd->WinRect.left);
                    pt->y +=(hdc->hwnd->CliRect.top - hdc->hwnd->WinRect.top);
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

void    UpdateDisplay(void)
{
    GK_ApiSyncShow(1000*mS);
}

/*============================================================================*/
/*============================================================================*/

void    _InitDC(DC *pdc,struct tagGkWinRsc *gk_win,HWND hwnd,int dc_type)
{

    pdc->pGkWin     =gk_win;
    pdc->hwnd       =hwnd;
    pdc->DCType     =dc_type;
    pdc->pFontDef   =Font_GetCurFont();
    pdc->pFont      =pdc->pFontDef;
    pdc->pCharset	=Charset_NlsGetCurCharset();

    pdc->CurX       =0;
    pdc->CurY       =0;
    pdc->DrawColor  =RGB(255,255,255);
    pdc->FillColor  =RGB(1,1,1);
    pdc->TextColor  =RGB(255,255,255);
    pdc->SyncTime   =100*mS;
    pdc->RopCode    =CN_R2_COPYPEN;

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
//     rop_code: 新的光栅码.
//返回：旧的光栅码.
//------------------------------------------------------------------------------
u32 SetRopCode(HDC hdc,u32 rop_code)
{
    u32 old=0;
    if(hdc!=NULL)
    {
        old =hdc->RopCode;
        hdc->RopCode =rop_code;
    }
    return old;
}

//----获得当前光栅码-------------------------------------------------------------
//描述: 略.
//参数：hdc: 绘图上下文句柄.
//返回：当前光栅码.
//------------------------------------------------------------------------------
u32 GetRopCode(HDC hdc)
{
    u32 old=0;
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
static  void    _MoveTo(HDC hdc,int x,int y,POINT *old_pt)
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
void    MoveTo(HDC hdc,int x,int y,POINT *old_pt)
{
    if(hdc!=NULL)
    {
        _MoveTo(hdc,x,y,old_pt);
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
void SetPixel(HDC hdc,int x,int y,u32 color)
{
    POINT pt;
    if(BeginDraw(hdc))
    {
        pt.x =x;
        pt.y =y;
        _LPtoDP(hdc,&pt,1);
        GK_ApiSetPixel(hdc->pGkWin,pt.x,pt.y,color,hdc->RopCode,hdc->SyncTime);
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
void    DrawLine(HDC hdc,int x0,int y0,int x1,int y1)
{
    POINT pt[2];

    if(BeginDraw(hdc))
    {
        pt[0].x =x0;
        pt[0].y =y0;
        pt[1].x =x1;
        pt[1].y =y1;
        _LPtoDP(hdc,pt,2);

        GK_ApiLinetoIe(hdc->pGkWin,pt[0].x,pt[0].y,pt[1].x,pt[1].y,
                hdc->DrawColor,hdc->RopCode,hdc->SyncTime);
    }
}

//----使用当前位置画线-----------------------------------------------------------
//描述: 绘制单个像素宽的任意直线,使用DrawColor作为颜色值,使用当前位置作为起始坐标,
//     绘制完成后,并将结束坐标更新为当前坐标位置.
//参数：hdc: 绘图上下文句柄.
//      x,y: 结束坐标.
//返回：无.
//------------------------------------------------------------------------------
void    DrawLineTo(HDC hdc,int x,int y)
{
    POINT pt[2];

    if(BeginDraw(hdc))
    {
        pt[0].x =hdc->CurX;
        pt[0].y =hdc->CurY;
        pt[1].x =x;
        pt[1].y =y;

        _LPtoDP(hdc,pt,2);

        GK_ApiLinetoIe(hdc->pGkWin,pt[0].x,pt[0].y,pt[1].x,pt[1].y,
                hdc->DrawColor,hdc->RopCode,hdc->SyncTime);

        _MoveTo(hdc,x,y,NULL);
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
BOOL    TextOut(HDC hdc,int x,int y,const char *text,int count)
{
	    POINT pt;

	    if(BeginDraw(hdc))
	    {
	        pt.x =x;
	        pt.y =y;
	        _LPtoDP(hdc,&pt,1);

	        x =pt.x;
	        y =pt.y;

	        if(count<0)
	        {
	          	count = strlen(text);
	        }

	        if(text!=NULL)
	        {
	           GK_ApiDrawText(hdc->pGkWin,hdc->pFont,hdc->pCharset,
	                                  x,y,text,count,hdc->TextColor,
	                                  hdc->RopCode,hdc->SyncTime);

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
static  int _GetTextWidth(HDC hdc,const char *text,int count)
{
    int width;
    struct tagCharset *Set;
    struct tagFontRsc *Font;
    s32 len, loop;
    u32 wc;

    width=0;
    Set = hdc->pCharset;
    if(Set!=NULL)
    {
		Font = hdc->pFont;
		if(Font!=NULL)
		{
			for(loop = 0; loop < count;)
			{
				len= Set->mb_to_ucs4(&wc, text, count);
				if(len == -1)
				{ // 无效字符
					text++;
					loop++;
				}
				else if(len == 0)
				{
					break;
				}
				else
				{ // 有效字符
					text += len;
					loop += len;
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
static  void _AdjustTextRect(HDC hdc,const char *text,int count, RECT *prc,u32 flag)
{
    s32 Height;
    s32 i;

    if(count<0)
    {
    	count =strlen(text);
    }

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
                    i =_GetTextWidth(hdc,text,count);
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
                    i =_GetTextWidth(hdc,text,count);
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
static	s32  __GetStrLineCount(const char *str)
{
	s32 count;
	char ch;

	count=0;
	while(1)
	{
		ch=*str;
		if(ch=='\n')
		{
			count++;
		}

		if(ch=='\0')
		{
			if(*(str-1)!='\n')
			{
				count++;
			}
			break;
		}

		str++;

	}
	return count;
}

//----在矩形范围内绘制字符串-----------------------------------------------------
//描述: 在指定矩形范围内绘制字符串,使用TextColor作为颜色值,支持回车与换行符,
//      该函数可以指定是否绘制字符串边框和背景,以前指定对齐方式的组合.
//参数：hdc: 绘图上下文句柄.
//      x,y: 坐标位置.
//      count: 要绘制的字符数量,该参数小于0时,将绘制整个字符串.
//      prc: 要绘制的所在矩形位置和大小.
//      flag: 绘制标志参数.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    DrawText(HDC hdc,const char *text,int count,const RECT *prc,u32 flag)
{

    RECT rc,rc0;
    int line_count;

    if(NULL==prc)
    {
        return FALSE;
    }

    if(NULL==text)
    {
        return FALSE;
    }

    if(count<0)
    {
    	count = strlen(text);
    }

    line_count =__GetStrLineCount(text);

    if(BeginDraw(hdc))
    {
    	if(line_count<=1)
    	{
    		//单行
			_CopyRect(&rc0,prc);
			_CopyRect(&rc,prc);
			_InflateRect(&rc,-1,-1);
			_AdjustTextRect(hdc,text,count,&rc,flag);

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

			_CopyRect(&rc0,prc);
			_CopyRect(&rc,prc);
			_InflateRect(&rc,-1,-1);

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
                case    DT_VCENTER:
                		y0 += (RectH(&rc)-(line_count*line_h))>>1;
                        break;
                        ////
                case    DT_TOP:
                        break;
                        ////
                case    DT_BOTTOM:
                        y0 += (RectH(&rc) - (line_count*line_h));
                        break;
                        ////
                default:
                        break;

            }

            p0 =(char*)text;
            while(p0!=NULL)
            {
            	p1 =strchr(p0,'\n');
            	if(p1!=NULL)
            	{
            		i =(s32)((u32)p1-(u32)p0);
            	}
            	else
            	{
            		//如果是最后一行.
            		i =strlen(p0);
            	}

            	if(i<=0)
            	{
            		break;
            	}

            	//i += 1;
            	x0 =rc.left;
                switch(flag&DT_ALIGN_H_MASK)
                {
                    case    DT_CENTER:
                            x0 =(RectW(&rc)-_GetTextWidth(hdc,p0,i))>>1;
                            break;
                            ////
                    case    DT_LEFT:
                            x0  += 1;
                            break;
                            ////
                    case    DT_RIGHT:
                            x0 =(RectW(&rc)-_GetTextWidth(hdc,p0,i));
                            break;
                            ////
                    default:
                            break;

                }

                TextOut(hdc,x0,y0,p0,i);

                if(p1==NULL)
                {	//最后一行显示完后,则退出
                	break;
                }

                y0 += line_h;
                p0 += i+1;

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
    int x0,y0;
    int x1,y1;
    POINT pt;

    if(prc != NULL)
    {
        if(BeginDraw(hdc))
        {
            pt.x =prc->left;
            pt.y =prc->top;
            _LPtoDP(hdc,&pt,1);

            x0 =pt.x;
            y0 =pt.y;
            x1 =x0+RectW(prc)-1;
            y1 =y0+RectH(prc)-1;

            //瀹革拷
            GK_ApiLinetoIe(hdc->pGkWin,x0,y0,x0,y1,hdc->DrawColor,hdc->RopCode,hdc->SyncTime);
            //娑擄拷
            GK_ApiLinetoIe(hdc->pGkWin,x0,y0,x1,y0,hdc->DrawColor,hdc->RopCode,hdc->SyncTime);
            //閸欙拷
            GK_ApiLinetoIe(hdc->pGkWin,x1,y0,x1,y1,hdc->DrawColor,hdc->RopCode,hdc->SyncTime);
            //娑擄拷
            GK_ApiLinetoIe(hdc->pGkWin,x0,y1,x1,y1,hdc->DrawColor,hdc->RopCode,hdc->SyncTime);

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
    struct tagRectangle gk_rc;
    POINT pt;

    if(hdc!=NULL)
    {
        if(prc!=NULL)
        {
            if(BeginDraw(hdc))
            {
                pt.x =prc->left;
                pt.y =prc->top;
                _LPtoDP(hdc,&pt,1);

                gk_rc.left = pt.x;
                gk_rc.top = pt.y;
                gk_rc.right = pt.x+RectW(prc);
                gk_rc.bottom = pt.y+RectH(prc);

                GK_ApiFillRect(hdc->pGkWin,&gk_rc,hdc->FillColor,hdc->FillColor,
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
//            GFILL_L_R:丛左到右填充
//            GFILL_U_D:丛上到下
//            GFILL_LU_RD:丛左上到右下
//            GFILL_RU_LD:丛右上到左下
//返回：无.
//------------------------------------------------------------------------------
void    GradientFillRect(HDC hdc,const RECT *prc,u32 Color1,u32 Color2,u32 mode)
{
    struct tagRectangle gk_rc;
    RECT rc;

    if(hdc!=NULL)
    {
        if(prc!=NULL)
        {
            if(BeginDraw(hdc))
            {
                _CopyRect(&rc,prc);

                _LPtoDP(hdc,(POINT*)&rc,2);

                gk_rc.left = rc.left;
                gk_rc.top = rc.top;
                gk_rc.right = rc.right;
                gk_rc.bottom = rc.bottom;

                switch(mode)
                {
                    case GFILL_L_R:
                            mode =CN_FILLRECT_MODE_H;
                            break;
                    case GFILL_U_D:
                            mode =CN_FILLRECT_MODE_V;
                            break;
                    case GFILL_LU_RD:
                            mode =CN_FILLRECT_MODE_SP;
                            break;
                    case GFILL_RU_LD:
                            mode =CN_FILLRECT_MODE_SN;
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

/*============================================================================*/

void    DrawCircle(HDC hdc,int cx,int cy,int r)
{

}

void    FillCircle(HDC hdc,int cx,int cy,int r)
{

}


//----绘制曲线------------------------------------------------------------------
//描述: 使用DrawColor绘制曲线.
//参数：hdc: 绘图上下文句柄.
//      pt: 曲线坐标点.
//      count: 坐标点数量.
//返回：无.
//------------------------------------------------------------------------------
void    DrawPolyLine(HDC hdc,const POINT *pt,int count)
{
    int i;

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
    int i,text_w,text_h,text_offset;
    u32 old_color;

    POINT pt[6];
    RECT rc;

    rc.left =0;
    rc.top =0;
    rc.right =0;
    rc.bottom =0;
    _AdjustTextRect(hdc,Text,-1,&rc,DT_CENTER|DT_VCENTER);

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

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/


