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

#include "stdint.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"
#include "board-config.h"
#include "gkernel.h"
#include <gui/gkernel/gk_display.h>
#include "cpu_peri.h"
#include "stm32f7xx_hal_ltdc.h"
#include "cpu_peri_lcd.h"



static struct DisplayRsc lcd_display;
u16 *pFrameBufferFG;    //前台显示缓冲区.
//u32 u32g_frame_buffer[CN_LCD_XSIZE*CN_LCD_YSIZE];
/*===========================================================================*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_dma2d.h"
//#include "ARGB4444_480x272.h"
/** @addtogroup LTDC_ColorKeying
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PRESSED_FIRST       0x00
#define PRESSED_SECOND      0x01
#define LTDC_LAYER_1        1
#define ARGB4444_COLOR_KEY  0xFFFF
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static LTDC_HandleTypeDef        hltdc_F;
uint8_t ubPressedButton = PRESSED_FIRST;

__IO uint32_t ReloadFlag = 0;


#define DMA2D_timeout   50000


void DMA2D_init(void)
{

    RCC->AHB1ENR|=1<<23;            //使能DM2D时钟

}
//=============将颜色格式转换成DMA2D寄存器的值================//
static u32 Color_FToReg(u32 format)
{

	u32 reg;

	if(format==CN_SYS_PF_RGB565)
		reg=2;

	else if(format==CN_SYS_PF_ERGB4444)
	  reg=4;

	else if(format==CN_SYS_PF_RGB888)
	  reg=1;

	else if(format==CN_SYS_PF_ERGB8888)
	  reg=0;

	else
		return false;

	return reg;

}



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DMA2D_HandleTypeDef Dma2dHandle;







static bool_t LCD_Config(void)
{

    static LTDC_LayerCfgTypeDef      pLayerCfg;

    /* LTDC Initialization -------------------------------------------------------*/
    /* DeInit */
    HAL_LTDC_DeInit(&hltdc_F);

    /* Polarity configuration */
    /* Initialize the horizontal synchronization polarity as active low */
    hltdc_F.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    /* Initialize the vertical synchronization polarity as active low */
    hltdc_F.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    /* Initialize the data enable polarity as active low */
    hltdc_F.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    /* Initialize the pixel clock polarity as input pixel clock */
    hltdc_F.Init.PCPolarity = LTDC_PCPOLARITY_IPC;


    /* The LCD AMPIRE 640x480 is selected */
    /* Timing configuration */
    hltdc_F.Init.HorizontalSync = (AMPIRE640480_HSYNC - 1);
    hltdc_F.Init.VerticalSync = (AMPIRE640480_VSYNC - 1);
    hltdc_F.Init.AccumulatedHBP = (AMPIRE640480_HSYNC + AMPIRE640480_HBP - 1);
    hltdc_F.Init.AccumulatedVBP = (AMPIRE640480_VSYNC + AMPIRE640480_VBP - 1);
    hltdc_F.Init.AccumulatedActiveH = (AMPIRE640480_HEIGHT + AMPIRE640480_VSYNC + AMPIRE640480_VBP - 1);
    hltdc_F.Init.AccumulatedActiveW = (AMPIRE640480_WIDTH + AMPIRE640480_HSYNC + AMPIRE640480_HBP - 1);
    hltdc_F.Init.TotalHeigh = (AMPIRE640480_HEIGHT + AMPIRE640480_VSYNC + AMPIRE640480_VBP + AMPIRE640480_VFP - 1);
    hltdc_F.Init.TotalWidth = (AMPIRE640480_WIDTH + AMPIRE640480_HSYNC + AMPIRE640480_HBP + AMPIRE640480_HFP - 1);


    /* Configure R,G,B component values for LCD background color */
    hltdc_F.Init.Backcolor.Blue = 0;
    hltdc_F.Init.Backcolor.Green = 0;
    hltdc_F.Init.Backcolor.Red = 0;

    hltdc_F.Instance = LTDC;

    /* Layer1 Configuration ------------------------------------------------------*/

    pLayerCfg.WindowX0 = 0;
    pLayerCfg.WindowX1 = CN_LCD_XSIZE;
    pLayerCfg.WindowY0 = 0;
    pLayerCfg.WindowY1 = CN_LCD_YSIZE;

    /* Pixel Format configuration*/
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB4444;

    /* Start Address configuration : frame buffer is located at FLASH memory */
    pLayerCfg.FBStartAdress =  ( uint32_t)pFrameBufferFG;

    /* Alpha constant (255 totally opaque) */
    pLayerCfg.Alpha = 255;

    /* Default Color configuration (configure A,R,G,B component values) */
    pLayerCfg.Alpha0 = 0;
    pLayerCfg.Backcolor.Blue = 0;
    pLayerCfg.Backcolor.Green = 0;
    pLayerCfg.Backcolor.Red = 0;

    /* Configure blending factors */
    pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;

    /* Configure the number of lines and number of pixels per line */
    pLayerCfg.ImageWidth  = CN_LCD_XSIZE;
    pLayerCfg.ImageHeight = CN_LCD_YSIZE;

    /* Configure the LTDC */
    if(HAL_LTDC_Init(&hltdc_F) != HAL_OK)
        return false;
    /* Configure the Layer 1 */
    if(HAL_LTDC_ConfigLayer(&hltdc_F, &pLayerCfg, 1) != HAL_OK)
        return false;
    return true;
}

