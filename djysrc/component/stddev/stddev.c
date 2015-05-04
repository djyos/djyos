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
#include "config-prj.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "rsc.h"
#include "pool.h"
#include "stddev.h"
#include "msgqueue.h"
#include "djyos.h"

//所有未设置焦点的输入设备，默认以此为焦点
tpInputMsgQ tg_pDefaultFocusMsgQ = NULL;
struct tagStdinDeviceRsc *g_ptStdinDevice;
struct tagMemCellPool *g_ptStdinDevicePool;
s32 g_s32NextId = 0;       //每次安装输入设备时，以本变量为设备ID，然后本变量++
                            //删除设备，ID不收回。
static struct  tagStdinDeviceRsc *s_ptStdinDeviceRscTree;
tpInputMsgQ tg_pStdinInputMsgQ;    //  标准输入设备的消息队列

//----初始化标准输入模块-------------------------------------------------------
//功能: 初始化标准输入模块，执行初始化后，可以创建输入设备。
//参数: 无
//返回: true = 成功,false= 失败.
//-----------------------------------------------------------------------------
bool_t ModuleInstall_Stddev(ptu32_t para)
{
    static struct  tagStdinDeviceRsc root;

    tg_pStdinInputMsgQ = Stddev_CreatInputMsgQ(10,"StdInDev");
    if(tg_pStdinInputMsgQ == NULL)
        return false;
    g_ptStdinDevice = M_Malloc(gc_u32CfgStdinDeviceLimit
                                * sizeof(struct  tagStdinDeviceRsc),0);
    if(g_ptStdinDevice == NULL)
    {
        Stddev_DeleteInputMsgQ(tg_pStdinInputMsgQ);
        return false;
    }
    s_ptStdinDeviceRscTree = (struct  tagStdinDeviceRsc *)
                        Rsc_AddTree(&root.stdin_device_node,
                        sizeof(struct  tagStdinDeviceRsc),RSC_STDIN_OUT,"stdin input device");
    //初始化泛设备控制块内存池
    g_ptStdinDevicePool = Mb_CreatePool((void*)g_ptStdinDevice,
                                    gc_u32CfgStdinDeviceLimit,
                                    sizeof(struct  tagStdinDeviceRsc),
                                    2,10,
                                    "输入设备控制块内存池");
    Stddev_SetFocusDefault(tg_pStdinInputMsgQ);
    return true;
}

//----安装输入设备-------------------------------------------------------------
//功能: 把输入设备登记到资源结点中
//参数: device_name，目标设备的资源名
//返回: 输入设备ID,-1表示失败
//-----------------------------------------------------------------------------
s32 Stddev_InstallDevice(char *device_name,enum _STDIN_INPUT_TYPE_ stdin_type,
                         void *myprivate)
{
    struct tagStdinDeviceRsc *djy_stdin;

    if(Rsc_SearchSon(&s_ptStdinDeviceRscTree->stdin_device_node,device_name))
    {
        return -1;
    }
    else
    {
        djy_stdin = Mb_Malloc(g_ptStdinDevicePool,0);
        if(djy_stdin != NULL)
        {
            Rsc_AddSon(&s_ptStdinDeviceRscTree->stdin_device_node,
                    &djy_stdin->stdin_device_node,sizeof(struct tagStdinDeviceRsc),
                    RSC_STDIN_OUT,device_name);
            djy_stdin->input_type = stdin_type;
            djy_stdin->stdin_private = (ptu32_t)myprivate;
            djy_stdin->input_counter = 0;
            djy_stdin->FocusMsgQ = tg_pDefaultFocusMsgQ;
            g_s32NextId += 1;
            djy_stdin->device_id = g_s32NextId;
            return djy_stdin->device_id;
        }
        else
        {
            Djy_SaveLastError(EN_MEM_TRIED);
            printf("内存不足\n\r");
            return -1;
        }

    }
}

