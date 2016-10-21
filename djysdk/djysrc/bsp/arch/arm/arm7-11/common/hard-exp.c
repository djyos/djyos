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
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: cm3异常处理模块
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-08-30
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明:
//------------------------------------------------------
#include "stdint.h"
#include "hard-exp.h"
#include "exp.h"
#include "stdio.h"
#include "endian.h"
#include "cpu_peri.h"
#include "int.h"
#include "djyos.h"
bool_t  HardExp_Decoder(struct ExpThrowPara *parahead, u32 endian);

// =============================================================================
// 函数功能：此部分主要用来处理处理器的通用异常信息和NVIC的部分带有异常信息的寄存器
// 输入参数：layer,抛出层次
//           parahead，抛出时的参数
//           endian, 信息的存储格式
// 输出参数：无
// 返回值  : true成功, false失败
// =============================================================================
bool_t  HardExp_Decoder(struct ExpThrowPara *parahead, u32 endian)
{
    bool_t result = false;
    struct SysExceptionInfo  *mycpuinfo;

    if(parahead == NULL)
    {
        //非本层信息，无需解析
        printk("CPU核心硬件没有异常\r\n");
        result = false;
    }
    else
    {
        if((sizeof(struct SysExceptionInfo))== parahead->ExpInfoLen)//看看包是否完整
        {   /*
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
                printk("CPUINFO:deal result = 0x%08x\n\r", parahead->ExpAction);

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
            */
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
// 函数: 注册硬件异常的数据解析函数, 所有平台均须提供这个函数,供异常模块调用,否则
//       将导致异常模块编译不通过.
// 参数: 无
// 返回值     :true成功, false失败
// =============================================================================
bool_t HardExp_InfoDecoderInit(void)
{
    static struct ExpInfoDecoder HardExpDecoder;
    bool_t result;
    HardExpDecoder.MyDecoder = HardExp_Decoder;
    HardExpDecoder.DecoderName = CN_HARDEXP_DECODERNAME;
    result = Exp_RegisterThrowInfoDecoder(&HardExpDecoder);

    return result;
}

// =============================================================================
// 函数功能：异常初始化,包括异常向量表的初始化
// 输入参数：暂时无（后续待定）
// 输出参数：无
// 返回值  : true成功, false失败
// =============================================================================
void HardExp_Init(void)
{
    //__Exp_TableSet();
}
