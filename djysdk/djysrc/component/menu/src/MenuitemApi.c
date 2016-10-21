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
// 模块描述: DJY菜单模块之菜单项API（对菜单项操作的最基本的API函数）
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "MenuitemApi.h"
#include "MenuFunction.h"

#define                            cn_limit_menuitem        1000 //max menuitem num
static struct menu_item             sMenuitemRsc[cn_limit_menuitem];  // todo:
static struct Object             *pg_menuitem_root;  // the menuitem tree
struct MemCellPool               *pg_menuitem_pool;// the menuitem pool
//static struct menu_item             *pg_operating_menuitem;// the current operating menuitem ,for the visible

#define                            CN_MENU_ROOT_NUM           10    //the maxi node son of pg_menuitem_rsc_tree
static  u8                         menuitem_tree_count=0;     //the current node son of pg_menuitem_rsc_tree
//struct SemaphoreLCB              *pg_menuitem_semp;//use to limit the menuitem rsc tree operation limit

// =========================================================================
// 函数功能:菜单模块初始化函数，初始化相应资源，添加根节点
// 输入参数:    无
// 输出参数：无
// 返回值  :
// 说明    :建立菜单项的内存池和根节点
// =========================================================================
ptu32_t module_init_menu(ptu32_t para)
{
   static struct Object sMenuitemRoot;
   bool_t  result=false;

   //init the menuitem tree
   pg_menuitem_root= OBJ_AddTree(&sMenuitemRoot, sizeof(struct Object),RSC_RSCNODE,"MenuItem_root");
   if(NULL==pg_menuitem_root)
   {
      printf("create the pg_menuitem_root failed!\n");
      return result;
   }
   //init the menuitem pool
   pg_menuitem_pool = Mb_CreatePool((void*)sMenuitemRsc,
                                    cn_limit_menuitem,
                                    sizeof(struct menu_item),
                                    20,100,
                                    "MenuItem_pool");
   if(NULL==pg_menuitem_pool)
    {
       printf("create the pg_menuitem_pool failed!\n");
       return result;
    }
    result=true;
    return result;
}

// =========================================================================
// 函数功能:释放菜单项所占用的空间
// 输入参数:待释放的菜单项pMenuitem
// 输出参数：
// 返回值  : true：成功，false：失败（其node节点没有clean）
// 说明    :本函数仅仅会释放那些没有逻辑关联的菜单项，如果其node还没有clean掉，则不会释放
// =========================================================================
bool_t FreeMenuitem(struct menu_item * pMenuitem)
{
   bool_t result=false;
   if(NULL==pMenuitem)
   {
      printf("This is an empty pMenuitem!\n");
      result=true;
      return result;
   }
   else if((pMenuitem->node.child!=NULL)||(pMenuitem->node.parent!=NULL)||\
    (pMenuitem->node.previous!=NULL)||(pMenuitem->node.next!=NULL))
   {
      printf("use DelMenuBranchCompletely(struct menu_item * pMenuitem)!\n");
      return result;
   }
   else
   {
     Mb_Free(pg_menuitem_pool,(void *)pMenuitem);
     result = true;
     return result;
   }
}

// =========================================================================
// 函数功能:创建菜单树节点
// 输入参数:pTreeName，菜单树根节点的名字，如果为则会自动分配名字，自动分配的名字会
//         很难听，自己需要有心里准备
// 输出参数：
// 返回值    :创建的菜单树根节点
// 说明          :菜单树根节点是用来区分不同的菜单树，创建的菜单树根节点已经添加到菜单树的
//          menu_root节点下
// =========================================================================
struct menu_item *CreateMenuitemTreeNode(char * pTreeName)
{

   u8     charLen=0;
   u8     treeNumber=0;
   struct menu_item *result=NULL;
   char   name[namesize];//动态分配名字时使用

