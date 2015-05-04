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
//所属模块:大小端调整相关的公共函数
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:公共函数
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "arch_feature.h"
#include "endian.h"

//----读取一个小端16位数据-----------------------------------------------------
//功能: 从小端格式的存储器中读出一个16位数据。
//参数: buf，缓冲区
//      index，偏移量，即第几个16位数
//返回: 读取的16位数据
//----------------------------------------------------------------------------
u16 pick_little_16bit(u8 *buf,u32 index)
{
    //即使端格相同，也不能使用 return *(uint16_t *)buf简化，
    //因为不能保证buf是一个对齐了的地址
#if (CN_BYTE_BITS == 8)
    u32 align8=0;
    align8=(u32)buf;
    align8 &= 0x01;
    //加速提取的情况有两种：
    //1.无对齐要求，且cpu为小端模式
    //2.有对齐要求但buf是对齐存储的，且cpu为小端模式
    if(((CN_MEM_ALIGN == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN))
        ||  ((CN_MEM_ALIGN == 1)&&(align8 == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)))
    {
        return *(u16 *)buf;
    }
    else
    {
        u16 data;
        //data = buf[2*index];
        //data+= buf[2*index+1]<<8;
#if (CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)
        *((u8*)(&data)+0) = buf[2*index+0];//数据低位
        *((u8*)(&data)+1) = buf[2*index+1];
#elif (CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)
        *((u8*)(&data)+1) = buf[2*index+0];
        *((u8*)(&data)+0) = buf[2*index+1];
#endif
        return (u16)data;
    }
#else
    u16 data;
    data = buf[2*index]&0xFF;
    data+= (buf[2*index+1]&0xFF)<<8;
    return (u16)data;
#endif
}

//----读取一个小端32位数据-----------------------------------------------------
//功能: 从小端格式的存储器中读出一个32位数据。
//参数: buf，缓冲区
//      index，偏移量，即第几个32位数
//返回: 读取的32位数据
//----------------------------------------------------------------------------
u32 pick_little_32bit(u8 *buf,u32 index)
{
    //即使端格相同，也不能使用 return *(u32 *)buf简化，
    //因为不能保证buf是一个对齐了的地址
#if (CN_BYTE_BITS == 8)
    u32 align8=0;
    align8=(u32)buf;
    align8 &= 0x03;
    //加速提取的情况有两种：
    //1.无对齐要求，且cpu为小端模式
    //2.有对齐要求但buf是对齐存储的，且cpu为小端模式
    if(((CN_MEM_ALIGN == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN))
        ||  ((CN_MEM_ALIGN == 1)&&(align8 == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)))
    {
        return *(u32 *)buf;
    }
    else
    {
        u32 data;
        //data = buf[4*index];
        //data+= buf[4*index+1]<<8;
        //data+= buf[4*index+2]<<16;
        //data+= buf[4*index+3]<<24;
#if (CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)
        *((u8*)(&data)+0) = buf[4*index+0];//数据低位
        *((u8*)(&data)+1) = buf[4*index+1];
        *((u8*)(&data)+2) = buf[4*index+2];
        *((u8*)(&data)+3) = buf[4*index+3];
#elif (CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)
        *((u8*)(&data)+3) = buf[4*index+0];
        *((u8*)(&data)+2) = buf[4*index+1];
        *((u8*)(&data)+1) = buf[4*index+2];
        *((u8*)(&data)+0) = buf[4*index+3];
#endif
        return (u32)data;
    }
#else
    u32 data;
    data = buf[4*index]&0xFF;
    data+= (buf[4*index+1]&0xFF)<<8;
    data+= (buf[4*index+2]&0xFF)<<16;
    data+= (buf[4*index+3]&0xFF)<<24;
    return (u32)data;
#endif
}

