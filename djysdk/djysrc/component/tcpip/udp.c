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
#include <sys/socket.h>

#include "rout.h"
#include "tpl.h"
#include "ip.h"
#include "ipv4.h"
#include "tcpipconfig.h"

static tagTlayerProto  gTplProto;
typedef struct
{
    u16 portsrc;
    u16 portdst;
    u16 msglen;
    u16 chksum;
}tagUdpHdr;

#define CN_UDP_CHANNEL_STATASND    (1<<0)  //APP COULD SND DATA
#define CN_UDP_CHANNEL_STATARCV    (1<<1)  //APP COULD NOT SND DATA
#define CN_UDP_CHANNEL_STATKSND    (1<<2)  //STACK COULD SND DATA
#define CN_UDP_CHANNEL_STATKRCV    (1<<3)  //STAK COULD RCV DATA
#define CN_UDP_CHANNEL_STATCONGEST (1<<4)  //the rcv window is full
typedef struct
{
    int                       buflen;      //the data length in the buffer
    int                       buflenlimit; //the buffer length
    int                       triglevel;   //the buffer trigger level
    tagNetPkg                *ph;          //the data list head
    tagNetPkg                *pt;          //the data list tail
    struct SemaphoreLCB      *bufsync;     //if any data in the buffer, it will be activited
    u32                       timeout;     //if block, use this time to wait
}tagRBufUdp;
//each udp socket has the same controller
typedef struct UdpCB
{
    struct UdpCB           *next;              //used for the ucb control
    u32                     channelstat;       //used for the select
    int                     sndlimit;          //send the most byte a frame
    tagRBufUdp              rbuf;              //receive buffer
    s64                     framrcv;           //record the frame receive
    s64                     framsnd;           //record the frame send
    u32                     lasterr;           //record the last error
}tagUdpCB;
static tagUdpCB          *pUcbFreeLst = NULL;     //this is the udp control list
static struct MutexLCB   *pUcbFreeLstSync = NULL; //this is used for the ucb protect
//define for the udp msg pkg len
#define CN_UDP_PKGMSGLEN        1472
#define CN_UDP_BUFLENDEFAULT    0x2000      //8KB

//we use the hash tab to search add or remove an socket to the socket tab
typedef struct
{
    u32                          tablen;
    struct MutexLCB             *tabsync;
    tagSocket                   *array[0];
}tagUdpHashTab;
static tagUdpHashTab   *pUdpHashTab = NULL;
// =============================================================================
// FUNCTION：this function is used to initialize the udpv4 hash tab
// PARA  IN：len ,this parameter limites the hashtab lenth
// PARA OUT：
// RETURN  ：
// INSTRUCT:
// =============================================================================
static bool_t __hashTabInit(u32 len)
{
    bool_t result = false;

    pUdpHashTab = malloc(sizeof(tagUdpHashTab) + len *sizeof(tagSocket *));
    if(NULL == pUdpHashTab)
    {
        goto ERR_ARRAYMEM;
    }
    memset((void *)pUdpHashTab,0,sizeof(tagUdpHashTab) + len *sizeof(tagSocket *));

    pUdpHashTab->tabsync = Lock_MutexCreate(NULL);
    if(NULL == pUdpHashTab->tabsync)
    {
        goto ERR_SYNC;
    }

    pUdpHashTab->tablen = len;
    result = true;
    return result;

ERR_SYNC:
    free(pUdpHashTab);
    pUdpHashTab = NULL;
ERR_ARRAYMEM:
    return result;
}

// =============================================================================
// FUNCTION：this function is used to find an socket in the hashtab
// PARA  IN：the specified ip and port determines the item together
// PARA OUT：
// RETURN  ：the socket find else NULL(no one mathes)
// INSTRUCT:send an mail to the mail box, which to be dealt by the ip engine
// =============================================================================
static tagSocket *__hashSocketSearch(u32 ip, u16 port)
{
    tagSocket *result = NULL;

    tagSocket *tmp;
    u32 hashKey;

    if(Lock_MutexPend(pUdpHashTab->tabsync,CN_TIMEOUT_FOREVER))
    {
        hashKey = port%pUdpHashTab->tablen;
        tmp = pUdpHashTab->array[hashKey];
        while((NULL != tmp))
        {
            if((ip == tmp->element.v4.iplocal)&&(port == tmp->element.v4.portlocal))
            {
                result = tmp;
                break;
            }
            else
            {
                tmp = tmp->nxt;
            }
        }

        Lock_MutexPost(pUdpHashTab->tabsync);
    }
    return result;
}
// =============================================================================
// FUNCTION:and an socket to the hash tab
// PARA  IN:ip and port are the only hash property we need
// PARA OUT：
// RETURN  :true add success while false failed
// INSTRUCT:if the same ip and port has existed, then will fail
// =============================================================================
static bool_t __hashSocketAdd(tagSocket *sock)
{
    bool_t      result;
    tagSocket  *tmp;
    u32         hashKey;
    u32         ip;
    u16         port;

    result = false;
    ip = sock->element.v4.iplocal;
    port = sock->element.v4.portlocal;
    hashKey = port%pUdpHashTab->tablen;
    if(Lock_MutexPend(pUdpHashTab->tabsync,CN_TIMEOUT_FOREVER))
    {
        tmp = pUdpHashTab->array[hashKey];
        while(NULL != tmp)
        {
            if((ip == tmp->element.v4.iplocal)&&(port == tmp->element.v4.portlocal))
            {
                break;
            }
            else
            {
                tmp = tmp->nxt;
            }
        }
        if(NULL == tmp)
        {
            sock->nxt = pUdpHashTab->array[hashKey];
            pUdpHashTab->array[hashKey] = sock;
            result = true;
        }
        Lock_MutexPost(pUdpHashTab->tabsync);
    }
    return result;
}