   if(menuitem_tree_count>(CN_MENU_ROOT_NUM-1))
   {
     printf("has achived the max menuitem root number!");
     return result;
   }
   if((NULL==pTreeName)||(0==strlen(pTreeName,namesize))||\
      (NULL!=SearchSubMenuitemByName(pg_menuitem_root, pTreeName)))//名字为空、长度为0、重名都会导致创建失败
   {
      printf("Invalid name-----CreateMenuitemTreeNode\n");
      for(treeNumber=0;treeNumber<CN_MENU_ROOT_NUM;treeNumber++)
      {
         itoa(treeNumber,name,16);
         if(NULL==SearchSubMenuitemByName(pg_menuitem_root, name))
         {
           break;
         }
      }
      if(CN_MENU_ROOT_NUM==treeNumber)//没有可使用的名字，显然是不可能的，前面已经对tree number做了判断。
      {
        return result;
      }
      else
      {
        pTreeName=name;
      }
   }

   result=(struct menu_item *)Mb_Malloc(pg_menuitem_pool,0);
   if(NULL==result)
   {
      printf("menuitem mrmory pool is not enough!\n");
      return result;
   }
   charLen=strlen(pTreeName,namesize);
   if(namesize<=charLen)//源字符串太长
   {
     memcpy(result->name,pTreeName,namesize);
     result->name[namesize-1]='\0';
   }
   else
   {
     strcpy(result->name,pTreeName);
   }

   OBJ_Clean((struct Object *) result);
   OBJ_AddChildHead((struct Object *)(pg_menuitem_root),\
       (struct Object *) result, sizeof(struct menu_item),\
        RSCTYPE_USER,(const char*)(result->name));
   menuitem_tree_count++;
   printf("menuitem_tree_node:%s\n",result->node.name);

   return result;
}

// =========================================================================
// 函数功能:创建菜单项
// 输入参数:name：菜单项资源节点的名字，byTip:提示字符，passwd：密码，wpara：属性
//         FuncName：关联的钩子函数的名字
// 输出参数：
// 返回值    :指向创建的菜单项的指针
// 说明          :创建的菜单项的名字的长度由namesize规定，长了会截断
// =========================================================================
struct menu_item *CreateMenuitem(const char *name,const char *byTip,u32 byAttrib,u32 passWd,u32 wPara,\
    const char*FuncName)
{

    u8     charLen=0;
    struct menu_item  *result=NULL;

    if(strnlen(name,namesize)==0)
    {
     printf("invalid name for create the menuitem!\n");
     return result;
    }
    result=(struct menu_item *)Mb_Malloc(pg_menuitem_pool,0);
    if(NULL==result)
    {
      printf("Not enough mem for createmenuitem!\n");
      return result;
    }
    //初始化链表头
    OBJ_Clean(&result->node);
    //具体化成员变量
    //拷贝名字
    charLen=strnlen(name,namesize);
    if(namesize<=charLen)//源字符串太长
    {
      memcpy(result->name,name,namesize);
      result->name[namesize-1]='\0';
    }
    else
    {
       strcpy(result->name,name);
    }
    result->node.name=result->name;
    //拷贝bytip
    charLen=strnlen(byTip,namesize);
    if(namesize<=charLen)//源字符串太长
    {
      memcpy(result->bytip,byTip,namesize);
      result->name[namesize-1]='\0';
    }
    else
    {
      strcpy(result->bytip,byTip);
    }
    //初始化重要属性
    result->byAttrib=byAttrib;
    result->MenuitemFunction=getfunc_byname(FuncName);
    result->wPassword=passWd;
    result->wPara=wPara;
    result->font=0;

    return result;
}

// =========================================================================
// 函数功能:在子菜单项中寻找名字为name的子菜单
// 输入参数：pParent：父菜单项，name：子菜单项的名字
// 输出参数：
// 返回值    :找到的子菜单为name的菜单项（NULL没有找到）
// 说明          :
// =========================================================================
struct menu_item* SearchSubMenuitemByName(struct menu_item * pParent, const char * name)
{
   struct menu_item *result=NULL;

   result=(struct menu_item*)OBJ_SearchChild((struct Object * )pParent, name);//已经做了参数的检查
   return result;
}

// =========================================================================
// 函数功能:在同层菜单中寻找名字为name的兄弟菜单
// 输入参数：pBrother:提供的同层菜单，name：寻找的名字
// 输出参数：
// 返回值    :找到的菜单项为name的菜单项（NULL没有找到）
// 说明          :
// =========================================================================
struct menu_item * SearchBroMenuitemByName(struct menu_item * pBrother, const char * name)
{

