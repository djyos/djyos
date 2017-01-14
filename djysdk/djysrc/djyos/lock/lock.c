//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

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
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//所属模块: 锁模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 包含信号量和互斥量
//其他说明:
//修订历史:
//3. 日期:2013-2-20
//   作者:  罗侍田.
//   新版本号：1.1.0
//   修改说明: 修改mutex相关代码，使之在同一个事件内，可递归调用，即当一个事件
//      拥有mutex后，其他事件再调用pend的话，将阻塞；若自身再次调用pend，将增加
//      调用计数，并不阻塞。必须调用同样次数的post后，才能真正释放mutex。
//2. 日期:2013-1-4
//   作者:  罗侍田.
//   新版本号：1.0.1
//   修改说明: 修正了mutex_delete函数的一处错误，该bug由网友"王毛磊"提交
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "int.h"
#include "lock.h"
#include "pool.h"
#include "djyos.h"
#include "object.h"
#include "systime.h"
#include "core_config.h"

static struct Object *pg_semaphore_rsc=NULL;
static struct Object  *pg_mutex_rsc=NULL;
//说明： gc_u32CfgLockLimit 是用户配置的，由于用户并不知道操作系统需要用多少信号量，
//      所以操作系统并不占用 gc_u32CfgLockLimit 指标，用户使用的信号量从
//      tg_semp_pool定义的内存池中分配，操作系统使用的信号量自己定义，两不
//      相关，如果用户企图调用 Lock_SempDelete 删除操作系统定义的信号量，将引起造成
//      内存泄漏，而且相关操作系统资源也将失去信号量保护，后果不可预料
//定义信号量结构初始内存池
static union lock_MCB *g_ptLockMemBlock;
struct MemCellPool *g_ptLockPool;  //信号量结构内存池头指针

//这是在rsc模块中定义的，专供lock模块使用的一个函数
struct Object *__Lock_RscAddLockTree(struct Object *Obj,
                                u16 Size, const char *Name);

extern void __Djy_EventReady(struct EventECB *event_ready);
extern void __Djy_CutReadyEvent(struct EventECB *event);
extern void __Djy_ResumeDelay(struct EventECB *delay_event);
extern void __Djy_AddToDelay(u32 u32l_uS);
extern void __Djy_AddRunningToBlock(struct EventECB **Head,bool_t Qsort,u32 timeout,u32 Status);

//----初始化锁模块模块step1----------------------------------------------------
//功能：初始化信号量模块的第一步，此后可以调用除semp_create和mutex_create以外的
//      其他函数，包括semp_create_s和mutex_createe_s，该两函数被禁止是因
//      为module_init_memb还没有调用，又因mudule_init_memb需要使用信号量，故只能
//      在module_init_lock1后面调用。
//参数：无
//返回：1
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Lock1(ptu32_t para)
{
    static struct Object semp_root;
    static struct Object mutex_root;
    para = para;        //消除编译器告警
    pg_semaphore_rsc = __Lock_RscAddLockTree(&semp_root,sizeof(struct Object),"semaphore");
    pg_mutex_rsc = __Lock_RscAddLockTree(&mutex_root,sizeof(struct Object),"mutex");
    return 1;
}

//----初始化锁模块模块step2----------------------------------------------------
//功能：初始化信号量模块的第二步，此后可以调用含semp_create和mutex_createe在内的
//      所有锁函数了
//参数：无
//返回：1
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Lock2(ptu32_t para)
{
    static struct MemCellPool lock_pool;
    g_ptLockMemBlock = M_Malloc(gc_u32CfgLockLimit* sizeof(union lock_MCB),0);
    if(g_ptLockMemBlock == NULL)
        return 0;
    //初始化锁控制块内存池
    g_ptLockPool = Mb_CreatePool_s( &lock_pool,
                                    (void*)g_ptLockMemBlock,
                                    gc_u32CfgLockLimit,
                                    sizeof(union lock_MCB),
                                    0,0,
                                    "锁控制块池");
    return 1;
}

