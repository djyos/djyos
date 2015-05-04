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
// 模块描述: 异常的API数据结构及其接口声明(异常组件对外接口)
// 模块版本: V1.00
// 创建人员: Administrator
// 创建时间: 3:17:17 PM/May 8, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
// 备注:

#ifndef EXP_API_H_
#define EXP_API_H_

#include "stdint.h"
#define CN_EXP_NAMELEN_LIMIT            0x10
#define CN_EXP_DECODERNUM_LIMIT         0x10

//异常处理类型,不允许调整位置
enum _EN_EXP_DEAL_TYPE_
{
    EN_EXP_DEAL_IGNORE = 0,      //忽略该异常信息
    EN_EXP_DEAL_DEFAULT,         //按默认方式处理，抛出异常的时候不能使用该选项
    EN_EXP_DEAL_RECORD,          //需要记录该异常
    EN_EXP_DEAL_RESET,           //硬件重启，相当于上电启动
    EN_EXP_DEAL_REBOOT,          //跳转到运行模式选择
    EN_EXP_DEAL_RESTART,         //跳转到本次运行方式的最开始部分
};

//异常提示信息（用于抛出异常的时候使用）
struct tagExpThrowPara
{
    bool_t  validflag;                 //true表示该异常参数有效，否则无效
    char    *name;                     //异常信息解析器名字
    u8      *para;                     //异常信息地址
    u32     para_len;                  //信息长度
    u32     dealresult;                //异常处理类型,参考_SYSEXP_DEAL_TYPE
};

////////////////////////异常抛出信息解析器模型及其注册注销////////////////////////
//具体异常信息的解析器模型，参数定义和抛出时一模一样，我相信自己搜集的信息一定
//能够解析自己一定能够判断出是不是自己搜集的信息
typedef bool_t (*fnExp_ThrowinfoDecoderModule)(struct tagExpThrowPara *para, u32 endian);
// =============================================================================
// 函数功能：Exp_RegisterThrowinfoDecoder
//          注册软件异常信息解析器
// 输入参数：
//          decoder,异常解析器
//          name,异常解析器名字,至少保证是全局的且不会变的
// 输出参数：
// 返回值  ：true,成功注册;false,注册失败，
// 说明    ：
//          1,当对应的异常号已经被注册了的时候，会查找未被注册的异常号进行注册
//          2,无名或者已经存在对应命名的异常会导致注册失败澹(只是‘\0’也是无效的)
//          3,有注册失败的可能，因此注意检查返回结果
// =============================================================================
bool_t Exp_RegisterThrowinfoDecoder(fnExp_ThrowinfoDecoderModule decoder,const char *name);
// =============================================================================
// 函数功能:Exp_UnRegisterThrowinfoDecoder
//          注销软件异常信息解析器
// 输入参数:name,已经被注册的异常名字
// 输出参数：
// 返回值  :true,成功注销;false,注销失败，
// 说明    ：
// =============================================================================
bool_t Exp_UnRegisterThrowinfoDecoder(char *name);


//HOOK信息搜集者自己提供存储HOOK异常信息的空间
//////////////////////////////HOOK原型//////////////////////////////////////////
// =============================================================================
// 函数功能：HOOKDealer原型
// 输入参数：throwpara, 异常抛出者抛出的异常参数，hook能解就解，不能解就算了
// 输出参数：infoaddr,存储异常信息的地址
//           infolen,存储搜集信息长度
// 返回值  ： _SYSEXP_RESULT_TYPE，该处理结果会覆盖掉BSP的处理异常结果。
// 说明    ： 异常时调用的APP提供的异常处理函数，用来检测异常时系统应用程序的运行状态
//           应用程序提供的钩子函数，应用程序可在此做一些善后工作，并可返回附加的异常
//           信息，这些信息将与BSP收集的硬件和系统异常信息一起保存
//           该钩子的返回结果将会和BSP的处理结果一起作为异常结构的判断依据(一般而言
//           采用木桶原则，由严重程度大的来决定)
// =============================================================================
typedef  enum _EN_EXP_DEAL_TYPE_ (*fnExp_HookDealermodule)( \
                           struct tagExpThrowPara *throwpara,\
                           ptu32_t *infoaddr,u32 *infolen);

