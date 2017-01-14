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
//文件描述:提供固定块分配策略，内存块至少2个指针长度(通常为8字节)。
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include "align.h"
#include "stdint.h"
#include "object.h"
#include "int.h"
#include "lock.h"
#include "stdlib.h"
#include "pool.h"
#include <djyos.h>
#include "core_config.h"

static struct MemCellPool *s_ptPooCtrl;  //管理内存池控制块本身的内存池

//----初始化固定块分配模块------------------------------------------------------
//功能: 初始化操作系统的固定块内存分配模块.内存池控制块本身也是按照内存池的方式
//      分配的,但是此时内存池组件还没有初始化完成,故需要手动创建用于内存池控制块
//      的内存池.
//参数: 无.
//返回: 1.
//----------------------------------------------------------------------------
ptu32_t Mb_ModuleInit(ptu32_t para)
{
    static struct MemCellPool cell_pool;
    cell_pool.continue_pool = M_Malloc(gc_u32CfgMemPoolLimit * sizeof(struct MemCellPool),0);
    if(cell_pool.continue_pool == NULL)
        return 0;
    cell_pool.cell_size = sizeof(struct MemCellPool);
    cell_pool.free_list = NULL;
    cell_pool.pool_offset = (ptu32_t)cell_pool.continue_pool;
    cell_pool.pool_offset += gc_u32CfgMemPoolLimit * sizeof(struct MemCellPool);
    s_ptPooCtrl = &cell_pool;
    OBJ_AddTree(&cell_pool.memb_node,sizeof(struct MemCellPool),RSC_MEMPOOL,
                    "固定块分配池");
    Lock_SempCreate_s(&cell_pool.memb_semp,gc_u32CfgMemPoolLimit,
                      gc_u32CfgMemPoolLimit,CN_BLOCK_FIFO,"固定块分配池");
    return 1;
}

//----创建一个内存池-------------------------------------------------------
//功能: 初始化一个内存池，原始内存池的内存由用户提供。如果系统有对齐要求，则起始
//      地址至少按照指针类型对齐，cell_size也应该是指针长度的整数倍，分配规则：
//      1.开始时,free_list指针空,continue_pool指向一个连续内存池。
//      2.如果free_list非空，新内存从free_list队列头部取。
//      3.如果free_list空，新内存从continue_pool中取。
//      4.释放内存时，把内存块链接到free_list队列,而不是返回continue_pool数组.
//      内存池是一个临界资源,操作时需要关闭中断,如果新申请的内存块全部链接到
//      free_list,将会连续很长时间关闭中断,而现在采用的方法每次只操作一个内存块,
//      连续关中断的时间被降低到最小.
//参数: pool_original,最初由用户提供的内存池.
//      init_capacital,原始内存池的尺寸,以块为单位。
//      cell_size,内存块尺寸，若系统有对齐要求，必须为指针长度的整数倍，最小为两倍指针
//            长度.
//      increment,增量，本内存池中的内存单元耗尽时，将从堆中增加的内存块数，
//          设为0则不增加。注意，内存池扩大后，即使用户调用mb_free释放了内存，但
//          除非释放了内存池中的全部内存，否则新增的内存不会被收回。
//      limit,如果increment !=0，limit限制内存池的最大块数，以防无限制地增加，导
//          致内存耗尽。
//      name,给内存池起个名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 内存池指针.
//----------------------------------------------------------------------------
struct MemCellPool *Mb_CreatePool(void *pool_original,u32 init_capacital,
                                u32 cell_size,u32 increment,
                                u32 limit,const char *name)
{
    struct MemCellPool *pool;

    //创建内存池时，可以不提供内存块，此时init_capacital也必须是0.
    //此功能结合内存池动态增量功能，可帮助应用程序优化内存配置，例如:
    //某串口，可能有若干种通信协议，取决于现场应用。我们可以同时初始化好多种通信
    //协议的驱动程序，为每种通信协议初始化一个空的内存池，就可以实现只为实际发生
    //的通信协议分配内存。
    if(( pool_original == NULL) && (init_capacital != 0))
        return NULL;
//对齐参数检查，1、如果系统要求对齐访问，则块尺寸必须是指针尺寸的整数倍，内存池
//起始地址也是指针尺寸的整数倍。2、如果系统不要求对齐访问，则块尺寸大于指针尺寸
//就可以了。
    if(cell_size < 2*sizeof(void*))
        return NULL;
#if(CN_MEM_ALIGN == 1)
    if((cell_size % sizeof(void*) != 0)
            || (((ptu32_t)pool_original) % sizeof(void*) != 0))
        return NULL;
#endif

    //分配一个内存池控制头
    pool = (struct MemCellPool *)Mb_Malloc(s_ptPooCtrl,0);
    if(pool == NULL)
        return NULL;    //内存池控制头分配不成功

    pool->continue_pool = (void*)pool_original;     //连续池首地址
    pool->free_list = NULL;                 //开始时空闲链表是空的
    //设置连续池偏移地址
    pool->pool_offset = (ptu32_t)pool_original + init_capacital*cell_size;
    pool->cell_size = cell_size;
    pool->cell_increment = increment;
    pool->cell_limit = limit;
    pool->next_inc_pool = NULL;
    OBJ_AddChild(&s_ptPooCtrl->memb_node,
                &pool->memb_node,
                sizeof(struct MemCellPool),
                RSC_MEMPOOL,name);
    //init_capacital有可能是0
    Lock_SempCreate_s(&pool->memb_semp,init_capacital,init_capacital,
                        CN_BLOCK_FIFO,name);
    return pool;
}

