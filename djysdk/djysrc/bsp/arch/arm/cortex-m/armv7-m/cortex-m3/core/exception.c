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
//所属模块:中断模块
//作者：lst
//版本：V1.1.0
//文件描述: cm3异常处理模块
//其他说明:
//修订历史:
//2. 日期: 2010-01-20
//   作者: lst
//   新版本号: V1.1.0
//   修改说明: 修改 exp_esr_tick函数，否则在tick中断中调用异步信号的使能和禁止
//      函数时，由于en_asyn_signal_counter等参数未正确设置，会出问题。
//1. 日期: 2009-08-30
//   作者: lst
//   新版本号: V1.0.0
//   修改说明:
//------------------------------------------------------
#include "stdint.h"
#include "exception.h"
#include "exp_api.h"
#include "stdio.h"
#include "endian.h"
#include "cpu_peri.h"
#include "int.h"
#include "djyos.h"

extern ufast_t tg_int_lookup_table[];       //中断线查找表
extern struct tagIntMasterCtrl  tg_int_global;          //定义并初始化总中断控制结构
extern void __Djy_ScheduleAsynSignal(void);

//异常向量表的外部声名
extern void exp_systick_handler(void);
extern void exp_svc_handler(void);
extern void exp_memfault_handler(void);
extern void exp_usagefault_handler(void);
extern void exp_busfault_handler(void);

extern   uint32_t   msp_top[ ];
extern   void Init_Cpu(void);
extern   void hardfault_handler(void);


//系统tick函数的声名
void (*user_systick)(u32 inc_ticks);

u32 g_u32ExpTable[16] __attribute__((section(".table.exceptions")));
// =============================================================================
// 功能:异常向量表初始化，将向量表存放于指定的地址段内,并使能各种异常
// 参数:无
// 返回:无
// =============================================================================
static void __Exp_TableSet(void)
{
    g_u32ExpTable[0]                            = (uint32_t)msp_top;
    g_u32ExpTable[CN_SYSVECT_RESET]             = (uint32_t) Init_Cpu;
    g_u32ExpTable[CN_SYSVECT_NMI]               = (uint32_t) hardfault_handler;
    g_u32ExpTable[CN_SYSVECT_HARD_FAULT]        = (uint32_t) hardfault_handler;
    g_u32ExpTable[CN_SYSVECT_MEMMANAGE_FAULT]  = (uint32_t) exp_memfault_handler;
    g_u32ExpTable[CN_SYSVECT_BUS_FAULT]         = (uint32_t) exp_busfault_handler;
    g_u32ExpTable[CN_SYSVECT_USAGE_FAULT]   = (uint32_t) exp_usagefault_handler;
    g_u32ExpTable[7]                            = (uint32_t) hardfault_handler;
    g_u32ExpTable[8]                            = (uint32_t) hardfault_handler;
    g_u32ExpTable[9]                            = (uint32_t) hardfault_handler;
    g_u32ExpTable[10]                       = (uint32_t) hardfault_handler;
    g_u32ExpTable[CN_SYSVECT_SVC]               = (u32)exp_svc_handler;
    g_u32ExpTable[CN_SYSVECT_DEBUG]             = (uint32_t) hardfault_handler;
    g_u32ExpTable[13]                       = (uint32_t) hardfault_handler;
    g_u32ExpTable[CN_SYSVECT_PENDSV]            = (uint32_t) hardfault_handler;
    g_u32ExpTable[CN_SYSVECT_SYSTICK]           = (u32)exp_systick_handler;

    pg_scb_reg->VTOR = (u32)g_u32ExpTable;
    pg_scb_reg->systick_pri15 = 0xff;   //最低优先级,systick应该能随异步信号关断
    pg_scb_reg->svc_pri11 = 0x0;       //svc的优先级和异步信号相同。
    pg_scb_reg->SHCSR |=0x70000;     //允许 总线、用法、存储器 fault

    pg_scb_reg->CCR |= 0x610;       //强制SP自动对齐，除0
}

