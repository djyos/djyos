//-----------------------------------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然:
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明:本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
#include <sys/socket.h>

#include "tcpipconfig.h"

//attention than the biggest buffer is 8192,if you need more, please contact the author
//we will add some
//this var used to make the pkg aligned by the specified value
#define CN_ALIGN_DEFAULT         0X20                   //default align size
static u16 gPkgHdrSizeAlign = CN_ALIGN_DEFAULT;         //the package aligned size


#define CN_NETPKG_LEVEL           9                     //how many layers
static tagNetPkg *sgPkgFreeLst[CN_NETPKG_LEVEL];        //used to list the free pkg
#define CN_PKG_HDRSIZE    (sizeof(tagNetPkg))
static u32  sgPkgLevlMap[CN_NETPKG_LEVEL]=  {32,64,128,256,512,1024,2048,4096,8192}; //layer size
static u8  *pPkgMemSrc = NULL;                          //the package heap
static u32  gPkgMemOffset = 0;                          //the package use heap offset
static struct MutexLCB *pPkgMemSync = NULL;             //protect the heap and the free list
// =============================================================================
// FUNCTION   :use this function to set the aligned size
// PARAMS IN  :alignsize,must be larger than zero,if less than the default, then will set
//             it to the default
// PARAMS OUT :
// RETURN     :true success while false failed
// DESCRIPTION:
// =============================================================================
bool_t Pkg_SetAlignOffset(u16 alignsize)
{
	if(alignsize < CN_ALIGN_DEFAULT)
	{
		gPkgHdrSizeAlign = CN_ALIGN_DEFAULT;
	}
	else
	{
		gPkgHdrSizeAlign = alignsize;
	}
	return true;
}
// =============================================================================
// FUNCTION   :use this function to get the align size
// PARAMS IN  :
// PARAMS OUT :
// RETURN     :the align size used in the pkg
// DESCRIPTION:
// =============================================================================
u16 PkgAlignSize(u16 size)
{
	return gPkgHdrSizeAlign;
}

typedef struct
{
	 u32 mallocnum[CN_NETPKG_LEVEL]; //how many pack we have malloc ok from the heap
	 u32 freenum[CN_NETPKG_LEVEL];   //how many pack we have free
	 u32 failnum[CN_NETPKG_LEVEL];   //how many times we failed to malloc the buffer
	 u32 overbufnum;                 //which means it is out of the max buffer size times
}tagPkgStatistics;
static tagPkgStatistics gPkgStatistics;
// =============================================================================
// FUNCTION   :use this function to malloc a pkg
// PARAMS IN  :buffersize, the pkg buffer size
//             bufferpro, which means the pkg is the end in the queue
// PARAMS OUT :
// RETURN     :the package malloc or NULL failed
// DESCRIPTION:
// =============================================================================
tagNetPkg *PkgMalloc(u16 bufsize, u8 flags)
{
	u8 i ;
	u16  pkgsize;
	tagNetPkg *result;

	result = NULL;
	pkgsize = bufsize+CN_PKG_HDRSIZE;
	//find the pkgsize level
	for(i =0;i < CN_NETPKG_LEVEL;i++)
	{
		if(pkgsize < sgPkgLevlMap[i])
		{
			break;
		}
	}
	if(i < CN_NETPKG_LEVEL)
	{
		pkgsize = sgPkgLevlMap[i]; //we must aligned it to the layer
		//ok, now we should malloc it from the heap or the free list
		if(Lock_MutexPend(pPkgMemSync,CN_TIMEOUT_FOREVER))
		{
		    if(NULL != sgPkgFreeLst[i])
            {
                //fetch the pkg from the free lst
                result = sgPkgFreeLst[i];
                sgPkgFreeLst[i] = result->partnext;
                TCPIP_DEBUG_DEC(gPkgStatistics.freenum[i]);
            }
            else if((pkgsize+gPkgMemOffset)<gNetPkgMemSize)
            {
                result = (tagNetPkg *)(pPkgMemSrc + gPkgMemOffset);
                gPkgMemOffset += pkgsize;
                TCPIP_DEBUG_INC(gPkgStatistics.mallocnum[i]);
            }
            else
            {
                TCPIP_DEBUG_INC(gPkgStatistics.failnum[i]);
            }
            if(NULL != result)
            {
                result->partnext = NULL;
                result->bufsize = pkgsize - CN_PKG_HDRSIZE;
                result->buf = (u8 *)result + CN_PKG_HDRSIZE;
                result->datalen = 0;
                result->refers = 0;
                result->pkgflag =flags;
                result->level = i;
                result->offset = 0;
            }
		    Lock_MutexPost(pPkgMemSync);
		}
	}
	else
	{
		//no level matched
		TCPIP_DEBUG_INC(gPkgStatistics.overbufnum);
	}
	return result;
}

