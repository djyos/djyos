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
// 模块描述:exception.c
// 模块版本: V1.00(初始化版本)
// 创建人员:Administrator
// 创建时间:2013-7-26--上午10:15:29
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// ============================================================================
#include "stdint.h"
#include "stdio.h"
#include "endian.h"
#include "exp_api.h"
#include "exception.h"
#include "shell.h"

//#include "cpu_peri.h"

//#include "uart.h"
//#include "flash_s29glxxx.h"
//#include "shell.h"

#include <cdef21469.h>
#include <def21489.h>
#include <sru.h>
#include <sysreg.h>


extern ufast_t tg_int_lookup_table[];       //中断线查找表


extern exp_stack_space;      // The base of the exception stack
extern exp_stack_length;    // The size of the exception stack
extern exp_stack_end;       // The end of the exception stack


//#define Exception_StartAddress   (  (exp_stack_space + exp_stack_length - 1)
//                                          - ((exp_stack_space + exp_stack_length - 1) % 2) )

#define  CN_HARDEXP_INFODECODERNAME    "HARDEXP_DECODER"
#define  CN_SYSEXP_UNIVERSAL_LEN         ((u32)0x8b)
#define  CN_SYSEXP_SPORTSPECIAL_LEN      ((u32)0x69)
#define  CN_SYSEXP_SOVFI_MAX_LEN         ((u32)0x5d)




/*******************************************************************
*  function prototypes
*******************************************************************/

extern void __asm_exp_init(void);
extern bool_t Sh_InstallCmd(struct tagShellCmdTab const *cmd_tab,struct tagShellCmdRsc *cmd_rsc, u32 cmd_num);
extern void __asm_software_reset(void);

extern bool_t g_bScheduleEnable;



/* function prototypes */
enum _EN_EXP_DEAL_TYPE_ Exp_General(u32 major,u32 minor,u32 exp_len);
bool_t __Exp_InstructionDecoder(u32 *exp_addr);
bool_t __Exp_HardStackDecoder(u32 *exp_addr,u32 exp_len);
bool_t __Exp_SportDecoder(u32 *exp_addr,u32 exp_len);
bool_t __Exp_CalculateDecoder(u32 *exp_addr,u32 exp_len);
bool_t __Exp_UniversalDecoder(u32 *exp_addr,u32 exp_len);
bool_t __Exp_SpecialDecoder(u32 *exp_addr,u32 major,u32 exp_len);
void   __Exp_SwapCpuInfoByEndian(u32 *pAddr,u32 exp_len);
bool_t  Exp_Harddecoder(struct tagExpThrowPara *parahead, u32 endian);
bool_t Exp_Init(void);

bool_t Sh_SoftwareReset(char *param);
bool_t Sh_Reboot(char *param);
bool_t Sh_RestartSystem(char *param);


 void reset(void);
 void reboot(void);
 void restart_system(void);



//异常shell扩展命令
struct tagShellCmdTab const shell_cmd_reset_table[]=
{

    {
        "reset",
        Sh_SoftwareReset,
        "Reset, like the power on!",
        "COMMAND: reset+enter"
    },
    {
         "reboot",
        Sh_Reboot,
        "Reboot, go to the os version check to boot!",
        "COMMAND: reboot+enter"

    },


    {
         "restart-system",
        Sh_RestartSystem,
        "Only to restart the operating system",
        "COMMAND: restart-system+enter"

    },

};

#define cn_reset_shell_num  sizeof(shell_cmd_reset_table)/sizeof(struct tagShellCmdTab)
static struct tagShellCmdRsc tg_reset_shell_cmd_rsc[cn_reset_shell_num];


//定义各种异常向量名字
static char *s_pcMajorExpNameTab[CN_MAJOREXP_INVALID] =
{
    "IICDI",
    "SOVFI",
    "SPERRI",
    "CB7I",
    "CB15I",
    "FIXI",
    "FLTOI",
    "FLTUI",
    "FLTII",
    "Invalid Exception"
};

