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
// 模块描述: 异常的抛出以及异常信息解析
// 模块版本: V1.00
// 创建人员: zhangqf_cyg
// 创建时间: 5:59:37 PM/Apr 17, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注:所有异常的统一入口
// 该文件为异常的抛出处理，在抛出异常之前，务必保证针对该异常的特殊处理已经做了
// 该文件只是更好的管理异常信息及其架构，而非统辖整个异常处理
#include "config-prj.h"

#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "int.h"
#include "arch_feature.h"
#include "exp.h"
#include "endian.h"
#include "exp_api.h"
#include "exp_decoder.h"
#include "exp_hook.h"
#include "exp_osstate.h"
#include "Exp_Record.h"

//全部相同，那么不涉及到大小端的问题
#define CN_EXP_HEADINFO_MAGICNUMBER    ((u32)(0x77777777))
#define CN_EXP_PARTIALINFO_VALID       ((u32)(0x12345678))
struct tagExpHeadInfo       //todo: 各成员加上注释
{
    u32   recordendian;
    u32   magicnumber;
    u32   osstatevalid;
    u32   osstateinfolen;
    u32   hookvalid;
    u32   hookinfolen;
    u32   hookresult;
    u32   throwinfovalid;
    u32   throwinfolen;
    u32   throwresult;
    u32   dealresult;
    char  decodername[CN_EXP_NAMELEN_LIMIT];
};//可以考虑使用bit位来省存储空间，但是考虑下来，根本省不了几个字节，改动意义不大

static bool_t s_bExpMoDuleInitState = false;
// =============================================================================
// 函数功能:__Exp_ResultMerge
//           将HOOK结果和THROW结果做综合处理
// 输入参数:Result_Throw,设定的异常处理结果
//          Result_Hook,HOOK的异常处理结果
// 输出参数:无
// 返回值  :_SYSEXP_RESULT_TYPE,异常的处理结果
// 说明    :内部调用函数
// =============================================================================
static enum _EN_EXP_DEAL_TYPE_  __Exp_ResultMerge(enum _EN_EXP_DEAL_TYPE_ Result_Throw,\
                                             enum _EN_EXP_DEAL_TYPE_ Result_Hook)
{
    enum _EN_EXP_DEAL_TYPE_ result;

