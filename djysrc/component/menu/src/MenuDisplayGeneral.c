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
// 模块描述: DJY菜单模块之显示函数之通用显示方法的定义
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================
#include "MenuDisplayGeneral.h"
#include "MenuSpaceGeneral.h"
#include "MenuitemApi.h"

#include <math.h>//引用指数函数等，pow(a,b),a的b次方

//////////////////////////////////////////////////////////////////////////
////////////////////////////内部使用函数,不再做参数检查
//////////////////////////////////////////////////////////////////////////

 // =========================================================================
 // 函数功能:对菜单项进行画方框
 // 输入参数:进行画框的显示参数
 // 说明          :仅仅围绕着显示的内同画一个方框(垂直显示的时候)
 // =========================================================================
static bool_t DrawMenuLayerBox_V(struct menu_displaypara*  dispara)
 {
    bool_t result=false;
    u8   num_item=0;
    struct tagRectangle temp_rec={0,0,0,0};
    struct tagGkWinRsc  *win;

    win=dispara->CurWin;
    num_item=CalDisBettween2Menuitem(dispara->FVisualMenuitem, dispara->LVisualMenuitem)+1;
    temp_rec.left=cn_space_margin/4;
    temp_rec.top=cn_space_margin/4;
    temp_rec.bottom=(dispara->MenuitemSpace.height)*num_item+cn_space_margin/2;
    temp_rec.right=dispara->MenuitemSpace.width+cn_space_margin/2;

    GK_ApiLineto(win, temp_rec.left, temp_rec.top, temp_rec.right, temp_rec.top,\
                  CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
    GK_ApiLineto(win, temp_rec.left, temp_rec.bottom, temp_rec.right, temp_rec.bottom,\
                  CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
    GK_ApiLineto(win, temp_rec.left, temp_rec.top, temp_rec.left, temp_rec.bottom,\
                   CN_COLOR_BLACK, CN_R2_COPYPEN,0);
    GK_ApiLineto(win, temp_rec.right, temp_rec.top, temp_rec.right, temp_rec.bottom,\
                   CN_COLOR_BLACK, CN_R2_COPYPEN, CN_TIMEOUT_FOREVER);

     result=true;

    return result;
 }

  // =========================================================================
  // 函数功能:对菜单项进行画方框
  // 输入参数:进行画框的显示参数
  // 说明          :仅仅围绕着显示的内同画一个方框(水平显示的时候)
  // =========================================================================
 static bool_t DrawMenuLayerBox_H(struct menu_displaypara*  dispara)
  {
    bool_t result=false;
    u8   num_item=0;
    struct tagRectangle temp_rec={0,0,0,0};
    struct tagGkWinRsc  *win;

    win=dispara->CurWin;
    num_item=CalDisBettween2Menuitem(dispara->FVisualMenuitem, dispara->LVisualMenuitem)+1;
    temp_rec.left=cn_space_margin/4;
    temp_rec.top=cn_space_margin/4;
    temp_rec.bottom=dispara->MenuitemSpace.height+cn_space_margin/2;
    temp_rec.right=(dispara->MenuitemSpace.width)*num_item+cn_space_margin/2;
    GK_ApiLineto(win, temp_rec.left, temp_rec.top, temp_rec.right, temp_rec.top,\
                  CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
    GK_ApiLineto(win, temp_rec.left, temp_rec.bottom, temp_rec.right, temp_rec.bottom,\
                  CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
    GK_ApiLineto(win, temp_rec.left, temp_rec.top, temp_rec.left, temp_rec.bottom,\
                  CN_COLOR_BLACK, CN_R2_COPYPEN,0);
    GK_ApiLineto(win, temp_rec.right, temp_rec.top, temp_rec.right, temp_rec.bottom,\
                  CN_COLOR_BLACK, CN_R2_COPYPEN, CN_TIMEOUT_FOREVER);

    result=true;
    return result;
  }
  // =========================================================================
  // 函数功能:对菜单项进行画方框
  // 输入参数:进行画框的显示参数
  // 说明          :仅仅围绕着显示的内同画一个方框(水平显示的时候)
  // =========================================================================
static bool_t DrawMenuLayerBox_3by3(struct menu_displaypara*  dispara)
 {
       bool_t result=false;
       struct tagRectangle temp_rec={0,0,0,0};
       struct tagGkWinRsc  *win;

      win=dispara->CurWin;
      temp_rec.left=cn_space_margin/4;
      temp_rec.top=cn_space_margin/4;
      temp_rec.bottom=dispara->CurWin->bottom-dispara->CurWin->top-cn_space_margin/4;
      temp_rec.right=dispara->CurWin->right-dispara->CurWin->left-cn_space_margin/4;

      GK_ApiLineto(win, temp_rec.left, temp_rec.top, temp_rec.right, temp_rec.top,\
                    CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
      GK_ApiLineto(win, temp_rec.left, temp_rec.bottom, temp_rec.right, temp_rec.bottom,\
                      CN_COLOR_BLACK, CN_R2_COPYPEN, 0);
      GK_ApiLineto(win, temp_rec.left, temp_rec.top, temp_rec.left, temp_rec.bottom,\
                      CN_COLOR_BLACK, CN_R2_COPYPEN,0);
      GK_ApiLineto(win, temp_rec.right, temp_rec.top, temp_rec.right, temp_rec.bottom,\
                     CN_COLOR_BLACK, CN_R2_COPYPEN, CN_TIMEOUT_FOREVER);
      result=true;

    return result;
 }

// ===================================================================================================
// 函数功能:将黑白点阵转换为32位真彩色图
// 输入参数:src,点阵源，count，点阵源的字节数，data1：1对应的数值，data0，0对应的数值
// 输出参数：dest，转换后的数据
// 返回值  :
// 说明    :内部使用函数，将图像数据（BIT像素阵列）转换为真彩色像素阵列
// ==================================================================================================
static void changeB2I(u8* dest, u8* src, u32 count, u32 data1, u32 data0)
{
   u8 i=0,j=0;
   u8  data;
   for(i=0;i<count;i++)//字节数
   {
      data=*((u8 *)(src+i));
      for(j=0;j<8;j++)//自己位
      {
         if(data&(1<<(7-j)))
         {
           *((u32 *)(dest+i*32+j*4))=data1;
         }
         else
         {
             *((u32 *)(dest+i*32+j*4))=data0;
         }
      }
    }
}

// =========================================================================
// 函数功能：将文本按照当前使用的字体写到对应的窗口位置上
// 输入参数:para：写文本参数，里面包含窗口颜色等信息，text：文本字符串
// 输出参数：
// 返回值  :
// 说明    :内部使用函数
// =========================================================================
static void  draw_win_text(struct draw_wintext_para *para,const char *text)//确认
{

    s32 len, src_len,size,size_bak;
    u32 wc;
    s32 colortext; //字体颜色
    s32 colorback;  //背景颜色  int k=0;
    s32  colorsize=32*16;
    struct tagFontRsc* cur_font;
    u8       *colorbuf;//存储整条字符串的容量
    struct tagGkscParaDrawBitmapRop bitmap_para;
    struct tagGkscParaDrawBitmapRop mpara;
    u8 buf[128],*dbuf;  //定义一个足够存32点阵汉字的缓冲区，如果需要显示的字符
                        //超过此点阵，就需要动态分配

    colortext=para->colortext;
    colorback=para->colorback;
    cur_font = Font_GetCurFont();
    if(cur_font == NULL)
        return;

    // 计算字节数
    src_len = para->count;
    bitmap_para.x = para->x;
    bitmap_para.gkwin = para->gkwin;
    bitmap_para.y = para->y;
    bitmap_para.bitmap.bm_bits = NULL;
    dbuf = NULL;
    size_bak = 0;

    colorbuf=M_MallocLc(32*32,0);
    memset(colorbuf,0,colorsize);

    for(; src_len > 0;)
    {
        len= mbtowc(&wc, text, src_len);
        if(len == -1)
        { // 无效字符
            src_len--;
            text++;
        }else
        { // 有效字符
            text += len;
            src_len -= len;
            // 取字形
            bitmap_para.bitmap.bm_bits = NULL;      //NULL=查询bitmap.
            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));
            size = bitmap_para.bitmap.linebytes * bitmap_para.bitmap.height;

            if((size*32)>colorsize)
            {
              free(colorbuf);
              colorsize=size*32;
              colorbuf=M_MallocLc(colorsize,0);
              memset(colorbuf,0,colorsize);
              if(colorbuf==NULL)
                return;
            }
            if(size < 128)
            {
                bitmap_para.bitmap.bm_bits = buf;
            }
            else
            {
                if(size > size_bak)
                {
                    free(dbuf);
                    dbuf = M_MallocLc(size,0);
                    if(dbuf == NULL)
                        return;
                    size_bak = size;
                }
                bitmap_para.bitmap.bm_bits = dbuf;
            }
            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));


            changeB2I(colorbuf, bitmap_para.bitmap.bm_bits,\
                      size, colortext, colorback);

             mpara.gkwin=bitmap_para.gkwin;
             mpara.x=bitmap_para.x;
             mpara.y=bitmap_para.y;
             mpara.bitmap.height=bitmap_para.bitmap.height;
             mpara.bitmap.width=bitmap_para.bitmap.width;
             mpara.bitmap.linebytes=32*bitmap_para.bitmap.linebytes;
             mpara.bitmap.PixelFormat=CN_SYS_PF_ERGB8888;
             mpara.bitmap.bm_bits=colorbuf;
             //每次题图都要立即完成，不然会出现内存被重写，导致显示紊乱
             GK_ApiDrawBitMap(mpara.gkwin, &(mpara.bitmap), mpara.x, mpara.y,0,0, CN_TIMEOUT_FOREVER);
             //考虑到要释放内存，所以必须等待完成以后再返回，即存在线程切换。
              bitmap_para.x += bitmap_para.bitmap.width;
        }
    }

    free(colorbuf);
    free(dbuf);
}
// =========================================================================
// 函数功能：将菜单项的节点名字写在对应的窗口的对应位置上
// 输入参数:dispara：显示参数，menu：要写的菜单项
// 输出参数：
// 返回值  :
// 说明    :添加了相关的标识符："+"表示有子菜单，"."表示没有子菜单
// =========================================================================
static bool_t DrawMenuitemText(struct menu_displaypara  *dispara,struct menu_item  *menu,s32 x,s32 y)
{

   bool_t result=true;
   char  *apendSon="+";//有子菜单
   char  *apendNull=".";//无子菜单
   char content[50];//为了不至于修改菜单内容
   struct draw_wintext_para  para;

   content[0]='\0';
   if(NULL!=menu->node.child)//有子菜单，添加标志
   {
      strcat(content,apendSon);
      strcat(content,menu->node.name);
   }
   else
   {
      strcat(content,apendNull);
      strcat(content,menu->node.name);
   }
    //填充para结构体
   if(menu==dispara->OperatingMenuitem)
   {
       para.colorback=dispara->DisplayColor.menu_lighted_color;
   }
   else
   {
      para.colorback=dispara->DisplayColor.menu_color;
   }
   para.colortext=dispara->DisplayColor.text_color;
   para.gkwin=dispara->CurWin;
   para.count=strnlen(content,namesize);
   para.line_limit=namesize;
   para.x=x;
   para.y=y;
   para.draw_type=Left2Right;
    //完成菜单的显示
   draw_win_text(&para,content);

   return result;
}


////////////////////////////////////////////////////
//////////////基本显示模块函数
////////////////////////////////////////////////////
// =========================================================================
// 函数功能：水平显示函数
// 输入参数:dispara：待显示的显示参数
// 输出参数：
// 返回值    :
// 说明          :将dispra中的菜单项以同样大小的水平显示到对应的窗口上，显示的时候是从
//         dispara的Fvisual到Lvisual。使用者调用其他的函数负责调整可视的第一个菜单
//         和最后一个菜单
// =========================================================================
bool_t display_menu_horizontal(struct menu_displaypara * dispara)//调用写菜单内容函数和画方框函数完成菜单的显示
{
      bool_t  result=false;
      s32     x,y;                //存储写字的位置
      struct menu_item   *temp;//临时menuitem变量
      struct tagRectangle    LightedRec;

      if(NULL==dispara->FVisualMenuitem)
      {
          dispara->FVisualMenuitem=dispara->OperatingMenuitem;
      }
      if(NULL==dispara->LVisualMenuitem)
      {
          dispara->LVisualMenuitem=dispara->OperatingMenuitem;
      }
      temp=dispara->FVisualMenuitem;
      x=cn_space_margin/2;
      y=cn_space_margin/2;
          //首先要将整个窗口颜色复原,不应该在这里做，不然很浪费资源的，每次都要刷屏，太残忍了！
    //  GK_ApiFillWin(dispara->CurWin, dispara->DisplayColor.menu_color, CN_TIMEOUT_FOREVER);
      do{
          //个人自扫门前雪，每个menuitem的写入都要清除一下
          LightedRec.left=x,
          LightedRec.top=y;
          LightedRec.right=x+dispara->MenuitemSpace.width;
          LightedRec.bottom=y+dispara->MenuitemSpace.height;
          if(temp==dispara->OperatingMenuitem)//加亮当前操作项
          {
              //这个时候不需要切换事件，下面的写字函数会阻塞事件，直到写完
              GK_ApiFillRect(dispara->CurWin, &LightedRec,
                           dispara->DisplayColor.menu_lighted_color,
                           0,CN_FILLRECT_MODE_N,0);
          }
          else
          {
              GK_ApiFillRect(dispara->CurWin, &LightedRec,
                             dispara->DisplayColor.menu_color,
                             0,CN_FILLRECT_MODE_N,0);
          }

          DrawMenuitemText(dispara, temp,x,y);

          x=x+dispara->MenuitemSpace.width;//水平显示只需要更新x方向。
          temp=GetNextMenuitem(temp);
      }while(temp!=GetNextMenuitem(dispara->LVisualMenuitem));

      //draw the layers box
      DrawMenuLayerBox_H(dispara);
      result=true;

      return result;
}


// =========================================================================
// 函数功能：垂直显示函数
// 输入参数:dispara：待显示的显示参数
// 输出参数：
// 返回值  :
// 说明    :将dispra中的菜单项以同样大小的垂直显示到对应的窗口上，显示的时候是从
//         dispara的Fvisual到Lvisual。使用者调用其他的函数负责调整可视的第一个菜单
//         和最后一个菜单
// =========================================================================
//垂直显示同层菜单
bool_t display_menu_vertical(struct menu_displaypara * dispara)//调用写菜单内容函数和画方框函数完成菜单的显示
{
      bool_t  result=false;
      s32     x,y;                //存储写字的位置
      struct menu_item   *temp;//临时menuitem变量
      struct tagRectangle    LightedRec;

      if(NULL==dispara->FVisualMenuitem)
      {
          dispara->FVisualMenuitem=dispara->OperatingMenuitem;
      }
      if(NULL==dispara->LVisualMenuitem)
      {
          dispara->LVisualMenuitem=dispara->OperatingMenuitem;
      }
      //Draw the menuitem text
      temp=dispara->FVisualMenuitem;
      x=cn_space_margin/2;
      y=cn_space_margin/2;
    //首先要将整个窗口颜色复原,不应该在这里做，不然很浪费资源的，每次都要刷屏，太残忍了！
    //  GK_ApiFillWin(dispara->CurWin, dispara->DisplayColor.menu_color, CN_TIMEOUT_FOREVER);
      do{
          LightedRec.left=x,
          LightedRec.top=y;
          LightedRec.right=x+dispara->MenuitemSpace.width;
          LightedRec.bottom=y+dispara->MenuitemSpace.height;
          if(temp==dispara->OperatingMenuitem)//加亮当前操作项
          {
              //这个时候不需要切换事件，下面的写字函数会阻塞事件，直到写完
              GK_ApiFillRect(dispara->CurWin, &LightedRec,
                             dispara->DisplayColor.menu_lighted_color,
                             0,CN_FILLRECT_MODE_N,0);
          }
          else
          {
              GK_ApiFillRect(dispara->CurWin, &LightedRec,
                             dispara->DisplayColor.menu_color,
                             0,CN_FILLRECT_MODE_N,0);
          }
          DrawMenuitemText(dispara, temp,x,y);

          y=y+dispara->MenuitemSpace.height;//垂直显示只需要更新y方向。
          temp=GetNextMenuitem(temp);

      }while(temp!=GetNextMenuitem(dispara->LVisualMenuitem));
        //draw the layers box
      DrawMenuLayerBox_V(dispara);
      result=true;

  return result;
}

//基础显示模式函数
 // =========================================================================
 // 函数功能：九格显示函数
 // 输入参数:dispara：待显示的显示参数
 // 输出参数：
 // 返回值    :
 // 说明          :将dispra中的菜单项以同样大小的分三排显示到对应的窗口上的中心位置，显示的时候是从
 //         dispara的Fvisual到Lvisual。使用者调用其他的函数负责调整可视的第一个菜单
 //         和最后一个菜单
 // =========================================================================

//从左到右、从上到下的完成9格显示
bool_t display_menu_3by3(struct menu_displaypara * dispara)//调用写菜单内容函数和画方框函数完成菜单的显示
{
  bool_t  result=false;
  u8      k=0;                //用于标记层次
  s32     x,y;                //存储写字的位
  s32     x0,y0;              //记录开始显示的原点
  struct menu_item   *temp;//临时menuitem变量
  struct tagRectangle    LightedRec;//加亮的区域


  if(NULL==dispara->FVisualMenuitem)
  {
     dispara->FVisualMenuitem=dispara->OperatingMenuitem;
  }
  if(NULL==dispara->LVisualMenuitem)
  {
      dispara->LVisualMenuitem=dispara->OperatingMenuitem;
  }
  temp=dispara->FVisualMenuitem;

  //首先要将整个窗口颜色复原
  GK_ApiFillWin(dispara->CurWin, dispara->DisplayColor.menu_color, CN_TIMEOUT_FOREVER);

  x0=(dispara->CurWin->right-dispara->CurWin->left-cn_space_margin)/2-(dispara->MenuitemSpace.width*3)/2;
  y0=(dispara->CurWin->bottom-dispara->CurWin->top-cn_space_margin)/2-(dispara->MenuitemSpace.height*3)/2;
  x=x0;
  y=y0;
  do{
      LightedRec.left=x;
      LightedRec.top=y;
      LightedRec.right=x+dispara->MenuitemSpace.width;
      LightedRec.bottom=y+dispara->MenuitemSpace.height;
      if(temp==dispara->OperatingMenuitem)//加亮当前操作项
      {
          //这个时候不需要切换事件，下面的写字函数会阻塞事件，直到写完
          GK_ApiFillRect(dispara->CurWin, &LightedRec,
                         dispara->DisplayColor.menu_lighted_color,
                         0,CN_FILLRECT_MODE_N,0);
      }
      else
      {
          GK_ApiFillRect(dispara->CurWin, &LightedRec,
                         dispara->DisplayColor.menu_color,
                         0,CN_FILLRECT_MODE_N,0);
      }
      result= DrawMenuitemText(dispara, temp,x,y);
      k++;
      if(9==k)//显示完九格
      {
        break;//跳出循环，后面的不再显示
      }
      if(0==(k%3))//换行
      {
        y=y+dispara->MenuitemSpace.height+cn_space_margin/2;//
        x=x0;
      }
      else
      {
         x=x+dispara->MenuitemSpace.width+cn_space_margin/2;//
      }
      temp=GetNextMenuitem(temp);
  }while(temp!=GetNextMenuitem(dispara->LVisualMenuitem));
  //仅仅要显示9个，过多不候，虽然也是从FV开始到LV结束
  //draw the layers box
  DrawMenuLayerBox_3by3(dispara);
  result=true;

  return result;
}
// =========================================================================
// 函数功能:执行显示参数的钩子函数
// 输入参数:dispara：当前操作的菜单项的菜单显示参数
// 输出参数：
// 返回值    :
// 说明          :执行菜单显示参数中的当前操作的菜单项的钩子函数
// =========================================================================
//执行菜单回调函数
struct menu_displaypara *exe_menuitemfunc(struct menu_displaypara *operatingDispara)
{
 //  u8      dismode=0;//显示模式
   struct menu_item  *OperatingMenuitem=NULL;
   struct menu_displaypara  *pMainmenuDispara=NULL;
   struct menu_item  *FirstMenuitem=NULL;//整个菜单的第一个菜单项，用于恢复时用
   struct tagGkWinRsc  *Appwin=NULL;

   ptu32_t (*menu_init_function)(struct menu_item *FirstMenuitem, struct tagGkWinRsc *Appwin);

   if(NULL==operatingDispara)
   {
      printf("Invalid parameter-----exe_menuitemfunc\n");
      return operatingDispara;
   }
   OperatingMenuitem=operatingDispara->OperatingMenuitem;
   OperatingMenuitem->myprivate=operatingDispara->AppWin;

   if((NULL==OperatingMenuitem)||(NULL==OperatingMenuitem->MenuitemFunction))
   {
      printf("fatal error!--exe_menuitemfunc!\n");
      return operatingDispara;
   }

   operatingDispara=getmainmenudispara(operatingDispara);
   //保存相关的记录，用于恢复
   FirstMenuitem=GetBBmenuitem(operatingDispara->OperatingMenuitem);
   Appwin=operatingDispara->AppWin;
   menu_init_function=operatingDispara->MyMenuInitFunction;
  // dismode=operatingDispara->dismode;

   //删除整个菜单，执行钩子函数
   deldisplaypara(operatingDispara);
   operatingDispara=NULL;
   OperatingMenuitem->MenuitemFunction(OperatingMenuitem);//执行钩子函数

   //恢复主菜单
   operatingDispara=menu_init_function(FirstMenuitem,Appwin);
   printf("recover main:mem=%08x  size=%08x\n",operatingDispara,sizeof(struct menu_displaypara));
   if(NULL==operatingDispara)
   {
       printf("Failed to recover the main menu!----exe_menuitemfunc\n");
   }
   else
   {
      if(NULL==operatingDispara->MyMenuDispalyFunction)
      {
          printf("Failed to recover the main menu!----exe_menuitemfunc\n");
      }
      else
      {
         // operatingDispara->MyMenuDispalyFunction(operatingDispara);
      }
   }

   return operatingDispara;

}
// =========================================================================
// 函数功能：考虑的有多重显示方向的文本显示函数
// 输入参数:
// 输出参数：
// 返回值 :
// 说明   :处在试用版本。
// =========================================================================
void  my_draw_text(struct draw_wintext_para *para,const char *text)//确认
{
    struct tagGkscParaDrawBitmapRop bitmap_para;
    s32 len, src_len,size,size_bak;
    u32 wc;
    struct tagFontRsc* cur_font;
    u8 buf[128],*dbuf;  //定义一个足够存32点阵汉字的缓冲区，如果需要显示的字符
                        //超过此点阵，就需要动态分配
//修改的部分
    u8       *colorbuf;//存储整条字符串的容量
    struct tagGkscParaDrawBitmapRop mpara;
    s32 colortext; //字体颜色
    s32 colorback;  //背景颜色
    s32 k=0;
    s32  colorsize=32*16;//存储的是32位真彩色图像
    u32  number=0;

    colortext=para->colortext;
    colorback=para->colorback;

    cur_font = Font_GetCurFont();
    if(cur_font == NULL)
        return;

    // 计算字节数
    src_len = para->count;

    bitmap_para.x = para->x;
    bitmap_para.gkwin = para->gkwin;
    bitmap_para.y = para->y;
    bitmap_para.bitmap.bm_bits = NULL;
    dbuf = NULL;
    size_bak = 0;

    colorbuf=M_MallocLc(colorsize,0);
    memset(colorbuf,0,colorsize);


    for(; src_len > 0;)
    {
       // printf("------------src_len=%d-------------\n",src_len);
       number++;
        len= mbtowc(&wc, text, src_len);
        if(len == -1)
        { // 无效字符
            src_len--;
            text++;
        }else
        { // 有效字符
            text += len;
            src_len -= len;

            // 取字形
            bitmap_para.bitmap.bm_bits = NULL;      //NULL=查询bitmap.
            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));
            size = bitmap_para.bitmap.linebytes * bitmap_para.bitmap.height;

           if((size*32)>colorsize)
            {
              free(colorbuf);
              colorsize=size*32;
              colorbuf=M_MallocLc(colorsize,0);

              if(colorbuf==NULL)
                return;
              memset(colorbuf,0,colorsize);
           }

            if(size < 128)
            {
                bitmap_para.bitmap.bm_bits = buf;
            }
            else
            {
                if(size > size_bak)
                {
                    free(dbuf);
                    dbuf = M_MallocLc(size,0);
                    if(dbuf == NULL)
                        return;
                    size_bak = size;
                }
                bitmap_para.bitmap.bm_bits = dbuf;
            }
            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));
            changeB2I(colorbuf, bitmap_para.bitmap.bm_bits,\
                      size, colortext, colorback);
            mpara.gkwin=bitmap_para.gkwin;
            //看看是不是要换行
            if((number%44)==0)
            {
              bitmap_para.x=para->x;
              bitmap_para.y=bitmap_para.y+16;
              printf("x=%d  y=%d",bitmap_para.x,bitmap_para.y);
              printf("换行\n");
            }
             //由于写字的方式不同
            switch(para->draw_type)
            {
             case 1://从左到右
                   mpara.x=bitmap_para.x;
                   mpara.y=bitmap_para.y;
                   bitmap_para.x += bitmap_para.bitmap.width;
                   break;
             case 2://从右向左
                   mpara.x=bitmap_para.x-bitmap_para.bitmap.width;
                   mpara.y=bitmap_para.y;
                   bitmap_para.x -= bitmap_para.bitmap.width;
                   break;
              default://从左到右
                   mpara.x=bitmap_para.x;
                   mpara.y=bitmap_para.y;
                   bitmap_para.x += bitmap_para.bitmap.width;
                   break;
             }
             mpara.bitmap.height=bitmap_para.bitmap.height;
             mpara.bitmap.width=bitmap_para.bitmap.width;
             mpara.bitmap.linebytes=32*bitmap_para.bitmap.linebytes;
             mpara.bitmap.PixelFormat=CN_SYS_PF_ERGB8888;
             mpara.bitmap.bm_bits=colorbuf;
             //考虑到要释放内存，所以必须等待完成以后再返回，即存在线程切换。
             GK_ApiDrawBitMap(mpara.gkwin, &(mpara.bitmap), mpara.x, mpara.y,0,0, CN_TIMEOUT_FOREVER);
        }
    }
    free(colorbuf);//
    free(dbuf);
    return;
 //   printf("Gk_api_draw_mtext end!\n");
}
////////////////////////////////////////////////////////////////////////////////////////
////////实现基本的显示动作函数（展开、切换菜单项、返回）
////////////////////////////////////////////////////////////////////////////////////////
// =========================================================================
// 函数功能:下拉显示方法之显示子菜单
// 输入参数:operatingDispara，当前正在操作的显示参数
// 输出参数：
// 返回值     :子菜单的显示参数
// 说明          :对于返回值，用户注意保存、检查、和更新，如果没有子菜单，则会没有反应
// =========================================================================
struct menu_displaypara *goto_submenu(struct menu_displaypara *operatingDispara)
{
    struct menu_displaypara   *temp;

