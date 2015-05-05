// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名  : vnc_auth.h
// 创建人员: Administrator
// 创建时间: 2013-3-5
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef _VNC_AUTH_H_
#define _VNC_AUTH_H_

#include "rfb.h"

//发送认证信息
void rfbAuthNewClient(rfbClientPtr cl);

void rfbVncAuthSendChallenge(rfbClientPtr cl);

void rfbClientSendString(rfbClientPtr cl, const char *reason);

#endif /* AUTH_H_ */
