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
//所属模块: multiplex模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:实现多路复用的功能
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-06-24
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stddef.h"
#include "systime.h"
#include "object.h"
#include "pool.h"
#include "lock.h"
#include "multiplex.h"
#define CN_MULTIPLEX_ACTIVED   (1<<31)  //MultiplexSets是否已经触发，只有type设置
//为CN_MULTIPLEX_OBJECT_AND有效，在OR模式
//下，用ActiveQ判断即可

static struct MemCellPool *g_ptMultiplexSetsPool;
static struct MemCellPool *g_ptMultiplexObjectPool;
static struct MutexLCB MultiplexMutex;

//把Object从*SrcList队列取出，放到*TarList队列头上去
static void __ChangeList(struct MultiplexObjectCB **SrcList,
    struct MultiplexObjectCB **TarList,
    struct MultiplexObjectCB *Object)
{
    Object->NextObject->PreObject = Object->PreObject;
    Object->PreObject->NextObject = Object->NextObject;
    if (*SrcList == Object) {
        if (Object->NextObject == Object) {
            *SrcList = NULL;
        } else {
            *SrcList = Object->NextObject;
        }
    }
    //把Object放到Active队列中
    if (*TarList == NULL) {
        *TarList = Object;
        Object->NextObject = Object;
        Object->PreObject = Object;
    } else {
        Object->PreObject = (*TarList)->PreObject;
        Object->NextObject = *TarList;
        (*TarList)->PreObject->NextObject = Object;
        (*TarList)->PreObject = Object;

        *TarList = Object;
    }
}

//判断一个Object是否已经触发
bool_t __ObjectIsActived(u32 Pending, u32 Sensing, u32 Mode)
{
    if (Mode == CN_MULTIPLEX_SENSINGBIT_OR) {
        if ((Pending & Sensing) != 0)
            return true;
        else
            return false;
    }
    else
    {
        if ((Pending & Sensing) == Sensing)
            return true;
        else
            return false;
    }
}

//----模块初始化---------------------------------------------------------------
//功能: 初始化Multiplex模块。创建互斥量，初始化内存池。
//参数：para，无效。
//返回：0=成功，非0=失败。
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Multiplex(ptu32_t para)
{
    Lock_MutexCreate_s(&MultiplexMutex, "Multiplex control block");
    //创建MultiplexSets控制块内存池，内存池的初始状态是空的。
    g_ptMultiplexSetsPool = Mb_CreatePool(NULL, 0, sizeof(struct MultiplexSetsCB),
        5, CN_MULTIPLEX_MAX_SETS, "Multiplex control block");
    if (g_ptMultiplexSetsPool == NULL)
        return -1;
    //创建Multiplex对象控制块内存池，内存池的初始状态是空的。
    g_ptMultiplexObjectPool = Mb_CreatePool(NULL, 0, sizeof(struct MultiplexObjectCB),
        5, CN_MULTIPLEX_MAX_OBJECTS, "Multiplex control block");
    if (g_ptMultiplexObjectPool == NULL) {
        Mb_DeletePool(g_ptMultiplexObjectPool);
        return -1;
    }
    return 0;
}

//----创建MultiplexSets----------------------------------------------------------
//功能：创建一个MultiplexSets，并且设定属性。
//参数：ActiveLevel, 触发水平,该集合被触发的对象达到此数后,触发集合.
//返回：新创建的MultiplexSets指针
//----------------------------------------------------------------------------
struct MultiplexSetsCB* Multiplex_Creat(u32 ActiveLevel)
{
    struct MultiplexSetsCB *SetsQ;
    if (ActiveLevel == 0)       //触发水平必须大于0
        return NULL;
    SetsQ = (struct MultiplexSetsCB *)Mb_Malloc(g_ptMultiplexSetsPool,
        CN_TIMEOUT_FOREVER);
    if (SetsQ != NULL) {
        SetsQ->ObjectQ = NULL;
        SetsQ->ActiveQ = NULL;
        SetsQ->ObjectSum = 0;
        SetsQ->ActiveLevel = ActiveLevel;
        SetsQ->Actived = 0;
        SetsQ->SetsActived = false;
        Lock_SempCreate_s(&SetsQ->Lock, 1, 0, CN_SEMP_BLOCK_FIFO, NULL);
        return SetsQ;
    } else
        return NULL;
}