enum _EN_SYSEXP_CPU_Major_
{
    EN_SYSEXP_MAJOR_IICDI=0,
    EN_SYSEXP_MAJOR_SOVFI,
    EN_SYSEXP_MAJOR_SPERRI,
    EN_SYSEXP_MAJOR_BKPI,
    EN_SYSEXP_MAJOR_CB7I,
    EN_SYSEXP_MAJOR_CB15I,
    EN_SYSEXP_MAJOR_FIXI,
    EN_SYSEXP_MAJOR_FLTOI,
    EN_SYSEXP_MAJOR_FLTUI,
    EN_SYSEXP_MAJOR_FLTII,
    EN_SYSEXP_MAJOR_EMULI,
    EN_SYSEXP_MAJOR_INVALID_EXCEPTION,
};


static char *s_pcMinorExpNameTab[CN_MINOREXP_INVALID]=
{
    "Illegal_IOP_Access",
    "Unaligned_Long_Word_Access ",
    "PC_Full",
    "Status_Overflow",
    "Loop_Overflow",
    "SPORT0_Unexpected_Frame_Syncs",
    "SPORT0_A_Data_Error ",
    "SPORT0_B_Data_Error ",
    "SPORT1_Unexpected_Frame_Syncs",
    "SPORT1_A_Data_Error ",
    "SPORT1_B_Data_Error ",
    "SPORT2_Unexpected_Frame_Syncs",
    "SPORT2_A_Data_Error ",
    "SPORT2_B_Data_Error ",
    "SPORT3_Unexpected_Frame_Syncs",
    "SPORT3_A_Data_Error ",
    "SPORT3_B_Data_Error ",
    "SPORT4_Unexpected_Frame_Syncs",
    "SPORT4_A_Data_Error ",
    "SPORT4_B_Data_Error ",
    "SPORT5_Unexpected_Frame_Syncs",
    "SPORT5_A_Data_Error ",
    "SPORT5_B_Data_Error ",
    "SPORT6_Unexpected_Frame_Syncs",
    "SPORT6_A_Data_Error ",
    "SPORT6_B_Data_Error ",
    "SPORT7_Unexpected_Frame_Syncs",
    "SPORT7_A_Data_Error ",
    "SPORT7_B_Data_Error ",
    "I7 overflow",
    "I15 overflow",
    "ALU_Fixed_Overflow",
    "Multiplier_Fixed_Overflow",
    "ALU_Floating_Overflow",
    "Multiplier_Floating_Overflow",
    "ALU_Floating_Underflow",
    "Multiplier_Floating_Underflow",
    "ALU_Floating_Invalid",
    "Multiplier_Floating_Invalid",
    "Invalid Exception"
};


enum __EN_SYSEXP_CPU_Minor
{
    EN_SYSEXP_ILLEGAL_IOP_ACCESS     =1,
    EN_SYSEXP_PC_FULL,
    EN_SYSEXP_MINOR_PC_FULL,
    EN_SYSEXP_STATUS_OVERFLOW,
    EN_SYSEXP_LOOP_OVERFLOW,
    EN_SYSEXP_SP0_UNEXPECTED_FRAME_SYNCS,
    EN_SYSEXP_SP0_A_DATA_ERROR,
    EN_SYSEXP_SP0_B_DATA_ERROR,
    EN_SYSEXP_SP1_UNEXPECTED_FRAME_SYNCS,
    EN_SYSEXP_SP1_A_DATA_ERROR,
    EN_SYSEXP_SP1_B_DATA_ERROR,
    EN_SYSEXP_SP2_UNEXPECTED_FRAME_SYNCS,
    EN_SYSEXP_SP2_A_DATA_ERROR,
    EN_SYSEXP_SP2_B_DATA_ERROR,
    EN_SYSEXP_SP3_UNEXPECTED_FRAME_SYNCS,
    EN_SYSEXP_SP3_A_DATA_ERROR,
    EN_SYSEXP_SP3_B_DATA_ERROR,
    EN_SYSEXP_SP4_UNEXPECTED_FRAME_SYNCS,
    EN_SYSEXP_SP4_A_DATA_ERROR,
    EN_SYSEXP_SP4_B_DATA_ERROR,
    EN_SYSEXP_SP5_UNEXPECTED_FRAME_SYNCS,
    EN_SYSEXP_SP5_A_DATA_ERROR,
    EN_SYSEXP_SP5_B_DATA_ERROR,
    EN_SYSEXP_SP6_UNEXPECTED_FRAME_SYNCS,
    EN_SYSEXP_SP6_A_DATA_ERROR,
    EN_SYSEXP_SP6_B_DATA_ERROR,
    EN_SYSEXP_SP7_UNEXPECTED_FRAME_SYNCS,
    EN_SYSEXP_SP7_A_DATA_ERROR,
    EN_SYSEXP_SP7_B_DATA_ERROR,
    EN_SYSEXP_I7_OVERFLOW,
    EN_SYSEXP_I15_OVERFLOW,
    EN_SYSEXP_ALU_FIXED_OVERFLOW,
    EN_SYSEXP_MUL_FIXED_OVERFLOW,
    EN_SYSEXP_ALU_FLOAT_OVERFLOW,
    EN_SYSEXP_MUL_FLOAT_OVERFLOW,
    EN_SYSEXP_ALU_FLOAT_UNDERFLOW,
    EN_SYSEXP_MUL_FLOAT_UNDERFLOW,
    EN_SYSEXP_MUL_FLOAT_INVALID,
    EN_SYSEXP_ALU_FLOAT_INVALID,
    EN_SYSEXP_MINOR_INVALID_EXCEPTION,

};


