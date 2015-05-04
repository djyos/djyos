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
// 模块描述: 本文件主要是关于套接字的实现,提供基于BSD的统一接口模型
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 3:15:23 PM/Jul 15, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "os.h"

#include "socket.h"
#include "tpl.h"
//定义的内存池
//套接字内存池
#define CN_SOCKET_POOLSIZE   0x10
static tagSocket  sgSocketPool[CN_SOCKET_POOLSIZE];
static struct tagMemCellPool  *pgSocketPool = NULL;

#define CN_SOCKET_MAP_2USE         1
static tagSocket *sgSocketMap[CN_SOCKET_POOLSIZE];

// =============================================================================
// 函数功能：  __Socket_NewNo
//        alloc an new no to link the socket
// 输入参数：  无
// 输出参数： 
// 返回值  ： the link no if success, otherwise -1 failed
// 说明    ：
// =============================================================================
int __Socket_NewNo()
{
	static int newno = 0;
	int result;
	int i ;
	
	result = -1;
	atom_low_t atom;
	atom = Int_LowAtomStart();
	for(i =0; i <CN_SOCKET_POOLSIZE; i++ )
	{
		if(NULL == sgSocketMap[newno])
		{
			sgSocketMap[newno]= (tagSocket *)CN_SOCKET_MAP_2USE;
			result = newno;
			newno = (newno+1)%CN_SOCKET_POOLSIZE;
			i = CN_SOCKET_POOLSIZE;
		}
	}
	Int_LowAtomEnd(atom);
	
	return result;
}

// =============================================================================
// 函数功能：  __Socket_FreeInt
//        Free an link no
// 输入参数：  the no to free
// 输出参数： 
// 返回值  ：true success while false failed
// 说明    ：
// =============================================================================
bool_t __Socket_FreeInt(int fd)
{
	bool_t result;
	atom_low_t atom;

	result = false;	
	atom = Int_LowAtomStart();
	if((fd < CN_SOCKET_POOLSIZE)&&(fd >= 0))
	{
		sgSocketMap[fd] = (tagSocket *)NULL;
		result = true;
	}
	Int_LowAtomEnd(atom);
	
	return result;
}

// =============================================================================
// 函数功能：  __Socket_GetSocket
//           通过套接字号获取套接字
// 输入参数：  套接字号
// 输出参数： 
// 返回值  ：获取的套接字
// 说明    ：NULL表示参数不正确，或者没有该套接字
// =============================================================================
tagSocket * __Socket_GetSocket(int fd)
{
	tagSocket *result;	
	atom_low_t atom;
	
	result = NULL;
	atom = Int_LowAtomStart();
	if((fd < CN_SOCKET_POOLSIZE)&&(fd >= 0))
	{
		result = sgSocketMap[fd]; 
	}
	Int_LowAtomEnd(atom);
	
	return result;
}
// =============================================================================
// 函数功能：  socket
//        create an new socket
// 输入参数： family,所属域，如AF_INET AF_LOCAL等
//        type,类型，如SOCK_STREAM SOCK_DGRAM等
//        protocol,协议类型，一般的默认为0
// 输出参数：
// 返回值  ：套接字号，-1表示创建失败
// 说明    ：套接字创建不成功，可能是指定协议没有找到或者内存不足
// =============================================================================
//创建一个套接字
int socket(int family, int type, int protocol)
{
	int sockfd;
	tagTlayerProto  *proto;
	tagSocket      *sockstr;
	
	sockstr = NULL;
	sockfd = __Socket_NewNo();
	if(-1 != sockfd)
	{
		proto = TPL_GetProto(family, type, protocol);
		if(NULL != proto)
		{
			if(NULL != proto->socket)
			{
				sockstr = proto->socket(family, type, protocol);
				if(NULL != sockstr)
				{
					sgSocketMap[sockfd] = sockstr;
				}
			}
		}
	}
	if(NULL == sockstr)
	{
		__Socket_FreeInt(sockfd);
		sockfd = -1;
	}

	return  sockfd;
}

