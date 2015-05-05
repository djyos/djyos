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
//========================================================
// 文件模块说明:
// omapl138的cpu公共模块配置文件
// 文件版本: v1.00
// 开发人员: lst
// 创建时间: 2011.06.01
// Copyright(c) 2011-2011  深圳鹏瑞软件有限公司
//========================================================
// 程序修改记录(最新的放在最前面):
//  <版本号> <修改日期>, <修改人员>: <修改功能概述>
//========================================================
//例如：
// V1.02 2004-07-14, 张三: 接收函数增加对报文长度有效性校核

#include "stdint.h"
#include "cpu_peri.h"

#define SETBIT(dest,mask)     (dest |= mask)
#define CLRBIT(dest,mask)     (dest &= ~mask)
#define CHKBIT(dest,mask)     (dest & mask)

enum ModulePower Cpucfg_TestLPSC(u8 module)
{
    enum ModulePower result;
    u32 domain;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;
    if(module == cn_PSC0_DSP)
        domain = cn_RAM_Pseudo;
    else
        domain = cn_Always_ON;

    if(module <cn_psc1_base)
    {
        // spin until existing transitions are done.
        while (CHKBIT(g_ptLpsc0Reg->PTSTAT, domain)) {}

        // if we are already in the requested state...just return.
        if (CHKBIT(g_ptLpsc0Reg->MDSTAT[module], MASK_STATE) == PSC_ENABLE)
        {
            result = PlscPowerOn;
        }else
        {
            result = PlscPowerOff;
        }
    }else
    {
        module -= cn_psc1_base;
        // spin until existing transitions are done.
        while (CHKBIT(g_ptLpsc1Reg->PTSTAT, domain)) {}

        // if we are already in the requested state...just return.
        if (CHKBIT(g_ptLpsc1Reg->MDSTAT[module], MASK_STATE) == PSC_ENABLE)
        {
            result = PlscPowerOn;
        }else
        {
            result = PlscPowerOff;
        }
    }

    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
    return result;
}
//----使能psc------------------------------------------------------------------
// 函数功能: 使能外设,
// 函数参数: module，待操作的外设编号，在omapl138_peri_cpucfg.h文件中定义
// 返回值  : 无
//-----------------------------------------------------------------------------
void Cpucfg_EnableLPSC(u8 module)
{
    u32 domain;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;
    if(module == cn_PSC0_DSP)
        domain = cn_RAM_Pseudo;
    else
        domain = cn_Always_ON;

    if(module <cn_psc1_base)
    {
        // spin until existing transitions are done.
        while (CHKBIT(g_ptLpsc0Reg->PTSTAT, domain)) {}

        // if we are already in the requested state...just return.
        if (CHKBIT(g_ptLpsc0Reg->MDSTAT[module], MASK_STATE) == PSC_ENABLE)
        {
            goto exit_completed;
        }

        // setup the transition...clear the bits before setting the next state.
        CLRBIT(g_ptLpsc0Reg->MDCTL[module], NEXT);
        SETBIT(g_ptLpsc0Reg->MDCTL[module], PSC_ENABLE);

        // kick off the transition.
        g_ptLpsc0Reg->PTCMD = domain;

        // spin until transition is done.
        while (CHKBIT(g_ptLpsc0Reg->PTSTAT, domain)) {}

        while (CHKBIT(g_ptLpsc0Reg->MDSTAT[module], MASK_STATE) != PSC_ENABLE) {}
    }else
    {
        module -= cn_psc1_base;
        // spin until existing transitions are done.
        while (CHKBIT(g_ptLpsc1Reg->PTSTAT, domain)) {}

        // if we are already in the requested state...just return.
        if (CHKBIT(g_ptLpsc1Reg->MDSTAT[module], MASK_STATE) == PSC_ENABLE)
        {
            goto exit_completed;
        }

        // setup the transition...clear the bits before setting the next state.
        CLRBIT(g_ptLpsc1Reg->MDCTL[module], NEXT);
        SETBIT(g_ptLpsc1Reg->MDCTL[module], PSC_ENABLE);

        // kick off the transition.
        g_ptLpsc1Reg->PTCMD = domain;

        // spin until transition is done.
        while (CHKBIT(g_ptLpsc1Reg->PTSTAT, domain)) {}

        while (CHKBIT(g_ptLpsc1Reg->MDSTAT[module], MASK_STATE) !=PSC_ENABLE) {}
    }

exit_completed:
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
}

