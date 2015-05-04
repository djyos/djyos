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
// 创建时间: 7:11:21 PM/Mar 3, 2015
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"

#include "os.h"

#include "pkg.h"

//use this to define the net pkg mem size
extern const u32 gNetPkgMemSize;   
//this var used to make the pkg aligned by the specified value
static u16 sgPkgBufAlignSize = 0x10;
#define CN_NETPKG_SIZE  sizeof(tagNetPkg)

//PKG的各种FREE链表
#define CN_NETPKG_LEVEL             6
static tagNetPkg *sgPkgFreeLst[CN_NETPKG_LEVEL];  //used to list the free pkg
static u32 sgPkgLevlMap[CN_NETPKG_LEVEL]={64,128,256,512,1024,2048}; 
#define CN_PKG_SIZELEVEL_MAP 32
static u8 sgPkgSizeLevelMap[CN_PKG_SIZELEVEL_MAP]=
{0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};

// =============================================================================
// 函数功能：Pkg_SetAlignOffset
//          PKG设置对齐
// 输入参数：alignsize,对齐大小
// 输出参数：无
// 返回值  ：true，初始化成功 ，false初始化失败
// 说明：设置之后，该对齐对所有的后续对齐有效，如果以前设置过更高的，那么本次设置不会成功
// =============================================================================
bool_t Pkg_SetAlignOffset(u16 alignsize)
{
	bool_t result;

	result = false;
	if(alignsize >sgPkgBufAlignSize)
	{
		result = true;
	}
	return result;
}

static u8  *pgPkgMemSrc = NULL;
static u32  sgPkgMemOffset = 0;
// =============================================================================
// 函数功能：Pkg_Init
//          PKG管理单元初始化
// 输入参数：无
// 输出参数：无
// 返回值  ：true，初始化成功 ，false初始化失败
// 说明：初始化列表和各自的限制量
// =============================================================================
bool_t Pkg_Init(void)
{
    //FIRST MALLOC THE MEM FROM THE HEAP
    pgPkgMemSrc = M_Malloc(gNetPkgMemSize,0);
    if(NULL == pgPkgMemSrc)
    {
    	printk("Pkg_Init:No memory for net pkg!\n\r");
    	return false;
    }
    else
    {
    	printk("Pkg: MemSrc = %08x\n\r",(u32)pgPkgMemSrc);
    	sgPkgMemOffset = 0;
    	sgPkgBufAlignSize = 0x10;//initialize the buf align request(16 bytes)
    	return true;	
    }
}

