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
// 模块描述: DJY菜单模块之用户菜单意愿数据
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================


#include  "MenuData.h"
#include  "MenuFunction.h"


// =========================================================================
// 说明：在目前阶段，采用的是静态数组的方式来存储用户的菜单数据意愿，关于惨淡意愿的各项
//      含义可以参考相关结构体的定义，0,1,2,3代表的是菜单的层次结构。最后一行名字为空
//      代表着结束
// =========================================================================
const  tagMenuitemModel g_tMenuitemModelTab[]=
{
    {   0,      "查看",         "",                 0,                      0x0000,         0,              "Menuitem_func"            },
    {   1,      "装置信息",     "查看装置信息",     0,                      0x0000,         0,              "CheckEquipment"      },
    {   1,      "交流量",       "查看交流量",       EN_MEA_RLY,             0x0000,         EN_ANA_0,      "Menuitem_func"       },
    {   1,      "开入量",       "查看开入量",       EN_MEA_INPUT1,          0x0000,         EN_INPUT_0,     "Menuitem_func"       },
    {   1,      "定值",         "查看定值",         0,                      0x0000,         0,              "Menuitem_func"            },
    {     2,    "设备参数",     "",                 EN_FIGURE_SET,          0x0000,         EN_SIDE_DEVINF, "Menuitem_func"          },
    {     2,    "差动保护",     "查看差动保护定值", 0,                      0x0000,         0,                  "Menuitem_func"         },
    {       3,    "数值定值",     "", EN_FIGURE_SET,          0x0000,  EN_SIDE_ALL, "Menuitem_func"         },
    {       3,    "控制字",       "", EN_SOFT_SET,            0x0000,  EN_SIDE_ALL, "Menuitem_func"         },
    {     2,    "高压侧后备保护", "查看定值",                           0,          0x0000,         0,                 "Menuitem_func"            },
    {       3,    "数值定值",     "", EN_FIGURE_SET,          0x0000,  EN_SIDE_HIGH, "Menuitem_func"        },
    {       3,    "控制字",       "", EN_SOFT_SET,            0x0000,  EN_SIDE_HIGH, "Menuitem_func"         },
    {     2,    "中压侧后备保护", "查看定值",                           0,          0x0000,         0,                  "Menuitem_func"            },
    {       3,    "数值定值",     "", EN_FIGURE_SET,          0x0000,  EN_SIDE_MED1, "Menuitem_func"         },
    {       3,    "控制字",       "", EN_SOFT_SET,            0x0000,  EN_SIDE_MED1, "Menuitem_func"         },
    {     2,    "低1分支后备保护", "查看定值",                           0,         0x0000,         0,                  "Menuitem_func"            },
    {       3,    "数值定值",     "", EN_FIGURE_SET,          0x0000,  EN_SIDE_LOW1, "Menuitem_func"         },
    {       3,    "控制字",       "", EN_SOFT_SET,            0x0000,  EN_SIDE_LOW1, "Menuitem_func"         },
    {     2,    "低2分支后备保护", "查看定值",                           0,         0x0000,         0,                 "Menuitem_func"          },
    {       3,    "数值定值",     "", EN_FIGURE_SET,          0x0000,  EN_SIDE_LOW2, "Menuitem_func"         },
    {       3,    "控制字",       "", EN_SOFT_SET,            0x0000,  EN_SIDE_LOW2, "Menuitem_func"         },
//    {     2,    "电抗器后备保护",   "查看定值",                          0,         0x0000,         0,                  "Menuitem_NonPfunc"            },
//    {       3,    "数值定值",     "", EN_FIGURE_SET,          0x0000,  EN_SIDE_LK,     "MenuitemProc_See_Set"         },
//    {       3,    "控制字",       "", EN_SOFT_SET,            0x0000,  EN_SIDE_LK,     "MenuitemProc_See_Set"         },
//    {     2,    "自定义定值",     "查看定值",                           0,          0x0000,         0,                  "Menuitem_NonPfunc"            },
//    {       3,    "数值定值",     "", EN_FIGURE_SET,          0x0000,  EN_SIDE_DEF, "MenuitemProc_See_Set"         },
//    {       3,    "控制字",       "", EN_SOFT_SET,            0x0000,  EN_SIDE_DEF, "MenuitemProc_See_Set"         },
    {     2,    "跳闸矩阵",       "查看控制字定值", EN_FIGURE_SET,          0x0000,  EN_SIDE_MATRIX,    "Menuitem_func"         },
//  {     2,    "厂家定值",       "查看控制字定值", EN_SOFT_SET,            0x0000,  EN_SIDE_COP,        "Menuitem_func"         }
    {   1,      "软压板",       "",             0, 0x0000,         0,              "Menuitem_func"            },
    {   2,      "保护软压板",   "查看保护功能软压板",   0,                  0x0000,         EN_SOFT_PRO,   "Menuitem_func"         },     
    {   2,      "GOOSE软压板",  "查看GOOSE软压板",      0,                  0x0000,         EN_SOFT_GOOSE,  "Menuitem_func"         },
    {   1,      "遥测信息",     "查看遥测信息",     0,                      0x0000,         0,              "Menuitem_func"             },
    {     2,    "交流量",       "查看交流量",       EN_MEA_AC,              0x0000,         EN_INPUT_0,     "Menuitem_func"           },
//    {     2,    "直流量",       "查看直流量",       EN_MEA_DC,              0x0000,         EN_INPUT_0,     "MenuitemProc_See_YC"           },

 
    {     2,    "电度量",       "查看电度量",       EN_MEA_POWER,           0x0000,         EN_INPUT_0,     "Menuitem_func"           },
     {     2,    "谐波量",       "查看谐波量",       EN_MEA_SYN,             0x0000,         EN_INPUT_0,     "Menuitem_func"       },
  
    {     2,    "档位量",       "查看档位量",       EN_MEA_GEAR,            0x0000,         EN_INPUT_0,     "Menuitem_func"           },
    {   1,      "遥信信息",     "查看遥信信息",     EN_MEA_YX,              0x0000,         EN_INPUT_0,    "Menuitem_func"        },
    {   1,      "联锁信息",     "查看联锁信息",     EN_MEA_LS,              0x0000,         0,             "Menuitem_func"           },
    {   1,      "顺控信息",     "查看顺控信息",     0,                      0x0000,         0,             "Menuitem_func"             },
    {     2,    "操作票配置信息",   "查看操作票配置信息",   EN_MEA_SCRLY,   0x0000,         0,              "Menuitem_func"     },
    {   1,  "装置记录",     "查看各种装置记录",          0,                      0x0000,         0,                  "Menuitem_func"            },
    {     2,    "运行报告",     "",                 0,                      0x0000,         0,              "Menuitem_func"    },
    {     2,    "保护动作",     "",                 0,                      0x0000,         0,              "Menuitem_func"       },
    {     2,    "保护告警",     "",                 0,                      0x0000,         0,              "Menuitem_func"      },
//    {     2,    "保护越限",     "",                 0,                      0x0000,         0,            "Menuitem_func"     },
    {     2,    "保护启动",     "",                 0,                      0x0000,         0,             "Menuitem_func"    },
//  {     2,    "保护投退",     "",                 0,                      0x0000,         0,              "MenuitemProc_See_RecOnoff"     },
    {     2,    "SOE变位",      "",                 0,                      0x0000,         0,              "Menuitem_func"       },
    {     2,    "遥控记录",     "查看遥控操作记录", 0,                      0x0000,         0,              "Menuitem_func"        },
    {     2,    "顺控记录",     "查看顺控执行记录", 0,                      0x0000,         0,             "Menuitem_func"       },
    {     2,    "顺控单步",     "查看顺控单步信息", 0,                      0x0000,         0,             "Menuitem_func"},
    {     2,    "装置自检",     "",                 0,                      0x0000,         0,              "Menuitem_func"       },
    {     2,    "装置运行",     "",                 0,                      0x0000,         0,              "Menuitem_func"      },
//    {     2,    "瞬时闭锁",     "",                 0,                      0x0000,         0,              "MenuitemProc_See_RecLock"      },
    {     2,    "装置操作",     "",                 0,                      0x0000,         0,              "Menuitem_func"       },
    {   1,      "录波信息",     "查看各种录波信息", 0,                      0x0000,         0,              "Menuitem_func"            },
    {     2,    "动作录波",     "",                 EN_ACTWAVE_REC,         0x0000,         0,             "Menuitem_func"         },
    {     2,    "起动录波",     "",                 EN_STARTWAVE_REC,       0x0000,         0,              "Menuitem_func"         },
    {     2,    "手动录波",     "",                 EN_HANDWAVE_REC,        0x0000,         0,              "Menuitem_func"        },
//    {  1,       "自检状态",     "",                 0,                      0x0000,         0,              "Menuitem_NonPfunc"             },
//    {     2,    "主保护",       "",                 0,                      0x0000,         0,              "MenuitemProc_See_ChkInfo"      },
//    {     2,    "后备保护",     "",                 1,                      0x0000,         0,              "MenuitemProc_See_ChkInfo"      },
    {0,       "整定",     "",                               0,                      0x0000,         0            ,      "Menuitem_func"            },
    {   1,      "定值区号",     "",                 0,                      CN_USER_PWD,    0,              "Menuitem_func"  },
    {   1,      "设备参数",     "整定设备参数",     EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_DEVINF, "Menuitem_func"        },
    {   1,      "差动保护",     "整定差动保护定值", 0,                      0x0000,         0,              "Menuitem_func"             },
    {     2,    "数值定值",     "",                 EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_ALL,   "Menuitem_func"        },
    {     2,    "控制字",       "",                 EN_SOFT_SET,            CN_USER_PWD,    EN_SIDE_ALL,    "Menuitem_func"        },
    {   1,      "高压侧后备保护",   "整定定值",     0,                      0x0000,         0,              "Menuitem_func"            },
    {     2,    "数值定值",     "",                 EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_HIGH,   "Menuitem_func"        },
    {     2,    "控制字",       "",                 EN_SOFT_SET,            CN_USER_PWD,    EN_SIDE_HIGH,   "Menuitem_func"       },
    {   1,      "中压侧后备保护",   "整定定值",     0,                      0x0000,         0,              "Menuitem_func"             },
    {     2,    "数值定值",     "",                 EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_MED1,  "Menuitem_func"        },
    {     2,    "控制字",       "",                 EN_SOFT_SET,            CN_USER_PWD,    EN_SIDE_MED1,   "Menuitem_func"        },
    {   1,      "低1分支后备保护",  "整定定值",     0,                      0x0000,         0,              "Menuitem_func"             },
    {     2,    "数值定值",     "",                 EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_LOW1,   "Menuitem_func"        },
    {     2,    "控制字",       "",                 EN_SOFT_SET,            CN_USER_PWD,    EN_SIDE_LOW1,  "Menuitem_func"       },
    {   1,      "低2分支后备保护",  "整定定值",     0,                      0x0000,         0,              "Menuitem_func"             },
    {     2,    "数值定值",     "",                 EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_LOW2,   "Menuitem_func"       },
    {     2,    "控制字",       "",                 EN_SOFT_SET,            CN_USER_PWD,    EN_SIDE_LOW2,   "Menuitem_func"       },
//    {   1,      "电抗器后备保护",   "整定定值",     0,                      0x0000,         0,              "Menuitem_NonPfunc"             },
//    {     2,    "数值定值",     "",                 EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_LK,     "MenuitemProc_Set_Value"        },
//    {     2,    "控制字",       "",                 EN_SOFT_SET,            CN_USER_PWD,    EN_SIDE_LK,     "MenuitemProc_Set_Value"        },
//    {   1,      "自定义定值",   "整定定值",         0,                      0x0000,         0,              "Menuitem_NonPfunc"             },
//    {     2,    "数值定值",     "",                 EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_DEF,    "MenuitemProc_Set_Value"        },
//    {     2,    "控制字",       "",                 EN_SOFT_SET,            CN_USER_PWD,    EN_SIDE_DEF,    "MenuitemProc_Set_Value"        },
    {   1,      "跳闸矩阵",     "设置跳闸矩阵",     EN_FIGURE_SET,          CN_USER_PWD,    EN_SIDE_MATRIX, "Menuitem_func"        },
   
    {     2,    "保护软压板",   "设置保护功能软压板",   0,                  CN_USER_PWD,    EN_SOFT_PRO,    "Menuitem_func"         },
    {     2,    "GOOSE软压板",  "设置GOOSE软压板",  0,                      CN_USER_PWD,    EN_SOFT_GOOSE,  "Menuitem_func"        },
    {   1,      "定值拷贝",     "",                 0,                      CN_USER_PWD,    0,              "Menuitem_func"      },
    {   0,      "预设",         "",                         0,              0x0000,         0,             "Menuitem_func"            },
    {   1,      "时钟设置",     "设置系统时钟",             0,              CN_USER_PWD,    0,              "Menuitem_func"         },
    {   1,  "MU参数配置",   "设置MU参数值",                 0,                      0x0000,         0,                  "Menuitem_func"             },
//    {   2,  "同步延时",     "设置MU同步延时",               0,                      CN_USER_PWD,    0,                  "MenuitemProc_Cfg_MuDelay"},  //modified by zhanggl.110624
//    {   2,  "同步时钟源",   "设置有无同步时钟源",           0,                      CN_USER_PWD,    0,                  "MenuitemProc_Cfg_MuSyn"},  //modified by zhanggl.111027
    {   2,  "MU通道极性",   "设置MU采样值通道的接入极性",   0,                      CN_USER_PWD,    0,             "Menuitem_func"    },  //...liuyf:2010-9-3
//    {   1,      "相位基准",     "设置交流量显示时相位基准", 0,              CN_USER_PWD,    0,              "MenuitemProc_Cfg_AngleBase"    },
    {   1,      "遥控脉宽",     "设置遥控出口脉宽",         0,              CN_USER_PWD,    0,              "Menuitem_func"      },
    {   1,      "遥测设置",     "遥测参数设置",             0,              0x0000,         0,              "Menuitem_func"            },
    {     2,    "功率计算方式", "设置功率计算方式",         0,              CN_USER_PWD,    0,              "Menuitem_func"      },
    {     2,    "直流变送器参数",   "设置直流变送器的参数", 0,              CN_USER_PWD,    0,              "Menuitem_func"     },
    {     2,    "档位变送器参数",   "设置档位变送器的参数", 0,              CN_USER_PWD,    0,              "Menuitem_func"     },
    {     2,    "PTCT设置",     "设置PTCT 额定值",          0,              CN_USER_PWD,    0,              "Menuitem_func"      },
    {     2,    "通讯上送类别", "设置遥测量的通讯上送类别", 0,              CN_USER_PWD,    0,              "Menuitem_func"   },
    {     2,    "遥测量死区门槛",   "设置遥测量死区门槛",   0,              CN_USER_PWD,    0,              "Menuitem_func"   },
    {     2,    "遥测量微调系数",   "设置遥测量的微调系数", EN_MEA_ADJ,     CN_USER_PWD,    EN_ANA_0,       "Menuitem_func"     },
    {     1,    "遥信设置",     "遥信参数设置",             0,              0x0000,         0,             "Menuitem_func"             },
    {     2,    "遥信类型",     "设置遥信类型",             0,              CN_USER_PWD,    0,             "Menuitem_func"   },
    {     2,    "遥信防抖时间", "设置遥信防抖时间",         0,              CN_USER_PWD,    0,             "Menuitem_func"     },
    {     2,    "双点遥信",     "设置双点遥信虚端子",       0,              CN_USER_PWD,    0,              "Menuitem_func"      },
    {   1,      "虚拟功能",     "设置各种虚拟值",           0,              0x0000,         0,              "Menuitem_func"            },
    {     2,    "虚拟遥测",     "设置虚拟遥测值",           0,              0x0000,         0,             "Menuitem_func"            },
    {       3,  "交流虚遥测",   "设置虚拟交流遥测值",       EN_MEA_AC,      CN_USER_PWD,    EN_ANA_0,       "Menuitem_func"       },
    {       3,  "直流虚遥测",   "设置虚拟直流遥测值",       EN_MEA_DC,      CN_USER_PWD,    EN_ANA_0,       "Menuitem_func"         },
    {       3,  "电度虚遥测",   "设置虚拟电度遥测值",       EN_MEA_POWER,   CN_USER_PWD,    EN_ANA_0,       "Menuitem_func"        },
    {       3,  "档位虚遥测",   "设置虚拟档位遥测值",       EN_MEA_GEAR,    CN_USER_PWD,    EN_ANA_0,       "Menuitem_func"        },
    {     2,    "虚拟遥信",     "设置虚拟遥信值",           0,              CN_USER_PWD,    0,              "Menuitem_func"         },
    {     2,    "虚拟事件",     "设置虚拟事件",             0,              0x0000,         0,              "Menuitem_func"            },
    {       3,  "动作事件",     "设置虚拟动作事件",         EN_ACT_REC,     CN_USER_PWD,    0,              "Menuitem_func"      },
    {       3,  "告警事件",     "设置虚拟告警事件",         EN_ALM_REC,     CN_USER_PWD,    0,              "Menuitem_func"      },
    {   1,      "五防顺控",     "",                         0,              0x0000,         0,              "Menuitem_func"            },
    {     2,    "五防投退",     "五防投退总设置",           0,              CN_USER_PWD,    0,              "Menuitem_func"       },
    {     2,    "五防控制字",   "设置各通道五防投退",       0,              CN_USER_PWD,    0,             "Menuitem_func"    },
    {     2,    "顺控投退",     "顺控投退总设置",           0,              CN_USER_PWD,    0,              "Menuitem_func"       },
    {   1,      "GOCB控制字",  "设置GOCB网络接收使能",      0,              CN_USER_PWD,    0,             "Menuitem_func"  },  //增加GOCB网络接收使能
    {   1,      "保护功能",     "",                         0,              0x0000,         0,              "Menuitem_func"             },
//    {     2,    "复归事件",     "不可复归未返回事件",       EN_REV_FREE,    CN_USER_PWD,    0,              "MenuitemProc_Cfg_RevEvent"     },
    {     2,    "强制复归",     "可复归未返回事件",         EN_REV_FORCE,   CN_USER_PWD,    0,              "Menuitem_func"    },
    {     2,    "手动录波",     "启动手动录波",             0,              CN_USER_PWD,    0,              "Menuitem_func"  },
    {     2,    "清除记录",     "",                         0,              CN_USER_PWD,    0,              "Menuitem_func"       },
    {   1,      "通讯功能",     "",                         0,              0x0000,         0,              "Menuitem_func"           },
    {     2,    "通讯设置",     "外部通讯设置",             0,              CN_USER_PWD,    0,              "Menuitem_func"     },
    {     2,    "装置编号",     "设置装置号",               0,              CN_USER_PWD,    0,              "Menuitem_func"   },
//    {     2,    "装置检修",     "投入时不上传事件",         0,              CN_USER_PWD,    0,              "MenuitemProc_Cfg_DevRepair"    },
    {     2,    "网口设置",     "",                         0,              CN_USER_PWD,    0,              "Menuitem_func"       },
    {   0,      "调试",         "",                         0,              0x0000,         0,             "Menuitem_func"             },
//    {   1,      "交流量",       "",                         0,              0x0000,         0,              "Menuitem_NonPfunc"             },
//    {   1,      "交流量",       "",                         EN_MEA_RLY,     0x0000,         EN_ANA_0,       "MenuitemProc_Dbg_AnaValue"     },
//    {     2,    "后备保护",     "",                         EN_MEA_RLY2,    0x0000,         EN_ANA_1,       "MenuitemProc_Dbg_AnaValue"     },
//    {   1,      "开入量",       "",                         0,              0x0000,         0,              "Menuitem_NonPfunc"             },
//    {   1,      "开入量",       "",                         0,              0x0000,         EN_INPUT_0,     "MenuitemProc_Dbg_InputValue"   },
//    {     2,    "后备保护",     "",                         0,              0x0000,         EN_INPUT_1,     "MenuitemProc_Dbg_InputValue"   },
    {   1,      "动作出口",     "进入此菜单保护退出",       EN_OUTPUT_TRIP, CN_USER_PWD,    EN_INPUT_0,     "Menuitem_func"       },
    {   1,      "信号出口",     "进入此菜单保护退出",       EN_OUTPUT_SIGN, CN_USER_PWD,    0,              "Menuitem_func"        },
    {   0,      "打印",         "",                         0,                  0x0000,     0,             "Menuitem_func"           },
    {   1,      "装置信息",     "",                         0,                  0x0000,     0,             "Menuitem_func"         },
    {   1,      "交流量",       "",                         0,                  0x0000,     0,              "Menuitem_func"          },
//    {   1,      "开入状态",     "",                         0,                  0x0000,     0,              "Menuitem_Prn_InputInfo"        },
    {   1,      "定值信息",     "",                         0,                  0x0000,     0,              "Menuitem_func"       },
    {   1,      "保护记录",     "",                         0,                  0x0000,     0,              "Menuitem_func"       },
    {   1,      "波形信息",     "",                         0,                  0x0000,     0,              "Menuitem_func"      },
//    {   1,      "动作报告",     "",                         0,                  0x0000,     0,              "MenuitemProc_Prn_PrnAct"       },
    {   1,      "运行报告",     "",                         EN_PRINT_RUN_RPT,   0x0000,     0,              "Menuitem_func"      },
    {   1,      "事故报告",     "",                         EN_PRINT_ACT_RPT,   0x0000,     0,             "Menuitem_func"      },
    {   1,      "取消打印",     "",                         0,                  0x0000,     0,             "Menuitem_func"          },
    {   0,  "厂家",             "",                         0,                  0x0000,     0,             "Menuitem_func"             },
    {   1,  "交流显示方式",     "",                         0,                  0x0000,     0,              "Menuitem_func"  },
    {   1,  "调试信息",         "查看各种调试信息",         0,                  CN_COP_PWD, EN_NO_USER_PWD, "Menuitem_func"      },
    {   1,  "恢复默认",         "",                         0,                  0x0000,     0,              "Menuitem_func"             },
    {   2,  "恢复默认设置",     "",                         0,                  CN_COP_PWD, EN_NO_USER_PWD, "Menuitem_func"     },
    {   2,  "定值",             "",                         0,                  CN_COP_PWD, EN_NO_USER_PWD, "Menuitem_func"      },
    {   2,  "软压板",           "",                         0,                  CN_COP_PWD, EN_NO_USER_PWD, "Menuitem_func"     },
    {   1,  "瞬时闭锁",     "",                             0,                 0x0000,         0,                  "Menuitem_func"      },
    {   1,  "板件版本信息",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
    {   2,  "板件版本信息",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
    {   3,  "板件版本信息",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
    {   4,  "板件版本信息",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
    {   0,  "备1",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
    {   0,  "备2",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
    {   0,  "备3",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
	//{   0,  "备4",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
    //{   0,  "备5",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
    //{   0,  "备6",  "查看板件版本信息",         0,                    0x0000,         0,              "Menuitem_func"      },
	{   0,  "",  "",         0,                    0x0000,         0,              ""      },
};





