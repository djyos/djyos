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

#include    "gdd.h"
#include    "./include/gdd_private.h"

/*
typedef struct  __WNDCLASS  //窗口类数据结构
{
    WNDPROC *WinProc;
    u32 DrawColor;
    u32 FillColor;
    u32 TextColor;
}WNDCLASS;
*/


/*============================================================================*/

static  HWND HWND_Desktop=NULL;
static	HWND HWND_Focus=NULL;

static  struct tagGkWinRsc *pGkWinDesktop=NULL;

//----分配窗口对象内存-----------------------------------------------------------
//描述: 略
//参数：无
//返回：窗口对象内存内存指针
//------------------------------------------------------------------------------
static  WINDOW* WIN_Alloc(void)
{
    WINDOW *pwin;

    pwin=malloc(sizeof(WINDOW));
    if(NULL!=pwin)
    {
        pwin->Tag =WIN_TAG;
    }
    return pwin;
}

//----释放窗口对象内存-----------------------------------------------------------
//描述: 略
//参数：窗口对象内存内存指针
//返回：无
//------------------------------------------------------------------------------
static  void    WIN_Free(WINDOW *pwin)
{
    pwin->Tag =INVALD_TAG;
    free(pwin);
}

//----锁定窗口------------------------------------------------------------------
//描述: 锁定窗口,用于对窗口互斥操作,该函数返回TRUE时,必须调用HWND_Unlock解锁;
//      而当该函数返回FALSE,则无需调用HWND_Unlock.
//参数：hwnd:窗口句柄
//返回：成功:TRUE; 失败:FLASE;
//------------------------------------------------------------------------------
BOOL    HWND_Lock(HWND hwnd)
{
    GDD_Lock();
    if(NULL!=hwnd)
    if(WIN_TAG==hwnd->Tag)
    {
        //Lock_MutexPend(hwnd->mutex_lock,5000000);
        //GDD_Unlock();
        return TRUE;
    }
    GDD_Unlock();
    return  FALSE;
}

//----解锁窗口------------------------------------------------------------------
//描述: 当窗口锁定成功后,由该函数进行解锁操作.
//参数：hwnd:窗口句柄
//返回：无
//------------------------------------------------------------------------------
void    HWND_Unlock(HWND hwnd)
{
    GDD_Unlock();
    //Lock_MutexPost(hwnd->mutex_lock);
}

//----屏幕坐标转客户坐标---------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性.
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：无
//------------------------------------------------------------------------------
void    _ScreenToClient(HWND hwnd,POINT *pt,s32 count)
{
    int i;
    for(i=0;i<count;i++)
    {
        pt[i].x -= hwnd->CliRect.left;
        pt[i].y -= hwnd->CliRect.top;
    }
}

//----屏幕坐标转客户坐标---------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    ScreenToClient(HWND hwnd,POINT *pt,s32 count)
{
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            _ScreenToClient(hwnd,pt,count);
            HWND_Unlock(hwnd);
            return TRUE;
        }
    }
    return  FALSE;
}

//----客户坐标转屏幕坐标---------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：无
//------------------------------------------------------------------------------
void    _ClientToScreen(HWND hwnd,POINT *pt,s32 count)
{
    int i;

    for(i=0;i<count;i++)
    {
        pt[i].x += hwnd->CliRect.left;
        pt[i].y += hwnd->CliRect.top;
    }
}

//----客户坐标转屏幕坐标---------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    ClientToScreen(HWND hwnd,POINT *pt,s32 count)
{
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            _ClientToScreen(hwnd,pt,count);
            HWND_Unlock(hwnd);
            return  TRUE;
        }

    }
    return  FALSE;
}

//----屏幕坐标转窗口坐标---------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：无
//------------------------------------------------------------------------------
void    _ScreenToWindow(HWND hwnd,POINT *pt,s32 count)
{
    int i;

    for(i=0;i<count;i++)
    {
        pt[i].x -= hwnd->WinRect.left;
        pt[i].y -= hwnd->WinRect.top;
    }
}

//----屏幕坐标转窗口坐标---------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    ScreenToWindow(HWND hwnd,POINT *pt,s32 count)
{
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            _ScreenToWindow(hwnd,pt,count);
            HWND_Unlock(hwnd);
            return  TRUE;
        }
    }
    return  FALSE;
}

//----窗口坐标转屏幕坐标---------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：无
//------------------------------------------------------------------------------
void    _WindowToScreen(HWND hwnd,POINT *pt,s32 count)
{
    int i;
    for(i=0;i<count;i++)
    {
        pt->x += hwnd->WinRect.left;
        pt->y += hwnd->WinRect.top;
    }
}

