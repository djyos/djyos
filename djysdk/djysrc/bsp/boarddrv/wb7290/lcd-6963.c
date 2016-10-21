// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：LCD.c
// 模块描述: LCD驱动程序，使用了SMC外设
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 07/03.2016
// =============================================================================

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "os.h"
#include "cpu_peri.h"
#include "lcd-6963.h"

//========================================LCD================================================
//========== MMI驱动 =====================
//EBI pins for address bus of LCD
//#define PIN_EBI_ABUS_LCD      {PIO_PC18A_A0,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}

//PIN_LCD_BACKLIGHT
#define PIN_LCD_BACKLED         {PIO_PD23, PIOD,ID_PIOD,PIO_OUTPUT_0,PIO_PULLUP}
//PIN_LCD_RST
//#define PIN_LCD_RST               {1<<0,PIOA,ID_PIOA,PIO_OUTPUT_0,PIO_PULLUP}

#define DBUS_DTAT(x)            (*(volatile unsigned char *)(x))
#define LCD_DATA_ADDR           (EBI_CS0_ADDR)     //修改
#define LCD_COMMAND_ADDR        (EBI_CS0_ADDR+0x01)
#define LCD_COMMAND_MODE        (0x00)
#define LCD_DATA_MODE           (0x01)

//lcd display attribute define (128*64)
#define LCD_DISPLAY_PAGE_NUMBER     8
#define LCD_DISPLAY_COLUMN_NUMBER   64
#define LCD_DISPLAY_AREA_NUMBER     2
#define LCD_DISPLAY_X_NUMBER        (LCD_DISPLAY_AREA_NUMBER*LCD_DISPLAY_COLUMN_NUMBER)
#define LCD_DISPLAY_Y_NUMBER        (LCD_DISPLAY_PAGE_NUMBER*8)

//lcd oprate command
#define LCD_CMD_DISPLAY_OFF     (0xAE)
#define LCD_CMD_DISPLAY_ON      (0xAF)
#define LCD_CMD_SET_LINE(x)     ((x)|( 0x40))
#define LCD_CMD_SET_PAGE(x)     ((x)|( 0xB0))
#define LCD_CMD_SET_COLUMN_MSB(x)   ((x)|( 0x10))
#define LCD_CMD_SET_COLUMN_LSB(x)   ((x)|( 0x00))
#define LCD_CMD_ADC_NORMAL      (0xA0)
#define LCD_CMD_ADC_REVERSE     (0xA1)
#define LCD_CMD_DISPLAY_NORMAL      (0xA6)
#define LCD_CMD_DISPLAY_REVERSE     (0xA7)
#define LCD_CMD_ALL_POINTS_ON       (0xA5)
#define LCD_CMD_ALL_POINTS_OFF      (0xA4)
#define LCD_CMD_BIAS_MODE_ONE       (0xA2)
#define LCD_CMD_BIAS_MODE_TWO       (0xA3)
#define LCD_CMD_R_M_W_MODE      (0xE0)
#define LCD_CMD_END_R_M_W_MODE      (0xEE)
#define LCD_CMD_RESET           (0xE2)
#define LCD_CMD_COM_MODE_NORMAL     (0xC0)
#define LCD_CMD_COM_MODE_REVERSE    (0xC8)
//#define LCD_CMD_BOOSTER_OFF       (0x28)
//#define LCD_CMD_BOOSTER_ON        (0X2C)
#define LCD_CMD_SET_POWER_CONTROL   (0x28)
#define LCD_BOOSTER_BIT_ON      (1<<2)
#define LCD_VOLTAGE_REGULATOR_BIT_ON    (1<<1)
#define LCD_VOLTAGE_FOLLOEWR_BIT_ON (1<<0)
#define LCD_CMD_SET_V0(x)       ((x)|(0x20))
#define LCD_CMD_ELECTORINIC_VOLUME_MODE (0x81)
#define LCD_CMD_BOOSTER_RATIO_MODE  (0xF8)
#define LCD_CMD_NOP         (0xE3)
#define LCD_CMD_TEST            (0xFC)
#define LCD_DISPLAY_MODE_PAGE_COLUMN    (1)
#define LCD_DISPLAY_MODE_PICTURE    (0)


//temp 暂时放在这里