// =============================================================================
// 函数功能：HOOKDecoder原型
// 输入参数：throwpara, 异常抛出者抛出的异常参数，hook能解就解，不能解就算了
//          infoaddr,HOOK信息存储地址
//          infolen, HOOK信息有效长度
//          endian, hook消息的存储大小端
// 输出参数：无
// 返回值  ：true成功  false 失败（没有注册等因素）
// =============================================================================
typedef  bool_t (*fnExp_HookInfoDecodermodule)(struct tagExpThrowPara *throwpara,\
                                        ptu32_t infoaddr, u32 infolen,u32 endian);

//用户注册或者注销HOOK的方法
bool_t Exp_RegisterHook(fnExp_HookDealermodule  fnappdealer,\
                            fnExp_HookInfoDecodermodule fnappdecoder);
bool_t Exp_UnRegisterHook(void);

//抛出异常处理结果
enum _EN_EXP_DEAL_REASULT_
{
    EN_EXP_DEALT_PARAERR = 0,          //抛出参数错误
    EN_EXP_DEALT_IGNORE,                    //经过最终决定--忽略
    EN_EXP_DEALT_RECORDFAILED,              //存储出错
    EN_EXP_DEALT_SUCCESS,                   //处理成功，包括记录
};
////////////////////////存储方案模型及其注册注销//////////////////////
 struct tagExpRecordPara
{
    u32     headinfolen;     //头信息长度
    ptu32_t headinfoaddr;    //头信息地址
    u32     osstateinfolen;  //OS状态信息长度
    ptu32_t osstateinfoaddr; //OS状态信息地址
    u32     hookinfolen;     //HOOK信息长度
    ptu32_t hookinfoaddr;    //HOOK信息地址
    u32     throwinfolen;    //throw信息长度
    ptu32_t throwinfoaddr;   //throw信息地址
};
//存储返回结果
enum _EN_EXP_RECORDRESULT_
{
    EN_EXP_DEAL_RECORD_SUCCESS,       //记录成功
    EN_EXP_DEAL_RECORD_NOSPACE,       //存储空间不足
    EN_EXP_DEAL_RECORD_HARDERR,       //存储硬件出错
    EN_EXP_DEAL_RECORD_PARAERR,       //存储参数出错
    EN_EXP_DEAL_RECORD_NOMETHOD,      //存储参数出错,无存储方案
};
// =============================================================================
// 函数功能：记录一帧异常信息
//           记录异常信息。存储或者输出，或者两者兼顾，由异常存储方案设计者决定
// 输入参数：pSysExpFram,一帧异常信息
//          dwFramvalidLen,帧信息的有效长度
//          之所以指定两个长度，主要是为了存储的方便（有可能只记录由效长度即可）
// 输出参数：
// 返回值  ：见en_sysExp_Record_result
// =============================================================================
typedef  enum _EN_EXP_RECORDRESULT_ (*fnExp_RecordModule)(\
                                       struct tagExpRecordPara  *recordpara);
// =============================================================================
// 函数功能：fnExp_RecordCleanModule
//          清除所有的异常信息，清除异常信息存储区域
// 输入参数：无
// 输出参数：无
// 返回值  ：TRUE 成功， FALSE失败
// =============================================================================
typedef  bool_t (*fnExp_RecordCleanModule)(void);

// =============================================================================
// 函数功能：fnExp_RecordCheckNumModule
//          查看一共存储了多少条异常信息
// 输入参数：
// 输出参数：recordnum,存储的异常信息条目数
// 返回值  ：FALSE,失败 TRUE成功
// =============================================================================
typedef  bool_t (*fnExp_RecordCheckNumModule)(u32 *recordnum);

