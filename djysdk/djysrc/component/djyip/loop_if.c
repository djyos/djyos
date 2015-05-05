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
// 模块描述: LoopIf.c:
// 模块版本: V1.00
// 创建人员: Zqf
// 创建时间: 9:04:50 AM/Sep 18, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================

#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "string.h"

#include "os.h"

#include "netdev.h"
#include "rout.h"
#include "link.h"
#include "tcpip_cdef.h"

static u8   sgLoopMac[6]={0,0,0,0,0,0};
static u32  sgLoopIpAddr = 0x7F000001;//127.0.0.1
static u32  sgLoopIpMsk = 0xFFFFFF00 ; //255.255.255.0
static u32  sgLoopGateWay = 0x7F000001; //192.168.1.1

#define CN_LOOPPKG_LEN  8*1024    //8KB
#define CN_LOOPLINK_LEN 0

static struct tagMsgQueue  *pgLoopMsgQ = NULL;
static tagNetDev           *pgLoopNetDev = NULL;


// =============================================================================
// 函数功能：Loop_Send
//          本地发送函数
// 输入参数： netdev,发送所使用的网卡(必须)
//           type,网络层协议类型（必须）
//           pkg,待发送的网络层数据包（必须）
//           netdevtask,需要网卡完成的工作
//           macsrc和macdst根据netdev的属性看是否需要，如果是裸设备则不需要MAC
//           para,根绝netdev的类型来确认
// 输出参数：无
// 返回值  ：
// 说明    ：该函数由链路层调用
// =============================================================================
bool_t Loop_Send(tagNetDev *netdev,tagNetPkg *pkg,u32 netdevtask,\
		           u8 *macdst, u8 *macsrc ,u16 type, ptu32_t para)
{
	bool_t result;

	result = false;
	if((NULL == netdev) || (NULL == pkg)||(EN_NET_PROTO_IP != type))
	{
		Pkg_LstFlagFree(pkg);    //释放所有的pkg
	}
	else
	{
		result = NetDev_DevSend(netdev,pkg,netdevtask);
	}
	return result;
}

// =============================================================================
// 函数功能：Loop_Process
//          网卡传输一帧数据到协议栈
// 输入参数：pkg,链路层数据包
// 输出参数：无
// 返回值  ：true成功  false失败（无对应的网络协议或者处理失败）
// 说明    ：将一帧数据传递到链路层的以太网
// =============================================================================
bool_t Loop_Process(tagNetPkg *pkg, struct tagLinkProtoMsg *Msg)
{
	bool_t result = true;
	
	Msg->type = EN_NET_PROTO_IP;	
	Msg->macdst = NULL;
	Msg->macsrc = NULL;
	return result;
}
// =============================================================================
// 函数功能：__Loop_IfSnd
//          网卡发送数据包
// 输入参数：netdev,使用的网卡
//          pkg,待发送的数据包
//          netdevtask，需要网卡完成的工作，TCP IP校验和等
// 输出参数：
// 返回值  ：true发送成功  false发送失败。
// 说明    ：采用拷贝的方式发送，后续考虑使用链表发送
// =============================================================================
bool_t __Loop_IfSnd(tagNetDev *netdev,tagNetPkg *pkg,u32 netdevtask)
{
	bool_t  result;
	tagNetPkg *tmp;
	tagNetPkg *sndpkg;
	u8 *src;
	u8 *dst;
	u32 sndlen;
	
	result = false;
	if((pgLoopNetDev == netdev)&&(NULL != pkg))
	{
		sndlen = 0;
		
		//查看有多少数据并申请内存保存
		tmp = pkg;
		while(NULL != tmp)
		{
			sndlen += tmp->datalen;
			tmp = tmp->partnext;
		}
		sndpkg = Pkg_Alloc(sndlen,CN_PKGFLAG_FREE);
		if(NULL != sndpkg)
		{
			tmp = pkg;
			while(NULL != tmp)
			{
				src = (u8 *)(tmp->buf + tmp->offset);
				dst = (u8 *)(sndpkg->buf + sndpkg->offset + sndpkg->datalen);
				memcpy(dst, src, tmp->datalen);
				sndpkg->datalen += tmp->datalen;
				tmp = tmp->partnext;				
			}

			//发送信息呗
			result = MsgQ_Send(pgLoopMsgQ,(u8 *)&sndpkg,sizeof(void *),\
			               CN_TIMEOUT_FOREVER,CN_MSGQ_PRIO_NORMAL);
		}
		Pkg_LstFlagFree(pkg);
	}

	return result;
}

