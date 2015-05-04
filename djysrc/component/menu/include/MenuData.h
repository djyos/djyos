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
// 模块描述: DJY菜单模块之用户菜单意愿数据结构的声明
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
//          该文件主要用来声明表达用户创建的菜单树意愿的数据结构
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================


#ifndef _MENU_DATA_H
#define _MENU_DATA_H


#include "MenuDesigner.h"

extern const  struct _tagMenuitemModel g_tMenuitemModelTab[];
extern const  u32 CN_MENU_MAX_ITEM ;


//u32  Menu_func(struct _tagMenuModel *fnpara);//only supplyed by the user and 
//used to fill the struct;

//============================================================================
//上送CPU定义
//============================================================================
enum _ANA_CPU_  {                   // 字节高三位分别表示
    EN_CPU0 = 32,					// bit5
    EN_CPU1 = 64,					// bit6
    EN_CPU2 = 128,					// bit7
};

//============================================================================
//各种记录类型码
//============================================================================
/*
enum _REC_TYPE_
{
    EN_REC_WAVE=0,                  // 录波
    EN_REC_ACT,                     // 事件
    EN_REC_ALM,                     // 告警
    EN_REC_OVER,                    // 越限
    EN_REC_START,                   // 起动
    EN_REC_INPUT,                   // 开入
    EN_REC_CHK,                     // 自检
    EN_REC_RUN,                     // 运行
    EN_REC_ONOFF,                   // 投退
    EN_REC_LOCK,                    // 闭锁
    EN_REC_COMBRK,                  // 通讯中断
    EN_REC_YK,                      // 遥控
    EN_REC_SC,                      // 顺控
    EN_REC_SOE,
    EN_REC_COS,
    EN_REC_OPT,						// 操作
    EN_REC_TQ,
    EN_REC_PRN,                     // 打印
    EN_REC_MENWAVE,                 // 手动录波
    EN_NUM_REC_TYP,                 // 总的记录类型数，注意：这一项固定放在最后！

    EN_ACT_WAVE,
    EN_STA_WAVE,
    EN_HAND_WAVE,
};
*/
//============================================================================
//模拟量常量表类型定义（应用于交流量常量表，及）
//============================================================================
enum    _ANA_TYP_  {                // 模拟量类型定义
    EN_ANA_TYP_NULL = 0,            // 未定义
    EN_ANA_TYP_I,                   // 电流
    EN_ANA_TYP_U,                   // 电压
    EN_ANA_TYP_U_S,                 // 站用变电压

    EN_ANA_TYP_P,                   // 有功功率
    EN_ANA_TYP_Q,                   // 无功功率
    EN_ANA_TYP_S,                   // 视在功率
    EN_ANA_TYP_COS,                 // 功率因素
    EN_ANA_TYP_DC_U,                // 直流电压
    EN_ANA_TYP_DC_I,                // 直流电流
    EN_ANA_TYP_GEAR,                // 档位
    EN_ANA_TYP_FR,                  // 频率
    EN_ANA_TYP_T,                   // 温度
    EN_ANA_TYP_DU,					// 直流
    EN_ANA_TYP_MC,					// 脉冲
    EN_ANA_TYP_AMP,
    EN_ANA_TYP_ST,                  // 设备态
    EN_ANA_TYP_IMP,                 // 阻抗
};

//===================================================================================================
//
// 以下常量需用户定义
//
//===================================================================================================

enum _INDEX_VALUE_TYPE_{

    EN_SOFT_SET,
    EN_FIGURE_SET,
};

//============================================================================
//开出量菜单对应wParameter执行菜单参数（应用于菜单常量表）
//============================================================================
enum    _OUTPUT_PARA_  {
    EN_OUTPUT_0 = 1,
    EN_OUTPUT_1,
    EN_OUTPUT_2,
    EN_OUTPUT_3,
};

enum _REV_TYPE_                             // 复归类型
{
    EN_REV_FREE,                            // 非强制复归
    EN_REV_FORCE,                           // 强制复归
};

