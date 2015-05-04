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
//所属模块: 系统初始化
//作者：lst
//版本：V1.0.0
//文件描述: 时钟和电源控制寄存器结构定义
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-08-21
//   作者: lst
//   新版本号：V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __RCC_H__
#define __RCC_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------ Reset and Clock Control ---------------------------
//几个时钟配置常数
#define cn_pll          (CN_CFG_MCLK/CN_CFG_EXTCLK-2)   //pll乘数
#define cn_AHB_scaler   0           //不分频
#define cn_pclk2_scaler 0           //不分频
#define cn_pclk1_scaler 4           //二分频

//定义RCC_TypeDef.CR位掩码，只有1位的用位带地址而不是bitmask---------------
#define bb_rcc_cr_base   (0x42000000 + 0x21000*32)  //位带基址
//初始化检查值，不等于(0x030b0000)表示时钟未初始化
#define cn_cr_check ((1<<25)+(1<<24)+(1<<19)+(0<<18)+(1<<17)+(1<<16)+(0<<0))
//初始化检查值的掩码 0x030f0001
#define cn_cr_check_mask ((1<<25)+(1<<24)+(1<<19)+(1<<18)+(1<<17)+(1<<16)+(1<<0))
//初始化设置值
#define cn_cr_set cn_cr_check

//rw,hsion,内部高速时钟使能,1=使能
#define bb_rcc_cr_hsion    (*(vu32 *)(bb_rcc_cr_base+0*4))
//r,hsirdy,内部高速时钟就绪标志，1=就绪
#define bb_rcc_cr_hsirdy   (*(vu32 *)(bb_rcc_cr_base+1*4))
#define bm_rcc_hsitrim     (0x1f<<3)  //rw,内部高速时钟调整
#define bm_rcc_hsical      (0xff<<8)  //r,内部高速时钟校准寄存器
//rw,外部高速时钟使能，进入待机和停止模式时，该位由硬件清零。 0=HSE振荡器关闭，1=使能
#define bb_rcc_cr_hseon    (*(vu32 *)(bb_rcc_cr_base+16*4))
//r,外部1-25MHz时钟就绪标志,0=不就绪，1=就绪
#define bb_rcc_cr_hserdy   (*(vu32 *)(bb_rcc_cr_base+17*4))
//rw,外部高速时钟旁路，在调试模式下由软件置1或清零来旁路外部晶体振荡器。只有在外部
//1-25MHz振荡器关闭的情况下，该位才可以写入。0=接入，1=旁路
#define bb_rcc_cr_hsebyp   (*(vu32 *)(bb_rcc_cr_base+18*4))
//rw,时钟安全系统使能，0=时钟检测器关闭，1=如果外部1-25MHz时钟就绪，时钟监测器开启。
#define bb_rcc_cr_csson    (*(vu32 *)(bb_rcc_cr_base+19*4))
//rw,PLL使能由软件置1或清零。当进入待机和停止模式时，该位由硬件清零。 0=关闭，1=使能
#define bb_rcc_cr_pllon    (*(vu32 *)(bb_rcc_cr_base+24*4))
//r,0=PLL未锁定，1=PLL锁定
#define bb_rcc_cr_pllrdy   (*(vu32 *)(bb_rcc_cr_base+25*4))

//定义RCC_TypeDef.CFGR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_cfgr_base  (0x42000000 + 0x21004*32)  //位带基址
//初始化检查值，不等于此值表示时钟未初始化
#define cn_cfgr_check   ((2<<2)+(cn_AHB_scaler<<4)+(cn_pclk1_scaler<<8) \
                        +(cn_pclk2_scaler<<11)+(1<<16)+(1<<17)+(cn_pll<<18))
//初始化检查值的掩码 0x003f3ff3
#define cn_cfgr_check_mask    ((3<<2)+(0xf<<4)+(0x7<<8)+(0x7<<11)+(1<<16) \
                        +(1<<17)+(0xf<<18))