// =============================================================================
// 函数功能：  bind
//           为一个套接字绑定一个网络地址
// 输入参数：  sockfd,待绑定的套接字
//           myaddr,指定的网络地址
//           addrlen,网络地址长度
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：失败一般的是因为该网络地址冲突（端口号）
// =============================================================================
//创建一个套接字
int bind(int sockfd,struct sockaddr *myaddr, int addrlen)
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->bind)
			{
				result = sockstr->proto->bind(sockstr, myaddr, addrlen);
			}
		}
	}

	
	return  result;
}
// =============================================================================
// 函数功能：  listen
//           指定套接字为监听状态
// 输入参数：  sockfd,待指定的套接字
//           backlog,允许的链接个数，默认的为5
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：失败一般的是因为重复指定
// =============================================================================
int listen(int sockfd, int backlog)
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->listen)
			{
				result = sockstr->proto->listen(sockstr, backlog);
			}
		}
	}

	return  result;

}
// =============================================================================
// 函数功能：  accept
//           等待客户端的链接
// 输入参数：  sockfd,服务器端套接字
// 输出参数：  addr,连接的客户端地址
//           addrlen，地址长度
// 返回值  ：链接的客户端套接字，-1出错
// 说明    ：出错一般是因为非阻塞而且没有连接的客户端，或者超出链接限制数
// =============================================================================
int accept(int sockfd, struct sockaddr *addr, int *addrlen)
{
	int  result;
	tagSocket *sockstr;
	tagSocket *consocket;
	
	sockstr = NULL;
	result = __Socket_NewNo();
	if(-1 != result)
	{

		sockstr = __Socket_GetSocket(sockfd);
		if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
		{
			if(NULL != sockstr->proto)
			{
				if(NULL != sockstr->proto->accept)
				{
					consocket = sockstr->proto->accept(sockstr, addr,addrlen);
					if(NULL != consocket)
					{
						sgSocketMap[result] = consocket;
					}
				}
			}
		}
	
		if(NULL == consocket)
		{
			__Socket_FreeInt(result);
			result = -1;
		}
	}
	
	return  result;
}

// =============================================================================
// 函数功能：  connect
//           链接服务器
// 输入参数：  sockfd,客户端套接字
//           addr,连接的服务器地址
//           addrlen，地址长度
// 输出参数：  
// 返回值  ：-1出错，0成功
// 说明    ：
// =============================================================================
int connect(int sockfd, struct sockaddr *serv_addr, int addrlen) 
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->connect)
			{
				result = sockstr->proto->connect(sockstr, serv_addr, addrlen);
			}
		}
	}

	return  result;
}
// =============================================================================
// 函数功能：  send
//           向套接字发送数据
// 输入参数：  sockfd,目的套接字
//           msg,待发送的信息
//           len,信息长度
//           flags,一般为0
// 输出参数：  
// 返回值  ：-1出错，否则返回发送字节数
// 说明    ：
// =============================================================================
int send(int sockfd, const void *msg, int len, int flags)
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->send)
			{
				result = sockstr->proto->send(sockstr, msg, len, flags);
			}
		}
	}

	return  result;	
}
// =============================================================================
// 函数功能：  recv
//           读取接字数据
// 输入参数：  sockfd,目的套接字
//           flags,一般为0
// 输出参数：  
//           buf,收到的信息
//           len,buf长度
// 返回值  ：-1出错，否则返回收到字节数
// 说明    ：
// =============================================================================
int recv(int sockfd, void *buf,int len, unsigned int flags)
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->recv)
			{
				result = sockstr->proto->recv(sockstr, buf, len, flags);
			}
		}
	}

	return  result;
}
// =============================================================================
// 函数功能：  sendto
//           向目的网络地址发送数据
// 输入参数：  sockfd,目的套接字
//           msg,待发送的信息
//           len,信息长度
//           flags,一般为0
//           addr,目的网络地址
//           addrlen,目的网络地址长度
// 输出参数： 
// 返回值  ：-1出错，否则返回发送字节数
// 说明    ：
// =============================================================================
int sendto(int sockfd, const void *msg,int len, unsigned int flags,\
		  const struct sockaddr *addr, int addrlen)
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->sendto)
			{
				result = sockstr->proto->sendto(sockstr, msg, len, flags,addr, addrlen);
			}
		}	
	}

	
	return  result;	
}
// =============================================================================
// 函数功能：  recvfrom
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
int recvfrom(int sockfd,void *buf, int len, unsigned int flags,\
		    struct sockaddr *addr, int *addrlen)
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{	
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->recvfrom)
			{
				result = sockstr->proto->recvfrom(sockstr, buf, len, flags,addr, addrlen);
			}
		}
	}

	return  result;	
}
// =============================================================================
// 函数功能：  shutdown
//           关闭套接字通道
// 输入参数：  sockfd,目的套接字
//           how,关闭方法， 见SHUT_RD等定义
// 输出参数： 
// 返回值  ：false失败  true 成功
// 说明    ：
// =============================================================================
bool_t  shutdown(int sockfd, u32 how)
{
	bool_t  result;
	tagSocket *sockstr;
	
	result = false;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->shutdown)
			{
				result = sockstr->proto->shutdown(sockstr, how);
			}
		}	
	}

	return  result;	
}

