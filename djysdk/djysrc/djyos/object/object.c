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
//所属模块: 资源管理
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 创建、添加、移动、删除、搜索资源节点的服务
//其他说明:
//修订历史:
//4. 日期:20130220
//   作者:  罗侍田.
//   新版本号：v1.3.0
//   修改说明:
//      1.修正了rsc操作可能导致死锁的问题，bug是由王毛磊提交的。
//3. 日期:20090518
//   作者:  罗侍田.
//   新版本号：v1.2.0
//   修改说明:
//      1.在概念上，分开了树根和资源根，函数名也相应地调整了
//      2.增加rsc_get_root函数，原rsc_get_root改名为rsc_get_tree，且不再兼职
//      3.因有了rsc_get_root，取消1.1.0版本的"修改3"
//      4.补全了NULL参数检查
//2. 日期:20090131
//   作者:  罗侍田.
//   新版本号：v1.1.0
//   修改说明:
//      1.加了个总根节点，添加了模块初始化函数
//      2.用信号量保护并发访问安全
//      3.改进了遍历函数，允许用NULL代表遍历整个队列
//      4.修正了一些bug
//1. 日期: 2009-01-04
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stddef.h"
#include "string.h"
#include "int.h"
#include "object.h"
#include "lock.h"
//-------------资源链算法说明----------------------
//各同级节点连成双向循环链表
//每个节点的child指针直接指向的节点称为该节点的所在队列的队列头，它的previous节点
//是队列尾。
//所有节点的Parent指针均指向父节点

//特别提示，资源队列的并发访问安全由资源管理模块负责，资源节点自身的并发访问
//          保护由使用者负责。
//对链表的修改都会使用原子操作保证链表的完备性
//对链表的读操作则不使用任何安全机制，由资源所属模块来保证资源竞争的安全性

//
//
//
#define ROOT_FILENAME       "/"
#define SYS_FILENAME        "sys"

//static struct Object s_tRscRoot;
static struct Object *s_ptRscRoot;
//static struct MutexLCB s_tMutexRscRoot;

//
// 根节点
//
struct VCommon g_tRootCom =
{
    .Context = NULL,
    .Name = ROOT_FILENAME,
};
//
// 系统节点
//
struct VCommon g_tSysCom =
{
    .Context = NULL,
    .Name = SYS_FILENAME,
};

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: 非NULL -- 成功; NULL -- 失败;
//备注: 给RSC个过渡使用的函数接口,注意此时系统锁还未建立
//-----------------------------------------------------------------------------
struct Object *RootObjInit(void)
{

    // object体系根初始化
    g_tRootCom.Obj.Type = RSC_RSCNODE;
    g_tRootCom.Obj.Size = sizeof(struct VCommon) + sizeof(ROOT_FILENAME);//+"/"字符串空间
    g_tRootCom.Obj.Name = g_tRootCom.Name;
    g_tRootCom.Obj.Inuse = 0;
    g_tRootCom.Obj.Child = g_tRootCom.Obj.Parent = NULL;
    g_tRootCom.Obj.Previous = g_tRootCom.Obj.Next = &g_tRootCom.Obj;// 原RSC逻辑


    // "/sys"体系初始化
    g_tSysCom.Obj.Type = RSC_RSCNODE;
    g_tSysCom.Obj.Size = sizeof(struct VCommon) + sizeof(SYS_FILENAME);// +"sys"字符串空间
    g_tSysCom.Obj.Name = g_tSysCom.Name;
    g_tSysCom.Obj.Inuse = 0;
    g_tSysCom.Obj.Parent = &g_tRootCom.Obj;
    g_tRootCom.Obj.Child = &g_tSysCom.Obj;
    g_tSysCom.Obj.Child = NULL;
    g_tSysCom.Obj.Previous = g_tSysCom.Obj.Next = &g_tSysCom.Obj;// 原RSC逻辑