//----创建输入消息队列--------------------------------------------------------
//功能: 创建输入消息队列, 应用程序从队列中接收输入事件消息.
//参数: MsgNum,消息队列中可容纳的输入消息数量
//      Name, 输入消息队列的名字
//返回: 新创建的输入消息队列指针
//----------------------------------------------------------------------------
tpInputMsgQ Stddev_CreatInputMsgQ(u32 MsgNum,const char *Name)
{
    return MsgQ_Create(MsgNum,sizeof(struct tagInputDeviceMsg),CN_MSGQ_TYPE_FIFO);
}


//----销毁输入消息队列--------------------------------------------------------
//功能: 销毁输入消息队列, 应用程序不再需要输入消息时,删除输入队列.
//      删除操作也将清空队列中已有的消息.
//参数: MsgNum,消息队列中可容纳的输入消息数量
//      Name, 输入消息队列的名字
//返回: 新创建的输入消息队列指针
//----------------------------------------------------------------------------
bool_t Stddev_DeleteInputMsgQ(tpInputMsgQ InputMsgQ)
{
    return MsgQ_Delete(InputMsgQ);
}

//----设置输入焦点------------------------------------------------------------
//功能: 设置输入设备的输入焦点，即把输入消息(例如键盘击键、鼠标点击)传给谁
//参数: device_name，目标设备的资源名
//      focus_evtt，新的焦点
//返回: true =成功，false=输入设备找不到
//----------------------------------------------------------------------------
bool_t Stddev_SetFocus(char *device_name, tpInputMsgQ FocusMsgQ)
{
    struct tagStdinDeviceRsc *result;

    result =
        (struct tagStdinDeviceRsc *)Rsc_SearchSon
        (&s_ptStdinDeviceRscTree->stdin_device_node,device_name);
    if(result != NULL)
    {
        result->FocusMsgQ = FocusMsgQ;
        return true;
    }
    else
    {
        return false;
    }
}

//----设置默认输入焦点---------------------------------------------------------
//功能: 所有输入设备，都有焦点，即设备有数据输入时，给哪个事件类型发消息。未设定
//      焦点的设备，使用默认焦点，如果默认焦点是CN_INVALID_EVTT_ID，则未设置焦点
//      的设备便没有焦点
//参数: focus_evtt，默认焦点，
//返回: 无
//-----------------------------------------------------------------------------
void Stddev_SetFocusDefault(tpInputMsgQ FocusMsgQ)
{
    tg_pDefaultFocusMsgQ = FocusMsgQ;
}

tpInputMsgQ Stddev_GetFocusDefault(void)
{
   return tg_pDefaultFocusMsgQ;
}

//----设置所有设备输入焦点-----------------------------------------------------
//功能: 把所有输入设备的焦点设为指定值
//参数: focus_evtt，新的焦点焦点
//返回: 无
//-----------------------------------------------------------------------------
//void Stddev_SetFocusAll(u16 focus_evtt)
//{
//    //在这里遍历所有输入设备，即"input device"资源的子孙资源，设置他们的focus_evtt
//    struct  tagRscNode  *current,*start,*target;
//    start = &(s_ptStdinDeviceRscTree->stdin_device_node);
//    current = start;
//    while((target =
//        Rsc_TraveScion(start,current))!=NULL)
//    {
//        ((struct tagStdinDeviceRsc *)target)->focus_evtt = focus_evtt;
//        current = target;
//    }
//}