enum _MENUCREAT_TYPE_ {                     // 建立菜单链表类型
    EN_SEESET_SOFT_MENU = 0x7c,             // 查看定值菜单类型
    EN_SETVAL_SOFT_MENU = 0xc7,             // 整定定值菜单类型
    EN_SEESET_FIGURE_MENU = 0x77,           // 查看定值菜单类型
    EN_SETVAL_FIGURE_MENU = 0xcc,           // 整定定值菜单类型
    EN_SEESET_PARA_MENU = 0x66,           	// 查看定值菜单类型
    EN_SETVAL_PARA_MENU = 0xBB,           	// 整定定值菜单类型
};

//============================================================================
//实时数据类型标志
//============================================================================
enum _MEA_TYPE_{
    EN_MEA_RLY,                             // 保护交流量1
    EN_MEA_RLY2,                            // 保护交流量2
    EN_MEA_RLY3,                            // 保护交流量3
    EN_MEA_ANA,                             // 保护测量量1
    EN_MEA_ANA2,                            // 保护测量量2
    EN_MEA_ANA3,                            // 保护测量量3
    EN_MEA_DC,                              // 保护直流量
    EN_MEA_SYN,                             // 保护谐波量
    EN_MEA_POWER,                           // 保护电能量
    EN_MEA_GEAR,                            // 保护档位量
    EN_MEA_TQ,                              // 保护同期量
    EN_MEA_INPUT1,                     		// 保护交流量1
    EN_MEA_INPUT2,                          // 保护交流量2
    EN_MEA_INPUT3,                          // 保护交流量3
	EN_INPUT_RLY_ALL,
	EN_INPUT_RLY_FAULT,
	EN_INPUT_RLY_OTHER,
	EN_INPUT_BS_ALL,
	EN_INPUT_BS_FAULT,
	EN_INPUT_BS_OTHER,
	EN_MEA_AC,								// 遥测交流量
	EN_MEA_ADJ,								// 遥测微调系数
	EN_MEA_YX,								// 遥信状态
	EN_OUTPUT_TRIP, 						// 保护出口调试
	EN_OUTPUT_SIGN, 						// 信号出口调试
	EN_MEA_LS,                  			//联锁信号.zhanggl
	EN_MEA_SCRLY,               			//顺控配置信号.zhanggl
};

enum _NO_USER_PASSWORD_{
    EN_NO_USER_PWD = 0x55,
};

enum _FACTORY_PASSWORD_{
    EN_FACTORY_PASSWORD = 0x55,
};

//==============================================================================
// 菜单常量表相关常量

//============================================================================
//交流量菜单对应wParameter执行菜单参数（应用于菜单常量表）
//============================================================================
enum    _ANA_PARA_  {
    EN_ANA_0 = 1,
    EN_ANA_1    ,
};

//============================================================================
//开入量菜单对应wParameter执行菜单参数（应用于菜单常量表）
//============================================================================
enum    _INPUT_PARA_  {
    EN_INPUT_0 = 1,
    EN_INPUT_1,
};

enum _ANA_TYPE_
{
	EN_TYPE_DIF_CURRENT=0 ,
	EN_TYPE_UNIT_CURRENT ,
	EN_TYPE_UNIT_VOLTAGE ,

};

