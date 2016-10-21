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
//所属模块:堆管理模块
//作者:  罗侍田.
//版本：V1.0.1
//文件描述: 从堆中做准静态内存分配
//其他说明:
//修订历史:
//4. 日期:2014-9-1
//   作者:  罗侍田.
//   新版本号：2.0.0
//   修改说明: 支持多个堆，每个堆由多个片段组成
//      1、彻底删除用链表记录每个时间申请的内存的代码。
//      2、不再需要在config_prj.h中配置heap相关的条目，改由lds提供。
//      3、增加显示heap分配信息的函数
//      4、实现两套函数，一套从默认堆中分配，一套从指定堆中分配。
//      5、分配时先查找最合适的片段
//4. 日期:2013-1-18
//   作者:  罗侍田.
//   新版本号：1.0.3
//   修改说明: 修正了m_free函数的一处错误，该bug由网友"王毛磊"提交
//3. 日期:2009-10-31
//   作者:  罗侍田.
//   新版本号：1.0.2
//   修改说明: 修正了__m_check_memory和__m_wait_memory函数的一处错误，该bug由
//   网友"再回首"提交
//2. 日期:2009-03-03
//   作者:  罗侍田.
//   新版本号：1.0.1
//   修改说明: 修正了__m_check_memory函数的一处错误，该bug由网友sniper提交
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "board-config.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "list.h"
#include "align.h"
#include "int.h"
#include "lock.h"
#include "systime.h"
#include "heap-in.h"
#include "heap.h"
extern void *pHeapList;             //在脚本中定义
struct HeapCB *tg_pHeapList=NULL;   //堆链指针，系统中所有的堆被链接在一起。
struct HeapCB *tg_pSysHeap=NULL;   //堆链指针，系统中所有的堆被链接在一起。

void *__M_StaticMallocHeap(ptu32_t size,struct HeapCB *Heap,u32 Timeout);
void *__M_StaticMalloc(ptu32_t size,u32 timeout);
void __M_StaticFreeHeap(void *pl_mem,struct HeapCB *Heap);
void __M_StaticFree(void *pl_mem);
void *__M_StaticRealloc(void *, ptu32_t NewSize,u32 timeout);
void *__M_StaticMallocStack(ptu32_t size);

ptu32_t Heap_StaticModuleInit(ptu32_t para);

ptu32_t __M_StaticFormatSizeHeap(ptu32_t size,struct HeapCB *Heap);
ptu32_t __M_StaticFormatSize(ptu32_t size);
ptu32_t __M_StaticGetMaxFreeBlockHeap(struct HeapCB *Heap);
ptu32_t __M_StaticGetMaxFreeBlock(void);
ptu32_t M_GetHeapSizeHeap(struct HeapCB *Heap);
ptu32_t M_GetHeapSize(void);
struct HeapCB *M_FindHeap(const char *HeapName);       //在heap-dynamic.c中
ptu32_t __M_StaticGetFreeMem(void);
ptu32_t __M_StaticGetFreeMemHeap(struct HeapCB *Heap);
ptu32_t __M_StaticCheckSize(void * mp);

void *  (*M_Malloc)(ptu32_t size,u32 timeout);
void *  (*M_Realloc) (void *, ptu32_t NewSize,u32 timeout);
void    (*free)(void * pl_mem);
void *  (*M_MallocHeap)(ptu32_t size,struct HeapCB *Heap,u32 timeout);
void *  (*M_MallocLc)(ptu32_t size,u32 timeout);
void *  (*M_MallocLcHeap)(ptu32_t size,struct HeapCB *Heap, u32 timeout);
void    (*M_FreeHeap)(void * pl_mem,struct HeapCB *Heap);
void *  (*__MallocStack)(ptu32_t size);
ptu32_t (*M_FormatSizeHeap)(ptu32_t size,struct HeapCB *Heap);
ptu32_t (*M_FormatSize)(ptu32_t size);
ptu32_t (*M_GetMaxFreeBlockHeap)(struct HeapCB *Heap);
ptu32_t (*M_GetMaxFreeBlock)(void);
ptu32_t (*M_GetFreeMem)(void);
ptu32_t (*M_GetFreeMemHeap)(struct HeapCB *Heap);
ptu32_t (*M_CheckSize)(void * mp);

u32 __memStrLen(char *addr)
{
    u32 result = 0;
    while(*addr != '\0')
    {
        result ++;
        addr++;
    }
    return result;
}

