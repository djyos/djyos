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
//所属模块:内存池管理模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:提供固定块分配策略
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __POOL_H__
#define __POOL_H__
#include "stdint.h"
#include "lock.h"
#ifdef __cplusplus
extern "C" {
#endif

struct tagMemCellPool
{
    struct tagRscNode memb_node;    //资源结点,整个内存池将作为一个资源结点
    void  *continue_pool;    //连续内存池首地址，使用它可以增加实时性。
    struct tagMemCellFree  *free_list;        //未分配块链表,单向,NULL结尾
    struct tagSemaphoreLCB memb_semp;
    ptu32_t pool_offset;    //连续池中的偏移量(当前地址)
    u32 cell_size;          //块大小,初始化时将按系统要求调整尺寸。
    u32 cell_increment;     //当内存池耗尽时，将自动从堆中增加内存池尺寸，单位为
                            //内存块数量，设为0表示不增量，注意，内存池尺寸是只
                            //增不减的，即一旦从堆中申请了内存，是不释放的。
    u32 cell_limit;         //如果increment !=0，本成员限定该内存池的最大尺寸，
                            //单位为内存块数。
    struct  tagEventECB *cell_sync;   //同步队列。
    void * next_inc_pool;   //增量地址，用于记录本内存池运行过程中的增量，利于在
                            //删除内存池时释放之。
};

//空闲内存块构成双向循环链表。但初始化内存池时并不把内存加入链表，而是在释放时
//才加入。
//空闲链表节点，把free_list做成双向链表，可以增加程序的健壮性，如果是单向链表，
//虽然可以实现功能，但是，如果重复释放一个已经释放的内存块，则可能导致破坏链表。
//用双向链表，则很容易判断该结点是否已经在free_list中，如是，则不重复执行释放操
//作，单向链表虽然可以用遍历的方法判断是否在free_list，但执行时间不确定，不符合
//实时系统的要求。
//用双向链表的后果是，单块内存至少两个指针长度，通常为8字节。
struct tagMemCellFree
{
    struct tagMemCellFree *previous;
    struct tagMemCellFree *next;
};
ptu32_t Mb_ModuleInit(ptu32_t para);
struct tagMemCellPool *Mb_CreatePool(void *pool_original,u32 capacital,
                                u32 cell_size,u32 increment,
                                u32 limit,char *name);
struct tagMemCellPool *Mb_CreatePool_s(struct tagMemCellPool *pool,
                                void *pool_original,u32 capacital,
                                u32 cell_size,u32 increment,
                                u32 limit,char *name);
bool_t Mb_DeletePool(struct tagMemCellPool *pool);
bool_t Mb_DeletePool_s(struct tagMemCellPool *pool);
void *Mb_Malloc(struct tagMemCellPool *pool,u32 timeout);
void Mb_Free(struct tagMemCellPool *pool,void *block);
u32 Mb_QueryFree(struct tagMemCellPool *pool);
u32 Mb_QueryCapacital(struct tagMemCellPool *pool);

#ifdef __cplusplus
}
#endif

#endif //__POOL_H__

