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
//文件描述:提供块相联内存分配策略
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
#include "stdlib.h"
#include "string.h"
#include "list.h"
#include "align.h"
#include "int.h"
#include "heap-in.h"
#include "heap.h"
#include "lock.h"
#include "djyos.h"
#include "stdio.h"

//常用常数定义
//高n位为1，其余低位为0的常数
#define CN_HIGH_1BIT_1      (0x80000000)
#define CN_HIGH_2BIT_1      (0xc0000000)
#define CN_HIGH_3BIT_1      (0xe0000000)
#define CN_HIGH_4BIT_1      (0xf0000000)
#define CN_HIGH_5BIT_1      (0xf8000000)
#define CN_HIGH_6BIT_1      (0xfc000000)
#define CN_HIGH_7BIT_1      (0xfe000000)
#define CN_HIGH_8BIT_1      (0xff000000)
#define CN_HIGH_9BIT_1      (0xff800000)
#define CN_HIGH_10BIT_1     (0xffc00000)
#define CN_HIGH_11BIT_1     (0xffe00000)
#define CN_HIGH_12BIT_1     (0xfff00000)
#define CN_HIGH_13BIT_1     (0xfff80000)
#define CN_HIGH_14BIT_1     (0xfffc0000)
#define CN_HIGH_15BIT_1     (0xfffe0000)
#define CN_HIGH_16BIT_1     (0xffff0000)
#define CN_HIGH_17BIT_1     (0xffff8000)
#define CN_HIGH_18BIT_1     (0xffffc000)
#define CN_HIGH_19BIT_1     (0xffffe000)
#define CN_HIGH_20BIT_1     (0xfffff000)
#define CN_HIGH_21BIT_1     (0xfffff800)
#define CN_HIGH_22BIT_1     (0xfffffc00)
#define CN_HIGH_23BIT_1     (0xfffffe00)
#define CN_HIGH_24BIT_1     (0xffffff00)
#define CN_HIGH_25BIT_1     (0xffffff80)
#define CN_HIGH_26BIT_1     (0xffffffc0)
#define CN_HIGH_27BIT_1     (0xffffffe0)
#define CN_HIGH_28BIT_1     (0xfffffff0)
#define CN_HIGH_29BIT_1     (0xfffffff8)
#define CN_HIGH_30BIT_1     (0xfffffffc)
#define CN_HIGH_31BIT_1     (0xfffffffe)
#define CN_ALLBIT_1         (0xffffffff)

//低n位为1，其余位为0的常数
#define CN_LOW_1BIT_1      (0x00000001)
#define CN_LOW_2BIT_1      (0x00000003)
#define CN_LOW_3BIT_1      (0x00000007)
#define CN_LOW_4BIT_1      (0x0000000f)
#define CN_LOW_5BIT_1      (0x0000001f)
#define CN_LOW_6BIT_1      (0x0000003f)
#define CN_LOW_7BIT_1      (0x0000007f)
#define CN_LOW_8BIT_1      (0x000000ff)
#define CN_LOW_9BIT_1      (0x000001ff)
#define CN_LOW_10BIT_1     (0x000003ff)
#define CN_LOW_11BIT_1     (0x000007ff)
#define CN_LOW_12BIT_1     (0x00000fff)
#define CN_LOW_13BIT_1     (0x00001fff)
#define CN_LOW_14BIT_1     (0x00003fff)
#define CN_LOW_15BIT_1     (0x00007fff)
#define CN_LOW_16BIT_1     (0x0000ffff)
#define CN_LOW_17BIT_1     (0x0001ffff)
#define CN_LOW_18BIT_1     (0x0003ffff)
#define CN_LOW_19BIT_1     (0x0007ffff)
#define CN_LOW_20BIT_1     (0x000fffff)
#define CN_LOW_21BIT_1     (0x001fffff)
#define CN_LOW_22BIT_1     (0x003fffff)
#define CN_LOW_23BIT_1     (0x007fffff)
#define CN_LOW_24BIT_1     (0x00ffffff)
#define CN_LOW_25BIT_1     (0x01ffffff)
#define CN_LOW_26BIT_1     (0x03ffffff)
#define CN_LOW_27BIT_1     (0x07ffffff)
#define CN_LOW_28BIT_1     (0x0fffffff)
#define CN_LOW_29BIT_1     (0x1fffffff)
#define CN_LOW_30BIT_1     (0x3fffffff)
#define CN_LOW_31BIT_1     (0x7fffffff)

