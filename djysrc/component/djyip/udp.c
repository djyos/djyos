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
// 模块描述: Udp协议的实现，感觉最揪心的协议
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 5:55:35 PM/Jul 14, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 		 1.关于FIN信号的发出有认识误区，FIN表示尽量发送所有的数据而关闭链接，而不是必须保证
//         所有的数据发送完毕（收到确认），因此FIN信号的发送也遵守重传等机制,不会有专门
//         的FIN定时器而是复用重传定时器
//       2.发送了FIN信号之后还有可能重传以前发送的数据，因为FIN信号是作为数据发送的；
//         FIN发送时并没有确保以前发送的数据得到确认，因此很有可能会重传数据
//       3.报文头的合法与否都做统一的处理检测（SYN FIN RST的兼容检测问题）
//       4.RST报文做统一的处理
//       5.FIN标记的发送：需要发送FIN时，如shutdown wr或者CLOSE，如果发送队列为空，则直接
//         发送FIN，否则等待所有数据发送后，要么加载报文中发送，要么独立发送FIN报文
//       6,UNACK队列是按照从UNACK(循环号)从小到大的顺序排列

#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "endian.h"
#include "string.h"

#include "os.h"

#include "rout.h"
#include "tpl.h"
#include "ipV4.h"
#include "udp.h"
//define for the udp msg pkg len
#define CN_UDP_PKGMSGLEN      1472

//define for the register in the tpl layer
static tagTplProtoRscNode     *pgUdpProtoRsc;
static struct tagMutexLCB     *pgUdpSockQSync;
static tagSocket              *pgUdpSockQH;            //Udp sock head
static tagSocket              *pgUdpSockQT;            //Udp sock tail

//used for the port alloc
#define CN_UDP_PORT_TEMPBASE  0x8000
#define CN_UDP_PORT_INVALID   0x0
#define CN_UDP_PORT_LIMIT     0xFFFF
static  u16                   sgUdpPortCurFree = CN_UDP_PORT_TEMPBASE;

//used for manage the ucb mem
#define CN_UCB_POOLSIZE       5  //100对链接，差不多了，可以动态增加
static tagUCB                 sgUCBPool[CN_UCB_POOLSIZE];
static struct tagMemCellPool  *pgUCBPool = NULL;

#define CN_USESSION_POOLSIZE  5
static tagUdpTnode            sgUdpSessionPool[CN_USESSION_POOLSIZE];
static struct tagMemCellPool  *pgUdpSessionPool = NULL;

///////////////////////////FUNCTION METHOD//////////////////////////////////////
// =============================================================================
// 函数功能：__Udp_GetSysTime
//        获取系统时间
// 输入参数：无
// 输出参数：
// 返回值  ：s64 获取系统时间
// 说明    :
// =============================================================================
s64 __Udp_GetSysTime()
{
	return DjyGetTime();
}
// =============================================================================
// 函数功能：__Udp_InitCB
//       Init the controller
// 输入参数：ucb, the ucb to init
// 输出参数：
// 返回值  ：无
// 说明    :
// =============================================================================
void __Udp_InitCB(tagUCB *ucb)
{
    //fill all the space to zeros, then set some data what we need
    memset((void *)ucb, 0, sizeof(tagUCB));
    //set algorithm default
    //set the channelstate, permit the kernel receive and snd
    ucb->channelstate.krcv = 1;
    ucb->channelstate.ksnd = 1;
    //set the snd and receive buf limit to default
    ucb->sbuf.buflenlimit  = CN_UDP_BUF_LEN_lIMIT;
    ucb->rbuf.buflenlimit  = CN_UDP_BUF_LEN_lIMIT;
    ucb->lifetick = CN_UDP_TERMINAL_LIFE;
}
// =============================================================================
// 函数功能：__Udp_RstCB
//       UDP控制块的复位
// 输入参数：udp,待复位的ucb控制块
// 输出参数：
// 返回值  ：无
// 说明    :将UDP控制块恢复成其刚声请时的模样
// =============================================================================
void __Udp_RstCB(tagUCB *ucb)
{
	//release the receive buf
	if(NULL != ucb->rbuf.pkglsthead)
	{
	    Pkg_LstFlagFree(ucb->rbuf.pkglsthead);
	}
    ucb->rbuf.buflen = 0;
    ucb->rbuf.buflenlimit  = CN_UDP_BUF_LEN_lIMIT;
    ucb->rbuf.pkglsthead = NULL;
    ucb->rbuf.pkglsttail = NULL;
    ucb->rbuf.triglevel = 0;
	Lock_SempPost(ucb->rbuf.bufsync);
	//状态切换
	ucb->channelstate.arcv = 0;
	ucb->channelstate.asnd = 0;

	//set the socket invalid, avoiding the hook to snd some invalid msg
	ucb->owner->valid = false;
}

static tagUCB * sgDebugucb;    //used for ucb
// =============================================================================
// 函数功能：__Udp_NewCB
//          申请一个ucb控制块
// 输入参数：NULL
// 输出参数：
// 返回值  ：申请的ucb控制块
// 说明    :主要是空间分配，以及资源的初始化
// =============================================================================
tagUCB *__Udp_NewCB(void)
{
	tagUCB *ucb;
	//申请ucb内存资源
	ucb = Mb_Malloc(pgUCBPool, CN_TIMEOUT_FOREVER);
	if(NULL != ucb)
	{
		//init the ucb member
		__Udp_InitCB(ucb);
		//create two semphore for the rcv and sndbuf
		ucb->rbuf.bufsync = Lock_SempCreate(1,1,CN_SEMP_BLOCK_FIFO,NULL);
		if(NULL == ucb->rbuf.bufsync)
		{
			Mb_Free(pgUCBPool,ucb);
			ucb =NULL;
		}
		else
		{
			sgDebugucb = ucb;
		}
	}

	return ucb;
}
// =============================================================================
// 函数功能：__Udp_FreeCB
//          释放一个ucb控制块
// 输入参数：NULL
// 输出参数：
// 返回值  ：true成功 fasle失败
// 说明    :主要是空间分配
// =============================================================================
bool_t __Udp_FreeCB(tagUCB *ucb)
{
	Lock_SempDelete(ucb->rbuf.bufsync);
	Lock_SempDelete(ucb->sbuf.bufsync);
	Mb_Free(pgUCBPool,ucb);
	return true;
}
static tagUdpTnode*  sgDebugTnode; //used fo debug
// =============================================================================
// 函数功能：__Udp_NewTnode
//          new a Tnode
// 输入参数：void
// 输出参数：
// 返回值  ：tagUdpTnode
// 说明    :
// =============================================================================
tagUdpTnode* __Udp_NewTnode(void)
{
	tagUdpTnode *result = NULL;

	result =(tagUdpTnode *) Mb_Malloc(pgUdpSessionPool,CN_TIMEOUT_FOREVER);
	if(NULL != result)
	{
		result->connum = 0;
		result->connumlimit = CN_UDP_BUF_LEN_lIMIT;
		result->clientlst = NULL;
		sgDebugTnode = result;
	}

	return  result;
}

