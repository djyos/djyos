
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
// 模块描述: DJY菜单模块之九宫格显示空间计算
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================

#include "MenuSpace_9cell.h"

#include "MenuitemApi.h"

// =========================================================================
// 函数功能:为9宫格显示方法分配显示空间
// 输入参数:pre，前级显示参数，cur当前显示参数
// 输出参数：
// 返回值  :通过计算得到的九宫格主菜单或者子菜单所需的空间
// 说明    :pre为NULL，则证明是为主菜单分配空间
//          pre->pre==NULL,则证明是从主菜单弹出子菜单
//          其余的是子菜单弹出子菜单
//          该显示参数所占用的空间，当然，要依赖前级菜单
//          对于九宫格而言，子菜单弹出子菜单是从垂直菜单弹出垂直菜单
//                           主菜单弹出子菜单是从九格弹出垂直菜单
// =========================================================================
/*
static struct Rectangle   CalDisAreaFromPre_9cell(struct menu_displaypara *pre,struct menu_displaypara  *cur)
{


   struct Rectangle  result={0,0,0,0};//计算的结果
   //内部调用，无需参数检查

   cur->MenuitemSpace=cal_menuitemarea(cur->OperatingMenuitem);
    //根据罗总意见，不论是主菜单还是子菜单都是重新建立一个全页菜单。
    return result;
}
*/
struct Rectangle   CalDisAreaFromPre_9cell(struct menu_displaypara *pre,struct menu_displaypara  *cur)
{

   u32       menuitem_num;//当前菜单有多少个菜单项

   struct Rectangle  result={0,0,0,0};//计算的结果

   u8        number=0;//用于计算第几个窗口弹出的

    s32     limit_end;//可视域边界点
    s32     limit_start;//可视起点
    s32     cal_end;//计算边界
    s32     cal_start;//计算起点

    s32     Midpoint;//中间位置

    u32     length=0;


    //参数检测
    if(NULL==cur)
    {
      printf("Invalid para for cal 9cell space!\n");
      return result;
    }
    if(NULL==cur->FVisualMenuitem)
    {
      printf("Invalid para for cal 9cell space!\n");
      return result;
    }

    cur->MenuitemSpace=cal_menuitemarea(cur->FVisualMenuitem);
    if(NULL==pre)//主菜单
    {
      return  result;//新建页
    }
    else if(NULL==pre->pre)//主菜单弹出子菜单
    {
      return result;//新建页
    }
    else//这个要考虑多种情况，比较复杂
    {

       menuitem_num=CalMenuitemNum(cur->FVisualMenuitem);
       number=CalDisBettween2Menuitem(pre->FVisualMenuitem, pre->OperatingMenuitem);
      //计算水平位置
      limit_start=pre->MenuitemSpace.width+cn_space_margin;
      limit_end=cur->AppWin->absx0+cur->AppWin->right-cur->AppWin->left-pre->CurWin->absx0;
      cal_start=limit_start;//在九宫格中这两者是相同的
      cal_end=cal_start+cur->MenuitemSpace.width+cn_space_margin;
      if(cal_end>limit_end)//水平位置不够，则一票否决
      {
        return result;
      }
      else
      {
         result.left=cal_start;
         result.right=cal_end;
      }
      //计算垂直位置，记得放在中间
      length=(cur->MenuitemSpace.height)*menuitem_num+cn_space_margin;
      limit_start=cur->AppWin->absy0-pre->CurWin->absy0;
      limit_end=limit_start+cur->AppWin->bottom-cur->AppWin->top;
      if(length>(limit_end-limit_start))//空间不富裕，不必考虑中间位置了
      {
         result.top=limit_start;
         result.bottom=limit_end;
         return result;
      }
      else//有富余，考虑中间位置
      {
          Midpoint=pre->MenuitemSpace.height*number+pre->MenuitemSpace.height/2;
          if(((Midpoint-limit_start)>=length/2)&&(limit_end-Midpoint)>=length/2)//两边都满足，可以放在中间
           {
                 result.top=Midpoint-length/2;
                 result.bottom=length+result.top;
           }
          else if((Midpoint-limit_start)<length/2)//上边不够
          {
               result.top=limit_start;
               result.bottom=length+result.top;
          }
          else//下边不够
          {
              result.bottom=limit_end;
              result.top=result.bottom-length;
          }
          return  result;
       }
     }
}