//EBI Data Bus pins
#define PIN_EBI_DBUS            {0xFF,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}
//EBI pins for address bus
//#define PIN_EBI_ABUS          {0x1f<<18,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//注意地址总线
#define PIN_EBI_ABUS0           {1<<18,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//
#define PIN_EBI_ABUS1           {1<<19,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//
#define PIN_EBI_ABUS2           {1<<20,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//
#define PIN_EBI_ABUS3           {1<<21,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//
#define PIN_EBI_ABUS4           {1<<22,PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//

//EBI NCS1 pin
#define PIN_EBI_ENCS1           {1<<15, PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//XCS2 nCS1 pin
//EBI NCS3 pin
#define PIN_EBI_ENCS3           {1<<12, PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//XCS3 nCS3 pin

//EBI NRD pin(PC11)
#define PIN_EBI_DNRD            {1<<11, PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}
//EBI NWE pin(PC8)
#define PIN_EBI_DNWE            {1<<8,  PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}
//EBI NCS0 pin(PC14)
#define PIN_EBI_ENCS0           {1<<14, PIOC,ID_PIOC,PIO_PERIPH_A,PIO_PULLUP}//XCS1 nCS0 pin

static const Pin LCD_Pins[] = {
    PIN_EBI_DBUS,
    PIN_EBI_DNRD,
    PIN_EBI_DNWE,
    PIN_EBI_ABUS0,
    PIN_EBI_ENCS0,
    PIN_LCD_BACKLED,
//  PIN_LCD_RST
};






//===========================================
//===========================================
//===========================================

#define CN_6963_RD_Msk          0x8000
#define CN_6963_WR_Msk          0x4000
#define CN_6963_BL_Msk          0x2000
#define CN_6963_A0_Msk          0x0800
#define CN_6963_CS_Msk          0x0400
#define CN_6963_DATA_Msk        0x00ff

//显示定位数据
#define CN_LCD_XSIZE            ((uint32_t)128)               // 液晶X坐标最大值
#define CN_LCD_YSIZE            ((uint32_t)64)                // 液晶Y坐标最大值
#define CN_LCD_LINE_SIZE        (((uint8_t)CN_LCD_XSIZE)/8)   // 液晶一行的字符内存大小

//显示方式定义: 全图形方式
//6963命令字定义
#define CN_6963_SET_ADDRESS         0x24    //设置操作地址
#define CN_6963_SET_GRAM_ADDRESS    0x42    //设置图形区首地址
#define CN_6963_SET_LINE_BYTE       0x43    //设置每行占用字节数
#define CN_6963_SET_AUTO_WRITE      0xB0    //进入写地址自动增量方式
#define CN_6963_SET_AUTO_READ       0xB1    //进入读地址自动增量方式
#define CN_6963_EXIT_AUTO_WRITE     0xB2    //结束写地址自动增量方式
#define CN_6963_EXIT_AUTO_READ      0xB3    //结束读地址自动增量方式
#define CN_6963_SET_GRAPH_MODE      0x98    //设为图形模式
#define CN_6963_SET_TG_OR           0x81    //设为图形和文本相或模式

//6963状态字定义
#define CN_6963_CMD_READY           0x01
#define CN_6963_DATA_READY          0x02
#define CN_6963_CMD_DATA_READY      0x03
#define CN_6963_AUTO_READ_READY     0x04
#define CN_6963_AUTO_WRITE_READY    0x08

