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
// 模块描述:基于P1020的DJYOS的中断控制器实现（中断底层操作）
// 模块版本: V1.00(初始化版本)
// 创建人员:zhangqf_cyg
// 创建时间:2013-7-26--上午10:15:29
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注：该文件实现的对外接口函数都是移植敏感的
//       下面条例是各个API都通用的，对违反该规则的调用概不负责，各个API不再重复说明：
//     1，当操作中断号时，务必保证该中断号是在使用的范围之类的，不然的话很可能引起异常
//
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "os.h"
#include "Pic.h"
#include "cpu.h"
#include "int_hard.h"

extern const ufast_t tg_IntUsed[];          //save the int number used
extern const ufast_t tg_IntUsedNum;         //本变量在int_config.c中定义
extern struct IntLine *tg_pIntLineTable[];       //中断线查找表
struct IntMasterCtrl  tg_int_global;     //定义并初始化总中断控制结构
//P1020的PIC实在是过于复杂，并且中断的控制器寄存器位置不是很规律，特此开辟
//数组来存放其地址
static u32 sgPicIvprAddrTab[CN_INT_LINE_LAST+1]; //IVPR地址
static u32 sgPicIdrAddrTab[CN_INT_LINE_LAST+1];  //IDR地址

// =============================================================================
// 函数功能：__Int_ClearAllLine
//          清除所有挂起的中断
// 输入参数：无
// 输出参数：无
// 返回值     ：true成功  false失败
// 说明          ：
// =============================================================================
void __Int_ClearAllLine(void)
{
    //may be we could use the eoi to do some thing,another problem is that you should
    //clear the int signal as well(IACK)
    u32 irq_number;
    u32 frr;
    u32 frrvalue;
    u32 i;
    frr = cn_pic_frr_reg;
    frrvalue = read32(frr);
    irq_number = (frrvalue >> cn_pic_frr_nirq_shift) & cn_pic_frr_nirq_mask;
    for (i =0; i< irq_number; i++)
    {
        //读IACK 写EOI会让所有挂起的中断都灰飞烟灭
        read32(cn_core_iack_addr);
        write32(cn_int_eoi_addr, 0);
    }
    return;
}
// =============================================================================
// 函数功能：__Int_GetIvpr
//          获取对应中断号的IVPR地址
// 输入参数：
//          ufl_line,中断号
// 输出参数：addr,存储IVPR的地址
// 返回值     ：true成功  false失败
// 说明          ：当还没有获取中断寄存器地址时采用该方法，当已经初始化中断向量地址后不再
//           使用该方法，主要用在中断初始化当中
// =============================================================================
bool_t __Int_GetIvpr(ufast_t ufl_line,u32 *addr)
{
    ufast_t   int_offset;//相对于该中断类型（片上中断、片外中断等）的偏移量
    u32  ivpr_addr = 0;//寄存器地址

    if(ufl_line < cn_int_line_msgbase)//external interrupt
    {
        int_offset = ufl_line - cn_int_line_extbase;
        ivpr_addr = (u32)(cn_int_ext_baddr + int_offset*2*cn_int_register_step);
    }
    else if(ufl_line < cn_int_line_smsgbase)//msg
    {
        int_offset = ufl_line - cn_int_line_msgbase;
        ivpr_addr = (u32)(cn_int_msg_baddr + int_offset*2*cn_int_register_step);
    }
    else if(ufl_line < cn_int_line_ipibase0)//share msg
    {
        int_offset = ufl_line - cn_int_line_smsgbase;
        ivpr_addr = (u32)(cn_int_smsg_baddr + int_offset*cn_int_register_step);
    }
    else if(ufl_line < cn_int_line_ipibase1)//ipi0
    {
        int_offset = ufl_line - cn_int_line_ipibase0;
        ivpr_addr = (u32)(cn_int_ipi_vp_baddr+ int_offset*cn_int_register_step);
    }
    else if(ufl_line < cn_int_line_gbtimer_abase)//ipi1,use the same pv register
    {
        int_offset = ufl_line - cn_int_line_ipibase1;
        ivpr_addr =(u32)(cn_int_ipi_vp_baddr+ int_offset*cn_int_register_step);
    }
    else if(ufl_line < cn_int_line_gbtimer_bbase)//gbtimer_groupa
    {
        int_offset = ufl_line - cn_int_line_gbtimer_abase;
        ivpr_addr = (u32)(cn_int_gbtimer_ga_baddr+ int_offset*4*cn_int_register_step);
    }
    else if(ufl_line < cn_int_line_interbase)//gbtimer_groupb
    {
        int_offset = ufl_line - cn_int_line_gbtimer_bbase;
        ivpr_addr = (u32)(cn_int_gbtimer_gb_baddr+ int_offset*4*cn_int_register_step);
    }

    else if(ufl_line <= CN_INT_LINE_LAST)//interal interrupt
    {
        int_offset = ufl_line - cn_int_line_interbase;
        ivpr_addr = (u32)(cn_int_interal_baddr + int_offset*2*cn_int_register_step);
    }
    else
    {
        ivpr_addr = 0;//invalid uf_line;
    }
    if(0 != ivpr_addr)
    {
        *addr = ivpr_addr;
        return true;
    }
    else
    {
        return false;
    }
}
// =============================================================================
// 函数功能：__Int_RstPic
//          复位PIC控制器
// 输入参数：无
// 输出参数：无
// 返回值     ：无
// 说明          ：直到PIC重启后才返回
// =============================================================================
void __Int_RstPic(void)
{
    u32 value;
    u32 addr;

    addr = cn_pic_gcr;
    write32(addr,cn_pic_gcr_rst);
    value = read32(addr);
    while(value & cn_pic_gcr_rst)//重启成功
    {
        value = read32(addr);
    }
    return;
}
// =============================================================================
// 函数功能：int_init
//          中断初始化
// 输入参数：无
// 输出参数：无
// 返回值     ：true成功  false失败
// 说明          ： 1.初始化中断硬件,初始化中断线数据结构
//           2.异步信号保持禁止,它会在线程启动引擎中打开.
//           3.总中断允许
//           4.初始化了所有的中断（硬件），但是只有使用的中断才会有控制结构体
//            用户初始化过程应该遵守如下规则:
//           1.系统开始时就已经禁止所有异步信号,用户初始化时无须担心异步信号发生.
//           2.初始化过程中如果需要操作总中断/实时中断/异步信号,应该成对使用.禁止使
//             异步信号实际处于允许状态(即异步和总中断开关同时允许).
//           3.可以操作中断线,比如连接、允许、禁止等,但应该成对使用.
//           4.建议使用save/restore函数对,不要使用enable/disable函数对.
// =============================================================================
void Int_Init(void)
{
    bool_t result_bool;
    ufast_t i;
    ufast_t ufl_line;
    u32 value;
    u32 addr;
    u32 len ;
    u8 *src;

    Int_CutTrunk();//close the main int key
    __Int_RstPic();
    //初始化查找表
    for(i=0;i <= CN_INT_LINE_LAST;i++)
    {
        tg_pIntLineTable[i] = NULL;
    }

    //将所有的中断进行初始化，记录其寄存器地址
    for(ufl_line=0;ufl_line <= CN_INT_LINE_LAST;ufl_line++)
    {
        result_bool =  __Int_GetIvpr(ufl_line, &addr);
        if(result_bool)
        {
            sgPicIvprAddrTab[ufl_line] = addr;
            sgPicIdrAddrTab[ufl_line] = addr + cn_pic_ivpr2idr;
            //设置IVPR,只需要设置VECTOR，这样就会让CTPR=0，MSK=0，LEVE=0
            value = ufl_line;
            value = value & cn_ivpr_vector_msk;
            write32(addr,value );
            //设置IDR,都设置为INT0
            addr = addr + cn_pic_ivpr2idr;
            value = cn_pic_idr_p0;
            write32(addr,value);
        }
    }
    for(ufl_line=0; ufl_line < CN_INT_BITS_WORDS; ufl_line++)
    {
        //属性位图清零,全部置为异步信号方式
        tg_int_global.property_bitmap[ufl_line] = 0;
        //中断使能位图清0,全部处于禁止状态
        tg_int_global.enable_bitmap[ufl_line] = 0;
    }

    tg_int_global.nest_asyn_signal =0;
    tg_int_global.nest_real=0;

    tg_int_global.en_asyn_signal_counter = 1;   //异步信号计数
    __Int_ClearAllLine();//clear all the suspend interrupt

    Int_CutAsynSignal();
    //set the cpu ctpr to zero
    write32(cn_core_ctpr_addr,cn_prior_core_event);
//  //初始化所有的定时器
//  extern void module_init_pictimer(void);
//  module_init_pictimer();
    //set the spuriouse vector
    write32(cn_pic_svr, cn_pic_svr_vector);
    //set the pic to the mixed mode
    write32(cn_pic_gcr_addr,cn_pic_gcr_mode_m);
//    tg_int_global.en_trunk = true;
    tg_int_global.en_trunk_counter = 0;       //总中断计数
    Int_ContactTrunk(); //接通总中断开关
}