// =============================================================================
// 函数功能：fnExp_RecordCheckLenModule
//          产看指定异常条目的长度
// 输入参数：assignedno,指定的条目
// 输出参数：recordlen,该条目的长度
// 返回值  ：FALSE,失败 TRUE成功
// =============================================================================
typedef  bool_t (*fnExp_RecordCheckLenModule)(u32 assignedno, u32 *recordlen);

// =============================================================================
// 函数功能：fnExp_RecordGetModule
//          从存储介质中获取指定条目的异常帧信息
// 输入参数：assignedno,指定的异常帧条目
//           buflen,缓冲区长度
// 输出参数：buf,用于存储获取指定条目的异常信息
//          recordpara,异常信息存储时的参数，在此是对buf的各个部分的定义
// 返回值  ：FALSE,失败 TRUE成功
// =============================================================================
typedef  bool_t (*fnExp_RecordGetModule)(u32 assignedno,u32 buflen,u8 *buf,\
                                         struct tagExpRecordPara *recordpara);

// =============================================================================
// 函数功能：fnExp_RecordScanModule
//           开机的时候扫描异常存储记录，获取关键信息方便以后存储
// 输入参数：无
// 输出参数：无
// 返回值  ：空
// =============================================================================
typedef  void (*fnExp_RecordScanModule)(void);

 struct tagExpRecordOperate
{
    fnExp_RecordScanModule        fnexprecordscan;    //开机存储区扫描，
    fnExp_RecordModule            fnexprecord;        //记录一条异常信息
    fnExp_RecordCleanModule       fnexprecordclear;   //清除所有异常信息
    fnExp_RecordCheckNumModule    fnexprecordchecknum;//获取存储异常信息的条目数
    fnExp_RecordCheckLenModule    fnexprecordchecklen;//获取指定条目的长度
    fnExp_RecordGetModule         fnexprecordget;     //获取指定条目的异常信息
};
// =============================================================================
// 函数功能：Exp_RegisterRecordOpt
//          注册异常信息处理方法
// 输入参数：opt,需要注册的异常信息处理方法
// 输出参数：
// 返回值  ：FALSE,失败  TRUE成功，失败的话会使用BSP默认的处理方法
// 注意    ：理论上参数结构里面指定的处理方法都应该提供，否则的话会注册不成功
// =============================================================================
bool_t  Exp_RegisterRecordOpt(struct tagExpRecordOperate *opt);

// =============================================================================
// 函数功能：Exp_UnRegisterRecordOpt
//           注销异常信息处理方法
// 输入参数：无
// 输出参数：无
// 返回值  ：TRUE成功  FALSE失败,会使用默认的存储方案
// =============================================================================
bool_t Exp_UnRegisterRecordOpt(void);

////////////////////////////异常抛出模块////////////////////////////////////////
// =============================================================================
// 函数功能:Exp_Throw
//          处理所有异常的入口
// 输入参数:throwpara,抛出的异常信息参数
// 输出参数:dealresult,该异常的最终处理结果
// 返回值  :true，成功， false，失败(参数或者存储等未知原因)
// 说明    :本函数可以在bsp中调用，也可以在系统中调用
//          此设计是为了异常模块处理的统一，版本之间差异缩减为最低，同时方便移植
//          各个异常处理者之间相互独立，不相互干扰
// =============================================================================
bool_t  Exp_Throw(struct tagExpThrowPara *throwpara, u32 *dealresult);
// =============================================================================
// 函数功能:ModuleInstall_Exp
//         系统异常组件初始化
// 输入参数:para
// 输出参数:无
// 返回值  :ptu32_t 暂时无定义
// 说明    :主要是初始化存储方案
// =============================================================================
ptu32_t ModuleInstall_Exp(ptu32_t para);

#endif /* EXP_API_H_ */
