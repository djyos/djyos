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
// 模块描述: telnet.c
// 模块版本: V1.00
// 创建人员: ZQF
// 创建时间: 下午2:12:24/2015-1-26
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stdio.h"
#include "stddef.h"
#include "stdlib.h"
#include "endian.h"
#include "string.h"

#include "os.h"
#include "stddev.h"

#include "socket.h"

#define CN_TELNET_SERVERPORT  23

//当发送消息时应该是把消息广播给所有的客户端
typedef struct _tagTelCB
{
	struct _tagTelCB *pre;
	struct _tagTelCB *nxt;
	int sockfd;
	u32  status; //后续密码认证等状态
}tagTelCB;

struct tagMutexLCB *pgTelClientSync;       //客户端队列同步
tagTelCB  *pgTelClst = NULL;               //所有的客户端队列
tagTelCB  *pgTelCBLst = NULL;              //空闲的客户端队列
#define CN_TELNET_CLIENTMAX    0X10
tagTelCB  tgTelCBTab[CN_TELNET_CLIENTMAX];               //客户端控制块
struct tagMultiplexSetsCB        *pgTelReadSets = NULL;  //客户端发送信息集
static s32  sgTelnetDevStdin = -1;                       //telnet模拟的输入终端
static s32  sgTelnetDevStdout = -1;                      //telnet模拟的输出终端
// =============================================================================
// 函数功能：分配一个客户端控制块
// 输入参数：空
// 输出参数：空
// 返回值  ：分配到的客户端资源控制块
// 说明    :
// =============================================================================
tagTelCB *__Telnet_MallocCB()
{
	atom_low_t  atom;
	tagTelCB *result = NULL;

	atom = Int_LowAtomStart();
	if(NULL != pgTelCBLst)
	{
		result = pgTelCBLst;
		pgTelCBLst = result->nxt;
		pgTelCBLst->pre = NULL;
		result->nxt = NULL;
		result->sockfd = -1;
	}
	Int_LowAtomEnd(atom);
	return result;
}
// =============================================================================
// 函数功能：释放一个客户端控制块
// 输入参数：空
// 输出参数：空
// 返回值  ：无
// 说明    :
// =============================================================================
void __Telnet_FreeCB(tagTelCB *client)
{
	atom_low_t  atom;
	atom = Int_LowAtomStart();
	if(NULL != client)
	{
		client->pre = NULL;
		client->nxt = pgTelCBLst;
		if(NULL != pgTelCBLst)
		{
			pgTelCBLst->pre = client;
		}

	}
	Int_LowAtomEnd(atom);
	return ;
}
// =============================================================================
// 函数功能：添加一个客户端
// 输入参数：client,客户端控制块
// 输出参数：空
// 返回值  ：无
// 说明    :
// =============================================================================
void __Telnet_AddClient(tagTelCB *client)
{
	if(Lock_MutexPend(pgTelClientSync, CN_TIMEOUT_FOREVER))
	{
		client->pre = NULL;
		client->nxt = pgTelClst;
		if(NULL != pgTelClst)
		{
			pgTelClst->pre = client;
		}
		pgTelClst = client;
		Lock_MutexPost(pgTelClientSync);
	}
	return;
}

