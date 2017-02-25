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
//所属模块:核心模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:事件类型、事件管理以及多事件调度相关的代码全在这里了。
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __djyos_H__
#define __djyos_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include "errno.h"
#include "arch_feature.h"
//结构类型声明,来自其他文件定义的结构
struct SemaphoreLCB;
//结构类型声明,本文件定义的结构
struct ThreadVm;
struct EventECB;
struct EventType;

#define CN_BLOCK_FIFO          0       //按进入阻塞队列的先后顺序排队
#define CN_BLOCK_PRIO          1       //按阻塞队列中事件的优先级排队

// todo:这里命名为ContainerOf就报错
#define Container(Ptr, Type, Member)  ((Type *)((char *)(Ptr)-(unsigned long)(&((Type *)0)->Member)))/* 引自Linux */

//无论事件还是事件类型id，都小于0x8000。0x8000以上的数有特殊用途，例如内存管理
//模块中用来标识内存page的分配情况，具体见struct mem_global的index_event_id成员
//的注释
#define CN_EVENT_ID_INVALID CN_LIMIT_UINT16
#define CN_EVTT_ID_INVALID  CN_LIMIT_UINT16     //非法事件类型号
#define CN_EVTT_ID_ASYN     (CN_LIMIT_UINT16-1) //异步信号事件类型标志
#define CN_EVENT_ID_ASYN    (CN_LIMIT_UINT16-1) //异步信号事件ID标志
#define CN_EVENT_ID_BASE    0           //最小事件id号
#define CN_EVENT_ID_LIMIT   16383       //最大事件id号
#define CN_EVTT_ID_BASE     16384       //最小事件类型id号
#define CN_EVTT_ID_LIMIT    32767       //最大事件类型id号
#define CN_EVTT_ID_MASK     0x4000      //事件类型号的基数

#define CN_SYNC_SUCCESS     0       //同步条件达成
#define CN_SYNC_TIMEOUT     1       //同步条件未达成，超时返回，或timeout设为0
#define CN_SYNC_ERROR       CN_LIMIT_UINT32    //出错

//出错信息定义
enum _KNL_ERROR_CODE_
{
    EN_KNL_NO_ERROR = CN_KNL_NO_ERROR,  //没有错误
    EN_KNL_ETCB_ERROR,            //事件类型控制块错误
    EN_KNL_ECB_EXHAUSTED,         //事件控制块耗尽
    EN_KNL_ETCB_EXHAUSTED,        //事件类型控制块耗尽
    EN_KNL_PCB_EXHAUSTED,         //参数控制块耗尽
    EN_KNL_CANT_SCHED,            //企图在禁止调度时执行可能引起事件调度的操作
    EN_KNL_INVALID_PRIO,          //非法优先级
    EN_KNL_VPU_OVER,              //事件类型的线程超过限制数
    EN_KNL_PARA_OVER,             //事件参数数量超过限制数
    EN_KNL_EVTT_HOMONYMY,         //事件类型重名
    EN_KNL_EVENT_FREE,            //企图使用空闲事件控制块
    EN_KNL_EVTT_FREE,             //企图使用空闲事件类型控制块
    EN_KNL_EVTTID_LIMIT,          //事件类型id越限
    EN_KNL_EVTT_UNREGISTER,       //事件类型未登记
    EN_KNL_EVENT_SYNC_EXIT,       //事件同步中，被同步的事件异常结束
    EN_KNL_MEMORY_OVER,           //内存不足
    EN_KNL_STACK_OVERFLOW         //栈溢出风险警告
};

//事件优先级名称定义
#define CN_PRIO_CRITICAL    (100)
#define CN_PRIO_RLYMAIN     (110)
#define CN_PRIO_RECORD      (112)
#define CN_PRIO_REAL        (130)
#define CN_PRIO_RRS         (200)
#define CN_PRIO_WDT         (1)
#define CN_PRIO_SYS_SERVICE (250)
#define CN_PRIO_INVALID     (255)   //非法优先级

struct ProcessVm       //进程
{
    u8  res;
};

#define CN_SAVE_CONTEXT_INT     1
#define CN_SAVE_CONTEXT_NOINT   0
//有mmu的机器上,地址分配:0~1G操作系统,1~2G进程共享区,2~4G进程私有空间.
//特别注意:本结构要被汇编访问，其成员顺序不能改变，也不能随意增加成员
 struct ThreadVm          //线程数据结构
{
    //线程栈指针,在线程被抢占时保存sp,运行时并不动态跟踪sp变化
    u32    *stack;
    u32    *stack_top;     //线程的栈顶指针
    struct ThreadVm *next;     //用于把evtt的所有空闲线程连成一个单向开口链表
                                //该链表由evtt的my_free_vm指针索引
    u32    stack_size;     //栈深度
    struct ProcessVm *host_vm;  //宿主进程，在si和dlsp模式中为NULL
};

