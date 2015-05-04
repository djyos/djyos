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
//文件描述: 事件输入管理层
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
#define VK_LBUTTON  1
#define VK_RBUTTON  2
#define VK_MBUTTON  3

#define VK_NUM  256

static  u8 key_map[VK_NUM];

#define KEY_STATE_UP    (0<<0)
#define KEY_STATE_DOWN  (1<<0)

/*============================================================================*/

static BOOL _IsKeyDown(int vkey)
{
    if(key_map[vkey]&KEY_STATE_DOWN)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void _SetKeyDown(int vkey)
{
    key_map[vkey] |=  KEY_STATE_DOWN;
}

static void _SetKeyUp(int vkey)
{
    key_map[vkey] &= ~KEY_STATE_DOWN;
}

/*============================================================================*/
#if 0
#define KEY_STATE_UP    0
#define KEY_STATE_DOWN  1

void    GDD_KeyboardInput(u8 vkey,u8 statte)
{
    if(vkey<VK_NUM)
    {

    }
}
#endif


void    _SetKeyState(u8 vkey,u8 key_state)
{
    key_map[vkey] = key_state;
}

/*============================================================================*/

u8  GetMouseKeyState(void)
{
    u8 key_state=0;

    if(_IsKeyDown(VK_LBUTTON))
    {
        key_state |= MK_LBUTTON;
    }

    if(_IsKeyDown(VK_RBUTTON))
    {
        key_state |= MK_RBUTTON;
    }

    if(_IsKeyDown(VK_MBUTTON))
    {
        key_state |= MK_MBUTTON;
    }

    return key_state;
}

/*============================================================================*/

static  int cursor_x,cursor_y;
static  HWND HWND_LButton_Down=NULL;
static  int LButton_Msg=0;

void    MouseInput(s32 x,s32 y,u32 key_state)
{
    HWND hwnd;
    POINT pt;
    RECT rc;

    pt.x =x;
    pt.y =y;

    GDD_Lock();
    hwnd =GetWindowFromPoint(&pt); //记录LButtonDown窗口.

    // LBUTTON
    if(key_state&MK_LBUTTON)
    {
        if(!_IsKeyDown(VK_LBUTTON))
        {
            _SetKeyDown(VK_LBUTTON);

            HWND_LButton_Down =hwnd;    //记录LButtonDown窗口.
            GetClientRectToScreen(hwnd,&rc);
            if(_PtInRect(&rc,&pt))
            {
                LButton_Msg =MSG_LBUTTON_DOWN;
                _ScreenToClient(hwnd,&pt,1);    //客户区使用客户坐标.
            }
            else
            {
                LButton_Msg =MSG_NCLBUTTON_DOWN;
            }

            PostMessage(hwnd,LButton_Msg,0,(pt.y<<16)|(pt.x));
        }
    }
    else
    {
        if(_IsKeyDown(VK_LBUTTON))
        {
            _SetKeyUp(VK_LBUTTON);
            hwnd =HWND_LButton_Down;
            if(LButton_Msg==MSG_LBUTTON_DOWN)
            {
                _ScreenToClient(hwnd,&pt,1);    //客户区使用客户坐标.
                LButton_Msg =MSG_LBUTTON_UP;
            }
            else
            {
                LButton_Msg =MSG_NCLBUTTON_UP;
            }
            PostMessage(hwnd,LButton_Msg,0,(pt.y<<16)|(pt.x));
        }
    }



    if((cursor_x!=x) || (cursor_y!=y))
    {
        cursor_x =x;
        cursor_y =y;
        pt.x =x;
        pt.y =y;
        ScreenToClient(hwnd,&pt,1);
        PostMessage(hwnd,MSG_MOUSE_MOVE,key_state,(pt.y<<16)|(pt.x));
    }

    GDD_Unlock();
}

/*============================================================================*/

void	KeyboardInput(u16 key_val,EN_GDD_KEY_EVENT key_event,u32 time)
{
	HWND hwnd;
	switch(key_event)
	{
		case	KEY_EVENT_DOWN:
		{
			hwnd =GetFocusWindow();
			if(hwnd!=NULL)
			{
				PostMessage(hwnd,MSG_KEY_DOWN,key_val,time);
			}
		}
		break;
		////

		case	KEY_EVENT_UP:
		{
			hwnd =GetFocusWindow();
			if(hwnd!=NULL)
			{
				PostMessage(hwnd,MSG_KEY_UP,key_val,time);
			}
		}
		break;
		////

		default:	break;
		////

	}
}

/*============================================================================*/
