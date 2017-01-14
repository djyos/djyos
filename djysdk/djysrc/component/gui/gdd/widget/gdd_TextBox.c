//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//所属模块: GDD
//作者:  zhb.
//版本：V1.0.0
//文件描述: 按钮控件实现
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2016-6-14
//   作者:  zhb.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//---------------------------------

#include  "gdd.h"
#include  <gui/gdd/gdd_private.h>
#include  "font.h"
#include  <gdd_widget.h>

extern HWND g_CursorHwnd;         //光标窗口

#define CN_CHAR_NUM_MAX                 255
#define CN_CANCLE_KEY                   0xA3

// =============================================================================
// 函数功能: 获取字符串长度字节数,TextBox只能显示单行信息，首先检查字符串,如果遇到
//           \n(换行符)则结束，截取换行符之前的字符作为有效的字符串.
// 输入参数: char *str:字符串
// 输出参数: 无。
// 返回值  :获取到有效字符字节数。
// =============================================================================
static s16 __GetValidStrLen(char *str)
{
    u8 cnt=0;
    char ch;
    s16 str_len;
    if(str==NULL)
        return -1;
    cnt=GetStrLineCount(str);
    if(cnt>1)
    {
        while(1)
        {
            ch=*str;
            if(ch!='\n')
            {
                str_len++;
                str++;
            }
            else
            {
                *(str+1)='\0';  //todo: 如果是const串怎么办？
                break;
            }
        }
    }
    else
    {
        str_len=strlen(str);
    }
    return str_len;
}

// =============================================================================
// 函数功能: 获取字符串中指定编号的字符，编号从1,2,3......开始.
// 输入参数: str:字符串指针
//           idx:字符编号
// 输出参数:
// 返回值  :ucs4字符
// =============================================================================
static u32 __GetCharByIndex(char *str,u8 idx)
{
    s16 str_len=0,len=0;
    struct FontRsc* cur_font;
    struct Charset* cur_enc;
    u32 wc=0;
    u8 cnt;
    if(str==NULL)
        return false;
    str_len=__GetValidStrLen(str);
    if(str_len==-1)
        return false;
     //计算字符串中字符数
    cur_font = Font_GetCurFont();
    cur_enc = Charset_NlsGetCurCharset();
    for(; str_len > 0;)
    {
       len= cur_enc->MbToUcs4(&wc, str, -1);
       if(len == -1)
       {  // 无效字符
           str_len--;
           str++;
       }
       else if(len == 0)
       {
            break;
       }
       else
       {    // 有效字符
           str += len;
           str_len -= len;
           cnt++;
           if(cnt==idx)
           {
               return wc;
           }
        }
    }
    return wc;

}
// =============================================================================
// 函数功能: 获取字符串中字符数及总的字符所占像素数。
// 输入参数: str:字符串指针
//          pChunm:字符数指针,输入参数为NULL时则不计算字符数。
//          pChWidthSum:字符所占总像素数指针，输入参数为NULL时则不计算字符所占总像素宽度.
// 输出参数: 无。
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
static bool_t __GetValidStrInfo(char *str,u16 *pChunm,u16 *pChWidthSum)
{
    u16 cnt=0,chwidthsum=0;
    s16 str_len=0;
    s32 len,chwidth=0;
    struct FontRsc* cur_font;
    struct Charset* cur_enc;
    u32 wc;
    if(str==NULL)
        return false;
    str_len=__GetValidStrLen(str);
    if(str_len==-1)
        return false;
     //计算字符串中字符数
     cur_font = Font_GetCurFont();
     cur_enc = Charset_NlsGetCurCharset();
     for(; str_len > 0;)
     {
        len= cur_enc->MbToUcs4(&wc, str, -1);
        if(len == -1)
        {   // 无效字符
            str_len--;
            str++;
        }
        else if(len == 0)
        {
             break;
        }
        else
        {          // 有效字符
            str += len;
            str_len -= len;
            if(pChunm!=NULL)
            {
              cnt++;
            }
            if(pChWidthSum!=NULL)
            {
              chwidth=cur_font->GetCharWidth(wc);
              chwidthsum+=chwidth;
            }
            continue;
        }
     }

     *pChWidthSum=chwidthsum;
     *pChunm=cnt;
     return true;
}

