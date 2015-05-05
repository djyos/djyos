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
//所属模块: djysh模块
//作者：wjz
//版本：V0.1.0
//文件描述: djyos的shell主模块，提供shell的实现过程以及内置命令
//修订历史:
// 1. 日期:2010-09-10之前
//   作者:lst
//   新版本号：djysh0.0.1
//     修改说明：shell实现的设计。
// 2. 日期:2011-06-15
//   作者:wjz
//   新版本号：djysh0.0.2
//   修改说明: 添加文件系统命令以及注册文件系统shell模块。
// 2. 日期:2011-09-24
//   作者:wjz
//   新版本号：djysh1.0.0
//   修改说明: 添加事件、事件类型、中断、内存、帮助等命令，
//               djysh1.0.0，可以作为一个简单完整的shell了。
//------------------------------------------------------
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "rsc.h"
#include "djyos.h"
#include "stddev.h"
#include "shell.h"
#include "version.h"
#include "time.h"
#include "systime.h"
#include "char_term.h"

bool_t  Sh_ResetCpu();
bool_t  Sh_ResetCpuHot();

extern bool_t __erase_all_nand(char *param);
bool_t Sh_ShowForCycle(char *param);
bool_t Sh_ListRscSon(char *param);
bool_t Sh_ListRscTree(char *param);
void Sh_ListRscAll(void);
void __Sh_ShowSon(struct tagRscNode *branche);
void __Sh_ShowBranche(struct tagRscNode *branche);
bool_t Sh_ShowMemory(char *param);
bool_t Sh_FillMemory(char *param);
bool_t Sh_CmdParser(const char *buf,const char *keyword,char **param);
void Sh_CmdHelp(void);
bool_t Sh_ListCmdHelp(char *param);
bool_t Sh_Ver(char *param);
bool_t Sh_Date(char *param);
bool_t Sh_Time(char *param);
bool_t Sh_ExecCommand(char *cmd);
bool_t Sh_UninstallCmdByName(char *param);

ptu32_t Sh_Service(void);
ptu32_t ModuleInstall_Sh(ptu32_t para);

void (*fng_pPrintWorkPath)(void) = NULL;
bool_t (*fng_pCD_PTT)(const char *PTT_Name) = NULL;

extern bool_t Sh_PrintWorkPath(void);
//内置命令表,包含系统自带的命令
struct tagShellCmdTab const shell_cmd_table[] =
{
    {
        "rsc-tree",
        Sh_ListRscTree,
        "列出系统中的资源名字和层次关系",
        "格式: rsc_tree TreeName,省略TreeName则列出全部资源"
    },
    {
        "rsc-son",
        Sh_ListRscSon,
        "列出子资源",
        "格式: rsc_tree RscName,省略RscName则列出根资源"
    },
    {
        "speed",
        Sh_ShowForCycle,
        "测量for循环运行速度",
        NULL
    },
    {
        "d",
        Sh_ShowMemory,
        "读取内存里的数据",
        "命令格式:d 地址 单元数 每单元字节数"
    },
    {
        "f",
        Sh_FillMemory,
        "写数据到内存",
        NULL
    },
    {
        "help",
        Sh_ListCmdHelp,
        "DJYOS的命令帮助信息",
        NULL
    },
    {
        "ver",
        Sh_Ver,
        "显示djyos的版本",
        NULL
    },
    {
        "date",
        Sh_Date,
        "显示或者设置日期",
        NULL
    },
    {
        "time",
        Sh_Time,
        "显示当前时间或者设置输入新时间",
        NULL
    },

    {
        "uninstall-cmd",
        Sh_UninstallCmdByName,
        "删除shell命令--用名字",
        NULL
    },
};

static struct tagShellCmdRsc tg_shell_cmd_rsc
                        [sizeof(shell_cmd_table)/sizeof(struct tagShellCmdTab)];
static struct tagRscNode cmd_list_node;

