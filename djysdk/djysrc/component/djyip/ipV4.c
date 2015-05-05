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
// 模块描述: IPV4协议的实现
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 5:56:55 PM/Jul 11, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "endian.h"
#include "stdio.h"

#include "os.h"

#include "ipV4.h"
#include "link.h"
#include "tcpip_cdef.h"
#include "rout.h"

#define CN_IP_MTU_MIN       46     //少于该长度的IP数据包是要进行填充的
#define CN_IP_MTU_MAX       1500   //大于该长度的IP数据包要分片
#pragma pack(1)
typedef struct _tagIpv4Head
{
#if(CN_CFG_BYTE_ORDER == CN_CFG_BIG_ENDIAN)
  u8    version :4;         //IP VERSION
  u8    headlen :4;         //IP headlen
#else
  u8    headlen :4;         //IP headlen
  u8    version :4;         //IP VERSION
#endif
  u8	tos;				// type of service
  u16	len;		        // total length
  u16	id;			        // identification
#define CN_IP_DF_MSK	0x4000		// dont fragment flag
#define CN_IP_MF_MSK	0x2000		// more fragments flag
#define CN_IP_OFFMASK	0x1FFF	    // mask for fragmenting bits
  u16	fragment;	        // Flags and Fragment Offset
#define CN_IP_TTL_DEFALT	0x40
  u8	ttl;				// time to live
  u8	protocol;			// protocol
  u16	chksum;	            // IP header Checksum
  u32	ipsrc;		        // source IP address
  u32	ipdst;		        // destination IP address
  u8    option[0];          //IP选项
}tagIpv4Head;
#pragma pack()

//用于标记重组
typedef struct _tagIpv4Recombin
{
	struct tagIpReComBin *next;       //组织单向链表
	u32                  ipsrc;       //源IP
	u32                  ipdst;       //目的IP
	u16                  rebflags;    //重组标记
	u16                  offset;      //片偏移
	tagNetPkg            *pkg;        //待重组的数据包
}tagIpv4Recombin;

// =============================================================================
// 函数功能：__Ip_ChksumSoft16
//          IP软件校验
// 输入参数：len,本次校验数据的长度（单位为字节）
//           buf,待校验的数据
//           checksum,前次的校验和
//           done,本次是否是最后一次校验(最后一次校验要求和取反)
// 输出参数：无
// 返回值  ：生成的校验码
// 说明    ：16位求和取反校验码生成，内部调用，无需参数检测;确信至少是2字节对齐的
// =============================================================================
int __Ip_ChksumSoft16(void *buf,short len,int chksum, bool_t done)
{
	u16 *buf16;

	buf16 = (u16 *)buf;
	for(;len>1;len-=2)
	{
		chksum+=*buf16++;
	}
	if(len ==1)
	{
		chksum += htons((u16)(*(u8 *)buf16) << 8);
	}
	if (done)
	{
		chksum = (chksum>>16) + (chksum&0xFFFF);//将高16bit与低16bit相加
		chksum += (chksum >>16);//将进位到高位的16bit与低16bit 再相加

		return (u16)~chksum;
	}

	return chksum;
}
// =============================================================================
// 函数功能：Ip_ChksumSoft16
//          IP软件校验
// 输入参数：len,本次校验数据的长度（单位为字节）
//           buf,待校验的数据
//           chksum,前次的校验和
//           done,本次是否是最后一次校验(最后一次校验要求和取反)
// 输出参数：无
// 返回值  ：生成的校验码
// 说明    ：16位求和取反校验码生成，内部调用，无需参数检测
// =============================================================================
int Ip_ChksumSoft16(void *buf,u16 len,int chksum, bool_t done)
{
	int result = 0;
	if((NULL != buf)&&(len != 0))
	{
		result = __Ip_ChksumSoft16(buf, (short)len ,chksum, done);
	}
	return result;
}

// =============================================================================
// 函数功能：Ip_Send
//          IP数据包的发送函数
// 输入参数： ipsrc, 源IP,为0表示不指定IP,由链路自适应查找，一般的用作第一次发起通信
//           ipdst, 目的IP
//           pkg,传输层数据包
//           translen,传输长度
//           proto,传输层协议类型
//           waithook,如果链路不通，则等待的钩子，待链路通后会执行
//           hookpara,钩子的第一个参数，第二个参数由协议栈提供，true表示链路通，false表示超时还不通
//           checksumtype,需要校验的类型
//           u8 *checksum,存储校验码
// 输出参数：无
// 返回值  ：参考__EN_IP_SEND_RESULT的定义
// 说明    ：IP报文的发送
// =============================================================================
//ip标志
static u16 Ipv4Flg = 0;
//单独一个PKG
void __Ip_PkgChksum(tagNetPkg *pkg,u16 *chksum)
{
	u16                             chklen;
	int                             chksumtmp;
	void                            *chkbuf;

	chklen = pkg->datalen;
	chkbuf = (u8 *)(pkg->offset + pkg->buf);
	chksumtmp = __Ip_ChksumSoft16((void *)chkbuf, \
			            chklen,0, true);
	*chksum =(u16)chksumtmp;
	return;
}