// =============================================================================
// 函数功能：exception_generaldealer
//           此部分主要用来处理CPU异常通用接口，根据异常类型，决定是否抛出异常。
// 输入参数：u32 major,异常主编号；
//           u32 minor，异常分支编号；
//           u32 exp_len,异常信息长度
// 输出参数：无
// 返回值  ：_SYSEXP_RESULT_TYPE
// =============================================================================
enum _EN_EXP_DEAL_TYPE_ Exp_General(u32 major,u32 minor,u32 exp_len)
{
    char *majorname;
    struct tagExpThrowPara  head;
    u32  result = EN_EXP_DEAL_RESET;


    //make sure not shedule
    bool_t   shedule_bak;
    shedule_bak = g_bScheduleEnable ;
    g_bScheduleEnable = false;

    //着手异常处理
    //在异常处理汇编处理部分即出错，这个时候直接复位无需CPU参与处理的异常
    if(major>= EN_SYSEXP_MAJOR_INVALID_EXCEPTION||minor>=EN_SYSEXP_MINOR_INVALID_EXCEPTION)
    {
        major = EN_SYSEXP_MAJOR_INVALID_EXCEPTION;
        minor = EN_SYSEXP_MINOR_INVALID_EXCEPTION;
        majorname = s_pcMajorExpNameTab[major];
        printk("This exception is invalid!\n\r");
        //无需CORE 信息和PERI信息
        result = EN_EXP_DEAL_RESET;
    }
    else
    {
        majorname = s_pcMajorExpNameTab[major];
        //printk("Exception:%s\n\r",majorname);


     //接下来需要根据具体异常类型确定异常处理结果(result)
     switch (major)
     {
      case EN_SYSEXP_MAJOR_IICDI:
      result=EN_EXP_DEAL_RECORD;
      break;
      case EN_SYSEXP_MAJOR_SOVFI:
      result=EN_EXP_DEAL_RESTART;
      break;
      case EN_SYSEXP_MAJOR_SPERRI:
      result=EN_EXP_DEAL_RESTART;
      break;
      case EN_SYSEXP_MAJOR_CB7I:
      result=EN_EXP_DEAL_RESTART;
      break;
      case EN_SYSEXP_MAJOR_CB15I:
      result=EN_EXP_DEAL_RESTART;
      break;
      case EN_SYSEXP_MAJOR_FIXI:
      result=EN_EXP_DEAL_RESTART;
      break;
      case EN_SYSEXP_MAJOR_FLTOI:
      result=EN_EXP_DEAL_RESTART;
      break;
      case EN_SYSEXP_MAJOR_FLTUI:   //FLTUI异常只需记录，不需要复位。
      result=EN_EXP_DEAL_RECORD;
      break;
      case EN_SYSEXP_MAJOR_FLTII:
      result=EN_EXP_DEAL_RESTART;
      break;
      default:
      result=EN_EXP_DEAL_RESTART;
      break;

     }
    }