//----创建一个信号量-----------------------------------------------------------
//功能：建立一个信号量，以后就可以使用这个信号量了。信号量控制块从内存池中分配。
//参数：semp_limit,信号灯的总数，cn_limit_uint32=有无限多的信号灯。
//      init_lamp,初始信号灯数量
//      sync_order,被阻塞的事件排队顺序，取值为 CN_BLOCK_FIFO
//          或 CN_BLOCK_PRIO，注意，互斥量总是以优先级排队。
//      name，信号量的名字，所指向的字符串内存区不能是局部变量，可以是空
//返回：新建立的信号量指针
//-----------------------------------------------------------------------------
struct SemaphoreLCB *Lock_SempCreate(u32 lamps_limit,u32 init_lamp,
                                        u32 sync_order,const char *name)
{
    struct SemaphoreLCB *semp;
    if(init_lamp > lamps_limit)
        return NULL;
    semp = Mb_Malloc(g_ptLockPool,0);
    if(semp == NULL)
        return NULL;
    semp->sync_order = sync_order;
    semp->lamps_limit = lamps_limit;
    semp->lamp_counter = init_lamp;
    semp->semp_sync = NULL;
    //把新节点挂到信号量根节点下
    OBJ_AddChild(pg_semaphore_rsc,&semp->node,
                        sizeof(struct SemaphoreLCB),RSC_SEMP,name);
    return semp;
}

//----创建信号量---------------------------------------------------------------
//功能：创建一个信号量，调用者须提供信号量控制块，高可靠性的应用中，不应该使用动
//      态分配的方式，静态定义更可靠，然后把指针传递过来。内核中使用的信号量，都
//      是使用静态定义的。应用程序配置cfg_locks_limit时，只需要考虑自己的需求就
//      可以了。
//参数：semp,目标信号量指针
//      semp_limit,信号灯的总数，cn_limit_uint32=有无限多的信号灯。
//      init_lamp,初始信号灯数量
//      sync_order,被阻塞的事件排队顺序，取值为 CN_BLOCK_FIFO
//          或 CN_BLOCK_PRIO，注意，互斥量总是以优先级排队。
//      name，信号量的名字，所指向的字符串内存区不能是局部变量，可以是空
//返回：新建立的信号量指针
//-----------------------------------------------------------------------------
struct SemaphoreLCB *Lock_SempCreate_s( struct SemaphoreLCB *semp,
                       u32 lamps_limit,u32 init_lamp,u32 sync_order,const char *name)
{
    if(semp == NULL)
        return NULL;
    semp->sync_order = sync_order;
    semp->lamps_limit = lamps_limit;
    semp->lamp_counter = init_lamp;
    semp->semp_sync = NULL;
    //把新节点挂到信号量根节点下
    OBJ_AddChild(pg_semaphore_rsc,&semp->node,
                        sizeof(struct SemaphoreLCB),RSC_SEMP,name);

    return semp;
}

//----点亮一个信号灯------------------------------------------------------------
//功能：释放一个信号量，可用信号量加1。
//参数：semp,信号量指针
//返回：无
//-----------------------------------------------------------------------------
void Lock_SempPost(struct SemaphoreLCB *semp)
{
    struct EventECB *event;

    if(semp == NULL)
        return;
    Int_SaveAsynSignal();
    if(semp->semp_sync == NULL)     //等待队列空，点亮一盏信号灯
    {
        if(semp->lamps_limit == CN_LIMIT_UINT32)        //信号灯数量不限
        {
            if(semp->lamp_counter != CN_LIMIT_UINT32)   //信号灯数量未到上限
                semp->lamp_counter ++;                  //点亮一盏信号灯
        }else if(semp->lamp_counter < semp->lamps_limit )//信号灯数量未到上限
            semp->lamp_counter++;                       //点亮一盏信号灯
    } else      //有事件在等待，把信号灯直接转交给队列中优先级最高的事件
    {
        event = semp->semp_sync;
        event->sync_head = NULL;
        if(event->multi_next == event)
        {//队列中只有一个事件
            semp->semp_sync = NULL;
        }else
        {//队列中有多个事件，取出队列头部的事件
            semp->semp_sync = event->multi_next;
            event->multi_next->multi_previous = event->multi_previous;
            event->multi_previous->multi_next = event->multi_next;
            event->multi_next = NULL;
            event->multi_previous = NULL;
        }
        if(event->event_status & CN_STS_SYNC_TIMEOUT)
            __Djy_ResumeDelay(event);    //如果事件在超时等待队列中，取出
        event->wakeup_from = CN_STS_WAIT_SEMP;
        event->event_status = CN_STS_EVENT_READY;
        __Djy_EventReady(event);
    }
    Int_RestoreAsynSignal();
}

