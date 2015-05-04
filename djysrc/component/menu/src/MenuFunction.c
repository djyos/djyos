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
// 模块描述: DJY菜单模块之菜单项钩子函数定义
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// 说明          ：如果你要定义更多的钩子函数，请在下面添加钩子函数的定义，并添加到
//          对应的函数表中。
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================
#include "stdint.h"
#include "MenuFunction.h"
#include "MenuDisplay.h"
// todo : 加注释
void  Menuitem_func(struct menu_item *menuitem)
{

  bool_t  result=true;
  u8  i=0;
  char *content="Welcome-DJYOS!";

  if((NULL==menuitem)||(NULL==menuitem->myprivate))
  {
      return;
  }
  else
  {
      printf("In menuitem function!--menuitem name=%s\n",menuitem->node.name);
  }

  GK_ApiDrawText(menuitem->myprivate,NULL,NULL,0,30,content,strnlen(content,namesize),CN_COLOR_BLUE,0,CN_TIMEOUT_FOREVER);
  //
  for(i=0;i<3;i++)
  {
      printf("wait_%d second\n",i);
      Djy_EventDelay(1000*mS);
  }
  return;
}

void  ConnectSwitcher(struct menu_item *menuitem)//合断路器
{

  printf("In menuitem function!---Begin to connect the switcher!\n");

}
void  CheckEquipment(struct menu_item *menuitem)//查看装置信息
{
    bool_t  result;
    u8      keyvalue;
    int     i=0;
    s32    width=0;
    s32    height=0;

    struct draw_wintext_para para;

    struct tagInputDeviceMsg *input_msg;
    struct tagGkWinRsc  CheckWin_text;
    struct tagGkWinRsc  *Appwin=NULL;
    char*   content="装置名称:/装置参数:/软件RCR :/工程日期:";

    if((NULL==menuitem)||(NULL==menuitem->myprivate))
    {
       printf("Invalid parameter---CheckEquipment\n");
       return ;
    }
    Appwin=(struct tagGkWinRsc*)(menuitem->myprivate);

    width=Appwin->right-Appwin->left;
    height=Appwin->bottom-Appwin->top;
    result=GK_ApiCreateGkwin(Appwin, &CheckWin_text, 0, 0, width/2+16, height,\
                             0x00202060, CN_WINBUF_NONE, "Checkwin_text",0,0,0, 0);


    para.colorback=0x00202060;
    para.colortext=CN_COLOR_WHITE;
    para.gkwin=&CheckWin_text;
    para.x=0;
    para.y=0;
    para.count=strlen(content);
//    windows_draw_text(&para,content);
    GK_ApiDrawText(&CheckWin_text,NULL,NULL,0,0,"装置名称：",10,0,CN_COLOR_BLUE,0);
    GK_ApiDrawText(&CheckWin_text,NULL,NULL,0,16,"装置参数：",10,0,CN_COLOR_BLUE,0);
    GK_ApiDrawText(&CheckWin_text,NULL,NULL,0,32,"软件CRC：",9,0,CN_COLOR_BLUE,0);
    GK_ApiDrawText(&CheckWin_text,NULL,NULL,0,48,"工程日期：",10,0,CN_COLOR_BLUE,CN_TIMEOUT_FOREVER);

    for ( ; ; )
    {
//      input_msg = (struct tagInputDeviceMsg *)Djy_GetEventPara(NULL,NULL);
        if(Stddev_ReadDefaultMsg(&input_msg,20*mS))
        {
            //查看是否单点触摸屏
            if(input_msg->input_type == EN_STDIN_SINGLE_TOUCH)
            {


            }
            else if(input_msg->input_type == EN_STDIN_KEYBOARD)//查看是否键盘
            {
                //按键松开才确认，因此先判断是否断码
                if(input_msg->input_data.key_board.key_value[1] != CN_BREAK_CODE)
                {

              //键盘响应测试
                   keyvalue= input_msg->input_data.key_board.key_value[0];
                 if(keyvalue==27)
                   {
                       printf("received a key:%02d\n",keyvalue);
                       break;
                   }
                   else
                   {
                       printf("received a key:%02d\n",keyvalue);
                   }

                }
            }
//          Djy_ParaUsed(0);
        }
//      Djy_WaitEvttPop(Djy_MyEvttId(),1,20*mS);
    }

    GK_ApiDestroyWin(&CheckWin_text);
    return;
}

struct menu_item_function   menufunc_tab[]=//menu item hook function tab
{
 {"Menuitem_func",Menuitem_func},

 {"ConnectSwitcher",ConnectSwitcher},
 {"CheckEquipment",CheckEquipment}

};


void NULL_FUNC(void)
{

   printf("This is an NULL func,you have not supply func yet!\n");
}
const u16 MenuitemFuncSize=sizeof(menufunc_tab)/sizeof(struct menu_item_function);


// =========================================================================
// 函数功能:根据名字寻找相关的菜单钩子函数
// 输入参数:钩子函数的名字
// 输出参数：
// 返回值  :
// 说明    :寻找的同名的钩子函数
// =========================================================================
void*   getfunc_byname(char *funcname)
{
   u16 i=0;
   while(i<MenuitemFuncSize)
   {
     if(0==strcmp(funcname,menufunc_tab[i].Funcname))
     {
        break;
     }
     else
     {
       i++;
     }
   }
   if(i==MenuitemFuncSize)
   {

     return NULL_FUNC;
   }
   else
   {
    return menufunc_tab[i].Func;
   }
}