// 从图形文件LG（CYG深瑞）.jpg中转换过来的点阵
// 宽*高:  128*64
const uint8_t g_byIcon_szn[ ] =
{
/*------------------------------------------------------------------------------
  宽×高（像素）: 128×64
  字模格式/大小 : 单色点阵液晶字模，横向取模，字节正序/1024字节
  数据转换日期  : 2012/12/3 10:44:23
------------------------------------------------------------------------------*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x3E,0x0F,0x80,0x0F,0x80,0x0F,0xE0,0x00,0x3F,0xFF,0x8F,0xEE,0x30,0xC0,
0x00,0x00,0xFF,0x0F,0xC0,0x1F,0x80,0x3F,0xF8,0x08,0x3F,0xFF,0x8F,0xEE,0x30,0xC0,
0x00,0x03,0xFF,0x87,0xC0,0x1F,0x00,0xFF,0xFC,0x0E,0x3F,0xFF,0x8F,0xEE,0x30,0xC0,
0x00,0x0F,0xFF,0x83,0xE0,0x3E,0x01,0xFF,0xFE,0x0E,0x3F,0xFF,0x8F,0xEE,0x30,0xC0,
0x00,0x1F,0xFF,0x83,0xE0,0x3E,0x03,0xF0,0x0E,0x0F,0x37,0x39,0x81,0x8F,0xFF,0xC0,
0x00,0x3C,0x0F,0x81,0xF0,0x7C,0x07,0xE0,0x06,0x03,0x37,0x1D,0x81,0x8F,0xFF,0xC0,
0x00,0x70,0x07,0x01,0xF0,0x7C,0x0F,0xC0,0x02,0x01,0x0E,0x1D,0x81,0x80,0x00,0x00,
0x00,0xE0,0x02,0x00,0xF8,0xF8,0x1F,0x80,0x00,0x08,0x0E,0x1E,0x01,0x80,0x00,0x00,
0x01,0xC0,0x00,0x00,0xF8,0xF8,0x1F,0x00,0x00,0x08,0x0E,0x0E,0x01,0x80,0x00,0x00,
0x03,0x80,0x00,0x00,0x7D,0xF0,0x3E,0x00,0x00,0x0F,0x3C,0xE7,0x8F,0xEF,0xFF,0xC0,
0x03,0x00,0x00,0x00,0x7F,0xF0,0x3C,0x00,0x00,0x0F,0x38,0xE7,0x8F,0xEF,0xFF,0xC0,
0x07,0x00,0x00,0x00,0x3F,0xE0,0x7C,0x00,0x00,0x0F,0x38,0xE3,0x8F,0xEF,0xFF,0xC0,
0x07,0x00,0x00,0x00,0x3F,0xE0,0x7C,0x00,0x00,0x07,0x00,0xE1,0x8F,0xEF,0xFF,0xC0,
0x07,0x00,0x00,0x00,0x1F,0xC0,0x7C,0x01,0xFE,0x01,0x00,0xE0,0x01,0x81,0xE0,0x00,
0x07,0x00,0x00,0x00,0x1F,0xC0,0x7C,0x01,0xFE,0x00,0x3F,0xFF,0x81,0x8F,0xFF,0xC0,
0x07,0x00,0x00,0x00,0x1F,0xC0,0x7C,0x00,0xFE,0x07,0x3F,0xFF,0x81,0x8F,0xFF,0xC0,
0x07,0x00,0x00,0x00,0x0F,0x80,0x7C,0x00,0x7E,0x07,0x03,0xF8,0x01,0x8F,0xFF,0xC0,
0x07,0x00,0x00,0x00,0x0F,0x80,0x7C,0x00,0x3E,0x07,0x03,0xF8,0x01,0x8E,0xEC,0xC0,
0x07,0x00,0x00,0x00,0x0F,0x80,0x7C,0x00,0x3E,0x07,0x07,0xF8,0x01,0x8E,0xEC,0xC0,
0x07,0x80,0x00,0x00,0x0F,0x80,0x3C,0x00,0x3E,0x07,0x07,0xF8,0x01,0xAE,0xEC,0xC0,
0x07,0xC0,0x00,0x00,0x0F,0x80,0x3E,0x00,0x3E,0x0E,0x0E,0xFC,0x01,0xEE,0xEC,0xC0,
0x07,0xE0,0x00,0x00,0x0F,0x80,0x1F,0x00,0x3E,0x0E,0x0E,0xFE,0x01,0xEE,0xEC,0xC0,
0x07,0xF0,0x00,0x00,0x0F,0x80,0x1F,0x80,0x3E,0x0E,0x0E,0xFE,0x01,0xEE,0xEC,0xC0,
0x03,0xF8,0x00,0x00,0x0F,0x80,0x0F,0xC0,0x3E,0x0E,0x3E,0xEF,0x01,0xEE,0xEC,0xC0,
0x01,0xFC,0x00,0x00,0x0F,0x80,0x07,0xFF,0xFE,0x1E,0x38,0xE7,0x8F,0x8E,0xEC,0xC0,
0x00,0xFF,0xFF,0x80,0x0F,0x80,0x03,0xFF,0xFE,0x18,0x30,0xE3,0x8F,0x0E,0xED,0xC0,
0x00,0x3F,0xFF,0x00,0x0F,0x80,0x01,0xFF,0xFC,0x18,0x20,0xE1,0x8E,0x0E,0xED,0xC0,
0x00,0x0F,0xFE,0x00,0x0F,0x80,0x00,0x7F,0xF8,0x10,0x20,0xE1,0x8E,0x0E,0xED,0xC0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

//===========================================================================================

//static uint16_t s_u16Contrast = 70;           // 液晶对比度在0~100之间，负压-3.2V

//==============================================================================
// 函数功能: 完成液晶显示的硬件初始化工作，包括设置默认的背景色、前景色等。
// 函数参数: vaddr，显存地址
// 返回值  : 无
// =========================================================================
void Lidd_Init(void)
{
/*  // enable power and setup lcdc.
    EVMOMAPL138_lpscTransition(PSC1, DOMAIN0, LPSC_LCDC, PSC_ENABLE);       //使能外设
    //lcd2-7
    EVMOMAPL138_pinmuxConfig(16, 0xFFFFFF00, 0x22222200);
    //lcd0 1 10-15
    EVMOMAPL138_pinmuxConfig(17, 0xFFFFFFFF, 0x22222222);
    //lcd8 9
    EVMOMAPL138_pinmuxConfig(18, 0x000000FF, 0x00000022);
    //lcd_PCLK  MCLI
    EVMOMAPL138_pinmuxConfig(18, 0x0F000000, 0x02000000);
    //lcd_cs  VSYN  HSYN
    EVMOMAPL138_pinmuxConfig(19, 0x0F0000FF, 0x02000022);

    g_ptLcdcReg->LIDD_CTRL     = 0x00000003|(0x1<<3);
    //g_ptLcdcReg->LIDD_CS0_CONF = (0x05<<27)|(0x05<<21)|(0x3<<17)|(0x05<<12)|(0x05<<6)|(0x3<<2)|0x03;
    g_ptLcdcReg->LIDD_CS0_CONF = (0x10<<27)|(0x1f<<21)|(0x7<<17)
                                |(0x10<<12)|(0x1f<<6)|(0x7<<2)|0x03;
    g_ptLcdcReg->LCD_STAT      = 0x000003ff;     // Clear status bits
    g_ptLcdcReg->LCD_CTRL      = 0x00002000;     // PCLK = 150MHz / 17 = 8.824 MHz, LIDD MODE

*/}

