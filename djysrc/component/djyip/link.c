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
// 模块描述: 链路层的实现
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 10:08:24 AM/Jul 4, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"

#include "os.h"

#include "link.h"
#include "ethernet.h"
extern bool_t Loop_Send(tagNetDev *netdev,tagNetPkg *pkg,u32 netdevtask,\
        u8 *macdst, u8 *macsrc ,u16 type, ptu32_t para);
// =============================================================================
// 函数功能：Link_Send
//          链路层发送函数
// 输入参数： netdev,发送所使用的网卡(必须)
//           type,网络层协议类型（必须）
//           pkg,待发送的网络层数据包（必须）
//           netdevtask,需要网卡完成的工作（IP TCP校验和生成等）
//           macdst根据netdev的属性看是否需要，如果是裸设备则不需要MAC
//           macsrc,由于dev已经指定，所以不用再指定源mac
//           para,根绝netdev的类型来确认
// 输出参数：无
// 返回值  ：
// 说明    ：该函数由网络层协议的发送函数调用
// =============================================================================
bool_t Link_Send(tagNetDev *netdev,tagNetPkg *pkg,u32 netdevtask,\
		           u8 *macdst,u16 type, ptu32_t para)
{
	bool_t result;
	u8     *macsrc;
	
	result = false;
	if((NULL != netdev)&&(NULL != pkg))
	{
		//交给具体的链路进行包装发送
		switch (netdev->iftype)
		{
			case  EN_LINK_INTERFACE_ETHERNET:
				macsrc = NetDev_GetDevMac(netdev);
				result = Ethernet_Send(netdev,pkg,netdevtask,macdst,macsrc,type,para);
				break;
			case  EN_LINK_INTERFACE_SLIP:
				break;
			case  EN_LINK_INTERFACE_LOOP:
				macsrc = NetDev_GetDevMac(netdev);
				result = Loop_Send(netdev,pkg,netdevtask,macdst,macsrc,type,para);
				break;
			case  EN_LINK_INTERFACE_RAW:
				break;
			default:
				break;
		}
	}
	return result;
}

//当网卡有数据需要向应用传递时，需要调用该函数实现数据的接收处理,该函数是由网卡驱动调用
extern bool_t Loop_Process(tagNetPkg *pkg, struct tagLinkProtoMsg *Msg);
extern bool_t Ip_PostPkg(tagNetDev *netdev, tagNetPkg *pkglst);
extern bool_t Arp_PostPkg(tagNetDev *netdev,tagNetPkg *pkglst,u8 *macdst, u8 *macsrc);

// =============================================================================
// 函数功能：Link_Process
//          链路层数据包接收入口
// 输入参数： netdev,接收的设备
//          pkg,接收到的数据包
// 输出参数：无
// 返回值  ：
// 说明    ：网卡驱动收到数据时候调用,只要数据包传递给上一层了，那么我们就无需管理该PKG
//         该PKG的释放等问题完全留给上一层去处理
// =============================================================================
bool_t Link_Process(tagNetDev *netdev,tagNetPkg *pkg)
{	
	bool_t result;
	struct tagLinkProtoMsg Msg;
	
	result = false;
	if((NULL != netdev)&&(NULL != pkg))
	{
		//交给具体的链路进行解析
		switch (netdev->iftype)
		{
			case  EN_LINK_INTERFACE_ETHERNET:
				result = Ethernet_Process(pkg,&Msg);
				break;
			case  EN_LINK_INTERFACE_SLIP:
				break;
			case  EN_LINK_INTERFACE_LOOP:
				result = Loop_Process(pkg,&Msg);
				break;
			case  EN_LINK_INTERFACE_RAW:
				break;
			default:
				break;
		}
		if(result)
		{
			switch(Msg.type)
			{
				case EN_NET_PROTO_IP:
					result = Ip_PostPkg(netdev,pkg);
					break;
				case EN_NET_PROTO_ARP:
					result = Arp_PostPkg(netdev,pkg,Msg.macdst,Msg.macsrc);
					break;
				default:
					result = false;
					break;
			}
		}
	}
	return result;
}

// =============================================================================
// 函数功能：Link_Init
//          链路层初始化
// 输入参数：无
// 输出参数：
// 返回值  ：true成功 false失败
// 说明    ：该层的初始话很简单，本层不涉及到资源的分配，因此只需将所有的网络协议处理
//         资源初始化为空即可
// =============================================================================
bool_t Link_Init(void)
{	
	return true;
}

