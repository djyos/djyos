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
// Copyright (C) 2012-2020 深圳鹏瑞软件有限公司 All Rights Reserved
// 模块描述: Icmp.c:ICMP协议的实现
// 模块版本: V1.00
// 创建人员: Zqf
// 创建时间: 3:59:27 PM/Sep 20, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "endian.h"
#include "string.h"

#include "os.h"

#include "pkg.h"
#include "icmp.h"
#include "ipV4.h"
#include "tcpip_cdef.h"  //refered to the INADDR_ANY
#pragma pack(1)
typedef struct
{
	u8        type;        //type for icmp
	u8        code;        //code for icmp
	u16       chksum;      //pkg chksum
    u8        data[0];     //additional data for specified type and data
}tagIcmpHdr;

typedef struct
{
	u16 taskid;
	u16 seqno;
	u8  data[0];
}tagIcmpHdrEcho;

#pragma pack()

enum _EN_ICMP_TYPE
{
	EN_ICMP_ECHOREPLY = 0,
	EN_ICMP_DSTNOTREACH =3,
	EN_ICMP_SRCSHUTDOWN,
	EN_ICMP_RELOCATE,
	EN_ICMP_ECHOREQUEST =8,
	EN_ICMP_ROUTNOTICE,
	EN_ICMP_ROUTREQUEST,
	EN_ICMP_TIMEOUT,
	EN_ICMP_PARAERR,
	EN_ICMP_TIMEREQUEST,
	EN_ICMP_TIMEREPLY,
	EN_ICMP_ADDRMSKREQUEST = 17,
	EN_ICMP_ADDRMSKREPLY,
};
#define CN_ICMP_ECHOREPLY_CODE           0    //PING REPLY CODE 
#define CN_ICMP_ECHOREQUEST_CODE         0    //PING REQUEST CODE


//the data used for request and answer info
typedef struct __tagIcmpTask
{
	struct __tagIcmpTask             *pre;     //pre  lst
	struct __tagIcmpTask             *nxt;     //nxt  lst
	struct tagSemaphoreLCB           sync;     //when rcv, active it
	//the member used for specifying
	u8        type;                             //icmp type
	u8        code;                             //icmp code
	u8        *data;                            //task data:the data type and code need
}tagIcmpTask;

typedef struct
{
	u16     taskid;
	u16     seqno;
	u32     ipdst;
}tagIcmpTaskEcho;



static tagIcmpTask                *pgIcmpTaskLst;    //icmp queue
static struct tagMutexLCB         sgIcmpTaskSync;    //icmp queue sync
// =============================================================================
// 函数功能：__Icmp_TaskAdd
//          add an task to the queue
// 输入参数：task, the task to add
// 输出参数：
// 返回值  ：true success while  false failed
// 说明    :
// =============================================================================
bool_t __Icmp_TaskAdd(tagIcmpTask *task)
{	
	if(Lock_MutexPend(&sgIcmpTaskSync, CN_TIMEOUT_FOREVER))
	{
		//create the semp sync
		Lock_SempCreate_s(&task->sync,1,0,CN_SEMP_BLOCK_FIFO,NULL);

		task->pre = NULL;
		task->nxt = pgIcmpTaskLst;
		if(NULL != pgIcmpTaskLst)
		{
			pgIcmpTaskLst->pre = task;
		}
		pgIcmpTaskLst = task;
		Lock_MutexPost(&sgIcmpTaskSync);
		return true;
	}
	else
	{
		return false;
	}
}

