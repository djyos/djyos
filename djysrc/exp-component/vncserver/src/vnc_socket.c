// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：socket.c
// 创建人员: Administrator
// 创建时间: 2013-3-5
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================
/*
 * sockets.c - deal with TCP & UDP sockets.
 *
 * This code should be independent of any changes in the RFB protocol.  It just
 * deals with the X server scheduling stuff, calling rfbNewClientConnection and
 * rfbProcessClientMessage to actually deal with the protocol.  If a socket
 * needs to be closed for any reason then rfbCloseClient should be called, and
 * this in turn will call rfbClientConnectionGone.  To make an active
 * connection out, call rfbConnect - note that this does _not_ call
 * rfbNewClientConnection.
 *
 * This file is divided into two types of function.  Those beginning with
 * "rfb" are specific to sockets using the RFB protocol.  Those without the
 * "rfb" prefix are more general socket routines (which are used by the http
 * code).
 *
 * Thanks to Karl Hakimian for pointing out that some platforms return EAGAIN
 * not EWOULDBLOCK.
 */

/*
 *  OSXvnc Copyright (C) 2001 Dan McGuirk <mcguirk@incompleteness.net>.
 *  Original Xvnc code Copyright (C) 1999 AT&T Laboratories Cambridge.
 *  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */
#include <WinSock2.h>
#include <Windows.h>
#include "rfb.h"
#include "debug.h"
//#include "misc.h"  //一些很杂很乱的定义
//#include "rfbproto_processor.h"
#include "vnc_thread.h"  //锁资源
#include  "colormap.h"
#include  "color_remap.h"
#include "rfbproto_processor.h"

int rfbMaxClientWait = 2000;   /* time (ms) after which we decide client has
                                   gone away - needed to stop us hanging */
#pragma comment(lib, "WS2_32")

// =========================================================================
// 函数功能：善后客户端关闭后的内存问题
// 输入参数：待关闭的客户端参数cl
// 输出参数：
// 返回值    ：
// 说明         ：切断待关闭的客户端cl和其关联的screen之间的关系
// =========================================================================
void rfbClientConnectionGone(rfbClientPtr cl)
{
//    int i;
 //   debug_printf("Client %s gone\n",cl->host);
    free(cl->host);//目前使用的是windows的gethostname.

    /* Release the compression state structures if any. */
    /*
    if ( cl->compStreamInited == TRUE ) {
    deflateEnd( &(cl->compStream) );
    }

    for (i = 0; i < 4; i++) {
    if (cl->zsActive[i])
        deflateEnd(&cl->zsStruct[i]);
    }
    */
    if (cl->prev)
        cl->prev->next = cl->next;
    else
        cl->screen->rfbClientHead = cl->next;
    if (cl->next)
        cl->next->prev = cl->prev;

    cl->screen->cl_number--;
    /*
    if (cl->translateLookupTable)
        free(cl->translateLookupTable);
    */
    free(cl);
    debug_printf("@close socket completely!\n");
}

// =========================================================================
// 函数功能：关闭客户端
// 输入参数：待关闭的客户端参数cl
// 输出参数：
// 返回值    ：
// 说明         ：关闭套接字，处理内存，切断和screen之间的关系
// =========================================================================
void rfbCloseClient(rfbClientPtr cl)
{
    debug_printf("begin to shutdown the client!\n");
//  从screen的fds表中删除
    FD_CLR(cl->sock,&(cl->screen->allFds));
//  关闭socket
    closesocket(cl->sock);
//    WSACleanup();
//  处理cl的内存问题
    rfbClientConnectionGone(cl);
}

/*
 * ReadExact reads an exact number of bytes from a client.  Returns 1 if
 * those bytes have been read, 0 if the other end has closed, or -1 if an error
 * occurred (errno is set to ETIMEDOUT if it timed out).
 */