//----当前工作路函数指针赋值-----------------------------------------------------
//功能: 函数指针赋值，该指针指向获取当前工作路径的函数
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Sh_PrintWorkPathSet(void)
{
    fng_pPrintWorkPath = (void*)Sh_PrintWorkPath;
}

//----调试模块初始化函数-------------------------------------------------------
//功能: 初始化调试模块
//参数: 无
//返回: 1
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Sh(ptu32_t para)
{
    u16 shell_evtt;
    u32 loop;
    para = para;        //消除编译器告警
    shell_evtt = Djy_EvttRegist(EN_CORRELATIVE,  //关联型事件
                                CN_PRIO_REAL,       //默认优先级
                                0,                  //线程保留数，关联型无效
                                0,                  //线程上限，关联型无效
//                                1,                  //参数队列长度上限
                                Sh_Service,         //入口函数
                                NULL,               //由系统分配栈
                                0x1000,             //栈尺寸
                                "shell"             //事件类型名
                                );
    if(shell_evtt == CN_EVTT_ID_INVALID)
        return false;
    if(Djy_EventPop(shell_evtt,NULL,0,0,0,0)
                        == CN_EVENT_ID_INVALID)
    {
        Djy_EvttUnregist(shell_evtt);
    }
    Rsc_AddTree(&cmd_list_node,sizeof(struct tagRscNode),RSC_RSCNODE,"shell cmd list");
    for(loop = 0;
            loop<sizeof(shell_cmd_table)/sizeof(struct tagShellCmdTab);loop++)
    {
        tg_shell_cmd_rsc[loop].shell_cmd_func
                                    = shell_cmd_table[loop].shell_cmd_func;
        tg_shell_cmd_rsc[loop].help_hint
                                    = shell_cmd_table[loop].help_hint;
        tg_shell_cmd_rsc[loop].help_detailed
                                    = shell_cmd_table[loop].help_detailed;
        Rsc_AddSon(&cmd_list_node,&(tg_shell_cmd_rsc[loop].cmd_node),
                    sizeof(struct tagShellCmdRsc),RSC_SHELL_CMD,shell_cmd_table[loop].cmd);
    }
    return 1;
}

//----添加shell命令------------------------------------------------------------
//功能: 添加用户的shell命令，由于si版本是一个二进制的可执行影像，不含符号表，不
//      能通过shell直接调用函数，必须把新命令添加到命令表中。在dlsp版本中，可以
//      通过直接调用函数的方式实现，但用命令表的方式，仍然有速度快的优势。
//参数: cmd_tab，命令表指针，可参照shell_cmd_table的方式定义
//      cmd_rsc，资源节点指针，可参照tg_shell_cmd_rsc的方式定义
//      cmd_num, 新添加的表中包含的命令数
//返回: true = 成功。
//      false= 失败，可能是参数错误，也可能是命令表中包含重名或与现有命令重名的
//          命令，如果是重名，则不重名的命令仍然正常加入命令表
//-----------------------------------------------------------------------------
bool_t Sh_InstallCmd(struct tagShellCmdTab const *cmd_tab,
                      struct tagShellCmdRsc *cmd_rsc,u32 cmd_num)
{
    u32 loop;
    bool_t result = true;
    if( (cmd_tab == NULL) || (cmd_rsc == NULL) || (cmd_num == 0))
        return result;