// =============================================================================
// 函数功能：__Icmp_TaskDel
//          remove an task from the queue
// 输入参数：task, the task to remove
// 输出参数：
// 返回值  ：true success while  false failed
// 说明    :
// =============================================================================
bool_t __Icmp_TaskDel(tagIcmpTask *task)
{	
	if(Lock_MutexPend(&sgIcmpTaskSync, CN_TIMEOUT_FOREVER))
	{
		if(task == pgIcmpTaskLst)
		{
			task = pgIcmpTaskLst;
			pgIcmpTaskLst = task->nxt;
			if(NULL != pgIcmpTaskLst)
			{
				pgIcmpTaskLst->pre = NULL;
			}
		}
		else
		{
			if(NULL == task->nxt)
			{
				task->pre->nxt = NULL;
			}
			else
			{
				task->pre->nxt = task->nxt;
				task->nxt->pre = task->pre;
			}
		}
		//del the semp sync
		Lock_SempDelete_s(&task->sync);
		Lock_MutexPost(&sgIcmpTaskSync);
		return true;
	}
	else
	{
		return false;
	}
}
// =============================================================================
// 函数功能：__Icmp_TaskEchoActive
// 输入参数：
//          ipsrc:pkg src
//          ipdst:pkg dst
//          type: icmp type
//          code: icmp code
//          pkg:  icmp info pkg
// 输出参数：
// 返回值  ：
// 说明    :
// =============================================================================
void __Icmp_TaskEchoActive(u32 ipsrc, u32 ipdst,u8 type, u8 code,tagNetPkg *pkg)
{
	tagIcmpTask         *tmp;
	tagIcmpTaskEcho     *taskdata;
	tagIcmpHdrEcho      *echodata;
	
	if(Lock_MutexPend(&sgIcmpTaskSync, CN_TIMEOUT_FOREVER))
	{
		tmp = pgIcmpTaskLst;
		echodata = (tagIcmpHdrEcho *)(pkg->buf + pkg->offset);
		while(NULL != tmp)
		{
			if((tmp->type == type)&&(tmp->code == code))
			{
				taskdata = (tagIcmpTaskEcho *)tmp->data;
				if((taskdata->ipdst == ipsrc)&&\
				   (taskdata->taskid ==echodata->taskid)&&\
				   (taskdata->seqno == echodata->seqno))
				{
					Lock_SempPost(&tmp->sync);
					break;
				}
			}
			tmp =tmp->nxt;
		}
		Lock_MutexPost(&sgIcmpTaskSync);
	}
	return;
}
// =============================================================================
// 函数功能：Icmp_EchoRequest
//          ping request
// 输入参数： ipdst, the ip of the host we will ping
//           data, additionnal data
//           len, additionnal data len
// 输出参数：
// 返回值  ：true , request success while false timeout 
// 说明    :
// =============================================================================
#define CN_ICMP_RESNDECHO_TIMES   0X10        //if the channel is not prepare
                                              //resnd 16 times
#define CN_ICMP_ECHOTIMEOUT       (10*1000*mS)//10 Sencond
bool_t Icmp_EchoRequest(u32 ipdst, u8 *data, int len)
{
	bool_t               result;
	bool_t               sndresult;
	static u16           seqno = 0;
	u16                  pkglen;
	tagNetPkg            *sndpkg;
	tagIcmpHdr           *icmppkg;
	tagIcmpHdrEcho       *icmppkgecho;
	tagIcmpTask          icmptask;
	tagIcmpTaskEcho      icmptaskecho;
	
	result = false;
	if((NULL != data)&&(len > 0)&&(ipdst != INADDR_ANY))
	{
		pkglen = sizeof(tagIcmpHdr)+sizeof(tagIcmpHdrEcho)+len;
		sndpkg = Pkg_Alloc(pkglen,CN_PKGFLAG_FREE);
		if(NULL != sndpkg)
		{
			seqno++;
			sndpkg->datalen = pkglen;
			sndpkg->partnext = NULL;
			//fill the snd pkg
			icmppkg = (tagIcmpHdr *)(sndpkg->buf + sndpkg->offset);
			icmppkg->type = EN_ICMP_ECHOREQUEST;
			icmppkg->code = CN_ICMP_ECHOREQUEST_CODE;
			icmppkg->chksum = 0;	
			icmppkgecho = (tagIcmpHdrEcho *)(&icmppkg->data[0]);
			icmppkgecho->seqno = htons(seqno);
			icmppkgecho->taskid = htons(Djy_MyEvttId());
			memcpy(&icmppkgecho->data[0], data, len);	
			//combin the task
			icmptaskecho.ipdst = ipdst;
			icmptaskecho.seqno = icmppkgecho->seqno;
			icmptaskecho.taskid = icmppkgecho->taskid;
			icmptask.type = EN_ICMP_ECHOREPLY;       //we expect the reply
			icmptask.code = CN_ICMP_ECHOREPLY_CODE;
			icmptask.data = (u8 *)&icmptaskecho;
			__Icmp_TaskAdd(&icmptask);			
			//ok, the pkg has been commpletted. now we snd it 
			sndresult = Ip_Send(INADDR_ANY,ipdst,sndpkg,pkglen,CN_IPTPL_ICMP,\
					CN_IPDEV_ICMPOCHKSUM,&icmppkg->chksum);				    
			//just wait for the echo, if false, it must be timeout
			result = Lock_SempPend(&icmptask.sync, CN_ICMP_ECHOTIMEOUT);
			__Icmp_TaskDel(&icmptask);	
			if(false == sndresult)
			{
				Pkg_PartFree(sndpkg);
			}
		}
	}

	return result;
}

