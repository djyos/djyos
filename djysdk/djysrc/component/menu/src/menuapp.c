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
// 模块描述: DJY菜单模块之用户应用例程
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================
#include "stdint.h"
#include "cpu_peri.h"
#include "gkernel.h"

#include "cpu_peri_lcd_touch_mouse.h"


#include "MenuDesigner.h"
#include "MenuitemApi.h"

#include "MenuData.h"
#include "MenuInitGeneral.h"
#include "MenuPeripheral_9cell.h"
#include "MenuPeripheral_pull.h"
#include "MenuInitGeneral.h"


//DISMODE，0：下拉，1：九宫格
#define   DISMODE  1
void start_menu(void)
{
    struct InputDeviceMsg input_msg;
    s32 touch_x=0, touch_y=0;
    struct coordinate_point   mypoint={0,0};
    u8  keyvalue;
 //测试菜单树
  s32 height,width;
  struct menu_item      *tempmenuitem;
  bool_t   result;
  struct GkWinRsc *desktop;
  struct GkWinRsc testwin;
  struct menu_item   *menutreenode;
  struct menu_displaypara  *operatingDispara;
  //设置鼠标焦点
//  HmiIn_SetFocusDefault(Djy_MyEvttId());     //todo:设置焦点的函数变了。

  //开始测试菜单项
  printf("begin to test the menuitem tree!\n");
  module_init_menu(0);

  printf("Init ---end\n");
  //建立菜单树
  menutreenode=create_menu_tree(NULL,g_tMenuitemModelTab);
  if(NULL==menutreenode)
  {
     printf("menuitem tree test end!----failed\n");
     return;
  }
  else
  {
      printf("menuitem tree test end!----success!\n");
   }

  printf("----This menu_display show time-------\n");
  //创建祖先窗口
  desktop = GK_ApiGetDesktop("sim_display");
  height=desktop->limit_bottom;
  width=desktop->limit_right;
  printf("Display:height=%d,width=%d\n",height,width);
  result=GK_ApiCreateGkwin(desktop, &testwin, 0, 0, width, height,\
                             0x00202060, CN_WINBUF_BUF, "menu window", 0,0,0,0);

  GK_ApiDrawText(&testwin,NULL,NULL,0,0,"Appwin",6,CN_COLOR_BLUE,0,0,CN_TIMEOUT_FOREVER);
  //开始测试我们的显示功能
  tempmenuitem=GetSonMenuitem(menutreenode);
  if(tempmenuitem==NULL)
  {
      printf("Failed to get the first menuitem!\n");
      return;
  }
  printf("---in main --1\n");
//DISMODE，0：下拉，1：九宫格
  operatingDispara=menuappinit_general(tempmenuitem,&testwin,DISMODE);
  printf("---in main --2\n");


    for ( ; ; )
    {
        //printf("---in main --3\n");
//        input_msg = (struct InputDeviceMsg *)Djy_GetEventPara(NULL,NULL);
        if(HmiIn_ReadDefaultMsg(&input_msg,20*mS))
        {
            //查看是否单点触摸屏
            if(input_msg->input_type == EN_HMIIN_SINGLE_TOUCH)
            {
                //除以放大倍数
                touch_x = input_msg->input_data.SingleTouchMsg.x/CN_LCD_X_SCALE;
                touch_y = input_msg->input_data.SingleTouchMsg.y/CN_LCD_Y_SCALE;

                mypoint.x=touch_x;
                mypoint.y=touch_y;

                operatingDispara = menumousetouch_respond(operatingDispara,&mypoint);
            }
            else if(input_msg->input_type == EN_HMIIN_KEYBOARD)//查看是否键盘
            {
                keyvalue= input_msg->input_data.key_board.key_value[0];
                //按键松开才确认，因此先判断是否断码,不是断码，表示按键按下
                if(input_msg->input_data.key_board.key_value[1] != CN_BREAK_CODE)
                {
                   //键盘响应测试
                    printf("key ---%d\n",keyvalue);
                   operatingDispara=menukeyboard_respond(operatingDispara,&keyvalue);//添加menu对键盘的相
                }
            }
//          Djy_ParaUsed(0);
        }
//      Djy_WaitEvttPop(Djy_MyEvttId(),NULL,20*mS);
    }
}



