// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名  : rfbproto_processor.h
// 创建人员: Administrator
// 创建时间: 2013-3-6
//           该文件主要是处理RFB协议的消息处理（主要是建立连接和cl和server的正常通信）
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-6>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef RFBPROTO_PROCESSOR_H_
#define RFBPROTO_PROCESSOR_H_

#include "rfb.h"

void rfbProcessClientMessage( rfbClientPtr cl);

#endif /* RFBSERVER_H_ */
