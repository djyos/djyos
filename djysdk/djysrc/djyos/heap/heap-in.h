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
//版本：V1.0.0
//文件描述:提供块相联内存分配策略
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __HEAP_IN_H__
#define __HEAP_IN_H__

#include "stdint.h"
#include "board-config.h"
#include "lock.h"
#ifdef __cplusplus
extern "C" {
#endif

struct HeapCession
{
    struct HeapCession *Next;
    u8   *static_bottom;  //堆底指针，从准静态分配算起
    u8   *heap_bottom;    //堆底指针，从块相联动态分配算起
    u8   *heap_top;       //堆顶指针
    list_t   *last;       //静态分配最后一块的链表地址指针。
#if ((CN_CFG_DYNAMIC_MEM == 1))
    u32    PageSize;
    u32    ua_pages_num;    //总页数
    u32    free_pages_num;  //空闲内存页数
    u32    ua_block_max;    //最大内存块尺寸(字节数)
    u32    ua_free_block_max; //空闲的最大内存块大小(字节数);
    ufast_t     uf_grades;      //总阶数

    //以下3部分在初始化时从堆中划出。
    //第一部分，每页分配一个16位字记录拥有本页内存的事件的ID。不允许跨事件分配和
    //释放局部内存，这是一个容易造成混乱的操作。
    //本表还用于标示该次分配的页数,free据此查询待释放的字节数.
    //如果djy_event_completed函数发现事件仍有未释放的局部内存块，依据此表强制释
    //放之，格式如下:
    //1.单页局部内存:事件id,
    //2.双页局部内存:0xffff+事件id
    //3.多页局部内存:0xfffe+事件id+阶号
    //4.单页全局内存:0xfffd,全局内存没有所属事件id,只需要知道内存大小
    //5.双(多)页全局内存:0xfffc+阶号.
    //6.空闲内存:0xfffb
    u16    *index_event_id;
    ufast_t     *p_classes;     //第二部分,各阶块空闲金字塔级数表
    ucpu_t      ***ppp_bitmap;  //第三部分,指向"空闲金字塔位图指针的指针",
                                //每级一个,指向该阶空闲金字塔的第一个指针
#endif   //for #if ((CN_CFG_DYNAMIC_MEM == 1))
};

struct HeapCB
{
    struct HeapCB *PreHeap;
    struct HeapCB *NextHeap;
    struct HeapCession *Cession;
    u32  CessionNum;            //组成该heap的session数量。
    u32  AlignSize;             //该heap所要求的对齐尺寸，有些堆用于dma或特殊用
                                //途，在该heap上分配的内存，有特殊的对齐尺寸。
    u32  HeapProperty;          //0=通用堆,1=专用堆
    char *HeapName;
    struct MutexLCB HeapMutex;
#if ((CN_CFG_DYNAMIC_MEM == 1))
    // 等待分配内存的事件链表,双向循环链表。
    // 无论是否支持动态分配,在准静态分配期间,指针无效.
    // 仅专用堆的指针有效,通用堆将用静态变量s_ptMemSync替代.
    struct EventECB *mem_sync;
#endif   //for #if ((CN_CFG_DYNAMIC_MEM == 1))
} ;

//用于把事件申请的局部内存块串起来，调用y_event_done函数时，据此回收内存。
struct MemRecord
{
    struct MemRecord   *next;
    struct EventECB *owner;
    u8 *address;
};


#ifdef __cplusplus
}
#endif

#endif //__HEAP_IN_H__