    //参数检查
    if((NULL==operatingDispara)||(NULL==operatingDispara->OperatingMenuitem)||(NULL==operatingDispara->CurWin))
    {
         printf("Invalid parameter----goto_submenu\n");
         return operatingDispara;
    }
    if(NULL==GetSonMenuitem(operatingDispara->OperatingMenuitem))
    {
      printf("No sub menu ---goto_submenu\n");
      return operatingDispara;
    }
    temp=operatingDispara->CreateMenuDisparaBypre(operatingDispara, NULL, NULL);
    if(NULL==temp)
    {
      printf("Failed --goto_submenu\n");
      return operatingDispara;
    }
    //参数调整
    temp->AdjustVisualMenuitem(temp);
    //显示
    GK_ApiFillWin(temp->CurWin, operatingDispara->DisplayColor.menu_color, 0);
    temp->MyMenuDispalyFunction(temp);

    return temp;
}
// =========================================================================
// 函数功能:下拉显示方法之返回到父菜单
// 输入参数:operatingDispara，当前正在操作的显示参数
// 输出参数：
// 返回值     :父菜单的显示参数
// 说明          :对于返回值，用户注意保存、检查、和更新；该操作会删除当前及其后面的显示参数
// =========================================================================
struct menu_displaypara *goto_parmenu(struct menu_displaypara *operatingDispara)
{
    struct menu_displaypara *temp;
    struct menu_displaypara *pMainmenuDispara;