// =============================================================================
// 函数功能：__Udp_FreeTnode
//          free a Tnode
// 输入参数：tnode
// 输出参数：
// 返回值  ：true success while false failed
// 说明    :
// =============================================================================
bool_t __Udp_FreeTnode(tagUdpTnode* tnode)
{
	Mb_Free(pgUdpSessionPool,(void *)tnode);
	return true;
}
// =============================================================================
// 函数功能：  Udp_Socket
//         创建一个套接字
// 输入参数：  family,所属域，如AF_INET AF_LOCAL等
//        type,类型，如SOCK_STREAM SOCK_DGRAM等
//        protocal,协议类型，一般的默认为0
// 输出参数：
// 返回值  ：套接字号，-1表示创建失败
// 说明    ：套接字创建不成功，可能是指定协议没有找到或者内存不足
//        输入参数应该是没有任何意义的，因为套接字是靠着这些参数来找到该协议的
// =============================================================================
tagSocket * Udp_Socket(int family, int type, int protocal)
{
	tagSocket    *sockfd;

	sockfd = Socket_New();
	if(NULL != sockfd)
	{
		sockfd->laddr.port = CN_UDP_PORT_INVALID;//设置为无效端口号，排在队尾
		sockfd->proto = &pgUdpProtoRsc->proto;
		sockfd->appstate = EN_SOCKET_LOCAL_CREATE;
		//we don't add the socket to the queue now
		//when bind or connect,it will alloc valid port to the socket. then we will
		//add it to the socket queue
	}
	return sockfd;
}
// =============================================================================
// 函数功能：  __Udp_NewPort
//        Find an valid port to use
// 输入参数：  void
// 输出参数：  void
// 返回值  ：u16, the valid port to use, if CN_UDP_PORT_INVALID means could
//       not alloc the port dynamic
// 说明    ：
// =============================================================================
u16 __Udp_NewPort()
{
	int chktimes;
	int chktimeslimit;
	tagSocket *sock;
	u16 result = CN_UDP_PORT_INVALID;

	chktimes = 0;
	chktimeslimit = (int)(CN_UDP_PORT_LIMIT - CN_UDP_PORT_TEMPBASE);

	while(chktimes < chktimeslimit)
	{
		sock = pgUdpSockQH;
		while(NULL != sock)
		{
			if(sgUdpPortCurFree == sock->laddr.port)
			{
				sock =NULL;
			}
			else
			{
				if(NULL == sock->nxt)
				{
					//chk all the sock, and find that the sock is valid to use
					result = sgUdpPortCurFree;
				}
				else
				{
					sock = sock->nxt;
				}
			}
		}
		if(result != CN_UDP_PORT_INVALID)//find the valid port
		{
			//end the chk sume
			chktimes = chktimeslimit;
		}
		//add the free port
		if(sgUdpPortCurFree == CN_UDP_PORT_LIMIT)
		{
			sgUdpPortCurFree = CN_UDP_PORT_TEMPBASE;
		}
		else
		{
			sgUdpPortCurFree++;
		}
	}

	return result;
}
// =============================================================================
// 函数功能：  __Udp_ChkBindValid
//        to chk wheather we could bind the specified ip and port
// 输入参数：  ip, port
// 输出参数：
// 返回值  ：true, valid while false invalid
// 说明    ：
// =============================================================================
u16 __Udp_ChkBindValid(u32 ip, u16 port)
{

	bool_t result;
	tagSocket *sock;

	result = true;
	sock = pgUdpSockQH;
	while(NULL != sock)
	{
		if(port == sock->laddr.port)
		{
			if(ip == sock->laddr.ip)
			{
				result = false;
				sock =NULL;
			}
			else
			{
				sock = sock->nxt;
			}
		}
		else
		{
			sock = sock->nxt;
		}
	}
	return result;
}

// =============================================================================
// 函数功能：  Udp_Bind
//        为一个套接字绑定一个网络地址
// 输入参数： sockfd,待绑定的套接字
//        myaddr,指定的网络地址
//        addrlen,网络地址长度
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：失败一般的是因为该网络地址冲突（端口号）
// =============================================================================
int Udp_Bind(tagSocket *sockfd,struct sockaddr *addr, int addrlen)
{
	int  result;
	u32 ip;
	u16 port;
	struct sockaddr_in *sockaddrin;

	result = -1;

	if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
	{
		if(EN_SOCKET_LOCAL_CREATE == sockfd->appstate)
		{
			sockaddrin = (struct sockaddr_in *)addr;
			ip = sockaddrin->sin_addr.s_addr;
			ip = ntohl(ip);
			port = sockaddrin->sin_port;
			port = ntohs(port);
			if(Rout_IsLocalIp(ip))//是本地IP
			{
				Lock_MutexPend(pgUdpSockQSync, CN_TIMEOUT_FOREVER);
				if(CN_UDP_PORT_INVALID == port)
				{
					//alloc an new port and add the socket to the socket queue
					port = __Udp_NewPort();
					if(port == CN_UDP_PORT_INVALID)
					{
						printk("Tcp:bind:no new port to bind");
					}
					else
					{
						sockfd->laddr.port  = port;
						result = 0;
					}
				}
				else
				{
					//chk the specifyed ip and port valid
					if(__Udp_ChkBindValid(ip,port))
					{
						result = 0;
					}
				}
				if(0 == result)
				{
					//add the socket to the tcp socket queue
					sockfd->nxt = NULL;
					sockfd->appstate = EN_SOCKET_LOCAL_BIND;
					sockfd->laddr.ip = ip;
					sockfd->laddr.port = port;
					if(NULL ==pgUdpSockQT)
					{
						pgUdpSockQH = sockfd;
						sockfd->pre = NULL;
					}
					else
					{
						pgUdpSockQT->nxt = sockfd;
						sockfd->pre = NULL;
					}
					pgUdpSockQT = sockfd;
				}

				Lock_MutexPost(pgUdpSockQSync);
			}
		}
		Lock_MutexPost(sockfd->sync);
	}

	return  result;
}
// =============================================================================
// 函数功能：  Udp_Listen
//        指定套接字为监听状态
// 输入参数：  sockfd,待指定的套接字
//        backlog,允许的链接个数，默认的为5
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：失败一般的是因为重复指定
// =============================================================================
int Udp_Listen(tagSocket *sockfd, int backlog)
{
	int  result;
	tagUdpTnode *tnode;

	result = -1;
	if(NULL != sockfd)
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			if(EN_SOCKET_LOCAL_BIND == sockfd->appstate)
			{
				//create the casync
				sockfd->casync = Lock_SempCreate(1,0,CN_SEMP_BLOCK_FIFO,NULL);
				if(NULL != sockfd->casync)
				{
					tnode = __Udp_NewTnode();
					if(NULL != tnode)
					{
						if(0 != backlog)
						{
							tnode->connumlimit = backlog;
						}
						sockfd->appstate = EN_SOCKET_LOCAL_LISTEN;
						sockfd->socktype = EN_SOCKET_TREENODE;
						sockfd->cb = (void *)tnode;
						result = 0;
					}
					else
					{
						Lock_SempDelete(sockfd->casync);
						sockfd->casync = NULL;
					}
				}
			}
			Lock_MutexPost(sockfd->sync);
		}
	}
	return  result;
}

// =============================================================================
// 函数功能：__Udp_AceptNew
//        accept一个新链接
// 输入参数：  sockfd,服务器端套接字
// 输出参数：
// 返回值  ：链接的客户端套接字
// 说明    ：该函数仅仅用于在SOCKFD中查找一个新的未被accept的链接
//        至于阻塞之类的可以由使用者自己去控制
// =============================================================================
tagSocket *__Udp_AceptNew(tagSocket *sockfd)
{
	tagSocket    *result;
	tagSocket    *clientsock;
	tagUCB       *ucb;
	tagUdpTnode  *tnode;

	result = NULL;
	tnode = (tagUdpTnode *)sockfd->cb;
	clientsock = tnode->clientlst;
	if(tnode->connum2accept > 0)
	{
		while(NULL != clientsock)
		{
			ucb = (tagUCB *)clientsock->cb;
			if(0 == ucb->channelstate.open)
			{
				ucb->channelstate.open = 1;
				ucb->channelstate.arcv = 1;
				ucb->channelstate.asnd = 1;

				clientsock->status = CN_SOCKET_WRITEALBE|CN_SOCKET_READALBE;
				result = clientsock;
				clientsock = NULL;//结束循环
			}
			else
			{
				clientsock= clientsock->nxt;
			}
		}
		if(NULL != result)
		{
			tnode->connum2accept--;
			if(tnode->connum2accept == 0)
			{
				sockfd->status &=(~CN_SOCKET_ACCEPT);
				Multiplex_Set(sockfd->multiio, sockfd->status);
			}
		}
	}


	return result;
}
// =============================================================================
// 函数功能：  Udp_Accept
//        等待客户端的链接
// 输入参数：  sockfd,服务器端套接字
// 输出参数：  addr,连接的客户端地址
//        addrlen,地址长度
// 返回值  ：链接的客户端套接字
// 说明    ：出错一般是因为非阻塞而且没有连接的客户端，或者超出链接限制数
// =============================================================================
tagSocket *Udp_Accept(tagSocket *sockfd, struct sockaddr *addr, int *addrlen)
{
	tagSocket  	*result;
	tagUCB   *ucb;

	struct sockaddr_in  *addrin;

	result = NULL;
	if(NULL != sockfd)//参数检查
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			if(EN_SOCKET_LOCAL_LISTEN == sockfd->appstate) //make sure it is a server
			{
				result = __Udp_AceptNew(sockfd);
				if((NULL == result)&&(0 == sockfd->constate.noblock))
				{
					while(NULL == result)//一直等待
					{
						Lock_MutexPost(sockfd->sync);
						Lock_SempPend(sockfd->casync,CN_TIMEOUT_FOREVER);
						Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER);
						result = __Udp_AceptNew(sockfd);
					}
				}
				else
				{
					sockfd->errorno = EN_SOCKET_TRY_AGAIN;
				}
			}
			Lock_MutexPost(sockfd->sync);
		}
	}
	if((NULL != result)&&(NULL != addr) &&(NULL != addrlen))
	{
		//填充地址
		ucb = (tagUCB *)result->cb;
		addrin = (struct sockaddr_in *)addr;
		addrin->sin_family = AF_INET;
		addrin->sin_port = htons(ucb->raddr.port);
		addrin->sin_addr.s_addr = htonl(ucb->raddr.ip);
		*addrlen = sizeof(struct sockaddr);
	}
	return result;
}