// =========================================================================
// 函数功能:根据九格的形式进行显示项的调整
// 输入参数:待调整的显示参数dispara
// 输出参数：
// 返回值    :
// 说明         :将修改显示参数的FVisualMenuitem和LVisualMenuitem，对于主菜单则不会修改，
//          考虑了需要滚屏的情况，尽最大可能的显示更多的菜单项
// =========================================================================
void adjustvisualmenuitem_9cell(struct menu_displaypara  *dispara)
{

     u8            dis_num;//窗口可以显示的菜单项条目
     u8           OperaNumber=0;
     u8           FVisualNumber=0;
     u8           LVisualNumber=0;

     s32           dis_height;//可视域的宽度
     s32           deep_limit;
     s32           deep_cal;

    struct menu_item   *BBmenuitem=NULL,*LBmenuitem=NULL,*tempmenuitem=NULL;
    //参数检查
    if((NULL==dispara)||(NULL==dispara->OperatingMenuitem))
    {
        printf("Invalid parameter----adjustvisualmenuitem_9cell\n");
        return;
    }
    if(NULL==dispara->pre)
    {
    //  printf("No need to adjsutVisualMenuitem for main menuitem!\n");
      dis_num=9;
    }
    else
    {
       deep_cal=dispara->CurWin->absy0+dispara->CurWin->bottom-dispara->CurWin->top;
       deep_limit=dispara->AppWin->absy0+dispara->AppWin->bottom-dispara->AppWin->top;
       if(deep_cal>deep_limit)//有超出的部分
       {
        dis_height=deep_limit-dispara->CurWin->absy0;
       }
       else
       {
        dis_height=deep_cal-dispara->CurWin->absy0;
       }
       if(dis_height==0)
       {
            printf("Menu space is NULL, so do nothing!\n");
            return;
       }
       dis_num=dis_height/(dispara->MenuitemSpace.height);
    }

    BBmenuitem=GetBBmenuitem(dispara->OperatingMenuitem);
    LBmenuitem=GetLBmenuitem(dispara->OperatingMenuitem);
    tempmenuitem=dispara->OperatingMenuitem;
    if(dis_num>=CalMenuitemNum(dispara->OperatingMenuitem))//能够容纳全部;
    {
         dispara->FVisualMenuitem=BBmenuitem;
         dispara->LVisualMenuitem=LBmenuitem;
    }
    else//容纳不了
    {
        //考虑翻页的情况
        FVisualNumber=CalMenuitemNumber(dispara->FVisualMenuitem);
        LVisualNumber=CalMenuitemNumber(dispara->LVisualMenuitem);
        OperaNumber=CalMenuitemNumber(dispara->OperatingMenuitem);
        if(OperaNumber<FVisualNumber)//当前操作项已经转到第一显示项前面，向前翻一页
        {
          if(dispara->OperatingMenuitem==BBmenuitem)//从最未位到达最前面，因为是循环的
          {
            tempmenuitem=dispara->OperatingMenuitem;
            while(dis_num>1) //将LastVisual指针后调
             {
               if(tempmenuitem==LBmenuitem)
                {
                   break;
                }
               tempmenuitem=GetNextMenuitem(tempmenuitem);
               dis_num--;
              }
             dispara->LVisualMenuitem=tempmenuitem;
             dispara->FVisualMenuitem=dispara->OperatingMenuitem;
          }
          else//顺序到达的
          {
            tempmenuitem=dispara->OperatingMenuitem;
            while(dis_num>1) //将SlayerFrist指针前调
             {
               if(tempmenuitem==BBmenuitem)
                {
                   break;
                }
               tempmenuitem=GetPreMenuitem(tempmenuitem);
               dis_num--;
              }
             dispara->FVisualMenuitem=tempmenuitem;
             dispara->LVisualMenuitem=dispara->OperatingMenuitem;
           }
        }
        else if(OperaNumber>LVisualNumber)//当前操作项已经转到最后显示项后面，向后翻一页
        {
          if(dispara->OperatingMenuitem==LBmenuitem)//从最前位到达最后面，因为是循环的
          {
            tempmenuitem=dispara->OperatingMenuitem;
            while(dis_num>1) //将FirstVisual指针后调
             {
               if(tempmenuitem==BBmenuitem)
                {
                   break;
                }
               tempmenuitem=GetPreMenuitem(tempmenuitem);
               dis_num--;
              }
             dispara->FVisualMenuitem=tempmenuitem;
             dispara->LVisualMenuitem=dispara->OperatingMenuitem;
          }
          else//顺序到达的
          {
            tempmenuitem=dispara->OperatingMenuitem;
            while(dis_num>1) //将SlayerFrist指针前调
             {
               if(tempmenuitem==LBmenuitem)
                {
                   break;
                }
               tempmenuitem=GetNextMenuitem(tempmenuitem);
               dis_num--;
              }
             dispara->LVisualMenuitem=tempmenuitem;
             dispara->FVisualMenuitem=dispara->OperatingMenuitem;
           }
        }
        else//没有超出显示页，尽可能的显示多
        {
          if(dis_num<=CalDisBettween2Menuitem(dispara->FVisualMenuitem, dispara->LVisualMenuitem))//在最初始分配的时候会出现
          {
             tempmenuitem=dispara->OperatingMenuitem;
             while(dis_num>1) //将SlayerFrist指针前调
             {
               if(tempmenuitem==BBmenuitem)
               {
                  break;
               }
               tempmenuitem=GetPreMenuitem(tempmenuitem);
               dis_num--;
             }
             dispara->FVisualMenuitem=tempmenuitem;
             tempmenuitem=dispara->OperatingMenuitem;
             while(dis_num>1) //将SlayerLast指针后调
             {
               if(tempmenuitem==LBmenuitem)
               {
                 break;
               }
              tempmenuitem=GetNextMenuitem(tempmenuitem);
               dis_num--;
             }
             dispara->LVisualMenuitem=tempmenuitem;
          }
          else
          {
           //不予改变可视的第一项和最后一项
          }

        }

     }
    //进行调试的信息，看看到底对否
  //  printf("BB_menuitem=%s\n",BBmenuitem->node.name);
 // printf("Fv_menuitem=%s\n",dispara->FVisualMenuitem->node.name);
//  printf("Op_menuitem=%s\n",dispara->OperatingMenuitem->node.name);
//  printf("Lv_menuitem=%s\n",dispara->LVisualMenuitem->node.name);
//  printf("LB_menuitem=%s\n\n",LBmenuitem->node.name);
    return;
}

