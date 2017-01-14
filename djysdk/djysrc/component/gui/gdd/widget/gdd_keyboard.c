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
//作者:  zhb.
//版本：V1.0.0
//文件描述: 虚拟键盘控件实现
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2016-10-08
//   作者:  zhb.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//---------------------------------

#include  "gdd.h"
#include  <gui/gdd/gdd_private.h>
#include "font.h"
#include    <gdd_widget.h>

static u8 gs_KeyBoardRowsDef=4;
static u8 gs_KeyBoardColsDef=3;
static u8 gs_KeySpaceDef=0;
static u8 gs_KeyVtimeLimitDef=10;
static char *gs_KeyValSetsDef="123456789.0Ｘ";
static u8 gs_KeyBoardRows;
static u8 gs_KeyBoardCols;
static u8 gs_KeySpace;
static char *gs_KeyValSets;
static u8 gs_KeyWidth,gs_KeyHeight;

static bool_t gs_bKeyDownFlag=false;
static char gs_CurKeyDown;
static s32 gs_KeyBoardId=0;

//-----------------------------------------------------------------------
//功能：虚拟键盘获得当前按下的键值
//参数：空
//返回：按下的键值ASCII值.
//-----------------------------------------------------------------------------
static u32 __VirKeyBoardRead(void)
{
      return (u32)gs_CurKeyDown;
}

//---------------------------------------------------------------------------
//功能：虚拟按键创建消息响应函数
//参数：pMsg，窗口消息指针
//返回：成功返回true,失败则返回false。
//-----------------------------------------------------------------------------
static bool_t VirKeyBoard_Create(struct WindowMsg *pMsg)
{
    HWND hwnd;
	VirKeyBoard *pVKB;
	struct KeyBoardPrivate *pkbp;
	char *name;
	char buf[5];
	if(pMsg==NULL)
		return false;
	hwnd=pMsg->hwnd;
	if(hwnd==NULL)
		return false;
	if(pMsg->Param1==0)
	{
	   pVKB=(VirKeyBoard *)malloc(sizeof(VirKeyBoard));
	   if(pVKB==NULL)
	       return false;
	   pkbp=(struct KeyBoardPrivate *)malloc(sizeof(struct KeyBoardPrivate));
	   if(pkbp==NULL)
		   return false;
	   pVKB->cols=gs_KeyBoardRowsDef;
	   pVKB->rows=gs_KeyBoardColsDef;
	   pVKB->keyspace=gs_KeySpaceDef;
	   pVKB->keysets=gs_KeyValSetsDef;
	   pVKB->id=gs_KeyBoardId;
	   pkbp->key_bak=0;
	   pkbp->key_now=0;
	   pkbp->vtime_count=0;
	   pkbp->vtime_limit=gs_KeyVtimeLimitDef;
	   pkbp->read_keyboard=__VirKeyBoardRead;
	   pVKB->pKBP=pkbp;
	   SetWindowPrivateData(hwnd,(void *)pVKB);
	   itoa(gs_KeyBoardId,buf,10);
	   name="vir single touch keyboard";
	   Keyboard_InstallDevice(name, pkbp);
	   gs_KeyBoardId++;
	}
	else
	{

	}
    return true;
}

//----------------------------------------------------------------------------
//功能：虚拟键盘控件的MSG_PAINT消息响应函数
//参数：pMsg，窗口消息指针
//返回：成功返回true,失败则返回false。
//-----------------------------------------------------------------------------
static  bool_t VirKeyBoard_Paint(struct WindowMsg *pMsg)
{
	HWND hwnd,keyhwnd;
	HDC hdc;
	RECT rc;
	u8 width,height,i,j,keynum,keycnt=0;
	u8 LeftStart,HighStart;
	VBK vbk;
	char keyval;
	if(pMsg==NULL)
		return false;
	hwnd=pMsg->hwnd;
	if(hwnd==NULL)
		return false;
	hdc =BeginPaint(hwnd);
	if(hdc==NULL)
		return false;
	vbk=(VBK)pMsg->Param1;
	if(vbk==NULL)
	{
		gs_KeyBoardRows=gs_KeyBoardRowsDef;
		gs_KeyBoardCols=gs_KeyBoardColsDef;
		gs_KeySpace=gs_KeySpaceDef;
		gs_KeyValSets=gs_KeyValSetsDef;
	}
	else
	{
		gs_KeyBoardRows=vbk->rows;
		gs_KeyBoardCols=vbk->cols;
		gs_KeySpace=vbk->keyspace;
		gs_KeyValSets=vbk->keysets;
	}

	GetClientRect(hwnd,&rc);
	SetFillColor(hdc,RGB(255,255,255));
	FillRect(hdc,&rc);
	width=RectW(&rc);
	height=RectH(&rc);
	gs_KeyWidth=(width-(gs_KeyBoardCols+1)*gs_KeySpace)/gs_KeyBoardCols;
	gs_KeyHeight=(height-(gs_KeyBoardRows+1)*gs_KeySpace)/gs_KeyBoardRows;
	LeftStart=gs_KeySpace;
	HighStart=gs_KeySpace;
	keynum=strlen(gs_KeyValSetsDef);
	for(i=0;i<gs_KeyBoardRows;i++)
	{
		for(j=0;j<gs_KeyBoardCols;j++)
		{

			keyval=*(gs_KeyValSetsDef+keycnt);
			keyhwnd=CreateButton(&keyval,WS_CHILD|BS_NORMAL|WS_BORDER|WS_VISIBLE|\
					BS_SIMPLE|BUTTON,LeftStart+j*gs_KeyWidth,HighStart+i*gs_KeyHeight,gs_KeyWidth,\
					gs_KeyHeight,hwnd,0x1000,&keyval,NULL);
			keycnt++;
			//检查一下是否创建成功
			if(keycnt==gs_KeyBoardRows*gs_KeyBoardCols)
			{
				if(keyhwnd==NULL)
				{
				    #ifdef DEBUG
							printf("VirKeyBoard Create Failed.\r\n");
							while(1);
					#endif
				   DestroyAllChild(hwnd);
				   DestroyWindow(hwnd);
				   EndPaint(hwnd,hdc);
				   return false;
				}
			}

		}
	}

	EndPaint(hwnd,hdc);
	return true;
}

