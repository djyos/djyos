// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：vncserver.h
// 创建人员: Administrator
// 创建时间: 2013-6-9
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef VNCSERVER_H_
#define VNCSERVER_H_

#include "rfb.h"
//需要注意的是，VNC必须在网络和GUI初始化完成后才可以调用
//这个就是VNC的server部分，要使用server，只需要调用此函数即可
int vncserver_start();

void vncserver_exit(rfbScreenInfoPtr rfbserver);

#endif /* VNCSERVER_H_ */