//初始化设置值
#define cn_cfgr_set ((2<<0)+(cn_AHB_scaler<<4)+(cn_pclk1_scaler<<8) \
                    +(cn_pclk2_scaler<<11)+(3<<14)+(1<<16)+(0<<17)+(cn_pll<<18))
#define bm_rcc_mco      (7<<24)  //rw,微控制器时钟输出，
                                 //0xx=无时钟输出，
                                 //100=系统时钟输出。
                                 //101=内部8MHz的RC振荡器时钟输出。
                                 //110=外部1-25MHz振荡器时钟输出
                                 //111: PLL时钟2分频后输出，
                                 //注意：- 该时钟输出在启动和切换MCO时钟源时
                                 //可能会被截断。- 在系统时钟作为输出时钟时，
                                 //请保证输出时钟频率不超过50MHz (IO口最高频率)
//rw,USB预分频,由软件设置来产生48MHz的USB时钟。在RCC_APB1ENR寄存器中使能USB时钟之前，必须保证该位已经有效。
//如果USB时钟被使能，该位可以被清零。 0 = PLL时钟1.5倍分频作为USB时钟 1 = PLL时钟直接作为USB时钟 .
#define bb_rcc_cfgr_usbpre  (*(vu32 *)(bb_rcc_cfgr_base+22*4))

#define bm_rcc_cfgr_pllmul       (0xf<<18)//rw,PLL倍频数，0~14对应2~16倍，15=16倍
//rw,由软件设置来分频HSE后作为PLL输入时钟。该位只有
//   在PLL关闭时才可以被写入。0 = 不分频，1 = 2分频
#define bb_rcc_cfgr_pllxtpre  (*(vu32 *)(bb_rcc_cfgr_base+17*4))
//rw,PLL输入时钟源,由软件设置来选择PLL输入时钟源。该位只有在PLL关闭时才可以被写入.
//   0 = HSI时钟2分频后作为PLL输入时钟
//   1 = HSE时钟作为PLL输入时钟。
#define bb_rcc_cfgr_pllsrc    (*(vu32 *)(bb_rcc_cfgr_base+16*4))
//rw,ADCPRE,b15~14,ADC预分频,设置PLCLK2分频后作为ADC时钟频率的
//   分频数，00~11对应2/4/6/8分频
#define bm_rcc_cfgr_adcpre     (3<<14)
//rw,PPRE2,b13~11,设置由HCLK产生APB2时钟的预分频数，0xx=不分频，
//   100~111分别对应2/4/8/16分频
#define bm_rcc_cfgr_ppre2    (7<<11)
//rw,PPRE1,b10~8,设置由HCLK产生APB1时钟的预分频数，必须保证APB1
//   时钟频率不超过36MHz。0xx=不分频，100~111对应2/4/8/16分频
#define bm_rcc_cfgr_ppre1    (7<<8)
//rw,HPRE,b7~4，设置由SYSCLK产生AHB时钟的预分频数，
//   0xxx=不分频，1000~1111表示2/4/8/16/64/128/256/512
#define bm_rcc_cfgr_hpre     (0xf<<4)
//r,SWS,b3~2，获取硬件设置的系统时钟（SYSCLK）选择状态，
//   00=HSI，01=HSE，10=pll输出，11=不允许
#define bm_rcc_cfgr_sws     (3<<2)
//rw,SW,b1~0,设置系统时钟（SYSCLK）源，注：在从停止或待机模式
//   中返回时或直接或间接作为系统时钟的HSE出现故障时，将由硬
//   件强制选择HSI作为系统时钟（如果时钟安全系统已经启动）
//   00=HSI，01=HSE，10=pll输出，11=不允许
#define bm_rcc_cfgr_sw     (3<<0)

