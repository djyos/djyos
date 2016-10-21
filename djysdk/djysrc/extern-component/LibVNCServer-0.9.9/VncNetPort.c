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

#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"


#include <sys/socket.h>
#include <rfb/rfb.h>
#include "VncOsPort.h"  //锁资源
#include "os.h"
#include "endian.h"
int rfbMaxClientWait = 2000;   /* time (ms) after which we decide client has
                                   gone away - needed to stop us hanging */


// =========================================================================
// 函数功能：关闭客户端
// 输入参数：待关闭的客户端参数cl
// 输出参数：
// 返回值    ：
// 说明         ：关闭套接字，处理内存，切断和screen之间的关系
// =========================================================================
void rfbCloseClient(rfbClientRec* cl)
{
    printk("begin to shutdown the client!\n\r");
//  从screen的fds表中删除
//    Socket_MultiplexDel(cl->screen->allFds, cl->sock);
//    closesocket(cl->sock);
//    WSACleanup();
//  处理cl的内存问题
    rfbClientConnectionGone(cl);
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

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printk("socket failed!\n\r");
        return -1;
    }

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        closesocket(sock);
        printk("bind failed!\n\r");
        return -1;
    }
    if (listen(sock, 5) < 0)
    {
        closesocket(sock);
        printk("listen failed!\n\r");
        return -1;
    }
    printk("ListenOnTCPPort:Create Socket Success!--socket = %d\n\r",sock);
    return sock;
}

// =========================================================================
// 函数功能：初始化screen的网络端口，在此只针对TCP链接问题
// 输入参数: 建立的服务器屏screen
// 输出参数：
// 返回值  :
// 说明    :  换句话说此函数主要实现了套接字和fds的设置
// =========================================================================
void  rfbInitServerSocket(rfbScreenInfoPtr rfbScreen)
{


    if ((rfbScreen->listenSock = ListenOnTCPPort(rfbScreen->listenport)) < 0)
    {
        printk("rfbInitServerSocket:ServerSocket Init Failed");
    }

    rfbScreen->allFds = Multiplex_Creat(1);
    Socket_MultiplexAdd(rfbScreen->allFds,rfbScreen->listenSock,CN_SOCKET_IOACCEPT);
}
// =========================================================================
// 函数功能:建立一个新的客户
// 输入参数:服务器端屏幕：screen，accept到的套接字
// 输出参数：
// 返回值    :新的客户端参数
// 说明         :该新建的参数已经添加到screen的cl链表当中，失败返回NULL
// =========================================================================
//rfbClientRec* rfbNewClient(rfbScreenInfoPtr rfbScreen,int sock)
//{
//
//    rfbProtocolVersionMsg pv;
//    rfbClientRec* cl;
//
//    cl = (rfbClientRec*)M_Malloc(sizeof(rfbClientRec),0);
//    if(NULL == cl)
//    {
//        printk("#error: no mem more for the newclient!\n\r");
//        return cl;
//    }
//
////开始初始化cl的成员变量
//    //关于socket的
//    cl->screen = rfbScreen;
//    cl->sock = sock;
////    getpeername(sock, (struct sockaddr *)&addr, &addrlen);
////    cl->host = strdup(inet_ntoa(addr.sin_addr));
//    cl->host = NULL;
//    //关于建立连接的部分
//    cl->readyForSetColourMapEntries = FALSE;
//    cl->initdone=false;
//    cl->framreq = false;
//    cl->state = RFB_PROTOCOL_VERSION;
//    //关于加密方式
//    cl->useCopyRect = FALSE;
//    cl->preferredEncoding = -1;
//    cl->correMaxWidth = 0;
//    cl->correMaxHeight = 0;
//    cl->reqregion.x=0;
//    cl->reqregion.y=0;
//    cl->reqregion.w=0;
//    cl->reqregion.h=0;
//    //关于压缩算法的部分初始化
//    cl->tightQualityLevel = -1;
//    cl->enableCursorShapeUpdates = FALSE;
//    cl->enableLastRectEncoding = FALSE;
//    cl->compStreamInited = FALSE;
//    cl->zlibCompressLevel = 5;
////设置cl的颜色映射函数，由于颜色映射做的仅仅和screen的颜色bits相关，所以在此设定
////  新建的客户，开始向客户端发送版本号，开始进行握手阶段
//    sprintf(pv,rfbProtocolVersionFormat,rfbProtocolMajorVersion,
//            rfbProtocolMinorVersion);
//    if (WriteExact(cl, pv, sz_rfbProtocolVersionMsg) < 0) {
//        printk("rfbNewClient: write");
//        rfbCloseClient(cl);
//        return NULL;
//    }
//
//    return cl;
//}

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
int rfbCheckFds(rfbScreenInfoPtr rfbScreen,long usec)
{
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    int option=1;
    int sock;
    rfbClientRec* cl=NULL;

    sock = Multiplex_Wait(rfbScreen->allFds, NULL, CN_TIMEOUT_FOREVER);

    //其实listensock就是传说中的TCPsock，是默认的
    if (sock == rfbScreen->listenSock)
    {
        if ((sock = accept(rfbScreen->listenSock,(struct sockaddr *)&addr, &addrlen))!= INVALID_SOCKET)
        {
            printk("rfbCheckFds: accept");
            if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,(char *)&option, sizeof(option)) != 0)
            {
                printk("#error：rfbCheckFds: setsockopt failed");
            }
            else
            {
                Socket_MultiplexAdd(rfbScreen->allFds,sock,CN_SOCKET_IOREAD);
                cl = rfbNewClient(rfbScreen,sock);
                if(NULL == cl)
                {
                	Socket_MultiplexDel(rfbScreen->allFds,sock);
                    closesocket(sock);
                }
                else
                {
                	socket_setprivate(sock,(ptu32_t)cl);
                }
            }

        }
    }
    else
    {
    	//chk the socket corresponding to the client
    	cl = (rfbClientRec*)socket_getprivate(sock);
        if(NULL != cl)//该client为脏，要删除的对象
        {
            rfbProcessClientMessage(cl);//处理client和server的正常通信状态
        }
    }
    return 0;
}


