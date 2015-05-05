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
//文件描述: 按钮控件实现
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-11-10
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include    "gdd.h"
#include    "../include/gdd_private.h"

/*============================================================================*/

static  int _get_button_type(HWND hwnd)
{
    return hwnd->Style&BS_TYPE_MASK;
}

static  void button_paint(MSG *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT rc;

    hwnd=pMsg->hwnd;
    hdc =BeginPaint(hwnd);
    if(NULL!=hdc)
    {
        GetClientRect(hwnd,&rc);


        if(hwnd->Style&BS_PUSHED)
        {
            SetTextColor(hdc,RGB(255,255,255));

            SetFillColor(hdc,RGB(180,0,240));
            FillRect(hdc,&rc);
            OffsetRect(&rc,1,1);
        }
        else
        {
            SetTextColor(hdc,RGB(1,1,1));
            switch(hwnd->Style&BS_SURFACE_MASK)
            {
                case    BS_NICE:
                        GradientFillRect(hdc,&rc,
                                RGB(210,210,210),RGB(150,150,150),GFILL_U_D);
                        break;

                case    BS_SIMPLE:
                        SetDrawColor(hdc,RGB(220,220,220));
                        DrawLine(hdc,0,0,0,RectH(&rc)-1); //L
                        DrawLine(hdc,0,0,RectW(&rc)-1,0);   //U

                        SetDrawColor(hdc,RGB(80,80,80));
                        DrawLine(hdc,RectW(&rc)-1,0,RectW(&rc)-1,RectH(&rc)-1); //R
                        DrawLine(hdc,0,RectH(&rc)-1,RectW(&rc)-1,RectH(&rc)-1); //D

                        InflateRect(&rc,-1,-1);
                        SetFillColor(hdc,RGB(150,150,150));
                        FillRect(hdc,&rc);
                        break;

                case    BS_FLAT:
                default:
                        SetFillColor(hdc,RGB(140,140,140));
                        FillRect(hdc,&rc);
                        break;

            }


        }

        DrawText(hdc,hwnd->Text,-1,&rc,DT_VCENTER|DT_CENTER);
        EndPaint(hwnd,hdc);
    }


}

u32 button_proc(MSG *pMsg)
{
    HWND hwnd;
    RECT rc;

    hwnd =pMsg->hwnd;
    switch(pMsg->Code)
    {
        case    MSG_CREATE:
                GetWindowRect(hwnd,&rc);
                printf("btn[%04XH]: MSG_CREATE.\r\n",hwnd->WinId);
                return 1;
                ////
        case    MSG_LBUTTON_DOWN:
                {
//                    int x,y;
//                    x =LO16(pMsg->Param2);
//                    y =HI16(pMsg->Param2);
                    switch(_get_button_type(hwnd))
                    {
                        case    BS_NORMAL:  //常规按钮
                                hwnd->Style |= BS_PUSHED;
                                SendMessage(GetParent(hwnd),MSG_NOTIFY,(BTN_DOWN<<16)|(hwnd->WinId),(ptu32_t)hwnd);
                                break;
                                ////
                        case    BS_HOLD:    //自锁按钮
                                hwnd->Style ^= BS_PUSHED;
                                if(hwnd->Style&BS_PUSHED)
                                {
                                    SendMessage(GetParent(hwnd),MSG_NOTIFY,(BTN_DOWN<<16)|(hwnd->WinId),(ptu32_t)hwnd);
                                }
                                else
                                {
                                    SendMessage(GetParent(hwnd),MSG_NOTIFY,(BTN_UP<<16)|(hwnd->WinId),(ptu32_t)hwnd);
                                }
                                break;
                                ////

                    }

                    InvalidateWindow(hwnd);
                }
                break;
                ////
        case    MSG_LBUTTON_UP:
                {
//                    int x,y;
//                    x =LO16(pMsg->Param2);
//                    y =HI16(pMsg->Param2);
                    switch(_get_button_type(hwnd))
                    {
                        case    BS_NORMAL:
                                hwnd->Style &= ~BS_PUSHED;
                                SendMessage(GetParent(hwnd),MSG_NOTIFY,(BTN_UP<<16)|(hwnd->WinId),(ptu32_t)hwnd);
                                break;
                                ////
                        case    BS_HOLD:
                                break;
                                ////

                    }
                    InvalidateWindow(hwnd);

                }
                break;
                ////
        case    MSG_MOUSE_MOVE:
                {
//                    int x,y;
//                    x =LO16(pMsg->Param2);
//                    y =HI16(pMsg->Param2);
                    //printf("btn: MSG_MOUSE_MOVE: %d,%d\r\n",x,y);
                }
                break;
                ////

        case    MSG_PAINT:
            //  printf("btn[%04XH]: MSG_PAINT.\r\n",hwnd->WinId);
                button_paint(pMsg);
                return 1;
                ////

        case    MSG_DESTROY:
                printf("btn[%04XH]: MSG_DESTROY.\r\n",hwnd->WinId);
                return 1;
                ////

        default:
                return DefWindowProc(pMsg);

    }
    return 0;

}
/*============================================================================*/




