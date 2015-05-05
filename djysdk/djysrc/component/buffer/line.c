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

//所属模块:线性缓冲区
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 提供线性缓冲区服务
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "int.h"
#include "line.h"
#include "string.h"

//----建立线性缓冲区----------------------------------------------------------
//功能: 建立线性缓冲区并初始化，使用这个函数之前，用户应该定义缓冲区内存块和
//      缓冲区数据结构。
//参数: line,目标线性缓冲区结构指针
//      buf,缓冲区起始地址
//      len,缓冲区长度.单位是字节数
//返回: 无
//-----------------------------------------------------------------------------
void Line_Init(struct tagLineBuf *line, u8 *buf, u32 len)
{
    line->buf = buf;
    line->limit= len;
    line->current = 0;
}

//----检查缓冲区容量-----------------------------------------------------------
//功能: 返回缓冲区容量
//参数: line,目标线性缓冲区结构指针
//返回: 缓冲区容量，就是调用line_init时使用的len参数。
//-----------------------------------------------------------------------------
u32 Line_Capacity(struct tagLineBuf *line)
{
    return line->limit;
}

//----取消若干数据---------------------------------------------------------------
//功能: 取消已经写入线性缓冲区的若干数据，就像从来没有写入一样。
//参数: line,目标线性缓冲区指针.
//      len,退回的数据数量
//返回: 实际退回的数据量
//------------------------------------------------------------------------------
u32 Line_SkipTail(struct tagLineBuf *line,u32 len)
{
    u32 result;
    if(line->current > len)
    {
        line->current -= len;
        result = len;
    }else
    {
        result = line->current;
        line->current = 0;
    }
    return result;
}

//----线性缓冲区写入-----------------------------------------------------------
//功能: 线性缓冲区写入若干个字节,返回实际写入的数据量,并移动写指针,如果线性
//      缓冲区没有足够的空间,按实际剩余空间写入
//参数: line,目标线性缓冲区结构指针
//      buffer,待写入的数据指针
//      len,待写入的数据长度.单位是字节数
//返回: 实际写入的字节数,如果缓冲区有足够的空间,=len
//-----------------------------------------------------------------------------
u32 Line_Write(struct tagLineBuf *line,u8 *buffer,u32 len)
{
    u32    wr_len;
    atom_low_t  atom_bak;
    wr_len = line->limit - line->current;
    if(wr_len == 0)
        return 0;
    if(wr_len > len)
        wr_len = len;
    memcpy( &line->buf[line->current],buffer,wr_len);
    atom_bak = Int_LowAtomStart();
    line->current += wr_len;
    Int_LowAtomEnd(atom_bak);
    return wr_len;
}

//----从线性缓冲区读-----------------------------------------------------------
//功能: 从线性缓冲区读出全部数据，并复位current指针
//参数: line,目标线性缓冲区结构指针
//      buffer,接收数据的缓冲区指针
//返回: 实际读出的字节数
//-----------------------------------------------------------------------------
u32 Line_Read(struct tagLineBuf *line,u8 *buffer)
{
    u32    wr_len;
    wr_len = line->current;
    memcpy( buffer,line->buf,wr_len);
    line->current = 0;
    return wr_len;
}

//----返回字节池地址-----------------------------------------------------------
//功能: 查询缓冲区的字节池地址，这个地址是用户调用line_init时使用的缓冲区地址。
//参数: line,目标线性缓冲区指针.
//返回: 线性缓冲区的字节池地址
//说明: 有两种情况会产生获得缓冲区地址的需求
//      1、要释放缓冲区内存，应用程序可以使用调用line_init时使用的指针，如果该
//         指针丢失，可用本函数取回该指针。
//      2、用户想自己动手访问该缓冲区，这不是一个明智的选择，有破坏模块独立性
//         的嫌疑，这时候，使用者应该完全明白自己在干什么!
//-----------------------------------------------------------------------------
u8 *Line_GetBuf(struct tagLineBuf *line)
{
    return line->buf;
}
//----检查缓冲区数据量---------------------------------------------------------
//功能: 检查指定的线性缓冲区中的数据量,返回字节数.
//参数: line,目标线性缓冲区指针.
//返回: 缓冲区中的数据量
//-----------------------------------------------------------------------------
u32    Line_Check(struct tagLineBuf *line)
{
    return line->current;
}

//----检查缓冲区是否空---------------------------------------------------------
//功能: 检查指定的线性缓冲区中是否已经空.
//参数: line,目标线性缓冲区指针.
//返回: 空则返回true,非空返回false
//-----------------------------------------------------------------------------
bool_t   Line_IsEmpty(struct tagLineBuf *line)
{
    return (line->current ==0)? true:false;
}

//----检查缓冲区是否满---------------------------------------------------------
//功能: 检查指定的线性缓冲区中是否已经满.
//参数: line,目标线性缓冲区指针.
//返回: 满则返回true,非满返回false
//-----------------------------------------------------------------------------
bool_t   Line_IsFull(struct tagLineBuf *line)
{
    return (line->current == line->limit)? true:false;
}

//----清空线性缓冲区-----------------------------------------------------------
//功能: 清除缓冲区中所有数据
//参数: line,目标线性缓冲区指针.
//返回: 无
//-----------------------------------------------------------------------------
void    Line_Flush(struct tagLineBuf *line)
{
    line->current = 0;
}

//----查找字符-----------------------------------------------------------------
//功能: 从line当前读位置开始查找字符c的位置
//参数: line,目标线性缓冲区指针
//      c,需查找的字符
//返回: c出现的位置,如果没有出现则返回 CN_LIMIT_UINT32
//-----------------------------------------------------------------------------
u32 Line_SearchCh(struct tagLineBuf *line, char c)
{
    u32    i;
    u8 *buf = line->buf;
    for(i=0; i<line->current; i++)
    {
        if(buf[i] == c)
            return i;
    }
    return CN_LIMIT_UINT32;
}

//----查找字符序列-------------------------------------------------------------
//功能: 从line当前读位置开始查找字符序列的位置,字符序列不以0结束,而是指定长度
//参数: line,目标线性缓冲区指针
//      string,需查找的字符序列
//      str_len,字符序列长度
//返回: string出现的位置相对offset_read的偏移量,如果没有出现返回 CN_LIMIT_UINT32
//备注: 这个功能可能比较常用,所以在编写时注意了速度优化,但却使代码量大增.
//-----------------------------------------------------------------------------
u32 Line_SearchStr(struct tagLineBuf *line, char *string,u32 str_len)
{
    u32 i,j;
    bool_t next;
    u8 *buf;

    if(line->current < str_len)
        return CN_LIMIT_UINT32;
    buf = line->buf;
    for(i=0; i <= line->current - str_len; i++)
    {
        next = false;
        for(j=0;j < str_len;j++)
        {
            if(buf[i+j] != string[j])
                next = true;
        }
        if(next == false)
        {
            return i;
        }
    }
    return CN_LIMIT_UINT32;
}