    return (&g_tSysCom.Obj);// 系统中的其他组件挂载在"/sys"下
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注: 内部调用，不开放，为了将数据结构独立
//-----------------------------------------------------------------------------
struct Object *__RootObj(void)
{
    return (&(g_tRootCom.Obj));
}
//----初始化资源管理模块-------------------------------------------------------
//功能: 创建资源节点入口，本函数被分裂成两个，第二个函数必须在执行完锁初始化后
//      才能调用，第一个则必须在锁初始化执行前调用
//参数: 无
//返回: 1
//------------------------------------------------------------------------------
ptu32_t ModuleInstall_OBJ(ptu32_t Para)
{
    Para = Para;        //消除编译器告警
#if 0
    s_tRscRoot.Name = "resouce queue";
    s_tRscRoot.Child = NULL;
    s_tRscRoot.Next =&s_tRscRoot;
    s_tRscRoot.Previous =&s_tRscRoot;
    s_tRscRoot.Parent = NULL;
    s_tRscRoot.Size = sizeof(s_tRscRoot);
    s_tRscRoot.Type = RSC_RSCNODE;
    s_ptRscRoot = &s_tRscRoot;
#else
    s_ptRscRoot = RootObjInit();
#endif
    return 1;
}
//ptu32_t Rsc2_ModuleInit(ptu32_t para)
//{
//    para = para;        //消除编译器告警
//    Lock_MutexCreate_s(&s_tMutexRscRoot,"mutex for resouce queue");
//    return 1;
//}
//----添加根节点----------------------------------------------------------------
//功能: 在资源链中添加一个根节点(特供锁模块，因此时锁尚未初始化，)
//参数: Obj,新添加的节点指针
//      size，新节点连负载的尺寸，字节数
//      name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct Object *__Lock_RscAddLockTree(struct Object *Obj,
                                     u16 Size, const char *Name)
{
    struct Object *root_node;

    if(Obj == NULL)
        return NULL;

    Obj->Parent = s_ptRscRoot;
    Obj->Child = NULL;
    Obj->Size = Size;
    Obj->Type = RSC_RSCNODE;
    Obj->Name = (char*)Name;
    if(s_ptRscRoot->Child == NULL)
    {
        s_ptRscRoot->Child = Obj;
        Obj->Next = Obj;
        Obj->Previous = Obj;
    }
    else
    {
        root_node = s_ptRscRoot->Child;
        Obj->Next = root_node;
        Obj->Previous = root_node->Previous;
        root_node->Previous->Next = Obj;
        root_node->Previous = Obj;
    }
    return Obj;
}

//----初始化节点----------------------------------------------------------------
//功能: 把一个节点的所有指针清零，但不清节点负载size及名字指针
//参数: Obj,新添加的节点指针
//返回: 无
//------------------------------------------------------------------------------
void OBJ_Clean(struct Object *Obj)
{
    if(Obj)
    {
        Obj->Child = NULL;
        Obj->Parent = NULL;
        Obj->Next = NULL;
        Obj->Previous = NULL;
    }
}

//----添加根节点----------------------------------------------------------------
//功能: 在资源链中添加一棵树的根节点
//参数: Obj,新添加的节点指针
//      Size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      Name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//备注: 修改逻辑,将原来挂载到根下的组建(/dev分支除外),全部接入新建的/SYS下
//------------------------------------------------------------------------------
struct Object *OBJ_AddTree(struct Object *Obj, u16 Size,
                           u16 RscType, const char *Name)
{
    struct Object *root_node;
    atom_low_t low_atom;

    if(Obj == NULL)
        return NULL;
        
    if(OBJ_SearchTree(Name) != NULL)
        return NULL;
        
    Obj->Parent = s_ptRscRoot;
    Obj->Child=NULL;
    Obj->Size = Size;
    Obj->Type = RscType;
    Obj->Name = (char *)Name;
    low_atom = Int_LowAtomStart();
    //资源队列中至少存在一个信号量节点，无须判断是否空
    root_node = s_ptRscRoot->Child;
    Obj->Next = root_node;
    Obj->Previous = root_node->Previous;
    root_node->Previous->Next = Obj;
    root_node->Previous = Obj;
    Int_LowAtomEnd(low_atom);
    return Obj;
}

//----插入节点------------------------------------------------------------------
//功能: 在资源链表中插入一个节点，新节点在原节点的previous位置
//参数  Obj,在此节点前面插入节点
//      NewObj,待插入的新节点
//      Size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      Name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct Object *OBJ_AddToPrevious(struct Object *Obj, struct Object *NewObj,
                                 u16 Size, u16 RscType, const char *Name)
{
    atom_low_t low_atom;

    if((Obj == NULL) || (NewObj == NULL))
        return NULL;
        
    low_atom = Int_LowAtomStart();
    NewObj->Next = Obj;
    NewObj->Previous = Obj->Previous;
    NewObj->Parent = Obj->Parent;
    NewObj->Child = NULL;
    NewObj->Size = Size;
    NewObj->Type = RscType;
    NewObj->Name = (char *)Name;
    Obj->Previous->Next = NewObj;
    Obj->Previous = NewObj;
    Int_LowAtomEnd(low_atom);
    return NewObj;
}

