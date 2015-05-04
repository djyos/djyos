// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名  ：vnc_socket.h
// 创建人员: Administrator
// 创建时间: 2013-3-5
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef VNC_SOCKET_H_
#define VNC_SOCKET_H_

#include "rfb.h"

//初始化服务器的端口，并且置于listen状态
void rfbInitSockets(rfbScreenInfoPtr rfbScreen);

//对服务器的端口进行检测，看是否有新的连接
void rfbCheckFds(rfbScreenInfoPtr rfbScreen,long usec);

//发送len个字长到客户端，一次发不完多发几次
int WriteExact(rfbClientPtr cl, char *buf, int len);

//从客户端读取n个字长，一次读不完多读几次
int ReadExact(rfbClientPtr cl, char *buf, int len);

//关闭客户端，完成关闭客户端后的一些善后处理
void rfbCloseClient(rfbClientPtr cl);

#endif /* SOCKET_H_ */
