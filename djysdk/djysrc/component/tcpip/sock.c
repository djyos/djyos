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
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
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
#include <sys/socket.h>

#include  "tpl.h"
#include  "tcpipconfig.h"

//套接字内存池
#define CN_SOCK_FREE            (1<<31)      //IF THIS SOCKET IS FREE, SET THIS BIT
#define CN_SOCK_FDMASK          (0x7FFFFFFF) //USE TO MASK THE SOCK FD
#define CN_SOCKET_BASEFD        0X200
static struct MutexLCB         *pgSocketMemSync = NULL;   //used to protect the mem
static tagSocket               *pgSocketMemHead = NULL;   //the socket mem

tagSocket *SocketMalloc(void)
{
    u32 i = 0;
    tagSocket *result = NULL;
    if(Lock_MutexPend(pgSocketMemSync,CN_TIMEOUT_FOREVER))
    {
        while(i < gSockNum)
        {
            if(CN_SOCK_FREE&pgSocketMemHead[i].sockfd)
            {
                result = &pgSocketMemHead[i];
                result->sync = Lock_MutexCreate_s(&result->syncmem,NULL);
                result->sockfd = i +CN_SOCKET_BASEFD;
                break;
            }
            i++;
        }
        Lock_MutexPost(pgSocketMemSync);
    }
    return result;
}

bool_t SocketFree(tagSocket *sock)
{
    bool_t result = false;
    if(Lock_MutexPend(pgSocketMemSync,CN_TIMEOUT_FOREVER))
    {
    	Lock_MutexDelete_s(sock->sync);
        memset((void *)sock,0, sizeof(tagSocket));
        sock->sockfd = CN_SOCK_FREE ;
        result = true;

        Lock_MutexPost(pgSocketMemSync);
    }
    return result;
}

static tagSocket *__SocketMapFd2Sock(int fd)
{
    tagSocket *result = NULL;
    if((fd >=CN_SOCKET_BASEFD)&&(fd <(CN_SOCKET_BASEFD+CN_SOCKET_BASEFD)))
    {
        result = pgSocketMemHead + (fd-CN_SOCKET_BASEFD);
        if(result->sockfd&CN_SOCK_FREE)
        {
            result = NULL;
        }
    }
    return result;
}
static int __SocketMapSock2Fd(tagSocket *sock)
{
    if(NULL != sock)
    {
        return sock->sockfd;
    }
    else
    {
        return -1;
    }
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
    tagTlayerProto  *proto = NULL;
    tagSocket       *sock = NULL;
    proto = TPL_GetProto(family, type, protocol);
    if(NULL != proto)
    {
        if(NULL != proto->socket)
        {
            sock = proto->socket(family, type, protocol);
        }
    }
    return  __SocketMapSock2Fd(sock);
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
int bind(int sockfd,struct sockaddr *addr, int addrlen)
{
    int  result;
    tagSocket *sock;
    
    result = -1;
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->bind))
    {
        result = sock->proto->bind(sock, addr, addrlen);
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
    tagSocket *sock;
    
    result = -1;
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->listen))
    {
        result = sock->proto->listen(sock, backlog);
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
    tagSocket *sock;
    tagSocket *result;
    
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->accept))
    {
        result = sock->proto->accept(sock, addr,addrlen);
    }
    return __SocketMapSock2Fd(result);
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
int connect(int sockfd, struct sockaddr *addr, int addrlen)
{
    int  result;
    tagSocket *sock;
    
    result = -1;
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->connect))
    {
        result = sock->proto->connect(sock, addr, addrlen);
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
    tagSocket *sock;
    
    result = -1;
    if((NULL != msg)&&(len > 0))
    {
        sock = __SocketMapFd2Sock(sockfd);
        if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->send))
        {
            result = sock->proto->send(sock, msg, len, flags);
        }
    }
    return  result; 
}
//the send or recv exact function used to deal the application proto
//which has negotiate the data structure
//we use the function to send the exact messages
//if return true, then the message has been sent in to the buffer at least
//if return false, then the socket must has some err
bool_t sendexact(int sock,u8 *buf,int len)
{
	bool_t result;
	int sentlen;

	sentlen = 0;
	result = true;
	while(len > 0)
	{
		sentlen = send(sock,buf,len,0);
		if(sentlen > 0)
		{
			len -= sentlen;
			buf += sentlen;
		}
		else if(sentlen == 0)
		{
			result = false;
			break; //maybe not permit to send any data
		}
		else
		{
			//maybe timeout continue
		}
	}
	return result;
}