// =============================================================================
// FUNCTION:this function is used to free the item created
// PARA  IN:ip and port are the only hash property we need
// PARA OUT：
// RETURN  :the hash item we create
// INSTRUCT:add the create item to the last
// =============================================================================
static bool_t __hashSocketRemove(tagSocket *sock)
{

    tagSocket  *tmp;
    u32         hashKey;

    if(Lock_MutexPend(pUdpHashTab->tabsync,CN_TIMEOUT_FOREVER))
    {
        hashKey = sock->element.v4.portlocal%pUdpHashTab->tablen;
        tmp = pUdpHashTab->array[hashKey];
        if(sock == tmp)
        {
            pUdpHashTab->array[hashKey] = sock->nxt;
            sock->nxt = NULL;
        }
        else
        {
            while(NULL != tmp)
            {
                if(tmp->nxt == sock)
                {
                    tmp->nxt = sock->nxt;
                    sock->nxt = NULL;
                    break;
                }
                else
                {
                    tmp = tmp->nxt;
                }
            }
        }

        Lock_MutexPost(pUdpHashTab->tabsync);
    }
    return true;
}
//used for manage the ucb mem
// =============================================================================
// FUNCTION：this function is used to do the udp free list initializing
// PARA  IN：len ,this parameter limit how many ucb you could create
// PARA OUT：
// RETURN  ：
// INSTRUCT:
// =============================================================================
static  bool_t  __UdpCbInit(int len)
{
    bool_t result = false;
    int i = 0;

    pUcbFreeLstSync = Lock_MutexCreate(NULL);
    if(NULL == pUcbFreeLstSync)
    {
        return result;
    }

    pUcbFreeLst = malloc(len *sizeof(tagUdpCB));
    if(NULL != pUcbFreeLst)
    {
        memset((void *)pUcbFreeLst,0,sizeof(tagUdpCB)*len);
        if(len > 2)
        {
            for(i = 0;i < len -2;i++)
            {
                pUcbFreeLst[i].next = &pUcbFreeLst[i +1];
            }
        }
        else if(len == 2)
        {
            pUcbFreeLst[0].next = &pUcbFreeLst[1];
        }
        else
        {
            //do nothing if only one
        }

        result = true;
    }

    return result;
}
// =============================================================================
// FUNCTION：this function is used to free an ucb
// PARA  IN：
// PARA OUT：
// RETURN  ：the ucb you malloc else NULL
// INSTRUCT:
// =============================================================================
static  bool_t  __UdpCbFree(tagUdpCB  *cb)
{
    if(NULL != cb)
    {
        PkgTryFreeQ(cb->rbuf.ph);
        Lock_SempDelete(cb->rbuf.bufsync);
        if(Lock_MutexPend(pUcbFreeLstSync,CN_TIMEOUT_FOREVER))
        {
            memset((void *)cb,0,sizeof(tagUdpCB));
            cb->next =pUcbFreeLst;
            pUcbFreeLst = cb;
            Lock_MutexPost(pUcbFreeLstSync);
        }
    }
    return true;
}

