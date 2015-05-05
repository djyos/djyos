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
//文件描述: GDD公共头文件(供GDD内核及用户使用)
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2011-11-10
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __GDD_H__
#define __GDD_H__

#if __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "gkernel.h"

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef MIN
#define MIN(a,b) (a)<(b)?(a):(b)
#endif

#ifndef MAX
#define MAX(a,b) (a)>(b)?(a):(b)
#endif

#ifndef ABS
#define ABS(a) (a)>0?(a):(-a)
#endif

#define MS2US(ms)   (ms*1000)
#define LO16(a)     (a&0xFFFF)
#define HI16(a)     ((a>>16)&0xFFFF)

#define RGB(r,g,b)  ((u8)r<<16)|((u8)g<<8)|((u8)b)

typedef bool_t  BOOL;
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;

typedef struct  tagWINDOW*  HWND;
typedef struct  tagDC*      HDC;
typedef struct 	tagFontRsc*    	HFONT;
typedef struct  tagTIMER    TIMER;

typedef struct tagPointCdn      POINT;
typedef struct tagRectangle     RECT;
typedef struct tagRectBitmap    BITMAP;

/*============================================================================*/
//// 消息数据结构
typedef struct  tagMSG
{
    HWND hwnd;      //消息所属窗口.
    u32 Code;       //消息代码.
    u32 Param1;     //消息参数1,不同消息代码,意义不同.
    ptu32_t Param2;     //消息参数2,不同消息代码,意义不同.
    const void* ExData;     //消息扩展数据

}MSG;

typedef u32 WNDPROC(MSG *pMsg);
typedef u32 *PFN_WNDPROC(MSG *pMsg);

/*============================================================================*/
//进度条Flag
#define	PBF_ORG_LEFT	(0<<0)
#define	PBF_ORG_RIGHT	(1<<0)
#define	PBF_ORG_TOP		PBF_ORG_LEFT
#define	PBF_ORG_BOTTOM	PBF_ORG_RIGHT
#define	PBF_SHOWTEXT	(1<<1)

//进度条数据结构
typedef struct
{
	u32 Flag;
	u32 Range;
	u32 Pos;
	u32 FGColor;
	u32 BGColor;
	u32 TextColor;
	u32 DrawTextFlag;

}PROGRESSBAR_DATA;

/*============================================================================*/
//// DrawText flag
#define DT_VCENTER      (0<<0)  //正文垂直居中.
#define DT_TOP          (1<<0)  //正文顶端对齐.
#define DT_BOTTOM       (2<<0)  //正文底部对齐.
#define DT_ALIGN_V_MASK (3<<0)

#define DT_CENTER       (0<<2)  //使正文在矩形中水平居中.
#define DT_LEFT         (1<<2)  //正文左对齐.
#define DT_RIGHT        (2<<2)  //正文右对齐.
#define DT_ALIGN_H_MASK (3<<2)

#define DT_BORDER       (1<<4)  //绘制边框(使用DrawColor)
#define DT_BKGND        (1<<5)  //填充背景(使用FillColor)

//// 渐变填充模式
#define GFILL_L_R   0   //丛左到右填充
#define GFILL_U_D   1   //丛上到下
#define GFILL_LU_RD 2   //丛左上到右下
#define GFILL_RU_LD 3   //丛右上到左下

//// 系统保留的窗口类
u32 button_proc(MSG *pMsg);
u32 checkbox_proc(MSG *pMsg);
u32 progressbar_proc(MSG *pMsg);
u32 listbox_proc(MSG *pMsg);

#define BUTTON      (WNDPROC*)(button_proc) //按钮
#define RADIOBOX    (WNDPROC*)(0x0002)  //单选框
#define CHECKBOX    (WNDPROC*)(checkbox_proc)   //复选框
#define GROUPBOX    (WNDPROC*)(0x0004)  //组合框
#define SLIDER      (WNDPROC*)(0x0005)  //滑动器
#define SCROLL      (WNDPROC*)(0x0006)  //卷轴
#define PROGRESSBAR (WNDPROC*)(progressbar_proc) //进度条
#define	LISTBOX		(WNDPROC*)(listbox_proc)	//列表框

