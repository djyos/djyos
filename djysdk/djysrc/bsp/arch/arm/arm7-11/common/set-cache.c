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
//所属模块:调度器
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:cache操作相关的代码。
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2011-10-21
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "set-cache.h"
#include "arch_feature.h"

//----作废cache----------------------------------------------------------------
//功能: 把指令cache和数据cache全部作废，数据cache中的"脏"数据并不写回主存。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_InvalidAll(void)
{
    u32 reg = 0;
#if         defined( arm720t) \
        ||  defined( arm920t) \
        ||  defined( arm922t)\
        ||  defined( arm926ej_s)\
        ||  defined( arm1022e)\
        ||  defined( arm1026ej_s)\
        ||  defined( strong_arm)\
        ||  defined( xscale)
    __asm__(
        "mov    %0,#0 \n\t"
        "mcr    p15,0,%0,c7,c7,0 \n\t"
        :
        :"r"(reg)
        );
#elif       defined( arm940t) ||  defined( arm946e_s)
    __asm__(
        "mov    %0,#0 \n\t"
        "mcr    p15,0,%0,c7,c5,0 \n\t"
        "mcr    p15,0,%0,c7,c6,0 \n\t"
        :
        :"r"(reg)
        );
#endif
}

//----作废指令cache------------------------------------------------------------
//功能: 把指令cache作废
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_InvalidInst(void)
{
    u32 reg = 0;
#if     defined( arm920t) \
    ||  defined( arm922t)\
    ||  defined( arm926ej_s)\
    ||  defined( arm940t)\
    ||  defined( arm946e_s)\
    ||  defined( arm1022e)\
    ||  defined( arm1026ej_s)\
    ||  defined( strong_arm)\
    ||  defined( xscale)
    __asm__(
        "mov    %0,#0 \n\t"
        "mcr    p15,0,%0,c7,c5,0 \n\t"
        :
        :"r"(reg)
        );
#elif defined( arm720t) || defined( arm740t)
    __asm__(
        "mov    %0,#0 \n\t"
        "mcr    p15,0,%0,c7,c7,0 \n\t"
        :
        :"r"(reg)
        );
#endif
}

//----作废数据cache------------------------------------------------------------
//功能: 把数据cache全部作废，数据cache中的"脏"数据并不写回主存。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_InvalidData(void)
{
    u32 reg = 0;
#if     defined( arm920t) \
        ||  defined( arm922t)\
        ||  defined( arm926ej_s)\
        ||  defined( arm940t)\
        ||  defined( arm946e_s)\
        ||  defined( arm1022e)\
        ||  defined( arm1026ej_s)\
        ||  defined( strong_arm)\
        ||  defined( xscale)
        __asm__(
            "mov    %0,#0 \n\t"
            "mcr    p15,0,%0,c7,c6,0 \n\t"
            :
            :"r"(reg)
            );
#elif defined( arm720t) || defined( arm740t)
        __asm__(
            "mov    %0,#0 \n\t"
            "mcr    p15,0,%0,c7,c7,0 \n\t"
            :
            :"r"(reg)
            );
#endif
}

//----使能cache----------------------------------------------------------------
//功能: 使能指令和数据cache
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_EnableAll(void)
{
    u32 reg = 0;
    __asm__(
        "mrc    p15, 0, %0, c1, c0, 0 \n\t"
        "orr    %0, %0, #0x1000 \n\t"
        "orr    %0, %0, #0x4 \n\t"
        "mcr    p15,0,%0,c1,c0,0 \n\t"
        :
        :"r"(reg)
        );
}

//----使能指令cache------------------------------------------------------------
//功能: 使能指令cache
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_EnableInst(void)
{
    u32 reg = 0;
    __asm__(
        "mrc    p15, 0, %0, c1, c0, 0 \n\t"
        "orr    %0, %0, #0x1000 \n\t"
        "mcr    p15,0,%0,c1,c0,0 \n\t"
        :
        :"r"(reg)
        );
}