    for(loop = 0; loop < cmd_num;loop++)
    {
        if(Rsc_SearchSon(&cmd_list_node,cmd_tab[loop].cmd) == NULL)
        {
            cmd_rsc[loop].shell_cmd_func = cmd_tab[loop].shell_cmd_func;
            cmd_rsc[loop].help_hint = cmd_tab[loop].help_hint;
            cmd_rsc[loop].help_detailed = cmd_tab[loop].help_detailed;
            Rsc_AddSon(&cmd_list_node,&(cmd_rsc[loop].cmd_node),
                        sizeof(struct tagShellCmdRsc),RSC_SHELL_CMD,cmd_tab[loop].cmd);
        }
        else
        {
            result = false;
        }
    }
    return result;
}
//----卸载shell命令------------------------------------------------------------
//功能: 卸载用户shell命令
//参数: cmd_tab，命令表指针，可参照shell_cmd_table的方式定义,cmd_num，表的大小
//返回: 成功卸载的个数
//-----------------------------------------------------------------------------
u32 sh_uninstall_cmd_bytab(struct tagShellCmdTab const *cmd_tab,u32 cmd_num)
{
    u32 loop;
    u32 result = 0;
    struct tagRscNode *tagShellCmdRsc;
    if( (cmd_tab == NULL) || (cmd_num == 0))
        return result;

    for(loop = 0; loop < cmd_num;loop++)
    {
        tagShellCmdRsc = Rsc_SearchSon(&cmd_list_node,cmd_tab[loop].cmd);
        if(NULL != tagShellCmdRsc)
        {
            Rsc_DelNode(tagShellCmdRsc);
            result ++;
        }

    }
    return result;
}
bool_t Sh_UninstallCmdByName(char *param)
{
    bool_t result = 0;
    char *cmdname;
    struct tagRscNode *tagShellCmdRsc;
    char *next_param;
    if(NULL == param)
    {
        return false;
    }
    cmdname = Sh_GetWord(param,&next_param);
    if( cmdname == NULL)
        return result;

    tagShellCmdRsc = Rsc_SearchSon(&cmd_list_node,cmdname);
    if(NULL != tagShellCmdRsc)
    {
        Rsc_DelNode(tagShellCmdRsc);
        result = true;
    }
    else
    {
        result = false;
    }
    return result;
}

//----提取目录---------------------------------------------------------------
//功能: 从buf中提取一个由'\\'或行结束符隔开的单词，next用于返回下一个单词首地址，
//      如果没有下一个单词，则next=NULL。
//参数: buf，待分析的字符串
//      next，返回下一个单词指针
//返回: 提取的单词指针，已将单词后面的分隔符换成串结束符'\0'
//-----------------------------------------------------------------------------
char *Sh_GetItem(char *buf,char **next)
{
    uint32_t i=0;
    *next = NULL;
    while(1)
    {
        if((buf[i] == '\\') || (buf[i] == 0))
        {
            if(buf[i] == 0)
                return buf;
            else
            {
                buf[i] = '\0';
                break;
            }
        }
        i++;
    }
    i++;
    while(buf[i] != 0)
    {
        if((buf[i]!='\\') && (buf[i] != '\n') && (buf[i] != '\r'))
        {
            *next = &buf[i];
            break;
        }
        i++;
    }
    return buf;
}


//----提取单词---------------------------------------------------------------
//功能: 从buf中提取一个由空格或行结束符隔开的单词，next用于返回下一个单词首地址，
//      如果没有下一个单词，则next=NULL。
//参数: buf，待分析的字符串
//      next，返回下一个单词指针
//返回: 提取的单词指针，已将单词后面的分隔符换成串结束符'\0'
//-----------------------------------------------------------------------------
char *Sh_GetWord(char *buf,char **next)
{
    uint32_t i=0;
    *next = NULL;
    if(buf == NULL)
        return NULL;
    while(1)
    {
        if((buf[i] == ' ') || (buf[i] == 0))
        {
            if(buf[i] == 0)
                return buf;
            else
            {
                buf[i] = '\0';
                break;
            }
        }
        i++;
    }
    i++;
    while(buf[i] != 0)
    {
        if(buf[i]!=' ')
        {
            if((buf[i] == '\n') || (buf[i] == '\r'))
                *next = NULL;
            else
                *next = &buf[i];
            break;
        }
        i++;
    }
    return buf;
}

//----显示for循环时间----------------------------------------------------------
//功能: 显示一个for(;;)循环的执行时间，循环变量分别是8位、16位、32位。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
extern u32 g_u32CycleSpeed; //for(i=j;i>0;i--);每循环纳秒数*1.024

