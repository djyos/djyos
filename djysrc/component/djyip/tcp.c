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
// 模块描述: TCP协议的实现，感觉最揪心的协议
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
#include "stdio.h"
#include "stddef.h"
#include "stdlib.h"
#include "endian.h"
#include "string.h"

#include "os.h"

#include "rout.h"
#include "tcp.h"
#include "tpl.h"
#include "ipV4.h"


//define for the register in the tpl layer
static tagTplProtoRscNode     *pgTcpProtoRsc;
static struct tagMutexLCB     *pgTcpSockTreeSync;
static tagSocket              *pgTcpSockLstH;            //tcp sock head
static tagSocket              *pgTcpSockLstT;            //tcp sock tail

//used for the port alloc
#define CN_TCP_PORT_TEMPBASE  0x8000
#define CN_TCP_PORT_INVALID   0x0
#define CN_TCP_PORT_LIMIT     0xFFFF
static  u16                   sgTcpPortCurFree = CN_TCP_PORT_TEMPBASE;

//used for manage the tcb mem
#define CN_TCP_TCBSIZE        5  //100对链接，差不多了，可以动态增加
static tagTcpTcb              sgTcpTcbPool[CN_TCP_TCBSIZE];
static struct tagMemCellPool  *pgTcpTcbPool = NULL;

#define CN_TCP_TNODESIZE      5
static tagTcpTnode            sgTcpTnodePool[CN_TCP_TNODESIZE];
static struct tagMemCellPool  *pgTcpTnodePool = NULL;

#define CN_TCP_RECOMB_SECNODE 100
static tagTcpReCombNode       sgTcpReCombNodePool[CN_TCP_RECOMB_SECNODE];
static struct tagMemCellPool  *pgTcpReCombNodePool = NULL;
static tagTcpReCombNode       *pgTcpReCombNodeQueueFree = NULL;
static u16                    sgTcpReCombNodeMemFree = 0;
static u16                    sgTcpReCombNodeMemUsed = 0;

//used for the connection to snd syn (server or client)  tcp option
static tagSynOption           sgSynOption;   //when snd syn,use this const option


bool_t __Tcp_Syn2Snd(tagTcpTcb *tcb);
///////////////////////////FUNCTION METHOD//////////////////////////////////////
// =============================================================================
// 函数功能：__Tcp_GetSysTime
//        获取系统时间
// 输入参数：无
// 输出参数：
// 返回值  ：s64 获取系统时间
// 说明    :
// =============================================================================
s64 __Tcp_GetSysTime()
{
	return DjyGetTime();
}

#define CN_TCP_UACKNODE_LIMIT 100
static tagTcpSndUnackNode     sgtTcpSndUnackNodeMem[CN_TCP_UACKNODE_LIMIT];
static tagTcpSndUnackNode     *pgTcpUackNodeQ = NULL;
static u32                    sgTcpUackNodeFreeNum;
static struct tagMemCellPool  *pgTcpUnackNodePool = NULL;
// =============================================================================
// 函数功能：__Tcp_UnackNodeInit
//         Init the UnackNode
// 输入参数：void
// 输出参数：
// 返回值  ：void
// 说明    :
// =============================================================================
void __Tcp_UnackNodeInit(void)
{
	//add all the free ack node to the queue
	pgTcpUackNodeQ = NULL;
	//create the recombination mem
	pgTcpUnackNodePool = Mb_CreatePool((void *)sgtTcpSndUnackNodeMem,\
						CN_TCP_UACKNODE_LIMIT,sizeof(tagTcpSndUnackNode), \
						CN_TCP_UACKNODE_LIMIT, CN_TCP_UACKNODE_LIMIT*100, NULL);
	if(NULL == pgTcpUnackNodePool)
	{
		printk("Tcp:UnackNode Init Failed!\n\r");
	}
	sgTcpUackNodeFreeNum = 0;
}

u32 UackAllocCalltimes = 0;
#define CN_UACKNODE_USEMASKS 0X5555
// =============================================================================
// 函数功能：__Tcp_AllocUnackedNode
//       used to alloc an node to hold the unacked pkg
// 输入参数：void
// 输出参数：void
// 返回值  ：the alloc node, while NULL means failed
// 说明    :
// =============================================================================
tagTcpSndUnackNode *__Tcp_AllocUnackedNode()
{
	atom_low_t atomop;
	tagTcpSndUnackNode *result;
	result = NULL;
	UackAllocCalltimes++;
	atomop = Int_LowAtomStart();
	if(NULL != pgTcpUackNodeQ)
	{
		result = pgTcpUackNodeQ;
		pgTcpUackNodeQ = result->nxt;
		result->nxt = NULL;
		result->masks = CN_UACKNODE_USEMASKS;
		sgTcpUackNodeFreeNum--;
		Int_LowAtomEnd(atomop);
	}
	else
	{
		Int_LowAtomEnd(atomop);
		result = Mb_Malloc(pgTcpUnackNodePool, CN_TIMEOUT_FOREVER);
		if(NULL != result)
		{
			result->nxt = NULL;
			result->masks = CN_UACKNODE_USEMASKS;
		}
	}
	return result;
}
u32 UackFreeCalltimes = 0;
#define CN_UACKNODE_FREEMASKS 0Xaaaa
// =============================================================================
// 函数功能：__Tcp_FreeUnackedNode
//       Free the node that hold the unacked pkg
// 输入参数：the node to free
// 输出参数：
// 返回值  ：void
// 说明    :
// =============================================================================
void __Tcp_FreeUnackedNode(tagTcpSndUnackNode *secbuf)
{
	atom_low_t atomop;
	UackFreeCalltimes++;
	//Free the unacked node to the lst
	atomop = Int_LowAtomStart();
	if(NULL != secbuf)
	{
		secbuf->masks = CN_UACKNODE_FREEMASKS;
		secbuf->nxt = pgTcpUackNodeQ;
		pgTcpUackNodeQ = secbuf;
		sgTcpUackNodeFreeNum++;
	}
	Int_LowAtomEnd(atomop);
	return ;
}
// =============================================================================
// 函数功能：__Tcp_AllocReCombNode
//       used to alloc an node to hold the recombination pkg
// 输入参数：void
// 输出参数：void
// 返回值  ：the alloc node, while NULL means failed
// 说明    :
// =============================================================================
tagTcpReCombNode *__Tcp_AllocReCombNode()
{
	atom_low_t atomop;
	tagTcpReCombNode *result;
	atomop = Int_LowAtomStart();
	if(NULL == pgTcpReCombNodeQueueFree)
	{
		Int_LowAtomEnd(atomop);
		result = Mb_Malloc(pgTcpReCombNodePool, CN_TIMEOUT_FOREVER);
		if(NULL != result)
		{
			sgTcpReCombNodeMemUsed++;
			result->nxt = NULL;
		}
		else
		{
			printk("Tcp:Alloc Recomb Node Failed\n\r");
		}
	}
	else
	{
		result = pgTcpReCombNodeQueueFree;
		pgTcpReCombNodeQueueFree = result->nxt;
		Int_LowAtomEnd(atomop);
		result->nxt = NULL;
		sgTcpReCombNodeMemFree--;
	}
	return result;
}
// =============================================================================
// 函数功能：__Tcp_FreeReCombNode
//       Free the node that hold the recombination pkg
// 输入参数：the node to free
// 输出参数：
// 返回值  ：void
// 说明    :
// =============================================================================
void __Tcp_FreeReCombNode(tagTcpReCombNode *secbuf)
{
	atom_low_t atomop;
	if(NULL!=secbuf)
	{
		//Free the unacked node to the lst
		atomop = Int_LowAtomStart();
		secbuf->nxt = pgTcpReCombNodeQueueFree;
		pgTcpReCombNodeQueueFree = secbuf;
		Int_LowAtomEnd(atomop);

		sgTcpReCombNodeMemFree++;
	}
	return ;
}
// =============================================================================
// 函数功能：__Tcp_InitTcb
//       Init the tcb controller
// 输入参数：tcb, the tcb to init
// 输出参数：
// 返回值  ：无
// 说明    :
// =============================================================================
void __Tcp_InitTcb(tagTcpTcb *tcb)
{
#define CN_RCV_WIN_DEFAULT     0x1000    //4KB
//定义用于重新生成的ISN号
    // 用于生成ISN的宏参数
    #define CN_TCP_ISNPRE_MS    (250)
    #define CN_ISN_PER_CONNECT  (64000)
    static u16 sIsnCnt = 0;    // 用于生成ISN的静态变量
    
    //fill all the space to zeros, then set some data what we need
    memset((void *)tcb, 0, sizeof(tagTcpTcb));

    //set state default
    tcb->state = EN_TCPSTATE_CREATED;            //set the state create
    //set algorithm default
    tcb->algorithm.nagle = 1;                    //open nagle
    //set the channelstate, permit the kernel receive and snd
    tcb->channelstate.krcv = 1;
    tcb->channelstate.ksnd = 1;
    
    //set the snd and receive buf limit to default
    tcb->sbuf.buflenlimit  = CN_TCP_BUF_LEN_lIMIT;
    tcb->rbuf.buflenlimit  = CN_TCP_BUF_LEN_lIMIT;

    //set the local mss to default
    tcb->lmss = CN_TCP_MSS_DEFAULT;   //默认的包大小

    //generate the first seqno to snd
    tcb->sndnxt = (u32)((sIsnCnt++)*CN_ISN_PER_CONNECT +
            DjyGetTime()*mS*CN_TCP_ISNPRE_MS); //发送数据后赋值,第一次算出一个值
    tcb->sndnxt = 0;
    //set the tcb window to default
    tcb->rcvwnd = CN_TCP_RCVWIN_DEFAULT;//默认的本地窗口的大小
    tcb->sndwnd = 0;
    tcb->sndwndscale = 0;

    //init the congestion window
    tcb->sa = CN_TCP_SA_INIT;
    tcb->sd = CN_TCP_SD_INIT;
    tcb->rto = tcb->sa + tcb->sd*2; //rto init:rto = A+2D
    tcb->cwnd = CN_TCP_MSS_DEFAULT*10;
    tcb->ssthresh = 65535;
}
// =============================================================================
// 函数功能：__Tcp_RstTcb
//          TCP控制块的复位
// 输入参数：tcb,待复位的TCB控制块
// 输出参数：
// 返回值  ：无
// 说明    :将TCB控制块恢复成其刚声请时的模样
// =============================================================================
void __Tcp_RstTcb(tagTcpTcb *tcb)
{
	tagTcpReCombNode   *recombnode;
	tagTcpSndUnackNode *unacknode;

	//release the snd buf
	if(NULL != tcb->sbuf.pkglsthead)
	{
	    Pkg_LstFree(tcb->sbuf.pkglsthead);
	}
	tcb->sbuf.buflen = 0;
    tcb->sbuf.buflenlimit  = CN_TCP_BUF_LEN_lIMIT;
    tcb->sbuf.pkglsthead = NULL;
    tcb->sbuf.pkglsttail = NULL;
    tcb->sbuf.triglevel = 0;
	Lock_SempPost(tcb->sbuf.bufsync);
	
	//release all the unacked queue
	unacknode = tcb->unacksechead;
	while(NULL != unacknode)
	{
		tcb->unacksechead = unacknode->nxt;
		Pkg_PartFree(unacknode->lsthead);
		__Tcp_FreeUnackedNode(unacknode);
		unacknode = tcb->unacksechead;
	}
    tcb->unacksechead = NULL;
    tcb->unacksectail = NULL;
	tcb->snduna = tcb->sndnxt;

	//release the receive buf
	if(NULL != tcb->rbuf.pkglsthead)
	{
	    Pkg_LstFlagFree(tcb->rbuf.pkglsthead);
	}
    tcb->rbuf.buflen = 0;
    tcb->rbuf.buflenlimit  = CN_TCP_BUF_LEN_lIMIT;
    tcb->rbuf.pkglsthead = NULL;
    tcb->rbuf.pkglsttail = NULL;
    tcb->rbuf.triglevel = 0;
	Lock_SempPost(tcb->rbuf.bufsync);
	
	//release the recombination queue
	recombnode = tcb->recomblst;
	while(NULL != recombnode)
	{
		tcb->recomblst = recombnode->nxt;
		Pkg_LstFlagFree(recombnode->lsthead);
		__Tcp_FreeReCombNode(recombnode);
		recombnode = tcb->recomblst;
	}
	tcb->recomblst = NULL;
	//状态切换
	tcb->channelstate.arcv = 0;
	tcb->channelstate.asnd = 0;
	tcb->channelstate.krcv = 0;
	tcb->channelstate.ksnd = 0;

	if(tcb->channelstate.close)
	{
		tcb->state = EN_TCPSTATE_2FREE;
	}
	else if(tcb->channelstate.open)
	{
		tcb->state = EN_TCPSTATE_CLOSED;
	}
	else
	{
		tcb->state = EN_TCPSTATE_2FREE;
	}
	//clear all the timer
	tcb->timerflag = 0;

	//set the socket invalid, avoiding the hook to snd some invalid msg
	tcb->owner->valid = false;
	
	printk("Tcp:Rst Tcb\n\r");
}

static tagTcpTcb * sgDebugTcb;    //used for tcb
// =============================================================================
// 函数功能：__Tcp_NewTcb
//          申请一个TCB控制块
// 输入参数：NULL
// 输出参数：
// 返回值  ：申请的TCB控制块
// 说明    :主要是空间分配，以及资源的初始化
// =============================================================================
tagTcpTcb *__Tcp_NewTcb(void)
{
	tagTcpTcb *tcb;
	//申请TCB内存资源
	tcb = Mb_Malloc(pgTcpTcbPool, CN_TIMEOUT_FOREVER);
	if(NULL != tcb)
	{
		//init the tcb member
		__Tcp_InitTcb(tcb);
		//create two semphore for the rcv and sndbuf
		tcb->sbuf.bufsync = Lock_SempCreate(1,1,CN_SEMP_BLOCK_FIFO,NULL);
		if(NULL == tcb->sbuf.bufsync)
		{
			Mb_Free(pgTcpTcbPool,tcb);
			tcb =NULL;
		}
		else
		{
			tcb->rbuf.bufsync = Lock_SempCreate(1,0,CN_SEMP_BLOCK_FIFO,NULL);
			if(NULL == tcb->rbuf.bufsync)
			{
				Lock_SempDelete(tcb->sbuf.bufsync);
				Mb_Free(pgTcpTcbPool,tcb);
				tcb =NULL;
			}
			else
			{
				sgDebugTcb = tcb;
			}
		}
	}

	return tcb;
}
// =============================================================================
// 函数功能：__Tcp_FreeTcb
//          释放一个TCB控制块
// 输入参数：NULL
// 输出参数：
// 返回值  ：true成功 fasle失败
// 说明    :主要是空间分配
// =============================================================================
bool_t __Tcp_FreeTcb(tagTcpTcb *tcb)
{
	Lock_SempDelete(tcb->rbuf.bufsync);
	Lock_SempDelete(tcb->sbuf.bufsync);
	Mb_Free(pgTcpTcbPool,tcb);
	return true;
}
static tagTcpTnode*  sgDebugTnode; //used fo debug
// =============================================================================
// 函数功能：__Tcp_NewTnode
//          new a Tnode
// 输入参数：void
// 输出参数：
// 返回值  ：tagTcpTnode
// 说明    :
// =============================================================================
tagTcpTnode* __Tcp_NewTnode(void)
{
	tagTcpTnode *result = NULL;
	
	result =(tagTcpTnode *) Mb_Malloc(pgTcpTnodePool,CN_TIMEOUT_FOREVER);
	if(NULL != result)
	{
		result->connum = 0;
		result->connum2accept = 0;
		result->connumlimit = CN_TCP_LISTEN_DEFAULT;
		result->clientlst = NULL;
		sgDebugTnode = result;
	}
	
	return  result;
}

