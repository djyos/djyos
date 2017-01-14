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

#ifndef __SOCKET_H
#define __SOCKET_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>   //use of malloc
#include <endian.h>   //use of hton and ntoh
#include <stdio.h>
#include <string.h>

#include <os.h>
#include <shell.h>
#include <multiplex.h>

//一个PKGLST在传输的过程中，当某个PKG拥有CN_PKLGLST_END标记或者NULL == partnext，即可认为
//该PKGLST结束，该特性在发送的时候尤其明显
#define CN_PKLGLST_END   (1<<0)
//tagNetPkg的原理
//bufsize在申请时指定，使用过程中一直不变;data一直指向buf的起始位置，保持不变
//当向PKG写入数据时，offset不变，从buf的offset+datalen的地方开始写新数据，写完之后
//                  datalen +=len(len为写入数据长度)
//当从PKG读取数据时，从buf的offset开始cpy，cpy完成之后，
//                  offset += len,datalen -= len(len为取出数据长度)
typedef struct NetPkg
{
    struct NetPkg   *partnext;        //该组指针负责数据包在协议栈的传递
    ptu32_t  private;   //used for the module who malloc the package
    u8   level;         // PKG的大小等级
    u8   pkgflag;       // PKG的后续扩展属性
    u8   refers;        // 缓存的次数
    u16  datalen;       // buf中的有效数据长度
    u16  bufsize;       // buf的长度
    u16  offset;        // 有效数据偏离buf的位置，offset之前的数据无效,当分拆数据或者数据对齐的时候很有用
    u8   *buf;          // pkg的buf（数据缓存区）
}tagNetPkg;

bool_t     Pkg_SetAlignOffset(u16 alignsize);
tagNetPkg *PkgMalloc(u16 bufsize,u8 flags);
bool_t     PkgTryFreePart(tagNetPkg *pkg);
bool_t     PkgTryFreeLst(tagNetPkg  *pkglst);
bool_t     PkgTryFreeQ(tagNetPkg  *pkglst);
bool_t     PkgCachedPart(tagNetPkg  *pkg);
bool_t     PkgCachedLst(tagNetPkg   *pkglst);
u16        PkgAlignSize(u16 size);
#define    PKG_ISLISTEND(pkg)      (pkg->pkgflag&CN_PKLGLST_END)

//used to defines the net device task
#define CN_IPDEV_TCPOCHKSUM  (1<<0)
#define CN_IPDEV_TCPICHKSUM  (1<<1)
#define CN_IPDEV_UDPOCHKSUM  (1<<2)
#define CN_IPDEV_UDPICHKSUM  (1<<3)
#define CN_IPDEV_ICMPOCHKSUM (1<<4)
#define CN_IPDEV_ICMPICHKSUM (1<<5)
#define CN_IPDEV_IPOCHKSUM   (1<<6)
#define CN_IPDEV_IPICHKSUM   (1<<7)
#define CN_IPDEV_ALL         (0xFFFFFFFF)
#define CN_IPDEV_NONE        (0X0)

//enum the ip version
typedef enum
{
	EN_IPV_4 =4,
	EN_IPV_6 =6,
}enum_ipv_t;

//this is for the
#define SOMAXCONN       100

//this is the domain family
#define AF_UNSPEC		(0) /* unspecified */
#define AF_LOCAL		(1) /* local to host (pipes, portals) */
#define AF_UNIX			AF_LOCAL /* backward compatibility */
#define AF_INET			(2) /* internetwork: UDP, TCP, etc. */
#define PF_INET         AF_INET
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

//proto
#define SOL_SOCKET       4
#define IPPROTO_IP       0      // dummy for IP
#define IPPROTO_ICMP     1      // control message protocol
#define IPPROTO_IGMP     2      // internet group management protocol
#define IPPROTO_GGP      3      // gateway^2 (deprecated)
#define IPPROTO_TCP      6      // tcp
#define IPPROTO_PUP      12     // pup
#define IPPROTO_UDP      17     // user datagram protocol
#define IPPROTO_IDP      22     // xns idp
#define IPPROTO_ND       77     // UNOFFICIAL net disk proto */
#define IPPROTO_RAW      255    // raw IP packet
#define IPPROTO_MAX      256