//----增加节点------------------------------------------------------------------
//功能: 在资源链表中增加一个节点，新节点在原节点的next位置
//参数  Obj,在此节点后面插入节点
//      NewObj,待插入的新节点
//      Size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      Name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct Object *OBJ_AddToNext(struct Object *Obj, struct Object *NewObj,
                             u16 Size, u16 RscType, const char *Name)
{
    atom_low_t   low_atom;

    if((Obj == NULL) || (NewObj == NULL))
        return NULL;
        
    low_atom = Int_LowAtomStart();
    NewObj->Previous = Obj;
    NewObj->Next = Obj->Next;
    NewObj->Parent = Obj->Parent;
    NewObj->Child = NULL;
    NewObj->Size = Size;
    NewObj->Type = RscType;
    NewObj->Name = (char *)Name;
    Obj->Next->Previous = NewObj;
    Obj->Next = NewObj;
    Int_LowAtomEnd(low_atom);
    return NewObj;
}

//----插入子节点----------------------------------------------------------------
//功能: 给指定节点增加一个子节点,新节点将在队列尾位置
//参数  Parent,新节点的父节点
//      Child,待插入的新节点
//      Size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      Name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct Object *OBJ_AddChild(struct Object *Parent, struct Object *Child,
                            u16 Size, u16 RscType, const char *Name)
{
    atom_low_t low_atom;
    struct Object *p;

    if((Parent == NULL) || (Child == NULL))
        return NULL;

    low_atom = Int_LowAtomStart();
    Child->Size = Size;
    Child->Type = RscType;
    Child->Name = (char *)Name;
    Child->Child = NULL;
    if(Parent->Child == NULL)
    {
        Parent->Child = Child;
        Child->Parent = Parent;
        Child->Next = Child;
        Child->Previous = Child;
    }
    else
    {
        p = Parent->Child;
        Child->Next = p;
        Child->Previous = p->Previous;
        Child->Parent = Parent;
        p->Previous->Next = Child;
        p->Previous = Child;
    }
    Child->Inuse = 0;
    Int_LowAtomEnd(low_atom);
    return Child;
}

//----插入一个头节点---------------------------------------------------------
//功能: 给指定节点增加一个子节点,新节点将在队列头位置
//参数  Parent,新节点的父亲节点
//      Child,待插入的新节点
//      Size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      Name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct Object *OBJ_AddChildHead(struct Object *Parent, struct  Object *Child,
                                u16 Size, u16 RscType, const char *Name)
{
    atom_low_t low_atom;
    struct Object *p;

    if((Parent == NULL) || (Child == NULL))
        return NULL;
    low_atom = Int_LowAtomStart();
    Child->Size = Size;
    Child->Type = RscType;
    Child->Name = (char *)Name;
    Child->Child = NULL;
    if(Parent->Child == NULL)
    {
        Parent->Child = Child;
        Child->Parent = Parent;
        Child->Next = Child;
        Child->Previous = Child;
    }
    else
    {
        p = Parent->Child;
        Child->Next = p;
        Child->Previous = p->Previous;
        Child->Parent = Parent;
        p->Previous->Next = Child;
        p->Previous = Child;
        Parent->Child = Parent->Child->Previous;
    }
    Int_LowAtomEnd(low_atom);
    return Child;
}

//----替换节点-----------------------------------------------------------------
//功能: 用一个新节点替换掉资源队列中的原有节点，新节点原来必须不是队列中的节点
//参数: Old，被替换的节点
//      New，新节点
//返回: 无
//-----------------------------------------------------------------------------
bool_t OBJ_Displace(struct Object *Old, struct Object *New)
{
    struct Object *temp1, *temp2;
    atom_low_t low_atom;

    if((NULL == Old) || (NULL == New))
        return FALSE;

    low_atom = Int_LowAtomStart();
    temp1 = Old->Child;
    if(temp1 != NULL)   //把oldnode所有子节点的父指针指向newnode
    {
        temp2 = temp1;
        do
        {
            temp2->Parent = New;
            temp2 = temp2->Next;
        }while(temp2 != temp1);
    }
    New->Child = temp1;
    New->Next = Old->Next;
    New->Previous = Old->Previous;
    New->Parent = Old->Parent;
    Old->Next->Previous = New;
    Old->Previous->Next = New;
    if(Old->Parent->Child == Old) //如果是队列头节点
        Old->Parent->Child = New; //父节点的子节点指向队列头节点
    Int_LowAtomEnd(low_atom);

    return TRUE;
}

//----删除一个树枝-------------------------------------------------------------
//功能: 把一个树枝从资源队列中删除
//参数: Branch，被删除的分支。
//返回: 被删除分支指针，NULL表示分支不存在
//-----------------------------------------------------------------------------
struct Object *OBJ_DelBranch(struct Object *Branch)
{
    struct Object *result;
    atom_low_t low_atom;

    if(Branch == NULL)
        return NULL;