//----使能数据cache------------------------------------------------------------
//功能: 使能数据cache
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_EnableData(void)
{
    u32 reg = 0;
    __asm__(
        "mrc    p15, 0, %0, c1, c0, 0 \n\t"
        "orr    %0, %0, #0x4 \n\t"
        "mcr    p15,0,%0,c1,c0,0 \n\t"
        :
        :"r"(reg)
        );
}

//----禁止cache----------------------------------------------------------------
//功能: 禁止指令和数据cache
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_DisableAll(void)
{
    u32 reg = 0;
    __asm__(
        "mrc    p15, 0, %0, c1, c0, 0 \n\t"
        "orr    %0, %0, #0x1000 \n\t"
        "orr    %0, %0, #0x4 \n\t"
        "mcr    p15,0,%0,c1,c0,0 \n\t"
        :
        :"r"(reg)
        );
}

//----禁止指令cache------------------------------------------------------------
//功能: 禁止指令cache
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_DisableInst(void)
{
    u32 reg = 0;
    __asm__(
        "mrc    p15, 0, %0, c1, c0, 0 \n\t"
        "orr    %0, %0, #0x1000 \n\t"
        "mcr    p15, 0, %0, c1, c0, 0 \n\t"
        :
        :"r"(reg)
        );
}

//----禁止数据cache------------------------------------------------------------
//功能: 禁止数据cache
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_DisableData(void)
{
    u32 reg = 0;
    __asm__(
        "mrc    p15, 0, %0, c1, c0, 0 \n\t"
        "orr    %0, %0, #0x4 \n\t"
        "mcr    p15,0,%0,c1,c0,0 \n\t"
        :
        :"r"(reg)
        );
}


//----使能写缓冲---------------------------------------------------------------
//功能: 使能写缓冲
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_EnableWriteBuf(void)
{
    u32 reg = 0;
    __asm__(
        "mrc    p15, 0, %0, c1, c0, 0 \n\t"
        "orr    %0, %0, #0x8 \n\t"
        "mcr    p15, 0, %0, c1, c0, 0 \n\t"
        :
        :"r"(reg)
        );
}

//----禁止写缓冲---------------------------------------------------------------
//功能: 禁止写缓冲
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_DisableWriteBuf(void)
{
    u32 reg = 0;
    __asm__(
        "mrc    p15, 0, %0, c1, c0, 0 \n\t"
        "orr    %0, %0, #0x8 \n\t"
        "mcr    p15,0,%0,c1,c0,0 \n\t"
        :
        :"r"(reg)
        );
}

//----清理数据cache------------------------------------------------------------
//功能: 清理整个数据cache，数据cache中的"脏"数据写回主存，以保持同步。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Cache_CleanData(void)
{
#if defined( arm926ej_s)      ||  defined( arm1026ej_s)
    __asm__("clean: \n\t"
        "mrc p15,0,pc,c7,c10,3 \n\t"
        "bne clean \n\t");
#elif       defined( arm920t) \
        ||  defined( arm922t)\
        ||  defined( arm940t)\
        ||  defined( arm946e_s)\
        ||  defined( arm1022e)
    u32 rd,way,line;
    for(way = 0; way < CN_CACHE_WAY; way++)
    {
        //下式计算得到每路的行数
        for(line = 0;line<CN_CACHE_SIZE/CN_CACHE_WAY/CN_CACHE_LINE_SIZE; line++)
        {
            rd = (way<<CN_C7_OFFSET_WAY) + (line<<CN_C7_OFFSET_SET);
            __asm__("mcr    p15, 0, %0, c7, c10, 2 \n\t"
                    :
                    :"r"(rd)
            );
        }
    }
#elif defined( arm720t) || defined( arm740t)
//这两cpu只有写通方式，无须清理。
#else
#error 不认识的cpu类型
#endif
}

void Cache_config(void)
{
	//...
}