bool_t __Int_ChkIntValid(ufast_t ufl_line)
{
    bool_t result = false;
    if(ufl_line <= CN_INT_LINE_LAST)
    {
        result = true;
    }
    return result;
}
// =============================================================================
// 函数功能：int_set_line_triger_type
//          设置中断的触发方式
// 输入参数：ufl_line,中断号
//           mode,见enum_int_trigertype中的枚举定义
// 输出参数：无
// 返回值     ：
// 说明          ：无效中断号或者没有使用的中断或者没有初始化的中断都将会不成功，甚至异常
// =============================================================================
bool_t Int_SetLineTrigerType(ufast_t ufl_line, ufast_t mode)
{
    bool_t result =false;
    u32 addr;
    u32 value;
    if( __Int_ChkIntValid(ufl_line) && (mode <=EN_INT_TRIGER_TAP))
    {
        addr = sgPicIvprAddrTab[ufl_line];
        value = read32(addr);
        value = value & (~cn_ivpr_trigertype_msk);
        result = true;
        switch (mode)
        {
            case EN_INT_TRIGER_HIGHLEVEL:
                value = value |(cn_ivpr_sense_msk | cn_ivpr_polarity_msk);
                write32(addr,value);
                break;
            case EN_INT_TRIGER_LOWLEVEL:
                value = value | cn_ivpr_sense_msk ;
                write32(addr,value);
                break;
            case EN_INT_TRIGER_RISINGEDGE:
                value = value | cn_ivpr_polarity_msk ;
                write32(addr,value);
                break;
            case EN_INT_TRIGER_FALLINGEDGE:
                write32(addr,value);
                break;
            case EN_INT_TRIGER_TAP:
            default:
                result = false;
                break;
        }
    }

    return result;
}

