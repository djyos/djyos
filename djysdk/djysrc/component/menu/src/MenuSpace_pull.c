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
// MenuSpace_pull.c
// 模块描述: DJY菜单模块之下拉模式空间计算方法
// 模块版本:
// 创建时间: 2012-12-27
// 作者          : Administrator
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================


#include "MenuSpace_pull.h"

#include "MenuitemApi.h"

// =========================================================================
// 函数功能:为下拉模式分计算显示空间
// 输入参数:pre，前级显示参数，cur当前显示参数
// 输出参数：
// 返回值     :通过计算得到的下拉模式主菜单或者子菜单所需的空间
// 说明          :pre为NULL，则证明是为主菜单分配空间，否则是为子菜单分配空间
//          该显示参数所占用的空间，当然，要依赖前级菜单
//          对于下拉显示模式而言，只有主菜单是横向，其余的都是纵向显示
// =========================================================================
//根据罗总的意见进行了修改
//对于主菜单，依然建立的是铺满整屏的窗口
//对于以及主菜单，建立在当前操作项的正下面（优先），如果位置不够，要向左移动。
//对于二级子菜单及其以后的子菜单，都优先建立在当前菜单的右边，如果右边位置不够，则建立在左边；
struct Rectangle   CalDisAreaFromPre_pull(struct menu_displaypara *pre,struct menu_displaypara  *cur)
{
   u8        number=0;//用于计算第几个窗口弹出的
   u32                     menuitem_num;//当前菜单有多少个菜单项
   s32     limit_end;//可视域边界点
   s32     limit_start;//可视起点
   s32     cal_end;//计算边界
   s32     cal_start;//计算起点
   s32     Midpoint;//中间位置
   u32     length=0;
   s32    length_left=0;//存储可以划分的显示区域
   s32    length_need=0;//存储需要的显示区域
   struct Rectangle  result={0,0,0,0};//计算的结果
   struct menu_displaypara  *pMainMenuDispara;//暂时页节点