//// 鼠标按键状态
#define MK_RBUTTON  (1<<0)
#define MK_MBUTTON  (1<<1)
#define MK_LBUTTON  (1<<2)

//// 键盘事件类型
typedef enum{
	KEY_EVENT_DOWN =1,	//按键按下.
	KEY_EVENT_UP   =2,	//按键弹起.
}EN_GDD_KEY_EVENT;

/*============================================================================*/
//// 通用窗口消息定义
#define MSG_CREATE              0x0002  //窗口创建消息; Param1:由WindowCreate的pdata传入;Param2:忽略.
#define MSG_NCPAINT             0x0003  //窗口非客户区绘制; Param1:忽略; Param2:忽略.
#define MSG_PAINT               0x0004  //窗口客户区绘制; Param1:忽略; Param2:忽略.
#define MSG_TIMER               0x0005  //定时器消息: Param1:定时Id; Param2:定时器对象.
#define MSG_SETTEXT             0x0006  //设置窗口文字
#define MSG_GETTEXT             0x0007  //获得窗口文字
#define MSG_GETTEXTFLENGTH      0x0008  //获得窗口文字长度
#define MSG_CLOSE               0x0009  //窗口关闭消息
#define MSG_DESTROY             0x000A  //窗口销毁消息
#define MSG_QUIT                0x000B  //窗口退出消息
#define MSG_NOTIFY              0x000C  //通知消息; Param1:控件Id(L16),通知码(H16); Param2:控件HWND.
#define	MSG_SETFOCUS			0x000D	//窗口获得焦点; Param1:忽略; Param2:忽略;
#define	MSG_KILLFOCUS			0x000E	//窗口失去焦点; Param1:忽略; Param2:忽略;
#define MSG_LBUTTON_DOWN        0x0100  //鼠标左键按下; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_LBUTTON_UP          0x0101  //鼠标左键弹起; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_RBUTTON_DOWN        0x0102  //鼠标右键按下; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_RBUTTON_UP          0x0103  //鼠标右键弹起; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_MBUTTON_DOWN        0x0104  //鼠标中键按下; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_MBUTTON_UP          0x0105  //鼠标中键弹起; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_MOUSE_MOVE          0x0106  //鼠标移动; Param1:鼠标按键状态; Param2:x坐标(L16),y坐标(H16).
#define MSG_NCLBUTTON_DOWN      0x0107  //非客户区鼠标左键按下; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_NCLBUTTON_UP        0x0108  //非客户区鼠标左键弹起; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_NCRBUTTON_DOWN      0x0109  //非客户区鼠标右键按下; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_NCRBUTTON_UP        0x010A  //非客户区鼠标右键弹起; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_NCMBUTTON_DOWN      0x010B  //非客户区鼠标中键按下; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_NCMBUTTON_UP        0x010C  //非客户区鼠标中键弹起; Param1:忽略; Param2:x坐标(L16),y坐标(H16).
#define MSG_NCMOUSE_MOVE        0x010D  //非客户区鼠标移动; Param1:鼠标按键状态; Param2:x坐标(L16),y坐标(H16).

#define MSG_KEY_DOWN            0x0120  //键盘按下; Param1:按键值(L16); Param2:事件产生的时间(毫秒单位).
#define MSG_KEY_UP              0x0121  //键盘弹起; Param1:按键值(L16); Param2:事件产生的时间(毫秒单位).

//#define   MSG_GET_POS         0xF000

//// 进度条消息
#define	PBM_SETDATA		0x2100
#define	PBM_GETDATA		0x2101
#define	PBM_SETRANGE	0x2102	//设置量程: Param1:量程值; Param2:忽略; 返回:忽略.
#define	PBM_GETRANGE	0x2103	//获得量程: Param1:忽略; Parame2:忽略; 返回:量程值.
#define	PBM_SETPOS		0x2104	//设置当前位置: Param1:当前位置; Param2:忽略; 返回:忽略.
#define	PBM_GETPOS		0x2105	//获得当前位置: Param1:忽略; Param1:忽略; 返回:当前位置.

