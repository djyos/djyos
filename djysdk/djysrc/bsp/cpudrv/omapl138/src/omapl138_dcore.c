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
//所属模块:cpu双核控制
//作者：网络
//版本：V1.0.0
//文件描述:双核控制代码
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include "stdint.h"
#include "cpu_peri.h"

// =========================================================================
// 函数功能: 加载 Ti 编译器所转换的bin格式二进制代码，调用前，须先把bin文件读入
//      到内存中。bin文件的编码格式:
// 入口地址，4bytes
// section 1 size，4bytes
// section 1 dest，4bytes
// section 1 data，若干数据

// section 2 size，4bytes
// section 2 dest，4bytes
// section 2 data，若干数据

//……

// section n size，4bytes
// section n dest，4bytes
// section n data，若干数据
// 00000000      ，4bytes
// 函数参数: code_buf，bin文件存放地址，要求4字节对齐
// 返回值  : 程序运行指针
// =========================================================================
void (*Dcore_LoadTiBinCode(u8 *code_buf,u32 *code_info))(void)
{
    void (*result)(void);
    u8 *dst;
    u32 size;
    u8 *src = code_buf;
    u32 loop;
    result = (void (*)(void))(src[0] + (src[1]<<8) + (src[2]<<16) + (src[3]<<24));
    if((u32)result != 0xc0400000)
        return NULL;        //入口地址不对，bin格式没有标志字，以正确的入口作为
                            //已烧录代码的标志。
    src = code_buf + 4;     //跳过入口地址
    if(code_info != NULL)
        code_info[0] = (u32)code_buf;
    while(1)
    {
        size = src[0] + (src[1]<<8) + (src[2]<<16) + (src[3]<<24);
        if(size == 0)
            break;
        dst = (u8*)(src[4] + (src[5]<<8) + (src[6]<<16) + (src[7]<<24));
        src += 8;
        for(loop = 0; loop < size; loop++)
            *dst++ = *src++;
    }
    if(code_info != NULL)
        code_info[1] = size;
    return result;
}


// =========================================================================
// 函数功能: 由arm调用，唤醒dsp并使之从start_address地址开始运行。调用本函数前，
//      必须先把代码加载到start_address所在的地址，dsp不可调用本函数。
// 函数参数: start_address，dsp的入口地址
// 返回值  : 无
// =========================================================================
void Dcore_WakeupDspRun(void (*start_address)(void))
{
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;
    g_ptLpsc0Reg->MDCTL[cn_PSC0_DSP] &= ~(u32)LRST;
    g_ptSysconfig0Reg->HOST1CFG = (u32)start_address;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
    Cpucfg_EnableLPSC(cn_PSC0_DSP);
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;
    g_ptLpsc0Reg->MDCTL[cn_PSC0_DSP] |= LRST;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
}

// =========================================================================
// 函数功能: 由arm调用，唤醒dsp并使之处于复位态。
// 函数参数: start_address，dsp的入口地址
// 返回值  : 无
// =========================================================================
void Dcore_WakeupDspReset(void)
{
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;
    g_ptLpsc0Reg->MDCTL[cn_PSC0_DSP] &= ~(u32)LRST;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
    Cpucfg_EnableLPSC(cn_PSC0_DSP);
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;
    g_ptLpsc0Reg->MDCTL[cn_PSC0_DSP] |= LRST;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
}

#define CN_AisMagicWord         0x41504954
#define CN_SectionLoad          0x58535901
#define CN_SectionFill          0x5853590A
#define CN_EnableCRC            0x58535903
#define CN_DisableCRC           0x58535904
#define CN_ValidateCRC          0x58535902
#define CN_Jump_Close           0x58535906
#define CN_Jump2App             0x58535905
#define CN_SequentialReadEnable 0x58535963
#define CN_FunctionExecute      0x5853590D
#define CN_BootTable            0x58535907
// =========================================================================
// 函数功能: 加载 Ti AIS格式二进制代码，调用前，须先把AIS文件读入到内存中。
//      ais文件的编码格式见Ti文档:SPRAB41C UsingtheOMAP-L1x8Bootloader
// 函数参数: code_buf，ais文件存放地址，要求4字节对齐
// 返回值  : 程序运行指针
// =========================================================================
void (*Dcore_LoadAisCode(u8 *code_buf,u32 *code_info))(void)
{
    void (*result)(void);
    u32 *dst;
    s32 size;
    u32 *src = (u32*)code_buf;
    u32 loop,command,data;
    if(((u32)code_buf & 0x3) != 0)   //要求输入缓冲区4字节对齐
        return NULL;
    if(code_info != NULL)
        code_info[0] = (u32)code_buf;
    if(*src != CN_AisMagicWord)         //非AIS文件
        return NULL;
    src++;
    while(1)
    {
        switch(*src++)
        {
            case CN_SectionLoad:
            {
                dst = (u32*)(*src++);
                size = *src++;
//                memcpy(dst,src,size);
//                src += size>>2;
                for(; size>0; size -=4)
                    *dst++ = *src++;
            }break;
            case CN_SectionFill:
            {
                dst = (u32*)(*src++);
                size = *src++;
//                memset(dst,0,size);
//                src += size>>2;
                for(; size>0; size -=4)
                    *dst++ = 0;
            }break;
            case CN_Jump_Close:
            case CN_Jump2App:
            {
                result = (void (*)(void))(*src);
                if(code_info != NULL)
                    code_info[1] = (u32)(src+1) - (u32)code_buf;
                return result;
            }break;
            default:break;
        }
    }
    if(code_info != NULL)
        code_info[1] = (u32)(src) - (u32)code_buf;
    return result;
}