//we use the function to recv the exact messages
//if return true, then the message has been cpied to the buffer
//if return false, then the socket must has some err
bool_t recvexact(int sock,u8 *buf,int len)
{
	bool_t result;
	int recvlen;

	recvlen = 0;
	result = true;
	while(len > 0)
	{
		recvlen = recv(sock,buf,len,0);
		if(recvlen > 0)
		{
			len -= recvlen;
			buf += recvlen;
		}
		else if(recvlen == 0)
		{
			result = false;
			break; //maybe the socket has been shutdown
		}
		else
		{
			//maybe timeout continue
		}
	}
	return result;
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
    tagSocket *sock;
    
    result = -1;

    if((NULL != buf)&&(len > 0))
    {
        sock = __SocketMapFd2Sock(sockfd);
        if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->recv))
        {
            result = sock->proto->recv(sock, buf, len, flags);
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
    tagSocket *sock;
    
    result = -1;
    if((NULL != msg)&&(len > 0))
    {
        sock = __SocketMapFd2Sock(sockfd);
        if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->sendto))
        {
            result = sock->proto->sendto(sock, msg, len, flags,addr, addrlen);
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
    tagSocket *sock;
    
    result = -1;
    if((NULL != buf)&&(len > 0))
    {
        sock = __SocketMapFd2Sock(sockfd);
        if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->recvfrom))
        {
            result = sock->proto->recvfrom(sock, buf, len, flags,addr, addrlen);
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
    tagSocket *sock;
    
    result = false;
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->shutdown))
    {
        result = sock->proto->shutdown(sock, how);
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
    tagSocket *sock;

    result = false;
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->close))
    {
        result = sock->proto->close(sock);
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
    tagSocket *sock;
    
    result = -1;
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->setsockopt))
    {
        result = sock->proto->setsockopt(sock, level, optname,\
                                            optval,optlen);
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
    tagSocket *sock;
    
    result = -1;
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(NULL != sock->proto)&&(NULL != sock->proto->getsockopt))
    {
        result = sock->proto->getsockopt(sock, level, optname,\
                                            optval,optlen);
    }
    return  result;     
}

//返回以前设置的app关联数据
ptu32_t socket_setprivate(int sockfd, ptu32_t private)
{
    ptu32_t result;
    tagSocket *sock;

    result =(ptu32_t)NULL;
    sock = __SocketMapFd2Sock(sockfd);
    if(NULL != sock)
    {
        result = sock->private;
        sock->private = private;
    }

    return result;
}
ptu32_t socket_getprivate(int sockfd)
{
    ptu32_t result;
    tagSocket *sock;

    result =(ptu32_t)NULL;
    sock = __SocketMapFd2Sock(sockfd);
    if(NULL != sock)
    {
        result = sock->private;
    }

    return result;
}
int getsockname(int sockfd,struct sockaddr *addr,socklen_t *addrlen)
{
    int result = -1;
    tagSocket *sock;
    struct sockaddr_in *addrin;

    sock = __SocketMapFd2Sock(sockfd);
    if(NULL != sock)
    {
        result = 0;
        addrin =(struct sockaddr_in*)addr;

        addrin->sin_family = sock->af_inet;
        addrin->sin_addr.s_addr = sock->element.v4.iplocal;
        addrin->sin_port = sock->element.v4.portlocal;

        *addrlen = sizeof(struct sockaddr_in);
    }

    return result;
}