//// ListBox消息.
#define	LBM_ADDSTRING		0x2200	//增加一个字符项; Param1:项目索引; Param2:字符指针; 返回:实际项目索引.
#define	LBM_DELSTRING		0x2201	//删除一个字符项; Param1:项目索引; Param2:忽略; 返回:忽略.
#define	LBM_SETCURSEL		0x2202	//设置当前选择项; Param1:项目索引; Param2:忽略; 返回:忽略.
#define	LBM_GETCURSEL		0x2203	//获得当前选择项; Param1:忽略; Param2:忽略; 返回:当前选择项索引.
#define	LBM_SETTOPINDEX		0x2204	//设置顶部首个可见项; Param1:项目索引; Param2:忽略; 返回:忽略.
#define	LBM_GETTOPINDEX		0x2205	//获得顶部首个可见项; Param1:忽略; Param2:忽略; 返回:顶部首个可见项索引.
#define	LBM_GETCOUNT		0x2206	//获得整个列表框的项目数量; Param1:忽略; Param2:忽略; 返回:项目数量.
#define	LBM_RESETCONTENT	0x2207	//删除列表框所有项目; Param1:忽略; Param2:忽略; 返回:忽略.
#define LBM_GETTEXTLEN      0x2208	//获得指定项目的文字字节数; Param1:项目索引; Param2:忽略; 返回:忽略.
#define LBM_GETTEXT         0x2209	//获得指定项目的文字; Param1:项目索引; Param2:输出缓冲区; 返回:忽略.
#define	LBM_SETITEMHEIGHT	0x220A	//设置列表框项目高度; Param1:高度值(像素单位); Param2:忽略; 返回:忽略.
#define	LBM_GETITEMHEIGHT	0x220B	//获得列表框项目高度; Param1:忽略; Param2:忽略; 返回:高度值(像素单位).
#define	LBM_SETITEMDATA		0x220C	//设置列表框项目数据; Param1:项目索引; Param2:项目数值; 返回:忽略.
#define	LBM_GETITEMDATA		0x220D	//获得列表框项目数据; Param1:项目索引; Param2:忽略; 返回:项目数值.

/*============================================================================*/

//Button控件通知码
#define BTN_DOWN    1    //按下
#define BTN_UP      2    //弹起

//Checkbox控件通知码
#define CBN_SELECTED    1    //被选中
#define CBN_UNSELECTED  2    //未被选中

//Listbox控件通知码
#define	LBN_SELCHANGE	1	//当前选择项目补改变

/*============================================================================*/
//// 窗口公共风格
#define WS_CHILD    (1<<16) //子窗口标志,控件窗口必须指定该标志.
#define WS_VISIBLE  (1<<17) //窗口是否可见
#define WS_DISABLE  (1<<18) //窗口是否为禁止状态,如果指定该标志,窗口将不响应输入消息
#define WS_BORDER   (1<<19) //窗口是否有边框
#define WS_DLGFRAME (1<<20) //窗口是否有对话边框
#define WS_CAPTION  (1<<21) //窗口是否有标题栏
#define WS_SYSMENU  (1<<22) //指定该标志时,标题栏将出现"关闭"按钮,窗口必须指定了 WS_CAPTION标志时,才有效.

#define WS_MAIN_WINDOW  (WS_BORDER|WS_DLGFRAME|WS_CAPTION|WS_SYSMENU)

/*============================================================================*/
//// 控件私有风格

// 按钮风格
#define BS_TYPE_MASK        (3<<0)  //类型掩码
#define BS_NORMAL           (0<<0)  //常规按钮
#define BS_HOLD             (1<<0)  //自锁按钮
#define BS_RADIO            (2<<0)  //单选按钮

