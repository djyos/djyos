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
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __semp_h__
#define __semp_h__

#include "rsc.h"
#include "errno.h"
#ifdef __cplusplus
extern "C" {
#endif


struct  tagEventECB;

// 当事件请求信号量不得，将进入该信号量的同步队列，等待持有信号量的事件释放信号
// 量，等待队列有两种排序方式，即优先级顺序和时间顺序方式。
// 只有信号量可以用先后顺序，互斥量只能用优先级排序方式。
#define CN_SEMP_BLOCK_FIFO          0       //按请求信号量的先后顺序排队
#define CN_SEMP_BLOCK_PRIO          1       //按请求信号量的事件的优先级排队

//---------不限量信号的意义--------------
//1、不限量信号可提供受保护的资源的使用计数
//2、为可限量也可不限量的资源提供一致的代码，尤其是提供在运行时配置的可能性
struct tagSemaphoreLCB
{
    struct  tagRscNode node;
    u32 sync_order;
    u32 lamps_limit;   //信号灯数量上限，cn_limit_uint32表示不限数量
    u32 lamp_counter;  //可用信号灯数量。
    struct  tagEventECB *semp_sync;    //等候信号的事件队列
};

//互斥量是可以递归请求的，即同一个事件允许反复请求，但要求释放的次数和请求的次数
//相同。即如果互斥量被一个事件连续请求三次，则要释放三次才算真正释放。
struct tagMutexLCB
{
    struct tagRscNode node;
    s32  enable;                        //0=可用，>0 = 被线程占用，<0 = 中断占用
    ufast_t  prio_bak;                  //优先级继承中备份原优先级
    struct  tagEventECB *owner;         //占用互斥量的事件，若被中断占用，则无效
    struct  tagEventECB *mutex_sync;    //等候互斥量的事件队列，优先级排队
};

//用于信号量和互斥量共享内存池
union lock_MCB
{
   struct tagSemaphoreLCB sem;
   struct tagMutexLCB  mut;
};

//出错信息定义
enum _LOCK_ERROR_CODE_
{
    EN_LOCK_NO_ERROR = CN_LOCK_NO_ERROR,      //没有错误
    EN_LOCK_PARA_ERROR,             //函数参数检查错误。
    EN_LOCK_RANGE_ERROR,            //信号量范围超过限制
    EN_LOCK_BLOCK_IN_INT,           //中断ISR不允许阻塞
};

ptu32_t ModuleInstall_Lock1(ptu32_t para);
ptu32_t ModuleInstall_Lock2(ptu32_t para);
struct tagSemaphoreLCB *Lock_SempCreate(u32 lamps_limit,u32 init_lamp,
                                        u32 sync_order,char *name);
struct tagSemaphoreLCB *Lock_SempCreate_s( struct tagSemaphoreLCB *semp,
                       u32 lamps_limit,u32 init_lamp,u32 sync_order,char *name);
void Lock_SempPost(struct tagSemaphoreLCB *semp);
bool_t Lock_SempPend(struct tagSemaphoreLCB *semp,u32 timeout);
bool_t Lock_SempDelete_s(struct tagSemaphoreLCB *semp);
bool_t Lock_SempDelete(struct tagSemaphoreLCB *semp);
u32 Lock_SempQueryCapacital(struct tagSemaphoreLCB *semp);
bool_t Lock_SempCheckBlock(struct tagSemaphoreLCB *Semp);
u32 Lock_SempQueryFree(struct tagSemaphoreLCB *semp);
void Lock_SempSetSyncSort(struct tagSemaphoreLCB *semp,u32 order);
struct tagMutexLCB *Lock_MutexCreate(char *name);
struct tagMutexLCB *  Lock_MutexCreate_s( struct tagMutexLCB *mutex,char *name);
void Lock_MutexPost(struct tagMutexLCB *mutex);
bool_t Lock_MutexPend(struct tagMutexLCB *mutex,u32 timeout);
bool_t Lock_MutexDelete(struct tagMutexLCB *mutex);
bool_t Lock_MutexDelete_s(struct tagMutexLCB *mutex);
bool_t Lock_MutexQuery(struct tagMutexLCB *mutex);
bool_t Lock_MutexCheckBlock(struct tagMutexLCB *mutex);
u16 Lock_MutexGetOwner(struct tagMutexLCB *mutex);

#ifdef __cplusplus
}
#endif

#endif //__semp_h__

