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
// 模块描述: 本文件套接字的定义，以及使用的传输层的接口标准的定义
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 3:15:08 PM/Jul 15, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================

#ifndef SOCKET_H_
#define SOCKET_H_
#include "stdint.h"
#include "stddef.h"

#include "os.h"
#include "multiplex.h"

#include "pkg.h"
#include "tcpip_cdef.h"
// family
#define AF_UNSPEC		(0) /* unspecified */
#define AF_LOCAL		(1) /* local to host (pipes, portals) */
#define AF_UNIX			AF_LOCAL /* backward compatibility */
#define AF_INET			(2) /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK		(3) /* arpanet imp addresses */
#define AF_PUP			(4) /* pup protocols: e.g. BSP */
#define AF_CHAOS        (5) /* mit CHAOS protocols */
#define AF_NS			(6) /* XEROX NS protocols */
#define AF_ISO			(7) /* ISO protocols */
#define AF_OSI			AF_ISO
#define AF_ECMA			(8) /* european computer manufacturers */
#define AF_DATAKIT		(9) /* datakit protocols */
#define AF_CCITT        (10) /* CCITT protocols, X.25 etc */
#define AF_SNA			(11) /* IBM SNA */
#define AF_DECnet		(12) /* DECnet */
#define AF_DLI			(13) /* DEC Direct data link interface */
#define AF_LAT			(14) /* LAT */
#define AF_HYLINK		(15) /* NSC Hyperchannel */
#define AF_APPLETALK    (16) /* Apple Talk */
#define AF_ROUTE        (17) /* Internal Routing Protocol */
#define AF_LINK			(18) /* Link layer interface */
#define pseudo_AF_XTP	(19) /* eXpress Transfer Protocol (no AF) */
#define AF_COIP			(20) /* connection-oriented IP, aka ST II */
#define AF_CNT			(21) /* Computer Network Technology */
#define pseudo_AF_RTIP	(22) /* Help Identify RTIP packets */
#define AF_IPX			(23) /* Novell Internet Protocol */
#define AF_SIP			(24) /* Simple Internet Protocol */
#define pseudo_AF_PIP	(25) /* Help Identify PIP packets */

// type
#define SOCK_STREAM		1        // 可靠的双向字节流服务 - TCP
#define SOCK_DGRAM		2        // 最好的运输层数据报服务 - UDP
#define SOCK_RAW        3        // 最好的网络层数据报服务 - ICMP IGMP 原始IP
#define SOCK_RDW        4        // 可靠的数据报服务（未实现） - n/a
#define SOCK_SEQPACKET	5        // 可靠的双向记录流服务 - n/a

//BSD标准的网络地址结构
//内核使用的sockaddr结构
struct sockaddr
{
    u16 sa_family;    // 地址族，AF_INET/AF_ISO/AP_UNIX或其它AF_xxx
    char sa_data[14]; // 14 字节的协议地址 ，保存该socket的端口号和IP地址
};
struct in_addr 
{ 
	union 
	{ 
		struct { u8 s_b1,s_b2,s_b3,s_b4; } S_un_b; 
		struct { u16 s_w1,s_w2; } S_un_w; 
		unsigned long S_addr; 
	} S_un;
#define s_addr S_un.S_addr
};
// 为了更便于使用sockaddr，这里自定义一个socketaddr结构，用户程序使用本结构体设置参数
struct sockaddr_in
{
    u16             sin_family;    // 地址族，AF_INET/AF_ISO/AP_UNIX或其它AF_xxx
    u16             sin_port;      // 端口地址，网络字节序
    struct in_addr  sin_addr;      // IP地址
    u8              sin_zero[8];   // 填充字节
};

//套接字选项设置定义:level
enum _EN_SETSOCKOPT_LEVEL
{
	//LEVEL        //MEANS
	SOL_SOCKET=0,  //通用套接字选项
	IPPROTO_IP,    //IP选项
	IPPROTO_TCP,   //tcp选项
};

