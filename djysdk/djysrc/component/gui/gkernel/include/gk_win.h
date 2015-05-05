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
//文件描述: gui kernel主体部分
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-11-21
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __GK_WIN_H__
#define __GK_WIN_H__
#include "errno.h"
#include "config-prj.h"
#include "ring.h"
#ifdef __cplusplus
extern "C" {
#endif

struct tagGkscParaCreateDesktop;
struct tagGkscParaCreateGkwin;
struct tagGkscParaSetRopCode;
struct tagGkscParaSetPatBuf;
struct tagGkscParaSetPrio;
struct tagGkscParaSetTransparentColor;
struct tagGkscParaSetBitmsk;
struct tagGkscParaDestroyWin;
struct tagGkscParaSetBoundMode;
struct tagGkscParaMoveWin;
struct tagGkscParaChangeWinArea;
struct tagGkscParaSetDirectScreen;
struct tagGkscParaUnsetDirectScreen;

struct tagDisplayRsc;

//记录窗口修改参数
#define CN_GKWIN_CHANGE_NONE    0   //没有修改
#define CN_GKWIN_CHANGE_PART    1   //部分修改
#define CN_GKWIN_CHANGE_ALL     2   //全部修改

struct tagGkChunnel       //用户程序(通常是gui windows) 和 gui kernel的通信管道
{
    struct tagRingBuf ring_syscall;         //gui 系统调用环形缓冲区
    struct tagRingBuf ring_usercall;        //gui 用户调用唤醒缓冲区。
    u8 *syscall_buf;                        //为系统调用环形缓冲区分配内存
    u8 *usercall_buf;                       //为用户调用唤醒缓冲区分配内存
    struct tagMutexLCB *syscall_mutex;      //互斥量,用于防止并发写
    struct tagSemaphoreLCB *syscall_semp;   //信号量，写时如果缓冲区满则等候。
//    struct tagMutexLCB *usercall_mutex;   //互斥量,用于防止并发写
                                            //usercall ring只有gkwin单线程写
    struct tagSemaphoreLCB *usercall_semp;  //信号量，写时如果缓冲区满则等候。
};

ptu32_t ModuleInstall_GK(ptu32_t para);
struct tagGkWinRsc *__GK_GetZsectionStart(struct tagGkWinRsc *gkwin);
struct tagGkWinRsc *__GK_GetZsectionEnd(struct tagGkWinRsc *gkwin);
struct tagGkWinRsc *__GK_GetZsectionAllStart(
                                                    struct tagGkWinRsc *gkwin);
struct tagGkWinRsc *__GK_GetZsectionAllEnd(
                                                    struct tagGkWinRsc *gkwin);
struct tagGkWinRsc *GK_CreateDesktop(struct tagGkscParaCreateDesktop *para);
struct tagGkWinRsc *GK_CreateWin(struct tagGkscParaCreateGkwin *para);
void GK_SetName(struct tagGkWinRsc *gkwin,char *name);
void GK_SetUnlock(struct tagGkWinRsc *gkwin);
void GK_SetLock(struct tagGkWinRsc *gkwin);
void GK_SetHide(struct tagGkWinRsc *gkwin);
void GK_SetShow(struct tagGkWinRsc *gkwin);
void GK_MoveWin(struct tagGkscParaMoveWin *para);
void __GK_SetBound(struct tagGkWinRsc *gkwin);
void GK_SetBoundMode(struct tagGkscParaSetBoundMode *para);
void GK_SetPrio(struct tagGkscParaSetPrio *para);
bool_t GK_SetRopCode(struct tagGkscParaSetRopCode *para);
bool_t GK_SetPatBuf(struct tagGkscParaSetPatBuf *para);
bool_t GK_SetTransparentColor(
                                struct tagGkscParaSetTransparentColor *para);
bool_t GK_SetBitMsk(struct tagGkscParaSetBitmsk *para);
void GK_SetDirectScreen(struct tagGkscParaSetDirectScreen *para);
void GK_UnSetDirectScreen(struct tagGkscParaUnsetDirectScreen *para);
void __GK_DestroyWinChild(struct tagGkWinRsc *gkwin);
void GK_DestroyWin(struct tagGkscParaDestroyWin *para);
bool_t GK_ChangeWinArea(struct tagGkscParaChangeWinArea *para);
void GK_SetCanvasColor(struct tagGkWinRsc *gkwin,u32 color);
void GK_GetCanvasColor(struct tagGkWinRsc *gkwin,u32 *color);
void __GK_OutputRedraw(struct tagDisplayRsc *display);
void __GK_OutputFrameBuffer(struct tagDisplayRsc *display);
u16 GK_SyscallChunnel(u16 command,u32 sync_time,void *param1,u16 size1,
                                                void *param2,u16 size2);
u16 GK_UsercallChunnel(u16 usercall_id,struct tagSemaphoreLCB *semp,
                        void **result,void *param,u16 size);
ptu32_t GK_Server(void);

#ifdef __cplusplus
}
#endif

#endif //__GK_WIN_H__