    low_atom = Int_LowAtomStart();
    if(Branch->Next == Branch)   //说明该节点没有兄弟节点.
    {
        Branch->Parent->Child = NULL;
    }
    else
    {
        if(Branch->Parent->Child == Branch)
        {   //说明该节点是队列头节点,需要改变队列头节点
            Branch->Parent->Child = Branch->Next;
        }
        Branch->Previous->Next = Branch->Next;
        Branch->Next->Previous = Branch->Previous;
    }
    result = Branch;
    Int_LowAtomEnd(low_atom);

    return result;
}
//---删除一个节点---------------------------------------------------------------
//功能: 把一个节点从资源链表中断开节点,该节点不能有子节点
//参数: Obj,被删除的节点,如该节点有子节点则不删除
//返回: 返回被删除节点指针，出错则返回NULL
//------------------------------------------------------------------------------
struct Object *OBJ_Del(struct Object *Obj)
{
    atom_low_t low_atom;
    struct Object *result;

    if(Obj == NULL)
        return NULL;

    low_atom = Int_LowAtomStart();
    if(Obj->Child != NULL) //子节点非空,不操作
        result = NULL;
    else
    {
        if(Obj->Next == Obj)   //说明该节点没有兄弟节点.
        {
            Obj->Parent->Child = NULL;
        }
        else
        {
            if(Obj->Parent->Child == Obj)
            {   //说明该节点是队列头节点,需要改变队列头节点
                Obj->Parent->Child = Obj->Next;
            }
            Obj->Previous->Next = Obj->Next;
            Obj->Next->Previous = Obj->Previous;
        }
        result = Obj;
    }
    Int_LowAtomEnd(low_atom);

    return result;
}

//---移动一棵树---------------------------------------------------------------
//功能: 移动一棵树到别的节点下面成为其子树
//参数: node,被移动的节点指针
//返回: TRUE = 成功执行，FALSE = 失败
//-----------------------------------------------------------------------------
bool_t OBJ_MoveToTree(struct Object *Parent,struct  Object *Obj)
{
    return TRUE;
}

//----移动节点到最后---------------------------------------------------------
//功能: 朝next指针方向移动资源队列中的一个节点，到同级队列头的前一个节点位置
//参数: Obj,被移动的节点指针
//返回: 无
//-----------------------------------------------------------------------------
bool_t OBJ_MoveToLast(struct Object *Obj)
{
    struct Object *eldest;
    atom_low_t low_atom;

    if(Obj == NULL)
        return FALSE;

    low_atom = Int_LowAtomStart();
    if(Obj->Parent != NULL)    //根节点不能移动
    {
        eldest = Obj->Parent->Child;
        if(eldest == Obj)
        //如果node是头节点,则直接移动父节点的子指针到下一个节点就可以了.
            Obj->Parent->Child = Obj->Next;
        //以下从链表中取出节点
        Obj->Next->Previous = Obj->Previous;
        Obj->Previous->Next = Obj->Next;
        //以下把node插入队列尾位置,由于是循环链表,头节点的前面就是尾节点.
        Obj->Next = eldest;
        Obj->Previous = eldest->Previous;
        eldest->Previous->Next = Obj;
        eldest->Previous = Obj;
    }
    Int_LowAtomEnd(low_atom);

    return TRUE;
}

//----移动节点成为队列头-------------------------------------------------------
//功能: 朝previous指针方向移动资源队列中的一个节点，成为同级队列头
//参数: Obj,被移动的节点指针
//返回: TRUE = 成功执行，FALSE = 失败
//------------------------------------------------------------------------------
bool_t OBJ_MoveToHead(struct Object *Obj)
{
    atom_low_t low_atom;

    if(Obj == NULL)
        return FALSE;
    low_atom = Int_LowAtomStart();
    if(Obj->Parent)    //根节点不能移动
    {
        OBJ_MoveToLast(Obj);
        Obj->Parent->Child = Obj;
    }
    Int_LowAtomEnd(low_atom);
    return TRUE;
}

//----移动节点到某节点next位置-------------------------------------------------
//功能: 移动资源队列中的一个节点NewNext到另一个节点Obj的next位置
//参数: node,被移动的节点指针
//      elder,目标节点,node移动到本节点后面
//返回: TRUE = 成功执行，FALSE = 失败
//------------------------------------------------------------------------------
bool_t OBJ_MoveToNext(struct Object *Obj, struct Object *NewNext)
{
    atom_low_t low_atom;

    if((Obj == NULL) || (NewNext == NULL) || (Obj == NewNext))
        return FALSE;
    else if(Obj->Parent != NewNext->Parent)
        return FALSE;

    low_atom = Int_LowAtomStart();
    //以下从链表中取出节点
    NewNext->Next->Previous = NewNext->Previous;
    NewNext->Previous->Next = NewNext->Next;
    NewNext->Previous = Obj;
    NewNext->Next = Obj->Next;
    Obj->Next->Previous = NewNext;
    Obj->Next = NewNext;
    Int_LowAtomEnd(low_atom);

    return TRUE;
}

