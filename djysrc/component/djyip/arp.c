//----------------------------------------------------
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
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
// 
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

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
// 模块描述: Arp协议实现
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 9:23:45 AM/Jul 7, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "endian.h"
#include "string.h"

#include "os.h"

#include "link.h"
#include "rout.h"
#include "arp.h"

//arp的数据报
#pragma pack(1)
typedef struct _tagArpHead 
{
#define CN_ARP_HW_TYPE_ETHERNET        0x01
	u16	hwtype;
#define CN_ARP_PR_TYPE_IP              0x0800
	u16	protype;
#define CN_ARP_HW_LEN_ETHERNET         0x06
	u8	hwaddrlen;
#define CN_ARP_PR_LEN_IP               0x04
	u8	proaddrlen;
#define CN_ARP_OP_REQUEST              0x01
#define CN_ARP_OP_RESPONSE             0x02
	u16	opcode;
	u8	senhwaddr[6];
	u8	senproaddr[4];
	u8	tarhwaddr[6];
	u8	tarproaddr[4];
	u8	rev_blank[18];	// 以太网上要求上层数据包至少有46字节，这18字节用于填充
}tagArpHead;
#pragma pack()
#define CN_ARP_HDRLEN   46         
// =============================================================================
// 函数功能：____Arp_Process_Request
//          处理收到的ARP请求包
// 输入参数：netdev, 收到ARP数据包的网卡
//          arp，待处理的arp数据
// 输出参数：
// 返回值  ：true处理成功，false处理失败
// 说明    :处理ARP请求数据包
// =============================================================================
bool_t ____Arp_Process_Request(tagNetDev *netdev,tagArpHead *arp)
{
	bool_t             result;
	u32                ipdst;
	tagNetPkg          *pkg;
	tagArpHead         *hdr; 
	u8                 *macsrc;
	u32                ipsrc;

	result = false;
	memcpy(&ipdst,arp->tarproaddr,4);
	ipdst = ntohl(ipdst);
	//如果目的请求就是我们的主机IP,则还必须回应该
	if(Rout_IsLocalIp(ipdst))
	{
		//更新RoutItem
		macsrc = arp->senhwaddr;
		memcpy(&ipsrc,arp->senproaddr,4);
		ipsrc = ntohl(ipsrc);
		Rout_ItemUpdate(ipsrc, macsrc); //ARP报文包含的ARP信息更新
		
		//response the arp request

		pkg = Pkg_Alloc(CN_ARP_HDRLEN+ netdev->linklen,CN_PKGFLAG_FREE);
		if(NULL != pkg) //分配不成功
		{
			//LEAVE SOME SPACE FOR THE LINK
			pkg->offset += netdev->linklen;
			//填写数据包
			//填充ARP报文
			hdr = (tagArpHead *)(pkg->buf + pkg->offset);
			memcpy(hdr->tarhwaddr, arp->senhwaddr, 6);
			memcpy(hdr->tarproaddr ,arp->senproaddr,4);
			memcpy(hdr->senproaddr , arp->tarproaddr,4);
			memcpy(hdr->senhwaddr,netdev->mac,6);
			hdr->hwtype = htons(CN_ARP_HW_TYPE_ETHERNET);
			hdr->protype = htons(CN_ARP_PR_TYPE_IP);
			hdr->hwaddrlen = CN_ARP_HW_LEN_ETHERNET;
			hdr->proaddrlen = CN_ARP_PR_LEN_IP;
			hdr->opcode = htons(CN_ARP_OP_RESPONSE);
			pkg->datalen = sizeof(tagArpHead);
			result = Link_Send(netdev,pkg,0,hdr->tarhwaddr,\
					           EN_NET_PROTO_ARP, (ptu32_t)NULL);
			if(false == result)
			{
				Pkg_PartFree(pkg);
			}
		}	
	
	}
	return result;	
}