//----------------------------------------------------------------
//功能: 开关LTDC
//参数:0/1
//----------------------------------------------------------------------------
void LTDC_Switch(u8 sw)
{
    if(sw==1)
        LTDC->GCR|=1<<0;    //打开LTDC
    else
        LTDC->GCR&=~(1<<0); //关闭LTDC
}
//----------------------------------------------------------------
//功能: 开关指定层Layer
//参数:  层layer 1/2
//      开关sw 1/0
//返回值:ture/false
//----------------------------------------------------------------------------
bool_t LTDC_Layer_Switch(u8 layerx,u8 sw)
{
    switch (sw)
    {
        case 1:
            if(layerx==0)
                LTDC_Layer1->CR&=~(1<<0);   //关闭层1
            else
                LTDC_Layer2->CR&=~(1<<0);   //关闭层2
            break;
        case 2:
            if(layerx==0)
                LTDC_Layer1->CR|=1<<0;      //开启层1
            else
                LTDC_Layer2->CR|=1<<0;      //开启层2

            break;
        default:   return false;
    }
    LTDC->SRCR|=1<<0; //重新加载配置
    return true;
}
//----------------------------------------------------------------
//功能: LTDC时钟设置。
//参数: pllsaidivr:LCD时钟分频系数,取值范围:0~3,对应分频2^(pllsaidivr+1)
//返回值:0,成功;1,失败。
//备注:CLK=(pllsain/pllsair)/(2^pllsaidivr)
//----------------------------------------------------------------------------
bool_t LTDC_Clk_Set(u32 pllsain,u32 pllsair,u32 pllsaidivr)
{
    u16 timeout=0;
    RCC->CR&=~(1<<28);                  //关闭SAI时钟
    while(((RCC->CR&(1<<29)))&&(timeout<0X2000))
        timeout++;//等待SAI时钟失锁
    if(timeout==0X1FFF)
        return false;           //LTDC时钟关闭失败
    else
    {
        RCC->PLLSAICFGR |=(pllsain<<6)|(pllsair<<28);   //设置LTDC的倍频和分频
        RCC->DCKCFGR1&=~(3<<16);        //清除原来的设置.
        RCC->DCKCFGR1|=pllsaidivr<<16;  //设置fdclk分频
        RCC->CR|=1<<28;                 //开启SAI时钟
        while(((RCC->CR&(1<<29))==0)&&(timeout<0X1FFF))
            timeout++;//等待SAI时钟锁定
        if(timeout==0X1FFF)
            return false;
    }
    return true;
}