bool_t Sh_ShowForCycle(char *para)
{
    para = para;        //消除编译器告警
    printf("空for循环执行时间: %dnS\r\n", g_u32CycleSpeed);

    return true;
}

//----显示子资源----------------------------------------------------------
//功能: 按参数要求显示资源队列中资源的名字，若该资源名字为空，则显示"无名资源"
//参数: 参数字符串，待列出的父资源名，省却则列出根结点下的资源
//返回: true
//-----------------------------------------------------------------------------
bool_t Sh_ListRscSon(char *param)
{
    struct tagRscNode *rsc_tree;
    if(param == NULL)
        rsc_tree = Rsc_GetRoot();
    else
    {
        rsc_tree = Rsc_Search(Rsc_GetRoot(),param);
        if(rsc_tree == NULL)
        {
            printf("没找到 %s 资源\r\n",param);
        }
    }
    __Sh_ShowSon(rsc_tree);
    return true;
}


//----显示资源树----------------------------------------------------------
//功能: 按参数要求显示资源队列中资源的名字，若该资源名字为空，则显示"无名资源"
//参数: 参数字符串，待列出的树名，省却则列出全部资源
//返回: true
//-----------------------------------------------------------------------------
bool_t Sh_ListRscTree(char *param)
{
    struct tagRscNode *rsc_tree;
    if(param == NULL)
        rsc_tree = Rsc_GetRoot();
    else
    {
        rsc_tree = Rsc_Search(Rsc_GetRoot(),param);
        if(rsc_tree == NULL)
        {
            printf("没找到 %s 资源树\r\n",param);
        }
    }
    __Sh_ShowBranche(rsc_tree);
    return true;
}

//----显示子资源---------------------------------------------------------------
//功能: 显示某资源结点起始的一个资源分支，不包含该资源自身
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void __Sh_ShowSon(struct tagRscNode *branche)
{
    struct tagRscNode *current_node = branche;
    while(1)
    {
        current_node = Rsc_TraveSon(branche,current_node);
        if(current_node == NULL)
        {
            printf("\r\n");
            break;
        }
        if(current_node->name != NULL)
        {
            printf("  %s\r\n", current_node->name);
        }else
        {
            printf("  无名资源\r\n");
        }
    }
}

//----显示资源树---------------------------------------------------------------
//功能: 显示某资源结点起始的一个资源分支，不包含该资源自身
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void __Sh_ShowBranche(struct tagRscNode *branche)
{
    struct tagRscNode *current_node = branche;
    ucpu_t len;
    char neg[20];
    for(len = 0; len<20; len++)
        neg[len] = '-';
    while(1)
    {
        current_node = Rsc_TraveScion(branche,current_node);
        if(current_node == NULL)
        {
            printf("\r\n");
            break;
        }
        len = Rsc_GetClass(current_node);
        neg[len] = '\0';
        printf("%s", neg);
        neg[len] = '-';
        if(current_node->name != NULL)
        {
            printf("%s\r\n", current_node->name);
        }else
        {
            printf("无名资源\r\n");
        }
    }
}