// =============================================================================
// 函数功能: 获取字符串到指定编号字符所占的像素宽度.
// 输入参数: str:字符串指针
//          idx:字符编号.
// 输出参数: 无。
// 返回值  :字符像素总宽度.
// =============================================================================
static s16 __GetStrWidth(char *str,u16 idx)
{
    struct FontRsc* cur_font;
    struct Charset* cur_enc;
    u32 wc;
    s32 len,chwidth=0;
    u16 cnt=0,chwidthsum=0;
    s16 str_len=0;
    if(str==NULL)
         return -1;
    str_len=__GetValidStrLen(str);
    if(str_len==-1)
         return -1;
    if(idx==0)
        return 0;
    //计算字符串中字符数
    cur_font = Font_GetCurFont();
    cur_enc = Charset_NlsGetCurCharset();
    for(; str_len > 0;)
    {
        len= cur_enc->MbToUcs4(&wc, str, -1);
        if(len == -1)
        {   // 无效字符
            str_len--;
            str++;
        }
        else if(len == 0)
             break;
        else
        {          // 有效字符
            str += len;
            str_len -= len;
            cnt++;
            chwidth=cur_font->GetCharWidth(wc);
            chwidthsum+=chwidth;
            if(cnt==idx)
                break;
            else
               continue;
        }
    }

     return chwidthsum;
}

// =============================================================================
// 函数功能: 移动光标.
// 输入参数: hwnd:文本框窗口句柄;
//          idx:文本框字符编号.
// 输出参数: 无。
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
static bool_t __MoveCursor(HWND hwnd,u8 idx)
{
    //内部调用，无需进行参数检查
    TextBox *pTB;
    char *str;
    RECT rc;
    u16 width;
    s32 x,y;
    pTB=(TextBox *)GetWindowPrivateData(hwnd);
    if(pTB==NULL)
        return false;
    GetClientRectToScreen(hwnd,&rc);
    str=hwnd->Text;
    if(str==NULL)
        return false;
    if(idx==0)
    {
        x=rc.left+1;
    }
    else
    {
        width=__GetStrWidth(str,idx);
        x=rc.left+width+1;
    }
    y=rc.top;
    MoveWindow(g_CursorHwnd,x,y);
    UpdateDisplay(CN_TIMEOUT_FOREVER);
    return true;
}
// =============================================================================
// 函数功能: 根据触摸点坐标获取文本框中字符编号
// 输入参数: hwnd:文本框窗口句柄
//           x:触摸点x方向坐标
//           y:触摸点y方向坐标
// 输出参数: 无。
// 返回值  :文本框中字符编号.
// =============================================================================
static s16 __FindIdx(HWND hwnd,u16 x,u16 y)
{
    char *str;
    TextBox *pTB;
    RECT rc;
    s16 tmp,val1,val2;
    u16 chnum,idx,width=0;
    str=hwnd->Text;
    if(str==NULL)
        return -1;
    pTB=(TextBox *)GetWindowPrivateData(hwnd);
    if(pTB==NULL)
        return -1;
    chnum=pTB->ChNum;
    GetClientRectToScreen(hwnd,&rc);
    for(idx=1;idx<chnum;idx++)
    {
        width=__GetStrWidth(str, idx);
        tmp=rc.left+width;
        if(tmp>x)
        {
           break;
        }
    }
    val1=tmp-x;
    width=__GetStrWidth(str, idx-1);
    tmp=rc.left+width;
    val2=x-tmp;
    if(val1>val2)
    {
        return idx-1;
    }
    else
    {
        return idx;
    }
}
// =============================================================================
// 函数功能: 获取字符串中从开始到指定编号的字符总的字节数.
// 输入参数: str:字符串指针
//           idx:字符串中指定字符序号，从1开始，依次往后增加.
// 输出参数: 无
// 返回值  :指定字符地址.
// =============================================================================
static s16 __CharToBytes(char *str,u8 num)
{
     u16 bytes=0;
     s16 str_len=0;
     u8 cnt=0;
     u32 wc;
     s32 len;
     struct FontRsc* cur_font;
     struct Charset* cur_enc;
     if(str==NULL)
          return -1;
     str_len=__GetValidStrLen(str);
     if(str_len==-1)
        return -1;
     if(num==0)
         return -1;
     cur_font = Font_GetCurFont();
     cur_enc = Charset_NlsGetCurCharset();
     for(; str_len > 0;)
     {
        len= cur_enc->MbToUcs4(&wc, str, -1);
        if(len == -1)
        {     // 无效字符
            str_len--;
            str++;
            bytes++;
        }
        else if(len == 0)
        {
            break;
        }
        else
        {          // 有效字符
             str += len;
             str_len -= len;
             bytes+=len;
             cnt++;
         if(cnt==num)
         {
             return bytes;
         }
              continue;
         }
     }
     return bytes;
}

