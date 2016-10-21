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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：crtouch.c
// 模块描述: 触摸芯片CRTOUCH驱动
// 模块版本: V1.10
// 创建人员: hm
// 创建时间: 15/10.2014
// =============================================================================

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "stdint.h"
#include "stdlib.h"

#include "int_hard.h"
#include "int.h"
#include "cpu_peri.h"
#include "Touch.h"
#include "spibus.h"
#include "gkernel.h"
#include "systime.h"
#include <LCM240128C/sr5333/lcm240128c_config.h>
#include <driver/flash/flash.h>


struct AdjustValue
{
	s16 XAdjustLeft;
	s16 YAdjustTop;
	s16 XAdjustRight;
	s16 YAdjustBottom;
};


#define CN_ADJUST_ADDR   0x5ffff8
static struct AdjustValue* s_ptAdjustVaule=(struct AdjustValue*)(CN_ADJUST_ADDR);

static const Pin PenStatus[] = {
        {PIO_PC28, PIOC, ID_PIOC, PIO_INPUT, PIO_PULLUP}
};

//定义SPIBUS架构下的SPI设备结构
static struct SPI_Device s_tgTouch_Dev;

extern  s32 EEFC_PageProgram(u32 Page, u8 *Data, u32 Flags);
extern  s32 EEFC_PageRead(u32 Page, u8 *Data, u32 Flags);
extern  s32 EEFC_SectorEarse(u32 SectorNo);


// =============================================================================
// 功能：将新校准得到的校准值写进片上Flash的最后8个字节中。
// 参数：pAdjustValue,校准值结构体指针
// 返回：true,写成功;false,写失败
// =============================================================================
static bool_t Touch_WriteAdjustValue(struct AdjustValue* pAdjustValue)
{
    u16 left,right,top,bottom;
    u8 buf[512];
    s32 ret;
    if(pAdjustValue==NULL)
    	return false;
    left=pAdjustValue->XAdjustLeft;
    right=pAdjustValue->XAdjustRight;
    top=pAdjustValue->YAdjustTop;
    bottom=pAdjustValue->YAdjustBottom;
    //先读最后一页
    ret=EEFC_PageRead(4095,buf,0);
    if(ret==-1)
    	return false;
    //然后擦除整个Sector，擦除只能以Sector为单位
    ret=EEFC_SectorEarse(15);
    if(ret==-1)
        return false;
    buf[504]=left;
    buf[505]=left>>8;
    buf[506]=top;
    buf[507]=top>>8;
    buf[508]=right;
    buf[509]=right>>8;
    buf[510]=bottom;
    buf[511]=bottom>>8;
    ret=EEFC_PageProgram(4095,buf,0);
    if(ret==-1)
        return false;
    return true;
}


// =============================================================================
// 功能：读取触摸屏AD采样值，分别为X值和Y值，未做校正的数据
// 参数：X,Y，输出参数，存储读取到的数据地址
// 返回：true,读取成功;false,读取失败
// =============================================================================
static bool_t Touch_ReadXY(s16 *X, s16 *Y)
{
    struct SPI_DataFrame data;
    u8 xybuf[] = {  0xd0,00,00,0x90,00,00,
//                    0xd0,00,00,0x90,00,00,
//                    0xd0,00,00,0x90,00,00,
//                    0xd0,00,00,0x90,00,00,
//                    0xd0,00,00,0x90,00,00,
                    0xd0,00,00,0x90,00,00,
                    0xd0,00,00,0x90,00,00,
                    0xd0,00,00,0x90,00,00};
    u16 maxx=0,minx=65535,maxy=0,miny=65535;
    s16 x0[4],y0[4];
    u32 loop;
    s32 result;
    s32 tempx=0,tempy=0;

#if 1
    data.SendBuf = xybuf;
    data.SendLen = 24;
    data.RecvBuf = xybuf;
    data.RecvLen = 23;
    data.RecvOff = 1;
    //读4个采样点的值
    result = SPI_Transfer(&s_tgTouch_Dev, &data, true, CN_TIMEOUT_FOREVER);
#else

    data.SendBuf = xybuf;
    data.SendLen = 1;
    data.RecvBuf = xybuf;
    data.RecvLen = 2;
    data.RecvOff = 1;
    result = SPI_Transfer(&s_tgTouch_Dev, &data, true, CN_TIMEOUT_FOREVER);

    data.SendBuf = xybuf+2;
    data.SendLen = 1;
    data.RecvBuf = xybuf+2;
    data.RecvLen = 2;
    data.RecvOff = 1;
    result = SPI_Transfer(&s_tgTouch_Dev, &data, true, CN_TIMEOUT_FOREVER);
#endif
    //丢弃第一个采样点（不准），去除后面3个采样点，并提取最大最小值
    for(loop = 1;loop<4;loop++)
    {
        x0[loop] = (xybuf[loop *6] << 8) +xybuf[loop*6+1];
        x0[loop] >>=3;
        y0[loop] = (xybuf[loop*6+3] << 8) +xybuf[loop*6+4];
        y0[loop] >>=3;
        if(x0[loop] > maxx)
            maxx = x0[loop];
        if(x0[loop] < minx)
            minx = x0[loop];
        if(y0[loop] > maxy)
            maxy = y0[loop];
        if(y0[loop] < miny)
            miny = y0[loop];
    }

    //最大最小值之差大于20的话，说明采样值不稳定（触笔在抖动）
    if( (maxx - minx > 20) || (maxy - miny > 20) )
        return false;
    else
    {
        for(loop = 1;loop<4;loop++)
        {
            tempx +=x0[loop];
            tempy +=y0[loop];
        }
        tempx -= minx;      //减掉最大值和最小值
        tempx -= maxx;
        tempy -= miny;
        tempy -= maxy;
        *X = tempx / 1;
        *Y = tempy / 1;
        return true;
    }
}

