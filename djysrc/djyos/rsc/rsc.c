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
#include "stdint.h"
#include "stddef.h"
#include "string.h"
#include "int.h"
#include "rsc.h"
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



static struct tagRscNode s_tRscRoot;
static struct tagRscNode *s_ptRscRoot;
static struct tagMutexLCB s_tMutexRscRoot;


//----初始化资源管理模块-------------------------------------------------------
//功能: 创建资源节点入口，本函数被分裂成两个，第二个函数必须在执行完锁初始化后
//      才能调用，第一个则必须在锁初始化执行前调用
//参数: 无
//返回: 1
//------------------------------------------------------------------------------
ptu32_t Rsc1_ModuleInit(ptu32_t para)
{
    para = para;        //消除编译器告警
    s_tRscRoot.name = "resouce queue";
    s_tRscRoot.child = NULL;
    s_tRscRoot.next =&s_tRscRoot;
    s_tRscRoot.previous =&s_tRscRoot;
    s_tRscRoot.parent = NULL;
    s_tRscRoot.RscSize = sizeof(s_tRscRoot);
    s_tRscRoot.RscType = RSC_RSCNODE;
    s_ptRscRoot = &s_tRscRoot;
    return 1;
}
ptu32_t Rsc2_ModuleInit(ptu32_t para)
{
    para = para;        //消除编译器告警
    Lock_MutexCreate_s(&s_tMutexRscRoot,"mutex for resouce queue");
    return 1;
}
//----添加根节点----------------------------------------------------------------
//功能: 在资源链中添加一个根节点(特供锁模块，因此时锁尚未初始化，)
//参数: node,新添加的节点指针
//      size，新节点连负载的尺寸，字节数
//      name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct  tagRscNode * __Lock_RscAddLockTree(struct  tagRscNode *node,
                                u32 size,char *name)
{
    struct  tagRscNode *root_node;
    if(node == NULL)
        return NULL;
    node->parent = s_ptRscRoot;
    node->child=NULL;
    node->RscSize = size;
    node->RscType = RSC_RSCNODE;
    node->name = name;
    if(s_ptRscRoot->child == NULL)
    {
        s_ptRscRoot->child = node;
        node->next = node;
        node->previous = node;
    }else
    {
        root_node = s_ptRscRoot->child;
        node->next = root_node;
        node->previous = root_node->previous;
        root_node->previous->next = node;
        root_node->previous = node;
    }
    return node;
}

//----初始化节点----------------------------------------------------------------
//功能: 把一个节点的所有指针清零，但不清节点负载size及名字指针
//参数: node,新添加的节点指针
//返回: 无
//------------------------------------------------------------------------------
void Rsc_CleanNode(struct tagRscNode *node)
{
    node->child = NULL;
    node->parent = NULL;
    node->next = NULL;
    node->previous = NULL;
}

//----添加根节点----------------------------------------------------------------
//功能: 在资源链中添加一棵树的根节点
//参数: node,新添加的节点指针
//      size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct  tagRscNode * Rsc_AddTree(struct  tagRscNode *node,
                                 u16 size,u8 RscType,char *name)
{
    struct  tagRscNode *root_node;
    atom_low_t   low_atom;

    if(node == NULL)
        return NULL;
    if(Rsc_SearchTree(name) != NULL)
        return NULL;
    node->parent = s_ptRscRoot;
    node->child=NULL;
    node->RscSize = size;
    node->RscType = RscType;
    node->name = name;
    low_atom = Int_LowAtomStart();
    //资源队列中至少存在一个信号量节点，无须判断是否空
    root_node = s_ptRscRoot->child;
    node->next = root_node;
    node->previous = root_node->previous;
    root_node->previous->next = node;
    root_node->previous = node;
    Int_LowAtomEnd(low_atom);
    return node;
}

