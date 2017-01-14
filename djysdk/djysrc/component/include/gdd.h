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


#define MS2US(ms)   (ms*1000)
#define LO16(a)     (a&0xFFFF)
#define HI16(a)     ((a>>16)&0xFFFF)

#define RGB(r,g,b)  ((u8)r<<16)|((u8)g<<8)|((u8)b)

typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;

typedef struct WINDOW*  HWND;
typedef struct DC*      HDC;
typedef struct FontRsc*     HFONT;

typedef struct PointCdn      POINT;
typedef struct Rectangle     RECT;
typedef struct RectBitmap    BITMAP;
typedef struct  __GIF_DECODE GIF_DECODE;

/*============================================================================*/
//// BMP相关数据结构

#pragma pack(1)

typedef struct
{
    u16 bfType;      /* 文件类型,   必须为   "BM "   (0x4D42)   */
    u32 bfSize;      /* 文件的大小(字节)   */
    u16 bfRsvd1;     /* 保留,   必须为   0   */
    u16 bfRsvd2;     /* 保留,   必须为   0   */
    u32 bfOffBits;   /* 位图阵列相对于文件头的偏移量(字节)   */

    u32 biSize;       /* size   of   BITMAPINFOHEADER   */
    u32 biWidth;      /* 位图宽度(像素)   */
    u32 biHeight;     /* 位图高度(像素)   */
    u16 biPlanes;     /* 目标设备的位平面数,   必须置为1   */
    u16 biBitCount;   /* 每个像素的位数,   1,4,8或24   */
    u32 biCompress;   /* 位图阵列的压缩方法,0=不压缩   */
    u32 biSizeImage;  /* 图像大小(字节)   */
    u32 biXPelsPerMeter;  /* 目标设备水平每米像素个数   */
    u32 biYPelsPerMeter;  /* 目标设备垂直每米像素个数   */
    u32 biColorUsed;      /* 位图实际使用的颜色表的颜色数   */
    u32 biColorImportant; /* 重要颜色索引的个数   */
    u32 biRedMask;
    u32 biGreenMask;
    u32 biBlueMask;
    u32 biAlphaMask;

}tagBMP_HEADER;

typedef struct
{
    u16 Format;      /* 颜色格式  */
    u16 Width;       /* 图像宽度(像素值) */
    u16 Height;      /* 图像高度(像素值) */
    u16 LineBytes;   /* 每行图像数据的字节数  */
}tagBMP_INFO;

#pragma pack()




/*============================================================================*/
//// 消息数据结构
struct WindowMsg
{
    HWND hwnd;      //消息所属窗口.
    u32 Code;       //消息代码.
    ptu32_t Param1;     //消息参数1,不同消息代码,意义不同.
    ptu32_t Param2;     //消息参数2,不同消息代码,意义不同.
    const void* ExData;     //消息扩展数据

};

//本表用于定义控件（窗口）的消息处理函数列表
struct MsgProcTable
{
    u32 MsgCode;
    ptu32_t (*MsgProc)(struct WindowMsg *pMsg);
};

// 窗口需要处理的消息链表，实现时注意，链表的Head节点是最先需要处理的，直到
// LinkNext == NULL才结束处理。
struct MsgTableLink
{
    struct MsgTableLink *LinkNext;
    struct MsgProcTable *myTable;
    u32 MsgNum;
};




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

//// 渐变填充模式，参见gkernel.h




//// 鼠标按键状态
#define MK_RBUTTON  (1<<0)
#define MK_MBUTTON  (1<<1)
#define MK_LBUTTON  (1<<2)

//// 键盘事件类型
typedef enum{
    KEY_EVENT_DOWN =1,  //按键按下.
    KEY_EVENT_UP   =2,  //按键弹起.
}EN_GDD_KEY_EVENT;