// =========================================================================
// 函数功能：从客户端读取数据
// 输入参数：客户端参数cl，存储buf，长度len
// 输出参数：
// 返回值  ：1成功，-1失败
// 说明    ：读取固定为len的字节数
// =========================================================================
int ReadExact(rfbClientPtr cl, char *buf, int len)
{
    int sock = cl->sock;
    int n;
    fd_set fds;
    struct timeval tv;
    int totalTimeWaited = 0;
    while (len > 0)
    {
        n = recv(sock, buf, len,0);

        if (n > 0) {

            buf += n;
            len -= n;

        }  else {
            FD_ZERO(&fds);
            FD_SET(sock, &fds);
            //tv.tv_sec = rfbMaxClientWait / 1000;
            //tv.tv_usec = (rfbMaxClientWait % 1000) * 1000;
            tv.tv_sec = 0;
            tv.tv_usec =1000;

            n = select(sock+1, &fds, NULL, NULL, &tv);
            if (n < 0) {
                debug_printf("@ReadExact: select");
                return n;
            }
            if (n == 0) {

                  debug_printf("@read:select time out!\n");
                 totalTimeWaited += 1000;
                if (totalTimeWaited >= rfbMaxClientWait) {

                    return -1;
                }
            }
            else{
                totalTimeWaited=0;
            }
        }
    }
    return 1;
}

/*
 * WriteExact writes an exact number of bytes to a client.  Returns 1 if
 * those bytes have been written, or -1 if an error occurred (errno is set to
 * ETIMEDOUT if it timed out).
 */
// =========================================================================
// 函数功能：向客户端cl写入数据
// 输入参数：客户端参数cl，存储buf，长度len
// 输出参数：
// 返回值     ：1成功，-1失败
// 说明          ：写入固定为len的字节数
// =========================================================================
#define WAIT_TIMES  50000   //50毫秒
int WriteExact(rfbClientPtr cl, char *buf, int len)
{
    int sock = cl->sock;
    int n;
    fd_set fds;
    struct timeval tv;
    int totalTimeWaited = 0;
    int write_len=0;
    int total_len=0;
    while (len > 0)
    {
        n = send(sock, buf, len,0);
        if (n > 0)
        {
            buf += n;
            len -= n;
            total_len+=n;
        }
        else
        {
            FD_ZERO(&fds);
            FD_SET(sock, &fds);
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            n = select(sock+1, NULL, &fds, NULL, &tv);
            if (n < 0)
            {
                debug_printf("##WriteExact: select");
                return -1;
            }
            if (n == 0)
            {
                totalTimeWaited += 1000;
                debug_printf("##write:select time out");
                if (totalTimeWaited >= WAIT_TIMES) {
                    return -1;
                }
            }
            else
            {
                totalTimeWaited = 0;
            }
        }
    }
    return 1;
}
// ========================================================================
// 函数功能:处理TCP的listen问题
// 输入参数:port，所需要监听的TCP端口号，-1代表失败
// 输出参数：
// 返回值    :对应于此端口号的socket套接字
// 说明         :使用程序者应该能保证所提供给的port是可用的，否则会不成功
// =========================================================================
int ListenOnTCPPort(int port)
{
    struct sockaddr_in addr;
    int sock;
    int one = 1024;

    int length=0;

    //windows特有的，开启网络
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    //addr.sin_addr.s_addr = interface.s_addr;
    addr.sin_addr.s_addr = INADDR_ANY;

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        debug_printf("socket failed!\n");
        return -1;
    }
    /**/
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,(char *)&one, sizeof(one)) < 0)
    {
        closesocket(sock);
        debug_printf("setsockopt failed!\n");
        return -1;
    }


    printf("----has reached!--------------!\n");

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        closesocket(sock);
        debug_printf("bind failed!\n");
        return -1;
    }
    if (listen(sock, 5) < 0)
    {
        closesocket(sock);
        debug_printf("listen failed!\n");
        return -1;
    }

    getsockname(sock,&addr,&length);

    printf( "本侧IP：%s ", inet_ntoa(addr.sin_addr));
    printf( "本侧PORT：%d ", ntohs(addr.sin_port));

    return sock;
}