//----插入节点------------------------------------------------------------------
//功能: 在资源链表中插入一个节点，新节点在原节点的previous位置
//参数  node,在此节点前面插入节点
//      new_node,待插入的新节点
//      size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct  tagRscNode * Rsc_AddToPrevious(struct  tagRscNode *node,
                                   struct tagRscNode *new_node,
                                   u16 size,u8 RscType,char *name)
{
    atom_low_t   low_atom;

    if((node==NULL)||(new_node==NULL))
        return NULL;
    low_atom = Int_LowAtomStart();
    new_node->next=node;
    new_node->previous=node->previous;
    new_node->parent=node->parent;
    new_node->child=NULL;
    new_node->RscSize = size;
    new_node->RscType = RscType;
    new_node->name = name;
    node->previous->next=new_node;
    node->previous=new_node;
    Int_LowAtomEnd(low_atom);
    return new_node;
}

//----增加节点------------------------------------------------------------------
//功能: 在资源链表中增加一个节点，新节点在原节点的next位置
//参数  node,在此节点后面插入节点
//      new_node,待插入的新节点
//      size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct  tagRscNode * Rsc_AddToNext(struct  tagRscNode *node,
                                struct  tagRscNode *new_node,
                                u16 size,u8 RscType,char *name)
{
    atom_low_t   low_atom;

    if((node==NULL)||(new_node==NULL))
        return NULL;
    low_atom = Int_LowAtomStart();
    new_node->previous=node;
    new_node->next=node->next;
    new_node->parent=node->parent;
    new_node->child=NULL;
    new_node->RscSize = size;
    new_node->RscType = RscType;
    new_node->name = name;
    node->next->previous=new_node;
    node->next=new_node;
    Int_LowAtomEnd(low_atom);
    return new_node;
}

//----插入子节点----------------------------------------------------------------
//功能: 给指定节点增加一个子节点,新节点将在队列尾位置
//参数  parent_node,新节点的父节点
//      new_node,待插入的新节点
//      size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct  tagRscNode * Rsc_AddSon(struct  tagRscNode *parent_node,
                               struct  tagRscNode *new_node,
                               u16 size,u8 RscType,char *name)
{
    atom_low_t   low_atom;
    struct  tagRscNode *p;
    if((parent_node==NULL)||(new_node==NULL))
        return NULL;
    low_atom = Int_LowAtomStart();
    new_node->RscSize = size;
    new_node->RscType = RscType;
    new_node->name = name;
    new_node->child=NULL;
    if(parent_node->child==NULL)
    {
        parent_node->child=new_node;
        new_node->parent=parent_node;
        new_node->next=new_node;
        new_node->previous=new_node;
    }else
    {
        p=parent_node->child;
        new_node->next=p;
        new_node->previous=p->previous;
        new_node->parent=parent_node;
        p->previous->next=new_node;
        p->previous=new_node;
    }
    Int_LowAtomEnd(low_atom);
    return new_node;
}

//----插入一个头节点---------------------------------------------------------
//功能: 给指定节点增加一个子节点,新节点将在队列头位置
//参数  parent_node,新节点的父亲节点
//      new_node,待插入的新节点
//      size，新节点连负载的尺寸，字节数
//      RscType, 资源类型,用于区分不同资源,原则上,资源的数据结构不同,类型就不同
//      name，资源名字，所指向的字符串内存区不能是局部变量，可以是空
//返回: 新加入的节点
//------------------------------------------------------------------------------
struct  tagRscNode * Rsc_AddHeadSon(struct  tagRscNode *parent_node,
                                    struct  tagRscNode *new_node,
                                    u16 size,u8 RscType,char *name)
{
    atom_low_t   low_atom;
    struct  tagRscNode *p;
    if((parent_node==NULL)||(new_node==NULL))
        return NULL;
    low_atom = Int_LowAtomStart();
    new_node->RscSize = size;
    new_node->RscType = RscType;
    new_node->name = name;
    new_node->child=NULL;
    if(parent_node->child==NULL)
    {
        parent_node->child=new_node;
        new_node->parent=parent_node;
        new_node->next=new_node;
        new_node->previous=new_node;
    }else
    {
        p=parent_node->child;
        new_node->next=p;
        new_node->previous=p->previous;
        new_node->parent=parent_node;
        p->previous->next=new_node;
        p->previous=new_node;
        parent_node->child = parent_node->child->previous;
    }
    Int_LowAtomEnd(low_atom);
    return new_node;
}

