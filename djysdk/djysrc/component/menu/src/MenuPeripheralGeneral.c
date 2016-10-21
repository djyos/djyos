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
// MenuPeripheralGeneral.c
// 模块描述:
// 模块版本: 针对外设的通用方法
// 创建时间: 2013-1-16
// 作者    : Administrator
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================

#include "MenuPeripheralGeneral.h"
#include "MenuDisplayGeneral.h"
#include "MenuSpaceGeneral.h"

#include "MenuitemApi.h"
// ==============================================================================================
// 函数功能:判断一个点是否在规定的区域内
// 输入参数:rec，规定的区域，myCoordinatePoint,待测试的坐标点
// 输出参数：true，点在矩形框内，false不在矩形框内
// 返回值  :
// 说明    :
// ==============================================================================================

bool_t JudgeCoordinatePointInRec(struct Rectangle rec, struct coordinate_point myCoordinatePoint)
{
    if((rec.left<=myCoordinatePoint.x)&&(rec.right>=myCoordinatePoint.x)&&\
       (rec.top<=myCoordinatePoint.y)&&(rec.bottom>=myCoordinatePoint.y))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// ==============================================================================================
// 函数功能:针对水平显示的显示参数进行触摸的反应
// 输入参数:operatingDispara，当前正在操作的显示参数，myCoordinatePoint，鼠标或者触摸的坐标
// 输出参数：
// 返回值  :经过更新的当前正在操作的菜单项
// 说明    :切换到相应的坐标的菜单项。(目前只支持在菜单之间的切换，不包括菜单项的展开和返回)
// ==============================================================================================
struct menu_displaypara *touchlcdactionhorizontal(struct menu_displaypara *operatingDispara,\
                                                  struct coordinate_point myCoordinatePoint)
{

    u8              menuNum=0,menuitemNumber=0;
    s8              step=0;//跳转的步长
    u8              FNumber=0;
    u8              ONumber=0;
    struct Rectangle  temp_rec={0,0,0,0};
    //内部使用，不用做参数检查


    temp_rec.left=operatingDispara->CurWin->absx0;
    temp_rec.right=operatingDispara->CurWin->absx0+operatingDispara->CurWin->right-operatingDispara->CurWin->left;
    temp_rec.top=operatingDispara->CurWin->absy0;
    temp_rec.bottom=operatingDispara->CurWin->absy0+operatingDispara->CurWin->bottom-operatingDispara->CurWin->top;
    if(JudgeCoordinatePointInRec(temp_rec,myCoordinatePoint))//先看是否在窗口内，不是的话什么也不做
    {
       menuNum=CalDisBettween2Menuitem(operatingDispara->FVisualMenuitem,operatingDispara->LVisualMenuitem)+1;
       temp_rec.left+=cn_space_margin/4;
       temp_rec.top+=cn_space_margin/4;
       temp_rec.right=temp_rec.left+menuNum*(operatingDispara->MenuitemSpace.width);
       temp_rec.bottom=temp_rec.top+operatingDispara->MenuitemSpace.height;
       if(JudgeCoordinatePointInRec(temp_rec,myCoordinatePoint))//看是否在窗口的可视区域内，否则什么都不用做
       {
           //看看落在第几个菜单项，从第一个可视菜单项算起
           if(operatingDispara->MenuitemSpace.width!=0)
           {
               //之所以-1，是因为我们算的是距离第一个可视菜单项的位置
               menuitemNumber=((myCoordinatePoint.x-temp_rec.left)-1)/(operatingDispara->MenuitemSpace.width);
               //
               FNumber=CalMenuitemNumber(operatingDispara->FVisualMenuitem);
               ONumber=CalMenuitemNumber(operatingDispara->OperatingMenuitem);
               step=FNumber+menuitemNumber-ONumber;
           //    printf("Fnumber=%d,Onumber=%d,menuitemnumber=%d step=%d\n",FNumber,ONumber,menuitemNumber,step);
               goto_menuitem_inslayers(operatingDispara,step);
           }
           else
           {
//             return operatingDispara;
           }

       }
       else
       {
//         return operatingDispara;
       }
    }
    else
    {
//      return operatingDispara;
    }
    return operatingDispara;
}
// ==============================================================================================
// 函数功能:针对垂直显示的显示参数进行触摸的反应
// 输入参数:operatingDispara，当前正在操作的显示参数，myCoordinatePoint，鼠标或者触摸的坐标
// 输出参数：
// 返回值  :经过更新的当前正在操作的菜单项
// 说明    :切换到相应的坐标的菜单项。(目前只支持在菜单之间的切换，不包括菜单项的展开和返回)
// ==============================================================================================
struct menu_displaypara *touchlcdactionvertical(struct menu_displaypara *operatingDispara,\
                                                struct coordinate_point myCoordinatePoint)
{
    struct Rectangle  temp_rec={0,0,0,0};
    u8              menuNum=0,menuitemNumber=0;
    s8              step=0;//跳转的步长
    u8              FNumber=0;
    u8              ONumber=0;

   //内部使用，不用做参数检查
    temp_rec.left=operatingDispara->CurWin->absx0;
    temp_rec.right=operatingDispara->CurWin->absx0+operatingDispara->CurWin->right-operatingDispara->CurWin->left;
    temp_rec.top=operatingDispara->CurWin->absy0;
    temp_rec.bottom=operatingDispara->CurWin->absy0+operatingDispara->CurWin->bottom-operatingDispara->CurWin->top;
    if(JudgeCoordinatePointInRec(temp_rec,myCoordinatePoint))//先看是否在窗口内，不是的话什么也不做
    {
       menuNum=CalDisBettween2Menuitem(operatingDispara->FVisualMenuitem,operatingDispara->LVisualMenuitem)+1;
       temp_rec.left+=cn_space_margin/4;
       temp_rec.top+=cn_space_margin/4;
       temp_rec.right=temp_rec.left+operatingDispara->MenuitemSpace.width;
       temp_rec.bottom=temp_rec.top+menuNum*(operatingDispara->MenuitemSpace.height);
       if(JudgeCoordinatePointInRec(temp_rec,myCoordinatePoint))//看是否在窗口的可视区域内，否则什么都不用做
       {
           //看看落在第几个菜单项，从第一个可视菜单项算起
           if(operatingDispara->MenuitemSpace.height!=0)
           {
               //之所以-1，是因为我们算的是距离第一个可视菜单项的位置
               menuitemNumber=((myCoordinatePoint.y-temp_rec.top)-1)/(operatingDispara->MenuitemSpace.height);
               //
               FNumber=CalMenuitemNumber(operatingDispara->FVisualMenuitem);
               ONumber=CalMenuitemNumber(operatingDispara->OperatingMenuitem);
               step=FNumber+menuitemNumber-ONumber;
            //   printf("Fnumber=%d,Onumber=%d,menuitemnumber=%d step=%d--length=%d\n",FNumber,ONumber,\
            //         menuitemNumber,step,(myCoordinatePoint.y-temp_rec.top)-1);
               goto_menuitem_inslayers(operatingDispara,step);
           }
           else
           {
//             return operatingDispara;
           }

       }
       else
       {
//         return operatingDispara;
       }
    }
    else
    {
//      return operatingDispara;
    }
    return operatingDispara;
}

// ==============================================================================================
// 函数功能:针对九格显示的显示参数
// 输入参数:operatingDispara，当前正在操作的显示参数，myCoordinatePoint，鼠标或者触摸的坐标
// 输出参数：
// 返回值  :经过更新的当前正在操作的菜单项
// 说明    :切换到相应的坐标的菜单项。(目前只支持在菜单之间的切换，不包括菜单项的展开和返回)
// ==============================================================================================
struct menu_displaypara *touchlcdaction3by3(struct menu_displaypara *operatingDispara,\
                                            struct coordinate_point myCoordinatePoint)
{
    struct Rectangle  temp_rec={0,0,0,0};
    s8              menuNum=0,menuitemNumber=0;
    s8              step=0;//跳转的步长
    s8              FNumber=0;
    s8              ONumber=0;
    s8              Line=0;
    s8              Row=0;

   //内部使用，不用做参数检查
    temp_rec.left=operatingDispara->CurWin->absx0;
    temp_rec.right=operatingDispara->CurWin->absx0+operatingDispara->CurWin->right-operatingDispara->CurWin->left;
    temp_rec.top=operatingDispara->CurWin->absy0;
    temp_rec.bottom=operatingDispara->CurWin->absy0+operatingDispara->CurWin->bottom-operatingDispara->CurWin->top;
    if(JudgeCoordinatePointInRec(temp_rec,myCoordinatePoint))//先看是否在窗口内，不是的话什么也不做
    {
       menuNum=CalDisBettween2Menuitem(operatingDispara->FVisualMenuitem,operatingDispara->LVisualMenuitem)+1;
       temp_rec.left+=(operatingDispara->CurWin->right-operatingDispara->CurWin->left-cn_space_margin)/2-(operatingDispara->MenuitemSpace.width*3)/2;;
       temp_rec.top+=(operatingDispara->CurWin->bottom-operatingDispara->CurWin->top-cn_space_margin)/2-(operatingDispara->MenuitemSpace.height*3)/2;
       temp_rec.right=temp_rec.left+operatingDispara->MenuitemSpace.width*3+cn_space_margin;
       temp_rec.bottom=temp_rec.top+operatingDispara->MenuitemSpace.height*3+cn_space_margin;
       if(JudgeCoordinatePointInRec(temp_rec,myCoordinatePoint))//看是否在窗口的可视区域内，否则什么都不用做
       {
           //看看落在第几个菜单项，从第一个可视菜单项算起
           if((operatingDispara->MenuitemSpace.height!=0)&&(0!=operatingDispara->MenuitemSpace.width))
           {
               //之所以-1，是因为我们算的是距离第一个可视菜单项的位置
               Line=((myCoordinatePoint.y-temp_rec.top)-cn_space_margin/2)/(operatingDispara->MenuitemSpace.height);
               Row=((myCoordinatePoint.x-temp_rec.left)-cn_space_margin/2)/(operatingDispara->MenuitemSpace.width);
               menuitemNumber=CalDisBettween2Menuitem(operatingDispara->FVisualMenuitem,operatingDispara->LVisualMenuitem);
               if((Line*3+Row)<=menuitemNumber)//在可视菜单项中
               {
                   FNumber=CalMenuitemNumber(operatingDispara->FVisualMenuitem);
                   ONumber=CalMenuitemNumber(operatingDispara->OperatingMenuitem);
                   step=FNumber+Line*3+Row-ONumber;
               //    printf("Fnumber=%d,Onumber=%d, L=%d R=%d menuitemnumber=%d step=%d\n",FNumber,ONumber,Line,Row,\
               //          menuitemNumber,step);
                   goto_menuitem_inslayers(operatingDispara,step);
               }
           }
           else
           {
//             return operatingDispara;
           }

       }
       else
       {
//         return operatingDispara;
       }
    }
    else
    {
//      return operatingDispara;
    }
    return operatingDispara;
}
// =========================================================================
// 函数功能:外设响应函数
// 输入参数:operatingDispara，当前正在操作的菜单项
// 输出参数：
// 返回值  :经过更新的当前正在操作的菜单显示参数
// 说明    :外设对于菜单显示参数的操作，无非就是对于菜单项的显示和隐藏
// =========================================================================

struct menu_displaypara *peripheralrespond_general(struct menu_displaypara *operatingDispara, void *para)
{
   struct menu_displaypara  *temp;
   u8 cmd=*(u8 *)(para);
   switch (cmd)
   {
      case GOTO_SUBMENU:
            temp=goto_submenu(operatingDispara);
            if(NULL!=temp)
            {
                operatingDispara=temp;
            }
            break;
      case GOTO_PARENT:
            temp=goto_parmenu(operatingDispara);
            if(NULL!=temp)
            {
                operatingDispara=temp;
            }
            break;
      case GOTO_NEXT:
           goto_nextmenuitem(operatingDispara);
           break;
      case GOTO_PRE:
           goto_premenuitem(operatingDispara);
           break;
      case  GOTO_EXE:
            operatingDispara=exe_menuitemfunc(operatingDispara);
            break;
      default:
              break;
   }
   return operatingDispara;
}