// =============================================================================
// 函数功能:在TextBox中添加字符串
// 输入参数: hwnd:文本框窗体句柄
//          str:字符串指针
// 输出参数: 无。
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
static bool_t TextBox_AddChar(HWND hwnd,char *str )
{
     char *text;
     s16 len=0,str_len=0;
     u16 cnt,num=0;
     s16 bytes;
     u8 i;
     TextBox *pTB;
     bool_t ret;
     if(hwnd==NULL)
        return false;
     if(str==NULL)
        return false;
     pTB=(TextBox *)GetWindowPrivateData(hwnd);
     if(pTB==NULL)
        return false;
    //检查文本框编辑属性
     if(pTB->EditProperty==EN_R_O)
        return false;
     text=hwnd->Text;
     len=__GetValidStrLen(text);
     if(len==-1)
          return false;
     //检查一下str的合法性
      str_len=__GetValidStrLen(str);
      if(str_len==-1)
          return false;
      ret=__GetValidStrInfo(str,&num,NULL);
      if(!ret)
          return false;
      //字符串原有的字符数
      cnt=pTB->ChNum;
      if(cnt+num>CN_CHAR_NUM_MAX)
      {
           num=CN_CHAR_NUM_MAX-cnt;
      }
      bytes=__CharToBytes(str, num);
      //将字符串加在原来字符串后面
      for(i=0;i<bytes;i++)
      {
          *(text+len+i)=*(str+i);
      }
      *(text+len+bytes)='\0';
      return true;
}

// =============================================================================
// 函数功能:在TextBox指定位置开始删除字符串
// 输入参数: hwnd,文本框窗体句柄；
//          idx,字符编号；
//          count,字符数.
// 输出参数: 无。
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
static bool_t TextBox_DeleteChar(HWND hwnd,u8 idx,u8 count)
{
     char *text;
     char temp_str[2*CN_CHAR_NUM_MAX+1];
     u8 i,cnt,k,tmp;
     u16 f_len,len,last_len=0;
     s16 str_len=0;
     bool_t ret;
     TextBox *pTB;
     if(hwnd==NULL)
         return false;
     pTB=(TextBox *)GetWindowPrivateData(hwnd);
     if(pTB==NULL)
            return false;
     if(pTB->EditProperty==EN_R_O)
           return false;
     text=hwnd->Text;
     cnt=pTB->ChNum;
     str_len=__GetValidStrLen(text);
     if(str_len==-1)
         return false;
     if(count>idx)
         return false;
     tmp=idx-count;
     if(tmp==0)
     {
         k=cnt-idx;
         if(k==0)
         {
            *text='\0';
            return true;
         }
         else
         {
             len=__CharToBytes(text,idx);
             last_len=str_len-len;
             for(i=0;i<last_len;i++)
             {
                temp_str[i]=*(text+len+i);
             }
             temp_str[last_len]='\0';
             str_len=last_len+1;
             memcpy(text,temp_str,str_len);
             return true;
         }
     }

     f_len=__CharToBytes(text,tmp);
     for(i=0;i<f_len;i++)
     {
        temp_str[i]=*(text+i);
     }
     if(idx!=cnt)
     {
        len=__CharToBytes(text,idx);
        last_len=str_len-len;
        for(i=0;i<last_len;i++)
        {
            temp_str[f_len+i]=*(text+len+i);
        }
     }
     temp_str[f_len+last_len]='\0';
     str_len=f_len+last_len+1;
     memcpy(text,temp_str,str_len);
     return true;
}