// =============================================================================
// 函数功能：  close
//           关闭套接字
// 输入参数：  sockfd,目的套接字
// 输出参数：
// 返回值  ：false失败  true 成功
// 说明    ：
// =============================================================================
bool_t closesocket(int sockfd)
{
	bool_t  result;
	tagSocket *sockstr;

	result = false;
	sockstr = __Socket_GetSocket(sockfd);
	if((NULL != sockstr)&&((tagSocket *)CN_SOCKET_MAP_2USE != sockstr))
	{
		if(NULL != sockstr->proto)
		{
			if(NULL != sockstr->proto->close)
			{
				result = sockstr->proto->close(sockstr);
				if(result)
				{
					__Socket_FreeInt(sockfd);
				}
			}
		}
	}

	return  result;
}
// =============================================================================
// 函数功能：  setsockopt
//        设置套接字选项
// 输入参数：  sockfd,目的套接字
//        level,设置的层次，支持SOL_SOCKET、IPPROTO_TCP、IPPROTO_IP和IPPROTO_IPV6
//        optname,需设置选项
//        optval,缓冲区
//        optlen,缓冲区长度
// 输出参数： 
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int setsockopt(int sockfd, int level, int optname,\
		       const void *optval, int optlen)
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if(NULL != sockstr->proto)
	{
		if(NULL != sockstr->proto->setsockopt)
		{
			result = sockstr->proto->setsockopt(sockstr, level, optname,\
					                            optval,optlen);
		}
	}
	return  result;		
}
// =============================================================================
// 函数功能：  getsockopt
//        设置套接字选项
// 输入参数：  sockfd,目的套接字
//        level,设置的层次例如，支持的层次有SOL_SOCKET、IPPROTO_TCP
//        optname,需设置选项
//        optval,缓冲区
//        optlen,缓冲区长度
// 输出参数： 
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
int getsockopt(int sockfd, int level, int optname, void *optval,\
		       int *optlen)
{
	int  result;
	tagSocket *sockstr;
	
	result = -1;
	sockstr = __Socket_GetSocket(sockfd);
	if(NULL != sockstr->proto)
	{
		if(NULL != sockstr->proto->getsockopt)
		{
			result = sockstr->proto->getsockopt(sockstr, level, optname,\
					                            optval,optlen);
		}
	}

	return  result;		
}