#define BS_SURFACE_MASK     (3<<2)  //外观掩码
#define BS_NICE             (0<<2)  //美观风格
#define BS_SIMPLE           (1<<2)  //简朴风格
#define BS_FLAT             (2<<2)  //平面风格

#define BS_PUSHED           (1<<4)  //按钮按下

// 复选框风格
#define CBS_SELECTED        (1<<1)  //复选框选中

// 进度条风格
#define	PBS_HOR		(0<<0)	//水平进度条
#define	PBS_VER		(1<<0)	//垂直进度条
#define	PBS_FLAT 	(1<<1)	//平面风格

// 列表框风格
#define	LBS_FLAT	(1<<0)	//平面风格

/*============================================================================*/
//// GetWindow nCmd参数
#define GW_CHILD        0   //返回Z序顶端的子窗口.
#define GW_HWNDPREV     1   //返回Z序上一个同级窗口.
#define GW_HWNDNEXT     2   //返回Z序下一个同级窗口.
#define GW_HWNDFIRST    3   //返回Z序最高端同级窗口.
#define GW_HWNDLAST     4   //返回z序最低端同级窗口.

//// 定时器标记
#define TMR_START       (1<<0)
#define TMR_SINGLE      (1<<1)

//// 进度条模式
typedef enum{
    PBM_LEFT = 0,   //起点在左边
    PBM_RIGHT = 1,  //起点在右边
    PBM_TOP = 2,    //起点在上边
    PBM_BOTTOM = 3, //起点在底边

}EN_PB_MODE;

/*============================================================================*/


HDC     CreateDC(struct tagGkWinRsc *gk_win,const RECT *prc);
BOOL    DeleteDC(HDC hdc);

u32     SetRopCode(HDC hdc,u32 rop_code);
u32     GetRopCode(HDC hdc);
void    MoveTo(HDC hdc,int x,int y,POINT *old_pt);

u32 SetDrawColor(HDC hdc,u32 color);
u32 GetDrawColor(HDC hdc);
u32 SetFillColor(HDC hdc,u32 color);
u32 GetFillColor(HDC hdc);
u32 SetTextColor(HDC hdc,u32 color);
u32 GetTextColor(HDC hdc);
u32 SetSyncTime(HDC hdc,u32 sync_time);
u32 GetSyncTime(HDC hdc);

HFONT   SetFont(HDC hdc,HFONT hFont);
HFONT   GetFont(HDC hdc);


void    SetPixel(HDC hdc,int x,int y,u32 color);
void    DrawLine(HDC hdc,int x0,int y0,int x1,int y1);
void    DrawLineTo(HDC hdc,int x,int y);
BOOL    TextOut(HDC hdc,int x,int y,const char *text,int count);
BOOL    DrawText(HDC hdc,const char *text,int count,const RECT *prc,u32 flag);
void    DrawRect(HDC hdc,const RECT *prc);;
void    FillRect(HDC hdc,const RECT *prc);
void    GradientFillRect(HDC hdc,const RECT *prc,u32 Color1,u32 Color2,u32 mode);

void    DrawCircle(HDC hdc,int cx,int cy,int r);
void    FillCircle(HDC hdc,int cx,int cy,int r);
void    DrawPolyLine(HDC hdc,const POINT *pt,int count);
void    DrawGroupBox(HDC hdc,const RECT *prc,const char *Text);

u32     DispatchMessage(MSG *pMsg);
u32     SendMessage(HWND hwnd,u32 msg,u32 param1,ptu32_t param2);
BOOL    PostMessage(HWND hwnd,u32 msg,u32 param1,ptu32_t param2);
BOOL    PostQuitMessage(HWND hwnd,u32 exit_code);
BOOL    PeekMessage(MSG *pMsg,HWND hwnd);
BOOL    GetMessage(MSG *pMsg,HWND hwnd);

BOOL    ScreenToClient(HWND hwnd,POINT *pt,s32 count);
BOOL    ClientToScreen(HWND hwnd,POINT *pt,s32 count);
BOOL    ScreenToWindow(HWND hwnd,POINT *pt,s32 count);
BOOL    WindowToScreen(HWND hwnd,POINT *pt,s32 count);

