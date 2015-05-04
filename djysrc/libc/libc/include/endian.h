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
//所属模块:公共函数
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
#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "arch_feature.h"
#include "stdint.h"
u16 pick_little_16bit(u8 *buf,u32 index);
u32 pick_little_32bit(u8 *buf,u32 index);
u64 pick_little_64bit(u8 *buf,u32 index);
void fill_little_16bit(u8 *buf,u32 index,u16 data);
void fill_little_32bit(u8 *buf,u32 index,u32 data);
void fill_little_64bit(u8 *buf,u32 index,u64 data);
u16 pick_big_16bit(u8 *buf,u32 index);
u32 pick_big_32bit(u8 *buf,u32 index);
u64 pick_big_64bit(u8 *buf,u32 index);
void fill_big_16bit(u8 *buf,u32 index,u16 data);
void fill_big_32bit(u8 *buf,u32 index,u32 data);
void fill_big_64bit(u8 *buf,u32 index,u64 data);

//系统字节序转换
 //----16位数据大小端对换---------------------------------------------------------
 //功能: 16位数据做大小端兑换
 //参数: value,输入的16位数据
 //返回: 转换后的16位数据
 //------------------------------------------------------------------------------
#define swaps(value) (((value&((u16)0xff00))>>8)|((value&((u16)0x00ff))<<8))
 //----32位数据大小端对换---------------------------------------------------------
 //功能: 32位数据做大小端兑换
 //参数: value,输入的32位数据
 //返回: 转换后的32位数据
 //------------------------------------------------------------------------------

#define swapl(value)  (((value&((u32)0xff000000))>>24)|((value&((u32)0xff0000))>>8)|\
                      ((value&((u32)0xff00))<<8)|((value&((u32)0xff))<<24))
 //----16位数据本地端转换为小端---------------------------------------------------
 //功能: 16位数据本地端转换为小端
 //参数: value,输入的16位数据
 //返回: 转换后的16位数据
 //------------------------------------------------------------------------------

#if(CN_CFG_BYTE_ORDER == CN_CFG_BIG_ENDIAN)
#define htoles(value)  swaps(value)
#define htolel(value)  swapl(value)
#define htobes(value)  (value)
#define htobel(value)  (value)
#else
#define htoles(value)  (value)
#define htolel(value)  (value)
#define htobes(value)  swaps(value)
#define htobel(value)  swapl(value)
#endif

//网络字节序定义
#define htons      htobes      //主机字节序转换为网络字节序（16位）
#define htonl      htobel      //主机字节序转换为网络字节序（32位）
#define ntohs      htobes      //网络字节序转换为主机字节序（16位）
#define ntohl      htobel      //网络字节序转换为主机字节序（32位

#ifdef __cplusplus
}
#endif

#endif // _ENDIAN_H_