//typedef int socklen_t;
#define socklen_t int
#define CN_INVALID_FD  -1


//BSD标准的网络地址结构
//内核使用的sockaddr结构
struct sockaddr
{
    u16 sa_family;    // 地址族，AF_INET/AF_ISO/AP_UNIX或其它AF_xxx
    char sa_data[14]; // 14 字节的协议地址 ，保存该socket的端口号和IP地址
};
struct in_addr
{
    unsigned long s_addr;
};
// 为了更便于使用sockaddr，这里自定义一个socketaddr结构，用户程序使用本结构体设置参数
struct sockaddr_in
{
    u16             sin_family;    // 地址族，AF_INET/AF_ISO/AP_UNIX或其它AF_xxx
    u16             sin_port;      // 端口地址，网络字节序
    struct in_addr  sin_addr;      // IP地址
    u8              sin_zero[8];   // 填充字节
};

//SOL_SOCKET
enum _EN_SOL_SOCKET_OPTNAME
{
    //OPTNAME              MEANS                            optvaltype
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
    SO_REUSEADDR,     //允许重用本地地址和端口　　　　　　　　　  int
    SO_TYPE,           // 获得套接字类型　　　　　　　　　　　　  int
    SO_BSDCOMPAT,      //与BSD系统兼容　　　　　　　　　　　　　 int
    SO_NOBLOCK,        //DJY 非阻塞                                                    int
};
//IPPROTO_IP
enum _EN_IPPROTO_IP_OPTNAME
{
    //OPTNAME              MEANS                           optvaltype
    IP_HDRINCL = 0,   //在数据包中包含IP首部　　　　　　　　　　int
    IP_OPTIONS    ,   //IP首部选项　　　　　　　　　　　　　　　int
    IP_TOS        ,   //服务类型                                                        int
    IP_TTL        ,   //生存时间　　　　　　　　　　　　　　　　int
};
//IPPROTO_TCP
enum _EN_IPPROTO_TCP_OPTNAME
{
    //OPTNAME              MEANS                           optvaltype
    TCP_MAXSEG = 0,      //TCP最大数据段大小　　　　　　　　　 int
    TCP_NODELAY   ,      //不使用Nagle算法　　　　　　　　　　 int
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
//SOCKET IOSTAT
#define  CN_SOCKET_IOREAD      CN_MULTIPLEX_SENSINGBIT_READ   //COULE  READ
#define  CN_SOCKET_IOWRITE     CN_MULTIPLEX_SENSINGBIT_WRITE  //COULD  WRITE, which means the
#define  CN_SOCKET_IOERR       CN_MULTIPLEX_SENSINGBIT_ERROR  //SOCKET ERROR
#define  CN_SOCKET_IOACCEPT    (CN_SOCKET_IOREAD)             //COULD  ACCEPT
#define  CN_SOCKET_IOOR        CN_MULTIPLEX_SENSINGBIT_OR     //BITS OR TRIGGLE

//SOCKET SOCKSTAT
#define CN_SOCKET_CLIENT        (1<<0)
#define CN_SOCKET_BIND          (1<<1)
#define CN_SOCKET_CONNECT       (1<<2)
#define CN_SOCKET_LISTEN        (1<<3)
#define CN_SOCKET_SHUTWR        (1<<5)
#define CN_SOCKET_SHUTRD        (1<<6)
#define CN_SOCKET_CLOSE         (1<<7)
#define CN_SOCKET_OPEN          (1<<8)    //if use socket or accept to get the sock
#define CN_SOCKET_PROBLOCK      (1<<10)
#define CN_SOCKET_PROASYNC      (1<<11)
#define CN_SOCKET_PROOOB        (1<<12)
#define CN_SOCKET_PRONAGLE      (1<<13)
#define CN_SOCKET_PROLINGER     (1<<14)
#define CN_SOCKET_PROCORK       (1<<15)
#define CN_SOCKET_PROREUSE      (1<<16)
#define CN_SOCKET_PROKEEPALIVE  (1<<17)
#define CN_SOCKET_PROBROAD      (1<<18)

typedef struct
{
    u32 iplocal;
    u32 ipremote;;
    u16 portlocal;
    u16 portremote;
}tagSockElementV4;

#define CN_IPV6_LEN_WORD    4
typedef struct
{
    u32 iplocal[CN_IPV6_LEN_WORD];
    u32 ipremote[CN_IPV6_LEN_WORD];
    u16 portlocal;
    u16 portremote;
}tagSockElementv6;

typedef union
{
    tagSockElementV4  v4;
    tagSockElementv6  v6;
}uSockElement;

typedef struct Socket tagSocket;
//传输层协议接口定义
//传输层定义的接口,各个传输层协议，比方TCP,UDP等必须按照这个接口来实现
//对于不需要的函数接口，可以置空
//返回值为标准的BSD返回值
typedef struct TlayerProto
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
    int (*isactive)(tagSocket *sockfd,int mode);
    //设置套接字选项
    int (*setsockopt)(tagSocket *sockfd, int level, int optname,\
                   const void *optval, int optlen);
    //获取套接字选项
    int (*getsockopt)(tagSocket *sockfd, int level, int optname, void *optval,\
                   int *optlen);
    void (*debuginfo)(tagSocket *sockfd,char *filter);
}tagTlayerProto;