//----请求一盏信号灯-----------------------------------------------------------
//功能：请求然后熄灭一盏信号灯，表示可用资源数减1。
//参数：semp,信号量指针
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//      超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回：true=取得信号返回或资源不受信号灯保护(semp == NULL)，semp==NULL时返回
//      true是有意义的，如果你希望代码在不管是否受信号灯保护都保持一致，把semp
//      设为NULL是明智的选择，比如你要构建一个不受信号灯保护的设备，可把该设备
//      struct pan_device的semp成员设为NULL。
//      false=没有取得信号(超时返回或其他原因)
//-----------------------------------------------------------------------------
bool_t Lock_SempPend(struct SemaphoreLCB *semp,u32 timeout)
{
    struct EventECB *event;
    bool_t lamp,sch;

    if(semp == NULL)
    {
        g_ptEventRunning->error_no = EN_LOCK_PARA_ERROR;
        return false;
    }
    sch = Djy_QuerySch();
    Int_SaveAsynSignal();
    if(semp->lamps_limit == CN_LIMIT_UINT32)   //本信号量有无限多信号灯
    {
        if(semp->lamp_counter != 0) //信号灯数量不为0
            semp->lamp_counter--;   //熄灭一盏信号灯
        lamp = true;
    }else                           //本信号量的信号灯数量有限
    {
        if(semp->lamp_counter > 0)  //有点亮的信号灯
        {
            lamp = true;            //标记资源可用
            semp->lamp_counter--;   //熄灭一盏信号灯
        }else                       //没有点亮的信号灯
            lamp = false;           //标记资源不可用
    }

    if(lamp == true)
    {
        Int_RestoreAsynSignal();  //恢复中断，将触发上下文切换
        return true;    //取得信号灯返回
    }
    else if(!sch || (timeout == 0))//禁止调度时,不能阻塞.或timeout为0
    {
        Int_RestoreAsynSignal();  //恢复中断，将触发上下文切换
        return false;   //没有取得信号灯返回
    }

    __Djy_AddRunningToBlock(&(semp->semp_sync),semp->sync_order,timeout,CN_STS_WAIT_SEMP);
//  __Djy_CutReadyEvent(g_ptEventRunning);
//  g_ptEventRunning->previous = NULL;
//  g_ptEventRunning->next = NULL;
//
//  g_ptEventRunning->sync_head = &semp->semp_sync;
//  if(semp->semp_sync == NULL)
//  {//同步队列空,running事件自成双向循环链表
//      g_ptEventRunning->multi_next = g_ptEventRunning;
//      g_ptEventRunning->multi_previous = g_ptEventRunning;
//      semp->semp_sync = g_ptEventRunning;
//  }else
//  {
//      event = semp->semp_sync;
//      if(semp->sync_order == CN_BLOCK_PRIO)   //同步队列按优先级排序
//      {
//          do
//          { //找到一个优先级低于新事件的事件.
//              if(event->prio <= g_ptEventRunning->prio)
//                  event = event->multi_next;
//              else
//                  break;
//          }while(event != semp->semp_sync);
//          g_ptEventRunning->multi_next = event;
//          g_ptEventRunning->multi_previous = event->multi_previous;
//          event->multi_previous->multi_next = g_ptEventRunning;
//          event->multi_previous = g_ptEventRunning;
//          if(semp->semp_sync->prio > g_ptEventRunning->prio)
//              semp->semp_sync = semp->semp_sync->multi_previous;
//      }else                               //按先后顺序，新事件直接排在队尾
//      {
//          g_ptEventRunning->multi_next = event;
//          g_ptEventRunning->multi_previous = event->multi_previous;
//          event->multi_previous->multi_next = g_ptEventRunning;
//          event->multi_previous = g_ptEventRunning;
//      }
//  }
//  if(timeout != CN_TIMEOUT_FOREVER)
//  {
//      //事件状态设为等待信号量 +  超时
//      g_ptEventRunning->event_status = CN_STS_WAIT_SEMP + CN_STS_SYNC_TIMEOUT;
//      __Djy_AddToDelay(timeout);
//  }else
//  {
//      g_ptEventRunning->event_status = CN_STS_WAIT_SEMP;  //事件状态设为等待信号量
//  }

    Int_RestoreAsynSignal();  //恢复中断，将触发上下文切换
    //检查从哪里返回，是超时还是同步事件完成。
    if(g_ptEventRunning->wakeup_from & CN_STS_SYNC_TIMEOUT)
    {//说明同步条件未到，从超时返回，应从目标事件的同步队列中取出事件。
     //此时，被同步的事件肯定还没有完成。
        return false;
    }else
    {//说明是得到信号量返回
        return true;
    }
}