//定义RCC_TypeDef.CIR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_cir_base     (0x42000000 + 0x21008*32)         //位带基址
#define bb_rcc_cir_cssc     (*(vu32 *)(bb_rcc_cir_base+23*4)) //w,写1来清除CSSF安全系统中断标志位CSSF。
#define bb_rcc_cir_pllrdyc  (*(vu32 *)(bb_rcc_cir_base+20*4)) //w,写1来清除PLL就绪中断标志位PLLRDYF。
#define bb_rcc_cir_hserdyc  (*(vu32 *)(bb_rcc_cir_base+19*4)) //w,写1来清零HSE就绪中断标志位HSERDYF。
#define bb_rcc_cir_hsirdyc  (*(vu32 *)(bb_rcc_cir_base+18*4)) //w,写1来清零HSI就绪中断标志位HSIRDYF。
#define bb_rcc_cir_lserdyc  (*(vu32 *)(bb_rcc_cir_base+17*4)) //w,写1来清零LSE就绪中断标志位LSERDYF。
#define bb_rcc_cir_lsirdyc  (*(vu32 *)(bb_rcc_cir_base+16*4)) //w,写1来清零LSI就绪中断标志位LSIRDYF。
#define bb_rcc_cir_pllrdyie (*(vu32 *)(bb_rcc_cir_base+12*4)) //rw,PLL就绪中断使能，0 = 禁止，1 = 使能
#define bb_rcc_cir_hsercyie (*(vu32 *)(bb_rcc_cir_base+11*4)) //rw,HSE就绪中断使能，0 = 禁止，1 = 使能
#define bb_rcc_cir_hsirdyie (*(vu32 *)(bb_rcc_cir_base+10*4)) //rw,HSI就绪中断使能，0 = 禁止，1 = 使能
#define bb_rcc_cir_lserdyie (*(vu32 *)(bb_rcc_cir_base+9*4))  //rw,LSE就绪中断使能，0 = 禁止，1 = 使能
#define bb_rcc_cir_lsirdyie (*(vu32 *)(bb_rcc_cir_base+8*4))  //rw,LSI就绪中断使能，0 = 禁止，1 = 使能
#define bb_rcc_cir_cssf     (*(vu32 *)(bb_rcc_cir_base+7*4))  //r,时钟安全系统中断标志,发生1-25M振荡器故障时由硬件置位，
                                                              //    软件写1到CSSC位清零，0 = 无中断，1 = 有中断，
#define bb_rcc_cir_pllrdyf  (*(vu32 *)(bb_rcc_cir_base+4*4))  //r,PLL就绪中断标志,PLL就绪且PLLRDYIE位被置1时由硬件
                                                              //    置位，软件写1到 PLLRDYC位清零，0 = 无中断，1 = 有中断，
#define bb_rcc_cir_hserdyf  (*(vu32 *)(bb_rcc_cir_base+3*4))  //r,HSE就绪中断标志 ,外部低速时钟就绪且HSERDYIE位被置1时由
                                                              //    硬件置位,软件写1到HSERDYC位清零，0 = 无中断，1 = 有中断
#define bb_rcc_cir_hsirdyf  (*(vu32 *)(bb_rcc_cir_base+2*4))  //r,HSI就绪中断标志 ,外部低速时钟就绪且HSIRDYIE位被置1时由
                                                              //    硬件置位,软件写1到HSIRDYC位清零，0 = 无中断，1 = 有中断
#define bb_rcc_cir_lserdyf  (*(vu32 *)(bb_rcc_cir_base+1*4))  //r,LSE就绪中断标志 ,外部低速时钟就绪且LSERDYIE位被置1时由
                                                              //    硬件置位,软件写1到LSERDYC位清零，0 = 无中断，1 = 有中断
#define bb_rcc_cir_lsirdyf  (*(vu32 *)(bb_rcc_cir_base+0*4))  //r,LSI就绪中断标志 ,外部低速时钟就绪且LSIRDYIE位被置1时由
                                                              //    硬件置位,软件写1到LSIRDYC位清零，0 = 无中断，1 = 有中断