// =============================================================================
// 函数功能:在TextBox指定字符处开始插入字符串
// 输入参数:  hwnd,文本框窗体句柄；
//           idx,字符编号；
//           str,字符串指针.
// 输出参数: 无。
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
static bool_t TextBox_InsertChar(HWND hwnd,u8 idx,char *str)
{
     char temp_str[2*CN_CHAR_NUM_MAX+1];
     char *text;
     u8 i;
     u16 f_len,last_len;
     u16 num=0,cnt;
     s16 bytes,str_len=0;
     TextBox *pTB;
     bool_t ret;
     if(hwnd==NULL)
        return false;
     if(str==NULL)
        return false;
     pTB=(TextBox *)GetWindowPrivateData(hwnd);
     if(pTB==NULL)
        return false;
     //检查文本框编辑属性
     if(pTB->EditProperty==EN_R_O)
            return false;
     text=hwnd->Text;
     str_len=__GetValidStrLen(text);
     if(str_len==-1)
         return false;
     ret=__GetValidStrInfo(str,&num,NULL);
     if(!ret)
         return false;
    //字符串原有的字符数
     cnt=pTB->ChNum;
     if(cnt+num>CN_CHAR_NUM_MAX)
     {
          num=CN_CHAR_NUM_MAX-cnt;
     }
     bytes=__CharToBytes(str, num);
     f_len=__CharToBytes(text,idx);

     for(i=0;i<f_len;i++)
     {
         temp_str[i]=*(text+i);
     }
     //Insert
     for(i=0;i<bytes;i++)
     {
         temp_str[f_len+i]=*(str+i);
     }
     last_len=str_len-f_len;
     for(i=0;i<last_len;i++)
     {
         temp_str[f_len+bytes+i]=*(text+f_len+i);
     }
     temp_str[f_len+bytes+last_len]='\0';
     str_len=f_len+bytes+last_len+1;
     memcpy(text,temp_str,str_len);

     return true;
}