// 通用窗口消息定义
#define MSG_CREATE              0x0001  //窗口创建消息; Param1:由WindowCreate的pdata传入;Param2:忽略.
#define MSG_ERASEBKGND          0x0002  //窗口背景擦除; Param1:绘图上下文; Param2:忽略.
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
#define MSG_SETFOCUS            0x000D  //窗口获得焦点; Param1:忽略; Param2:忽略;
#define MSG_KILLFOCUS           0x000E  //窗口失去焦点; Param1:忽略; Param2:忽略;
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
#define MSG_SETATTR             0x010E  //窗口属性设置；

#define MSG_KEY_DOWN            0x0120  //键盘按下; 高16位(H16)WinID,Param1:按键值(L16); Param2:事件产生的时间(毫秒单位).
#define MSG_KEY_UP              0x0121  //键盘弹起; Param1:按键值(L16); Param2:事件产生的时间(毫秒单位).
#define MSG_KEY_PRESS           0x0122



#define MSG_TOUCH_DOWN          0x0130   //触摸屏触摸下触摸点
#define MSG_TOUCH_UP            0x0131   //触摸屏离开触摸点
#define MSG_TOUCH_MOVE          0x0132
#define MSG_NCTOUCH_DOWN        0x0133
#define MSG_NCTOUCH_UP          0x0134
#define MSG_NCTOUCH_MOVE        0x0135

#define MSG_TIMER_START         0x0140  //定时器消息: Param1:定时Id; Param2:定时器对象.
#define MSG_TIMER_STOP          0x0141  //定时器消息: Param1:定时Id; Param2:定时器对象.
//#define   MSG_GET_POS         0xF000

//// 进度条消息
#define MSG_ProcessBar_SETDATA     0x2100  //设置数据：Param1：进度条数量结构指针；Param2：忽略；返回：忽略.
#define MSG_ProcessBar_GETDATA     0x2101  //获得数据：Param1：进度条数据结构指针；Param2：忽略；返回：忽略.
#define MSG_ProcessBar_SETRANGE    0x2102  //设置量程: Param1:量程值; Param2:忽略; 返回:忽略.
#define MSG_ProcessBar_GETRANGE    0x2103  //获得量程: Param1:忽略; Parame2:忽略; 返回:量程值.
#define MSG_ProcessBar_SETPOS      0x2104  //设置当前位置: Param1:当前位置; Param2:忽略; 返回:忽略.
#define MSG_ProcessBar_GETPOS      0x2105  //获得当前位置: Param1:忽略; Param1:忽略; 返回:当前位置.

//// ListBox消息.
#define MSG_ListBox_ADDSTRING       0x2200  //增加一个字符项; Param1:项目索引; Param2:字符指针; 返回:实际项目索引.
#define MSG_ListBox_DELSTRING       0x2201  //删除一个字符项; Param1:项目索引; Param2:忽略; 返回:忽略.
#define MSG_ListBox_SETCURSEL       0x2202  //设置当前选择项; Param1:项目索引; Param2:忽略; 返回:忽略.
#define MSG_ListBox_GETCURSEL       0x2203  //获得当前选择项; Param1:忽略; Param2:忽略; 返回:当前选择项索引.
#define MSG_ListBox_SETTOPINDEX     0x2204  //设置顶部首个可见项; Param1:项目索引; Param2:忽略; 返回:忽略.
#define MSG_ListBox_GETTOPINDEX     0x2205  //获得顶部首个可见项; Param1:忽略; Param2:忽略; 返回:顶部首个可见项索引.
#define MSG_ListBox_GETCOUNT        0x2206  //获得整个列表框的项目数量; Param1:忽略; Param2:忽略; 返回:项目数量.
#define MSG_ListBox_RESETCONTENT    0x2207  //删除列表框所有项目; Param1:忽略; Param2:忽略; 返回:忽略.
#define MSG_ListBox_GETTEXTLEN      0x2208  //获得指定项目的文字字节数; Param1:项目索引; Param2:忽略; 返回:忽略.
#define MSG_ListBox_GETTEXT         0x2209  //获得指定项目的文字; Param1:项目索引; Param2:输出缓冲区; 返回:忽略.
#define MSG_ListBox_SETITEMHEIGHT   0x220A  //设置列表框项目高度; Param1:高度值(像素单位); Param2:忽略; 返回:忽略.
#define MSG_ListBox_GETITEMHEIGHT   0x220B  //获得列表框项目高度; Param1:忽略; Param2:忽略; 返回:高度值(像素单位).
#define MSG_ListBox_SETITEMDATA     0x220C  //设置列表框项目数据; Param1:项目索引; Param2:项目数值; 返回:忽略.
#define MSG_ListBox_GETITEMDATA     0x220D  //获得列表框项目数据; Param1:项目索引; Param2:忽略; 返回:项目数值.

