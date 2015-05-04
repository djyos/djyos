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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
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

#ifndef __MSGQUEUE_H__
#define __MSGQUEUE_H__

#include "stdint.h"
#include "rsc.h"
#include "lock.h"
#ifdef __cplusplus
extern "C" {
#endif

//消息队列属性选项
#define CN_MSGQ_TYPE_MASK   0x01
#define CN_MSGQ_TYPE_FIFO   0x00    //等候读取消息的线程按先到先得方式等待
#define CN_MSGQ_TYPE_PRIO   0x01    //等候读取消息的线程按优先级高者先得

#define CN_MSGQ_PRIO_NORMAL 0       //普通消息
#define CN_MSGQ_PRIO_URGENT 1       //高优先级消息

struct tagMsgQueue
{
    struct tagRscNode MsgNode;
    struct tagSemaphoreLCB MsgSendSemp;   //发送端控制信号量
    struct tagSemaphoreLCB MsgRecvSemp;   //接收端控制信号量

    u32    MsgSize;                     //
    u32    MsgUsed;                     //
    u32    ReadOffset;                  //
    u8     *buf;                        //
};

struct tagMsgQueue *MsgQ_Create( u32 MaxMsgs,u32  MsgLength,u32 Options);
ptu32_t ModuleInstall_MsgQ (ptu32_t para);
struct tagMsgQueue *MsgQ_Create_s(struct tagMsgQueue *pMsgQ, u32 MaxMsgs,
                                  u32  MsgLength,u32 Options,void *buf);
bool_t MsgQ_Delete(struct tagMsgQueue *pMsgQ);
bool_t MsgQ_Delete_s(struct tagMsgQueue *pMsgQ);
bool_t MsgQ_Send(struct tagMsgQueue *pMsgQ,u8 *buffer,u32 nBytes,u32 Timeout,
                 bool_t priority);
bool_t MsgQ_Receive(struct tagMsgQueue *pMsgQ,u8 *buffer,u32 nBytes,u32 Timeout);

#ifdef __cplusplus
}
#endif

#endif /* __MSGQUEUE_H__ */