//----删除内核信号量-----------------------------------------------------------
//功能：删除一个信号量，与semp_create_s函数对应
//参数：semp，被删除的信号量
//返回：无
//-----------------------------------------------------------------------------
bool_t Lock_SempDelete_s(struct SemaphoreLCB *semp)
{
    bool_t result;
    if(semp == NULL)    //参数错误
        return false;
    Int_SaveAsynSignal();
    if(semp->semp_sync == NULL)     //没有事件在等待信号灯
    {
        OBJ_Del(&semp->node);  //删除信号量结点
        result = true;
    }else
        result = false;
    Int_RestoreAsynSignal();
    return result;
}

//----删除一个信号量-----------------------------------------------------------
//功能：删除一个信号量
//参数：semp，被删除的信号量
//返回：无
//-----------------------------------------------------------------------------
bool_t Lock_SempDelete(struct SemaphoreLCB *semp)
{
    bool_t result;
    if(semp == NULL)                //参数错误
        return false;
    Int_SaveAsynSignal();
    if(semp->semp_sync == NULL)     //没有事件在等待信号灯
    {
        OBJ_Del(&semp->node);  //删除信号量结点
        Mb_Free(g_ptLockPool,semp); //释放内存
        result = true;
    }else
        result = false;
    Int_RestoreAsynSignal();
    return result;
}

//----查询信号量容量-----------------------------------------------------------
//功能：查询一个信号量包含信号灯的总数
//参数：semp，被查询的信号量
//返回：信号灯总数
//-----------------------------------------------------------------------------
u32 Lock_SempQueryCapacital(struct SemaphoreLCB *semp)
{
    u32 result;
    if(semp == NULL)            //参数错误
        return 0;
    else
    {
#if (32 > CN_CPU_BITS)
        atom_low_t atom_low;
        atom_low = Int_LowAtomStart();
#endif
        result = semp->lamps_limit;     //返回已使用的信号灯数量
#if (32 > CN_CPU_BITS)
        Int_LowAtomEnd();
#endif
    }
    return result;
}

//----查询可用信号灯数量-------------------------------------------------------
//功能：查询一个信号量可用信号灯的数量
//参数：semp，被查询的信号量
//返回：信号灯总数
//-----------------------------------------------------------------------------
u32 Lock_SempQueryFree(struct SemaphoreLCB *semp)
{
    u32 result;
    if(semp == NULL)            //参数错误
        return 0;
    else
    {
#if (32 > CN_CPU_BITS)
        atom_low_t atom_low;
        atom_low = Int_LowAtomStart();
#endif
        result = semp->lamp_counter;     //返回已使用的信号灯数量
#if (32 > CN_CPU_BITS)
        Int_LowAtomEnd();
#endif
    }
    return result;
}

//----查询阻塞-----------------------------------------------------------
//功能：查询该信号量是否有事件被阻塞
//参数：mutex，被查询的互斥量
//返回：true = 有，false = 没有
//-----------------------------------------------------------------------------
bool_t Lock_SempCheckBlock(struct SemaphoreLCB *Semp)
{
    if(Semp == NULL)    //参数错误
        return false;
    if(NULL != Semp->semp_sync)
        return true;
    else
        return false;
}