//----添加对象到MultiplexSets--------------------------------------------------
//功能: MultiplexSets中添加一个对象。如果该Object的初始状态是已经触发，则加入到
//      ActiveQ队列，否则加入ObjectQ队列。
//参数: Sets，被操作的MultiplexSets指针
//      ObjectHead，被操作的Object队列头指针的指针，*ObjectHead=NULL表示该对象尚
//          未加入任何MultiplexSets，因此，第一次调用Multiplex_AddObject前,
//          *ObjectHead初始化值必须是NULL。允许多次调用Multiplex_AddObject把一个
//          Object 允许加入多个MultiplexSets.每加入一个MultiplexSets，将增加一个
//          struct MultiplexObjectCB *类型的结点，所有结点的NextSets指针连接
//          成一个单向链表，*ObjectHead指向该链表头。*ObjectHead再也不允许在外部
//          修改，否则结果不可预料。
//      ObjectStatus，加入时的状态，31bit的位元组,bit31无效
//      ObjectID，被Multiplex的对象的ID,由使用者自己解析,Multiplex模块不处理。
//      SensingBit，对象敏感位标志，31个bit，设为1表示本对象对这个bit标志敏感
//          bit31表示敏感类型，CN_SENSINGBIT_AND，或者CN_SENSINGBIT_OR
//返回: true=成功，false=失败。
//-----------------------------------------------------------------------------
bool_t Multiplex_AddObject(struct MultiplexSetsCB *Sets,
                            struct MultiplexObjectCB **ObjectHead,
                            u32 ObjectStatus, ptu32_t ObjectID, u32 SensingBit)
{
    struct MultiplexObjectCB *temp;
    struct MultiplexObjectCB **TargetQ;
    bool_t repeat = false;
    u32 ActivedInc = 0;
    if (Sets == NULL)
        return false;
    Lock_MutexPend(&MultiplexMutex, CN_TIMEOUT_FOREVER);
    temp = *ObjectHead;
    //循环检查一个Object是否重复加入同一个MultiplexSets
    //如果ObjectHead=NULL,检查结果是不重复，后续处理能够正确运行。
    while (temp != NULL) {
        if (temp->MySets != Sets)
            temp = temp->NextSets;
        else {
            repeat = true;
            break;
        }
    }
    Lock_MutexPost(&MultiplexMutex);

    if (repeat == false) {
        ObjectStatus &= ~CN_MULTIPLEX_SENSINGBIT_MODE;      //清除模式位
        //下面检查新加入的Object是否已经触发，以决定加入到MultiplexSets的哪个队列中
        if (__ObjectIsActived(ObjectStatus,
                SensingBit & ~CN_MULTIPLEX_SENSINGBIT_MODE,
                SensingBit & CN_MULTIPLEX_SENSINGBIT_MODE))
        {
            TargetQ = &Sets->ActiveQ;
            ActivedInc = 1;
        } else
            TargetQ = &Sets->ObjectQ;

        temp = Mb_Malloc(g_ptMultiplexObjectPool, CN_TIMEOUT_FOREVER);
        if (temp != NULL) {
            Sets->ObjectSum++;
            temp->SensingBit = SensingBit;
            temp->PendingBit = ObjectStatus;
            temp->ObjectID = ObjectID;
            Lock_MutexPend(&MultiplexMutex, CN_TIMEOUT_FOREVER);
            temp->MySets = Sets;            //设定对象所属MultiplexSets
                                            //同一个MultiplexSets包含多个对象，NextObject把这些对象链接起来。
            if (*TargetQ == NULL) {
                *TargetQ = temp;
                temp->NextObject = temp;
                temp->PreObject = temp;
            } else {
                //新加入MultiplexSets的对象插入队列头部
                temp->PreObject = (*TargetQ)->PreObject;
                temp->NextObject = *TargetQ;
                (*TargetQ)->PreObject->NextObject = temp;
                (*TargetQ)->PreObject = temp;

                (*TargetQ) = temp;
            }
            //同一个对象被多个MultiplexSets包含，用NextSets链接。
            //NextSets是单向链表，新对象插入链表头部
            temp->NextSets = *ObjectHead;
            *ObjectHead = temp;
            Lock_MutexPost(&MultiplexMutex);
            if (ActivedInc == 1) {
                Sets->Actived += ActivedInc;
                if ((Sets->Actived >= Sets->ActiveLevel)
                    || (Sets->Actived >= Sets->ObjectSum)) {
                    if (false == Sets->SetsActived) {
                        Sets->SetsActived = true;
                        Lock_SempPost(&Sets->Lock);
                    }
                }
            }
        } else
            return false;
    } else {
        //重复加入，无须做任何处理
    }
    return true;
}

