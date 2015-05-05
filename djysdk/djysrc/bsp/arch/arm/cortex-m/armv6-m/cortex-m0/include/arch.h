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
//作者：lst
//版本：V1.0.0
//文件描述: 内核配置头文件
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-10-16
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __ARCH_H_
#define __ARCH_H_

#ifdef __cplusplus
extern "C" {
#endif

//存储器大小端的配置
#define CN_CFG_LITTLE_ENDIAN       0
#define CN_CFG_BIG_ENDIAN          1
#define CN_CFG_BYTE_ORDER          CN_CFG_LITTLE_ENDIAN

//与CPU字长相关的配置
#define CN_CPU_BITS     32  //处理器字长
#define CN_PTR_BITS     32  //CPU地址位数，移植敏感
#define CN_CPU_BITS_SUFFIX_ZERO 5
#define CN_BYTE_BITS    8   //字节位宽

    #define CN_LIMIT_UCPU   CN_LIMIT_UINT32
    #define CN_MIN_UCPU     (0)

    #define CN_LIMIT_SCPU   CN_LIMIT_SINT32
    #define CN_MIN_SCPU     CN_MIN_SINT32

    #define CN_LIMIT_UFAST  CN_LIMIT_UINT8
    #define CN_MIN_UFAST    (0)

    #define CN_LIMIT_SFAST  CN_LIMIT_SINT8
    #define CN_MIN_SFAST    CN_MIN_SINT8

//下面定义cpu系统的栈类型
#define CN_FULL_DOWN_STACK  0   //向下生长的满栈
#define CN_EMPTY_DOWN_STACK 1   //向下生长的空栈
#define CN_FULL_UP_STACK    2   //向上生长的满栈
#define CN_EMPTY_UP_STACK   3   //向上生长的空栈
#define CN_STACK_TYPE       CN_FULL_DOWN_STACK

//定义是否支持cache，loader根据本常量决定是否做cache同步
#define cn_cache_used       0
#define cn_mmu_used         0   //定义是否支持mmu
#define cortex_m0

//代理内核栈的深度计算方法:栈需求最深的那个系统服务
#define CN_KERNEL_STACK     300     //代理内核栈，线程需要为系统服务额外增加的栈
                                    //目前最大api函数dev_add_root_device 需256bytes
                                    //----2009-10-11

//定义一些与对齐相关的宏
#define MEM_ALIGN           1           //如果目标系统没有对齐要求，改为0
#define ALIGN_SIZE          8           //arm(含cm3)要求8字节对齐

#define align_down(bound,x) ((x)&(~((bound)-1)))    //把x按bound边界向下对齐
#define align_up(bound,x)   (((x)+((bound)-1))&(~((bound)-1)))  //向上对齐,

//以下声明本系统的数据对齐方式，移植敏感
#define align_down_sys(x)   align_down(8,x)   //arm要求8字节对齐
#define align_up_sys(x)     align_up(8,x)     //arm要求8字节对齐

#define define_align_buf(name,size)     \
                align_type name[align_up_sys(size)/sizeof(ALIGN_SIZE)]

#ifdef __cplusplus
}
#endif
#endif // __ARCH_H_