// =============================================================================
// 函数功能：__Udp_MsgSend
//       snd the msg
// 输入参数：ucb,
//        msg,   the msg to snd
//        len,   the msg len
//        flags, msg flags
// 输出参数：
// 返回值  ：
// 说明    :just snd the msg, maybe the route is resolving,but
// =============================================================================
int __Udp_MsgSend(tagUCB *ucb, const void *msg, int len, int flags)
{
	bool_t          sndresult;
	int             result;
	u16             translen;
	u32             iplocal, ipremote;
	u16             portlocal, portremote;
	tagUdpHdr       *hdr;
	tagNetPkg       *pkg;
	u8              *src;
	u8              *dst;

	result = 0;
	if((len <= ucb->sbuf.buflenlimit)&&(1== ucb->channelstate.asnd))
	{
		iplocal = ucb->owner->laddr.ip;
		portlocal =  ucb->owner->laddr.port;
		ipremote = ucb->raddr.ip;
		portremote = ucb->raddr.port;
		src = (u8 *)msg;
		translen = 0;
		while(len > 0)
		{
			translen = len>CN_UDP_PKGMSGLEN?CN_UDP_PKGMSGLEN:len;
			pkg = Pkg_Alloc(translen + sizeof(tagUdpHdr),CN_PKGFLAG_FREE);
			if(NULL != pkg)
			{
				//do the udp header
				hdr = (tagUdpHdr *)(pkg->buf + pkg->offset);
				hdr->portdst = htons(portremote);
				hdr->portsrc = htons(portlocal);
				hdr->msglen = translen + sizeof(tagUdpHdr);
				hdr->msglen = htons(hdr->msglen);
				hdr->chksum = 0;
				//cpy the msg
				dst = (u8 *)hdr + sizeof(tagUdpHdr);
				memcpy(dst,src, translen);
				//ok, the msg has cpy to the pkg, then snd the pkg
				pkg->datalen = translen + sizeof(tagUdpHdr);
				sndresult = Ip_Send(iplocal, ipremote, pkg,translen,CN_IPTPL_UDP,\
						CN_IPDEV_UDPOCHKSUM,&hdr->chksum);
				if(sndresult)
				{
					//update the src and len
					result += translen;
					len-=translen;
					src = src + translen;
				}
				else
				{
					Pkg_PartFree(pkg);
					break;
				}
			}
			else
			{
				printk("Udp:Snd:No more mem--len = 0x%08x!\n\r",translen);
				break;  //no mem for the pkg
			}
		}
	}
	ucb->lifetick = CN_UDP_TERMINAL_LIFE;

	if(0 == result)
	{
		result = -1;
	}
	return result;
}
// =============================================================================
// 函数功能：  Udp_Connect
//        链接服务器
// 输入参数：  sockfd,客户端套接字
//        addr,连接的服务器地址
//        addrlen，地址长度
// 输出参数：
// 返回值  ：-1出错，0成功
// 说明    ：1,先判断该sockfd是否适合connect
//      2,如果该sockfd是新建的，则需分配端口号并添加到队列中，否则不必移动
//      3,创建对应的通信远端，设置好本地地址和远端地址，拷贝本地的通信属性
//      4,像远端发送一个SYN报文，开启SYN重传定时器
//      5,等待本地端consync信号的到来
// =============================================================================
int Udp_Connect(tagSocket *sockfd, struct sockaddr *serv_addr, int addrlen)
{
	int  result=-1;
	struct sockaddr_in *addrin;
	tagUCB   *ucb;

	if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
	{
		if(EN_SOCKET_LOCAL_CREATE == sockfd->appstate)
		{
			//没有被BIND,需要被重新分配端口号
			sockfd->laddr.port = __Udp_NewPort();
			if(CN_UDP_PORT_INVALID != sockfd->laddr.port)
			{
				//add the socket to the tcp sock queue
				Lock_MutexPend(pgUdpSockQSync, CN_TIMEOUT_FOREVER);
				sockfd->nxt = NULL;
				if(NULL == pgUdpSockQT)
				{
					pgUdpSockQH = sockfd;
				}
				else
				{
					pgUdpSockQT->nxt = sockfd;
				}
				pgUdpSockQT = sockfd;
				Lock_MutexPost(pgUdpSockQSync);
				sockfd->appstate= EN_SOCKET_LOCAL_BIND;
			}
		}
		if(EN_SOCKET_LOCAL_BIND == sockfd->appstate)
		{
			//这个正是我们需要的状态,创建ucb并进行通信
			ucb = (tagUCB *)__Udp_NewCB();
			if(NULL != ucb)
			{
				sockfd->cb = (void *)ucb;
				sockfd->socktype = EN_SOCKET_TERMINAL;
				ucb->owner = sockfd;
				addrin = (struct sockaddr_in *)serv_addr;
				ucb->raddr.port = ntohs(addrin->sin_port);
				ucb->raddr.ip   = ntohl(addrin->sin_addr.s_addr);
				ucb->channelstate.open = 1;
				ucb->channelstate.arcv = 1;
				ucb->channelstate.asnd = 1;
				sockfd->status =  CN_SOCKET_WRITEALBE;
				Multiplex_Set(sockfd->multiio, sockfd->status);
				result = 0;
			}
		}
		Lock_MutexPost(sockfd->sync);
	}
	return  result;
}

// =============================================================================
// 函数功能:Udp_Send
//         向套接字发送数据
// 输入参数:sockfd,目的套接字
//        msg,待发送的信息
//        len,信息长度
//        flags,一般为0
// 输出参数：
// 返回值  ：-1出错，否则返回发送字节数
// 说明    ：
// =============================================================================
int Udp_Send(tagSocket *sockfd, const void *msg, int len, int flags)
{
	int                result;
	tagUCB             *ucb;

	result = -1;
	if(EN_SOCKET_TERMINAL == sockfd->socktype)//其他条件则不必发送了
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			ucb = (tagUCB *)sockfd->cb;
			result = __Udp_MsgSend(ucb,msg,len,flags);

			Lock_MutexPost(sockfd->sync);
		}
	}

	return  result;
}
// =============================================================================
// 函数功能：  __Udp_CpyfRcvbuf
//        像ucb的sbuf拷贝数据
// 输入参数： ucb,待拷贝ucb
//        buf,待发送的信息
//        len,信息长度
// 输出参数：
// 返回值  ：-1出错，否则返回拷贝的字节数
// 说明    ：从ucb中拷贝数据，直到len ==0 或者 ucb的PKG为空为止
// =============================================================================
int __Udp_CpyfRcvbuf(tagUCB *ucb, void *buf, int len)
{
	int  result;
	int cpylen;
	u8   *cpybuf;
	u8   *srcbuf;
	tagNetPkg  *pkg;

	srcbuf = (u8 *)buf;
	result = 0;
	//可能会超负载，但是这不是关键
	//先看看看有没有没有填满的数据包来接纳我们的数据
	pkg = ucb->rbuf.pkglsthead;
	while((len >0) && (NULL != pkg))
	{
		cpylen = len > pkg->datalen?pkg->datalen:len;
		cpybuf = (u8 *)(pkg->buf + pkg->offset);
		memcpy(srcbuf, cpybuf,cpylen);
		pkg->offset += cpylen;
		pkg->datalen -= cpylen;
		ucb->rbuf.buflen -= cpylen;
		result +=cpylen;
		len-= cpylen;
		srcbuf+=cpylen;
		if(0==pkg->datalen)
		{
			//该PKG已经无用了，可以释放了
			ucb->rbuf.pkglsthead = pkg->partnext;
			if(NULL == ucb->rbuf.pkglsthead)
			{
				ucb->rbuf.pkglsttail = NULL;
			}
			Pkg_PartFlagFree(pkg);
			pkg = ucb->rbuf.pkglsthead;
		}
	}

	return result;
}
// =============================================================================
// 函数功能：  __Udp_RcvMsg
//        rcv msg from the ucb
// 输入参数： ucb,the ucb
//        buf,the buf to hold the msg
//        len,the buf len
//        flags,then rcv flags
// 输出参数：
// 返回值  ：-1出错，否则返回拷贝的字节数
// 说明    ：从ucb中拷贝数据，直到len ==0 或者 ucb的PKG为空为止
// =============================================================================
int __Udp_RcvMsg(tagUCB *ucb,void *buf, int len, int flags)
{
	int result;

	result = -1;
	if(ucb->rbuf.buflen > 0)
	{
		//OK, the rcv buf has data to read,so just cpy
		result = __Udp_CpyfRcvbuf(ucb,buf,len);
	}
	else
	{
        if((0 == ucb->channelstate.arcv)||(0 == ucb->channelstate.krcv))
		{
			//the local or the remote has shutdown the rcv channel
			result = 0;
			ucb->owner->errorno = EN_SOCKET_CHANNEL_SHUTDOWN_R;
		}
		else
		{
			//no data current, if block, we will wait here, else return -1
			if(ucb->owner->constate.noblock)
			{
				ucb->owner->errorno = EN_SOCKET_TRY_AGAIN;
			}
			else
			{
				Lock_MutexPost(ucb->owner->sync);
				//block here
				while(0 ==ucb->rbuf.buflen)
				{
					Lock_SempPend(ucb->rbuf.bufsync, CN_TIMEOUT_FOREVER);
				}
				Lock_MutexPend(ucb->owner->sync, CN_TIMEOUT_FOREVER);

				if(ucb->rbuf.buflen > 0)
				{
					result = __Udp_CpyfRcvbuf(ucb,buf,len);
				}
				else
				{
					if((0 == ucb->channelstate.arcv)||(0 == ucb->channelstate.krcv))
					{
						//the channel has been shutdown when we wait
						ucb->owner->errorno = EN_SOCKET_CHANNEL_SHUTDOWN_R;
						result = 0;
					}
				}
			}
		}
	}
    if(result > 0)
	{
        if((ucb->rbuf.buflen < ucb->rbuf.triglevel)&&\
                !((0 == ucb->channelstate.krcv)||\
                (0 == ucb->channelstate.arcv)))
		{
			//could not read any more
			ucb->owner->status &= (~CN_SOCKET_READALBE);
			Multiplex_Set(ucb->owner->multiio, ucb->owner->status);
		}
        if(0< ucb->rbuf.buflen)
		{
        	Lock_SempPost(ucb->rbuf.bufsync);
		}
	}

    return result;
}