//所有包裹的校验和
void Ip_PkgLstChksum(tagNetPkg *pkg,u16 *chksum)
{
	u16                             chklen;
	int                             chksumtmp;
	void                            *chkbuf;
	tagNetPkg                       *pkgtmp;

	chksumtmp = 0;
	//校验所有的PKG
	pkgtmp = pkg;
	do{
		chklen = pkgtmp->datalen;
		chkbuf = (u8 *)(pkgtmp->offset + pkgtmp->buf);
		if(NULL == pkgtmp->partnext)
		{
			chksumtmp = __Ip_ChksumSoft16((void *)chkbuf, \
					            chklen,chksumtmp, true);
			*chksum =(u16)chksumtmp;
		}
		else
		{
			chksumtmp = __Ip_ChksumSoft16((void *)chkbuf, \
					            chklen,chksumtmp, false);
		}
		pkgtmp = pkgtmp->partnext;
	}while(NULL != pkgtmp);
}

//所有包裹的校验和并且加上伪部首
void Ip_PkgLstChksumPseudo(u32 iplocal, u32 ipdst, u8  proto,tagNetPkg *pkg,u16 translen,u16 *chksum)
{
	u16                             chklen;
	int                             chksumtmp;
	void                            *chkbuf;
	tagPseudoHdr                    pseudohdr;   //用于校验的伪部首
	tagNetPkg                       *pkgtmp;
	
	pseudohdr.ipsrc = htonl(iplocal);
	pseudohdr.ipdst = htonl(ipdst);
	pseudohdr.zero = 0;
	pseudohdr.protocol = proto;
	pseudohdr.len = htons(translen);
	chksumtmp = __Ip_ChksumSoft16((void *)&pseudohdr, \
			                           sizeof(pseudohdr),0, false);
	//校验所有的PKG
	pkgtmp = pkg;
	do{
		chklen = pkgtmp->datalen;
		chkbuf = (u8 *)(pkgtmp->offset + pkgtmp->buf);
		if(NULL == pkgtmp->partnext)
		{
			chksumtmp = __Ip_ChksumSoft16((void *)chkbuf, \
					            chklen,chksumtmp, true);
			*chksum =(u16)chksumtmp;
		}
		else
		{
			chksumtmp = __Ip_ChksumSoft16((void *)chkbuf, \
					            chklen,chksumtmp, false);
		}
		pkgtmp = pkgtmp->partnext;
	}while(NULL != pkgtmp);
}