// =========================================================================
// 函数功能:为九格显示参数创建显示参数
// 输入参数:pre，前级显示参数，menuitem，当前显示参数所关联的菜单项，Appwin,应用程序提供的显示窗口
//          注意当pre为空的时候表示的是创建主显示参数，这时候会用到APPWIN和menuitem,否则这两项会从
//          pre中提取
// 输出参数：
// 返回值  :
//需要说明的是:位置前面都计算好了，已经没有啥子好说的了
// =========================================================================
struct menu_displaypara *createmenudisparabypre_9cell(struct menu_displaypara *pre,struct menu_item *menuitem,struct GkWinRsc  *Appwin)
{

  bool_t  result;
  struct menu_displaypara  *cur=NULL;
  struct Rectangle   Disparawin_rec;
 // struct GkWinRsc  *win;
   //简单的根据前一个菜单窗口创建

  if((NULL==pre)&&((NULL==menuitem)||(NULL==Appwin)))
  {
     printf("Not enough  para to create the menudispara,You must be joking!\n");
  }
  cur=mallocdisplaypara();
  if(NULL==cur)
  {
     printf("Not enough mem for CreateParaMenuwinBypre!\n");
     return cur;
  }

  if(NULL==pre)
  {
     // printf("This is the main menu!\n");

      cur->AppWin=Appwin;
      cur->FVisualMenuitem=GetBBmenuitem(menuitem);
      cur->OperatingMenuitem=menuitem;
      cur->LVisualMenuitem=GetPreMenuitem(cur->FVisualMenuitem);

      cur->MyMenuDispalyFunction=NULL;
      cur->MyPeripheralGeneralAction=NULL;
      cur->MyDisplayPeripheralRespond=NULL;
      cur->AdjustVisualMenuitem=NULL;
      cur->CreateMenuDisparaBypre=NULL;
      cur->MyMenuInitFunction=NULL;
  }
  else
  {
      // printf("This is the sub menu!\n");
       cur->AppWin=pre->AppWin;
       cur->FVisualMenuitem=GetSonMenuitem(pre->OperatingMenuitem);
       cur->OperatingMenuitem=cur->FVisualMenuitem;
       cur->LVisualMenuitem=GetPreMenuitem(cur->FVisualMenuitem);
       //初始化一些函数指针
       cur->menukeyboard_action = pre->menukeyboard_action;
       cur->menumousetouch_ction = pre->menumousetouch_ction;
       cur->MyPeripheralGeneralAction = pre->MyPeripheralGeneralAction;

       cur->MyMenuDispalyFunction=pre->MyMenuDispalyFunction;
       cur->MyPeripheralGeneralAction=pre->MyPeripheralGeneralAction;
       cur->MyDisplayPeripheralRespond=pre->MyDisplayPeripheralRespond;
       cur->AdjustVisualMenuitem=pre->AdjustVisualMenuitem;
       cur->CreateMenuDisparaBypre=pre->CreateMenuDisparaBypre;
       cur->MyMenuInitFunction=pre->MyMenuInitFunction;
       //添加到链表中
       adddisplaypara(pre,cur);
    }
  //计算空间
   Disparawin_rec=CalDisAreaFromPre_9cell(pre,cur);
 //创建窗口
   result=create_menudisparawin(cur, Disparawin_rec, cur->FVisualMenuitem->node.name);
   if(!result)
   {
      printf("Failed--- createmenudisparabypre_9cell!\n");
      deldisplaypara(cur);
      cur=NULL;
      return cur;
   }
   return cur;
}