struct Socket
{
    //the following used by the proto
    struct Socket                  *nxt;         //nxt node
    struct MutexLCB                *sync;         //used to protect the socket
    struct MutexLCB                 syncmem;      //used for the sync mem
    tagTlayerProto                 *proto;        //protocol for the socket
    void                           *cb;           //the control node
    struct MultiplexObjectCB       *ioselect;     //used for the select;
    u32                             iostat;       //used for the select stat;
    u32                             errorno;      //record the socket errorno
    u32                             sockstat;     //which used by the specified protocal
    u32                             sockfd;       //the socket user fd
    ptu32_t                         private;      //used for the user, we could know who use the
                                                  //socket to communicate
    int                             af_inet;      //AF_FAMILY
    uSockElement                    element;      //socket addr
};

//linger选项
struct linger
{
    int l_onff;//0 off while 1 on
    int l_linger;//linger time,Units:Seconds
};

//some defines for the STACK SOCKET TOOL FUNCTION
#define CN_SOCKET_PORT_TEMPBASE  0x8000
#define CN_SOCKET_PORT_INVALID   0x0
#define CN_SOCKET_PORT_LIMIT     0xFFFF

//the sock layer only supply the sock malloc and free
tagSocket *SocketMalloc(void);
bool_t     SocketFree(tagSocket *sock);

//FOR ALL THE APPLICATIONS, ONLY THE FOLLOWING INTERFACE COULD BE USED
int socket(int family, int type, int protocol);
int bind(int sockfd,struct sockaddr *addr, int addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, int *addrlen);
int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
int send(int sockfd, const void *msg, int len, int flags);
int recv(int sockfd, void *buf,int len, unsigned int flags);
bool_t sendexact(int sock,u8 *buf,int len);
bool_t recvexact(int sock,u8 *buf,int len);
int sendto(int sockfd, const void *msg,int len, unsigned int flags,\
          const struct sockaddr *addr, int addrlen);
int recvfrom(int sockfd,void *buf, int len, unsigned int flags,\
            struct sockaddr *addr, int *addrlen);
bool_t shutdown(int sockfd, u32 how);
bool_t closesocket(int sockfd);
int setsockopt(int sockfd, int level, int optname,\
                const void *optval, int optlen);