bool_t Ip_Send(u32 ipsrc, u32 ipdst, tagNetPkg *pkg,u16 translen,u8 proto,\
		       u32 devtask, u16 *chksum)
{

	bool_t                          result;
	u32                             iplocal;	 //通信的本地IP
	u32                             devfunc;     //链路层提供的功能
	tagRoutLink                     linkrout;    //链路
	tagNetPkg                       *ippkg;      //ip包
	tagIpv4Head                     *hdr;        //ip报文头

	result = false;
	//先看链路是否通，如果不通，就没有组包的必要了
	result = Rout_Resolve(ipdst, &linkrout);
	if(false == result)//链路通畅，组包发送吧
	{
		return result;
	}

	if(ipsrc != INADDR_ANY)
	{
		iplocal = ipsrc;
	}
	else
	{
		iplocal = linkrout.ip;
	}
	//解决掉上层传递需要网卡做的任务，如果网卡没有该能力，那么应该在IP层用软件解决
	devfunc = NetDev_GetDevFun(linkrout.dev);
	if((0 == (devfunc&devtask))&&(CN_IPDEV_NONE!=devtask))
	{
		//有任务但是网卡却办不到,那么就用软件办呗
		result = true;
		switch (devtask)
		{
			case  CN_IPDEV_TCPOCHKSUM:
				Ip_PkgLstChksumPseudo(iplocal,ipdst,proto,pkg,translen,chksum);
				break;
			case  CN_IPDEV_UDPOCHKSUM:
				Ip_PkgLstChksumPseudo(iplocal,ipdst,proto,pkg,translen,chksum);
				devtask = 0;
				break;
			case  CN_IPDEV_ICMPOCHKSUM:
				Ip_PkgLstChksum(pkg,chksum);
				break;
			default:
				result = false;
				break;
		}
		devtask = CN_IPDEV_NONE;
	}
	if(false == result)
	{
		return result;  //软件没有办好
	}
	//组装IP报文头，目前不支持IP报文头数据选项
	if((translen +20)< linkrout.dev->pkglen)
	{
		ippkg = Pkg_Alloc(sizeof(tagIpv4Head)+linkrout.dev->linklen,CN_PKGFLAG_FREE);
		if(NULL != ippkg)
		{
			ippkg->offset += linkrout.dev->linklen;
			//进行IP数据头的填充
			hdr = (tagIpv4Head *)(ippkg->buf + ippkg->offset);
			hdr->headlen = 5;
			hdr->version = 4;
			hdr->tos = 0;
			hdr->len = htons(translen + 20);
			hdr->id = htons(Ipv4Flg);
			Ipv4Flg++;
			hdr->fragment = htons(CN_IP_DF_MSK);
			hdr->ttl = 64;
			hdr->protocol = proto;
			hdr->chksum = 0;
			hdr->ipsrc = htonl(iplocal);
			hdr->ipdst = htonl(ipdst);
			//PKG填充
			ippkg->datalen = sizeof(tagIpv4Head);
			ippkg->partnext = pkg;
			//IP报文的校验码生成
			if(devfunc & CN_IPDEV_IPOCHKSUM)
			{
				devtask|=CN_IPDEV_IPOCHKSUM;
			}
			else
			{
				__Ip_PkgChksum(ippkg,&hdr->chksum);
			}
			//OK,组装成功，发送出去吧
			result= Link_Send(linkrout.dev,ippkg,devtask,\
							  linkrout.mac,EN_NET_PROTO_IP,(ptu32_t)NULL);
			if(false == result)
			{
				Pkg_PartFree(ippkg);
			}
		}
		else
		{
			result = false;
		}
	}
	else
	{
		result = false;   //--TODO, large TPL data, must be devided
	}
	
	return result;
}
// =============================================================================
// 函数功能：IP数据包分包的重组
// 输入参数：pkg，待重组的包
// 输出参数：无
// 返回值  ：重组好的包序列，如果还没有重组完毕则返回NULL
// 说明    ：
// =============================================================================
tagNetPkg * __Ip_PkgRecover()
{
	return NULL;
}


extern bool_t Icmp_RecvProcess(u32 ipsrc, u32 ipdst, tagNetPkg *pkglst, u32 devfunc);
extern bool_t Udp_RecvProcess(u32 ipsrc, u32 ipdst, tagNetPkg *pkg, u32 devfunc);
extern bool_t Tcp_RecvProcess(u32 ipsrc, u32 ipdst, tagNetPkg *pkg, u32 devfunc);
// =============================================================================
// 函数功能：IP数据包的处理函数
// 输入参数：pkg，传递给IP层的数据包
//          devfunc,网卡硬件的功能
// 输出参数：无
// 返回值  ：true pkg被缓存,不能被释放， false没有被缓存，需要自己释放
// 说明    ：
// =============================================================================
bool_t __Ip_Process(tagNetPkg *pkg,u32 devfunc)
{
	bool_t result;
	u8     tplproto;
	u32    headlen;
	u32    ipdst;
	u32    ipsrc;
	tagIpv4Head *hdr;
	tagNetPkg   *tplpkg;           //可能是组装后的数据包

	hdr = (tagIpv4Head *)(pkg->buf + pkg->offset);
	//进行数据校验，如果正确则没有必要继续处理，当然校验先看看硬件是否已经校验，
	//硬件没有校验则软件校验，否则不校验
	headlen = hdr->headlen*4;
	pkg->offset += headlen;
	pkg->datalen = ntohs(hdr->len) - headlen;
	if(0 == pkg->datalen)
	{
		result = Pkg_LstFlagFree(pkg);
		return result;
	}
	
	if(0 ==(devfunc &CN_IPDEV_IPICHKSUM))
	{
		//do the chksum
		if(0 != __Ip_ChksumSoft16((void *)hdr,headlen,0,true))
		{
			result = Pkg_LstFlagFree(pkg);
			return result;
		}
	}
	//看看是否是本机报文，这个需要更新IP
	ipdst = ntohl(hdr->ipdst);
	ipsrc = ntohl(hdr->ipsrc);
	if(false == Rout_IsLocalDst(ipdst)) 
	{
		result = Pkg_LstFlagFree(pkg);//TODO，转发功能
	}
	else
	{
		tplproto = hdr->protocol;
		hdr->fragment = ntohs(hdr->fragment);
		if((hdr->fragment & CN_IP_DF_MSK)||\
		  ((0==(hdr->fragment&CN_IP_MF_MSK))&&(0 == (hdr->fragment&CN_IP_OFFMASK))))
		{
			tplpkg = pkg;
		}
		else
		{	
			tplpkg = NULL;//TODO,分片重组功能
			result = Pkg_LstFlagFree(pkg); 
		}
		if(NULL != tplpkg)
		{
			switch(tplproto)
			{
				case CN_IPTPL_TCP:
					result =  Tcp_RecvProcess(ipsrc, ipdst,tplpkg, devfunc);
					break;
				case CN_IPTPL_UDP:
					result =  Udp_RecvProcess(ipsrc, ipdst,tplpkg, devfunc);
					break;
				case CN_IPTPL_ICMP:
					result =  Icmp_RecvProcess(ipsrc, ipdst,tplpkg, devfunc);
					break;
				default:
					result = false;
					break;
			}
			if(false == result)
			{
				result = Pkg_LstFlagFree(tplpkg);
			}
		}
	}

	return result;
}

