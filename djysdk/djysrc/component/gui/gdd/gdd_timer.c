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
//文件描述: 窗口定时器系统机制和管理
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-11-10
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include    <gui/gdd/gdd_private.h>

static  list_t  list_system_timer;

//----分配定时器内存-------------------------------------------------------------
//描述: 略
//参数：无
//返回：定时器内存指针
//------------------------------------------------------------------------------
static  struct WinTimer*  TMR_Alloc(void)
{
    struct WinTimer *tmr;

    tmr =malloc(sizeof(struct WinTimer));
    return tmr;
}

//----释放定时器内存-------------------------------------------------------------
//描述: 释放一个由TMR_Alloc分配的定时器内存.
//参数：定时器内存指针
//返回：无
//------------------------------------------------------------------------------
static  void    TMR_Free(struct WinTimer *ptmr)
{
    free(ptmr);
}

//----锁定定时器----------------------------------------------------------------
//描述: 锁定定时器,用于对定时器互斥操作,该函数返回TRUE时,必须调用TMR_Unlock解锁;
//      而当该函数返回FALSE,则无需调用TMR_Unlock.
//参数：定时器对象指针
//返回：成功:TRUE; 失败:FLASE;
//------------------------------------------------------------------------------
bool_t    TMR_Lock(struct WinTimer *ptmr)
{
    if(NULL == ptmr)
        return FALSE;
    if(GDD_Lock())
        return TRUE;

    GDD_Unlock();
    return  FALSE;
}

//----解锁定时器----------------------------------------------------------------
//描述: 当定时器锁定成功后,由该函数进行解锁操作.
//参数：定时器对象指针
//返回：无
//------------------------------------------------------------------------------
void    TMR_Unlock(struct WinTimer *ptmr)
{
    GDD_Unlock();
}

//----GDD定时器模块初始化--------------------------------------------------------
//描述: 该函数由GDD内部调用
//参数：无
//返回：无
//------------------------------------------------------------------------------
void    GDD_TimerInit(void)
{
    dListInit(&list_system_timer);
}

//----创建定时器----------------------------------------------------------------
//描述: 略
//参数：hwnd: 定时器所属窗口.定时到则给该窗口发定时器消息。
//      Id:   定时器Id.
//      Flag: 定时器标记.
//      IntervalMS: 定时间隔时间(毫秒数).
//返回：定时器对象指针
//------------------------------------------------------------------------------
struct WinTimer*  GDD_CreateTimer(HWND hwnd,u16 Id,u32 IntervalMS,u16 Flag)
{
    struct WinTimer *ptmr=NULL;

    if(HWND_Lock(hwnd))
    {
        ptmr =TMR_Alloc();;
        if(NULL!=ptmr)
        {

            dListInit(&ptmr->node_sys);
            dListInit(&ptmr->node_hwnd);
            dListInit(&ptmr->node_msg_timer);

            ptmr->hwnd          =hwnd;
            ptmr->Id            =Id;
            ptmr->Flag          =Flag;
            ptmr->Interval      =IntervalMS;
            ptmr->HoldTime      =GUI_GetTickMS();

            GDD_Lock();
            dListInsertBefore(&list_system_timer,&ptmr->node_sys);
            dListInsertBefore(&hwnd->list_timer,&ptmr->node_hwnd);
            GDD_Unlock();

        }
        HWND_Unlock(hwnd);
    }
    return  ptmr;
}

//----查找定时器----------------------------------------------------------------
//描述: 按Id查找指定窗口的定时器
//参数：hwnd: 要查找的定时器所在窗口.
//      Id: 要查找的定时器Id
//返回：成功返回定时器对象指定,否则返回NULL
//------------------------------------------------------------------------------
struct WinTimer*  GDD_FindTimer(HWND hwnd,u16 Id)
{
    list_t *lst,*n;
    struct WinTimer *ptmr=NULL;

    if(GDD_Lock())
    {
        if(HWND_Lock(hwnd))
        {
            lst =&hwnd->list_timer;
            n   =lst->next;
            while(n!=lst)
            {
                ptmr =(struct WinTimer*)dListEntry(n,struct WinTimer,node_hwnd);
                if(NULL!=ptmr)
                {
                    if(ptmr->Id==Id)
                    {
                        break;
                    }
                }
                n=n->next;
            }
            HWND_Unlock(hwnd);
        }

        GDD_Unlock();
    }
    return ptmr;
}