// =============================================================================
// 函数功能：添加套接字到指定的多用复用集合     
// 输入参数：  MultiplexSets,指定的套接字集合
//        sock,待加入的套接字
//        SensingBit,需要监听的位
// 输出参数： 
// 返回值  ：true 成功 false失败
// 说明    ：
// =============================================================================
bool_t Socket_MultiplexAdd(struct tagMultiplexSetsCB *MultiplexSets,
                           int sock,u32 SensingBit)
{
	bool_t result;
	tagSocket *sockstr;
	
	result = false;
	if(NULL != MultiplexSets)
	{
		sockstr = __Socket_GetSocket(sock);
		if((NULL != sockstr)&&(NULL != sockstr->proto))
		{
			if(NULL != sockstr->proto->multiioadd)
			{
				result = sockstr->proto->multiioadd(MultiplexSets,\
						(ptu32_t)sock, SensingBit, sockstr);
			}
		}
	}
    
	return result;
}
// =============================================================================
// 函数功能：将套接字从指定的多用复用集合删除     
// 输入参数：  MultiplexSets,指定的套接字集合
//        sock,待删除的套接字
// 输出参数： 
// 返回值  ：true 成功 false失败
// 说明    ：
// =============================================================================
bool_t Socket_MultiplexDel(struct tagMultiplexSetsCB *MultiplexSets,int sock)
{
	bool_t result;
	tagSocket *sockstr;
	
	result = false;
	if(NULL != MultiplexSets)
	{
		sockstr = __Socket_GetSocket(sock);
		if((NULL != sockstr)&&(NULL != sockstr->proto))
		{
			if(NULL != sockstr->proto->multiiodel)
			{
				result = sockstr->proto->multiiodel(MultiplexSets,\
						(ptu32_t)sock, sockstr);
			}
		}
	}
    
	return result;
}

static tagSocket* sgDebugSocket;
// =============================================================================
// 函数功能：  Socket_New
//           分配套接字内存空间
// 输入参数： 无
// 输出参数： 无
// 返回值  ：NULL表示分配不成功（内存不足），否则返回分配的套接字结构
// 说明    ：分配时空间已经被初始化完毕
// =============================================================================
tagSocket* Socket_New(void)
{
	tagSocket *result;
	result = Mb_Malloc(pgSocketPool, CN_TIMEOUT_FOREVER);
	if(NULL != result)
	{
		memset(result,0,sizeof(tagSocket));
		result->sync = Lock_MutexCreate(NULL);
		if(NULL != result->sync)
		{
			//init the socket
			result->valid = true;
			result->socktype = EN_SOCKET_NOTSURE;
			sgDebugSocket = result;
		}
		else
		{
			Mb_Free(pgSocketPool, (void *)result);
			result = NULL;
		}
	}
	return result;
}
// =============================================================================
// 函数功能：  Socket_Free
//           释放套接字内存空间
// 输入参数： fd, 待释放的套接字
// 输出参数： 无
// 返回值  ：无
// 说明    ：
// =============================================================================
bool_t Socket_Free(tagSocket *sockfd)
{
	if(NULL != sockfd)
	{
		Lock_MutexDelete(sockfd->sync);
		sockfd->valid = false;
		sockfd->socktype = EN_SOCKET_NOTSURE;

		Mb_Free(pgSocketPool, (void *)sockfd);
		return true;
	}
	else
	{
		return false;
	}
}

// =============================================================================
// 函数功能：  Socket_Init
//           
// 输入参数：para，目前无意义 
// 输出参数： 
// 返回值  ：true成功 false失败
// 说明    ：1,建立套接字所需要的内存池
// =============================================================================
bool_t Socket_Init(ptu32_t para)
{
	bool_t result;
	int i;
	//套接字内存池
	pgSocketPool = Mb_CreatePool((void *)sgSocketPool, CN_SOCKET_POOLSIZE,\
			                     sizeof(tagSocket), 0, 0, "Socket Pool");
	if (NULL == pgSocketPool)
	{
		printk("Socket:Creat SocketPool Failed\n\r");
		result =  false;
	}
	else
	{
		
		printk("Socket:Creat SocketPool Success\n\r");
		for(i =0; i < CN_SOCKET_POOLSIZE; i++)
		{
			sgSocketMap[i] = NULL;
		}
		result = true;
	}	
	
	return result;
}




