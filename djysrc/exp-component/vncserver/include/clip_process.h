// =======================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述: VNCServer
// 模块版本: V1.00(初始化版本)
// 文件名     ：clip_process.h
// 创建人员: Administrator
// 创建时间: 2013-6-9
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <V1.00> <2013-3-5>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef __CLIP_PROCESS_H__
#define __CLIP_PROCESS_H__

#include "stdint.h"

void vnc_set_vncclip_pool(struct tagMemCellPool  *newpool);

struct tagClipRect *get_vncclip_space();

void free_vncclip_space(struct tagClipRect *clip);

void free_vnccliplink_space(struct tagClipRect *clip);


void connect_clip2cliplink(struct tagClipRect **mlink,struct tagClipRect *sub);

struct tagClipRect*  remove_clip_from_cliplink(struct tagClipRect **mlink);

void anastomose_vncclip(struct tagClipRect **head, struct tagClipRect *newclip);

struct tagClipRect * combine_vnccliplink(struct tagClipRect *clipq);

//调试使用函数
bool_t check_clip_queque(struct tagClipRect *cliphead);

u16 count_clip_number(struct tagClipRect *cliphead);

void show_clip_queque(struct tagClipRect *clip);


#endif /* __CLIP_PROCESS_H__ */
