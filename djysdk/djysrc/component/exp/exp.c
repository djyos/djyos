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


#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "int.h"
#include "arch_feature.h"
#include "exp.h"
#include "endian.h"
#include "exp_decoder.h"
#include "exp_hook.h"
#include "exp_osstate.h"
#include "Exp_Record.h"

const char *gExpActionName[EN_EXP_DEAL_LENTH] = {
    "IGNORE",
    "DEFAULT",
    "RECORD",
    "RESET",
    "REBOOT",
    "RESTART",
    "WAIT",
    "DELERR",
};

const char *ExpActionName(enum EN_ExpAction action)
{
    if(action < EN_EXP_DEAL_LENTH)
    {
        return gExpActionName[action];
    }
    else
    {
        return NULL;
    }
}

//全部相同，那么不涉及到大小端的问题
#define CN_EXP_HEADINFO_MAGICNUMBER    ((u32)(0x77777777))
#define CN_EXP_PARTIALINFO_VALID       ((u32)(0x12345678))

extern bool_t HardExp_InfoDecoderInit(void);

struct ExpHeadInfo
{
    u32   recordendian;     //异常记录是大端还是小端,利于离线分析异常记录
    u32   magicnumber;      //用于检查信息是否有效
    u32   ExpType;          //异常类型
    u32   osstatevalid;     //CN_EXP_PARTIALINFO_VALID=操作系统运行状态信息有效
    u32   osstateinfolen;   //操作系统运行状态信息长度
    u32   hookvalid;        //CN_EXP_PARTIALINFO_VALID=hook收集的信息有效
    u32   hookinfolen;      //hook收集的信息长度
    u32   HookAction;       //hook要求的异常处理动作,由enum EN_ExpAction定义
    u32   throwinfovalid;   //抛出的信息存储区是否有效
    u32   throwinfolen;     //抛出的信息长度
    u32   ThrowAction;      //抛出时要求的处理动作,由enum EN_ExpAction定义
    u32   ExpAction;        //最终采取的动作.,由enum EN_ExpAction定义
    char  decodername[CN_EXP_NAMELEN_LIMIT];  //异常解码器名字,长度不得超过15
                            //如果是个NULL指针,则表示无解码器
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
static enum EN_ExpAction  __Exp_ResultMerge(enum EN_ExpAction Result_Throw,\
                                             enum EN_ExpAction Result_Hook)
{
    enum EN_ExpAction result;

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
// 函数功能: 处理异常的最终结果
// 输入参数:expdealresult,异常结果
//          expinfo,搜集的异常信息
// 输出参数:无
// 返回值  :异常的最终处理结果(见enum _EN_EXP_DEAL_REASULT声明)
// 说明    :内部调用函数,如果异常结果中有需要重启服务，那么就不会返回的。
// =============================================================================
static enum EN_ExpDealResult  __Exp_ExecAction(u32 FinalAction,\
                                    struct ExpRecordPara *expinfo)
{
    enum EN_ExpDealResult result;
    result = Exp_Record(expinfo);
    if(result != EN_EXP_RESULT_SUCCESS)
    {
    	printk("expresultdealer:RECORD FAILED!\n\r");
    }
    switch (FinalAction)
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
        case EN_EXP_DEAL_WAIT:
            while(1);
            break;
        default:
            break;
    }
    return result;
}
// =============================================================================
// 函数功能: 抛出异常信息
// 输入参数:throwpara,抛出的异常信息参数
// 返回值  :异常处理需采取的行动，如果有hook，可能会被hook的返回值替代
// 说明    :本函数很可能不返回，直接复位
// =============================================================================
enum EN_ExpDealResult  Exp_Throw(struct ExpThrowPara *throwpara)
{
    bool_t result;
    enum EN_ExpAction   HookAction;
    enum EN_ExpAction   ThrowAction;
    enum EN_ExpAction   FinalAction;
    enum EN_ExpDealResult Result;
    ptu32_t infoaddr;
    u32 infolen;
    struct ExpHeadInfo  headinfo;
    struct ExpRecordPara recordpara;
    if(s_bExpMoDuleInitState == false)  //组件未初始化，很多系统调用都不能使用
    {
        return EN_EXP_RESULT_PARAERR;
    }
    if(NULL != throwpara)   //抛出有效
    {
        //抛出信息处理
        if(NULL != throwpara->DecoderName)
        {
            strncpy(headinfo.decodername, throwpara->DecoderName, CN_EXP_NAMELEN_LIMIT);
        }
        else
        {
            headinfo.decodername[0] = '\0';
        }
        headinfo.throwinfolen = throwpara->ExpInfoLen;
        headinfo.ThrowAction = (u32)throwpara->ExpAction;
        headinfo.ExpType = throwpara->ExpType;
        headinfo.throwinfovalid = CN_EXP_PARTIALINFO_VALID;
        ThrowAction = throwpara->ExpAction;
        recordpara.throwinfoaddr = (ptu32_t)(throwpara->ExpInfo);
        recordpara.throwinfolen = throwpara->ExpInfoLen;
        recordpara.headinfoaddr = (ptu32_t)(&(headinfo));
        recordpara.headinfolen = (u32)(sizeof(headinfo));

        //HOOK信息的搜集（如果搜集不成功，并且抛出者无意记录，则直接返回）
        result = Exp_HookDealer(throwpara, &infoaddr, &infolen, &HookAction);
        if(false == result)
        {
            HookAction = EN_EXP_DEAL_DEFAULT;
            headinfo.hookvalid = ~CN_EXP_PARTIALINFO_VALID;
            infoaddr = 0;
            infolen = 0;
        }
        else
        {
            headinfo.hookvalid = CN_EXP_PARTIALINFO_VALID;
        }
        FinalAction = __Exp_ResultMerge(ThrowAction, HookAction);

        if(FinalAction < EN_EXP_DEAL_RECORD)
        {
            //连记录都无需的话，那么就直接返回吧,简直开玩笑
            Result = EN_EXP_RESULT_IGNORE;
        }
        else
        {
            headinfo.ExpAction = (u32)FinalAction;
            headinfo.hookinfolen = infolen;
            headinfo.HookAction = HookAction;
            recordpara.hookinfoaddr = infoaddr;
            recordpara.hookinfolen = infolen;

            //操作系统运行状态信息的搜集(如果搜集不成功，无关大局)
            __Exp_OsStateInfoGather(&infoaddr, &infolen);
            headinfo.osstatevalid = CN_EXP_PARTIALINFO_VALID;
            headinfo.osstateinfolen = infolen;
            headinfo.recordendian = CN_CFG_BYTE_ORDER;
            headinfo.magicnumber = CN_EXP_HEADINFO_MAGICNUMBER;

            recordpara.osstateinfoaddr = infoaddr;
            recordpara.osstateinfolen = infolen;

            //该搜集的信息都搜集完毕，那么就处理吧
            Result = __Exp_ExecAction(FinalAction, &recordpara);
        }
    }
    else
    {
        Result = EN_EXP_RESULT_IGNORE;
    }
    return Result;
}

// =============================================================================
// 函数功能: 信息头字节序转换
// 输入参数:expinfo,异常信息
// 输出参数:无
// 返回值  :无
// 说明    :将expinfo中已知信息转换字节序,name以前的字节必须是4字节对齐的
// =============================================================================
void  __Exp_HeadinfoSwapByEndian(struct ExpHeadInfo *headinfo)
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
// 函数功能: 异常信息头解析
// 输入参数:headinfo,异常信息头
// 输出参数:无
// 返回值  :true成功 false失败
// 说明    :内部调用
// =============================================================================
bool_t __Exp_HeadinfoDecoder(struct ExpHeadInfo *headinfo)
{
    printf("exp_headinfo:magicnumber   :0x%08x\n\r",headinfo->magicnumber);
    printf("exp_headinfo:record endian :%s\n\r",\
            headinfo->recordendian==CN_CFG_LITTLE_ENDIAN?"LittleEndian":"BigEndian");
    printf("exp_headinfo:ExpType       :0x%08x\n\r",headinfo->ExpType);
    printf("exp_headinfo:osstatevalid  :%s\n\r",\
            headinfo->osstatevalid==CN_EXP_PARTIALINFO_VALID?"Valid":"Invalid");
    printf("exp_headinfo:osstateinfolen:%d Bytes\n\r",headinfo->osstateinfolen);
    printf("exp_headinfo:hookinfo stat :%s\n\r",\
            headinfo->hookvalid==CN_EXP_PARTIALINFO_VALID?"Valid":"Invalid");
    printf("exp_headinfo:hookinfolen   :%d Bytes\n\r",headinfo->hookinfolen);
    printf("exp_headinfo:HookAction    :%s\n\r",ExpActionName(headinfo->HookAction));
    printf("exp_headinfo:throwinfo stat:%s\n\r",\
            headinfo->throwinfovalid==CN_EXP_PARTIALINFO_VALID?"Valid":"Invalid");
    printf("exp_headinfo:throwinfolen  :%d Bytes\n\r",headinfo->throwinfolen);
    printf("exp_headinfo:ThrowAction   :%s\n\r",ExpActionName(headinfo->ThrowAction));
    printf("exp_headinfo:ExpAction     :%s\n\r",ExpActionName(headinfo->ExpAction));
    printf("exp_headinfo:DecoderName   :%s\n\r",headinfo->decodername);

    return true;
}

// =============================================================================
// 函数功能: 解析异常信息
// 输入参数:recordpara,异常信息
// 输出参数:无
// 返回值  :true成功 false失败
// 说明    :异常信息一定已经拷贝到了内存当中，或则可能会修改原信息
// =============================================================================
bool_t  Exp_InfoDecoder(struct ExpRecordPara *recordpara)
{
    bool_t  result = false;
    u32  endian;
    struct ExpHeadInfo *headinfo;
    struct ExpThrowPara throwpara;
    if(NULL == recordpara)
    {
        result = false;
        printf("Exp_InfoDecoder:invalid parameter!\n\r");
    }
    else if(( 0 == recordpara->headinfoaddr ) || (sizeof(struct ExpHeadInfo) != recordpara->headinfolen))
    {
        result = false;
        printf("Exp_InfoDecoder:incomplete exception headinfo!\n\r");
    }
    else
    {
        headinfo = (struct ExpHeadInfo *)(recordpara->headinfoaddr);

        //用小端作为标准主要是因为小端是0，大小端都一样
        if(((headinfo->recordendian == CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER != CN_CFG_LITTLE_ENDIAN))||\
           ((headinfo->recordendian != CN_CFG_LITTLE_ENDIAN)&&(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)))
        {
            //需要大小端转换
            __Exp_HeadinfoSwapByEndian(headinfo);
        }
        if(CN_EXP_HEADINFO_MAGICNUMBER != headinfo->magicnumber)
        {
            printf("Exp_InfoDecoder:headinfo has been destroyed!\n\r");
            result = false;
        }
        else
        {
            //重现抛出信息参数
            endian = headinfo->recordendian;
            throwpara.ExpAction = headinfo->ThrowAction;
            throwpara.DecoderName = headinfo->decodername;
            throwpara.ExpInfo = (u8 *)(recordpara->throwinfoaddr);
            throwpara.ExpInfoLen = recordpara->throwinfolen;
            throwpara.ExpType = headinfo->ExpType;
            //信息头解析
            if(recordpara->headinfolen != sizeof(struct ExpHeadInfo))
            {
                printf("Exp_InfoDecoder:headinfo incomplete!\n\r");
            }
            else
            {
                result = __Exp_HeadinfoDecoder(headinfo);
            }
            //OS状态解析,解析器自己判断包的数据是否被修改
            if(recordpara->osstateinfolen != headinfo->osstateinfolen)
            {
                printf("Exp_InfoDecoder:osstateinfo incomplete!\n\r");
            }
            else
            {
                result = __Exp_OsStateInfoDecoder(recordpara->osstateinfoaddr,\
                                             recordpara->osstateinfolen, endian);
            }
            //抛出异常解析,解析器自己判断包的长度是否完整，包的内容是否被破坏
            if(recordpara->throwinfolen != headinfo->throwinfolen)
            {
                printf("Exp_InfoDecoder:throwinfo incomplete!\n\r");
            }
            else
            {
                result = Exp_ThrowInfodecode(&throwpara,endian);
            }
            //HOOK解析,解析器自己判断包的长度是否完整，包的内容是否被破坏
            if(recordpara->hookinfolen != headinfo->hookinfolen)
            {
                printf("Exp_InfoDecoder:hookinfo incomplete!\n\r");
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
// 函数功能: 异常组件初始化,同时安装了硬件异常的信息解析函数。
// 输入参数:para
// 输出参数:无
// 返回值  :ptu32_t 暂时无定义
// =============================================================================
ptu32_t ModuleInstall_Exp(ptu32_t para)
{
    extern bool_t Exp_ShellInit();
    Exp_ShellInit();
    dListInit(&ExpDecoderListHead);

    // 初始化硬件异常信息解析函数,这里要特别说明一下,本着谁抛出异常谁解析的原则,
    // 例如看门狗异常,在ModuleInstall_Wdt函数中调用Exp_RegisterThrowinfoDecoder
    // 初始化看门狗的异常解析函数.硬件异常信息解析函数,也应该由初始化硬件异常的
    // 模块注册。但硬件异常不一样,它是在预加载时初始化的,而那时异常组件还没有准
    // 备好,故必须在这里初始化.
    HardExp_InfoDecoderInit( );
    s_bExpMoDuleInitState = true;

    return para;
}

