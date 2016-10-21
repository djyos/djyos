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
//文件描述:进度条控件实现
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2015-03-23
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include    "gdd.h"
#include    <gui/gdd/gdd_private.h>
#include    <widget.h>


bool_t    MakeProgressRect(RECT *dst,const RECT *src,u32 Range,u32 Val,EN_PB_MODE mode)
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


//----进度条(Progressbar)绘制函数----------------------------------------------------------
//功能：这是进度条(Progressbar)控件的MSG_PAINT消息响应函数
//参数：pMsg，消息指针
//返回：固定true//HWND hwnd,PROGRESSBAR_DATA *pPB)//todo 参数传递不一致
//-----------------------------------------------------------------------------
static  bool_t ProgressBar_Paint(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT m_rc[2];
    RECT rc0;
    s32 gfill_mode=0;
    PROGRESSBAR_DATA *pPB;
    hwnd =pMsg->hwnd;
    pPB=(PROGRESSBAR_DATA*)hwnd->PrivateData;
    hdc =BeginPaint(hwnd);
    if(NULL!=hdc)
    {
        GetClientRect(hwnd,&rc0);
        if(hwnd->Style&PBS_VER)
        {
            gfill_mode =CN_FILLRECT_MODE_LR;
            if(pPB->Flag&PBF_ORG_BOTTOM)
            {
                MakeProgressRect(m_rc,&rc0,pPB->Range,pPB->Pos,PBM_BOTTOM);
            }
            else
            {
                MakeProgressRect(m_rc,&rc0,pPB->Range,pPB->Pos,PBM_TOP);
            }
        }
        else
        {
            gfill_mode =CN_FILLRECT_MODE_UD;
            if(pPB->Flag&PBF_ORG_RIGHT)
            {
                MakeProgressRect(m_rc,&rc0,pPB->Range,pPB->Pos,PBM_RIGHT);
            }
            else
            {
                MakeProgressRect(m_rc,&rc0,pPB->Range,pPB->Pos,PBM_LEFT);
            }
        }

        if(hwnd->Style&PBS_FLAT)
        {
            SetFillColor(hdc,pPB->FGColor);
            FillRect(hdc,&m_rc[0]);
            SetFillColor(hdc,pPB->BGColor);
            FillRect(hdc,&m_rc[1]);
        }
        else
        {

            GradientFillRect(hdc,&m_rc[0],
                            AlphaBlendColor(pPB->FGColor,RGB(250,250,250),100),
                            AlphaBlendColor(pPB->FGColor,RGB(10,10,10),160),
                            gfill_mode);

            GradientFillRect(hdc,&m_rc[1],
                            AlphaBlendColor(pPB->BGColor,RGB(250,250,250),100),
                            AlphaBlendColor(pPB->BGColor,RGB(10,10,10),160),
                            gfill_mode);

        }

        if(pPB->Flag&PBF_SHOWTEXT)
        {
            SetTextColor(hdc,pPB->TextColor);
            DrawText(hdc,hwnd->Text,-1,&rc0,pPB->DrawTextFlag);
        }
        EndPaint(hwnd,hdc);
        return true;
    }
    return false;
}

//----MSG_CREATE消息响应函数----------------------------------------------------
//功能：创建进度条数据结构体
//参数：pMsg，消息指针
//返回：成功则返回true，失败则返回false.
//--------------------------------------------------------------------------
static bool_t ProgressBar_Create(struct WindowMsg *pMsg)
{
     HWND hwnd;
     hwnd =pMsg->hwnd;
     PROGRESSBAR_DATA *pPB;

     if(pMsg->Param1==0)
     {
         pPB =(PROGRESSBAR_DATA*)malloc(sizeof(PROGRESSBAR_DATA));
         pMsg->Param1=(ptu32_t*)pPB;
         pPB->Flag =0;
         pPB->Range =100;
         pPB->Pos   =0;
         pPB->FGColor =RGB(200,0,0);
         pPB->BGColor =RGB(0,0,200);
         pPB->DrawTextFlag =DT_VCENTER|DT_CENTER;
     }
     SetWindowPrivateData(hwnd,pMsg->Param1);
     return true;

}

//----设置进度条数据结构消息响应函数-------------------------------------------
//功能：设置进度条数据结构消息响应函数
//参数：pMsg，消息指针
//返回：成功则返回true，失败则返回false.
//-----------------------------------------------------------------------------
static bool_t ProgressBar_SetData(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;
    PROGRESSBAR_DATA *pPB;
    if(pMsg->Param1!=0)
    {
        pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
        if(pPB!=NULL)
        {
            memcpy(pPB,(char*)pMsg->Param1,sizeof(PROGRESSBAR_DATA));
            InvalidateWindow(pMsg->hwnd,FALSE);
            return true;
        }
    }
    return false;
}