#define MSG_WM_USER                 0x10000 //用户定义的消息起始代码
/*============================================================================*/
//// 控件通知码

//Button
#define MSG_BTN_DOWN    1    //按下
#define MSG_BTN_UP      2    //弹起

//ListBox
//#define LBN_SELCHANGE   1   //当前选择项目补改变

//Listview
#define LVN_CLICK   1   //被"点击"

/*============================================================================*/
//// 窗口公共风格，与控件风格以及控件类型组合成32位字，其中窗口公共风格使用高16位
//   控件风格使用剩下16位中的高8位，控件类型使用剩下16位中的低8位。
#define WS_CHILD    (1<<16) //子窗口标志,控件窗口必须指定该标志.
#define WS_VISIBLE  (1<<17) //窗口是否可见
#define WS_DISABLE  (1<<18) //窗口是否为禁止状态,如果指定该标志,窗口将不响应输入消息
#define WS_BORDER   (1<<19) //窗口是否有边框
#define WS_DLGFRAME (1<<20) //窗口是否有对话边框
#define WS_CAPTION  (1<<21) //窗口是否有标题栏
#define WS_SYSMENU  (1<<22) //指定该标志时,标题栏将出现"关闭"按钮,窗口必须指定了 WS_CAPTION标志时,才有效.

#define WS_MAIN_WINDOW  (WS_BORDER|WS_DLGFRAME|WS_CAPTION|WS_SYSMENU)

/*============================================================================*/
//// 控件私有风格，与公共风格组合成32位字，使用低16位

// 列表框风格
//#define LBS_FLAT    (1<<0)  //平面风格

/*============================================================================*/

//// 定时器标记
#define TMR_START       (1<<0)
#define TMR_SINGLE      (1<<1)

/*============================================================================*/

////定义控件类型
#define BUTTON              0x1
#define CHECKBOX            0x2
#define LABEL               0x3
#define TEXTBOX             0x4
#define RICHTEXTBOX         0x5
#define LISTBOX             0x6
#define LISTVIEW            0x7
#define PROGRESSBAR         0x8
#define KEYBOARD            0x9





#define CN_WINDOW_ZPRIO_MOUSE       (-100)
#define CN_WINDOW_ZPRIO_CURSOR      (-101)




HDC     CreateDC(struct GkWinRsc *gk_win,const RECT *prc);
bool_t    DeleteDC(HDC hdc);

struct RopGroup SetRopCode(HDC hdc,struct RopGroup RopCode);
struct RopGroup GetRopCode(HDC hdc);
void    MoveTo(HDC hdc,s32 x,s32 y,POINT *old_pt);

u32 SetDrawColor(HDC hdc,u32 color);
u32 GetDrawColor(HDC hdc);
u32 SetFillColor(HDC hdc,u32 color);
u32 GetFillColor(HDC hdc);
u32 SetTextColor(HDC hdc,u32 color);
u32 GetTextColor(HDC hdc);
u32 SetSyncTime(HDC hdc,u32 sync_time);
u32 GetSyncTime(HDC hdc);

s32  GetStrLineCount(const char *str);
void AdjustTextRect(HDC hdc,const char *text,int count, RECT *prc,u32 flag);

HFONT   SetFont(HDC hdc,HFONT hFont);
HFONT   GetFont(HDC hdc);