//----重置定时器----------------------------------------------------------------
//描述: 重新设置定时器参数
//参数：ptmr: 定时器对象.
//      IntervalMS: 定时间隔时间(毫秒数).
//返回：TRUE:成功; FALSE:失败;
//------------------------------------------------------------------------------
bool_t    GDD_ResetTimer(struct WinTimer *ptmr,u32 IntervalMS,u32 Flag)
{
    bool_t res=FALSE;

    if(TMR_Lock(ptmr))
    {
        if(HWND_Lock(ptmr->hwnd))
        {
            ptmr->Flag      =Flag;
            ptmr->Interval  =IntervalMS;
            ptmr->HoldTime  =GUI_GetTickMS();
            dListRemove(&ptmr->node_msg_timer);

            res =TRUE;
            HWND_Unlock(ptmr->hwnd);
        }

        TMR_Unlock(ptmr);
    }

    return res;
}

//----删除定时器----------------------------------------------------------------
//描述: 该函数为内部调用,不进行参数检查.
//参数：ptmr: 定时器对象.
//返回：无
//------------------------------------------------------------------------------
static void __DeleteTimer(struct WinTimer*ptmr)
{

    dListRemove(&ptmr->node_hwnd);
    dListRemove(&ptmr->node_sys);
    dListRemove(&ptmr->node_msg_timer);

    TMR_Free(ptmr);

}

//----删除定时器----------------------------------------------------------------
//描述: 略
//参数：ptmr: 定时器对象.
//返回：TRUE:成功; FALSE:失败;
//------------------------------------------------------------------------------
bool_t    GDD_DeleteTimer(struct WinTimer *ptmr)
{
    HWND hwnd;

    if(TMR_Lock(ptmr))
    {
        hwnd =ptmr->hwnd;
        if(HWND_Lock(hwnd))
        {
            __DeleteTimer(ptmr);
            HWND_Unlock(hwnd);
            return TRUE;
        }

        TMR_Unlock(ptmr);
    }
    return FALSE;
}

//----删除窗口所有定时器-------------------------------------------------------
//描述: 该函数为GDD内部调用
//参数：hwnd: 窗口句柄.
//返回：无.
//------------------------------------------------------------------------------
void __RemoveWindowTimer(HWND hwnd)
{
    list_t *lst,*n,*next;
    struct WinTimer *ptmr;

    lst =&hwnd->list_timer;
    n   =lst->next;

    while(!dListIsEmpty(n))
    {
        next =n->next;
        ptmr =(struct WinTimer*)dListEntry(n,struct WinTimer,node_hwnd);
        if(NULL!=ptmr)
        {
            if(TMR_Lock(ptmr))
            {
                __DeleteTimer(ptmr);
                TMR_Unlock(ptmr);
            }
        }

        n =next;
    }
}

//----定时器处理函数-------------------------------------------------------------
//描述: 该函数由GDD内部调用
//参数：ptmr: 定时器对象指针;
//     tick_time_ms: 当前tick时间,这个必须由GUI_GetTickMS提供.
//返回：TRUE:定时器超时; FALSE:定时器未超时.
//------------------------------------------------------------------------------
bool_t    __TimerHandler(struct WinTimer *ptmr,u32 tick_time_ms)
{
    u32 time;

    if(ptmr->Flag&TMR_START)
    {
        if(tick_time_ms < ptmr->HoldTime)
        {
            //32位时间值溢出处理
            time =(0xFFFFFFFF-ptmr->HoldTime)+tick_time_ms;
        }
        else
        {
            time =tick_time_ms - ptmr->HoldTime;
        }

        //检查是否超时
        if(time >= ptmr->Interval)
        {
            //更新HoldTime
            ptmr->HoldTime =tick_time_ms;

            if(ptmr->Flag&TMR_SINGLE)
            {
                //如果设置了单次定时标记,则停止该定时器
                ptmr->Flag &=~TMR_START;
            }
            return TRUE;
        }

    }
    return FALSE;
}

//----GDD系统定时器模块执行函数--------------------------------------------------
//描述: 该函数由GDD内部调用,每调用一次该函数,将对系统所有定时器扫描一遍是否有
//      超时事件,如果有超时事件产生,便向该定时器所属窗口发送定时器超时消息.
//参数：tick_time_ms: 当前tick时间(毫秒数).
//返回：无
//------------------------------------------------------------------------------
void    GDD_TimerExecu(u32 tick_time_ms)
{
    list_t *lst,*n;
    struct WinTimer *ptmr;

    if(GDD_Lock())
    {
        lst =&list_system_timer;
        n =lst->next;
        while(n!=lst)
        {
            ptmr =(struct WinTimer*)dListEntry(n,struct WinTimer,node_sys);
            if(TMR_Lock(ptmr))
            {
                if(__TimerHandler(ptmr,tick_time_ms))
                {
                    __PostTimerMessage(ptmr);
                }
                TMR_Unlock(ptmr);
            }

            n =n->next;
        }

        GDD_Unlock();
    }
}