// =============================================================================
// 函数功能：__SwapExpCpuInfoByEndian
//          转换硬件CPU异常信息字节序
// 输入参数：cpuinfo, CPU异常通用信息
// 输出参数：cpuinfo, CPU异常通用信息（转换后）
// 返回值  ：无
// 说明    ：内部调用
// =============================================================================
void __SwapExpCpuInfoByEndian(struct SysExceptionInfo *cpuinfo)
{
    u32 *temp;
    u32 i,len;
    temp =(u32 *)cpuinfo;
    len = (sizeof(struct SysExceptionInfo))/4;
    for(i = 0; i<len; i++)
    {
        *temp = swapl(*temp);
        temp++;
    }
}

// =============================================================================
// 功能:SYSTICK函数关联函数，将指定的tick函数初始化，在Exp_SystickTickHandler被调用
// 参数:tick,SYSTICK的用户服务函数
// 返回:无
// =============================================================================
void Exp_ConnectSystick(void (*tick)(u32 inc_ticks))
{
    user_systick = tick;
}

// =============================================================================
// 功能:在Cortex M系统内核中，内核系统定时器SYSTICI定时中断属于异常，该函数是从异常
//      向量表的汇编程序中跳到C的服务函数，它实现系统TICK服务
// 参数:无
// 返回:无
// =============================================================================
void Exp_SystickTickHandler(void)
{
    g_bScheduleEnable = false;
//    tg_int_global.en_asyn_signal = false;
    tg_int_global.en_asyn_signal_counter = 1;
    tg_int_global.nest_asyn_signal = 1;

    user_systick(1);

    tg_int_global.nest_asyn_signal = 0;
//    tg_int_global.en_asyn_signal = true;
    tg_int_global.en_asyn_signal_counter = 0;
    if(g_ptEventReady != g_ptEventRunning)
        __Djy_ScheduleAsynSignal();       //执行中断内调度
    g_bScheduleEnable = true;
}

// =============================================================================
// 功能:系统内存管理异常服务函数，主要处理与memory manage 异常相关的异常
// 参数:core_info，异常核心现场
// 返回: _SYSEXP_RESULT_TYPE
// =============================================================================
enum _EN_EXP_DEAL_TYPE_ Exp_MemManageFaultHandler(u32 *core_info)
{
    struct SysExceptionInfo ExpRecord;
    struct tagExpThrowPara  head;
    u32 ExpPC;
    u32 result = EN_EXP_DEAL_IGNORE;
    bool_t   shedule_bak,MMA_Flag=false;

    shedule_bak = g_bScheduleEnable ;
    g_bScheduleEnable = false;

    ExpRecord.CoreInfo = *((struct SysExpCoreInfo *)core_info);

    ExpRecord.NvicInfo.bfsr = pg_scb_reg->BFSR;
    ExpRecord.NvicInfo.dfsr = pg_scb_reg->DFSR;
    ExpRecord.NvicInfo.hfsr = pg_scb_reg->HFSR;
    ExpRecord.NvicInfo.mfsr = pg_scb_reg->MFSR;
    ExpRecord.NvicInfo.ufsr = pg_scb_reg->UFSR;

    ExpRecord.NvicInfo.mmar = pg_scb_reg->MMFAR;
    ExpRecord.NvicInfo.bfar = pg_scb_reg->BFAR;
    ExpRecord.SysExpCpuFlag = CN_SYS_EXP_CPUINFO_VALIDFLAG;