// =============================================================================
// 函数功能：int_contact_line
//          使能中断
// 输入参数：ufl_line，中断号
// 输出参数：无
// 返回值     ：true成功  false失败
// 说明          ：中断的使能与屏蔽采用的是对其优先级的操作
//          如果直接使用MSK，则会造成在关闭中断的过程中的中断丢失，不符合OS需求
//          直接回复其中断的PRIOR即可
// =============================================================================
bool_t  Int_ContactLine(ufast_t ufl_line)
{
    bool_t result = false;
    u32 addr;
    u32 value = 0;
    u32 prior = 0;

    if(__Int_ChkIntValid(ufl_line))
    {
        addr =sgPicIvprAddrTab[ufl_line];
        value = read32(addr);
        //异步信号和实时中断的关中断方式不一样,异步信号用prior控制，实时中断用MSK
        if(tg_pIntLineTable[ufl_line]->int_type == CN_ASYN_SIGNAL)
        {
            value = value &(~cn_ivpr_prior_msk);
            prior = tg_pIntLineTable[ufl_line]->prio<<16;
            prior = prior&cn_ivpr_prior_msk;
            value = value|prior;
            value = value &(~cn_ivpr_en_msk); //alse must clear the msk
        }
        else
        {
            value = value &(~cn_ivpr_en_msk);
        }
        write32(addr, value);
        result = true;
    }

    return result;
}

