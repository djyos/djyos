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

//所属模块: gui
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: gui core的显示器部分，不包含硬件相关部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-11-21
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stdlib.h"
#include "rsc.h"
#include "gkernel.h"
#include "..\include\gk_win.h"
#include "gk_display.h"
#include <string.h>

//----安装显示器---------------------------------------------------------------
//功能: 把一台新显示器登记到显示器资源队列中。
//参数: display，待安装的显示器，应该在driver中进行必要的初始化
//      frame_bitmap,帧缓冲，若为NULL，则表示该显示器没有帧缓冲，帧缓冲由一块能
//          直接以内存方式访问的存储器构成，比如有一些lcd模块，虽然模块内部有一
//          块内存用于帧缓冲，但其不能映射到系统内存，不能用在这里。
//      name，显示器名，所指向的字符串内存区不能是局部变量
//返回: true=成功初始化，false=失败
//-----------------------------------------------------------------------------
bool_t GK_InstallDisplay(struct tagDisplayRsc *display,
//                        struct tagRectBitmap *frame_bitmap,
                          const char *name)
{
    struct tagRscNode *rsc;
    struct tagRectBitmap *frame_bitmap;
    struct tagGkWinRsc *frame_buffer;
    u32 msk_size;
    if(display == NULL)
        return false;
    display->desktop = NULL;
    display->reset_clip = false;
    //用户没有指定显存专用的堆，则使用系统堆。
    if(display->DisplayHeap == NULL)
        display->DisplayHeap = M_FindHeap("sys");
    frame_buffer = display->frame_buffer;
    if(frame_buffer != NULL)
    {
        frame_bitmap = frame_buffer->wm_bitmap;
        if((frame_bitmap->PixelFormat&CN_PF_INDEX_MSK) >= CN_DISPLAY_PF_LIMIT)
            return false;       //不允许的显示器格式
        frame_buffer->changed_msk.PixelFormat = CN_SYS_PF_GRAY1 ;
        frame_buffer->changed_msk.height = (frame_bitmap->height+7)/8;
        frame_buffer->changed_msk.width = (frame_bitmap->width+7)/8;
        frame_buffer->changed_msk.linebytes = (frame_buffer->changed_msk.width+7)/8;
        msk_size = frame_buffer->changed_msk.linebytes *
                                frame_buffer->changed_msk.height;
        frame_buffer->changed_msk.bm_bits = M_Malloc(msk_size,0);
        if(frame_buffer->changed_msk.bm_bits== NULL)    //分配修改掩码内存失败
        {
            return false;
        }
        memset(frame_buffer->changed_msk.bm_bits,0,msk_size);

        frame_buffer->user_private = NULL;
        frame_buffer->z_back = NULL;
        frame_buffer->z_top = NULL;
        frame_buffer->visible_bak = NULL;
        frame_buffer->visible_clip = NULL;
        frame_buffer->redraw_clip = NULL;
        frame_buffer->changed_clip = NULL;
        frame_buffer->copy_clip = NULL;
        frame_buffer->disp = display;
        frame_buffer->z_prio = 0;
        frame_buffer->direct_screen = false;
        frame_buffer->KeyColor = 0;
        frame_buffer->RopCode = 0;
        //帧缓冲总是CN_R3_SRCCOPY，dest_blend总是FALSE
        frame_buffer->dest_blend = false;
        //帧缓冲总是CN_R3_SRCCOPY，不需要pattern。
//        frame_buffer->pat_buf = NULL;
        frame_buffer->change_flag = CN_GKWIN_CHANGE_NONE;
        frame_buffer->bound_limit = true;
        frame_buffer->absx0 = 0;
        frame_buffer->absy0 = 0;
        frame_buffer->left = 0;
        frame_buffer->top = 0;
        frame_buffer->right = frame_bitmap->width;
        frame_buffer->bottom = frame_bitmap->height;
        frame_buffer->limit_left = 0;
        frame_buffer->limit_top = 0;
        frame_buffer->limit_right = frame_bitmap->width;
        frame_buffer->limit_bottom = frame_bitmap->height;
    }
    rsc = Rsc_SearchTree("display");     //取显示器资源树
    if(NULL != Rsc_AddSon(rsc,&display->node,sizeof(struct tagDisplayRsc),RSC_DISPLAY,name))
        return true;
    else
        return false;
}

//----安装镜像显示器-----------------------------------------------------------
//功能: 把一台显示器作为当前默认显示器的镜像显示器登记到显示器资源队列中。
//参数: display，待安装的显示器，应该在driver中进行必要的初始化
//      name，显示器名，所指向的字符串内存区不能是局部变量
//返回: true=成功初始化，false=失败
//-----------------------------------------------------------------------------
bool_t GK_InstallDisplayMirror(struct tagDisplayRsc *base_display,
                                 struct tagDisplayRsc *mirror_display,char *name)
{
    if(NULL != Rsc_AddSon(&base_display->node,&mirror_display->node,
                            sizeof(struct tagDisplayRsc),RSC_DISPLAY,name))
        return true;
    else
        return false;
}

//----设置默认显示器-----------------------------------------------------------
//功能: 设置默认显示器
//参数: name，显示器名
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t GK_SetDefaultDisplay(char *name)
{
    struct tagRscNode *rsc;
    rsc = Rsc_SearchTree("display");     //取显示器资源树
    rsc = Rsc_SearchSon(rsc,name);     //找到被操作的显示器资源
    if(rsc == NULL)
    {
        return false;
    }else
    {
        Rsc_MoveToHead(rsc);          //资源树中的队列头结点就是默认显示器
        return true;
    }
}

//----取显示器的默认显示设置---------------------------------------------------
//功能: 取一个显示器的默认显示设置，实际上就是桌面窗口的资源节点
//参数: display，显示器指针
//返回: draw_set指针
//-----------------------------------------------------------------------------
struct tagGkWinRsc *GK_GetRootWin(struct tagDisplayRsc *display)
{
    return display->desktop;
}

//----切换frame buffer---------------------------------------------------------
//功能: 当一个显示器有多个frame buffer时，用本函数切换当前frame buffer，在系统
//      中，frame buffer是一种资源
//参数: display，被操作的显示器
//      fbuf，缓冲区指针
//返回: true=成功，false=失败，失败的原因只有一个:该显示驱动不是帧缓冲型
//-----------------------------------------------------------------------------
bool_t GK_SwitchFrameBuffer(struct tagDisplayRsc *display,
                              struct tagRectBitmap *fbuf)
{
    return true;
}

//----创建frame buffer---------------------------------------------------------
//功能: 为某显示器创建一个frame buffer
//参数: display，被操作的显示器
//返回: 新frame buffer 指针，NULL=创建失败
//-----------------------------------------------------------------------------
struct tagRectBitmap *GK_CreateFrameBuffer(struct tagDisplayRsc *display)
{
    return NULL;
}