//----替换节点-----------------------------------------------------------------
//功能: 用一个新节点替换掉资源队列中的原有节点，新节点原来必须不是队列中的节点
//参数: oldnode，被替换的节点
//      newnode，新节点
//返回: 无
//-----------------------------------------------------------------------------
bool_t Rsc_DisplaceNode(struct tagRscNode *oldnode,struct tagRscNode *newnode)
{
    struct tagRscNode *temp1,*temp2;
    atom_low_t   low_atom;
    low_atom = Int_LowAtomStart();
    temp1 = oldnode->child;
    if(temp1 != NULL)   //把oldnode所有子节点的父指针指向newnode
    {
        temp2 = temp1;
        do
        {
            temp2->parent = newnode;
            temp2 = temp2->next;
        }while(temp2 != temp1);
    }
    newnode->child = temp1;
    newnode->next = oldnode->next;
    newnode->previous = oldnode->previous;
    newnode->parent = oldnode->parent;
    oldnode->next->previous = newnode;
    oldnode->previous->next = newnode;
    if(oldnode->parent->child == oldnode) //如果是队列头节点
        oldnode->parent->child = newnode; //父节点的子节点指向队列头节点
    Int_LowAtomEnd(low_atom);
    return true;
}

//----删除一个树枝-------------------------------------------------------------
//功能: 把一个树枝从资源队列中删除
//参数: branch，被删除的分支。
//返回: 被删除分支指针，NULL表示分支不存在
//-----------------------------------------------------------------------------
struct  tagRscNode * Rsc_DelBranch(struct  tagRscNode *branch)
{
    struct  tagRscNode * result;
    atom_low_t   low_atom;

    if(branch == NULL)
        return NULL;
    low_atom = Int_LowAtomStart();
    if(branch->next == branch)   //说明该节点没有兄弟节点.
    {
        branch->parent->child = NULL;
    }else
    {
        if(branch->parent->child == branch)
        {   //说明该节点是队列头节点,需要改变队列头节点
            branch->parent->child = branch->next;
        }
        branch->previous->next = branch->next;
        branch->next->previous = branch->previous;
    }
    result = branch;

    Int_LowAtomEnd(low_atom);
    return result;
}
//---删除一个节点---------------------------------------------------------------
//功能: 把一个节点从资源链表中断开节点,该节点不能有子节点
//参数: node,被删除的节点,如该节点有子节点则不删除
//返回: 返回被删除节点指针，出错则返回NULL
//------------------------------------------------------------------------------
struct  tagRscNode * Rsc_DelNode(struct  tagRscNode *node)
{
    atom_low_t   low_atom;
    struct  tagRscNode * result;
    if(node == NULL)
        return NULL;
    low_atom = Int_LowAtomStart();
    if(node->child != NULL) //子节点非空,不操作
        result = NULL;
    else
    {
        if(node->next == node)   //说明该节点没有兄弟节点.
        {
            node->parent->child = NULL;
        }else
        {
            if(node->parent->child == node)
            {   //说明该节点是队列头节点,需要改变队列头节点
                node->parent->child = node->next;
            }
            node->previous->next = node->next;
            node->next->previous = node->previous;
        }
        result = node;
    }
    Int_LowAtomEnd(low_atom);
    return result;
}

//---移动一棵树---------------------------------------------------------------
//功能: 移动一棵树到别的节点下面成为其子树
//参数: node,被移动的节点指针
//返回: true = 成功执行，false = 失败
//-----------------------------------------------------------------------------
bool_t Rsc_MoveToTree(struct tagRscNode *parent,struct  tagRscNode *node)
{
    return true;
}