// =========================================================================
// 函数功能：初始化screen的网络端口，在此只针对TCP链接问题
// 输入参数: 建立的服务器屏screen
// 输出参数：
// 返回值  :
// 说明    :  换句话说此函数主要实现了套接字和fds的设置
// =========================================================================
void  rfbInitSockets(rfbScreenInfoPtr rfbScreen)
{
    if (rfbScreen->socketInitDone)
    return;

    rfbScreen->socketInitDone = TRUE;
//  除了TCP，其他的方式建立的连接不做处理--TODO
    debug_printf("Begin to set TCP port %d at listen!\n", rfbScreen->rfbPort);

    if ((rfbScreen->rfbListenSock = ListenOnTCPPort(rfbScreen->rfbPort)) < 0)
    {
        debug_printf("fatal error!ListenOnTCPPort=%d\n",rfbScreen->rfbListenSock);
        exit(1);
    }

    FD_ZERO(&(rfbScreen->allFds));
    FD_SET(rfbScreen->rfbListenSock, &(rfbScreen->allFds));
    rfbScreen->maxFd = rfbScreen->rfbListenSock;

    debug_printf("Has  set TCP port %d at listen!\n", rfbScreen->rfbPort);
}
// =========================================================================
// 函数功能:建立一个新的客户
// 输入参数:服务器端屏幕：screen，accept到的套接字
// 输出参数：
// 返回值    :新的客户端参数
// 说明         :该新建的参数已经添加到screen的cl链表当中，失败返回NULL
// =========================================================================
rfbClientPtr rfbNewClient(rfbScreenInfoPtr rfbScreen,int sock)
{

    rfbProtocolVersionMsg pv;
    rfbClientPtr cl;
    struct sockaddr_in addr;
    int addrlen = sizeof(struct sockaddr_in);
    int i = 0;

    cl = (rfbClientPtr)M_MallocLc(sizeof(rfbClientRec),0);
    if(NULL == cl)
    {
        debug_printf("#error: no mem more for the newclient!\n");
        return cl;
    }
    rfbScreen->cl_number++;
    //加入到screen的链表之中，新建的总是在排在链表头，双向不循环列表
    cl->next = rfbScreen->rfbClientHead;
    cl->prev = NULL;
    if (rfbScreen->rfbClientHead)
        rfbScreen->rfbClientHead->prev = cl;
    rfbScreen->rfbClientHead = cl;
//开始初始化cl的成员变量
    //关于socket的
    cl->screen = rfbScreen;
    cl->sock = sock;
    getpeername(sock, (struct sockaddr *)&addr, &addrlen);
    cl->host = strdup(inet_ntoa(addr.sin_addr));


    //关于建立连接的部分
    cl->readyForSetColourMapEntries = FALSE;
    cl->InitDone=FALSE;
    cl->HasRequest = 0;
    cl->state = RFB_PROTOCOL_VERSION;
    cl->client_dirty = 0;

    //关于加密方式
    cl->useCopyRect = FALSE;
    cl->preferredEncoding = rfbEncodingRaw;
    cl->correMaxWidth = 48;
    cl->correMaxHeight = 48;
    cl->modifiedregion.x=0;
    cl->modifiedregion.y=0;
    cl->modifiedregion.w=0;
    cl->modifiedregion.h=0;
    //关于压缩算法的部分初始化
    cl->tightCompressLevel = TIGHT_DEFAULT_COMPRESSION;
    cl->tightQualityLevel = -1;
    for (i = 0; i < 4; i++)
        cl->zsActive[i] = FALSE;
    cl->enableCursorShapeUpdates = FALSE;
    cl->enableLastRectEncoding = FALSE;
    cl->compStreamInited = FALSE;
    cl->zlibCompressLevel = 5;




//设置cl的颜色映射函数，由于颜色映射做的仅仅和screen的颜色bits相关，所以在此设定
    cl->format = cl->screen->rfbServerFormat;
    set_client_colormap_function(cl);
//  新建的客户，开始向客户端发送版本号，开始进行握手阶段
    sprintf(pv,rfbProtocolVersionFormat,rfbProtocolMajorVersion,
            rfbProtocolMinorVersion);
    if (WriteExact(cl, pv, sz_rfbProtocolVersionMsg) < 0) {
        debug_printf("rfbNewClient: write");
        rfbCloseClient(cl);
        return NULL;
    }

    return cl;
}

