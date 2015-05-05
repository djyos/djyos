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
// 这份授权条款，在使用者符合以下二条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、此三条件表列，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、此三条件表列，以及下述
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
//所属模块: stdlib
//作者：罗侍田
//版本：V1.0.0
//文件描述: 产生随机数的函数。
//其他说明:
//修订历史:
//2. ...
//1. 日期  : 2014/12/10
//   修改人: 罗侍田
//   新版本号:
//   修改说明: 新建
//------------------------------------------------------

#include <stdlib.h>

static u64 s_u64RandSeed = 1;
static u64 s_u64Rand48Seed = 1;

//-----------------------------------------------------------------------------
//功能: 初始化伪随机函数rand的随机种子
//参数: Seed，随机种子，例如调用时的高精度time值，或者从端口读入一个随机值。
//输出: 无
//返回: 无
//------------
//更新记录:
// 1.日期: 2014/12/10
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
void srand(u32 Seed)
{
    s_u64RandSeed = Seed;
}

//-----------------------------------------------------------------------------
//功能: 取伪随机数，调用前最好找一个随机数去初始化随机种子（调用srand函数）
//参数: 无
//输出: 无
//返回: 伪随机数
//------------
//更新记录:
// 1.日期: 2014/12/10
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
s32 rand(void)
{
    s_u64RandSeed = s_u64RandSeed * 6364136223846793005LL + 1;
    return (s32)((s_u64RandSeed >> 32) & CN_LIMIT_SINT32);
}

#define Rand48_M 0x100000000LL
#define Rand48_C 0xB16
#define Rand48_A 0x5DEECE66DLL

//-----------------------------------------------------------------------------
//功能: 产生32位伪随机数
//参数:
//输出: 无
//返回: 48位伪随机数
//------------
//更新记录:
// 1.日期: 2014/12/10
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
u32 lrand48(void)
{
	s_u64Rand48Seed = (Rand48_A * s_u64Rand48Seed + Rand48_C) & 0xFFFFFFFFFFFFLL;
    return (u32)(s_u64Rand48Seed >> 16);
}


//-----------------------------------------------------------------------------
//功能: 产生0~1之间的伪随机数
//参数:
//输出: 无
//返回: 伪随机浮点数
//------------
//更新记录:
// 1.日期: 2014/12/10
//   说明: 创建
//   作者: 罗侍田
//-----------------------------------------------------------------------------
double drand48(void)
{
    u32 x;
    s_u64Rand48Seed = (Rand48_A * s_u64Rand48Seed + Rand48_C) & 0xFFFFFFFFFFFFLL;
    x = (u32)(s_u64Rand48Seed >> 16);
    return  ((double)x / (double)Rand48_M);

}

void srand48(s32 i)
{
    s_u64Rand48Seed  = (long long int)i << 16;
}