// =============================================================================
// 函数功能：  Udp_Recv
//        读取接字数据
// 输入参数：  sockfd,目的套接字
//        flags,一般为0
// 输出参数：
//       buf,收到的信息
//       len,buf长度
// 返回值  ：-1出错，否则返回收到字节数
// 说明    ：     1,checkpara
//        2,看是否适合rcv
//        3,找到合适的通信远端
//        4,看是否有数据读
//        5,看是否是阻塞型，非阻塞型而又没有数据的话直接返回出错
//        6,读取数据后一定记得释放PKG
//        7,如果没有数据且已经收到对面的FIN后记得返回0而不是-1
//        8,暂时忽略flags标志
// =============================================================================
int Udp_Recv(tagSocket *sockfd, void *buf,int len, unsigned int flags)
{
	int  result;
	tagUCB    *ucb;

	result =-1;
	if(EN_SOCKET_TERMINAL == sockfd->socktype)//其他条件则不接收
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			ucb = (tagUCB *)sockfd->cb;
			result = __Udp_RcvMsg(ucb,buf,len,flags);

			Lock_MutexPost(sockfd->sync);
		}
	}
	return  result;
}
// =============================================================================
// 函数功能：  __Udp_NewTerminal
//        Create an new terminal socket
// 输入参数：iplocal, ipremote,portlocal,portremote
// 输出参数：
// 返回值  ：NULL failed else the created socket
// 说明    ：
// =============================================================================
tagSocket* __Udp_NewTerminal(u32 iplocal,u32 ipremote,u16 portlocal, u16 portremote)
{
	tagSocket     *result;
	tagUCB        *ucb;

	//新建立一个客户端
	result =Socket_New();
	if(NULL != result)
	{
		result->proto = &pgUdpProtoRsc->proto;
		result->appstate = EN_SOCKET_LOCAL_CREATE;
		ucb = __Udp_NewCB();
		if(NULL != ucb)
		{
			result->cb = (void *)ucb;
			result->socktype = EN_SOCKET_TERMINAL;
			ucb->owner = result;
			result->laddr.ip = iplocal;
			result->laddr.port = portlocal;
			ucb->raddr.ip = ipremote;
			ucb->raddr.port = portremote;
			ucb->channelstate.arcv =1;
			ucb->channelstate.asnd = 1;
		}
		else
		{
			Socket_Free(result);
			result = NULL;
		}
	}

	return result;
}

// =============================================================================
// 函数功能：  Udp_Sendto
//        向目的网络地址发送数据
// 输入参数：  sockfd,     目的套接字
//        msg,        待发送的信息
//        len,        信息长度
//        flags,      一般为0
//        addr,       目的网络地址
//        addrlen,    目的网络地址长度
// 输出参数：
// 返回值  ：-1出错，否则返回发送字节数
// 说明    ：
// =============================================================================
int Udp_Sendto(tagSocket *sockfd, const void *msg,int len, unsigned int flags,\
		      const struct sockaddr *addr, int addrlen)
{
	int                  result;
	u32                  ip;
	u16                  port;
	tagUCB               *ucb;
	tagUdpTnode          *tnode;
	struct sockaddr_in   *sockaddrin;
	tagSocket            *sock;
	tagSocket            *socktmp;

	result = -1;
	if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
	{
		sockaddrin = (struct sockaddr_in *)addr;
		ip = ntohl(sockaddrin->sin_addr.s_addr);
		port = ntohs(sockaddrin->sin_port);
		if(EN_SOCKET_TERMINAL == sockfd->socktype)
		{
			//this is an terminal,so we must check whether the addr maps the socket
			ucb = (tagUCB *)sockfd->cb;
			ucb->channelstate.open = 1;
			if((ucb->raddr.ip == ip)&&(ucb->raddr.port == port))
			{
				//this maps the addr, just snd it
				result = __Udp_MsgSend(ucb,msg,len,flags);
			}
		}
		else if(EN_SOCKET_TREENODE == sockfd->socktype)
		{
			//this is an treenode, if ca't find the ternimal, we'd better create it
			sock = NULL;
			tnode = (tagUdpTnode *)sockfd->cb;
			socktmp = tnode->clientlst;
			while(NULL != socktmp)
			{
				ucb = (tagUCB *)socktmp->cb;
				if((ucb->raddr.port == port)&&(ucb->raddr.ip == ip))
				{
					//find the client
					sock = socktmp;
					socktmp =NULL; //end the find
				}
				else
				{
					//chknxt
					socktmp = socktmp->nxt;
				}
			}
			if(NULL == sock)
			{
                //alloc an new socket,and add it to the tnode
				sock = __Udp_NewTerminal(sockfd->laddr.ip,ip,sockfd->laddr.port,port);
				sock->nxt = tnode->clientlst;
				if(NULL != tnode->clientlst)
				{
					tnode->clientlst->pre = sock;
				}
				tnode->clientlst = sock;
			}
			if(NULL != sock)
			{
				if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
				{
					ucb = (tagUCB *)sock->cb;
					result = __Udp_MsgSend(ucb,msg,len,flags);
					Lock_MutexPost(sock->sync);
				}
			}
		}
		else
		{
			//this must be an new create socket
			//1,alloc an cb
			//2,alloc a new port for this local terminal,and add it
			//  to the socket queue
			//3,snd the msg
			ucb = __Udp_NewCB();
			if(NULL != ucb)
			{
				sockfd->cb = (void *)ucb;
				sockfd->socktype = EN_SOCKET_TERMINAL;
				ucb->raddr.ip = ip;
				ucb->raddr.port = port;
				//alloc an new port and add the socket to the socket queue
				if(Lock_MutexPend(pgUdpSockQSync, CN_TIMEOUT_FOREVER))
				{
					port = __Udp_NewPort();
					if(CN_UDP_PORT_INVALID != port)
					{
						sockfd->laddr.port  = port;
						//add the socket to the tcp socket queue
						sockfd->nxt = NULL;
						sockfd->appstate = EN_SOCKET_LOCAL_BIND;
						sockfd->laddr.ip = INADDR_ANY;
						sockfd->laddr.port = port;
						if(NULL ==pgUdpSockQT)
						{
							pgUdpSockQH = sockfd;
							sockfd->pre = NULL;
						}
						else
						{
							pgUdpSockQT->nxt = sockfd;
							sockfd->pre = NULL;
						}
						pgUdpSockQT = sockfd;
					}
					Lock_MutexPost(pgUdpSockQSync);
				}
			}
			if((sockfd->socktype == EN_SOCKET_TERMINAL) &&\
			   (sockfd->appstate == EN_SOCKET_LOCAL_BIND))
			{
				//the cb and port are all created success, then snd
				result = __Udp_MsgSend(ucb,msg,len,flags);
			}
		}
		Lock_MutexPost(sockfd->sync);
	}
	return  result;
}
// =============================================================================
// 函数功能： Udp_Recvfrom
//           从目的网络地址读取数据
// 输入参数：  sockfd,目的套接字
//           flags,一般为0
//           addr,目的网络地址
//           addrlen,目的网络地址长度
// 输出参数：
//           buf,读取的数据
//           len,信息长度
// 返回值  ：-1出错，否则返回收到字节数
// 说明    ：
// =============================================================================
int Udp_Recvfrom(tagSocket *sockfd,void *buf, int len, unsigned int flags,\
		    struct sockaddr *addr, int *addrlen)
{
	int                  result;
	u32                  ip;
	u16                  port;
	tagUCB               *ucb;
	tagUdpTnode          *tnode;
	struct sockaddr_in   *sockaddrin;
	tagSocket            *sock;
	tagSocket            *socktmp;