//----输入消息-----------------------------------------------------------------
//功能: 输入设备获得新的输入后(例如新的键盘击键、鼠标点击等)，调用本函数，把新的
//      消息传入系统。弹出事件，如果in_device->focus_evtt已初始化，则事件类型是
//      in_device->focus_evtt，如果未初始化，则事件类型是u16g_default_focus，
//      如果u16g_default_focus也没有初始化，则不弹出事件
//参数: in_device，目标输入设备的资源接点
//      msg_data，包含输入的数据
//      msg_size，输入消息的长度，字节数
//返回: true=成功，false=失败，一般是因为输入设备未安装。
//-----------------------------------------------------------------------------
bool_t Stddev_InputMsg(s32 stdin_id,u8 *msg_data, u32 msg_size)
{
    struct  tagRscNode  *current,*start,*target;
    struct tagInputDeviceMsg input_msg;
    tpInputMsgQ InputMsgQ;

    msg_size = msg_size;        //消除编译器告警

    start = &s_ptStdinDeviceRscTree->stdin_device_node;
    current = &s_ptStdinDeviceRscTree->stdin_device_node;

    //在资源队列中查找stdin_id对应的输入设备
    while((target = Rsc_TraveScion(start,current))!=NULL)
    {
        if(((struct tagStdinDeviceRsc *)target)->device_id == stdin_id)
            break;
        current = target;
    }
    if(target == NULL)        //未找到输入设备
        return false;

    InputMsgQ = ((struct tagStdinDeviceRsc *)target)->FocusMsgQ;
    ((struct tagStdinDeviceRsc *)target)->input_counter++;
    if(InputMsgQ == NULL)      //该设备的输入焦点未初始化
    {
        InputMsgQ = tg_pDefaultFocusMsgQ;      //取默认焦点(可能也未初始化)
    }

    if(InputMsgQ != NULL)
    {
        input_msg.input_data = *(union un_input_data *)msg_data;
        input_msg.input_type = ((struct tagStdinDeviceRsc *)target)->input_type;
        input_msg.device_id = ((struct tagStdinDeviceRsc *)target)->device_id;
        MsgQ_Send(InputMsgQ,(u8*)&input_msg,sizeof(struct tagInputDeviceMsg),
                    CN_TIMEOUT_FOREVER,CN_MSGQ_PRIO_NORMAL);
    }
    return true;
}

//----读取消息-----------------------------------------------------------------
//功能: 从输入消息队列中读取输入消息
//参数: InputMsgQ, 接收消息的消息队列
//      MsgBuf, 保存接收到的消息的缓冲区
//      TimeOut, 如果消息队列空, 等候消息的时限
//返回: true=成功，false=失败，一般是因为输入设备未安装。
//-----------------------------------------------------------------------------
bool_t Stddev_ReadMsg(tpInputMsgQ InputMsgQ,
                     struct tagInputDeviceMsg *MsgBuf,u32 TimeOut)
{
    return MsgQ_Receive(InputMsgQ,(u8*)MsgBuf,sizeof(struct tagInputDeviceMsg),TimeOut);
}

//----从默认队列读取消息-------------------------------------------------------
//功能: 从默认输入消息队列中读取输入消息
//参数: MsgBuf, 保存接收到的消息的缓冲区
//      TimeOut, 如果消息队列空, 等候消息的时限
//返回: true=成功，false=失败，一般是因为输入设备未安装。
//-----------------------------------------------------------------------------
bool_t Stddev_ReadDefaultMsg(struct tagInputDeviceMsg *MsgBuf,u32 TimeOut)
{
    return MsgQ_Receive(tg_pStdinInputMsgQ,(u8*)MsgBuf,sizeof(struct tagInputDeviceMsg),TimeOut);
}

//----卸载输入设备-------------------------------------------------------------
//功能: 把输入设备从资源接点删除，从"input device"资源的子资源中，找到
//      "device_name"资源接点，删除之，并把内存释放到pg_stdin_device_pool
//      内存池中
//参数: device_name，目标设备的资源名
//返回: true = 成功删除，0=失败。
//-----------------------------------------------------------------------------
bool_t Stddev_UnInstallDevice(char *device_name)
{
    struct  tagRscNode *temp;
    struct tagStdinDeviceRsc *djy_stdin;
    djy_stdin = (struct tagStdinDeviceRsc *)Rsc_SearchSon(
                 &s_ptStdinDeviceRscTree->stdin_device_node,device_name);
    if(djy_stdin == NULL)
        return false;

    temp = &djy_stdin->stdin_device_node;
    if(Rsc_DelNode(temp) != NULL)
    {
        Mb_Free(g_ptStdinDevicePool,djy_stdin);
        return true;
    }
    else
        return false;
}