//----初始化lcd硬件------------------------------------------------------------
//功能: 初始化lcd硬件。
//参数: 无
//返回: 无
//备注: 本函数是移植关键函数
//----------------------------------------------------------------------------
static bool_t __lcd_hard_init(u16 *lcd_buffer_addr)
{

    RCC->APB2ENR |=(1<<26);//使能ltd外设

    GPIO_PowerOn(GPIO_A);//背光控制引脚
    GPIO_PowerOn(GPIO_I);
    GPIO_PowerOn(GPIO_J);
    GPIO_PowerOn(GPIO_K);
    LTDC_Clk_Set(251,5,0);
    //控制背光的引脚
    GPIO_CfgPinFunc(GPIO_A,PIN8,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_NONE);
    GPIO_CfgPinFunc(GPIO_I,
            PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_NONE);
    GPIO_CfgPinFunc(GPIO_J,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7|PIN8|PIN9|PIN10|
            PIN11|PIN12|PIN13|PIN14|PIN15,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_NONE);
    GPIO_CfgPinFunc(GPIO_K,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7,
            GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_NONE);

    GPIO_AFSet(GPIO_I,
            PIN12|PIN13|PIN14|PIN15,AF14);
    GPIO_AFSet(GPIO_J,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7|PIN8|PIN9|PIN10|
            PIN11|PIN12|PIN13|PIN14|PIN15,AF14);
    GPIO_AFSet(GPIO_K,
            PIN0|PIN1|PIN2|PIN3|PIN4|PIN5|PIN6|PIN7,AF14);

    if (LCD_Config()==false)
        return false;
#if(0)

	u32 timeout=0;
	//清屏
	RCC->AHB1ENR|=1<<23;            //使能DM2D时钟
	DMA2D->CR&=~(1<<0);             //先停止DMA2D
	DMA2D->CR=3<<16;                //寄存器到存储器模式
	DMA2D->OPFCCR=Color_FToReg(CN_LCD_PIXEL_FORMAT);    //设置输出颜色格式
	DMA2D->OOR  = 0;    //设置行偏移
	DMA2D->OMAR=(u32)lcd_buffer_addr;               //输出存储器地址
	DMA2D->NLR  = (CN_LCD_YSIZE|(CN_LCD_XSIZE<<16));    //设定行数寄存器
	DMA2D->OCOLR=0xff0f;                //设定输出颜色寄存器
	DMA2D->CR|=1<<0;                //启动DMA2D
	while((DMA2D->ISR&(1<<1))==0)   //等待传输完成
	{
	  timeout+=500;
	  Djy_EventDelay(500);
	  if(timeout>DMA2D_timeout)
		  break;  //超时退出
	}
	DMA2D->IFCR|=1<<1;              //清除传输完成标志


//传输数据超出显存区不需显示

	u16  XSIZE=480;
	u16  YSIZE=270;
	u32 outOffset=640-480;

	RCC->AHB1ENR|=1<<23;                 //使能DM2D时钟
	DMA2D->CR&=~(1<<0);                  //先停止DMA2D
	DMA2D->CR &=~(3<<16);                //存储器到存储器模式
	DMA2D->OPFCCR=Color_FToReg(CN_LCD_PIXEL_FORMAT);    //设置输出颜色格式
	DMA2D->OPFCCR |=(0xff<<24);    //Alpha 值 (Alpha value)
	DMA2D->OOR  = outOffset;    //设置行偏移
	DMA2D->FGPFCCR |=(0xff<<24);//前景层Alpha值
	DMA2D->FGPFCCR |=Color_FToReg(CN_LCD_PIXEL_FORMAT);//前景层图像的颜色格式
	DMA2D->OMAR=(u32)lcd_buffer_addr;       //输出存储器地址
	DMA2D->NLR  = (YSIZE|(XSIZE<<16));    //设定行数寄存器
	DMA2D->FGMAR= (uint32_t)&ARGB4444_480x272;  //设定输入地址
	DMA2D->CR|=1<<0;                //启动DMA2D
	while((DMA2D->ISR&(1<<1))==0)   //等待传输完成
	{
	      timeout+=500;
	      Djy_EventDelay(500);
	      if(timeout>DMA2D_timeout)
	    	  break;  //超时退出
	}
	DMA2D->IFCR|=1<<1;              //清除传输完成标志
#endif

//  /*##-1- DMA2D configuration ################################################*/
//  DMA2D_Config(DMA2D_M2M,outOffset,0,DMA2D_INPUT_ARGB4444);
//
//  /*##-2- LCD Configuration ##################################################*/
//    if(HAL_DMA2D_Start_IT(&Dma2dHandle,
//                          (uint32_t)&ARGB4444_480x272, /* Source buffer in format ARGB4444 and size 150x150      */
//                          (uint32_t)lcd_buffer_addr,    /* Destination buffer in format ARGB4444 and size 150x150 */
//                          XSIZE, /* width in pixels  */
//                          YSIZE) /* height in pixels */
//       != HAL_OK)
//    {
//      /* Initialization Error */
//          return false;
//    }
//
//   DMA2D_Config(DMA2D_R2M,addr,0,0);
//////    /*##-3- Start DMA2D transfer ###############################################*/
//  if(HAL_DMA2D_Start_IT(&Dma2dHandle,
//                      0xF0FF, /* Color value in Register to Memory DMA2D mode */
//                      (uint32_t)lcd_buffer_addr,  /* DMA2D output buffer */
//                       XSIZE, /* width of buffer in pixels */
//                       YSIZE) /* height of buffer in lines */
//   != HAL_OK)
//  {
//      return false;
//
//  }

     return true;



}
//------------------------------------------------------------------
//功能: 背光开关
//参数: true/false
//返回:
//-----------------------------------------------------------------------------
static  void __lcd_backlight_onoff(bool_t onoff)
{
  if(onoff==true)
      GPIO_SettoLow(GPIO_A,PIN8);
  else
      GPIO_SettoHigh(GPIO_A,PIN8);

}

