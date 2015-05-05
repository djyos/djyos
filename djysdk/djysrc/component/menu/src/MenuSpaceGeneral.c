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
// 模块描述: DJY菜单模块之显示空间计算的通用函数定义
// 模块版本: V1.00(初始化版本)
// 创建人员: zqf
// 创建时间: 20121224
// =======================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =======================================================================
#include "MenuSpaceGeneral.h"
#include "MenuitemApi.h"
//默认的颜色


// =========================================================================
// 函数功能：初始化一个矩形结构体
// 输入参数：
// 输出参数：
// 返回值  ： 一个内部成员全为0的矩形结构体
// =========================================================================
struct tagRectangle  zero_strect(void)
{
  struct tagRectangle result={0,0,0,0};
  return result;
}


// =========================================================================
// 函数功能：为显示参数分配空间，并负责初始化公共成员变量
// 输入参数：
// 输出参数：
// 返回值  ： 经过初始化的显示参数的指针
// =========================================================================
struct menu_displaypara *mallocdisplaypara(void)
{

   struct menu_displaypara  *result;

   result=(struct menu_displaypara *)M_MallocLc(sizeof(struct menu_displaypara),0);
   if(NULL==result)
   {
      printf("not enough mem----mallocdisplaypara\n");
      return result;
   }
/*
   //初始化链表指针
   result->next=NULL;
   result->pre=NULL;

   //初始化窗口指针
   result->AppWin=NULL;
   result->CurWin=NULL;


   //初始化菜单项指针
   result->OperatingMenuitem=NULL;
   result->FVisualMenuitem=NULL;
   result->LVisualMenuitem=NULL;
    //初始化显示空间
   result->MenuitemSpace.height=0;
   result->MenuitemSpace.width=0;

   //私有数据
   result->myprivate=NULL;
   //其他成员会在显示参数创建的过程中初始化
   */
    memset(result, 0, sizeof(struct menu_displaypara));
   //初始化颜色，使用默认颜色
   result->DisplayColor.menu_color=cn_menu_color;
   result->DisplayColor.menu_lighted_color=cn_menu_lighted_color;
   result->DisplayColor.text_color=cn_menu_text_color;

   return result;
}

// =========================================================================
// 函数功能:释放显示参数空间，并将其成员复原
// 输入参数:待释放显示参数
// 输出参数：
// 返回值  :无
// =========================================================================

void deldisplaypara(struct menu_displaypara *menudispara)
{

    struct menu_displaypara  *temp1;//遍历要删除的显示参数
    struct menu_displaypara  *temp2;//删除的对象

    temp1=menudispara;

    if( NULL==menudispara)
    {
        printf("This is an NULL menudispara,no need to del!----nDelDisplaypara\n");
        return ;
    }
    while(NULL!=temp1->next)//找到最后一个，从后往前删除
    {
         temp1=temp1->next;
    }
    while( menudispara!=temp1)//删除所有后面的节点
    {
        if(NULL!=temp1->CurWin)//销毁窗口
        {
           GK_ApiDestroyWin(temp1->CurWin);//该函数是立即执行函数，不会阻塞
           free(temp1->CurWin);//删除窗口所占用的内存
        }

        temp2=temp1;
        temp1=temp1->pre;
        free(temp2);
    }
    //删除当前节点
    if(NULL!=temp1->CurWin)//
    {
        GK_ApiDestroyWin(temp1->CurWin);
        //立即删除当前窗口，不经过管道,因为要待会释放内存，不然的话在释放内存后销毁
        //必然会造成内存紊乱
        free(temp1->CurWin);//释放窗口占用的内存
        temp1->CurWin=NULL;
    }
    free(temp1);
    return;
}
// =========================================================================
// 函数功能:将显示参数加入到链表中
// 输入参数:head:链表队列   newpara:待加入的显示参数
// 输出参数：
// 返回值  :无
// =========================================================================
void adddisplaypara(struct menu_displaypara * head,\
        struct menu_displaypara * newpara)
{
    if((NULL==head)||(NULL==newpara))
    {
     printf("You have supplied an null para--adddisplaypara!\n");
     return ;
    }
    while(NULL!=head->next)
        {
      head=head->next;
    }
    head->next=newpara;
    newpara->pre=head;
    //用户有可能是添加的一个链表，所以注释掉一下行,所以不用管newpara->next的情况
}
// =========================================================================
// 函数功能:将显示参数从链表中移除
// 输入参数:需要移除的显示参数:operatingDispara
// 输出参数：
// 返回值    :无
//  说明      :虽然从该链表中移除了此显示参数，但是并没有删除此显示参数
// =========================================================================
void removedisplaypara(struct menu_displaypara * operatingDispara)
{
    if(NULL==operatingDispara)
    {
     printf("Invalid parameter--removedisplaypara!\n");
     return ;
    }
    if((NULL==operatingDispara->pre)&&(NULL==operatingDispara->next))//只有一个
    {
        //return;
    }
    else if (NULL==operatingDispara->pre)
    {
      operatingDispara->next->pre=NULL;
    }
    else if(NULL==operatingDispara->next)
    {
        operatingDispara->pre->next=NULL;
    }
    else
    {
        operatingDispara->pre->next=operatingDispara->next;
        operatingDispara->next->pre=operatingDispara->pre;
    }
    return;
}

// =========================================================================
// 函数功能:根据提供的字符集和字体计算字符串的位图的宽和高
// 输入参数:text，待测试的字符串，count:字符串的长度，不包括结束符
// 输出参数：
// 返回值  :无
// =========================================================================
struct menu_item_space cal_textarea(const char *text, u32 count )
{