    //参数检查
    if(NULL==operatingDispara)
    {
         printf("Invalid parameter--- goto_parmenu!\n");
         return operatingDispara;
    }
    if(NULL==operatingDispara->pre)//主菜单，不再返回
    {
        printf("MainMenu----goto_parmenu\n");
        return operatingDispara;
    }
    else//子菜单
    {
        temp=operatingDispara;
        operatingDispara=operatingDispara->pre;
        operatingDispara->next=NULL;
//        GK_ApiFillWin(temp->CurWin, temp->DisplayColor.menu_color, CN_TIMEOUT_FOREVER);
        deldisplaypara(temp);
//因为子菜单可能挡住了很多父菜单，所以要依次回复所有的菜单，尽管目前多余，但是后续开发很重要--TODO
//因为是恢复菜单，所以此时是没有必要调整菜单项的。
        pMainmenuDispara=getmainmenudispara(operatingDispara);
        temp=pMainmenuDispara;
        while(temp!=operatingDispara)
        {
            GK_ApiFillWin(temp->CurWin, operatingDispara->DisplayColor.menu_color, 0);
            temp->MyMenuDispalyFunction(temp);
            temp=temp->next;
        }
        GK_ApiFillWin(temp->CurWin, operatingDispara->DisplayColor.menu_color, 0);
        temp->MyMenuDispalyFunction(temp);
         return operatingDispara;
    }
}