// =============================================================================
// 函数功能：__Tcp_FreeTnode
//          free a Tnode
// 输入参数：tnode
// 输出参数：
// 返回值  ：true success while false failed
// 说明    :
// =============================================================================
bool_t __Tcp_FreeTnode(tagTcpTnode* tnode)
{
	Mb_Free(pgTcpTnodePool,(void *)tnode);
	return true;
}
// =============================================================================
// 函数功能：  Tcp_Socket
//         创建一个套接字
// 输入参数：  family,所属域，如AF_INET AF_LOCAL等
//        type,类型，如SOCK_STREAM SOCK_DGRAM等
//        protocal,协议类型，一般的默认为0
// 输出参数：
// 返回值  ：套接字号，-1表示创建失败
// 说明    ：套接字创建不成功，可能是指定协议没有找到或者内存不足
//        输入参数应该是没有任何意义的，因为套接字是靠着这些参数来找到该协议的
// =============================================================================
tagSocket * Tcp_Socket(int family, int type, int protocal)
{
	tagSocket    *sockfd;

	sockfd = Socket_New();
	if(NULL != sockfd)
	{
		sockfd->casync = Lock_SempCreate(1,0,CN_SEMP_BLOCK_FIFO,NULL);
		if(NULL != sockfd->casync)
		{
			sockfd->laddr.port = CN_TCP_PORT_INVALID;//设置为无效端口号，排在队尾
			sockfd->proto = &pgTcpProtoRsc->proto;
			sockfd->appstate = EN_SOCKET_LOCAL_CREATE;
			//we don't add the socket to the queue now
			//when bind or connect,it will alloc valid port to the socket. then we will
			//add it to the tcp socket queue
		}
		else
		{
			Socket_Free(sockfd);
			sockfd = NULL;
		}
	}
	return sockfd;
}
// =============================================================================
// 函数功能：  __Tcp_NewPort
//        Find an valid port to use
// 输入参数：  void
// 输出参数：  void
// 返回值  ：u16, the valid port to use in tcp, if CN_TCP_PORT_INVALID means could
//       not alloc the port dynamic
// 说明    ：
// =============================================================================
u16 __Tcp_NewPort()
{
	int chktimes;
	int chktimeslimit;
	tagSocket *sock;
	u16 result = CN_TCP_PORT_INVALID;

	chktimes = 0;
	chktimeslimit = (int)(CN_TCP_PORT_LIMIT - CN_TCP_PORT_TEMPBASE);

	while(chktimes < chktimeslimit)
	{
		sock = pgTcpSockLstH;
		while(NULL != sock)
		{
			if(sgTcpPortCurFree == sock->laddr.port)
			{
				sock =NULL;
			}
			else
			{
				if(NULL == sock->nxt)
				{
					//chk all the sock, and find that the sock is valid to use
					result = sgTcpPortCurFree;
				}
				else
				{
					sock = sock->nxt;
				}
			}
		}
		if(result != CN_TCP_PORT_INVALID)//find the valid port
		{
			//end the chk sume
			chktimes = chktimeslimit;
		}
		//add the free port
		if(sgTcpPortCurFree == CN_TCP_PORT_LIMIT)
		{
			sgTcpPortCurFree = CN_TCP_PORT_TEMPBASE;
		}
		else
		{
			sgTcpPortCurFree++;
		}
	}

	return result;
}
// =============================================================================
// 函数功能：  __Tcp_ChkBindValid
//        to chk wheather we could bind the specified ip and port
// 输入参数：  ip, port
// 输出参数：
// 返回值  ：true, valid while false invalid
// 说明    ：
// =============================================================================
u16 __Tcp_ChkBindValid(u32 ip, u16 port)
{

	bool_t result;
	tagSocket *sock;

	result = true;
	sock = pgTcpSockLstH;
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
// 函数功能：  Tcp_Bind
//        为一个套接字绑定一个网络地址
// 输入参数： sockfd,待绑定的套接字
//        myaddr,指定的网络地址
//        addrlen,网络地址长度
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：失败一般的是因为该网络地址冲突（端口号）
// =============================================================================
int Tcp_Bind(tagSocket *sockfd,struct sockaddr *myaddr, int addrlen)
{
	int  result;
	u32 ip;
	u32 port;
	struct sockaddr_in *sockaddrin;
	
	result = -1;
	if((NULL != sockfd)&& (NULL != myaddr)&&(NULL != pgTcpProtoRsc))
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			if(EN_SOCKET_LOCAL_CREATE == sockfd->appstate)
			{
				sockaddrin = (struct sockaddr_in *)myaddr;
				ip = sockaddrin->sin_addr.s_addr;
				ip = ntohl(ip);
				port = sockaddrin->sin_port;
				port = ntohs(port);
				if(Rout_IsLocalIp(ip))//是本地IP
				{
					Lock_MutexPend(pgTcpSockTreeSync, CN_TIMEOUT_FOREVER);
					if(CN_TCP_PORT_INVALID == port)
					{
						//alloc an new port and add the socket to the socket queue
						port = __Tcp_NewPort();
						if(port == CN_TCP_PORT_INVALID)
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
						if(__Tcp_ChkBindValid(ip,port))
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
						if(NULL ==pgTcpSockLstT)
						{
							pgTcpSockLstH = sockfd;
							sockfd->pre = NULL;
						}
						else
						{
							pgTcpSockLstT->nxt = sockfd;
							sockfd->pre = NULL;
						}
						pgTcpSockLstT = sockfd;
					}

					Lock_MutexPost(pgTcpSockTreeSync);
				}
			}
			Lock_MutexPost(sockfd->sync);
		}
	}
	return  result;
}
// =============================================================================
// 函数功能：  Tcp_Listen
//        指定套接字为监听状态
// 输入参数：  sockfd,待指定的套接字
//        backlog,允许的链接个数，默认的为5
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：失败一般的是因为重复指定
// =============================================================================
int Tcp_Listen(tagSocket *sockfd, int backlog)
{
	int  result;
	tagTcpTnode *tnode;
	
	result = -1;
	if(NULL != sockfd)
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			if(EN_SOCKET_LOCAL_BIND == sockfd->appstate)
			{
				tnode = __Tcp_NewTnode();
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
			}
			Lock_MutexPost(sockfd->sync);
		}
	}
	return  result;
}