// =============================================================================
// 函数功能：Int_CutLine
//          屏蔽中断
// 输入参数：ufl_line，中断号
// 输出参数：无
// 返回值     ：true成功  false失败
// 说明          ：关闭中断源，采用降低中断源的优先级的策略
//          如果直接使用MSK，则会造成在关闭中断的过程中的中断丢失，不符合OS需求
//          将其PRIOE设置为0即可
// =============================================================================
bool_t  Int_CutLine(ufast_t ufl_line)
{
    bool_t result = false;
    u32 addr;
    u32 value = 0;

    if(__Int_ChkIntValid(ufl_line))
    {
        addr =sgPicIvprAddrTab[ufl_line];
        value = read32(addr);
        //异步信号和实时中断的关中断方式不一样,异步信号用prior控制，实时中断用MSK
        if(tg_pIntLineTable[ufl_line]->int_type == CN_ASYN_SIGNAL)
        {
            value = value &(~cn_ivpr_prior_msk);
        }
        else
        {
            value = value |cn_ivpr_en_msk;
        }
        write32(addr, value);
        result = true;
    }

    return result;
}
// =============================================================================
// 函数功能：Int_GetEIntNumber 获取异步信号号码
// 输入参数：
// 输出参数：
// 返回值    ：中断号，正在响应的中断的号码
// 说明       ：本函数是移植敏感函数
//          在E500V2系列处理器中，该寄存器智能被读取之后自动复位;如果不是在中断状态读取该
//          寄存器，读取的将会是无效值
// =============================================================================
ufast_t  Int_GetEIntNumber(void)
{
    u32       addr;
    ufast_t   result;

    addr = cn_core_iack_addr;
    result = read32(addr);
    return result;
}
// =============================================================================
// 函数功能：Int_GetCIntNumber  获取实时信号号码
//          清除中断
// 输入参数：
// 输出参数：
// 返回值    ：中断号，正在响应的中断的号码
// 说明       ：本函数是移植敏感函数
//          在E500V2系列处理器中，该寄存器智能被读取之后自动复位;如果不是在中断状态读取该
//          寄存器，读取的将会是无效值
// =============================================================================
ufast_t  Int_GetCIntNumber(void)
{
    u32       addr;
    ufast_t   result;

    addr = cn_core_iack_addr;
    result = read32(addr);
    return result;
}

// =============================================================================
// 函数功能：Int_ClearLine
//          清除中断
// 输入参数：ufl_line，中断号
// 输出参数：无
// 返回值     ：true成功  false失败
// 说明          ：硬件应该有相应的功能，提供清除中断挂起的操作，清除前，不能响应同一中断线
//          的后续中断，清除后，才可以响应后续中断。本函数与该中断线被设置为实时中断
//          还是异步信号无关.
//备注          ：P1020中，出现supriouse中断号时无需写EOI
//          本函数是移植敏感函数
// =============================================================================
bool_t Int_ClearLine(ufast_t ufl_line)
{
    bool_t result = false;
    u32 value;
    u32 addr;

    if(__Int_ChkIntValid(ufl_line))
    {
       if(tg_pIntLineTable[ufl_line]->int_type == CN_ASYN_SIGNAL)
       {
           addr = sgPicIvprAddrTab[ufl_line];
           value = read32(addr);
           if(value&cn_ivpr_activity_msk) //在挂起状态
           {
               write32(cn_int_eoi_addr, 0);
           }
       }
       result = true;
    }

    return result;
}

