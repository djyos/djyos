// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：random.h
// 创建人员: Administrator
// 创建时间: 2013-3-7
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-7>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef RANDOM_H_
#define RANDOM_H_

#include "rfb.h"

void RandomData(rfbClientPtr cl);//为cl的挑战认证生成随机数
void rfbEncryptBytes(unsigned char *bytes, char *passwd);//数据加密

#endif /* RANDOM_ */