// =============================================================================
// 函数功能：__Tcp_AceptNew
//        accept一个新链接
// 输入参数：  sockfd,服务器端套接字
// 输出参数：
// 返回值  ：链接的客户端套接字
// 说明    ：该函数仅仅用于在SOCKFD中查找一个新的未被accept的链接
//        至于阻塞之类的可以由使用者自己去控制
// =============================================================================
tagSocket *__Tcp_AceptNew(tagSocket *sockfd)
{

	tagSocket    *result;
	tagSocket    *clientsock;
	tagTcpTcb    *tcb;
	tagTcpTnode  *tnode;

	result = NULL;
	tnode = (tagTcpTnode *)sockfd->cb;
	clientsock = tnode->clientlst;
	if(tnode->connum2accept > 0)
	{
		while(NULL != clientsock)
		{
			tcb = (tagTcpTcb *)clientsock->cb;
			if((0 == (tcb->channelstate.open))&&\
			   (tcb->state == EN_TCPSTATE_ESTABLISHED))
			{
				tcb->channelstate.open = 1;
				tcb->channelstate.arcv = 1;
				tcb->channelstate.asnd = 1;

				clientsock->status = CN_SOCKET_WRITEALBE;
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
// 函数功能：  Tcp_Accept
//        等待客户端的链接
// 输入参数：  sockfd,服务器端套接字
// 输出参数：  addr,连接的客户端地址
//           addrlen,地址长度
// 返回值  ：链接的客户端套接字
// 说明    ：出错一般是因为非阻塞而且没有连接的客户端，或者超出链接限制数
// =============================================================================
tagSocket *Tcp_Accept(tagSocket *sockfd, struct sockaddr *addr, int *addrlen)
{
	tagSocket  	*result;
	tagTcpTcb   *tcb;

	struct sockaddr_in  *addrin;

	result = NULL;
	if(NULL != sockfd)//参数检查
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			if(EN_SOCKET_LOCAL_LISTEN == sockfd->appstate) //make sure it is a server
			{
				result = __Tcp_AceptNew(sockfd);
				if((NULL == result)&&(0 == sockfd->constate.noblock))
				{
					while(NULL == result)//一直等待
					{
						Lock_MutexPost(sockfd->sync);
						Lock_SempPend(sockfd->casync,CN_TIMEOUT_FOREVER);
						Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER);
						result = __Tcp_AceptNew(sockfd);
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
		tcb = (tagTcpTcb *)result->cb;
		addrin = (struct sockaddr_in *)addr;
		addrin->sin_family = AF_INET;
		addrin->sin_port = htons(tcb->raddr.port);
		addrin->sin_addr.s_addr = htonl(tcb->raddr.ip);
		*addrlen = sizeof(struct sockaddr);
	}
	return result;
}
// =============================================================================
// 函数功能：__Tcp_MsgSend
//       发送一个pkg
// 输入参数：tcb,待发送的tcb控制块
//        flag,tcp包头标志
//        opt,选项控制
//        optlen,选项长度
//        pkgdata,待发数据包
//        seqno,数据包的序列号
// 输出参数：
// 返回值  ：无
// 说明    :只负责发送数据包，不管TCB成员的更新
//        true,证明发送成功，false发送失败，或者正在寻找路由
// =============================================================================
bool_t __Tcp_MsgSend(tagTcpTcb *tcb,u8 flag,u8 *opt, u8 optlen,\
		          tagNetPkg *pkgdata, u32 seqno)
{
	bool_t result;
	tagNetPkg *pkg;
	tagTcpHdr   *hdr;
	tagSocket   *sockfd;
	u16 translen;
	u32 iplocal, ipremote;

	result = false;
	pkg = Pkg_Alloc(sizeof(tagTcpHdr)+optlen,CN_PKGFLAG_FREE);
	if(NULL != pkg)
	{
		hdr = (tagTcpHdr *)(pkg->offset + pkg->buf);
		sockfd = tcb->owner;
		iplocal = sockfd->laddr.ip;
		hdr->portsrc = htons(sockfd->laddr.port);
		ipremote = tcb->raddr.ip;
		hdr->portdst = htons(tcb->raddr.port);
		hdr->ackno = htonl(tcb->rcvnxt);
		hdr->seqno = htonl(seqno);
		hdr->flags = CN_TCP_FLAG_MSK & flag;
		hdr->reserv1 = 0;
		hdr->urgencyp = 0;//根本不会包含数据
		hdr->window = htons(tcb->rcvwnd);
		//-TODO, this because the rcv buf is full, so we must set this bit
		//       this action should be done in the rcv state,when the app
		//       read the rcvbuf, they will clear this bit and snd the window
		//       changged msg
		if(tcb->rcvwnd < tcb->mss)
		{
			tcb->rcvwndcongest = true;
		}
		hdr->chksum = 0;
		//opt拷贝，我们希望opt已经是网络端格式
		if(NULL != opt)
		{
			memcpy(hdr->opt,opt,optlen);
			hdr->hdrlen = (0x05 + (optlen + 3)/4);
		}
		else
		{
			memset(hdr->opt,0,optlen);
			hdr->hdrlen = 0x05;
		}
		//封包
		pkg->datalen = sizeof(tagTcpHdr) + optlen;
		translen = pkg->datalen;
		//将数据包添加在pkg包身后
		pkg->partnext = pkgdata;
		if(NULL != pkgdata)
		{
			translen += pkgdata->datalen;
		}
		//发送给IP层
		result = Ip_Send(iplocal, ipremote, pkg,translen,CN_IPTPL_TCP,\
					     CN_IPDEV_TCPOCHKSUM,&hdr->chksum);
	}
	return result;
}
// =============================================================================
// 函数功能：  Tcp_Connect
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
int Tcp_Connect(tagSocket *sockfd, struct sockaddr *serv_addr, int addrlen) 
{
	int  result=-1;
	struct sockaddr_in *addrin;
	tagTcpTcb   *tcb;
	
	if((NULL != sockfd) &&(NULL != serv_addr)&&(addrlen == sizeof(struct sockaddr_in)))
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			if(EN_SOCKET_LOCAL_CREATE == sockfd->appstate)
			{
				//没有被BIND,需要被重新分配端口号
				sockfd->laddr.port = __Tcp_NewPort();
				if(CN_TCP_PORT_INVALID != sockfd->laddr.port)
				{
					//add the socket to the tcp sock queue
					Lock_MutexPend(pgTcpSockTreeSync, CN_TIMEOUT_FOREVER);
					sockfd->nxt = NULL;
					if(NULL == pgTcpSockLstT)
					{
						pgTcpSockLstH = sockfd;
					}
					else
					{
						pgTcpSockLstT->nxt = sockfd;
					}
					pgTcpSockLstT = sockfd;
					Lock_MutexPost(pgTcpSockTreeSync);
					sockfd->appstate= EN_SOCKET_LOCAL_BIND;
				}
			}
			if(EN_SOCKET_LOCAL_BIND == sockfd->appstate)
			{
				//这个正是我们需要的状态,创建TCB并进行通信
				tcb = (tagTcpTcb *)__Tcp_NewTcb();
				if(NULL != tcb)
				{
					sockfd->cb = (void *)tcb;
					sockfd->socktype = EN_SOCKET_TERMINAL;
					tcb->owner = sockfd;
					addrin = (struct sockaddr_in *)serv_addr;
					tcb->raddr.port = ntohs(addrin->sin_port);
					tcb->raddr.ip   = ntohl(addrin->sin_addr.s_addr);
					//发送SYN报文
                    __Tcp_Syn2Snd(tcb);
                    
					Lock_MutexPost(sockfd->sync);
					Lock_SempPend(sockfd->casync, CN_TIMEOUT_FOREVER);//静等本地端链接成功后激活
					
					Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER);
					if(EN_TCPSTATE_ESTABLISHED == tcb->state)//因为被激活还肯能是对面RST我们了
					{
						tcb->channelstate.open = 1;
						tcb->channelstate.arcv = 1;
						tcb->channelstate.asnd = 1;
						sockfd->status =  CN_SOCKET_WRITEALBE;
						Multiplex_Set(sockfd->multiio, sockfd->status);
						result = 0;
					}
				}
			}
			Lock_MutexPost(sockfd->sync);
		}
	}

	return  result;
}
// =============================================================================
// 函数功能：  __Tcp_Cpy2Sndbuf
//        像TCB的sbuf拷贝数据
// 输入参数： tcb,待拷贝tcb
//        msg,待发送的信息
//        len,信息长度
// 输出参数：  
// 返回值  ：-1出错，否则返回拷贝的字节数
// 说明    ：保证我们的每个PKG都不超过MSS的大小，这样不用拆包
// =============================================================================
int __Tcp_Cpy2Sndbuf(tagTcpTcb *tcb, const void *msg, int len)
{
	int  result;
	int cpylen;
	u16 pkglen;
	u8   *cpybuf;
	u8   *srcbuf;
	tagNetPkg  *pkg;
	
	srcbuf = (u8 *)msg;
	result = 0;
	//可能会超负载，但是这不是关键
	//先看看看有没有没有填满的数据包来接纳我们的数据
	if(NULL != tcb->sbuf.pkglsttail)
	{
		pkg = tcb->sbuf.pkglsttail;
		pkglen = (pkg->bufsize-pkg->offset)>tcb->mss?tcb->mss:(pkg->bufsize-pkg->offset);
		cpylen = pkglen-pkg->datalen;
		if(cpylen > 0)
		{
			if(cpylen > len)
			{
				cpylen = len;
			}
			//还可以拷贝的数目
			cpybuf = (u8 *)(pkg->buf + \
				pkg->offset + pkg->datalen);
			memcpy(cpybuf, srcbuf,cpylen);
			len -= cpylen;
			srcbuf += cpylen;
			result += cpylen;
			pkg->datalen += cpylen;
			tcb->sbuf.buflen += cpylen;
		}
	}
	//如果还没有填充满，则需要不断的包进行填充
	while(len > 0)
	{
		//需要分配新的数据包来接纳数据
		//pkg都是mss标准的
		pkg = Pkg_Alloc(tcb->mss,CN_PKGFLAG_NULL);
		if(NULL == pkg)
		{
			printk("No mem for pkg alloc!\n\r");
			break;//内存资源不够呢
		}
		else
		{
			cpylen = tcb->mss>len?len:tcb->mss;
			//还可以拷贝的数目
			cpybuf = (u8 *)(pkg->buf + pkg->offset);
			memcpy(cpybuf, srcbuf,cpylen);
			len -= cpylen;
			srcbuf += cpylen;
			result += cpylen;
			pkg->datalen = cpylen;
			tcb->sbuf.buflen += cpylen;
			if(NULL == tcb->sbuf.pkglsttail)
			{
				tcb->sbuf.pkglsthead = pkg;
				tcb->sbuf.pkglsttail = pkg;
			}
			else
			{
				//将pkg挂在队尾即可
				tcb->sbuf.pkglsttail->partnext = pkg;
				tcb->sbuf.pkglsttail = pkg;
			}
		}
	}
	
	return result;
}
// =============================================================================
// 函数功能：__Tcp_ChkLen2Snd
//       检测可以发送的数据量
// 输入参数：tcb,待发的TCB
// 输出参数：
// 返回值  ：可以发送的字节数目
// 说明    ：表示目前的网络条件下还可以发送多少数据
// =============================================================================
int __Tcp_ChkLen2Snd(tagTcpTcb *tcb)
{
	int result;
	int sndwnd;
	int csndwnd;

	result = 0;
	sndwnd = tcb->sndwnd<<tcb->sndwndscale;
	csndwnd = tcb->cwnd;

	if(tcb->algorithm.nagle)
	{
		if(tcb->sbuf.buflen<tcb->mss)
		{
			if(NULL == tcb->unacksechead)
			{
				result = tcb->mss;
			}
		}
		else
		{
			result = csndwnd > sndwnd?sndwnd:csndwnd;
		}
	}
	else
	{
		result = csndwnd > sndwnd?sndwnd:csndwnd;
	}
	return result;
}
// =============================================================================
// 函数功能：__Tcp_Data2Snd
//        发送数据
// 输入参数：tcb,待发的TCB
// 输出参数：
// 返回值  ：true 成功  false 失败
// 说明    :合理处理，最好发送所有的数据
// =============================================================================
bool_t __Tcp_Data2Snd(tagTcpTcb *tcb)
{
	bool_t result;
	bool_t channelbad;
	u8  flag;
	u8  lastflag;  //the last pkg snt flag,maybe reffered to the state change
	u16  sndlen;
	int  sntlen;
	int  sndlenlimit;
	s64  sndtime;
	s64  sectimeout;
	tagNetPkg *pkgdata;
	tagTcpSndUnackNode *secbuf;
	//1,chk how many msg we could snd
	//2,snd the corresponding msg
	//3,update the tcb member when we snd the msg until unsucess or snd it all
	channelbad = false;
	sntlen = 0;
	lastflag = 0;

	sndlenlimit = __Tcp_ChkLen2Snd(tcb);
	sndtime = __Tcp_GetSysTime();
	sectimeout =sndtime +tcb->rto;
	//ok, we now try to snd all the snd buf if we could
	while(sndlenlimit>0)
	{
		//fetch one pkg from the sbuf to snd
		pkgdata = tcb->sbuf.pkglsthead;
		if(NULL == pkgdata)
		{
			break;//if there is no pkg
		}
		tcb->sbuf.pkglsthead = pkgdata->partnext;
		pkgdata->partnext = NULL;
		if(NULL == tcb->sbuf.pkglsthead)
		{
			tcb->sbuf.pkglsttail = NULL;
		}
		sndlen = pkgdata->datalen;
		//采集要发送的标记
		//处理发送标记
		flag = CN_TCP_FLAG_ACK;
		if(NULL == tcb->sbuf.pkglsthead)//就这么一包数据了
		{
			flag |= CN_TCP_FLAG_PSH;//最后一个包要标记PSH
			if(1== tcb->channelstate.fin)//需要发送FIN
			{
				flag |= CN_TCP_FLAG_FIN;
				//启动FIN重传定时器
				sndlen +=1;
			}
		}
		//OK,组包完毕，可以发送了
		result = __Tcp_MsgSend(tcb, flag, NULL, 0, pkgdata, tcb->sndnxt);
		if(result)
		{
			//update sntlen
			lastflag = flag;
			//add the pkgdata to the unack queque
			//because we used the timeoutforever,so we must got the buf when we return
			secbuf = __Tcp_AllocUnackedNode();
			pkgdata->partnext = NULL;
			secbuf->lsthead = pkgdata;
			secbuf->datalen = pkgdata->datalen;
//			secbuf->lsttail = pkgdata;
			secbuf->seqno = tcb->sndnxt;
			secbuf->nxt = NULL;
			secbuf->sndtime = sndtime;
			secbuf->timeout =sectimeout;
			secbuf->rsndtimes = 0;
			secbuf->flags = flag;
			if(NULL == tcb->unacksechead)
			{
				tcb->unacksechead = secbuf;
				tcb->unacksectail = secbuf;
			}
			else
			{
				tcb->unacksectail->nxt = secbuf;
				tcb->unacksectail = secbuf;
			}
			//must update when snd success
			tcb->sndnxt += sndlen;
			sndlenlimit-=sndlen;
			sntlen += sndlen;
		}
		else
		{
			//snd the data failed,so readd it to the snd queue
			pkgdata->partnext = tcb->sbuf.pkglsthead;
			tcb->sbuf.pkglsthead = pkgdata;
			if(NULL == tcb->sbuf.pkglsttail)
			{
				tcb->sbuf.pkglsttail = pkgdata;
			}
			channelbad = true;
			break;
		}

	}//end for while
	//see what we have done
	if(sntlen > 0)
	{
		//update some tcb member
		tcb->sbuf.buflen -= sntlen;
		sntlen = sntlen >>tcb->sndwndscale;
		if(tcb->sndwnd > sntlen)
		{
			tcb->sndwnd -= sntlen;
		}
		else
		{
			tcb->sndwnd = 0;
		}
		//we have snt some data
		if((tcb->sbuf.buflenlimit - tcb->sbuf.buflen)>tcb->sbuf.triglevel)
		{
			if(0 == (tcb->owner->status & CN_SOCKET_WRITEALBE))
			{
				tcb->owner->status |= CN_SOCKET_WRITEALBE;
				Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
			}
			Lock_SempPost(tcb->sbuf.bufsync);
		}
		//处理标记，如果发送成功，则还要看看是否消去FIN
		if(lastflag & CN_TCP_FLAG_FIN)
		{
			//发送了FIN，状态切换
			if(EN_TCPSTATE_CLOSE_WAIT == tcb->state)
			{
				tcb->state = EN_TCPSTATE_LAST_ACK;
			}
			else
			{
				tcb->state = EN_TCPSTATE_FIN_WAITE1;
			}
		}

        //turn off the ack timer
        tcb->timerflag &= (~CN_TCP_TIMER_ACK);
		//turn on the resnd timer
		tcb->timerflag |= CN_TCP_TIMER_RETRANSMIT;
		//if the sndwindow is zero, then we'd better to open the persist timer
		if((tcb->sndwnd < tcb->mss)||channelbad)
		{
			tcb->timerflag |= CN_TCP_TIMER_PERSIST;
			tcb->persistimer = CN_TCP_TICK_PERSIST;
		}
		result = true;
	}
	else
	{
		//the window or the rout is error, so probe any time
		tcb->persistimer = CN_TCP_TICK_PERSIST;
		tcb->persistimer = CN_TCP_TICK_PERSIST;

		result = false;
	}
	return result;
}
// =============================================================================
// 函数功能：__Tcp_DataProbe
//        坚定定时器超时所用
// 输入参数：tcb,待发的TCB
// 输出参数：
// 返回值  ：true 成功  false 失败
// 说明    :when the snd window is zero and we also has some data to send, then we
//      use this function to snd a pkg to avoid the ackno miss
// =============================================================================
bool_t __Tcp_DataProbe(tagTcpTcb *tcb)
{
	bool_t result;
	u8  flag;
	u16  sndlen;
	tagNetPkg *pkgdata;
	tagTcpSndUnackNode *secbuf;

	//fetch one pkg from the sbuf to snd
	pkgdata = tcb->sbuf.pkglsthead;
	tcb->sbuf.pkglsthead = pkgdata->partnext;
	pkgdata->partnext = NULL;
	if(NULL == tcb->sbuf.pkglsthead)
	{
		tcb->sbuf.pkglsttail = NULL;
	}
	sndlen = pkgdata->datalen;
	//采集要发送的标记
	//处理发送标记
	flag = CN_TCP_FLAG_ACK;
	if(NULL == tcb->sbuf.pkglsthead)//就这么一包数据了
	{
		flag |= CN_TCP_FLAG_PSH;//最后一个包要标记PSH
		if(1== tcb->channelstate.fin)//需要发送FIN
		{
			flag |= CN_TCP_FLAG_FIN;
			//启动FIN重传定时器
			sndlen +=1;
		}
	}
	//OK,组包完毕，可以发送了
	result = __Tcp_MsgSend(tcb, flag, NULL, 0, pkgdata, tcb->sndnxt);
	if(result)
	{
		//add the pkgdata to the unack queque
		//because we used the timeoutforever,so we must got the buf when we return
		secbuf = __Tcp_AllocUnackedNode();
		pkgdata->partnext = NULL;
		secbuf->lsthead = pkgdata;
		secbuf->datalen = pkgdata->datalen;
//		secbuf->lsttail = pkgdata;
		secbuf->seqno = tcb->sndnxt;
		secbuf->nxt = NULL;
		secbuf->sndtime = __Tcp_GetSysTime();
		secbuf->timeout =secbuf->sndtime +tcb->rto;
		secbuf->rsndtimes = 0;
		secbuf->flags = flag;

		if(NULL == tcb->unacksechead)
		{
			tcb->unacksechead = secbuf;
			tcb->unacksectail = secbuf;
		}
		else
		{
			tcb->unacksectail->nxt = secbuf;
			tcb->unacksectail = secbuf;
		}

		//must update when snd success
		tcb->sndnxt += sndlen;

		//update some tcb member
		tcb->sbuf.buflen -= sndlen;

		//we have snt some data
		if((tcb->sbuf.buflenlimit - tcb->sbuf.buflen)>tcb->sbuf.triglevel)
		{
			if(0 == (tcb->owner->status & CN_SOCKET_WRITEALBE))
			{
				tcb->owner->status |= CN_SOCKET_WRITEALBE;
				Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
			}
			Lock_SempPost(tcb->sbuf.bufsync);
		}
		//处理标记，如果发送成功，则还要看看是否消去FIN
		if(flag & CN_TCP_FLAG_FIN)
		{
			//发送了FIN，状态切换
			if(EN_TCPSTATE_CLOSE_WAIT == tcb->state)
			{
				tcb->state = EN_TCPSTATE_LAST_ACK;
			}
			else
			{
				tcb->state = EN_TCPSTATE_FIN_WAITE1;
			}
		}
        //turn off the ack timer
        tcb->timerflag &= (~CN_TCP_TIMER_ACK);
		//turn on the resnd timer
		tcb->timerflag |= CN_TCP_TIMER_RETRANSMIT;
        //turn off the ack timer
        tcb->timerflag &= (~CN_TCP_TIMER_ACK);

		result = true;
	}
	else
	{
		//snd the data failed,so readd it to the snd queue
		pkgdata->partnext = tcb->sbuf.pkglsthead;
		tcb->sbuf.pkglsthead = pkgdata;
		if(NULL == tcb->sbuf.pkglsttail)
		{
			tcb->sbuf.pkglsttail = pkgdata;
		}
	}

	return result;
}
// =============================================================================
// 函数功能：__Tcp_Syn2Snd
//          需要发送SYN标记
// 输入参数：tcb,待发的TCB
// 输出参数：
// 返回值  ：true 成功  false 失败
// 说明    :专门处理发送SYN的问题
// =============================================================================
bool_t __Tcp_Syn2Snd(tagTcpTcb *tcb)
{
	bool_t              result;     //the function result
	u8                  flag;       //the snd flags
	tagTcpSndUnackNode  *secbuf;    //used to storage the snd message

	result = false;
	//the server need to snd syn ack while the client just need to snd syn
	flag =CN_TCP_FLAG_SYN;
	if(EN_TCPSTATE_SSYNRCV ==tcb->state)
	{
		flag |= CN_TCP_FLAG_ACK;
	}
	else if(EN_TCPSTATE_CREATED == tcb->state)
	{
		flag = flag;
	}
	else
	{
		return result;        //other state we could never snd the syn flags
	}
	if(__Tcp_MsgSend(tcb, flag, (u8 *)&sgSynOption, \
			         sizeof(sgSynOption), NULL,tcb->sndnxt))
	{
		//alloc the sndbuf to manage the syn resnd
		secbuf = __Tcp_AllocUnackedNode();
		secbuf->lsthead = NULL;
		secbuf->seqno = tcb->sndnxt;
		secbuf->nxt = NULL;
		secbuf->sndtime = __Tcp_GetSysTime();
		secbuf->timeout =secbuf->sndtime +tcb->rto;
		secbuf->rsndtimes = 0;
		secbuf->flags = flag;
		secbuf->datalen = 1;

		//update the tcb members
		tcb->timerflag |= CN_TCP_TIMER_RETRANSMIT;
		tcb->snduna = tcb->sndnxt;
		tcb->sndnxt ++;
		//changge the tcb state
		if(EN_TCPSTATE_CREATED == tcb->state)
		{
			tcb->state = EN_TCPSTATE_CSYNSNT;
		}
		tcb->unacksechead = secbuf;
		tcb->unacksectail = secbuf;
        //turn off the ack timer
        tcb->timerflag &= (~CN_TCP_TIMER_ACK);

		result = true;
	}
	return result;
}
// =============================================================================
// 函数功能：__Tcp_Fin2Snd
//          需要发送FIN标记
// 输入参数：tcb,待发的TCB
// 输出参数：
// 返回值  ：true 成功  false 失败
// 说明    :专门处理发送FIN的问题
// =============================================================================
bool_t __Tcp_Fin2Snd(tagTcpTcb *tcb)
{
	bool_t              result;     //the function result
	u8                  flag;       //the snd flags
	tagTcpSndUnackNode  *secbuf;    //used to storage the snd message

	result = false;
	//发送FIN报文
	if((tcb->channelstate.fin ==1)&&((tcb->state == EN_TCPSTATE_ESTABLISHED)||\
	   (tcb->state == EN_TCPSTATE_CLOSE_WAIT)))
	{
		flag = CN_TCP_FLAG_FIN|CN_TCP_FLAG_ACK |CN_TCP_FLAG_PSH;
	}
	else
	{
		return result;
	}

	if(__Tcp_MsgSend(tcb, flag,NULL, 0, NULL,tcb->sndnxt))
	{
		//alloc the sndbuf to manage the fin resnd
		secbuf = __Tcp_AllocUnackedNode();
		secbuf->lsthead = NULL;
//		secbuf->lsttail = NULL;
		secbuf->seqno = tcb->sndnxt;
		secbuf->nxt = NULL;
		secbuf->sndtime = __Tcp_GetSysTime();
		secbuf->timeout =secbuf->sndtime +tcb->rto;
		secbuf->rsndtimes = 0;
		secbuf->flags = flag;

		//update the tcb members
		if(NULL == tcb->unacksechead)
		{
			tcb->unacksechead = secbuf;
			tcb->unacksectail = secbuf;
		}
		else
		{
			tcb->unacksectail->nxt = secbuf;
			tcb->unacksectail = secbuf;
		}
		tcb->timerflag |= CN_TCP_TIMER_RETRANSMIT; //turn on the retransmit timer
		tcb->sndnxt ++;

		//状态切换
		if(EN_TCPSTATE_CLOSE_WAIT==tcb->state)
		{
			tcb->state = EN_TCPSTATE_LAST_ACK;
		}
		else
		{
			tcb->state = EN_TCPSTATE_FIN_WAITE1;
		}

        //turn off the ack timer
        tcb->timerflag &= (~CN_TCP_TIMER_ACK);;

		result = true;
	}

	return result;
}


// =============================================================================
// 函数功能：  Tcp_Send
//         向套接字发送数据
// 输入参数：  sockfd,目的套接字
//        msg,待发送的信息
//        len,信息长度
//        flags,一般为0
// 输出参数：
// 返回值  ：-1出错，否则返回发送字节数
// 说明    ：    1,先检查参数是否合法
//        2,然后看看该sockfd是否允许发送
//        3,看看缓冲区还能否容纳该数据,阻塞则等待该数据能够容纳位置，非阻塞则直接返回
//        4,在拷贝数据的时候，如果队列中有没有被填写满的数据，则填满，不够则声请新的PKG
//        5,激活该套接字的发送(需要检查条件到底是否允许激活)
//        6,暂时忽略flags
//        7,发送的时候是按照一帧一帧的发送，因此指定的数据我们希望要么都写进去，要么
//          都写不进去,buf因为是pkg队列，因此允许偶尔的超标
//        8,对每个TCB而言，其sbuf的pkg最多容纳数据为其MSS的大小，这样的效率会更高
//          不会出现频繁的拆包（大包分拆成很多小包发送）
// =============================================================================
int Tcp_Send(tagSocket *sockfd, const void *msg, int len, int flags)
{
	int  result;
	tagTcpTcb    *tcb;

	result = -1;

	if(EN_SOCKET_TERMINAL == sockfd->socktype)//其他条件则不必发送了
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			tcb = (tagTcpTcb *)sockfd->cb;
			if(1 == tcb->channelstate.asnd)
			{
				//if the free space of sbuf is bigger than the triglevel,
				//then we will cpy it directly,else wait until the semp
				if(tcb->sbuf.buflenlimit>tcb->sbuf.buflen )
				{
					//enough,we could cpy the buf to the sbuf;
					result = __Tcp_Cpy2Sndbuf(tcb,msg,len);
				}
				else
				{
					//no buf mem for the cpy, if block operate the we will wait
					//else we will return
					//the snd buf is full, so if block we must wait
					if(sockfd->constate.noblock)
					{
						sockfd->errorno = EN_SOCKET_TRY_AGAIN;
					}
					else
					{
						Lock_MutexPost(sockfd->sync);
						//block here to wait
						while((tcb->sbuf.buflenlimit<tcb->sbuf.buflen)&&\
								tcb->channelstate.asnd)
						{
							Lock_SempPend(tcb->sbuf.bufsync, CN_TIMEOUT_FOREVER);
						}
						Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER);
						//if the channel has been shutdown then the write bit will also been
						//set,but we could send any data any more
						if(tcb->channelstate.asnd)
						{
							//we could cpy the buf to the sbuf;
							result = __Tcp_Cpy2Sndbuf(tcb,msg,len);
						}
						else
						{
							//the snd channel has been shut down
							Lock_SempPost(tcb->sbuf.bufsync);
							sockfd->errorno = EN_SOCKET_CHANNEL_SHUTDOWN_W;
						}
					}
				}
			}
			else
			{
				// the snd channel has been shut down
				sockfd->errorno = EN_SOCKET_CHANNEL_SHUTDOWN_W;
			}
			if(result >= 0)
			{
				//we could chk here weather we could snd it to the ip layer
				//if permit, we will snd it directly
				//which means we have snd some thing
				__Tcp_Data2Snd(tcb);
				//if the fin or send is not normal, then will not clean the trigle
				//which means that the channel is shutdown and you could got -1 result
				if((tcb->sbuf.buflenlimit - tcb->sbuf.buflen) <= tcb->sbuf.triglevel)
				{
					sockfd->status&= (~CN_SOCKET_WRITEALBE);
					Multiplex_Set(sockfd->multiio, sockfd->status);
				}
				if(tcb->sbuf.buflenlimit> tcb->sbuf.buflen)
				{
					Lock_SempPost(tcb->sbuf.bufsync);
				}
			}
			Lock_MutexPost(sockfd->sync);
		}
	}

	return  result;
}
// =============================================================================
// 函数功能：  __Tcp_CpyfRcvbuf
//        像TCB的sbuf拷贝数据
// 输入参数： tcb,待拷贝tcb
//        buf,待发送的信息
//        len,信息长度
// 输出参数：
// 返回值  ：-1出错，否则返回拷贝的字节数
// 说明    ：从TCB中拷贝数据，直到len ==0 或者 tcb的PKG为空为止
// =============================================================================
int __Tcp_CpyfRcvbuf(tagTcpTcb *tcb, void *buf, int len)
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
	pkg = tcb->rbuf.pkglsthead;
	while((len >0) && (NULL != pkg))
	{
		cpylen = len > pkg->datalen?pkg->datalen:len;
		cpybuf = (u8 *)(pkg->buf + pkg->offset);
		memcpy(srcbuf, cpybuf,cpylen);
		pkg->offset += cpylen;
		pkg->datalen -= cpylen;
		tcb->rbuf.buflen -= cpylen;
		result +=cpylen;
		len-= cpylen;
		srcbuf+=cpylen;
		if(0==pkg->datalen)
		{
			//该PKG已经无用了，可以释放了
			tcb->rbuf.pkglsthead = pkg->partnext;
			if(NULL == tcb->rbuf.pkglsthead)
			{
				tcb->rbuf.pkglsttail = NULL;
			}
			Pkg_PartFlagFree(pkg);
			pkg = tcb->rbuf.pkglsthead;
		}
	}

	return result;
}
// =============================================================================
// 函数功能：  Tcp_Recv
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
int Tcp_Recv(tagSocket *sockfd, void *buf,int len, unsigned int flags)
{
	int  result;
	tagTcpTcb    *tcb;

	result = -1;
	if(EN_SOCKET_TERMINAL == sockfd->socktype)//其他条件则不接收
	{
		if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
		{
			tcb = (tagTcpTcb *)sockfd->cb;
			if(tcb->rbuf.buflen > 0)
			{
				//OK, the rcv buf has data to read,so just cpy
				result = __Tcp_CpyfRcvbuf(tcb,buf,len);
			}
			else
			{
                if((0 == tcb->channelstate.arcv)||(0 == tcb->channelstate.krcv))
				{
					//the local or the remote has shutdown the rcv channel
					result = 0;
					sockfd->errorno = EN_SOCKET_CHANNEL_SHUTDOWN_R;
				}
				else
				{
					//no data current, if block, we will wait here, else return -1
					if(sockfd->constate.noblock)
					{
						sockfd->errorno = EN_SOCKET_TRY_AGAIN;
					}
					else
					{
						Lock_MutexPost(sockfd->sync);
						//block here
						while(0 ==tcb->rbuf.buflen)
						{
							Lock_SempPend(tcb->rbuf.bufsync, CN_TIMEOUT_FOREVER);
						}
						Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER);

						if(tcb->rbuf.buflen > 0)
						{
							result = __Tcp_CpyfRcvbuf(tcb,buf,len);
						}
						else
						{
							if((0 == tcb->channelstate.arcv)||(0 == tcb->channelstate.krcv))
							{
								//the channel has been shutdown when we wait
								sockfd->errorno = EN_SOCKET_CHANNEL_SHUTDOWN_R;
								result = 0;
							}
						}
					}
				}
			}
            if(result > 0)
			{
                if((tcb->rbuf.buflen < tcb->rbuf.triglevel)&&\
                        !((0 == tcb->channelstate.krcv)||\
                        (0 == tcb->channelstate.arcv)))
				{
					//could not read any more
					sockfd->status &= (~CN_SOCKET_READALBE);
					Multiplex_Set(sockfd->multiio, sockfd->status);
				}
                if(0< tcb->rbuf.buflen)
				{
                	Lock_SempPost(tcb->rbuf.bufsync);
					//not read any more
				}
                //changed the rcv window
                tcb->rcvwnd += result;
                //if the data is zero, we could snd the window
                if((true == tcb->rcvwndcongest)&&(tcb->rcvwnd<tcb->mss))
                {
                	__Tcp_MsgSend(tcb, CN_TCP_FLAG_ACK, NULL, 0, NULL, tcb->sndnxt);
                	tcb->rcvwndcongest = false;
                }
			}
			Lock_MutexPost(sockfd->sync);
		}
	}
	return  result;
}