    if(EN_EXP_DEAL_DEFAULT == Result_Hook)
    {
        result = Result_Throw;
    }
    else
    {
        result = Result_Hook;
    }
    return result;
}
extern void reset(void);
extern void reboot(void);
extern void restart_system(void);
// =============================================================================
// 函数功能:__Exp_ResultDeal
//          处理异常的最终结果
// 输入参数:expdealresult,异常结果
//          expinfo,搜集的异常信息
// 输出参数:无
// 返回值  :异常的最终处理结果(见enum _EN_EXP_DEAL_REASULT声明)
// 说明    :内部调用函数,如果异常结果中有需要重启服务，那么就不会返回的。
// =============================================================================
static u32  __Exp_ResultDeal(u32 expdealresult,\
                            struct tagExpRecordPara *expinfo)
{
    u32 result;
    if(expdealresult < EN_EXP_DEAL_RECORD)
    {
        result = EN_EXP_DEALT_IGNORE;
    }
    else
    {
        result = Exp_Record(expinfo);
        if(result != EN_EXP_DEAL_RECORD_SUCCESS)
        {
            printk("expresultdealer:RECORD FAILED!\n\r");
            result = EN_EXP_DEALT_RECORDFAILED;
#ifdef DEBUG                 //调试时候挂起
            while(1)
            {

            }
#endif
        }
        else
        {
            result = EN_EXP_DEALT_SUCCESS;
        }
        switch (expdealresult)
        {
            //重启是不会返回的，不然必然是出错了
            case EN_EXP_DEAL_RESET:
                reset();
                break;
            case EN_EXP_DEAL_REBOOT:
                reboot();
                break;
            case EN_EXP_DEAL_RESTART:
                restart_system();
                break;
            default:
                break;
        }
    }
    return result;
}
// =============================================================================
// 函数功能:Exp_Throw
//          处理所有异常的入口
// 输入参数:throwpara,抛出的异常信息参数
// 输出参数:dealresult,该异常的最终处理结果
// 返回值  :true，成功， false，失败(参数或者存储等未知原因)
// 说明    :异常抛出的唯一入口;如果要重启，则该函数不返回
// =============================================================================
bool_t  Exp_Throw(struct tagExpThrowPara *throwpara, u32 *dealresult)
{
    bool_t result;
    u32   result_hook;
    u32   result_throw;
    u32   result_deal;
    ptu32_t infoaddr;
    u32 infolen;
    struct tagExpHeadInfo  headinfo;
    struct tagExpRecordPara recordpara;
    if(s_bExpMoDuleInitState == false)  //组件未初始化，证明很多系统调用都不能使用
    {
        return false;
    }
    Int_SaveAsynSignal();
    if((NULL != throwpara) &&( true == throwpara->validflag))//抛出有效
    {
        //抛出信息处理
        if(NULL != throwpara->name)
        {
            strncpy(&headinfo.decodername[0], throwpara->name, CN_EXP_NAMELEN_LIMIT);
        }
        else
        {
            headinfo.decodername[0] = '\0';
        }
        headinfo.throwinfolen = throwpara->para_len;
        headinfo.throwresult = throwpara->dealresult;
        headinfo.throwinfovalid = CN_EXP_PARTIALINFO_VALID;
        result_throw = throwpara->dealresult;
        recordpara.throwinfoaddr = (ptu32_t)(throwpara->para);
        recordpara.throwinfolen = throwpara->para_len;
        recordpara.headinfoaddr = (ptu32_t)(&(headinfo));
        recordpara.headinfolen = (u32)(sizeof(headinfo));

        //HOOK信息的搜集（如果搜集不成功，并且抛出者无意记录，则直接返回）
        result = Exp_HookDealer(throwpara, &infoaddr, &infolen, &result_hook);
        if(false == result)
        {
            result_hook = EN_EXP_DEAL_DEFAULT;
            headinfo.hookvalid = ~CN_EXP_PARTIALINFO_VALID;
            infoaddr = 0;
            infolen = 0;
        }
        else
        {
            headinfo.hookvalid = CN_EXP_PARTIALINFO_VALID;
        }
        result_deal = __Exp_ResultMerge(result_throw, result_hook);

        if(result_deal < EN_EXP_DEAL_RECORD)
        {
            //连记录都无需的话，那么就直接返回吧,简直开玩笑
            result = true;
            *dealresult = EN_EXP_DEALT_IGNORE;
            goto throw_logic_exit;
        }
        headinfo.dealresult = result_deal;
        headinfo.hookinfolen = infolen;
        headinfo.hookresult = result_hook;
        recordpara.hookinfoaddr = infoaddr;
        recordpara.hookinfolen = infolen;

        //OSSTATE信息的搜集(如果搜集不成功，无关大局)
        result = Exp_OsStateInfoGather(throwpara, &infoaddr, &infolen);
        if(false == result)
        {
            infoaddr = 0;
            infolen = 0;
            headinfo.osstatevalid = ~CN_EXP_PARTIALINFO_VALID;
        }
        else
        {
            headinfo.osstatevalid = CN_EXP_PARTIALINFO_VALID;
        }
        headinfo.osstateinfolen = infolen;
        headinfo.recordendian = CN_CFG_BYTE_ORDER;
        headinfo.magicnumber = CN_EXP_HEADINFO_MAGICNUMBER;
        recordpara.osstateinfoaddr = infoaddr;
        recordpara.osstateinfolen = infolen;

        //该搜集的信息都搜集完毕，那么就处理吧
        *dealresult = __Exp_ResultDeal(result_deal, &recordpara);
        result = true;
    }
    else
    {
        *dealresult = EN_EXP_DEALT_PARAERR;
        result = true;
    }
throw_logic_exit:
    Int_RestoreAsynSignal();
    return result;
}

// =============================================================================
// 函数功能:__Exp_HeadinfoSwapByEndian
//         信息头字节序转换
// 输入参数:expinfo,异常信息
// 输出参数:无
// 返回值  :无
// 说明    :将expinfo中已知信息转换字节序,name以前的字节必须是4字节对齐的
// =============================================================================
void  __Exp_HeadinfoSwapByEndian(struct tagExpHeadInfo *headinfo)
{
    u32 temp;
    u32 limit;
    u32 *addr;

    temp = (u32)headinfo;
    limit = (u32)(&(headinfo->decodername[0]));
    while(temp < limit)
    {
        addr = (u32 *)temp;
        *addr = swapl(*addr);
        temp = temp +4;
    }
}
// =============================================================================
// 函数功能:__Exp_HeadinfoDecoder
//          异常信息头解析
// 输入参数:headinfo,异常信息头
// 输出参数:无
// 返回值  :true成功 false失败
// 说明    :内部调用
// =============================================================================
bool_t __Exp_HeadinfoDecoder(struct tagExpHeadInfo *headinfo)
{
    printk("exp_headinfo:recordendian    = 0x%08x\n\r",headinfo->recordendian);
    printk("exp_headinfo:magicnumber     = 0x%08x\n\r",headinfo->magicnumber);
    printk("exp_headinfo:osstatevalid    = 0x%08x\n\r",headinfo->osstatevalid);
    printk("exp_headinfo:osstateinfolen  = 0x%08x\n\r",headinfo->osstateinfolen);
    printk("exp_headinfo:hookvalid       = 0x%08x\n\r",headinfo->hookvalid);
    printk("exp_headinfo:hookinfolen     = 0x%08x\n\r",headinfo->hookinfolen);
    printk("exp_headinfo:hookresult      = 0x%08x\n\r",headinfo->hookresult);
    printk("exp_headinfo:throwinfovalid  = 0x%08x\n\r",headinfo->throwinfovalid);
    printk("exp_headinfo:throwinfolen    = 0x%08x\n\r",headinfo->throwinfolen);
    printk("exp_headinfo:throwresult     = 0x%08x\n\r",headinfo->throwresult);
    printk("exp_headinfo:dealresult      = 0x%08x\n\r",headinfo->dealresult);
    printk("exp_headinfo:expdecodername  = %s\n\r",headinfo->decodername);

    return true;
}