// =============================================================================
// FUNCTION：this function is used to malloc an ucb
// PARA  IN：
// PARA OUT：
// RETURN  ：the ucb you malloc else NULL
// INSTRUCT:
// =============================================================================
static  tagUdpCB * __UdpCbMalloc(void )
{
    tagUdpCB           *result = NULL;
    if(Lock_MutexPend(pUcbFreeLstSync,CN_TIMEOUT_FOREVER))
    {
        result = pUcbFreeLst;
        if(NULL != result)
        {
            pUcbFreeLst = result->next;
        }
        Lock_MutexPost(pUcbFreeLstSync);
    }

    if(NULL != result)
    {
        //init the ucb member
        memset((void *)result, 0, sizeof(tagUdpCB));
        result->rbuf.bufsync = Lock_SempCreate(1,0,CN_BLOCK_FIFO,NULL);
        if(NULL == result->rbuf.bufsync)
        {
            __UdpCbFree(result);
        }
        else
        {
            //create two semphore for the rcv and sndbuf
            result->sndlimit          = CN_UDP_PKGMSGLEN;
            result->rbuf.buflenlimit  = CN_UDP_BUFLENDEFAULT;
            result->rbuf.timeout      = CN_TIMEOUT_FOREVER;
            result->channelstat       = CN_UDP_CHANNEL_STATASND|CN_UDP_CHANNEL_STATARCV|\
                                     CN_UDP_CHANNEL_STATKSND|CN_UDP_CHANNEL_STATKRCV;
        }
    }
    return result;
}
// =============================================================================
// FUNCTION：this function is used to generate an socket
// PARA  IN：family:such as AF_INET AF_INET6  AF_LOCAL
// PARA OUT：the socket generated
// RETURN  ：
// INSTRUCT:
// =============================================================================
static u16        gPortEngineUdp = 0x400;
static tagSocket * __socket(int family, int type, int protocal)
{
    tagSocket  *sock;
    tagSocket  *tmp;
    u32         hashKey;
    u16         port;
    u32         ip;
    bool_t      getport;
    tagUdpCB   *ucb;

    sock = NULL;
    if(AF_INET == family)
    {
        if(Lock_MutexPend(pUdpHashTab->tabsync,CN_TIMEOUT_FOREVER))
        {
            getport = false;
            while(getport == false)
            {
                port = gPortEngineUdp;
                ip = INADDR_ANY;
                hashKey = port%pUdpHashTab->tablen;
                tmp = pUdpHashTab->array[hashKey];
                while(NULL != tmp)
                {
                    if((ip == tmp->element.v4.iplocal)&&(port == tmp->element.v4.portlocal))
                    {
                        break;
                    }
                    else
                    {
                        tmp = tmp->nxt;
                    }
                }
                if(NULL == tmp)
                {
                    getport = true;
                }
                gPortEngineUdp++;
            }
            //now we got an port, so we should malloc an socket and do the initialize
            sock = SocketMalloc();
            ucb = __UdpCbMalloc();
            if((NULL != sock)&&(NULL != ucb))
            {
                //ok, now we got an port here
                sock->af_inet = AF_INET;
                sock->element.v4.iplocal = INADDR_ANY;
                sock->element.v4.portlocal = port;
                //initialize the sock add the sock to the hash tab
                sock->sockstat |=CN_SOCKET_PROBLOCK;
                sock->proto = &gTplProto;
                sock->iostat |= CN_SOCKET_IOWRITE;
                sock->cb = ucb;
                //add it to the socket queue
                sock->nxt = pUdpHashTab->array[hashKey];
                pUdpHashTab->array[hashKey] = sock;
            }
            else
            {
                if(NULL != ucb)
                {
                    __UdpCbFree(ucb);
                }
                if(NULL != sock)
                {
                    SocketFree(sock);
                    sock = NULL;
                }
            }
            Lock_MutexPost(pUdpHashTab->tabsync);
        }
    }//end if AF_INET == family
    return sock;
}
// =============================================================================
// FUNCTION：this function is used to bind an sock to the specified ip and port
// PARA  IN：
// PARA OUT：
// RETURN  ：0 success while -1 failed
// INSTRUCT:
// =============================================================================
static int __bind(tagSocket *sock,struct sockaddr *addr, int addrlen)
{
    int  result = -1;
    struct sockaddr_in *sockaddrin;
    tagSocket    *tmp;

    if((NULL != addr)&&(AF_INET == addr->sa_family)&&((NULL != sock)))
    {
        if((0 == (sock->sockstat&CN_SOCKET_BIND))&&\
           (0 == (CN_SOCKET_CLOSE&sock->sockstat)))  //NOT BIND YET
        {
            sockaddrin = (struct sockaddr_in *)addr;
            tmp = __hashSocketSearch(sockaddrin->sin_addr.s_addr,sockaddrin->sin_port);
            if(tmp == sock) //same socket, do noting
            {
                sock->element.v4.iplocal = sockaddrin->sin_addr.s_addr;
                sock->element.v4.portlocal = sockaddrin->sin_port;
                sock->sockstat |= CN_SOCKET_BIND;
                result = 0;
            }
            else
            {
                //remove it and reinsert
                __hashSocketRemove(sock);
                sock->element.v4.iplocal = sockaddrin->sin_addr.s_addr;
                sock->element.v4.portlocal = sockaddrin->sin_port;
                sock->sockstat |= CN_SOCKET_BIND;
                if( __hashSocketAdd(sock))
                {
                    result = 0;
                }
            }
        }
    }//end if AF_INET == family

    return result;
}