	result = -1;
	if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
	{
		sockaddrin = (struct sockaddr_in *)addr;
		ip = ntohl(sockaddrin->sin_addr.s_addr);
		port = ntohs(sockaddrin->sin_port);
		if(EN_SOCKET_TERMINAL == sockfd->socktype)
		{
			//this is an terminal,so we must check whether the addr maps the socket
			ucb = (tagUCB *)sockfd->cb;
			ucb->channelstate.open = 1;
			if((ucb->raddr.ip == ip)&&(ucb->raddr.port == port))
			{
				//this maps the addr, just snd it
				result = __Udp_RcvMsg(ucb,buf,len,flags);
			}
			Lock_MutexPost(sockfd->sync);
		}
		else if(EN_SOCKET_TREENODE == sockfd->socktype)
		{
			//this is an treenode, if ca't find the ternimal, we'd better create it
			sock = NULL;
			tnode = (tagUdpTnode *)sockfd->cb;
			socktmp = tnode->clientlst;
			while(NULL != socktmp)
			{
				ucb = (tagUCB *)socktmp->cb;
				if((ucb->raddr.port == port)&&(ucb->raddr.ip == ip))
				{
					//find the client
					sock = socktmp;
					socktmp =NULL; //end the find
				}
				else
				{
					//chknxt
					socktmp = socktmp->nxt;
				}
			}
			if(NULL == sock)
			{
                //alloc an new socket
				sock = __Udp_NewTerminal(sockfd->laddr.ip,ip,sockfd->laddr.port,port);

				sock->nxt = tnode->clientlst;
				if(NULL != tnode->clientlst)
				{
					tnode->clientlst->pre = sock;
				}
				tnode->clientlst = sock;
			}

			Lock_MutexPost(sockfd->sync);
			if(NULL != sock)
			{
				if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
				{
					ucb = (tagUCB *)sock->cb;
					//snt the msg
					result = __Udp_RcvMsg(ucb,buf,len,flags);
					Lock_MutexPost(sock->sync);
				}
			}
		}
		else
		{
			//this must be an new create socket
			//1,alloc an cb
			//2,alloc a new port for this local terminal,and add it
			//  to the socket queue
			//3,snd the msg
			ucb = __Udp_NewCB();
			if(NULL != ucb)
			{
				sockfd->cb = (void *)ucb;
				sockfd->socktype = EN_SOCKET_TERMINAL;
				ucb->raddr.ip = ip;
				ucb->raddr.port = port;
				//alloc an new port and add the socket to the socket queue
				if(Lock_MutexPend(pgUdpSockQSync, CN_TIMEOUT_FOREVER))
				{
					port = __Udp_NewPort();
					if(CN_UDP_PORT_INVALID != port)
					{
						sockfd->laddr.port  = port;
						//add the socket to the tcp socket queue
						sockfd->nxt = NULL;
						sockfd->appstate = EN_SOCKET_LOCAL_BIND;
						sockfd->laddr.ip = INADDR_ANY;
						sockfd->laddr.port = port;
						if(NULL ==pgUdpSockQT)
						{
							pgUdpSockQH = sockfd;
							sockfd->pre = NULL;
						}
						else
						{
							pgUdpSockQT->nxt = sockfd;
							sockfd->pre = NULL;
						}
						pgUdpSockQT = sockfd;
					}
					Lock_MutexPost(pgUdpSockQSync);
				}
			}
			if((sockfd->socktype == EN_SOCKET_TERMINAL) &&\
			   (sockfd->appstate == EN_SOCKET_LOCAL_BIND))
			{
				//the cb and port are all created success, then snd
				result = __Udp_RcvMsg(ucb,buf,len,flags);
			}
			Lock_MutexPost(sockfd->sync);
		}
	}
	return  result;
}
// =============================================================================
// 函数功能：  __Udp_ShutdownRD
//        shutdown the socked  read channel
// 输入参数：  socket, the socket to be shutdown
// 输出参数：
// 返回值  ：0 success while -1 failed
// 说明    ：
// =============================================================================
int __Udp_ShutdownRD(tagSocket *sock)
{
	int result;
	tagUCB *ucb;

	result = -1;
	if(EN_SOCKET_TERMINAL == sock->socktype)
	{
		ucb = (tagUCB *)sock->cb;
		//clear the rcv buf and recombination queue
		//and set some specified flags at the same time
		ucb->channelstate.arcv = 0;//cant rcv any more
		//release the rcv and recombination queue
		if(NULL != ucb->rbuf.pkglsthead)
		{
		    Pkg_LstFlagFree(ucb->rbuf.pkglsthead);
		}
	    ucb->rbuf.buflen = 0;
	    ucb->rbuf.buflenlimit  = CN_UDP_BUF_LEN_lIMIT;
	    ucb->rbuf.pkglsthead = NULL;
	    ucb->rbuf.pkglsttail = NULL;
	    ucb->rbuf.triglevel = 0;
		Lock_SempPost(ucb->rbuf.bufsync);

		result = 0;
	}

	return result;
}
// =============================================================================
// 函数功能：  __Udp_ShutdownWR
//        shutdown the socked  snd channel
// 输入参数：  socket, the socket to be shutdown
// 输出参数：
// 返回值  ：0 success while -1 failed
// 说明    ：
// =============================================================================
int __Udp_ShutdownWR(tagSocket *sock)
{
	int result;
	tagUCB *ucb;

	result = -1;
	if(EN_SOCKET_TERMINAL == sock->socktype)
	{
		ucb = (tagUCB *)sock->cb;
		//clear the rcv buf and recombination queue
		//and set some specified flags at the same time
		ucb->channelstate.asnd = 0; //cant snd any more
	}

	return result;
}

// =============================================================================
// 函数功能：  Udp_Shutdown
//        关闭套接字
// 输入参数：  sockfd,目的套接字
//        how,关闭方法， 见SHUT_RD等定义
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：
// =============================================================================
int Udp_Shutdown(tagSocket *sock, u32 how)
{
	int    result;

	if(NULL != sock)
	{
		Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER);
		switch(how)
		{
			case SHUT_RD:
				result = __Udp_ShutdownRD(sock);
				break;
			case SHUT_WR:
				result = __Udp_ShutdownWR(sock);
				break;
			case SHUT_RDWR:
				result = __Udp_ShutdownRD(sock);
				if(result)
				{
					result = __Udp_ShutdownWR(sock);
				}
				break;
			default:
				result = -1;
				break;
		}
		Lock_MutexPost(sock->sync);
	}
	return  result;
}