    head.dealresult=result;
    head.name=CN_HARDEXP_INFODECODERNAME;
    head.para=(u8*)(Exception_StartAddress-5);
    head.para_len=exp_len;
    head.validflag=true;
    Exp_Throw(&head, &result);
    //recovethe shedule
    g_bScheduleEnable = shedule_bak;


    return result;
}


// =============================================================================
// 函数功能：_ExpInstructionDecoder
//           此部分用来针对 IICDI/CB7I/CB15I异常解析发生异常地址处前面50条指令。
// 输入参数：u32 *exp_addr，存储指令地址
// 输出参数：无
// 返回值  ：成功解析返回true,错误返回false。
// =============================================================================
bool_t __Exp_InstructionDecoder(u32 *exp_addr)
{
    u32 i=0;
    printk("The 50 instructions before the address ocuured exception are printed as follwings:\r\n");
    for(i=0;i<50;i++)
    {
        printk("The %02d line is:%d\r\n",i,*(exp_addr+i));
    }
    return true;
}

// =============================================================================
// 函数功能：_ExpHardStackDecoder
//           此部分用来解析硬件栈溢出异常SOVFI中硬件栈中内容，依次输出Loop Stack
//           Status Stack、PC Stack三个栈深度及栈中内容。
// 输入参数：u32 *exp_addr，存储指令地址
//           u32 exp_len,   异常存储信息长度(SHARC21469 1个字节=32bit)
// 输出参数：无
// 返回值  ：成功解析返回true,错误返回false。
// =============================================================================
bool_t __Exp_HardStackDecoder(u32 *exp_addr,u32 exp_len)
{
    u32 i=0;
    u32 loop_couter=0;
    u32 sts_couter=0;
    u32 pc_couter=0;
    u32 temp=0;
    if(exp_len>CN_SYSEXP_SOVFI_MAX_LEN)
    {
        printk("Sport Exception info len is inconsistent!\n\r");
        return false; //可能不是一样的CPU或者版本，无法解析
    }

    else
   {
     printk("The HardStack infomation are printed as follwings:\r\n");

     //首先解析Loop Stack。
     loop_couter=*(exp_addr+exp_len-1);
     printk("The Loop Stack ocupied %02d layers\r\n",loop_couter);
    if(loop_couter!=0)
    {
        printk("Start to print Loop Stack.\r\n");
        for(i=1;i<=loop_couter;i++)
        {
          printk("%02d layer,LCNTR:%08x\r\n",   (loop_couter-i+1), *(exp_addr+exp_len-1-3*(i-1)-3));
          printk("%02d layer,LADDR:%08x\r\n",   (loop_couter-i+1), *(exp_addr+exp_len-1-3*(i-1)-2));
          printk("%02d layer,CURLCNTR:%08x\r\n",(loop_couter-i+1), *(exp_addr+exp_len-1-3*(i-1)-1));
        }
    }
    else
    {
         printk("The Loop Stack is empty.\r\n");
    }

    //开始解析Status Stack。
    temp=exp_len-1-3*loop_couter-1;
    sts_couter=*(exp_addr+temp);
     printk("The Status Stack ocupied %02d layers\r\n",sts_couter);
      if(sts_couter!=0)
        {
           printk("Start to print Status Stack.\r\n");
           for(i=1;i<=sts_couter;i++)
           {
            printk("%02d layer,MODE1:%08x\r\n", (sts_couter-i+1),*(exp_addr+temp-3*(i-1)-3));
            printk("%02d layer,ASTATx:%08x\r\n",(sts_couter-i+1),*(exp_addr+temp-3*(i-1)-2));
            printk("%02d layer,ASTATy:%08x\r\n",(sts_couter-i+1),*(exp_addr+temp-3*(i-1)-1));
           }
        }
             else
        {
            printk("The Status Stack is empty.\r\n");
        }

        //开始解析PC Stack.
        temp=exp_len-1-3*loop_couter-1-3*sts_couter-1;
        pc_couter=*(exp_addr+temp);
        printk("The PC Stack ocupied %02d layers\r\n",pc_couter);

        if(pc_couter!=0)
        {
            printk("Start to print PC Stack.\r\n");
            for(i=1;i<=pc_couter;i++)
            {
              printk("%02d layer, PCSTK:%08x\r\n",(pc_couter-i+1),*(exp_addr+temp-i));
            }
        }
        else
        {
            printk("The PC stack is empty.\r\n");
        }

        printk("Print Hard Stack end.\r\n");

    return true;
  }
}