    struct menu_item * result=NULL;

    result=(struct menu_item *)OBJ_SearchSibling((struct Object *)pBrother,  name);

    return result;
}

// =========================================================================
// 函数功能:为父菜单项添加子菜单
// 输入参数：pParent:父菜单项，pNewmenuitem：待添加的菜单项，head：true,添加的是嫡长子菜单
//          false,添加的是最小子菜单项
// 输出参数：
// 返回值    :true，添加成功，false，失败
// 说明          :当有同名的子菜单存在时，会添加失败的
// =========================================================================

bool_t AddSubMenuitem(struct menu_item * pParent, struct menu_item *pNewmenuitem, bool_t head)
{
    bool_t  result=false;
    struct Object  *temp_node;

    //check if any sub menuitem named this
    if(NULL!=SearchSubMenuitemByName(pParent,pNewmenuitem->name))
    {
        printf("There has been a sub menuitem named this!\n");
        return result;
    }
    if (head)
    {
        temp_node= OBJ_AddChildHead((struct Object *)pParent, (struct Object *)pNewmenuitem, \
            sizeof(struct menu_item),RSCTYPE_USER,(const char*)(pNewmenuitem->name));
    }
    else
    {
        temp_node= OBJ_AddChild((struct Object *)pParent, (struct Object *)pNewmenuitem, \
            sizeof(struct menu_item),RSCTYPE_USER,(const char*)(pNewmenuitem->name));
    }

    if(temp_node!=NULL)
        result=true;
    return  result;
}

// =========================================================================
// 函数功能:添加兄弟菜单项
// 输入参数：pBrother:兄弟菜单项，pNewmenuitem：待添加的菜单项，insert：true,添加的是兄菜单（pre）
//          false,添加的是弟菜单项
// 输出参数：
// 返回值    :true，成功，false失败
// 说明          :当同层菜单中已经有同名的菜单项存在时，添加失败
// =========================================================================
bool_t AddBroMenuitem(struct menu_item * pBrother, struct menu_item *pNewmenuitem, bool_t insert)
{
    bool_t result=false;
    struct Object *temp_node;

    //check any brother menuitem named this
    if(NULL!=SearchBroMenuitemByName(pBrother, pNewmenuitem->name))
    {
       printf("There has been a brother menuitem named this!\n");
       return result;
    }
    if(insert)
    {
         temp_node=OBJ_AddToPrevious((struct Object *)pBrother, (struct Object *)pNewmenuitem,\
            sizeof(struct menu_item), RSCTYPE_USER,(const char*)(pNewmenuitem->name));
    }
    else
    {
         temp_node=OBJ_AddToNext(&(pBrother->node), &(pNewmenuitem->node),\
                    sizeof(struct menu_item),RSCTYPE_USER,(const char*)(pNewmenuitem->name));
    }
    if(NULL!=temp_node)
        result=true;

    return  result;
}

// =========================================================================
// 函数功能:寻找当前菜单项的一个端点（该菜单不再有子菜单）,末梢节点
// 输入参数：pMnuitem，待寻找的祖先菜单
// 输出参数：
// 返回值    :找到的没有子菜单的后代菜单
// 说明          :从前往后扫描，
// =========================================================================
struct menu_item* SearchSubTernalMenuitem(struct menu_item * pMenuitem)
{

   struct menu_item* result=NULL;
   result=(struct menu_item *)OBJ_GetTwig((struct  Object *)pMenuitem);
   return result;
}