int getsockopt(int sockfd, int level, int optname, void *optval,int *optlen);
bool_t Socket_MultiplexAdd(struct MultiplexSetsCB *MultiplexSets,
                           int sock,u32 SensingBit);
bool_t Socket_MultiplexDel(struct MultiplexSetsCB *MultiplexSets,int sock);
ptu32_t socket_setprivate(int sock, ptu32_t private);
ptu32_t socket_getprivate(int sock);
int issocketactive(int sockfd, int mode);
void sockinfo(int sockfd,char *filter);
bool_t sockallinfo(char *param);
int getsockname(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
int getpeername(int sockfd,struct sockaddr *addr,socklen_t *addrlen);

//THIS IS THE COMPORT DEFINES FOR THE FREE SOFTWARE**********************//
//fcntl.h
/* open/fcntl - O_SYNC is only implemented on blocks devices and on files
   located on an ext2 file system */
#ifndef O_ACCMODE
#define O_ACCMODE   00000003
#endif

#ifndef O_RDONLY
#define O_RDONLY    00000000
#endif

#ifndef O_WRONLY
#define O_WRONLY    00000001
#endif

#ifndef O_RDWR
#define O_RDWR      00000002
#endif


#ifndef O_CREAT
#define O_CREAT     00000100    /* not fcntl */
#endif
#ifndef O_EXCL
#define O_EXCL      00000200    /* not fcntl */
#endif
#ifndef O_NOCTTY
#define O_NOCTTY    00000400    /* not fcntl */
#endif
#ifndef O_TRUNC
#define O_TRUNC     00001000    /* not fcntl */
#endif
#ifndef O_APPEND
#define O_APPEND    00002000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK  00004000
#endif
#ifndef O_SYNC
#define O_SYNC      00010000
#endif
#ifndef FASYNC
#define FASYNC      00020000    /* fcntl, for BSD compatibility */
#endif
#ifndef O_DIRECT
#define O_DIRECT    00040000    /* direct disk access hint */
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE 00100000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 00200000    /* must be a directory */
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW  00400000    /* don't follow links */
#endif
#ifndef O_NOATIME
#define O_NOATIME   01000000
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC   02000000    /* set close_on_exec */
#endif
#ifndef O_NDELAY
#define O_NDELAY    O_NONBLOCK
#endif

#ifndef FD_CLOEXEC
#define FD_CLOEXEC     (1<<0)
#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE     (10)    //the mac select num
#endif
enum __FCNTL_CMD
{
	F_SETFD = 0,
	F_GETFD,
	F_DUPFD,
	F_DUPFD_CLOEXEC,
	F_GETFL,
	F_SETFL,
};

#include <unistd.h>
extern int fcntl(int fd,int cmd,...);
extern off_t lseek(int fd,off_t offset, int whence);

//netdb.h
/* Description of data base entry for a single host.  */
struct hostent
{
	char * h_name;			/* Official name of host.  */
	char **h_aliases;		/* Alias list.  */
	int    h_addrtype;		/* Host address type.  */
	int    h_length;	    /* Length of address.  */
	char **h_addr_list;		/* List of addresses from name server.  */
	# define	h_addr	h_addr_list[0] /* Address, for backward compatibility.*/
};
/* Description of data base entry for a single service.  */
struct servent
{
	char      *s_name;			/* Official service name.  */
	char     **s_aliases;		/* Alias list.  */
	int        s_port;			/* Port number.  */
	char      *s_proto;		    /* Protocol to use.  */
};

/* Description of data base entry for a single service.  */
struct protoent
{
	char    *p_name;			/* Official protocol name.  */
	char   **p_aliases;		    /* Alias list.  */
	int      p_proto;			/* Protocol number.  */
};

struct servent *getservbyname(const char *name, const char *proto);
u32 inet_addr(const char *addr);
char *inet_ntoa(struct in_addr addr);
int inet_aton(const char *string,struct in_addr *addr);
struct hostent *gethostbyname(const char *name);
int  gethostname(char *name, int len);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);