// =============================================================================
// FUNCTION：this function is used to snd the meassage to the ip layer
// PARA  IN：
// PARA OUT：
// RETURN  ：0 success while -1 failed
// INSTRUCT:the ipv6 udp not implement yet
// =============================================================================
static int __msgsnd(tagSocket *sock, const void *msg, int len, int flags,\
                  const struct sockaddr *addr, int addrlen)
{
    int             result;
    u16             translen;
    u32             iplocal, ipremote;
    u16             portlocal, portremote;
    tagUdpHdr       *hdr;
    tagNetPkg       *pkg,*pkgdata;
    tagUdpCB        *ucb;
    struct sockaddr_in *addrin;

    result = -1;
    ucb =(tagUdpCB *)sock->cb;
    if(ucb->sndlimit >=len)
    {
        pkg = PkgMalloc(sizeof(tagUdpHdr),0);
        if(NULL != pkg)
        {
            pkgdata = PkgMalloc(sizeof(tagNetPkg),CN_PKLGLST_END);
            if(NULL != pkgdata)
            {
                //compute the ip and port
                iplocal = sock->element.v4.iplocal;
                portlocal =  sock->element.v4.portlocal;

                addrin = (struct sockaddr_in*)addr;
                if(NULL != addrin)
                {
                    ipremote = addrin->sin_addr.s_addr;
                    portremote = addrin->sin_port;
                }
                else
                {
                    ipremote = sock->element.v4.ipremote;
                    portremote = sock->element.v4.portremote;
                }
                translen = len + sizeof(tagUdpHdr);
                //do the udp header
                hdr = (tagUdpHdr *)(pkg->buf + pkg->offset);
                hdr->portdst = portremote;
                hdr->portsrc = portlocal;
                hdr->msglen = htons(translen);
                hdr->chksum = 0;
                pkg->datalen = sizeof(tagUdpHdr);

                pkg->partnext = pkgdata;
                pkgdata->partnext = NULL;
                pkgdata->offset = 0;
                pkgdata->buf = (u8 *)msg;
                pkgdata->datalen = len;
                //ok, the msg has cpy to the pkg, then snd the pkg
                if(IpSend(EN_IPV_4,(ptu32_t)iplocal, (ptu32_t)ipremote, pkg,translen,IPPROTO_UDP,\
                                    CN_IPDEV_UDPOCHKSUM,&hdr->chksum))
                {
                    result = len;
                }
                PkgTryFreePart(pkg);
                PkgTryFreePart(pkgdata);
                TCPIP_DEBUG_INC(ucb->framsnd);
            }
            else
            {
                PkgTryFreePart(pkg);
            }
        }
    }
    return result;
}

// =============================================================================
// FUNCTION:use this function for the connect,
// PARA  IN:sock,the socket
//          addr,the net addr
//          addrlen,the netaddr len
// PARA OUT:
// RETURN  :0 success while -1 failed
// INSTRUCT:
// =============================================================================
static int __connect(tagSocket *sock, struct sockaddr *serveraddr, int addrlen)
{
    int  result=-1;
    struct sockaddr_in *addrin;

    addrin = (struct sockaddr_in *)serveraddr;
    if((NULL == addrin)||(addrlen != sizeof(struct sockaddr))||\
        (addrin->sin_family!= AF_INET))
    {
        return result;  //if para error
    }
    if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    {
        if(0 == (sock->sockstat & CN_SOCKET_CONNECT)) //make sure not connect yet
        {
            sock->element.v4.ipremote = addrin->sin_addr.s_addr;
            sock->element.v4.portremote = addrin->sin_port;
            sock->sockstat |= CN_SOCKET_CONNECT;
            result = 0;
        }
        Lock_MutexPost(sock->sync);
    }
    return  result;
}
// =============================================================================
// 函数功能:__send
//         向套接字发送数据
// 输入参数:sock,目的套接字
//        msg,待发送的信息
//        len,信息长度
//        flags,一般为0
// 输出参数：
// 返回值  ：-1出错，否则返回发送字节数
// 说明    ：
// =============================================================================
static int __send(tagSocket *sock, const void *msg, int len, int flags)
{
    int  result= -1;
    tagUdpCB  *ucb;

    if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    {
        ucb = (tagUdpCB *)sock->cb;
        if(CN_UDP_CHANNEL_STATASND&ucb->channelstat)
        {
            result = __msgsnd(sock,msg,len,flags,NULL,0);
        }
        Lock_MutexPost(sock->sync);
    }
    return  result;
}
// =============================================================================
// 函数功能：  __cpyfromrbuf
// 输入参数：
// 输出参数：
// 返回值  ：-1出错，否则返回拷贝的字节数
// 说明    ：must remember that each time only one frame
// =============================================================================
static int __cpyfromrbuf(tagSocket *sock, void *buf, int len,\
                     struct sockaddr *addr, int *addrlen)
{
    int cpylen;
    u8   *srcbuf;
    tagNetPkg  *pkg;
    tagUdpCB *ucb;