/*
 * rfbInitSockets sets up the TCP and UDP sockets to listen for RFB
 * connections.  It does nothing if called again.
 */

void
rfbInitSockets(rfbScreenInfoPtr rfbScreen)
{

}

void rfbShutdownSockets(rfbScreenInfoPtr rfbScreen)
{

}

/*
 * rfbCheckFds is called from ProcessInputEvents to check for input on the RFB
 * socket(s).  If there is input to process, the appropriate function in the
 * RFB server code will be called (rfbNewClientConnection,
 * rfbProcessClientMessage, etc).
 */



rfbBool
rfbProcessNewConnection(rfbScreenInfoPtr rfbScreen)
{
    return TRUE;
}


void
rfbDisconnectUDPSock(rfbScreenInfoPtr rfbScreen)
{
  rfbScreen->udpSockConnected = FALSE;
}


/*
 * rfbConnect is called to make a connection out to a given TCP address.
 */

int
rfbConnect(rfbScreenInfoPtr rfbScreen,
           char *host,
           int port)
{
    return 0;
}

/*
 * ReadExact reads an exact number of bytes from a client.  Returns 1 if
 * those bytes have been read, 0 if the other end has closed, or -1 if an error
 * occurred (errno is set to ETIMEDOUT if it timed out).
 */

int
rfbReadExactTimeout(rfbClientPtr cl, char* buf, int len, int timeout)
{

    int sock = cl->sock;
    int n;
    while (len > 0)
    {
        n = recv(sock, buf, len,0);

        if (n > 0) {

            buf += n;
            len -= n;
        }
    }
    return 1;
}

int rfbReadExact(rfbClientPtr cl,char* buf,int len)
{
  /* favor the per-screen value if set */
    int sock = cl->sock;
    int n;
    while (len > 0)
    {
        n = recv(sock, buf, len,0);

        if (n > 0) {

            buf += n;
            len -= n;
        }
    }
    return 1;
}

/*
 * PeekExact peeks at an exact number of bytes from a client.  Returns 1 if
 * those bytes have been read, 0 if the other end has closed, or -1 if an
 * error occurred (errno is set to ETIMEDOUT if it timed out).
 */

int
rfbPeekExactTimeout(rfbClientPtr cl, char* buf, int len, int timeout)
{
    int sock = cl->sock;
    int n;
    while (len > 0)
    {
        n = recv(sock, buf, len,0);

        if (n > 0) {

            buf += n;
            len -= n;
        }
    }
    return 1;
}

/*
 * WriteExact writes an exact number of bytes to a client.  Returns 1 if
 * those bytes have been written, or -1 if an error occurred (errno is set to
 * ETIMEDOUT if it timed out).
 */

int
rfbWriteExact(rfbClientPtr cl,
              const char *buf,
              int len)
{
    int n;
    while (len > 0)
    {
        n = send(cl->sock, buf, len,0);
        if (n > 0)
        {
            buf += n;
            len -= n;
        }
    }
    return 1;
}

/* currently private, called by rfbProcessArguments() */
int
rfbStringToAddr(char *str, in_addr_t *addr)
{
	return 0;
}




int
rfbListenOnTCP6Port(int port,
                    const char* iface)
{
    rfbLogPerror("This LibVNCServer does not have IPv6 support");
    return -1;
}


int
rfbConnectToTcpAddr(char *host,
                    int port)
{
	return -1;
}

int
rfbListenOnUDPPort(int port,
                   in_addr_t iface)
{
	return -1;
}

/*
 * rfbSetNonBlocking sets a socket into non-blocking mode.
 */
rfbBool rfbSetNonBlocking(int sock)
{
  return TRUE;
}

#include "endian.h"

const char rfbEndianTest = (CN_CFG_BYTE_ORDER == CN_CFG_BIG_ENDIAN)?0:1;




