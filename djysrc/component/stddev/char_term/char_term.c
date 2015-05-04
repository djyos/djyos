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
//所属模块: 调度器
//作者：lst
//版本：V1.0.1
//文件描述: 调度器中与CPU直接相关的代码。
//其他说明:
//修订历史:
//2. 日期: 2009-04-24
//   作者: lst
//   新版本号: V1.0.1
//   修改说明: 删除了一些为dlsp版本准备的东西
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "version.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "multiplex.h"
#include "driver.h"
#include "shell.h"
#include "stddev.h"
#include "systime.h"
#include "djyos.h"
#include "char_term.h"

static s32 s_s32CharTermId;
static u32 s_u32Stdinout = 0;
struct tagSemaphoreLCB *g_ptSempCharTermHdl;
tpInputMsgQ tg_pCharInputMsgQ;    //  标准输入设备的消息队列

ptu32_t Term_Scan(void)
{
   struct tagCharTermineralMsg msg={0,0};
   struct tagMultiplexSetsCB *CharTermSets;
   tagDevHandle StdinoutHdl;
   u32 ReadLevel = 16;

   Driver_MultiplexCtrl(s_u32Stdinout,&ReadLevel,NULL);
   CharTermSets  = Multiplex_Creat(1);
   Driver_MultiplexAdd(CharTermSets,&s_u32Stdinout,1,CN_MULTIPLEX_SENSINGBIT_READ);
   StdinoutHdl = Driver_OpenDeviceAlias(s_u32Stdinout,O_RDONLY,0);

   while(1)
   {
       Multiplex_Wait(CharTermSets,NULL,100*mS);
       if(NULL != StdinoutHdl)
       {
           //CN_CHAR_BUF_LIMIT
           msg.num = Driver_ReadDevice(StdinoutHdl,msg.input_char,
                                       CN_CHAR_BUF_LIMIT,0,0);
//           Driver_CloseDevice(StdinoutHdl);
           if(msg.num != 0)
           {
               msg.time = DjyGetTimeTick();
               Stddev_InputMsg(s_s32CharTermId,(u8*)&msg,sizeof(msg));
           }
       }
   }
    return 0;
}

//----终端模块初始化函数-------------------------------------------------------
//功能: 初始化调试模块
//参数: para,输入输出使用的设备句柄，如串口
//返回: 终端设备的id(注:这不是泛设备),出错则返回-1
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_CharTerm(ptu32_t para)
{
    u16 term_scan_evtt;
    if(para == 0)
        return (ptu32_t)-1;
    s_s32CharTermId = Stddev_InstallDevice("char_term", EN_CHAR_TERMINERAL,NULL);
    if(s_s32CharTermId == -1)
        goto exit_install_device;
    g_ptSempCharTermHdl = Lock_SempCreate(1, 1, CN_SEMP_BLOCK_FIFO,"char term semp");
    s_u32Stdinout = (u32)para;
    if (g_ptSempCharTermHdl == NULL)
        goto exit_create_semp;
    tg_pCharInputMsgQ = Stddev_CreatInputMsgQ(10,"StdInDev");
    if(tg_pCharInputMsgQ == NULL)
        goto exit_create_msgq;
    term_scan_evtt = Djy_EvttRegist(EN_CORRELATIVE,          //关联型事件
                                CN_PRIO_REAL,       //默认优先级
                                0,                  //线程保留数，关联型无效
                                0,                  //线程上限，关联型无效
//                                1,                  //参数队列长度上限
                                Term_Scan,          //入口函数
                                NULL,               //由系统分配栈
                                1024,               //栈尺寸
                                "Term_Scan"         //事件类型名
                                );
    if(term_scan_evtt == CN_EVTT_ID_INVALID)
    {
        goto exit_install_evtt;
    }
    if(Djy_EventPop(term_scan_evtt,NULL,0,para,0,0)
                        == (uint16_t)CN_EVENT_ID_INVALID)
    {
        goto exit_pop_event;
    }
    Stddev_SetFocus("char_term", tg_pCharInputMsgQ);

    printf("\r\n欢迎使用都江堰操作系统\r\n");
    printf("%s\r\n",djyos_kernel_version);
    printf("编译时间：%s %s\r\n", __DATE__, __TIME__);
    if ((fng_pPrintWorkPath != NULL))
    {
        fng_pPrintWorkPath( );
    }
    printf(">");

    return (ptu32_t)s_s32CharTermId;

exit_pop_event:
    Djy_EvttUnregist(term_scan_evtt);
exit_install_evtt:
    Stddev_DeleteInputMsgQ(tg_pCharInputMsgQ);
exit_create_msgq:
    Lock_SempDelete(g_ptSempCharTermHdl);
exit_create_semp:
    Stddev_UnInstallDevice("char_term");
exit_install_device:
    return (ptu32_t)-1;
}