void    SetPixel(HDC hdc,s32 x,s32 y,u32 color);
void    DrawLine(HDC hdc,s32 x0,s32 y0,s32 x1,s32 y1);
void    DrawDottedLine(HDC hdc,s32 x0,s32 y0,s32 x1,s32 y1);
void    DrawLineEx(HDC hdc,s32 x0,s32 y0,s32 x1,s32 y1,u32 color);
void    DrawLineTo(HDC hdc,s32 x,s32 y);
bool_t    TextOut(HDC hdc,s32 x,s32 y,const char *text,s32 count);
bool_t    DrawText(HDC hdc,const char *text,s32 count,const RECT *prc,u32 flag);
void    DrawRect(HDC hdc,const RECT *prc);;
void    FillRect(HDC hdc,const RECT *prc);
void    FillRectEx(HDC hdc,const RECT *prc,u32 color);
void    GradientFillRect(HDC hdc,const RECT *prc,u32 Color1,u32 Color2,u32 mode);
void    Fill3DRect(HDC hdc,const RECT *prc,u32 Color1,u32 Color2);

void    DrawCircle(HDC hdc,s32 cx,s32 cy,s32 r);
void    FillCircle(HDC hdc,s32 cx,s32 cy,s32 r);
void    DrawEllipse(HDC hdc,s32 cx, s32 cy, s32 rx, s32 ry);
void    FillEllipse(HDC hdc,s32 cx, s32 cy, s32 rx, s32 ry);
void    DrawSector(HDC hdc, s32 xCenter, s32 yCenter, s32 radius,s32 angle1,s32 angle2);
void    FillSector(HDC hdc, s32 xCenter, s32 yCenter, s32 radius,s32 angle1,s32 angle2);
void    DrawBezier3(HDC hdc,const POINT *pt,s32 cnt);

void    DrawPolyLine(HDC hdc,const POINT *pt,s32 count);
void    DrawGroupBox(HDC hdc,const RECT *prc,const char *Text);
bool_t    DrawBitmap(HDC hdc,s32 x,s32 y,struct RectBitmap *bitmap,u32 key_color,struct RopGroup RopCode);

bool_t    GetBMPInfo(tagBMP_INFO *bm_info,tagBMP_HEADER *pBmpHdr);
bool_t    DrawBMP(HDC hdc,s32 x,s32 y,const void *bmp_data);
GIF_DECODE*    OpenGIF(const void *dat,u32 size);
u32    GetGIFWidth(GIF_DECODE *hgif);
u32    GetGIFHeight(GIF_DECODE *hgif);
u32    GetGIFFrameCount(GIF_DECODE *hgif);
s32    GetGIFFrameDelay(GIF_DECODE *hgif,u32 frame_idx);
bool_t   DrawGIFFrame(HDC hdc,s32 x,s32 y,GIF_DECODE *hgif,u32 bk_color,u32 frame_idx);
void   CloseGIF(GIF_DECODE *hgif);

u32     DispatchMessage(struct WindowMsg *pMsg);
u32     SendMessage(HWND hwnd,u32 msg,u32 param1,ptu32_t param2);
bool_t    PostMessage(HWND hwnd,u32 msg,u32 param1,ptu32_t param2);
bool_t    PostQuitMessage(HWND hwnd,u32 exit_code);
bool_t    PeekMessage(struct WindowMsg *pMsg,HWND hwnd);
bool_t    GetMessage(struct WindowMsg *pMsg,HWND hwnd);

bool_t    ScreenToClient(HWND hwnd,POINT *pt,s32 count);
bool_t    ClientToScreen(HWND hwnd,POINT *pt,s32 count);
bool_t    ScreenToWindow(HWND hwnd,POINT *pt,s32 count);
bool_t    WindowToScreen(HWND hwnd,POINT *pt,s32 count);

