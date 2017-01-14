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
//文件描述: 窗口及控件机制及管理
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-11-10
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include    <gui/gdd/gdd_private.h>


/*
typedef struct  __WNDCLASS  //窗口类数据结构
{
    WNDPROC *WinProc;
    u32 DrawColor;
    u32 FillColor;
    u32 TextColor;
}WNDCLASS;
*/

extern HWND g_CursorHwnd;         //光标窗口
extern void GK_ApiSetVisible(struct GkWinRsc *gkwin, u32 visible,u32 SyncTime);

/*============================================================================*/

static  HWND HWND_Desktop=NULL;
static  HWND HWND_Focus=NULL;

//----锁定窗口------------------------------------------------------------------
//描述: 锁定窗口,用于对窗口互斥操作,该函数返回TRUE时,必须调用HWND_Unlock解锁;
//      而当该函数返回FALSE,则无需调用HWND_Unlock.
//参数：hwnd:窗口句柄
//返回：成功:TRUE; 失败:FLASE;
//------------------------------------------------------------------------------
bool_t    HWND_Lock(HWND hwnd)
{
    return GDD_Lock();
}

//----取窗口句柄---------------------------------------------------------------
//功能：根据gkwin指针，去所属的gdd窗口句柄
//参数：gkwin，gkwin指针
//返回：gdd窗口句柄
//---------------------------------------------------------------------------
HWND GetGddHwnd(struct GkWinRsc *gkwin)
{
    return (HWND)GK_ApiGetUserTag(gkwin);
}
//----解锁窗口------------------------------------------------------------------
//描述: 当窗口锁定成功后,由该函数进行解锁操作.
//参数：hwnd:窗口句柄
//返回：无
//------------------------------------------------------------------------------
void    HWND_Unlock(HWND hwnd)
{
    GDD_Unlock();
}

//----屏幕坐标转客户坐标---------------------------------------------------------
//描述: 把一组屏幕坐标值，转换为窗口客户区的相对坐标
//参数：hwnd:窗口句柄
//      pt:  需要转换的屏幕坐标点指针
//      count: 需要转换的坐标点数量
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    ScreenToClient(HWND hwnd,POINT *pt,s32 count)
{
    u32 i;
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            for(i=0;i<count;i++)
            {
                pt[i].x -= hwnd->CliRect.left;
                pt[i].y -= hwnd->CliRect.top;
            }
            HWND_Unlock(hwnd);
            return TRUE;
        }
    }
    return  FALSE;
}

//----客户坐标转屏幕坐标---------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    ClientToScreen(HWND hwnd,POINT *pt,s32 count)
{
    u32 i;
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            for(i=0;i<count;i++)
            {
                pt[i].x += hwnd->CliRect.left;
                pt[i].y += hwnd->CliRect.top;
            }
            HWND_Unlock(hwnd);
            return  TRUE;
        }

    }
    return  FALSE;
}

//----屏幕坐标转窗口坐标---------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    ScreenToWindow(HWND hwnd,POINT *pt,s32 count)
{
    u32 i;
    RECT rc;
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            GK_ApiGetArea(hwnd->pGkWin, &rc);
            for(i=0;i<count;i++)
            {
                pt[i].x -= rc.left;
                pt[i].y -= rc.top;
            }
            HWND_Unlock(hwnd);
            return  TRUE;
        }
    }
    return  FALSE;
}

//----窗口坐标转屏幕坐标---------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    WindowToScreen(HWND hwnd,POINT *pt,s32 count)
{
    u32 i;
    RECT rc;
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            GK_ApiGetArea(hwnd->pGkWin, &rc);
            for(i=0;i<count;i++)
            {
                pt->x += rc.left;
                pt->y += rc.top;
            }
            HWND_Unlock(hwnd);
            return TRUE;
        }
    }
    return FALSE;
}

//----获得桌面窗口句柄-----------------------------------------------------------
//描述: 略
//参数：无
//返回：桌面窗口句柄.
//------------------------------------------------------------------------------
HWND    GetDesktopWindow(void)
{
    HWND hwnd;

    hwnd =NULL;
    if(HWND_Lock(HWND_Desktop))
    {
        hwnd =HWND_Desktop;
        HWND_Unlock(HWND_Desktop);
    }
    return hwnd;
}

