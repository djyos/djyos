// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：vncserverapi.h
// 创建人员: Administrator
// 创建时间: 2013-3-5
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef VNCSERVERAPI_H_
#define VNCSERVERAPI_H_

#include "rfb.h"
//为screen分配空间并进行基本的初始化
void rfbDefaultScreenInit(rfbScreenInfoPtr rfbScreen);

void   ProcessMediaInterfaceThread();

void   ProcessFrameUpdateThread();

#endif /* VNCSERVERAPI_H_ */
