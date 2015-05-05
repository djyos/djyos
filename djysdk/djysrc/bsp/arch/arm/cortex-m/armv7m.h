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
// 这份授权条款，在使用者符合以下二条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、此三条件表列，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、此三条件表列，以及下述
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
//所属模块: bsp
//作者：罗侍田
//版本：V1.0.0
//文件描述: armv7m的公共定义
//其他说明:
//修订历史:
//2. ...
//1. 日期  : 2014/12/19
//   修改人: 罗侍田
//   新版本号:
//   修改说明: 新建
//------------------------------------------------------

#ifndef __ARMV7M_H__
#define __ARMV7M_H__
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STIR_reg_addr   0xe000ef00
extern struct scb_reg volatile * const pg_scb_reg;


//定义scb_reg.ICSR 中断控制及状态寄存器?0xE000_ED04 ,bm_为位掩码?bo_为位偏移
#define bm_scb_icsr_nmipendset  (1<<31)     //rw 写1挂起nmi
#define bm_scb_icsr_pendsvset   (1<<28)     //rw 写1挂起pendsv?读返回挂起状态
#define bm_scb_icsr_pendsvclr   (1<<27)     //w 写1清除pendsv挂起状态
#define bm_scb_icsr_pendstset   (1<<26)     //rw 写1挂起systick?读返回挂起状态
#define bm_scb_icsr_pendstclr   (1<<25)     //w 写1清除systick挂起状态
#define bm_scb_icsr_isrpreempt  (1<<23)     //r 调试用
#define bm_scb_icsr_isrpending  (1<<22)     //r 有中断挂起?不含nmi?
#define bm_scb_icsr_vectpending (0x3ff<<12) //r 挂起的中断中优先级最高的中断号
#define bm_scb_icsr_rettobase   (1<<11)     //r 1=执行异常服务程序且没有嵌套。
                                            //  0=主程序中或嵌套异常服务中
#define bm_scb_icsr_vectactive  (0xff<<0)   //r 正在服务的异常号
#define bo_scb_icsr_nmipendset  31
#define bo_scb_icsr_pendsvset   28
#define bo_scb_icsr_pendsvclr   27
#define bo_scb_icsr_pendstset   26
#define bo_scb_icsr_pendstclr   25
#define bo_scb_icsr_isrpreempt  23
#define bo_scb_icsr_isrpending  22
#define bo_scb_icsr_vectpending 12
#define bo_scb_icsr_rettobase   11
#define bo_scb_icsr_vectactive  0

//AIRCR 应用程序中断及复位控制寄存器?0xE000_ED0c ,bm_为位掩码?bo_为位偏移
#define bm_scb_aircr_vectkey        (0xffff<<16)
#define bm_scb_aircr_endianess      (1<<15)
#define bm_scb_aircr_prigroup       (7<<8)
#define bm_scb_aircr_sysresetreq    (1<<2)
#define bm_scb_aircr_vectclractive  (1<<1)
#define bm_scb_aircr_vectreset      (1<<0)
#define bo_scb_aircr_vectkey        16  //rw 写本寄存器?必须同时对此域写0x05fa
#define bo_scb_aircr_endianess      15  //r 1=大端?0=小端
#define bo_scb_aircr_prigroup       8   //rw 本域+1=子优先级占用位数
#define bo_scb_aircr_sysresetreq    2   //w 写1逻辑复位芯片
#define bo_scb_aircr_vectclractive  1   //w 写1清除所有异常活动状态信息。
#define bo_scb_aircr_vectreset      0   //w 写1复位cm3内核?不影响内核以外电路

//定义scb_reg.SCR 系统控制寄存器,0xE000_ED10 ,bm_为位掩码?bo_为位偏移
#define bm_scb_scr_sevonpend    (1<<4)
#define bm_scb_scr_sleepdeep    (1<<2)
#define bm_scb_scr_sleeponexit  (1<<1)
#define bo_scb_scr_sevonpend    4      //rw 1=任何中断和事件都可以唤醒处理器
                                       //   0=只有使能的中断和事件可以唤醒处理器
#define bo_scb_scr_sleepdeep    2      //rw 0=普通睡眠?1=深度睡眠
#define bo_scb_scr_sleeponexit  1      //rw 1=从中断返回时进入sleep

//定义scb_reg.CCR 配置与控制寄存器,0xE000_ED14 ,bm_为位掩码?bo_为位偏移
#define bm_scb_ccr_stkalign         (1<<9)
#define bm_scb_ccr_bfhfnmign        (1<<8)
#define bm_scb_ccr_div0trp          (1<<4)
#define bm_scb_ccr_unaligntrp       (1<<3)
#define bm_scb_ccr_usersetmpend     (1<<1)
#define bm_scb_ccr_nonbasethrdena   (1<<0)
#define bo_scb_ccr_stkalign         9   //rw 异常响应自动入栈时强制dp双字对齐
#define bo_scb_ccr_bfhfnmign        8   //rw 1=硬fault和nmi服务中忽略数据总线fault
#define bo_scb_ccr_div0trp          4   //rw 1=使能除o用法fault
#define bo_scb_ccr_unaligntrp       3   //rw 1=未对齐访问将掉进用法fault
#define bo_scb_ccr_usersetmpend     1   //rw 1=允许用户代码设置stir
#define bo_scb_ccr_nonbasethrdena   0   //rw,写1允许异服程序修改exc_return进入线程模式