//----控制显示器---------------------------------------------------------------
//功能: 这是由driver提供商编写的，用于设置显示器各种参数的函数，属于应用程序的
//      范畴，功能由设备厂商定义。
//参数: disp，显示器指针
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_disp_ctrl(struct DisplayRsc *disp)
{
    return true;
}


//----位图中画像素-------------------------------------------------------------
//功能: 在位图中画一个像素，只有在bitmap的像素格式为cn_custom_pf时，才需要绘制。
//      如果显卡不打算支持自定义格式，本函数直接返回。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分
//      x、y，坐标
//      color，绘图用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_bm(struct RectBitmap *bitmap,
                     s32 x,s32 y,u32 color,u32 r2_code)
{
    return false;
}
//----位图中画线---------------------------------------------------------------
//功能: 在位图中画一条直线，不绘制(x2,y2)点，只绘制在limit限定的区域内的部分。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----位图中画线(含端点)-------------------------------------------------------
//功能: 在位图中画一条直线，只绘制在limit限定的区域内的部分。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm_ie(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----位图中填充矩形-----------------------------------------------------------
//功能: 把位图中的矩形用color颜色填充。
//参数: bitmap，目标位图
//    dst_rect，待填充的矩形
//    color，填充颜色，cn_sys_pf_e8r8g8b8格式
//返回: true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_bm(struct RectBitmap *dst_bitmap,
                          struct Rectangle *Target,
                          struct Rectangle *Focus,
                          u32 Color0,u32 Color1,u32 Mode)
{

    u16 pixel;
    u16 *dst;
    s32 height,width;
    u32 timeout;
    if(Mode != CN_FILLRECT_MODE_N)
        return false;
//todo DMA2D支持不同颜色格式传递待添加
    if(dst_bitmap->PixelFormat != CN_SYS_PF_ERGB4444)
        return false;

    pixel = GK_ConvertRGB24ToPF(CN_SYS_PF_ERGB4444,Color0);

    dst = (u16*)((u32)dst_bitmap->bm_bits
                      + Focus->top * dst_bitmap->linebytes);
    dst += Focus->left;

    width=Focus->right-Focus->left;

    height=Focus->top-Focus->bottom;

    RCC->AHB1ENR|=1<<23;            //使能DM2D时钟
    DMA2D->CR&=~(1<<0);             //先停止DMA2D
    DMA2D->CR=3<<16;                //寄存器到存储器模式
    DMA2D->OPFCCR=Color_FToReg(CN_LCD_PIXEL_FORMAT);  //设置输出颜色格式
    DMA2D->OOR  = lcd_display.width-width;  //设置行偏移
    DMA2D->OMAR=(u32)dst;               //输出存储器地址
    DMA2D->NLR  = height|(width<<16);   //设定行数寄存器
    DMA2D->OCOLR=pixel;                //设定输出颜色寄存器
    DMA2D->CR|=1<<0;                //启动DMA2D
    timeout =0;
    while((DMA2D->ISR&(1<<1))==0)   //等待传输完成
    {
        timeout+=500;
        Djy_EventDelay(500);
        if(timeout>DMA2D_timeout)break;  //超时退出
    }
    DMA2D->IFCR|=1<<1;              //清除传输完成标志
    if(timeout<DMA2D_timeout)
        return true;
    return false;
}
//--------------bitmap中位块传送-----------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，两个位图的像素格式
//      源和目的位图可以相同，
//参数:  dst_bitmap，目标位图
//      dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc、ysrc，源位图中被传送的区域左上角坐标
//      pat，pattern指针
//      bitmsk，rop4操作需要的掩码
//      rop_code，三元光栅操作码
//      transparentcolor，透明色
//返回: true=成功,false=失败
//-----------------------------------------------------------------------------