// =============================================================================
// 函数功能: 文本框控件Create消息处理函数.
// 输入参数: pMsg,窗体消息指针.
// 输出参数: 无。
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
static bool_t TextBox_Create(struct WindowMsg *pMsg)
{
	HWND hwnd;
	TextBox *pTB;
	if(pMsg==NULL)
		return false;
	hwnd =pMsg->hwnd;
	if(hwnd==NULL)
		return false;
	if(pMsg->Param1==0)
	{
		pTB=(TextBox *)malloc(sizeof(TextBox));
		if(pTB==NULL)
			return false;
		pTB->ChNum=0;
		pTB->CharNumLimit=CN_CHAR_NUM_MAX;
		pTB->EditProperty=EN_R_W;
		pTB->Visible=true;
		pTB->CursorLoc=0;
        pTB->IsMultiLines=false;
        pTB->MaxLines=1;
        pTB->CharWidthSum=0;
        SetWindowPrivateData(hwnd,(void *)pTB);
    }

    return true;
}
// =============================================================================
// 函数功能: 判断按键按下的字符是否为数字或者小数点字符.
// 输入参数: keyval:字符ASCII码.
// 输出参数: 无。
// 返回值  :是则返回true,否则返回false.
// =============================================================================
static bool_t __IsValidInputKey(u8 keyval)
{
    bool_t ret=true;
    if(keyval<VK_NUM_0-1)
    {
        if(keyval!=VK_DECIMAL_POINT)
            return false;
    }
    if(keyval>VK_NUM_9+1)
        return false;
    return ret;
}
// =============================================================================
// 函数功能: 文本框控件绘制函数
// 输入参数: pMsg,窗体消息指针
// 输出参数: 无。
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
static  bool_t TextBox_Paint(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC hdc;
    RECT rc;
    u8 linecount;
    char *str;
    u16 count;
    u32 flag;
    TextBox *pTB;
    bool_t ret;
    if(pMsg==NULL)
        return false;
    hwnd =pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    pTB=(TextBox *)GetWindowPrivateData(hwnd);
    if(pTB==NULL)
        return false;
    hdc =BeginPaint(hwnd);
    if(NULL!=hdc)
    {
        //由于TextBox只能显示单行信息,因此先判断一下字符串是否包含多行,若为多行,则
        //只显示第一行信息,其他行信息直接忽略.
        if(!pTB->IsMultiLines)
        {
            str=hwnd->Text;
            ret=__GetValidStrInfo(str,&count,NULL);
            if(!ret)
                return false;
       }
       GetClientRect(hwnd,&rc);
       SetFillColor(hdc,RGB(255,0,0));
       FillRect(hdc,&rc);
       if(hwnd->Style&WS_BORDER)
       {
          if(hwnd->Style&BORDER_FIXED3D)
          {
             SetDrawColor(hdc,RGB(173,173,173));
             DrawLine(hdc,0,0,0,RectH(&rc)-1); //L
             SetDrawColor(hdc,RGB(234,234,234));
             DrawLine(hdc,0,0,RectW(&rc)-1,0);   //U
             DrawLine(hdc,RectW(&rc)-1,0,RectW(&rc)-1,RectH(&rc)-1); //R
             DrawLine(hdc,0,RectH(&rc)-1,RectW(&rc)-1,RectH(&rc)-1); //D
          }
          else
          {
             SetDrawColor(hdc,RGB(169,169,169));
             DrawLine(hdc,0,0,0,RectH(&rc)-1); //L
             DrawLine(hdc,0,0,RectW(&rc)-1,0);   //U
             DrawLine(hdc,RectW(&rc)-1,0,RectW(&rc)-1,RectH(&rc)-1); //R
             DrawLine(hdc,0,RectH(&rc)-1,RectW(&rc)-1,RectH(&rc)-1); //D
          }
        }

        SetTextColor(hdc,RGB(1,1,1));
        DrawText(hdc,str,count,&rc,DT_VCENTER|DT_LEFT);
        EndPaint(hwnd,hdc);
      }
    return true;
}

