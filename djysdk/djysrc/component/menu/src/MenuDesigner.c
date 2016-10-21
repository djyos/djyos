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
// 模块描述: DJY菜单模块之菜单设计器的定义
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
#include "MenuDesigner.h"
#include "MenuitemApi.h"
// =============================================================================
// 函数功能:判断用户提供的菜单树的意愿是否合法（符合规则）
// 输入参数:tab：待检查的意愿链表；tab_size：意愿的个数
// 输出参数：
// 返回值  :true：符合规则；  false：不合法
// 说明    :判断规则：1，后级的class不允许比前级大2及其以上
//                 2，每一级的class不允许为负
//                 3, 不允许没有名字
//                 4，第一项的class一般的为0
// =============================================================================
bool_t  CheckTagMenuitemModelTab(const struct MenuitemModel *tab,u32 tab_size)
{
   bool_t    result=false;
   s8        temp_class=0;
   u32       num=0;

   if((NULL==tab)||(0==tab_size))
   {
      printf("Invalid parameter----CheckTagMenuitemModelTab\n");
      return result;
   }
   temp_class=tab[0].byClass;
   for(num=0; num< tab_size; num ++)
    {
         if(tab[num].byClass<0)//不允许class小于0的
         {
              printf("The menuitem model tab is illegal!num=%d--class=%d\n",\
              tab[num].byClass);
              return result;
         }
         if((tab[num].byClass-temp_class)>1)//不允许后级的class比前级大2及其以上
         {
                printf("The menuitem model tab is illegal!--Num=%d-CurClass=%d,preClass=%d\n",\
                    num,tab[num].byClass,temp_class);
                return result;
          }
         if(strlen(tab[num].byName,namesize)==0)
         {
             printf("The menuitem model tab is illegal!--NULL name!\n");
             return result;
         }
         temp_class=tab[num].byClass;
    }
   result=true;
   return result;
}

// =========================================================================
// 函数功能:计算用户创建的意愿表的大小
// 输入参数:tablist：待检查的意愿链表
// 输出参数：
// 返回值  :tablist所包含的菜单项意愿的个数
// 说明    :当tab_list的名字为空的时候表示结束。
// =========================================================================
u32  CalTagMenuitemModuleTabSize(const struct MenuitemModel * tab_list)
{
    u32  num=0;
    bool_t flag=true;
    while(flag)
    {
        if(strlen(tab_list[num].byName,namesize)==0)
        {
          flag=false;
        }
        else
        {
          num++;
        }
    }
    return num;
}

// ===================================================================================
// 函数功能:以存在的菜单树的一个节点为根节点添加一个新的菜单树
// 输入参数:menuitem_tree：存在的菜单树的一个节点；tab_list：用户菜单树意愿表
// 输出参数：
// 返回值:添加的菜单树的根节点，一般的为menuitem_tree，但添加不成功
//          等情况下返回NULL；
// 说明:首先检查tab_list的合法性，然后按照相对位置进行添加,tablist的第一个必须是
//          是主菜单项（处于最高菜单层次，class最小）
// ===================================================================================
struct menu_item*  append_menu_branch(struct menu_item * menuitem_tree, const struct MenuitemModel * tab_list)
{
     bool_t    result=false;
     u8        current_class=0;
     s8        layers=0;//todo;
     u32       num;
     u32       count=0;  //record how many tab has been added
     u32       size=CalTagMenuitemModuleTabSize(tab_list);

     struct menu_item  *current_menuitem=NULL;
     struct menu_item  *temp_menuitem=NULL;
     struct menu_item  *Mresult=NULL;