//----创建一个内核内存池-------------------------------------------------------
//功能: 创建内存池，调用者须提供内存池控制块，高可靠性的应用中，不应该使用动态分
//      配的方式，静态定义更可靠，然后把指针传递过来。内核中使用的内存池，都是使
//      用静态定义的。应用程序配置cfg_mem_pools时，只需要考虑自己的需求就可以了。
//参数: pool,用户提供的内存池控制块
//      pool_original,最初由用户提供的内存池.
//      init_capacital,原始内存池的尺寸,以字节为单位,不能以块为单位,因为块尺寸将按
//          系统对齐要求调整,0表示直接从系统内存中申请.
//      increment,增量，本内存池中的内存单元耗尽时，将从堆中增加的内存单元数量，
//          设为0则不增加。注意，内存池扩大后，即使用户调用mb_free释放了内存，但
//          除非释放了内存池中的全部内存，否则新增的内存不会被收回。
//      limit,如果increment !=0，limit限制内存池的最大尺寸，以防无限制地增加，导
//          致内存耗尽。
//      cell_size,块尺寸,将按对齐要求调整,ARM中调整为8的整数倍.
//      name,给内存池起个名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 内存池指针.
//注: 本函数是专门为内核准备的，故不作参数检查。
//----------------------------------------------------------------------------
struct MemCellPool *Mb_CreatePool_s(struct MemCellPool *pool,
                                void *pool_original,u32 init_capacital,
                                u32 cell_size,u32 increment,
                                u32 limit,const char *name)
{
    pool->continue_pool = (void*)pool_original;     //连续池首地址
    pool->free_list = NULL;                 //开始时空闲链表是空的
    //设置连续池偏移地址
    pool->pool_offset = (ptu32_t)pool_original + init_capacital*cell_size;
    pool->cell_size = cell_size;
    pool->cell_increment = increment;
    pool->cell_limit = limit;
    pool->next_inc_pool = NULL;
    OBJ_AddChild(&s_ptPooCtrl->memb_node,
                &pool->memb_node,
                sizeof(struct MemCellPool),
                RSC_MEMPOOL,name);
    Lock_SempCreate_s(&pool->memb_semp,init_capacital,init_capacital,
                            CN_BLOCK_FIFO,name);
    return pool;
}