// =============================================================================
// 函数功能: TextBox控件KEY_DOWN_MSG消息响应函数
// 输入参数: pMsg,窗体消息结构体指针
// 输出参数: 无。
// 返回值  :成功返回true,失败则返回false。
// =============================================================================
static bool_t TextBox_KeyDown(struct WindowMsg *pMsg)
{
    HWND hwnd;
    u8 cursorloc,chnum,chnummax,keyval;
    TextBox *pTB;
    char tmpbuf[2];
    bool_t ret;
    struct FontRsc* cur_font;
    struct Charset* cur_enc;
    u32 wc;
    s32 chwidth,width=0;
    u32 ch;
    char *str;
    RECT rc;
    if(pMsg==NULL)
        return false;
    hwnd =pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    ret=IsFocusWindow(hwnd);
    if(!ret)
        return false;
    keyval=(u8)pMsg->Param1;
    tmpbuf[0]=(char)keyval;
    tmpbuf[1]='\0';
    pTB=(TextBox *)GetWindowPrivateData(hwnd);
    cursorloc=pTB->CursorLoc;
    chnum=pTB->ChNum;
    chnummax=pTB->CharNumLimit;
    ret=__IsValidInputKey( keyval);
    if(ret)
    {
         if(chnum!=chnummax)
         {
            cur_font = Font_GetCurFont();
            cur_enc = Charset_NlsGetCurCharset();
            cur_enc->MbToUcs4(&wc, tmpbuf, -1);
            chwidth=cur_font->GetCharWidth(wc);
            GetClientRect(hwnd,&rc);
            width=RectW(&rc);
            if(width>=pTB->CharWidthSum+chwidth)
            {
                if(chnum!=cursorloc)
                {
                    TextBox_InsertChar(hwnd, cursorloc, tmpbuf);
                }
                else
                {
                    TextBox_AddChar(hwnd, tmpbuf);
                }
                cur_font = Font_GetCurFont();
                cur_enc = Charset_NlsGetCurCharset();
                cur_enc->MbToUcs4(&wc, tmpbuf, -1);
                chwidth=cur_font->GetCharWidth(wc);
                pTB->ChNum++;
                pTB->CursorLoc++;
                pTB->CharWidthSum+=chwidth;
                InvalidateWindow( hwnd, true);
                __MoveCursor(hwnd,pTB->CursorLoc);
             }
         }
    }
    else
    {
         switch (keyval)
         {
               case VK_RETURN:
                //todo
                break;
               case VK_DEL:
               case CN_CANCLE_KEY:
                   if(pTB->ChNum>=1)
                   {
                      if(cursorloc!=0)
                      {
                         str=hwnd->Text;
                         ch=__GetCharByIndex(str,pTB->CursorLoc);
                         cur_font = Font_GetCurFont();
                         chwidth=cur_font->GetCharWidth(ch);
                         TextBox_DeleteChar(hwnd, cursorloc,1);
                         pTB->ChNum--;
                         pTB->CursorLoc--;
                         pTB->CharWidthSum-=chwidth;
                      }
                      InvalidateWindow( hwnd, true);
                      __MoveCursor(hwnd,pTB->CursorLoc);
                   }
                break;

           default:
            break;

         }
    }

    SetWindowPrivateData( hwnd, (void *)pTB);
    return true;
}

// =============================================================================
// 函数功能: TextBox控件KEY_UP_MSG消息响应函数
// 输入参数: pMsg,窗体消息结构体指针
// 输出参数: 无。
// 返回值  :无。
// =============================================================================
static bool_t TextBox_KeyUp(struct WindowMsg *pMsg)
{

}
// =============================================================================
// 函数功能: TextBox控件KEY_PRESS_MAG消息绘制函数
// 输入参数: pMsg,窗体消息结构体指针
// 输出参数: 无。
// 返回值  :无。
// =============================================================================
static bool_t TextBox_KeyPress(struct WindowMsg *pMsg)
{
    return true;
}



static bool_t TextBoxL_Down(struct WindowMsg *pMsg)
{
    return true;
}

static bool_t TextBoxL_Up(struct WindowMsg *pMsg)
{
   return true;
}


static void TextBox_TouchUp(struct WindowMsg *pMsg)
{

}

