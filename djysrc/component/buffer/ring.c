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
//所属模块: 缓冲区
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 提供环形缓冲区服务
//其他说明:
//修订历史:
//2. 日期: 2010-02-10
//   作者:  罗侍田.
//   新版本号: V1.1.0
//   修改说明: 对struct ring_buf的len成员实施原子保护，使之支持读写并发.如果缓冲
//      区的读和写均只有一个线程(可以是不同线程),则可以安全地操作。如果有超过1个
//      线程读或写，则还需要做信号量保护以确保并发访问安全。
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "int.h"
#include "ring.h"
#include <string.h>

//----建立环形缓冲区----------------------------------------------------------
//功能: 建立环形缓冲区并初始化，使用这个函数之前，用户应该定义缓冲区内存块和
//      缓冲区数据结构。
//参数: ring,目标环形缓冲区结构指针
//      buf,缓冲区起始地址
//      len,缓冲区长度.单位是字节数
//返回: 无
//-----------------------------------------------------------------------------
void Ring_Init(struct tagRingBuf *ring, u8 *buf, u32 len)
{
    ring->buf = buf;
    ring->max_len = len;
    ring->offset_write = 0;
    ring->offset_read  = 0;
    ring->len = 0;
}

//----检查缓冲区容量-----------------------------------------------------------
//功能: 返回缓冲区容量
//参数: ring,目标线性缓冲区结构指针
//返回: 缓冲区容量，就是调用ring_init时使用的len参数。
//-----------------------------------------------------------------------------
u32 Ring_Capacity(struct tagRingBuf *ring)
{
    return ring->max_len;
}

//----返回字节池地址-----------------------------------------------------------
//功能: 查询缓冲区的字节池地址，这个地址是用户调用ring_init时使用的缓冲区地址。
//参数: ring,目标线性缓冲区指针.
//返回: 环形缓冲区的字节池地址
//说明: 有两种情况会产生获得缓冲区地址的需求
//      1、要释放缓冲区内存，应用程序可以使用调用ring_init时使用的指针，如果该
//         指针丢失，可用本函数取回该指针。
//      2、用户想自己动手访问该缓冲区，这不是一个明智的选择，有破坏模块独立性
//         的嫌疑，这时候，使用者应该完全明白自己在干什么!
//-----------------------------------------------------------------------------
u8 *Ring_GetBuf(struct tagRingBuf *ring)
{
    return ring->buf;
}

//----环形缓冲区写入-----------------------------------------------------------
//功能: 环形缓冲区写入若干个字节,返回实际写入的数据量,并移动写指针,如果环形
//      缓冲区没有足够的空间,按实际剩余空间写入
//参数: ring,目标环形缓冲区结构指针
//      buffer,待写入的数据指针
//      len,待写入的数据长度.单位是字节数
//返回: 实际写入的字节数,如果缓冲区有足够的空间,=len
//-----------------------------------------------------------------------------
u32 Ring_Write(struct tagRingBuf *ring,u8 *buffer,u32 len)
{
    u32    wr_len;
    u32    partial;
    atom_low_t  atom_bak;
    wr_len = ring->max_len - ring->len;
    if(wr_len == 0)
        return 0;
    if(wr_len > len)
        wr_len = len;
    if((ring->offset_write + wr_len) > ring->max_len)
    {   //数据发生环绕
        partial = ring->max_len - ring->offset_write;
        memcpy(&ring->buf[ring->offset_write],buffer,partial);    //写第一部分
        memcpy( ring->buf,&buffer[partial],wr_len - partial);    //写第二部分
        ring->offset_write = wr_len - partial;
    }else
    {   //不发生环绕
        memcpy( &ring->buf[ring->offset_write],buffer,wr_len);
        ring->offset_write +=wr_len;
    }
    atom_bak = Int_LowAtomStart();
    ring->len += wr_len;
    Int_LowAtomEnd(atom_bak);
    return wr_len;
}

//----从环形缓冲区读-----------------------------------------------------------
//功能: 从环形缓冲区读出若干个字节,返回实际读出的数据量,并且移动读指针如果
//      缓冲区内数据不足，按实际数量读取。
//参数: ring,目标环形缓冲区结构指针
//      buffer,接收数据的缓冲区指针
//      len,待读出的数据长度.单位是字节数
//返回: 实际读出的字节数,如果缓冲区有足够的数据,=len
//------------------------------------------------------------------------------
u32    Ring_Read(struct tagRingBuf *ring,u8 *buffer,u32 len)
{
    u32    wr_len,ring_len;
    atom_low_t  atom_bak;
    ring_len = ring->len;
    wr_len = (ring_len < len)? ring_len : len;
    if((ring->offset_read + wr_len) > ring->max_len)
    {   //数据发生环绕
        u32  partial;
        partial =ring->max_len - ring->offset_read;
        memcpy( buffer,&ring->buf[ring->offset_read],partial);    //写第一部分
        memcpy( &buffer[partial],ring->buf,wr_len - partial);    //写第二部分
        ring->offset_read = wr_len - partial;
    }else
    {   //不发生环绕
        memcpy( buffer,&ring->buf[ring->offset_read],wr_len);
        ring->offset_read += wr_len;
    }
    atom_bak = Int_LowAtomStart();
    ring->len -= wr_len;
    Int_LowAtomEnd(atom_bak);
    return wr_len;
}