    if(pg_scb_reg->MFSR & CN_EXP_MFSR_MMARVALID)
    {
        ExpPC = pg_scb_reg->MMFAR;
        MMA_Flag = true;
    }
    //handler the exp
    if(pg_scb_reg->MFSR & CN_EXP_MFSR_IACCVIOL)
    {
        if(MMA_Flag == true)
        {
            printk("MEM Fault: Instruction Access Err At 0x%08x\n\r",ExpPC);
        }
        else
        {
            printk("MEM Fault: Instruction Access Err\n\r");
        }
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->MFSR & CN_EXP_MFSR_DACCVIOL)
    {
        if(MMA_Flag == true)
        {
            printk("MEM Fault: Data Access Err At 0x%08x\n\r",ExpPC);
        }
        else
        {
            printk("MEM Fault: Data Access Err\n\r");
        }
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->MFSR & CN_EXP_MFSR_MUNSTKERR)
    {
        printk("MEM Fault: Unstack Err\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->MFSR & CN_EXP_MFSR_MSTKERR)
    {
        printk("MEM Fault: Stack Err\n\r");
        result = EN_EXP_DEAL_RESET;
        result = EN_EXP_DEAL_RESET;
    }
    else
    {
        printk("MEM Fault: %s\n\r","Unknown MEM Fault");
        result = EN_EXP_DEAL_RESET;
    }
    //clear the flag of exp
    pg_scb_reg->HFSR = 0xFFFFFFFF;
    pg_scb_reg->BFSR = 0xFF;
    pg_scb_reg->MFSR = 0xFF;
    pg_scb_reg->UFSR = 0xFFFF;


   head.dealresult = result;
   head.para  = (u8 *)(&ExpRecord);
   head.para_len = sizeof(struct SysExceptionInfo);
   head.name = CN_HARDEXP_DECODERNAME;
   head.validflag = true;
   Exp_Throw(&head,&result);

    //recovethe shedule
    g_bScheduleEnable = shedule_bak;

    return result;
}

// =============================================================================
// 功能:系统硬件错误，可能会是由于其他的fault上仿的结果，如果是，则应该进一步查询原因
// 参数:core_info，异常核心现场
// 返回: _SYSEXP_RESULT_TYPE
// =============================================================================
enum _EN_EXP_DEAL_TYPE_ Exp_HardFaultHandler(u32 *core_info)
{
    struct SysExceptionInfo ExpRecord;
    struct tagExpThrowPara  head;
    u32 result = EN_EXP_DEAL_IGNORE;
    bool_t   shedule_bak;

    shedule_bak = g_bScheduleEnable ;
    g_bScheduleEnable = false;

    ExpRecord.CoreInfo = *((struct SysExpCoreInfo *)core_info);

    ExpRecord.NvicInfo.bfsr = pg_scb_reg->BFSR;
    ExpRecord.NvicInfo.dfsr = pg_scb_reg->DFSR;
    ExpRecord.NvicInfo.hfsr = pg_scb_reg->HFSR;
    ExpRecord.NvicInfo.mfsr = pg_scb_reg->MFSR;
    ExpRecord.NvicInfo.ufsr = pg_scb_reg->UFSR;

    ExpRecord.NvicInfo.mmar = pg_scb_reg->MMFAR;
    ExpRecord.NvicInfo.bfar = pg_scb_reg->BFAR;
    ExpRecord.SysExpCpuFlag = CN_SYS_EXP_CPUINFO_VALIDFLAG;

    //handler the exp
    if(pg_scb_reg->HFSR & CN_EXP_HFSR_VECTBL)
    {
        printk("Hard Fault: VECTBL Access Err\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->UFSR & CN_EXP_HFSR_FORCED)
    {
        printk("Hard Fault: Forced By Other Fault\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->HFSR & CN_EXP_HFSR_DEBUGEVT)
    {
        printk("Hard Fault: Because Of Debug\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else
    {
        printk("Hard Fault: %s\n\r","Unknown Hard Fault");
        result = EN_EXP_DEAL_RESET;
    }
    //clear the flag of exp
    pg_scb_reg->HFSR = 0xFFFFFFFF;
    pg_scb_reg->BFSR = 0xFF;
    pg_scb_reg->MFSR = 0xFF;
    pg_scb_reg->UFSR = 0xFFFF;


   head.dealresult = result;
   head.para  = (u8 *)(&ExpRecord);
   head.para_len = sizeof(struct SysExceptionInfo);
   head.name = CN_HARDEXP_DECODERNAME;
   head.validflag = true;
   Exp_Throw(&head,&result);

    //recovethe shedule
    g_bScheduleEnable = shedule_bak;

    return result;
}

// =============================================================================
// 功能:系统由于调试引起的异常，不作分析，只报告错误
// 参数:core_info，异常核心现场
// 返回: _SYSEXP_RESULT_TYPE
// =============================================================================
enum _EN_EXP_DEAL_TYPE_ Exp_DebugFaultHandler(u32 *core_info)
{
    struct SysExceptionInfo ExpRecord;
    struct tagExpThrowPara  head;
    u32 result = EN_EXP_DEAL_IGNORE;
    bool_t   shedule_bak;

    shedule_bak = g_bScheduleEnable ;
    g_bScheduleEnable = false;

    ExpRecord.CoreInfo = *((struct SysExpCoreInfo *)core_info);

    ExpRecord.NvicInfo.bfsr = pg_scb_reg->BFSR;
    ExpRecord.NvicInfo.dfsr = pg_scb_reg->DFSR;
    ExpRecord.NvicInfo.hfsr = pg_scb_reg->HFSR;
    ExpRecord.NvicInfo.mfsr = pg_scb_reg->MFSR;
    ExpRecord.NvicInfo.ufsr = pg_scb_reg->UFSR;

    ExpRecord.NvicInfo.mmar = pg_scb_reg->MMFAR;
    ExpRecord.NvicInfo.bfar = pg_scb_reg->BFAR;
    ExpRecord.SysExpCpuFlag = CN_SYS_EXP_CPUINFO_VALIDFLAG;

    //handler the exp
    printk("Debug Fault: %s\n\r","This is a debug fault");
    result = EN_EXP_DEAL_RESET;

    //clear the flag of exp
    pg_scb_reg->HFSR = 0xFFFFFFFF;
    pg_scb_reg->BFSR = 0xFF;
    pg_scb_reg->MFSR = 0xFF;
    pg_scb_reg->UFSR = 0xFFFF;


   head.dealresult = result;
   head.para  = (u8 *)(&ExpRecord);
   head.para_len = sizeof(struct SysExceptionInfo);
   head.name = CN_HARDEXP_DECODERNAME;
   head.validflag = true;
   Exp_Throw(&head,&result);

    //recovethe shedule
    g_bScheduleEnable = shedule_bak;

    return result;
}

// =============================================================================
// 功能:系统内存管理异常服务函数，主要处理与memory manage 异常相关的异常
// 参数:core_info，异常核心现场
// 返回: _SYSEXP_RESULT_TYPE
// =============================================================================
enum _EN_EXP_DEAL_TYPE_ Exp_BusFaultHandler(u32 *core_info)
{
    struct SysExceptionInfo ExpRecord;
    struct tagExpThrowPara  head;
    u32 ExpPC;
    u32 result = EN_EXP_DEAL_IGNORE;
    bool_t   shedule_bak;

    shedule_bak = g_bScheduleEnable ;
    g_bScheduleEnable = false;

    ExpRecord.CoreInfo = *((struct SysExpCoreInfo *)core_info);

    ExpRecord.NvicInfo.bfsr = pg_scb_reg->BFSR;
    ExpRecord.NvicInfo.dfsr = pg_scb_reg->DFSR;
    ExpRecord.NvicInfo.hfsr = pg_scb_reg->HFSR;
    ExpRecord.NvicInfo.mfsr = pg_scb_reg->MFSR;
    ExpRecord.NvicInfo.ufsr = pg_scb_reg->UFSR;

    ExpRecord.NvicInfo.mmar = pg_scb_reg->MMFAR;
    ExpRecord.NvicInfo.bfar = pg_scb_reg->BFAR;

    ExpRecord.SysExpCpuFlag = CN_SYS_EXP_CPUINFO_VALIDFLAG;

    //handler the exp
    if(pg_scb_reg->BFSR & CN_EXP_BFSR_IBUSERR)
    {
        printk("Bus Fault: Access Instruction Err\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->BFSR & CN_EXP_BFSR_PRECISERR)
    {
        //查找异常时的地址
        if(pg_scb_reg->BFSR & CN_EXP_BFSR_BFARVALID)
        {
            ExpPC = ((struct SysExpCoreInfo *)core_info)->spr_pc;
            printk("Bus Fault: Data Access Err At 0x%08x\n\r",ExpPC);
        }
        else
        {
            printk("Bus Fault: Data Access Err Unknown PC\n\r");
        }
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->BFSR & CN_EXP_BFSR_IMPRECISERR)
    {
        printk("Bus Fault: Data Access Err\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->BFSR & CN_EXP_BFSR_UNSTKERR)
    {
        printk("Bus Fault: Exception Unstack Err\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->BFSR & CN_EXP_BFSR_STKERR)
    {
        printk("Bus Fault: Exception Stack Err\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else
    {
        printk("Bus Fault: %s\n\r","Unknown Fault");
        result = EN_EXP_DEAL_RESET;
    }
    //clear the flag of exp
    pg_scb_reg->HFSR = 0xFFFFFFFF;
    pg_scb_reg->BFSR = 0xFF;
    pg_scb_reg->MFSR = 0xFF;
    pg_scb_reg->UFSR = 0xFFFF;


   head.dealresult = result;
   head.para  = (u8 *)(&ExpRecord);
   head.para_len = sizeof(struct SysExceptionInfo);
   head.name = CN_HARDEXP_DECODERNAME;
   head.validflag = true;
   Exp_Throw(&head,&result);

    //recovethe shedule
    g_bScheduleEnable = shedule_bak;

    return result;
}

// =============================================================================
// 功能：此部分主要用来处理处理器的Usage异常信息
// 参数：core_info,异常核心现场
// 返回 ： _SYSEXP_RESULT_TYPE
// =============================================================================
enum _EN_EXP_DEAL_TYPE_ Exp_UsageFaultHandler(u32 *core_info)
{
    struct SysExceptionInfo ExpRecord;
    struct tagExpThrowPara  head;
    u32 ExpPC;
    u32 result = EN_EXP_DEAL_IGNORE;
    bool_t   shedule_bak;

    shedule_bak = g_bScheduleEnable ;
    g_bScheduleEnable = false;

    ExpRecord.CoreInfo = *((struct SysExpCoreInfo *)core_info);

    ExpRecord.NvicInfo.bfsr = pg_scb_reg->BFSR;
    ExpRecord.NvicInfo.dfsr = pg_scb_reg->DFSR;
    ExpRecord.NvicInfo.hfsr = pg_scb_reg->HFSR;
    ExpRecord.NvicInfo.mfsr = pg_scb_reg->MFSR;
    ExpRecord.NvicInfo.ufsr = pg_scb_reg->UFSR;

    ExpRecord.NvicInfo.mmar = pg_scb_reg->MMFAR;
    ExpRecord.NvicInfo.bfar = pg_scb_reg->BFAR;

    ExpRecord.SysExpCpuFlag = CN_SYS_EXP_CPUINFO_VALIDFLAG;

    //handler the exp
    if(pg_scb_reg->UFSR & CN_EXP_UFSR_UNDEFINSTR)
    {
        printk("Usage Fault: Undefined Instruction\n\r");
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->UFSR & CN_EXP_UFSR_INVSTATE)
    {
        ExpPC = ((struct SysExpCoreInfo *)core_info)->spr_pc;
        printk("Usage Fault: Attempt To Enter ARM State At 0x%08x\n\r",ExpPC);
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->UFSR & CN_EXP_UFSR_INVPC)
    {
        ExpPC = ((struct SysExpCoreInfo *)core_info)->spr_pc;
        printk("Usage Fault: Invalid PC 0x%08x\n\r",ExpPC);
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->UFSR & CN_EXP_UFSR_NOCP)
    {
        ExpPC = ((struct SysExpCoreInfo *)core_info)->spr_pc;
        printk("Usage Fault: Attempt to Coprocessor At 0x%08x\n\r",ExpPC);
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->UFSR & CN_EXP_UFSR_UNALIGNED)
    {
        ExpPC = ((struct SysExpCoreInfo *)core_info)->spr_pc;
        printk("Usage Fault: Unaligned At 0x%08x\n\r",ExpPC);
        result = EN_EXP_DEAL_RESET;
    }
    else if(pg_scb_reg->UFSR & CN_EXP_UFSR_DIVBYZERO)
    {
        ExpPC = ((struct SysExpCoreInfo *)core_info)->spr_pc;
        printk("Usage Fault: Divide By Zero At 0x%08x\n\r",ExpPC);
        result = EN_EXP_DEAL_RESET;
    }
    else
    {
        printk("Usage Fault: %s\n\r","Unknown Fault");
        result = EN_EXP_DEAL_RESET;
    }
    //clear the flag of exp
    pg_scb_reg->HFSR = 0xFFFFFFFF;
    pg_scb_reg->BFSR = 0xFF;
    pg_scb_reg->MFSR = 0xFF;
    pg_scb_reg->UFSR = 0xFFFF;


   head.dealresult = result;
   head.para  = (u8 *)(&ExpRecord);
   head.para_len = sizeof(struct SysExceptionInfo);
   head.name = CN_HARDEXP_DECODERNAME;
   head.validflag = true;
   Exp_Throw(&head,&result);

    //recovethe shedule
    g_bScheduleEnable = shedule_bak;

    return result;
}

// =============================================================================
// 函数功能：此部分主要用来处理处理器的通用异常信息和NVIC的部分带有异常信息的寄存器
// 输入参数：layer,抛出层次
//           parahead，抛出时的参数
//           endian, 信息的存储格式
// 输出参数：无
// 返回值  : true成功, false失败
// =============================================================================
bool_t  Exp_HardDecoder(struct tagExpThrowPara *parahead, u32 endian)
{
    bool_t result = false;
    struct SysExceptionInfo  *mycpuinfo;

    if((parahead == NULL) || (false == parahead->validflag))
    {
        //非本层信息，无需解析
        printk("CPU核心硬件没有异常\r\n");
        result = false;
    }
    else
    {
        if((sizeof(struct SysExceptionInfo))== parahead->para_len)//看看包是否完整
        {
            //基本算是完整包，慢慢解析吧
            mycpuinfo = (struct SysExceptionInfo *)(parahead->para);
            //存储端转换
            if(endian != CN_CFG_BYTE_ORDER)
            {
                __SwapExpCpuInfoByEndian(mycpuinfo);
            }
            if(CN_SYS_EXP_CPUINFO_VALIDFLAG == mycpuinfo->SysExpCpuFlag)//当前版本，可以解析
            {
                printk("CPUINFO LIST:\r\n");
                printk("CPUINFO:len = 0x%08x\n\r",parahead->para_len);
                printk("CPUINFO:deal result = 0x%08x\n\r", parahead->dealresult);

                //EXP FAULT 信息解析
                printk("SysExpInfo:Cortex M3/M4 General Registers:\n\r");
                printk("R0:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r0);
                printk("R1:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r1);
                printk("R2:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r2);
                printk("R3:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r3);
                printk("R4:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r4);
                printk("R5:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r5);
                printk("R6:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r6);
                printk("R7:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r7);
                printk("R8:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r8);
                printk("R9:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r9);
                printk("R10:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r10);
                printk("R11:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r11);
                printk("R12:0x%08x\n\r",mycpuinfo->CoreInfo.ger_r12);

                printk("SysExpInfo:Cortex M3/M4 Special Registers:\n\r");
                printk("SP:0x%08x\n\r",mycpuinfo->CoreInfo.spr_sp);
                printk("LR:0x%08x\n\r",mycpuinfo->CoreInfo.spr_lr);
                printk("PC:0x%08x\n\r",mycpuinfo->CoreInfo.spr_pc);
                printk("XPSR:0x%08x\n\r",mycpuinfo->CoreInfo.spr_xpsr);

                printk("SysExpInfo:NVIC Exception Registers:\n\r");
                printk("MFSR:0x%02x\n\r",mycpuinfo->NvicInfo.mfsr);
                printk("BFSR:0x%02x\n\r",mycpuinfo->NvicInfo.bfsr);
                printk("UFSR:0x%04x\n\r",mycpuinfo->NvicInfo.ufsr);
                printk("HFSR:0x%08x\n\r",mycpuinfo->NvicInfo.hfsr);
                printk("DFSR:0x%04x\n\r",mycpuinfo->NvicInfo.dfsr);

                result = true;
            }
            else
            {
                printk("CPU Info Decode:ivalid info flag!\n\r");
                result = false; //可能不是一样的CPU或者版本，无法解析
            }
        }
        else
        {
            printk("CPU Info Decode:incomplete info frame!\n\r");
            result = false;
        }
    }

    return result;
}

// =============================================================================
// 函数功能：异常数据解析函数，异常硬件解码函数的注册,初始化异常模块后调用
// 输入参数：无
// 输出参数：无
// 返回值     :true成功, false失败
// =============================================================================
bool_t Exp_InfoDecoderInit(void)
{
    bool_t result;
    result = Exp_RegisterThrowinfoDecoder(Exp_HardDecoder,
                CN_HARDEXP_DECODERNAME);

    return result;
}

// =============================================================================
// 函数功能：异常初始化,包括异常向量表的初始化
// 输入参数：暂时无（后续待定）
// 输出参数：无
// 返回值     :true成功, false失败
// =============================================================================
void Exp_Init(void)
{
    __Exp_TableSet();
}