bool_t __lcd_blt_bm_to_bm(struct RectBitmap *dst_bitmap,
						  struct Rectangle *DstRect,
						  struct RectBitmap *src_bitmap,
						  struct Rectangle *SrcRect,
						  struct RopGroup RopCode,u32 transparentcolor)
{
    u16 *src_offset,*dst_offset;    //源位图点阵缓冲区可能不对齐!!!
    u32 timeout;
    u16 *outaddr,y;
    struct RopGroup Rop = { 0, 0, 0, CN_R2_COPYPEN, 0, 0, 0  };

    if((dst_bitmap->PixelFormat != CN_LCD_PIXEL_FORMAT)
             ||(src_bitmap->PixelFormat != CN_LCD_PIXEL_FORMAT)
             ||(memcmp(&RopCode, &Rop ,sizeof(struct RopGroup))!=0))
    {
        return false;
    }
    else
    {
        if((ptu32_t)src_bitmap->bm_bits & 1)    //源位图缓冲区非对齐模型
        {
            return false;
        }
        else
        {
            dst_offset = (u16*)((ptu32_t)dst_bitmap->bm_bits
                                      + DstRect->top * dst_bitmap->linebytes);
            dst_offset += DstRect->left;

            src_offset = (u16*)((ptu32_t)src_bitmap->bm_bits
                                      + SrcRect->top * src_bitmap->linebytes);
            src_offset += SrcRect->left;



            y=(CN_LCD_XSIZE*DstRect->top)+DstRect->left;
            outaddr =&pFrameBufferFG[y];

            RCC->AHB1ENR|=1<<23;            //使能DM2D时钟
            DMA2D->CR&=~(1<<0);             //先停止DMA2D
            DMA2D->CR=0<<16;                //存储器到存储器模式
            DMA2D->FGPFCCR=Color_FToReg(CN_LCD_PIXEL_FORMAT); //设置颜色格式
            DMA2D->OPFCCR |=((Rop.SrcAlpha)<<24);    //Alpha 值 (Alpha value)
            DMA2D->FGOR=0;                  //前景层行偏移为0
            DMA2D->FGPFCCR |=((Rop.DstAlpha)<<24);     //前景层Alpha值
            DMA2D->OOR=(dst_bitmap->linebytes-(DstRect->right-DstRect->left));//设置行偏移
            DMA2D->FGMAR=(u32)src_offset;   //源地址
            DMA2D->OMAR=(u32)outaddr;  //输出存储器地址
            DMA2D->NLR=(DstRect->bottom-DstRect->top)|
                        ((DstRect->right-DstRect->left)<<16);   //设定行数寄存器
            DMA2D->CR|=1<<0;                //启动DMA2D
            timeout=0;
            while((DMA2D->ISR&(1<<1))==0)   //等待传输完成
            {
                timeout++;
                if(timeout>DMA2D_timeout)break;  //超时退出
            }
            DMA2D->IFCR|=1<<1;              //清除传输完成标志
            if(timeout>DMA2D_timeout)
                return false;
        }
    }
    return true;
}

//----screen中画像素-----------------------------------------------------------
//功能: 在screen中画一个像素.
//参数: x、y，像素坐标
//      color，颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=没有绘制，或无需绘制
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_screen(s32 x,s32 y,u32 color,u32 rop2_code)
{

	u32 dest;
    u32 byteoffset;
    byteoffset = (y*CN_LCD_XSIZE + x);


    color = GK_ConvertRGB24ToPF(CN_LCD_PIXEL_FORMAT,color);

    dest = (u32)pFrameBufferFG[byteoffset];
    dest = GK_BlendRop2(dest,color,rop2_code);
    pFrameBufferFG[byteoffset] = dest;



    return true;
}