// =============================================================================
// 功能：读取此时是否触摸屏上有触摸
// 参数：无
// 返回：1,触摸;0,未触摸
// =============================================================================
static bool_t Touch_Touched(void)
{
    u8 TouchS;
    TouchS = PIO_Get(PenStatus);
    if(TouchS ==0)
        return true;
    else
        return false;
}

static s32 xp,yp,z;
//----读取触摸点坐标-----------------------------------------------------------
//功能: 读取stmpe811采集到的触摸点坐标，如果有多点，则平均之
//参数: touch_data，采集到的坐标
//返回: 1=触摸笔按下，0=触摸笔提起，
//-----------------------------------------------------------------------------
static ufast_t ReadTouch(struct SingleTouchMsg *touch_data)
{
    atom_low_t atom;
    touch_data->display = NULL;
    atom = Int_LowAtomStart( );
    touch_data->x =xp;
    touch_data->y =yp;
    touch_data->z = z;
    Int_LowAtomEnd(atom);
    return z;

}

//----读取触摸点坐标-----------------------------------------------------------
//功能: 读取stmpe811采集到的触摸点坐标，如果有多点，则平均之
//参数: touch_data，采集到的坐标
//返回: 1=触摸笔按下，0=触摸笔提起，
//-----------------------------------------------------------------------------
static ptu32_t ScanTouch(void)
{
    bool_t trans;
    atom_low_t atom;
    s16 x,y;
    s32 count =  0;
    s16 s_s16gXAdjustLeft,s_s16gXAdjustRight;
    s16 s_s16gYAdjustTop,s_s16gYAdjustBottom;
    while(1)
    {
        trans = Touch_Touched( );
        if(trans)
        {
            if(count++ > 2)
            {
                trans = Touch_ReadXY(&x,&y);
                trans &= Touch_Touched( );
                if(trans)
                {
                	s_s16gXAdjustLeft=s_ptAdjustVaule->XAdjustLeft;
                	s_s16gXAdjustRight=s_ptAdjustVaule->XAdjustRight;
                	s_s16gYAdjustTop=s_ptAdjustVaule->YAdjustTop;
                	s_s16gYAdjustBottom=s_ptAdjustVaule->YAdjustBottom;
                    x = 200 * (s32)(x - s_s16gXAdjustLeft)/(s_s16gXAdjustRight - s_s16gXAdjustLeft)+20;
                    y = 88 * (s32)(y - s_s16gYAdjustTop)/(s_s16gYAdjustBottom - s_s16gYAdjustTop)+20;

                    if( ( ( (xp -x)*(xp-x) + (yp-y)*(yp-y) ) >= 36) && (count >5 ) )
                    {
                    	//如果连续触摸且两 次采样之间移动距离达到6个像素，说明是提笔过程中的误读,不处理
                    }
                    else
                    {
						atom = Int_LowAtomStart( );
						xp = x;
						yp = y;
						z = 1;
						Int_LowAtomEnd(atom);
                    }
                    count = 5;
                }
                else
                    count = 0;
            }
        }
        else
        {
            if(count-- < -2)
            {
                count = -3;
                z = 0;
            }
        }
        Djy_EventDelay(10*mS);
    }
    return 0;
}