// =============================================================================
// 函数功能：  Udp_Close
//        关闭套接字
// 输入参数：  sockfd,待关闭的套接字
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：1, if it is an terminal, then close the channel and set the app state to
//         close, when the tick comes,it will delete the socket
//      2, if it is an treenode, then set the app state to close, and set all the
//         hung socket that not open yet close, when the tick comes, if the treenode
//         with no socket on it, it will delete the treenode
//      3, if it is not sure yet, then set the app state close, when the tick comes
//         it will delete it
// =============================================================================
int Udp_Close(tagSocket *sock)
{
	int                  result;
	tagUCB               *ucb;
	tagUdpTnode          *tnode;
	tagSocket            *cs;

	result = 0;
	if(Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER))
	{
		if(EN_SOCKET_TERMINAL == sock->socktype)
		{
			sock->appstate = EN_SOCKET_LOCAL_CLOSE;
			ucb = (tagUCB *)sock->cb;
			ucb->channelstate.arcv = 0;
			ucb->channelstate.asnd = 0;
			ucb->channelstate.close = 1;
			ucb->lifetick = 0;
			//FREE THE RECV QUEUE
			if(NULL != ucb->rbuf.pkglsthead)
			{
			    Pkg_LstFlagFree(ucb->rbuf.pkglsthead);
			}
		    ucb->rbuf.buflen = 0;
		    ucb->rbuf.buflenlimit  = CN_UDP_BUF_LEN_lIMIT;
		    ucb->rbuf.pkglsthead = NULL;
		    ucb->rbuf.pkglsttail = NULL;
		    ucb->rbuf.triglevel = 0;
			Lock_SempPost(ucb->rbuf.bufsync);
		}
		else if(EN_SOCKET_TREENODE == sock->socktype)
		{
			sock->appstate = EN_SOCKET_LOCAL_CLOSE;
			tnode = (tagUdpTnode *)sock->cb;
			cs = tnode->clientlst;
			while(NULL != cs)
			{
				if(Lock_MutexPend(cs->sync,CN_TIMEOUT_FOREVER))
				{
					ucb = (tagUCB *)cs->cb;
					if(0 == ucb->channelstate.open)
					{
						cs->appstate = EN_SOCKET_LOCAL_CLOSE;
						ucb = (tagUCB *)cs->cb;
						ucb->channelstate.arcv = 0;
						ucb->channelstate.asnd = 0;
						ucb->lifetick = 0;
						//FREE THE RECV QUEUE
						if(NULL != ucb->rbuf.pkglsthead)
						{
						    Pkg_LstFlagFree(ucb->rbuf.pkglsthead);
						}
					    ucb->rbuf.buflen = 0;
					    ucb->rbuf.buflenlimit  = CN_UDP_BUF_LEN_lIMIT;
					    ucb->rbuf.pkglsthead = NULL;
					    ucb->rbuf.pkglsttail = NULL;
					    ucb->rbuf.triglevel = 0;
						Lock_SempPost(ucb->rbuf.bufsync);
					}
					Lock_MutexPost(cs->sync);
				}
                cs = cs->nxt;  //chk all the client
			}
		}
		else
		{
			sock->appstate = EN_SOCKET_LOCAL_CLOSE;
		}
		Lock_MutexPost(sock->sync);
	}

	return result;
}
// =============================================================================
// 函数功能：  __Udp_Sol_Socket
//           SOL_SOCKET选项处理
// 输入参数：  sockfd,待处理的套接字
//           optname,选项名字
//           optval,待设置参数
//           optlen,参数长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int __Udp_Sol_Socket(tagSocket *sockfd,int optname,const void *optval, int optlen)
{
	bool_t result;
	tagUCB *ucb;

	result = -1;
	switch(optname)
	{
		case SO_BROADCAST:
			break;
		case SO_DEBUG:
			break;
		case SO_DONTROUTE:
			break;
		case SO_ERROR:
			break;
		case SO_KEEPALIVE:
			break;
		case SO_LINGER:
			break;
		case SO_OOBINLINE:
			break;
		case SO_RCVBUF:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				ucb = (tagUCB *)sockfd->cb;
				if(*(int *)optval>0)
				{
					ucb->rbuf.buflenlimit = *(int *)optval;
					result = 0;
				}
			}
			break;
		case SO_SNDBUF:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				ucb = (tagUCB *)sockfd->cb;
				if(*(int *)optval>0)
				{
					ucb->sbuf.buflenlimit = *(int *)optval;
					result = 0;
				}
			}
			break;
		case SO_RCVLOWAT:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				ucb = (tagUCB *)sockfd->cb;
				if(*(int *)optval>0)
				{
					ucb->rbuf.triglevel = *(int *)optval;
					result = 0;
				}
			}
			break;
		case SO_SNDLOWAT:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				ucb = (tagUCB *)sockfd->cb;
				if(*(int *)optval>0)
				{
					ucb->sbuf.triglevel = *(int *)optval;
					result = 0;
				}
			}
			break;
		case SO_RCVTIMEO:
			break;
		case SO_SNDTIMEO:
			break;
		case SO_REUSERADDR:
			break;
		case SO_TYPE:
			break;
		case SO_BSDCOMPAT:
			break;
		case SO_NOBLOCK:
			if(*(int *)optval)
			{
				sockfd->constate.noblock = 1;
			}
			else
			{
				sockfd->constate.noblock = 0;
			}
			result = 0;
			break;
		default:
			break;
	}
	return result;
}
// =============================================================================
// 函数功能：  __Udp_IpProto_Ip
//           IPPROTO_IP选项处理
// 输入参数：         sockfd,待处理的套接字
//           optname,选项名字
//           optval,待设置参数
//           optlen,参数长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int __Udp_IpProto_Ip(tagSocket *sockfd,int optname,const void *optval, int optlen)
{
	bool_t result;

	result = -1;
	switch(optname)
	{
		case IP_HDRINCL:
			break;
		case IP_OPTIONS:
			break;
		case IP_TOS:
			break;
		case IP_TTL:
			break;
		default:
			break;
	}

	return result;
}
// =============================================================================
// 函数功能：  Udp_Setsockopt
//           设置套接字选项
// 输入参数：  sockfd,目的套接字
//           level,设置的层次，支持SOL_SOCKET、IPPROTO_TCP、IPPROTO_IP和IPPROTO_IPV6
//           optname,需设置选项
//           optval,缓冲区
//           optlen,缓冲区长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int Udp_Setsockopt(tagSocket *sockfd, int level, int optname,\
		       const void *optval, int optlen)
{
	int  result;

	result = -1;
	if((NULL != sockfd)&&(NULL != optval))
	{
		if(Lock_MutexPend(sockfd->sync,CN_TIMEOUT_FOREVER))
		{
			switch(level)
			{
				case SOL_SOCKET:
					result = __Udp_Sol_Socket(sockfd,optname,optval,optlen);
					break;
				case IPPROTO_IP:
					result = __Udp_IpProto_Ip(sockfd,optname,optval,optlen);
					break;
				case IPPROTO_TCP:
					break;
				default:
					//暂时不支持的选项层
					break;
			}
			Lock_MutexPost(sockfd->sync);
		}
	}
	return  result;
}
// =============================================================================
// 函数功能：  Udp_Getsockopt
//           设置套接字选项
// 输入参数：  sockfd,目的套接字
//           level,设置的层次例如，支持的层次有SOL_SOCKET、IPPROTO_TCP
//           optname,需设置选项
//           optval,缓冲区
//           optlen,缓冲区长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int Udp_Getsockopt(tagSocket *sockfd, int level, int optname, void *optval,\
		       int *optlen)
{
	int  result = -1;


	return  result;
}
////////////////////////状态机的处理////////////////////////////////////////////
// =============================================================================
// 函数功能：  __Udp_GetSocket
//          查找指定条件的套接字（通信实体）
// 输入参数：  iplocal,本地IP
//        ipremote,通信端IP
//        portlocal,本地port
//        portremote,远端port
// 输出参数：
// 返回值  ：符合条件的通信实体，如果没有通信实体，则返回通信节点，如果找不到对应的
//         通信节点，则返回NULL
// 说明    ：IP可以匹配也可以通配，当指定匹配的时候会尽力找到匹配的，找不到的话则返回通配的
//        通配的即IP = INADDR_ANY
//        换句话讲，IP是指定的话，不会返回PORT相同而IP不同的套接字
// =============================================================================
tagSocket *__Udp_GetSocket(u32 iplocal, u32 ipremote, u16 portlocal, u16 portremote)
{
	tagSocket *result = NULL;
	tagSocket *ltmp = NULL;
	tagSocket *rtmp = NULL;
	tagUCB *ucb;
	tagUdpTnode *tnode;

	ltmp = pgUdpSockQH;
	while(NULL != ltmp)
	{
		if(ltmp->laddr.port == portlocal)
		{
			if(EN_SOCKET_TERMINAL==ltmp->socktype)
			{
				//if the remote port and ip match, then we will return it
				//else, return NULL, which means there is no such sock
				ucb = ltmp->cb;
				if((ucb->raddr.port == portremote)&&(ucb->raddr.ip == ipremote))
				{
					result = ltmp;//this must be an local client socket
				}
			}
			else
			{
				//this is an server node, we chk if any sock hang on the ltmp
				//match it, if not, we will return the ltmp
				result = ltmp;
				tnode= (tagUdpTnode *)ltmp->cb;
				rtmp = tnode->clientlst;
				while(NULL != rtmp)
				{
					ucb = (tagUCB *)rtmp->cb;
					if((ucb->raddr.port == portremote)&&(ucb->raddr.ip == ipremote))
					{
						//just match the socket
						result = rtmp;
						rtmp = NULL;
					}
					else
					{
						//chk nxt;
						rtmp = rtmp->nxt;
					}
				}
			}
			//any way , we end the search for the socket
			ltmp = NULL;
		}
		else
		{
			//chk nxt
			ltmp = ltmp->nxt;
		}
	}
	return result;
}