// =============================================================================
// FUNCTION   :use this function to free an package
// PARAMS IN  :pkg, the package to free
// PARAMS OUT :
// RETURN     :true
// DESCRIPTION:if the package's refers is zero then we will free it else sub the refers
//             if its matches the heap's offset, then will free it to the heap
// =============================================================================
bool_t PkgTryFreePart(tagNetPkg *pkg)
{
	u8         level;
	u32        len;
	if(NULL != pkg)
	{
	    if(Lock_MutexPend(pPkgMemSync,CN_TIMEOUT_FOREVER))
	    {
	        if(pkg->refers == 0)
	        {
	        	//we check free it to the queue or the heap
	            level = pkg->level;
	        	len = sgPkgLevlMap[level];

	            if((pPkgMemSrc + gPkgMemOffset)==((u8 *)pkg + len))
	            {
	            	//free it to the heap.
	            	memset((void *)pkg,0,len);
	            	gPkgMemOffset -= len;
	            	TCPIP_DEBUG_DEC(gPkgStatistics.mallocnum[level]);
	            }
	            else
	            {
	            	//free it to the queue
		            pkg->partnext = sgPkgFreeLst[level];
		            sgPkgFreeLst[level] = pkg;
		    		TCPIP_DEBUG_INC(gPkgStatistics.freenum[level]);
	            }
	        }
	        else
	        {
	            pkg->refers--;
	        }
	        Lock_MutexPost(pPkgMemSync);
	    }
	}
	return true;
}
// =============================================================================
// FUNCTION   :we use this function to free the queue which terminated by the queue
// PARAMS IN  :pkglst, the package to free
// PARAMS OUT :
// RETURN     :true
// DESCRIPTION:
// =============================================================================
bool_t PkgTryFreeLst(tagNetPkg  *pkglst)
{
	u8          level;
	tagNetPkg  *pkg;
	tagNetPkg  *pkgnxt;
	if(Lock_MutexPend(pPkgMemSync,CN_TIMEOUT_FOREVER))
	{
	    pkg = pkglst;
	    while(NULL != pkg)
	    {
	        if(PKG_ISLISTEND(pkg))
	        {
	            pkgnxt = NULL;// this is the end of the lst
	        }
	        else
	        {
	            pkgnxt = pkg->partnext;
	        }
	        if(pkg->refers==0)//允许释放
	        {
	            level = pkg->level;
	            pkg->partnext = sgPkgFreeLst[level];
	            sgPkgFreeLst[level] = pkg;
	    		TCPIP_DEBUG_INC(gPkgStatistics.freenum[level]);
	        }
	        else
	        {
	            pkg->refers--;
	        }
	        pkg = pkgnxt;
	    }
	    Lock_MutexPost(pPkgMemSync);
	}
	return true;
}
// =============================================================================
// FUNCTION   :we use this function to free the queue which terminated by partnxt is NULL
// PARAMS IN  :pkglst, the package to free
// PARAMS OUT :
// RETURN     :true
// DESCRIPTION:
// =============================================================================
bool_t PkgTryFreeQ(tagNetPkg  *pkglst)
{
	u8          level;
	tagNetPkg  *pkg;
	tagNetPkg  *pkgnxt;
	if(Lock_MutexPend(pPkgMemSync,CN_TIMEOUT_FOREVER))
	{
	    pkg = pkglst;
	    while(NULL != pkg)
	    {
            pkgnxt = pkg->partnext;
	        if(pkg->refers==0)
	        {
	            level = pkg->level;
	            pkg->partnext = sgPkgFreeLst[level];
	            sgPkgFreeLst[level] = pkg;
	    		TCPIP_DEBUG_INC(gPkgStatistics.freenum[level]);
	        }
	        else
	        {
	            pkg->refers--;
	        }
	        pkg = pkgnxt;
	    }
	    Lock_MutexPost(pPkgMemSync);
	}
	return true;
}
// =============================================================================
// FUNCTION   :we use this function to cache a part, which add the refers
// PARAMS IN  :pkg, the package to cache
// PARAMS OUT :
// RETURN     :true
// DESCRIPTION:
// =============================================================================
bool_t PkgCachedPart(tagNetPkg  *pkg)
{
	if(NULL != pkg)
	{
		pkg->refers++;
	}
	return true;
}
// =============================================================================
// FUNCTION   :we use this function to cache a package list, which add the refers
// PARAMS IN  :pkglst, the package list to cache
// PARAMS OUT :
// RETURN     :true
// DESCRIPTION:
// =============================================================================
bool_t PkgCachedLst(tagNetPkg   *pkglst)
{
	tagNetPkg *pkg;
	pkg = pkglst;
	while(NULL != pkg)
	{
		pkg->refers++;
		if(PKG_ISLISTEND(pkg))
		{
			pkg = NULL;// this is the end of the lst
		}
		else
		{
			pkg = pkg->partnext;
		}
	}
	return true;
}