// =============================================================================
// 函数功能：Icmp_MsgSnd
//          snd icmp msg
// 输入参数： ipsrc, the ip of the msg src
//          ipdst, the ip of the  msg dst
//          type, icmp type
//          code, icmp code
//          info, the info buf ptr
//          infolen, the len of the info
// 输出参数：
// 返回值  ：true , snd success while false failed 
// 说明    :
// =============================================================================
bool_t Icmp_MsgSnd(u32 ipsrc, u32 ipdst, u8 type, u8 code, u8 *info, u16 infolen)
{
	bool_t            result;
	u16               pkglen;
	tagNetPkg         *pkg2snd;
	tagIcmpHdr        *hdr;
	u8                *src;
	u8                *dst;
	result = false;
	pkglen = sizeof(tagIcmpHdr)+infolen;
	pkg2snd = Pkg_Alloc(pkglen,CN_PKGFLAG_FREE);
	if(NULL != pkg2snd)
	{
		pkg2snd->datalen = pkglen;
		pkg2snd->partnext = NULL;
		
		hdr = (tagIcmpHdr *)(pkg2snd->buf + pkg2snd->offset);
		hdr->type = type;
		hdr->code = code;
		hdr->chksum = 0;
		src = info;
		dst = &hdr->data[0];
		memcpy(dst, src, infolen);
		
		result = Ip_Send(ipdst,ipsrc,pkg2snd,pkg2snd->datalen,CN_IPTPL_ICMP,\
				CN_IPDEV_ICMPOCHKSUM,&hdr->chksum);
		if(false == result)
		{
			Pkg_PartFree(pkg2snd);
		}
	}
	
	return result;
}