// =========================================================================
// 函数功能:下拉显示方法之切换到下一个菜单项（next）
// 输入参数:operatingDispara，当前正在操作的显示参数
// 输出参数：
// 返回值    :
// 说明         :
// =========================================================================
void goto_nextmenuitem(struct menu_displaypara  *operatingDispara)
{
    if((NULL==operatingDispara)||(NULL==operatingDispara->OperatingMenuitem)||(NULL==operatingDispara->CurWin))
    {
         printf("Invalid parameter---GotoNextmeuitem!\n");
         return;
    }
    operatingDispara->OperatingMenuitem=GetNextMenuitem(operatingDispara->OperatingMenuitem);
//    GK_ApiFillWin(operatingDispara->CurWin, operatingDispara->DisplayColor.menu_color, 0);
    operatingDispara->AdjustVisualMenuitem(operatingDispara);
    operatingDispara->MyMenuDispalyFunction(operatingDispara);
}

// =========================================================================
// 函数功能:下拉显示方法之切换到上一个菜单项（pre）
// 输入参数:operatingDispara，当前正在操作的显示参数
// 输出参数：
// 返回值  :
// 说明    :
// =========================================================================
void goto_premenuitem(struct menu_displaypara  *operatingDispara)
{
    if((NULL==operatingDispara)||(NULL==operatingDispara->OperatingMenuitem)||(NULL==operatingDispara->CurWin))
    {
         printf("Invalid parameter---goto_premenuitem!\n");
         return;
    }
    operatingDispara->OperatingMenuitem=GetPreMenuitem(operatingDispara->OperatingMenuitem);
//   GK_ApiFillWin(operatingDispara->CurWin, operatingDispara->DisplayColor.menu_color, 0);
    operatingDispara->AdjustVisualMenuitem(operatingDispara);
    operatingDispara->MyMenuDispalyFunction(operatingDispara);
}
// =========================================================================
// 函数功能:下拉显示方法之在同层菜单项中以步长step进行切换
// 输入参数:operatingDispara，当前正在操作的显示参数
// 输出参数：
// 返回值    :
// 说明         :step为步长，负值代表想后（next）的步长切换），为正值代表向前（pre）的步长切换
//         在同级菜单项中切换，步长可指定，采用循环求余的方法
// =========================================================================