// =============================================================================
// 函数功能：  __Tcp_ShutdownRD
//        shutdown the socked  read channel
// 输入参数：  socket, the socket to be shutdown
// 输出参数：
// 返回值  ：0 success while -1 failed
// 说明    ：
// =============================================================================
int __Tcp_ShutdownRD(tagSocket *sock)
{
	int result;
	tagTcpTcb *tcb;
	tagTcpReCombNode *rcnode;

	result = -1;
	if(EN_SOCKET_TERMINAL == sock->socktype)
	{
		tcb = (tagTcpTcb *)sock->cb;
		//clear the rcv buf and recombination queue
		//and set some specified flags at the same time
		tcb->channelstate.arcv = 0;//cant rcv any more
		tcb->channelstate.krcv = 0;//ignaore any reached data
		//release the rcv and recombination queue
		if(NULL != tcb->rbuf.pkglsthead)
		{
		    Pkg_LstFlagFree(tcb->rbuf.pkglsthead);
		}
	    tcb->rbuf.buflen = 0;
	    tcb->rbuf.buflenlimit  = CN_TCP_BUF_LEN_lIMIT;
	    tcb->rbuf.pkglsthead = NULL;
	    tcb->rbuf.pkglsttail = NULL;
	    tcb->rbuf.triglevel = 0;
		Lock_SempPost(tcb->rbuf.bufsync);

		//释放所有的重组链表
		rcnode = tcb->recomblst;
		while(NULL != rcnode)
		{
			tcb->recomblst = rcnode->nxt;
			if(NULL != rcnode->lsthead)
			{
				Pkg_LstFlagFree(rcnode->lsthead);
			}
			__Tcp_FreeReCombNode(rcnode);

			rcnode = tcb->recomblst;
		}
		tcb->recomblst = NULL;
		result = 0;
	}

	return result;
}
// =============================================================================
// 函数功能：  __Tcp_ShutdownWR
//        shutdown the socked  snd channel
// 输入参数：  socket, the socket to be shutdown
// 输出参数：
// 返回值  ：0 success while -1 failed
// 说明    ：
// =============================================================================
int __Tcp_ShutdownWR(tagSocket *sock)
{
	int result;
	tagTcpTcb *tcb;

	result = -1;
	if(EN_SOCKET_TERMINAL == sock->socktype)
	{
		tcb = (tagTcpTcb *)sock->cb;
		//clear the rcv buf and recombination queue
		//and set some specified flags at the same time
		tcb->channelstate.asnd = 0;//cant snd any more
		                           //app cant snd, but the kernel could snd
		//release the rcv and recombination queue
		if(0 == tcb->channelstate.fin)
		{
			tcb->channelstate.fin = 1;
			//just snd msg,let the snd service to snd the fin
			if(NULL == tcb->sbuf.pkglsthead)
			{
                __Tcp_Fin2Snd(tcb);
			}
			result = 0;
		}
	}

	return result;
}