//----读取一个小端64位数据-----------------------------------------------------
//功能: 从小端格式的存储器中读出一个64位数据。
//参数: buf，缓冲区
//      index，偏移量，即第几个64位数
//返回: 读取的64位数据
//----------------------------------------------------------------------------
u64 pick_little_64bit(u8 *buf,u32 index)
{
#if (CN_BYTE_BITS == 8)
    u32 align8=0;
    align8=(u32)buf;
    align8 &= 0x07;
    //加速提取的情况有两种：
    //1.无对齐要求，且cpu为小端模式
    //2.有对齐要求但buf是对齐存储的，且cpu为小端模式
    if(((CN_MEM_ALIGN == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN))
        ||  ((CN_MEM_ALIGN == 1)&&(align8 == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)))
    {
        return *(u64 *)buf;
    }
    else
    {
        u64 data=0;

        /*data=buf[8*index+7];
        data<<=8;
        data+=buf[8*index+6];
        data<<=8;
        data+=buf[8*index+5];
        data<<=8;
        data+=buf[8*index+4];
        data<<=8;
        data+=buf[8*index+3];
        data<<=8;
        data+=buf[8*index+2];
        data<<=8;
        data+=buf[8*index+1];
        data<<=8;
        data+=buf[8*index+0];*/
#if (CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)
        *((u8*)(&data)+0) = buf[8*index+0];//数据低位
        *((u8*)(&data)+1) = buf[8*index+1];
        *((u8*)(&data)+2) = buf[8*index+2];
        *((u8*)(&data)+3) = buf[8*index+3];
        *((u8*)(&data)+4) = buf[8*index+4];
        *((u8*)(&data)+5) = buf[8*index+5];
        *((u8*)(&data)+6) = buf[8*index+6];
        *((u8*)(&data)+7) = buf[8*index+7];
#elif (CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)
        *((u8*)(&data)+7) = buf[8*index+0];
        *((u8*)(&data)+6) = buf[8*index+1];
        *((u8*)(&data)+5) = buf[8*index+2];
        *((u8*)(&data)+4) = buf[8*index+3];
        *((u8*)(&data)+3) = buf[8*index+4];
        *((u8*)(&data)+2) = buf[8*index+5];
        *((u8*)(&data)+1) = buf[8*index+6];
        *((u8*)(&data)+0) = buf[8*index+7];
#endif

        return (u64)data;
    }
#else
    u64 data=0;

    data=buf[8*index+7]&0xFF;
    data<<=8;
    data+=buf[8*index+6]&0xFF;
    data<<=8;
    data+=buf[8*index+5]&0xFF;
    data<<=8;
    data+=buf[8*index+4]&0xFF;
    data<<=8;
    data+=buf[8*index+3]&0xFF;
    data<<=8;
    data+=buf[8*index+2]&0xFF;
    data<<=8;
    data+=buf[8*index+1]&0xFF;
    data<<=8;
    data+=buf[8*index+0]&0xFF;

    return (u64)data;
#endif
}

//----填充一个小端16位数据-----------------------------------------------------
//功能: 把一个16位数据填充到小端格式的存储器中。
//参数: buf，缓冲区
//      index，偏移量，即第几个16位数
//      data，填充的数据
//返回: 无
//----------------------------------------------------------------------------
void fill_little_16bit(u8 *buf,u32 index,u16 data)
{
    buf[2*index+0] = (u8)data;
    buf[2*index+1] = (u8)(data>>8);
#if (CN_BYTE_BITS != 8)
    buf[2*index+0] &= 0xFF;
    buf[2*index+1] &= 0xFF;
#endif  //#if (CN_BYTE_BITS != 8)
}

//----填充一个小端32位数据-----------------------------------------------------
//功能: 把一个32位数据填充到小端格式的存储器中。
//参数: buf，缓冲区
//      index，偏移量，即第几个32位数
//      data，填充的数据
//返回: 无
//----------------------------------------------------------------------------
void fill_little_32bit(u8 *buf,u32 index,u32 data)
{
    buf[4*index+0] = (u8)data;
    buf[4*index+1] = (u8)(data>>8);
    buf[4*index+2] = (u8)(data>>16);
    buf[4*index+3] = (u8)(data>>24);
#if (CN_BYTE_BITS != 8)
    buf[4*index+0] &= 0xFF;
    buf[4*index+1] &= 0xFF;
    buf[4*index+2] &= 0xFF;
    buf[4*index+3] &= 0xFF;
#endif  //#if (CN_BYTE_BITS != 8)
}