//----移动节点到某节点previous位置---------------------------------------------
//功能: 移动资源队列中的一个节点NewPre到另一个节点Obj的previous位置
//参数: Obj,被移动的节点指针
//      NewPre,目标节点,node移动到本节点前面
//返回: TRUE = 成功执行，FALSE = 失败
//------------------------------------------------------------------------------
bool_t OBJ_MoveToPrevious(struct Object *Obj, struct Object *NewPre)
{
    atom_low_t low_atom;

    if((Obj == NULL) || (NewPre == NULL) || (Obj == NewPre))
        return FALSE;
    else if(Obj->Parent != NewPre->Parent)
        return FALSE;

    low_atom = Int_LowAtomStart();
    //以下从链表中取出节点
    NewPre->Next->Previous = NewPre->Previous;
    NewPre->Previous->Next = NewPre->Next;
    NewPre->Next = Obj;
    NewPre->Previous = Obj->Previous;
    Obj->Previous->Next = NewPre;
    Obj->Previous = NewPre;
    Int_LowAtomEnd(low_atom);

    return TRUE;
}

//----队列头位置后移--------------------------------------------------------------
//功能: Parent的子节点的相对位置不变,队列头朝previous方向移动一格。
//参数: Parent,父节点指针
//返回: TURE = 成功执行，FALSE = 失败
//------------------------------------------------------------------------------
bool_t OBJ_RoundPrevious(struct Object *Parent)
{
    atom_low_t low_atom;

    if(Parent == NULL)
        return FALSE;

    low_atom = Int_LowAtomStart();
    if(Parent->Child != NULL)
    {
        Parent->Child = Parent->Child->Previous;
    }
    Int_LowAtomEnd(low_atom);

    return TRUE;
}

//----队列头位置前移-----------------------------------------------------------
//功能: Parent的子节点的相对位置不变,队列头朝next方向移动一格。
//参数: Parent,父节点指针
//返回: TRUE = 成功执行，FALSE = 失败
//------------------------------------------------------------------------------
bool_t OBJ_RoundNext(struct Object *Parent)
{
    atom_low_t low_atom;

    if(Parent == NULL)
        return FALSE;

    low_atom = Int_LowAtomStart();
    if(Parent->Child != NULL)
    {
        Parent->Child = Parent->Child->Next;
    }
    Int_LowAtomEnd(low_atom);

    return TRUE;
}

//----资源更名-----------------------------------------------------------------
//功能: 修改资源名
//参数: Obj,被修改的资源节点指针
//      NewName,新名字
//返回: TRUE = 成功执行，FALSE = 失败
//-----------------------------------------------------------------------------
bool_t OBJ_Rename(struct Object *Obj, const char *NewName)
{
    if((Obj == NULL) || (NewName == NULL))
        return FALSE;

    Obj->Name = (char *)NewName;  //无需判断新名字字符串的长度，资源允许名字是空串

    return TRUE;
}

//----获取树根节点-------------------------------------------------------------
//功能: 返回指定节点所在的树的根节点指针
//参数: Obj,目标节点。
//返回: 根节点指针
//----------------------------------------------------------------------------
struct Object *OBJ_GetTree(struct Object *Obj)
{
    atom_low_t low_atom;
    struct Object *node = Obj;
    
    if(node == NULL)    //目标节点空
        return NULL;
    
    low_atom = Int_LowAtomStart();
    while(node->Parent != s_ptRscRoot)
    {
        if(NULL != node->Parent)
            node = node->Parent;
        else
        {
            node = NULL;
            break;
        }
    }
    Int_LowAtomEnd(low_atom);
    return node;
}

//----获取资源根节点-----------------------------------------------------------
//功能: 返回整个资源系统的根节点指针
//参数: 无。
//返回: 资源树指针
//----------------------------------------------------------------------------
struct Object *OBJ_SysRoot(void)
{
    return s_ptRscRoot;
}

//----获取节点名字---------------------------------------------------------------
//功能: 返回某节点的名字,
//参数: Obj,待操作的资源节点
//返回: 该节点的名字，应该是一个char *的指针
//注意: 该节点可能没有名字，因此返回的即使是成功也可能为NULL，因为确实没有名字
//      成功失败不应该据此做判断
//-----------------------------------------------------------------------------
char *OBJ_Name(struct Object *Obj)
{
    char *name;
    
    if(Obj == NULL)
        return NULL;

    name = Obj->Name;
    
    return name;
}