// =============================================================================
// 函数功能：  Tcp_Shutdown
//        关闭套接字
// 输入参数：  sockfd,目的套接字
//        how,关闭方法， 见SHUT_RD等定义
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：
// =============================================================================
int Tcp_Shutdown(tagSocket *sock, u32 how)
{
	int    result;

	if(NULL != sock)
	{
		Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER);
		switch(how)
		{
			case SHUT_RD:
				result = __Tcp_ShutdownRD(sock);
				break;
			case SHUT_WR:
				result = __Tcp_ShutdownWR(sock);
				break;
			case SHUT_RDWR:
				result = __Tcp_ShutdownRD(sock);
				if(result)
				{
					result = __Tcp_ShutdownWR(sock);
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
// 函数功能：  Tcp_Close
//           套接字关闭
// 输入参数：  sockfd,待关闭的套接字
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：1,该处理中，删除并不会真正的删除,仅仅标记状态，真正的删除发生在TCP TICK中
//        会在TICK扫描发现2FREE的立即删除
//        发现本地端为CLOSED的，如果没有远端挂在其下，也会删除,如果有远端则先解决其远端
//      2,close的发送FIN还是RST(通信远端)：linger关闭，则直接发送RST;linger打开，
//        如果接收缓冲区还有数据则发送RST，否则执行四次握手关闭
// =============================================================================
int Tcp_Close(tagSocket *sock)
{
	int    result;
	tagTcpTcb   *tcb;

	result = 0;

	if(Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER))
	{
		if(EN_SOCKET_TERMINAL == sock->socktype)
		{
			//it is an communicate point
			tcb = (tagTcpTcb *)sock->cb;
			if(EN_TCPSTATE_CLOSED == tcb->state)
			{
				tcb->state = EN_TCPSTATE_2FREE;
			}
			else
			{
				if(tcb->algorithm.linger)
				{
					if(0 == tcb->rbuf.buflen)
					{
						//if the buf is empty, then we will wait the
						//the socket to disconnect by fin
						tcb->channelstate.arcv= 0;
						tcb->channelstate.asnd = 0;
						tcb->channelstate.krcv = 0;
						tcb->channelstate.close = 1;
						tcb->timerflag |=CN_TCP_TIMER_LINGER;
						//snd msg to make the sndservice to snd the fin
						//release the rcv and recombination queue
						if(0 == tcb->channelstate.fin)
						{
							tcb->channelstate.fin = 1;
							//just snd msg,let the snd service to snd the fin
							if(NULL == tcb->sbuf.pkglsthead)
							{
                                __Tcp_Fin2Snd(tcb);
							}
							result = 0;
						}
					}
					else
					{
						//接收缓冲区有数据，执行RST关闭
						printk("CLose the socket!\n\r");
						__Tcp_RstTcb(tcb);
						tcb->channelstate.close = 1;
						tcb->state = EN_TCPSTATE_2FREE;
					}
				}
				else
				{
					//延时没有打开,执行RST关闭
					printk("CLose the socket!\n\r");
					__Tcp_RstTcb(tcb);
					tcb->channelstate.close = 1;
					tcb->state = EN_TCPSTATE_2FREE;
				}
			}
		}
		else
		{
			//if it is the server or not defined yet, then just set the close bit
			//if no communicate is exist, then in tcp tick process will free it
			sock->appstate = EN_SOCKET_LOCAL_CLOSE;
		}
		Lock_MutexPost(sock->sync);
	}

	return result;
}
// =============================================================================
// 函数功能：  __Tcp_Sol_Socket
//           SOL_SOCKET选项处理
// 输入参数：  sockfd,待处理的套接字
//           optname,选项名字
//           optval,待设置参数
//           optlen,参数长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int __Tcp_Sol_Socket(tagSocket *sockfd,int optname,const void *optval, int optlen)
{
	bool_t result;
	tagTcpTcb *tcb;
	struct linger      *mylinger;

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
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				//远端的话处理一个就OK了
				tcb = (tagTcpTcb *)sockfd->cb;
				if(*(int *)optval)
				{
					tcb->timerflag |= CN_TCP_TIMER_KEEPALIVE;
					tcb->keeptimer = CN_TCP_TICK_KEEPALIVE;
				}
				else
				{
					tcb->timerflag &= (~CN_TCP_TIMER_KEEPALIVE);
				}
				result = 0;
			}
			break;
		case SO_LINGER:
			if((EN_SOCKET_TERMINAL == sockfd->socktype)&&\
					(optlen == sizeof(struct linger)))
			{
				tcb = (tagTcpTcb *)sockfd->cb;
				mylinger  = (struct linger *)optval;
				if(mylinger->l_onff)
				{
					tcb->algorithm.linger = 1;
					tcb->lingertimer = mylinger->l_linger*1000/CN_TCP_TICK_TIME;
				}
				else
				{
					tcb->algorithm.linger = 0;
				}
				result = 0;
			}
			break;
		case SO_OOBINLINE:
			break;
		case SO_RCVBUF:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				tcb = (tagTcpTcb *)sockfd->cb;
				if(*(int *)optval>0)
				{
					tcb->rbuf.buflenlimit = *(int *)optval;
					result = 0;
				}
			}
			break;
		case SO_SNDBUF:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				tcb = (tagTcpTcb *)sockfd->cb;
				if(*(int *)optval>0)
				{
					tcb->sbuf.buflenlimit = *(int *)optval;
					result = 0;
				}
			}
			break;
		case SO_RCVLOWAT:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				tcb = (tagTcpTcb *)sockfd->cb;
				if(*(int *)optval>0)
				{
					tcb->rbuf.triglevel = *(int *)optval;
					result = 0;
				}
			}
			break;
		case SO_SNDLOWAT:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				tcb = (tagTcpTcb *)sockfd->cb;
				if(*(int *)optval>0)
				{
					tcb->sbuf.triglevel = *(int *)optval;
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
// 函数功能：  __Tcp_IpProto_Ip
//           IPPROTO_IP选项处理
// 输入参数：         sockfd,待处理的套接字
//           optname,选项名字
//           optval,待设置参数
//           optlen,参数长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int __Tcp_IpProto_Ip(tagSocket *sockfd,int optname,const void *optval, int optlen)
{
	bool_t result;
	tagTcpTcb *tcb;

	result = -1;
	switch(optname)
	{
		case IP_HDRINCL:
			break;
		case IP_OPTIONS:
			break;
		case IP_TOS:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				//远端的话处理一个就OK了
				tcb = (tagTcpTcb *)sockfd->cb;
				tcb->ipoption.tos = *(u8 *)optval;
				result = 0;
			}
			break;
		case IP_TTL:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				tcb = (tagTcpTcb *)sockfd->cb;
				tcb->ipoption.ttl = *(u8 *)optval;
				result = 0;
			}
			break;
		default:
			break;
	}
	
	return result;
}
// =============================================================================
// 函数功能：  __Tcp_IpProto_Tcp
//           IPPROTO_TCP选项处理
// 输入参数：  sockfd,待处理的套接字
//           optname,选项名字
//           optval,待设置参数
//           optlen,参数长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int __Tcp_IpProto_Tcp(tagSocket *sockfd,int optname,const void *optval, int optlen)
{
	bool_t result;
	tagTcpTcb *tcb;

	result = -1;
	switch(optname)
	{
		case TCP_MAXSEG:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				tcb = (tagTcpTcb *)sockfd->cb;
				tcb->lmss = *(u8 *)optval;
				result = 0;
			}
			break;
		case TCP_NODELAY:
			if(EN_SOCKET_TERMINAL == sockfd->socktype)
			{
				tcb = (tagTcpTcb *)sockfd->cb;
				if(*(int *)optval)
				{
					tcb->algorithm.nagle = 0;
				}
				else
				{
					tcb->algorithm.nagle = 1;
				}
				result = 0;
			}
			break;
		default:
			break;
	}
	
	return result;
}
// =============================================================================
// 函数功能：  Tcp_Setsockopt
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
int Tcp_Setsockopt(tagSocket *sockfd, int level, int optname,\
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
					result = __Tcp_Sol_Socket(sockfd,optname,optval,optlen);
					break;
				case IPPROTO_IP:
					result = __Tcp_IpProto_Ip(sockfd,optname,optval,optlen);
					break;
				case IPPROTO_TCP:
					result = __Tcp_IpProto_Tcp(sockfd,optname,optval,optlen);
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
// 函数功能：  Tcp_Getsockopt
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
int Tcp_Getsockopt(tagSocket *sockfd, int level, int optname, void *optval,\
		       int *optlen)
{
	int  result = -1;


	return  result;
}
// =============================================================================
// 函数功能：  Tcp_MultiIoAdd
//           used to add an tcp socket to the sets
// 输入参数：  sets,which represent the target  sets we interested
//           objectid, which represents the object who trigger the sets
//           sendingBit, the bits that we are interested
//           sockfd, the socket we need to monitor
// 输出参数：
// 返回值  ：if success returns 0 else -1
// 说明    ：
// =============================================================================
bool_t Tcp_MultiIoAdd(struct tagMultiplexSetsCB  *Sets,\
        ptu32_t ObjectID, u32 SensingBit,tagSocket *sockfd)
{
	bool_t result;
	u32  initstatus;

	result = false;
	if(Lock_MutexPend(sockfd->sync, CN_TIMEOUT_FOREVER))
	{

		initstatus = sockfd->status;
		result = Multiplex_AddObject(Sets,&sockfd->multiio,initstatus,\
				                     ObjectID,SensingBit);
		Lock_MutexPost(sockfd->sync);
	}

	return result;
}

////////////////////////状态机的处理////////////////////////////////////////////
// =============================================================================
// 函数功能：  __Tcp_GetSocket
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
tagSocket *__Tcp_GetSocket(u32 iplocal, u32 ipremote, u16 portlocal, u16 portremote)
{
	tagSocket *result = NULL;
	tagSocket *ltmp = NULL;
	tagSocket *rtmp = NULL;
	tagTcpTcb *tcb;
	tagTcpTnode *tnode;

	ltmp = pgTcpSockLstH;
	while(NULL != ltmp)
	{
		if(ltmp->laddr.port == portlocal)
		{
			if(EN_SOCKET_TERMINAL==ltmp->socktype)
			{
				//if the remote port and ip match, then we will return it
				//else, return NULL, which means there is no such sock
				tcb = ltmp->cb;
				if((tcb->raddr.port == portremote)&&(tcb->raddr.ip == ipremote))
				{
					result = ltmp;//this must be an local client socket
				}
			}
			else
			{
				//this is an server node, we chk if any sock hang on the ltmp
				//match it, if not, we will return the ltmp
				result = ltmp;
				tnode= (tagTcpTnode *)ltmp->cb;
				rtmp = tnode->clientlst;
				while(NULL != rtmp)
				{
					tcb = (tagTcpTcb *)rtmp->cb;
					if((tcb->raddr.port == portremote)&&(tcb->raddr.ip == ipremote))
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
// 函数功能：__Tcp_ChksumRcvPkg
//        tcp校验码生成
// 输入参数： iplocal,本地端IP
//        ipremote,远端IP
// 输出参数：
// 返回值  ：true 校验通过 false校验失败
// 说明    :收到的TCP报文软校验
// =============================================================================
bool_t  __Tcp_ChksumRcvPkg(u32 iplocal,u32 ipremote, tagNetPkg *pkglst)
{
	bool_t  result;
	u16     chksum;
	u16     translen;
	tagNetPkg *pkg;
	translen = 0;
	pkg = pkglst;
	while(pkg != NULL)
	{
		translen += pkg->datalen;
		pkg = pkg->partnext;
	}
	Ip_PkgLstChksumPseudo(iplocal, ipremote, CN_IPTPL_TCP,\
			             pkglst,translen,&chksum);
	if(0 == chksum)
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
// 函数功能：__Tcp_CheckTcpHdr
//          检查TCP头是否合法
// 输入参数：hdr,待检查的TCP头
// 输出参数：
// 返回值  ：true 合法 false 非法
// 说明    :通过对TCP头的标记的检测，来防止恶意攻击或者垃圾报文
// =============================================================================
bool_t  __Tcp_CheckTcpHdr(tagTcpHdr *hdr)
{
	bool_t result;
	u8     flag;
	flag = hdr->flags & CN_TCP_FLAG_MSK;
	result = true;
	if((flag &CN_TCP_FLAG_SYN)&& (flag &CN_TCP_FLAG_FIN))
	{
		result = false;//SYN FIN 包
	}
	else if((flag &CN_TCP_FLAG_SYN)&& (flag &CN_TCP_FLAG_RST))
	{
		result = false;//SYN RST包
	}
	else if((flag &CN_TCP_FLAG_RST)&&(0 ==(flag & CN_TCP_FLAG_ACK)))//有RST 但是无ACK
	{
		result = false;
	}
	else if((flag &CN_TCP_FLAG_FIN)&&(0 == (flag & CN_TCP_FLAG_ACK)))
	{
		result = false;
	}
	else if((flag &CN_TCP_FLAG_SYN)&&(hdr->hdrlen <=0x05)) //SYN报文必须要有选项
	{
		result = false;
	}
	else if(0 == flag)  
	{
		result = false;//NULL标记包，一般的ACK作为0消费包，应该标记ack
	}
	else
	{
		result = true;
	}
	
	return result;
}

// =============================================================================
// 函数功能：__Tcp_RstRemoteRaw
//          重启对端的套接字
// 输入参数： iplocal,本地端IP
//        portlocal,本地端端口
//        ipremote,远端IP
//        portremote,远端port
//        ackno, 必须落在对方的发送窗口，否则对端
// 输出参数：
// 返回值  ：true 成功 false失败
// 说明    :当TCP协议收到不明的报文时，自然会这么做，例如找不到对应的端口号
//       找不到为其服务的服务器，这个时候套接字建立不起来，自然不会有TCB，所以只需要
//       回复其一个裸的TCP头裸报文即可
//       直接打包一个TCP头报文发送出去即可
//       需要注意的是RST报文也需要ACK号，该ACK号应该为发送者发送过来的序号
// =============================================================================
bool_t __Tcp_RstRemoteRaw(u32 iplocal, u16 portlocal,u32 ipremote, u16 portremote,u32 ackno)
{
	bool_t result ;	
	tagNetPkg *pkg;
	tagTcpHdr   *tcphdr;
	u16 translen;
	
	result = false;
	pkg = Pkg_Alloc(sizeof(tagTcpHdr),CN_PKGFLAG_FREE);
	if(NULL != pkg)
	{
		tcphdr = (tagTcpHdr *)(pkg->offset + pkg->buf);
		tcphdr->chksum = 0;
		tcphdr->flags = CN_TCP_FLAG_RST|CN_TCP_FLAG_ACK;
		tcphdr->hdrlen = 0x05;
		tcphdr->portdst = htons(portremote);
		tcphdr->portsrc = htons(portlocal);
		tcphdr->ackno  = htonl(ackno);
		tcphdr->seqno = 0;
		tcphdr->urgencyp = 0;	
		pkg->datalen = sizeof(tagTcpHdr);
		translen = pkg->datalen;
		
		result = Ip_Send(iplocal, ipremote, pkg,translen,CN_IPTPL_TCP,\
				 CN_IPDEV_TCPOCHKSUM,&tcphdr->chksum);
		if(false == result)
		{
			Pkg_PartFree(pkg);
		}		
	}
	return result;
}

enum __EN_TCP_OPT_CODE
{
	EN_TCP_OPT_END = 0,
	EN_TCP_OPT_NONE,
	EN_TCP_OPT_MSS ,
	EN_TCP_OPT_SCALE,
	EN_TCP_OPT_SACKP,
	EN_TCP_OPT_SACKR,
	EN_TCP_OPT_TAMP = 8,
};
// =============================================================================
// 函数功能：__Tcp_OptDeal
//        通过TCP报文头的选项来更新TCB的一些内容
// 输入参数：void *opt,选项指针
//        u8   len, 选项长度
//        bool_t  syn, 报文是否包含syn标志
// 输出参数：tcb,待更新的tcb
// 返回值  ：无
// 说明    :处理的选项包括MSS SACK，其他选项暂时不支持
// =============================================================================
void __Tcp_OptDeal(tagTcpTcb *tcb, tagTcpHdr *hdr)
{
	u8  totallen;
	u8  optlen;
	u8  kind;
	u8  data8;
	u16 data16;
	u8  *src;
	u8  *optsrc;

	totallen = (hdr->hdrlen -0x05)*4;
	src = (u8 *)hdr->opt;
	while(totallen > 0)
	{
	    optsrc = src;
		kind = *optsrc++;
		switch(kind)
		{
			case EN_TCP_OPT_END:      //OPTION END
				printk("%s:kind = %d\n\r",\
					   __FUNCTION__,kind);
				totallen = 0;
				break;
			case EN_TCP_OPT_NONE:     //OPTION NONE
				printk("%s:kind = %d\n\r",\
					   __FUNCTION__,kind);
				totallen -= 1;
				src += 1;
				break;
			case EN_TCP_OPT_MSS:      //SET THE REMOTE MSS
				optlen = *optsrc++;
				data16 = *(u16 *)(optsrc);
				tcb->rmss = htons(data16);
				tcb->mss = tcb->lmss>tcb->rmss?tcb->rmss:tcb->lmss;
				printk("%s:kind = %d len = %d data = %d\n\r",\
					   __FUNCTION__,kind,optlen,data16);
				totallen -= optlen;
				src += optlen;
				break;
			case EN_TCP_OPT_SCALE:      //SET THE REMOTE WND SCALE
				optlen = *optsrc++;
				data8 = *(u8 *)(optsrc);
				tcb->sndwndscale = data8;
				printk("%s:kind = %d len = %d data = %d\n\r",\
					   __FUNCTION__,kind,optlen,data8);
				totallen -= optlen;
				src += optlen;
				break;
			case EN_TCP_OPT_TAMP:
				optlen = *optsrc++;
				printk("%s:kind = %d len = %d \n\r",\
					   __FUNCTION__,kind,optlen);
				totallen -= optlen;
				src += optlen;
				break;
			default:
				printk("%s:kind = %d\n\r",\
					   __FUNCTION__,kind);
				totallen = 0;
				break;
		}
	}
	return ;
}
//////////////////////////////////状态处理机函数/////////////////////////////////
//RST报文可以看做是一种特殊报文，我们做统一的处理即可--复位
//因此在各个状态机的处理中，我们不考虑RST报文的处理，按照正常的状态转换图处理即可
//对于状态转换：如果是收到某报文而不需要回复时的状态迁移可以直接在收处理中进行，
//对于收到报文需要回复才能进行状态迁移，必须在回复完毕之后才进行状态迁移，每种状态
//要充分考虑到收到异样报文的处理
//规则：
//1,所有的状态，在没有得到本地端的允许之下，是不会发送FIN的；
//2,只有SHUT_WR SHUT_RDWR CLOSE才会允许发送FIN,才会真正的关闭连接
//3,在没有收到本地端关闭的命令下，所有的远端状态最多到达RST状态
//4,refered标志只有在accept的时候被置为真，其他的情况下都是false，复位不会复位该标志
//5,在状态机中，pkg的offset一定要移动到仅仅包含数据的阶段，跳过hdr和opt
// =============================================================================
// 函数功能：__TcpRetransmitDeal
//        重传定时器
// 输入参数：tcb,指定的TCB控制块
//          fastretrans, true为快速重传，false 则检查是否是超吃重传
// 输出参数：
// 返回值  ：
// 说明    :重传定时器，负责SYN/数据/FIN的重传，其实不论是SYN DATA FIN，都是需要重传的，他们都是需要占用seqno
//      号码的，无论哪一种重传，我们都可以认为是flags+data的形式，即我们在需要发送的
// =============================================================================
void __TcpRetransmitDeal(tagTcpTcb *tcb,bool_t fastretrans)
{
	tagTcpSndUnackNode  *secbuf;
	s64 timenow;
	//the tcb timer is running,so we chk weather the tcb has the
	//retransmit secbuf, because the timer is turned on ,so the secbuf
	//must be UNNULL
	//this is an assert chk
	secbuf = tcb->unacksechead;
	if(NULL != secbuf)
	{
		timenow = __Tcp_GetSysTime();
		if((secbuf->timeout < timenow)||(true == fastretrans))
		{
			//this is an timeout secbuf
			if(secbuf->rsndtimes < CN_TCP_RSNDTIMES_LIMIT)
			{
				//still in the retransmit target
				if(secbuf->flags&CN_TCP_FLAG_SYN)
				{
					//the retransmit include the syn flag,must transmit the option
					__Tcp_MsgSend(tcb,secbuf->flags,(u8 *)&sgSynOption,\
							      sizeof(sgSynOption),NULL,secbuf->seqno);
				}
				else
				{
					//maybe the data transmition
					__Tcp_MsgSend(tcb,secbuf->flags,NULL,0,secbuf->lsthead,secbuf->seqno);
				}
				//deal the timeout congestion
				//update the new sec timeout time
				tcb->rto = tcb->rto<<1;
				if(tcb->rto > CN_TCP_RTOMAX)
				{
					tcb->rto = CN_TCP_RTOMAX;
				}
				secbuf->timeout = timenow + tcb->rto;
				secbuf->rsndtimes++;
				//because we has dealt an timeout retransmition, so we must deal the
				//congestion window
				tcb->ssthresh = tcb->cwnd>>1;
				if(tcb->ssthresh < (2*tcb->mss))
				{
					tcb->ssthresh = 2*tcb->mss;
				}
				if(fastretrans)
				{
					tcb->cwnd = tcb->ssthresh + 3*tcb->mss;
				}
				else
				{
					tcb->cwnd = tcb->mss;
				}
				//for we has snd the flags,turn off the acktimer
				tcb->timerflag&=(~CN_TCP_TIMER_ACK);

				printk("Tcp Tick: Rsnd!--RTO=0X%08X\n\r",tcb->rto);
			}
			else
			{
			    //it has exceed the retransmit target, so just reset
				__Tcp_RstTcb(tcb);
			}
		}
	}

	return;
}

// =============================================================================
// 函数功能：__Tcp_CheckInTargetArea
//          判断某个数值是否在指定的范围内
// 输入参数：target_start,指定起始值
//          target_end,指定结束值
//          tojudge,待判定的数值
// 输出参数：
// 返回值  ：true,在指定范围，false不再指定范围
// 说明    :该判断需考虑绕回现象
// =============================================================================
bool_t __Tcp_CheckInTargetArea(u32 target_start, u32 target_end, u32 tojudge)
{
	bool_t result = false;

	if(target_end < target_start) //有绕回
	{
		if((tojudge>= target_start)|| (tojudge <= target_end))
		{
			result = true;
		}
	}
	else //无绕回
	{
		if((tojudge >= target_start) &&(tojudge<= target_end))
		{
			result = true;
		}
	}
	return result;
}

static u32 sgLastAck;
// =============================================================================
// 函数功能：__Tcp_DealAck
//          处理ACK标记
// 输入参数：tcb, tcb控制块
//        hdr, tcp 包头
// 输出参数：
// 返回值  ：bool_t true处理成功 false失败
// 说明    :1,当ACK报文到来时，如果ACK号和WINDOW激活了TCB要发送数据，则激活，否则仅仅更新
//        相关成员量即可
//      2,关于ackno,需要注意的是要记得释放未确认的链表
//      3,对于延时关闭而言，如果收到ACK并且确认无再可发送数据且确认了所有数据都发送
//         了，那么需要发送FIN信号
// =============================================================================
bool_t __Tcp_DealAck(tagTcpTcb *tcb, tagTcpHdr *hdr)
{
	bool_t             result;
	bool_t             retransever;
	u32                newackno;
	u32                newwindow;
	u32                addwnd;
	s64                timenow;
	s64                timesnd;
	s32                err;
	s32                uerr;
	tagTcpSndUnackNode *secbuf;
	
	result = false;
	secbuf = tcb->unacksechead;
	newackno = ntohl(hdr->ackno);
	newwindow = ntohs(hdr->window);
	
	sgLastAck = newackno;              //for the debug--TODO CLEAR
	//acked the unack pkg
	if((newackno == tcb->snduna)&&(NULL != secbuf))
	{
		//the same ack, maybe an need fast retransmit
		tcb->dupacks++;
		if(tcb->dupacks >= CN_TCP_FASTRETRANS)
		{
			//do the fast retransmition
			__TcpRetransmitDeal(tcb,true);
		}	
	}
	else if(NULL == secbuf)
	{
		tcb->dupacks = 0;
	}
	else 
	{
		//update the ackno and free the acked secbuf
		addwnd =0;
		retransever = false;
		//free the secbuf
		while(NULL != secbuf)
		{
			//-1 means that when seqno == newackno, which means that this secbuf could
			//not release;
			//in the windows stack, they could snd the ack not complete pkg.
			//which means they ack the pkg partely,so we could not release the
			//whole pkg
			if(__Tcp_CheckInTargetArea(tcb->snduna, newackno, secbuf->seqno+secbuf->datalen))
			{
				tcb->unacksechead = secbuf->nxt;
				if(secbuf->rsndtimes > 0)
				{
					retransever = true;
					timesnd = secbuf->sndtime;
				}
				if(NULL == tcb->unacksechead)
				{
					tcb->unacksectail = NULL;
				}
				Pkg_PartFree(secbuf->lsthead);
				__Tcp_FreeUnackedNode(secbuf);
				secbuf = tcb->unacksechead;
				addwnd++;
			}
			else
			{
				secbuf = NULL;
			}
		}
		//update rtt measure
		if(false == retransever)
		{
			//do the rtt test,update the rto
			timenow = __Tcp_GetSysTime();
			err = timenow - timesnd - tcb->sa;
			uerr = err>0?err:(-err);
			//scale the sa sd and rto
			tcb->sa = tcb->sa+(err>>3);
			tcb->sd = tcb->sd + ((uerr-tcb->sd)>>2);
			tcb->rto = tcb->sa + (tcb->sd<<2);
			if(tcb->rto < CN_TCP_RTOMIN)
			{
				tcb->rto = CN_TCP_RTOMIN;
			}
		}
		//update the congestion wnd
		if(tcb->dupacks >= CN_TCP_FASTRETRANS)
		{
			//an fast retransmit has happened
			tcb->cwnd = tcb->ssthresh;
		}
		else
		{
			//chk it is slow start or congestion avoid
			if(tcb->cwnd > tcb->ssthresh)
			{
				//it is congestion avoid
				tcb->cwnd += tcb->cwnd/32;
			}
			else
			{
				addwnd = addwnd*tcb->mss;
				tcb->cwnd += addwnd;

			}
		}  
		//update the ackno
		tcb->snduna = newackno;
		tcb->dupacks = 0;
		//retransmit timer
		if(NULL == tcb->unacksechead)
		{
			//turn off the retransmit timer
			tcb->timerflag &= (~CN_TCP_TIMER_RETRANSMIT);
		}
		
        result = true;
	}
	//update the window
	if(tcb->sndnxt >= tcb->snduna)
	{
		addwnd = tcb->sndnxt- tcb->snduna;
	}
	else
	{
		addwnd = 0xFFFFFFFF - tcb->snduna + tcb->sndnxt;
	}

	newwindow = newwindow<<tcb->sndwndscale;
	if(newwindow > addwnd)
	{
		tcb->sndwnd = (newwindow - addwnd)>>tcb->sndwndscale;
	    __Tcp_Data2Snd(tcb);
	}
	else
	{
		tcb->sndwnd = 0;
		if(NULL != tcb->sbuf.pkglsthead)
		{
			tcb->timerflag |=CN_TCP_TIMER_PERSIST;
		}
	}	
	return result;
}

// =============================================================================
// 函数功能：__Tcp_DealSeqno
//          处理ACK标记
// 输入参数：tcb, tcb控制块
//          hdr, tcp 包头
// 输出参数：pkgreserve,是保留PKG，true表示PKG已经插入队列中，false表示pkg需要释放
// 返回值  ：true,该数据包被插进了缓冲区队列，否则表示可以释放PKG
// 说明    :1,在调用该函数之前，应该检查pkg的有效性（有效长度是否为0,是否超出MSS）
//       2,因为数据包是合法的，因此务必要打开ACK定时器，可以在处理的最后打开
//       3,窗口为0时，如果接收缓冲区中的数据已经达到窗口的4倍，那么就丢弃该数据包
//                   否则继续处理该数据包，否则会导致我们的缓冲区无限扩大
//       4,处理该数据包时，如果seqno小于rcvnxt，证明是重复发的数据，不用理会；
//                        等于rcvnxt,则该数据包恰到好处，此时还要看重组队列中是否
//                                   还有包需要重组
//                        大于rcvnxt，则证明是乱序数据，直接放进重组序列即可
// =============================================================================
bool_t __Tcp_DealRcvData(tagTcpTcb *tcb, u32 seqno,tagNetPkg *pkg)
{
	bool_t  result;
	int     rcvlen;
	tagTcpReCombNode *secbuf;
	tagTcpReCombNode *tmpsec1;
	tagTcpReCombNode *tmpsec2;
	
	result = false;
	pkg->partnext = NULL;
	rcvlen = 0;
	if(seqno == tcb->rcvnxt)
	{
		//this is just the pkg what we rcv next
		//this is an sequence pkg what we need,add it to the rcv buf
		if(NULL == tcb->rbuf.pkglsttail)
		{
			tcb->rbuf.pkglsthead = pkg;
		}
		else
		{
			tcb->rbuf.pkglsttail->partnext = pkg;
		}
		tcb->rbuf.pkglsttail = pkg;
		tcb->rbuf.buflen += pkg->datalen;
		tcb->rcvnxt += pkg->datalen;
		tcb->rcvwnd = tcb->rcvwnd > pkg->datalen?(tcb->rcvwnd-pkg->datalen):0;
		rcvlen += pkg->datalen;
		//check if the recombination queue has some corresponding pkg to add
		secbuf = tcb->recomblst;
		while(secbuf != NULL)
		{
			if(secbuf->seqno == tcb->rcvnxt)
			{
				//this is the recombinnation pkg
				tcb->rbuf.pkglsttail->partnext = secbuf->lsthead;
				tcb->rbuf.pkglsttail = secbuf->lsttail;
				tcb->rbuf.buflen += secbuf->datalen;
				tcb->rcvnxt += secbuf->datalen;
				tcb->rcvwnd = tcb->rcvwnd>secbuf->datalen?\
						     (tcb->rcvwnd-secbuf->datalen):0;
				rcvlen += secbuf->datalen;
				tcb->recomblst = secbuf->nxt;
				__Tcp_FreeReCombNode(secbuf);
				secbuf= tcb->recomblst;
			}
			else
			{
				//because we have queue it when we add it into the rb queue
				//if the pre pkg is not the bingo one, then there is no chance
				//for the nxt one, if it is not in the target area, we will
				//realease it to avoid exist in the queue forever
				if(__Tcp_CheckInTargetArea(tcb->rcvnxt, \
						tcb->rcvnxt+CN_TCP_RCVWIN_DEFAULT,secbuf->seqno))
				{
					//this secbuf is the latest one, but can't recombin it yet,
					//so we end the recombin action
					secbuf = NULL;//end for the while
				}
				else
				{
					//release the secbuf,because it is out of the target area,
					//we will recycle the resource it alloc
					tcb->recomblst = secbuf->nxt;
					Pkg_LstFlagFree(secbuf->lsthead);
					__Tcp_FreeReCombNode(secbuf);
					secbuf= tcb->recomblst;
				}
			}
		}
		//rcv the new data, notice the app to read it
		//means the pkg has been cached in the rcb process
		if(tcb->rbuf.buflen >= tcb->rbuf.triglevel)
		{
			if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
			{
				tcb->owner->status |= CN_SOCKET_READALBE;
				Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
			}
			Lock_SempPost(tcb->rbuf.bufsync);
		}
		result = true;
	}
	else if(__Tcp_CheckInTargetArea(tcb->rcvnxt, \
			tcb->rcvnxt+CN_TCP_RCVWIN_DEFAULT,seqno))
	{
		//this is an recombin pkg
		result = true;
		if(NULL == tcb->recomblst)
		{
			secbuf = __Tcp_AllocReCombNode();
			secbuf->lsthead = pkg;
			secbuf->lsttail = pkg;
			secbuf->seqno = seqno;
			secbuf->datalen = pkg->datalen;
			secbuf->nxt = NULL;
			tcb->recomblst = secbuf;
		}
		else
		{
			tmpsec1 = tcb->recomblst;
			tmpsec2 = tmpsec1->nxt;
			if(__Tcp_CheckInTargetArea(tcb->rcvnxt,tmpsec1->seqno,seqno))
			{
				//check if it is just btween rcvnxt and sec1
				if(tmpsec1->seqno == (seqno + pkg->datalen))
				{
					pkg->partnext = tmpsec1->lsthead;
					tmpsec1->lsthead = pkg;
					tmpsec1->seqno = seqno;
					tmpsec1->datalen+=pkg->datalen;
				}
				else
				{
					//alloc a new secbuf insert before sec1
					secbuf = __Tcp_AllocReCombNode();
					secbuf->lsthead = pkg;
					secbuf->lsttail = pkg;
					secbuf->seqno = seqno;
					secbuf->datalen = pkg->datalen;
					secbuf->nxt = tmpsec1;
					tcb->recomblst = secbuf;
				}
			}
			else
			{
				while(NULL != tmpsec2)
				{
					if(__Tcp_CheckInTargetArea(tmpsec1->seqno,tmpsec2->seqno,\
											   seqno))
					{
						break; //binggo, we have got one
					}
					else
					{
						tmpsec1 = tmpsec2;
						tmpsec2 = tmpsec2->nxt;
					}
				}
				if(NULL == tmpsec2)
				{
					if((tmpsec1->seqno+ tmpsec1->datalen) == seqno)
					{

						//just add it to the sec1
						tmpsec1->lsttail->partnext = pkg;
						tmpsec1->lsttail = pkg;
						tmpsec1->datalen+=pkg->datalen;
					}
					else
					{
						//alloc an new secbuf and add it to rb tail
						secbuf = __Tcp_AllocReCombNode();
						secbuf->lsthead = pkg;
						secbuf->lsttail = pkg;
						secbuf->seqno = seqno;
						secbuf->datalen = pkg->datalen;
						secbuf->nxt = NULL;
						tmpsec1->nxt =secbuf;
					}
				}
				else
				{
					if(((tmpsec1->seqno + tmpsec1->datalen)== seqno)&&\
							((seqno + pkg->datalen)== tmpsec2->seqno))
					{
						//this pkg could connect tmpsec1 and tmpsec2
						//so add all sec2 and pkg to sec1
						tmpsec1->lsttail->partnext = pkg;
						pkg->partnext = tmpsec2->lsthead;
						tmpsec1->lsttail = tmpsec2->lsttail;
						tmpsec1->datalen+= pkg->datalen+ tmpsec2->datalen;
					}
					else if((tmpsec1->seqno + tmpsec1->datalen)== seqno)
					{
						//add pkg to sec1 tail
						tmpsec1->lsttail->partnext = pkg;
						tmpsec1->lsttail = pkg;
						tmpsec1->datalen+=pkg->datalen;

					}
					else if((seqno + pkg->datalen)== tmpsec2->seqno)
					{
						//add pkg to sec2 head
						pkg->partnext = tmpsec2->lsthead;
						tmpsec2->lsthead = pkg;
						tmpsec2->datalen+=pkg->datalen;
						tmpsec2->seqno = seqno;
					}
					else
					{
						//alloc an new secbuf to insert between sec1 and sec2
						secbuf = __Tcp_AllocReCombNode();
						secbuf->lsthead = pkg;
						secbuf->lsttail = pkg;
						secbuf->seqno = seqno;
						secbuf->datalen = pkg->datalen;
						secbuf->nxt = NULL;
						tmpsec1->nxt = secbuf;
						secbuf->nxt = tmpsec2;
					}
				}
			}
		}
		//it is an recombin pkg, so we'd better ack it now
		result = true;
	}
	return result;
}

// =============================================================================
// 函数功能：__Tcp_SSynRcvProcess
//          当前状态为SSynSent的处理
// 输入参数：tcb, tcb控制块
//           hdr, tcp 包头
//           pkg, 收到的数据包
// 输出参数：
// 返回值  ：true,该包留在套接字缓冲区， false，该套接字未留在套接字缓冲区
// 说明    :here we are just interested in the ack to the syn we has snd
// =============================================================================
bool_t __Tcp_SSynRcvProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                    tagNetPkg *pkg)
{
	bool_t result;
	u32    newackno;
	tagSocket *lfd;
	tagSocket *rfd;
	tagTcpTnode *tnode;
	
	result = false;
	if(CN_TCP_FLAG_ACK&hdr->flags)
	{
		newackno = ntohl(hdr->ackno);
		if(newackno == tcb->sndnxt)
		{
			//转入ESTABLISH状态
			tcb->state = EN_TCPSTATE_ESTABLISHED;
			//deal the ack
			__Tcp_DealAck(tcb,hdr);
            //更新本地状态
			rfd = tcb->owner;
			lfd = rfd->hungnode;
			tnode = (tagTcpTnode*)lfd->cb;
			//待ACCEPT的增加，当然阻塞状态下有可能要激活CONSYNC信号量的
			tnode->connum2accept++;
			tnode->connum++;
			Lock_SempPost(lfd->casync);
			if(0 == (CN_SOCKET_ACCEPT & lfd->status))
			{
				lfd->status |= CN_SOCKET_ACCEPT;
				Multiplex_Set(lfd->multiio, lfd->status);
			}
		}	
	}
	return result;
}
// =============================================================================
// 函数功能：__Tcp_CSynSentSProcess
//        当前状态为CSynSent的处理
// 输入参数：tcb, tcb控制块
//           hdr, tcp 包头
//           pkg, 收到的数据包
// 输出参数：
// 返回值  ：true,该包留在套接字缓冲区， false，该套接字未留在套接字缓冲区
// 说明    :we only intereted in the pkg include syn and ack
// =============================================================================
bool_t __Tcp_CSynSentSProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                      tagNetPkg *pkg)
{
	bool_t result;
	u32  newackno;
	tagSocket *rfd;
	
	result = false;
	if((hdr->flags & CN_TCP_FLAG_ACK)&&(hdr->flags &CN_TCP_FLAG_SYN))
	{
		newackno = ntohl(hdr->ackno);
		if(newackno == tcb->sndnxt) //正是我们需要的报文
		{
			//处理OP选项，主要是MSS
			__Tcp_OptDeal(tcb,hdr);
			//更新窗口和接收号
			tcb->rcvnxt = ntohl(hdr->seqno) + 1;
			//转入到稳定链接状态
			tcb->state = EN_TCPSTATE_ESTABLISHED;
			//deal the ack
			__Tcp_DealAck(tcb,hdr);
			__Tcp_MsgSend(tcb, CN_TCP_FLAG_ACK, NULL, 0, NULL, tcb->sndnxt);
			//激活本地端的consync，因为本地端的CONNECT还在等待我们的建立链接
			rfd = tcb->owner;
			//直接激活就是了
			Lock_SempPost(rfd->casync);
		}
	}
	return result;
}
// =============================================================================
// 函数功能：__Tcp_EstablishedProcess
//        deal the established state
// 输入参数：tcb, tcb控制块
//        hdr, tcp 包头
//        pkg, 收到的数据包
// 输出参数：
// 返回值  ：true,该包留在套接字缓冲区， false，该套接字未留在套接字缓冲区
// 说明    :when in established state, we are only intereted in three types pkg:
//      1,normal data
//      2,only fin pkg,this means the pkg == NULL
//      3,fin with normal data,the last two types are dealed in seqno deal
// =============================================================================
bool_t __Tcp_EstablishedProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                         tagNetPkg *pkg)
{
	bool_t result;
	u32    seqno;

	result = false;
	seqno =ntohl(hdr->seqno);
	if(CN_TCP_FLAG_FIN & hdr->flags)
	{
		//this is the only fin,if not normal fin, we just ignore it
		if(seqno == tcb->rcvnxt)
		{
			//this is just the fin that need
			tcb->rcvnxt++;
			seqno++;
			tcb->state = EN_TCPSTATE_CLOSE_WAIT;
			//if any data with this fin
			if(NULL != pkg)
			{
				if(tcb->channelstate.krcv)
				{
					result = __Tcp_DealRcvData(tcb,seqno,pkg);
				}
			}
			tcb->channelstate.krcv = 0;
			if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
			{
				tcb->owner->status |= CN_SOCKET_READALBE;
				Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
			}
			Lock_SempPost(tcb->rbuf.bufsync);
		}
	}
	else if(NULL != pkg)
	{
		if(tcb->channelstate.krcv)
		{
			result = __Tcp_DealRcvData(tcb,seqno,pkg);
			if(tcb->rbuf.buflen > tcb->rbuf.triglevel)
			{
				Lock_SempPost(tcb->rbuf.bufsync);
				if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
				{
					tcb->owner->status |= CN_SOCKET_READALBE;
					Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
				}
			}
		}
	}
	else
	{

	}
	//deal the ack
	 __Tcp_DealAck(tcb, hdr);
	//anyway open the acktimer if data reached
	if((NULL != pkg)||(CN_TCP_FLAG_FIN & hdr->flags)||(CN_TCP_FLAG_SYN & hdr->flags))
	{
		if(0 == (tcb->timerflag &CN_TCP_TIMER_ACK))
		{
			tcb->timerflag |= CN_TCP_TIMER_ACK;
			tcb->acktimer = CN_TCP_TICK_ACK;
		}
	}
	return result;
}
// =============================================================================
// 函数功能：__Tcp_Finwait1SProcess
//          当前状态为FIN_WAIT1的处理
// 输入参数：tcb, tcb控制块
//        hdr, tcp 包头
//        pkg, 收到的数据包
// 输出参数：
// 返回值  ：true,该包留在套接字缓冲区， false，该套接字未留在套接字缓冲区
// 说明    :when we shutdown the send channel of our own record, we will state at this
//      state.of course, the interesting pkg we need is:
//      1,ack the fin we sent,we will goto the fin_wait2 state
//      2,ack the fin we sent and send the fin, we will goto the time_wait state
//      3,only send the fin, not ack the fin we sent
//      all of above maybe include the valid data
// =============================================================================
bool_t __Tcp_Finwait1SProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                      tagNetPkg *pkg)
{
	bool_t result;
	u32  seqno;
	u32  ackno;
	
	result = false;
	seqno = ntohl(hdr->seqno);
	ackno = ntohl(hdr->ackno);
	if(ackno == tcb->sndnxt)
	{
		//rcv the ack to the fin we sent,if only ack we will turn to fin2, if
		//also rcv the fin, turn to time wait
		if((seqno == tcb->rcvnxt)&&(CN_TCP_FLAG_FIN & hdr->flags))
		{
			//also rcv the fin
			tcb->rcvnxt++;
			seqno++;
			//if any data with this fin
			if(NULL != pkg)
			{
				if(tcb->channelstate.krcv)
				{
					result = __Tcp_DealRcvData(tcb,seqno,pkg);
				}
			}
			tcb->channelstate.krcv = 0;
			if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
			{
				tcb->owner->status |= CN_SOCKET_READALBE;
				Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
			}
			Lock_SempPost(tcb->rbuf.bufsync);

			tcb->state = EN_TCPSTATE_TIME_WAIT;
		}
		else
		{
			//only rcv the ack to the fin we snt
			//if any data
			if(NULL != pkg)
			{
				if(tcb->channelstate.krcv)
				{
					result = __Tcp_DealRcvData(tcb,seqno,pkg);
					if(tcb->rbuf.buflen > tcb->rbuf.triglevel)
					{
						Lock_SempPost(tcb->rbuf.bufsync);
						if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
						{
							tcb->owner->status |= CN_SOCKET_READALBE;
							Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
						}
					}
				}
			}

			tcb->state = EN_TCPSTATE_FIN_WAITE2;
		}
	}
	else if((seqno == tcb->rcvnxt)&&(CN_TCP_FLAG_FIN & hdr->flags))
	{
		//rcv the remote fin, turn to CLOSING
		tcb->rcvnxt++;
		seqno++;
		//if any data with this fin
		if(NULL != pkg)
		{
			if(tcb->channelstate.krcv)
			{
				result = __Tcp_DealRcvData(tcb,seqno,pkg);
			}
		}
		tcb->channelstate.krcv = 0;
		if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
		{
			tcb->owner->status |= CN_SOCKET_READALBE;
			Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
		}
		Lock_SempPost(tcb->rbuf.bufsync);

		tcb->state = EN_TCPSTATE_CLOSING;
	}
	else if(NULL != pkg)
	{
		if(tcb->channelstate.krcv)
		{
			result = __Tcp_DealRcvData(tcb,seqno,pkg);
			if(tcb->rbuf.buflen > tcb->rbuf.triglevel)
			{
				Lock_SempPost(tcb->rbuf.bufsync);
				if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
				{
					tcb->owner->status |= CN_SOCKET_READALBE;
					Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
				}
			}
		}
	}
	else
	{

	}
	//deal the ack
	 __Tcp_DealAck(tcb, hdr);
	//anyway open the acktimer if data reached
	if((NULL != pkg)||(CN_TCP_FLAG_FIN & hdr->flags)||(CN_TCP_FLAG_SYN & hdr->flags))
	{
		if(0 == (tcb->timerflag &CN_TCP_TIMER_ACK))
		{
			tcb->timerflag |= CN_TCP_TIMER_ACK;
			tcb->acktimer = CN_TCP_TICK_ACK;
		}
	}

	return result;
}
// =============================================================================
// 函数功能：__Tcp_Finwait2SProcess
//          当前状态为FIN_WAIT2的处理
// 输入参数：tcb, tcb控制块
//        hdr, tcp 包头
//        pkg, 收到的数据包
// 输出参数：
// 返回值  ：true,该包留在套接字缓冲区， false，该套接字未留在套接字缓冲区
// 说明    :when we shutdown the send channel of our own record, and we rcv the remote
//      confirm of the fin we snt, we will state at this
//      state.of course, the interesting pkg we need is:
//      1, the remote fin, we turn to the TIME_WAIT
//      all of above maybe include the valid data
// =============================================================================
bool_t __Tcp_Finwait2SProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                      tagNetPkg *pkg)
{
	bool_t result;
	u32  seqno;
	
	result = false;
	seqno = ntohl(hdr->seqno);
	//if rcv the remote fin ,we will turn to time wait
	if((seqno == tcb->rcvnxt)&&(CN_TCP_FLAG_FIN & hdr->flags))
	{
		//also rcv the fin
		tcb->rcvnxt++;
		seqno++;
		//if any data with this fin
		if(NULL != pkg)
		{
			if(tcb->channelstate.krcv)
			{
				result = __Tcp_DealRcvData(tcb,seqno,pkg);
			}
		}
		tcb->channelstate.krcv = 0;
		if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
		{
			tcb->owner->status |= CN_SOCKET_READALBE;
			Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
		}
		Lock_SempPost(tcb->rbuf.bufsync);

		tcb->state = EN_TCPSTATE_TIME_WAIT;
	}
	else if(NULL != pkg)
	{
		if(tcb->channelstate.krcv)
		{
			result = __Tcp_DealRcvData(tcb,seqno,pkg);
			if(tcb->rbuf.buflen > tcb->rbuf.triglevel)
			{
				Lock_SempPost(tcb->rbuf.bufsync);
				if(0 ==(CN_SOCKET_READALBE & tcb->owner->status))
				{
					tcb->owner->status |= CN_SOCKET_READALBE;
					Multiplex_Set(tcb->owner->multiio, tcb->owner->status);
				}
			}
		}
	}
	else
	{

	}
	//deal the ack
	 __Tcp_DealAck(tcb, hdr);
	//anyway open the acktimer if data reached
	if((NULL != pkg)||(CN_TCP_FLAG_FIN & hdr->flags)||(CN_TCP_FLAG_SYN & hdr->flags))
	{
		if(0 == (tcb->timerflag &CN_TCP_TIMER_ACK))
		{
			tcb->timerflag |= CN_TCP_TIMER_ACK;
			tcb->acktimer = CN_TCP_TICK_ACK;
		}
	}

	return result;
}
// =============================================================================
// 函数功能：__Tcp_ClosingProcess
//        当前状态为CLOSING的处理
// 输入参数：tcb, tcb控制块
//        hdr, tcp 包头
//        pkg, 收到的数据包
// 输出参数：
// 返回值  ：always false
// 说明    :We state at this state because we has received the fin of remote, because this
//      closing has been started by ourselves, so we just ignore any data of remote
//      1,if any data or fin ,just turn on the acktimer
//      2,if the ack is to the fin we sent, turn to time_wait state
// =============================================================================
bool_t __Tcp_ClosingProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                    tagNetPkg *pkg)
{
	u32  ackno;
	
	ackno = ntohl(hdr->ackno);
	if(ackno == tcb->sndnxt)
	{
		tcb->state = EN_TCPSTATE_TIME_WAIT;
	}
	//deal the ack
	 __Tcp_DealAck(tcb, hdr);
	//anyway open the acktimer if data reached
	if((NULL != pkg)||(CN_TCP_FLAG_FIN & hdr->flags))
	{
		if(0 == (tcb->timerflag &CN_TCP_TIMER_ACK))
		{
			tcb->timerflag |= CN_TCP_TIMER_ACK;
			tcb->acktimer = CN_TCP_TICK_ACK;
		}
	}
	return false;
}
// =============================================================================
// 函数功能：__Tcp_TimeWaitSProcess
//          当前状态为FIN_WAIT2的处理
// 输入参数：tcb, tcb控制块
//        hdr, tcp 包头
//        pkg, 收到的数据包
// 输出参数：
// 返回值  ：always false
// 说明    :this state means that local and remote agree to turn off the channel, and
//      the channel has been shutdown, so we could ignore any data
//      if the fin or data ,this means the last ack has been missed,
// =============================================================================
bool_t __Tcp_TimeWaitSProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                      tagNetPkg *pkg)
{
	//anyway open the acktimer if data reached
	if((NULL != pkg)||(CN_TCP_FLAG_FIN & hdr->flags))
	{
		if(0 == (tcb->timerflag &CN_TCP_TIMER_ACK))
		{
			tcb->timerflag |= CN_TCP_TIMER_ACK;
			tcb->acktimer = CN_TCP_TICK_ACK;
		}
	}
	return false;
}
// =============================================================================
// 函数功能：__Tcp_CloseWaitSProcess
//          当前状态为FIN_WAIT2的处理
// 输入参数：tcb, tcb控制块
//        hdr, tcp 包头
//        pkg, 收到的数据包
// 输出参数：
// 返回值  ：always false
// 说明    :this state means the remote has shutdown the channel, and we has confirmed
//      the shutdown, but for reason, we can't shutdown it now;maybe the ack to the remote
//      fin missed; at the state we still could transmit the data, so we must
//      deal the ack
// =============================================================================
bool_t __Tcp_CloseWaitSProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                       tagNetPkg *pkg)
{
	//deal the ack
	 __Tcp_DealAck(tcb, hdr);
	//anyway open the acktimer if data reached
	if((NULL != pkg)||(CN_TCP_FLAG_FIN & hdr->flags))
	{
		if(0 == (tcb->timerflag &CN_TCP_TIMER_ACK))
		{
			tcb->timerflag |= CN_TCP_TIMER_ACK;
			tcb->acktimer = CN_TCP_TICK_ACK;
		}
	}
	return false;
}