// =============================================================================
// 函数功能：_ExpHardStackDecoder
//           此部分用来解析硬件栈溢出异常SOVFI中硬件栈中内容，依次输出Loop Stack
//           Status Stack、PC Stack三个栈深度及栈中内容。
// 输入参数：u32 *exp_addr，存储指令地址
//           u32 exp_len,   异常存储信息长度(SHARC21469 1个字节=32bit)
// 输出参数：无
// 返回值  ：成功解析返回true,错误返回false。
// =============================================================================
bool_t __Exp_SportDecoder(u32 *exp_addr,u32 exp_len)
{
    u32 i=0;
    u32 j=0;

    if(exp_len!=CN_SYSEXP_SPORTSPECIAL_LEN)
    {
        printk("Sport Exception info len is inconsistent!\n\r");
        return false; //可能不是一样的CPU或者版本，无法解析
    }
    else
    {
     printk("The SPORT related registers printed as follwings:\r\n");
     printk("SPERRSTAT:%08x\r\n",*exp_addr);
    for (i=0;i<=7;i++)
    {
     printk("SPERRCTL%01d:%08x\r\n",i,*(exp_addr+i+1));
    }
    for(i=0;i<=7;i++)
    {
        printk("DIV%01d:%08x\r\n",i,*(exp_addr+9+i));
    }
    for(i=0;i<=7;i++)
    {
        printk("SPCTL%01d:%08x\r\n",i,*(exp_addr+17+i));
    }
    for(i=0;i<=7;i++)
    {
        printk("SPCTLN%01d:%08x\r\n",i,*(exp_addr+25+i));
    }
    for(i=0;i<=7;i++)
    {
        printk("SPMCTL%01d:%08x\r\n",i,*(exp_addr+33+i));
    }
    for(i=0;i<=7;i++)
    {
        for(j=0;j<=3;j++)
        {
            printk("SP%01dCS%01d:%08x\r\n",i,j,*(exp_addr+41+4*i+j));
        }
    }
    for(i=0;i<=7;i++)
    {
        for(j=0;j<=3;j++)
        {
            printk("SP%01dCCS%01d:%08x\r\n",i,j,*(exp_addr+73+4*i+j));
        }
    }
    printk("Print the SPORT related registers end.\r\n");

    return true;
    }

}


// =============================================================================
// 函数功能：_ExpUniversalDecoder
//           此部分用来解析异常信息通用寄存器值。
// 输入参数：u32 *exp_addr，存储指令地址
//           u32 exp_len,   异常存储信息长度(SHARC21469 1个字节=32bit)
// 输出参数：无
// 返回值  ：成功解析返回true,错误返回false。
// =============================================================================