//----设置同步队列排序方式-----------------------------------------------------
//功能: 设置等待信号量的事件的排序方式，可设为按发出请求的时间排序，或者按被阻塞
//      的事件的优先级排序。信号量初创建的时候，默认是按优先级排序的。
//      该设置只影响调用本函数后加入队列的事件，原来在队列中的事件不受影响。
//参数: semp: 被设置的信号量
//      order: CN_BLOCK_PRIO=优先级排队，sort_time=先后顺序排队
//返回: 无
//-----------------------------------------------------------------------------
void Lock_SempSetSyncSort(struct SemaphoreLCB *semp,u32 order)
{
    if(NULL == semp)
        return;
    semp->sync_order = order;
}
//-----------------------下面是互斥量------------------------------------------

//----创建一个信号量-----------------------------------------------------------
//功能：建立一个互斥量。
//参数：name，互斥量的名字，所指向的字符串内存区不能是局部变量，可以是空
//返回：新建立的互斥量指针
//-----------------------------------------------------------------------------
struct MutexLCB *Lock_MutexCreate(const char *name)
{
    struct MutexLCB *mutex;
    mutex = Mb_Malloc(g_ptLockPool,0);
    if(mutex == NULL)
        return NULL;
    mutex->enable = 0;   //互斥量的初始状态是可用的,否则必须指定onwer
    mutex->mutex_sync = NULL;
//  mutex->prio_bak = CN_PRIO_INVALID;
    mutex->owner = NULL;
    //把新节点挂到信号量根节点下
    OBJ_AddChild(pg_mutex_rsc,&mutex->node,
                        sizeof(struct MutexLCB),RSC_MUTEX,name);
    return mutex;
}

//----创建互斥量---------------------------------------------------------------
//功能：创建一个互斥量，调用者须提供互斥量控制块，高可靠性的应用中，不应该使用动
//      态分配的方式，静态定义更可靠，然后把指针传递过来。内核中使用的互斥量，都
//      是使用静态定义的。应用程序配置cfg_locks_limit时，只需要考虑自己的需求就
//      可以了。
//参数：mutex,互斥量指针
//      name，互斥量的名字，所指向的字符串内存区不能是局部变量，可以是空
//返回：新建立的互斥量指针
//-----------------------------------------------------------------------------
struct MutexLCB *Lock_MutexCreate_s( struct MutexLCB *mutex,const char *name)
{
    if(mutex == NULL)
        return NULL;
    mutex->enable = 0;   //互斥量的初始状态是可用的,否则必须指定onwer
    mutex->mutex_sync = NULL;
//  mutex->prio_bak = CN_PRIO_INVALID;
    mutex->owner = NULL;
    //把新节点挂到信号量根节点下
    OBJ_AddChild(pg_mutex_rsc,&mutex->node,
                        sizeof(struct MutexLCB),RSC_MUTEX,name);
    return mutex;
}