void goto_menuitem_inslayers(struct menu_displaypara *operatingDispara, s8 step)
{
     s8     MenuitemNo;
     struct menu_item   *pDestMenuitem;
     if((NULL==operatingDispara)||(NULL==operatingDispara->OperatingMenuitem)||(NULL==operatingDispara->CurWin))
     {
       printf("Invalid parameter---goto_menuitem_inslayers!\n");
       return;
     }
     if(0==step)//正是当前菜单项，不需要再重绘的
     {
       return;
     }
     MenuitemNo=CalMenuitemNumber(operatingDispara->OperatingMenuitem);
     MenuitemNo+=step;

     pDestMenuitem=GetMenuitemBynumber(operatingDispara->OperatingMenuitem,MenuitemNo);
     if(NULL==pDestMenuitem)
     {
       printf("Can not found the dest menuitem ---goto_menuitem_inslayers\n");
       return;
     }
    //重绘当前菜单窗口
    operatingDispara->OperatingMenuitem=pDestMenuitem;
//    GK_ApiFillWin(operatingDispara->CurWin,operatingDispara->DisplayColor.menu_color, 0);
    operatingDispara->AdjustVisualMenuitem(operatingDispara);
    operatingDispara->MyMenuDispalyFunction(operatingDispara);
    return;
}

