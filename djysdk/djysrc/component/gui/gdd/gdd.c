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
//文件描述: 公共代码集
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-11-10
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include <gui\gkernel\gk_display.h>
#include    <gui/gdd/gdd_private.h>
#include    <cfg/gui_config.h>
#include "list.h"
/*============================================================================*/
HWND g_CursorHwnd=NULL;         //光标窗口

u32 GUI_GetTickMS(void)
{
    return ((u32)DjyGetSysTime( )/1000);
}


static  struct MutexLCB *gdd_mutex_lock=NULL;

bool_t    GDD_Lock(void)
{
        Lock_MutexPend(gdd_mutex_lock,5000*mS);
        return TRUE;
}

void    GDD_Unlock(void)
{
        Lock_MutexPost(gdd_mutex_lock);
}

void    GDD_WindowInit(void);
void    GDD_TimerInit(void);

/*============================================================================*/
void    GDD_Init(void)
{
    gdd_mutex_lock =Lock_MutexCreate(NULL);
    if(gdd_mutex_lock!=NULL)
    {
        GDD_WindowInit();
        GDD_TimerInit();
    }
}

/*============================================================================*/

static  const char **GDD_InputDevName;

static HWND sg_pMouseHwnd;
static  ptu32_t gdd_input_thread(void)
{
    HWND hwnd;
    tpInputMsgQ pMsgQ;
    struct InputDeviceMsg msg;
    s32 i;

    pMsgQ=HmiIn_CreatInputMsgQ(20,"input_msg");

    if(pMsgQ!=NULL)
    {
        if(GDD_InputDevName!=NULL)
        {
            i=0;
            while(1)
            {
                if(GDD_InputDevName[i] == NULL)
                {
                    break;
                }

                HmiIn_SetFocus((char*)GDD_InputDevName[i],pMsgQ);
                i++;
            }
        }

        while(1)
        {
            if(HmiIn_ReadMsg(pMsgQ, &msg, CN_TIMEOUT_FOREVER) != false)
            {
                //单点触摸屏消息，模拟成鼠标消息
                if(msg.input_type == EN_HMIIN_SINGLE_TOUCH)
                {
                    struct SingleTouchMsg *TouchMsg;
                    static s32 z=0;
                    s32 Touch_Msg;
                    bool_t NC;
                    POINT pt;
                    RECT rc;
                    TouchMsg = &msg.input_data.SingleTouchMsg;
                    pt.x = TouchMsg->x;
                    pt.y = TouchMsg->y;
                    GDD_Lock();
                    //TouchDown所在窗口.
                    if(TouchMsg->display != NULL)
                        hwnd = GetWindowFromPoint(TouchMsg->display->desktop, &pt);
                    else
                        hwnd = GetWindowFromPoint(GetDesktopWindow()->pGkWin, &pt);
                    //MoveWindow(sg_pMouseHwnd, pt.x-4, pt.y-4);
                    UpdateDisplay(CN_TIMEOUT_FOREVER);
                    if(hwnd != NULL)
                    {
                        GetClientRectToScreen(hwnd,&rc);
                        if(PtInRect(&rc, &pt))
                            NC = false;
                        else
                            NC = true;

                        if(z == TouchMsg->z)    //相等，必然是按下并拖动
                        {
                            if(NC )
                                Touch_Msg = MSG_TOUCH_MOVE;
                            else
                                Touch_Msg = MSG_NCTOUCH_MOVE;
                            PostMessage(hwnd, Touch_Msg, 0, (pt.y << 16) | pt.x);
                        }
                        else
                        {
                            z = TouchMsg->z;
                            if(z>0)     //touch按下，模拟成鼠标左键按下
                            {
                                if(NC)
                                    Touch_Msg = MSG_NCTOUCH_DOWN;
                                else
                                    Touch_Msg = MSG_TOUCH_DOWN;
                            }
                            else        //touch离开，模拟成鼠标左键松开
                            {
                                if(NC)
                                    Touch_Msg = MSG_NCTOUCH_UP;
                                else
                                    Touch_Msg = MSG_TOUCH_UP;
                            }
                            PostMessage(hwnd, Touch_Msg, 0, (pt.y << 16) | pt.x);
                        }
                    }
                    GDD_Unlock();
                }

                if(msg.input_type == EN_HMIIN_KEYBOARD)
                {
                    u8* key;
                    u8 val,event;
                    u32 KeyTime;

                    hwnd = GetFocusWindow();
                    if(NULL != hwnd)
                    {
                        key  = msg.input_data.key_board.key_value;
                        KeyTime = msg.input_data.key_board.time;

                        val  =key[0];
                        event =key[1];

                        if(event==0x00)
                        {
                            PostMessage(hwnd, MSG_KEY_DOWN, val, KeyTime);
                        }

                        if(event==0xF0)
                        {
                            PostMessage(hwnd, MSG_KEY_UP, val, KeyTime);
                        }
                    }
                }


            }
        }

        HmiIn_DeleteInputMsgQ(pMsgQ);
    }
    return 0;
}