//定义RCC_TypeDef.APB2RSTR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_apb2rstr_base        (0x42000000 + 0x2100c*32)  //位带基址
#define bb_rcc_apb2rstr_uart1rst    (*(vu32*)(bb_rcc_apb2rstr_base+14*4)) //rw,写0无效，写1=复位USART1
#define bb_rcc_apb2rstr_spi1rst     (*(vu32*)(bb_rcc_apb2rstr_base+12*4)) //rw,写0无效，写1=复位SPI1
#define bb_rcc_apb2rstr_tim1rst     (*(vu32*)(bb_rcc_apb2rstr_base+11*4)) //rw,写0无效，写1=复位TIM1
#define bb_rcc_apb2rstr_adc2rst     (*(vu32*)(bb_rcc_apb2rstr_base+10*4)) //rw,写0无效，写1=复位ADC2
#define bb_rcc_apb2rstr_adc1rst     (*(vu32*)(bb_rcc_apb2rstr_base+9*4))  //rw,写0无效，写1=复位ADC1
#define bb_rcc_apb2rstr_ioperst     (*(vu32*)(bb_rcc_apb2rstr_base+6*4))  //rw,写0无效，写1=复位IO口E
#define bb_rcc_apb2rstr_iopdrst     (*(vu32*)(bb_rcc_apb2rstr_base+5*4))  //rw,写0无效，写1=复位IO口D
#define bb_rcc_apb2rstr_iopcrst     (*(vu32*)(bb_rcc_apb2rstr_base+4*4))  //rw,写0无效，写1=复位IO口C
#define bb_rcc_apb2rstr_iopbrst     (*(vu32*)(bb_rcc_apb2rstr_base+3*4))  //rw,写0无效，写1=复位IO口B
#define bb_rcc_apb2rstr_ioparst     (*(vu32*)(bb_rcc_apb2rstr_base+2*4))  //rw,写0无效，写1=复位IO口A
#define bb_rcc_apb2rstr_afiorst     (*(vu32*)(bb_rcc_apb2rstr_base+0*4))  //rw,写0无效，写1=复位复用IO功能
//定义RCC_TypeDef.APB1RSTR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_apb1rstr_base        (0x42000000 + 0x21010*32)  //位带基址
#define bb_rcc_apb1rstr_pwrrst      (*(vu32*)(bb_rcc_apb1rstr_base+28*4)) //rw,写0无效，写1=复位电源电路
#define bb_rcc_apb1rstr_bkprst      (*(vu32*)(bb_rcc_apb1rstr_base+27*4)) //rw,写0无效，写1=复位备份电路
#define bb_rcc_apb1rstr_canrst      (*(vu32*)(bb_rcc_apb1rstr_base+25*4)) //rw,写0无效，写1=复位CAN
#define bb_rcc_apb1rstr_usbrst      (*(vu32*)(bb_rcc_apb1rstr_base+23*4)) //rw,写0无效，写1=复位USB
#define bb_rcc_apb1rstr_i2c2rst     (*(vu32*)(bb_rcc_apb1rstr_base+22*4)) //rw,写0无效，写1=复位I2C2
#define bb_rcc_apb1rstr_i2c1rst     (*(vu32*)(bb_rcc_apb1rstr_base+21*4)) //rw,写0无效，写1=复位I2C1
#define bb_rcc_apb1rstr_uart3rst    (*(vu32*)(bb_rcc_apb1rstr_base+18*4)) //rw,写0无效，写1=复位USART3
#define bb_rcc_apb1rstr_uart2rst    (*(vu32*)(bb_rcc_apb1rstr_base+17*4)) //rw,写0无效，写1=复位USART2
#define bb_rcc_apb1rstr_spi2rst     (*(vu32*)(bb_rcc_apb1rstr_base+14*4)) //rw,写0无效，写1=复位SPI2
#define bb_rcc_apb1rstr_wwdgrst     (*(vu32*)(bb_rcc_apb1rstr_base+11*4)) //rw,写0无效，写1=复位窗口看门狗
#define bb_rcc_apb1rstr_tim4rst     (*(vu32*)(bb_rcc_apb1rstr_base+2*4))  //rw,写0无效，写1=复位timer4
#define bb_rcc_apb1rstr_tim3rst     (*(vu32*)(bb_rcc_apb1rstr_base+1*4))  //rw,写0无效，写1=复位timer3
#define bb_rcc_apb1rstr_tim2rst     (*(vu32*)(bb_rcc_apb1rstr_base+0*4))  //rw,写0无效，写1=复位timer2
//定义RCC_TypeDef.AHBENR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_ahbenr_base     (0x42000000 + 0x21014*32)  //位带基址
#define bb_rcc_ahbenr_flitfen   (*(vu32*)(bb_rcc_ahbenr_base+4*4))//rw,b4,闪存接口电路时钟使能,
                                                                //0：睡眠模式时闪存接口电路时钟关闭
                                                                //1：睡眠模式时闪存接口电路时钟开启