// =============================================================================
// 函数功能：Pkg_Alloc
//          分配合适大小的PKG
// 输入参数：bufsize,用户所需的缓冲区的大小
// 输出参数：无
// 返回值  ：分配的PKG，NULL表示分配失败（如参数不对或者内存不足）
// 说明    ：bufsize表示的是缓冲区的大小，一般而言，bufsize加上pkg本身占用的空间一定不会
//        超过2K，对于特别需求需要超过2K的，可以使用多个大包组合即可。
//        不论从池中分配的还是从FREE链表分配的还是动态分配的，一旦分配成功，则会
//        进行必要的初始化，并添加到pgTCPIPPkgScanLst队列中
// =============================================================================
tagNetPkg *Pkg_Alloc(u16 bufsize, u16 flag)
{
	u8 i ;
	u16  pkgsize;
	atom_low_t atomop;
	tagNetPkg *result = NULL;
	
	pkgsize = bufsize + CN_NETPKG_SIZE +sgPkgBufAlignSize;
	i = pkgsize/64;
	if(i < CN_PKG_SIZELEVEL_MAP)
	{
		//map the size to the defined size
		i = sgPkgSizeLevelMap[i];
		pkgsize = sgPkgLevlMap[i];
		
		atomop = Int_LowAtomStart();
		if(NULL != sgPkgFreeLst[i])
		{
			//fetch the pkg from the free lst
			result = sgPkgFreeLst[i];
			sgPkgFreeLst[i] = result->partnext;
		}
		else if((pkgsize+sgPkgMemOffset)<gNetPkgMemSize)
		{
			//alloc pkg from the specified mem src
			result = (tagNetPkg *)(pgPkgMemSrc + sgPkgMemOffset);
			sgPkgMemOffset += pkgsize;
//			printk("PkgAlloc:New mem 0ffse =%08x\n\r",sgPkgMemOffset);
			
		}
		else
		{
			printk("PkgAlloc:Not Enough Mem size = %08x\n\r",pkgsize);
		}
		if(NULL != result)
		{
//			printk("PkgAlloc:%08x %08x %08x\n\r",(u32)result,pkgsize,i);
			
			result->partnext = NULL;
			result->bufsize = pkgsize - CN_NETPKG_SIZE;
			result->buf = (u8 *)result + CN_NETPKG_SIZE;
			result->datalen = 0;
			result->flag = flag;
			//CN_NETPKG_DEBUG
			result->pkglsthead = &sgPkgFreeLst[i];
			//you'd better do the align here
			i = (sgPkgBufAlignSize-1)&((u32)result->buf);
			if(i != 0)
			{
				result->offset = sgPkgBufAlignSize- i;
			}
			else
			{
				result->offset =0;
			}
		}
		
		Int_LowAtomEnd(atomop);
	}

	return result;
}
//void __Pkg_Free(tagNetPkg *pkg)
//{
//	tagNetPkg **pkglsthead;
//	
//	printk("PkgFree:%08x \n\r",(u32)pkg);
//	pkglsthead =pkg->pkglsthead;
//	pkg->partnext = *pkglsthead;
//	*pkglsthead = pkg;
//}
//// =============================================================================
//// 函数功能：Pkg_PartFree
//// 输入参数：pkg,待释放的数据包
//// 输出参数：无
//// 返回值  ：
//// 说明    ：该函数主要用于那些自己分配的PKG，同时没有指定其释放方式;比方TCP自己分配，收到
////         ACK自己释放的数据包，可以采用这种方式；如果某个PKG不是该协议分配的，请使用
////         Pkg_LstFlagFree来释放
//// =============================================================================
//bool_t Pkg_PartFree(tagNetPkg *pkg)
//{
//	atom_low_t atomop;
//
//	if(NULL != pkg)
//	{
//		atomop = Int_LowAtomStart();
//		__Pkg_Free(pkg);
//		
//		Int_LowAtomEnd(atomop);
//	}
//	return true;
//}
//// =============================================================================
//// 函数功能：Pkg_PartFlagFree
//// 输入参数：pkg,待释放的数据包
//// 输出参数：无
//// 返回值  ：
//// 说明    ：该函数释放一个PKG，并且其flag标志位
//// =============================================================================
//bool_t Pkg_PartFlagFree(tagNetPkg *pkg)
//{
//	atom_low_t atomop;
//
//	if((NULL != pkg)&&(pkg->flag&CN_PKGFLAG_FREE))
//	{
//		atomop = Int_LowAtomStart();
//		__Pkg_Free(pkg);
//
//		Int_LowAtomEnd(atomop);
//	}
//	return true;
//}
//// =============================================================================
//// 函数功能：Pkg_LstFree
//// 输入参数：plst,待释放的pkg队列
//// 输出参数：无
//// 返回值  ：true 成功 false失败
//// 说明：该函数直接释放PKG队列，不判断其FLAG。
//// =============================================================================
//bool_t Pkg_LstFree(tagNetPkg *plst)
//{
//	tagNetPkg *pkg;
//	atom_low_t atomop;
//	tagNetPkg *pkgnxt;
//
//	atomop = Int_LowAtomStart();
//	pkg = plst;
//	while(NULL != pkg)
//	{
//		pkgnxt = pkg->partnext;
//		__Pkg_Free(pkg);
//
//		pkg = pkgnxt;
//	}
//	Int_LowAtomEnd(atomop);
//	return true;
//}
//
//// =============================================================================
//// 函数功能：Pkg_LstFlagFree
//// 输入参数：plst,待释放的pkg队列
//// 输出参数：无
//// 返回值  ：true 成功 false失败
//// 说明：该函数会逐一根据PKG的标志来决定是否释放
//// =============================================================================
//bool_t Pkg_LstFlagFree(tagNetPkg *plst)
//{
//	tagNetPkg *pkg;
//	atom_low_t atomop;
//	tagNetPkg **pkglsthead;
//	tagNetPkg *pkgnxt;
//
//	atomop = Int_LowAtomStart();
//	pkg = plst;
//	while(NULL != pkg)
//	{
//		pkgnxt = pkg->partnext;
//		if(pkg->flag & CN_PKGFLAG_FREE)//允许释放
//		{
//			__Pkg_Free(pkg);
//		}
//		pkg = pkgnxt;
//	}
//	Int_LowAtomEnd(atomop);
//	return true;
//}
// =============================================================================
// 函数功能：Pkg_PartFree
// 输入参数：pkg,待释放的数据包
// 输出参数：无
// 返回值  ：
// 说明    ：该函数主要用于那些自己分配的PKG，同时没有指定其释放方式;比方TCP自己分配，收到
//         ACK自己释放的数据包，可以采用这种方式；如果某个PKG不是该协议分配的，请使用
//         Pkg_LstFlagFree来释放
// =============================================================================
bool_t Pkg_PartFree(tagNetPkg *pkg)
{
	atom_low_t atomop;
	tagNetPkg **pkglsthead;

	if(NULL != pkg)
	{
		atomop = Int_LowAtomStart();
		pkglsthead =pkg->pkglsthead;
		pkg->partnext = *pkglsthead;
		*pkglsthead = pkg;
		Int_LowAtomEnd(atomop);
	}
	return true;
}
// =============================================================================
// 函数功能：Pkg_PartFlagFree
// 输入参数：pkg,待释放的数据包
// 输出参数：无
// 返回值  ：
// 说明    ：该函数释放一个PKG，并且其flag标志位
// =============================================================================
bool_t Pkg_PartFlagFree(tagNetPkg *pkg)
{
	atom_low_t atomop;
	tagNetPkg **pkglsthead;

	if((NULL != pkg)&&(pkg->flag&CN_PKGFLAG_FREE))
	{
		atomop = Int_LowAtomStart();
		pkglsthead =pkg->pkglsthead;
		pkg->partnext = *pkglsthead;
		*pkglsthead = pkg;
		Int_LowAtomEnd(atomop);
	}
	return true;
}
// =============================================================================
// 函数功能：Pkg_LstFree
// 输入参数：plst,待释放的pkg队列
// 输出参数：无
// 返回值  ：true 成功 false失败
// 说明：该函数直接释放PKG队列，不判断其FLAG。
// =============================================================================
bool_t Pkg_LstFree(tagNetPkg *plst)
{
	tagNetPkg *pkg;
	atom_low_t atomop;
	tagNetPkg **pkglsthead;
	tagNetPkg *pkgnxt;

	atomop = Int_LowAtomStart();
	pkg = plst;
	while(NULL != pkg)
	{
		pkgnxt = pkg->partnext;
		pkglsthead =pkg->pkglsthead;
		pkg->partnext = *pkglsthead;
		*pkglsthead = pkg;
		pkg = pkgnxt;
	}
	Int_LowAtomEnd(atomop);
	return true;
}

// =============================================================================
// 函数功能：Pkg_LstFlagFree
// 输入参数：plst,待释放的pkg队列
// 输出参数：无
// 返回值  ：true 成功 false失败
// 说明：该函数会逐一根据PKG的标志来决定是否释放
// =============================================================================
bool_t Pkg_LstFlagFree(tagNetPkg *plst)
{
	tagNetPkg *pkg;
	atom_low_t atomop;
	tagNetPkg **pkglsthead;
	tagNetPkg *pkgnxt;

	atomop = Int_LowAtomStart();
	pkg = plst;
	while(NULL != pkg)
	{
		pkgnxt = pkg->partnext;
		if(pkg->flag & CN_PKGFLAG_FREE)//允许释放
		{
			pkglsthead =pkg->pkglsthead;
			pkg->partnext = *pkglsthead;
			*pkglsthead = pkg;
		}
		pkg = pkgnxt;
	}
	Int_LowAtomEnd(atomop);
	return true;
}