//----释放一个互斥量-----------------------------------------------------------
//功能：释放互斥量，只有互斥量的拥有者才能释放互斥量。
//参数：mutex,互斥量指针
//返回：无
//-----------------------------------------------------------------------------
void Lock_MutexPost(struct MutexLCB *mutex)
{
    struct EventECB *event;

    if(mutex == NULL)
        return;
    if(Int_GetRunLevel() >0)      //异步信号中释放互斥量
    {
        if(mutex->enable == -1)
        {
            mutex->enable = 0;
        }
        return;
    }
    else
    {
        if(mutex->enable == -1)
        {
            return;                        //程序应该不会走到这里，容错一下
        }
    }
    if(mutex->owner != g_ptEventRunning)    //互斥量只能由拥有者释放
        return;
    Int_SaveAsynSignal();
    if(mutex->enable > 0)
        mutex->enable--;
    else
        event = NULL;
    if(mutex->enable == 0)
    {
        if(mutex->mutex_sync == NULL)     //等待队列空，设置互斥量为可用状态
        {
            mutex->owner = NULL;
        } else      //有事件在等待，把互斥量直接转交给队列中优先级最高的事件
        {
            event = mutex->mutex_sync;
            event->sync_head = NULL;
            mutex->owner = event;
            mutex->enable = 1;
            if(event->multi_next == event)
            {//队列中只有一个事件
                mutex->mutex_sync = NULL;
            }else
            {//队列中有多个事件，取出队列头部的事件
                mutex->mutex_sync = event->multi_next;
                event->multi_next->multi_previous = event->multi_previous;
                event->multi_previous->multi_next = event->multi_next;
                event->multi_next = NULL;
                event->multi_previous = NULL;
            }
            if(event->event_status & CN_STS_SYNC_TIMEOUT)
                __Djy_ResumeDelay(event);    //如果事件在超时等待队列中，取出
            event->event_status = CN_STS_EVENT_READY;
            event->wakeup_from = CN_STS_WAIT_MUTEX;
//          if( (mutex->prio_bak != CN_PRIO_INVALID)  //该互斥量发生了优先级继承
//             ||(!Djy_IsEventPrioChanged(event->event_id))) //且无主动改变优先级
            Djy_RestorePrio( );
            __Djy_EventReady(event);
        }
    }
    Int_RestoreAsynSignal();
}

//----请求互斥量-----------------------------------------------------------
//功能：请求互斥量，若成功则拥有该互斥量。由于有owner成员，互斥量不适合在事件之
//      间或事件与异步信号之间同步。只有存在owner成员，才能实现优先级继承。
//      如果在异步信号中请求互斥量，则不会启动阻塞机制，也不会有优先级继承。
//参数：mutex,互斥量指针
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//      超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回：true=取得信号返回或资源不受信号灯保护(mutex == NULL)，mutex==NULL时返回
//      true是有意义的，如果你希望代码在不管是否受信号灯保护都保持一致，把semp
//      设为NULL是明智的选择，比如你要构建一个不受信号灯保护的设备，可把该设备
//      struct pan_device的semp成员设为NULL。
//      false=没有取得信号(超时返回或其他原因)
//-----------------------------------------------------------------------------
bool_t Lock_MutexPend(struct MutexLCB *mutex,u32 timeout)
{
    struct EventECB *pl_ecb;
    bool_t lamp,sch;

    if(mutex == NULL)
    {
        if(Int_GetRunLevel() == 0)
            g_ptEventRunning->error_no = EN_LOCK_PARA_ERROR;
        return false;
    }
    //异步信号中请求互斥量，不做阻塞处理
    if(Int_GetRunLevel() >0)
    {
        if(mutex->enable == 0)
        {
            mutex->enable = -1;
            return true;
        }
        else
        {
            g_ptEventRunning->error_no = EN_LOCK_BLOCK_IN_INT;
            return false;
        }
    }
    else
    {
        if(mutex->enable == -1)
        {
            return false;                  //程序应该不会走到这里，容错一下
        }
    }
    if(g_ptEventRunning == mutex->owner)
    {
        mutex->enable++;
        return true;
    }
    sch = Djy_QuerySch();
    Int_SaveAsynSignal();
    if(mutex->enable == 0)   //信号灯可用
    {
        lamp = true;
        mutex->enable = 1;
//      mutex->prio_bak = CN_PRIO_INVALID;
        mutex->owner = g_ptEventRunning;
    }else                       //信号灯不可用
    {
        lamp = false;
    }

    if(lamp == true)
    {
        Int_RestoreAsynSignal();  //恢复中断，将触发上下文切换
        return true;    //取得互斥量返回
    }
    else if(!sch || (timeout == 0))//禁止调度时,不能阻塞.或timeout=0
    {
        Int_RestoreAsynSignal();  //恢复中断，将触发上下文切换
        return false;   //没有取得互斥量返回
    }

    __Djy_AddRunningToBlock(&(mutex->mutex_sync),CN_BLOCK_PRIO,timeout,CN_STS_WAIT_MUTEX);
//  __Djy_CutReadyEvent(g_ptEventRunning);
//  g_ptEventRunning->previous = NULL;
//  g_ptEventRunning->next = NULL;
//
//  g_ptEventRunning->sync_head = &mutex->mutex_sync;
//  if(mutex->mutex_sync == NULL)
//  {//同步队列空,running事件自成双向循环链表
//      g_ptEventRunning->multi_next = g_ptEventRunning;
//      g_ptEventRunning->multi_previous = g_ptEventRunning;
//      mutex->mutex_sync = g_ptEventRunning;
//  }else
//  {//同步队列非空,按优先级排序
//      pl_ecb = mutex->mutex_sync;
//      do
//      { //找到一个优先级低于新事件的事件.
//          if(pl_ecb->prio <= g_ptEventRunning->prio)
//              pl_ecb = pl_ecb->multi_next;
//          else
//              break;
//      }while(pl_ecb != mutex->mutex_sync);
//      g_ptEventRunning->multi_next = pl_ecb;
//      g_ptEventRunning->multi_previous = pl_ecb->multi_previous;
//      pl_ecb->multi_previous->multi_next = g_ptEventRunning;
//      pl_ecb->multi_previous = g_ptEventRunning;
//      if(mutex->mutex_sync->prio > g_ptEventRunning->prio)
//          mutex->mutex_sync = mutex->mutex_sync->multi_previous;
//  }
//  if(timeout != CN_TIMEOUT_FOREVER)
//  {
//      //事件状态设为等待信号量 + 超时
//      g_ptEventRunning->event_status = CN_STS_WAIT_MUTEX +CN_STS_SYNC_TIMEOUT;
//      __Djy_AddToDelay(timeout);
//  }else
//  {
//      g_ptEventRunning->event_status = CN_STS_WAIT_MUTEX;  //事件状态设为等待信号量
//  }

    //下面看看是否要做优先级继承
    pl_ecb = mutex->owner;
    Djy_RaiseTempPrio(pl_ecb->event_id);
//    if(pl_ecb->prio > g_ptEventRunning->prio)  //需要继承优先级
//    {
////        Djy_SetEventTempPrio(pl_ecb->event_id,g_ptEventRunning->prio);
//        pl_ecb->prio = g_ptEventRunning->prio;
//    }
    Int_RestoreAsynSignal();  //恢复中断，将触发上下文切换
    //检查从哪里返回，是超时还是同步事件完成。
    if(g_ptEventRunning->wakeup_from & CN_STS_SYNC_TIMEOUT)
    {//说明同步条件未到，从超时返回，应从目标事件的同步队列中取出事件。
     //此时，被同步的事件肯定还没有完成。
        return false;
    }else
    {//说明是得到互斥量返回
        return true;
    }
}