bool_t __Exp_UniversalDecoder(u32 *exp_addr,u32 exp_len)
{
     u32 i =0;
     //u32 *regvalue;
     if(exp_len!=CN_SYSEXP_UNIVERSAL_LEN)
     {
            printk("cpuinfodecoder:incomplete info frame!\n\r");
            return false;
     }
     else
     {
     printk("SysExpInfo:SHARC21469 CORE REGISTERS:\n\r");
     printk("Universal REGISTERS(IN HEX):\n\r");
     //regvalue=(u32 *)exp_addr;

    printk("MODE1:%08x\n\r",*(exp_addr+2));
    printk("MODE2:%08x\n\r",*(exp_addr+3));
    for(i=1;i<5;i++)
    {
        printk("USTAT%d:%08x\n\r",i,*(exp_addr+3+i));
    }
    printk("PCSTK:%08x\n\r",*(exp_addr+0xa));
    printk("ASTATx:%08x\n\r",*(exp_addr+0xb));
    printk("ASTATy:%08x\n\r",*(exp_addr+0xc));
    printk("STKYx:%08x\n\r",*(exp_addr+0xd));
    printk("STKYy:%08x\n\r",*(exp_addr+0xe));
    printk("IRPTL:%08x\n\r",*(exp_addr+0xf));
    printk("LIRPTL:%08x\n\r",*(exp_addr+0x10));
    printk("IMASK:%08x\n\r",*(exp_addr+0x11));

    //Print R0~R15
    printk("PX1:%08x\n\r",*(exp_addr+0x12));
    printk("PX2:%08x\n\r",*(exp_addr+0x13));
    printk("R0(low):%08x\n\r",*(exp_addr+0x14));
    printk("R0(high):%08x\n\r",*(exp_addr+0x15));
    printk("R1:%08x\n\r",*(exp_addr+0x1));
    printk("R2:%08x\n\r",*(exp_addr+0x0));
    printk("R3(low):%08x\n\r",*(exp_addr+0x16));
    printk("R3(high):%08x\n\r",*(exp_addr+0x17));
    printk("R4(low):%08x\n\r",*(exp_addr+0x18));
    printk("R4(high):%08x\n\r",*(exp_addr+0x19));
    printk("R5:%08x\n\r",*(exp_addr+0x1a));
    printk("R6:%08x\n\r",*(exp_addr+0x1b));
    printk("R7(low):%08x\n\r",*(exp_addr+0x1c));
    printk("R7(high):%08x\n\r",*(exp_addr+0x1d));
    printk("R8(low):%08x\n\r",*(exp_addr+0x1e));
    printk("R8(high):%08x\n\r",*(exp_addr+0x1f));
    printk("R9(low):%08x\n\r",*(exp_addr+0x20));
    printk("R9(high):%08x\n\r",*(exp_addr+0x21));
    printk("R10:%08x\n\r",*(exp_addr+0x22));
    printk("R11(low):%08x\n\r",*(exp_addr+0x23));
    printk("R11(high):%08x\n\r",*(exp_addr+0x24));
    printk("R12(low):%08x\n\r",*(exp_addr+0x25));
    printk("R12(high):%08x\n\r",*(exp_addr+0x26));
    printk("R13:%08x\n\r",*(exp_addr+0x27));
    printk("R14:%08x\n\r",*(exp_addr+0x28));
    printk("R15:%08x\n\r",*(exp_addr+0x29));
    //Print S0~S15
    printk("S0(low):%08x\n\r",*(exp_addr+0x2a));
    printk("S0(high):%08x\n\r",*(exp_addr+0x2b));
    printk("S1:%08x\n\r",*(exp_addr+0x2c));
    printk("S2:%08x\n\r",*(exp_addr+0x2d));
    printk("S3:%08x\n\r",*(exp_addr+0x2e));
    printk("S4(low):%08x\n\r",*(exp_addr+0x2f));
    printk("S4(high):%08x\n\r",*(exp_addr+0x30));
    printk("S5:%08x\n\r",*(exp_addr+0x31));
    printk("S6:%08x\n\r",*(exp_addr+0x32));
    printk("S7:%08x\n\r",*(exp_addr+0x33));
    printk("S8:%08x\n\r",*(exp_addr+0x34));
    printk("S9(low):%08x\n\r",*(exp_addr+0x35));
    printk("S9(high):%08x\n\r",*(exp_addr+0x36));
    printk("S10:%08x\n\r",*(exp_addr+0x37));
    printk("S11(low):%08x\n\r",*(exp_addr+0x38));
    printk("S11(high):%08x\n\r",*(exp_addr+0x39));
    printk("S12(low):%08x\n\r",*(exp_addr+0x3a));
    printk("S12(high):%08x\n\r",*(exp_addr+0x3b));
    printk("S13:%08x\n\r",*(exp_addr+0x3c));
    printk("S14:%08x\n\r",*(exp_addr+0x3d));
    printk("S15:%08x\n\r",*(exp_addr+0x3e));
    //Print I0~I15
    for(i=0;i<16;i++)
    {
        printk("I%d:%08x\n\r",i,*(exp_addr+0x3f+i));
    }
    //Print M0~M15
    for(i=0;i<16;i++)
    {
        printk("M%d:%08x\n\r",i,*(exp_addr+0x4f+i));
    }
    //Print B0~B15
    for(i=0;i<16;i++)
    {
        printk("B%d:%08x\n\r",i,*(exp_addr+0x5f+i));
    }
    //Print L0~L15
    for(i=0;i<16;i++)
    {
        printk("L%d:%08x\n\r",i,*(exp_addr+0x6f+i));
    }
    //Print MRB/MRF
    printk("MR0F:%08x\n\r",*(exp_addr+0x7f));
    printk("MR1F:%08x\n\r",*(exp_addr+0x80));
    printk("MR2F:%08x\n\r",*(exp_addr+0x81));
    printk("MR0B:%08x\n\r",*(exp_addr+0x82));
    printk("MR1B:%08x\n\r",*(exp_addr+0x83));
    printk("MR2B:%08x\n\r",*(exp_addr+0x84));
    return true;
    }

}