//事件进入运行态的原因.
//事件应该是内闭的,djyos不提供查询任意事件当前状态的功能.查询别人的状态,有干涉
//内政之嫌.
//使用方法:当事件进入运行态时,根据进入前状态设置事件的wakeup_from.
//查询函数根据wakeup_from值判断事件从何种状态进入运行态.
//因优先级低而等待的不予考虑,例如,某事件延时结束后因为优先级低而等待一定时间后
//才运行,查询结果为"闹铃响"
#define CN_STS_EVENT_READY      (u32)0
#define CN_STS_EVENT_DELAY      (u32)(1<<0)     //延时到，不包括Djy_DelayUs
#define CN_STS_SYNC_TIMEOUT     (u32)(1<<1)     //超时
#define CN_STS_WAIT_EVENT_DONE  (u32)(1<<2)     //事件同步
#define CN_STS_EVENT_EXP_EXIT   (u32)(1<<3) //事件同步中被同步的目标事件异常退出
#define CN_STS_WAIT_EVTT_POP    (u32)(1<<4)     //事件类型弹出同步
#define CN_STS_WAIT_EVTT_DONE   (u32)(1<<5)     //事件类型完成同步
#define CN_STS_WAIT_MEMORY      (u32)(1<<6)     //从系统堆分配内存同步
#define CN_STS_WAIT_SEMP        (u32)(1<<7)     //信号量同步
#define CN_STS_WAIT_MUTEX       (u32)(1<<8)     //互斥量同步
#define CN_STS_WAIT_ASYN_SIGNAL (u32)(1<<9)     //异步信号同步
#define CN_STS_WAIT_CPIPE       (u32)(1<<10)    //定长pipe
#define CN_STS_WAIT_VPIPE       (u32)(1<<11)    //动态长度pipe
#define CN_STS_WAIT_MSG_SENT    (u32)(1<<12)    //等待消息发送
#define CN_STS_WAIT_PARA_USED   (u32)(1<<13)    //等待消息处理完成
#define CN_WF_EVTT_DELETED      (u32)(1<<14)    //事件类型相关的同步，因目标类型
                                                //被删除而解除同步。
#define CN_WF_EVENT_RESET       (u32)(1<<15)    //复位后首次切入运行
#define CN_WF_EVENT_NORUN       (u32)(1<<16)    //事件还未开始处理

#define CN_BLOCK_PRIO_SORT      (u32)(1<<17)    //是否在优先级排序的阻塞队列中

#define CN_DB_INFO_EVENT        0
#define CN_DB_INFO_EVTT         1
#define CN_DB_INFO_HEAP         2
#define CN_DB_INFO_HEAPSPY      3
#define CN_DB_INFO_STACK        4
#define CN_DB_INFO_LOCK         5

#define CN_PARA_LIMITED     32      //参数长度上限
#define CN_CREAT_NEW_PARA   0x01    //1=为关联型事件创建新任务，0=覆盖旧任务
#define CN_MALLOC_OVER_32   0x02    //1=参数超过cn_para_limited时，动态分配空间
//说明:
//1、弹出事件时，如果携带参数，系统将创建参数控制块(tagParaPCB)记录该参数。
//2、如果参数尺寸小于cn_para_limited，将直接copy到参数控制块的static_para成员中。
//3、如果大于cn_para_limited，则视调用djy_event_pop函数时的para_options参数而定。
//   详见djy_event_pop函数的注释。
//struct ParaPCB
//{
//    struct ParaPCB *next,*previous;
//    struct EventECB *sync;     //等待本参数所代表的事件处理完成的事件队列，
//                                //若为NULL,则不等待处理，直接返回
////#if(CN_CFG_DEBUG_INFO == 1)
////    s64 ParaStartTime;             //参数产生的时间，uS
////#endif
//    bool_t dynamic_mem;         //true=保存参数的内存是动态申请的
//    void *event_para;
//    //事件参数，参数尺寸少于32字节，直接存在这里，格式由弹出和处理事件的双方协商
//    //确定.已按最严格的对齐，应用程序可以直接把它强制类型转换到任何需要的类型。
//    align_type static_para[32/sizeof(align_type)];
//};
struct EventInfo
{
    s64    EventStartTime;      //事件发生时间，uS
    s64    consumed_time;       //事件消耗的总时间
    u32    error_no;            //本事件执行产生的最后一个错误号
    ptu32_t event_result;       //如果本事件处理时弹出了事件，并且等待处理结果
                                //(即调用pop函数时，timeout !=0)，且正常返回，这
                                //里保存该事件处理结果。
};
struct EventECB
{
    //事件链指针,用于构成下列链表
    //pg_event_free: 特殊，参见文档
    //g_ptEventReady:特殊链表，参见文档
    //g_ptEventDelay:双向循环
    struct EventECB *next,*previous;
    //多功能链表指针，用于连接以下链表:
    //1、各同步队列，比如事件同步，事件类型弹出同步等
    //2、就绪队列中优先级单调队列，双向循环，用以实现轮询和y_event_ready中
    //   的O(1)算法。
    struct EventECB *multi_next,*multi_previous;      //条件同步队列
    struct ThreadVm  *vm;               //处理本事件的线程指针
    ptu32_t param1,param2;                  //事件参数,只保存最后一次弹出传入的参数
//    struct ParaPCB *para_high_prio;   //高优先级参数队列
//    struct ParaPCB *para_low_prio;    //低优先级参数队列
//    struct ParaPCB *para_current;     //当前将要或正在处理的参数。
    struct EventECB *sync;            //同步于本事件的队列，当本事件完成后，
                                        //激活队列中的事件
                                        //与参数同步不一样，参数同步是弹出时用的
    struct EventECB **sync_head;      //记住自己在哪一个同步队列中，以便超时
                                        //返回时从该同步队列取出事件