// =========================================================================
// 函数功能: 由arm调用，每次调用将触发一次dsp的nmi中断，如果nmi中断已经挂起，则
//      不执行任何操作。dsp不可调用本函数。
// 函数参数: 无
// 返回值  : 无
// =========================================================================
void Dcore_AssertDspIntNmi(void)
{
    g_ptSysconfig0Reg->CHIPSIG = 0x10;
}

// =========================================================================
// 函数功能: 由arm调用，每次调用将触发一次dsp的chipint2中断(5号中断)，如果
//      chipint2中断已经挂起，则不执行任何操作。dsp不可调用本函数。
// 函数参数: 无
// 返回值  : 无
// =========================================================================
void Dcore_AssertDspIntCi2(void)
{
    g_ptSysconfig0Reg->CHIPSIG = 0x04;
}

// =========================================================================
// 函数功能: 由arm调用，每次调用将触发一次dsp的chipint3中断(67号中断)，如果
//      chipint3中断已经挂起，则不执行任何操作。dsp不可调用本函数。
// 函数参数: 无
// 返回值  : 无
// =========================================================================
void Dcore_AssertDspIntCi3(void)
{
    g_ptSysconfig0Reg->CHIPSIG = 0x08;
}

// =========================================================================
// 函数功能: 由dsp调用，每次调用将触发一次arm的chipint0中断(28号中断)，如果
//      chipint0中断已经挂起，则不执行任何操作。arm不可调用本函数。
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Dcore_AssertArmIntCi0(void)
{
    g_ptSysconfig0Reg->CHIPSIG = 0x1;
}

// =========================================================================
// 函数功能: 由dsp调用，每次调用将触发一次arm的chipint1中断(29号中断)，如果
//      chipint1中断已经挂起，则不执行任何操作。arm不可调用本函数。
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Dcore_AssertArmIntCi1(void)
{
    g_ptSysconfig0Reg->CHIPSIG = 0x2;
}

// =========================================================================
// 函数功能: 由dsp调用，每次调用将触发一次arm的chipint2中断(30号中断)，如果
//      chipint2中断已经挂起，则不执行任何操作。arm不可调用本函数。
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Dcore_AssertArmIntCi2(void)
{
    g_ptSysconfig0Reg->CHIPSIG = 0x4;
}

// =========================================================================
// 函数功能: 由dsp调用，每次调用将触发一次arm的chipint3中断(28号中断)，如果
//      chipint3中断已经挂起，则不执行任何操作。arm不可调用本函数。
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Dcore_AssertArmIntCi3(void)
{
    g_ptSysconfig0Reg->CHIPSIG = 0x8;
}

// =========================================================================
// 函数功能: 由dsp调用，清除已挂起的dsp的nmi中断，如果nmi中断未挂起，则不执行
//      任何操作。arm不可调用本函数。
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Dcore_ClearDspIntNmi(void)
{
    g_ptSysconfig0Reg->CHIPSIG_CLR = 0x10;
}

// =========================================================================
// 函数功能: 由dsp调用，清除已挂起的dsp的chipint2中断(5号中断)，如果chipint2中
//      断未挂起，则不执行任何操作。arm不可调用本函数。
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Dcore_ClearDspIntCi2(void)
{
    g_ptSysconfig0Reg->CHIPSIG_CLR = 0x04;
}

// =========================================================================
// 函数功能: 由dsp调用，清除已挂起的dsp的chipint3中断(67号中断)，如果chipint3中
//      断未挂起，则不执行任何操作。arm不可调用本函数。
// 输入参数：无
// 输出参数：无
// 返回值  ：无
// =========================================================================
void Dcore_ClearDspIntCi3(void)
{
    g_ptSysconfig0Reg->CHIPSIG_CLR = 0x08;
}

// =========================================================================
// 函数功能: 由arm调用，清除已挂起的arm的chipint0中断(28号中断)，如果chipint0中
//      断未挂起，则不执行任何操作。dsp不可调用本函数。
// 函数参数: 无
// 返回值  : 无
// =========================================================================
void Dcore_ClearArmIntCi0(void)
{
    g_ptSysconfig0Reg->CHIPSIG_CLR = 0x1;
}

// =========================================================================
// 函数功能: 由arm调用，清除已挂起的arm的chipint1中断(29号中断)，如果chipint1中
//      断未挂起，则不执行任何操作。dsp不可调用本函数。
// 函数参数: 无
// 返回值  : 无
// =========================================================================
void Dcore_ClearArmIntCi1(void)
{
    g_ptSysconfig0Reg->CHIPSIG_CLR = 0x2;
}

// =========================================================================
// 函数功能: 由dsp调用，清除已挂起的arm的chipint2中断(30号中断)，如果chipint2中
//      断未挂起，则不执行任何操作。dsp不可调用本函数。
// 函数参数: 无
// 返回值  : 无
// =========================================================================
void Dcore_ClearArmIntCi2(void)
{
    g_ptSysconfig0Reg->CHIPSIG_CLR = 0x4;
}

// =========================================================================
// 函数功能: 由arm调用，清除已挂起的arm的chipint3中断(31号中断)，如果chipint3中
//      断未挂起，则不执行任何操作。dsp不可调用本函数。
// 函数参数: 无
// 返回值  : 无
// =========================================================================
void Dcore_ClearArmIntCi3(void)
{
    g_ptSysconfig0Reg->CHIPSIG_CLR = 0x08;
}

