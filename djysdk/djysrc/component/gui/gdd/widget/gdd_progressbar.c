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
#include    "../include/gdd_private.h"
/*============================================================================*/

static  void progressbar_paint(HWND hwnd,PROGRESSBAR_DATA *pPB)
{
    HDC hdc;
    RECT m_rc[2];
    RECT rc0;
    int gfill_mode=0;

    hdc =BeginPaint(hwnd);
    if(NULL!=hdc)
    {
        GetClientRect(hwnd,&rc0);

        if(hwnd->Style&PBS_VER)
        {
        	gfill_mode =GFILL_L_R;
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
        	gfill_mode =GFILL_U_D;
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
    }
}

u32 progressbar_proc(MSG *pMsg)
{
    HWND hwnd;
    RECT rc;
    PROGRESSBAR_DATA *pPB;

    hwnd =pMsg->hwnd;
    switch(pMsg->Code)
    {
        case    MSG_CREATE:
        		pPB =(PROGRESSBAR_DATA*)malloc(sizeof(PROGRESSBAR_DATA));
        		if(pPB!=NULL)
        		{
        			if(pMsg->Param1==NULL)
        			{
        				pPB->Flag =0;
						pPB->Range =100;
						pPB->Pos   =0;
						pPB->FGColor =RGB(200,0,0);
						pPB->BGColor =RGB(0,0,200);
						pPB->DrawTextFlag =DT_VCENTER|DT_CENTER;
        			}
        			else
        			{
        				PROGRESSBAR_DATA *pPB_Usr;

        				pPB_Usr =(PROGRESSBAR_DATA*)pMsg->Param1;
        				pPB->Flag =pPB_Usr->Flag;
        				pPB->Range =pPB_Usr->Range;
        				pPB->Pos =pPB_Usr->Pos;
        				pPB->FGColor =pPB_Usr->FGColor;
        				pPB->BGColor =pPB_Usr->BGColor;
        				pPB->DrawTextFlag =pPB_Usr->DrawTextFlag;

        			}
        			SetWindowPrivateData(hwnd,(void*)pPB);
					return TRUE;
        		}
        		return FALSE;
                ////
        case	PBM_SETDATA:
        		if(pMsg->Param1!=NULL)
        		{
					pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
					if(pPB!=NULL)
					{
						memcpy(pPB,(char*)pMsg->Param1,sizeof(PROGRESSBAR_DATA));
						InvalidateWindow(pMsg->hwnd);
						return TRUE;
					}
        		}
        		return FALSE;
        		////
        case	PBM_GETDATA:
        		if(pMsg->Param1!=NULL)
        		{
					pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
					if(pPB!=NULL)
					{
						memcpy((char*)pMsg->Param1,pPB,sizeof(PROGRESSBAR_DATA));
						InvalidateWindow(pMsg->hwnd);
						return TRUE;
					}
        		}
        		return FALSE;
        		////
        case	PBM_SETRANGE:
        		pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
        		if(pPB!=NULL)
        		{
        			pPB->Range =(u32)pMsg->Param1;
        			InvalidateWindow(pMsg->hwnd);
        		}
        		return TRUE;
        		////
        case	PBM_GETRANGE:
				pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
				if(pPB!=NULL)
				{
					return pPB->Range;
				}
				return 0;
				////
        case	PBM_SETPOS:
        		pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
        		if(pPB!=NULL)
        		{
        			pPB->Pos =(u32)pMsg->Param1;
        			InvalidateWindow(pMsg->hwnd);
        		}
        		return TRUE;
        		////
        case	PBM_GETPOS:
				pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
				if(pPB!=NULL)
				{
					return pPB->Pos;
				}
				return 0;
				////
        case    MSG_PAINT:
        		pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
        		progressbar_paint(pMsg->hwnd,pPB);
                return TRUE;
                ////
        case    MSG_DESTROY:
        		pPB =(PROGRESSBAR_DATA*)GetWindowPrivateData(hwnd);
        		if(pPB!=NULL)
        		{
        			free(pPB);
        			SetWindowPrivateData(hwnd,NULL);
        		}
                return TRUE;
                ////

        default:
                return DefWindowProc(pMsg);

    }
    return FALSE;

}
/*============================================================================*/