//----删除一个内存池-----------------------------------------------------------
//功能: 删除一个内存池，当某内存池不再需要时，可调用本函数.本函数只清理了内存池
//      的信号量和资源结点，内存池缓冲区是调用者提供的，应由调用者清理。
//参数: pool，内存池指针
//返回: true= 成功删除，false=失败
//-----------------------------------------------------------------------------
bool_t Mb_DeletePool(struct MemCellPool *pool)
{
    void *inc_memory,*temp;
    if(!Lock_SempDelete_s(&pool->memb_semp))
        return false;
    inc_memory = pool->next_inc_pool;
    //如果内存池在使用过程中做了动态扩容操作，这里释放增加的内存
    while(inc_memory != NULL)
    {
        temp = *(void**)inc_memory;
        free(inc_memory);
        inc_memory = temp;
    }
    if(!OBJ_Del(&pool->memb_node))
        return false;
    Mb_Free(s_ptPooCtrl,pool);
    return true;
}

//----删除一个内核内存池-------------------------------------------------------
//功能: 删除一个内存池，与mb_create_pool_s对应
//参数: pool，内存池指针
//返回: 无
//-----------------------------------------------------------------------------
bool_t Mb_DeletePool_r(struct MemCellPool *pool)
{
    void *inc_memory,*temp;
    if(!Lock_SempDelete_s(&pool->memb_semp))
        return false;
    inc_memory = pool->next_inc_pool;
    //如果内存池在使用过程中做了动态扩容操作，这里释放增加的内存
    while(inc_memory != NULL)
    {
        temp = *(void**)inc_memory;
        free(inc_memory);
        inc_memory = temp;
    }
    if(!OBJ_Del(&pool->memb_node))
        return false;
    return true;
}