    s64    EventStartTime;              //事件发生时间，uS
    s64    consumed_time;               //事件消耗的总时间
//#if(CN_CFG_DEBUG_INFO == 1)
    u32    consumed_time_second;        //最近1秒消耗的时间
    u32    consumed_time_record;        //上次整秒时，消耗的时间快照
//#endif
    s64    delay_start_tick;    //设定闹铃时间
    s64    delay_end_tick;      //闹铃响时间
    u32    error_no;            //本事件执行产生的最后一个错误号
    ptu32_t event_result;       //如果本事件处理时弹出了事件，并且等待处理结果
                                //(即调用pop函数时，timeout !=0)，且正常返回，这
                                //里保存该事件处理结果。
    u32    wait_mem_size;       //等待分配的内存数量.
    //以下两行参见CN_STS_EVENT_READY系列定义
    u32 wakeup_from;            //用于查询事件进入就绪态的原因,todo,直接返回状态
    u32 event_status;           //当前状态,本变量由操作系统内部使用,
    ufast_t  prio_base;         //临时调整优先级时，将不改变prio_base
    ufast_t  prio;              //事件优先级
//  ufast_t  prio_new;       //优先级备份，用于修改处于阻塞态的事件优先级时，
                                //暂存新优先级。

    u16    evtt_id;             //事件类型id，0~32767
    u16    sync_counter;        //同步计数
//    u16    paras;               //事件消息队列当前长度

    //事件id范围:0~32767(CN_EVENT_ID_LIMIT)
    u16    event_id;            //事件序列编号,等同于事件在事件块数组中的偏移位置
    u32    local_memory;        //本事件处理过程中申请的局部内存块数
                                //不收回将导致内存泄漏.
};



//事件处理函数出错返回后的处理方案
#define CN_EXIT_ACTION_STOP     0   //删除事件和线程
#define CN_EXIT_ACTION_RESTART  1   //重新开始线程

//事件属性定义表
struct EvttStatus
{
    u16 correlative:1;  //0=表示独立事件,事件队列中可能同时存在多条该类事件
                        //1=同一类型的多次事件间是有关联的，必须协作处理
                        //故事件队列中只允许存在一条该事件
    u16 registered:1;   //0=该事件类型还没有注册,系统将拒绝pop该类型录
                        //1=该事件类型已经注册,可以pop该类型事件
    u16 inuse:1;        //0=所有队列中都没有该类型的事件
                        //1=队列中(包括等待中)至少有一条该类型的事件
    u16 deleting:1;     //0=正常状态，1=等待注销状态。
};

enum enEventRelation
{
    EN_INDEPENDENCE=0,//逻辑上，可称作独立型事件,每次弹出本类型事件，都将分配
                        //独立的事件控制块；从处理过程看，可称作是并行事件，每次
                        //弹出本类型事件都创建新线程进行处理。
    EN_CORRELATIVE,   //逻辑上，可称作关联型事件，同一类型的多次事件间是有关联
                        //的，必须协作处理故事件队列中只允许存在一条该事件。处理
                        //过程看，是串行事件，同一类型事件多次弹出，必须串行处理
                        //每条事件。
};

enum enSwitchType
{
    EN_SWITCH_IN=0,     //上下文切换时切入某事件。
    EN_SWITCH_OUT,      //上下文切换时切离某事件。
};
typedef void (*SchHookFunc)(ucpu_t SchType);