/*
 * rfbCheckFds is called from ProcessInputEvents to check for input on the RFB
 * socket(s).  If there is input to process, the appropriate function in the
 * RFB server code will be called (rfbNewClientConnection,
 * rfbProcessClientMessage, etc).
 */
// =========================================================================
// 函数功能:轮循screen的端口，包括服务器端口和客户端端口
// 输入参数：服务器屏幕screen以及超时时间设置
// 输出参数：
// 返回值  :
// 说明    :轮循服务器的端口是为了检测是否有新的客户需要加入，
//          轮循客户端口是为了看看客户端是否有新的通信信息需要处理
// =========================================================================
void rfbCheckFds(rfbScreenInfoPtr rfbScreen,long usec)
{
    int nfds;
    fd_set fds;
    struct timeval tv;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
//    char buf[6];
    const int one = 32*1024;
    int noblock=1;
    int sock;
    rfbClientPtr cl,cl_next=NULL;

    memcpy((char *)&fds, (char *)&(rfbScreen->allFds), sizeof(fd_set));
    tv.tv_sec = 0;
    tv.tv_usec = usec;
    nfds = select(rfbScreen->maxFd + 1, &fds, NULL, NULL, &tv);
    if (nfds == 0)
    {
//      debug_printf("--fds:select time out!\n");
        return;
    }
    if (nfds < 0)
    {
        debug_printf("#error：rfbCheckFds: select");
        return;
    }

    //其实listensock就是传说中的TCPsock，是默认的
    if ((rfbScreen->rfbListenSock != -1) && ( FD_ISSET(rfbScreen->rfbListenSock, &fds)))
    {
        if ((sock = accept(rfbScreen->rfbListenSock,(struct sockaddr *)&addr, &addrlen)) == INVALID_SOCKET)
        {
            debug_printf("rfbCheckFds: accept");
            goto check_sockfd;
        }
        ioctlsocket(sock,FIONBIO,&noblock);
        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,(char *)&one, sizeof(one)) != 0)
        {
            debug_printf("#error：rfbCheckFds: setsockopt failed");
            closesocket(sock);
            goto check_sockfd;
        }

        debug_printf("Got connection from client %s\n", inet_ntoa(addr.sin_addr));

//      将新连接的sock添加到fds的列表中
        FD_SET(sock, &(rfbScreen->allFds));
        rfbScreen->maxFd = max(sock,rfbScreen->maxFd);

        rfbNewClient(rfbScreen,sock);

        FD_CLR(rfbScreen->rfbListenSock, &fds);
    }
check_sockfd:
    cl= rfbScreen->rfbClientHead;
    do{
        if(cl)
        {
            cl_next=cl->next;
            if(cl->client_dirty == 1)//该client为脏，要删除的对象
            {
                rfbCloseClient(cl);
            }
            else
            {
                if (FD_ISSET(cl->sock, &fds) && FD_ISSET(cl->sock, &(rfbScreen->allFds)))
                {
                    rfbProcessClientMessage(cl);//处理client和server的正常通信状态
                }
                cl=cl_next;
            }
        }
        else
        {
          break;
        }
    }while(cl);
}