//==========================================    //
// 字符串代号常量定义
enum _STR_FLAG_ {
    EN_STR_FLAG_FUNSET = 0,                     // 投入,退出
    EN_STR_FLAG_OKCANCEL,
    EN_VALUE_FLAG_SLOTTYPE,                     // 板件类型
    EN_VALUE_FLAG_YESNO,                        // 是否
    EN_VALUE_FLAG_CHN_NUM,                      // 中文数字
    EN_VALUE_FLAG_YXSET,                        // 遥信设置
    EN_VALUE_FLAG_YKSTEP,                       // 遥控步骤
    EN_VALUE_FLAG_YKACTION,                     // 遥控操作类型
    EN_VALUE_FLAG_CHK,                          // 自检信息
    EN_VALUE_FLAG_BEUSED,                       // 是否使用
    EN_VALUE_FLAG_PORTTYPE,                     // 端口类型
    EN_VALUE_FLAG_MAPNAME,                      // 转发表名称
    EN_VALUE_FLAG_RECTITLE,                     // 记录类型表
    EN_VALUE_FLAG_BHINFO,                       // 保护动作/复归
    EN_VALUE_FLAG_BHSET,                        // 保护动作/复归
    EN_VALUE_FLAG_OKCANCEL,                     // 确定/取消
    EN_VALUE_FLAG_FILETYPE,                     // 文件类型
    EN_VALUE_FLAG_ROLE,                         // 备份角色
    EN_VALUE_FLAG_COMMSTATE,                    // 通讯状态
    EN_VALUE_FLAG_ALARM,                        // 告警状态
    EN_VALUE_FLAG_YXTYPE,                       // 遥信类型
    EN_STR_FLAG_MENUALAUTO,
    EN_STR_FLAG_LINKBREAK,
    EN_STR_FLAG_DONEUNDONE,
    EN_STR_FLAG_ACTBACK,
    EN_STR_FLAG_RESUMEBREAK,
    EN_STR_FLAG_USESTOP,
    EN_STR_FLAG_BAUDRATE,
    EN_STR_FLAG_VERIFY,
    EN_STR_FLAG_PROTOL,
    EN_STR_FLAG_RECORD,
    EN_STR_FLAG_MESSAGE,
    EN_STR_FLAG_COM,
    EN_STR_FLAG_MODE,
    EN_STR_FLAG_COMBAUD,
    EN_STR_FLAG_DATABIT,
    EN_STR_FLAG_STOPBIT,
    EN_STR_FLAG_COMTYPE,
    EN_STR_FLAG_SAMEV,
    EN_STR_FLAG_PT,
    EN_STR_FLAG_CT,
    EN_STR_FLAG_TRANSTYPE,
    EN_STR_FLAG_LINEPHS,
    EN_STR_FLAG_RETOUT,
    EN_STR_FLAG_YXDI,
    EN_STR_FLAG_YKPUL,
    EN_STR_FLAG_WAVE,
    EN_STR_FLAG_MEMTYPE,
    EN_STR_FLAG_CHANNEL,
    EN_STR_FLAG_POWERTYPE,
    EN_STR_FLAG_YCCOMMTYPE,
    EN_STR_FLAG_YXCOMMTYPE,
//    EN_STR_FLAG_HARMOTYPE,
//    EN_STR_FLAG_DWINTYPE,
    EN_STR_FLAG_CTRLWORD,
    EN_STR_FLAG_BUSNAME,
    EN_STR_FLAG_UNITSHOWMODE,
    EN_STR_FLAG_BAYMODE,
    EN_STR_FLAG_BASEANA,
    EN_STR_FLAG_BASEPHASE,
    EN_STR_FLAG_BAYCODE,
    EN_STR_FLAG_SET_NUM,
    EN_STR_FLAG_DCTYPE,
    EN_STR_FLAG_DCINTYPE,
    EN_STR_FLAG_DWINTYPE,
    EN_STR_FLAG_PTCT,
    EN_STR_FLAG_HARMOTYPE,
    EN_STR_FLAG_SHOWANATYPE,
    EN_STR_FLAG_ANAPOLARTYPE,						// 交流通道极性
}enumStrType;

/*================================================================================*/
// Const_Soft.h 常量固定部分
/*================================================================================*/
// 软压板类型
enum _SOFT_TYPE_NUMBER
{
    EN_SOFT_PRO = 0,                            // 保护功能软压板
    EN_SOFT_GOOSE,                              // GOOSE软压板
    EN_SOFT_MU,                                 // MU软压板
    EN_SOFT_SWITCH,                             // 刀闸强制软压板
    EN_SOFT_BAK,                                // 备用软压板
//----------------------------------------------//
    EN_SOFT_TYPE_END,                           // 软压板类型总数目,不可改动
};
#define CN_NUM_SOFT_TYPE        (EN_SOFT_TYPE_END)  // 软压板类别数
// 装置软压板
#define CN_SUM_SOFT             (EN_SOFT_END)       // 软压板总数


enum _SET_SIDE_TYPE_                            // 定值类别
{
    EN_SIDE_START = 0,                          // 备用侧
    EN_SIDE_BASIC,                              // 基本信息
    EN_SIDE_DEVINF,                             // 装置参数
    EN_SIDE_COP,                                // 内部定值
    EN_SIDE_MATRIX,                             // 内部定值 出口矩阵
    EN_SIDE_ALL,                                // 全侧
    EN_SIDE_HIGH,                               // 高压侧
    EN_SIDE_MED1,                               // 中压侧
    EN_SIDE_MED2,                               // 中压侧
    EN_SIDE_LOW1,                               // 低压1侧
    EN_SIDE_LOW2,                               // 低压2侧
    EN_SIDE_LK,                                 // 电抗器
    EN_SIDE_Z,                                  // Z变
    EN_SIDE_DEF,                                // 自定义
    EN_SIDE_NONE = 0xFF,                        // 不存在
};