//----检查缓冲区数据量----------------------------------------------------------
//功能: 检查指定的环形缓冲区中的数据量,返回字节数.
//参数: ring,目标环形缓冲区指针.
//返回: 缓冲区中的数据量
//------------------------------------------------------------------------------
u32    Ring_Check(struct tagRingBuf *ring)
{
    return ring->len;       //len是ucpu_t类型的，可以确保读操作的原子性
}

//----检查缓冲区是否空-----------------------------------------------------------
//功能: 检查指定的环形缓冲区中是否已经空.
//参数: ring,目标环形缓冲区指针.
//返回: 空则返回true,非空返回false
//------------------------------------------------------------------------------
bool_t   Ring_IsEmpty(struct tagRingBuf *ring)
{
    return (ring->len ==0)? true:false;
}

//----检查缓冲区是否满-----------------------------------------------------------
//功能: 检查指定的环形缓冲区中是否已经满.
//参数: ring,目标环形缓冲区指针.
//返回: 满则返回true,非满返回false
//------------------------------------------------------------------------------
bool_t   Ring_IsFull(struct tagRingBuf *ring)
{
    return (ring->len == ring->max_len)? true:false;
}

//----清空环形缓冲区-------------------------------------------------------------
//功能: 清除缓冲区中所有数据
//参数: ring,目标环形缓冲区指针.
//返回: 无
//特别注意: 调用前，请确认没有其他线程在使用缓冲区，否则可能出现不可预料的结果
//------------------------------------------------------------------------------
void    Ring_Flush(struct tagRingBuf *ring)
{
    ring->len = 0;
    ring->offset_write = 0;
    ring->offset_read = 0;
}

//----释放若干数据---------------------------------------------------------------
//功能: 从读指针开始,释放掉指定大小的数据,相当于哑读了len个字节
//参数: ring,目标环形缓冲区指针.
//      len,释放的数据数量
//返回: 实际释放的数据量
//------------------------------------------------------------------------------
u32 Ring_DumbRead(struct tagRingBuf *ring,u32 len)
{
    u32    result,ring_len;
    atom_low_t  atom_bak;
    ring_len = ring->len;
    result = (ring_len < len)? ring_len : len;
    if((ring->offset_read + result) > ring->max_len)
    {   //数据发生环绕
        ring->offset_read = result + ring->offset_read - ring->max_len;
    }else
    {   //不发生环绕
        ring->offset_read += result;
    }
    atom_bak = Int_LowAtomStart();
    ring->len -= result;
    Int_LowAtomEnd(atom_bak);
    return result;
}

//----退回若干数据-------------------------------------------------------------
//功能: 本函数与ring_dumb_read函数正好相反，把缓冲区指针退回len字节，如果退回的
//      长度超过缓冲区的空闲长度，则取缓冲区空闲长度。相当于把缓冲区中已经读出
//      的数据返回缓冲区，好像没有读过的样子。ring模块并不校验退回的部分是否包含
//      原来的数据。
//参数: ring,目标环形缓冲区指针.
//      len,退回的数据数量
//返回: 实际退回的数据量
//-----------------------------------------------------------------------------
u32 Ring_RecedeRead(struct tagRingBuf *ring,u32 len)
{
    u32    result;
    atom_low_t  atom_bak;

    atom_bak = Int_LowAtomStart();
    if((ring->max_len - ring->len) > len)   //空闲长度大于欲退回的长度
        result = len;
    else
        result = ring->max_len - ring->len;
    if(ring->offset_read < result)
    {   //数据发生环绕
        ring->offset_read = ring->max_len - (result - ring->offset_read);
    }else
    {   //不发生环绕
        ring->offset_read -= result;
    }
    ring->len += result;
    Int_LowAtomEnd(atom_bak);
    return result;
}

//----取消若干数据-------------------------------------------------------------
//功能: 取消已经写入线性缓冲区的若干数据，就像从来没有写入一样。
//参数: ring,目标环形缓冲区指针.
//      len,退回的数据数量
//返回: 实际退回的数据量
//-----------------------------------------------------------------------------
u32 Ring_SkipTail(struct tagRingBuf *ring,u32 size)
{
    u32 result;
    atom_low_t  atom_bak;

    atom_bak = Int_LowAtomStart();
    if(ring->len > size)
    {
        result = size;
        if(ring->offset_write < result)
        {   //数据发生环绕
            ring->offset_write = ring->max_len - (result - ring->offset_write);
        }else
        {   //不发生环绕
            ring->offset_write -= result;
        }
        ring->len -=result;
    }else
    {
        result = ring->len;
        Ring_Flush(ring);
    }
    Int_LowAtomEnd(atom_bak);
    return result;
}