// =============================================================================
// 函数功能: TextBox控件触摸屏按下响应函数。
// 输入参数: pMsg,窗体消息结构体指针
// 输出参数: 无。
// 返回值  :成功返回true,失败则返回false。
// =============================================================================
static bool_t TextBox_TouchDown(struct WindowMsg *pMsg)
{
    HWND hwnd;
    u32 loc;;
    u16 chnum,idx,CharWidth,x,y;
    TextBox *pTB;
    char *str;
    s32 tmp;
    RECT rc;
    bool_t ret;
    if(pMsg==NULL)
        return false;
    hwnd =pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    pTB=(TextBox *)GetWindowPrivateData(hwnd);
    if(pTB==NULL)
        return false;
    //将当前TextBox控件设置为焦点窗口
    SetFocusWindow(hwnd);
    //在文本框上
    loc=pMsg->Param2;   //获取触摸点信息
    x=loc;
    y=loc>>16;
    chnum=pTB->ChNum;
    str=hwnd->Text;
    if(str==NULL)
    {
         pTB->CursorLoc=0;
        __MoveCursor(hwnd,0);
    }
    else
    {
        ret=__GetValidStrInfo(str,NULL,&CharWidth);
        GetWindowRect(hwnd,&rc);
        tmp=rc.left+CharWidth;
        if(x>tmp)
        {
            pTB->CursorLoc=chnum;
            __MoveCursor(hwnd,chnum);
        }
        else
        {
           idx=__FindIdx(hwnd,x,y);
           pTB->CursorLoc=idx;
           __MoveCursor(hwnd,idx);
        }
    }
    SetWindowPrivateData(hwnd,(void *)pTB);
    return true;
}

// =============================================================================
// 函数功能: TextBox控件获得焦点消息响应函数。
// 输入参数: pMsg,窗体消息指针
// 输出参数: 无。
// 返回值  :成功返回true,失败则返回false。
// ======================================================================
static bool_t TextBox_SetFocus(struct WindowMsg *pMsg)
{
    HWND hwnd,Tmrhwnd;
    if(pMsg==NULL)
        return false;
    hwnd =pMsg->hwnd;
    if(hwnd==NULL)
        return false;
    Tmrhwnd=GetDesktopWindow();
    if(Tmrhwnd!=NULL)
    {
       PostMessage(Tmrhwnd,MSG_TIMER_START,CN_CURSOR_TIMER_ID,(ptu32_t)hwnd);
    }
    return true;
}
// =============================================================================
// 函数功能: TextBox控件失去焦点消息响应函数。
// 输入参数: pMsg,窗体消息指针
// 输出参数: 无。
// 返回值  :成功返回true,失败则返回false。
// ======================================================================
static bool_t TextBox_KillFocus(struct WindowMsg *pMsg)
{
     HWND hwnd,Tmrhwnd;
     if(pMsg==NULL)
          return false;
     hwnd =pMsg->hwnd;
     if(hwnd==NULL)
          return false;
     Tmrhwnd=GetDesktopWindow();
     if(Tmrhwnd!=NULL)
     {
         PostMessage(Tmrhwnd,MSG_TIMER_STOP,CN_CURSOR_TIMER_ID,(ptu32_t)hwnd);
     }
     return true;
}

// =============================================================================
// 函数功能: TextBox控件获取文本内容函数。
// 输入参数: hwnd,文本框窗口句柄.
// 输出参数: 无。
// 返回值  :无。
// =============================================================================
static void __TextBox_GetText(HWND hwnd,char *text)
{
     TextBox *pTB;
     u16 len;
     char *str;
     pTB=GetWindowPrivateData(hwnd);
     if(pTB==NULL)
         return ;
     str=hwnd->Text;
     len=strlen(str);
     memcpy(text,str,len);
     *(text+len)='\0';
}

// =============================================================================
// 函数功能: TextBox控件设置文本Text函数。
// 输入参数: hwnd,文本框窗口句柄.
// 输出参数: 无。
// 返回值  :无。
// =============================================================================
static void __TextBox_SetText(HWND hwnd,char *str)
{
     u16 len;
     char *dst;
     dst=hwnd->Text;
     len=strlen(str);
     memcpy(dst,str,len);
     *(dst+len)='\0';
     InvalidateWindow(hwnd, true);
}