//----窗口坐标转屏幕坐标---------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      pt:  需要转换的坐标点指针
//      count: 需要转换的坐标点数量
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    WindowToScreen(HWND hwnd,POINT *pt,s32 count)
{
    if(NULL!=pt)
    {
        if(HWND_Lock(hwnd))
        {
            _WindowToScreen(hwnd,pt,count);
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
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：无
//------------------------------------------------------------------------------
void    _GetWindowRect(HWND hwnd,RECT *prc)
{
    _CopyRect(prc,&hwnd->WinRect);
}

//----获得窗口矩形区-----------------------------------------------------------
//描述: 获得窗口矩形区,矩形为屏幕坐标.
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    GetWindowRect(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    if(HWND_Lock(hwnd))
    {
        _GetWindowRect(hwnd,prc);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----获得窗口客户矩形区---------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：无
//------------------------------------------------------------------------------
void    _GetClientRect(HWND hwnd,RECT *prc)
{

    prc->left =0;
    prc->top =0;
    prc->right =RectW(&hwnd->CliRect);
    prc->bottom =RectH(&hwnd->CliRect);
}

//----获得窗口客户矩形区---------------------------------------------------------
//描述: 获得窗口客户矩形区,矩形为客户坐标.
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：TRUE:成功; FALSE:失败
//------------------------------------------------------------------------------
BOOL    GetClientRect(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    if(HWND_Lock(hwnd))
    {
        _GetClientRect(hwnd,prc);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----获得窗口客户矩形区并转换为屏幕坐标--------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：无
//------------------------------------------------------------------------------
void    _GetClientRectToScreen(HWND hwnd,RECT *prc)
{
    _CopyRect(prc,&hwnd->CliRect);
}

//----获得窗口客户矩形区并转换为屏幕坐标------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      prc:用于保存矩形数据的指针.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    GetClientRectToScreen(HWND hwnd,RECT *prc)
{
    if(NULL!=prc)
    {
        if(HWND_Lock(hwnd))
        {
            _GetClientRectToScreen(hwnd,prc);
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
MSGQ*   _GetWindowMsgQ(HWND hwnd)
{
    return hwnd->pMsgQ;
}

//----获得窗口过程回调-----------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数合法性,非线程安全
//参数：hwnd:窗口句柄
//返回：窗口过程指针.
//------------------------------------------------------------------------------
WNDPROC* _GetWindowProc(HWND hwnd)
{
    return hwnd->WndProc;
}

//----获得窗口线程Id-------------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数合法性,非线程安全
//参数：hwnd:窗口句柄
//返回：窗口线程Id.
//------------------------------------------------------------------------------
u32 _GetWindowEvent(HWND hwnd)
{
    return hwnd->EventID;
}

//----获得父窗口-----------------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数合法性,非线程安全
//参数：hwnd:窗口句柄
//返回：父窗口句柄.
//------------------------------------------------------------------------------
HWND    _GetParent(HWND hwnd)
{
    return  hwnd->Parent;
}

//----获得父窗口-----------------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//返回：父窗口句柄.
//------------------------------------------------------------------------------
HWND    GetParent(HWND hwnd)
{
    HWND wnd=NULL;
    if(HWND_Lock(hwnd))
    {
        wnd= _GetParent(hwnd);
        HWND_Unlock(hwnd);
    }
    return wnd;
}

//----获得窗口风格---------------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法,非线程安全
//参数：hwnd:窗口句柄
//返回：窗口风格.
//------------------------------------------------------------------------------
u32 _GetWindowStyle(HWND hwnd)
{
    return  hwnd->Style;
}

u32	GetWindowStyle(HWND hwnd)
{
	u32 style=0;

	if(HWND_Lock(hwnd))
	{
		style =_GetWindowStyle(hwnd);
		HWND_Unlock(hwnd);
	}
	return style;
}

//----获得窗口私有数据-----------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法,非线程安全
//参数：hwnd:窗口句柄
//返回：窗口私有数据.
//------------------------------------------------------------------------------
void* _GetWindowPrivateData(HWND hwnd)
{
    return  hwnd->PrivateData;
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
		data =_GetWindowPrivateData(hwnd);
		HWND_Unlock(hwnd);
	}
	return data;
}

//----设置窗口私有数据-----------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法,非线程安全
//参数：hwnd:窗口句柄
//      data: 私有数据
//返回：无.
//------------------------------------------------------------------------------
void _SetWindowPrivateData(HWND hwnd,void *data)
{
   hwnd->PrivateData=data;
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
		_SetWindowPrivateData(hwnd,data);
		HWND_Unlock(hwnd);
	}
}

//----获得窗口用户数据-----------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法,非线程安全
//参数：hwnd:窗口句柄
//返回：窗口用户数据.
//------------------------------------------------------------------------------
void* _GetWindowUserData(HWND hwnd)
{
    return  hwnd->UserData;
}

//----获得窗口用户数据-----------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//返回：窗口用户数据.
//------------------------------------------------------------------------------
void* GetWindowUserData(HWND hwnd)
{
	void *data;

	data=NULL;
	if(HWND_Lock(hwnd))
	{
		data =_GetWindowUserData(hwnd);
		HWND_Unlock(hwnd);
	}
	return data;
}

//----设置窗口用户数据-----------------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法,非线程安全
//参数：hwnd:窗口句柄
//      data: 用户数据
//返回：无.
//------------------------------------------------------------------------------
void _SetWindowUserData(HWND hwnd,void *data)
{
   hwnd->UserData=data;
}

//----设置窗口用户数据-----------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      data: 用户数据
//返回：无.
//------------------------------------------------------------------------------
void SetWindowUserData(HWND hwnd,void *data)
{
	if(HWND_Lock(hwnd))
	{
		_SetWindowUserData(hwnd,data);
		HWND_Unlock(hwnd);
	}
}

//----按命令参数获得有特定关系的窗口----------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全
//参数：hwnd:窗口句柄
//      nCmd:命令参数
//返回：与指定窗口有特定关系的窗口句柄.
//------------------------------------------------------------------------------
HWND    _GetWindow(HWND hwnd,int nCmd)
{
    switch(nCmd)
    {
        case GW_CHILD:
            return  hwnd->Child;
            ////
        case GW_HWNDPREV:
            return  hwnd->Prev;
            ////
        case GW_HWNDNEXT:
            return hwnd->Next;
            ////
        case GW_HWNDFIRST:
            while(hwnd->Prev)
            {
                hwnd =hwnd->Prev;
            }
            return hwnd;
            ////
        case GW_HWNDLAST:
            while(hwnd->Next)
            {
                hwnd =hwnd->Next;
            }
            return hwnd;
            ////
        default:
            return NULL;

    }
}

//----按命令参数获得有特定关系的窗口----------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄
//      nCmd:命令参数
//返回：与指定窗口有特定关系的窗口句柄.
//------------------------------------------------------------------------------
HWND    GetWindow(HWND hwnd,int nCmd)
{
    HWND wnd=NULL;

    if(HWND_Lock(hwnd))
    {
        wnd =_GetWindow(hwnd,nCmd);
        HWND_Unlock(hwnd);
    }
    return wnd;
}

//----按Id查找窗口中的子窗口(控件)-----------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全
//参数：hwnd:窗口句柄
//     Id: 需要查找的子窗口(控件)Id.
//返回：查找到的子窗口(控件)句柄.
//------------------------------------------------------------------------------
HWND    _GetDlgItem(HWND hwnd,int Id)
{
    hwnd =hwnd->Child;
    while(hwnd)
    {
        if(hwnd->WinId == Id)
        {
            return hwnd;
        }

        hwnd =hwnd->Next;
    }
    return NULL;

}

//----按Id查找窗口中的子窗口(控件)-----------------------------------------------
//描述: 略.
//参数：hwnd:窗口句柄
//     Id: 需要查找的子窗口(控件)Id.
//返回：查找到的子窗口(控件)句柄.
//------------------------------------------------------------------------------
HWND    GetDlgItem(HWND hwnd,int Id)
{
    if(HWND_Lock(hwnd))
    {
        hwnd =_GetDlgItem(hwnd,Id);
        HWND_Unlock(hwnd);
    }
    return hwnd;
}

//----设置窗口文字内容-----------------------------------------------------------
//描述: 略.
//参数：hwnd:窗口句柄.
//     text: 文字内容.
//     max_len: 文字最大字节长度.
//返回：无.
//------------------------------------------------------------------------------
void    _SetWindowText(HWND hwnd,const char *text,s32 max_len)
{
	s32 len;

	if(text!=NULL)
	{
		len =MIN(max_len,sizeof(hwnd->Text));

		strncpy(hwnd->Text,text,len);
	}
}

void	SetWindowText(HWND hwnd,const char *text,s32 max_len)
{
	if(HWND_Lock(hwnd))
	{
		_SetWindowText(hwnd,text,max_len);
		HWND_Unlock(hwnd);
	}
}

//----获得窗口文字内容-----------------------------------------------------------
//描述: 略.
//参数：hwnd:窗口句柄.
//     text: 文字内容缓冲区.
//     max_len: 文字内容缓冲区最大字节长度.
//返回：文字缓冲区指针.
//------------------------------------------------------------------------------
char* _GetWindowText(HWND hwnd,char *buf,s32 max_len)
{
	s32 len;

	if(buf!=NULL)
	{
		len =MIN(max_len,sizeof(hwnd->Text));

		strncpy(buf,hwnd->Text,len);
		return buf;
	}

	return hwnd->Text;

}

char* GetWindowText(HWND hwnd,char *buf,s32 max_len)
{
	char *p;

	p=NULL;
	if(HWND_Lock(hwnd))
	{
		p =_GetWindowText(hwnd,buf,max_len);
		HWND_Unlock(hwnd);
	}
	return p;
}

//----按坐标查找窗口中的子窗口(控件)----------------------------------------------
//描述: 该函数为内部调用.
//参数：hwnd:窗口句柄
//     pt: 坐标点
//返回：查找到的子窗口(控件)句柄.
//------------------------------------------------------------------------------
HWND    GetChildWindowFromPoint(HWND hwnd,POINT *pt)
{
    RECT rc;

    if(NULL!=hwnd)
    {
        hwnd =GetWindow(hwnd,GW_CHILD);
        while(NULL!=hwnd)
        {
            if(NULL!=hwnd->Child)
            {
                GetChildWindowFromPoint(hwnd->Child,pt);
            }
            GetWindowRect(hwnd,&rc);

            if(PtInRect(&rc,pt))
            {
                return hwnd;
            }
            hwnd =GetWindow(hwnd,GW_HWNDNEXT);
        }
    }

    return NULL;
}

//----按坐标查找主窗口-----------------------------------------------------------
//描述: 该函数为内部调用.
//参数：pt:坐标点
//返回：查找到的主窗口句柄.
//------------------------------------------------------------------------------
HWND    GetMainWindowFromPoint(POINT *pt)
{
    HWND hwnd;
    RECT rc;

    hwnd =GetWindow(GetDesktopWindow(),GW_CHILD);
    while(NULL!=hwnd)
    {
        GetWindowRect(hwnd,&rc);
        if(_PtInRect(&rc,pt))
        {
            return hwnd;
        }
        hwnd =GetWindow(hwnd,GW_HWNDNEXT);

    }
    /////
    return  GetDesktopWindow();
}

//----按坐标查找窗口(主窗口或子窗口)----------------------------------------------
//描述: 略.
//参数：pt:坐标点
//返回：查找到的窗口句柄.
//------------------------------------------------------------------------------
HWND    GetWindowFromPoint(POINT *pt)
{
    HWND hwnd;
    HWND wnd=NULL;;

    hwnd =GetMainWindowFromPoint(pt);
    if(hwnd != GetDesktopWindow())
    {
        wnd =GetChildWindowFromPoint(hwnd,pt);
        if(wnd != NULL)
        {
            hwnd =wnd;
        }
    }

    return hwnd;
}

//----设置焦点窗口---------------------------------------------------------------
//描述: 略.
//参数：hwnd: 新的焦点窗口
//返回：旧的焦点窗口.
//------------------------------------------------------------------------------
HWND	SetFocusWindow(HWND hwnd)
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
HWND	GetFocusWindow(void)
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
BOOL	IsFocusWindow(HWND hwnd)
{
	BOOL res=FALSE;

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
static  void    _InitWindow(WINDOW *pwin,WNDPROC *proc,const char *Text,u32 Style,
                            int x,int y,int w,int h,HWND hParent,u32 WinId)
{
    RECT rc;

    _SetRect(&rc,x,y,w,h);
    pwin->Parent =hParent;
    pwin->Child  =NULL;
    pwin->Prev  =NULL;
    pwin->Next  =NULL;
    pwin->PrivateData =NULL;
    pwin->UserData    =NULL;
    pwin->WndProc   =proc;

    //pwin->Text        =Text;
    strncpy(pwin->Text,Text,250);
    itoa((u32)pwin,pwin->Name,16);

    pwin->Style     =Style;
    pwin->WinId     =WinId&0x0000FFFF;
    list_init(&pwin->list_timer);
    list_init(&pwin->node_msg_close);
    list_init(&pwin->node_msg_ncpaint);
    list_init(&pwin->node_msg_paint);

    if(hParent != NULL)
    {
        _ClientToScreen(hParent,(POINT*)&rc,2);
    }

    _CopyRect(&pwin->WinRect,&rc);

    if(Style&WS_BORDER)
    {
        pwin->BorderSize =DEF_BORDER_SIZE;
        _InflateRectEx(&rc, -DEF_BORDER_SIZE,
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
        pwin->DlgFrameSize =DEF_DLGFRAME_SIZE;
        _InflateRectEx(&rc, -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE,
                            -DEF_DLGFRAME_SIZE);
    }
    else
    {
        pwin->DlgFrameSize =0;
    }

    if(Style&WS_CAPTION)
    {
        pwin->CaptionSize =DEF_CAPTION_SIZE;
        _InflateRectEx(&rc,0,-DEF_CAPTION_SIZE,0,0);
    }
    else
    {
        pwin->CaptionSize =0;
    }
    _CopyRect(&pwin->CliRect,&rc);

}

//----添加一个子窗口到父窗口-----------------------------------------------------
//描述: 该函数为内部调用,不检查函数参数的合法性,非线程安全.
//参数：hParent:父窗口句柄.
//      hChild:需要添加的子窗口句柄.
//返回：无
//------------------------------------------------------------------------------
static void _AddChild(HWND hParent,HWND hChid)
{
    WINDOW *old;

    old =hParent->Child;

    hChid->Prev =NULL;
    hChid->Next =old;

    if(NULL!=old)
    {
        old->Prev =hChid;
    }

    hParent->Child =hChid;
}

//----创始桌面窗口--------------------------------------------------------------
//描述: 该函数为内部调用,仅限于创建桌面.
//参数：proc: 桌面窗口过程回调函数
//      Text: 桌面窗口文字指针
//      x,y,w,h: 桌面窗口位置和大小
//      pdata: 用户自定义附加数据.
//返回：桌面窗口句柄.
//------------------------------------------------------------------------------
HWND    CreateDesktop(
                    WNDPROC *proc,
                    const char *Text,
                    int x,int y,int w,int h,
                    const void *pdata)
{
    WINDOW *pwin=NULL;
    struct tagGkWinRsc *pGkWin=NULL;
    u32 Style;

    if(NULL!=proc)
    {
        pwin =WIN_Alloc();
        if(NULL!=pwin)
        {

            Style = 0;
            Style &=~(WS_CHILD);

            _InitWindow(pwin,proc,Text,Style,x,y,w,h,NULL,0);
            pGkWin =(struct tagGkWinRsc*)malloc(sizeof(struct tagGkWinRsc));

            if(!GK_ApiCreateGkwin(pGkWinDesktop, pGkWin, x,y,x+w,y+h,
                                RGB(0,0,0), CN_WINBUF_BUF, "desktop",
                                CN_SYS_PF_DISPLAY, 0,0,0))
            {
                free(pGkWin);
                return FALSE;
            }

            pwin->pGkWin     =pGkWin;
            pwin->mutex_lock =Lock_MutexCreate(NULL);
            pwin->pMsgQ      =GUI_CreateMsgQ(32);
            pwin->EventID   =Djy_MyEventId();
            pwin->Zorder     =0;
            GK_ApiSetPrio(pGkWin,pwin->Zorder,100*mS);

            HWND_Desktop =pwin;
            SendMessage(pwin,MSG_CREATE,(u32)pdata,0);
        }
    }
    return pwin;
}

//----创建窗口------------------------------------------------------------------
//描述: 该函数可以创建主窗口和子窗口(控件)
//参数：proc: 窗口过程回调函数
//      Text: 窗口文字指针
//      Style: 窗口风格(具体参考窗口风格组合说明)
//      x,y,w,h: 桌面窗口位置和大小
//      hParent: 父窗口句柄.如果是NULL,则默认桌面为父窗口.
//      WinId: 窗口Id.如果是主窗口,该参数被忽略.
//      pdata: 用户自定义附加数据.
//返回：窗口句柄.
//------------------------------------------------------------------------------
HWND    CreateWindow(WNDPROC *pfWinProc,
                    const char *Text,u32 Style,
                    s32 x,s32 y,s32 w,s32 h,
                    HWND hParent,u32 WinId,const void *pdata)
{
    WINDOW *pwin=NULL;
    struct tagGkWinRsc *pGkWin=NULL;

    if(NULL==hParent)
    {
        hParent =GetDesktopWindow();
    }

    if(HWND_Lock(hParent))
    {
        pwin =WIN_Alloc();
        if(NULL!=pwin)
        {
            //初始化窗口数据
            _InitWindow(pwin,pfWinProc,Text,Style,x,y,w,h,hParent,WinId);

            x =pwin->WinRect.left;
            y =pwin->WinRect.top;
            w =RectW(&pwin->WinRect);
            h =RectH(&pwin->WinRect);

            pGkWin =(struct tagGkWinRsc*)malloc(sizeof(struct tagGkWinRsc));

            if(Style&WS_CHILD)
            {
                if(!GK_ApiCreateGkwin(pGkWinDesktop, pGkWin,x,y,x+w,y+h,
                                    RGB(0,0,0), CN_WINBUF_BUF, pwin->Name,
                                    CN_SYS_PF_DISPLAY, 0,0,0))
                {
                    free(pGkWin);
                    return NULL;
                }

                pwin->mutex_lock =hParent->mutex_lock;  //子窗口使用父窗口锁
                pwin->pMsgQ      =hParent->pMsgQ;       //子窗口使用父窗口消息队列
                pwin->EventID   =hParent->EventID;
                pwin->Zorder     =hParent->Zorder-1;
                GK_ApiSetPrio(pGkWin,pwin->Zorder,100*mS);

            }
            else
            {
                if(!GK_ApiCreateGkwin(pGkWinDesktop, pGkWin,x,y,x+w,y+h,
                                    RGB(0,0,0), CN_WINBUF_BUF, pwin->Name,
                                    CN_SYS_PF_DISPLAY, 0,0,0))
                {
                    free(pGkWin);
                    return NULL;
                }

                pwin->mutex_lock =Lock_MutexCreate(NULL);
                pwin->pMsgQ      =GUI_CreateMsgQ(32);
                pwin->EventID   =Djy_MyEventId();
                pwin->Zorder     =hParent->Zorder-1;
                GK_ApiSetPrio(pGkWin,pwin->Zorder,100*mS);

            }
            pwin->pGkWin = pGkWin;

            //将新窗口添加到父窗口
            _AddChild(hParent,pwin);

        }
        HWND_Unlock(hParent);

        if(NULL!=pwin)
        {
            SendMessage(pwin,MSG_CREATE,(u32)pdata,0);
            InvalidateWindow(pwin);
        }

    }
    return pwin;
}

//----删除一个子窗口数据结构-----------------------------------------------------
//描述:该函数为内部调用,除了释放窗口结构内存,还将自己丛父窗口去移除.
//参数：需要删除的子窗口句柄
//返回：无.
//------------------------------------------------------------------------------
static void _DeleteChildWindowData(HWND hwnd)
{
    WINDOW *prev,*next;

    if(hwnd->Style&WS_CHILD)
    {
        prev =hwnd->Prev;
        next =hwnd->Next;

        if(NULL!=next)
        {
            next->Prev =prev;
        }

        if(NULL!=prev)
        {
            prev->Next =next;
        }
        else
        {
            hwnd->Parent->Child =NULL;
        }

        hwnd->Prev =NULL;
        hwnd->Next =NULL;

        list_remove(&hwnd->node_msg_close);
        list_remove(&hwnd->node_msg_ncpaint);
        list_remove(&hwnd->node_msg_paint);
        _RemoveWindowTimer(hwnd);
        GK_ApiDestroyWin(hwnd->pGkWin);
        free(hwnd->pGkWin);
        hwnd->pGkWin =NULL;

        hwnd->mutex_lock =NULL; //子窗口没有私有的 mutex_lock,不用释放.

        WIN_Free(hwnd);
    }

}
//----删除一个主窗口数据结构-----------------------------------------------------
//描述:该函数为内部调用,除了释放窗口结构内存,还将自己丛父窗口去移除.
//参数：需要删除的主窗口句柄
//返回：无.
//------------------------------------------------------------------------------
void _DeleteMainWindowData(HWND hwnd)
{
    WINDOW *prev,*next;

    //GDD_Lock();

    prev =hwnd->Prev;
    next =hwnd->Next;

    if(NULL!=next)
    {
        next->Prev =prev;
    }

    if(NULL!=prev)
    {
        prev->Next =next;
    }
    else
    {
        hwnd->Parent->Child =NULL;
    }

    hwnd->Prev =NULL;
    hwnd->Next =NULL;

    list_remove(&hwnd->node_msg_close);
    list_remove(&hwnd->node_msg_ncpaint);
    list_remove(&hwnd->node_msg_paint);

    _RemoveWindowTimer(hwnd);
    GUI_DeleteMsgQ(hwnd->pMsgQ);
    UpdateDisplay();
    GK_ApiDestroyWin(hwnd->pGkWin);
    UpdateDisplay();
    free(hwnd->pGkWin);
    hwnd->pGkWin =NULL;

    Lock_MutexDelete(hwnd->mutex_lock);
    hwnd->mutex_lock =NULL;
    WIN_Free(hwnd);

    //GDD_Unlock();
}


//----销毁一个子窗口-------------------------------------------------------------
//描述: 该函数为内部调用,除了删除自身窗口结构,还将发送WSG_DESTROY消息给父窗口.
//参数：需要销毁的子窗口句柄
//返回：无.
//------------------------------------------------------------------------------
static void _DestroyChildWindow(HWND hwnd)
{
    HWND wnd,next;

    if(NULL!=hwnd->Child)   //先销毁子窗口
    {
        wnd =hwnd->Child;
        while(NULL!=wnd)
        {
            if(NULL!=wnd->Child)
            {
                _DestroyChildWindow(wnd);
            }
            next =wnd->Next;

            SendMessage(wnd,MSG_DESTROY,0,0);
            _DeleteChildWindowData(wnd);

            wnd =next;
        }
    }

}

//----销毁一个窗口--------------------------------------------------------------
//描述: 可以是主窗口和子窗口,除了删除自身窗口结构,还将发送WSG_DESTROY消息给父窗口.
//参数：hwnd:需要销毁的窗口句柄
//返回：无.
//------------------------------------------------------------------------------
void    DestroyWindow(HWND hwnd)
{
	if(IsFocusWindow(hwnd))
	{
		SetFocusWindow(NULL);
	}

    UpdateDisplay();

    GDD_Lock();
    if(HWND_Lock(hwnd))
    {
        _DestroyChildWindow(hwnd);

        SendMessage(hwnd,MSG_DESTROY,0,0);
        if(hwnd->Style&WS_CHILD)
        {
            _DeleteChildWindowData(hwnd);
        }

        HWND_Unlock(hwnd);

    }
    GDD_Unlock();

}

//----偏移一个窗口--------------------------------------------------------------
//描述: 偏移一个窗口位置,不包括子窗口.该函数为内部调用,非线程安全
//参数：hwnd:窗口句柄.
//      dx,dy: 水平和垂直方向的偏移量.
//返回：无.
//------------------------------------------------------------------------------
static  void    _OffsetWindowRect(HWND hwnd,int dx,int dy)
{
    _OffsetRect(&hwnd->WinRect,dx,dy);
    _OffsetRect(&hwnd->CliRect,dx,dy);

    GK_ApiMoveWin(  hwnd->pGkWin,
                    hwnd->WinRect.left,hwnd->WinRect.top,
                    0*mS);
    PostMessage(hwnd,MSG_PAINT,0,0);

}

//----偏移窗口------------------------------------------------------------------
//描述: 偏移一个窗口位置,包括子窗口.该函数为内部调用,非线程安全
//参数：hwnd:窗口句柄.
//      dx,dy: 水平和垂直方向的偏移量.
//返回：无.
//------------------------------------------------------------------------------
static  void    _OffsetWindow(HWND hwnd,int dx,int dy)
{
    HWND wnd;

    _OffsetWindowRect(hwnd,dx,dy);

    wnd =hwnd->Child;
    while(NULL!=wnd)
    {
        _OffsetWindowRect(wnd,dx,dy);
        wnd =wnd->Next;
    }

}

//----偏移窗口------------------------------------------------------------------
//描述: 偏移一个窗口位置,包括子窗口.
//参数：hwnd:窗口句柄.
//      dx,dy: 水平和垂直方向的偏移量.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    OffsetWindow(HWND hwnd,int dx,int dy)
{
    if(HWND_Lock(hwnd))
    {
        _OffsetWindow(hwnd,dx,dy);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;

}

//----移动窗口------------------------------------------------------------------
//描述: 移动一个窗口位置,包括子窗口.该函数为内部调用.非线程安全.
//参数：hwnd:窗口句柄.
//      x,y: 相对于父窗口客户区坐标位置.
//返回：无.
//------------------------------------------------------------------------------
static void _MoveWindow(HWND hwnd,int x,int y)
{
    HWND hParent;
    POINT pt;

    if(hwnd == HWND_Desktop)
    {//桌面不允许移动
        return;
    }

    pt.x =x;
    pt.y =y;

    hParent =_GetParent(hwnd);
    if(NULL!=hParent)
    {
        _ClientToScreen(hParent,&pt,1);
    }

    x = pt.x - hwnd->WinRect.left;
    y = pt.y - hwnd->WinRect.top;
    _OffsetWindow(hwnd,x,y);


}

//----移动窗口------------------------------------------------------------------
//描述: 移动一个窗口位置,包括子窗口.
//参数：hwnd:窗口句柄.
//      x,y: 相对于父窗口客户区坐标位置.
//返回：TRUE:成功;FALSE:失败.
//------------------------------------------------------------------------------
BOOL    MoveWindow(HWND hwnd,int x,int y)
{
    if(HWND_Lock(hwnd))
    {
        _MoveWindow(hwnd,x,y);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----判断窗口是否可见-----------------------------------------------------------
//描述: 该函数为内部调用,不进行函数参数检测,非线程安全.
//参数：hwnd:窗口句柄.
//返回：TRUE:窗口可见;FALSE:窗口不可见.
//------------------------------------------------------------------------------
BOOL    _IsWindowVisible(HWND hwnd)
{
    while(hwnd)
    {
        if(!(hwnd->Style&WS_VISIBLE))
        {
            return FALSE;
        }
        hwnd=hwnd->Parent;
    }
    return TRUE;
}

//----判断窗口是否可见-----------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄.
//返回：TRUE:窗口可见;FALSE:窗口不可见.
//------------------------------------------------------------------------------
BOOL    IsWindowVisible(HWND hwnd)
{
    BOOL res=FALSE;

    if(HWND_Lock(hwnd))
    {
        res =_IsWindowVisible(hwnd);
        HWND_Unlock(hwnd);
    }
    return res;
}

//----设置窗口为无效-----------------------------------------------------------
//描述: 设置窗口为无效的意思是:窗口需要重绘.发送重绘消息便可,该函数为内部调用.
//参数：hwnd:窗口句柄.
//返回：无
//------------------------------------------------------------------------------
void    _InvalidateWindow(HWND hwnd)
{
    if(_IsWindowVisible(hwnd))
    {
        PostMessage(hwnd,MSG_NCPAINT,0,0);
        PostMessage(hwnd,MSG_PAINT,0,0);
    }
}

//----设置窗口为无效-------------------------------------------------------------
//描述: 略
//参数：hwnd:窗口句柄.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    InvalidateWindow(HWND hwnd)
{
    if(HWND_Lock(hwnd))
    {
        _InvalidateWindow(hwnd);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

//----显示/隐藏子窗口-------------------------------------------------------------
//描述: 设置所有子窗口为显示或隐藏,该函数为内部调用.
//参数：hwnd:窗口句柄.
//      bShow: TURE:显示窗口; FALSE:隐藏窗口.
//返回：无.
//------------------------------------------------------------------------------
static void _ShowChild(HWND hwnd,BOOL bShow)
{

    if(hwnd->Child!=NULL)
    {
        _ShowChild(hwnd->Child,bShow);
    }

    hwnd =_GetWindow(hwnd,GW_HWNDLAST);
    while(hwnd)
    {
        if(bShow)
        {
            hwnd->Style |= WS_VISIBLE;
            PostMessage(hwnd,MSG_PAINT,0,0);
            PostMessage(hwnd,MSG_NCPAINT,0,0);
        }
        else
        {
            hwnd->Style &= ~WS_VISIBLE;
        }
        hwnd =_GetWindow(hwnd,GW_HWNDPREV);
    }
}

//----显示/隐藏窗口-------------------------------------------------------------
//描述: 设置窗口为显示或隐藏(包括子窗口),该函数为内部调用.
//参数：hwnd:窗口句柄.
//      bShow: TURE:显示窗口; FALSE:隐藏窗口.
//返回：无.
//------------------------------------------------------------------------------
static void _ShowWindow(HWND hwnd,BOOL bShow)
{
    UpdateDisplay();
    if(bShow)
    {
        //printf("show: %08XH\r\n",hwnd);

        GK_ApiSetPrio(hwnd->pGkWin,hwnd->Zorder,100*mS);

        hwnd->Style |= WS_VISIBLE;
        PostMessage(hwnd,MSG_PAINT,0,0);
        PostMessage(hwnd,MSG_NCPAINT,0,0);
        _ShowChild(hwnd,bShow);

    }
    else
    {
        //printf("hide:%08XH\r\n",hwnd);

    //  GK_ApiSetPrio(hwnd->pGkWin,1,100*mS);
        hwnd->Style &= ~WS_VISIBLE;
        _ShowChild(hwnd,bShow);

    }

}

//----显示/隐藏窗口-------------------------------------------------------------
//描述: 设置窗口为显示或隐藏(包括子窗口),该函数为内部调用.
//参数：hwnd:窗口句柄.
//      bShow: TURE:显示窗口; FALSE:隐藏窗口.
//返回：TRUE:成功;FALSE:失败.
//------------------------------------------------------------------------------
BOOL    ShowWindow(HWND hwnd,BOOL bShow)
{
    if(HWND_Lock(hwnd))
    {
        _ShowWindow(hwnd,bShow);
        HWND_Unlock(hwnd);
        return TRUE;
    }
    return FALSE;
}

#if 0
//----使能/禁止窗口-------------------------------------------------------------
//描述: 设置窗口为使能或禁止.
//参数：hwnd:窗口句柄.
//      bEnable: TURE:使能窗口; FALSE:禁止窗口.
//返回：TRUE:成功;FALSE:失败.
//------------------------------------------------------------------------------
BOOL    EnableWindow(HWND hwnd,bool_t bEnable)
{
    return  FALSE;
}
#endif

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
        _InitDC(pdc,hwnd->pGkWin,hwnd,DC_TYPE_WINDOW);
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
        _InitDC(pdc,hwnd->pGkWin,hwnd,DC_TYPE_CLIENT);
    }
    return DC2HDC(pdc);

}

//----获得窗口客户区绘图上下文(DC)-----------------------------------------------
//描述: 该函数获得的DC,只能在窗口客户区范围内绘图输出).
//参数：hwnd:窗口句柄.
//返回：DC句柄.
//------------------------------------------------------------------------------
BOOL    ReleaseDC(HWND hwnd,HDC hdc)
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
    DC *pdc;

    pdc =malloc(sizeof(DC));
    if(pdc!=NULL)
    {
        _InitDC(pdc,hwnd->pGkWin,hwnd,DC_TYPE_PAINT);
    }
    return DC2HDC(pdc);
}

//----指定窗口结束绘图-----------------------------------------------------------
//描述: 该函数只能在MSG_PAINT中调用,必须与BeginPaint成对使用.
//参数：hwnd: 窗口句柄.
//      hdc: DC句柄,必须是BeginPaint返回的DC句柄.
//返回：TRUE:成功; FALSE:失败.
//------------------------------------------------------------------------------
BOOL    EndPaint(HWND hwnd,HDC hdc)
{
    UpdateDisplay();

    return  DeleteDC(hdc);
}

//----默认的窗口非客户区绘制消息处理函数------------------------------------------
//描述: 该函数为内部函数,在DefWindowPorc的MSG_NCPAINT中被调用.
//参数：pMsg: 消息指针.
//返回：无.
//------------------------------------------------------------------------------
static void DefWindowProc_NCPAINT(MSG *pMsg)
{
    HWND hwnd=pMsg->hwnd;
    HDC hdc;
    RECT rc;

    if(HWND_Lock(hwnd))
    {
        hdc =GetWindowDC(hwnd);
        if(NULL!=hdc)
        {
            _GetWindowRect(hwnd,&rc);
            _ScreenToWindow(hwnd,(POINT*)&rc,2);

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

                GradientFillRect(hdc,&rc,RGB(180,180,200),RGB(0,0,180),GFILL_U_D);

                SetTextColor(hdc,WINDOW_CAPTION_TEXT_COLOR);
                InflateRect(&rc,-1,-1);
                DrawText(hdc,hwnd->Text,-1,&rc,DT_LEFT|DT_VCENTER);
            }

            ReleaseDC(hwnd,hdc);
        }

        HWND_Unlock(hwnd);
    }


}

//----默认的窗口消息处理函数-----------------------------------------------------
//描述: 用户不感兴趣的消息,由该函数处理(系统并不会将所有消息都处理).
//参数：pMsg: 消息指针.
//返回：消息处理结果.
//------------------------------------------------------------------------------
u32 DefWindowProc(MSG *pMsg)
{

    switch(pMsg->Code)
    {
        case    MSG_NCPAINT:
                DefWindowProc_NCPAINT(pMsg);
                break;
                ////
        case    MSG_CLOSE:
                DestroyWindow(pMsg->hwnd);
                return 1;
                ////
        case    MSG_DESTROY:
                PostQuitMessage(pMsg->hwnd,0);
                return 1;

        default:
                break;
    }

    return 0;
}

//----GDD窗口模块初始化函数------------------------------------------------------
//描述: 该函数为内部调用.
//参数：无
//返回：无
//------------------------------------------------------------------------------
void    GDD_WindowInit(void)
{
    HWND_Desktop =NULL;
    HWND_Focus   =NULL;
}

//----默认的桌面窗口过程回调函数------------------------------------------------------
//描述: 该函数为内部调用.
//参数：消息指针.
//返回：消息处理结果.
//------------------------------------------------------------------------------

static HWND wnd=NULL;
static  u32 desktop_proc(MSG *msg)
{
	RECT rc;
    HWND hwnd=msg->hwnd;
    switch(msg->Code)
    {
        case    MSG_CREATE:
                wnd=NULL;
           //     printf("Desktop: MSG_CREATE: %08XH.\r\n",hwnd);
                GetClientRect(hwnd,&rc);
             //   GDD_CreateTimer(hwnd,1122,1000,TMR_START);  //循环定时

                break;
                ////
        case    MSG_LBUTTON_DOWN:
         //       printf("Desktop: MSG_LBUTTON_DOWN: %d,%d\r\n",LO16(msg->Param2),HI16(msg->Param2));
                break;
                ////
        case    MSG_LBUTTON_UP:
          //      printf("Desktop: MSG_LBUTTON_UP: %d,%d.\r\n",LO16(msg->Param2),HI16(msg->Param2));
                break;
                ////
        case    MSG_MOUSE_MOVE:
          //      printf("Desktop: MSG_MOUSE_MOVE: %08XH,%d,%d.\r\n",msg->Param1,LO16(msg->Param2),HI16(msg->Param2));
                break;
                ////
        /*
        case    MSG_GET_POS:
                printf("get_pos:%d\r\n",msg->Param1);
                return 0x1234;
                break;
        */
                /////

        case    MSG_TIMER:
                {
                    if(msg->Param1==1)
                    {
                       // InvalidateWindow(hwnd);
                        //PostMessage(hwnd,MSG_PAINT,0,0);
                    }
                }
                break;
                /////

        case    MSG_PAINT:
                {


                    HDC hdc;
                    RECT rc;


                    hdc =BeginPaint(hwnd);

                    GetClientRect(hwnd,&rc);

                    //SetFillColor(hdc,RGB(50,100,200));
                    //FillRect(hdc,&rc);

                    GradientFillRect(hdc,&rc,RGB(120,120,255),RGB(20,20,80),GFILL_U_D);

                    EndPaint(hwnd,hdc);
                }
                break;
        default:
                return DefWindowProc(msg);
    }

    return  0;
}


//----GDD主函数-----------------------------------------------------------------
//描述: GDD服务执行函数,该函数不会返回.
//参数：dev_name: 显示设备名
//返回：无
//------------------------------------------------------------------------------

void    GDD_TimerExecu(u32 tick_time_ms);

void    GDD_Execu(struct tagGkWinRsc *desktop)
{
    HWND hwnd;
    MSG msg;
    int w,h;

    pGkWinDesktop=desktop;

    w=pGkWinDesktop->right-pGkWinDesktop->left;
    h=pGkWinDesktop->bottom-pGkWinDesktop->top;


    hwnd =CreateDesktop(desktop_proc,"Desktop",
                        0,0,w,h,
                        NULL);

    ShowWindow(hwnd,TRUE);

    while(1)
    {

        if(PeekMessage(&msg,hwnd))
        {
            DispatchMessage(&msg);
        }
        else
        {
            Djy_EventDelay(mS*50);
            GDD_TimerExecu(GUI_GetTickMS());
          //  TouchScreenExecu();
        }
    }
}

//*============================================================================*/




