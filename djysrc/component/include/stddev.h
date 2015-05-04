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
//所属模块:用户界面模块
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 鼠标、键盘等输入设备管理
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2011-01-24
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 创建文件
//------------------------------------------------------
#ifndef __STDIN_DEV_H__
#define __STDIN_DEV_H__
#include "rsc.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagMsgQueue * tpInputMsgQ;

enum _STDIN_INPUT_TYPE_
{
    EN_CHAR_TERMINERAL,
    EN_STDIN_KEYBOARD,            //键盘
    EN_STDIN_MOUSE_2D,            //2d鼠标
    EN_STDIN_MOUSE_3D,            //3d鼠标
    EN_STDIN_SINGLE_TOUCH,        //单点触摸屏
    EN_STDIN_MUTI_TOUCH,          //多点触摸屏
    EN_STDIN_AREA_TOUCH,          //区域触摸屏
};

//字符终端设备数据结构
#define CN_CHAR_BUF_LIMIT   16
struct tagCharTermineralMsg
{
    u32 time;               //输入字符的时间,ticks数
    u32 num;
    u8 input_char[CN_CHAR_BUF_LIMIT];         //输入的字符
};

//键盘消息数据结构
struct tagKeyBoardMsg
{
    u32 time;              //按键事件发生时间,ticks数
    u8 key_value[2];       //键值,每个键由2部分组成
};

//2维鼠标数据结构，即目前最常用的鼠标
struct tagMouse2D_Msg
{
    u32 time;               //鼠标事件发生时间,ticks数
    u16 key_no;             //动作的鼠标键号
};

//3维鼠标数据结构
struct tagMouse3D_Msg
{
    u32 time;              //按键事件发生时间,ticks数
};

//单点触摸屏，即最常用的触摸屏
struct tagSingleTouchMsg
{
    u32 time;              //按键事件发生时间,ticks数
    s32 x,y,z;  //x,y表示触摸位置，z>0标志触摸压力，0标志未触摸
};

//多点触摸屏
struct tagMultiTouchMsg
{
    u32 time;              //按键事件发生时间,ticks数
};

//区域触摸屏，表示被触摸的是一个区域。
struct tagAreaTouchMsg
{
    u32 time;              //按键事件发生时间,ticks数
};

union un_input_data
{
    struct tagCharTermineralMsg char_termineral;
    struct tagKeyBoardMsg key_board;
    struct tagMouse2D_Msg  mouse_2d;
    struct tagMouse3D_Msg  mouse_3d;
    struct tagSingleTouchMsg  tagSingleTouchMsg;
    struct tagMultiTouchMsg   muti_touch;
    struct tagAreaTouchMsg    area_touch;
};

struct tagInputDeviceMsg
{
    enum _STDIN_INPUT_TYPE_ input_type;         //输入消息类型，
    s32 device_id;          //输入设备id
    union un_input_data input_data;
};

struct tagStdinDeviceRsc
{
    struct tagRscNode stdin_device_node;
    enum _STDIN_INPUT_TYPE_ input_type;   //输入设备类型
    s32 device_id;                      //输入设备id
    u32 input_counter;                  //本输入设备累计输入了多少消息
    tpInputMsgQ FocusMsgQ;             //接收本输入设备的输入消息队列
    ptu32_t stdin_private;              //输入设备的私有数据结构。
};

bool_t ModuleInstall_Stddev(ptu32_t para);
s32 Stddev_InstallDevice(char *device_name,enum _STDIN_INPUT_TYPE_ stdin_type,
                        void *myprivate);
bool_t Stddev_UnInstallDevice(char *device_name);
tpInputMsgQ Stddev_CreatInputMsgQ(u32 MsgNum,const char *Name);
bool_t Stddev_DeleteInputMsgQ(tpInputMsgQ InputMsgQ);
bool_t Stddev_ReadMsg(tpInputMsgQ InputMsgQ,
                     struct tagInputDeviceMsg *MsgBuf,u32 TimeOut);
bool_t Stddev_ReadDefaultMsg(struct tagInputDeviceMsg *MsgBuf,u32 TimeOut);
bool_t Stddev_SetFocus(char *device_name,tpInputMsgQ FocusMsgQ);
void Stddev_SetFocusDefault(tpInputMsgQ FocusMsgQ);
void Stddev_SetFocusAll(tpInputMsgQ FocusMsgQ);
tpInputMsgQ Stddev_GetFocusDefault(void);
bool_t Stddev_InputMsg(s32 stdin_id,u8 *msg_data, u32 msg_size);

#ifdef __cplusplus
}
#endif

#endif //__STDIN_DEV_H__