    ucb = (tagUdpCB *)sock->cb;
    srcbuf = (u8 *)buf;
    //可能会超负载，但是这不是关键
    //先看看看有没有没有填满的数据包来接纳我们的数据
    pkg = ucb->rbuf.ph;
    cpylen = len > pkg->datalen?pkg->datalen:len;
    srcbuf = (u8 *)(pkg->buf);
    if((NULL != addr)&&(NULL != addrlen))   //cpy the address
    {
        memcpy((void *)addr,srcbuf, sizeof(struct sockaddr));
        *addrlen = sizeof(struct sockaddr);
    }
    srcbuf += pkg->offset;
    memcpy(buf, srcbuf,cpylen);
    //udp是按数据帧的格式，所以即使没有接受完毕，我们也丢弃掉剩余的数据
    ucb->rbuf.buflen-=pkg->datalen;
    ucb->rbuf.ph = pkg->partnext;
    if(NULL == ucb->rbuf.ph)
    {
        ucb->rbuf.pt = NULL;
    }
    PkgTryFreePart(pkg);
    return cpylen;
}
// =============================================================================
// 函数功能：  __recv
//        读取接字数据
// 输入参数：  sock,目的套接字
//        flags,一般为0
// 输出参数：
//       buf,收到的信息
//       len,buf长度
// 返回值  ：-1出错，否则返回收到字节数
// 说明    ：
// =============================================================================
static int __recv(tagSocket *sock, void *buf,int len, unsigned int flags)
{
    int  result = -1;
    tagUdpCB *ucb;
    u32  timeout;
    ucb = (tagUdpCB *)sock->cb;

    timeout = ucb->rbuf.timeout;
    if(Lock_SempPend(ucb->rbuf.bufsync,timeout))
    {
        if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
        {
            if(ucb->channelstat & CN_UDP_CHANNEL_STATARCV)
            {
                if(NULL != ucb->rbuf.ph)
                {
                    result = __cpyfromrbuf(sock,buf,len,NULL, NULL);
                }
                else
                {
                    result = -1;
                }
            }
            else
            {
                result = 0;
            }

            //if any data or could receive again
            if((ucb->rbuf.buflen > ucb->rbuf.triglevel)||(0 == (ucb->channelstat & CN_UDP_CHANNEL_STATARCV)))
            {
                sock->iostat |= CN_SOCKET_IOREAD;
                Multiplex_Set(sock->ioselect, sock->iostat);
                Lock_SempPost(ucb->rbuf.bufsync);
            }
            else
            {
                sock->iostat &= (~CN_SOCKET_IOREAD);
                Multiplex_Set(sock->ioselect, sock->iostat);
            }
            Lock_MutexPost(sock->sync);
        }
    }
    else
    {
        result = -1;//maybe timeout ;need more try
    }
    return  result;
}


// =============================================================================
// 函数功能：  __sendto
//        向目的网络地址发送数据
// 输入参数：  sock,     目的套接字
//        msg,        待发送的信息
//        len,        信息长度
//        flags,      一般为0
//        addr,       目的网络地址
//        addrlen,    目的网络地址长度
// 输出参数：
// 返回值  ：-1出错，否则返回发送字节数
// 说明    ：
// =============================================================================
static int __sendto(tagSocket *sock, const void *msg,int len, unsigned int flags,\
              const struct sockaddr *addr, int addrlen)
{
    int  result= -1;
    tagUdpCB   *ucb;

    if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    {
        ucb = (tagUdpCB *)sock->cb;
        if(CN_UDP_CHANNEL_STATASND&ucb->channelstat)
        {
            result = __msgsnd(sock,msg,len,flags,addr,addrlen);
        }
        Lock_MutexPost(sock->sync);
    }
    return  result;
}
// =============================================================================
// 函数功能： __recvfrom
//           从目的网络地址读取数据
// 输入参数：  sock,目的套接字
//           flags,一般为0
//           addr,目的网络地址
//           addrlen,目的网络地址长度
// 输出参数：
//           buf,读取的数据
//           len,信息长度
// 返回值  ：-1出错，否则返回收到字节数
// 说明    ：
// =============================================================================
static int __recvfrom(tagSocket *sock,void *buf, int len, unsigned int flags,\
            struct sockaddr *addr, int *addrlen)
{
    int  result = -1;
    tagUdpCB *ucb;
    u32  timeout;