//----GDD主函数-----------------------------------------------------------------
//描述: GDD服务执行函数,该函数不会返回.
//参数：无
//返回：无
//------------------------------------------------------------------------------
void    GDD_TimerExecu(u32 tick_time_ms);
static  ptu32_t gdd_server_thread(void)
{
    HWND hwnd;
    struct WindowMsg msg;
    hwnd = GetDesktopWindow( );
    UpdateDisplay(CN_TIMEOUT_FOREVER);

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
    return 0;
}
//----鼠标绘制函数-------------------------------------------------------------
//功能：这是按钮控件的MSG_PAINT消息响应函数
//参数：pMsg，消息指针
//返回：固定true
//-----------------------------------------------------------------------------
static  bool_t MousePaint(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT rc;

    hwnd=pMsg->hwnd;
    hdc =BeginPaint(hwnd);
    if(NULL!=hdc)
    {
        GetClientRect(hwnd,&rc);
        SetFillColor(hdc,RGB(255,0,0));
        FillRect(hdc,&rc);

        SetDrawColor(hdc,RGB(1,1,1));
        DrawLine(hdc,0,4,8,4);
        DrawLine(hdc,4,0,4,8);

        EndPaint(hwnd,hdc);
    }
    return true;
}

//鼠标消息处理函数表，处理用户函数表中没有处理的消息。
static struct MsgProcTable s_gMouseMsgProcTable[] =
{
    {MSG_CREATE,MousePaint}
};

static struct MsgTableLink  s_gMouseMsgLink;


//----光标绘制函数-------------------------------------------------------------
//功能：这是按钮控件的MSG_PAINT消息响应函数
//参数：pMsg，消息指针
//返回：固定true
//-----------------------------------------------------------------------------
static bool_t Cursor_Create(struct WindowMsg *pMsg)
{
    HWND hwnd;
    s32 x,y;
    RECT rc;
    HDC hdc;
    if(pMsg==NULL)
       return false;
    hwnd=pMsg->hwnd;
    if(hwnd==NULL)
         return false;
    hdc =BeginPaint(hwnd);
    if(NULL!=hdc)
    {
        GetClientRect(hwnd,&rc);
        x=RectW(&rc);
        y=RectH(&rc);
        SetFillColor(hdc,RGB(255,0,0));
        FillRect(hdc,&rc);
        SetDrawColor(hdc,RGB(1,1,1));
        DrawLine(hdc,x/2,0,x/2,y);
        EndPaint(hwnd,hdc);
    }
    return true;
}

//光标消息处理函数链表
static struct MsgProcTable s_gCursorMsgProcTable[]=
{
    {MSG_CREATE,Cursor_Create}
};