HWND    GetDesktopWindow(void);
bool_t    GetWindowRect(HWND hwnd,RECT *prc);
bool_t    GetClientRect(HWND hwnd,RECT *prc);
bool_t    GetClientRectToScreen(HWND hwnd,RECT *prc);
u32     GetWindowStyle(HWND hwnd);
char    *GetWindowText(HWND hwnd);
void*   GetWindowPrivateData(HWND hwnd);
void    SetWindowPrivateData(HWND hwnd,void *data);

HDC     GetWindowDC(HWND hwnd);
HDC     GetDC(HWND hwnd);
bool_t    ReleaseDC(HWND hwnd,HDC hdc);
HDC     BeginPaint(HWND hwnd);
bool_t    EndPaint(HWND hwnd,HDC hdc);
HWND    CreateWindow(const char *Text,u32 Style,
                     s32 x,s32 y,s32 w,s32 h,
                     HWND hParent,u32 WinId,
                     u32 BufProperty, void *pdata,
                     struct MsgTableLink *pUserMsgTableLink);
void    DestroyWindow(HWND hwnd);
void    DestroyAllChild(HWND hwnd);

bool_t    MoveWindow(HWND hwnd,s32 x,s32 y);
bool_t    OffsetWindow(HWND hwnd,s32 dx,s32 dy);
bool_t    IsWindowVisible(HWND hwnd);
bool_t    InvalidateWindow(HWND hwnd,bool_t bErase);
bool_t    SetWindowShow(HWND hwnd);
bool_t    SetWindowHide(HWND hwnd);
bool_t    EnableWindow(HWND hwnd,bool_t bEnable);
HWND    Gdd_GetWindowParent(HWND hwnd);
HWND    GetWindow(HWND hwnd,s32 nCmd);
HWND    GetDlgItem(HWND hwnd,u16 id);
void    SetWindowText(HWND hwnd,const char *text,s32 max_len);

HWND    GetWindowFromPoint(struct GkWinRsc *desktop, POINT *pt);
HWND    SetFocusWindow(HWND hwnd);
HWND    GetFocusWindow(void);
bool_t  IsFocusWindow(HWND hwnd);
bool_t  SetWindowShow(HWND hwnd);
bool_t SetWindowHide(HWND hwnd);



s32 GetWinMsgProc(struct WindowMsg *pMsg,struct MsgProcTable *MsgTable);

struct WinTimer*  GDD_CreateTimer(HWND hwnd,u16 Id,u32 IntervalMS,u16 Flag);
struct WinTimer*  GDD_FindTimer(HWND hwnd,u16 Id);
bool_t    GDD_ResetTimer(struct WinTimer *ptmr,u32 IntervalMS,u32 Flag);
bool_t    GDD_DeleteTimer(struct WinTimer *ptmr);

s32     RectW(const RECT *prc);
s32     RectH(const RECT *prc);

void    SetRect(RECT *prc,s32 x,s32 y,s32 w,s32 h);
void    SetRectEmpty(RECT *prc);
bool_t    CopyRect(RECT *dst,const RECT *src);
bool_t    IsRectEmpty(const RECT *prc);
bool_t    OffsetRect(RECT *prc,s32 dx,s32 dy);
bool_t    MoveRect(RECT *prc,s32 x,s32 y);
bool_t    InflateRect(RECT *prc,s32 dx,s32 dy);
bool_t    InflateRectEx(RECT *prc,s32 l,s32 t,s32 r,s32 b);
bool_t    PtInRect(const RECT *prc,const POINT *pt);



/*========================================================================================*/
void    ModuleInstall_GDD(struct GkWinRsc *desktop,const char *InputDevName[]);
void    GDD_Execu(struct GkWinRsc *desktop);

void    MouseInput(s32 x,s32 y,u32 key_state);
void    KeyboardInput(u16 key_val,EN_GDD_KEY_EVENT key_event);
u32     AlphaBlendColor(u32 bk_c,u32 fr_c,u8 alpha);
void    UpdateDisplay(u32 timeout);

/*========================================================================================*/


#if __cplusplus
}
#endif
#endif  /*__GDD_H__*/