//定义scb_reg.SHCSR位掩码?0xE000ED24,bm_为位掩码?bo_为位偏移
#define bm_scb_shcsr_usgfaultena    (1<<18)
#define bm_scb_shcsr_busfaultena    (1<<17)
#define bm_scb_shcsr_memfaultena    (1<<16)
#define bm_scb_shcsr_svcallpended   (1<<15)
#define bm_scb_shcsr_busfaultpended (1<<14)
#define bm_scb_shcsr_memfaultpended (1<<13)
#define bm_scb_shcsr_usgfaultpended (1<<12)
#define bm_scb_shcsr_systickact     (1<<11)
#define bm_scb_shcsr_pendsvact      (1<<10)
#define bm_scb_shcsr_monitoract     (1<<8)
#define bm_scb_shcsr_svcallact      (1<<7)
#define bm_scb_shcsr_usgfaultact    (1<<3)
#define bm_scb_shcsr_busfaultact    (1<<1)
#define bm_scb_shcsr_memfaultact    (1<<0)
#define bo_scb_shcsr_usgfaultena    18  //rw,b,1 = 使能用法 fault。
#define bo_scb_shcsr_busfaultena    17  //rw,b,1 = 使能总线 fault。
#define bo_scb_shcsr_memfaultena    16  //rw,b,1 = 使能存储器管理 fault。
#define bo_scb_shcsr_svcallpended   15  //rw,b,1 = SVC 悬起中。
#define bo_scb_shcsr_busfaultpended 14  //rw,b,1 = 总线 fault 悬起中
#define bo_scb_shcsr_memfaultpended 13  //rw,b,1 = 存储器管理 fault 悬起中
#define bo_scb_shcsr_usgfaultpended 12  //rw,b,1 = 用法 fault 悬起中
#define bo_scb_shcsr_systickact     11  //rw,b,1 = SysTick 异常活动中
#define bo_scb_shcsr_pendsvact      10  //rw,b,1 = PendSV 异常活动中
#define bo_scb_shcsr_monitoract     8   //rw,b,1 = Devug Monitor 异常活动中
#define bo_scb_shcsr_svcallact      7   //rw,b,1= SVC 异常活动中
#define bo_scb_shcsr_usgfaultact    3   //rw,b,1= 用法 fault 异常活动中
#define bo_scb_shcsr_busfaultact    1   //rw,b,1 = 总线 fault 异常活动中
#define bo_scb_shcsr_memfaultact    0   //rw,b,1 = 存储器管理 fault 异常活动中

//MFSR 存储器管理 fault 状态寄存器,0xE000_ED28 ,bm_为位掩码?bo_为位偏移
#define bm_scb_mfsr_MMARVALID   (1<<7)
#define bm_scb_mfsr_STKERR      (1<<4)
#define bm_scb_mfsr_UNSTKERR    (1<<3)
#define bm_scb_mfsr_DACCVIOL    (1<<1)
#define bm_scb_mfsr_IACCVIOL    (1<<0)
#define bo_scb_mfsr_MMARVALID   7      //r,存储器管理fault地址寄存器?MMAR?有效
#define bo_scb_mfsr_STKERR      4      //r/Wc,入栈时发生错误
#define bo_scb_mfsr_UNSTKERR    3      //r/Wc,出栈时发生错误
#define bo_scb_mfsr_DACCVIOL    1      //r/Wc,数据访问违例
#define bo_scb_mfsr_IACCVIOL    0      //r/Wc,取址访问违例

//定义scb_reg.BFSR 总线 fault 状态寄存器,0xE000_ED29 ,bm_为位掩码?bo_为位偏移
#define bm_scb_bfsr_BFARVALID   (1<<7)
#define bm_scb_bfsr_STKERR      (1<<4)
#define bm_scb_bfsr_UNSTKERR    (1<<3)
#define bm_scb_bfsr_IMPREISERR  (1<<2)
#define bm_scb_bfsr_PRECISERR   (1<<1)
#define bm_scb_bfsr_IACCVIOL    (1<<0)
#define bo_scb_bfsr_BFARVALID   7    //r,总线fault地址寄存器?BFAR?有效
#define bo_scb_bfsr_STKERR      4    //r/Wc,入栈时发生错误
#define bo_scb_bfsr_UNSTKERR    3    //r/Wc,出栈时发生错误
#define bo_scb_bfsr_IMPREISERR  2    //r/Wc,不精确的数据访问违例?violation?
#define bo_scb_bfsr_PRECISERR   1    //r/Wc,精确的数据访问违例?violation?
#define bo_scb_bfsr_IACCVIOL    0    //r/Wc,取址访问违例