//----从MultiplexSets删除对象-----------------------------------------------------
//功能: MultiplexAdd的逆函数
//参数: Sets，被操作的MultiplexSets指针
//      ObjectHead，被操作的Object队列头指针的指针
//返回: true=成功，false=失败。
//-----------------------------------------------------------------------------
bool_t Multiplex_DelObject(struct MultiplexSetsCB *Sets,
                           struct MultiplexObjectCB **ObjectHead)
{
    struct MultiplexObjectCB *Object,*following = NULL;
    if ((Sets == NULL) || (ObjectHead == NULL))
        return false;
    Lock_MutexPend(&MultiplexMutex, CN_TIMEOUT_FOREVER);
    Object = *ObjectHead;
    while (Object != NULL) {       //查找被删除的对象控制块
        if (Object->MySets != Sets) {
            following = Object;
            Object = Object->NextSets;
        } else
            break;
    }

    if (Object != NULL) {
        //下面检查被删除的Object是否已经触发，
        if (__ObjectIsActived(Object->PendingBit,
                Object->SensingBit & ~CN_MULTIPLEX_SENSINGBIT_MODE,
                Object->SensingBit & CN_MULTIPLEX_SENSINGBIT_MODE)) {
            if (Sets->Actived != 0) {
                Sets->Actived--;
                if (Sets->Actived == 0)
                    Sets->SetsActived = false;
            }
        }

        Sets->ObjectSum--;
        if (Object->NextObject != Object) {    //链表中不止一个结点
            Object->NextObject->PreObject = Object->PreObject;
            Object->PreObject->NextObject = Object->NextObject;
            if (Object == Sets->ObjectQ) {
                Sets->ObjectQ = Object->NextObject;
            }
            if (Object == Sets->ActiveQ) {
                Sets->ActiveQ = Object->NextObject;
            }
        } else {                                //链表只有一个结点
            if (Object == Sets->ObjectQ) {
                Sets->ObjectQ = NULL;
            }
            if (Object == Sets->ActiveQ) {
                Sets->ActiveQ = NULL;
            }
        }
        if (following == NULL) {
            *ObjectHead = NULL;
        } else {
            following->NextObject = Object->NextObject;
        }
        Mb_Free(g_ptMultiplexObjectPool, Object);
    }
    Lock_MutexPost(&MultiplexMutex);
    return true;
}