#define bb_rcc_ahbenr_sramen    (*(vu32*)(bb_rcc_ahbenr_base+2*4))//rw,b2,SRAM时钟使能,
                                                                //0：睡眠模式时SRAM时钟关闭
                                                                //1：睡眠模式时SRAM时钟开启
#define bb_rcc_ahbenr_dma1en    (*(vu32*)(bb_rcc_ahbenr_base+0*4))//rw,b0, 0：DMA1时钟关闭，1：DMA时钟开启
#define bb_rcc_ahbenr_dma2en    (*(vu32*)(bb_rcc_ahbenr_base+1*4))//rw,b0, 0：DMA1时钟关闭，1：DMA时钟开启
//定义RCC_TypeDef.APB2ENR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_apb2enr_base     (0x42000000 + 0x21018*32)  //位带基址
#define bb_rcc_apb2enr_uart1en  (*(vu32*)(bb_rcc_apb2enr_base+14*4))  //rw,0=USART1时钟关闭,1=USART1时钟开启
#define bb_rcc_apb2enr_spi1en   (*(vu32*)(bb_rcc_apb2enr_base+12*4))  //rw,0=SPI1时钟关闭,  1=SPI1时钟开启
#define bb_rcc_apb2enr_tim1en   (*(vu32*)(bb_rcc_apb2enr_base+11*4))  //rw,0=TIM1时钟关闭,  1=TIM1时钟开启
#define bb_rcc_apb2enr_adc2en   (*(vu32*)(bb_rcc_apb2enr_base+10*4))  //rw,0=ADC2时钟关闭,  1=ADC2时钟开启
#define bb_rcc_apb2enr_adc1en   (*(vu32*)(bb_rcc_apb2enr_base+9*4))   //rw,0=ADC1时钟关闭,  1=ADC1时钟开启
#define bb_rcc_apb2enr_iopgen   (*(vu32*)(bb_rcc_apb2enr_base+8*4))   //rw,0=IO口E时钟关闭, 1=IO口E时钟开启
#define bb_rcc_apb2enr_iopfen   (*(vu32*)(bb_rcc_apb2enr_base+7*4))   //rw,0=IO口E时钟关闭, 1=IO口E时钟开启
#define bb_rcc_apb2enr_iopeen   (*(vu32*)(bb_rcc_apb2enr_base+6*4))   //rw,0=IO口E时钟关闭, 1=IO口E时钟开启
#define bb_rcc_apb2enr_iopden   (*(vu32*)(bb_rcc_apb2enr_base+5*4))   //rw,0=IO口D时钟关闭, 1=IO口D时钟开启
#define bb_rcc_apb2enr_iopcen   (*(vu32*)(bb_rcc_apb2enr_base+4*4))   //rw,0=IO口C时钟关闭, 1=IO口C时钟开启
#define bb_rcc_apb2enr_iopben   (*(vu32*)(bb_rcc_apb2enr_base+3*4))   //rw,0=IO口B时钟关闭, 1=IO口B时钟开启
#define bb_rcc_apb2enr_iopaen   (*(vu32*)(bb_rcc_apb2enr_base+2*4))   //rw,0=IO口A时钟关闭, 1=IO口A时钟开启
#define bb_rcc_apb2enr_afioen   (*(vu32*)(bb_rcc_apb2enr_base+0*4))   //rw,0=复用功能IO时钟关闭, 1=复用功能IO时钟开启
//定义RCC_TypeDef.APB1ENR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_apb1enr_base     (0x42000000 + 0x2101c*32)  //位带基址
#define bb_rcc_apb1enr_pwren    (*(vu32*)(bb_rcc_apb1enr_base+28*4))  //rw,0=关闭，1=开启电源时钟
#define bb_rcc_apb1enr_bkpen    (*(vu32*)(bb_rcc_apb1enr_base+27*4))  //rw,0=关闭，1=开启备份时钟
#define bb_rcc_apb1enr_canen    (*(vu32*)(bb_rcc_apb1enr_base+25*4))  //rw,0=关闭，1=开启CAN时钟
#define bb_rcc_apb1enr_usben    (*(vu32*)(bb_rcc_apb1enr_base+23*4))  //rw,0=关闭，1=开启USB时钟
#define bb_rcc_apb1enr_i2c2en   (*(vu32*)(bb_rcc_apb1enr_base+22*4))  //rw,0=关闭，1=开启I2C2时钟
#define bb_rcc_apb1enr_i2c1en   (*(vu32*)(bb_rcc_apb1enr_base+21*4))  //rw,0=关闭，1=开启I2C1时钟
#define bb_rcc_apb1enr_uart5en  (*(vu32*)(bb_rcc_apb1enr_base+20*4))  //rw,0=关闭，1=开启USART5时钟
#define bb_rcc_apb1enr_uart4en  (*(vu32*)(bb_rcc_apb1enr_base+19*4))  //rw,0=关闭，1=开启USART4时钟
#define bb_rcc_apb1enr_uart3en  (*(vu32*)(bb_rcc_apb1enr_base+18*4))  //rw,0=关闭，1=开启USART3时钟
#define bb_rcc_apb1enr_uart2en  (*(vu32*)(bb_rcc_apb1enr_base+17*4))  //rw,0=关闭，1=开启USART2时钟
#define bb_rcc_apb1enr_spi2en   (*(vu32*)(bb_rcc_apb1enr_base+14*4))  //rw,0=关闭，1=开启SPI2时钟
#define bb_rcc_apb1enr_wwdgen   (*(vu32*)(bb_rcc_apb1enr_base+11*4))  //rw,0=关闭，1=开启窗口看门狗时钟
#define bb_rcc_apb1enr_tim4en   (*(vu32*)(bb_rcc_apb1enr_base+2*4))   //rw,0=关闭，1=开启timer4时钟
#define bb_rcc_apb1enr_tim3en   (*(vu32*)(bb_rcc_apb1enr_base+1*4))   //rw,0=关闭，1=开启timer3时钟
#define bb_rcc_apb1enr_tim2en   (*(vu32*)(bb_rcc_apb1enr_base+0*4))   //rw,0=关闭，1=开启timer2时钟
//定义RCC_TypeDef.BDCR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_bdcr_base     (0x42000000 + 0x21020*32)  //位带基址
#define bb_rcc_bdcr_bdrst   (*(vu32*)(bb_rcc_bdcr_base+16*4)) //rw,写1：复位整个备份域。
#define bb_rcc_bdcr_rtcen   (*(vu32*)(bb_rcc_bdcr_base+15*4)) //rw,RTC时钟使能 ，0=关闭，1=开启
#define bb_rcc_bdcr_lsebyp  (*(vu32*)(bb_rcc_bdcr_base+2*4))  //rw,1=外部低速时钟旁路，0=未旁路
#define bb_rcc_bdcr_lserdy  (*(vu32*)(bb_rcc_bdcr_base+1*4))  //r ,0=32k时钟未就绪，1=就绪
#define bb_rcc_bdcr_lseon   (*(vu32*)(bb_rcc_bdcr_base+0*4))  //rw,0=32k振荡器关闭，1=开启
#define bm_rcc_bdcr_rtcsel  (3<<8)  //RTC时钟选择，00=无时钟。01=lse，10=lsi，11=hse的128分频

