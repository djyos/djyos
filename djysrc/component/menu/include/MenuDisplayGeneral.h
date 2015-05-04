//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, 
//    this list of conditions and the following disclaimer in the documentation 
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
// 
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
// 
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
// 模块描述: DJY菜单模块之显示函数之通用显示方法声明
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef  _MENU_DISPLAY_GENERAL_H
#define  _MENU_DISPLAY_GENREAL_H

#include "MenuDisplay.h"

//基本显示方法,该方法都是模块内部使用，不再做参数检测
//bool_t DrawMenuLayerBox(struct menu_displaypara*  dispara);

bool_t display_menu_horizontal(struct menu_displaypara * dispara);
bool_t display_menu_vertical(struct menu_displaypara * dispara);
bool_t display_menu_3by3(struct menu_displaypara * dispara);

//显示手段
struct menu_displaypara *exe_menuitemfunc(struct menu_displaypara *dispara);
struct menu_displaypara *goto_submenu(struct menu_displaypara * operatingDispara);
struct menu_displaypara *goto_parmenu(struct menu_displaypara *operatingDispara);

void goto_nextmenuitem(struct menu_displaypara  *dispara);

void goto_premenuitem(struct menu_displaypara  *dispara);

void goto_menuitem_inslayers(struct menu_displaypara *pDispara, s8 step);

void  windows_draw_text(struct draw_wintext_para *para,const char *text);


//文本写字函数，包括换行，只需要一个起点即可，不保证所有的都显示，显示的区域不一定是你想要的

char*  draw_text_in_rec(struct draw_wintext_para *para,const char *text);

char*  draw_text_on_win(struct draw_wintext_para *para,const char *text);

void   draw_digital_in_rec(struct draw_wintext_para *para, s32 data,s8 hex,s8 bitNumber,\
	                       s8 selectBit,u32 selectTextColor,u32 selectBackColor);



#endif















