//----Multiplex执行------------------------------------------------------------
//功能：当MultiplexSets中的对象状态发生变化，由相关模块调用本函数告知Multiplex
//      模块。
//参数: ObjectHead，被操作的Object队列头指针
//      Status，Object的当前状态
//返回: true=成功，false=失败。
//-----------------------------------------------------------------------------
bool_t Multiplex_Set(struct MultiplexObjectCB *ObjectHead, u32 Status)
{
    struct MultiplexObjectCB *Object;
    struct MultiplexSetsCB *Sets;
    u32 Sensing, Type;
    u32 OldPend;
    if (ObjectHead == NULL)
        return false;
    Lock_MutexPend(&MultiplexMutex, CN_TIMEOUT_FOREVER);
    Object = ObjectHead;
    while (Object != NULL) {
        OldPend = Object->PendingBit;
        Sets = Object->MySets;
        Sensing = Object->SensingBit & ~CN_MULTIPLEX_SENSINGBIT_MODE;
        Type = Object->SensingBit & CN_MULTIPLEX_SENSINGBIT_MODE;
        Object->PendingBit = Status & Sensing;          //更新PendingBit
        if (__ObjectIsActived(OldPend, Sensing, Type)) {    //调用前，Object已触发
            if (!__ObjectIsActived(Object->PendingBit, Sensing, Type)) {
                //调用Multiplex_Set导致对象变成未触发
                //把Object从Sets->ActiveQ队列拿出，放到ObjectQ队列中
                __ChangeList(&(Sets->ActiveQ), &(Sets->ObjectQ), Object);
                if (Sets->Actived != 0)
                    Sets->Actived--;
                if (Sets->Actived == 0)
                    Sets->SetsActived = false;
            }
        } else {                                            //调用前，Object未触发
            if (__ObjectIsActived(Object->PendingBit, Sensing, Type)) {
                //调用Multiplex_Set导致对象被触发

                //把Object从Sets->ObjectQ队列拿出，放到ActiveQ队列中
                __ChangeList(&(Sets->ObjectQ), &(Sets->ActiveQ), Object);
                if (Sets->Actived < Sets->ObjectSum)
                    Sets->Actived++;
                //异步触发模式，须释放信号量
                if ((Sets->Actived >= Sets->ActiveLevel)
                    || (Sets->Actived >= Sets->ObjectSum)) {
                    if (false == Sets->SetsActived) {
                        Sets->SetsActived = true;
                        Lock_SempPost(&Sets->Lock);
                    }
                }
            }
        }
        Object = Object->NextSets;
    }
    Lock_MutexPost(&MultiplexMutex);
    return true;
}

//----等待MultiplexSets-----------------------------------------------------------
//功能: 设置好MultiplexSets后，应用程序调用本函数等待MultiplexSets被触发，根据
//      Type值，等待方式分轮询和异步触发两种。
//参数: Sets，被操作的MultiplexSets指针
//      Status，返回对象当前状态的指针，如果应用程序不关心其状态，可以给NULL。
//      Timeout，阻塞等待的最长时间，uS。
//返回：如果MultiplexSets被触发，则返回MultiplexSets中一个被触发对象的ObjectID，
//      否则返回-1.
//注意: 调用本函数后,Sets的触发状态并没有改变,只有Multiplex_Set会修改Sets的触发
//      状态,Multiplex的运行过程是:
//      1.应用程序调用Multiplex_Wait,取得ObjectID返回
//      2.根据ObjectID访问相应的对象.
//      3.被访问的对象内部,如果本次访问导致了Sending Bit 变化,调用Multiplex_Set
//          "告知"Multiplex组件.
//      4.Multiplex_Set函数修改Sets的状态.
//      5.反复1~4步,直到全部活动的对象访问完,然后调用 Multiplex_Wait 会进入阻塞状态.
//      后,如果不对被select的对象做读写
//-----------------------------------------------------------------------------
ptu32_t Multiplex_Wait(struct MultiplexSetsCB *Sets, u32 *Status, u32 Timeout)
{
    struct MultiplexObjectCB *Object;
    ptu32_t result;
    if (Sets == NULL)
        return -1;
//    if ((Sets->ActiveQ == NULL) && (Sets->ObjectQ == NULL))
//        return -1;
//     上述语句必须注释掉，原因如下：
//     1,当我们创建sets的时候，可能有些任务已经调用了等待，然而此时并没有任何的等待对象加入
//     2,当等待对象从1变为0时，容易在次数死循环（使用sets wait的程序一般使用while(1)）
//判断或等待直到MultiplexSets被触发。

    //判断或等待直到MultiplexSets被触发。
    if (!Sets->SetsActived) {
        Lock_SempPend(&Sets->Lock, Timeout);
    }

    if (Sets->ActiveQ != NULL) {   //阻塞等待结束后，SetsActived非空即视为触发。
        Sets->SetsActived = true;
        Object = Sets->ActiveQ;
        result = Object->ObjectID;
        if (Status != NULL)
            *Status = Object->PendingBit;
    } else
        result = -1;

    return result;
}