// =============================================================================
// 函数功能：__Icmp_EchoReply
//          deal the echo reply pkg
// 输入参数：ipsrc, pkg source host
//          ipdst, pkg dst host
//          type, icmp type
//          code, icmp code
//          pkg, info for the specified type and code
// 输出参数：
// 返回值  ：true succees while false failed
// 说明    :Find any task in the queue,if any the set it,otherwise do nothing
//       anyway, free the pkg in 
// =============================================================================
bool_t __Icmp_EchoReply(u32 ipsrc, u32 ipdst,tagIcmpHdr *hdr,tagNetPkg *pkg)
{	
	bool_t result;
	u8 type;
	u8 code;
	
	type = hdr->type;
	code = hdr->code;
	__Icmp_TaskEchoActive(ipsrc, ipdst, type, code, pkg);
	result = Pkg_LstFlagFree(pkg);
	return result;
}
// =============================================================================
// 函数功能：__Icmp_EchoRequest
//          deal the icmp request pkg
// 输入参数：ipsrc, pkg source host
//          ipdst, pkg dst host
//          type, icmp type
//          code, icmp code
//          pkg, info for the specified type and code
// 输出参数：
// 返回值  ：true , request success while false timeout 
// 说明    :
// =============================================================================
bool_t __Icmp_EchoRequest(u32 ipsrc, u32 ipdst,tagIcmpHdr *hdr,tagNetPkg *pkglst)
{
	bool_t              result;
	u16                 pkglen;
	tagNetPkg           *pkg;
	tagNetPkg           *pkg2snd;
	u8                  *dst;
	u8                  *src;
	//result = false
	
	//no need to alloc new pkg, but use the pkg we rcved, and do some recongnize
	hdr->type = EN_ICMP_ECHOREPLY;
	hdr->code = CN_ICMP_ECHOREPLY_CODE;
	hdr->chksum = 0;
	pkglst->offset -= sizeof(tagIcmpHdr);
	pkglst->datalen +=sizeof(tagIcmpHdr);

	pkglen = 0;
	pkg = pkglst;
	while(NULL != pkg)
	{
		pkglen += pkg->datalen;
		pkg = pkg->partnext;
	}
	//MALLOC NEW PKG FOR SEND, BECAUSE THE OLD ONE MAY NOT ALIGNED
	pkg2snd =  Pkg_Alloc(pkglen,CN_PKGFLAG_FREE);
	if(NULL != pkg2snd)
	{
		//cpy the data to the new pkg
		pkglen = 0;
		pkg = pkglst;
		dst = (u8 *)(pkg2snd->buf + pkg2snd->offset);
		while(NULL != pkg)
		{
			pkglen = pkg->datalen;
			src = (u8 *)(pkg->buf + pkg->offset);
			memcpy(dst, src, pkglen);
			dst += pkglen;
			pkg2snd->datalen += pkglen;
			pkg = pkg->partnext;
		}
		
		hdr = (tagIcmpHdr *)(pkg2snd->buf + pkg2snd->offset);
		
		result = Ip_Send(ipdst,ipsrc,pkg2snd,pkg2snd->datalen,CN_IPTPL_ICMP,\
				CN_IPDEV_ICMPOCHKSUM,&hdr->chksum);
		if(false == result)
		{
			Pkg_PartFree(pkg2snd);
		}
	}
	
	//free the old mem
	Pkg_LstFlagFree(pkglst);

	return 	result;
}

// =============================================================================
// 函数功能：Icmp_RecvProcess
//          Icmp协议数据包处理
// 输入参数：ipsrc,源IP
//          ipdst,目的IP
//          pkglst,icmp数据包,存在重组包的可能
//          devfunc,网卡已经做得工作
// 输出参数：
// 返回值  ：true数据包已经被缓存或者释放， false数据包没有被缓存或者释放失败
// 说明    :
// =============================================================================
bool_t Icmp_RecvProcess(u32 ipsrc, u32 ipdst, tagNetPkg *pkglst, u32 devfunc)
{
	bool_t         result;
	tagIcmpHdr     *hdr;
	u16            chksum16; 

	if(0 == (devfunc&CN_IPDEV_ICMPICHKSUM))
	{
		//not check it yet, then we'd better got the chksum ourselves
		//if true the set the checksum true
		Ip_PkgLstChksum(pkglst,&chksum16);
		if(0 != chksum16)
		{
			result =Pkg_LstFlagFree(pkglst);
			return result;
		}
	}

	hdr = (tagIcmpHdr *)(pkglst->buf + pkglst->offset);
	pkglst->offset += sizeof(tagIcmpHdr);
	pkglst->datalen -=sizeof(tagIcmpHdr);
	switch(hdr->type)
	{
		case EN_ICMP_ECHOREQUEST:
			result = __Icmp_EchoRequest(ipsrc,ipdst,hdr,pkglst);
			break;
		case EN_ICMP_ECHOREPLY:
			result = __Icmp_EchoReply(ipsrc,ipdst,hdr,pkglst);
			break;
		default:
			result =Pkg_LstFlagFree(pkglst);
			break;
	}
	
	return result;
}
// =============================================================================
// 函数功能：Icmp_Init
//          ICMP协议初始化函数
// 输入参数：para,暂时无意义
// 输出参数：
// 返回值  ：true 成功 false失败
// 说明    :1,注册进IP层
//       2,注册进TPL层
// =============================================================================
bool_t Icmp_Init(ptu32_t para)
{
	return true;
}