    s32 len, src_len,size_bak;
    u32 wc;
    struct tagFontRsc* cur_font;
    struct tagGkscParaDrawBitmapRop bitmap_para;
    struct menu_item_space   result={0,0};

    cur_font = Font_GetCurFont();
    if(cur_font == NULL)
        return result;
    src_len = count;

    result.height=0;
    result.width=0;

    bitmap_para.bitmap.bm_bits = NULL;
    size_bak = 0;

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

            result.width+=bitmap_para.bitmap.width;//长度累加

            if(bitmap_para.bitmap.height>result.height)//高度取最高的一个
            {
                result.height=bitmap_para.bitmap.height;
            }

        }
    }

  return result;
}


// =========================================================================
// 函数功能:计算提供的菜单项的同层菜单项在当前字符集下的占用的最大空间
// 输入参数:待测的菜单项
// 输出参数：
// 返回值  :计算得到的菜单项在该当前字符集下的map大小
// 说明    ：其空间指的是菜单项的资源节点名字，当然包括相应的标识符
// =========================================================================
struct menu_item_space cal_menuitemarea(struct menu_item *menuitem)
{
    u8 size=0;
    struct menu_item *temp=NULL;
    struct menu_item_space result={0,0};
    struct menu_item_space tempSpace={0,0};

    if(NULL==menuitem)
    {
      printf("Invalid parameter--cal_menuitemarea\n");
      return result;
    }
    temp=menuitem;
    do{
        size=strlen(temp->node.name,namesize);
        tempSpace=cal_textarea(temp->node.name, size);
        //考虑到对齐，有没有子菜单都要加上12，用于标签和显示空余
        if(NULL!=temp->node.child)
        {
          tempSpace.width+=12;
        }
        else
        {
          tempSpace.width+=12;
        }
        if(tempSpace.width>result.width)
        {
          result.width=tempSpace.width;
        }
        if(tempSpace.height>result.height)
        {
          result.height=tempSpace.height;
        }
        temp=GetNextMenuitem(temp);

   }while(temp!=menuitem);
  return result;
}
// =========================================================================
// 函数功能:获取主菜单显示参数
// 输入参数:当前显示参数
// 输出参数 ：
// 返回值      :主菜单显示参数
// =========================================================================
struct menu_displaypara*  getmainmenudispara(struct menu_displaypara *cur)
{
    //参数检查
   if(cur==NULL)
   {
       printf("Invalid parameter---getmainmenudispara!\n");
       return NULL;
   }
   else
   {
       while(cur->pre!=NULL)
       {
           cur=cur->pre;
       }
       return cur;
   }
}

// =========================================================================
// 函数功能:为各种显示模式创建显示窗口
// 输入参数:cur当前显示参数，win_rec，计算出的显示参数，win_name，窗口名字
// 输出参数：
// 返回值  :
//需要说明的是:位置前面都计算好了，已经没有啥子好说的了
// =========================================================================
bool_t create_menudisparawin(struct menu_displaypara  *menudispara,struct tagRectangle win_rec,const char *win_name)
{

  bool_t result=false;

  struct tagGkWinRsc  *parwin;

  //参数检查
  if(NULL==menudispara)
  {
    printf("Invalid parameter --- creatdisparawin!\n");
     return result;
  }
  if(NULL==menudispara->pre)//主菜单
  {
    if(NULL==menudispara->AppWin)
    {
      printf("Invalid parameter --- creatdisparawin!\n");
      return result;
    }
    else
    {
     parwin=menudispara->AppWin;
     win_rec.top=menudispara->AppWin->absy0-parwin->absy0;
     win_rec.left=menudispara->AppWin->absx0-parwin->absx0;
     win_rec.right=win_rec.left+menudispara->AppWin->right-menudispara->AppWin->left;
     win_rec.bottom=win_rec.top+menudispara->AppWin->bottom-menudispara->AppWin->top;
    }
  }
  else//子菜单
  {
    parwin=menudispara->pre->CurWin;
    if((0==win_rec.bottom)&&(0==win_rec.right)&&(0==win_rec.top)&&(0==win_rec.bottom))//新建全屏窗口
    {
      win_rec.top=menudispara->AppWin->absy0-parwin->absy0;
      win_rec.left=menudispara->AppWin->absx0-parwin->absx0;
      win_rec.right=win_rec.left+menudispara->AppWin->right-menudispara->AppWin->left;
      win_rec.bottom=win_rec.top+menudispara->AppWin->bottom-menudispara->AppWin->top;
    }
    else
    {
      //子菜单正常大小
    }
  }
     //分配窗口空间，如果分配不到，一切都是浮云
  menudispara->CurWin=(struct tagGkWinRsc *)M_MallocLc(sizeof(struct tagGkWinRsc),0);

  if(NULL == menudispara->CurWin)
  {
    printf("no enough mem for the displaypara win!\n");
    return result;
  }
  result=GK_ApiCreateGkwin(parwin,menudispara->CurWin,\
                             win_rec.left, win_rec.top,win_rec.right,win_rec.bottom,\
                             menudispara->DisplayColor.menu_color, CN_WINBUF_NONE,\
                             win_name, 0,0,0,0);

  if(!result)//创建失败，释放所占用的内存
  {
    printf("Failed--- create_menudisparawin!\n");
    free(menudispara->CurWin);
    menudispara->CurWin=NULL;
    return result;
  }
  else
  {
    GK_ApiSetBoundMode(menudispara->CurWin, CN_BOUND_UNLIMIT);
    menudispara->CurWin->user_private=menudispara;
    return result;
  }
}