//----删除互斥量-----------------------------------------------------------
//功能：删除一个信号量，与mutex_create_s函数对应
//参数：mutex，被删除的互斥量
//返回：true=成功删除，false=失败
//-----------------------------------------------------------------------------
bool_t Lock_MutexDelete_s(struct MutexLCB *mutex)
{
    bool_t result;
    if(mutex == NULL)    //参数错误
        return false;
    Int_SaveAsynSignal();
    if(mutex->mutex_sync == NULL)     //没有事件在等待互斥量
    {
        OBJ_Del(&mutex->node);  //删除互斥量结点
        result = true;
    }else
        result = false;
    Int_RestoreAsynSignal();
    return result;
}

//----删除一个互斥量-----------------------------------------------------------
//功能：删除一个互斥量
//参数：mutex，被删除的互斥量
//返回：无
//-----------------------------------------------------------------------------
bool_t Lock_MutexDelete(struct MutexLCB *mutex)
{
    bool_t result;
    if(mutex == NULL)    //参数错误
        return false;
    Int_SaveAsynSignal();
    if(mutex->mutex_sync == NULL)     //没有事件在等待互斥量
    {
        OBJ_Del(&mutex->node);  //删除互斥量结点
        Mb_Free(g_ptLockPool,mutex); //释放内存
        result = true;
    }else
        result = false;
    Int_RestoreAsynSignal();
    return result;
}