// =============================================================================
// 函数功能: TextBox控件删除文本函数。
// 输入参数: hwnd,文本框窗口句柄.
// 输出参数: 无。
// 返回值  :无。
// =============================================================================
static void __TextBox_DeleteText(HWND hwnd)
{
     hwnd->Text='\0';
     InvalidateWindow(hwnd, true);
}
// =============================================================================
// 函数功能: TextBox控件显示内容控制函数
// 输入参数: hwnd,TextBox控件窗口句柄;
//          ctrlcmd,详情参见enum TextCtrlCmd
//          para1:对于EN_GET_TEXT、EN_SET_TEXT、EN_DELETE_TEXT该参数无效,可直接置0。
//          para2:对于EN_DELETE_TEXT该参数无效,可直接置0,对于EN_GET_TEXT、EN_SET_TEXT
//          输入字符串指针.
// 返回值  :成功则返回true，失败则返回false.
// =============================================================================
bool_t TextBox_TextCtrl(HWND hwnd,u8 ctrlcmd,u32 para1,ptu32_t para2)
{
    if(hwnd==NULL)
        return false;
    switch(ctrlcmd)
    {
        case EN_GET_TEXT:
            __TextBox_GetText(hwnd,(char *)para2);
            break;
        case EN_SET_TEXT:
            __TextBox_SetText(hwnd,(char *)para2);
            break;
        case EN_DELETE_TEXT:
            __TextBox_DeleteText(hwnd);
            break;
        default:
            break;
    }
    return true;
}


//默认按钮消息处理函数表，处理用户函数表中没有处理的消息。
static struct MsgProcTable s_gTextBoxMsgProcTable[] =
{
    {MSG_KEY_DOWN,TextBox_KeyDown},
    {MSG_KEY_UP,TextBox_KeyUp},
    {MSG_PAINT,TextBox_Paint},
    {MSG_CREATE,TextBox_Create},
    {MSG_TOUCH_DOWN,TextBox_TouchDown},
    {MSG_TOUCH_UP,TextBox_TouchUp},
    {MSG_SETFOCUS,TextBox_SetFocus},
    {MSG_KILLFOCUS,TextBox_KillFocus}
};

static struct MsgTableLink  s_gTextBoxMsgLink;
// =============================================================================
// 函数功能: TextBox控件创建函数。
// 输入参数: Text:文本框窗口Text;
//           Style:文本框风格，参见gdd.h;
//           x:文本框起始位置x方向坐标(单位：像素);
//           y:文本框起始位置y方向坐标(单位：像素);
//           w:文本框宽度(单位：像素);
//           h:文本框高度(单位：像素);
//           hParent:文本框父窗口句柄;
//           WinId:文本框控件Id;
//           pdata:文本框控件私有数据结构;
//           UserMsgTableLink:文本框控件用户消息列表结构指针。
// 输出参数: 无。
// 返回值  :成功则返回文本框句柄，失败则返回NULL。
// =============================================================================
HWND CreateTextBox(const char *Text,u32 Style,
                    s32 x,s32 y,s32 w,s32 h,
                    HWND hParent,u32 WinId,void *pdata,
                    struct MsgTableLink *UserMsgTableLink)
{
    WINDOW *pGddWin=NULL;
    struct MsgTableLink *Current;
    if(UserMsgTableLink != NULL)
    {
        Current = UserMsgTableLink;
        while(Current->LinkNext != NULL)
            Current = Current->LinkNext;
        Current->LinkNext = &s_gTextBoxMsgLink;
        Current = UserMsgTableLink;
    }
    else
        Current = &s_gTextBoxMsgLink;
    s_gTextBoxMsgLink.LinkNext = NULL;
    s_gTextBoxMsgLink.MsgNum = sizeof(s_gTextBoxMsgProcTable) / sizeof(struct MsgProcTable);
    s_gTextBoxMsgLink.myTable = (struct MsgProcTable *)&s_gTextBoxMsgProcTable;
    pGddWin=CreateWindow(Text,WS_CHILD|Style|TEXTBOX,x,y,w,h,hParent,WinId,CN_WINBUF_PARENT,pdata,Current);
    return pGddWin;
}