// =========================================================================
// 函数功能：将文本按照当前使用的字体写到对应的窗口位置上
// 输入参数:para：写文本参数，里面包含窗口颜色等信息，text：文本字符串
// 输出参数：
// 返回值  :
// 说明    :内部使用函数
// =========================================================================
 void  windows_draw_text(struct draw_wintext_para *para,const char *text)//确认
{

    s32 len, src_len,size,size_bak;
    u32 wc;
    s32 colortext; //字体颜色
    s32 colorback;  //背景颜色  int k=0;
    s32  colorsize=32*16;
    struct tagFontRsc* cur_font;
    u8       *colorbuf;//存储整条字符串的容量
    struct tagGkscParaDrawBitmapRop bitmap_para;
    struct tagGkscParaDrawBitmapRop mpara;
    u8 buf[128],*dbuf;  //定义一个足够存32点阵汉字的缓冲区，如果需要显示的字符
                        //超过此点阵，就需要动态分配

    colortext=para->colortext;
    colorback=para->colorback;
    cur_font = Font_GetCurFont();
    if(cur_font == NULL)
        return;

    // 计算字节数
    src_len = para->count;
    bitmap_para.x = para->x;
    bitmap_para.gkwin = para->gkwin;
    bitmap_para.y = para->y;
    bitmap_para.bitmap.bm_bits = NULL;
    dbuf = NULL;
    size_bak = 0;

    colorbuf=M_MallocLc(32*32,0);
    memset(colorbuf,0,colorsize);

    for(; src_len > 0;)
    {
        if('/'==*text)
        {
          printf("换行！---windows_draw_text\n");
          bitmap_para.x = para->x;
          bitmap_para.y =bitmap_para.y+16;
          src_len--;
          text++;
          continue;
        }
        len= mbtowc(&wc, text, src_len);
        if(len == -1)
        { // 无效字符
            src_len--;
            text++;
        }else
        { // 有效字符
            text += len;
            src_len -= len;
            // 取字形
            bitmap_para.bitmap.bm_bits = NULL;      //NULL=查询bitmap.
            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));
            size = bitmap_para.bitmap.linebytes * bitmap_para.bitmap.height;

            if((size*32)>colorsize)
            {
              free(colorbuf);
              colorsize=size*32;
              colorbuf=M_MallocLc(colorsize,0);
              memset(colorbuf,0,colorsize);
              if(colorbuf==NULL)
                return;
            }
            if(size < 128)
            {
                bitmap_para.bitmap.bm_bits = buf;
            }
            else
            {
                if(size > size_bak)
                {
                    free(dbuf);
                    dbuf = M_MallocLc(size,0);
                    if(dbuf == NULL)
                        return;
                    size_bak = size;
                }
                bitmap_para.bitmap.bm_bits = dbuf;
            }
            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));


            changeB2I(colorbuf, bitmap_para.bitmap.bm_bits,\
                      size, colortext, colorback);

             mpara.gkwin=bitmap_para.gkwin;
             mpara.x=bitmap_para.x;
             mpara.y=bitmap_para.y;
             mpara.bitmap.height=bitmap_para.bitmap.height;
             mpara.bitmap.width=bitmap_para.bitmap.width;
             mpara.bitmap.linebytes=32*bitmap_para.bitmap.linebytes;
             mpara.bitmap.PixelFormat=CN_SYS_PF_ERGB8888;
             mpara.bitmap.bm_bits=colorbuf;
             //每次题图都要立即完成，不然会出现内存被重写，导致显示紊乱
             printf("x=%d  y=%d\n",mpara.x,mpara.y);
             GK_ApiDrawBitMap(mpara.gkwin, &(mpara.bitmap), mpara.x, mpara.y,0,0, CN_TIMEOUT_FOREVER);
             //考虑到要释放内存，所以必须等待完成以后再返回，即存在线程切换。
              bitmap_para.x += bitmap_para.bitmap.width;
        }
    }

    free(colorbuf);
    free(dbuf);
}

