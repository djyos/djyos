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
// 模块描述: DJY菜单模块之显示模块重要数据结构定义
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================

#ifndef __MENUDISPLAY_H__
#define __MENUDISPLAY_H__

#include "stdint.h"
#include "Menuitem.h"

#define cn_space_margin   4//窗口的余量,四个像素

//关于状态栏的颜色
#define  cn_column_space   cn_space_margin//状态栏宽度
#define  color_back        CN_COLOR_BLACK //状态栏背景色
#define  color_light       CN_COLOR_WHITE //状态栏加亮色

//默认的菜单颜色
#define cn_menu_text_color      CN_COLOR_WHITE //菜单文本颜色
#define cn_menu_color           0x00c0c0c0     //菜单颜色
#define cn_menu_lighted_color   CN_COLOR_BLUE  //菜单加亮色

enum KEYVALUE
{
  KEYL=37,//key left
  KEYR=39,//key  right
  KETU=38,//key  up
  KEYD=40,//key down
  KEYE=13,//key enter
  KEYESC=27,//key esc
};

enum  DIS_CMD
{
   GOTO_SUBMENU=0,//切换到子菜单
   GOTO_PARENT,//切换到父菜单
   GOTO_NEXT,//切换到下一个菜单项
   GOTO_PRE,//切换到上一个菜单项
   GOTO_EXE,//执行菜单钩子函数
   GOTO_SLAYSERS//在同层菜单项之间切换
};

//目前定义的显示模式，如果您创建了新的显示模式，请添加在这里。
enum  DISPLAY_MODE
{
   DIS_PULL=0,
   DIS_9CELL
};
struct display_color
{
 //  s32 win_color;菜单颜色就是窗口颜色
   s32 menu_color;  //菜单颜色
   s32 text_color;   //文本颜色
   s32 menu_lighted_color;//加亮颜色
};
//记录菜单所占用的空间:宽和高
struct menu_item_space
{
   s32 width;
   s32 height;
};
//点坐标（x，y）
struct coordinate_point
{
   s32  x;
   s32  y;
};

struct menu_displaypara
{
  //通用数据成员变量
 //  u8    dismode;
  //记录路径
   struct menu_displaypara  *pre;
   struct menu_displaypara  *next;
  //记录当前菜单项的显示特征
  struct display_color   DisplayColor; //用于保存显示的颜色
  struct menu_item_space  MenuitemSpace;//用于记录菜单条目的空间，第一次计算后就记录，不必每次都计算一次

  //当前菜单层的标记动作
  struct menu_item   *OperatingMenuitem;//当前层菜单正在操作的菜单项
  struct menu_item   *FVisualMenuitem;//当前层菜单显示的第一个菜单项
  struct menu_item   *LVisualMenuitem;//当前层菜单显示的最后一个菜单项
  //窗口
  struct GkWinRsc      *AppWin;   //根据应用程序创建的窗口
  struct GkWinRsc      *CurWin;   //显示本层菜单的窗口

  //创建显示参数
  ptu32_t (*CreateMenuDisparaBypre)(struct menu_displaypara *pre,\
          struct menu_item *menuitem,struct GkWinRsc  *Appwin);
  //显示方法
  ptu32_t (*MyMenuDispalyFunction)(struct menu_displaypara *para);
  //调整可视菜单项
  ptu32_t (*AdjustVisualMenuitem)(struct menu_displaypara  *para);
  // 针对的是具体的外设，比如键盘触摸板等的实现。
  ptu32_t  (*menukeyboard_action)(struct menu_displaypara *operatingDispara, void *para );
  ptu32_t  (*menumousetouch_ction)(struct menu_displaypara *operatingDispara, void *para );

  ptu32_t  (*MyDisplayPeripheralRespond)(struct menu_displaypara *operatingDispara, void *para );
  //外设动作函数，实现的是一般的展开、切换、或者返回
  ptu32_t (*MyPeripheralGeneralAction)(struct menu_displaypara *operatingDispara,void *para);
  //初始化函数
  ptu32_t  (*MyMenuInitFunction)(struct menu_item *FirstMenuitem,struct GkWinRsc *Appwin);

  void  *myprivate;//指向每种风格自己的数据成员，用于每种显示方法特有的一些内容，指向的是一个函数指针结构体。
};

struct menu_display_private
{
  //创建显示参数
  ptu32_t (*CreateMenuDisparaBypre)(struct menu_displaypara *pre,\
          struct menu_item *menuitem,struct GkWinRsc  *Appwin);
  //显示方法
  ptu32_t (*MyMenuDispalyFunction)(struct menu_displaypara *para);
  //调整可视菜单项
  ptu32_t (*AdjustVisualMenuitem)(struct menu_displaypara  *para);
  // 针对的是具体的外设，比如键盘等的实现。
  ptu32_t  (*MyDisplayPeripheralRespond)(struct menu_displaypara *operatingDispara, void *para );
  //外设动作函数，实现的是一般的展开、切换、或者返回
  ptu32_t (*MyPeripheralGeneralAction)(struct menu_displaypara *operatingDispara,void *para);

};
enum draw_text_direction
{
   Left2Right=0,
   Right2Left,
   Top2Bottom,
   Bottom2Top
};


//用于画菜单文本
struct draw_wintext_para
{
    u8  draw_type;                  //字体显示方式:1，左到右，2，右到左 3，上到下
    u8  line_limit;                 //超过此值就会换行
    s32 x, y;                       //显示区域的起始位置，相对于gkwin，表示起始位置
    s32 width,height;               //显示区域的宽度和高度
    s32 count;                      //缓冲区长度，字节数，不含传结束符。
    u32 colortext;                  //字体颜色
    u32 colorback;                  //背景颜色
    struct GkWinRsc* gkwin;        //目标窗口
};

#endif