// =============================================================================
// 函数功能：____Arp_Process_Response
//          处理收到的ARP响应包
// 输入参数：netdev, 收到ARP数据包的网卡
//          arp，待处理的arp数据
// 输出参数：
// 返回值  ：true处理成功，false处理失败
// 说明    :处理ARP响应数据包
// =============================================================================
bool_t ____Arp_Process_Response(tagNetDev *netdev,tagArpHead *arp)
{
	u8     *macsrc;
	u32     ipsrc;
	macsrc = arp->senhwaddr;
	memcpy(&ipsrc,arp->senproaddr,4);
	ipsrc = ntohl(ipsrc);
	Rout_ItemUpdate(ipsrc, macsrc); //ARP报文包含的ARP信息更新
	return true;
}
// =============================================================================
// 函数功能：__Arp_Process
//          处理收到的ARP包
// 输入参数：netdev, 收到ARP数据包的网卡
//          pkg，收到的ARP包
//          macdst,目的MAC
//          macsrc,源MAC
// 输出参数：
// 返回值  ：true处理成功，false处理失败
// 说明    ：目前只会处理以太网硬件的IP的操作
//           1,不论是ARP请求还是应答，必然会有发送端的ARP的MAC和IP，要ARP更新
//           2,如果是ARP请求，且目的IP是本地网卡IP，则ARP响应否则不响应
// =============================================================================
bool_t __Arp_Process(tagNetDev *netdev,tagNetPkg *pkg,u8 *macdst, u8 *macsrc)
{
	bool_t       result;
	tagArpHead   *hdr;
	u16          opcode;
	if((NULL != netdev)&& (NULL!= pkg) &&(NULL != macdst) &&(NULL != macsrc))
	{
		hdr = (tagArpHead *)(pkg->buf + pkg->offset);
		pkg->offset += sizeof(tagArpHead);
		pkg->datalen -= sizeof(tagArpHead);
		opcode = ntohs(hdr->opcode);
		switch (opcode)
		{
			case CN_ARP_OP_REQUEST:
				result = ____Arp_Process_Request(netdev,hdr);
				break;
			case CN_ARP_OP_RESPONSE:
				result = ____Arp_Process_Response(netdev,hdr);
				break;
			default:
				break;
		}
		Pkg_LstFlagFree(pkg);
		result = true;
	}
	return result;
}
bool_t Arp_PostPkg(tagNetDev *netdev,tagNetPkg *pkglst,u8 *macdst, u8 *macsrc)
{
	return __Arp_Process(netdev,pkglst,macdst,macsrc);
}

// =============================================================================
// 函数功能：ARP_Request
//          进行ARP请求
// 输入参数：netdev,使用的通信设备
//          ipdst,请求的IP对象
//          iplocal,请求的本地IP
// 输出参数：
// 返回值  ：true处理成功，false处理失败
// 说明    ：广播一个ARP请求报文
// =============================================================================
bool_t Arp_Request(tagNetDev *netdev,u32 ipdst, u32 iplocal)
{
	bool_t            result;
	tagNetPkg         *pkg;
	tagArpHead        *hdr;
	u8                broadmac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};//广播MAC地址
	
	result = false;
	if(NULL != netdev)
	{
		pkg = Pkg_Alloc(CN_ARP_HDRLEN+ netdev->linklen,CN_PKGFLAG_FREE);
		if(NULL != pkg) //分配不成功
		{
			//LEAVE SOME SPACE FOR THE LINK
			pkg->offset += netdev->linklen;
			//填写数据包
			hdr = (tagArpHead *)(pkg->buf + pkg->offset);
			hdr->hwtype = htons(CN_ARP_HW_TYPE_ETHERNET);
			hdr->protype = htons(CN_ARP_PR_TYPE_IP);
			hdr->hwaddrlen = CN_ARP_HW_LEN_ETHERNET;
			hdr->proaddrlen = CN_ARP_PR_LEN_IP;
			hdr->opcode = htons(CN_ARP_OP_REQUEST);
			
			ipdst = htonl(ipdst);
			memcpy(hdr->tarproaddr,&ipdst,4);
			iplocal = htonl(iplocal);
			memcpy(hdr->senproaddr,&iplocal,4);
			memcpy(hdr->senhwaddr,netdev->mac,6);
			pkg->datalen = sizeof(tagArpHead);
			result = Link_Send(netdev,pkg,0,broadmac, EN_NET_PROTO_ARP,(ptu32_t)NULL);
			if(false == result)
			{
				Pkg_PartFree(pkg);
			}
		}	
	
	}
	return result;
}


// =============================================================================
// 函数功能：Arp_Init
//          ARP模块初始化
// 输入参数：无
// 输出参数：无
// 返回值  ：true 成功  false 失败
// 说明    ：主要是向Link层注册ARP协议包的处理函数
// =============================================================================
bool_t Arp_Init(void)
{
	return true;
}