void __memHeapScan(void)
{
    struct HeapCB *HeapTemp;
    struct HeapCession *Cession;
    u32 CessionNum,Ctrlsize,NameLen,n;
    u8  *Offset;
    u32 *u32Offset;
    u32 HeapNo=0;
    u32 AlignSize;
    u32 Property;
    Offset = (u8*)&pHeapList;
    CessionNum = *(u32*)Offset;
    while(CessionNum != 0)     //本循环取得所需堆控制块数量
    {
        u32Offset = (u32*)Offset + 1;
        AlignSize = *u32Offset++;
        Property = *u32Offset++;
        //以下计算公式，参看lds文件格式
        Offset += sizeof(u32)*3*CessionNum + 3*sizeof(u32);
        NameLen = __memStrLen((char*)Offset);
        //计算heap控制块和session控制块所需要的内存尺寸。
        Ctrlsize = NameLen + 1 + sizeof( struct HeapCB )
                    + CessionNum * sizeof(struct HeapCession);
        Ctrlsize = align_up_sys(Ctrlsize);
        Cession = NULL;
        //下面for循环功能:
        //1、扫描所有session，找到第一个满足Ctrlsize需求的session。
        //2、在该session顶部分配用于控制结构的内存。
        //3、丢弃size小于Ctrlsize的session。
        //4、按准静态分配的要求初始化heap控制块和session控制块。
        //5、把所有的session连接成链表
        for(n = 0; n < CessionNum; n++)
        {
            if(Cession != NULL)
            {
                Cession->static_bottom = (void*)(*u32Offset);
                Cession->heap_bottom = (void*)(*u32Offset);
                Cession->heap_top = (void*)(*(u32Offset+1));
                Cession->last = (list_t *)(*u32Offset);
                dListInit(Cession->last);
#if ((CN_CFG_DYNAMIC_MEM == 1))
                Cession->PageSize = *(u32Offset+2);
#endif
                if(n == CessionNum -1)
                    Cession->Next = NULL;
                else
                    Cession->Next = Cession+1;
                Cession++;
            }
            //找到了第一个符合要求的session
            if((Cession == NULL) && ((*(u32Offset+1) - *u32Offset) >=Ctrlsize))
            {
                HeapTemp = (struct HeapCB *)(*(u32Offset+1) - Ctrlsize);
                HeapTemp = (struct HeapCB *)align_down_sys(HeapTemp);
                if(tg_pHeapList == NULL)
                {
                    tg_pHeapList = HeapTemp;
                    tg_pHeapList->NextHeap = HeapTemp;
                    tg_pHeapList->PreHeap = HeapTemp;
                    //第一个堆可做系统堆。
                    if(HeapNo == 0)
                        tg_pSysHeap = tg_pHeapList;
                }
                else
                {
                    HeapTemp->NextHeap = tg_pHeapList;
                    HeapTemp->PreHeap = tg_pHeapList->PreHeap;
                    tg_pHeapList->PreHeap->NextHeap = HeapTemp;
                    tg_pHeapList->PreHeap = HeapTemp;
                }
                Cession = (struct HeapCession*)(HeapTemp + 1);
                HeapTemp->Cession = Cession;
                HeapTemp->CessionNum = CessionNum - n;
                if(AlignSize == 0)
                    HeapTemp->AlignSize = CN_ALIGN_SIZE;
                else
                    HeapTemp->AlignSize = AlignSize;
                HeapTemp->HeapProperty = Property;
                HeapTemp->HeapName = (char *)Offset;
#if ((CN_CFG_DYNAMIC_MEM == 1))
                HeapTemp->mem_sync = NULL;
#endif
                Cession->static_bottom = (void*)(*u32Offset);
                Cession->heap_bottom = (void*)(*u32Offset);
                Cession->heap_top = (void*)HeapTemp;
                Cession->last = (list_t *)(*u32Offset);
                dListInit(Cession->last);
#if ((CN_CFG_DYNAMIC_MEM == 1))
                Cession->PageSize = *(u32Offset+2);
#endif
                if(n == CessionNum -1)
                    Cession->Next = NULL;
                else
                    Cession->Next = Cession+1;
                Cession++;
            }
            Ctrlsize -= sizeof(struct HeapCession);
            u32Offset += 3;
        }
        Offset += NameLen + 1;      //+1跨过串结束符
        Offset = (u8*)align_up(sizeof(u32),Offset);
        CessionNum = *(u32*)Offset;
        HeapNo++;
    }
}
//----内存结构初始化-----------------------------------------------------------
//功能: 扫描全部heap，分配Heap控制块和Cession控制块所需的内存，并初始化。
//参数: 无
//返回: 总是1.
//备注: 1.本函数在初始化完成之前调用,中断尚未开启,无需考虑关闭中断的问题.
//      2.软件启动后首先调用本函数,启动静态分配功能,此后还不具备动态分配功能.
//      3.由配置工具确保堆不重名，这里不做检查。
//-----------------------------------------------------------------------------
ptu32_t Heap_StaticModuleInit(ptu32_t para)
{
    __memHeapScan();

    M_Malloc = __M_StaticMalloc;
    M_Realloc = __M_StaticRealloc;
    free = __M_StaticFree;
    M_MallocHeap = __M_StaticMallocHeap;
    M_MallocLc = __M_StaticMalloc;
    M_MallocLcHeap = __M_StaticMallocHeap;
    M_FreeHeap = __M_StaticFreeHeap;
    __MallocStack = __M_StaticMallocStack;
    M_FormatSizeHeap = __M_StaticFormatSizeHeap;
    M_FormatSize = __M_StaticFormatSize;
    M_GetMaxFreeBlockHeap = __M_StaticGetMaxFreeBlockHeap;
    M_GetMaxFreeBlock = __M_StaticGetMaxFreeBlock;
    M_GetFreeMem = __M_StaticGetFreeMem;
    M_GetFreeMemHeap = __M_StaticGetFreeMemHeap;
    M_CheckSize = __M_StaticCheckSize;

    return 1;
}