// =============================================================================
// 函数功能:Exp_InfoDecoder
//          解析一条异常信息
// 输入参数:info,异常信息
//          infolen, 异常信息长度
// 输出参数:无
// 返回值  :true成功 false失败
// 说明    :异常信息一定已经拷贝到了内存当中，或则可能会修改原信息
// =============================================================================
bool_t  Exp_InfoDecoder(struct tagExpRecordPara *recordpara)
{
    bool_t  result = false;
    u32  endian;
    struct tagExpHeadInfo *headinfo;
    struct tagExpThrowPara throwpara;
    if(NULL == recordpara)
    {
        result = false;
        printk("Exp_InfoDecoder:invalid parameter!\n\r");
    }
    else if(( 0 == recordpara->headinfoaddr ) || (sizeof(struct tagExpHeadInfo) != recordpara->headinfolen))
    {
        result = false;
        printk("Exp_InfoDecoder:incomplete exception headinfo!\n\r");
    }
    else
    {
        headinfo = (struct tagExpHeadInfo *)(recordpara->headinfoaddr);

        //用小端作为标准主要是因为小端是0，大小端都一样
        if(((headinfo->recordendian == CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER != CN_CFG_LITTLE_ENDIAN))||\
           ((headinfo->recordendian != CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)))
        {
            //需要大小端转换
            __Exp_HeadinfoSwapByEndian(headinfo);
        }
        if(CN_EXP_HEADINFO_MAGICNUMBER != headinfo->magicnumber)
        {
            printk("Exp_InfoDecoder:headinfo has been destroyed!\n\r");
            result = false;
        }
        else
        {
            //重现抛出信息参数
            endian = headinfo->recordendian;
            throwpara.dealresult = headinfo->throwresult;
            throwpara.name = headinfo->decodername;
            throwpara.para = (u8 *)(recordpara->throwinfoaddr);
            throwpara.para_len = recordpara->throwinfolen;
            throwpara.validflag = true;
            //信息头解析
            if(recordpara->headinfolen != sizeof(struct tagExpHeadInfo))
            {
                printk("Exp_InfoDecoder:headinfo incomplete!\n\r");
            }
            else
            {
                result = __Exp_HeadinfoDecoder(headinfo);
            }
            //OS状态解析,解析器自己判断包的数据是否被修改
            if(recordpara->osstateinfolen != headinfo->osstateinfolen)
            {
                printk("Exp_InfoDecoder:osstateinfo incomplete!\n\r");
            }
            else
            {
                result = Exp_OsStateInfoDecoder(&throwpara,recordpara->osstateinfoaddr,\
                                             recordpara->osstateinfolen, endian);
            }
            //抛出异常解析,解析器自己判断包的长度是否完整，包的内容是否被破坏
            if(recordpara->throwinfolen != headinfo->throwinfolen)
            {
                printk("Exp_InfoDecoder:throwinfo incomplete!\n\r");
            }
            else
            {
                result = Exp_Throwinfodecoder(&throwpara,endian);
            }
            //HOOK解析,解析器自己判断包的长度是否完整，包的内容是否被破坏
            if(recordpara->hookinfolen != headinfo->hookinfolen)
            {
                printk("Exp_InfoDecoder:hookinfo incomplete!\n\r");
            }
            else
            {
                result = Exp_HookInfoDecoder(&throwpara,recordpara->hookinfoaddr,\
                                             recordpara->hookinfolen, endian);
            }
        }
    }
    return result;
}

// =============================================================================
// 函数功能:ModuleInstall_Exp
//         异常组件初始化
// 输入参数:para
// 输出参数:无
// 返回值  :ptu32_t 暂时无定义
// 说明    :主要是初始化存储方案
// =============================================================================
ptu32_t ModuleInstall_Exp(ptu32_t para)
{
    extern bool_t Exp_ShellInit();
    Exp_ShellInit();
    s_bExpMoDuleInitState = true;

    return para;
}