//----获取节点尺寸---------------------------------------------------------------
//功能: 返回某节点的数据结构尺寸,包含节点本身,
//参数: Obj,待操作的资源节点
//返回: 该节点的尺寸
//-----------------------------------------------------------------------------
u16 OBJ_Size(struct Object *Obj)
{
    if(Obj == NULL)
        return 0;

    return Obj->Size;
}

//----获取节点类型---------------------------------------------------------------
//功能: 返回某节点的类型,
//参数: Obj,待操作的资源节点
//返回: 该节点的类型
//-----------------------------------------------------------------------------
u16 OBJ_Type(struct Object *Obj)
{
    if(Obj == NULL)
        return 0;

    return Obj->Type;
}

//----获取父节点---------------------------------------------------------------
//功能: 返回某节点的父节点,
//参数: Obj,需要查找的树枝的任意一个子节点
//返回: 父节点.
//-----------------------------------------------------------------------------
struct Object *OBJ_Parent(struct Object *Obj)
{
    struct Object *node;

    if(Obj == NULL)
        return NULL;

    node = Obj->Parent;

    return node;
}

//----获取子节点--------------------------------------------------------------
//功能: 返回子节点，因父节点的child指针指向子节点的队列头，实际返回子节点的队列头
//参数: Obj,需要查找的树枝的父节点
//返回: 子节点队列头
//-----------------------------------------------------------------------------
struct Object *OBJ_Child(struct Object *Obj)
{
    struct Object *node;

    if(Obj == NULL)
        return NULL;

    node = Obj->Child;

    return node;
}

//----获取前一个节点-----------------------------------------------------------
//功能: 返回Obj节点的previous节点
//参数: Obj,需要查找的树枝的兄节点中的一个节点
//返回: previous节点.
//-----------------------------------------------------------------------------
struct Object *OBJ_Previous(struct Object *Obj)
{
    struct Object *node;
    
    if(Obj == NULL)
        return NULL;

    node = Obj->Previous;

    return node;
}

//----获取弟节点--------------------------------------------------------------
//功能: 返回Obj节点的next节点
//参数: Obj,需要查找的树枝的兄节点中的一个节点
//返回: next节点.
//-----------------------------------------------------------------------------
struct Object *OBJ_Next(struct Object *Obj)
{
    struct Object *node;
    
    if(Obj == NULL)
        return NULL;

    node = Obj->Next;

    return node;
}

//----获取队列头---------------------------------------------------------------
//功能: 返回Obj节点所在队列的队列头
//参数: Obj,需要查找的树枝的任意节点
//返回: 队列头指针.
//-----------------------------------------------------------------------------
struct Object *OBJ_GetHead(struct Object *Obj)
{
    struct Object *node;

    if(Obj == NULL)
        return NULL;

    node = Obj->Parent->Child;

    return node;
}

//----获取树枝一个末梢节点-----------------------------------------------------
//功能: 返回某树枝的一个末梢节点,末梢节点是指没有子节点的节点.
//参数: Obj,需要搜索的树枝的父节点
//返回: 树枝的一个末梢节点.当没有子节点时，返回NULL
//备注: 当需要删除整个树枝时,本函数很有用,结合rsc_del_node函数,反复调用本函数,
//      并把返回的节点删除,直到本函数返回NULL.需要删除一个文件夹或者删除一个存
//      在子窗口的gui窗口时,就需要用到删除整个树枝的操作.
//-----------------------------------------------------------------------------
struct Object *OBJ_GetTwig(struct Object *Obj)
{
    struct Object *result = NULL, *current;
    
    if(Obj == NULL)
        return NULL;

    current = Obj;
    while(current->Child != NULL)
    {
        current = current->Child;
    }
    if(current == Obj)
        result = NULL;
    else
        result = current;

    return result;
}

//----取节点的级别-----------------------------------------------------
//功能: 查看某节点是根节点的第几级子节点，
//参数: Obj,被查的节点
//返回: 子节点级数，0代表根节点。cn_limit_uint32代表出错。
//-----------------------------------------------------------------------------
u32 OBJ_GetLevel(struct Object *Obj)
{
    u32 n = 0;
    struct Object *pl_node = Obj;
    
    if(pl_node == NULL)
        return CN_LIMIT_UINT32;

    while(pl_node->Parent != s_ptRscRoot)
    {
        if(NULL != pl_node->Parent)
            pl_node = pl_node->Parent;
        else
        {
            n = CN_LIMIT_UINT32;
            break;
        }
        n++;
    }

    return n;
}