// =============================================================================
// FUNCTION   :we use this function browse the package module status
// PARAMS IN  :param,NO USE NOW
// PARAMS OUT :
// RETURN     :true
// DESCRIPTION:
// =============================================================================
bool_t PkgMemShow(char *param)
{
	int i =0;
	printf("PkgMem:HdrSize:%d\n\r",CN_PKG_HDRSIZE);
	printf("PkgMem:    TOTAL:%08d   USE:%08d  USAGE:%02d%%\n\r",\
	        gNetPkgMemSize,gPkgMemOffset,gPkgMemOffset*100/(gNetPkgMemSize+1));
	printf("OverLayer:%d\n\r",gPkgStatistics.overbufnum);
	for(i = 0; i<CN_NETPKG_LEVEL;i++)
	{
	    printf("layer:%04d:TOTAL:0x%08x   Free:0x%08x  Failed:0x%08x\n\r",\
		        sgPkgLevlMap[i],gPkgStatistics.mallocnum[i],gPkgStatistics.freenum[i],gPkgStatistics.failnum[i]);
	}
	return true;
}

struct ShellCmdTab  gPkgDebug[] =
{
    {
        "pkgmem",
		PkgMemShow,
        "usage:pkgmem",
        "show the pkg module statistics"
    },

};

#define CN_PKGDEBUG_ITEMNUM  ((sizeof(gPkgDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gPkgDebugCmdRsc[CN_PKGDEBUG_ITEMNUM];
// =============================================================================
// FUNCTION:this function is used to initialize the package memory
// PARA  IN:
// PARA OUT:
// RETURN  :true success while false failed
// INSTRUCT:
// =============================================================================
bool_t PkgInit(void)
{
    bool_t result = false;
    //FIRST MALLOC THE MEM FROM THE HEAP
    gPkgMemOffset = 0;
	pPkgMemSync = Lock_MutexCreate(NULL);
	if(NULL == pPkgMemSync)
	{
	    printf("%s:create memory sync failed\r\n",__FUNCTION__);
	    goto EXIT_SYNC;
	}
    pPkgMemSrc = malloc(gNetPkgMemSize);
    if(NULL == pPkgMemSrc)
    {
        printf("%s:create memory block failed\r\n",__FUNCTION__);
        goto EXIT_MEM;
    }

    result = Sh_InstallCmd(gPkgDebug,gPkgDebugCmdRsc,CN_PKGDEBUG_ITEMNUM);
    if(false == result)
    {
        printf("%s:install memory command failed\r\n",__FUNCTION__);
        goto EXIT_CMD;
    }
    return result;
EXIT_CMD:
    free((void *)pPkgMemSrc);
    pPkgMemSrc = NULL;
EXIT_MEM:
    Lock_MutexDelete(pPkgMemSync);
    pPkgMemSync = NULL;
EXIT_SYNC:
    result = false;
    return result;
}