#if 0
uint8_t Lcd_ReadDataPort(void)
{
    uint8_t rtn;

    rtn=DBUS_DTAT(LCD_DATA_ADDR)&0x00000ff;
    Djy_DelayUs(1);
    return rtn;
}
#endif


extern void __lcd_data_write(u8 data);
void Lcd_WriteDataPort(uint8_t u8Data)
{
    Djy_DelayUs(1);
    *(volatile u8*)LCD_DATA_ADDR = u8Data&0x000000ff;
    //__lcd_data_write(u8Data);

}

extern  u8 __lcd_cmd_read(void);
uint8_t Lcd_ReadStatus(void)
{
    uint8_t i;

    Djy_DelayUs(1);
    for(i=0;i<100;i++);

    i= *(volatile u8*)LCD_COMMAND_ADDR;
    //i = __lcd_cmd_read();

    return i; //这里状态返回的是0，直接返回CN_6963_CMD_DATA_READY  0x03
}

extern void __lcd_cmd_write(u8 cmd);
void Lcd_WriteCmdPort(uint8_t u8Data)
{
    Djy_DelayUs(1);

    *(volatile u8*)LCD_COMMAND_ADDR = u8Data;
    //__lcd_cmd_write(u8Data);

}

/*========================================================*/
//* 函数功能：LCD写数据
//* 输入参数：UINT8 byLcdData
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
uint8_t Lcd_WriteData(uint8_t byLcdData)
{
    uint32_t u32Loop;
    for(u32Loop=0;u32Loop<250;u32Loop++)               // out if error
    {
        if ((Lcd_ReadStatus() & CN_6963_CMD_DATA_READY)
                            ==CN_6963_CMD_DATA_READY)   // 6963 busy
        {
            Lcd_WriteDataPort(byLcdData);
            break;
        }
    }
    if(u32Loop >=250)
        return FALSE;
    else
        return TRUE;
}

/*========================================================*/
//* 函数功能：LCD读数据
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：UINT8
/*========================================================*/
uint8_t Lcd_ReadData(void)
{
  return(Lcd_ReadDataPort());
}

/*========================================================*/
//* 函数功能：LCD写命令
//* 输入参数：UINT8 byLcdData
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/