     //参数检测
     if(menuitem_tree==NULL)
     {
        return Mresult;
     }
     size=CalTagMenuitemModuleTabSize(tab_list);
     //检查列表规则,检查名字长度、class的等级等规则
     result= CheckTagMenuitemModelTab( tab_list,size);
     if(result==false)
     {
        return Mresult;
     }
     //起始点
     current_menuitem=menuitem_tree;
     current_class=tab_list[0].byClass;
     //from here we begin to add the tab to the menuitem_0x_n tree
     for(num=0;num<size; num++)
     {
        temp_menuitem=CreateMenuitem(tab_list[num].byName, tab_list[num].byTip, tab_list[num].byAttrib, \
                                     tab_list[num].passWd, tab_list[num].wPara, tab_list[num].FunctionName);
        if(NULL==temp_menuitem)
        {
           printf("Failed to creat the menuitem--not enught mem!\n");
           //内存不足，那么这颗树必然创建不成功
           DelMenuBranch(menuitem_tree);
           return Mresult;
        }
        if(tab_list[num].byClass>current_class)
        {
           //肯定只会是1，因为在规则检查里面已经检查了。
           result=AddSubMenuitem(current_menuitem,temp_menuitem,false);
           if(result==false)
           {
              printf("Low_layer: the num.%d name=%s.layers=%d class=%d,priviouse class=%d!\n",\
                          num,temp_menuitem->node.name,layers,tab_list[num].byClass,current_class);
              FreeMenuitem(temp_menuitem);
              DelMenuBranch(menuitem_tree);
              return Mresult;
            }
        }
        else if(tab_list[num].byClass==current_class)
        {
              if(num==0)
              {
                 result=AddSubMenuitem(current_menuitem,temp_menuitem,true);//the first tab
              }
              else
              {
                 result=AddBroMenuitem(current_menuitem,temp_menuitem,false);
              }
              if(result==false)
              {
                printf("Equ_layer: the num.%d name=%s.layers=%d class=%d,priviouse class=%d!\n",\
                                      num,temp_menuitem->node.name,layers,tab_list[num].byClass,\
                                               current_class);
                FreeMenuitem(temp_menuitem);
                DelMenuBranch(menuitem_tree);
                  return Mresult;
              }
              else
              {

              }
        }
        else
        {
           for(layers=tab_list[num].byClass;layers<current_class;layers++)
           {
              current_menuitem=GetParMenuitem(current_menuitem);
           }
           result= AddBroMenuitem(current_menuitem, temp_menuitem,false);//first menuitem node
           if(result==false)
           {
              printf("High_layer: the num.%d name=%s.layers=%d class=%d,priviouse class=%d!\n",\
                   num,temp_menuitem->node.name,layers,tab_list[num].byClass,current_class);
              FreeMenuitem(temp_menuitem);
              DelMenuBranch(menuitem_tree);
                  return Mresult;
           }
         }
         current_menuitem=temp_menuitem;
         current_class=tab_list[num].byClass;
         count++;
     }
     if(count==size)
     {
         printf("Has mount the tab completely!---num=%d\n",count);
     }
     else
     {
         printf("Has mount part of the tab!count_should_be=%d  count=%d\n",\
         size,count+1);
         result= false;
     }
     Mresult=menuitem_tree;

     return Mresult;
}

// =========================================================================
// 函数功能:在menu_root节点下创建一个菜单树
// 输入参数:treename：树的根节点的名字，如果为空或者已经存在则会自动分配名字，
//                   自动分配的名字很难听；要自己有心里准备
//                   tab_list：用户菜单树意愿表
// 输出参数：
// 返回值  :添加的菜单树的根节点，该节点已经挂载在menu_root的子节点下，但是在添加不
//        成功的时候等情况下返回NULL；
// 说明    :首先检查tab_list的合法性，然后按照相对位置进行添加
// =========================================================================
struct menu_item  *create_menu_tree(const char *treename,const struct MenuitemModel * tab_list)
{

     struct menu_item    *result;//used to store the result
     struct menu_item    *temp;//used to store the result

     result=CreateMenuitemTreeNode(treename);
     if(NULL==result)
     {
         printf("failed create tree node---create_menu_tree\n");
         return result;
     }
     temp=append_menu_branch(result, tab_list);
     if(NULL==temp)
     {
       DelMenuBranchCompletely(result);
       result=NULL;
     }

     return result;
}