//// =========================================================================
//// 函数功能：将文本显示在特定的矩形区域，超过的部分将不会被显示，
//// 输入参数: para：写文本参数，里面包含窗口颜色等信息，text：文本字符串
//// 输出参数：
//// 返回值  : 显示的结尾字符，当然有可能没有显示完，所以不一定是字符串的结尾。
//// 说明    : 宽度不够不进行显示，高度不够会尽量显示一部分。
//// =========================================================================
// char*  draw_text_in_rec(struct draw_wintext_para *para,const char *text)
//{
//    s32 len, src_len,size,size_bak;
//    u32 wc;
//  s32 width_limit,height_limit;//限制的长度和高度，相对于起始点
//  s32 width_draw,height_draw;//当前显示的长度和高度，相对于起始点，
//  s32 height_line=0;//当前行的高度，以最高的一个字符算
//  s32 colortext; //字体颜色
//  s32 colorback;  //背景颜色
//  s32  colorsize=32*128;
//    struct tagFontRsc* cur_font;
//    u8       *colorbuf;//存储整条字符串的容量
//    struct tagGkscParaDrawBitmap bitmap_para;
//    struct tagGkscParaDrawBitmap mpara;
//    u8 buf[128],*dbuf;  //定义一个足够存32点阵汉字的缓冲区，如果需要显示的字符
//                        //超过此点阵，就需要动态分配
//
//  //参数合法性检查
//  if((NULL==para)||(NULL==para->gkwin)||(NULL==text))
//  {
//      printf("Invalid parameter----draw_text_in_rec\n");
//      return text;
//  }
//
//  colortext=para->colortext;
//  colorback=para->colorback;
//    cur_font = Font_GetCurFont();
//  if(cur_font == NULL)
//      return;
//
//    // 计算字节数
//    src_len = para->count;
//
//    bitmap_para.bitmap.bm_bits = NULL;
//    dbuf = NULL;
//    size_bak = 0;
//
//    colorbuf=M_MallocLc(colorsize,0);
//    memset(colorbuf,0,colorsize);
//
//    for(; src_len > 0;)
//    {
//      if('/'==*text)
//      {
//        printf("换行！---windows_draw_text\n");
//        width_draw =0;
//        height_draw+=height_line;
//        src_len--;
//        free(colorbuf);
//            colorsize=size*32;
//            colorbuf=M_MallocLc(colorsize,0);
//            memset(colorbuf,0,colorsize);
//            if(colorbuf==NULL)
//              goto exit;
//        continue;
//      }
//        len= mbtowc(&wc, text, src_len);
//        if(len == -1)
//        { // 无效字符
//            src_len--;
//            text++;
//        }else
//        { // 有效字符
//            text += len;
//            src_len -= len;
//            // 取字形
//            bitmap_para.bitmap.bm_bits = NULL;      //NULL=查询bitmap.
//            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));
//            size = bitmap_para.bitmap.linebytes * bitmap_para.bitmap.height;
//
//            if((size*32)>colorsize)
//              {
//              free(colorbuf);
//            colorsize=size*32;
//            colorbuf=M_MallocLc(colorsize,0);
//            memset(colorbuf,0,colorsize);
//            if(colorbuf==NULL)
//              goto exit;
//          }
//            if(size < 128)
//            {
//                bitmap_para.bitmap.bm_bits = buf;
//          }
//            else
//            {
//                if(size > size_bak)
//                {
//                    free(dbuf);
//                    dbuf = M_MallocLc(size,0);
//                    if(dbuf == NULL)
//                        goto exit;
//                  free(colorbuf);
//                  colorsize=size*32;
//                  colorbuf=M_MallocLc(colorsize,0);
//                  memset(colorbuf,0,colorsize);
//                  if(colorbuf==NULL)
//                     goto exit;
//                    size_bak = size;
//                }
//                bitmap_para.bitmap.bm_bits = dbuf;
//            }
//            cur_font->GetBitmap(wc,0,0,&(bitmap_para.bitmap));
//
//          if((width_draw+bitmap_para.bitmap.width)>width_limit)//行要出边界，不好，还是不画了吧
//          {
//               width_draw+=bitmap_para.bitmap.width;
//
//               continue;//我们相信会有换行的
//          }
//          else if(height_draw>=height_limit)//高度已经超标了，或许我们不应再画了，
//          {
//              goto exit;
//          }
//          else if((height_draw+bitmap_para.bitmap.height)>height_limit)//高度即将超标，但是还没有超标，就画那么一点点就可以了
//          {
//              bitmap_para.bitmap.height=height_limit-height_draw;//人为的修改高度
//              size = bitmap_para.bitmap.linebytes * bitmap_para.bitmap.height;
//
//              changeB2I(colorbuf, bitmap_para.bitmap.bm_bits,size, colortext, colorback);
//              mpara.gkwin=para->gkwin;
//              mpara.x=para->x+width_draw;
//              mpara.y=para->y+height_draw;
//              mpara.bitmap.height=bitmap_para.bitmap.height;
//              mpara.bitmap.width=bitmap_para.bitmap.width;
//              mpara.bitmap.linebytes=32*bitmap_para.bitmap.linebytes;
//              mpara.bitmap.pf_type=CN_SYS_PF_E8R8G8B8;
//              mpara.bitmap.bm_bits=colorbuf;
//           //每次题图都要立即完成，不然会出现内存被重写，导致显示紊乱
//              printf("x=%d  y=%d\n",mpara.x,mpara.y);
//              GK_ApiDrawBitMap(mpara.gkwin, &(mpara.bitmap), mpara.x, mpara.y, CN_TIMEOUT_FOREVER);
//              //增加显示的宽度和高度
//              width_draw+=bitmap_para.bitmap.width;
//              height_draw+=bitmap_para.bitmap.height;
//             //考虑到要释放内存，所以必须等待完成以后再返回，即存在线程切换。
//          }
//          else//很正常的情况，在矩形框内显示
//          {
//              changeB2I(colorbuf, bitmap_para.bitmap.bm_bits,size, colortext, colorback);
//              mpara.gkwin=para->gkwin;
//              mpara.x=para->x+width_draw;
//              mpara.y=para->y+height_draw;
//              mpara.bitmap.height=bitmap_para.bitmap.height;
//              mpara.bitmap.width=bitmap_para.bitmap.width;
//              mpara.bitmap.linebytes=32*bitmap_para.bitmap.linebytes;
//              mpara.bitmap.pf_type=CN_SYS_PF_E8R8G8B8;
//              mpara.bitmap.bm_bits=colorbuf;
//           //每次题图都要立即完成，不然会出现内存被重写，导致显示紊乱
//              printf("x=%d  y=%d\n",mpara.x,mpara.y);
//              GK_ApiDrawBitMap(mpara.gkwin, &(mpara.bitmap), mpara.x, mpara.y, CN_TIMEOUT_FOREVER);
//              //增加显示的宽度和高度
//              width_draw+=bitmap_para.bitmap.width;
//              height_draw+=bitmap_para.bitmap.height;
//          }
//
//        }
//    }
//exit:
//  free(colorbuf);
//    free(dbuf);
//  printf("end ----window_draw_text!\n");
//  return text;
//}
//// ========================================================================================================
//// 函数功能：将数字按位以为本的形式显示，进制和显示的长度自己指定
//// 输入参数: para：主要是显示的文本的一些信息。
////           data:待显示的数据，hex指定的进制，
////           bitNumber，显示成几位，不够的补0，超过的低位不显示，数据不准确不应该是我的责任,包括正负号
////                      正数：空格+绝对值，负数：“-”+绝对值
////           selectBit,编辑的第几位，
////           selectTextColor,selectBackColor,编辑的数据和背景颜色
//// 输出参数：
//// 返回值  :
//// 说明    : 宽度不够不进行显示，高度不够会尽量显示一部分。
////           我相信全世界的数字应该都是阿拉伯形式的，本API就是阿拉伯的，
////           如果不是，那么请做相应的修改，这个应该不是很难。
//// ==========================================================================================================
//void   draw_digital_in_rec(struct draw_wintext_para para, s32 data,s8 hex,s8 bitNumber,\
//                         s8 selectBit,u32 selectTextColor,u32 selectBackColor)
//{
//
//  u32  i=0,k=0;
//  bool_t signedflag=true;//正负数，正数true，负数false
//    //参数检查
//  if((NULL==para.gkwin)||(hex<2))//目前还没有2进制以下的数据，如果有请联系我
//  {
//      printf("Invalid parameter----draw_digital_in_rec\n");
//    return;
//  }
//  //计算在该进制下这个数据有多长
//  //判断正负数
//  if(data<0)
//  {
//    signedflag=false;
//    k=-data;
//  }
//  else
//  {
//    signedflag=true;
//    k=data;
//  }
//
//}