// =============================================================================
// 函数功能：__Udp_ChksumRcvPkg
//        tcp校验码生成
// 输入参数： iplocal,本地端IP
//        ipremote,远端IP
// 输出参数：
// 返回值  ：true 校验通过 false校验失败
// 说明    :收到的TCP报文软校验
// =============================================================================
bool_t  __Udp_ChksumRcvPkg(u32 iplocal,u32 ipremote, tagNetPkg *pkg)
{
	bool_t  result;
	int     checksumpsedo;
	u16     checksum;
	void    *buf;
	u16     len;
	tagPseudoHdr  psedohdr;

	psedohdr.ipdst = htonl(iplocal);
	psedohdr.ipsrc = htonl(ipremote);
	psedohdr.len = htons(pkg->datalen);
	psedohdr.protocol = CN_IPTPL_UDP;
	psedohdr.zero = 0;

	buf = (void*)(&psedohdr);
	len = sizeof(psedohdr);
	checksumpsedo = Ip_ChksumSoft16(buf, len,0, false);

	buf = (void *)(pkg->buf + pkg->offset);
	len = pkg->datalen;
	checksum = Ip_ChksumSoft16(buf, len, checksumpsedo, true);

	if(0 == checksum)
	{
		result =true;
	}
	else
	{
		result = false;
	}


	return result;
}

// =============================================================================
// 函数功能：__Udp_DealPkg
//        deal the rcv data
// 输入参数： ucb, the udp control block
//        pkg, which hold the msg data
// 输出参数：
// 返回值  ：true :the pkg has been cached to the buf, else no cached
// 说明    :
// =============================================================================
bool_t __Udp_DealPkg(tagUCB *ucb, tagNetPkg *pkg)
{
	bool_t result;

	result = false;
	if((ucb->rbuf.buflen < ucb->rbuf.buflenlimit)&&(ucb->channelstate.arcv))
	{
		//ok ,add the data to the rbuf
		pkg->partnext = NULL;
		if(NULL == ucb->rbuf.pkglsttail)
		{
			ucb->rbuf.pkglsthead = pkg;
		}
		else
		{
			ucb->rbuf.pkglsttail->partnext = pkg;
		}
		ucb->rbuf.pkglsttail = pkg;
		ucb->rbuf.buflen+=pkg->datalen;

		if(ucb->rbuf.buflen > ucb->rbuf.triglevel)
		{
			if(0 ==(CN_SOCKET_READALBE & ucb->owner->status))
			{
				ucb->owner->status |= CN_SOCKET_READALBE;
				Multiplex_Set(ucb->owner->multiio, ucb->owner->status);
			}
			Lock_SempPost(ucb->rbuf.bufsync);
		}
		result = true;
	}

	ucb->lifetick = CN_UDP_TERMINAL_LIFE;

	return result;
}
// =============================================================================
// 函数功能：Udp_RecvProcess
//          TCP接收处理函数(处理IP层传递过来的数据包)
// 输入参数：ipsrc,源IP
//          ipdst,目的IP
//          pkg,udp数据包链
//          devfunc,网卡已经做的功能
// 输出参数：
// 返回值  ：true 成功 false失败
// 说明    :deal the pkg rcved for tcp
// =============================================================================
bool_t Udp_RecvProcess(u32 ipsrc, u32 ipdst, tagNetPkg *pkg, u32 devfunc)
{
	bool_t              result ;      //该数据包是否已经添加到套接字中
	u16                 portdst;
	u16                 portsrc;
	tagUdpHdr           *hdr;         //udp header register
	tagSocket           *ts;           //terminal socket
	tagSocket           *ss;           //socket found
	tagUCB              *ucb;          //udp control block
	tagUdpTnode         *tnode;        //server tree node
	tagNetPkg           *datapkg;      //pkg include the msg but not the hdr
	
	result = false;
	if((INADDR_ANY == ipdst) || (INADDR_ANY == ipsrc) || (NULL == pkg))
	{
		goto __RCVPKG_DEALEND;  //para error
	}
	hdr = (tagUdpHdr *)(pkg->buf + pkg->offset);
	if(0 == (devfunc&CN_IPDEV_UDPICHKSUM))
	{
		Ip_PkgLstChksumPseudo(ipdst, ipsrc, CN_IPTPL_UDP,\
				       pkg,htons(hdr->msglen),&hdr->chksum);
		if(0 != hdr->chksum)
		{
			goto __RCVPKG_DEALEND;  //chksum failed
		}
	}

	if(pkg->datalen > sizeof(tagUdpHdr))
	{
		pkg->datalen -= sizeof(tagUdpHdr);
		datapkg = pkg;
	}
	else
	{
		pkg->datalen = 0;
		datapkg = NULL;
	}
	pkg->offset += sizeof(tagUdpHdr);

	//ok,now we deal the pkg now, the pkg check success
	portdst = ntohs(hdr->portdst);
	portsrc = ntohs(hdr->portsrc);
	//有了IP和PORT,则可以找到通信的那个port了
	//查找本地SOCKET,并判断其状态，看是否合适再继续处理
	Lock_MutexPend(pgUdpSockQSync, CN_TIMEOUT_FOREVER);
	ss = __Udp_GetSocket(ipdst,ipsrc, portdst,portsrc);
	if((NULL != ss)&&Lock_MutexPend(ss->sync, CN_TIMEOUT_FOREVER))
	{
		Lock_MutexPost(pgUdpSockQSync);
		if((EN_SOCKET_TREENODE == ss->socktype)&&\
		   (EN_SOCKET_LOCAL_LISTEN == ss->appstate))
		{
			//which means that the server has no session with the remote
			//so create one and add it to the tnode
			tnode = (tagUdpTnode *)ss->cb;
			if(tnode->connum < tnode->connumlimit)
			{
                //alloc an new socket,and add it to the tnode
				ts = __Udp_NewTerminal(ipdst,ipsrc,portdst,portsrc);
				if(ts)
				{
					ts->nxt = tnode->clientlst;
					if(NULL != tnode->clientlst)
					{
						tnode->clientlst->pre = ts;
					}
					tnode->clientlst = ts;
					((tagUdpTnode*)ss->cb)->connum2accept++;
					Lock_SempPost(ss->casync);
					Lock_MutexPend(ts->sync, CN_TIMEOUT_FOREVER);
				}
			}
			Lock_MutexPost(ss->sync);
		}
		else if(EN_SOCKET_TERMINAL == ss->socktype)
		{
			ts = ss;  //ok, this is the terminal we searched
		}
		else
		{
			Lock_MutexPost(ss->sync);
		}
		if(ts)
		{
			ucb = (tagUCB*)ts->cb;
			if(NULL != datapkg)
			{
				result =__Udp_DealPkg(ucb, datapkg);
			}
			Lock_MutexPost(ts->sync);
		}
		else
		{
			//which means no local  udp port for this pkg,so icmp cant reached
		}
	}//end for fin an sock to deal the pkg
	else
	{
		Lock_MutexPost(pgUdpSockQSync);
		//which means no local  udp port for this pkg,so icmp cant reached
	}//主机不可达，RST报文

__RCVPKG_DEALEND:
	if(false == result)//该PKG未添加到套接字队列中去
	{
		//if the pkg not cached to the buf, then release it
		Pkg_LstFlagFree(pkg);
		result = true;
	}

	return true;
}
// =============================================================================
// 函数功能：__Udp_DeleteClient
//        删除本地客户端
// 输入参数：sock,
// 输出参数：
// 返回值  ：
// 说明    :仅仅是删除，不做任何的保护
// =============================================================================
void __Udp_DeleteClient(tagSocket *sock)
{
	tagUCB *ucb;
	ucb = (tagUCB *)sock->cb;
	sock = ucb->owner;
	if(NULL != sock->pre)
	{
		sock->pre->nxt = sock->nxt;
	}
	else
	{
		pgUdpSockQH = sock->nxt;
	}
	if(NULL != sock->nxt)
	{
		sock->nxt->pre = sock->pre;
	}
	else
	{
		pgUdpSockQT = sock->pre;
	}
	//reset and free the ucb
	__Udp_RstCB(ucb);
	__Udp_FreeCB(ucb);

	Socket_Free(sock);
}
// =============================================================================
// 函数功能：__Udp_DeleteServer
//        删除本地服务器
// 输入参数：待删除的socket
// 输出参数：
// 返回值  ：
// 说明    :仅仅是删除，不做任何的保护
// =============================================================================
void __Udp_DeleteServer(tagSocket *sock)
{

	if(NULL != sock->pre)
	{
		sock->pre->nxt = sock->nxt;
	}
	else
	{
		pgUdpSockQH = sock->nxt;
	}
	if(NULL != sock->nxt)
	{
		sock->nxt->pre = sock->pre;
	}
	else
	{
		pgUdpSockQT = sock->pre;
	}
	//reset and free the tnode
	__Udp_FreeTnode((tagUdpTnode *)sock->cb);

	Lock_SempDelete(sock->casync);
	Socket_Free(sock);
}
// =============================================================================
// 函数功能：__Udp_DeleteServerSession
//        删除本地服务器的一个会话
// 输入参数：待删除的服务器的一个会话ucb
// 输出参数：
// 返回值  ：
// 说明    :仅仅是删除，不做任何的保护
// =============================================================================
void __Udp_DeleteServerSession(tagUCB *ucb)
{
	tagSocket *sock;
	tagUdpTnode *tnode;

	sock = ucb->owner;
	tnode = (tagUdpTnode *)sock->hungnode->cb;

	if(NULL != sock->pre)
	{
		sock->pre->nxt = sock->nxt;
	}
	else
	{
		tnode->clientlst = sock->nxt;
	}
	if(NULL != sock->nxt)
	{
		sock->nxt->pre = sock->pre;
	}

	//update the tnode
	tnode->connum--;

	//reset and free the ucb
	__Udp_RstCB(ucb);
	__Udp_FreeCB(ucb);

	Socket_Free(sock);
}