// =============================================================================
// 函数功能：删除一个客户端
// 输入参数：client,客户端控制块
// 输出参数：空
// 返回值  ：无
// 说明    :
// =============================================================================
void __Telnet_DelClient(tagTelCB *client)
{
	if(Lock_MutexPend(pgTelClientSync, CN_TIMEOUT_FOREVER))
	{
		if(client == pgTelClst)
		{
			pgTelClst = client->nxt;
			if(NULL != pgTelClst)
			{
				pgTelClst->pre = NULL;
			}
		}
		else
		{
			if(NULL != client->nxt)
			{
				client->nxt->pre = client->pre;
				client->pre->nxt = client->nxt;
			}
			else
			{
				client->pre->nxt = NULL;
			}
		}
		Lock_MutexPost(pgTelClientSync);
	}
	return;
}
// =============================================================================
// 函数功能：查找socket对应的客户端
// 输入参数：client,客户端控制块
// 输出参数：空
// 返回值  ：tagTelCB，查找到的客户端
// 说明    :
// =============================================================================
tagTelCB* __Telnet_FindClient(int socket)
{
	tagTelCB *result;
	if(Lock_MutexPend(pgTelClientSync, CN_TIMEOUT_FOREVER))
	{
		result = pgTelClst;
		while(NULL != result)
		{
			if(result->sockfd == socket)
			{
				break;
			}
			else
			{
				result = result->nxt;
			}
		}
		Lock_MutexPost(pgTelClientSync);
	}
	return result;
}

#define CN_TELNET_WELCOM   "Welcome to the djyos telnet console!\n\r"

// =============================================================================
// 函数功能：服务器监听任务
// 输入参数：
// 输出参数：
// 返回值  ：
// 说明    :
// =============================================================================
ptu32_t Telnet_Listen(void)
{
	struct sockaddr_in sa_server;
	int sockserver;
	int sockclient;
	tagTelCB *client;
	int sockopt;

	sockserver = socket(AF_INET, SOCK_STREAM, 0);
	sa_server.sin_family = AF_INET;
	sa_server.sin_port = htons(CN_TELNET_SERVERPORT);
	sa_server.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockserver, (struct sockaddr *) &sa_server, sizeof(sa_server)) < 0)
	{
		printk("bind()");
		closesocket(sockserver);
		return 0;
	}

	if (listen(sockserver, 2) != 0)
	{
		printk("listen()");
		closesocket(sockserver);
		return 0;
	}
	//here we accept all the client
	while(1)
	{
		sockclient = accept(sockserver, NULL, 0);
		if(sockclient > 0)
		{
			//set it no nagle
			sockopt = 0;
			if(0 !=setsockopt(sockclient,IPPROTO_TCP,TCP_NODELAY,&sockopt,sizeof(sockopt)))
			{
				printk("Telnet:Set client nodelay failed");
			}
		    //snd the welcom msg
			send(sockclient,CN_TELNET_WELCOM,strlen(CN_TELNET_WELCOM)+1,0);
			//add the client to the read sets
			Socket_MultiplexAdd(pgTelReadSets,sockclient,CN_SOCKET_READALBE);
			client = __Telnet_MallocCB();
			if(NULL != client)
			{
				client->sockfd = sockclient;
				client->status = 0;
				__Telnet_AddClient(client);
			}
		}
	}
	return 0;//never reach here
}
#define CN_TELNET_MSGLENMAX 0x100
// =============================================================================
// 函数功能：服务器接收从客户端发送来的消息
// 输入参数：
// 输出参数：
// 返回值  ：
// 说明    :
// =============================================================================
ptu32_t Telnet_Rcv(void)
{
	int sockclient;
	tagTelCB *client;
	struct tagCharTermineralMsg msg={0,0};
	while(1)
	{
		sockclient = Multiplex_Wait(pgTelReadSets,NULL, CN_TIMEOUT_FOREVER);
		if(sockclient > 0)
		{
			msg.num = recv(sockclient,msg.input_char,CN_CHAR_BUF_LIMIT,0);
			if(msg.num > 0)
			{
//				msg.input_char[msg.num] = '\0';
//				printk("%s",msg.input_char);
				//ok, snd the msg to the stdin device
				msg.time = DjyGetTimeTick();
				Stddev_InputMsg(sgTelnetDevStdin,(u8*)&msg,sizeof(msg));
				//and snd the msg to the terminal, this will means that echo display
//				send(sockclient,msg.input_char,msg.num,0);
			}
			else
			{
				//find the client and del it from the queue, then free it
				client = __Telnet_FindClient(sockclient);
				__Telnet_DelClient(client);
				__Telnet_FreeCB(client);
				//del the multiio from the sets,and then shudown the client	
				Socket_MultiplexDel(pgTelReadSets,sockclient);
				shutdown(sockclient,SHUT_RDWR);
				closesocket(sockclient);
			}
		}
	}
	return 0;
}
// =============================================================================
// 函数功能：将本系统的消息发送给客户端
// 输入参数：
// 输出参数：
// 返回值  ：发送的字节数，-1 返回失败
// 说明    :
// =============================================================================
void Telnet_Snd(char *msg)
{
	int       len;
	tagTelCB  *client;
	if(NULL != msg)
	{
		len = strlen(msg);
		if(Lock_MutexPend(pgTelClientSync, CN_TIMEOUT_FOREVER))
		{
			//check all the client list
			client = pgTelClst;
			while(NULL != client)
			{
				if(client->sockfd > 0)
				{
					send(client->sockfd,msg,len,0);
					client = client->nxt;
				}
			}
			Lock_MutexPost(pgTelClientSync);
		}
	}
	return ;
}