    //只是内部调用，不再进行参数检测，以提高效率
    cur->MenuitemSpace=cal_menuitemarea(cur->FVisualMenuitem);
    if(NULL==pre)//主菜单
    {
      return  result;//新建一个铺满整屏的窗口
    }
    else//这个要考虑多种情况，比较复杂
    {
       menuitem_num=CalMenuitemNum(cur->FVisualMenuitem);
       number=CalDisBettween2Menuitem(pre->FVisualMenuitem, pre->OperatingMenuitem);
      //计算水平位置
       if(NULL==pre->pre)//前级是水平，即主菜单
       {
          limit_start=(pre->MenuitemSpace.width)*number+cn_space_margin;
          limit_end=pre->CurWin->right-pre->CurWin->left;
          cal_start=limit_start;
          cal_end=cal_start+cur->MenuitemSpace.width+cn_space_margin;
          if(cal_end>limit_end)//水平位置不够，看看能否进行调整
          {
            //右边不够，就向左移动，实在不够那就没有办法了
            if((limit_end)>(cal_end-cal_start))//当前层还可以容纳下
            {
                result.left=limit_end-cur->MenuitemSpace.width-cn_space_margin;
                result.right=limit_end;
            }
            else//容纳不下，一般的是不可能的，除非屏幕足够窄，或者当前菜单层太宽
            {
               result.left=0;
               result.right=limit_end;
            }
          }
          else//水平位置够了
          {
             result.left=cal_start;
             result.right=cal_end;
          }
      }
      else//二级子菜单及其以下,先看右边够不够，够用的话就右边，不够的话就左边，
      {
          length_need=cur->MenuitemSpace.width+cn_space_margin;
          length_left=pre->AppWin->absx0+pre->AppWin->right-pre->AppWin->left-pre->CurWin->right-pre->CurWin->absx0+pre->CurWin->left;
          if(length_need>length_left)//右边不够，
          {
            //看看左边够不够
            length_left=pre->CurWin->absx0-pre->AppWin->absx0;
            length_need=cur->MenuitemSpace.width+cn_space_margin;
            if(length_need>length_left)//左边也不够
            {
              //看看是左边多，还是右边多，哪边多就放哪边
              length_need=pre->AppWin->absx0+pre->AppWin->right-pre->AppWin->left-pre->CurWin->right-pre->CurWin->absx0+pre->CurWin->left;
              if(length_left>length_need)//左边多
              {
                 result.left=-(length_left);
                 result.right=0;
              }
              else//右边多
              {
                 result.left=pre->CurWin->right-pre->CurWin->left;
                 result.right=result.left+length_need;
              }

            }
            else//左边够
            {
                result.right=0;
                result.left=result.right-length_need;
            }
          }
          else//右边够
          {
             result.left=pre->CurWin->right-pre->CurWin->left;
             result.right=result.left+length_need;
          }
      }
      //根据罗总的意见，都是在一页的。top和bottom受主菜单水平显示的限制
      pMainMenuDispara=getmainmenudispara(pre);//获取主菜单
      if(NULL==pMainMenuDispara)
      {
          printf("fata_err_in cal pull space!");
          return result;
      }

      limit_start=pre->AppWin->absy0+pMainMenuDispara->MenuitemSpace.height+cn_space_margin-pre->CurWin->absy0;
      length_need=(cur->MenuitemSpace.height)*menuitem_num+cn_space_margin;
      limit_end=cur->AppWin->absy0+cur->AppWin->bottom-cur->AppWin->top-pre->CurWin->absy0;
      if(NULL==pre->pre)//直接在主菜单下，水平弹出垂直菜单
      {
          if(length_need>(limit_end-limit_start))//考虑空间大小,空间不足，考虑分配整数个。
          {
            result.top=limit_start;
            result.bottom=result.top+((limit_end-limit_start)/(cur->MenuitemSpace.height))*(cur->MenuitemSpace.height)+cn_space_margin;
            return result;
          }
          else
          {
             result.top=limit_start;
             result.bottom=result.top+length_need;
             return result;
          }
      }
      else//垂直弹出垂直菜单
      {
          if(length_need>(limit_end-limit_start))//空间不富裕，不必考虑中间位置了
          {
             result.top=limit_start;
             result.bottom=limit_end;
             return result;
          }
          else//有富余，考虑中间位置
          {
             Midpoint=pre->MenuitemSpace.height*number+pre->MenuitemSpace.height/2;
             if(((Midpoint-limit_start)>=length_need/2)&&(limit_end-Midpoint)>=length_need/2)//两边都满足，可以放在中间
             {
                 result.top=Midpoint-length_need/2;
                 result.bottom=length_need+result.top;
             }
             else if((Midpoint-limit_start)<length_need/2)//上边不够
             {
                 result.top=limit_start;
                 result.bottom=length_need+result.top;
             }
             else//下边不够
             {
                  result.bottom=limit_end;
                  result.top=result.bottom-length_need;
             }
             return  result;
          }
      }
    }
}
// =========================================================================
// 函数功能:根据下拉模进行显示项的调整
// 输入参数:待调整的显示参数dispara
// 输出参数：
// 返回值 :
// 说明   :将修改显示参数的FVisualMenuitem和LVisualMenuitem，对于主菜单则不会修改，
//          考虑了需要滚屏的情况，尽最大可能的显示更多的菜单项
// =========================================================================
void adjustvisualmenuitem_pull(struct menu_displaypara  *dispara)
{

     u8            dis_num;//窗口可以显示的菜单项条目
     u8           OperaNumber=0;
     u8           FVisualNumber=0;
     u8           LVisualNumber=0;
     s32           length_Visual;//可视域的宽度
     s32           length_E;//每条菜单项占用的长度，水平指的是width，垂直指的是height
     struct menu_item   *BBmenuitem=NULL,*LBmenuitem=NULL,*tempmenuitem=NULL;

    //参数检查
    if((NULL==dispara)||(NULL==dispara->OperatingMenuitem))
    {
        printf("Invalid para for Adjust!\n");
        return;
    }
    if(NULL==dispara->pre)//主菜单，横向
    {
        length_Visual=dispara->CurWin->right-dispara->CurWin->left;
    //  deep_limit=dispara->AppWin->absx0+dispara->AppWin->right-dispara->AppWin->left;
        length_E=dispara->MenuitemSpace.width;
    }
    else//子菜单，纵向
    {
        length_Visual=dispara->CurWin->bottom-dispara->CurWin->top;
    //    deep_limit=dispara->AppWin->absy0+dispara->AppWin->bottom-dispara->AppWin->top;
        length_E=dispara->MenuitemSpace.height;
    }
    if(length_E==0)
    {
        printf("Menu space is NULL, so do nothing!\n");
        return;
    }
     dis_num=length_Visual/(length_E);

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
    //进行调试的信息，进行显示校对，看是否正确。
  //  printf("BB_menuitem=%s\n",BBmenuitem->node.name);
//  printf("Fv_menuitem=%s\n",dispara->FVisualMenuitem->node.name);
//  printf("Op_menuitem=%s\n",dispara->OperatingMenuitem->node.name);
//  printf("Lv_menuitem=%s\n",dispara->LVisualMenuitem->node.name);
//  printf("LB_menuitem=%s\n\n",LBmenuitem->node.name);
    return;
}

// =========================================================================
// 函数功能:为下拉显示模式创建显示参数
// 输入参数:pre，前级显示参数，menuitem，当前显示参数所关联的菜单项，Appwin,应用程序提供的显示窗口
//          注意当pre为空的时候表示的是创建主显示参数，这时候会用到APPWIN和menuitem,否则这两项会从
//          pre中提取
// 输出参数：
// 返回值  :
//需要说明的是:位置前面都计算好了，已经没有啥子好说的了
// =========================================================================
struct menu_displaypara *createmenudisparabypre_pull(struct menu_displaypara *pre,struct menu_item *menuitem,struct GkWinRsc  *Appwin)
{

  bool_t  result;
  struct menu_displaypara  *cur=NULL;
  struct Rectangle   Disparawin_rec;
 // struct GkWinRsc  *win;
   //简单的根据前一个菜单窗口创建

  if((NULL==pre)&&((NULL==menuitem)||(NULL==Appwin)))
  {
     printf("Invalid parameter----createmenudisparabypre_pull\n");
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
   if((NULL==cur->OperatingMenuitem)||(NULL==cur->AppWin))
   {
      deldisplaypara(cur);
      return cur;
   }
  //计算空间
   Disparawin_rec=CalDisAreaFromPre_pull(pre,cur);
 //创建窗口
   result=create_menudisparawin(cur, Disparawin_rec, cur->OperatingMenuitem->node.parent->name);

   if(!result)
   {
      printf("Failed---createmenudisparabypre_pull!\n");
      deldisplaypara(cur);
      cur=NULL;
      return cur;
   }

   return cur;
}