static  void draw_cursor(struct GkWinRsc *desktop,int x,int y)
{
    GK_ApiLineto(desktop,x,y-9,x,y+9,CN_COLOR_WHITE,CN_R2_COPYPEN,0); //上
    GK_ApiLineto(desktop,x-9,y,x+9,y,CN_COLOR_WHITE,CN_R2_COPYPEN,0);  //右
    GK_ApiSyncShow(1000*mS);
}

static  void clr_cursor(struct GkWinRsc *desktop,s32 x,s32 y)
{
    GK_ApiLineto(desktop,x,y-9,x,y+9,CN_COLOR_BLACK,CN_R2_COPYPEN,0); //上
    GK_ApiLineto(desktop,x-9,y,x+9,y,CN_COLOR_BLACK,CN_R2_COPYPEN,0);  //右
    GK_ApiSyncShow(1000*mS);

}



void touch_ratio_adjust(struct GkWinRsc *desktop)
{
    u32 loop;
    s32 tempx,tempy;
    u16 maxx=0,minx=65535,maxy=0,miny=65535;
    s16 x[7],y[7];
    bool_t b_AdjustFlag=false;
    struct AdjustValue* pAdjustValue;
    s64 temp=0;
    s64 adjustvalue=0;
    bool_t result=false;
    s16 s_s16gXAdjustLeft,s_s16gXAdjustRight;
    s16 s_s16gYAdjustTop,s_s16gYAdjustBottom;

    pAdjustValue=(struct AdjustValue*)malloc(sizeof(struct AdjustValue));
    if(pAdjustValue==NULL)
    	return;
	tempx = 0;
	tempy = 0;
	GK_ApiFillWin(desktop,CN_COLOR_BLACK,0);

	temp=s_ptAdjustVaule->XAdjustLeft;
	adjustvalue|=temp;
	temp=s_ptAdjustVaule->XAdjustRight;
	temp=temp<<16;
	adjustvalue|=temp;
	temp=s_ptAdjustVaule->YAdjustBottom;
	temp=temp<<32;
	adjustvalue|=temp;
	temp=s_ptAdjustVaule->YAdjustTop;
	temp=temp<<48;
	adjustvalue|=temp;

	//先判断Flash中是否已存储有效的校准值   必须要校验
	if(adjustvalue==0xffffffffffffffff)
	{
		GK_ApiDrawText(desktop,NULL,NULL,40,20,
						"触摸屏矫正 ",11,CN_COLOR_WHITE,CN_R2_COPYPEN,0);
		GK_ApiDrawText(desktop,NULL,NULL,40,40,
						"请准确点击十字交叉点",20,CN_COLOR_WHITE,CN_R2_COPYPEN,0);

		draw_cursor(desktop, 20, 20);
		//    for(loop =10; loop < 128;)
		//    {
		//      GK_ApiLineto(desktop,0,loop,240,loop,CN_COLOR_WHITE,CN_R2_COPYPEN,0); //上
		//      loop +=10;
		//    }
		//    for(loop =10; loop < 240;)
		//    {
		//        GK_ApiLineto(desktop,loop,0,loop,128,CN_COLOR_WHITE,CN_R2_COPYPEN,0); //上
		//      loop+=10;
		//    }
		//    GK_ApiSyncShow(1000*mS);

		while(Touch_Touched() == false);
		Djy_EventDelay(100*mS);

		for(loop = 0;loop < 50; )
		{
			if(Touch_Touched())
			{
				loop++;
				Djy_DelayUs(30000);
			}
		}
		for(loop = 0;loop < 7; )
		{
			if(Touch_Touched())
			{
				if(Touch_ReadXY(&x[loop], &y[loop]))
				{
					if(Touch_Touched())
					{
						tempx +=x[loop];
						tempy +=y[loop];
						if(x[loop] > maxx)
							maxx = x[loop];
						if(x[loop] < minx)
							minx = x[loop];
						if(y[loop] > maxy)
							maxy = y[loop];
						if(y[loop] < miny)
							miny = y[loop];
						loop++;
					}
				}
				Djy_DelayUs(30000);
			}
		}
		tempx -= maxx + minx;
		tempy -= maxy + miny;
		s_s16gXAdjustLeft = tempx / 5;
		s_s16gYAdjustTop = tempy / 5;


		//    printk("x=%d,y=%d\n\r",s_s16gXAdjustLeft,s_s16gYAdjustTop);
		//    s_s16gXAdjustLeft = 0;
		//    s_s16gYAdjustTop = 0;
		//    maxx=0,minx=65535,maxy=0,miny=65535,tempx=0,tempy=0;
		//    Djy_DelayUs(300*mS);
		//    }
		clr_cursor(desktop, 20, 20);
		draw_cursor(desktop, 220, 108);
		while(Touch_Touched() == true);
		Djy_EventDelay(300*mS);
		while(Touch_Touched() == false);
		Djy_EventDelay(100*mS);
		maxx=0;
		minx=65535;
		maxy=0;
		miny=65535;
		tempx = 0;
		tempy = 0;
		for(loop = 0;loop < 50; )
		{
			if(Touch_Touched())
			{
				loop++;
				Djy_DelayUs(30000);
			}
		}
		for(loop = 0;loop < 7; )
		{
			if(Touch_Touched())
			{
				if(Touch_ReadXY(&x[loop], &y[loop]))
				{
					if(Touch_Touched())
					{
						tempx +=x[loop];
						tempy +=y[loop];
						if(x[loop] > maxx)
							maxx = x[loop];
						if(x[loop] < minx)
							minx = x[loop];
						if(y[loop] > maxy)
							maxy = y[loop];
						if(y[loop] < miny)
							miny = y[loop];
						loop++;
					}
				}
				Djy_DelayUs(30000);
			}
		}
		tempx -= maxx +minx;
		tempy -= maxy +miny;
		s_s16gXAdjustRight = tempx / 5;
		s_s16gYAdjustBottom = tempy / 5;

		pAdjustValue->XAdjustLeft=s_s16gXAdjustLeft;
		pAdjustValue->XAdjustRight=s_s16gXAdjustRight;
		pAdjustValue->YAdjustBottom=s_s16gYAdjustBottom;
		pAdjustValue->YAdjustTop=s_s16gYAdjustTop;
		result=Touch_WriteAdjustValue(pAdjustValue);
		if(result)
		{
			GK_ApiDrawText(desktop,NULL,NULL,50,600,"校准成功",
										8,CN_COLOR_WHITE,CN_R2_COPYPEN,0);
			Djy_EventDelay(200*mS);
		}

		clr_cursor(desktop, 220, 108);

	}

}
//----初始化触摸屏模块---------------------------------------------------------
//功能:
//参数: display_dev_name:本触摸屏对应的显示器名(资源名)
//      touch_dev_name:触摸屏设备名.
//返回: 无
//-----------------------------------------------------------------------------
bool_t ModuleInstall_LCM240128Touch(struct GkWinRsc *desktop,const char *touch_dev_name)
{
    static struct SingleTouchPrivate touch_dev;
    u16 evtt_id,event_id;
    s32 Ret;

    PIO_Configure(PenStatus,PIO_LISTSIZE(PenStatus));

    s_tgTouch_Dev.AutoCs = false;
    s_tgTouch_Dev.CharLen = 8;
    s_tgTouch_Dev.Cs = 0;
    s_tgTouch_Dev.Freq = 1000000;
    s_tgTouch_Dev.Mode = SPI_MODE_1;
    s_tgTouch_Dev.ShiftDir = SPI_SHIFT_MSB;

    if(NULL != SPI_DevAdd_s(&s_tgTouch_Dev,SPI_BUS_NAME,touch_dev_name))
    {
        SPI_BusCtrl(&s_tgTouch_Dev,CN_SPI_SET_POLL,0,0);
    }
    else
        printf("安装触摸屏SPI驱动出错\n\r");

extern s32 ModuleInstall_EmbededFlash(const char *ChipName, u32 Flags, u16 Start);
    Ret = ModuleInstall_EmbededFlash("embedded flash", FLASH_BUFFERED, 0);
    if(Ret)
    {
	   printf("install \"embedded flash\" error\r\n");
	   return (-1);
    }

    touch_ratio_adjust(desktop);

    touch_dev.read_touch = ReadTouch;
    touch_dev.touch_loc.display = NULL;
    Touch_InstallDevice(touch_dev_name,&touch_dev);

    evtt_id = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                ScanTouch,NULL,1024,
                                "touch scaning");
    //事件的两个参数暂设为0?如果用shell启动?可用来采集shell命令行参数
    Djy_EventPop(evtt_id,NULL,0,0,0,0);

    return true;
}