//select.h
#include <sys/time.h>
#define CN_SELECT_MAXNUM      FD_SETSIZE
#define CN_SELECT_TIMEDEFAULT 10
typedef struct
{
	int  mode;
	u32  fd[CN_SELECT_MAXNUM];
}_types_fd_set;

#define fd_set _types_fd_set
#define ARG_MAX 256
int FD_SET(int fd, fd_set *sets);
int FD_CLR(int fd, fd_set *sets);
int FD_ISSET(int fd, fd_set *sets);
int FD_ZERO(fd_set *sets);

int select(int maxfd, fd_set *reads,fd_set *writes, fd_set *exps, \
           struct timeval *timeout);
int string2arg(int *argc, char *argv[],char *string);
bool_t string2Mac(char *str,u8 *mac);  //string must be xx-xx-xx-xx-xx-xx style
char* Mac2string(u8 *mac);

////////////////////////NETDEV DRIVER USED INTERFACE///////////////////////////////
//ip address defines
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK     0x7f000001     //127.0.0.1
#endif
#define INADDR_ANY          0
#define INADDR_NONE         0XFFFFFFFF
#define INADDR_BROAD        0XFFFFFFFF
#define INADDR_BROADCAST    INADDR_BROAD
#define INVALID_SOCKET      -1
#define CN_IPADDR_LEN_V4     4
#define CN_IPADDR_LEN_V6     16
#define CN_MACADDR_LEN       6
#define CN_IPADDR_ANY_V4     INADDR_ANY
#define CN_MAC_BROAD         "\xff\xff\xff\xff\xff\xff"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////I WANT TO REBUILD THE NEW LINK ROUT///////////////////
//this defines for the debug
#define TCPIP_DEBUG_INC(x)     do{x++;}while(0)
#define TCPIP_DEBUG_DEC(x)     do{x--;}while(0)

#define CN_TCPIP_NAMELEN      32    //IN THE TCP IP STACK ,THIS IS THE NAME LEN LIMIT

typedef enum
{
	EN_NETDEV_FRAME_NOPKG = 0,
	EN_NETDEV_FRAME_BROAD,
	EN_NETDEV_FRAME_POINT,
	EN_NETDEV_FRAME_MULTI,
	EN_NETDEV_FRAME_LAST,
}enNetDevFramType;
typedef enum
{
    EN_NETDEV_SETNOPKG = 0,      //PARA IS NOT CARE
	EN_NETDEV_SETBORAD,          //para is int,0 disable else enable
	EN_NETDEV_SETPOINT,          //para is int,0 disable else enable
	EN_NETDEV_SETMULTI,          //para is int,0 disable else enable
	EN_NETDEV_SETRECV,           //para is int,0 disable else enable
	EN_NETDEV_SETSEND,           //para is int,0 disable else enable
    EN_NETDEV_SETMAC,            //para point to an buf which contains the mac
                                 //driver must modify the dev struct mac at the same time
	EN_NETDEV_SETMULTIMAC,       //para point to an buf which contains the mac
	EN_NETDEV_GTETMAC,           //para point to an buf which used to contain the mac
	EN_NETDEV_RESET,             //para must be true
	EN_NETDEV_CMDLAST,           //which means the max command
}enNetDevCmd;