// =============================================================================
// 函数功能：_ExpSpecialDecoder
//           此部分用来解析异常信息中非通用信息，针对不同异常类型，保存的特殊信息。
// 输入参数：u32 major，    主异常编号
//           u32 exp_len,   异常存储信息长度(SHARC21469 1个字节=32bit)
// 输出参数：无
// 返回值  ：成功解析返回true,错误返回false。
// =============================================================================

bool_t __Exp_SpecialDecoder(u32 *exp_addr,u32 major,u32 exp_len)
{
    u32 i=0;
     switch(major)
    {
        case  2||5||6:
        __Exp_InstructionDecoder(exp_addr);
          break;
        case  3:
        __Exp_HardStackDecoder(exp_addr, exp_len);
          break;
        case  4:
        __Exp_SportDecoder(exp_addr,exp_len);
         break;
        case 7:
        __Exp_InstructionDecoder(exp_addr);
        break;
        case 8:
        __Exp_InstructionDecoder(exp_addr);
        break;
        case 9:
        __Exp_InstructionDecoder(exp_addr);
        break;
        case 10:
        __Exp_InstructionDecoder(exp_addr);
        break;
        default:
        break;
    }



}


// =============================================================================
// 函数功能：__SwapExpCpuInfoByEndian
//          转换硬件CPU异常信息字节序
// 输入参数：cpuinfo, CPU异常通用信息
// 输出参数：cpuinfo, CPU异常通用信息（转换后）
// 返回值  ：无
// 说明    ：内部调用
// =============================================================================
void __Exp_SwapCpuInfoByEndian(u32 *pAddr,u32 exp_len)
{
    //u32 *temp;
    u32 i=0;
    for(i = 0; i<exp_len; i++)
    {
        *pAddr = swapl(*pAddr);
        pAddr++;
    }
}


// =============================================================================
// 函数功能：exception_harddecoder
//           此部分主要用来处理处理器的通用异常信息
// 输入参数：parahead，抛出时的参数
//           endian, 信息的存储格式
// 输出参数：无
// 返回值  : true成功, false失败
// 说明    ：
// =============================================================================
bool_t  Exp_Harddecoder(struct tagExpThrowPara *parahead, u32 endian)
{
    bool_t result = false;
    u32 exp_len=0;
    u32 major=0;
    u32 * exp_addr;

    //struct tagSysExpCpuInfo     *CpuInfo;

    if((parahead == NULL) || (false == parahead->validflag))
    {
        //非本层信息，无需解析
        printk("cpuinfodecoder:para error or no need to decode-- SHARC21469 decoder\n\r");
        result = false;
    }
    else
    {
            //基本算是完整包，慢慢解析吧
            //CpuInfo = (struct tagSysExpCpuInfo *)(parahead->para);
            exp_addr=(u32 *)(parahead->para);
            exp_len=parahead->para_len;

            //存储端转换
            if(endian != CN_CFG_BYTE_ORDER)
            {
                __Exp_SwapCpuInfoByEndian(exp_addr,exp_len);
            }
            if(cn_sysexp_cpuinfo_validflag == *exp_addr)//当前版本，可以解析
            {
                printk("CPUINFO LIST:\n\r");
                printk("cpuinfo:len = 0x%08x\n\r",parahead->para_len);
                printk("cpuinfo:major =0x%08x, %s\n\r",*(exp_addr+1),\
                        s_pcMajorExpNameTab[(*(exp_addr+1))]);
                printk("cpuinfo:minor =0x%08x, %s\n\r",*(exp_addr+2),\
                        s_pcMinorExpNameTab[(*(exp_addr+2))]);
                printk("cpuinfo:deal result = 0x%08x\n\r", parahead->dealresult);
                //通用信息解析
                exp_addr=exp_addr+5;
                exp_len=CN_SYSEXP_UNIVERSAL_LEN;

                __Exp_UniversalDecoder(exp_addr,exp_len);

                //特殊信息解析

                major=*(exp_addr-4);
                exp_len=parahead->para_len-0x5-CN_SYSEXP_UNIVERSAL_LEN;
                exp_addr=(u32 *)((parahead->para)+0x5+CN_SYSEXP_UNIVERSAL_LEN);
                __Exp_SpecialDecoder(exp_addr, major,exp_len);
                result = true;


        }
        else
        {
            printk("cpuinfodecoder:incomplete info frame!\n\r");
            result = false;
        }
    }

    return result;
}
//------------------------------------------------------------------------------
//function : this is used to set the exception vectors.
//parameter: NULL, we will set the vector address by the symbols in the asm file
//return   : NULL, just set the exception tab, only could execute once
//remarks  : sensitive to transplant,we do it the asm file
//           if you want to debug the code, please set the vector in the initial
//           stage which means set in the asm mode, else please set it in the
//           C mode which means in this function.
//------------------------------------------------------------------------------