//在screen中画一个像素，有frame buffer的情况下，正常显示gui不会调用这个函数，
//如果窗口direct_screen==true，则可能调用本函数，无论是否有frame buffer，
//driver都必须提供并且必须实现本函数
//镜像显示器必须实现本函数的功能,不能直接返回false
//参数:
// PixelGroup: 像素坐标数组指针.
// color: 颜色值,CN_SYS_PF_ERGB8888格式
// n: 坐标点的数量
// r2_code: 显示效果,详见gkernel.h中的CN_R2_BLACK族常数定义
bool_t __lcd_set_pixel_group_screen(struct PointCdn *PixelGroup,u32 color,u32 n, u32 r2_code)
{
    u32 dest,i;
    u32 offset;
    color = GK_ConvertRGB24ToPF(CN_LCD_PIXEL_FORMAT,color);

    for(i=0;i<n;i++)
    {
        offset = PixelGroup[i].y*CN_LCD_XSIZE + PixelGroup[i].x;
        dest = (u32)pFrameBufferFG[offset];
        dest = GK_BlendRop2(dest,color,r2_code);

        pFrameBufferFG[offset] = dest;
    }


    return true;
}

//----screen中画线---------------------------------------------------------------
//功能: 在screen中画一条任意直线，不含端点，只绘制在limit限定的区域内的部分。如
//      不支持画线的硬件加速，直接返回false。
//参数: limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen(struct Rectangle *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{

    return false;
}
//----screen中画线(含端点)------------------------------------------------------
//功能: 在screen中画一条直线，只绘制在limit限定的区域内的部分。
//参数: limit，限制矩形，只绘制在该矩形内部的部分
//      x1、y1、x2、y2，起点终点坐标
//      color，绘图用的颜色，cn_sys_pf_e8r8g8b8格式
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen_ie(struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{

    return false;
}
//----screen中填充矩形-----------------------------------------------------------
//功能: 把screen中的矩形用color颜色填充，color的格式是cn_sys_pf_e8r8g8b8。
//参数: dst_rect，待填充的矩形
//      color，填充颜色
//返回: true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_screen(struct Rectangle *Target,
                              struct Rectangle *Focus,
                              u32 Color0,u32 Color1,u32 Mode)
{
    u32 y;
    u32 pen,timeout;
    u16 *p;
    u32 height,width;


    if(Mode != CN_FILLRECT_MODE_N)
        return false;
    pen = GK_ConvertRGB24ToPF(CN_LCD_PIXEL_FORMAT,Color0);

    width=Focus->right-Focus->left;

    height=Focus->bottom-Focus->top;

    y=(CN_LCD_XSIZE*Focus->top)+Focus->left;

    p =&pFrameBufferFG[y];

    RCC->AHB1ENR|=1<<23;            //使能DM2D时钟
    DMA2D->CR&=~(1<<0);             //先停止DMA2D
    DMA2D->CR=3<<16;                //寄存器到存储器模式
    DMA2D->OPFCCR=DMA2D_OUTPUT_ARGB4444;  //设置输出颜色格式
    DMA2D->OOR  = lcd_display.width-width;  //设置行偏移
    DMA2D->OMAR=(u32)p;             //输出存储器地址
    DMA2D->NLR  = height|(width<<16);   //设定行数寄存器
    DMA2D->OCOLR=pen;               //设定输出颜色寄存器
    DMA2D->CR|=1<<0;                //启动DMA2D
    timeout =0;
    while((DMA2D->ISR&(1<<1))==0)   //等待传输完成
    {
        timeout+=500;
        Djy_EventDelay(500);
        if(timeout > DMA2D_timeout)
            break;  //超时退出
    }
    DMA2D->IFCR|=1<<1;              //清除传输完成标志
    if(timeout< DMA2D_timeout)
        return true;
    return false;

}