//SOL_SOCKET
enum _EN_SOL_SOCKET_OPTNAME
{
	//OPTNAME              MEANS                              optvaltype	
	SO_BROADCAST = 0 , //允许发送广播数据　　　　　　　　　　　　int
	SO_DEBUG,          //允许调试　　　　　　　　　　　　　　　　int
	SO_DONTROUTE,      //不查找路由　　　　　　　　　　　　　　　int
	SO_ERROR,          //获得套接字错误　　　　　　　　　　　　　int
	SO_KEEPALIVE,      //保持连接　　　　　　　　　　　　　　　　int
	SO_LINGER,         //延迟关闭连接　　　　　　　　　　　　　　struct linger
	SO_OOBINLINE,      //带外数据放入正常数据流　　　　　　　　　int
	SO_RCVBUF,         //接收缓冲区大小　　　　　　　　　　　　　int
	SO_SNDBUF,         //发送缓冲区大小　　　　　　　　　　　　　int
	SO_RCVLOWAT,       //接收缓冲区下限　　　　　　　　　　　　　int
	SO_SNDLOWAT,       //发送缓冲区下限　　　　　　　　　　　　　int
	SO_RCVTIMEO,       //接收超时　　　　　　　　　　　　　　　　struct timeval
	SO_SNDTIMEO,       //发送超时　　　　　　　　　　　　　　　　struct timeval
	SO_REUSERADDR,     //允许重用本地地址和端口　　　　　　　　　int
	SO_TYPE,           // 获得套接字类型　　　　　　　　　　　　  int
	SO_BSDCOMPAT,      //与BSD系统兼容　　　　　　　　　　　　　 int
	SO_NOBLOCK,        //DJY 非阻塞                                                                         int
};
//IPPROTO_IP
enum _EN_IPPROTO_IP_OPTNAME
{
	//OPTNAME              MEANS                           optvaltype	
	IP_HDRINCL = 0,//在数据包中包含IP首部　　　　　　　　　　int
	IP_OPTIONS    ,//IP首部选项　　　　　　　　　　　　　　　int
	IP_TOS        ,//服务类型
	IP_TTL        ,//生存时间　　　　　　　　　　　　　　　　int	
};
//IPPROTO_TCP
enum _EN_IPPROTO_TCP_OPTNAME
{
	//OPTNAME              MEANS                           optvaltype	
	TCP_MAXSEG = 0,  //TCP最大数据段大小　　　　　　　　　　                   int
	TCP_NODELAY   ,  //不使用Nagle算法　　　　　　　　　　　　　　　int
};
//确定一个通信空间的二次源
typedef struct _tagSocketNetAddr
{
	u16 port;
	u32 ip;
}tagSocketNetAddr;

typedef struct _tagSocketConState
{
	u8 noblock: 1;  //非阻塞
	u8 asyn:    1;  //异步
}tagSocketConState;


enum _EN_SOCKET_TYPE
{
	EN_SOCKET_NOTSURE =0,   //not sure yet
	EN_SOCKET_TERMINAL,     //the communication node
	EN_SOCKET_TREENODE,     //the tree node,sometimes means it is an server node
};

enum _EN_SOCKEY_ERRORNO
{
	EN_SOCKET_ERROR_PARAERR =0 ,      //para error
	EN_SOCKET_TRY_AGAIN,              //when there is no data what we need, and
	                                  //the socket is noblock, then the no will be set
	EN_SOCKET_ERROR_SNDPKG2LARGE ,    //the snd buf exceed the sndbuf len
	EN_SOCKET_CHANNEL_SHUTDOWN_W,     //the communication has been shutdown WRITE
	EN_SOCKET_CHANNEL_SHUTDOWN_R,     //the communication has been shutdown READ
};
struct _tagTlayerProto;
typedef struct _tagSocket
{
	struct _tagSocket               *pre;         //pre node
	struct _tagSocket               *nxt;         //nxt node
	struct _tagSocket               *hungnode;    //which means this socket hung on
	u8                              socktype;     //套接字类型，见_EN_SOCKET_TYPE定义
	bool_t                          valid;        //是否有效
	u8                              appstate;     //应用状态，bind connect等
    struct tagMutexLCB              *sync;        //资源互斥节点
    struct tagSemaphoreLCB          *casync;      //used for accept and connect sync

	tagSocketNetAddr                laddr;        //本地端地址
	tagSocketConState               constate;     //本地端SOCKET的通信方式
	struct _tagTlayerProto          *proto;	      //传输层协议 
	void                            *cb;          //the control node
	                                              //when terminal it is tcb else treenode

	struct tagMultiplexObjectCB     *multiio;     //多路复用
	u32                             status;       //可读可写等状态 
	u32                             errorno;      //当发生错误的时候可以使用
}tagSocket;

enum _EN_SOCKET_LOCALSTATE
{
	EN_SOCKET_LOCAL_RST =0,    //复位状态
	EN_SOCKET_LOCAL_CREATE ,   //创建成功后
	EN_SOCKET_LOCAL_BIND,      //调用BIND后
	EN_SOCKET_LOCAL_LISTEN,    //调用Listen后
	EN_SOCKET_LOCAL_CONNECT,   //调用connect后
	EN_SOCKET_LOCAL_CLOSE,     //调用close后，但是由于挂载的RFD原因，当时删除不掉
	EN_SOCKET_LOCAL_2FREE,     //待释放的，已经删除了所有的RFD，可以释放了
};

// socket缓冲区结构
typedef struct _tagSocketBuf
{
	int                       buflen;      //已经包含的数据量
	int                       buflenlimit; //该buf的长度限制，超过此长度认为溢出
    int                       triglevel;   //触发水平
    tagNetPkg                 *pkglsthead; //数据包链表头，发送缓冲区采用Pkg链表，接收缓冲区采用part链表
	tagNetPkg                 *pkglsttail; //链表尾部
    struct tagSemaphoreLCB    *bufsync;    //当BUF空间有变化时需要激活量
}tagSocketBuf;