//----查询一个互斥量-----------------------------------------------------------
//功能：查询该互斥量是否可用
//参数：mutex，被查询的互斥量
//返回：true = 可用，false = 不可用
//-----------------------------------------------------------------------------
bool_t Lock_MutexQuery(struct MutexLCB *mutex)
{
    if(mutex == NULL)    //参数错误
        return false;
    return ! mutex->enable;
}

//----查询阻塞-----------------------------------------------------------
//功能：查询该互斥量是否有事件被阻塞
//参数：mutex，被查询的互斥量
//返回：true = 有，false = 没有
//-----------------------------------------------------------------------------
bool_t Lock_MutexCheckBlock(struct MutexLCB *mutex)
{
    if(mutex == NULL)    //参数错误
        return false;
    if(NULL != mutex->mutex_sync)
        return true;
    else
        return false;
}

//----查询一个互斥量-----------------------------------------------------------
//功能：查询该互斥量的拥有者
//参数：mutex，被查询的互斥量
//返回：拥有者的事件ID
//-----------------------------------------------------------------------------
u16 Lock_MutexGetOwner(struct MutexLCB *mutex)
{
    if(mutex == NULL)    //参数错误
        return CN_EVENT_ID_INVALID;
    if( NULL != mutex->owner)
        return mutex->owner->event_id;
    else
        return CN_EVENT_ID_INVALID;
}

void __Lock_ShowLock(void)
{
    struct Object *current_node,*lock;
    struct EventECB *pl_ecb;
    char *Name;
    current_node = OBJ_SearchTree("semaphore");
    lock = current_node;
    printf("信号量 semaphore 列表：\r\n");
    printf("类型  信号数上限  可用信号数  阻塞事件  名字\r\n");
    while(1)
    {
        current_node = OBJ_TraveScion(lock,current_node);
        if(current_node == NULL)
        {
            break;
        }
        if(((struct SemaphoreLCB *)current_node)->sync_order
                                        == CN_BLOCK_PRIO)
        {
            printf("prio  ");
        }
        else
        {
            printf("fifo  ");
        }
        printf("%8d    %8d    ",
            ((struct SemaphoreLCB *)current_node)->lamps_limit,
            ((struct SemaphoreLCB *)current_node)->lamp_counter);
        pl_ecb = ((struct SemaphoreLCB *)current_node)->semp_sync;
        if(pl_ecb)
            printf("%05d     ",pl_ecb->event_id);
        else
            printf("无        ");

        Name = OBJ_Name(current_node);
        if(Name != NULL)
        {
            printf("%s\r\n", Name);
        }else
        {
            printf("无名信号量\r\n");
        }
    }

    current_node = OBJ_SearchTree("mutex");
    lock = current_node;
    printf("\n\r互斥量 mutex 列表：\r\n");
    printf("状态  拥有者  原优先级  现优先级  阻塞事件  名字\r\n");
    while(1)
    {
        current_node = OBJ_TraveScion(lock,current_node);
        if(current_node == NULL)
        {
            break;
        }
        if(((struct MutexLCB *)current_node)->enable == 0)
        {
            printf("可用                              ");
        }
        else
        {
            printf("占用  ");
            pl_ecb = ((struct MutexLCB *)current_node)->owner;
            if(pl_ecb->prio_base != pl_ecb->prio)
            {
                printf("%05d   %03d       ",pl_ecb->event_id,
                        pl_ecb->prio_base);
                printf("%03d       ",pl_ecb->prio);
            }
            else
            {
                printf("%05d   未发生优先级继承    ",pl_ecb->event_id);
            }
        }

        pl_ecb = ((struct MutexLCB *)current_node)->mutex_sync;
        if(pl_ecb)
            printf("%05d     ",pl_ecb->event_id);
        else
            printf("无        ");

        Name = OBJ_Name(current_node);
        if(Name != NULL)
        {
            printf("%s\r\n", Name);
        }else
        {
            printf("无名互斥量\r\n");
        }
    }
    printf("\n\r用户配置锁控制块总数:%d，空闲数: %d \r\n",
            gc_u32CfgLockLimit,Mb_QueryFree(g_ptLockPool));
}
