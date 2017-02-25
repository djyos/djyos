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
/*
 * osarch.c
 *
 *  Created on: 2016年11月25日
 *      Author: zhangqf
 */

#include "osarch.h"

//mutex
mutex_t mutex_init(const char *name)
{
	return Lock_MutexCreate(name);
}
bool_t mutex_lock(mutex_t mutex)
{
	return Lock_MutexPend(mutex,CN_TIMEOUT_FOREVER);
}
bool_t mutex_locktimeout(mutex_t mutex,u32 timeout)
{
	return Lock_MutexPend(mutex,CN_TIMEOUT_FOREVER);
}
bool_t mutex_unlock(mutex_t mutex)
{
	Lock_MutexPost(mutex);
	return 0;
}

void mutex_del(mutex_t mutex)
{
	Lock_MutexDelete(mutex);
	return;
}

//-----------------------------------------------------------------------------
//功能:use this function to get a random data, which got from the system time
//参数:NULL
//返回:the random data
//备注:
//-----------------------------------------------------------------------------
u32 get_random(void)
{
	return (u32)DjyGetSysTime();
}


//-----------------------------------------------------------------------------
//功能:use this function to create a semaphore
//参数:
//返回:the semaphore if success and NULL when failed
//备注:
//-----------------------------------------------------------------------------
semp_t semp_init()
{
	return Lock_SempCreate(0x1000,0,CN_BLOCK_FIFO,NULL);
}
//-----------------------------------------------------------------------------
//功能:use this function to pend a semaphore
//参数:semp, the semaphore we will pend later
//返回:true when got the semaphore else false
//备注:never return if not got the semaphore and the condition permit waiting
//-----------------------------------------------------------------------------
bool_t semp_pend(semp_t semp)
{
	return Lock_SempPend(semp,CN_TIMEOUT_FOREVER);
}

bool_t semp_pendtimeout(semp_t semp, unsigned int timeout)
{
	return Lock_SempPend(semp,timeout);
}

bool_t semp_post(semp_t semp)
{
	Lock_SempPost(semp);
	return true;
}

void semp_del(semp_t semp)
{
	Lock_SempDelete(semp);
	return;
}

void semp_reset(semp_t semp)
{
	semp->lamp_counter = 0;
	return ;
}

//-----------------------------------------------------------------------------
//功能:use this function to create a task
//参数:
//返回:
//备注:
//作者:zhangqf@下午4:38:04/2017年1月5日
//-----------------------------------------------------------------------------
bool_t taskcreate(const char *name,u16 stacksize,u8 prior,ptu32_t (*fnTask)(void),ptu32_t para)
{
	u16 evttID,eventID;
    evttID= Djy_EvttRegist(EN_INDEPENDENCE, prior, 0, 1,fnTask,NULL, stacksize,name);
    if(evttID == CN_EVTT_ID_INVALID)
    {
        goto EXIT_EVTTFAILED;
    }
    eventID = Djy_EventPop(evttID, NULL, 0, para, 0, 0);
    if(eventID == CN_EVENT_ID_INVALID)
    {
        goto EXIT_EVENTFAILED;
    }
    return true;

EXIT_EVENTFAILED:
	Djy_EvttUnregist(evttID);
EXIT_EVTTFAILED:
	return false;
}



