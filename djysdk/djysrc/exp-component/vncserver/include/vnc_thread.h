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

int  create_thread(void *(*start_routine)(void *),void *arg);


void  XMutexInit(struct tagMutexLCB *mutex, char  *name);
int  XMutexDelete(struct tagMutexLCB *mutex);
void  XMutexWait(struct tagMutexLCB *mutex);
void  XMutexPost(struct tagMutexLCB *mutex);

#endif /* __VNC_THREAD_H__ */
