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
//所属模块:IO初始化
//作者：lst
//版本：V1.0.0
//文件描述:IO初始化
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-03-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __cpu_peri_gpio_h_
#define __cpu_peri_gpio_h_

#ifdef __cplusplus
extern "C" {
#endif

//定义IO模式
//输入模式不区分是外设输入还是gpio输入
//作为片内外设输出时，多个外设连接到一个引脚上的，只能使能一个外设，或者用
//remap功能把它们映射到别的引脚上。
#define CN_GPIO_MODE_IN_ANALOG          0x00    //模拟输入
#define CN_GPIO_MODE_IN_FLOATING        0x04    //浮空输入
#define CN_GPIO_MODE_IN_PULLDOWN        0x08    //下拉输入
#define CN_GPIO_MODE_IN_PULLUP          0x18    //上拉输入

#define CN_GPIO_MODE_GPIO_OUT_OD_10Mhz  0x05    //gpio开漏输出，速度10Mhz
#define CN_GPIO_MODE_GPIO_OUT_OD_2Mhz   0x06    //gpio开漏输出，速度2Mhz
#define CN_GPIO_MODE_GPIO_OUT_OD_50Mhz  0x07    //gpio开漏输出，速度50Mhz
#define CN_GPIO_MODE_GPIO_OUT_PP_10Mhz  0x01    //gpio推挽输出，速度10Mhz
#define CN_GPIO_MODE_GPIO_OUT_PP_2Mhz   0x02    //gpio推挽输出，速度2Mhz
#define CN_GPIO_MODE_GPIO_OUT_PP_50Mhz  0x03    //gpio推挽输出，速度50Mhz

#define CN_GPIO_MODE_PERI_OUT_OD_10Mhz  0x0d    //片内外设开漏输出，速度10Mhz
#define CN_GPIO_MODE_PERI_OUT_OD_2Mhz   0x0e    //片内外设开漏输出，速度2Mhz
#define CN_GPIO_MODE_PERI_OUT_OD_50Mhz  0x0f    //片内外设开漏输出，速度50Mhz
#define CN_GPIO_MODE_PERI_OUT_PP_10Mhz  0x09    //片内外设推挽输出，速度10Mhz
#define CN_GPIO_MODE_PERI_OUT_PP_2Mhz   0x0a    //片内外设推挽输出，速度2Mhz
#define CN_GPIO_MODE_PERI_OUT_PP_50Mhz  0x0b    //片内外设推挽输出，速度50Mhz

#define CN_GPIO_A   0
#define CN_GPIO_B   1
#define CN_GPIO_C   2
#define CN_GPIO_D   3
#define CN_GPIO_E   4
#define CN_GPIO_F   5
#define CN_GPIO_G   6

//gpioa at 0x40010800;
//gpiob at 0x40010c00;
//gpioc at 0x40011000;
//gpiod at 0x40011400;
//gpioe at 0x40011800;
//gpiof at 0x40011c00;
//gpiog at 0x40012000;

struct tagGPIO_StReg
{
  vu32 CRL;
  vu32 CRH;
  vu32 IDR;
  vu32 ODR;
  vu32 BSRR;
  vu32 BRR;
  vu32 LCKR;
};

//afio位域定义，bo_为位偏移，1位的用位带地址，bb_前缀，多位用掩码，bm_前缀
#define bb_afio_mapr_base           (0x42000000 + 0x10000*32)        //位带基址
#define bb_afio_mapr_spi1           (*(vu32*)(bb_afio_mapr_base+4*0))
#define bb_afio_mapr_i2c1           (*(vu32*)(bb_afio_mapr_base+4*1))
#define bb_afio_mapr_usart1         (*(vu32*)(bb_afio_mapr_base+4*2))
#define bb_afio_mapr_usart2         (*(vu32*)(bb_afio_mapr_base+4*3))
#define bm_afio_mapr_usart3         0x00000030
#define bm_afio_mapr_tim1           0x000000c0
#define bm_afio_mapr_tim2           0x00000300
#define bm_afio_mapr_tim3           0x00000c00
#define bb_afio_mapr_tim4           (*(vu32*)(bb_afio_mapr_base+4*12))
#define bm_afio_mapr_can            0x00006000
#define bb_afio_mapr_pd01           (*(vu32*)(bb_afio_mapr_base+4*15))
#define bb_afio_mapr_tim5ch4        (*(vu32*)(bb_afio_mapr_base+4*16))
#define bb_afio_mapr_adc1_etrginj   (*(vu32*)(bb_afio_mapr_base+4*17))
#define bb_afio_mapr_adc1_etrgreg   (*(vu32*)(bb_afio_mapr_base+4*18))
#define bb_afio_mapr_adc2_etrginj   (*(vu32*)(bb_afio_mapr_base+4*19))
#define bb_afio_mapr_adc2_etrgreg   (*(vu32*)(bb_afio_mapr_base+4*20))
#define bm_afio_mapr_swj_cfg        0x07000000
#define bo_afio_mapr_spi1           0
#define bo_afio_mapr_i2c1           1
#define bo_afio_mapr_usart1         2
#define bo_afio_mapr_usart2         3
#define bo_afio_mapr_usart3         4
#define bo_afio_mapr_tim1           6
#define bo_afio_mapr_tim2           8
#define bo_afio_mapr_tim3           10
#define bo_afio_mapr_tim4           12
#define bo_afio_mapr_can            13
#define bo_afio_mapr_pd01           15
#define bo_afio_mapr_tim5ch4        16
#define bo_afio_mapr_adc1_etrginj   17
#define bo_afio_mapr_adc1_etrgreg   18
#define bo_afio_mapr_adc2_etrginj   19
#define bo_afio_mapr_adc2_etrgreg   20
#define bo_afio_mapr_swj_cfg        24
struct tagAFIO_StReg     //at 0x40010000;
{
  vu32 EVCR;
  vu32 MAPR;
  vu32 EXTICR[4];
} ;

extern struct tagGPIO_StReg volatile * const pg_gpio_rega;
extern struct tagGPIO_StReg volatile * const pg_gpio_regb;
extern struct tagGPIO_StReg volatile * const pg_gpio_regc;
extern struct tagGPIO_StReg volatile * const pg_gpio_regd;
extern struct tagGPIO_StReg volatile * const pg_gpio_rege;
extern struct tagGPIO_StReg volatile * const pg_gpio_regf;
extern struct tagGPIO_StReg volatile * const pg_gpio_regg;
extern struct tagAFIO_StReg volatile * const pg_afio_reg;
extern struct tagGPIO_StReg volatile * const pg_gpio_reg[];
void GPIO_CfgPinFunc(u32 port,u32 pinnum,u32 func_no);
u32 GPIO_GetData(u32 port);
void GPIO_OutData(u32 port,u32 data);
void GPIO_SettoHigh(u32 port,u32 msk);
void GPIO_SettoLow(u32 port,u32 msk);
void GPIO_PowerOn(u32 port);
void GPIO_PowerOff(u32 port);

#ifdef __cplusplus
}
#endif

#endif /*__cpu_peri_gpio_h_*/