static struct tagMsgQueue   *pgIpMsgBox =NULL;
typedef struct
{
	tagNetDev  *netdev;
	tagNetPkg  *pkglst;
}tagIpPostMsg;

// =============================================================================
// 函数功能：Ip_PostPkg,向IP层投递数据包信息
// 输入参数：netdev ,投递数据的网卡
//          pkglst数据包
// 输出参数：无
// 返回值  ：true 成功该数据包被成功处理;false,该数据包处理失败
// =============================================================================
bool_t Ip_PostPkg(tagNetDev *netdev, tagNetPkg *pkglst)
{
	bool_t result = false;
	tagIpPostMsg  msg;

	if((NULL != netdev)&&(NULL != pkglst))
	{
		msg.netdev = netdev;
		msg.pkglst = pkglst;
		result = MsgQ_Send(pgIpMsgBox,(u8 *)&msg,sizeof(tagIpPostMsg),\
			               CN_TIMEOUT_FOREVER,CN_MSGQ_PRIO_NORMAL);
	}

	return result;
}
// =============================================================================
// 函数功能：__Ip_RcvPkg,从消息箱中接收信息
// 输入参数：
// 输出参数：msg,接收到的信息
// 返回值  ：true,接收成功, false接收失败
// =============================================================================
bool_t __Ip_RcvPkg(tagIpPostMsg *msg)
{
	return MsgQ_Receive(pgIpMsgBox,(u8 *)msg,sizeof(tagIpPostMsg),\
		                CN_TIMEOUT_FOREVER);
}


// =============================================================================
// 函数功能：__Ip_ProcessTask,处理IP层的接收到的数据包
// 输入参数：
// 输出参数：无
// 返回值  ：该线程是IP数据处理的主线程,没有意外的情况是不会退出的
// =============================================================================
ptu32_t __Ip_ProcessTask(void)
{
	tagIpPostMsg msg;
	while(1)
	{
		if(__Ip_RcvPkg(&msg))
		{
			__Ip_Process(msg.pkglst,msg.netdev->devfunc);
		}
	}
}
#define CN_IPMSG_BOX   100          //缓存256条数据信息
// =============================================================================
// 函数功能：Ip_Init
//          IP模块初始化
// 输入参数：para,表示IP层有哪些功能
// 输出参数：无
// 返回值  ：目前无意义
// 说明    ：做一些IP模块的初始化，具体工作待定
// =============================================================================
bool_t Ip_Init(ptu32_t para)
{
	bool_t result = false;
	u16   evttid;

	pgIpMsgBox = MsgQ_Create(CN_IPMSG_BOX, sizeof(tagIpPostMsg),CN_MSGQ_TYPE_FIFO);
	if(NULL != pgIpMsgBox)
	{
		evttid= Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1, 
				(ptu32_t (*)(void))__Ip_ProcessTask,NULL, 0x800, "Ip_ProcessTask");
		if (evttid != CN_EVTT_ID_INVALID)
		{
			evttid = Djy_EventPop(evttid, NULL, 0, 0, 0, 0);
			if(evttid != CN_EVENT_ID_INVALID)
			{
				result = true;
			}
			else
			{
				printk("IP_INIT:pop the process task failed\n\r");
			}
		}
		else
		{
			printk("IP_INIT:create the process task failed\n\r");
		}

	}
	else
	{
		printk("IP_INIT:Create the msg box failed\n\r");
	}
	
	return result;
}