//----获取进度条数据结构消息响应函数-------------------------------------------
//功能：获取进度条数据结构消息响应函数
//参数：pMsg，消息指针
//返回：成功则返回true，失败则返回false.
//-----------------------------------------------------------------------------
static bool_t ProgressBar_GetData(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;
    PROGRESSBAR_DATA *pPB;
    if(pMsg->Param1!=0)
    {
        pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
        if(pPB!=NULL)
        {
            memcpy((char*)pMsg->Param1,pPB,sizeof(PROGRESSBAR_DATA));
            InvalidateWindow(pMsg->hwnd,FALSE);
            return true;
        }
    }
    return false;
}

//----设置进度条范围消息响应函数-----------------------------------------------
//功能：设置进度条范围消息响应函数
//参数：pMsg，消息指针
//返回：成功则返回true，失败则返回false.
//-----------------------------------------------------------------------------
static bool_t ProgressBar_SetRange(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;
    PROGRESSBAR_DATA *pPB;
    pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
    if(pPB!=NULL)
    {
        pPB->Range =(u32)pMsg->Param1;
        InvalidateWindow(pMsg->hwnd,FALSE);
        return true;
    }
    return false;
}

//----获取进度条范围消息响应函数-----------------------------------------------
//功能：获取进度条范围消息响应函数
//参数：pMsg，消息指针
//返回：成功则返回范围值，失败则返回0.
//-----------------------------------------------------------------------------
static u32 ProgressBar_GetRange(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;
    PROGRESSBAR_DATA *pPB;
    pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
    if(pPB!=NULL)
    {
        return pPB->Range;
    }
    return 0;
}

//----设置进度条进度当前值消息响应函数-----------------------------------------------
//功能：设置进度条范围消息响应函数
//参数：pMsg，消息指针
//返回：成功则返回true，失败则返回false.
//-----------------------------------------------------------------------------
static bool_t ProgressBar_SetPos(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;
    PROGRESSBAR_DATA *pPB;
    pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
    if(pPB!=NULL)
    {
        pPB->Pos =(u32)pMsg->Param1;
        InvalidateWindow(pMsg->hwnd,FALSE);
        return true;
    }
    return false;
}

//----获取进度条进度当前值消息响应函数-----------------------------------------------
//功能：获取进度条范围消息响应函数
//参数：pMsg，消息指针
//返回：成功则返回范围值，失败则返回0.
//-----------------------------------------------------------------------------
static u32 ProgressBar_GetPos(struct WindowMsg *pMsg)
{
    HWND hwnd;
    hwnd =pMsg->hwnd;
    PROGRESSBAR_DATA *pPB;
    pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
    if(pPB!=NULL)
    {
        return pPB->Pos;
    }
    return 0;
}


//默认进度条消息处理函数表，处理用户函数表中没有处理的消息。
static struct MsgProcTable s_gProgressBarMsgProcTable[] =
{
    {MSG_ProcessBar_SETDATA,ProgressBar_SetData},
    {MSG_ProcessBar_GETDATA,ProgressBar_GetData},
    {MSG_ProcessBar_SETRANGE,ProgressBar_SetRange},
    {MSG_ProcessBar_GETRANGE,ProgressBar_GetRange},
    {MSG_ProcessBar_SETPOS,ProgressBar_SetPos},
    {MSG_ProcessBar_GETPOS,ProgressBar_GetPos},
    {MSG_CREATE,ProgressBar_Create},
    {MSG_PAINT,ProgressBar_Paint}
};


static struct MsgTableLink  s_gProgressBarMsgLink;

HWND CreateProgressBar(  const char *Text,u32 Style,
                    s32 x,s32 y,s32 w,s32 h,
                    HWND hParent,u32 WinId,void *pdata,
                    struct MsgTableLink *UserMsgTableLink)
{
    WINDOW *pGddWin=NULL;
    struct MsgTableLink *Current;
    if(UserMsgTableLink != NULL)
    {
        Current = UserMsgTableLink;
        while(Current->LinkNext != NULL)
            Current = Current->LinkNext;
        Current->LinkNext = &s_gProgressBarMsgLink;
        Current = UserMsgTableLink;
    }
    else
        Current = &s_gProgressBarMsgLink;
    s_gProgressBarMsgLink.LinkNext = NULL;
    s_gProgressBarMsgLink.MsgNum = sizeof(s_gProgressBarMsgProcTable) / sizeof(struct MsgProcTable);
    s_gProgressBarMsgLink.myTable = (struct MsgProcTable *)&s_gProgressBarMsgProcTable;
    pGddWin=CreateWindow(Text,WS_CHILD|Style,x,y,w,h,hParent,WinId, CN_WINBUF_PARENT,pdata,Current);
    return pGddWin;
}


/*============================================================================*/