//----填充一个小端64位数据-----------------------------------------------------
//功能: 把一个64位数据填充到小端格式的存储器中。
//参数: buf，缓冲区
//      index，偏移量，即第几个64位数
//      data，填充的数据
//返回: 无
//----------------------------------------------------------------------------
void fill_little_64bit(u8 *buf,u32 index,u64 data)
{
    buf[8*index+0] = (u8)data;
    buf[8*index+1] = (u8)(data>>8);
    buf[8*index+2] = (u8)(data>>16);
    buf[8*index+3] = (u8)(data>>24);
    buf[8*index+4] = (u8)(data>>32);
    buf[8*index+5] = (u8)(data>>40);
    buf[8*index+6] = (u8)(data>>48);
    buf[8*index+7] = (u8)(data>>56);
#if (CN_BYTE_BITS != 8)
    buf[8*index+0] &= 0xFF;
    buf[8*index+1] &= 0xFF;
    buf[8*index+2] &= 0xFF;
    buf[8*index+3] &= 0xFF;
    buf[8*index+4] &= 0xFF;
    buf[8*index+5] &= 0xFF;
    buf[8*index+6] &= 0xFF;
    buf[8*index+7] &= 0xFF;
#endif  //#if (CN_BYTE_BITS != 8)
}

//----读取一个大端16位数据-----------------------------------------------------
//功能: 从大端格式的存储器中读出一个16位数据。
//参数: buf，缓冲区
//      index，偏移量，即第几个16位数
//返回: 读取的16位数据
//----------------------------------------------------------------------------
u16 pick_big_16bit(u8 *buf,u32 index)
{
    //即使端格相同，也不能使用 return *(uint16_t *)buf简化，
    //因为不能保证buf是一个对齐了的地址
#if (CN_BYTE_BITS == 8)
    u32 align8=0;
    align8=(u32)buf;
    align8 &= 0x01;
    //加速提取的情况有两种：
    //1.无对齐要求，且cpu为大端模式
    //2.有对齐要求但buf是对齐存储的，且cpu为大端模式
    if(((CN_MEM_ALIGN == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN))
        ||  ((CN_MEM_ALIGN == 1)&&(align8 == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)))
    {
        return *(u16 *)buf;
    }
    else
    {
        u16 data;
        //data = buf[2*index+1];
        //data+= buf[2*index]<<8;
#if (CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)
        *((u8*)(&data)+1) = buf[2*index+0];//数据高位
        *((u8*)(&data)+0) = buf[2*index+1];
#elif (CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)
        *((u8*)(&data)+0) = buf[2*index+0];
        *((u8*)(&data)+1) = buf[2*index+1];
#endif
        return (u16)data;
    }
#else
    u16 data;
    data = buf[2*index+1]&0xFF;
    data+= (buf[2*index]&0xFF)<<8;
    return (u16)data;
#endif
}

//----读取一个大端32位数据-----------------------------------------------------
//功能: 从大端格式的存储器中读出一个32位数据。
//参数: buf，缓冲区
//      index，偏移量，即第几个32位数
//返回: 读取的32位数据
//----------------------------------------------------------------------------
u32 pick_big_32bit(u8 *buf,u32 index)
{
    //即使端格相同，也不能使用 return *(u32 *)buf简化，
    //因为不能保证buf是一个对齐了的地址
#if (CN_BYTE_BITS == 8)
    u32 align8=0;
    align8=(u32)buf;
    align8 &= 0x03;
    //加速提取的情况有两种：
    //1.无对齐要求，且cpu为大端模式
    //2.有对齐要求但buf是对齐存储的，且cpu为大端模式
    if(((CN_MEM_ALIGN == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN))
        ||  ((CN_MEM_ALIGN == 1)&&(align8 == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)))
    {
        return *(u32 *)buf;
    }
    else
    {
        u32 data;
        /*data = buf[4*index+3];
        data+= buf[4*index+2]<<8;
        data+= buf[4*index+1]<<16;
        data+= buf[4*index]<<24;*/
#if (CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)
        *((u8*)(&data)+3) = buf[4*index+0];//数据高位
        *((u8*)(&data)+2) = buf[4*index+1];
        *((u8*)(&data)+1) = buf[4*index+2];
        *((u8*)(&data)+0) = buf[4*index+3];
#elif (CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)
        *((u8*)(&data)+0) = buf[4*index+0];
        *((u8*)(&data)+1) = buf[4*index+1];
        *((u8*)(&data)+2) = buf[4*index+2];
        *((u8*)(&data)+3) = buf[4*index+3];
#endif
        return (u32)data;
    }
#else
    u32 data;
    data = buf[4*index+3]&0xFF;
    data+= (buf[4*index+2]&0xFF)<<8;
    data+= (buf[4*index+1]&0xFF)<<16;
    data+= (buf[4*index]&0xFF)<<24;
    return (u32)data;
#endif
}