//-----------------------------------------------------------------------------
//功能：判断某个窗口是否能接受虚拟键盘输入，只有TextBox、RICHTEXTBOX、LISTBOX、LISTVIEW
//等可以接受虚拟键盘输入.
//参数：hwnd，窗口句柄.
//返回：是则返回true,否则返回false.
//-----------------------------------------------------------------------------
//static bool_t __IsKeyBoradInputValidControl(HWND hwnd)
//{
//      u8 style;
//	  bool_t ret=false;
//	  style=(u8)hwnd->Style;     //取最低的8位
//	  switch (style)
//	  {
//	  	case BUTTON:
//		case CHECKBOX:
//		case LABEL:
//		case PROGRESSBAR:
//				ret=false;
//				break;
//		case TEXTBOX:
//		case RICHTEXTBOX:
//		case LISTBOX:
//		case LISTVIEW:
//				ret=true;
//				break;
//		default:
//				break;
//
//	  }
//	  return ret;
//}

//-----------------------------------------------------------------
//功能：虚拟键盘控件MSG_NOTIFY消息响应函数
 //参数：pMsg，窗口消息指针。
 //返回：成功返回true,失败则返回false。
//-----------------------------------------------------------------------------
static bool_t VirKeyBoard_NotifyHandle(struct WindowMsg *pMsg)
{
    HWND hwnd,keyhwnd;
	u16 event,id;
	u32 time;
	char *keyval;
	if(pMsg==NULL)
		 return false;
	hwnd =pMsg->hwnd;
	if(hwnd==NULL)
		 return false;
	event=HI16(pMsg->Param1);
	keyhwnd=(HWND)pMsg->Param2;
    if(keyhwnd==NULL)
    	return false;
    keyval=GetWindowPrivateData(keyhwnd);
	if(event==MSG_BTN_UP)
	{
		if(gs_bKeyDownFlag)
		{
			gs_CurKeyDown=*keyval;
			PostMessage( hwnd, MSG_KEY_UP, (u32) keyval, time);
		}

	}
	else if(event==MSG_BTN_DOWN)
	{
		gs_bKeyDownFlag=true;
	 	PostMessage( hwnd, MSG_KEY_DOWN, (u32) keyval, time);
	}
	else
	{

	}
	 return true;
}



//默认虚拟键盘消息处理函数表，处理用户函数表中没有处理的消息。
static struct MsgProcTable s_gVirKeyBoardMsgProcTable[] =
{
    {MSG_CREATE,VirKeyBoard_Create},
    {MSG_PAINT,VirKeyBoard_Paint},
    {MSG_NOTIFY,VirKeyBoard_NotifyHandle}
};

static struct MsgTableLink  s_gVirKeyBoardMsgLink;
// =============================================================================
// 函数功能: 虚拟键盘控件创建函数。
// 输入参数: Text:虚拟键盘窗口Text;
//           Style:虚拟键盘风格，参见gdd.h;
//           x:虚拟键盘起始位置x方向坐标(单位：像素);
//           y:虚拟键盘起始位置y方向坐标(单位：像素);
//           w:虚拟键盘宽度(单位：像素);
//           h:虚拟键盘高度(单位：像素);
//           hParent:虚拟键盘父窗口句柄;
//           WinId:虚拟键盘控件Id;
//           pdata:虚拟键盘控件私有数据结构;
//           UserMsgTableLink:虚拟键盘控件用户消息列表结构指针。
// 输出参数: 无。
// 返回值  :成功则返回文本框句柄，失败则返回NULL。
// =============================================================================
HWND CreateVirKeyBoard(const char *Text,u32 Style,
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
        Current->LinkNext = &s_gVirKeyBoardMsgLink;
        Current = UserMsgTableLink;
    }
    else
        Current = &s_gVirKeyBoardMsgLink;
    s_gVirKeyBoardMsgLink.LinkNext = NULL;
    s_gVirKeyBoardMsgLink.MsgNum = sizeof(s_gVirKeyBoardMsgProcTable) / sizeof(struct MsgProcTable);
    s_gVirKeyBoardMsgLink.myTable = (struct MsgProcTable *)&s_gVirKeyBoardMsgProcTable;
    pGddWin=CreateWindow(Text,WS_CHILD|Style,x,y,w,h,hParent,WinId,CN_WINBUF_PARENT,pdata,Current);
    return pGddWin;
}