// =============================================================================
// 函数功能：__Tcp_LastAckSProcess
//          当前状态为LAST ACK的处理
// 输入参数：tcb, tcb控制块
//           hdr, tcp 包头
//           pkg, 收到的数据包
// 输出参数：
// 返回值  ：always false
// 说明    :this condition meas that we also snt the fin at the request of remote
//      if we rcv the confirmed ack to the fin we sent, we turn to another state
// =============================================================================
bool_t __Tcp_LastAckSProcess(tagTcpTcb *tcb, tagTcpHdr *hdr,\
							 tagNetPkg *pkg)
{
	u32    ackno;

	ackno = ntohl(hdr->ackno);
	if(ackno == tcb->sndnxt)
	{
		//the ack to the fin we sent
		if(tcb->channelstate.close)
		{
			tcb->state = EN_TCPSTATE_CLOSED;
		}
		else
		{
			tcb->state = EN_TCPSTATE_2FREE;
		}
	}
	//deal the ack
	 __Tcp_DealAck(tcb, hdr);
	//anyway open the acktimer if data reached
	if((NULL != pkg)||(CN_TCP_FLAG_FIN & hdr->flags))
	{
		if(0 == (tcb->timerflag &CN_TCP_TIMER_ACK))
		{
			tcb->timerflag |= CN_TCP_TIMER_ACK;
			tcb->acktimer = CN_TCP_TICK_ACK;
		}
	}

	return false;
}
// =============================================================================
// 函数功能：__Tcp_RstPkgDeal
//          RST报文处理
// 输入参数：tcb,所发送给的TCB
//           hdr,收到的RST报文
// 输出参数：
// 返回值  ：空
// 说明    :仅仅处理RST事宜
// =============================================================================
void __Tcp_RstPkgDeal(tagTcpTcb *tcb, tagTcpHdr *hdr)
{
	tagSocket *sock;
	
	sock = tcb->owner;
	//ack报文必须是自己发送的报文的序列号的窗口范围之类，大于等于第一个未确认，小于等于
	//最后一个待确认的，即保证和自己通话的对端不是伪造的或者恶意攻击的
	if(__Tcp_CheckInTargetArea(tcb->snduna,tcb->sndnxt,ntohl(hdr->ackno)))
	{
		printk("Rcv the rst pkg!\n\r");
		__Tcp_RstTcb(tcb);
		
		//如果当前的本地端还处于链接状态，则应该激活
		if(EN_SOCKET_LOCAL_CONNECT == sock->appstate)
		{
			Lock_SempPost(sock->casync);
		}
		else
		{
			if(1 == tcb->channelstate.open)
			{
				//已经被打开了，应该通知可读写
				tcb->owner->status |= CN_SOCKET_READALBE|CN_SOCKET_WRITEALBE;
				Multiplex_Set(tcb->owner->multiio, tcb->owner->status);

				//post the snd and rcv
				Lock_SempPost(tcb->sbuf.bufsync);
				Lock_SempPost(tcb->rbuf.bufsync);
			}
			else
			{
				//还未打开过，可以释放掉了
				tcb->state = EN_TCPSTATE_2FREE;
			}
		}
	}
}
// =============================================================================
// 函数功能：__Tcp_DealRemotePkg
//          激活发送线程
// 输入参数：socket,待处理的远程套接字
//           tcphdr,TCP包头
//           pkg,  远端发来的数据包
// 输出参数：
// 返回值  ：true, 数据包被挂在socket队列中，false，没有挂在socket队列中
// 说明    :处理的选项包括MSS SACK，其他选项暂时不支持
// =============================================================================
bool_t __Tcp_DealRemotePkg(tagTcpTcb *tcb, tagTcpHdr *hdr,\
		                 tagNetPkg *pkg)
{
   	bool_t  result;

   	result = false;
   	//报文的标记处理
   	if(hdr->flags & CN_TCP_FLAG_RST)//RST报文统一处理，这样就可以加速
   	{
   		__Tcp_RstPkgDeal(tcb,hdr);
   	}
   	else  
   	{
   		//rcv the pkg means we still in the communication, so reset the keepalive
   		tcb->keeptimer = CN_TCP_TICK_KEEPALIVE;
   		//状态机处理
   		switch(tcb->state)
   		{
   			case EN_TCPSTATE_CSYNSNT:
   				result = __Tcp_CSynSentSProcess(tcb, hdr, pkg);
   				break;
   			case EN_TCPSTATE_SSYNRCV:
   				result = __Tcp_SSynRcvProcess(tcb, hdr,pkg);
   				break;
   			case EN_TCPSTATE_ESTABLISHED:
   				result = __Tcp_EstablishedProcess(tcb, hdr,pkg);
   				break;
   			case EN_TCPSTATE_FIN_WAITE1:
   				result = __Tcp_Finwait1SProcess(tcb, hdr, pkg);
   				break;
   			case EN_TCPSTATE_FIN_WAITE2:
   				result = __Tcp_Finwait2SProcess(tcb, hdr,pkg);
   				break;
   			case EN_TCPSTATE_CLOSING:
   				result = __Tcp_ClosingProcess(tcb,hdr,pkg);
   				break;
   			case EN_TCPSTATE_TIME_WAIT:
   				result = __Tcp_TimeWaitSProcess(tcb, hdr, pkg);
   				break;
   			case EN_TCPSTATE_CLOSE_WAIT:
   				result = __Tcp_CloseWaitSProcess(tcb,hdr,pkg);
   				break;
   			case EN_TCPSTATE_LAST_ACK:
   				result = __Tcp_LastAckSProcess(tcb,hdr,pkg);
   				break;
   			default:
   				result = false;
   				break;
   		}
   	}
    return result;
}
// =============================================================================
// 函数功能：Tcp_RecvProcess
//          TCP接收处理函数(处理IP层传递过来的数据包)
// 输入参数：ipsrc,源IP
//        ipdst,目的IP
//        pkg,TCP数据包
//        checksum,是否已经做过了校验
// 输出参数：
// 返回值  ：true 成功 false失败
// 说明    :deal the pkg rcved for tcp
// =============================================================================
bool_t Tcp_RecvProcess(u32 ipsrc, u32 ipdst, \
		        tagNetPkg *pkg, u32 devfunc)
{
	bool_t  result ;  //该数据包是否已经添加到套接字中
	u16    portdst;
	u16    portsrc;
	tagTcpHdr *hdr;
	tagSocket *sockcon;
	tagSocket *socknode;
	tagTcpTcb   *tcb;
	tagTcpTnode *tnode;	
	tagNetPkg   *datapkg;
	result = false;
	if((INADDR_ANY == ipdst) || (INADDR_ANY == ipsrc) || (NULL == pkg))
	{
		goto __RCVPKG_DEALEND;  //para error
	}
	if(0 == (devfunc &CN_IPDEV_TCPICHKSUM))
	{
		//no hard chksum, we do the soft chksum
		if(false == __Tcp_ChksumRcvPkg(ipdst, ipsrc, pkg))
		{
			goto __RCVPKG_DEALEND;  //chksum failed
		}
	}
	hdr = (tagTcpHdr *)(pkg->buf + pkg->offset);
	if(pkg->datalen > (hdr->hdrlen*4))
	{
		pkg->datalen -= hdr->hdrlen*4;
		datapkg = pkg;
	}
	else
	{
		pkg->datalen = 0;
		datapkg = NULL;
	}
	pkg->offset += hdr->hdrlen*4;

	if(false == __Tcp_CheckTcpHdr(hdr))
	{
		goto __RCVPKG_DEALEND;  //hdr not normal
	}

	//ok,now we deal the pkg now, the pkg check success
	portdst = ntohs(hdr->portdst);
	portsrc = ntohs(hdr->portsrc);
	//有了IP和PORT,则可以找到通信的那个port了
	//查找本地SOCKET,并判断其状态，看是否合适再继续处理
	Lock_MutexPend(pgTcpSockTreeSync, CN_TIMEOUT_FOREVER);
	socknode = __Tcp_GetSocket(ipdst,ipsrc, portdst,portsrc);
	if(NULL != socknode)
	{
		Lock_MutexPend(socknode->sync, CN_TIMEOUT_FOREVER);
		if(EN_SOCKET_TREENODE == socknode->socktype)
		{
			//no server communication for the talk now, better to create one
			tnode = (tagTcpTnode *)socknode->cb;
			if((tnode->connum < tnode->connumlimit)&&\
			   (EN_SOCKET_LOCAL_LISTEN == socknode->appstate)&&\
			   (hdr->flags & CN_TCP_FLAG_SYN))
			{
				//新建立一个客户端
				sockcon =Socket_New();
				if(NULL != sockcon)
				{
					tcb = __Tcp_NewTcb();
					if(NULL != tcb)
					{
						sockcon->cb = (void *)tcb;
						sockcon->socktype = EN_SOCKET_TERMINAL;
						tcb->owner = sockcon;
					}
					else
					{
						Socket_Free(sockcon);
						sockcon = NULL;
					}
				}
			}
			if(NULL != sockcon)
			{
				//add this socket to the node
				if(NULL != tnode->clientlst)
				{
					sockcon->nxt = tnode->clientlst;
					tnode->clientlst->pre = sockcon;
				}
				tnode->clientlst = sockcon;
				sockcon->hungnode = socknode;
				sockcon->proto = &pgTcpProtoRsc->proto;
				sockcon->laddr.ip = ipdst;
				sockcon->laddr.port = portdst;
				tcb->raddr.ip = ipsrc;
				tcb->raddr.port = portsrc;
				sockcon->constate = socknode->constate;
				tcb->rcvnxt = htonl(hdr->seqno)+1;
				__Tcp_OptDeal(tcb, hdr);
				tcb->state = EN_TCPSTATE_SSYNRCV;
                __Tcp_Syn2Snd(tcb);
			}
			else
			{
				//主机不可达，RST报文
				__Tcp_RstRemoteRaw(ipdst, portdst,ipsrc, \
						portsrc,ntohl(hdr->seqno)+1);
			}
			Lock_MutexPost(socknode->sync);
			Lock_MutexPost(pgTcpSockTreeSync);
		}
		else
		{
			Lock_MutexPost(pgTcpSockTreeSync);
			sockcon =socknode;
			//认真处理远端的数据包
			tcb = (tagTcpTcb *)(sockcon->cb);
			result = __Tcp_DealRemotePkg(tcb,hdr,datapkg);
			Lock_MutexPost(socknode->sync);
		}
	}//end for fin an sock to deal the pkg
	else
	{
		Lock_MutexPost(pgTcpSockTreeSync);
		//snd the rst to chk
		if(0==(hdr->flags & CN_TCP_FLAG_RST))
		{
			__Tcp_RstRemoteRaw(ipdst, portdst,ipsrc, \
					portsrc,ntohl(hdr->seqno)+1);
		}
	}//主机不可达，RST报文

__RCVPKG_DEALEND:
	if(false == result)//该PKG未添加到套接字队列中去
	{
		//if the pkg not cached to the buf, then release it
		Pkg_LstFlagFree(pkg);
		result = true;
	}
	
	return result;
}