uint8_t Lcd_WriteCmd(uint8_t u8Data0,uint8_t u8Data1,uint8_t u8Cmd)
{
    uint32_t u32Loop;
    uint8_t result = FALSE;
    switch(u8Cmd)
    {
        case CN_6963_SET_ADDRESS:
        case CN_6963_SET_GRAM_ADDRESS:
        case CN_6963_SET_LINE_BYTE:         //以上是3字节命令
        {
            for(u32Loop=0;u32Loop<250;u32Loop++)               // out if error
            {
                if ((Lcd_ReadStatus() & CN_6963_CMD_DATA_READY)
                                    ==CN_6963_CMD_DATA_READY)   // 6963 busy
                {
                    Lcd_WriteDataPort(u8Data0);
                    break;
                }
            }
            if(u32Loop >=250)
            {
                result = FALSE;
                break;
            }
        }
        //若有2字节命令，再此插入case语句
        {
            for(u32Loop=0;u32Loop<250;u32Loop++)               // out if error
            {
                if ((Lcd_ReadStatus() & CN_6963_CMD_DATA_READY)
                                    ==CN_6963_CMD_DATA_READY)   // 6963 busy
                {
                    Lcd_WriteDataPort(u8Data1);
                    break;
                }
            }
            if(u32Loop >=250)
            {
                result = FALSE;
                break;
            }
        }
        default:    //单字节命令
        {
            for(u32Loop=0;u32Loop<250;u32Loop++)               // out if error
            {
                if ((Lcd_ReadStatus() & CN_6963_CMD_DATA_READY)
                                    ==CN_6963_CMD_DATA_READY)   // 6963 busy
                {
                    Lcd_WriteCmdPort(u8Cmd);
                    break;
                }
            }
            if(u32Loop >=250)
            {
                result = FALSE;
                break;
            }
        }
    }
    return result;
}


/*========================================================*/
//* 函数功能：LCD读命令
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：UINT8
/*========================================================*/
uint8_t Lcd_ReadCmd(void)
{
  return(Lcd_ReadStatus());
}

//======== 显示驱动 =============
//以下移植自G系列装置LCD驱动

// 对应度设置，使用DACC
#if 1
static const Pin Lcd_Contrast_Pin[] = {
                    {PIO_PB13,PIOB,ID_PIOB,PIO_OUTPUT_0,PIO_PULLUP}
};

static void DACC_Channel0Init(u16 data)
{
    DACC->DACC_CR = DACC_CR_SWRST;
    DACC->DACC_MR = DACC_MR_PRESCALER(2048);//150M PRE
    DACC->DACC_CHER = DACC_CHER_CH0;
    while( !(DACC->DACC_CHSR & DACC_CHSR_CH0) );
    DACC->DACC_CDR[0] = DACC_CDR_DATA0(data);
}
void Lcd_ContrastSet(uint16_t u16Contrast)//调整液晶对比度
{
    //初始化IO
    PIO_Configure(Lcd_Contrast_Pin,1);
    //初始化DACC,使能时钟,初始化配置
    PMC_EnablePeripheral(ID_DACC);

    DACC_Channel0Init(u16Contrast);
}
#endif
//static const Pin pwm_out_pin[] = {
//      {PIO_PB13, PIOB, ID_PIOB, PIO_OUTPUT_1,    PIO_DEFAULT}
//};
//void Lcd_Contrast(void)
//{
//  PIO_Configure(pwm_out_pin, PIO_LISTSIZE(pwm_out_pin));
//
//  while(1)
//  {
//      Djy_EventDelay(8000);
//      PIO_Set(pwm_out_pin);
//
//      Djy_EventDelay(2000);
//      PIO_Clear(pwm_out_pin);
//  }
//}

//void Lcd_ContrastSet(uint16_t u16Contrast)//调整液晶对比度
//{
////    LcdBkPwm0Init(1000,u16Contrast);//PWM频率1khz
//  Lcd_Contrast();
//  if(u16Contrast>1023)
//  {
//      u16Contrast=1023;
//  }
////    DAC_OUTPUT(u16Contrast);///////---todo
//
//}
 /**/


/*========================================================*/
//* 函数功能：复位LCD
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/


#if 0
void Lcd_ReadReg(uint8_t u8Data0,uint8_t u8Data1,uint8_t u8Cmd)
{
    Lcd_WriteCmdPort(u8Cmd);
    u8Data1 = Lcd_ReadDataPort();
    u8Data0 = Lcd_ReadDataPort();
}
#endif


