// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名  ：vnc_thread.h
// 创建人员: Administrator
// 创建时间: 2013-3-8
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-8>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef __VNC_THREAD_H__
#define __VNC_THREAD_H__
#include "os.h"

typedef struct MutexLCB  tagMutexCB;
bool_t  TaskCreate(ptu32_t (*task)(void),void *arg,const char *name);
tagMutexCB *MutexCreate(const char *name);
bool_t MutexDelete(tagMutexCB *mutex);
bool_t MutexWait(tagMutexCB *mutex);
void MutexPost(tagMutexCB *mutex);

#define delay(x)       Djy_EventDelay(x*1000)  //都统一到毫秒延时

#endif /* __VNC_THREAD_H__ */