//----移动节点到最后---------------------------------------------------------
//功能: 朝next指针方向移动资源队列中的一个节点，到同级队列头的前一个节点位置
//参数: node,被移动的节点指针
//返回: 无
//-----------------------------------------------------------------------------
bool_t Rsc_MoveToLast(struct  tagRscNode *node)
{
    struct  tagRscNode *eldest;
    atom_low_t   low_atom;

    if(node == NULL)
        return false;
    low_atom = Int_LowAtomStart();

    if(node->parent != NULL)    //根节点不能移动
    {
        eldest = node->parent->child;
        if(eldest == node)
        //如果node是头节点,则直接移动父节点的子指针到下一个节点就可以了.
            node->parent->child = node->next;
        //以下从链表中取出节点
        node->next->previous = node->previous;
        node->previous->next = node->next;
        //以下把node插入队列尾位置,由于是循环链表,头节点的前面就是尾节点.
        node->next = eldest;
        node->previous = eldest->previous;
        eldest->previous->next = node;
        eldest->previous = node;
    }
    Int_LowAtomEnd(low_atom);
    return true;
}

//----移动节点成为队列头-------------------------------------------------------
//功能: 朝previous指针方向移动资源队列中的一个节点，成为同级队列头
//参数: node,被移动的节点指针
//返回: true = 成功执行，false = 失败
//------------------------------------------------------------------------------
bool_t Rsc_MoveToHead(struct  tagRscNode *node)
{
    atom_low_t   low_atom;

    if(node == NULL)
        return false;
    low_atom = Int_LowAtomStart();
    if(node->parent)    //根节点不能移动
    {
        Rsc_MoveToLast(node);
        node->parent->child = node;
    }
    Int_LowAtomEnd(low_atom);
    return true;
}

//----移动节点到某节点next位置-------------------------------------------------
//功能: 移动资源队列中的一个节点node到另一个节点elder的next位置
//参数: node,被移动的节点指针
//      elder,目标节点,node移动到本节点后面
//返回: true = 成功执行，false = 失败
//------------------------------------------------------------------------------
bool_t Rsc_MoveToNext(struct  tagRscNode *elder,struct  tagRscNode *node)
{
    atom_low_t   low_atom;

    if((elder==NULL)||(node==NULL)||(elder==node))
        return false;
    else if(elder->parent != node->parent)
        return false;
    low_atom = Int_LowAtomStart();
    //以下从链表中取出节点
    node->next->previous = node->previous;
    node->previous->next = node->next;
    node->previous = elder;
    node->next = elder->next;
    elder->next->previous = node;
    elder->next = node;

Int_LowAtomEnd(low_atom);
    return true;
}

//----移动节点到某节点previous位置---------------------------------------------
//功能: 移动资源队列中的一个节点node到另一个节点lesser的previous位置
//参数: node,被移动的节点指针
//      lesser,目标节点,node移动到本节点前面
//返回: true = 成功执行，false = 失败
//------------------------------------------------------------------------------
bool_t Rsc_MoveToPrevious(struct  tagRscNode *lesser,struct  tagRscNode *node)
{
    atom_low_t   low_atom;

    if((lesser==NULL)||(node==NULL)||(lesser==node))
        return false;
    else if(lesser->parent != node->parent)
        return false;
    low_atom = Int_LowAtomStart();

    //以下从链表中取出节点
    node->next->previous = node->previous;
    node->previous->next = node->next;
    node->next = lesser;
    node->previous = lesser->previous;
    lesser->previous->next = node;
    lesser->previous = node;

    Int_LowAtomEnd(low_atom);
    return true;
}

//----队列头位置后移--------------------------------------------------------------
//功能: parent的子节点的相对位置不变,队列头朝previous方向移动一格。
//参数: parent,父节点指针
//返回: true = 成功执行，false = 失败
//------------------------------------------------------------------------------
bool_t Rsc_RoundPrevious(struct  tagRscNode *parent)
{
    atom_low_t   low_atom;

    if(parent == NULL)
        return false;
    low_atom = Int_LowAtomStart();
    if(parent->child != NULL)
    {
        parent->child = parent->child->previous;
    }
    Int_LowAtomEnd(low_atom);
    return true;
}