//字节数据前导0个数表,256字节,用于快速查寻首个非0位的位置
u8 const cn_leading_zero[]=
{
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const ucpu_t cn_low_xbit_msk_exp2 []=
{
    CN_LOW_1BIT_1,
    CN_LOW_2BIT_1,
    CN_LOW_4BIT_1,
    CN_LOW_8BIT_1,
    CN_LOW_16BIT_1,
    CN_ALLBIT_1,
};

const ucpu_t cn_low_xbit_msk []=
{
    0,
    CN_LOW_1BIT_1,
    CN_LOW_2BIT_1,
    CN_LOW_3BIT_1,
    CN_LOW_4BIT_1,
    CN_LOW_5BIT_1,
    CN_LOW_6BIT_1,
    CN_LOW_7BIT_1,
    CN_LOW_8BIT_1,
    CN_LOW_9BIT_1,
    CN_LOW_10BIT_1,
    CN_LOW_11BIT_1,
    CN_LOW_12BIT_1,
    CN_LOW_13BIT_1,
    CN_LOW_14BIT_1,
    CN_LOW_15BIT_1,
    CN_LOW_16BIT_1,
    CN_LOW_17BIT_1,
    CN_LOW_18BIT_1,
    CN_LOW_19BIT_1,
    CN_LOW_20BIT_1,
    CN_LOW_21BIT_1,
    CN_LOW_22BIT_1,
    CN_LOW_23BIT_1,
    CN_LOW_24BIT_1,
    CN_LOW_25BIT_1,
    CN_LOW_26BIT_1,
    CN_LOW_27BIT_1,
    CN_LOW_28BIT_1,
    CN_LOW_29BIT_1,
    CN_LOW_30BIT_1,
    CN_LOW_31BIT_1,
    CN_ALLBIT_1,
};

struct EventECB;
static struct EventECB *s_ptMemSync;
#define CN_MEM_DOUBLE_PAGE_LOCAL    0xffff
#define CN_MEM_MANY_PAGE_LOCAL      0xfffe
#define CN_MEM_SINGLE_PAGE_GLOBAL   0xfffd
#define CN_MEM_MANY_PAGE_GLOBAL     0xfffc
#define CN_MEM_FREE_PAGE            0xfffb

extern void *pHeapList;             //在脚本中定义
extern struct HeapCB *tg_pHeapList;   //堆链指针，系统中所有的堆被链接在一起。
extern struct HeapCB *tg_pSysHeap;   //堆链指针，系统中所有的堆被链接在一起。

void *__M_Malloc(ptu32_t size,u32 timeout);
void  __M_Free(void * pl_mem);
void *__M_Realloc(void *, ptu32_t NewSize,u32 timeout);
void *__M_MallocHeap(ptu32_t size,struct HeapCB *Heap,u32 timeout);
void *__M_MallocLc(ptu32_t size,u32 timeout);
void *__M_MallocLcHeap(ptu32_t size,struct HeapCB *Heap, u32 timeout);
void  __M_FreeHeap(void * pl_mem,struct HeapCB *Heap);
void *__M_MallocStack(ptu32_t size);

ptu32_t Heap_DynamicModuleInit(ptu32_t para);
void __M_ShowHeap(void);
void __M_ShowHeapSpy(void);

ptu32_t __M_FormatSizeHeap(ptu32_t size,struct HeapCB *Heap);
ptu32_t __M_FormatSize(ptu32_t size);
ptu32_t __M_GetMaxFreeBlockHeap(struct HeapCB *Heap);
ptu32_t __M_GetMaxFreeBlock(void);
ptu32_t M_GetHeapSizeHeap(struct HeapCB *Heap);  //在heap-static.c中
ptu32_t M_GetHeapSize(void);  //在heap-static.c中
struct HeapCB *M_FindHeap(const char *HeapName);
ptu32_t __M_GetFreeMem(void);
ptu32_t __M_GetFreeMemHeap(struct HeapCB *Heap);
ptu32_t __M_CheckSize(void * mp);

extern void *  (*M_Malloc)(ptu32_t size,u32 timeout);
extern void *  (*M_Realloc) (void *, ptu32_t NewSize,u32 timeout);
extern void  (*free)(void * pl_mem);
extern void *  (*M_MallocHeap)(ptu32_t size,struct HeapCB *Heap,u32 timeout);
extern void *  (*M_MallocLc)(ptu32_t size,u32 timeout);
extern void *  (*M_MallocLcHeap)(ptu32_t size,struct HeapCB *Heap, u32 timeout);
extern void    (*M_FreeHeap)(void * pl_mem,struct HeapCB *Heap);
extern void *  (*__MallocStack)(ptu32_t size);
extern ptu32_t (*M_FormatSizeHeap)(ptu32_t size,struct HeapCB *Heap);
extern ptu32_t (*M_FormatSize)(ptu32_t size);
extern ptu32_t (*M_GetMaxFreeBlockHeap)(struct HeapCB *Heap);
extern ptu32_t (*M_GetMaxFreeBlock)(void);
extern ptu32_t (*M_GetFreeMem)(void);
extern ptu32_t (*M_GetFreeMemHeap)(struct HeapCB *Heap);
extern ptu32_t (*M_CheckSize)(void * mp);


//内部函数申明
void *__M_MallocBlock(ufast_t grade,struct HeapCession *Cession);

extern void __Djy_EventReady(struct EventECB *event_ready);
extern void __Djy_CutReadyEvent(struct EventECB *event);
extern void __Djy_ResumeDelay(struct EventECB *delay_event);
extern void __Djy_AddToDelay(u32 u32l_uS);
extern void __Djy_CutEventFromEvttMarked(struct EventECB *event);

//----查找堆-------------------------------------------------------------------
//功能: 通过名字，查找堆控制块指针
//参数: HeapName，堆的名字
//返回: 堆指针，找不到或其他原因返回NULL
//-----------------------------------------------------------------------------
struct HeapCB *M_FindHeap(const char *HeapName)
{
    struct HeapCB *Heap = NULL,*Work;
    if((tg_pHeapList == NULL) || (HeapName == NULL))
        return NULL;
    Work = tg_pHeapList;
    do
    {
        if( ! strcmp(HeapName,Work->HeapName))
        {
            Heap = Work;
            break;
        }
        Work = Work->NextHeap;
    }while(Work != tg_pHeapList);
    return Heap;
}

void __M_ShowHeap(void)
{
    struct HeapCB *Heap;
    struct HeapCession *Cession;
    struct EventECB *ECB;
    u32 n,m,k;

    if(tg_pHeapList == NULL)
        return;
    Heap = tg_pHeapList;
    printf("列出系统中所有的Heap\r\n\r\n");
    do
    {
        printf("Heap name: %s，含 %d 个 Cessions\r\n",
                    Heap->HeapName,Heap->CessionNum);
#if ((CN_CFG_DYNAMIC_MEM == 1))
        ECB = Heap->mem_sync;
        n = 0;
        if(ECB != NULL)
        {
            do
            {
                n++;
                ECB = ECB->multi_next;
            }while(ECB != Heap->mem_sync);
        }
#endif
        printf("有 %d 个事件在阻塞等待本 Heap 分配内存\r\n\r\n",n);
        Cession = Heap->Cession;
        n = 0;
        m = 0;
        k = 0;
        while(Cession != NULL)
        {
            printf("Cession %d:\r\n",n+1);

            printf("准静态分配空间:  0x%08x\r\n",
                    (ptu32_t)(Cession->heap_bottom - Cession->static_bottom));
#if ((CN_CFG_DYNAMIC_MEM == 1))
            printf("动态分配空间:    0x%08x\r\n",
                        Cession->ua_pages_num * Cession->PageSize);
#endif
            printf("静态起始地址:    0x%08x,    动态起始地址:  0x%08x\r\n",
                        (ptu32_t)Cession->static_bottom,
                        (ptu32_t)Cession->heap_bottom);
#if ((CN_CFG_DYNAMIC_MEM == 1))
            printf("总页数:          %10d，   页尺寸:        0x%08x\n\r",
                        Cession->ua_pages_num,Cession->PageSize);
            printf("空闲页数:        %10d，   空闲空间:      0x%08x\r\n",
                        Cession->free_pages_num,
                        Cession->free_pages_num * Cession->PageSize);
            printf("最大块尺寸:      0x%08x，   最大空闲块尺寸:0x%08x\r\n\r\n",
                        Cession->ua_block_max,
                        Cession->ua_free_block_max);
#else
            printf("本cession空闲：  %08x\n\r",(ptu32_t)(Cession->heap_top - Cession->heap_bottom));
#endif
#if ((CN_CFG_DYNAMIC_MEM == 1))
            m += Cession->ua_pages_num * Cession->PageSize;
            k += Cession->free_pages_num * Cession->PageSize;
#else
            k += Cession->heap_top - Cession->heap_bottom;
#endif
            n++;
            Cession = Cession->Next;
        }
        if(n > 1)
        {
            printf("%d个Cession总动态分配空间 %d bytes\r\n",n,m);
            printf("%d个Cession总空闲内存     %d bytes\r\n",n,k);
            printf("\r\n");
        }
        Heap = Heap->NextHeap;
    }while(Heap != tg_pHeapList);
}

void __M_ShowHeapSpy(void)
{
    struct HeapCB *Heap;
    struct HeapCession *Cession;
    u16 *pl_eid;
    u32 n,m,k;

    if(tg_pHeapList == NULL)
        return;
    Heap = tg_pHeapList;
    printf("列出所有Heap的分配情况\r\n");
    do
    {
        printf("\r\nHeap name: %s，含 %d 个 Cessions\r\n",
                    Heap->HeapName,Heap->CessionNum);
        Cession = Heap->Cession;
        n = 0;
        k = 0;
        while(Cession != NULL)
        {
            printf("\r\nCession %d:\r\n",n+1);
            printf("         页号           属性   尺寸       事件\r\n");
            m = 0;
#if ((CN_CFG_DYNAMIC_MEM == 1))
            pl_eid = Cession->index_event_id;
            while(m < Cession->ua_pages_num)
            {
                switch(pl_eid[m])
                {
                    case CN_MEM_DOUBLE_PAGE_LOCAL:
                    {//双页局部分配,-1+id
                        printf("%10d = %-10d 局部  0x%08x %05d\n\r", m, m + 1, Cession->PageSize*2,pl_eid[m + 1]);
                        m += 2;
                    }break;
                    case CN_MEM_MANY_PAGE_LOCAL:
                    {//多页局部分配:-2+id+阶号
                        printf("%10d = %-10d 局部   0x%08x %05d\n\r",
                                    m,m+(1<<pl_eid[m+2])-1, Cession->PageSize*(1<<pl_eid[m+2]),pl_eid[m+1]);
                        m += 1<<pl_eid[m+2];
                    }break;
                    case CN_MEM_SINGLE_PAGE_GLOBAL:
                    {//单页全局内存:-3
                        printf("%10d - %-10d 全局   0x%08x\n\r",m,m,Cession->PageSize);
                        m++;
                    }break;
                    case CN_MEM_MANY_PAGE_GLOBAL:
                    {//双(多)页全局内存:-4+阶号.
                        printf("%10d - %-10d 全局   0x%08x\n\r",m,m+(1<<pl_eid[m+1])-1, Cession->PageSize*(1<<pl_eid[m+1]));
                        m += 1<<pl_eid[m+1];
                    }break;
                    case CN_MEM_FREE_PAGE:
                    {//空闲内存
                        for(k = m+1; k < Cession->ua_pages_num; k++)
                        {
                            if(pl_eid[k] != CN_MEM_FREE_PAGE)
                                break;
                        }
                        printf("%10d ~ %-10d 空闲   0x%08x\n\r",m,k-1,Cession->PageSize*(k-m));
                        m = k;
                    }break;
                    default :
                    {
                        printf("%10d = %-10d 局部   0x%08x %05d\n\r",m,m,Cession->PageSize,pl_eid[m]);
                        m++;
                    }break;
                }
            }
#endif
            n++;
            Cession = Cession->Next;
        }
        Heap = Heap->NextHeap;
    }while(Heap != tg_pHeapList);
}

//----查询内存尺寸-------------------------------------------------------------
//功能: 根据给定的指针,查询该指针所在的内存块的尺寸.
//参数: mp,动态分配的内存指针.
//返回: 内存块尺寸,返回0有几种含义:1.非法指针,2.mp是由准静态分配的指针.
//-----------------------------------------------------------------------------
ptu32_t __M_CheckSize(void * mp)
{
    struct HeapCB *Heap;
    struct HeapCession *Cession;
    ptu32_t ua_pages_number;
    ptu32_t *temp;
    uint16_t *pl_id;
    ufast_t uf_free_grade_th;

    if(tg_pHeapList == NULL)
        return 0;
    Heap = tg_pHeapList;
    do
    {
        Cession = Heap->Cession;

        while(Cession != NULL)
        {

            if(((u8*)mp >= Cession->static_bottom)
                        && ((u8*)mp < Cession->heap_top))
            {
                if((u8*)mp < Cession->heap_bottom)    //该指针在静态分配区
                {
                    temp = mp;
                    temp--;
                    return *temp;
                }else                   //该指针在块相联动态分配区
                {
                #if ((CN_CFG_DYNAMIC_MEM == 1))
                    //计算释放的内存的页号
                    ua_pages_number=(ptu32_t)((u8*)mp - Cession->heap_bottom)
                                                /Cession->PageSize;

                    //查找释放的内存块的阶号,从0起计.通过阶号也可以确定内存块的
                    //大小.确定内存分配类型,局部分配需要知道拥有该内存的事件id，
                    //全局分配无需清除内存分配跟踪表,无需知道拥有该内存的事件id
                    pl_id = &Cession->index_event_id[ua_pages_number];
                    switch(pl_id[0])
                    {
                        case CN_MEM_DOUBLE_PAGE_LOCAL :
                        {//双页局部分配,-1+id
                            uf_free_grade_th = 1;
                        }break;
                        case CN_MEM_MANY_PAGE_LOCAL :
                        {//多页局部分配:-2+id+阶号
                            uf_free_grade_th = (ufast_t)pl_id[2];
                        }break;
                        case CN_MEM_SINGLE_PAGE_GLOBAL :
                        {//单页全局内存:-3
                            uf_free_grade_th = 0;
                        }break;
                        case CN_MEM_MANY_PAGE_GLOBAL :
                        {//双(多)页全局内存:-4+阶号.
                            uf_free_grade_th = (ufast_t)pl_id[1];
                        }break;
                        case CN_MEM_FREE_PAGE :
                        {//双(多)页全局内存:-4+阶号.
                            uf_free_grade_th = CN_LIMIT_UFAST;
                        }break;
                        default :
                        {
                            if(pl_id[0] <= CN_EVENT_ID_LIMIT)
                                uf_free_grade_th = 0;       //单页局部分配:id,
                            else
                            {
                                Djy_SaveLastError(EN_MEM_ERROR);
                                return 0;
                            }
                        }break;
                    }
                    if(uf_free_grade_th == CN_LIMIT_UFAST)
                        return 0;
                    else
                        return Cession->PageSize << uf_free_grade_th;
                #endif
                }
            }
            Cession = Cession->Next;
        }
        Heap = Heap->NextHeap;
    }while(Heap != tg_pHeapList);

    //mp指针不在任何heap的范围之内，程序将走到这里。
    return 0;
}

//----返回数组中首位1位置（前导0的个数）----------------------------------------
//功能：从数组的最高字节的最高位开始计算前导0的数量。
//参数：data,输入数组指针
//      len,数组的大小
//返回：前导0的个数
//------------------------------------------------------------------------------
ucpu_t __M_LeadingZero(u8 *data,u8 len)
{
    u8 u8_i;
    ucpu_t  uc_j=0;
#if CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN
    u8_i=len-1;
    do
    {
        if(cn_leading_zero[data[u8_i]] != 8)
        {
            uc_j+=cn_leading_zero[data[u8_i]];
            break;
        }else
            uc_j +=8;
    }while(u8_i-- !=0);
#else
    u8_i=0;
    do
    {
        if(cn_leading_zero[data[u8_i]] != 8)
        {
            uc_j+=cn_leading_zero[data[u8_i]];
            break;
        }else
            uc_j +=8;
    }while(u8_i++ != len-1);
#endif
    return uc_j;
}

//----计算控制表所需的字节数---------------------------------------------------
//功能：计算控制表所需的字节数
//参数：u32_pages,内存页数
//返回：字节数
//注意: 保持函数功能的单一性,不要同时计算控制结构各部分的字节数
//      移植敏感:本函数与你的机器的对齐方式有关,移植时你必须了解机器的对齐方式.
//      本函数能满足绝大部分cpu的对齐要求,但不排除有例外.所以在移植时你最好读懂
//      本函数，然后考虑是否需要修改
//-----------------------------------------------------------------------------
u32 __M_CalculateExpense(ptu32_t ua_pages)
{
    ptu32_t ua_bits,u32_return;
    ufast_t uf_grades,uf_grade_th;

    //计算阶数，即一共有多少种页尺寸,方法:地址长度减去ua_pages中前导0的数量就是
    //阶数,例如,ua_pages=5,最高位位置是bit2,有29个前导0，页尺寸有1,2,4页3种,阶数=3
    uf_grades=CN_PTR_BITS-__M_LeadingZero((u8 *)&ua_pages,sizeof(ptu32_t));

    //每页分配一个16位字记录拥有本页内存的事件的ID。
    //当分配高阶内存块时，只记录首页，free时并不清除。设置本字是为了防止在
    //事件间交叉分配和释放内存时不至于出错，但并不建议用户这样使用。
    u32_return = ua_pages * sizeof(u16);

    //下一个数据域是ufast_t类型的,需要按ufast_t类型对齐.
    u32_return = align_up(sizeof(ufast_t),u32_return);

    //每阶还要分配一个ufast_t类型的数据,纪录本阶的级数
    u32_return += uf_grades*(sizeof(ufast_t));

    //下一个数据域是指针,需要按指针对齐
    u32_return = align_up(sizeof(void*),u32_return);

    //每阶一个指针,指向位图所引表
    u32_return += uf_grades*sizeof(void*);

    for(uf_grade_th = 0; uf_grade_th < uf_grades; uf_grade_th++)
    {
        ua_bits = ua_pages>>uf_grade_th;   //计算本阶总位数
        do
        {
            ua_bits=(ua_bits+CN_CPU_BITS-1)>>CN_CPU_BITS_SUFFIX_ZERO;
            u32_return += ua_bits*sizeof(ucpu_t);   //该级位图字数
            u32_return += sizeof(void*);            //需要一个指针指向该位图首字
        }while(ua_bits>1); //直到所有级都分配完.
    }
    return u32_return;
}

//----返回ucpu_t类数据前导0的个数----------------------------------------------
//功能：从ucpu_t类数据的最高位开始计算前导0的数量。
//参数：data,输入数
//返回：前导0的个数
//-----------------------------------------------------------------------------
ucpu_t __M_LeadingUcpuZero(ucpu_t data)
{
    return(__M_LeadingZero((u8*)&data,sizeof(ucpu_t)));
}

//----返回规格化阶-------------------------------------------------------------
//功能：把任意数据规格化为规格化尺寸。调用前，已知size小于Cession的最大块尺寸
//参数：size,欲分配的内存块尺寸
//      Heap，目标堆。
//返回：阶号。
//-----------------------------------------------------------------------------
ufast_t __M_GetGrade(ptu32_t size,struct HeapCession *Cession)
{
    ucpu_t  grade;
    u32 pages;

    pages = (size + Cession->PageSize -1)/Cession->PageSize;
    grade = __M_LeadingZero((u8 *)&pages,sizeof(u32));
    if((CN_HIGH_1BIT_1>>grade) != pages)
    //如果grade不是规格化的数,调整到比它大的最小允许块大小
    //否则维持size的大小,不做调整
        grade--;
    grade = CN_PTR_BITS-1-grade;
    //返回阶数,从0起计,即最小页尺寸前导0减块尺寸前导0.
    return grade;
}


//----返回空闲的规格化阶-------------------------------------------------------
//功能：把任意数据规格化为堆中空闲内存的规格化。用阶号和cession表示，同样的阶
//      号，不同cession，其尺寸是不一样的。
//      调用本函数前，已经确认heap中有足够的空闲内存。
//参数：size,欲分配的内存块尺寸
//      Heap，目标堆。
//      result，包含合适的空闲内存块的Cession。
//返回：阶号。
//-----------------------------------------------------------------------------
ufast_t __M_GetFreeGrade(ptu32_t size,struct HeapCB *Heap,
                         struct HeapCession **result)
{
    ucpu_t  grade,result_grade = 0;
    struct HeapCession *Cession;
    struct HeapCB *Work;
    ptu32_t tsize,fsize = CN_LIMIT_PTU32;
    u32 pages;

    if((Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_PRIVATE )
    {
        Cession = Heap->Cession;
        while(Cession != NULL)
        {
            if(Cession->ua_free_block_max >= size)
            {
                pages = (size + Cession->PageSize -1)/Cession->PageSize;
                grade = __M_LeadingZero((u8 *)&pages,sizeof(u32));
                if((CN_HIGH_1BIT_1>>grade) != pages)
                //如果grade不是规格化的数,调整到比它大的最小允许块大小
                //否则维持size的大小,不做调整
                    grade--;
                grade = CN_PTR_BITS-1-grade;
                tsize = Cession->PageSize << grade;
                if(tsize < fsize)
                {
                    *result = Cession;
                    fsize = tsize;
                    result_grade = grade;
                 }
            }
            Cession = Cession->Next;
        }
    }
    else
    {
        Work = Heap;
        do
        {
            if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
            {
                Cession = Work->Cession;
                while(Cession != NULL)
                {
                    if(Cession->ua_free_block_max >= size)
                    {
                        pages = (size + Cession->PageSize -1)/Cession->PageSize;
                        grade = __M_LeadingZero((u8 *)&pages,sizeof(u32));
                        if((CN_HIGH_1BIT_1>>grade) != pages)
                        //如果grade不是规格化的数,调整到比它大的最小允许块大小
                        //否则维持size的大小,不做调整
                            grade--;
                        grade = CN_PTR_BITS-1-grade;
                        tsize = Cession->PageSize << grade;
                        if(tsize < fsize)
                        {
                            *result = Cession;
                            fsize = tsize;
                            result_grade = grade;
                         }
                    }
                    Cession = Cession->Next;
                }
            }
            Work = Work->NextHeap;
        }while(Work != Heap);
    }

    //返回阶数,从0起计,即最小页尺寸前导0减块尺寸前导0.
    return result_grade;
}
//----查看并等待空闲内存----------------------------------------------------
//功能：等待队列是一个经过排序的双向循环链表,按照申请的内存量排序,小的在前,
//      当内存可用时,首先使申请量小的事件得到满足,这样可以使内存满足尽量多事件,
//      不使用优先级排序,因为实时事件是不允许使用动态分配的,所以不会影响实时性.
//      如果Heap的属性是通用堆,则使用通用堆的等待指针,如果阻塞,只要任意一个通用
//      堆有空闲内存,就返回true.
//参数：size，需要申请的内存尺寸
//      Heap，目标heap
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，
//      非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回: true = 有空闲内存供分配，false = 无内存可分配
//备注: 本函数由操作系统调用,用户不能调用.
//更新记录:
// 1.日期: 2015/4/25
//   说明: 增加判断通用堆还是专用堆
//   作者: 罗侍田
//------------------------------------------------------------------------------
bool_t __M_CheckMemory(ptu32_t size,struct HeapCB *Heap,u32 timeout)
{
    struct EventECB *event;
    struct EventECB **MemSyncHead;
    u32 u32l_start_time,u32l_rest_time = 0;
    bool_t   wait;
    ptu32_t FreeSize;

    u32l_start_time = (u32)DjyGetSysTime();
    if( (Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_PRIVATE)
        MemSyncHead = &Heap->mem_sync;      //使用专用堆的同步指针
    else
        MemSyncHead = &s_ptMemSync;         //使用通用堆的同步指针

    do
    {
        wait=false;

        FreeSize = __M_GetMaxFreeBlockHeap(Heap );
        if(FreeSize < size)
        {//最大的可分配块尺寸小于请求的内存,没有内存可以分配
            Int_SaveAsynSignal();
            wait=true;  //如果是经过了阻塞后重新执行,while循环需要再次判断
                        //是否有足够的内存分配.
            __Djy_CutReadyEvent(g_ptEventRunning);

            g_ptEventRunning->next = NULL;
            g_ptEventRunning->previous = NULL;

            g_ptEventRunning->wait_mem_size = size; //填充等待的字节数
            event = *MemSyncHead;       //获取内存等待表指针

            //以下把事件置入内存等待队列中
            g_ptEventRunning->sync_head = MemSyncHead;
            if(event == NULL)    //等待队列空
            {
                g_ptEventRunning->multi_next = g_ptEventRunning;
                g_ptEventRunning->multi_previous = g_ptEventRunning;
                Heap->mem_sync = g_ptEventRunning;
            }else
            {
                do
                {//本循环找到第一个请求内存大于新事件的事件.
                    if(event->wait_mem_size < size)
                        event = event->multi_next;
                    else
                        break;
                }while(event != *MemSyncHead);
                //如果没有找到申请内存比新事件长的事件,新事件插入队列尾,而
                //队列尾部就是event_wait的前面,此时event恰好等于event_wait
                //如果找到剩余延时时间长于新事件的事件,新事件插入该事件前面.
                //所以无论前面循环找到与否,均可使用下列代码
                g_ptEventRunning->multi_next = event;
                g_ptEventRunning->multi_previous = event->multi_previous;
                event->multi_previous->multi_next = g_ptEventRunning;
                event->multi_previous = g_ptEventRunning;
            }

            if(timeout != CN_TIMEOUT_FOREVER)
            {
                g_ptEventRunning->event_status= CN_STS_WAIT_MEMORY
                                                + CN_STS_SYNC_TIMEOUT;
                u32l_rest_time = timeout-((u32)(u32)DjyGetSysTime() - u32l_start_time);
                __Djy_AddToDelay(timeout-u32l_rest_time);
            }else
            {
                g_ptEventRunning->event_status = CN_STS_WAIT_MEMORY;
            }

            Int_RestoreAsynSignal();  //打开中断才能实现调度
            //检查从哪里返回，是超时还是有事件释放内存
            if(g_ptEventRunning->wakeup_from & CN_STS_SYNC_TIMEOUT)
            {//如果从超时返回，则应该返回false，说明没有内存可用
             //如果非超时返回，说明有线程释放内存，将进入下一次while循环
             //检查是否有足够的内存,
                return false;   //超时返回
            }
        }
    }while(wait==true);
    return true;    //return true前是不开中断的,以免在分配内存之前发生中断.
}

//----把事件放入等待队列-------------------------------------------------------
//功能: 把事件直接放进等待队列,不调度.等待队列是一个按请求的内存从小到大排列的
//      双向循环链表.用内存尺寸排序而不是用优先级排队，基于两点考量:1、可以最大
//      程度满足需求，2、在应用程序设计中，一般不会在非常紧急、优先级非常高的
//      事件处理中使用动态分配方法。
//      本函数只提供给djyos.c中创建线程时使用，对通用堆操作
//参数: event,待进入等待队列的事件指针
//返回: 无
//更新记录:
// 1.日期: 2015/4/25
//   说明: 改为把事件放进通用堆等待队列中
//   作者: 罗侍田
//-----------------------------------------------------------------------------
void __M_WaitMemory(struct EventECB *event)
{
    struct EventECB *pl_event;
    ptu32_t size;

    size = event->wait_mem_size;
    pl_event = s_ptMemSync;     //获取内存等待表指针
    event->sync_head = &s_ptMemSync;
    if(pl_event == NULL)            //等待队列空
    {
        event->next = NULL;
        event->previous = NULL;

        event->multi_next = event;
        event->multi_previous = event;

        s_ptMemSync = event;
    }else
    {
        do
        {//本循环找到第一个请求内存大于新事件的事件.
            if(pl_event->wait_mem_size < size)
                pl_event = pl_event->multi_next;
            else
                break;
        }while(pl_event != s_ptMemSync);
        //如果没有找到申请内存比新事件长的事件,新事件插入队列尾,而队列
        //尾部就是event_wait的前面,此时event恰好等于event_wait
        //如果找到剩余延时时间长于新事件的事件,新事件插入该事件前面.
        //所以无论前面循环找到与否,均可使用下列代码
        event->multi_next = pl_event;
        event->multi_previous = pl_event->multi_previous;
        pl_event->multi_previous->multi_next = event;
        pl_event->multi_previous = event;
    }
    return;
}

//----回收running事件申请的内存------------------------------------------------
//功能: 在内存管理的事件id表中查找由event_id事件申请的局部内存，如果有，则登记
//      enum_mem_leak错误，并强行释放之。
//参数: event_id，目标事件id
//返回: 无
//-----------------------------------------------------------------------------
void __M_CleanUp(uint16_t event_id)
{
    struct HeapCB *Heap;
    struct HeapCession *Cession;
    uint16_t *pl_id;
    u32 n;

    if(M_Malloc != __M_Malloc)
        return;             //动态内存分配未初始化。
    Heap = tg_pHeapList;
    do
    {
        Cession = Heap->Cession;

        while(Cession != NULL)
        {
            pl_id = Cession->index_event_id;
            n = 0;
            while(n < Cession->ua_pages_num)
            {
            //阅读以下条件句请结合struct HeapCession中index_event_id的注释.
                if(pl_id[n] == event_id)
                {
                    __M_Free(Cession->PageSize * n + Cession->heap_bottom);
                    n++;
                }
                else if( (pl_id[n] == CN_MEM_DOUBLE_PAGE_LOCAL)
                          && (pl_id[n+1] == event_id))
                {
                    __M_Free(Cession->PageSize * n + Cession->heap_bottom);
                    n += 2;
                }
                else if( (pl_id[n] == CN_MEM_MANY_PAGE_LOCAL)
                          && (pl_id[n+1] == event_id))
                {
                    __M_Free(Cession->PageSize * n + Cession->heap_bottom);
                    n += 1<<pl_id[n+1];
                }
            }
            Cession = Cession->Next;
        }
        Heap = Heap->NextHeap;
    }while(Heap != tg_pHeapList);

    //mp指针不在任何heap的范围之内，程序将走到这里。
    return ;
}

//----初始化内存堆-------------------------------------------------------------
//功能：1.从堆中分配访问控制结构所需的内存，位置在堆的顶部，占用整数块
//      2.初始化内存分配控制结构,初始化全局内存结构 struct MemGlobal
//      3.初始化内存分配表
//返回：1=成功，0=失败
//备注: 1.本函数在初始化完成之前调用,异步信号尚未开启,无需考虑关闭中断的问题.
//      2.调用本函数前,只能能执行静态分配功能,不能执行动态分配功能.
//      3.目前只适用于不带mmu的情况，带mmu特别是支持多进程的情况还没有想好。
//-----------------------------------------------------------------------------
ptu32_t Heap_DynamicModuleInit(ptu32_t para)
{
    ptu32_t ua_temp,ua_temp1=0,ua_temp2,ua_temp3 = 0,ua_faction;
    ptu32_t ua_pages,ua_table_size=0;
    ufast_t uf_classes,uf_grades;
    ucpu_t  ***pppl_bitmap,**ppl_bitmap,*pl_bitmap;
    ufast_t *pl_classes;

    struct HeapCB *Heap;
    struct HeapCession *Cession;

    if(tg_pHeapList == NULL)
        return 0;
    Heap = tg_pHeapList;
    do
    {
        Cession = Heap->Cession;
        Lock_MutexCreate_s(&(Heap->HeapMutex),(const char*)(Heap->HeapName));

        while(Cession != NULL)
        {
            ua_faction=(ptu32_t)(Cession->heap_top-Cession->heap_bottom);
            //计算堆的总页数，取整页，舍去余数部分
            ua_pages = ua_faction / Cession->PageSize;
            ua_faction = ua_faction % Cession->PageSize;
            //计算堆页数调整后的余数，可用于内存控制结构使用。
            Cession->heap_top -=ua_faction;

            if(ua_pages < 2) //如果堆中页数小于2页,控制结构可能还要占用1页,还有意义吗?
            {
                Cession->ua_pages_num = 0;
                Cession->free_pages_num = 0;
                Cession->ua_block_max = 0;
                Cession->ua_free_block_max = 0;
                Cession->uf_grades = 0;
                Cession = Cession->Next;
                continue;
            }

            ua_temp = __M_CalculateExpense(ua_pages); //初估控制结构需要的字节数
            if(ua_faction < ua_temp)
            {
                //初估控制结构除ua_faction外还需要的页数
                ua_temp = (ua_temp-ua_faction+Cession->PageSize-1)
                            /Cession->PageSize;
                for(ua_temp1 = ua_temp; ua_temp1 > 0; ua_temp1--)
                {//初估的页数肯定大于或等于实际需要的页数,尤其是内存页数较多时.
                 //估计值从初估的页数开始逐一减小,直到合适为止.
                    //重估控制结构字节数
                    ua_temp2 = __M_CalculateExpense(ua_pages-ua_temp1);
                    ua_temp3 = (ua_temp2-ua_faction+Cession->PageSize-1)
                            /Cession->PageSize;         //重新计算控制结构页数
                    if(ua_temp3 >= ua_temp1)
                    //当所需页数刚好大于或等于估计值时,循环终止
                        break;
                }
                //实际可分配的页数,u32_temp3为控制结构页数
                ua_pages = ua_pages -ua_temp3;
                Cession->heap_top -= ua_temp3 * Cession->PageSize;  //调整堆顶
            }
            Cession->ua_pages_num = ua_pages;
            Cession->free_pages_num = ua_pages;
            //内存控制结构清零
            memset(Cession->heap_top,0,ua_faction+(ua_temp3 *Cession->PageSize));

            //计算总阶数，即一共有多少种页尺寸,方法:地址长度减去ua_pages中前导0
            //的数量例如,ua_pages=5,最高位位置是bit2,有29个前导0，页尺寸有1,2,4
            //页3种,阶数=3
            uf_grades=CN_PTR_BITS-__M_LeadingZero((u8 *)&ua_pages,sizeof(ptu32_t));

            Cession->uf_grades = uf_grades;    //总阶数
            Cession->ua_block_max=Cession->PageSize<<(uf_grades-1);//最高阶块尺寸
            Cession->ua_free_block_max=Cession->ua_block_max; //最大可用块尺寸

            //计算位图指针表所需的指针数
            ua_table_size=0;
            for(ua_temp=0;ua_temp<uf_grades;ua_temp++)
            {//遍历各阶
                ua_temp1 = ua_pages>>ua_temp;   //计算ua_temp阶0级位图位数
                do
                {
                    //计算下一级位图的位数
                    ua_temp1=(ua_temp1+CN_CPU_BITS-1)>>CN_CPU_BITS_SUFFIX_ZERO;
                    ua_table_size++;        //需要一个指针指向该级位图首地址
                //下一级位图超过1位(本级超过cn_cpu_bits位),继续循环
                }while(ua_temp1>1);
            }

            //事件指针id表,详细说明见mem_global_t类型定义部分.
            Cession->index_event_id=(uint16_t*)Cession->heap_top;
            for(ua_temp = 0; ua_temp<ua_pages; ua_temp++)
                Cession->index_event_id[ua_temp] = CN_MEM_FREE_PAGE;
            //各阶的位图级数表指针，每阶用一个ufast_t型的数表示该阶的位图级数
            ua_temp = (ptu32_t)(Cession->index_event_id+ua_pages);
            ua_temp = (ua_temp+sizeof(ufast_t)-1)&(~(sizeof(ufast_t)-1)); //对齐
            pl_classes=(ufast_t *)ua_temp;
            Cession->p_classes=pl_classes;     //各阶空闲金字塔级数表首指针

            //随后是各级分配表指针的指针首址
            ua_temp = (ptu32_t)(pl_classes+uf_grades);
            ua_temp = (ua_temp+sizeof(void *)-1)&(~(sizeof(void *)-1));  //对齐
            pppl_bitmap=(void*)ua_temp;
            //各阶金字塔位图指针表的指针表的首指针
            Cession->ppp_bitmap=pppl_bitmap;

            //金字塔位图指针表的指针
            ppl_bitmap=(ucpu_t **)(pppl_bitmap+uf_grades);
            pl_bitmap=(ucpu_t *)(ppl_bitmap+ua_table_size); //金字塔位图指针表

            //填充各索引表和分配表
            for(ua_temp=0;ua_temp<uf_grades;ua_temp++)  //遍历各阶
            {
                ua_temp1 = ua_pages>>ua_temp;       //ua_temp阶位图总位数
                uf_classes=0;                       //ua_temp阶的位图级数
                pppl_bitmap[ua_temp]=ppl_bitmap;    //ua_temp阶位图表的指针
                do
                {
                    ucpu_t  msk=~0;     //类型位宽未知,用此方法置全1
                    ua_temp3=ua_temp1>>CN_CPU_BITS_SUFFIX_ZERO; //本级表的大小(整数部分)
                    ua_temp1=ua_temp1%CN_CPU_BITS;    //本路径级表的大小(余数部分)
                    if(ua_temp1==0)
                    //表中没有多余的位,无需处理
                        ua_temp1=ua_temp3;  //本级位图所占字数是下一级的位数
                    else
                    {//表中有多余的位,置1,使多余的位等同于已分配的位.
                        msk=msk<<ua_temp1;              //使多余的位置1
                        *(pl_bitmap+ua_temp3) |=msk;    //修改位图中相应的位
                        ua_temp1=ua_temp3+1;    //路径表修正,加上余数部分
                    }
                    *ppl_bitmap++ = pl_bitmap;  //记录本级位图指针
                    pl_bitmap +=ua_temp1;       //位图指针指向下一级位图起始地址
                    uf_classes++;                //ua_temp阶的级数增量
                }while(ua_temp1>1);             //直到遍历本阶所有位图级
                pl_classes[ua_temp]=uf_classes; //该级路径表深度
            }
            Cession = Cession->Next;
        }       //for while(Cession != NULL)
        Heap = Heap->NextHeap;
    }while(Heap != tg_pHeapList);

    M_Malloc = __M_Malloc;
    M_Realloc = __M_Realloc;
    free = __M_Free;
    M_MallocHeap = __M_MallocHeap;
    M_MallocLc = __M_MallocLc;
    M_MallocLcHeap = __M_MallocLcHeap;
    M_FreeHeap = __M_FreeHeap;
    __MallocStack = __M_MallocStack;
    M_FormatSizeHeap = __M_FormatSizeHeap;
    M_FormatSize = __M_FormatSize;
    M_GetMaxFreeBlockHeap = __M_GetMaxFreeBlockHeap;
    M_GetMaxFreeBlock = __M_GetMaxFreeBlock;
    M_GetFreeMem = __M_GetFreeMem;
    M_GetFreeMemHeap = __M_GetFreeMemHeap;
    M_CheckSize = __M_CheckSize;

    return 1;
}

//----从内存堆中分配内存-------------------------------------------------------
//功能：1.规格化内存尺寸，计算满足要求的最小内存尺寸，计算该块内存尺寸的阶数
//      2.读取该级访问路径深度，沿访问路径逐级查找，直到找到空闲内存为止。
//      3.重置内存分配表
//      4.把内存分配信息置入链表中,以备清理内存用.
//      5.如果内存不足，则把事件挂在tg_mem_global.event_wait下,并引发事件切换.
//参数：size,欲分配的内存块尺寸
//      Heap, 目标堆,如果这是个专用堆,则限定从这个堆分配,否则从所有通用堆分配
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//      超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回：分配的内存指针，NULL表示没有内存可以分配
//备注: 如果在多事件调度启动前调用本函数，记录拥有者时全部算在系统服务事件中。
//      系统服务事件永不结束，故等同于全局分配。
//-----------------------------------------------------------------------------
void *__M_MallocLcHeap(ptu32_t size,struct HeapCB *Heap,u32 timeout)
{
    struct HeapCession *Cession;
    void *ua_address;
    ufast_t  uf_grade_th;
    bool_t   en_scheduler;
    void *result;
    uint16_t *pl_id,id;
    u32 page;

    if( (size == 0) || (Heap == NULL) )
        return NULL;

    //不能在此直接判断size是否满足,因为取得互斥量前可能重入而判断无效.
    if(Lock_MutexPend(&Heap->HeapMutex, CN_TIMEOUT_FOREVER) == false)
        return NULL;
    en_scheduler = Djy_QuerySch( );
    //禁止调度条件下，如果当前没有足够的空闲内存，乖乖的走吧。
    if((__M_GetMaxFreeBlockHeap(Heap) < size)
                && ((timeout == 0) || !en_scheduler))
    {
        result = NULL;
    }else
    {
        if( !__M_CheckMemory(size,Heap,timeout) )
        {
            result = NULL;        //没有合适的空闲内存块
        }else
        {    //有合适的空闲内存块
            uf_grade_th=__M_GetFreeGrade(size,Heap,&Cession);    //取阶号
            ua_address=__M_MallocBlock(uf_grade_th,Cession);     //申请内存
            g_ptEventRunning->local_memory++;

            //阅读以下条件句请结合struct HeapCession中index_event_id的注释.
            pl_id = Cession->index_event_id;
            id = g_ptEventRunning->event_id;
            page = (ptu32_t)((u8*)ua_address-Cession->heap_bottom)/Cession->PageSize;
            if(uf_grade_th==0)
            {//分配1页
                pl_id[page] = id;
            }else if(uf_grade_th==1)
            {//分配2页
                pl_id[page] = CN_MEM_DOUBLE_PAGE_LOCAL;
                pl_id[page+1] = id;
            }else
            {   //分配多页
                pl_id[page] = CN_MEM_MANY_PAGE_LOCAL;
                pl_id[page+1] = id;
                pl_id[page+2] = uf_grade_th;
            }
            result = ua_address;
        }
    }
    Lock_MutexPost(&Heap->HeapMutex);
    return result;
}

void *__M_MallocLc(ptu32_t size,u32 timeout)
{
    return ( __M_MallocLcHeap(size,tg_pSysHeap,timeout) );
}

//----分配全局内存-------------------------------------------------------------
//功能：1.规格化内存尺寸，计算满足要求的最小内存尺寸，计算该块内存属于第几阶。
//      2.读取该级访问路径深度，沿访问路径逐级查找，直到找到空闲内存为止。
//      3.重置内存分配表
//      4.如果内存不足，则把事件挂在tg_mem_global.event_wait下,并引发事件切换.
//参数：size,欲分配的内存块尺寸
//      Heap, 目标堆,如果这是个专用堆,则限定从这个堆分配,否则从所有通用堆分配
//      timeout，超时设置,单位是微秒，CN_TIMEOUT_FOREVER=无限等待，0则立即按
//      超时返回。非0值将被向上调整为CN_CFG_TICK_US的整数倍
//返回：分配的内存指针，NULL表示没有内存可以分配
//备注: 用此函数分配的内存,并不会在事件完成时被收回.
//-----------------------------------------------------------------------------
void *__M_MallocHeap(ptu32_t size,struct HeapCB *Heap, u32 timeout)
{
    struct HeapCession *Cession;
    void *ua_address;
    ufast_t  uf_grade_th;
    uint16_t    *pl_id;
    bool_t   en_scheduler;
    void *result;

    if( (size == 0) || (Heap == NULL) )
        return NULL;
    //不能在此直接判断size是否满足,因为关取得互斥量前可能发生切换而判断无效.
    if(Lock_MutexPend(&Heap->HeapMutex,CN_TIMEOUT_FOREVER) == false)
        return NULL;
    en_scheduler = Djy_QuerySch();
    //禁止调度条件下，如果当前没有足够的空闲内存，乖乖的走吧。
    if((__M_GetMaxFreeBlockHeap(Heap) < size)
                && ((timeout == 0) || !en_scheduler))
    {
        result = NULL;
    }else
    {
        if( ! __M_CheckMemory(size,Heap,timeout))
        {
            result = NULL;
        }else
        {
            uf_grade_th=__M_GetFreeGrade(size,Heap,&Cession);    //取阶号
            ua_address=__M_MallocBlock(uf_grade_th,Cession);     //申请内存

            //以下在id表中记录本次分配的性质
            //阅读本段代码请结合mem_global_t中index_event_id成员定义的注释.
            pl_id = &Cession->index_event_id
                    [(ptu32_t)((u8*)ua_address-Cession->heap_bottom)
                            /Cession->PageSize];
            if(0==uf_grade_th)
                *pl_id = CN_MEM_SINGLE_PAGE_GLOBAL;
            else
            {
                *pl_id++ = CN_MEM_MANY_PAGE_GLOBAL;
                *pl_id = uf_grade_th;
            }
            result = ua_address;
        }
    }
    Lock_MutexPost(&Heap->HeapMutex);
    return result;
}
void *__M_Malloc(ptu32_t size,u32 timeout)
{
    return (__M_MallocHeap(size,tg_pSysHeap,timeout));
}

//----分配线程栈---------------------------------------------------------------
//功能：与m_malloc_gbl相似，但要求在关异步信号的条件下调用，本函数仅提供给创建
//      线程函数__CreateThread使用
//参数：size,欲分配的内存块尺寸
//返回：分配的内存指针，NULL表示没有内存可以分配
//更新记录:
// 1.日期: 2015/4/25
//   说明: 改从tg_pSysHeap堆中分配为从所有通用堆中分配,优先从tg_pSysHeap中分配。
//   作者: 罗侍田
//-----------------------------------------------------------------------------
void *__M_MallocStack(ptu32_t size)
{
    struct HeapCession *Cession;
    void *ua_address;
    ufast_t  uf_grade_th;
    u16    *pl_id;
    void *result;

    if( (size == 0) || (tg_pSysHeap == NULL) )
        return NULL;
    if(__M_GetMaxFreeBlockHeap(tg_pSysHeap) < size)
    {
        result = NULL;
    }else
    {
        uf_grade_th=__M_GetFreeGrade(size,tg_pSysHeap,&Cession);    //取阶号
        ua_address=__M_MallocBlock(uf_grade_th,Cession);     //申请内存

            //以下在id表中记录本次分配的性质
            //阅读本段代码请结合mem_global_t中index_event_id成员定义的注释.
            pl_id = &Cession->index_event_id
                    [(ptu32_t)((u8*)ua_address-Cession->heap_bottom)
                              /Cession->PageSize];
            if(0==uf_grade_th)
                *pl_id = CN_MEM_SINGLE_PAGE_GLOBAL;
            else
            {
                *pl_id++ = CN_MEM_MANY_PAGE_GLOBAL;
                *pl_id = uf_grade_th;
            }
            result = ua_address;
    }
    return result;
}

//----分配1块内存--------------------------------------------------------------
//功能: 从内存堆中分配一个内存块,内部调用,不开放给用户
//参数: grade,欲分配的块的阶号,0=1阶,1=2阶,类推之
//      Cession，从Cession段中分配
//返回: 获得的内存块指针,如果不能分配则返回NULL
//-----------------------------------------------------------------------------
void *__M_MallocBlock(ufast_t grade,struct HeapCession *Cession)
{
    ptu32_t ua_temp1;
    ptu32_t ua_pages_number;
    ufast_t uf_grade_th,uf_classes;
    u32 u32WordShift;
    ufast_t *pl_classes;
    ucpu_t  ***pppl_bitmap;
    ucpu_t  **ppl_bitmap;
    ucpu_t  *pl_bitmap;

    ptu32_t ua_word_offset,ua_bit_num,ua_loop;
    ucpu_t  uc_msk;               //字内偏移量

    pppl_bitmap=Cession->ppp_bitmap; //空闲金字塔位图指针的指针表的首指针
    ppl_bitmap=pppl_bitmap[grade];        //该阶空闲金字塔位图指针表的首址
    pl_classes=Cession->p_classes;   //读取各阶空闲金字塔级数表指针.
    uf_classes=pl_classes[grade]-1;       //该阶空闲金字塔最高一级指针在
                                          //ppl_bitmap中的偏移量
    Cession->free_pages_num -= (1<<grade); //剩余可用页数
    ua_pages_number=0;
    do
    {//本循环查找第一个可分配块(即bit=0)的偏移位数,从高到低查找,方法:
     //首先,找出该阶空闲位图金字塔最高级位图中第一个bit=0的位偏移量.
     //然后,根据这个偏移量计算次高级含0位的字偏移,读出该字并找出bit=0的位偏移.
     //如此反复可以找到最后一级第一个0位的偏移,这就是本次分配的目标块.

        pl_bitmap=ppl_bitmap[uf_classes];   //读取各级位图的指针

        //ua_pages_number是上一级的位偏移,ua_temp1是本级最后一字的字内位偏移
        ua_temp1=CN_CPU_BITS-1-__M_LeadingUcpuZero(~pl_bitmap[ua_pages_number]);

        //上一级的字偏移×字长+本级字内位偏移得到本级总位偏移
        ua_pages_number =(ua_pages_number<<CN_CPU_BITS_SUFFIX_ZERO)+ua_temp1;
    }while(uf_classes-- >0);

    ua_pages_number <<= grade;  //目标块的首页的页号

    //下面开始在bitmap表标记本次分配情况
    for(uf_grade_th=0;uf_grade_th<=grade;uf_grade_th++)
    {//从0阶直到当前阶(不包含),处理阶号低于当次申请的内存的位图.
        //块的起始页号右移阶号即得到在该阶0级位图中的偏移
        ua_word_offset = ua_pages_number>>uf_grade_th;
        ppl_bitmap=pppl_bitmap[uf_grade_th];    //取得该阶分配表指针表的首址
        ua_bit_num=1<<(grade-uf_grade_th);      //计算掩码位数
        for(ua_temp1=0;ua_temp1<pl_classes[uf_grade_th];ua_temp1++)
        {//从低级到高级遍历该阶各级位图
            pl_bitmap=ppl_bitmap[ua_temp1]; //相应级相应路径分配表首址
            //计算字内位偏移,下面算式起取模的作用
            u32WordShift=ua_word_offset&cn_low_xbit_msk[CN_CPU_BITS_SUFFIX_ZERO];
            ua_word_offset=ua_word_offset>>CN_CPU_BITS_SUFFIX_ZERO;  //字偏移
            if(ua_bit_num>=CN_CPU_BITS)   //ua_bit_num保存的是需要置1的位
            {//位数大于或等于1个字宽,因为采用2的n次方分阶,所以要处理的位数肯定
             //是整数个字,按整数个字置1就可以了.

                //计算字数,也是下一级位图中需置1的位数
                ua_bit_num >>=CN_CPU_BITS_SUFFIX_ZERO;
                for(ua_loop =0; ua_loop < ua_bit_num; ua_loop++)
                    pl_bitmap[ua_word_offset+ua_loop]=CN_ALLBIT_1;
            }else if(ua_bit_num>0)
            //位数小于一个字宽,且大于0
            {
                uc_msk=cn_low_xbit_msk[ua_bit_num]<<u32WordShift;
                pl_bitmap[ua_word_offset] |=uc_msk;
                if(pl_bitmap[ua_word_offset] == CN_ALLBIT_1)
                //被置1的位所在的字变为全1时，该字对应的下一级位图相应的位需置1
                    ua_bit_num=0;
                else
                    break;
            }else
            {//前一次操作使一个字从非全1变成全1，置位该字对应的下一级路径相应位
                pl_bitmap[ua_word_offset] |= 1<<u32WordShift;
                if(pl_bitmap[ua_word_offset] == CN_ALLBIT_1)
                {
                //被置1的位所在的字变为全1时，该字对应的下一级位图相应的位需置1
                }
                else
                    break;
            }
        }
    }

    for(; uf_grade_th < Cession->uf_grades; uf_grade_th++)
    {//当前阶(包含)到最高阶,处理阶号高于或等于当次申请的内存的位图.每次只需1位置1

        //块的起始页号右移阶号即得到在该阶0级位图中的偏移
        ua_word_offset = ua_pages_number >> uf_grade_th;
        if(ua_word_offset >= Cession->ua_pages_num>>uf_grade_th)
        //该阶无对应块,这是可能的,比如一个共10页的内存,第9页和第10页都在第3阶中没有
        //映射.条件式右边是本阶总块数,左端是从0起计的偏移量,最大只能是总块数-1
            break;
        ppl_bitmap=pppl_bitmap[uf_grade_th]; //取得该阶分配表指针表的首址

        for(ua_temp1=0;ua_temp1<pl_classes[uf_grade_th];ua_temp1++)
        {
            pl_bitmap = ppl_bitmap[ua_temp1];
            //计算字内位偏移,下面算式起取模的作用
            u32WordShift=ua_word_offset&cn_low_xbit_msk[CN_CPU_BITS_SUFFIX_ZERO];
            ua_word_offset=ua_word_offset>>CN_CPU_BITS_SUFFIX_ZERO;  //字偏移
            pl_bitmap[ua_word_offset] |= 1<<u32WordShift;
            if(pl_bitmap[ua_word_offset] == CN_ALLBIT_1)
            {
            //被掩模的位所在的字变为全1时，该字对应的下一级位图相应的位需置1
            }
            else
                break;
        }
    }

    //刷新最大空闲内存块
    if(Cession->free_pages_num != 0)
    {
        uf_grade_th=Cession->uf_grades-1;     //最高阶号
        do{
            uf_classes= pl_classes[uf_grade_th];        //第uf_grade_th阶总级数
            ppl_bitmap   = pppl_bitmap[uf_grade_th];    //位图索引表指针
            pl_bitmap    = ppl_bitmap[uf_classes-1];    //最高级位图指针
            if(*pl_bitmap != (~0))
            {//路径顶端只要不是全1就表示该阶肯定有空闲块.
                //根据阶号计算最大空闲块的尺寸.
                Cession->ua_free_block_max=Cession->PageSize<<uf_grade_th;
                break;
            }
        }while(uf_grade_th-- >0);   //从最高阶(最大块)扫描到0阶
    }
    else
        Cession->ua_free_block_max = 0;
    return Cession->heap_bottom
                    +(ua_pages_number * Cession->PageSize);
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
void *__M_Realloc(void *p, ptu32_t NewSize,u32 Timeout)
{
    ptu32_t OldSize;
    void *NewP = NULL;

    if(NewSize == 0)
    {
        __M_Free(p);
        return NULL;
    }

    if(p == NULL)
    {
        //若NewSize = 0,返回NULL
        NewP = __M_Malloc(NewSize,CN_TIMEOUT_FOREVER);
        return NewP;
    }

    OldSize = __M_CheckSize(p);
    if((NewSize > (OldSize>>1)) && (NewSize <= OldSize))
        return p;
    else
    {
        NewP = __M_Malloc(NewSize,CN_TIMEOUT_FOREVER);
        if(NewP)
        {
            memcpy(NewP,p,NewSize < OldSize ? NewSize:OldSize);
        }
        __M_Free(p);
    }
    return NewP;
}

//----获取格式化尺寸-----------------------------------------------------------
//功能: 虽然应用程序允许申请任意尺寸内存，但内存管理器分配给用户的内存却是满足
//      用户需求的规格化尺寸的内存块，本函数告诉你，如果你申请分配size字节内存，
//      系统将分配给你的内存块的实际尺寸，本函数不执行分配内存的操作，也不保证随
//      后的分配操作能否成功。
//参数: size，应用程序欲分配的尺寸
//      Heap, 目标堆,若该堆是通用堆,则优先检查该堆
//返回: 规格化尺寸
//------------
//更新记录:
// 1.日期: 2015/3/16
//   说明: 增加对参数size的零值判断
//   作者: 季兆林
//-----------------------------------------------------------------------------
ptu32_t __M_FormatSizeHeapIn(ptu32_t size,struct HeapCB *Heap)
{
    ucpu_t  grade;
    struct HeapCession *Cession;
    ptu32_t tsize,fsize = CN_LIMIT_PTU32;
    u32 pages;

    Cession = Heap->Cession;
    while(Cession != NULL)
    {
        pages = (size + Cession->PageSize -1)/Cession->PageSize;
        grade = __M_LeadingZero((u8 *)&pages,sizeof(u32));
        if((CN_HIGH_1BIT_1>>grade) != pages)
        //如果grade不是规格化的数,调整到比它大的最小允许块大小
        //否则维持size的大小,不做调整
            grade--;
        grade = CN_PTR_BITS-1-grade;
        tsize = Cession->PageSize << grade;
        if( (tsize < fsize) && (tsize <= Cession->ua_block_max) )
        {
            fsize = tsize;
        }
        Cession = Cession->Next;
    }
    return fsize;
}
ptu32_t __M_FormatSizeHeap(ptu32_t size,struct HeapCB *Heap)
{
    struct HeapCB *Work;
    ptu32_t result;

    if((0 == size) || (Heap == NULL))
        return (0);
    if((Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL)
    {
        Work = Heap;
        do
        {
            if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
            {
                result = __M_FormatSizeHeapIn(size,Work);
                if(result != 0)
                    break;
            }
            Work = Work->NextHeap;
        }while(Work != Heap);

    }
    else
    {
        result = __M_FormatSizeHeapIn(size,Heap);
    }
    return result;
}
ptu32_t __M_FormatSize(ptu32_t size)
{
    if(tg_pSysHeap != NULL)
        return __M_FormatSizeHeapIn(size,tg_pSysHeap);
    else
        return 0;
}

//----释放从堆分配的内存-------------------------------------------------------
//功能: 释放从堆中分配的一块内存，如果此时动态分配还没有初始化，则执行准静态
//      内存释放。释放内存后，重新计算最大空闲块的尺寸，如果有事件在内存同步
//      队列中，则把等待的内存尺寸小于新的最大空闲块尺寸的事件激活。
//参数：pl_mem,待释放的内存指针
//      Heap,待释放的内存所在的堆。如果是私有堆,则直接使用.
//           如果是通用堆,则从这个堆开始在所有通用堆中查找pl_mem所在的堆
//返回：错误返回flase,正确时返回true
//-----------------------------------------------------------------------------
void __M_FreeHeap(void * pl_mem,struct HeapCB *Heap)
{
    struct HeapCession *Cession;
    struct HeapCB *CurHeap;
    struct EventECB **MemSyncHead;
    ptu32_t ua_temp1;
    ptu32_t ua_word_offset;
    u32 u32WordShift;          //字内偏移量
    ptu32_t ua_pages_no;
    ufast_t uf_grade_th,uf_free_grade_th;
    ufast_t uf_classes;
    ufast_t *pl_classes;
    ucpu_t  ***pppl_bitmap;
    ucpu_t  **ppl_bitmap;
    ucpu_t  *pl_bitmap;
    uint16_t *pl_id;                //id;
    struct EventECB  *event;
    ptu32_t ua_bit_num;
    ucpu_t  uc_msk;

    u8    event_mem_stage = 0;

    if( (pl_mem == NULL) || (Heap == NULL) )
        return;

    if( (Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_PRIVATE)
    {
        CurHeap = Heap;
        MemSyncHead = &Heap->mem_sync;      //使用专用堆的同步指针
    }
    else
    {
        MemSyncHead = &s_ptMemSync;         //使用通用堆的同步指针
        CurHeap = Heap;
        do
        {
            if((CurHeap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
            {
                Cession = CurHeap->Cession;
                //以下循环找出待释放的内存在哪个Cession中
                while(Cession != NULL)
                {
                    if(((u8*)pl_mem < Cession->heap_top) && ((u8*)pl_mem >= Cession->heap_bottom) )
                        break ;
                    Cession = Cession->Next;
                }
            }
            if(Cession == NULL)
                CurHeap = CurHeap->NextHeap;
            else
                break;
        }while(CurHeap != Heap);
    }


    if(Cession == NULL)
    {
        Djy_SaveLastError(EN_MEM_ERROR);  //欲释放的内存不在内存堆中,直接退出.
        return;
    }

    //计算释放的内存块的首页页号
    ua_pages_no=(ptu32_t)((ptu32_t)pl_mem-(ptu32_t)Cession->heap_bottom)
                                            / Cession->PageSize;

    Lock_MutexPend(&(CurHeap->HeapMutex),CN_TIMEOUT_FOREVER);
    //查找释放的内存块的阶号,从0起计.通过阶号也可以确定内存块的大小.
    //确定内存块的类型,局部内存需要知道拥有该内存的事件id，
    //全局内存无需清理内存分配跟踪表,无需知道拥有该内存的事件id
    pl_id = &Cession->index_event_id[ua_pages_no];
    switch(pl_id[0])
    {
        case CN_MEM_DOUBLE_PAGE_LOCAL :
        {   //双页局部内存,-1+id
            pl_id[1] = CN_MEM_FREE_PAGE;
            uf_free_grade_th = 1;
        }break;
        case CN_MEM_MANY_PAGE_LOCAL :
        {   //多页局部内存:-2+id+阶号
            uf_free_grade_th = (ufast_t)pl_id[2];
            pl_id[1] = CN_MEM_FREE_PAGE;
            pl_id[2] = CN_MEM_FREE_PAGE;
        }break;
        case CN_MEM_SINGLE_PAGE_GLOBAL :
        {   //单页全局内存:-3
            uf_free_grade_th = 0;
        }break;
        case CN_MEM_MANY_PAGE_GLOBAL :
        {   //双(多)页全局内存:-4+阶号.
            uf_free_grade_th = (ufast_t)pl_id[1];
            pl_id[1] = CN_MEM_FREE_PAGE;
        }break;
        default :
        {
            if(pl_id[0] <= CN_EVENT_ID_LIMIT)
            {   //单页局部内存:id,
//                id = pl_id[0];
                pl_id[0] = CN_MEM_FREE_PAGE;
                uf_free_grade_th = 0;
            }else
            {
                Djy_SaveLastError(EN_MEM_ERROR);   //指针有错,直接退出
                Lock_MutexPost(&(CurHeap->HeapMutex) );
                pl_id[0] = CN_MEM_FREE_PAGE;
                return;

            }
        }break;
    }
    pl_id[0] = CN_MEM_FREE_PAGE;
    pppl_bitmap=Cession->ppp_bitmap; //空闲金字塔位图指针的指针表的首指针
//    ppl_bitmap=pppl_bitmap[uf_free_grade_th];//该阶空闲金字塔位图指针表的首址
//    pl_bitmap=ppl_bitmap[0];                 //空闲金字塔位图指针
    pl_classes=Cession->p_classes;    //读取各阶空闲金字塔级数表指针.
    uf_classes=pl_classes[uf_free_grade_th]-1;  //该阶空闲金字塔最高一级指针在
                                                //ppl_bitmap中的偏移量

    //计算位偏移
//    ua_word_offset =ua_pages_no>>uf_free_grade_th;
    //计算字内位偏移,下面算式起取模的作用
//    u32WordShift=ua_word_offset&cn_low_xbit_msk[CN_CPU_BITS_SUFFIX_ZERO];
//    ua_word_offset=ua_word_offset>>CN_CPU_BITS_SUFFIX_ZERO;  //计算字偏移

    //空闲页数增加
    Cession->free_pages_num += 1<<uf_free_grade_th;

    //下面开始在bitmap表标记本次释放情况
    for(uf_grade_th = 0; uf_grade_th <= uf_free_grade_th; uf_grade_th++)
    {//从0阶直到当前阶(不包含),处理阶号低于当次申请的内存的位图.
        //块的起始页号右移阶号即得到在该阶0级位图中的偏移
        ua_word_offset =ua_pages_no>>uf_grade_th;
        ppl_bitmap=pppl_bitmap[uf_grade_th];        //取得该级分配表指针表的首址
        ua_bit_num=1<<(uf_free_grade_th-uf_grade_th);       //计算掩码位数
        for(ua_temp1=0;ua_temp1<pl_classes[uf_grade_th];ua_temp1++)
        {//从低级到高级遍历该阶各级位图
            pl_bitmap = ppl_bitmap[ua_temp1]; //相应级相应路径分配表首址
            //计算字内位偏移,下面算式起取模的作用
            u32WordShift=ua_word_offset&cn_low_xbit_msk[CN_CPU_BITS_SUFFIX_ZERO];
            ua_word_offset = ua_word_offset>>CN_CPU_BITS_SUFFIX_ZERO;  //字偏移
            if(ua_bit_num>=CN_CPU_BITS)   //ua_bit_num保存的是需要清0的位
            {//位数大于或等于1个字宽,因为采用2的n次方分级,所以要处理的位数肯定
             //是整数个字,按整数个字清0就可以了.

                //计算字数,也是下一级位图中需清0的位数
                ua_bit_num >>=CN_CPU_BITS_SUFFIX_ZERO;
                //uc_word_shift在这里当临时变量使用
                for(u32WordShift =0; u32WordShift < ua_bit_num; u32WordShift++)
                    pl_bitmap[ua_word_offset+u32WordShift]=0;
            }else if(ua_bit_num > 0)
            {//位数小于一个字宽,且大于0
                uc_msk=~(cn_low_xbit_msk[ua_bit_num]<<u32WordShift);
                pl_bitmap[ua_word_offset] &=uc_msk;
                if(pl_bitmap[ua_word_offset] == uc_msk)
                //被清0的位所在的字全1全1->有0，该字对应的下一级位图相应位清0
                    ua_bit_num=0;
                else
                    break;
            }else
            {//前一次操作使一个字从全1->有0，该字对应的下一级路径相应位清0
                pl_bitmap[ua_word_offset] &= ~(1<<u32WordShift);
                if(pl_bitmap[ua_word_offset] == ~(1<<u32WordShift))
                {
                //被清0的位所在的字全1->有0，该字对应的下一级位图相应位清0
                }
                else
                    break;
            }
        }
    }

    for(;uf_grade_th<Cession->uf_grades;uf_grade_th++)
    {//当前阶(包含)到最高阶,处理阶号高于或等于当次申请的内存的位图.每次只需清1位
        //取低一阶对应本阶两块中较低地址的偏移量
        ua_word_offset = (ua_pages_no >> uf_grade_th)<<1;
        //计算字内位偏移,下面算式起取模的作用
        u32WordShift=ua_word_offset&cn_low_xbit_msk[CN_CPU_BITS_SUFFIX_ZERO];
        ua_word_offset=ua_word_offset>>CN_CPU_BITS_SUFFIX_ZERO;  //字偏移
        ppl_bitmap=pppl_bitmap[uf_grade_th-1]; //取得该级分配表指针表的首址
        pl_bitmap = ppl_bitmap[0];
        if( (pl_bitmap[ua_word_offset] & (3<<u32WordShift)))
            break;      //本阶位图无须清零，更高的阶也无须清零

        //块的起始页号右移阶号即得到在该阶0级位图中的偏移
        ua_word_offset=ua_pages_no>>uf_grade_th;
        if(ua_word_offset>=Cession->ua_pages_num>>uf_grade_th)
        //该阶无对应块,这是可能的,比如一个共10页的内存,第9页和第10页都在第3阶中
        //无映射.条件式右边是本阶总块数,左端是从0起计的偏移量,最大只能是总块数-1
            break;
        ppl_bitmap=pppl_bitmap[uf_grade_th]; //取得该级分配表指针表的首址

        for(ua_temp1=0;ua_temp1<pl_classes[uf_grade_th];ua_temp1++)
        {
            pl_bitmap = ppl_bitmap[ua_temp1];
            //计算字内位偏移,下面算式起取模的作用
            u32WordShift=ua_word_offset&cn_low_xbit_msk[CN_CPU_BITS_SUFFIX_ZERO];
            ua_word_offset=ua_word_offset>>CN_CPU_BITS_SUFFIX_ZERO;  //字偏移
            pl_bitmap[ua_word_offset] &= ~(1<<u32WordShift);
            if(pl_bitmap[ua_word_offset] == ~(1<<u32WordShift))
            {
            //被清0的位所在的字全1->有0，该字对应的下一级位图相应位清0
            }
            else
                break;
        }
    }

    //刷新最大空闲内存块
    uf_grade_th=Cession->uf_grades-1;     //取最高阶的阶号
    do{
        uf_classes   = pl_classes[uf_grade_th];     //第uf_grade_th阶的总级数
        ppl_bitmap   = pppl_bitmap[uf_grade_th];    //位图索引表指针
        pl_bitmap    = ppl_bitmap[uf_classes-1];    //最高级位图指针
        if(*pl_bitmap != (~0))
        {//路径顶端只要不是全1就表示该阶肯定有空闲块.
            //根据阶号计算最大空闲块的尺寸.
            Cession->ua_free_block_max=Cession->PageSize<<uf_grade_th;
            break;
        }
    }while(uf_grade_th-- >0);   //从最高阶(最大块)扫描到0阶

    Lock_MutexPost( &(CurHeap->HeapMutex) );
    //we don't konw if it is local memory, so we could not sub it directly ,
    //the local_memory must be used with the __M_MallocLc, so it should sub in
    //the matched __M_FreeLc, but up tills now, we have no __M_FreeLc--TODO
    //suggest we should branch it in the free function or new a function name
    //M_FreeLc
    if(g_ptEventRunning->local_memory > 0)
    {
        g_ptEventRunning->local_memory--;
    }
    //把内存等待队列中申请内存之和小于当前可用最大内存的几个事件放到ready队列
    //等待队列是双向循环链表
    Int_SaveAsynSignal();
    if(*MemSyncHead != NULL)
    {
        event = *MemSyncHead;   //取等待队列头
        ua_temp1 = Cession->ua_free_block_max;
        while(1)
        {
            //仔细看这里并没有更新event变量，会因为event移除导致下列代码出错、
            //因此应该至少有下面一句：
            event = *MemSyncHead;   //取等待队列头
            //ua_word_offset在此权当临时变量使用,规格化当前事件需要的内存.
            event_mem_stage = __M_GetGrade(event->wait_mem_size,Cession);
            ua_word_offset = Cession->PageSize<<(event_mem_stage);
            if(ua_word_offset <= ua_temp1)
            {//事件等待的内存量小于最大空闲内存块,把事件放到就绪队列
                //剩余可用尺寸减少当前激活的事件所申请的内存,但并不实际执行申请
                ua_temp1 -= ua_word_offset;
                event->sync_head = NULL;
                if(event->event_status & CN_STS_SYNC_TIMEOUT)
                    __Djy_ResumeDelay(event);
                event->event_status = CN_STS_EVENT_READY;
                event->wakeup_from = CN_STS_WAIT_MEMORY;
//                if(event->multi_next == event->multi_previous)
//                {//是最后一个事件
//                    tg_MCB.mem_sync  = NULL;
//                    break;
//                }else   //bug
                if(event->multi_next == event)
                {//是最后一个事件
                    *MemSyncHead  = NULL;
                    break;
                }else

                {
                    *MemSyncHead = event->multi_next;
                    event->multi_next->multi_previous = event->multi_previous;
                    event->multi_previous->multi_next = event->multi_next;
                }
                //看看该    ECB是不是有VM，如果没有则是因为创建VM不得，才放在此，
                //因此需要从maked队列中移除。
                if(NULL == event->vm)//创建VM不得，从marked队列删除
                {
                    __Djy_CutEventFromEvttMarked(event);
                }
                else//这种是在运行时申请内存而不得导致的
                {
                    //目前而言什么都不用做
                }
                __Djy_EventReady(event);
            }else
                break;
        }
    }
    Int_RestoreAsynSignal();  //恢复中断状态
    return;
}

void __M_Free(void * pl_mem)
{
    if( tg_pSysHeap == NULL)
        return;
    __M_FreeHeap(pl_mem,tg_pSysHeap);
    return;
}

ptu32_t __M_GetMaxFreeBlockHeapIn(struct HeapCB *Heap)
{
    struct HeapCession *Cession;
    ptu32_t result = 0;
    Cession = Heap->Cession;
    while(Cession != NULL)
    {
        if(result < Cession->ua_free_block_max)
            result = Cession->ua_free_block_max ;
        Cession = Cession->Next;
    }
    return result;
}
//----查询最大可用内存块-------------------------------------------------------
//功能：返回最大可用内存块的尺寸.
//参数：Heap,被查询的堆,如果这是个专用堆,则只查询这个堆,否则查询所有通用堆.
//返回：内存块字节数
//-----------------------------------------------------------------------------
ptu32_t __M_GetMaxFreeBlockHeap(struct HeapCB *Heap)
{
    struct HeapCB *Work;
    ptu32_t result = 0,temp;
    if(Heap== NULL)
        return 0;
    if((Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_PRIVATE)
    {
        result = __M_GetMaxFreeBlockHeapIn(Heap);
    }
    else
    {
        Work = Heap;
        do
        {
            if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
            {
                temp = __M_GetMaxFreeBlockHeapIn(Work);
                if(result < temp)
                    result = temp;
            }
            Work = Work->NextHeap;
        }while(Work != Heap);
    }

    return result;
}


//----查询最大可用内存块-------------------------------------------------------
//功能：返回通用堆的最大可用内存块的尺寸.
//参数：无
//返回：内存块字节数
//-----------------------------------------------------------------------------
ptu32_t  __M_GetMaxFreeBlock(void)
{
    struct HeapCB *Work;
    ptu32_t result = 0,temp;
    Work = tg_pHeapList;
    do
    {
        if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
        {
            temp = __M_GetMaxFreeBlockHeapIn(Work);
            if(result < temp)
                result = temp;
        }
        Work = Work->NextHeap;
    }while(Work != tg_pHeapList);

    return result;
}

ptu32_t  __M_GetFreeMemHeapIn(struct HeapCB *Heap)
{
    struct HeapCession *Cession;
    ptu32_t result=0;
    Cession = Heap->Cession;
    while(Cession != NULL)
    {
        result += Cession->free_pages_num * Cession->PageSize;
        Cession = Cession->Next;
    }
    return result;
}

//----查询空闲内存大小-------------------------------------------------------
//功能：返回所指定堆的空闲内存字节数
//参数：Heap,被查询的堆,如果这是个专用堆,则只查询这个堆,否则查询所有通用堆.
//返回：空闲内存字节数
//-----------------------------------------------------------------------------
ptu32_t  __M_GetFreeMemHeap(struct HeapCB *Heap)
{
    struct HeapCB *Work;
    ptu32_t result = 0;
    if(Heap== NULL)
        return 0;
    if((Heap->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_PRIVATE)
    {
        result = __M_GetMaxFreeBlockHeapIn(Heap);
    }
    else
    {
        Work = Heap;
        do
        {
            if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
            {
                result += __M_GetFreeMemHeapIn(Heap);
            }
            Work = Work->NextHeap;
        }while(Work != Heap);
    }

    return result;
}

//----查询总空闲内存大小-------------------------------------------------------
//功能：返回通用堆的空闲内存字节数
//参数：无
//返回：空闲内存字节数
//-----------------------------------------------------------------------------
ptu32_t  __M_GetFreeMem(void)
{
    struct HeapCB *Work;
    ptu32_t result = 0;
    Work = tg_pHeapList;
    do
    {
        if((Work->HeapProperty & CN_HEAP_PRIVATE) == CN_HEAP_GENERAL )
        {
            result += __M_GetFreeMemHeapIn(Work);
        }
        Work = Work->NextHeap;
    }while(Work != tg_pHeapList);

    return result;
}