//static u32 u32s_ptimes;
s32 Djy_GetChar(void)
{
    static struct tagInputDeviceMsg input_msg;
    struct tagCharTermineralMsg *char_msg;
    static u32 offset=0;
    s32 ch;

    if(offset == 0)
    {
        Stddev_ReadMsg(tg_pCharInputMsgQ,&input_msg,CN_TIMEOUT_FOREVER);
    }
    char_msg = &(input_msg.input_data.char_termineral);
    ch = char_msg->input_char[offset++];
    if(offset >= char_msg->num)
    {
        offset = 0;
    }
    return ch;
}

s32 Djy_Gets(char buf[])
{
//    char ch;
//
//    buf[0] = '\0';
//    do
//    {
//        ch = (char)Djy_GetChar();
//        if ((ch != '\r') && (ch != '\n'))
//        {
//            buf++ = ch;
//        }
//        else
//        {
//            buf++ = '\0';
//            return 1;
//        }
//    } while (1);
//
    return 0;
}

//----格式输入---------------------------------------------------------
//功能: 按格式输入字符串，与C库函数类似，但功能弱化了许多。
//参数: const char *fmt, ...
//返回:
//注意：当前仅用于获取字符串，其它功能全未实现！
//-----------------------------------------------------------------------------
s32 EasyScanf(const char *format, char *buf)
{
    char ch;
    u8 chars = 0;

    Lock_SempPend(g_ptSempCharTermHdl, CN_TIMEOUT_FOREVER);
//  u32s_ptimes = Djy_GetEvttPopTimes(Djy_MyEvttId());
//  Stddev_SetFocus("char_term",Djy_MyEvttId());
    while (1)
    {
        ch = (char)Djy_GetChar();
        if(ch == 0x1B ) // 碰到'ESC'，则直接结束
        {
            continue;
        }
        if ((ch == '\r') || (ch == '\n'))
        {
            buf[chars] = '\0';
            printf("\r\n");
            break;
        }
        else
        {
            if (ch == 8)    // Backspace
            {
                if(chars > 0)
                {
                    chars --;
                    printf("\b \b");
                }
            }
            else
            {
                if(chars < 255)  //命令串最长是32字节，超长的被忽略
                {
                    Djy_PutChar(ch);
                    buf[chars] = ch;
                    chars++;
                }
            }

        }
    }
    Lock_SempPost(g_ptSempCharTermHdl);
    return chars;
}

//-----------------------------------------------------------------------------
//功能: 输出一个字符到终端设备上
//参数: 字符c
//返回:
//注意?注意?该输出要经过终端服务线程?不是立即输出的
//-----------------------------------------------------------------------------
void Djy_PutChar(char c)
{
    tagDevHandle StdinoutHdl;
    StdinoutHdl = Driver_OpenDeviceAlias(s_u32Stdinout,O_WRONLY,1000*mS);
    if(NULL != StdinoutHdl)
    {
        Driver_WriteDevice(StdinoutHdl,(u8*)&c,1,0,
                       CN_BLOCK_BUFFER,CN_TIMEOUT_FOREVER);
        Driver_CloseDevice(StdinoutHdl);
    }
}

//-----------------------------------------------------------------------------
//功能: 输出字符串到终端设备上
//参数: 字符串
//返回:
//注意：注意，该输出仅仅是发给终端服务线程，不是立即输出，如果需要立即观察结果，
//      请勿使用，以免上当受骗
//-----------------------------------------------------------------------------
void Djy_Puts(char *v_string)
{
    tagDevHandle StdinoutHdl;
    StdinoutHdl = Driver_OpenDeviceAlias(s_u32Stdinout,O_WRONLY,1000*mS);
    if(NULL != StdinoutHdl)
    {
        Driver_WriteDevice(StdinoutHdl,(u8*)v_string,\
                        strlen(v_string),0,CN_BLOCK_BUFFER,CN_TIMEOUT_FOREVER);
        Driver_CloseDevice(StdinoutHdl);
    }
}