bool_t __Telnet_TaskCreate(ptu32_t (*taskfunc)(void), char *taskname)
{
	bool_t result = false;
	u16    taskID;
	taskID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1, 
			taskfunc,NULL, 0x400, taskname);
	if (taskID != CN_EVTT_ID_INVALID)
	{
		taskID = Djy_EventPop(taskID, NULL, 0, 0, 0, 0);
		if(taskID != CN_EVENT_ID_INVALID)
		{
			result = true;
		}
	}
	return result;
}

// =============================================================================
// 函数功能：TelNet服务器的初始化
// 输入参数：
// 输出参数：
// 返回值  ：true 成功  false失败
// 说明    :
//       1,创建服务器监听主线程
//       2,创建客户端接收主线程
// =============================================================================
ptu32_t Telnet_Init(char *param)
{
	u32    i;
	//init the client CB
	memset((char *)&tgTelCBTab[0],0, sizeof(tagTelCB)*CN_TELNET_CLIENTMAX);
	pgTelCBLst = &tgTelCBTab[0];
	pgTelCBLst->sockfd = -1;
	for(i = 1;i <CN_TELNET_CLIENTMAX; i++)
	{
		tgTelCBTab[i].nxt = pgTelCBLst;
		pgTelCBLst->pre = &tgTelCBTab[i];
		pgTelCBLst = pgTelCBLst->pre;
		pgTelCBLst->sockfd = -1;
	}
	//create the read sets and mutex sync
	pgTelReadSets = Multiplex_Creat(CN_SOCKET_OR | CN_SOCKET_READALBE);
	if(NULL == pgTelReadSets)
	{
		printk("Telnet:Create ReadSets Failed\n\r");
	}
	pgTelClientSync = Lock_MutexCreate(NULL);
	if(NULL == pgTelClientSync)
	{
		printk("Telnet:Create Client Sync Failed\n\r");
	}
	
	//add the telnet to the stdin device tree
    sgTelnetDevStdin = Stddev_InstallDevice("netconsole", EN_CHAR_TERMINERAL,NULL);
    if(-1 == sgTelnetDevStdin)
    {
    	printk("Telnet:Install Stdin Device Failed\n\r");
    }
	//add the telnet to the stdout device tree

	//create server listen task
	if(false == __Telnet_TaskCreate(Telnet_Listen,"TelnetServerListen"))
	{
		printk("Telnet:Create Listen Task Failed!\n\r");
	}
	//create server rcv task
	if(false == __Telnet_TaskCreate(Telnet_Rcv,"TelnetServerRcv"))
	{
		printk("Telnet:Create Rcv Task Failed!\n\r");
	}
	
	//set the printf stdout device
	//printf_setstdout(Telnet_Snd);

	return 0;
}