//----准静态内存分配-----------------------------------------------------------
//功能：执行准静态分配的方法，在指定的堆内分配一块内存，该方法实际上是模拟编译
//      器分配行为，再加上对齐。
//参数：size,欲分配的内存块尺寸
//      Heap,指定的堆
//返回：分配的内存指针，NULL表示没有内存可以分配
//备注: 1.准静态分配与静态内存分配类似,这种分配方法使用原子操作来确保数据一致，
//      不会引起阻塞,内存不足则直接返回。
//      在执行module_init_heap_dynamic之前,所有的内存分配均采用准静态分配
//------------
//更新记录:
// 2.日期: 2015/4/25
//   说明: 增加判断通用堆还是专用堆
//   作者: 季兆林
// 1.日期: 2015/3/16
//   说明: 增加对参数size的零值判断
//   作者: 季兆林
//-----------------------------------------------------------------------------
void *__M_StaticMallocHeapIn(ptu32_t size,struct HeapCB *Heap)
{
    void *temp,*result = NULL;
    list_t *pStaticList;
    struct HeapCession *Cession;
    atom_low_t atom_m;

    if((Heap == NULL) || (0 == size))
        return NULL;

    Cession = Heap->Cession;
    atom_m = Int_LowAtomStart();
    while(Cession != NULL)
    {
        temp = Cession->heap_bottom;   //保存当前堆底
        //留下保存块尺寸的空间后对齐
        Cession->heap_bottom = (u8 *)align_up(Heap->AlignSize,
                            Cession->heap_bottom + sizeof(list_t));
        if(((ptu32_t)Cession->heap_top > (ptu32_t)Cession->heap_bottom)&&\
           ((ptu32_t)(Cession->heap_top-Cession->heap_bottom) >= size))
        {
            //新分配的内存尺寸保存在当前堆底前面
            pStaticList = (list_t *)Cession->heap_bottom;
            pStaticList -= 1;
            dListInsertAfter(Cession->last,pStaticList);
            Cession->last = pStaticList;
            result = Cession->heap_bottom;
            Cession->heap_bottom += align_up(Heap->AlignSize,size);
            break;
        }
        else
        {
            Cession->heap_bottom = temp;   //恢复当前堆底
        }
        Cession = Cession->Next;
    }
    Int_LowAtomEnd(atom_m);
    return(result);
}
//----准静态内存分配-----------------------------------------------------------
//功能：执行准静态分配的方法，在指定的堆内分配一块内存，该方法实际上是模拟编译
//      器分配行为，再加上对齐。如果指定的堆是通用堆,则优先从该堆分配,分配失败
//      则从其他有通用属性的堆中分配;如果指定的堆是专用堆,则只从该堆分配.
//参数：size,欲分配的内存块尺寸
//      Heap,指定的堆
//返回：分配的内存指针，NULL表示没有内存可以分配
//备注: 1.准静态分配与静态内存分配类似,这种分配方法使用原子操作来确保数据一致，
//      不会引起阻塞,内存不足则直接返回。
//      在执行module_init_heap_dynamic之前,所有的内存分配均采用准静态分配
//------------
//更新记录:
// 1.日期: 2015/4/25
//   说明: 增加判断通用堆还是专用堆
//   作者: 罗侍田
//-----------------------------------------------------------------------------
void *__M_StaticMallocHeap(ptu32_t size,struct HeapCB *Heap,u32 Timeout)
{
    struct HeapCB *Work;
    void *result = NULL;

    if((Heap == NULL) || (0 == size))
        return NULL;
    if((Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL)
    {
        Work = Heap;
        do
        {
            if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
            {
                result = __M_StaticMallocHeapIn(size,Work);
                if(result != NULL)
                    break;
            }
            Work = Work->NextHeap;
        }while(Work != Heap);

    }
    else
    {
        result = __M_StaticMallocHeapIn(size,Heap);
    }
    return(result);
}
//----分配线程栈---------------------------------------------------------------
//功能：准静态条件下, 与malloc是一致的.
//参数：size,欲分配的内存块尺寸
//返回：分配的内存指针，NULL表示没有内存可以分配
//-----------------------------------------------------------------------------
void *__M_StaticMallocStack(ptu32_t size)
{
    struct HeapCB *Work;
    void *result = NULL;
    Work = tg_pHeapList;
    do
    {
        if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
        {
            result = __M_StaticMallocHeapIn(size,Work);
            if(result != NULL)
                break;
        }
        Work = Work->NextHeap;
    }while(Work != tg_pHeapList);

    return result;
}

void *__M_StaticMalloc(ptu32_t size,u32 timeout)
{
    return __M_StaticMallocHeap(size,tg_pSysHeap,0);
}
//----重新分配一块内存---------------------------------------------------------
//功能: 用新的尺寸,给p指针重新分配一块内存.
//      如果扩大,则把原有数据全部copy到新分配的内存中.
//      如果缩小,则按新尺寸copy原有数据,超出的数据将丢弃.
//      如果p=NULL,则相当于__M_Malloc
//      如果NewSize = 0,则相当于 __M_Free
//      如果新内存分配失败,原来的内存也将被释放,返回NULL
//      本函数分配的内存,可使用free函数释放.
//参数: p, 待修改尺寸的指针
//      NewSize,新的内存尺寸
//返回: 新尺寸的内存指针,注意可能是原指针.
//------------
//更新记录:
// 1.日期: 2015/3/16
//   说明: 优先对参数NewSize的判断逻辑
//   作者: 季兆林
//-----------------------------------------------------------------------------
void *__M_StaticRealloc(void *p, ptu32_t NewSize,u32 Timeout)
{
    ptu32_t OldSize;
    void *NewP = NULL;

    if(NewSize == 0)
    {
        __M_StaticFree(p);
        return NULL;
    }

    if(p == NULL)
    {
        //若NewSize = 0,返回NULL
        NewP = __M_StaticMalloc(NewSize,0);
        return NewP;
    }

    OldSize = __M_StaticCheckSize(p);
    if((NewSize > (OldSize - tg_pSysHeap->AlignSize)) && (NewSize <= OldSize))
        return p;
    else
    {
        NewP = __M_StaticMalloc(NewSize,CN_TIMEOUT_FOREVER);
        if(NewP)
        {
            memcpy(NewP,p,NewSize < OldSize ? NewSize:OldSize);
        }
    }
    return NewP;
}

//----释放准静态分配的内存块---------------------------------------------------
//功能：释放准静态分配的一个内存块，在heap的cession中查找，如果待释放的内存是从该
//      session分配的最后一块内存，释放之。
//参数：pl_mem，待释放的内存块指针
//      Heap，指定从这个堆中释放。
//返回：true = 成功释放，false =  释放失败
//备注: 1.准静态分配与静态内存分配类似,没有保护措施,正确性要程序员自己保证.这种
//      分配方法也不会引起阻塞,在执行module_init_heap_dynamic之前,所有的内存分配
//      均采用准静态分配
//      2.本函数在初始化完成之前调用,中断尚未开启,无需考虑关闭中断的问题.
//-----------------------------------------------------------------------------
void __M_StaticFreeHeap(void * pl_mem,struct HeapCB *Heap)
{
    atom_low_t atom_m;
    struct HeapCession *Cession;
    list_t *pStaticList;

    if(pl_mem == NULL)
        return ;
    if(pl_mem != (void *)align_up(Heap->AlignSize,pl_mem))//不符合对齐要求的指针
        return ;
    Cession = Heap->Cession;
    atom_m = Int_LowAtomStart();
    while(Cession != NULL)
    {
        if((pl_mem >=Cession->static_bottom) && (pl_mem <= Cession->heap_bottom))
        {
            pStaticList = (list_t*)pl_mem;
            pStaticList--;
            if(pStaticList == Cession->last)        //这是最后一块，直接回收。
            {
//                Cession->last = pStaticList->prev;
                Cession->last = dListGetBefore(pStaticList);
                Cession->heap_bottom = (u8*)pStaticList;
            }
            dListRemove(pStaticList);               //无论是否最后一块，都回收节点
            break;
        }
//      psize = (ptu32_t *)pl_mem;
//      psize--;
//      if( (*psize + (u8*)pl_mem) == Cession->heap_bottom)
//      {
//          //pl_mem是从该cession最后分配的内存，可以释放
//          Cession->heap_bottom = (void*)align_down(Heap->AlignSize,psize);
//          break;
//      }
        Cession = Cession->Next;
    }
    Int_LowAtomEnd(atom_m);

    return ;
}
void __M_StaticFree(void * pl_mem)
{
    __M_StaticFreeHeap(pl_mem,tg_pSysHeap);
}

//----查询堆空间大小---------------------------------------------------------
//功能：返回堆空间大小,此函数不分静态和准静态,都一样.
//参数：无
//返回：内存堆字节数
//-----------------------------------------------------------------------------
ptu32_t  M_GetHeapSizeHeap(struct HeapCB *Heap)
{
    struct HeapCession *Cession;
    ptu32_t result=0;
    Cession = Heap->Cession;
    while(Cession != NULL)
    {
        result += Cession->heap_top - Cession->static_bottom;
        Cession = Cession->Next;
    }
    return result;
}

ptu32_t  M_GetHeapSize(void)
{
    if(tg_pSysHeap != NULL)
        return M_GetHeapSizeHeap(tg_pSysHeap);
    else
        return 0;
}

//----获取格式化尺寸-----------------------------------------------------------
//功能: 虽然应用程序允许申请任意尺寸内存，但内存管理器分配给用户的内存却是满足
//      用户需求的规格化尺寸的内存块，本函数告诉你，如果你申请分配size字节内存，
//      系统将分配给你的内存块的实际尺寸，本函数不执行分配内存的操作，也不保证随
//      后的分配操作能否成功。
//参数: size，应用程序欲分配的尺寸
//返回: 规格化尺寸
//-----------------------------------------------------------------------------
ptu32_t __M_StaticFormatSizeHeap(ptu32_t size,struct HeapCB *Heap)
{
    return align_up(Heap->AlignSize,size);  //准静态分配，对齐即可
}
ptu32_t __M_StaticFormatSize(ptu32_t size)
{
    return align_up_sys(size);  //准静态分配，对齐即可
}

ptu32_t __M_StaticGetMaxFreeBlockHeapIn(struct HeapCB *Heap)
{
    struct HeapCession *Cession;
    ptu32_t result = 0;
    Cession = Heap->Cession;
    while(Cession != NULL)
    {
        if(result < (Cession->heap_top - Cession->heap_bottom))
            result = Cession->heap_top - Cession->heap_bottom;
        Cession = Cession->Next;
    }
    return result;
}
//----查询最大可用内存块-------------------------------------------------------
//功能：返回指定堆的最大可用内存块的尺寸
//参数：Heap,被查询的堆,如果这是个专用堆,则只查询这个堆,否则查询所有通用堆.
//返回：内存块字节数
//-----------------------------------------------------------------------------
ptu32_t  __M_StaticGetMaxFreeBlockHeap(struct HeapCB *Heap)
{
    struct HeapCB *Work;
    ptu32_t result = 0,temp;
    if(Heap== NULL)
        return 0;
    if((Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_PRIVATE)
    {
        result = __M_StaticGetMaxFreeBlockHeapIn(Heap);
    }
    else
    {
        Work = Heap;
        do
        {
            if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
            {
                temp = __M_StaticGetMaxFreeBlockHeapIn(Work);
                if(result < temp)
                    result = temp;
            }
            Work = Work->NextHeap;
        }while(Work != Heap);
    }

    return result;
}

//----查询最大可用内存块-------------------------------------------------------
//功能：从所有通用堆中检查最大可用内存块的尺寸
//参数：无
//返回：内存块字节数
//-----------------------------------------------------------------------------
ptu32_t  __M_StaticGetMaxFreeBlock(void)
{
    struct HeapCB *Work;
    ptu32_t result = 0,temp;
    Work = tg_pHeapList;
    do
    {
        if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
        {
            temp = __M_StaticGetMaxFreeBlockHeapIn(Work);
            if(result < temp)
                result = temp;
        }
        Work = Work->NextHeap;
    }while(Work != tg_pHeapList);

    return result;
}

//----查询空闲内存大小-------------------------------------------------------
//功能：返回所指定堆的空闲内存字节数
//参数：Heap,被查询的堆
//返回：空闲内存字节数
//-----------------------------------------------------------------------------
ptu32_t  __M_StaticGetFreeMemHeapIn(struct HeapCB *Heap)
{
    struct HeapCession *Cession;
    ptu32_t result=0;
    Cession = Heap->Cession;
    while(Cession != NULL)
    {
        result += Cession->heap_top - Cession->heap_bottom;
        Cession = Cession->Next;
    }
    return result;
}

//----查询空闲内存大小-------------------------------------------------------
//功能：返回所指定堆的空闲内存字节数
//参数：Heap,被查询的堆,如果这是个专用堆,则只查询这个堆,否则查询所有通用堆.
//返回：空闲内存字节数
//-----------------------------------------------------------------------------
ptu32_t  __M_StaticGetFreeMemHeap(struct HeapCB *Heap)
{
    struct HeapCB *Work;
    ptu32_t result = 0;
    if(Heap== NULL)
        return 0;
    if((Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_PRIVATE)
    {
        result = __M_StaticGetFreeMemHeapIn(Heap);
    }
    {
        Work = Heap;
        do
        {
            if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
            {
                result += __M_StaticGetFreeMemHeapIn(Work);
            }
            Work = Work->NextHeap;
        }while(Work != Heap);

    }
    return result;
}
//----查询总空闲内存大小-------------------------------------------------------
//功能：返回所有通用堆的空闲内存字节数之和
//参数：无
//返回：空闲内存字节数
//-----------------------------------------------------------------------------
ptu32_t  __M_StaticGetFreeMem(void)
{
    struct HeapCB *Work;
    ptu32_t result = 0;
    Work = tg_pHeapList;
    do
    {
        if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
        {
            result += __M_StaticGetFreeMemHeap(Work);
        }
        Work = Work->NextHeap;
    }while(Work != tg_pHeapList);

    return result;
}

//----查询内存尺寸-------------------------------------------------------------
//功能: 根据给定的指针,查询该指针所在的内存块的尺寸.
//参数: mp,动态分配的内存指针.
//返回: 内存块尺寸,返回0有几种含义:1.非法指针,2.mp是由准静态分配的指针.
//-----------------------------------------------------------------------------
ptu32_t __M_StaticCheckSize(void * pl_mem)
{
    atom_low_t atom_m;
    struct HeapCB *Work;
    struct HeapCession *Cession;
    list_t *NextMem,*LocMem;
    ptu32_t result = 0;
    if(pl_mem == NULL)
        return 0;
    if(pl_mem != (void *)align_up(tg_pHeapList->AlignSize,pl_mem))//不符合对齐要求的指针
        return 0;

    Work = tg_pHeapList;
    atom_m = Int_LowAtomStart();
    do
    {
    	Cession = Work->Cession;
        while(Cession != NULL)
        {
            if((pl_mem >=Cession->static_bottom) && (pl_mem <= Cession->heap_bottom))
            {
                LocMem = (list_t*)pl_mem;
                LocMem--;
                NextMem = LocMem->next;
                if(NextMem > LocMem)
                    result = (ptu32_t)NextMem - (ptu32_t)LocMem;
                else
                {
                    result = (ptu32_t)Cession->heap_bottom - (ptu32_t)LocMem;
                }
                break;
            }
            Cession = Cession->Next;
        }
    	Work = Work->NextHeap;
    }while(Work != tg_pHeapList);

    if(result != 0)
    	result -= sizeof(list_t);
    Int_LowAtomEnd(atom_m);
    return result;
}