//----读取一个大端64位数据-----------------------------------------------------
//功能: 从大端格式的存储器中读出一个64位数据。
//参数: buf，缓冲区
//      index，偏移量，即第几个64位数
//返回: 读取的64位数据
//----------------------------------------------------------------------------
u64 pick_big_64bit(u8 *buf,u32 index)
{
#if (CN_BYTE_BITS == 8)
    u32 align8=0;
    align8=(u32)buf;
    align8 &= 0x07;
    //加速提取的情况有两种：
    //1.无对齐要求，且cpu为大端模式
    //2.有对齐要求但buf是对齐存储的，且cpu为大端模式
    if(((CN_MEM_ALIGN == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN))
        ||  ((CN_MEM_ALIGN == 1)&&(align8 == 0)&&(CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)))
    {
        return *(u64 *)buf;
    }
    else
    {
        u64 data=0;

        /*data=buf[8*index+0];
        data<<=8;
        data+=buf[8*index+1];
        data<<=8;
        data+=buf[8*index+2];
        data<<=8;
        data+=buf[8*index+3];
        data<<=8;
        data+=buf[8*index+4];
        data<<=8;
        data+=buf[8*index+5];
        data<<=8;
        data+=buf[8*index+6];
        data<<=8;
        data+=buf[8*index+7];*/
#if (CN_CFG_BYTE_ORDER==CN_CFG_LITTLE_ENDIAN)
        *((u8*)(&data)+7) = buf[4*index+0];//数据高位
        *((u8*)(&data)+6) = buf[4*index+1];
        *((u8*)(&data)+5) = buf[4*index+2];
        *((u8*)(&data)+4) = buf[4*index+3];
        *((u8*)(&data)+3) = buf[4*index+4];
        *((u8*)(&data)+2) = buf[4*index+5];
        *((u8*)(&data)+1) = buf[4*index+6];
        *((u8*)(&data)+0) = buf[4*index+7];
#elif (CN_CFG_BYTE_ORDER==CN_CFG_BIG_ENDIAN)
        *((u8*)(&data)+0) = buf[4*index+0];
        *((u8*)(&data)+1) = buf[4*index+1];
        *((u8*)(&data)+2) = buf[4*index+2];
        *((u8*)(&data)+3) = buf[4*index+3];
        *((u8*)(&data)+4) = buf[4*index+4];
        *((u8*)(&data)+5) = buf[4*index+5];
        *((u8*)(&data)+6) = buf[4*index+6];
        *((u8*)(&data)+7) = buf[4*index+7];
#endif
        return (u64)data;
    }
#else
    u64 data=0;

    data=buf[8*index+0]&0xFF;
    data<<=8;
    data+=buf[8*index+1]&0xFF;
    data<<=8;
    data+=buf[8*index+2]&0xFF;
    data<<=8;
    data+=buf[8*index+3]&0xFF;
    data<<=8;
    data+=buf[8*index+4]&0xFF;
    data<<=8;
    data+=buf[8*index+5]&0xFF;
    data<<=8;
    data+=buf[8*index+6]&0xFF;
    data<<=8;
    data+=buf[8*index+7]&0xFF;

    return (u64)data;
#endif

}


//----填充一个大端16位数据-----------------------------------------------------
//功能: 把一个16位数据填充到大端格式的存储器中。
//参数: buf，缓冲区
//      index，偏移量，即第几个16位数
//      data，填充的数据
//返回: 无
//----------------------------------------------------------------------------
void fill_big_16bit(u8 *buf,u32 index,u16 data)
{
    buf[2*index+1] = (u8)data;
    buf[2*index+0] = (u8)(data>>8);
#if (CN_BYTE_BITS != 8)
    buf[2*index+1] &= 0xFF;
    buf[2*index+0] &= 0xFF;
#endif  //#if (CN_BYTE_BITS != 8)
}

//----填充一个大端32位数据-----------------------------------------------------
//功能: 把一个32位数据填充到大端格式的存储器中。
//参数: buf，缓冲区
//      index，偏移量，即第几个32位数
//      data，填充的数据
//返回: 无
//----------------------------------------------------------------------------
void fill_big_32bit(u8 *buf,u32 index,u32 data)
{
    buf[4*index+3] = (u8)data;
    buf[4*index+2] = (u8)(data>>8);
    buf[4*index+1] = (u8)(data>>16);
    buf[4*index+0] = (u8)(data>>24);
#if (CN_BYTE_BITS != 8)
    buf[4*index+3] &= 0xFF;
    buf[4*index+2] &= 0xFF;
    buf[4*index+1] &= 0xFF;
    buf[4*index+0] &= 0xFF;
#endif  //#if (CN_BYTE_BITS != 8)
}