    ucb = (tagUdpCB *)sock->cb;
    timeout = ucb->rbuf.timeout;
    if(Lock_SempPend(ucb->rbuf.bufsync,timeout))
    {
        if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
        {
            if(ucb->channelstat & CN_UDP_CHANNEL_STATARCV)
            {
                if(NULL != ucb->rbuf.ph)
                {
                    result = __cpyfromrbuf(sock,buf,len,addr, addrlen);
                }
                else
                {
                    result = -1;
                }
            }
            else
            {
                result = 0;
            }

            //if any data or could receive again
            if((ucb->rbuf.buflen > ucb->rbuf.triglevel)||(0 == (ucb->channelstat & CN_UDP_CHANNEL_STATARCV)))
            {
                sock->iostat |= CN_SOCKET_IOREAD;
                Multiplex_Set(sock->ioselect, sock->iostat);
                Lock_SempPost(ucb->rbuf.bufsync);
            }
            else
            {
                sock->iostat &= (~CN_SOCKET_IOREAD);
                Multiplex_Set(sock->ioselect, sock->iostat);
            }
            Lock_MutexPost(sock->sync);
        }
    }
    else
    {
        result = -1;//maybe timeout ;need more try
    }
    return  result;
}
// =============================================================================
// 函数功能：  __shutdown_rd
//           shutdown the socked  read channel
// 输入参数：  socket, the socket to be shutdown
// 输出参数：
// 返回值  ：0 success while -1 failed
// 说明    ：
// =============================================================================
static int __shutdown_rd(tagSocket *sock)
{
    int result;
    tagUdpCB *ucb;

    result = -1;
    ucb = (tagUdpCB *)sock->cb;
    if(CN_UDP_CHANNEL_STATARCV& ucb->channelstat)
    {
        //clear the rcv buf and recombination queue
        //and set some specified flags at the same time
        ucb->channelstat&=(~(CN_UDP_CHANNEL_STATARCV|CN_UDP_CHANNEL_STATKRCV));
        //release the rcv and recombination queue
        PkgTryFreeQ(ucb->rbuf.ph);
        ucb->rbuf.buflen = 0;
        ucb->rbuf.buflenlimit  = CN_UDP_BUFLENDEFAULT;
        ucb->rbuf.ph = NULL;
        ucb->rbuf.pt = NULL;
        ucb->rbuf.triglevel = 0;

        result = 0;
    }
    return result;
}
// =============================================================================
// 函数功能：  __shutdown_wr
//        shutdown the socked  snd channel
// 输入参数：  socket, the socket to be shutdown
// 输出参数：
// 返回值  ：0 success while -1 failed
// 说明    ：
// =============================================================================
static int __shutdown_wr(tagSocket *sock)
{
    int result;
    tagUdpCB     *ucb;

    result = -1;
    ucb = (tagUdpCB *)sock->cb;
    if(CN_UDP_CHANNEL_STATASND&ucb->channelstat)
    {
        ucb->channelstat&=(~(CN_UDP_CHANNEL_STATKSND|CN_UDP_CHANNEL_STATASND));  //APP NO PERMIT TO TRANSMISSION
        result = 0;
    }
    return result;
}
// =============================================================================
// 函数功能：  __shutdown
//        关闭套接字
// 输入参数：  sock,目的套接字
//        how,关闭方法， 见SHUT_RD等定义
// 输出参数：
// 返回值  ：0 成功  -1失败
// 说明    ：
// =============================================================================
static int __shutdown(tagSocket *sock, u32 how)
{
    int    result;

    if(Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER))
    {
        switch(how)
        {
            case SHUT_RD:
                result = __shutdown_rd(sock);
                break;
            case SHUT_WR:
                result = __shutdown_wr(sock);
                break;
            case SHUT_RDWR:
                result = __shutdown_rd(sock);
                if(0 ==result)
                {
                    result = __shutdown_wr(sock);
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
// 函数功能：  __socketclose
// 输入参数：  sock,待关闭的套接字
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
static int __socketclose(tagSocket *sock)
{
    int result = -1;

    __hashSocketRemove(sock);
    if(Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    {
        __UdpCbFree(sock->cb);
        sock->cb = NULL;
        SocketFree(sock);
        result = 0;
    }
    return result;
}
// =============================================================================
// 函数功能：  __sol_socket
//           SOL_SOCKET选项处理
// 输入参数：  sock,待处理的套接字
//           optname,选项名字
//           optval,待设置参数
//           optlen,参数长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
static int __sol_socket(tagSocket *sock,int optname,const void *optval, int optlen)
{
    bool_t result = -1;
    tagUdpCB *ucb;

    ucb = (tagUdpCB *)sock->cb;
    switch(optname)
    {
        case SO_BROADCAST:
            if(*(int *)optval)
            {
                sock->sockstat |=CN_SOCKET_PROBROAD;
            }
            else
            {
                sock->sockstat &=(~CN_SOCKET_PROBROAD);
            }
            result = 0;
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
            if(*(int *)optval > 0)
            {
                ucb->rbuf.buflenlimit =*(int *)optval;
                result = 0;
            }
            break;
        case SO_SNDBUF:
            if(*(int *)optval > 0)
            {
                ucb->sndlimit = *(int *)optval;
                result = 0;
            }
            break;
        case SO_RCVLOWAT:
            if(*(int *)optval >= 0)
            {
                ucb->rbuf.triglevel =*(int *)optval;
                result = 0;
            }
            break;
        case SO_SNDLOWAT:
            break;
        case SO_RCVTIMEO:
            sock->sockstat |=CN_SOCKET_PROBLOCK;
            ucb->rbuf.timeout = *(u32 *)optval;
            result = 0;
            break;
        case SO_SNDTIMEO:
            break;
        case SO_REUSEADDR:
            break;
        case SO_TYPE:
            break;
        case SO_BSDCOMPAT:
            break;
        case SO_NOBLOCK:
            if(*(int *)optval)
            {
                sock->sockstat &=(~CN_SOCKET_PROBLOCK);
                ucb = (tagUdpCB *)sock->cb;
                ucb->rbuf.timeout = 0;
            }
            else
            {
                sock->sockstat |=CN_SOCKET_PROBLOCK;
                ucb = (tagUdpCB *)sock->cb;
                ucb->rbuf.timeout = CN_TIMEOUT_FOREVER;
            }
            result = 0;
            break;
        default:
            break;
    }
    return result;
}
// =============================================================================
// 函数功能：  __ipproto_ip
//           IPPROTO_IP选项处理
// 输入参数：         sock,待处理的套接字
//           optname,选项名字
//           optval,待设置参数
//           optlen,参数长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
static int __ipproto_ip(tagSocket *sock,int optname,const void *optval, int optlen)
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
// 函数功能：  __setsockopt
//           设置套接字选项
// 输入参数：  sock,目的套接字
//           level,设置的层次，支持SOL_SOCKET、IPPROTO_TCP、IPPROTO_IP和IPPROTO_IPV6
//           optname,需设置选项
//           optval,缓冲区
//           optlen,缓冲区长度
// 输出参数：
// 返回值  ：0 成功 -1失败
// 说明    ：
// =============================================================================
static int __setsockopt(tagSocket *sock, int level, int optname,\
               const void *optval, int optlen)
{
    int  result;

    result = -1;

    if(Lock_MutexPend(sock->sync,CN_TIMEOUT_FOREVER))
    {
        switch(level)
        {
            case SOL_SOCKET:
                result = __sol_socket(sock,optname,optval,optlen);
                break;
            case IPPROTO_IP:
                result = __ipproto_ip(sock,optname,optval,optlen);
                break;
            case IPPROTO_TCP:
                break;
            default:
                break;
        }
        Lock_MutexPost(sock->sync);
    }

    return  result;
}

static void __addpkg2rbuf(tagUdpCB *ucb, tagNetPkg *pkg)
{
    if(NULL == ucb->rbuf.pt)
    {
        ucb->rbuf.pt = pkg;
        ucb->rbuf.ph = pkg;
    }
    else
    {
        ucb->rbuf.pt->partnext = pkg;
        ucb->rbuf.pt = pkg;
    }
    ucb->rbuf.buflen+= pkg->datalen;

    PkgCachedPart(pkg);
    TCPIP_DEBUG_INC(ucb->framrcv);
    return;
}

// =============================================================================
// FUNCTION:this function is used to do the udp rcv
// PARA  IN:
// PARA OUT：
// RETURN  :
// INSTRUCT:
// =============================================================================
static bool_t __UdpRcvV4(u32 ipsrc, u32 ipdst, tagNetPkg *pkg, tagRout *rout)
{
    bool_t              result ;
    u16                 portdst;
    u16                 portsrc;
    tagUdpHdr          *hdr;
    tagUdpCB           *ucb;
    tagSocket          *sock;
    struct  sockaddr_in addrin;
    u32                 devfunc;

    result  = true;
    devfunc = NetDevGetFunc(rout->dev);
    if((NULL == pkg)||(pkg->datalen <= sizeof(tagUdpHdr)))
    {
        goto EXIT_RCVEND;  //para error
    }
    hdr = (tagUdpHdr *)(pkg->buf + pkg->offset);
    if((0 != hdr->chksum)&&(0 == (devfunc&CN_IPDEV_UDPICHKSUM))) //if need chksum
    {
        IpPseudoPkgLstChkSumV4(ipdst, ipsrc, IPPROTO_UDP,\
                       pkg,htons(hdr->msglen),&hdr->chksum);
        if(0 != hdr->chksum)
        {
            goto EXIT_RCVEND;
        }
    }
    pkg->datalen -= sizeof(tagUdpHdr);
    pkg->offset += sizeof(tagUdpHdr);
    if(0 == pkg->datalen)
    {
        goto EXIT_RCVEND;
    }
    //ok,now we deal the pkg now, the pkg check success
    portdst = hdr->portdst;
    portsrc = hdr->portsrc;

    //find the socket
    sock = __hashSocketSearch(ipdst,portdst);
    if(NULL == sock)
    {
        sock =__hashSocketSearch(rout->ipaddr.ipv4.ip,portdst); //if rout ip match
    }
    if(NULL == sock)
    {
        sock =__hashSocketSearch(INADDR_ANY,portdst); //if INADDR_ANY match
    }

    if((NULL != sock)&&Lock_MutexPend(sock->sync, CN_TIMEOUT_FOREVER))
    {
        ucb = (tagUdpCB *)sock->cb;
        result = true;

        if((ipdst == INADDR_BROAD)&&(0 == (CN_SOCKET_PROBROAD &sock->sockstat)))
        {
            result = false;    //could not receive broad info
        }

        if(CN_UDP_CHANNEL_STATCONGEST&ucb->channelstat)
        {
            result = false;  //no mem for the info
        }
        if(ucb->rbuf.buflen >= ucb->rbuf.buflenlimit)
        {
            result = false;
        }

        if(result)
        {
            if(pkg->offset >= sizeof(struct sockaddr_in))
            {
                addrin.sin_family = AF_INET;
                addrin.sin_port =portsrc;
                addrin.sin_addr.s_addr = ipsrc;
                memcpy((void *)pkg->buf,(void *)&addrin, sizeof(addrin));
                __addpkg2rbuf(ucb,pkg);
                if(ucb->rbuf.buflen > ucb->rbuf.triglevel)
                {
                    sock->iostat |= CN_SOCKET_IOREAD;
                    Multiplex_Set(sock->ioselect, sock->iostat);
                    Lock_SempPost(ucb->rbuf.bufsync);
                }
            }
        }
        Lock_MutexPost(sock->sync);
    }
    else
    {
        //do the icmp host not reached
    }

EXIT_RCVEND:
    return true;
}

static bool_t __recvProcess(enum_ipv_t  ver, ptu32_t ipsrc, ptu32_t ipdst, tagNetPkg *pkg, tagRout *rout)
{
    bool_t result =false;
    if(EN_IPV_4 == ver)
    {
        result = __UdpRcvV4((u32)ipsrc, (u32)ipdst,pkg,rout);
    }

    return result;
}



#define CN_UDP_DEBUG_PREFIX  "         "
static void __debug(tagSocket *sock,char *filter)
{
    tagUdpCB    *ucb = NULL;
    char *prefix = CN_UDP_DEBUG_PREFIX;
    printf("UDPSOCKET:fd:%d address:0x%08x\n\r",sock->sockfd,(u32)sock);
    printf("%s:iplocal :%s    portlocal :%d\r\n",\
            prefix,inet_ntoa(*(struct in_addr*)&sock->element.v4.iplocal),ntohs(sock->element.v4.portlocal));
    printf("%s:ipremote:%s    portremote:%d\r\n",\
            prefix,inet_ntoa(*(struct in_addr*)&sock->element.v4.ipremote),ntohs(sock->element.v4.portremote));
    printf("%s:iostat  :0x%08x sockstat:0x%08x    private:0x%08x\n\r",\
            prefix,sock->iostat,sock->sockstat,sock->private);
    printf("%s:syncstat:%d\n\r",prefix,sock->sync->enable);
    printf("%s:errno   :%d\n\r",prefix,sock->errorno);

    ucb = (tagUdpCB    *)sock->cb;
    if(NULL != ucb)
    {
        printf("%s:channelstat:0x%04x\n\r",prefix,ucb->channelstat);
        printf("%s:rbuf:len:0x%08x buflen:0x%08x triglevel:0x%08x timeout:0x%08x sync:%d\n\r",\
                prefix,ucb->rbuf.buflen,ucb->rbuf.buflenlimit,ucb->rbuf.triglevel,\
                ucb->rbuf.timeout,ucb->rbuf.bufsync->lamp_counter);
        printf("%s:framrcv:0x%llx  framsnd:0x%llx\n\r",prefix,ucb->framrcv,ucb->framsnd);
    }
    else
    {
        printf("%s:no control block yet\n\r",prefix);
    }
}

// =============================================================================
// FUNCTION：this function is used to initialize the udp protocol
// PARA  IN：
// PARA OUT：
// RETURN  ：true success while false failed
// INSTRUCT:
// =============================================================================
bool_t UdpInit(ptu32_t para)
{
    bool_t   result = false;

    if(false == __hashTabInit(gUdpSockNum))
    {
        return result;
    }
    if(false ==__UdpCbInit(gUdpSockNum))
    {
        return result;
    }

    memset((void *)&gTplProto,0, sizeof(gTplProto));
    gTplProto.socket     = __socket;
    gTplProto.bind       = __bind;
    gTplProto.connect    = __connect;
    gTplProto.recv       = __recv;
    gTplProto.recvfrom   = __recvfrom;
    gTplProto.send       = __send;
    gTplProto.sendto     = __sendto;
    gTplProto.setsockopt = __setsockopt;
    gTplProto.shutdown   = __shutdown;
    gTplProto.close      = __socketclose;
    gTplProto.debuginfo  = __debug;

    result = TPL_RegisterProto(AF_INET,SOCK_DGRAM,0, &gTplProto);
    if(result)
    {
        result = TPL_RegisterProto(AF_INET,SOCK_DGRAM,IPPROTO_UDP, &gTplProto);
        if(result)
        {
            result = IpRegisterTplHandler(IPPROTO_UDP,__recvProcess);
        }
    }

    return result;
}