int getpeername(int sockfd,struct sockaddr *addr,socklen_t *addrlen)
{
    int result = -1;
    tagSocket *sock;
    struct sockaddr_in *addrin;

    sock = __SocketMapFd2Sock(sockfd);
    if(NULL != sock)
    {
        result = 0;
        addrin =(struct sockaddr_in*)addr;
        addrin->sin_family = sock->af_inet;
        addrin->sin_addr.s_addr = sock->element.v4.ipremote;
        addrin->sin_port = sock->element.v4.portremote;
        *addrlen = sizeof(struct sockaddr_in);
    }

    return result;
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
bool_t Socket_MultiplexAdd(struct MultiplexSetsCB *sets,
                           int sockfd,u32 SensingBit)
{
    bool_t result;
    tagSocket *sock;
    
    result = false;
    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER)))
    {
        result = Multiplex_AddObject(sets,&sock->ioselect,sock->iostat,\
                                     sockfd,SensingBit);
        Lock_MutexPost(sock->sync);
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
bool_t Socket_MultiplexDel(struct MultiplexSetsCB *sets,int sockfd)
{
    bool_t result;
    tagSocket *sock;
    
    result = false;

    sock = __SocketMapFd2Sock(sockfd);
    if((NULL != sock)&&(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER)))
    {
        result = Multiplex_DelObject(sets,&sock->ioselect);
        Lock_MutexPost(sock->sync);
    }
    return result;
}


// =============================================================================
// FUNCTION：CHECK WHETHER THE SOCKET IS ACTIVE
// PARA  IN：mode:
// PARA OUT：
// RETURN  ：
// INSTRUCT:
// =============================================================================
//#define  CN_SOCKET_IOREAD      CN_MULTIPLEX_SENSINGBIT_READ   //COULE  READ
//#define  CN_SOCKET_IOWRITE     CN_MULTIPLEX_SENSINGBIT_WRITE  //COULD  WRITE, which means the
//#define  CN_SOCKET_IOERR       CN_MULTIPLEX_SENSINGBIT_ERROR  //SOCKET ERROR
//#define  CN_SOCKET_IOACCEPT    (CN_SOCKET_IOERR<<1)           //COULD  ACCEPT
//#define  CN_SOCKET_IOOR        CN_MULTIPLEX_SENSINGBIT_OR     //BITS OR TRIGGLE
int issocketactive(int sockfd, int mode)
{
    int  result = 0;
    tagSocket *sock;

    result = false;
    sock = __SocketMapFd2Sock(sockfd);
    if(NULL != sock)
    {
        if(mode &sock->iostat)
        {
            result = 1;
        }
    }
    return  result;
}

void sockinfo(int sockfd,char *filter)
{
    tagSocket *sock;

    sock = __SocketMapFd2Sock(sockfd);
    if(NULL != sock)
    {
        if(NULL != sock->proto->debuginfo)
        {
            sock->proto->debuginfo(sock,filter);
        }
        else
        {
            printf("%s:sockfd:%d :no debug function\r\n",__FUNCTION__,sockfd);
        }
    }

    return ;
}

bool_t sockallinfo(char *param)
{
    int i =0;

    for (i =0; i <gSockNum;i++)
    {
        sockinfo(pgSocketMemHead[i].sockfd,param);
    }

    return true;
}

struct ShellCmdTab  gSocketDebug[] =
{
    {
        "sockall",
        sockallinfo,
        "usage:sockall",
        NULL
    }
};

#define CN_SOCKETDEBUG_NUM  ((sizeof(gSocketDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gSocketDebugCmdRsc[CN_SOCKETDEBUG_NUM];

// =============================================================================
// 函数功能：  Socket_Init
//
// 输入参数：para，目前无意义
// 输出参数：
// 返回值  ：true成功 false失败
// 说明    ：1,建立套接字所需要的内存池
// =============================================================================
bool_t SocketInit(ptu32_t para)
{
    bool_t result;
    int i =0;
    pgSocketMemSync = Lock_MutexCreate("SocketMemSync");
    if(NULL != pgSocketMemSync)
    {
        pgSocketMemHead = (tagSocket *)malloc(gSockNum*sizeof(tagSocket));
        if(NULL != pgSocketMemHead)
        {
            memset((void *)pgSocketMemHead,0, gSockNum*sizeof(tagSocket));
            for(i = 0;i <gSockNum;i++ )
            {
                pgSocketMemHead[i].sockfd|= CN_SOCK_FREE;
            }

            result = Sh_InstallCmd(gSocketDebug,gSocketDebugCmdRsc,CN_SOCKETDEBUG_NUM);
        }
        else
        {
            Lock_MutexDelete(pgSocketMemSync);
            pgSocketMemSync = NULL;
            printk("%s:ERR:SOCKET MEM MALLOC  FAILED\n\r,__FUNCTION__");
        }
    }
    else
    {
        printk("%s:ERR:SOCKET MEM SYNC CREATE FAILED\n\r,__FUNCTION__");
    }
    return result;
}