//----bitmap到screen位块传送---------------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，
//      源和目的位图可以相同。
//参数: dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc、ysrc，源位图中被传送的区域左上角坐标(以源位图左上角坐标为原点)
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_bm_to_screen(struct Rectangle *dst_rect,
                struct RectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
    u32 y;
    u32 timeout;
    u16 *p;
	u32 PixelFormat;
    if(src_bitmap->PixelFormat!=CN_LCD_PIXEL_FORMAT)
    {   //如果源位图与LCD格式不相同
    	return false;
    }

    PixelFormat=CN_LCD_PIXEL_FORMAT;


    y=(CN_LCD_XSIZE*dst_rect->top)+dst_rect->left;

    p =&pFrameBufferFG[y];

    RCC->AHB1ENR|=1<<23;            //使能DM2D时钟
    DMA2D->CR&=~(1<<0);             //先停止DMA2D
    DMA2D->CR=0<<16;                //存储器到存储器模式
    DMA2D->FGPFCCR=PixelFormat; //设置颜色格式
    DMA2D->FGOR=0;                  //前景层行偏移为0
    DMA2D->OOR= lcd_display.width-src_bitmap->width; ;//设置行偏移
    DMA2D->FGMAR=(u32)src_bitmap->bm_bits;      //源地址
    DMA2D->OMAR=(u32)p;        //输出存储器地址
    DMA2D->NLR=((src_bitmap->height)|(src_bitmap->width)<<16);   //设定行数寄存器
    DMA2D->CR|=1<<0;                //启动DMA2D

    timeout=0;
    while((DMA2D->ISR&(1<<1))==0)   //等待传输完成
    {
        timeout++;
        if(timeout>DMA2D_timeout)break;  //超时退出
    }
    DMA2D->IFCR|=1<<1;              //清除传输完成标志
    if(timeout>DMA2D_timeout)
        return false;
    return true;
}
//----从screen中取像素---------------------------------------------------------
//功能: 从screen中取一像素，并转换成cn_sys_pf_e8r8g8b8或cn_sys_pf_a8r8g8b8格式。
//参数: x、y，坐标
//返回: 像素颜色值
//-----------------------------------------------------------------------------
u32 __lcd_get_pixel_screen(s32 x,s32 y)
{
    u32 e,r,g,b,color;

    color = (u32)pFrameBufferFG[y*CN_LCD_XSIZE+x];
    e=(color>>12) & 0xf;
    r = (color>>8) &0xf;
    g = (color>>4) & 0xf;
    b = color & 0xf;
    return ((e<<28)|(r<<20) | (g<<12) | (b<<4));
}

//----读取bitmap中矩形块-------------------------------------------------------
//功能: 把一个位图中的矩形读取到另一个位图中。
//参数: rect，欲读取的矩形
//      dest，保存矩形的位图，其长宽必须与rect相同
//返回: true=成功读取，false=失败，原因可能是提供了不支持的像素格式
//-----------------------------------------------------------------------------
bool_t __lcd_get_rect_screen(struct Rectangle *rect,struct RectBitmap *dest)
{
    return false;
}


//----初始化lcd设备------------------------------------------------------------
//功能: 初始化lcd设备。
//参数: 无
//返回: 显示器资源指针
//-----------------------------------------------------------------------------

struct DisplayRsc* ModuleInstall_LCD(const char *DisplayName,const char* HeapName)
{
    struct HeapCB *heap;



    heap =M_FindHeap(HeapName);
    if(heap==NULL)
        return NULL;

    pFrameBufferFG =M_MallocHeap(CN_LCD_XSIZE*CN_LCD_YSIZE*2,heap,0);
    DMA2D_init();
    __lcd_hard_init(pFrameBufferFG);
    __lcd_backlight_onoff(false);

    lcd_display.frame_buffer =NULL;
    lcd_display.framebuf_direct = true;

    lcd_display.xmm = 0;
    lcd_display.ymm = 0;
    lcd_display.width = CN_LCD_XSIZE;
    lcd_display.height = CN_LCD_YSIZE;
    lcd_display.pixel_format = CN_LCD_PIXEL_FORMAT;


    lcd_display.draw.SetPixelToBitmap = __lcd_set_pixel_bm;
    lcd_display.draw.FillRectToBitmap = __lcd_fill_rect_bm;
    lcd_display.draw.LineToBitmap = __lcd_line_bm;
    lcd_display.draw.LineToBitmapIe = __lcd_line_bm_ie;
    lcd_display.draw.BltBitmapToBitmap = __lcd_blt_bm_to_bm;
    lcd_display.draw.SetPixelToScreen = __lcd_set_pixel_screen;
    lcd_display.draw.SetPixelGroupToScreen = __lcd_set_pixel_group_screen;
    lcd_display.draw.LineToScreen = __lcd_line_screen;
    lcd_display.draw.LineToScreenIe = __lcd_line_screen_ie;
    lcd_display.draw.FillRectToScreen = __lcd_fill_rect_screen;
    lcd_display.draw.CopyBitmapToScreen = __lcd_bm_to_screen;
    lcd_display.draw.GetPixelFromScreen = __lcd_get_pixel_screen;
    lcd_display.draw.GetRectFromScreen = __lcd_get_rect_screen;

    lcd_display.DisplayHeap =  heap;
    lcd_display.disp_ctrl = __lcd_disp_ctrl;


    if(GK_InstallDisplay(&lcd_display,DisplayName))
    {
        __lcd_backlight_onoff(true);

        return &lcd_display;
    }
    return NULL;

}