static struct MsgTableLink  s_gCursorMsgLink;
//----GDD主函数-----------------------------------------------------------------
//描述: GDD服务执行函数,该函数不会返回.
//参数：无
//返回：无
//------------------------------------------------------------------------------
void    ModuleInstall_GDD(struct GkWinRsc *desktop,const char *InputDevName[])
{
    u16 evtt;
    HWND hwnd;
    struct WinTimer *pTmr;
    struct RopGroup RopCode;
    GDD_Init();
    GDD_InputDevName =InputDevName;
    InitGddDesktop(desktop);
    if(gc_bShowMouse == true)
    {
        s_gMouseMsgLink.LinkNext = NULL;
        s_gMouseMsgLink.MsgNum = sizeof(s_gMouseMsgProcTable) / sizeof(struct MsgProcTable);
        s_gMouseMsgLink.myTable = &s_gMouseMsgProcTable;
        sg_pMouseHwnd = CreateWindow("Mouse_Cursor", 0, desktop->right / 2,
                                      desktop->bottom / 2, 8, 8, NULL, 0,
                                      CN_WINBUF_BUF, NULL, &s_gMouseMsgLink);
        if(sg_pMouseHwnd!=NULL)
        {
           GK_ApiSetPrio(sg_pMouseHwnd->pGkWin,CN_WINDOW_ZPRIO_MOUSE , CN_TIMEOUT_FOREVER);
//         GK_ApiSetHyalineColor(sg_pMouseHwnd->pGkWin,RGB(0, 0, 0));
           RopCode = (struct RopGroup){ 0, 0, 0, CN_R2_XORPEN, 0, 0, 0  };
           GK_ApiSetRopCode(sg_pMouseHwnd->pGkWin, RopCode, CN_TIMEOUT_FOREVER);
        }
    }

    s_gCursorMsgLink.LinkNext = NULL;
    s_gCursorMsgLink.MsgNum = sizeof(s_gCursorMsgProcTable) / sizeof(struct MsgProcTable);
    s_gCursorMsgLink.myTable = (struct MsgProcTable *)&s_gCursorMsgProcTable;
    g_CursorHwnd=CreateWindow("Cursor",0,desktop->right /3,desktop->bottom/3,2\
             , 12,NULL, 0, CN_WINBUF_BUF,NULL,&s_gCursorMsgLink);
    if(g_CursorHwnd!=NULL)
    {
         GK_ApiSetPrio(g_CursorHwnd->pGkWin,CN_WINDOW_ZPRIO_CURSOR , CN_TIMEOUT_FOREVER);
         RopCode = (struct RopGroup){ 0, 0, 0, CN_R2_XORPEN, 0, 0, 0  };
         GK_ApiSetRopCode(g_CursorHwnd->pGkWin, RopCode, CN_TIMEOUT_FOREVER);
         //在桌面窗口创建一个定时周期为800ms的定时器
         hwnd = GetDesktopWindow( );
         if(hwnd==NULL)
         {
           #ifdef DEBUG
               printf("Cursor Create Failed.\r\n");
               while(1);
           #endif
               DestroyWindow(g_CursorHwnd);
         }

         pTmr=GDD_CreateTimer(hwnd,CN_CURSOR_TIMER_ID,CN_INTERVAL_TIME,0);
         if(pTmr==NULL)
         {
             DestroyWindow(g_CursorHwnd);
             return ;
         }
         SetWindowHide(g_CursorHwnd);
     }
    ////gdd_server
    evtt = Djy_EvttRegist(  EN_CORRELATIVE, CN_PRIO_RRS, 0, 0,
                           gdd_server_thread, NULL,2048,"gdd server");
    if (evtt != CN_EVTT_ID_INVALID)
    {
        Djy_EventPop(evtt, NULL, 0, 0, 0, 0);
    }

    ////gdd_input
    evtt = Djy_EvttRegist(  EN_CORRELATIVE, CN_PRIO_RRS, 0, 0,
                          gdd_input_thread, NULL,2048,"gdd input");
    if (evtt != CN_EVTT_ID_INVALID)
    {
        Djy_EventPop(evtt, NULL, 0, 0, 0, 0);
    }

}
/*============================================================================*/
