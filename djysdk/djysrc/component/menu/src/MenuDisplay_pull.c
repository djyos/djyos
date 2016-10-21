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
// MenuDisplay_pull.c
// 模块描述:
// 模块版本:
// 创建时间: 2013-1-6
// 作者    : Administrator
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================

#include  "MenuDisplay_pull.h"
#include  "MenuDisplayGeneral.h"
#include  "MenuSpaceGeneral.h"
#include  "MenuitemApi.h"

// =========================================================================
// 函数功能：画状态条(最右边)(下拉模式)
// 输入参数:dispara：代表的显示参数
// 输出参数：
// 返回值  :
// 说明    :一般而言dispara的所有菜单项因为屏幕的原因不可能同时显示出来，此状态条
//          代表当前操作的菜单项在同层所有菜单中的位置
// =========================================================================
void drawstatecontent_pull_rightside(struct menu_displaypara  *dispara)
{
    u8  numOp=0;
    u8  numTotal=0;
    s32 lengthT=0;
    s32 LengthE=0;
    struct Rectangle  state_area={0,0,0,0};
    numOp=CalMenuitemNumber(dispara->OperatingMenuitem);
    numTotal=CalMenuitemNum(dispara->FVisualMenuitem);
    dispara=getmainmenudispara(dispara);
    //复原颜色
       state_area.left=dispara->CurWin->right-dispara->CurWin->left-cn_column_space;
       state_area.right=dispara->CurWin->right-dispara->CurWin->left;

       state_area.top=0;
       state_area.bottom=dispara->CurWin->bottom-dispara->CurWin->top;
       GK_ApiFillRect(dispara->CurWin, &state_area, color_back,
                      0,CN_FILLRECT_MODE_N,0);
       //标志当前的可视区
       lengthT=state_area.bottom-state_area.top;
       //提高精度；
       lengthT=lengthT*1000;

       LengthE=(lengthT+numTotal-1)/numTotal;

       state_area.top=(numOp-1)*LengthE;
       state_area.top=(state_area.top)/1000;

       state_area.bottom=(numOp)*LengthE;
       state_area.bottom=(state_area.bottom)/1000;

       GK_ApiFillRect(dispara->CurWin, &state_area, color_light,
                      0,CN_FILLRECT_MODE_N,0);
       GK_ApiSyncShow(CN_TIMEOUT_FOREVER);
        return;
}
// =========================================================================
// 函数功能：画状态条(最底边)
// 输入参数:dispara：代表的显示参数
// 输出参数：
// 返回值  :
// 说明    :一般而言dispara的所有菜单项因为屏幕的原因不可能同时显示出来，此状态条
//          代表当前操作的菜单项在同层所有菜单中的位置
// =========================================================================
//void DrawStateContentBottom(struct menu_displaypara  *dispara)
//{
//
//       struct Rectangle  state_area={0,0,0,0};
//
//
//     u8  numOp=0;
//     u8  numTotal=0;
//     s32 lengthT=0;
//     s32 LengthE=0;
//
//
//     numOp=CalMenuitemNumber(dispara->OperatingMenuitem);
//     numTotal=CalMenuitemNum(dispara->FVisualMenuitem);
//
//    //将状态栏画在最近的一个页节点上
////      dispara=SearchLatestPageNode(dispara);//关于页节点的问题仍需要进一步的考虑
//      //复原颜色
//       state_area.left=0;
//     state_area.right=dispara->CurWin->right-dispara->CurWin->left;
//
//     state_area.top=dispara->CurWin->bottom-dispara->CurWin->top-cn_column_space;
//     state_area.bottom=dispara->CurWin->bottom-dispara->CurWin->top;
//        GK_ApiFillRect(dispara->CurWin, &state_area, color_back,
//                       0,CN_FILLRECT_MODE_N,0);
//     //标志当前的可视区
//     lengthT=state_area.right-state_area.left;
//     //提高精度；
//     lengthT=lengthT*1000;
//
//     LengthE=(lengthT+numTotal-1)/numTotal;
//
//     state_area.left=(numOp-1)*LengthE;
//     state_area.left=(state_area.left)/1000;
//
//     state_area.right=(numOp)*LengthE;
//     state_area.right=(state_area.right)/1000;
//        GK_ApiFillRect(dispara->CurWin, &state_area, color_light,
//                       0,CN_FILLRECT_MODE_N,0);
//      GK_ApiSyncShow(CN_TIMEOUT_FOREVER);
//        return;
//}

// =========================================================================
// 函数功能:下拉显示方法的服务器
// 输入参数:待显示的显示参数dispara
// 说明          :
//          下拉显示方案，
//          目前下拉显示方案采用的是主界面采用下拉的方式，子菜单都采用下拉的方式
// =========================================================================
void  menu_display_pull(struct menu_displaypara  *dispara)
{

   if((NULL==dispara)||(NULL==dispara->OperatingMenuitem)||(NULL==dispara->CurWin))
   {
     printf("Invalid parameter---menu_display_pull!\n");
     return ;
   }

   if(NULL==dispara->pre)//主菜单，水平显示，是不用画状态栏的
   {
      display_menu_horizontal(dispara);
      drawstatecontent_pull_rightside(dispara);
   }
   else//子菜单，下拉模式
   {
      display_menu_vertical(dispara);
      drawstatecontent_pull_rightside(dispara);
   }
   printf("Operatingmenu=%s\n",dispara->OperatingMenuitem->node.name);

    return ;
}