// =============================================================================
// 函数功能：__Udp_DeleteSockNode
//        删除未知的socknode,本地端的
// 输入参数：待删除的socket
// 输出参数：
// 返回值  ：
// 说明    :仅仅是删除，不做任何的保护
// =============================================================================
void __Udp_DeleteSockNode(tagSocket *sock)
{
	if(NULL != sock->pre)
	{
		sock->pre->nxt = sock->nxt;
	}
	else
	{
		pgUdpSockQH = sock->nxt;
	}
	if(NULL != sock->nxt)
	{
		sock->nxt->pre = sock->pre;
	}
	else
	{
		pgUdpSockQT = sock->pre;
	}

	Lock_SempDelete(sock->casync);
	Socket_Free(sock);
}

// =============================================================================
// 函数功能：__Udp_DealUcbTimer
//        deal the udp timer,which only deal the tmp socket which created for the
//        server session
// 输入参数：ucb, udp control block
// 输出参数：
// 返回值  ：true the tcb to del while false not
// 说明    :
// =============================================================================
bool_t __Udp_DealUcbTimer(tagUCB *ucb)
{
	bool_t result;

	result = false;
	if(0 == ucb->channelstate.open)
	{
		//which not open yet,this is an tmp socket, so del it if timeout
		if((ucb->owner->appstate == EN_SOCKET_LOCAL_CLOSE)||\
			(ucb->lifetick ==0))
		{
			result = true;
		}
		else
		{
			ucb->lifetick--;
		}
	}
	else
	{
		if(1 == ucb->channelstate.close)
		{
			result = true;
		}
	}

	return result;
}
// =============================================================================
// 函数功能：Udp_Tick
//          TCP协议的定时器线程的处理函数
// 输入参数：无
// 输出参数：
// 返回值  ：无
// 说明    :
// =============================================================================
void Udp_Tick(void)
{
	tagSocket    *sockl1;
	tagSocket    *sockl1nxt;
	bool_t       sockl1del;
	tagSocket    *sockl2;
	tagSocket    *sockl2nxt;
	bool_t       sockl2del;
	tagUdpTnode  *tnode;
	tagUCB       *ucb;
	//this is an never return loop
	while(1)
	{
		Lock_MutexPend(pgUdpSockQSync, CN_TIMEOUT_FOREVER);
		sockl1 = pgUdpSockQH;
		while(NULL != sockl1)
		{
			//l1 layer
			Lock_MutexPend(sockl1->sync, CN_TIMEOUT_FOREVER);
			sockl1del = false;
			sockl1nxt = sockl1->nxt;
			if(EN_SOCKET_TREENODE == sockl1->socktype)
			{
				//this is an tree node, must chk its sub
				tnode = (tagUdpTnode *)sockl1->cb;
				sockl2 = tnode->clientlst;
				while(NULL != sockl2)
				{
					//l2 layer
					Lock_MutexPend(sockl2->sync, CN_TIMEOUT_FOREVER);
					sockl2del = false;
					sockl2nxt = sockl2->nxt;
					//do the server session timer deal
					ucb = (tagUCB *)sockl2->cb;
					sockl2del = __Udp_DealUcbTimer(ucb);

					if(false == sockl2del)
					{
						Lock_MutexPost(sockl2->sync);
					}
					else
					{
						__Udp_DeleteServerSession(ucb);
					}
					sockl2 = sockl2nxt;
				}
				//tree node has no timer except close
				if((sockl1->appstate == EN_SOCKET_LOCAL_CLOSE)&&\
				   (0 == tnode->connum))
				{
					__Udp_DeleteServer(sockl1);
					sockl1del = true;
				}
			}
			else if(EN_SOCKET_TERMINAL == sockl1->socktype)
			{
				//do the client timer deal
				if(sockl1->appstate == EN_SOCKET_LOCAL_CLOSE)
				{
					__Udp_DeleteClient(sockl1);
				}
			}
			else
			{
				//not certain yet, if closed ,then free it
				if(sockl1->appstate == EN_SOCKET_LOCAL_CLOSE)
				{
					__Udp_DeleteSockNode(sockl1);
					sockl1del = true;
				}
			}
			if(false == sockl1del)
			{
				Lock_MutexPost(sockl1->sync);
			}
			sockl1 = sockl1nxt;
		}
		Lock_MutexPost(pgUdpSockQSync);
		//the thread delay for the tcp tick
		Djy_EventDelay(1000*1000*mS);
	}
	return;
}
// =============================================================================
// 函数功能：Udp_Init
//          TCP协议初始化函数
// 输入参数：para,暂时无意义
// 输出参数：
// 返回值  ：true 成功 false失败
// 说明    :TCP的初始化的东西还是蛮多的
//       1,建立ucb内存池
//       2,建立定时器线程
//       3,建立发送线程
// =============================================================================
bool_t Udp_Init(ptu32_t para)
{

	u16  evttid;
	tagTlayerProto proto;
	//创建套接字资源节点锁
	pgUdpSockQSync = Lock_MutexCreate(NULL);
	if(NULL == pgUdpSockQSync)
	{
		printk("Udp:Create Local Mutex Failed\n\r");
		goto createlmutex_failed;
	}
	else
	{
		printk("Udp:Create Local Mutex  Success\n\r");
	}
	//建立ucb内存池
	pgUCBPool = Mb_CreatePool((void *)sgUCBPool, CN_UCB_POOLSIZE,\
			                     sizeof(tagUCB), 10, 0, NULL);
	if(NULL == pgUCBPool)
	{
		printk("Udp:Create UdpucbPool Failed\n\r");
		goto createucbpool_failed;
	}
	else
	{
		printk("Udp:Create UdpucbPool  Success\n\r");
	}
	//建立TNODE内存池
	pgUdpSessionPool = Mb_CreatePool((void *)sgUdpSessionPool, CN_USESSION_POOLSIZE,\
			                     sizeof(tagUdpTnode), 10, 0, NULL);
	if(NULL == pgUdpSessionPool)
	{
		printk("Udp:Create UdpTnodePool Failed\n\r");
		goto createtnodepool_failed;
	}
	else
	{
		printk("Udp:Create UdpTnodePool  Success\n\r");
	}
	//注册Udp到传输层，方便套接字使用
	proto.socket = Udp_Socket;
	proto.accept = Udp_Accept;
	proto.bind = Udp_Bind;
	proto.connect = Udp_Connect;
	proto.getsockopt = Udp_Getsockopt;
	proto.listen = Udp_Listen;
	proto.recv = Udp_Recv;
	proto.recvfrom = Udp_Recvfrom;
	proto.send = Udp_Send;
	proto.sendto = Udp_Sendto;
	proto.setsockopt = Udp_Setsockopt;
	proto.shutdown = Udp_Shutdown;
	proto.close = Udp_Close;
	proto.multiioadd = NULL;

	pgUdpProtoRsc = TPL_RegisterProto(AF_INET,SOCK_DGRAM,0, &proto);
	if(NULL == pgUdpProtoRsc)
	{
		printk("Udp:Register Udp in TPL Failed\n\r");
		goto resisterUdp_failed;
	}
	else
	{
		printk("Udp:Register Udp in TPL Success\n\r");
	}


	//建立定时器线程
    evttid= Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,
    		(ptu32_t (*)(void))Udp_Tick,NULL, 0x100, "UdpTick");
	if (evttid != CN_EVTT_ID_INVALID)
	{
		evttid = Djy_EventPop(evttid, NULL,0, NULL, NULL, 0);
	}
	else
	{
		printk("Udp:Register UdpTick Failed\n\r");
	}
	return true;

resisterUdp_failed:
	Mb_DeletePool(pgUdpSessionPool);
	pgUdpSessionPool = NULL;

createtnodepool_failed:
	Mb_DeletePool(pgUCBPool);
	pgUCBPool = NULL;

createucbpool_failed:
	Lock_MutexDelete(pgUdpSockQSync);
	pgUdpSockQSync = NULL;

createlmutex_failed:
	return false;
}