// =============================================================================
// 函数功能：Int_TapLine
//          触发中断
// 输入参数：ufl_line，中断号
// 输出参数：无
// 返回值     ：true成功  false失败
// 说明          ：移植敏感，在P1020上总是返回失败（未实现）
// =============================================================================
bool_t Int_TapLine(ufast_t ufl_line)
{
    return false;
}
// =============================================================================
// 函数功能：int_query_line
//           查询中断状态，若挂起则清除
// 输入参数：ufl_line,指定中断号
// 输出参数：无
// 返回值     ：true该中断在触发状态   否则false
// 说明          ：移植敏感函数,
//          查询并清除相应中断线状态，可用于查询式中断程序
// 备注         : 与硬件结构相关,有些结构可能不提供这个功能,慎用!
// =============================================================================
bool_t Int_QueryLine(ufast_t ufl_line)
{
   bool_t result = false;
   u32 addr;
   u32 value;

   if(__Int_ChkIntValid(ufl_line))
   {
       addr = sgPicIvprAddrTab[ufl_line];
       value = read32(addr);
       result = false;
       while(value&cn_ivpr_activity_msk)
       {
           value = read32(cn_core_iack_addr);
           write32(cn_int_eoi_addr, 0);
           result = true;
       }
   }
   return result;
}