//UFSR 用法 fault 状态寄存器?地址?0xE000_ED2A ,bm_为位掩码?bo_为位偏移
#define bm_scb_ufsr_DIVBYZERO   (1<<9)
#define bm_scb_ufsr_UNALIGNED   (1<<8)
#define bm_scb_ufsr_NOCP        (1<<3)
#define bm_scb_ufsr_INVPC       (1<<2)
#define bm_scb_ufsr_INVSTATE    (1<<1)
#define bm_scb_ufsr_UNDEFINSTR  (1<<0)
#define bo_scb_ufsr_DIVBYZERO   9   //r/Wc,表示除法运算时除数为零
#define bo_scb_ufsr_UNALIGNED   8   //r/Wc,未对齐访问导致的 fault?
#define bo_scb_ufsr_NOCP        3   //r/Wc,试图执行协处理器相关指令?
#define bo_scb_ufsr_INVPC       2   //r/Wc,在异常返回时EXC_RETURN非法
#define bo_scb_ufsr_INVSTATE    1   //r/Wc,试图切入 ARM 状态
#define bo_scb_ufsr_UNDEFINSTR  0   //r/Wc,执行的指令其编码是未定义的

//定义scb_reg.HFSR 硬 fault 状态寄存器  0xE000_ED2C ,bm_为位掩码?bo_为位偏移
#define bm_scb_hfsr_DEBUGEVT    (1<<31)
#define bm_scb_hfsr_FORCED      (1<<30)
#define bm_scb_hfsr_VECTBL      (1<<1)
#define bo_scb_hfsr_DEBUGEVT    31  //r/Wc,因调试事件而产生
#define bo_scb_hfsr_FORCED      30  //r/Wc,是总线 fault?存储器管理
                                    //    fault 或是用法 fault 上访的结果
#define bo_scb_hfsr_VECTBL      1    //r/Wc,在取向量时发生的
struct scb_reg
{
    vuc32 CPUID;
    vu32 ICSR;          //中断控制及状态寄存器?0xE000_ED04
    vu32 VTOR;          //向量表偏移量寄存器?0xE000_ED08
    vu32 AIRCR;         //应用程序中断及复位控制寄存器?0xE000_ED0c
    vu32 SCR;           //系统控制寄存器,0xE000_ED10
    vu32 CCR;           //配置与控制寄存器,0xE000_ED14
    vu8 mmf_pri4;       //存储器管理 fault 的优先级,0xE000_ED18
    vu8 bus_pri5;       //总线 fault 的优先级,0xE000_ED19
    vu8 usr_pri6;       //用法 fault 的优先级,0xE000_ED1a
    vu8 res1[4];
    vu8 svc_pri11;      //svc 异常的优先级,0xE000_ED1f
    vu8 dbg_pri12;      //调试监视器的优先级,0xE000_ED20
    vu8 res2[1];
    vu8 pendsv_pri14;   //pendsv 异常的优先级,0xE000_ED22
    vu8 systick_pri15;  //systick的优先级,0xE000_ED23
    vu32 SHCSR;
    vu8 MFSR;           //存储器管理 fault 状态寄存器(MFSR),0xE000_ED28
    vu8 BFSR;           //总线 fault 状态寄存器(MFSR),0xE000_ED29
    vu16 UFSR;          //用法 fault 状态寄存器(UFSR)?地址?0xE000_ED2A
    vu32 HFSR;          //硬 fault 状态寄存器      0xE000_ED2C
    vu32 DFSR;          //调试 fault 状态寄存器(DFSR)    0xE000_ED30
    vu32 MMFAR;         //发生存储管理fault的地址寄存器(MMAR)    0xE000_ED34
    vu32 BFAR;          //发生总线fault的地址寄存器(MMAR)    0xE000_ED38
    vu32 AFAR;          //由芯片制造商决定?可选?                    0xE000_ED3C
    //以下定义?只有M4以上才有
    vu32 PFR[2];        //Processor Feature Register            0xE000_ED40
    vu32 DFR;           //Debug Feature Register                0xE000_ED48
    vu32 ADR;           //Auxiliary Feature Register            0xE000_ED4C
    vu32 MMFR[4];       //Memory Model Feature Register         0xE000_ED50
    vu32 ISAR[5];       //Instruction Set Attributes Register   0xE000_ED60
    vu32 RESERVED0[5];
    vu32 CPACR;         //FPU内核访问控制寄存器(CPACR)              0xE000_ED88
    vu32 RESERVED1[106];
    vu32 FPCCR;         //FPU内核上下文控制寄存器(CPCCR)            0xE000_EF34
    union
    {
        vu32 FPCAR;         //FPU内核上下文地址寄存器(FPCAR)        0xE000_EF38
        vu32 FPSCR;         //FPU内核状态控制寄存器(FPSCR)          0xE000_EF38
    };
    vu32 FPDSCR;        //FPU内核默认状态控制寄存器(FPDSCR)          0xE000_EF3C
};

#ifdef __cplusplus
}
#endif

#endif /*__ARMV7M_H__*/