// =============================================================================
// 函数功能：__Tcp_DeleteClient
//        删除本地客户端
// 输入参数：tcb,指定的TCB控制块
// 输出参数：
// 返回值  ：
// 说明    :仅仅是删除，不做任何的保护
// =============================================================================
void __Tcp_DeleteClient(tagTcpTcb *tcb)
{
	tagSocket *sock;

	sock = tcb->owner;
	if(NULL != sock->pre)
	{
		sock->pre->nxt = sock->nxt;
	}
	else
	{
		pgTcpSockLstH = sock->nxt;
	}
	if(NULL != sock->nxt)
	{
		sock->nxt->pre = sock->pre;
	}
	else
	{
		pgTcpSockLstT = sock->pre;
	}
	//reset and free the tcb
	__Tcp_RstTcb(tcb);
	__Tcp_FreeTcb(tcb);

	Lock_SempDelete(sock->casync);
	Socket_Free(sock);
}
// =============================================================================
// 函数功能：__Tcp_DeleteServer
//        删除本地服务器
// 输入参数：待删除的socket
// 输出参数：
// 返回值  ：
// 说明    :仅仅是删除，不做任何的保护
// =============================================================================
void __Tcp_DeleteServer(tagSocket *sock)
{
	if(NULL != sock->pre)
	{
		sock->pre->nxt = sock->nxt;
	}
	else
	{
		pgTcpSockLstH = sock->nxt;
	}
	if(NULL != sock->nxt)
	{
		sock->nxt->pre = sock->pre;
	}
	else
	{
		pgTcpSockLstT = sock->pre;
	}
	//reset and free the tnode
	__Tcp_FreeTnode((tagTcpTnode *)sock->cb);

	Lock_SempDelete(sock->casync);
	Socket_Free(sock);
}
// =============================================================================
// 函数功能：__Tcp_DeleteServerSession
//        删除本地服务器的一个会话
// 输入参数：待删除的服务器的一个会话tcb
// 输出参数：
// 返回值  ：
// 说明    :仅仅是删除，不做任何的保护
// =============================================================================
void __Tcp_DeleteServerSession(tagTcpTcb *tcb)
{
	tagSocket *sock;
	tagTcpTnode *tnode;

	sock = tcb->owner;
	tnode = (tagTcpTnode *)sock->hungnode->cb;

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

	//reset and free the tcb
	__Tcp_RstTcb(tcb);
	__Tcp_FreeTcb(tcb);

	Socket_Free(sock);
}

