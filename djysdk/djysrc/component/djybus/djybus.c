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
// 文件名     ：djybus.c
// 模块描述: djybus模块主要对DJYOS总线管理，将所有总线分类，形成节点分别挂到总线树
//           "DjyBus"上面，每种类型可能有多条总线，而每条总线可能也有多个器件，形成
//           器件、总线、总线类型、“DjyBus”从树叶到树根的总线树。
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 17/06.2014
// =============================================================================

#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "object.h"

static struct Object tg_djybus_root;                //定义静态变量DjyBus的根结点
// =============================================================================
// 功能：建立并初始化DjyBus总线根节点，它是总线类型节点的父结点
// 参数：para,无实际意义
// 返回：true=成功, false=失败.
// =============================================================================
bool_t ModuleInstall_DjyBus (ptu32_t Para)
{
    //在资源链表中建立一个根结点，所有建立的总线结点都挂在该结点上。
    if(OBJ_AddTree(&tg_djybus_root,sizeof(struct  Object),RSC_RSCNODE,"DjyBus"))
    {
        printk("DjyBus Install Successed !\r\n");
        return true;
    }
    return false;
}

// =============================================================================
// 功能：在DjyBus总线树结点上增加新的总线类型结点，新增总线类型结点是DjyBus的子结点,
//       使用内存池分配的方式分配内存
// 参数：NewBusTypeName,总线类型名称
// 返回：返回建立的资源结点指针，失败时返回NULL
// =============================================================================
struct Object * DjyBus_BusTypeAdd (const char* NewBusTypeName)
{
    struct Object * NewBusType;

    NewBusType = (struct Object *)M_Malloc(sizeof(struct Object),0);
    if(NewBusType == NULL)
        return NULL;

    //避免重复创建同名的总线类型
    if(NULL != OBJ_SearchChild(&tg_djybus_root,NewBusTypeName))
        return NULL;

    OBJ_AddChild(&tg_djybus_root,NewBusType,
                sizeof(struct Object),RSC_RSCNODE,NewBusTypeName);

    return NewBusType;
}

// =============================================================================
// 功能：在DjyBus总线树结点上增加新的总线类型结点，新增总线类型结点是DjyBus的子结点,
//       该函数带r，表示由调用者提供内存，即NewBusType由用户定义
// 参数：NewBusTypeName,创建总线类型名称
//       NewBusType,创建总线类型结点，用记定义
// 返回：返回建立的资源结点指针，失败时返回NULL
// =============================================================================
struct Object * DjyBus_BusTypeAdd_s(struct Object * NewBusType,
                                    const char* NewBusTypeName)
{
    if((NewBusType == NULL) && (NewBusTypeName == NULL))
        return NULL;

    //避免重复创建同名的总线类型
    if(NULL != OBJ_SearchChild(&tg_djybus_root,NewBusTypeName))
        return NULL;

    OBJ_AddChild(&tg_djybus_root,NewBusType,
                sizeof(struct Object),RSC_RSCNODE,NewBusTypeName);

    return NewBusType;
}

// =============================================================================
// 功能：删除DjyBus总线根结点上的一个总线类型结点，需释放动态分配的内存块，,该类型结
//       点不能有子结点
// 参数：DelBusType,待删除的总线类型结点
// 返回：TRUE,删除成功;false,删除失败
// =============================================================================
bool_t DjyBus_BusTypeDelete(struct Object * DelBusType)
{
    bool_t result;
    if(DelBusType == NULL)
        return false;

    if(NULL == OBJ_Del(DelBusType))
    {
        result = false;
    }
    else
    {
        free(DelBusType);
        result = true;
    }
    return result;
}

// =============================================================================
// 功能：删除DjyBus总线根结点上的一个总线类型结点,该类型结点不能有子结点
// 参数：DelBusType,待删除的总线类型结点
// 返回：TRUE,删除成功;false,删除失败
// =============================================================================
bool_t DjyBus_BusTypeDelete_s(struct Object * DelBusType)
{
    bool_t result;
    if(DelBusType == NULL)
        return false;

    if(NULL == OBJ_Del(DelBusType))
    {
        result = false;
    }
    else
    {
        result = true;
    }
    return result;
}

// =============================================================================
// 功能：根据总线类型名称查找总线类型节点，返回查找到的结点指针
// 参数：BusTypeName,待查找的总线类型结点名称
// 返回：结点指针，NULL时查找失败
// =============================================================================
struct Object * DjyBus_BusTypeFind(const char * BusTypeName)
{
    return OBJ_SearchChild(&tg_djybus_root,BusTypeName);
}