//----查找字符------------------------------------------------------------------
//功能: 从ring当前读位置开始查找字符c的位置
//参数: ring,目标环形缓冲区指针
//      c,需查找的字符
//返回: c出现的位置相对读指针的偏移量,如果没有出现则返回 CN_LIMIT_UINT32
//------------------------------------------------------------------------------
u32 Ring_SearchCh(struct tagRingBuf *ring, char c)
{
    u32    i;
    u8 *buf = ring->buf;
    if(ring->offset_read > ring->offset_write)
    {   //缓冲区有回绕
        for(i=ring->offset_read;i<ring->max_len;i++)
        {
            if(buf[i] == c)
                return (i - ring->offset_read);
        }
        for(i=0;i<ring->offset_write;i++)
            if(buf[i] == c)
                return (i - ring->offset_read + ring->max_len);
    }else
    {   //缓冲区没有回绕
        for(i = ring->offset_read; i < ring->offset_write; i++)
            if(buf[i] == c)
                return (i - ring->offset_read);
    }
    return CN_LIMIT_UINT32;
}

//----查找字符序列--------------------------------------------------------------
//功能: 从ring当前读位置开始查找字符序列的位置,字符序列不以0结束,而是指定长度
//参数: ring,目标环形缓冲区指针
//      string,需查找的字符序列
//      str_len,字符序列长度
//返回: string出现的位置相对offset_read的偏移量,如果没有出现返回 CN_LIMIT_UINT32
//备注: 这个功能可能比较常用,所以在编写时注意了速度优化,但却使代码量大增.
//------------------------------------------------------------------------------
u32 Ring_SearchStr(struct tagRingBuf *ring, char *string,u32 str_len)
{
    u32 i,j;
    bool_t next;
    u8 *buf;
    u32  end,start;

    if(ring->len < str_len)
        return CN_LIMIT_UINT32;
    buf = ring->buf;
    if(ring->offset_read <= ring->offset_write)
    {   //缓冲区没有回绕
        for(i=ring->offset_read;i <= ring->offset_write - str_len;i++)
        {
            next = false;
            for(j=0;j < str_len;j++)
            {
                if(buf[i+j] != string[j])
                    next = true;
            }
            if(next == false)
            {
                return (i - ring->offset_read);
            }
        }
    }else
    {   //缓冲区有回绕
        //先处理不环绕部分,不包括缓冲区首末结合部分
        for(i=ring->offset_read;i <= (ring->max_len - str_len);i++)
        {
            next = false;
            for(j=0;j < str_len;j++)
            {
                if(buf[i+j] != string[j])
                    next = true;    //如果发现不等,则表示要进行下一个循环
            }
            if(next == false)
            {   //表示上一个循环中已经发现匹配的串,
                return (i - ring->offset_read);
            }
        }
        //再处理缓冲区首末结合部分以及回绕部分
        if(ring->offset_write >= str_len)
        {   //缓冲区中回绕部分的长度超过查找长度,末端和首端分别查找
            //先查找缓冲区末端部分
            for(;i < ring->max_len;i++)
            {
                next = false;
                //string分成两部分,end个字符在缓冲区末端,start个字符在缓冲区首
                end = ring->max_len - i;
                start = str_len - end;
                for(j=0;j<end;j++)
                {   //先比较缓冲区末端部分
                    if(buf[i+j] != string[j])
                        next = true;
                }
                if(next == false)
                {   //缓冲区末端部分全部匹配,需要看首端是否也匹配
                    for(j=0;j<start;j++)
                        if(buf[j] != string[start+j])
                            next = true;
                }
                if(next == false)
                {   //表示上一个循环中已经发现匹配的串,
                    return (i - ring->offset_read);
                }
            }
            //再查找首端部分
            for(i=0;i < (ring->offset_write - str_len);i++)
            {
                next = false;
                for(j=0;j < str_len;j++)
                {
                    if(buf[i+j] != string[j])
                        next = true;    //如果发现不等,则表示要进行下一个循环
                }
                if(next == false)
                {   //表示上一个循环中已经发现匹配的串,
                    return (i - ring->offset_read);
                }
            }
        }else
        {   //缓冲区中回绕部分长度没有超过str_len,只需要查找末端即可.
            next = false;
            for(;i < (ring->offset_read + ring->len - str_len);i++)
            {
                u32  end,start;
                next = false;
                //string分成两部分,end个字符在缓冲区末端,start个字符在缓冲区首
                end = ring->max_len - i;
                start = str_len - end;
                for(j=0;j<end;j++)
                {   //先比较缓冲区末端部分
                    if(buf[i+j] != string[j])
                        next = true;
                }
                if(next == false)
                {   //缓冲区末端部分全部匹配,需要看首端是否也匹配
                    for(j=0;j<start;j++)
                        if(buf[j] != string[start+j])
                            next = true;
                }
                if(next == false)
                {   //表示上一个循环中已经发现匹配的串,
                    return (i - ring->offset_read);
                }
            }
        }
    }
    return CN_LIMIT_UINT32;
}