//----禁止psc------------------------------------------------------------------
//功能: 禁止外设以省电
//参数: module，待操作的外设编号，在cpu_peri_cpucfg.h文件中定义
//返回: 无
//-----------------------------------------------------------------------------
void Cpucfg_DisableLPSC(u8 module)
{
    u32 domain;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;
    if(module == cn_PSC0_DSP)
        domain = cn_RAM_Pseudo;
    else
        domain = cn_Always_ON;

    if(module <cn_psc1_base)
    {
        // spin until existing transitions are done.
        while (CHKBIT(g_ptLpsc0Reg->PTSTAT, domain)) {}

        // if we are already in the requested state...just return.
        if (CHKBIT(g_ptLpsc0Reg->MDSTAT[module], MASK_STATE) == PSC_DISABLE)
        {
            goto exit_completed;
        }

        // setup the transition...clear the bits before setting the next state.
        CLRBIT(g_ptLpsc0Reg->MDCTL[module], NEXT);
        SETBIT(g_ptLpsc0Reg->MDCTL[module], PSC_DISABLE);

        // kick off the transition.
        g_ptLpsc0Reg->PTCMD = domain;

        // spin until transition is done.
        while (CHKBIT(g_ptLpsc0Reg->PTSTAT, domain)) {}

        while (CHKBIT(g_ptLpsc0Reg->MDSTAT[module], MASK_STATE) != PSC_DISABLE)
        {}
    }else
    {
        module -= cn_psc1_base;
        // spin until existing transitions are done.
        while (CHKBIT(g_ptLpsc1Reg->PTSTAT, domain)) {}

        // if we are already in the requested state...just return.
        if (CHKBIT(g_ptLpsc1Reg->MDSTAT[module], MASK_STATE) == PSC_DISABLE)
        {
            goto exit_completed;
        }

        // setup the transition...clear the bits before setting the next state.
        CLRBIT(g_ptLpsc1Reg->MDCTL[module], NEXT);
        SETBIT(g_ptLpsc1Reg->MDCTL[module], PSC_DISABLE);

        // kick off the transition.
        g_ptLpsc1Reg->PTCMD = domain;

        // spin until transition is done.
        while (CHKBIT(g_ptLpsc1Reg->PTSTAT, domain)) {}

        while (CHKBIT(g_ptLpsc1Reg->MDSTAT[module], MASK_STATE) !=PSC_DISABLE){}
    }

exit_completed:
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
}

//-----------------------------------------------------------------------------
// 函数功能: 模块同步复位
// 函数参数: module，待操作的外设编号，在omapl138_peri_cpucfg.h文件中定义
// 返回值  : 无
//-----------------------------------------------------------------------------
void Cpucfg_SyncResetLPSC(u8 module)
{
    u32 domain;
    g_ptSysconfig0Reg->KICKR[0] = KICK0R_UNLOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_UNLOCK;
    if(module == cn_PSC0_DSP)
        domain = cn_RAM_Pseudo;
    else
        domain = cn_Always_ON;



    if(module <cn_psc1_base)
    {
        if( (g_ptLpsc0Reg->MDSTAT[module] & MASK_STATE) != PSC_SYNCRESET )
        {
          g_ptLpsc0Reg->MDCTL[module] &= ~(u32)MASK_STATE;
          g_ptLpsc0Reg->MDCTL[module] |= PSC_SYNCRESET;
          g_ptLpsc0Reg->PTCMD = domain;

          /*Wait for power state transition to finish*/
          while (CHKBIT(g_ptLpsc0Reg->PTSTAT, domain)) {}

          while ((g_ptLpsc0Reg->MDSTAT[module] & MASK_STATE) != PSC_SYNCRESET);
        }
    }else
    {
        module -= cn_psc1_base;
        if( (g_ptLpsc1Reg->MDSTAT[module] & MASK_STATE) != PSC_SYNCRESET )
        {
          g_ptLpsc1Reg->MDCTL[module] &= ~(u32)MASK_STATE;
          g_ptLpsc1Reg->MDCTL[module] |= PSC_SYNCRESET;
          g_ptLpsc1Reg->PTCMD = domain;

          /*Wait for power state transition to finish*/
          while (CHKBIT(g_ptLpsc1Reg->PTSTAT, domain)) {}

          while ((g_ptLpsc1Reg->MDSTAT[module] & MASK_STATE) != PSC_SYNCRESET);
        }
    }

    g_ptSysconfig0Reg->KICKR[0] = KICK0R_LOCK;
    g_ptSysconfig0Reg->KICKR[1] = KICK1R_LOCK;
}