struct EventType
{
    //同一类型的事件可以有多条正在执行或等待调度,但这些事件有相同的属性.
    struct EvttStatus    property;
    //空闲线程指针,分配线程给事件时，优先从这里分配
    struct ThreadVm  *my_free_vm;
    // 事件调度回调函数,在本类型事件上下文切入和切离时调用.
    // 本函数使用的栈,如果是正常调度,则无论切入还是切离,都使用切离事件的栈.
    // 如果是中断引发切换,则使用中断栈.
    // 参数:
    // SchType: EN_SWITCH_IN = 切入,EN_SWITCH_OUT=切离
    // event_id: 切入或切离的事件ID
    SchHookFunc SchHook;
    char evtt_name[32]; //事件类型允许没有名字，但只要有名字，就不允许同名
                        //如果一个类型只在模块内部使用，可以不用名字。
                        //如模块间需要交叉弹出事件，用名字访问。
    //优先级小于0x80为紧急优先级,它影响线程的构建.类型优先级在初始化时设定,
    ufast_t     default_prio;       //事件类型优先级.不同于事件优先级,1~255,0非法.
    u16    events;     //分配的事件总数
    u16    vpus_res;   //系统为本类型事件保留的空闲线程上限，关联型无效
    u16    vpus_limit; //独立型:本类型事件允许同时建立的线程个数
                       //关联型:无效
    u16    vpus;       //独立型:本类型事件已经拥有的线程个数
                       //关联型:无效
//    u16    para_limit; //本类型事件参数控制块队列长度
    ptu32_t (*thread_routine)(void);//函数指针,可能是死循环.
    u32 stack_size;              //thread_routine所需的栈大小
    u32 pop_times;     //本类型事件历史累计弹出次数，超过0xffffffff将回绕0

    struct EventECB *mark_event;
    //关联型事件才有效，指向事件队列中的本类型事件
    //对于独立型事件，该队列仅仅是用于存放那些没有分配到VM的ECB,按照优先级从高
    //到低的顺序

    //这两队列都是以剩余次数排队的双向循环链表
    struct EventECB *done_sync,*pop_sync;//弹出同步和完成同步队列头指针,
};

//就绪队列(优先级队列),始终执行队列头部的事件,若有多个优先级相同,轮流执行
extern struct EventECB  *g_ptEventReady;
extern struct EventECB  *g_ptEventRunning;   //当前正在执行的事件
extern bool_t g_bScheduleEnable;

void Djy_IsrTick(u32 inc_ticks);
void Djy_SetRRS_Slice(u32 slices);
u32 Djy_GetRRS_Slice(void);
void Djy_CreateProcessVm(void);
u16 Djy_EvttRegist(enum enEventRelation relation,
                        ufast_t default_prio,
                        u16 vpu_res,
                        u16 vpus_limit,
//                        u16 para_limit,
                        ptu32_t (*thread_routine)(void),
                        void *Stack,
                        u32 stack_size,
                        char *evtt_name);
u16 Djy_GetEvttId(char *evtt_name);
bool_t Djy_EvttUnregist(u16 evtt_id);
bool_t Djy_QuerySch(void);
bool_t Djy_IsMultiEventStarted(void);
bool_t Djy_SetEventPrio(u16 event_id,ufast_t new_prio);
bool_t Djy_RaiseTempPrio(u16 event_id);
bool_t Djy_RestorePrio(void);
u32 Djy_EventDelay(u32 u32l_uS);
u32 Djy_EventDelayTo(s64 s64l_uS);
u32 Djy_WaitEventCompleted(u16 event_id,u32 timeout);
u32 Djy_WaitEvttCompleted(u16 evtt_id,u16 done_times,u32 timeout);
u32 Djy_WaitEvttPop(u16 evtt_id,u32 *base_times, u32 timeout);
u16 Djy_EventPop(  u16  evtt_id,
                    u32 *pop_result,
                    u32 timeout,
                    ptu32_t PopPrarm1,
                    ptu32_t PopPrarm2,
                    ufast_t prio);
u32 Djy_GetEvttPopTimes(u16 evtt_id);
ptu32_t Djy_GetEventResult(void);
void Djy_GetEventPara(ptu32_t *Param1,ptu32_t *Param2);
void Djy_EventExit(struct EventECB *event, u32 exit_code,u32 action);
void Djy_EventComplete(ptu32_t result);
void Djy_EventSessionComplete(ptu32_t result);
//void Djy_ParaUsed(ptu32_t result);
u32 Djy_WakeUpFrom(void);
u16 Djy_MyEvttId(void);
u16 Djy_MyEventId(void);
void Djy_ApiStart(u32 api_no);
void Djy_DelayUs(u32 time);

bool_t Djy_GetEventInfo(u16 id, struct EventInfo *info);
bool_t Djy_GetEvttName(u16 evtt_id, char *dest, u32 len);
bool_t Djy_RegisterHook(u16 EvttID,SchHookFunc HookFunc);

#ifdef __cplusplus
}
#endif
#endif //__djyos_H__