void Lcd_Rst(void)
{
    Lcd_WriteCmd(0x00,0x00,CN_6963_SET_GRAM_ADDRESS);
    Lcd_WriteCmd(CN_LCD_LINE_SIZE,0x00,CN_6963_SET_LINE_BYTE);
    Lcd_WriteCmd(0,0,CN_6963_SET_TG_OR);
    Lcd_WriteCmd(0,0,CN_6963_SET_GRAPH_MODE);
//    Lcd_ContrastSet(s_u16Contrast);
}


// =========================================================================
// 函数功能：LCD背光控制
// 输入参数：byCtrl 输入参数 0=关闭背光 非0=开背光
// 输出参数：
// 返回值  ：
// =========================================================================
void Lcd_SetBackLight(uint8_t byCtrl)
{
    if (0==byCtrl)
    {
        PIO_Set(&LCD_Pins[5]);
    }
    else
    {
        PIO_Clear(&LCD_Pins[5]);
    }
}

//液晶工作void模式初始化
void Lcd_ModInit(void)
{
    Lcd_WriteCmd(0x00,0x00,CN_6963_SET_GRAM_ADDRESS);
    Lcd_WriteCmd(CN_LCD_LINE_SIZE,0x00,CN_6963_SET_LINE_BYTE);
    Lcd_WriteCmd(0,0,CN_6963_SET_TG_OR);
    Lcd_WriteCmd(0,0,CN_6963_SET_GRAPH_MODE);
//    Lcd_ContrastSet(s_u16Contrast);
}

/**********************************************
写显存 void
left:屏幕?笊辖堑腦坐标,top：屏幕左上角的Y坐标
size:数据量的大小(byte),
srcadd:源数据区的起始地址
每行有320个void点阵
**********************************************/
void Lcd_wrDISPram(uint32_t dwleft,uint32_t dwtop,uint32_t dwsize,uint8_t * pbysrcadd)
{
    uint32_t dwloopi, dwposi;
    uint8_t byposil, byposih;

//    uint8_t data= 0xaa;

    dwposi = dwleft + dwtop * (CN_LCD_XSIZE/8);
    byposih = (uint8_t )(dwposi >> 8);
    byposil = (uint8_t )(dwposi & 0xff);
    Lcd_WriteCmd(byposil, byposih,CN_6963_SET_ADDRESS);
    Lcd_WriteCmd(0,0,CN_6963_SET_AUTO_WRITE);

    for(dwloopi=0;dwloopi < dwsize;dwloopi++)
    {
        Lcd_WriteData(pbysrcadd[dwloopi]);
//      data = Lcd_ReadData();
    }
    Lcd_WriteCmd(0,0,CN_6963_EXIT_AUTO_WRITE);
}


void LCD_WriteByte(uint8_t dat,uint8_t lcd_write_mode)
{
    if( lcd_write_mode)
        DBUS_DTAT(LCD_DATA_ADDR) = dat;
    else
        DBUS_DTAT(LCD_COMMAND_ADDR) = dat;
}

void LCD_Clr(void)
{
    uint8_t i,j;
    for(i=0;i<8;i++)
    {
        LCD_WriteByte(LCD_CMD_SET_PAGE(i),LCD_COMMAND_MODE);        //将页地址共8页
        LCD_WriteByte(LCD_CMD_SET_COLUMN_MSB(0),LCD_COMMAND_MODE);  //列高地址设置.
        LCD_WriteByte(LCD_CMD_SET_COLUMN_LSB(0),LCD_COMMAND_MODE);  //列低地址设置.
        for(j=0;j<128;j++)
        {
            LCD_WriteByte(0,LCD_DATA_MODE);             //数据0
        }
    }
}