//----填充一个大端64位数据-----------------------------------------------------
//功能: 把一个64位数据填充到大端格式的存储器中。
//参数: buf，缓冲区
//      index，偏移量，即第几个64位数
//      data，填充的数据
//返回: 无
//----------------------------------------------------------------------------
void fill_big_64bit(u8 *buf,u32 index,u64 data)
{
    buf[8*index+7] = (u8)data;
    buf[8*index+6] = (u8)(data>>8);
    buf[8*index+5] = (u8)(data>>16);
    buf[8*index+4] = (u8)(data>>24);
    buf[8*index+3] = (u8)(data>>32);
    buf[8*index+2] = (u8)(data>>40);
    buf[8*index+1] = (u8)(data>>48);
    buf[8*index+0] = (u8)(data>>56);
#if (CN_BYTE_BITS != 8)
    buf[8*index+7] &= 0xFF;
    buf[8*index+6] &= 0xFF;
    buf[8*index+5] &= 0xFF;
    buf[8*index+4] &= 0xFF;
    buf[8*index+3] &= 0xFF;
    buf[8*index+2] &= 0xFF;
    buf[8*index+1] &= 0xFF;
    buf[8*index+0] &= 0xFF;
#endif  //#if (CN_BYTE_BITS != 8)
}
////----16位数据大小端对换---------------------------------------------------------
////功能: 16位数据做大小端兑换
////参数: value,输入的16位数据
////返回: 转换后的16位数据
////------------------------------------------------------------------------------
//
//
////20140529修改，VisualDSP++不支持inline
//
//u16 swaps(u16 value)
//{
//    return ((value&((u16)0xff00))>>8)|((value&((u16)0x00ff))<<8);
//}
//
////----32位数据大小端对换---------------------------------------------------------
////功能: 32位数据做大小端兑换
////参数: value,输入的32位数据
////返回: 转换后的32位数据
////------------------------------------------------------------------------------
//
//u32 swapl(u32 value)
//{
//    return ((value&((u32)0xff000000))>>24)|((value&((u32)0xff0000))>>8)|\
//            ((value&((u32)0xff00))<<8)|((value&((u32)0xff))<<24);
//}
//
////----16位数据本地端转换为小端---------------------------------------------------
////功能: 16位数据本地端转换为小端
////参数: value,输入的16位数据
////返回: 转换后的16位数据
////------------------------------------------------------------------------------
//
////20140529修改，VisualDSP++不支持inline
//u16  htoles(u16 value)
//{
//    if(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)
//    {
//        return value;
//    }
//    else
//    {
//        return swaps(value);
//    }
//}
//
//
////----32位数据本地端转换为小端---------------------------------------------------
////功能: 32位数据本地端转换为小端
////参数: value,输入的32位数据
////返回: 转换后的32位数据
////------------------------------------------------------------------------------
//
////20140529修改，VisualDSP++不支持inline
//
//u32  htolel(u32 value)
//{
//    if(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)
//    {
//        return value;
//    }
//    else
//    {
//        return swapl(value);
//    }
//}
////----16位数据本地端转换为大端---------------------------------------------------
////功能: 16位数据本地端转换为大端
////参数: value,输入的16位数据
////返回: 转换后的16位数据
////------------------------------------------------------------------------------
//
////20140529修改，VisualDSP++不支持inline
//u16  htobes(u16 value)
//{
//    if(CN_CFG_BYTE_ORDER == CN_CFG_BIG_ENDIAN)
//    {
//        return value;
//    }
//    else
//    {
//        return swaps(value);
//    }
//}
//
////----32位数据本地端转换为大端---------------------------------------------------
////功能: 32位数据本地端转换为大端
////参数: value,输入的32位数据
////返回: 转换后的32位数据
////------------------------------------------------------------------------------
//
////20140529修改，VisualDSP++不支持inline
//u32  htobel(u32 value)
//{
//    if(CN_CFG_BYTE_ORDER == CN_CFG_BIG_ENDIAN)
//    {
//        return value;
//    }
//    else
//    {
//        return swapl(value);
//    }
//}