bool_t Exp_Init(void)
{

//注册硬件异常解析函数
    bool_t Exp_RegisterThrowinfoDecoder(fnExp_ThrowinfoDecoderModule decoder,const char *name);
    Exp_RegisterThrowinfoDecoder(Exp_Harddecoder, CN_HARDEXP_INFODECODERNAME);
    __asm_exp_init();

    return true;
}



ptu32_t module_exp_shell_init(ptu32_t para)
{
    para=para;
    Sh_InstallCmd(shell_cmd_reset_table,tg_reset_shell_cmd_rsc,cn_reset_shell_num);
    return 1;
}





//------------------------------------------------------------------------------
//function:this function is used to erase exception information in the flash.
//parameter:
//return:
//remarks:sensitive to transplant
//------------------------------------------------------------------------------
bool_t Sh_SoftwareReset(char *param)
{
    __asm_software_reset();
    return true;
}


//------------------------------------------------------------------------------
//function:this function is used to reboot the os .
//parameter:
//return:
//remarks:sensitive to transplant
//------------------------------------------------------------------------------

extern void ___second_stage_reboot(u32);

bool_t Sh_Reboot(char *param)
{
  u32 flag;
   char *word_addr,*next_param;
    //提取参数
    word_addr = Sh_GetWord(param,&next_param);
    if(word_addr == NULL)
    {
        printf("\r\n格式错误，正确格式是：\r\n异常条目\r\n");
        return false;
    }
    flag = strtol(word_addr, (char **)NULL, 0);
    if(flag==monitor_reboot)
        __second_stage_reboot(flag);
    else if(flag==release_reboot)
        __second_stage_reboot(flag);
    else
        return false;


 return true;
}

//------------------------------------------------------------------------------
//function:this function is used to restart the operation syatem .
//parameter:
//return:
//remarks:sensitive to transplant
//------------------------------------------------------------------------------
bool_t Sh_RestartSystem(char * param)
{
    u32 flag;
#ifdef Release_version
    flag=release_reboot;
#endif
#ifdef Monitor_version
    flag=monitor_reboot;
#endif
    __second_stage_reboot(flag);

return true;
}

// =============================================================================
// 函数功能：reset
//           复位CPU
// 输入参数：无
//
// 输出参数：无
// 返回值  : 无
// 说明    ：
// =============================================================================
void reset(void)
{
    __asm_software_reset();
}


// =============================================================================
// 函数功能：reboot
//           reboot CPU
// 输入参数：无
//
// 输出参数：无
// 返回值  : 无
// 说明    ：
// =============================================================================

void reboot(void)
{

}

// =============================================================================
// 函数功能：restart_system
//           reboot CPU
// 输入参数：无
//
// 输出参数：无
// 返回值  : 无
// 说明    ：
// =============================================================================

void restart_system(void)
{
}














