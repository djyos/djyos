// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名称：vnckeyboard.h
// 模块描述: VNCSERVER
// 模块版本: V1.00
// 创建时间: 2013-6-24
// 原始作者: Administrator
// 功能说明: 该文件用以提供操作模拟键盘的接口
// =============================================================================
#ifndef _VNCKEYBOARD_H_
#define _VNCKEYBOARD_H_

#include "rfb.h"
#include "rfbproto.h"


#define cn_key_down  1
#define cn_key_up    0
#define cn_vnc_key_num  9
#define cn_invalid_key  0xFF
#define cn_virtual_keyboard_id   0xFFEEFFEE
#define cn_vnc_func_key  0xE3//暂时定为CTRL键，后续根据实际情况修改--TODO
//单位像素
#define cn_vnc_keybotton_h_margin  0x08
#define cn_vnc_keybotton_v_margin  0x08
#define cn_vnc_key_width   40
#define cn_vnc_key_height  40
#define cn_max_multi_key_num  0x03

//拷贝keymap
bool_t vnc_keyboard_visual_init(rfbScreenInfoPtr server);

//安装驱动
bool_t install_vncserver_keyboard(rfbScreenInfoPtr server);

bool_t install_vncserver_touchlcd(rfbScreenInfoPtr server);

//相当于key和touch的harddevice
void  vncclient_keyevent(rfbScreenInfoPtr server, rfbClientToServerMsg msg);

void  vncclient_pointevent(rfbScreenInfoPtr server, rfbClientToServerMsg msg);

bool_t vnc_check_key_changed(rfbScreenInfoPtr server, u8 check_bit);

u8 vnc_get_keychanged_numbers(rfbScreenInfoPtr server);
//将key的显存依据特定的状态进行填充（或者颜色更改）--TODO
void vnc_changed_keyboard_visual(rfbScreenInfoPtr server);

void vnc_get_keyposition_by_number(rfbScreenInfoPtr server, u8 key_number,\
		u16 *x, u16 *y);

void vnc_reset_keyboard_statelist(rfbScreenInfoPtr server);


#endif /* VNCKEYBOARD_H_ */