// =========================================================================
// 函数功能:删除整个菜单分支包括祖先节点
// 输入参数：pMenuitem，待删除的菜单分支祖先节点
// 输出参数：
// 返回值  :true成功 false失败
// 说明    :删除当前菜单项的所有后代菜单以及当前菜单（逻辑上和物理上），通过循环删除
//          端节点来实现
// =========================================================================
bool_t DelMenuBranchCompletely(struct menu_item * pMenuitem)
{
   bool_t             result=false;
   struct menu_item    *temp_menuitem=pMenuitem;

  if((struct Object *)pMenuitem==pg_menuitem_root)//this is an menuitem root node
  {
     printf("Invalid para---DelMenuBranchCompletely\n");
    return result;
  }
  while((temp_menuitem=SearchSubTernalMenuitem(pMenuitem))!=NULL)
  {
    printf("begin to delete the menuitem=%s\n",temp_menuitem->node.name);
    OBJ_Del((struct Object *)temp_menuitem);
    Mb_Free(pg_menuitem_pool, temp_menuitem);
  }
  if(OBJ_Parent((struct Object *)pMenuitem)==pg_menuitem_root)
  {
     menuitem_tree_count--;
  }
  printf("begin to delete the menuitem=%s\n",pMenuitem->node.name);
  OBJ_Del((struct Object *)(&(pMenuitem->node)));
  Mb_Free(pg_menuitem_pool, pMenuitem);
  result=true;

    return result;

}
// =========================================================================
// 函数功能:删除菜单分支（不包括分支祖先节点）
// 输入参数：pMenuitem，待删除的菜单项的祖先菜单
// 输出参数：
// 返回值 :true成功 false失败
// 说明   :删除当前菜单项的所有后代菜单不包括当前菜单（逻辑上和物理上），通过循环删除
//          端节点来实现
// =========================================================================
bool_t DelMenuBranch(struct menu_item * pMenuitem)
{
   bool_t             result=false;
   struct menu_item    *temp_menuitem=pMenuitem;

  if((struct Object *)pMenuitem==pg_menuitem_root)//this is an menuitem root node
  {
     printf("Invalid para---DelMenuBranchCompletely\n");
    return result;
  }
  while((temp_menuitem=SearchSubTernalMenuitem(pMenuitem))!=NULL)
  {
    printf("begin to delete the menuitem=%s\n",temp_menuitem->node.name);
    OBJ_Del((struct Object *)temp_menuitem);
    Mb_Free(pg_menuitem_pool, temp_menuitem);
  }
  result=true;

    return result;

}
// =========================================================================
// 函数功能:获取提供菜单项的后一个菜单
// 输入参数：pMenuitem，提供的菜单项
// 输出参数：
// 返回值    :pMenuitem的next菜单项
// 说明          :
// =========================================================================

struct menu_item *GetNextMenuitem(struct menu_item * pMenuitem)
{
    struct menu_item * result=NULL;

    result=(struct menu_item *)OBJ_Next((struct Object *)pMenuitem);

    return result;


}
// =========================================================================
// 函数功能:获取提供菜单项的前一个菜单
// 输入参数：pMenuitem，提供的菜单项
// 输出参数：
// 返回值    :pMenuitem的pre菜单项
// 说明          :
// =========================================================================

struct menu_item *GetPreMenuitem(struct menu_item * pMenuitem)
{
    struct menu_item * result=NULL;

    result=(struct menu_item *)OBJ_Previous((struct Object *)pMenuitem);

    return result;


}
// =========================================================================
// 函数功能:获取提供菜单项的嫡长子菜单项
// 输入参数：pMenuitem，提供的菜单项
// 输出参数：
// 返回值    :pMenuitem的嫡长子菜单项
// 说明          :
// =========================================================================

struct menu_item *GetSonMenuitem(struct menu_item * pMenuitem)
{
    struct menu_item * result=NULL;
    result=(struct menu_item *)OBJ_Child((struct Object *)pMenuitem);
    return result;
}
// =========================================================================
// 函数功能:获取提供菜单项的父菜单
// 输入参数：pMenuitem，提供的菜单项
// 输出参数：
// 返回值    :pMenuitem的parent菜单项
// 说明          :
// =========================================================================

struct menu_item *GetParMenuitem(struct menu_item * pMenuitem)
{
    struct menu_item * result=NULL;

    result=(struct menu_item *)OBJ_Parent((struct Object *)pMenuitem);

    return result;
}

