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
//1. 日期: 2016-10-25
//   作者:  LiuWei.
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include    "gdd.h"
#include    <gui/gdd/gdd_private.h>
#include    <gdd_widget.h>
#include     <gkernel.h>
#include    "qrencode.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern QRcode *QRcode_encodeString(const char *string, int version,
									QRecLevel level, QRencodeMode hint,
									int casesensitive);
/*============================================================================*/
/*---------------------------------------------------------------------------
功能：  窗口创建函数
	输入   :tagWindowMsg *pMsg
	输出 :false或true
---------------------------------------------------------------------------*/
static bool_t HmiCreate_Qrcode(struct WindowMsg *pMsg)
{
	return true;
}
/*---------------------------------------------------------------------------
功能： 将从二维码函数库中得到的数据转换为指向存储像素阵列的数组
	输入   :QRcode结构体指针
	输出 :存储像素阵列的数组指针
---------------------------------------------------------------------------*/
static u8 *Data_conversion(QRcode*qrcode)
{
	u64 i,j,k,n,m;
	u8*Data;
	u8*QrcodeData;
	u8 *addr;
	QrcodeData=qrcode->data;
	n=(qrcode->width+7)/8;   //每行占得字节数
	Data=(u8*)malloc(n*qrcode->width);
	addr=Data;
	m=(qrcode->width)%8;
	for(k=0;k<qrcode->width;k++)
	{
		for(i=1;i<n;i++)//复制字节整数
		{
			for(j=0;j<8;j++)
			{
				if((*QrcodeData)&0x01)
					*Data &=~(0x80>>j);
				else
					*Data|=(0x80>>j);
				QrcodeData++;
			}
			Data++;
		}
		for(j=0;j<m;j++)//复制字节余数部分
		{
			if((*QrcodeData)&0x01)
				*Data &=~(0x80>>j);
			else
				*Data|=(0x80>>j);
			QrcodeData++;
		}
		Data++;
	}
	return addr;
}
/*---------------------------------------------------------------------------
功能：根据输入参数得到二维码信息
	输入   :string: 被编码的数据
		version: 版本号<=40     宽度=(版本号*4+17)
		level: 容错等级L（Low）：7%的字码可被修正
			   M（Medium）：15%的字码可被修正
			   Q（Quartile）：25%的字码可被修正
			   H（High）：30%的字码可被修正
		hint: 编码方式
		casesensitive: 是否区分大小写
	输出 :存储像素阵列的数组指针
---------------------------------------------------------------------------*/
//绘制消息处函数
static bool_t HmiPaint_Qrcode(struct WindowMsg *pMsg)
{
    HWND hwnd;
    HDC  hdc;
    RECT rc0;
    s32 height,width,size;
    u8 vernum;
    char *str;
    QRcode*qrcode; //最后结果
	struct RectBitmap   bitmap;
	struct RopGroup RopCode = (struct RopGroup){ 0, 0, 0, CN_R2_COPYPEN, 0, 0 };
	u8 *Data;
    if(pMsg==NULL)
       	return false;
    hwnd=pMsg->hwnd;
    if(hwnd==NULL)
    	return false;
	hdc =BeginPaint(hwnd);
	GetClientRect(hwnd,&rc0);
	SetDrawColor(hdc,CN_COLOR_BLACK);
	width=RectW(&rc0);
	height=RectH(&rc0);
    if(width<=height)
    {
    	size=width;
    }
    else
    {
    	size=height;
    }
    //根据尺寸计算二维码大小
    vernum=(size-17)/4;
    str=hwnd->Text;
/* 89x89位宽的二维码参数为
 * 版本号:18
 * 容错等级  (ECC)  数字  (容量)     字母	                             汉字          二进制代码
 * L			1,725		1,046	    442    586
 * M			1,346		816		    345    450
 * Q			948			574	        243    322
 * H			746			452		    191    250
 * */
   qrcode= QRcode_encodeString(str,vernum, QR_ECLEVEL_H, QR_MODE_8,0);
   if(qrcode==NULL)//存储空间不足或数据容量超出规定范围
   {
	   QRcode_free(qrcode);
	   return false;
   }
    Data=Data_conversion(qrcode);//提取显示数据到Data
    bitmap.bm_bits = Data;
    bitmap.linebytes = (qrcode->width+7)/8;
    bitmap.PixelFormat = CN_SYS_PF_GRAY1;
    bitmap.ExColor = CN_COLOR_WHITE;
    bitmap.height=(s32)(qrcode->width);
    bitmap.width=(s32)(qrcode->width);
    DrawBitmap(hdc,rc0.left,rc0.top,&bitmap,CN_SYS_PF_GRAY1,RopCode);
	EndPaint(hwnd,hdc);
    QRcode_free(qrcode);
    free(Data);
    return true;
}

//消息处理函数表
static struct MsgProcTable s_gQrcodeMsgProcTable[] =
{
    {MSG_CREATE,HmiCreate_Qrcode},         //主窗口创建消息
    {MSG_PAINT,HmiPaint_Qrcode}           //绘制消息
};

static struct MsgTableLink  s_gQrcodeMsgLink;


HWND CreateQrcode(  const char *Text,u32 Style,
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
        Current->LinkNext = &s_gQrcodeMsgLink;
        Current = UserMsgTableLink;
    }
    else
        Current = &s_gQrcodeMsgLink;
    s_gQrcodeMsgLink.LinkNext = NULL;
    s_gQrcodeMsgLink.MsgNum = sizeof(s_gQrcodeMsgProcTable) / sizeof(struct MsgProcTable);
    s_gQrcodeMsgLink.myTable = (struct MsgProcTable *)&s_gQrcodeMsgProcTable;
    pGddWin=CreateWindow(Text,WS_CHILD|Style,x,y,w,h,hParent,WinId,CN_WINBUF_PARENT,pdata,Current);
    return pGddWin;
}