//定义RCC_TypeDef.CSR位掩码，只有1位的用位带地址而不是bitmask------------
#define bb_rcc_csr_base     (0x42000000 + 0x21024*32)  //位带基址
#define bb_rcc_csr_lpwrrstf (*(vu32*)(bb_rcc_csr_base+31*4))  //rw,b31,0：无低功耗管理复位发生
                                                            //1：发生低功耗管理复位 ,须通过写1到RMVF清除。
#define bb_rcc_csr_wwdgrstf (*(vu32*)(bb_rcc_csr_base+30*4))  //rw,b30,0：无窗口看门狗复位发生
                                                            //1：发生窗口看门狗复位 ,须通过写1到RMVF清除。
#define bb_rcc_csr_iwdgrstf (*(vu32*)(bb_rcc_csr_base+29*4))  //rw,b29,0：无独立看门狗复位发生
                                                            //1：发生独立看门狗复位 ,须通过写1到RMVF清除。
#define bb_rcc_csr_sftrstf  (*(vu32*)(bb_rcc_csr_base+28*4))  //rw,b28,0：无软件复位发生
                                                            //1：发生软件复位 ,须通过写1到RMVF清除。
#define bb_rcc_csr_porrstf  (*(vu32*)(bb_rcc_csr_base+27*4))  //rw,b27,0：无上电/掉电复位发生
                                                            //1：发生上电/掉电复位 ,须通过写1到RMVF清除。
