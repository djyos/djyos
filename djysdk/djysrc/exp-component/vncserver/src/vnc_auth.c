// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：auth.c
// 创建人员: Administrator
// 创建时间: 2013-3-6
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-6>, <修改人员>: <修改功能概述>
// =======================================================================

/*
 * auth.c - deal with authentication.
 *
 * This file implements the VNC authentication protocol when setting up an RFB
 * connection.
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

#include "rfb.h"
#include "vnc_socket.h"
#include "rfbproto.h"
#include  "debug.h"
#include  "authencode.h"
#include  "vnc_auth.h"

char *rfbAuthPasswdFile = NULL;


// =========================================================================
// 函数功能：发送失败原因
// 输入参数：待交流的客户端cl，失败原因
// 输出参数：
// 返回值  ：
// 说明    ：函数主要用于3.7及其以上的版本在协议握手时
// =========================================================================
void rfbClientSendString(rfbClientPtr cl, const char *reason)
{
    char *buf;
    int len = strlen(reason);

    debug_printf("rfbClientSendString(\"%s\")\n", reason);

    buf = (char *)M_MallocLc(4 + len, 0);
    ((int *)buf)[0] = Swap32IfLE(len);
    memcpy(buf + 4, reason, len);

    if (WriteExact(cl, buf, 4 + len) < 0)
        debug_printf("rfbClientSendString: write");
    free(buf);

    rfbCloseClient(cl);
}
/*
 * Send the authentication challenge.
 */
// =========================================================================
// 函数功能：发送挑战数据
// 输入参数：待交流的客户端cl
// 输出参数：
// 返回值    ：
// 说明         ：对于不同的版本，都是一样的
// =========================================================================
void rfbVncAuthSendChallenge(rfbClientPtr cl)
{

    int i=0;
    /* 4 byte header is alreay sent. Which is rfbSecTypeVncAuth
       (same as rfbVncAuth). Just send the challenge. */
    RandomData(cl->authChallenge);//现在对于这个算法不处理，关于des3的问题

    for(i=0;i<CHALLENGESIZE;i++)
    {
        debug_printf("%d",cl->authChallenge[i]);
    }
    debug_printf("\n");
    if (WriteExact(cl, (char *)cl->authChallenge, CHALLENGESIZE) < 0) {
        debug_printf("rfbAuthNewClient: write");
        rfbCloseClient(cl);
        return;
    }

    /* Dispatch client input to rfbVncAuthProcessResponse. */
    cl->state = RFB_AUTHENTICATION;
}
/*
 * Tell the client what security type will be used (protocol 3.3).
 */
// =========================================================================
// 函数功能：处理V3.7以下版本的发送security type
// 输入参数：待交流的客户端cl
// 输出参数：
// 返回值    ：
// 说明         ：NoeAuth进入init状态，AUTHVNC进入challenge状态。
// =========================================================================
static void  rfbSendSecurityType(rfbClientPtr cl, u32 securityType)
{
    u32 value32;

    /* Send the value. */
    value32 = Swap32IfLE(securityType);
    if (WriteExact(cl, (char *)&value32, 4) < 0) {
    debug_printf("rfbSendSecurityType: write");
    rfbCloseClient(cl);
    return;
    }

    /* Decide what to do next. */
    switch (securityType) {
    case rfbNoAuth:
    /* Dispatch client input to rfbProcessClientInitMessage. */
    cl->state = RFB_INITIALISATION;
    break;
    case rfbVncAuth:
    /* Begin the standard VNC authentication procedure. */
    rfbVncAuthSendChallenge(cl);
    break;
    default:
    /* Impossible case (hopefully). */
        debug_printf("rfbSendSecurityType: assertion failed");
        rfbCloseClient(cl);
        break;
    }
}
//#define rfbConnFailed 0
//#define rfbNoAuth 1
//#define rfbVncAuth 2
// =========================================================================
// 函数功能：处理V3.7以下版本的发送security type
// 输入参数：待交流的客户端cl
// 输出参数：
// 返回值    ：
// 说明         ：NoeAuth进入init状态，AUTHVNC进入challenge状态。
// =========================================================================
static void  rfbSendSecurityTypeList(rfbClientPtr cl, u32 securityType)
{
    u8 buffer[4];
    int  size=4;

    buffer[0]=3;//一共有3种安全类型，invalid，none，vnc
    buffer[1]=rfbConnFailed;
    buffer[2]=rfbNoAuth;
    buffer[3]=rfbVncAuth;

    /* Send the list. */
    if (WriteExact(cl, (char *)buffer, size) < 0) {
    debug_printf("rfbSendSecurityTypeList: write");
    rfbCloseClient(cl);
    return;
    }

    /*
      * if count is 0, we need to send the reason and closesocket the connection.
      */
    if(size <= 1) {
    /* This means total count is Zero and so reason msg should be sent */
    /* The execution should never reach here */
    char* reason = "No authentication mode is registered!";

    rfbClientSendString(cl, reason);
    return;
    }

    /* Dispatch client input to rfbProcessClientSecurityType. */
    cl->state = RFB_SECURITY_TYPE;
}
/*
 * rfbAuthNewClient is called when we reach the point of authenticating
 * a new client.  If authentication isn't being used then we simply send
 * rfbNoAuth.  Otherwise we send rfbVncAuth plus the challenge.
 */
// =========================================================================
// 函数功能：处理安全类型的
// 输入参数：待交流的客户cl
// 输出参数：
// 返回值  ：
// 说明    ：在和cl协商过rfb的version之后进行的一个流程，不同版本处理方式不一样
//          v3.7 以下：server直接发送security type给cl（server决定），type只是一个
//          v3.7及其以上：server发送type表给cl，由cl选择相应的type
//          不同的type处理方式不一样，NoneAuth直接进入init状态,authvnc等要进入
//           challenge状态
// =========================================================================
void rfbAuthNewClient(rfbClientPtr cl)
{
//  u32 securityType = rfbNoAuth;
    u32 securityType = rfbVncAuth;
    if (cl->protocolMajorVersion==3 && cl->protocolMinorVersion < 7)
    {
    /* Make sure we use only RFB 3.3 compatible security types. */
        rfbSendSecurityType(cl, securityType);
    }
    else
    {
    /* Here it's ok when securityType is set to rfbSecTypeInvalid. */
        rfbSendSecurityTypeList(cl, securityType);
    }
}