//----遍历子节点--------------------------------------------------------------
//功能: 从当前节点开始,获取下一个节点的指针,直到遍历完全部子节点
//参数: Parent,需要搜索的树枝的祖先节点
//      Child,当前搜索位置,
//返回: 当前搜索位置的下一个节点指针,如果已经搜索完成,则返回NULL.
//------------------------------------------------------------------------------
struct Object *OBJ_TraveChild(struct Object *Parent, struct Object *Child)
{
    if((Parent == NULL) || (Child == NULL))
        return NULL;

    if(Child == Parent)
    {
        Child = Parent->Child;
    }
    else
    {
        Child = Child->Next;
        if(Child == Parent->Child)
            Child = NULL;
    }

    return Child;
}

//----遍历一个树枝--------------------------------------------------------------
//功能: 从当前节点开始,获取下一个节点的指针,沿着搜索路线,直到搜索完整个树枝.搜索
//      路线为:当前节点的子节点,如果子节点为空则搜索next节点,next节点又为空则搜
//      索父节点的next节点,直到搜索完成.
//      本函数只返回子孙节点，不返回ancestor_node本身
//参数: Ancestor,需要搜索的树枝的祖先节点
//      Current,当前搜索位置,
//返回: 当前搜索位置的下一个节点指针,如果已经搜索完成,则返回NULL.
//备注: 当需要对资源链表中某一个树枝或者整个链表中的节点逐一进行某种操作时,可
//      反复调用本函数,第一次调用current_node = parent_node,其后current_node
//      = 上次返回值，直到返回空.
//      本函数按父、子、孙、曾孙....的顺序搜索，先搜直系，再搜旁系，确保所有子孙
//      节点都能够访问到，如果对访问顺序有特殊要求，不能使用本函数
//------------------------------------------------------------------------------
struct Object *OBJ_TraveScion(struct Object *Ancestor, struct Object *Current)
{
    struct  Object *result = NULL,*current_copy;
    bool_t up = FALSE;

    if((Ancestor == NULL) || (Current == NULL))
        return NULL;

    if((Current != Ancestor) || Ancestor->Child)
    {
        current_copy = Current;
        do
        {
            if((up == FALSE) && (current_copy->Child != NULL))
            {   //子节点非空,返回子节点
                result = current_copy->Child;
                break;
            }
            else if(current_copy->Next != current_copy->Parent->Child)
            {   //子节点空,但本节点不是队列尾节点,返回next节点
                result = current_copy->Next;
                break;
            }
            else
            {   //无子节点,且本节点已经是队列尾节点,需要判断当前父节点的next节点
                current_copy = current_copy->Parent;
                up = TRUE;
            }
        }while(current_copy != Ancestor);
    }

    return result;
}

//----在兄弟节点中搜索资源-----------------------------------------------------
//功能: 在某一个资源节点的所有兄弟节点中搜索给定名字的资源
//参数: Brother,兄弟节点中任意一个资源指针.
//      Name,需要搜索的资源名
//返回: 如果搜索到资源返回资源节点指针,否则返回NULL
//-----------------------------------------------------------------------------
struct Object *OBJ_SearchSibling(struct Object *Brother, const char *Name)
{
    struct Object *temp, *result = NULL;

    if((Brother == NULL) || (Name == NULL))
        return NULL;

    temp = Brother;

    do
    {
        if(strcmp(temp->Name, Name) == 0)
        {
            result = temp;
            break;
        }
        temp = temp->Next;
    }while (temp != Brother);

    return result;
}

//----查找资源树---------------------------------------------------------------
//功能: 在资源根节点的直接子节点中查找名为name的资源树
//参数: 资源名称
//返回: 资源树节点指针。NULL = 没找到
//-----------------------------------------------------------------------------
struct Object *OBJ_SearchTree(const char *Name)
{
    struct Object *temp, *result = NULL;

    if(Name == NULL)
        return NULL;

    temp = s_ptRscRoot->Child;

    do
    {
        if(strcmp(temp->Name, Name)==0)
        {
            result = temp;
            break;
        }
        temp = temp->Next;
    }while (temp != s_ptRscRoot->Child);

    return result;
}

//----在子节点中搜索资源-----------------------------------------------------
//功能: 在某一个资源节点的所有子节点中搜索给定名字的资源
//参数: Parent,父节点资源指针.
//      Name,需要搜索的资源名,纯资源名,不包含路径.
//返回: 如果搜索到资源返回资源节点指针,否则返回NULL
//-----------------------------------------------------------------------------
struct Object *OBJ_SearchChild(struct Object *Parent, const char *Name)
{
    struct Object *temp, *result = NULL;

    if((Name == NULL) || (Parent == NULL))
        return NULL;

    temp = Parent->Child;
    if(temp == NULL)
        result = NULL;
    else
    {
        do
        {
            if(strcmp(temp->Name, Name)==0)
            {
                result = temp;
                break;
            }
            temp = temp->Next;
        }while (temp != Parent->Child);
    }

