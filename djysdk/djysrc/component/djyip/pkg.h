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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: 
// 模块版本: V1.00
// 创建人员: Admin
// 创建时间: 7:11:44 PM/Mar 3, 2015
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================

#ifndef NETPKG_H_
#define NETPKG_H_

#include "stdint.h"

//tagNetPkg的原理
//bufsize在申请时指定，使用过程中一直不变;data一直指向buf的起始位置，保持不变
//当向PKG写入数据时，offset不变，从buf的offset+datalen的地方开始写新数据，写完之后
//                  datalen +=len(len为写入数据长度)
//当从PKG读取数据时，从buf的offset开始cpy，cpy完成之后，
//                  offset += len,datalen -= len(len为取出数据长度)
#define CN_PKGFLAG_NULL      (1<<0)    //无属性，该类的PKG必须自己手动调用Pkg_PartFree释放
#define CN_PKGFLAG_FREE      (1<<1)    //档使用链表释放函数时，标志该位将会被释放掉，否则不会释放      
#define CN_PKGFLAG_BUFNODE   (1<<2)    //该标志表示该PKG仅仅是一个bufnode，buf由外部提供
typedef struct _tagNetPkg
{    	
	struct _tagNetPkg   *partnext;	       //该组指针负责数据包在协议栈的传递
	struct _tagNetPkg   **pkglsthead;      //该指针用于负责存储其所在空闲链表
	u16  flag;          // 该pkg的一些特性
    u16  datalen;       // buf中的有效数据长度
	u16  bufsize;	    // buf的长度
	u16  offset;        // 有效数据偏离buf的位置，offset之前的数据无效,当分拆数据或者数据对齐的时候很有用
    u8   *buf;	        // pkg的buf（数据缓存区）
}tagNetPkg;

tagNetPkg *Pkg_Alloc(u16 bufsize, u16 proper);
bool_t Pkg_PartFree(tagNetPkg *pkg);
bool_t Pkg_PartFlagFree(tagNetPkg *pkg);
bool_t Pkg_LstFree(tagNetPkg *plst);
bool_t Pkg_LstFlagFree(tagNetPkg *plst);
bool_t Pkg_SetAlignOffset(u16 alignsize);

#endif /* NETPKG_H_ */