#define bb_rcc_csr_pinrstf  (*(vu32*)(bb_rcc_csr_base+26*4))  //rw,b26,0：无NRST管脚复位发生
                                                            //1：发生NRST管脚复位,须通过写1到RMVF清除。
#define bb_rcc_csr_rmvf     (*(vu32*)(bb_rcc_csr_base+24*4))  //rw,b24,0：保持复位标志  1：清零复位标志
#define bb_rcc_csr_lsirdy   (*(vu32*)(bb_rcc_csr_base+1*4))   //r,b1,0：LSI(内部40kHz RC振荡器)时钟未就绪
                                                            //     1：LSI(内部40kHz RC振荡器)时钟就绪
#define bb_rcc_csr_lsion    (*(vu32*)bb_rcc_csr_base+0*4)   //rw,b0,0：内部40kHz RC振荡器关闭,1：内部40kHz RC振荡器开启
struct st_rcc_reg     //复位和时钟控制寄存器 at 0x40021000
{
    vu32 CR;      //时钟控制寄存器
    vu32 CFGR;    //时钟配置寄存器
    vu32 CIR;     //时钟中断寄存器 (RCC_CIR)
    vu32 APB2RSTR;//APB2外设复位寄存器
    vu32 APB1RSTR;//APB1外设复位寄存器
    vu32 AHBENR;  //AHB外设时钟使能寄存器
    vu32 APB2ENR; //APB2外设时钟使能寄存器
    vu32 APB1ENR; //APB1外设时钟使能寄存器
    vu32 BDCR;    //备份域控制寄存器
    vu32 CSR;     //控制/状态寄存器
} ;

extern struct st_rcc_reg volatile * const pg_rcc_reg;

#ifdef __cplusplus
}
#endif

#endif /*__RCC_H__*/