// =============================================================================
// 函数功能：__Tcp_DeleteSockNode
//        删除未知的socknode,本地端的
// 输入参数：待删除的socket
// 输出参数：
// 返回值  ：
// 说明    :仅仅是删除，不做任何的保护
// =============================================================================
void __Tcp_DeleteSockNode(tagSocket *sock)
{
	if(NULL != sock->pre)
	{
		sock->pre->nxt = sock->nxt;
	}
	else
	{
		pgTcpSockLstH = sock->nxt;
	}
	if(NULL != sock->nxt)
	{
		sock->nxt->pre = sock->pre;
	}
	else
	{
		pgTcpSockLstT = sock->pre;
	}

	Lock_SempDelete(sock->casync);
	Socket_Free(sock);
}

// =============================================================================
// 函数功能：__Tcp_DealTcbTimer
//        处理TCB的定时器问题
// 输入参数：tcb,指定的TCB控制块
// 输出参数：
// 返回值  ：该sock是否要删除
// 说明    :仅仅是处理定时器的TIMER
// =============================================================================
bool_t __Tcp_DealTcbTimer(tagTcpTcb *tcb)
{
	bool_t result;
	u8  timerflag;

	result = false;

	if(tcb->state == EN_TCPSTATE_2FREE)
	{
		result = true;
	}
	else
	{
		timerflag = tcb->timerflag;
		//deal variouse timer
		if(timerflag & CN_TCP_TIMER_LINGER)
		{
			//linger timer
			if(tcb->lingertimer == 0)
			{
				result = true;
				goto __TIMER_DEALEND;
			}
			else
			{
				tcb->lingertimer--;
			}
		}
		if(timerflag &CN_TCP_TIMER_2ML)
		{
			//2ML TIMER
			if(0 == tcb->mltimer)
			{
				__Tcp_RstTcb(tcb);
				goto __TIMER_DEALEND;
			}
			else
			{
				tcb->mltimer--;
			}
		}
		if(timerflag &CN_TCP_TIMER_RETRANSMIT)
		{
			//TIME OUT TIMER
			__TcpRetransmitDeal(tcb,false);
		}
		if(timerflag &CN_TCP_TIMER_PERSIST)
		{
			//PERSIST TIMER
			if((0 == tcb->persistimer)&&(NULL != tcb->sbuf.pkglsthead))
			{
				printk("%s:do the persistimer\n\r");
                __Tcp_DataProbe(tcb);                
				tcb->persistimer = CN_TCP_TICK_PERSIST;
			}
			else
			{
				tcb->persistimer--;
			}
		}
		if(timerflag &CN_TCP_TIMER_KEEPALIVE)
		{
			//KEEP ALIVE TIMER
			if(0 == tcb->keeptimer)
			{
				printk("%s:do the keeptimer\n\r");
				 __Tcp_MsgSend(tcb,CN_TCP_FLAG_ACK, NULL, 1, NULL, tcb->snduna-1);
				 tcb->keeptimer = CN_TCP_TICK_KEEPALIVE;
			}
			else
			{
				tcb->keeptimer--;
			}
		}
		else if(timerflag &CN_TCP_TIMER_CORK)
		{
            //CORK TIMER//--TODO
		}
		if(timerflag &CN_TCP_TIMER_ACK)
		{
			//ACK TIMER
			printk("%s:do the acktimer\n\r");
			if(__Tcp_MsgSend(tcb, CN_TCP_FLAG_ACK, NULL, 0, NULL, tcb->sndnxt))
			{
				tcb->timerflag &=(~CN_TCP_TIMER_ACK);
			}
		}
	}

__TIMER_DEALEND:
	return result;
}
// =============================================================================
// 函数功能：Tcp_Tick
//          TCP协议的定时器线程的处理函数
// 输入参数：无
// 输出参数：
// 返回值  ：无
// 说明    :
// =============================================================================
void Tcp_Tick(void)
{
	tagSocket    *sockl1;
	tagSocket    *sockl1nxt;
	bool_t       sockl1del;
	tagSocket    *sockl2;
	tagSocket    *sockl2nxt;
	bool_t       sockl2del;
	tagTcpTnode  *tnode;
	tagTcpTcb    *tcb;
	//this is an never return loop
	while(1)
	{
		Lock_MutexPend(pgTcpSockTreeSync, CN_TIMEOUT_FOREVER);
		sockl1 = pgTcpSockLstH;
		while(NULL != sockl1)
		{
			//l1 layer
			Lock_MutexPend(sockl1->sync, CN_TIMEOUT_FOREVER);
			sockl1del = false;
			sockl1nxt = sockl1->nxt;
			if(EN_SOCKET_TREENODE == sockl1->socktype)
			{
				//this is an tree node, must chk its sub
				tnode = (tagTcpTnode *)sockl1->cb;
				sockl2 = tnode->clientlst;
				while(NULL != sockl2)
				{
					//l2 layer
					Lock_MutexPend(sockl2->sync, CN_TIMEOUT_FOREVER);
					sockl2del = false;
					sockl2nxt = sockl2->nxt;
					//do the server session timer deal
					tcb = (tagTcpTcb *)sockl2->cb;
					sockl2del = __Tcp_DealTcbTimer(tcb);

					if(false == sockl2del)
					{
						Lock_MutexPost(sockl2->sync);
					}
					else
					{
						__Tcp_DeleteServerSession(tcb);
					}
					sockl2 = sockl2nxt;
				}
				//tree node has no timer except close
				if((sockl1->appstate == EN_SOCKET_LOCAL_CLOSE)&&\
				   (0 == tnode->connum))
				{
					__Tcp_DeleteServer(sockl1);
					sockl1del = true;
				}
			}
			else if(EN_SOCKET_TERMINAL == sockl1->socktype)
			{
				//do the client timer deal
				tcb = (tagTcpTcb *)sockl1->cb;
				sockl1del = __Tcp_DealTcbTimer(tcb);

				if(true == sockl1del)
				{
					__Tcp_DeleteClient(tcb);
				}
			}
			else
			{
				//not certain yet, if closed ,then free it
				if(sockl1->appstate == EN_SOCKET_LOCAL_CLOSE)
				{
					__Tcp_DeleteSockNode(sockl1);
					sockl1del = true;
				}
			}
			if(false == sockl1del)
			{
				Lock_MutexPost(sockl1->sync);
			}
			sockl1 = sockl1nxt;
		}
		Lock_MutexPost(pgTcpSockTreeSync);
		//the thread delay for the tcp tick
		Djy_EventDelay(CN_TCP_TICK_TIME);
	}
	return;
}

// =============================================================================
// 函数功能：Tcp_Init
//          TCP协议初始化函数
// 输入参数：para,暂时无意义
// 输出参数：
// 返回值  ：true 成功 false失败
// 说明    :TCP的初始化的东西还是蛮多的
//       1,建立TCB内存池
//       2,建立定时器线程
//       3,建立发送线程
// =============================================================================
bool_t Tcp_Init(ptu32_t para)
{
	u16  evttid;
	tagTlayerProto proto;
	//创建套接字资源节点锁
	pgTcpSockTreeSync = Lock_MutexCreate(NULL);
	if(NULL == pgTcpSockTreeSync)
	{
		printk("Tcp:Create Local Mutex Failed\n\r");
		goto createlmutex_failed;
	}
	else
	{
		printk("Tcp:Create Local Mutex  Success\n\r");
	}
	//建立TCB内存池
	pgTcpTcbPool = Mb_CreatePool((void *)sgTcpTcbPool, CN_TCP_TCBSIZE,\
			                     sizeof(tagTcpTcb), 10, 0, NULL);
	if(NULL == pgTcpTcbPool)
	{
		printk("Tcp:Create TcpTcbPool Failed\n\r");
		goto createtcbpool_failed;
	}
	else
	{
		printk("Tcp:Create TcpTcbPool  Success\n\r");
	}
	//建立TNODE内存池
	pgTcpTnodePool = Mb_CreatePool((void *)sgTcpTnodePool, CN_TCP_TNODESIZE,\
			                     sizeof(tagTcpTnode), 10, 0, NULL);
	if(NULL == pgTcpTnodePool)
	{
		printk("Tcp:Create TcpTnodePool Failed\n\r");
		goto createtnodepool_failed;
	}
	else
	{
		printk("Tcp:Create TcpTnodePool  Success\n\r");
	}
	//create the unacked node mem
	__Tcp_UnackNodeInit();
	//create the recombination mem
	pgTcpReCombNodePool = Mb_CreatePool((void *)sgTcpReCombNodePool, CN_TCP_RECOMB_SECNODE,\
			             sizeof(tagTcpReCombNode), CN_TCP_RECOMB_SECNODE, CN_TCP_RECOMB_SECNODE*100, NULL);
	if(NULL == pgTcpReCombNodePool)
	{
		printk("Tcp:Create UnackedNodePool Failed\n\r");
		goto createrecombnodepool_failed;
	}	
	//注册TCP到传输层，方便套接字使用
	proto.socket = Tcp_Socket;
	proto.accept = Tcp_Accept;
	proto.bind = Tcp_Bind;
	proto.connect = Tcp_Connect;
	proto.getsockopt = Tcp_Getsockopt;
	proto.listen = Tcp_Listen;
	proto.recv = Tcp_Recv;
	proto.recvfrom = NULL;
	proto.send = Tcp_Send;
	proto.sendto = NULL;
	proto.setsockopt = Tcp_Setsockopt;
	proto.shutdown = Tcp_Shutdown;
	proto.close = Tcp_Close;
	proto.multiioadd = Tcp_MultiIoAdd;
	
	pgTcpProtoRsc = TPL_RegisterProto(AF_INET,SOCK_STREAM,0, &proto);
	if(NULL == pgTcpProtoRsc)
	{
		printk("Tcp:Register Tcp in TPL Failed\n\r");
		goto resistertcp_failed;
	}
	else
	{
		printk("Tcp:Register Tcp in TPL Success\n\r");
	}

	//建立定时器线程
    evttid= Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1, 
    		(ptu32_t (*)(void))Tcp_Tick,NULL, 0x400, "TcpTick");
	if (evttid != CN_EVTT_ID_INVALID)
	{
		evttid = Djy_EventPop(evttid, NULL, 0, 0, 0, 0);
	}
	else
	{
		printk("Tcp:Register TcpTick Failed\n\r");
	}

	//init the syn option
	//添加MSS选项
	sgSynOption.mss.kind = EN_TCP_OPT_MSS;
	sgSynOption.mss.len = 4;
	sgSynOption.mss.mss = htons(CN_TCP_MSS_DEFAULT);
	sgSynOption.winscale.resvered = EN_TCP_OPT_NONE;
	sgSynOption.winscale.kind = EN_TCP_OPT_SCALE;
	sgSynOption.winscale.len = 3;
	sgSynOption.winscale.shiftbits = 0;

	return true;
	
resistertcp_failed:
createrecombnodepool_failed:
	Mb_DeletePool(pgTcpTnodePool);
	pgTcpTnodePool = NULL;

createtnodepool_failed:
	Mb_DeletePool(pgTcpTcbPool);
    pgTcpTcbPool = NULL;
    
createtcbpool_failed:
	Lock_MutexDelete(pgTcpSockTreeSync);
	pgTcpSockTreeSync = NULL;

createlmutex_failed:
	return false;
}