enum _EN_LINK_INTERFACE_TYPE
{
    EN_LINK_ETHERNET = 0,  //ethenet net device,ethernetII
    EN_LINK_RAW,           //raw device without mac
	EN_LINK_NOVEL,
	EN_LINK_80203,
	EN_LINK_SNAP,
	EN_LINK_80211,
    EN_LINK_LAST,
};
//net device type
//netdev snd module function
//return means the data has put out or put into the net card buffer
//pkg maybe an lst or not,you could use the PkgIsEnd to check
//pkglen is fram len
typedef bool_t (*fnIfSend)(ptu32_t dev,tagNetPkg *pkglst,u32 framlen,u32 netdevtask);
//used to ctrl the dev or get the dev stat
typedef bool_t (*fnIfCtrl)(ptu32_t dev,enNetDevCmd cmd,ptu32_t para);
typedef struct NetDevPara
{
    const char                    *name;    //dev name
    u8                             iftype;   //dev type
    fnIfSend                       ifsend;   //dev snd function
    fnIfCtrl                       ifctrl;   //dev ctrl or stat get fucntion
    u32                            devfunc;  //dev hard function,such as tcp chksum
    u16                            mtu;      //dev mtu
    ptu32_t                        private;  //the dev driver use this to has its owner property
    u8                             mac[CN_MACADDR_LEN];   //mac address
}tagNetDevPara;  //we use the para to create an net device
//the following we be cut in the socket.h
ptu32_t NetDevInstall(tagNetDevPara *para);          //return net dev handle, 0 failed
ptu32_t NetDevHandle(const char *name);
bool_t NetDevCtrl(const char *name,enNetDevCmd cmd, ptu32_t para);
bool_t NetDevCtrlByHandle(ptu32_t handle,enNetDevCmd cmd, ptu32_t para);
bool_t NetDevUninstall(const char *name);
bool_t NetDevPrivate(ptu32_t handle);
//link function that driver should use
bool_t LinkPost(ptu32_t devhandle,tagNetPkg *pkg);
//this is used for the net device filter
bool_t NetDevFilterSet(const char *name,enNetDevFramType type,u32 framelimit,\
		u32 actiontime,u32 measuretime);
bool_t NetDevFilterEnable(const char *name,enNetDevFramType type,bool_t enable);
bool_t NetDevFilterCounter(ptu32_t handle,enNetDevFramType type);
bool_t NetDevFilterCheck(ptu32_t handle);

//the user could use the following api to listen on more protocol or send specified frames
typedef bool_t (*fnLinkProtoDealer)(ptu32_t devhandle,u16 proto,tagNetPkg *pkg);
bool_t LinkRegisterRcvHook(fnLinkProtoDealer hook, ptu32_t devhandle,u16 proto,const char *hookname);
bool_t LinkUnRegisterRcvHook(fnLinkProtoDealer hook, ptu32_t devhandle,u16 proto,const char *hookname);
bool_t LinkSendRaw(ptu32_t devhandle,tagNetPkg *pkg,u32 framlen,u32 devtask);
bool_t LinkSendBufRaw(ptu32_t devhandle,u8 *buf,u32 len);

//the user could use these function to create delete modify the rout
//warning: ipaddr_t for ip, if ver is EN_IPV_4, then it is an u32 or an point
//         to u32, if EN_IPV_6, it must be an u8*, and the len is CN_IPADDR_LEN_V6
typedef ptu32_t   ipaddr_t;
typedef struct
{
    u32 ip;
    u32 dns;
    u32 gatway;
    u32 submask;
    u32 broad;
}tagHostAddrV4;
typedef struct
{
    u32 ip[4];
    u32 dns[4];
    u32 gatway[4];
    u32 subnetmask[4];
    u32 broad[4];
}tagHostAddrV6;
#define CN_ROUT_DHCP    (1<<0)  //use this bit to get ip address from the dhcp server
#define CN_ROUT_NONE    (0)
bool_t RoutCreate(const char *name,enum_ipv_t ver,void *netaddr,u32 pro);
bool_t RoutDelete(const char *name,enum_ipv_t ver,ipaddr_t addr);
bool_t RoutSet(const char *name,enum_ipv_t ver,ipaddr_t ipold,void *newaddr);
bool_t RoutSetDefault(enum_ipv_t ver,ipaddr_t ip);
bool_t RoutDns(enum_ipv_t ver, ipaddr_t ip);
//this function use to alloc an ip from the dhcp dynamicly
bool_t DhcpAddClientTask(const char *name);
//this is the tcpip stack main entry
ptu32_t ModuleInstall_TcpIp(ptu32_t para);

#endif /* __SOCKET_H */