//----队列头位置前移-----------------------------------------------------------
//功能: parent的子节点的相对位置不变,队列头朝next方向移动一格。
//参数: parent,父节点指针
//返回: true = 成功执行，false = 失败
//------------------------------------------------------------------------------
bool_t Rsc_RoundNext(struct  tagRscNode *parent)
{
    atom_low_t   low_atom;

    if(parent == NULL)
        return false;
    low_atom = Int_LowAtomStart();
    if(parent->child != NULL)
    {
        parent->child = parent->child->next;
    }
    Int_LowAtomEnd(low_atom);
    return true;
}

//----资源更名-----------------------------------------------------------------
//功能: 修改资源名
//参数: node，被修改的资源节点指针
//      new_name，新名字
//返回: true = 成功执行，false = 失败
//-----------------------------------------------------------------------------
bool_t Rsc_RenameNode(struct tagRscNode *node,char *new_name)
{
    if((node == NULL) || (new_name == NULL))
        return false;
    node->name = new_name;  //无需判断新名字字符串的长度，资源允许名字是空串
    return true;
}

//----获取树根节点-------------------------------------------------------------
//功能: 返回指定节点所在的树的根节点指针
//参数: scion_node,目标节点。
//返回: 根节点指针
//----------------------------------------------------------------------------
struct  tagRscNode *Rsc_GetTree(struct  tagRscNode *scion_node)
{
    atom_low_t   low_atom;
    struct  tagRscNode *node = scion_node;
    if(node == NULL)    //目标节点空
        return NULL;
    low_atom = Int_LowAtomStart();
    while(node->parent != s_ptRscRoot)
    {
        if(NULL != node->parent)
            node = node->parent;
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
//参数: scion_node,目标节点。
//返回: 资源树指针
//----------------------------------------------------------------------------
struct  tagRscNode *Rsc_GetRoot(void)
{
    return s_ptRscRoot;
}

//----获取节点名字---------------------------------------------------------------
//功能: 返回某节点的名字,
//参数: node,待操作的资源节点
//返回: 该节点的名字，应该是一个char *的指针
//注意: 该节点可能没有名字，因此返回的即使是成功也可能为NULL，因为确实没有名字
//      成功失败不应该据此做判断
//-----------------------------------------------------------------------------
char *Rsc_GetName(struct  tagRscNode *node)
{
    char *name;
    if(node == NULL)
        return NULL;

    name = node->name;
    return name;
}

//----获取节点尺寸---------------------------------------------------------------
//功能: 返回某节点的数据结构尺寸,包含节点本身,
//参数: node,待操作的资源节点
//返回: 该节点的尺寸
//-----------------------------------------------------------------------------
u16 Rsc_GetNodeSize(struct  tagRscNode *node)
{
    if(node == NULL)
        return 0;

    return node->RscSize;
}

//----获取节点尺寸---------------------------------------------------------------
//功能: 返回某节点的类型,
//参数: node,待操作的资源节点
//返回: 该节点的类型
//-----------------------------------------------------------------------------
u8 Rsc_GetNodeType(struct  tagRscNode *node)
{
    if(node == NULL)
        return 0;

    return node->RscType;
}

//----获取父节点---------------------------------------------------------------
//功能: 返回某节点的父节点,
//参数: son_node,需要查找的树枝的任意一个子节点
//返回: 父节点.
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_GetParent(struct  tagRscNode *son_node)
{
    struct  tagRscNode *node;
    if(son_node == NULL)
        return NULL;

    node = son_node->parent;

    return node;
}

//----获取子节点--------------------------------------------------------------
//功能: 返回子节点，因父节点的child指针指向子节点的队列头，实际返回子节点的队列头
//参数: parent_node,需要查找的树枝的父节点
//返回: 子节点队列头
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_GetSon(struct  tagRscNode *parent_node)
{
    struct  tagRscNode *node;
    if(parent_node == NULL)
        return NULL;
    node = parent_node->child;

    return node;
}

//----获取前一个节点-----------------------------------------------------------
//功能: 返回next_node节点的previous节点
//参数: next_node,需要查找的树枝的兄节点中的一个节点
//返回: previous节点.
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_GetPrevious(struct  tagRscNode *next_node)
{
    struct  tagRscNode *node;
    if(next_node == NULL)
        return NULL;

    node = next_node->previous;

    return node;
}

//----获取弟节点--------------------------------------------------------------
//功能: 返回previous_node节点的next节点
//参数: previous_node,需要查找的树枝的兄节点中的一个节点
//返回: next节点.
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_GetNext(struct  tagRscNode *previous_node)
{
    struct  tagRscNode *node;
    if(previous_node == NULL)
        return NULL;

    node = previous_node->next;

    return node;
}

//----获取队列头---------------------------------------------------------------
//功能: 返回rnode节点所在队列的队列头
//参数: rnode,需要查找的树枝的任意节点
//返回: 队列头指针.
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_GetHead(struct  tagRscNode *rnode)
{
    struct  tagRscNode *node;
    if(rnode == NULL)
        return NULL;

    node = rnode->parent->child;

    return node;
}

//----获取树枝一个末梢节点-----------------------------------------------------
//功能: 返回某树枝的一个末梢节点,末梢节点是指没有子节点的节点.
//参数: parent_node,需要搜索的树枝的父节点
//返回: 树枝的一个末梢节点.当没有子节点时，返回NULL
//备注: 当需要删除整个树枝时,本函数很有用,结合rsc_del_node函数,反复调用本函数,
//      并把返回的节点删除,直到本函数返回NULL.需要删除一个文件夹或者删除一个存
//      在子窗口的gui窗口时,就需要用到删除整个树枝的操作.
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_GetTwig(struct  tagRscNode *ancestor_node)
{
    struct  tagRscNode *result=NULL,*current;
    if(ancestor_node == NULL)
        return NULL;

    current = ancestor_node;
    while(current->child != NULL)
    {
        current = current->child;
    }
    if(current == ancestor_node)
        result = NULL;
    else
        result = current;

    return result;
}

//----取节点的级别-----------------------------------------------------
//功能: 查看某节点是根节点的第几级子节点，
//参数: node,被查的节点
//返回: 子节点级数，0代表根节点。cn_limit_uint32代表出错。
//-----------------------------------------------------------------------------
u32 Rsc_GetClass(struct  tagRscNode *node)
{
    u32 n=0;
    struct tagRscNode *pl_node = node;
    if(pl_node == NULL)
        return CN_LIMIT_UINT32;

    while(pl_node->parent != s_ptRscRoot)
    {
        if(NULL != pl_node->parent)
            pl_node = pl_node->parent;
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
//参数: parent_node,需要搜索的树枝的祖先节点
//      current_node,当前搜索位置,
//返回: 当前搜索位置的下一个节点指针,如果已经搜索完成,则返回NULL.
//------------------------------------------------------------------------------
struct  tagRscNode *Rsc_TraveSon(struct  tagRscNode *parent_node,
                                  struct  tagRscNode *current_son)
{
    if((parent_node==NULL)||(current_son==NULL))
    {
        return NULL;
    }

    if(current_son == parent_node)
    {
        current_son = parent_node->child;
    }
    else
    {
        current_son = current_son->next;
        if(current_son == parent_node->child)
            current_son = NULL;
    }

    return current_son;
}

//----遍历一个树枝--------------------------------------------------------------
//功能: 从当前节点开始,获取下一个节点的指针,沿着搜索路线,直到搜索完整个树枝.搜索
//      路线为:当前节点的子节点,如果子节点为空则搜索next节点,next节点又为空则搜
//      索父节点的next节点,直到搜索完成.
//      本函数只返回子孙节点，不返回ancestor_node本身
//参数: parent_node,需要搜索的树枝的祖先节点
//      current_node,当前搜索位置,
//返回: 当前搜索位置的下一个节点指针,如果已经搜索完成,则返回NULL.
//备注: 当需要对资源链表中某一个树枝或者整个链表中的节点逐一进行某种操作时,可
//      反复调用本函数,第一次调用current_node = parent_node,其后current_node
//      = 上次返回值，直到返回空.
//      本函数按父、子、孙、曾孙....的顺序搜索，先搜直系，再搜旁系，确保所有子孙
//      节点都能够访问到，如果对访问顺序有特殊要求，不能使用本函数
//------------------------------------------------------------------------------
struct  tagRscNode *Rsc_TraveScion(struct  tagRscNode *ancestor_node,
                                  struct  tagRscNode *current_node)
{
    struct  tagRscNode *result=NULL,*current_copy;
    bool_t up = false;
    if((ancestor_node==NULL)||(current_node==NULL))
    {
        return NULL;
    }

    if((current_node != ancestor_node) || ancestor_node->child)
    {
        current_copy = current_node;
        do
        {
            if((up == false) && (current_copy->child != NULL))
            {   //子节点非空,返回子节点
                result = current_copy->child;
                break;
            }else if(current_copy->next != current_copy->parent->child)
            {   //子节点空,但本节点不是队列尾节点,返回next节点
                result = current_copy->next;
                break;
            }else
            {   //无子节点,且本节点已经是队列尾节点,需要判断当前父节点的next节点
                current_copy = current_copy->parent;
                up = true;
            }
        }while(current_copy != ancestor_node);
    }

    return result;
}

//----在兄弟节点中搜索资源-----------------------------------------------------
//功能: 在某一个资源节点的所有兄弟节点中搜索给定名字的资源
//参数: brother,兄弟节点中任意一个资源指针.
//      name,需要搜索的资源名
//返回: 如果搜索到资源返回资源节点指针,否则返回NULL
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_SearchSibling(struct  tagRscNode *brother,char *name)
{
    struct  tagRscNode *temp,*result = NULL;
    if((brother == NULL)||(name == NULL))
        return NULL;
    temp = brother;

    do
    {
        if(strcmp(temp->name,name)==0)
        {
            result = temp;
            break;
        }
        temp = temp->next;
    }while (temp != brother);

    return result;
}

//----查找资源树---------------------------------------------------------------
//功能: 在资源根节点的直接子节点中查找名为name的资源树
//参数: 资源名称
//返回: 资源树节点指针。NULL = 没找到
//-----------------------------------------------------------------------------
struct tagRscNode *Rsc_SearchTree(char *tree_name)
{
    struct  tagRscNode *temp,*result = NULL;
    if(tree_name == NULL)
        return NULL;
    temp = s_ptRscRoot->child;

    do
    {
        if(strcmp(temp->name,tree_name)==0)
        {
            result = temp;
            break;
        }
        temp = temp->next;
    }while (temp != s_ptRscRoot->child);

    return result;
}

//----在子节点中搜索资源-----------------------------------------------------
//功能: 在某一个资源节点的所有子节点中搜索给定名字的资源
//参数: parent,父节点资源指针.
//      name,需要搜索的资源名,纯资源名,不包含路径.
//返回: 如果搜索到资源返回资源节点指针,否则返回NULL
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_SearchSon(struct  tagRscNode *parent,char *name)
{
    struct  tagRscNode *temp,*result = NULL;
    if((name == NULL)||(parent == NULL))
        return NULL;

    temp = parent->child;
    if(temp == NULL)
        result = NULL;
    else
    {
        do
        {
            if(strcmp(temp->name,name)==0)
            {
                result = temp;
                break;
            }
            temp = temp->next;
        }while (temp != parent->child);
    }

    return result;
}

//----在后代中搜索资源名-------------------------------------------------------
//功能: 与rsc_search_layer类似,搜索ancestor_node的所有后代节点,直到找到一个名称
//      匹配资源。
//参数: ancestor_node,树枝的父节点
//      name,需要搜索的资源名,纯资源名,不包含路径.
//返回: 如果搜索到资源返回资源节点指针,否则返回NULL
//------------------------------------------------------------------------------
struct tagRscNode *Rsc_SearchScion(struct tagRscNode *ancestor_node,char *name)
{
    struct  tagRscNode  *current,*temp,*result = NULL;
    if((ancestor_node == NULL)||(name == NULL))
        return NULL;
    current = ancestor_node;
    //在rsc_trave_scion中已经有信号量保护，此处无须保护
    while((temp = Rsc_TraveScion(ancestor_node,current)) != NULL)
    {
        if(strcmp(temp->name,name) == 0)
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
//参数: ancestor_node,树枝的父节点
//      path,包含路径名的资源名
//返回: 如果搜索到资源返回资源节点指针,否则返回NULL
//-----------------------------------------------------------------------------
struct  tagRscNode *Rsc_Search(struct  tagRscNode *ancestor_node,char *path)
{
    char *path_name,*dest_name;
    bool_t   match;
    u32 i;

    struct  tagRscNode *current,*result = NULL;
    if((path == NULL)||(ancestor_node == NULL))
        return NULL;

    current = ancestor_node->child;
    if(current == NULL)
    {

        return NULL;
    }
    path_name = path;
    while(1)
    {
        dest_name = current->name;
        match=false;
        for(i=0;i<CN_RSC_NAME_LIMIT;i++)   //资源名长度限制
        {
            if((path_name[i] == 0)||((path_name[i]=='\\')&&(path_name[i+1]==0)))
            {   //已经达到字符串结束,且相等
                result = current;
                break;
            }else if(path_name[i] == '\\')
            {   //字符串相等,但path_name未结束
                match = true;
                break;
            }
            if(path_name[i] != dest_name[i])
            {   //字符串不相等
                match = false;
                break;
            }
        }
        if(result != NULL)  //名字匹配且到达path字符串末,查找结束
            break;

        if(match != true)
        {   //本节点名字不匹配,查找下一个节点
            current = current->next;
            if(current == current->parent->child)
            //本级的所有节点已经搜索完,没有找到匹配的.
                break;
        }else
        {   //本节点名字匹配,但是还没到path串结束,继续查找子节点
            path_name += i+1;
            current = current->child;
            if(current == NULL)
            //或者资源链已经搜索结束,但path_name并未结束
                break;
        }
    }

    return result;
}

//----是否队列头-------------------------------------------------------------
//功能: 判断给定节点是否兄弟节点中的队列头节点
//参数: node，被测试节点
//返回: true=是队列头，false=不是队列头
//-----------------------------------------------------------------------------
bool_t Rsc_IsHead(struct tagRscNode *node)
{
    if(node == s_ptRscRoot)
        return true;
    if(node  == node->parent->child)
        return true;
    else
        return false;
}

//----是否队列尾---------------------------------------------------------------
//功能: 判断给定节点是否兄弟节点中的最后一个节点
//参数: node，被测试节点
//返回: true=是最后一个节点，false=不是
//-----------------------------------------------------------------------------
bool_t Rsc_IsLast(struct tagRscNode *node)
{
    if(node == s_ptRscRoot)
        return true;
    if(node->next  == node->parent->child)
        return true;
    else
        return false;
}

//----取节点顺序-------------------------------------------------------------
//功能: 取给定节点在兄弟节点中的位置，head=0，下一个是1，类推之
//参数: node，被测试节点
//返回: 节点位置号
//-----------------------------------------------------------------------------
u32 Rsc_NodeSequencing(struct tagRscNode *node)
{
    u32 ranking;
    struct tagRscNode *temp;
    if(node == s_ptRscRoot)
        return 0;
    ranking = 0;
    temp = node->parent->child;
    while(temp != node)
    {
        ranking +=1;
        temp = temp->next;
    }
    return ranking;
}
//-----------------------------------------------------------------
//功能: 把一个树枝从资源队列中删除,但保留枝节点
//参数: Branch，被删除的分支。
//返回: 被删除分支指针，NULL表示分支不存在
//------------
//更新记录:
//1.日期: 2015/3/21
//   说明: 创建
//   作者: 季兆林
//-----------------------------------------------------------------------------
bool_t Rsc_ClearBranch_t(struct tagRscNode *Branch)
{
    atom_low_t low_atom;

    if(NULL == Branch)
        return (false);
    low_atom = Int_LowAtomStart();
    Branch->child = NULL;
    Int_LowAtomEnd(low_atom);
    return (true);

}