enum _REC_TYPE_
{
    EN_ACT_REC = 0,                             // 事件记录
    EN_ALM_REC,                                 // 告警记录
    EN_CHK_REC,                                 // 自检记录
    EN_SOE_REC,                                 // SOE记录
    EN_COS_REC, 								// COS记录
    EN_LOCK_REC,                                // 瞬时闭锁记录
    EN_OVER_REC,                                // 越限记录
    EN_START_REC,                               // 启动记录
    EN_RUN_REC,                                 // 运行记录
    EN_INPUT_REC,                               // 开入变位记录
    EN_ONOFF_REC,                               // 保护投退记录
    EN_OPT_REC,                                 // 保护操作记录
    EN_YK_REC,                                 	// 保护遥控记录
    EN_SC_REC,                                 	// 装置顺控记录
    EN_SCSTEPINFO_REC,                          // 装置顺控单步记录
    EN_FAULT_REC,                           	// 事故报告记录
    EN_ACTWAVE_REC,                             // 动作录波信息记录
    EN_STARTWAVE_REC,                           // 启动录波信息记录
    EN_HANDWAVE_REC,                            // 手动录波信息记录
	EN_FAULT_NO,								// 故障序号
    EN_ALL_REC = 0xFF,                      	// 所有记录
    EN_NO_REC  = 0xFFFF,                    	// 无效记录
};

//============================================================================
//打印类型码
//============================================================================
enum _PRN_TYPE{
    EN_PRINT_EQUIPINF =1,           // 打印装置信息
    EN_PRINT_ACTINF,                // 打印保护动作记录
    EN_PRINT_ALARMINF,              // 打印保护告警记录
    EN_PRINT_STARTINF,              // 打印保护启动记录
    EN_PRINT_LOCKINF,               // 打印保护闭锁记录
    EN_PRINT_SETINF,                // 打印定值信息
    EN_PRINT_ACTWAVEINF,            // 打印动作录波信息
    EN_PRINT_HANDWAVEINF,           // 打印手动录波信息
    EN_PRINT_STARTWAVEINF,          // 打印启动录波信息
    EN_PRINT_ACTWAVEREPORT,         // 打印动作报告
    EN_PRINT_DSPINF,                // 打印屏幕显示信息
    EN_PRINT_UNITINF,               // 打印母差间隔信息
	EN_PRINT_SOEINF,				// 打印开入信息
	EN_PRINT_COSINF,				// 打印COS信息
    EN_PRINT_RUNINF,                // 打印运行信息
    EN_PRINT_OPTINF,                // 打印操作信息
    EN_PRINT_CHKINF,                // 打印保护自检记录
    EN_PRINT_ONOFFINF,              // 打印投退信息
    EN_PRINT_PARASETINF,            // 打印内部参数定值信息
    EN_PRINT_ANALOG,                // 打印交流量
    EN_PRINT_ACT_RPT,               // 国网标准事故报告
    EN_PRINT_RUN_RPT,               // 国网标准运行报告
    EN_PRINT_SPECIAL_SET,           // 国网标准内部定值
    EN_PRINT_SETINF_COMM,           // 通讯定值
    EN_PRINT_YCINF,      	        // 遥测信息
    EN_PRINT_KI,      	            // 开入量实时值
    EN_PRINT_SOFTYB,      	        // 软压板
    EN_PRINT_OVER,      	        // 越限记录
    EN_PRINT_YK,      	            // 遥控记录

    EN_PRINT_IDLE = 0XFF,           // 空闲
};

/*================================================================================*/
#define CN_NO_STATE             (0x5C5C)            // 外部通讯无状态标志
#define	CN_COP_PWD				(483)				// 厂家密码:可修改保护元件配置、查看内存等
#define	CN_USER_PWD				(800)				// 用户超级密码:可用于修改普通密码、定值，预设等

/*================================================================================*/

#endif