//linger选项
struct linger
{
	int l_onff;//0 off while 1 on
	int l_linger;//linger time,Units:Seconds
};

//传输层协议接口定义
//传输层定义的接口,各个传输层协议，比方TCP,UDP等必须按照这个接口来实现
//对于不需要的函数接口，可以置空
//返回值为标准的BSD返回值
typedef struct _tagTlayerProto
{
	//创建一个套接字
	tagSocket* (*socket)(int family, int type, int protocol);
	//绑定一个端口号
	int (*bind)(tagSocket *sockfd,struct sockaddr *myaddr, int addrlen);
	//让一个端口处于监听状态
	int (*listen)(tagSocket *sockfd, int backlog);
	//让一个主机端处于接收状态(服务器才会)
	tagSocket* (*accept)(tagSocket *sockfd, struct sockaddr *addr, int *addrlen);
	//让一个客户端去链接服务器
	int (*connect)(tagSocket *sockfd, struct sockaddr *serv_addr, int addrlen); 
	//发送数据
	int (*send)(tagSocket *sockfd, const void *msg, int len, int flags);
	//接收数据
	int (*recv)(tagSocket *sockfd, void *buf,int len, unsigned int flags);
	//直接发送数据到目的端
	int (*sendto)(tagSocket * sockfd, const void *msg,int len, unsigned int flags,\
			  const struct sockaddr *addr, int addrlen);
	//直接从目的端读取数据
	int (*recvfrom)(tagSocket * sockfd,void *buf, int len, unsigned int flags,\
			    struct sockaddr *addr, int *addrlen);
	//关闭一个套接口
#define SHUT_RD   0  //关闭读
#define SHUT_WR   1  //关闭写
#define SHUT_RDWR 2  //关闭读写
	int (*shutdown)(tagSocket *sockfd, u32 how);
	int (*close)(tagSocket *sockfd);
	
	//设置套接字选项
	int (*setsockopt)(tagSocket *sockfd, int level, int optname,\
			       const void *optval, int optlen);
	//获取套接字选项
	int (*getsockopt)(tagSocket *sockfd, int level, int optname, void *optval,\
			       int *optlen);
	
	//defines for the socket multiIO 
#define  CN_SOCKET_READALBE   CN_MULTIPLEX_SENSINGBIT_READ   //COULE  READ
#define  CN_SOCKET_WRITEALBE  CN_MULTIPLEX_SENSINGBIT_WRITE  //COULD  WRITE, which means the 
#define  CN_SOCKET_ERROR      CN_MULTIPLEX_SENSINGBIT_ERROR  //SOCKET ERROR
#define  CN_SOCKET_OR         CN_MULTIPLEX_SENSINGBIT_OR 

#define  CN_SOCKET_ACCEPT     CN_SOCKET_ERROR<<1             //COULD  ACCEPT
#define  CN_SOCKET_OPEN       CN_SOCKET_ERROR<<2             //COULD BE ACCEPT OR CONNECT
	bool_t (*multiioadd)(struct tagMultiplexSetsCB  *Sets,\
	        ptu32_t ObjectID, u32 SensingBit, tagSocket *sockfd);
	bool_t (*multiiodel)(struct tagMultiplexSetsCB  *Sets,\
	        ptu32_t ObjectID, tagSocket *sockfd);
}tagTlayerProto;

tagSocket *Socket_New(void);
bool_t     Socket_Free(tagSocket *sock);

#define INVALID_SOCKET -1
//套接字接口
int socket(int family, int type, int protocol);
int bind(int sockfd,struct sockaddr *myaddr, int addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, int *addrlen);
int connect(int sockfd, struct sockaddr *serv_addr, int addrlen); 
int send(int sockfd, const void *msg, int len, int flags);
int recv(int sockfd, void *buf,int len, unsigned int flags);
int sendto(int sockfd, const void *msg,int len, unsigned int flags,\
		  const struct sockaddr *addr, int addrlen);
int recvfrom(int sockfd,void *buf, int len, unsigned int flags,\
		    struct sockaddr *addr, int *addrlen);
bool_t shutdown(int sockfd, u32 how);
bool_t closesocket(int sockfd);
int setsockopt(int sockfd, int level, int optname,\
		        const void *optval, int optlen);
int getsockopt(int sockfd, int level, int optname, void *optval,int *optlen);
bool_t Socket_MultiplexAdd(struct tagMultiplexSetsCB *MultiplexSets,
                           int sock,u32 SensingBit);
bool_t Socket_MultiplexDel(struct tagMultiplexSetsCB *MultiplexSets,int sock);

#endif /* SOCKET_H_ */