//----获得窗口矩形区-----------------------------------------------------------
//描述: 获得窗口矩形区,矩形为屏幕坐标.
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    GetWindowRect(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    if(HWND_Lock(hwnd))
    {
        GK_ApiGetArea(hwnd->pGkWin, prc);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----获得窗口客户矩形区---------------------------------------------------------
//描述: 获得窗口客户矩形区,矩形为客户坐标.
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：TRUE:成功; FALSE:失败
//------------------------------------------------------------------------------
bool_t    GetClientRect(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    if(HWND_Lock(hwnd))
    {
        prc->left =0;
        prc->top =0;
        prc->right =RectW(&hwnd->CliRect);
        prc->bottom =RectH(&hwnd->CliRect);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----获得窗口客户矩形区并转换为屏幕坐标------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    GetClientRectToScreen(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    {
        if(HWND_Lock(hwnd))
        {
            CopyRect(prc,&hwnd->CliRect);
            HWND_Unlock(hwnd);
            return TRUE;
        }
    }
    return FALSE;
}

//----获得窗口消息队列-----------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数合法性,非线程安全
//参数：hwnd:窗口句柄
//返回：窗口消息队列指针.
//------------------------------------------------------------------------------
struct WindowMsgQ*   __GetWindowMsgQ(HWND hwnd)
{
    return hwnd->pMsgQ;
}

//----获得窗口线程Id-------------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数合法性,非线程安全
//参数：hwnd:窗口句柄
//返回：窗口线程Id.
//------------------------------------------------------------------------------
u32 __GetWindowEvent(HWND hwnd)
{
    return hwnd->EventID;
}

//----获得窗口风格---------------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法,非线程安全
//参数：hwnd:窗口句柄
//返回：窗口风格.
//------------------------------------------------------------------------------
u32 GetWindowStyle(HWND hwnd)
{
    u32 style=0;

    if(HWND_Lock(hwnd))
    {
        style =hwnd->Style;
        HWND_Unlock(hwnd);
    }
    return style;
}

//----获得窗口私有数据-----------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//返回：窗口私有数据.
//------------------------------------------------------------------------------
void* GetWindowPrivateData(HWND hwnd)
{
    void *data;

    data=NULL;
    if(HWND_Lock(hwnd))
    {
        data = hwnd->PrivateData;
        HWND_Unlock(hwnd);
    }
    return data;
}

//----设置窗口私有数据-----------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      data: 私有数据
//返回：无.
//------------------------------------------------------------------------------
void SetWindowPrivateData(HWND hwnd,void *data)
{
    if(HWND_Lock(hwnd))
    {
        hwnd->PrivateData=data;
        HWND_Unlock(hwnd);
    }
}

//----取父窗口-----------------------------------------------------------------
//功能：取窗口的父窗口句柄
//参数：hwnd，窗口句柄
//返回：父窗口句柄，无则返回NULL
//-----------------------------------------------------------------------------
HWND Gdd_GetWindowParent(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetParent(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}
//----取子窗口-----------------------------------------------------------------
//功能：取窗口的子窗口句柄
//参数：hwnd，窗口句柄
//返回：子窗口句柄，无则返回NULL
//-----------------------------------------------------------------------------
HWND GetWindowChild(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetChild(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----取一个前窗口-------------------------------------------------------------
//功能：取窗口的前窗口句柄
//参数：hwnd，窗口句柄
//返回：前窗口句柄，无则返回NULL
//-----------------------------------------------------------------------------
HWND GetWindowPrevious(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetPrevious(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----取一个后窗口-------------------------------------------------------------
//功能：取窗口的后窗口句柄
//参数：hwnd，窗口句柄
//返回：后窗口句柄，无则返回NULL
//-----------------------------------------------------------------------------
HWND GetWindowNext(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetNext(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----取一个最前窗口-------------------------------------------------------------
//功能：取窗口同级窗口中的最前窗口句柄
//参数：hwnd，窗口句柄
//返回：最前窗口句柄，无则返回NULL
//-----------------------------------------------------------------------------
HWND GetWindowFirst(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetFirst(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----取一个最后窗口-------------------------------------------------------------
//功能：取窗口的同级窗口中最后窗口句柄
//参数：hwnd，窗口句柄
//返回：最后窗口句柄，无则返回NULL
//-----------------------------------------------------------------------------
HWND GetWindowLast(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock( ))
    {
        wnd = (HWND)GK_ApiGetUserTag(GK_ApiGetLast(hwnd->pGkWin));
        GDD_Unlock( );
    }
    return wnd;
}

//----取一个最后窗口-------------------------------------------------------------
//功能：取窗口的同级窗口中最后窗口句柄
//参数：hwnd，窗口句柄
//返回：最后窗口句柄，无则返回NULL
//-----------------------------------------------------------------------------
char *GetWindowText(HWND hwnd)
{
    return hwnd->Text;
}

//----按Id查找窗口中的子窗口(控件)-----------------------------------------------
//描述: 略.
//参数：hwnd:窗口句柄
//     Id: 需要查找的子窗口(控件)Id.
//返回：查找到的子窗口(控件)句柄.
//------------------------------------------------------------------------------
HWND    GetDlgItem(HWND hwnd,u16 Id)
{
    HWND Current = hwnd;
    if(HWND_Lock(hwnd))
    {
        do
        {
            Current = (HWND)GK_ApiGetUserTag(GK_ApiTraveChild(hwnd->pGkWin,Current->pGkWin));
            if(Current->WinId == Id)
                break;
        }while(Current != NULL);
        HWND_Unlock(hwnd);
    }
    return Current;
}

//----按坐标查找窗口(主窗口或子窗口)-------------------------------------------
//描述: 略.
//参数：pt:坐标点
//返回：查找到的窗口句柄.
//------------------------------------------------------------------------------
HWND    GetWindowFromPoint(struct GkWinRsc *desktop, POINT *pt)
{
    struct GkWinRsc *GkWin;
    if((NULL == desktop) || (NULL == pt))
        return NULL;
    if(GDD_Lock( ))
    {
        GkWin = GK_ApiGetWinFromPt(desktop, pt);
        GDD_Unlock( );
    }
    if(GkWin != NULL)
        return (HWND)GK_ApiGetUserTag(GkWin);
    else
        return NULL;
}

//----设置窗口名字-------------------------------------------------------------
//描述: 略.
//参数：hwnd: 待设置的窗口
//      NewName，新的名字
//返回：旧的焦点窗口.
//------------------------------------------------------------------------------
void SetWindowName(HWND hwnd, char *NewName)
{
    GK_ApiSetName(hwnd->pGkWin, NewName);
    return ;
}

//----设置焦点窗口---------------------------------------------------------------
//描述: 略.
//参数：hwnd: 新的焦点窗口
//返回：旧的焦点窗口.
//------------------------------------------------------------------------------
HWND    SetFocusWindow(HWND hwnd)
{
    HWND wnd=NULL;

    if(GDD_Lock())
    {
        wnd =HWND_Focus;
        HWND_Focus =hwnd;
        GDD_Unlock();
    }
    else
    {
        return NULL;
    }

    if(wnd!=NULL)
    {
        SendMessage(wnd,MSG_KILLFOCUS,0,0);
    }
    if(hwnd!=NULL)
    {
        SendMessage(hwnd,MSG_SETFOCUS,0,0);
    }

    return wnd;

}

//----获得焦点窗口---------------------------------------------------------------
//描述: 略.
//参数：无.
//返回：焦点窗口.
//------------------------------------------------------------------------------
HWND    GetFocusWindow(void)
{
    HWND wnd=NULL;

    if(GDD_Lock())
    {
        wnd =HWND_Focus;
        GDD_Unlock();
    }
    return wnd;
}

//----判断是否为焦点窗口---------------------------------------------------------
//描述: 略.
//参数：无.
//返回：如果指定的窗口是焦点窗口,将返回TRUE,否则返回FALSE.
//------------------------------------------------------------------------------
bool_t    IsFocusWindow(HWND hwnd)
{
    bool_t res=FALSE;

    if(HWND_Lock(hwnd))
    {
        if(GDD_Lock())
        {
            if(hwnd == HWND_Focus)
            {
                res=TRUE;
            }
            GDD_Unlock();
        }
        HWND_Unlock(hwnd);
    }
    return res;

}

//----初始化窗口数据结构---------------------------------------------------------
//描述: 该函数为内部调用.
//参数：pwin:窗口数据结构指针.
//      proc:窗口过程回调函数指针．
//      Text:窗口文字内容指针.
//      Style:窗口风格.
//      x,y,w,h:窗口位置和大小.
//      hParent:父窗口句柄.
//      WinId: 窗口Id.
//返回：无.
//------------------------------------------------------------------------------
static  void    __InitWindow(WINDOW *pwin,u32 Style,u32 WinId)
{
    RECT rc;

    pwin->Flag =0;

    pwin->Style     =Style;
    pwin->WinId     =WinId&0x0000FFFF;
    pwin->Text = pwin->pGkWin->win_name;
    dListInit(&pwin->list_timer);
    dListInit(&pwin->node_msg_close);
    dListInit(&pwin->node_msg_ncpaint);
    dListInit(&pwin->node_msg_paint);
    GK_ApiGetArea(pwin->pGkWin, &rc);

    if(Style&WS_BORDER)
    {
        pwin->BorderSize =DEF_BORDER_SIZE;
        __InflateRectEx(&rc, -DEF_BORDER_SIZE,
                            -DEF_BORDER_SIZE,
                            -DEF_BORDER_SIZE,
                            -DEF_BORDER_SIZE);

    }
    else
    {
        pwin->BorderSize =0;
    }

    if(Style&WS_DLGFRAME)
    {
//      pwin->DlgFrameSize =DEF_DLGFRAME_SIZE;
        __InflateRectEx(&rc, -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE);
    }
//    else
//    {
//        pwin->DlgFrameSize =0;
//    }

    if(Style&WS_CAPTION)
    {
        pwin->CaptionSize =DEF_CAPTION_SIZE;
        __InflateRectEx(&rc,0,-DEF_CAPTION_SIZE,0,0);
    }
    else
    {
        pwin->CaptionSize =0;
    }
    CopyRect(&pwin->CliRect,&rc);

}


//桌面绘制函数
bool_t DesktopPaint(struct WindowMsg *pMsg)
{
    HDC hdc;
    RECT rc;

    HWND hwnd=pMsg->hwnd;
    hdc =BeginPaint(hwnd);

    GetClientRect(hwnd,&rc);

    GradientFillRect(hdc,&rc,RGB(120,120,255),RGB(20,20,80),CN_FILLRECT_MODE_UD);

    EndPaint(hwnd,hdc);
    return true;
}


//----光标绘制函数-------------------------------------------------------------
//功能：这是按钮控件的MSG_PAINT消息响应函数
//参数：pMsg，消息指针
//返回：固定true
//-----------------------------------------------------------------------------
static bool_t __Cursor_TmrHandle(struct WinTimer *pTmr)
{
    bool_t ret,bIsStart;
    bIsStart=pTmr->Flag;
    if(bIsStart&TMR_START)
    {
        ret=IsWindowVisible(g_CursorHwnd);
        if(ret)
        {
             SetWindowHide(g_CursorHwnd);
             UpdateDisplay(CN_TIMEOUT_FOREVER);
        }
        else
        {
             SetWindowShow(g_CursorHwnd);
             UpdateDisplay(CN_TIMEOUT_FOREVER);
        }
    }
    else
    {
        ret=IsWindowVisible(g_CursorHwnd);
        if(ret)
        {
             SetWindowHide(g_CursorHwnd);
             UpdateDisplay(CN_TIMEOUT_FOREVER);
        }
    }
    return true;
}

//----光标绘制函数-------------------------------------------------------------
//功能：这是按钮控件的MSG_PAINT消息响应函数
//参数：pMsg，消息指针
//返回：固定true
//-----------------------------------------------------------------------------
static bool_t DesktopTmrHandle(struct WindowMsg *pMsg)
{
    HWND hwnd;
    struct WinTimer *pTmr;
    u16 TmrId;
    if(pMsg==NULL)
        return false;
    hwnd=pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    TmrId=pMsg->Param1;
    pTmr=GDD_FindTimer(hwnd,TmrId);
    if(pTmr==NULL)
        return false;
    switch(TmrId)
    {
       case CN_CURSOR_TIMER_ID:
           __Cursor_TmrHandle(pTmr);
       break;
       default:
          break;
    }

    return true;
}
//----光标绘制函数-------------------------------------------------------------
//功能：这是按钮控件的MSG_PAINT消息响应函数
//参数：pMsg，消息指针
//返回：固定true
//-----------------------------------------------------------------------------
static bool_t DesktopTmrStart(struct WindowMsg *pMsg)
{
    HWND hwnd;
    struct WinTimer *pTmr;
    u16 TmrId;
    if(pMsg==NULL)
        return false;
    hwnd=pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    TmrId=pMsg->Param1;
    pTmr=GDD_FindTimer(hwnd,TmrId);
    if(pTmr==NULL)
        return false;
    pTmr->Flag|=TMR_START;
    return true;
}

//----光标绘制函数-------------------------------------------------------------
//功能：这是按钮控件的MSG_PAINT消息响应函数
//参数：pMsg，消息指针
//返回：固定true
//-----------------------------------------------------------------------------
static bool_t DesktopTmrStop(struct WindowMsg *pMsg)
{
    HWND hwnd;
    struct WinTimer *pTmr;
    u16 TmrId;
    if(pMsg==NULL)
        return false;
    hwnd=pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    TmrId=pMsg->Param1;
    pTmr=GDD_FindTimer(hwnd,TmrId);
    if(pTmr==NULL)
        return false;
    pTmr->Flag&=~TMR_START;
    return true;
}


//桌面消息处理函数表，
static struct MsgProcTable s_gDesktopMsgProcTable[] =
{
    {MSG_PAINT,DesktopPaint},
    {MSG_TIMER,DesktopTmrHandle},
    {MSG_TIMER_START,DesktopTmrStart},
    {MSG_TIMER_STOP,DesktopTmrStop}
};

static struct MsgTableLink  s_gDesktopMsgLink;

//----初始化桌面窗口-----------------------------------------------------------
//描述: 把gk的win转化为gdd的window.
//参数：proc: 桌面窗口过程回调函数
//      desktop,须初始化的桌面指针
//      pdata: 用户自定义附加数据.
//返回：桌面窗口句柄.
//------------------------------------------------------------------------------
HWND    InitGddDesktop(struct GkWinRsc *desktop)
{
    WINDOW *pGddWin=NULL;
    u32 Style;

    pGddWin=malloc(sizeof(WINDOW));
    if(NULL!=pGddWin)
    {

        Style = 0;
        s_gDesktopMsgLink.LinkNext = NULL;
        s_gDesktopMsgLink.MsgNum = sizeof(s_gDesktopMsgProcTable) / sizeof(struct MsgProcTable);
        s_gDesktopMsgLink.myTable = s_gDesktopMsgProcTable;
        pGddWin->pGkWin     = desktop;
        pGddWin->PrivateData = NULL;
        pGddWin->MyMsgTableLink = &s_gDesktopMsgLink;
        pGddWin->mutex_lock =Lock_MutexCreate(NULL);
        pGddWin->pMsgQ      =GUI_CreateMsgQ(32);
        pGddWin->EventID   = Djy_MyEventId();
        pGddWin->DrawColor = CN_DEF_DRAW_COLOR;
        pGddWin->FillColor = CN_DEF_FILL_COLOR;
        pGddWin->TextColor = CN_DEF_TEXT_COLOR;
        if((pGddWin->mutex_lock==NULL)||(pGddWin->pMsgQ==NULL))
        {
            if(pGddWin->mutex_lock!=NULL)
            {
                Lock_MutexDelete(pGddWin->mutex_lock);
            }
            if(pGddWin->pMsgQ!=NULL)
            {
                GUI_DeleteMsgQ(pGddWin->pMsgQ);
            }
            free(pGddWin);
            return NULL;
        }

        GK_ApiSetUserTag(desktop,pGddWin);
        __InitWindow(pGddWin,Style,0);
        HWND_Desktop = pGddWin;
        SendMessage(pGddWin,MSG_CREATE,0,0);
    }

        return pGddWin;
}

//----创建窗口-----------------------------------------------------------------
//描述: 该函数可以创建主窗口和子窗口(控件)
//参数：proc: 窗口过程回调函数
//      Style: 窗口风格(参考 WS_CHILD 族常量)
//      x,y,w,h: 窗口位置和大小,相对于父窗口
//      hParent: 父窗口句柄.如果是NULL,则默认桌面为父窗口.
//      WinId: 窗口Id.如果是主窗口,该参数被忽略.
//      BufProperty，窗口的buf属性，取值为 CN_WINBUF_PARENT等。
//      pdata: 用户自定义附加数据.
//      pUserMsgTableLink:窗口响应的消息表
//返回：窗口句柄.
//-----------------------------------------------------------------------------
HWND    CreateWindow(const char *Text,u32 Style,
                     s32 x,s32 y,s32 w,s32 h,
                     HWND hParent,u32 WinId,
                     u32 BufProperty,void *pdata,
                     struct MsgTableLink *pUserMsgTableLink)
{
    WINDOW *pGddWin=NULL;
    struct GkWinRsc *pGkWin=NULL;
    struct RopGroup RopCode = (struct RopGroup){ 0, 0, 0, CN_R2_COPYPEN, 0, 0, 0  };

    if(NULL==hParent)
    {
        hParent = HWND_Desktop;
    }

    if(HWND_Lock(hParent))
    {
        pGddWin=M_Malloc(sizeof(WINDOW) + sizeof(struct GkWinRsc),100*mS);
        if(NULL!=pGddWin)
        {
            pGkWin = (struct GkWinRsc*)(pGddWin+1);

            if(!GK_ApiCreateGkwin(hParent->pGkWin, pGkWin,x,y,x+w,y+h,
                                RGB(0,0,0), BufProperty, Text,
                                CN_SYS_PF_DISPLAY, 0,RGB(255, 255, 255),RopCode))
            {
                free(pGddWin);
                pGddWin = NULL;
            }
            else
            {
                if(Style&WS_CHILD)
                {
                    pGddWin->EventID   = hParent->EventID;
                    pGddWin->mutex_lock =hParent->mutex_lock;  //子窗口使用父窗口锁
                    pGddWin->pMsgQ      =hParent->pMsgQ;       //子窗口使用父窗口消息队列
                    pGddWin->DrawColor = hParent->DrawColor;
                    pGddWin->FillColor = hParent->FillColor;
                    pGddWin->TextColor = hParent->TextColor;
                }
                else
                {
                    pGddWin->EventID   = Djy_MyEventId();
                    pGddWin->mutex_lock =Lock_MutexCreate(NULL);
                    pGddWin->pMsgQ      =GUI_CreateMsgQ(32);
                    pGddWin->DrawColor = CN_DEF_DRAW_COLOR;
                    pGddWin->FillColor = CN_DEF_FILL_COLOR;
                    pGddWin->TextColor = CN_DEF_TEXT_COLOR;

                    if((pGddWin->mutex_lock==NULL)||(pGddWin->pMsgQ==NULL))
                    {
                        Lock_MutexDelete(pGddWin->mutex_lock);
                        GUI_DeleteMsgQ(pGddWin->pMsgQ);
                        GK_ApiDestroyWin(pGkWin);
                        free(pGkWin);
                        HWND_Unlock(hParent);
                        return NULL;
                    }

                }
                pGddWin->pGkWin = pGkWin;
                pGddWin->PrivateData = pdata;
                pGddWin->MyMsgTableLink = pUserMsgTableLink;
                GK_ApiSetUserTag(pGkWin,pGddWin);
                //初始化窗口数据
                __InitWindow(pGddWin,Style,WinId);
                GK_ApiSyncShow(CN_TIMEOUT_FOREVER);
                //将新窗口添加到父窗口
            }

        }
        HWND_Unlock(hParent);

        if(NULL!=pGddWin)
        {
            SendMessage(pGddWin,MSG_CREATE,(u32)pdata,0);
            PostMessage(pGddWin,MSG_NCPAINT,0,0);
            InvalidateWindow(pGddWin,TRUE);
        }

    }
    return pGddWin;
}

//----删除一个子窗口数据结构---------------------------------------------------
//描述:该函数为内部调用,除了释放窗口结构内存,还将自己丛父窗口去移除.
//参数：需要删除的子窗口句柄
//返回：无.
//------------------------------------------------------------------------------
static void __DeleteChildWindowData(HWND hwnd)
{
    dListRemove(&hwnd->node_msg_close);
    dListRemove(&hwnd->node_msg_ncpaint);
    dListRemove(&hwnd->node_msg_paint);
    __RemoveWindowTimer(hwnd);
    GK_ApiDestroyWin(hwnd->pGkWin);
    free(hwnd->pGkWin);
    hwnd->pGkWin =NULL;

    hwnd->mutex_lock =NULL; //子窗口没有私有的 mutex_lock,不用释放.

    free(hwnd);

}
//----删除一个主窗口数据结构---------------------------------------------------
//描述:该函数为内部调用,除了释放窗口结构内存,还将自己丛父窗口去移除.
//参数：需要删除的主窗口句柄
//返回：无.
//------------------------------------------------------------------------------
void __DeleteMainWindowData(HWND hwnd)
{
    dListRemove(&hwnd->node_msg_close);
    dListRemove(&hwnd->node_msg_ncpaint);
    dListRemove(&hwnd->node_msg_paint);

    __RemoveWindowTimer(hwnd);
    GUI_DeleteMsgQ(hwnd->pMsgQ);
    GK_ApiDestroyWin(hwnd->pGkWin);
    UpdateDisplay(CN_TIMEOUT_FOREVER);
    free(hwnd->pGkWin);
    hwnd->pGkWin =NULL;

    Lock_MutexDelete(hwnd->mutex_lock);
    hwnd->mutex_lock =NULL;
    free(hwnd);

}

//----销毁一个窗口--------------------------------------------------------------
//描述: 可以是主窗口和子窗口,除了删除自身窗口结构,还将发送WSG_DESTROY消息给父窗口.
//参数：hwnd:需要销毁的窗口句柄
//返回：无.
//------------------------------------------------------------------------------
void    DestroyWindow(HWND hwnd)
{
    struct GkWinRsc *Ancestor, *Current;

    SetWindowHide(hwnd);

    if(IsFocusWindow(hwnd))
    {
        SetFocusWindow(NULL);
    }

    GDD_Lock();
    if(HWND_Lock(hwnd))
    {
        Ancestor = hwnd->pGkWin;
        Current = Ancestor;
        Current = GK_ApiTraveScion(Ancestor,Current);
        while(Current != NULL)
        {
            __DeleteChildWindowData((HWND)GK_ApiGetUserTag(Current));
            Current = GK_ApiTraveScion(Ancestor,Current);
        }

        SendMessage(hwnd,MSG_DESTROY,0,0);
        if(hwnd->Style&WS_CHILD)
        {
            __DeleteChildWindowData(hwnd);   //若是主窗口，不会被销毁
        }
        HWND_Unlock(hwnd);

    }
    GDD_Unlock();

}

//----销毁全部子窗口-----------------------------------------------------------
//描述: 可以是主窗口和子窗口,除了删除自身窗口结构,还将发送WSG_DESTROY消息给父窗口.
//参数：hwnd:需要销毁的窗口句柄
//返回：无.
//------------------------------------------------------------------------------
void    DestroyAllChild(HWND hwnd)
{
    struct GkWinRsc *Ancestor, *Current;
    GDD_Lock();
    if(HWND_Lock(hwnd))
    {
        Ancestor = hwnd->pGkWin;
        Current = Container(OBJ_GetTwig(&Ancestor->node),struct GkWinRsc,node);
        while(Current != NULL)
        {
            __DeleteChildWindowData((HWND)GK_ApiGetUserTag(Current));
            Current = Container(OBJ_GetTwig(&Ancestor->node),struct GkWinRsc,node);
        }

        HWND_Unlock(hwnd);

    }
    GDD_Unlock( );

}

//----偏移窗口------------------------------------------------------------------
//描述: 偏移一个窗口位置,包括子窗口.该函数为内部调用,非线程安全
//参数：hwnd:窗口句柄.
//      dx,dy: 水平和垂直方向的偏移量.
//返回：无.
//------------------------------------------------------------------------------
static  void    __OffsetWindow(HWND hwnd,s32 dx,s32 dy)
{
    HWND wnd;
    RECT rc;
    struct GkWinRsc *Ancestor, *Current;

    Ancestor = hwnd->pGkWin;
    Current = Ancestor;
    Current = GK_ApiTraveScion(Ancestor,Current);
    __OffsetRect(&hwnd->CliRect,dx,dy);
    while(Current != NULL)
    {
        wnd = (HWND)GK_ApiGetUserTag(Current);
        OffsetRect(&wnd->CliRect,dx,dy);
        Current = GK_ApiTraveScion(Ancestor,Current);
    }
    GK_ApiGetArea(hwnd->pGkWin,&rc);
    GK_ApiMoveWin(  hwnd->pGkWin,rc.left+dx,rc.top+dy,0);

}

//----偏移窗口------------------------------------------------------------------
//描述: 偏移一个窗口位置,包括子窗口.
//参数：hwnd:窗口句柄.
//      dx,dy: 水平和垂直方向的偏移量.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    OffsetWindow(HWND hwnd,s32 dx,s32 dy)
{
    if(hwnd == HWND_Desktop)
    {//桌面不允许移动
        return false;
    }
    if(HWND_Lock(hwnd))
    {
        __OffsetWindow(hwnd,dx,dy);
        HWND_Unlock(hwnd);
        return true;
    }
    return FALSE;

}

//----移动窗口------------------------------------------------------------------
//描述: 移动一个窗口位置,包括子窗口.该函数为内部调用.非线程安全.
//参数：hwnd:窗口句柄.
//      x,y: 相对于父窗口客户区坐标位置.
//返回：无.
//------------------------------------------------------------------------------
static void __MoveWindow(HWND hwnd,s32 x,s32 y)
{
    HWND wnd;
    struct GkWinRsc *Ancestor, *Current;
    struct Rectangle rc;
    s32 dx,dy;
    POINT point;
    point.x=x;
    point.y=y;
    Ancestor = hwnd->pGkWin;
    Current = Ancestor;
    GK_ApiGetArea(Ancestor, &rc);//显示区域,显示器的绝对坐标
    wnd=Gdd_GetWindowParent(hwnd);
    ScreenToClient(wnd,(POINT *)&rc,2);//转化为客户区
    dx = x - rc.left;
    dy = y - rc.top;
    Current = GK_ApiTraveScion(Ancestor,Current);
    __OffsetRect(&hwnd->CliRect, dx, dy);
    while(Current != NULL)
    {
        wnd = (HWND)GK_ApiGetUserTag(Current);
        __OffsetRect(&wnd->CliRect,dx,dy);
        Current = GK_ApiTraveScion(Ancestor,Current);
    }
    ClientToScreen(wnd,&point,1);
    ScreenToWindow(wnd,&point,1);
    GK_ApiMoveWin(  hwnd->pGkWin,point.x,point.y,0);
}

//----移动窗口------------------------------------------------------------------
//描述: 移动一个窗口位置,包括子窗口.
//参数：hwnd:窗口句柄.
//      x,y: 相对于父窗口客户区坐标位置.
//返回：TRUE:成功;FALSE:失败.
//------------------------------------------------------------------------------
bool_t    MoveWindow(HWND hwnd,s32 x,s32 y)
{
    if(hwnd == HWND_Desktop)
    {//桌面不允许移动
        return false;
    }
    if(HWND_Lock(hwnd))
    {
        __MoveWindow(hwnd,x,y);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----判断窗口是否可见-----------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄.
//返回：TRUE:窗口可见;FALSE:窗口不可见.
//------------------------------------------------------------------------------
bool_t    IsWindowVisible(HWND hwnd)
{
    bool_t res=FALSE;

    if(HWND_Lock(hwnd))
    {
        res = GK_ApiIsWinVisible(hwnd->pGkWin);
        HWND_Unlock(hwnd);
    }
    return res;
}

//----设置窗口为无效-----------------------------------------------------------
//描述: 设置窗口为无效的意思是:窗口需要重绘.发送重绘消息便可,该函数为内部调用.
//参数：hwnd:窗口句柄.
//      bErase: 是否擦除背景.
//返回：无
//------------------------------------------------------------------------------
void    __InvalidateWindow(HWND hwnd,bool_t bErase)
{
    if(IsWindowVisible(hwnd))
    {
        //if(bErase)
        {
            PostMessage(hwnd,MSG_PAINT,bErase,0);
        }

    }
}

//----设置窗口为无效-------------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄.
//      bErase，true= 需要擦除背景，false=不需要擦除背景
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    InvalidateWindow(HWND hwnd,bool_t bErase)
{
    if(HWND_Lock(hwnd))
    {
        __InvalidateWindow(hwnd,bErase);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}


//----显示窗口-------------------------------------------------------------
//描述: 设置窗口为显示(包括子窗口)
//参数：hwnd:窗口句柄.
//返回：TRUE:成功;FALSE:失败.
//------------------------------------------------------------------------------
bool_t SetWindowShow(HWND hwnd)
{
    if(HWND_Lock(hwnd))
    {
        hwnd->Style |= WS_VISIBLE;
        GK_ApiSetVisible(hwnd->pGkWin,CN_GKWIN_VISIBLE,0);
        HWND_Unlock(hwnd);
        return TRUE;
    }
     return FALSE;
}
//----隐藏窗口-------------------------------------------------------------
//描述: 设置窗口为隐藏(包括子窗口)
//参数：hwnd:窗口句柄.
//返回：TRUE:成功;FALSE:失败.
//------------------------------------------------------------------------------
bool_t SetWindowHide(HWND hwnd)
{
    if(HWND_Lock(hwnd))
    {
        hwnd->Style |=~ WS_VISIBLE;
        GK_ApiSetVisible(hwnd->pGkWin,CN_GKWIN_HIDE,0);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----获得窗口绘图上下文(DC)-----------------------------------------------------
//描述: 该函数获得的DC,可以在整个窗口范围内绘图输出(非客户区+客户区).
//参数：hwnd:窗口句柄.
//返回：DC句柄.
//------------------------------------------------------------------------------
HDC GetWindowDC(HWND hwnd)
{
    DC *pdc;

    pdc =malloc(sizeof(DC));
    if(pdc!=NULL)
    {
        __InitDC(pdc,hwnd->pGkWin,hwnd,DC_TYPE_WINDOW);
    }
    return DC2HDC(pdc);

}

//----获得窗口客户区绘图上下文(DC)-----------------------------------------------
//描述: 该函数获得的DC,只能在窗口客户区范围内绘图输出).
//参数：hwnd:窗口句柄.
//返回：DC句柄.
//------------------------------------------------------------------------------
HDC GetDC(HWND hwnd)
{
    DC *pdc;

    pdc =malloc(sizeof(DC));
    if(pdc!=NULL)
    {
        __InitDC(pdc,hwnd->pGkWin,hwnd,DC_TYPE_CLIENT);
    }
    return DC2HDC(pdc);

}

//----获得窗口客户区绘图上下文(DC)-----------------------------------------------
//描述: 该函数获得的DC,只能在窗口客户区范围内绘图输出).
//参数：hwnd:窗口句柄.
//返回：DC句柄.
//------------------------------------------------------------------------------
bool_t    ReleaseDC(HWND hwnd,HDC hdc)
{
    return  DeleteDC(hdc);
}

//----指定窗口开始绘图-----------------------------------------------------------
//描述: 该函数只能在MSG_PAINT中调用,必须与EndPaint成对使用.
//参数：hwnd: 窗口句柄.
//返回：DC句柄.
//------------------------------------------------------------------------------
HDC BeginPaint(HWND hwnd)
{
    HDC hdc;
    struct WindowMsg msg;

    hdc =malloc(sizeof(DC));
    if(hdc!=NULL)
    {
        __InitDC(hdc,hwnd->pGkWin,hwnd,DC_TYPE_PAINT);

        if(HWND_Lock(hwnd))
        {
            if(hwnd->Flag&WF_ERASEBKGND)
            {
                hwnd->Flag &= ~WF_ERASEBKGND;

                msg.hwnd =hwnd;
                msg.Code =MSG_ERASEBKGND;
                msg.Param1 =(ptu32_t)hdc;
                msg.Param2 =0;
                msg.ExData =NULL;
                WinMsgProc(&msg);
            }

            HWND_Unlock(hwnd);
        }
        return hdc;
    }

    return NULL;
}

//----指定窗口结束绘图-----------------------------------------------------------
//描述: 该函数只能在MSG_PAINT中调用,必须与BeginPaint成对使用.
//参数：hwnd: 窗口句柄.
//      hdc: DC句柄,必须是BeginPaint返回的DC句柄.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
bool_t    EndPaint(HWND hwnd,HDC hdc)
{
    UpdateDisplay(CN_TIMEOUT_FOREVER);

    return  DeleteDC(hdc);
}

//----默认的窗口非客户区绘制消息处理函数---------------------------------------
//描述: 绘制窗口的非客户区.
//参数：pMsg: 消息指针.
//返回：无.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_NCPAINT(struct WindowMsg *pMsg)
{
    HWND hwnd=pMsg->hwnd;
    HDC hdc;
    RECT rc;

    if(HWND_Lock(hwnd))
    {
        hdc =GetWindowDC(hwnd);
        if(NULL!=hdc)
        {
            GetWindowRect(hwnd,&rc);
            ScreenToWindow(hwnd,(POINT*)&rc,2);

            if(hwnd->Style&WS_BORDER)
            {
                SetDrawColor(hdc,WINDOW_BORDER_COLOR);
                DrawRect(hdc,&rc);
                InflateRect(&rc,-1,-1);
            }

            if(hwnd->Style&WS_DLGFRAME)
            {
                SetDrawColor(hdc,WINDOW_DLGFRAME_COLOR1);
                DrawRect(hdc,&rc);
                InflateRect(&rc,-1,-1);

                SetDrawColor(hdc,WINDOW_DLGFRAME_COLOR2);
                DrawRect(hdc,&rc);
                InflateRect(&rc,-1,-1);

                SetDrawColor(hdc,WINDOW_DLGFRAME_COLOR3);
                DrawRect(hdc,&rc);
                InflateRect(&rc,-1,-1);

            }

            if(hwnd->Style&WS_CAPTION)
            {
                rc.bottom =rc.top+hwnd->CaptionSize;

                GradientFillRect(hdc,&rc,RGB(0,100,200),RGB(0,30,100),CN_FILLRECT_MODE_UD);

                SetTextColor(hdc,WINDOW_CAPTION_TEXT_COLOR);
                InflateRect(&rc,-1,-1);
                DrawText(hdc, hwnd->Text, -1, &rc, DT_LEFT | DT_VCENTER);
            }

            ReleaseDC(hwnd,hdc);
        }

        HWND_Unlock(hwnd);
    }
    return true;
}

//----默认的窗口客户区擦除背景消息处理函数-------------------------------------
//描述: 使用窗口的默认背景颜色，填充窗口客户区背景.
//参数：pMsg: 消息指针.
//返回：无.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_ERASEBKGND(struct WindowMsg *pMsg)
{
    HDC hdc;
    RECT rc;

    hdc =(HDC)pMsg->Param1;
    if(hdc!=NULL)
    {
        GetClientRect(pMsg->hwnd,&rc);
        FillRect(hdc,&rc);
        return TRUE;
    }
    return FALSE;
}

//----默认的窗口客户区绘制消息处理函数-------------------------------------
//描述: 好像没什么事做，先空着。.
//参数：pMsg: 消息指针.
//返回：无.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_PAINT(struct WindowMsg *pMsg)
{
    return true;
}

//----默认的窗口关闭消息处理函数-------------------------------------
//描述: 关闭窗口
//参数：pMsg: 消息指针.
//返回：无.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_CLOSE(struct WindowMsg *pMsg)
{
    DestroyWindow(pMsg->hwnd);
    return TRUE;
}

//----默认的销毁窗口消息处理函数-------------------------------------
//描述: 销毁.
//参数：pMsg: 消息指针.
//返回：无.
//-----------------------------------------------------------------------------
static bool_t DefWindowProc_DESTROY(struct WindowMsg *pMsg)
{
    PostQuitMessage(pMsg->hwnd,0);
    return TRUE;
}

//默认窗口消息处理函数表，处理用户窗口过程没有处理的消息。
static struct MsgProcTable s_gDefWindowMsgProcTable[] =
{
    {MSG_NCPAINT,DefWindowProc_NCPAINT},
    {MSG_ERASEBKGND,DefWindowProc_ERASEBKGND},
    {MSG_PAINT,DefWindowProc_PAINT},
    {MSG_CLOSE,DefWindowProc_CLOSE},
    {MSG_DESTROY,DefWindowProc_DESTROY},
};

//----在消息处理函数表中查找对应的消息-----------------------------------------
//功能：略
//参数：pMsg，目标消息
//      MsgTable，源表，以0结束。
//返回：若找到，则返回消息在MsgTable中的偏移，否则返回-1
//-----------------------------------------------------------------------------
s32 GetWinMsgFunc(struct WindowMsg *pMsg,struct MsgProcTable *MsgTable,u32 Num)
{
    u32 loop = 0;
    u32 Code,CodeTab;
    if((pMsg == NULL) || (MsgTable == NULL))
        return -1;
    Code = pMsg->Code;
    for(loop = 0; loop < Num; loop++)
    {
        CodeTab = MsgTable[loop].MsgCode;
        if(Code == CodeTab)
        {
            break;
        }
        else if(CodeTab == 0)
        {
            loop = -1;
            break;
        }
    }
    if(loop >= Num)
        loop = -1;
    return loop;

}
//----窗口消息处理函数---------------------------------------------------------
//描述: 处理窗口消息，消息可由hwnd->MyMsgTableLink成员实现覆盖，只要在子窗口中
//      实现同名消息，父窗口的该消息处理函数就会被“替换”掉。
//参数：pMsg: 消息指针.
//返回：消息处理结果.
//-----------------------------------------------------------------------------
ptu32_t WinMsgProc(struct WindowMsg *pMsg)
{
    HWND hwnd;
    u32 offset;
    struct MsgTableLink *MyTable;

    hwnd = pMsg->hwnd;
    MyTable = hwnd->MyMsgTableLink;
    //这是一级级消息处理继承机制，先从最低一级继承出发
    //以控件的paint消息处理为例说明一下：
    //一般来说，MyMsgTableLink直接指向的是用户的消息表，如果从用户表中找到了
    //      paint消息函数，则调用后返回。否则：
    //      MyMsgTableLink->LinkNext指向的是控件的消息表，里面一般能找到 paint
    //      消息处理函数，调用后返回。否则：
    //      从窗口系统的默认消息处理函数表 s_gDefWindowMsgProcTable中找。
    //这就是类似C++的继承机制。相当于用户继承控件，控件继承窗口系统。
    while(MyTable != NULL)
    {
        if(MyTable->myTable != NULL)
        {
            offset = GetWinMsgFunc(pMsg, MyTable->myTable, MyTable->MsgNum);
            if(offset != -1)
            {
                return MyTable->myTable[offset].MsgProc(pMsg);
            }
        }
        MyTable = MyTable->LinkNext;
    }

    offset = GetWinMsgFunc(pMsg, s_gDefWindowMsgProcTable,
                sizeof(s_gDefWindowMsgProcTable) / sizeof(struct MsgProcTable));
    if(offset != -1)
    {
        return s_gDefWindowMsgProcTable[offset].MsgProc(pMsg);
    }
    else
        return false;

}

//----GDD窗口模块初始化函数------------------------------------------------------
//描述: 该函数为内部调用.
//参数：无
//返回：无
//------------------------------------------------------------------------------
void    GDD_WindowInit(void)
{
}