bool_t __Loop_IfRcvIntTask(void)
{
	tagNetPkg  *pkgsnd;
	while(1)
	{
		if(MsgQ_Receive(pgLoopMsgQ,(u8 *)&pkgsnd,sizeof(void *),\
		                CN_TIMEOUT_FOREVER))
		{
			if(false == NetDev_PostPkg(pgLoopNetDev,pkgsnd))
			{
				Pkg_LstFlagFree(pkgsnd);
			}
		}
	}
}

#define CN_LOOP_MSGQSIZE 0X10
// =============================================================================
// 函数功能：Loop_Init
//          向协议栈添加一个网卡设备
// 输入参数：无
// 输出参数：无
// 返回值  ：true成功  false失败。
// 说明    ：1,添加设备
//        2,添加路由
// =============================================================================
bool_t Loop_Init(void)
{
	bool_t  result = false;
	tagNetDevPara  devpara;
	tagHostIpv4Addr devaddr;
	u16 evttid;

	pgLoopMsgQ = MsgQ_Create(CN_LOOP_MSGQSIZE, sizeof(void *),CN_MSGQ_TYPE_FIFO);
	if(NULL != pgLoopMsgQ)
	{

		//create the loop rcv int task
		evttid= Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1, 
				(ptu32_t (*)(void))__Loop_IfRcvIntTask,NULL, 0x400, "loopdevrcvint_task");
		if (evttid != CN_EVTT_ID_INVALID)
		{
			evttid = Djy_EventPop(evttid, NULL, 0, 0, 0, 0);
			if(evttid != CN_EVENT_ID_INVALID)
			{
				devpara.ifsend = __Loop_IfSnd;
				devpara.iftype = EN_LINK_INTERFACE_LOOP;
				memcpy(devpara.mac, sgLoopMac,6); 
				devpara.name = "LoopIf";
				devpara.private = 0;
				devpara.linklen = CN_LOOPLINK_LEN;
				devpara.pkglen = CN_LOOPPKG_LEN;
				devpara.devfunc = CN_IPDEV_ALL;

				pgLoopNetDev = NetDev_AddDev(&devpara);
				if(NULL != pgLoopNetDev)
				{
					devaddr.ip = sgLoopIpAddr;
					devaddr.gateway = sgLoopGateWay;
					devaddr.ipmsk = sgLoopIpMsk;
					if(NULL != Rout_AddRout(pgLoopNetDev, &devaddr))
					{
						result = true;
					}
				}
			}
			else
			{
				printk("LOOP_INIT:pop the LOOP RCV task failed\n\r");
			}
		}
		else
		{
			printk("LOOP_INIT:create the LOOP_INIT RCV task failed\n\r");
		}

	}
	else
	{
		printk("LOOP_INIT: Create msg queue failed\n\r");
	}


	return result;
}

// =============================================================================
// 函数功能：Loop_Shell
//          向协议栈添加一个网卡设备
// 输入参数：无
// 输出参数：无
// 返回值  ：true成功  false失败。
// 说明    ：1,添加设备
//        2,添加路由
// =============================================================================
bool_t Loop_AddDevShell(char *param)
{
	bool_t result;
	
	result = Loop_Init();
	if(result)
	{
		printk("Loop Init Success!\n\r");
	}
	else
	{
		printk("Loop Init Failed\n\r");
	}
	return result;
}