HWND    GetDesktopWindow(void);
BOOL    GetWindowRect(HWND hwnd,RECT *prc);
BOOL    GetClientRect(HWND hwnd,RECT *prc);
BOOL    GetClientRectToScreen(HWND hwnd,RECT *prc);
u32		GetWindowStyle(HWND hwnd);
void* 	GetWindowPrivateData(HWND hwnd);
void 	SetWindowPrivateData(HWND hwnd,void *data);
void* 	GetWindowUserData(HWND hwnd);
void 	SetWindowUserData(HWND hwnd,void *data);

HDC     GetWindowDC(HWND hwnd);
HDC     GetDC(HWND hwnd);
BOOL    ReleaseDC(HWND hwnd,HDC hdc);
HDC     BeginPaint(HWND hwnd);
BOOL    EndPaint(HWND hwnd,HDC hdc);

HWND    CreateWindow(WNDPROC *pfWinProc,const char *Text,u32 Style,s32 x,s32 y,s32 w,s32 h,HWND Parent,u32 WinId,const void *pdata);
void    DestroyWindow(HWND hwnd);

BOOL    MoveWindow(HWND hwnd,int x,int y);
BOOL    OffsetWindow(HWND hwnd,int dx,int dy);
BOOL    IsWindowVisible(HWND hwnd);
BOOL    InvalidateWindow(HWND hwnd);
BOOL    ShowWindow(HWND hwnd,BOOL bShow);
BOOL    EnableWindow(HWND hwnd,BOOL bEnable);
HWND    GetParent(HWND hwnd);
HWND    GetWindow(HWND hwnd,int nCmd);
HWND    GetDlgItem(HWND hwnd,int id);
void	SetWindowText(HWND hwnd,const char *text,s32 max_len);
char* 	GetWindowText(HWND hwnd,char *buf,s32 max_len);

HWND    GetWindowFromPoint(POINT *pt);
HWND	SetFocusWindow(HWND hwnd);
HWND	GetFocusWindow(void);
BOOL	IsFocusWindow(HWND hwnd);

u32     DefWindowProc(MSG *pMsg);

TIMER*  GDD_CreateTimer(HWND hwnd,u16 Id,u32 IntervalMS,u16 Flag);
TIMER*  GDD_FindTimer(HWND hwnd,u16 Id);
BOOL    GDD_ResetTimer(TIMER *ptmr,u32 IntervalMS,u32 Flag);
BOOL    GDD_DeleteTimer(TIMER *ptmr);

int     RectW(const RECT *prc);
int     RectH(const RECT *prc);

void    SetRect(RECT *prc,int x,int y,int w,int h);
void    SetRectEmpty(RECT *prc);
BOOL    CopyRect(RECT *dst,const RECT *src);
BOOL    IsRectEmpty(const RECT *prc);
BOOL    OffsetRect(RECT *prc,int dx,int dy);
BOOL    InflateRect(RECT *prc,int dx,int dy);
BOOL    InflateRectEx(RECT *prc,int l,int t,int r,int b);
BOOL    PtInRect(const RECT *prc,const POINT *pt);
BOOL    MakeProgressRect(RECT *dst,const RECT *src,u32 Range,u32 Val,EN_PB_MODE mode);


/*========================================================================================*/
void    ModuleInstall_GDD(struct tagGkWinRsc *desktop,const char *InputDevName[]);
void    GDD_Execu(struct tagGkWinRsc *desktop);

void    MouseInput(s32 x,s32 y,u32 key_state);
void	KeyboardInput(u16 key_val,EN_GDD_KEY_EVENT key_event,u32 time);
u32     AlphaBlendColor(u32 bk_c,u32 fr_c,u8 alpha);
void    UpdateDisplay(void);

/*========================================================================================*/


#if __cplusplus
}
#endif
#endif  /*__GDD_H__*/