// =========================================================================
// 函数功能:获取提供菜单项的最小弟菜单
// 输入参数：pMenuitem，提供的菜单项
// 输出参数：
// 返回值    :pMenuitem的最小brother，next的最未端
// 说明          :
// =========================================================================
struct menu_item * GetLBmenuitem(struct menu_item  *menuitem)
{
    struct menu_item   *result=NULL;


    result=GetBBmenuitem(menuitem);
    result=GetPreMenuitem(result);
    return result;
}
//获取最大的brother菜单项
// =========================================================================
// 函数功能:获取提供菜单项的最大brother菜单
// 输入参数：pMenuitem，提供的菜单项
// 输出参数：
// 返回值    :pMenuitem的最大brother，pre的最前端
// 说明          :
// =========================================================================
struct menu_item * GetBBmenuitem(struct menu_item  *menuitem)
{
    struct menu_item   *result=NULL;

    result=(struct menu_item *)OBJ_GetHead((struct Object *)menuitem);
    return  result;
}
// =========================================================================
// 函数功能:计算同层菜单的菜单数目
// 输入参数：pMenuitem，提供的菜单项
// 输出参数：
// 返回值    :同层菜单的数目
// 说明          :0x00表示逻辑出错，非正常菜单项链表
// =========================================================================
u32  CalMenuitemNum(struct menu_item  *menuitem)
{
     u32    result=0;
     struct menu_item   *temp;
     temp=menuitem;
     do{
          result++;
          temp=GetNextMenuitem(temp);
          if((0xFF==result)||(NULL==temp))
          {
              result=0x00;
              break;//逻辑出错或者死循环状态
          }
     }while(temp!=menuitem);
     return result;
}
// =========================================================================
// 函数功能:计算当前菜单项在同层菜单项的第几位
// 输入参数：pMenuitem，提供的菜单项
// 输出参数：
// 返回值    :当前菜单项是同层菜单项的第几个
// 说明          :0x00表示出错了,顺序是从嫡长子为第一个算起
// =========================================================================
u8 CalMenuitemNumber(struct menu_item *menuitem)   //todo:类似的函数一起改
{
   u8   result=0;

   struct menu_item *tempmenuitem=NULL;
   tempmenuitem=GetBBmenuitem(menuitem);
   if(NULL==tempmenuitem)
   {
      printf("Invalid para for calmenuitemNumber!\n");
      result=0x00;
      return result;
   }

   result=1;
   while(menuitem!=tempmenuitem)
   {
      result++;
      if(0xFF==result)
      {
        result=0x00;
        break;
      }
      tempmenuitem=GetNextMenuitem(tempmenuitem);
   }

   return result;
}
// =========================================================================
// 函数功能:获取同层菜单项的第number个菜单项
// 输入参数：pMenuitem，同层菜单项，number，待寻找的菜单项的号码
// 输出参数：
// 返回值    :同层菜单项的第number个菜单项
// 说明          :寻找的顺序是同层菜单项的嫡长子为第一个算起，采用循环取余的方法
// =========================================================================
struct menu_item *GetMenuitemBynumber(struct menu_item *pMenuitem, s8 number)
{
   struct menu_item *result=NULL;

   u8    TotalNum=0;

   if(NULL==pMenuitem)//||(没有number为0的菜单项)
   {
       printf("Invalid parameter for GetMenuitemByNunber!\n");
       return result;
   }

   TotalNum=CalMenuitemNum(pMenuitem);
   result=GetBBmenuitem(pMenuitem);
   //auto adjust the no of the searching menuitem,even the number is negative,we also adjust it
   while(number<=0)//这种情况需要调整
   {
      number=number+TotalNum;
   }
   if(number>TotalNum)
   {
      number=number%TotalNum;
   }
   TotalNum=1;
   while(TotalNum!=number)
   {
      result=GetNextMenuitem(result);
      if(NULL==result)
      {
          break;
      }
      TotalNum++;
   }