    return result;
}

//----在后代中搜索资源名-------------------------------------------------------
//功能: 与rsc_search_layer类似,搜索ancestor_node的所有后代节点,直到找到一个名称
//      匹配资源。
//参数: Ancestor,树枝的父节点
//      Name,需要搜索的资源名,纯资源名,不包含路径.
//返回: 如果搜索到资源返回资源节点指针,否则返回NULL
//------------------------------------------------------------------------------
struct Object *OBJ_SearchScion(struct Object *Ancestor, const char *Name)
{
    struct Object *current, *temp, *result = NULL;
    
    if((Ancestor == NULL) || (Name == NULL))
        return NULL;
        
    current = Ancestor;
    //在rsc_trave_scion中已经有信号量保护，此处无须保护
    while((temp = OBJ_TraveScion(Ancestor, current)) != NULL)
    {
        if(strcmp(temp->Name, Name) == 0)
        {
            result = temp;
            break;
        }
        current = temp;
    }
    return result;
}

//----沿路径搜索资源名---------------------------------------------------------
//功能: 与rsc_search_scion类似,不同的是，path是包含路径名的字符串，执行精确搜索。
//参数: Start,树枝的父节点
//      Path,包含路径名的资源名
//返回: 如果搜索到资源返回资源节点指针,否则返回NULL
//-----------------------------------------------------------------------------
struct Object *OBJ_Search(struct Object *Start, const char *Path)
{
    const char *path_name;
    char *dest_name;
    bool_t match;
    u32 i;
    struct Object *current, *result = NULL;

    if((Path == NULL) || (Start == NULL))
        return NULL;

    current = Start->Child;
    if(current == NULL)
    {
        return NULL;
    }
    path_name = Path;
    while(1)
    {
        dest_name = current->Name;
        match = FALSE;
        for(i=0;i<CN_RSC_NAME_LIMIT;i++)   //资源名长度限制
        {
            if((path_name[i] == 0)||((path_name[i]=='\\')&&(path_name[i+1]==0)))
            {   //已经达到字符串结束,且相等
                result = current;
                break;
            }else if(path_name[i] == '\\')
            {   //字符串相等,但path_name未结束
                match = TRUE;
                break;
            }
            if(path_name[i] != dest_name[i])
            {   //字符串不相等
                match = FALSE;
                break;
            }
        }
        if(result != NULL)  //名字匹配且到达path字符串末,查找结束
            break;

        if(match != TRUE)
        {   //本节点名字不匹配,查找下一个节点
            current = current->Next;
            if(current == current->Parent->Child)
            //本级的所有节点已经搜索完,没有找到匹配的.
                break;
        }
        else
        {   //本节点名字匹配,但是还没到path串结束,继续查找子节点
            path_name += i+1;
            current = current->Child;
            if(current == NULL)
            //或者资源链已经搜索结束,但path_name并未结束
                break;
        }
    }

    return result;
}

//----是否队列头-------------------------------------------------------------
//功能: 判断给定节点是否兄弟节点中的队列头节点
//参数: Obj，被测试节点
//返回: TRUE=是队列头，FALSE=不是队列头
//-----------------------------------------------------------------------------
bool_t OBJ_IsHead(struct Object *Obj)
{
    if(NULL == Obj)
        return (FALSE);

    if(Obj == s_ptRscRoot)
        return TRUE;

    if((Obj) && (Obj  == Obj->Parent->Child))
        return TRUE;
    else
        return FALSE;
}

//----是否队列尾---------------------------------------------------------------
//功能: 判断给定节点是否兄弟节点中的最后一个节点
//参数: Obj，被测试节点
//返回: TRUE=是最后一个节点，FALSE=不是
//-----------------------------------------------------------------------------
bool_t OBJ_IsLast(struct Object *Obj)
{
    if(NULL == Obj)
        return (FALSE);

    if(Obj == s_ptRscRoot)
        return TRUE;

    if(Obj->Next  == Obj->Parent->Child)
        return TRUE;
    else
        return FALSE;
}

//----取节点顺序-------------------------------------------------------------
//功能: 取给定节点在兄弟节点中的位置，head=0，下一个是1，类推之
//参数: Obj，被测试节点
//返回: 节点位置号
//-----------------------------------------------------------------------------
u32 OBJ_Sequencing(struct Object *Obj)
{
    u32 ranking;
    struct Object *temp;

    if(NULL == Obj)
        return (-1);

    if(Obj == s_ptRscRoot)
        return (0);
    ranking = 0;
    temp = Obj->Parent->Child;
    while(temp != Obj)
    {
        ranking +=1;
        temp = temp->Next;
    }
    return (ranking);
}

