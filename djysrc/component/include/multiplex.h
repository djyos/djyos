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
#ifndef __MULTIPLEX_H__
#define __MULTIPLEX_H__
#include "lock.h"
#ifdef __cplusplus
extern "C" {
#endif

#define CN_MULTIPLEX_MAX_SETS      10
#define CN_MULTIPLEX_MAX_OBJECTS   1000

#define CN_MULTIPLEX_SUCCESS   0   //调用成功
#define CN_MULTIPLEX_INVALID   1   //对象不支持Multiplex功能
#define CN_MULTIPLEX_ERROR     2   //对象虽然支持Multiplex，但拒绝了本次调用

//struct tagMultiplexSetsCB中Type成员的定义:
#define CN_MULTIPLEX_OBJECT_AND      0     //全部Multiplex对象都被触发才触发
#define CN_MULTIPLEX_OBJECT_OR   (1<<0)    //有一个Multiplex对象被触发就触发
//#define CN_MULTIPLEX_POLL            0     //查询型，即等待应用程序查询
//#define CN_MULTIPLEX_ASYN        (1<<1)    //异步触发型，即Sets触发时立即唤醒

//struct tagMultiplexObjectCB中SensingBit成员的定义:
#define CN_MULTIPLEX_SENSINGBIT_READ      (1<<0)  //对象可读
#define CN_MULTIPLEX_SENSINGBIT_WRITE     (1<<1)  //对象可写
#define CN_MULTIPLEX_SENSINGBIT_ERROR     (1<<2)  //对象出错
#define CN_MULTIPLEX_SENSINGBIT_AND        0      //SensingBit中全部置位才触发
#define CN_MULTIPLEX_SENSINGBIT_OR        (1<<31) //SensingBit中有一个置位就触发

struct tagMultiplexSetsCB;
struct tagMultiplexObjectCB;

struct tagMultiplexSetsCB
{
    struct tagMultiplexObjectCB *ObjectQ;   // 指向未触发的对象链表
    struct tagMultiplexObjectCB *ActiveQ;   // 指向已触发的对象链表
    u32 ObjectSum;          // 对象集中包含的对象数。
    u32 ActiveLevel;        // 触发水平，被触发对象数量达到ActiveLevel，将触发
                            // 对象集，大于ObjectSum 表示全触发
    u32 Actived;            // 已经触发的对象数
    bool_t SetsActived;     // 对象集是否已经触发。
    struct tagSemaphoreLCB Lock;        // Type定义了异步触发模式:同步用的锁。
};

struct tagMultiplexObjectCB
{
    struct tagMultiplexObjectCB *NextObject;//纵向双向链表，用于连接一个MultiplexSets
                                         //包含的多个object
    struct tagMultiplexObjectCB *PreObject;
    struct tagMultiplexObjectCB *NextSets;  //横向单向链表，用于一个object被多个
                                         //MultiplexSets包含的情况
    struct tagMultiplexSetsCB *MySets;   //指向主控制块
    ptu32_t ObjectID;                    //被MultiplexSets等待的对象
    u32 PendingBit;                      //对象中已经触发的bit
    u32 SensingBit;                      //敏感位标志，共31bit
                                         //bit31表示敏感位检测类型，
                                         //CN_MULTIPLEX_SENSINGBIT_AND:全部bit挂起才触发
                                         //CN_MULTIPLEX_SENSINGBIT_OR:有一个bit挂起就触发
};

ptu32_t ModuleInstall_Multiplex(ptu32_t para);
struct tagMultiplexSetsCB *Multiplex_Creat(u32 ActiveLevel);
bool_t Multiplex_AddObject(struct tagMultiplexSetsCB  *Sets,
                    struct tagMultiplexObjectCB **ObjectHead,
                    u32 ObjectStatus,ptu32_t ObjectID, u32 SensingBit);
bool_t Multiplex_DelObject(struct tagMultiplexSetsCB  *Sets,
               struct tagMultiplexObjectCB **ObjectHead);
bool_t Multiplex_Set(struct tagMultiplexObjectCB *ObjectHead,u32 Status);
ptu32_t Multiplex_Wait( struct tagMultiplexSetsCB *Sets,u32 *Status,u32 Timeout);

#ifdef __cplusplus
}
#endif

#endif //__MULTIPLEX_H__