//----显示内存-----------------------------------------------------------------
//功能: 显示某地址开始的一段内存，每行显示16个单元，只显示，不能修改
//参数: param，参数串，本命令需要三个参数，用空格隔开
//          参数1:起始地址，10进制或16进制(0x开头)
//          参数2:显示的单元数
//          参数3:每单元字节数，合法值是1、2、4、8，其他数值将返回错误
//返回: true=正常显示，false=错误
//-----------------------------------------------------------------------------
bool_t Sh_ShowMemory(char *param)
{
    ptu32_t addr;
    uint32_t unit_num,unit_bytes,len;
    char *word_addr,*word_un,*word_ub,*word_trail,*next_param;

    //提取3个参数
    word_addr = Sh_GetWord(param,&next_param);
    word_un = Sh_GetWord(next_param,&next_param);
    word_ub = Sh_GetWord(next_param,&next_param);
    word_trail = Sh_GetWord(next_param,&next_param);
    if((word_addr == NULL)||(word_un == NULL)
            ||(word_ub == NULL)||(word_trail != NULL))
    {
        printf("\r\n格式错误，正确格式是：\r\n>d 地址 单元数 每单元字节数\r\n");
        return false;
    }
    addr = strtol(word_addr, (char **)NULL, 0);
    unit_num = strtol(word_un, (char **)NULL, 0);
    unit_bytes = strtol(word_ub, (char **)NULL, 0);
#if (CN_BYTE_BITS == 8)  //字节位宽=8，最常见的情况
    switch(unit_bytes)
    {
        case 1:
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%02x ", *(uint8_t*)addr);
                addr ++;
                if(addr %8 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
            break;
        case 2:
            addr &= ~(ptu32_t)1;//只能从偶数开始
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%04x ", *(u16*)addr);
                addr +=2;
                if(addr %16 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
            break;
        case 4:
            addr &= ~(ptu32_t)3;//只能从偶数开始
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%08x ", *(uint32_t*)addr);
                addr +=4;
                if(addr %16 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
            break;
        case 8:
            addr &= ~(ptu32_t)7;//只能从偶数开始
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%08x", *(uint32_t*)addr);
                addr +=4;
                printf("%08x ", *(uint32_t*)addr);
                addr +=4;
                if(addr %32 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
            break;
        default :
            printf("\r\n参数错误\r\n");
            return false;
            break;
    }
#elif (CN_BYTE_BITS == 16)  //字节位宽=16
    switch(unit_bytes)
    {
        case 1:
        {
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%04x ", *(u16*)addr);
                addr ++;
                if(addr %8 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
        } break;
        case 2:
        {
            addr &= ~(ptu32_t)1;
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%08x ", *(uint32_t*)addr);
                addr +=2;
                if(addr %16 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
        } break;
        case 4:
        {
            addr &= ~(ptu32_t)3;
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%08x ", *(uint32_t*)addr);
                addr +=2;
                printf("%08x ", *(uint32_t*)addr);
                Djy_PutChar(' ');
                addr +=2;
                if(addr %16 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
        } break;
        default :
        {
            printf("\r\n参数错误\r\n");
            return false;
        } break;
    }
#elif (CN_BYTE_BITS == 32)  //字节位宽=32
    switch(unit_bytes)
    {
        case 1:
        {
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%08x ", *(uint32_t*)addr);
                addr ++;
                if(addr %8 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
        } break;
        case 2:
        {
            addr &= ~(ptu32_t)1;
            printf("0x%08x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                printf("%08x ", *(uint32_t*)addr);
                addr +=2;
                printf("%08x ", *(uint32_t*)addr);
                Djy_PutChar(' ');
                addr +=2;
                if(addr %16 == 0)
                {
                    printf("\r\n");
                    if(len != unit_num)
                    {
                        printf("0x%08x:", addr);
                    }
                }
            }
        } break;
        default :
        {
            printf("\r\n参数错误\r\n");
            return false;
        } break;
    }
#endif
    printf("\r\n");
    return true;
}

//----写入内存-----------------------------------------------------------------
//功能: 写入某地址开始的一段内存
//参数: param，参数串，本命令需要三个参数，用空格隔开
//          参数1:起始地址，10进制或16进制(0x开头)
//          参数2:显示的单元数
//          参数3:每单元字节数，合法值是1、2、4，其他数值将返回错误
//          参数4:待填充的内容
//返回: true=正常显示，false=错误
//-----------------------------------------------------------------------------
bool_t Sh_FillMemory(char *param)
{
    ptu32_t addr;
    uint32_t unit_num,unit_bytes,len,data;
    char *word,*next_param;

    word = Sh_GetWord(param,&next_param);
    addr = strtol(word, (char **)NULL, 0);
    word = Sh_GetWord(next_param,&next_param);
    unit_num = strtol(word, (char **)NULL, 0);
    word = Sh_GetWord(next_param,&next_param);
    unit_bytes = strtol(word, (char **)NULL, 0);
    word = Sh_GetWord(next_param,&next_param);
    data = strtol(word, (char **)NULL, 0);
#if (CN_BYTE_BITS == 8)  //字节位宽=8，最常见的情况
    switch(unit_bytes)
    {
        case 1:
            printf("0x%8x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                *(uint8_t*)addr = (u8)data;
                addr ++;
            }
            break;
        case 2:
            addr &= ~(ptu32_t)1;
            printf("0x%8x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                *(u16*)addr = (u16)data;
                addr +=2;
            }
            break;
        case 4:
            addr &= ~(ptu32_t)3;
            printf("0x%8x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                *(u32*)addr = (u32)data;
                addr +=4;
            }
            break;
        default :
            printf("\r\n参数错误\r\n");
            return false;
            break;
    }
#elif (CN_BYTE_BITS == 16)  //字节位宽=16
    switch(unit_bytes)
    {
        case 1:
        {
            printf("0x%8x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                *(u16*)addr = (u16)data;
                addr ++;
            }
        } break;
        case 2:
        {
            addr &= ~(ptu32_t)1;
            printf("0x%8x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                *(u32*)addr = (u32)data;
                addr +=2;
            }
        } break;
        default :
        {
            printf("\r\n参数错误\r\n");
            return false;
        } break;
    }
#elif (CN_BYTE_BITS == 32)  //字节位宽=32
    switch(unit_bytes)
    {
        case 1:
        {
            printf("0x%8x:", addr);
            for(len = 0; len < unit_num; len++)
            {
                *(u32*)addr = (u32)data;
                addr ++;
            }
        } break;
        default :
        {
            printf("\r\n参数错误\r\n");
            return false;
        } break;
    }
#endif
    return true;
}

//----命令串分析---------------------------------------------------------------
//功能: 分析buf包含的字符串第一个单词是否keyword命令。并且在param中返回keyword
//      后第一个非零且非回车换行的字符的指针，该指针实际包含第一个命令行参数的
//      地址。如果没有参数，则param=NULL。
//参数: buf，待分析的字符串
//      keyword，待匹配的命令
//      param，返回命令行参数指针
//返回: true = 命令匹配，false=命令不匹配
//-----------------------------------------------------------------------------
bool_t Sh_CmdParser(const char *buf,const char *keyword,char **param)
{
    uint32_t i=0;
    char buf2[255];
    bool_t result;

    if(buf == NULL)
        return false;
    *param = NULL;
    //获取不带参数的命令的串结束符或者命令后面空格的下标号i
    for(i=0;i<255;i++)
    {
        if((buf[i]==' ')||(buf[i] == '\0'))
            break;
    }
    memcpy(buf2,buf,i);
    buf2[i] = '\0';
    //比较没有带参数的命令
    if(strcmp(buf2,keyword)!=0)
        result = false;
    else
        result = true;

    while(buf[i] != 0)
    {
        if(buf[i]!=' ')
        {
            if((buf[i] == '\n') || (buf[i] == '\r'))
                *param = NULL;
            else
                *param = (char*)&buf[i];
            break;
        }
        i++;
    }
    return result;
}

//----显示帮助-----------------------------------------------------------------
//功能: 显示帮助信息
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void Sh_CmdHelp(void)
{
    struct tagShellCmdRsc *current_cmd;
    printf("\r\n有关具体命令的详细信息，请输入help [命令名]\r\n");
    printf("\r\n");
    current_cmd = (struct tagShellCmdRsc *)&cmd_list_node;
    while(1)
    {
        current_cmd = (struct tagShellCmdRsc *)
                    Rsc_TraveScion(&cmd_list_node,(struct tagRscNode *)current_cmd);
        if(current_cmd == NULL)
        {
            break;
        }
        else
        {
            if(current_cmd->help_hint != NULL)
            {
                //todo: 前福查下，第一个%s应该是%-32s才对，下6行同
                printf("%-24s%s",
                            current_cmd->cmd_node.name,current_cmd->help_hint);
            }
            else
            {
                printf("%-24s没有提供简要帮助信息",current_cmd->cmd_node.name);
            }
        }
        printf("\r\n");
    }
}


//----显示文件系统命令帮助-----------------------------------------------------
//功能:
//参数: 参数字符串，含义:
//返回: true
//-----------------------------------------------------------------------------
bool_t Sh_ListCmdHelp(char *param)
{
    char *cmd,*next_param;
    bool_t result;
    struct tagShellCmdRsc *help_cmd;

    if(param == NULL)
    {
        Sh_CmdHelp();
        result = true;
    }
    else
    {
        cmd = Sh_GetWord(param,&next_param);
        help_cmd = (struct tagShellCmdRsc *)Rsc_SearchSon(&cmd_list_node,cmd);
        if(help_cmd != NULL)
        {
            if(help_cmd->help_detailed != NULL)
                printf("%s",help_cmd->help_detailed);
            else
                printf("没有提供详细帮助信息");
            printf("\r\n");
            result = true;
        }else
        {
            printf("无效命令名\r\n");
            result = false;
        }
    }
    return result;;
}
bool_t Sh_Ver(char *param)
{
    printf("\r\n%s\r\n", djyos_kernel_version);

    return true;
}

bool_t Sh_Date(char *param)
{
    s64 nowtime;
    struct tm dtm;
    char command[256];
    char buf[8];
    int cmdlen;
    int res;

    nowtime = TM_Time(NULL);
    Tm_LocalTime_r(&nowtime,&dtm);

    printf("\r\n当前日期：%d/%02d/%02d %s",
            dtm.tm_year, dtm.tm_mon, dtm.tm_mday, g_cTmWdays[dtm.tm_wday]);
    printf("\r\n输入新日期：");

    cmdlen = EasyScanf(NULL, command);
    if ((8<=cmdlen) && (cmdlen<=10))
    {
        //日期为“2012/02/12”形式，在它背后加入时间
        strcat(command, ",");    // 逗号用于分开日期和时间
        itoa(dtm.tm_hour, buf, 10);
        strcat(command, buf);
        strcat(command, ":");
        itoa(dtm.tm_min, buf, 10);
        strcat(command, buf);
        strcat(command, ":");
        itoa(dtm.tm_sec, buf, 10);
        strcat(command, buf);

        res = Tm_SetDateTimeStr(command);
        switch (res)
        {
        case EN_CLOCK_YEAR_ERROR:
            printf("年份错误。");
            break;
        case EN_CLOCK_MON_ERROR:
            printf("月份错误。");
            break;
        case EN_CLOCK_DAY_ERROR:
            printf("日期错误。");
            break;
        default:
            break;
        }
    }

    printf("\r\n");
    return true;
}

bool_t Sh_Time(char *param)
{
    s64 nowtime;
    struct tm dtm;
    char command[256];
    char time[16];
    char buf[8];
    int cmdlen;
    int res;

    nowtime = TM_Time(NULL);
    Tm_LocalTime_r(&nowtime,&dtm);

    printf("\r\n当前时间：%02d:%02d:%02d", dtm.tm_hour, dtm.tm_min, dtm.tm_sec);
    printf("\r\n输入新时间：");

    cmdlen = EasyScanf(NULL, command);
    if ((5<=cmdlen) && (cmdlen<=8))
    {
        time[0] = '\0';
        strcpy(time, command);    // 将获得的时间字符串复制到buf中
        //时间为“03:32:1”形式，在它前面加日期
        command[0] = '\0';
        itoa(dtm.tm_year, buf, 10);
        strcat(command, buf);
        strcat(command, "/");
        itoa(dtm.tm_mon, buf, 10);
        strcat(command, buf);
        strcat(command, "/");
        itoa(dtm.tm_mday, buf, 10);
        strcat(command, buf);
        strcat(command, ",");    // 逗号用于分开日期和时间
        strcat(command, time);

        res = Tm_SetDateTimeStr(command);
        switch (res)
        {
        case EN_CLOCK_HOUR_ERROR:
            printf("小时错误。");
            break;
        case EN_CLOCK_MIN_ERROR:
            printf("分钟错误。");
            break;
        case EN_CLOCK_SEC_ERROR:
            printf("秒钟错误。");
            break;
        default:
            break;
        }
    }

    printf("\r\n");
    return true;
}

//----转换输入命令的大写字母为小写----------------------------------------------
//功能: 转换shell下输入的命令"buf"中空格前的大写字母为小写。
//参数: cmd，shell下输入的命令命令"buf"(cmd)
//返回: 0
//-----------------------------------------------------------------------------
#if 0
void capital_convert_lowercase(char *cmd)
{

    while(*cmd!='\0')
    {
           if(*cmd>='A'&&*cmd<='Z')
            *cmd=*cmd+('a'-'A');
            cmd++; //下一个字符
         //遇到空格
         if(*cmd==' ')
             break;
    }
    return ;
}
#endif


//----判断':'后面是否有数据-----------------------------------------------------
//功能: 从一个可能包含多级路径名和文件名的字符串中判断第一次遇到':'之后是否还有
//      数据
//参数: path，被测路径，格式如"C:\aaaa\bbbb\cccc"
//返回: true=成功, 说明':'之后有数据,或者没有':'
//      false=失败，说明':'之后没有数据
//备注: path应该是经过合法性检查的字符串，不会出现word超长的问题
//-----------------------------------------------------------------------------
bool_t __judge_colon_is_data(char *path)
{
    uint32_t index;
    uint32_t  name_len;

    name_len = strlen(path);
    for(index = 0; index< name_len; index++)
    {
        if(path[index] == ':')    //遇到字符'\'
        {
            if(path[index+1] != '\0')
                return true;
            else
                return false;
        }
    }
    //字符串里没有冒号
    if(index == name_len)
        return true;
    return false;
}

//----执行控制台命令---------------------------------------------------
//功能: 分析并执行控制台命令
//参数: 无
//返回: 无
//备注: 到时候用哈希表等，进行更加快速的比较。如果这样的比较，命令多了。
//      那还不无奈死了。
//---------------------------------------------------------------------
bool_t Sh_ExecCommand(char *buf)
{
    bool_t result = false;
    char *cmd,*next_param;
    struct tagShellCmdRsc *exec_cmd;
    //串口限制读取255字符，在这里提示超长就行。
    if(strnlen(buf, CN_SHELL_CMD_LIMIT+1) > CN_SHELL_CMD_LIMIT)
    {
        printf("输入字符串太长\r\n");
    }
    //转换buf中的大写字母为小写
//    capital_convert_lowercase(buf);

    if(!__judge_colon_is_data(buf))
    {
        if (fng_pCD_PTT != NULL)
        {
            if( ! fng_pCD_PTT(buf))
            {
                printf("分区 s% 不存在",buf);
            }
        }
        else
        {
            printf("not file system");
        }
        return true;
    }
    cmd = Sh_GetWord(buf,&next_param);
    strlwr(cmd);
    exec_cmd = (struct tagShellCmdRsc *)Rsc_SearchSon(&cmd_list_node,cmd);
    if(exec_cmd != NULL)
    {
        result = exec_cmd->shell_cmd_func(next_param);
    }else
    {
        printf("\r\n无此命令\r\n");
        result = true;
    }
    return result;
}
//----控制台服务函数-----------------------------------------------------------
//功能: 返回console输入的字符，带console输入回车符时，执行命令。一次命令不得超过
//      255字符。
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
ptu32_t Sh_Service(void)
{
    char command[CN_SHELL_CMD_LIMIT+1];
    while(1)
    {
        EasyScanf(NULL,command);
        if(strlen(command) != 0)
            Sh_ExecCommand(command);  //执行命令
        if ((fng_pPrintWorkPath != NULL))
            fng_pPrintWorkPath( );
        printf(">");
    }
}

