//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------

//所属模块: GDD
//作者:  zhb.
//版本：V1.0.0
//文件描述: 按钮控件实现
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2016-10-14
//   作者:  zhb.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __GDD_TEXTBOX_H__
#define __GDD_TEXTBOX_H__

#if __cplusplus
extern "C" {
#endif
#include    "gdd.h"
#include    "gdd_widget.h"


typedef enum{
	EN_R_W =0,   //可读可写
    EN_R_O ,  //read only.
    EN_W_O ,  //write only.
}EN_TEXTBOX_EDIT_PROPERTY;

//文本框Text内容操作控制码
enum TextCtrlCmd
{
   EN_GET_TEXT=0,
   EN_SET_TEXT,
   EN_DELETE_TEXT,
};


//文本框私有数据结构
typedef struct
{
   u16 ChNum;                 //字符数
   u8 CharNumLimit;          //用户可在文本框中键入或粘贴的最大字符数
   u16 CharWidthSum;        //当前总的字符宽度
   EN_TEXTBOX_EDIT_PROPERTY EditProperty;          //文本框内容是否只可读
   bool_t Visible;           //文本框是否可见
   bool_t IsMultiLines;       //是否支持多行
   u16    MaxLines;           //最大行数
   u8 CursorLoc;          //当前光标所在位置,0~CN_STR_LEN_MAX,当前文本框不为焦点窗口时，则该值默认为0xff
}TextBox;


#define BORDER_FIXEDSINGLE   (1<<0)
#define BORDER_FIXED3D       (1<<1)
// =============================================================================
// 函数功能: TextBox控件创建函数。
// 输入参数: Text:文本框窗口Text;
//           Style:文本框风格，参见gdd.h;
//           x:文本框起始位置x方向坐标(单位：像素);
//           y:文本框起始位置y方向坐标(单位：像素);
//           w:文本框宽度(单位：像素);
//           h:文本框高度(单位：像素);
//           hParent:文本框父窗口句柄;
//           WinId:文本框控件Id;
//           pdata:文本框控件私有数据结构;
//           UserMsgTableLink:文本框控件用户消息列表结构指针。
// 输出参数: 无。
// 返回值  :成功则返回文本框句柄，失败则返回NULL。
// =============================================================================
HWND CreateTextBox(const char *Text,u32 Style,
                    s32 x,s32 y,s32 w,s32 h,
                    HWND hParent,u32 WinId,void *pdata,
                    struct MsgTableLink *UserMsgTableLink);

// =============================================================================
// 函数功能: TextBox控件显示内容控制函数
// 输入参数: hwnd,TextBox控件窗口句柄;
//          ctrlcmd,详情参见enum TextCtrlCmd
//          para1:对于EN_GET_TEXT、EN_SET_TEXT、EN_DELETE_TEXT该参数无效,可直接置0。
//          para2:对于EN_DELETE_TEXT该参数无效,可直接置0,对于EN_GET_TEXT、EN_SET_TEXT
//          输入字符串指针.
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
bool_t TextBox_TextCtrl(HWND hwnd,u8 ctrlcmd,u32 para1,ptu32_t para2);


#if __cplusplus
}
#endif
#endif  /*__GDD_TEXTBOX_H__*/