void ModuleInstall_LCD(void)
{
    //初始化硬件管脚
//  LCD_GPIO_Init(); //IO模拟方式
    PIO_Configure(LCD_Pins,PIO_LISTSIZE(LCD_Pins));

#if 1

//  #define LCD_D0_D7      (0xFF)
//  #define LCD_RS         (PIO_PC18) //A0
//  #define LCD_RD         (PIO_PC11)
//  #define LCD_WR         (PIO_PC8)
//  #define LCD_NCS        (PIO_PC14) //NCS0
//
//  PMC_EnablePeripheral(ID_PIOC);
//
//
//  /* Configure the LCD Control pins --------------------------------------------*/
//    PMC->PMC_WPMR  = 0x504D4300;                  /* Disable write protect      */
//    PMC->PMC_PCER1 = (1 << ID_PIOC) |             /* Enable clock for PIOC      */
//                     (1 << ID_PIOD)  ;            /* Enable clock for PIOD      */
//
//  /* Configure the Data Bus pins -----------------------------------------------*/
//    PIOC->PIO_PDR    =  (LCD_D0_D7);              /* Enable Peripheral function */
//    PIOC->PIO_IDR    =  (LCD_D0_D7);              /* Disable interrupt          */
//    PIOC->PIO_ABCDSR[0]  &= ~(LCD_D0_D7);         /* Select Peripheral A        */
//    PIOC->PIO_ABCDSR[1]  &= ~(LCD_D0_D7);         /* Select Peripheral A        */
//
//  /* Configure the Control pins ------------------------------------------------*/
//    PIOC->PIO_PDR    =  (LCD_RS);                 /* Enable Peripheral function */
//    PIOC->PIO_IDR    =  (LCD_RS);                 /* Disable interrupt          */
//    PIOC->PIO_ABCDSR[0] &= ~(LCD_RS);             /* Select Peripheral A        */
//    PIOC->PIO_ABCDSR[1] &= ~(LCD_RS);             /* Select Peripheral A        */
//
//    PIOC->PIO_PDR    =  (LCD_RD | LCD_WR);        /* Enable Peripheral function */
//    PIOC->PIO_IDR    =  (LCD_RD | LCD_WR);        /* Disable interrupt          */
//    PIOC->PIO_ABCDSR[0] &= ~(LCD_RD | LCD_WR);    /* Select Peripheral A        */
//    PIOC->PIO_ABCDSR[1] &= ~(LCD_RD | LCD_WR);    /* Select Peripheral A        */
//
//    PIOC->PIO_PDR    =  (LCD_NCS);               /* Enable Peripheral function */
//    PIOC->PIO_IDR    =  (LCD_NCS);               /* Disable interrupt          */
//    PIOC->PIO_ABCDSR[0] &= ~(LCD_NCS);           /* Select Peripheral A        */
//    PIOC->PIO_ABCDSR[1] &= ~(LCD_NCS);           /* Select Peripheral A        */
      ////////////

    PMC_EnablePeripheral(ID_SMC ) ;

    SMC->SMC_CS_NUMBER[0].SMC_SETUP = (0
                    | (SMC_SETUP_NWE_SETUP(3))
                    | (SMC_SETUP_NCS_WR_SETUP(2))
                    | (SMC_SETUP_NRD_SETUP(3))
                    | (SMC_SETUP_NCS_RD_SETUP(2))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_PULSE = (0
                    | (SMC_PULSE_NWE_PULSE(5))
                    | (SMC_PULSE_NCS_WR_PULSE(5))

                    | (SMC_PULSE_NRD_PULSE(10))
                    | (SMC_PULSE_NCS_RD_PULSE(10))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_CYCLE = (0
                    | (SMC_CYCLE_NWE_CYCLE(15))
                    | (SMC_CYCLE_NRD_CYCLE(30))
                      );

    SMC->SMC_CS_NUMBER[0].SMC_MODE = (0
                    | (SMC_MODE_DBW_8_BIT)
                    | (SMC_MODE_EXNW_MODE_DISABLED)
                    | (SMC_MODE_BAT_BYTE_WRITE)
                    | (SMC_MODE_READ_MODE)
                    | (SMC_MODE_WRITE_MODE)
                      );
#endif

    Lcd_Rst();
    Lcd_ModInit();
//  Lcd_ContrastSet(300);


    //memset(g_ucDspBuf, 0, 1024);
    //Lcd_wrDISPram(0,0,1024,&g_ucDspBuf[0][0]);        // 液晶显示CYG log
    //DelayMs(100);
//  while(1)
//  {
        Lcd_wrDISPram(0,0,1024,(uint8_t *)g_byIcon_szn);       // 液晶显示CYG log
//      Djy_EventDelay(500*mS);
//
//      Lcd_wrDISPram(0,0,1024,(uint8_t *)g_byIcon_szn1);       // 液晶显示CYG log
//      Djy_EventDelay(500*mS);
//
//  }
    Lcd_ContrastSet(2500);
}