//----分配一块内存-------------------------------------------------------------
//功能: 从制定内存池中分配一块内存,只能分配一块,而且不能保证连续两次分配的内存
//      地址是连续的.
//参数: pool，内存池指针
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//      超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回: 申请成功返回内存地址,否则返回NULL.
//-----------------------------------------------------------------------------
void *Mb_Malloc(struct MemCellPool *pool,u32 timeout)
{
    void *result,*inc;
    u32 limit,inc_size,inc_cell,capacity,frees;
    char *name;
    atom_low_t atom_low;
    if(pool == NULL)
        return NULL;
    //没有取得信号量，表明内存池空,这个信号量是保护内存池的，确保被分配的内存块
    //不超过内存池的容量
    if(Lock_SempPend(&pool->memb_semp,0) == false)  //无信号量，表明内存块已经用完
    {
        //注意:上一行和下一行之间可能发生线程切换
        Int_SaveAsynSignal();
        //内存池容量未达到上限，可以继续增加
        if(Lock_SempQueryCapacital(&pool->memb_semp) < pool->cell_limit)
        {
            //此时timeout只能为0，因为在禁止异步信号的情况下，不为0也没用
            //+ align_up_sys(1)是为了对齐
            inc = M_Malloc(pool->cell_increment*pool->cell_size
                                + align_up_sys(1),timeout);
            if(inc != NULL)
            {
                //检查实际分配到的内存量
                inc_size = M_CheckSize(inc) - align_up_sys(1);
                inc_cell = inc_size/pool->cell_size;
                capacity = Lock_SempQueryCapacital(&pool->memb_semp);
                frees = Lock_SempQueryFree(&pool->memb_semp);
                limit = capacity + inc_cell;
                name = OBJ_Name((struct Object*)&pool->memb_semp);
                Lock_SempDelete_s(&pool->memb_semp);      //删除原信号量
                //按扩容后的容量重新申请信号量。
                Lock_SempCreate_s(&pool->memb_semp,limit,frees+inc_cell-1,
                                    CN_BLOCK_FIFO,name);
                pool->continue_pool = (void*)((ptu32_t)inc + align_up_sys(1));
                pool->pool_offset = (ptu32_t)pool->continue_pool + inc_cell*pool->cell_size;
                //以下初始化增量表，该表用于标记动态增加的内存块，利于删除内存池
                //时释放内存，以免内存丢失。
                *(void **)inc = pool->next_inc_pool;
                pool->next_inc_pool = inc;
                Int_RestoreAsynSignal();
            }
            else
            {
                Int_RestoreAsynSignal();
                return NULL;
            }
        }
        else
        {
            Int_RestoreAsynSignal();
            return NULL;
        }
    }

    //注:从semp_pend到int_low_atom_start之间发生抢占是允许的，因为信号量已经
    //取得，其他事件不可能把内存块分配光。
    atom_low = Int_LowAtomStart( );
    if(pool->free_list != NULL)     //空闲队列中有内存块
    {
        result = pool->free_list;   //取空队列表头部的内存块
        if(((struct MemCellFree*)result)->next==(struct MemCellFree*)result)
            pool->free_list = NULL; //这是空闲队列中的最后一块了，但可能不是内存
                                    //池中的最后一块，因为连续池中可能还有
        else
        {
            pool->free_list = pool->free_list->next;  //空闲队列下移一格.
            pool->free_list->previous=((struct MemCellFree*)result)->previous;
            ((struct MemCellFree*)result)->previous->next = pool->free_list;
        }
    }else                   //空闲队列中无内存块，从连续池中取
    {
        pool->pool_offset -= pool->cell_size;  //偏移地址调整
        //分配偏移地址处的内存块
        result = (void*)pool->pool_offset;
    }
    Int_LowAtomEnd(atom_low);
    return result;
}
//----释放内存--------------------------------------------------------------
//功能: 释放内存,把使用完毕的内存块放回指定的内存池,内存池和内存块必须匹配,否则
//      会发生灾难性的错误.新释放的块链接到 free_list 队列中,而不是放回连续池
//      中,也不重新返回系统堆.内存池从系统堆分配内存是单向的,一经分配永不释放.
//参数: block,待释放的内存块指针
//      pool,目标内存池.
//返回: 无
//-----------------------------------------------------------------------------
void Mb_Free(struct MemCellPool *pool,void *block)
{
    atom_low_t atom_low;
    if(pool == NULL)
        return;
//    pl_continue = (void*)pool->pool_offset;
//    block_sum = Lock_SempQueryCapacital(&pool->memb_semp);
    //20110613日注:增加扩容功能后，下列检查不再有效，注释掉。
    //被释放块合法性检查:1、地址不能低于内存池首地址
    //                   2、地址不能高于内存池末地址
    //                   3、地址必须是块首地址，即偏移必须是块尺寸的整数倍
//    if((block < (void*)pool->pool_offset)
//            ||((ptu32_t)block>=((ptu32_t)pl_continue+pool->cell_size*block_sum))
//            ||(((ptu32_t)block-(ptu32_t)pl_continue) % pool->cell_size != 0))
//        return ;
    atom_low = Int_LowAtomStart( );
    //查看待释放的内存块是否已经在free_list队列中。
    if(pool->free_list == NULL)
    {
        pool->free_list = (struct MemCellFree*)block;
        pool->free_list->next = (struct MemCellFree*)block;
        pool->free_list->previous = (struct MemCellFree*)block;

     }
    else
    {
        //if(((struct MemCellFree*)block)->previous->next
         //                                   != (struct MemCellFree*)block)
       // {   //以下4行把被释放的内存块插入到队列尾部
            ((struct MemCellFree*)block)->next = pool->free_list;
            ((struct MemCellFree*)block)->previous = pool->free_list->previous;
            pool->free_list->previous->next = (struct MemCellFree*)block;
            pool->free_list->previous = (struct MemCellFree*)block;
       // }
    }
    Int_LowAtomEnd(atom_low);
    Lock_SempPost(&pool->memb_semp);
}


//----查询剩余容量-------------------------------------------------------------
//功能: 查询内存池还有多少空闲内存块
//参数: pool,目标内存池.
//返回: 内存块数
//-----------------------------------------------------------------------------
u32 Mb_QueryFree(struct MemCellPool *pool)
{
    if(pool == NULL)
        return 0;
    return Lock_SempQueryFree(&pool->memb_semp);
}

//----查询内存池容量-----------------------------------------------------------
//功能: 查询内存池总共有多少内存块
//参数: pool,目标内存池.
//返回: 内存块数
//-----------------------------------------------------------------------------
u32 Mb_QueryCapacital(struct MemCellPool *pool)
{
    if(pool == NULL)
        return 0;
    return Lock_SempQueryCapacital(&pool->memb_semp);
}