   return result;
}
// =========================================================================
// 函数功能:计算同层两个菜单项之间的距离（间隔几个菜单项），
// 输入参数：menuitem1，第一个菜单项，menuitem2，第二个菜单项
// 输出参数：
// 返回值    :当前菜单项是同层菜单项的第几个
// 说明          :0xFF表示出错了,顺序是从嫡长子到最小子的方向
// =========================================================================
u8  CalDisBettween2Menuitem(struct menu_item * menuitem1, struct menu_item *menuitem2 )
{
     u8   result;//计算结果
     u8   num1=0,num2=0;//用来计算菜单的序号


     if((NULL==menuitem1)||(NULL==menuitem2))//参数检查
     {
           printf("Invalid parameter--CalDisBetween2Menuitem!\n");
           return 0xFF;
     }
     if(GetParMenuitem(menuitem1)!=GetParMenuitem(menuitem2))//不是同一层菜单
     {
        printf("They are not the same slayers!\n");
        return 0xFF;
     }
     //计算他们的序号
     //计算他们的序号
     num1=CalMenuitemNumber(menuitem1);
     num2=CalMenuitemNumber(menuitem2);
     if((num1==0)||(num2==0))
     {
         return 0xFF;
     }
     else
     {
         if(num1>num2)
         {
              result=num1-num2;
         }
         else
         {
              result=num2-num1;
         }
         return result;
     }
}
// =========================================================================
// 函数功能:计算目的菜单项相对源菜单项的位置
// 输入参数：DestMenuitem:目的菜单项，SrcMenuitem，源菜单项
// 输出参数：Relative,目的菜单项相对于源菜单项的位置，负数表示前面，正表示后面，
// 返回值    :
// 说明          :-128表示出错了,顺序是从嫡长子到最小子的方向
// =========================================================================
bool_t CalPosRelative2SrcMenuitem(struct menu_item * DestMenuitem, struct menu_item *SrcMenuitem, s8 *RelaPos)
{
     bool_t   result=false;
     u8  num1=0,num2=0;//用来计算菜单的序号
     s8   num=0;

     if((NULL==DestMenuitem)||(NULL==SrcMenuitem))//参数检查
     {
           printf("Invalid parameter---CalPosRelative2SrcMenuitem\n");
           return result;
     }
     if(GetParMenuitem(DestMenuitem)!=GetParMenuitem(SrcMenuitem))//不是同一层菜单
     {
        printf("Not same layers----CalPosRelative2SrcMenuitem\n");
        return result;
     }
     //计算他们的序号
     num1=CalMenuitemNumber(DestMenuitem);
     num2=CalMenuitemNumber(SrcMenuitem);
     if((num1==0)||(num2==0))
     {
         return result;
     }
     else
     {
         num=num1-num2;
         *RelaPos=num;
         result=true;
         return result;
     }
}
//设置菜单项属性
// =========================================================================
// 函数功能:设置菜单项名字
// 输入参数:pMenuitem 待修改的菜单项，pName：新名字
// 输出参数:
// 返回值    :true成功 false失败
// 说明          :因为不允许重名或者是没有名字，所以这些条件会导致失败。
// =========================================================================
bool_t  RenameMenuitem(struct menu_item *pMenuitem, char *pNewName)
{
   bool_t result=false;
   u8     charLen=0;

   if((NULL==pMenuitem)||(NULL==pNewName)||(0==(charLen=strnlen(pNewName,namesize))))
   {
       printf("Invalid para for rename the Menuitem!\n");
       return result;
   }
   if(charLen<namesize)
   {
     strcpy(pMenuitem->name,pNewName);
   }
   else
   {
      memcpy(pMenuitem->name,pNewName,namesize);
      pMenuitem->name[namesize-1]='\0';
   }
   result=true;
   return result;
}
// =========================================================================
// 函数功能:设置菜单项密码
// 输入参数:pMenuitem 待修改的菜单项，u16 passwd，新密码
// 输出参数:
// 返回值    :true成功 false失败
// 说明          :因为不允许重名或者是没有名字，所以这些条件会导致失败。
// =========================================================================
bool_t  SetMenuitemPasswd(struct menu_item *pMenuitem, u16 passwd)
{
   bool_t result=false;

   if(NULL==pMenuitem)
   {
       printf("Invalid para for rename the Menuitem!\n");
   }
   else
   {
       pMenuitem->wPassword=passwd;
       result=true;
   }
   return result;
}
