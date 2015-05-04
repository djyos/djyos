//----------------------------------------------------
//Copyright (C), 2005-2009,  lst.
//版权所有 (C), 2005-2009,   lst.
//所属模块:IO初始化
//作者：lst
//版本：V1.0.0
//文件描述:IO初始化
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-08-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __ppc1020_gpio_h_
#define __ppc1020_gpio_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
//#define CN_CCSBAR_ADDRESS
//#define CN_GPIO_ADDR   (0x0e00000000 + 0xf000)

//offset
struct ppc_gpio_reg
{
  vu32 GPDIR;        //0x0000
  vu32 GPODR;        //0x0004
  vu32 GPDAT;        //0x0008
  vu32 GPIER;        //0x000C
  vu32 GPIMR;        //0x0010
  vu32 GPICR;        //0x0014
};

//定义IO模式
//输入输出两种模式，输出模式分为开漏和主驱动输出
#define cn_gpio_mode_in          0x00    //输入
#define cn_gpio_mode_out_ad      0x01    //gpio开漏输出
#define cn_gpio_mode_out_od      0x02    //gpio直接驱动输出

//错误IO引脚号
#define error_pin_para                     0xff

//定义IO输出电平
#define GPIO_SET_LOW                    0
#define GPIO_SET_HIGH                   1

//定义IO口
#define GPIO_0                          0
#define GPIO_1                          1
#define GPIO_2                          2
#define GPIO_3                          3
#define GPIO_4                          4
#define GPIO_5                          5
#define GPIO_6                          6
#define GPIO_7                          7
#define GPIO_8                          8
#define GPIO_9                          9
#define GPIO_10                        10
#define GPIO_11                        11
#define GPIO_12                        12
#define GPIO_13                        13
#define GPIO_14                        14
#define GPIO_15                        15

#define GPIO_MAX                       15


void GPIO_CfgPinFunc(u32 pinnum,u32 func_no);
void GPIO_SettoHigh(u32 pinnum);
void GPIO_SettoLow(u32 pinnum);

u32 gpio_get_pin(u32 pinnum);

void gpio_enable_int(u32 pinnum);
void gpio_disable_int(u32 pinnum);

void gpio_clear_int(u32 pinnum);
u32 gpio_get_int(u32 pinnum);

void gpio_reset (void);

#ifdef __cplusplus
}
#endif

#endif /*__ppc1020_gpio_h_*/