// =============================================================================
// 函数功能：Int_SettoAsynSignal
//           将指定的中断设置为异步信号
// 输入参数：ufl_line,指定中断号
// 输出参数：无
// 返回值     ：true设置成功， false 设置失败
// 说明          ：
// =============================================================================
bool_t Int_SettoAsynSignal(ufast_t ufl_line)
{
    bool_t result = false;
    u32 addr;
    u32 value;
    atom_high_t  high_atom;

    if(__Int_ChkIntValid(ufl_line))
    {
        high_atom = Int_HighAtomStart();
        tg_pIntLineTable[ufl_line]->int_type = CN_ASYN_SIGNAL;
        tg_pIntLineTable[ufl_line]->enable_nest = false;
        tg_pIntLineTable[ufl_line]->prio = cn_prior_asyn_signal;
        tg_int_global.property_bitmap[ufl_line/CN_CPU_BITS]
                        &= ~(1<<(ufl_line % CN_CPU_BITS));   //设置位图
        //set the ivpr,the prior trigger type and int vector could not be set
        //set the idr
        addr = sgPicIdrAddrTab[ufl_line];
        value = read32(addr);
        value &= (~CN_PIC_IDR_C0);
        value |= cn_pic_idr_p0;
        write32(addr,value);

        Int_HighAtomEnd(high_atom);
        result = true;
    }
    return result;
}
// =============================================================================
// 函数功能：Int_SettoReal
//           将指定的中断设置为实时信号
// 输入参数：ufl_line,指定中断号
// 输出参数：无
// 返回值     ：true设置成功， false 设置失败
// 说明          ：设置触发CINIT即可
// =============================================================================
bool_t Int_SettoReal(ufast_t ufl_line)
{
    bool_t result = false;
    u32 addr;
    u32 value;
    atom_high_t  high_atom;

    if(__Int_ChkIntValid(ufl_line))
    {
        high_atom = Int_HighAtomStart();

        if(tg_pIntLineTable[ufl_line]->sync_event != NULL)
        {
            result = false;//有线程在等待这个中断，不能设为实时中断
        }
        else
        {
            tg_pIntLineTable[ufl_line]->int_type = CN_REAL;    //中断线类型
            tg_pIntLineTable[ufl_line]->enable_nest = true;
            tg_pIntLineTable[ufl_line]->prio = cn_prior_real_int_default;
            tg_int_global.property_bitmap[ufl_line/CN_CPU_BITS]
                    |= 1<<(ufl_line % CN_CPU_BITS);   //设置位图
            //set the ivpr
            addr = sgPicIvprAddrTab[ufl_line];
            value = read32(addr);
            value = value&cn_ivpr_trigertype_msk;  //save the trigger type
            value |= (cn_prior_asyn_signal<<16);
            write32(addr,value);
            //set the idr
            addr = sgPicIdrAddrTab[ufl_line];
            value = read32(addr);
            value &= (~cn_pic_idr_p0);
            value |= CN_PIC_IDR_C0;
            write32(addr,value);

            result = true;
        }
        Int_HighAtomEnd(high_atom);
    }

    return result;
}
// =============================================================================
// 函数功能：int_enable_nest
//          设置指定的中断为嵌套模式
// 输入参数：ufl_line,指定中断号
// 输出参数：无
// 返回值     ：true设置成功， false 设置失败
// 说明          ：只处理实时信号--P1020
// =============================================================================
bool_t Int_EnableNest(ufast_t ufl_line)
{
    bool_t result = false;
    if(__Int_ChkIntValid(ufl_line))
    {
        if(tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
        {
            tg_pIntLineTable[ufl_line]->enable_nest = true;
            result = true;
        }
    }
    return result;
}
// =============================================================================
// 函数功能：Int_DisableNest
//          设置指定的中断为非嵌套模式
// 输入参数：ufl_line,指定中断号
// 输出参数：无
// 返回值     ：true设置成功， false 设置失败
// 说明          ：只处理实时信号--P1020
// =============================================================================
bool_t Int_DisableNest(ufast_t ufl_line)
{
    bool_t result = false;
    if(__Int_ChkIntValid(ufl_line))
    {
        if(tg_pIntLineTable[ufl_line]->int_type == CN_REAL)
        {
            tg_pIntLineTable[ufl_line]->enable_nest = false;
            result = true;
        }

    }

    return result;
}


// =============================================================================
// 函数功能：int_set_prio
//          设置指定的中断的优先级
// 输入参数：ufl_line,指定中断号
//          prior,指定优先级
// 输出参数：无
// 返回值     ：true设置成功， false 设置失败
// 说明          ：只处理实时信号--P1020
// =============================================================================
bool_t Int_SetPrio(ufast_t ufl_line,u32 prior)
{
    bool_t result = false;
    u32 addr;
    u32 value;
    u32 myprior;
    atom_high_t  high_atom;

    if(__Int_ChkIntValid(ufl_line))
    {
         high_atom = Int_HighAtomStart();

         tg_pIntLineTable[ufl_line]->prio = prior;
         //看其原来的状态，修改后继续维持原来的状态
         addr = sgPicIvprAddrTab[ufl_line];
         value = read32(addr);
         myprior = (value&cn_ivpr_prior_msk)>>16;
         if(myprior != cn_prior_core_event)//原来打开状态
         {
             value = value |((prior<<16)& cn_ivpr_prior_msk);
             write32(addr, value);
         }
         result = true;

         Int_HighAtomEnd(high_atom);
    }

    return result;
}

static atom_high_t  sgTrunkKey;
// =============================================================================
// 函数功能：int_contact_trunk
//          使能总中断
// 输入参数：无
// 输出参数：无
// 返回值     ：
// 说明          ：使用MSR的EEbit,因为实时信号和异步信号都使用的是EE
// =============================================================================
void Int_ContactTrunk(void)
{
    Int_HighAtomEnd(sgTrunkKey);
}

// =============================================================================
// 函数功能：int_cut_trunk
//          关闭总中断
// 输入参数：无
// 输出参数：无
// 返回值     ：
// 说明          ：使用MSR的EEbit,因为实时信号和异步信号都使用的是EE
// =============================================================================
void Int_CutTrunk(void)
{
    sgTrunkKey = Int_HighAtomStart();
}



