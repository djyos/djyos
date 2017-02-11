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

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//GPIO设置专用宏定义
#define GPIO_MODE_IN    	0		//普通输入模式
#define GPIO_MODE_OUT		1		//普通输出模式
#define GPIO_MODE_AF		2		//AF功能模式
#define GPIO_MODE_AIN		3		//模拟输入模式

#define GPIO_SPEED_2M		0		//GPIO速度LOW
#define GPIO_SPEED_25M		1		//GPIO速度MEDIUM
#define GPIO_SPEED_50M		2		//GPIO速度High
#define GPIO_SPEED_100M		3		//GPIO速度very high
//todo 以上4个定义不太合适改为下面四个定义
#define GPIO_SPEED_L		0		//GPIO速度LOW
#define GPIO_SPEED_M		1		//GPIO速度MEDIUM
#define GPIO_SPEED_H		2		//GPIO速度High
#define GPIO_SPEED_VH	    3		//GPIO速度very high


#define GPIO_PUPD_NONE		0		//不带上下拉
#define GPIO_PUPD_PU		1		//上拉
#define GPIO_PUPD_PD		2		//下拉
#define GPIO_PUPD_RES		3		//保留

#define GPIO_OTYPE_PP		0		//推挽输出
#define GPIO_OTYPE_OD		1		//开漏输出

//GPIO端口设置
#define GPIO_A 				0
#define GPIO_B 				1
#define GPIO_C				2
#define GPIO_D 				3
#define GPIO_E 				4
#define GPIO_F 				5
#define GPIO_G 				6
#define GPIO_H 				7
#define GPIO_I 				8
#define GPIO_J 				9
#define GPIO_K 				10

//GPIO引脚定义
#define PIN0				(1<<0)
#define PIN1				(1<<1)
#define PIN2				(1<<2)
#define PIN3				(1<<3)
#define PIN4				(1<<4)
#define PIN5				(1<<5)
#define PIN6				(1<<6)
#define PIN7				(1<<7)
#define PIN8				(1<<8)
#define PIN9				(1<<9)
#define PIN10				(1<<10)
#define PIN11				(1<<11)
#define PIN12				(1<<12)
#define PIN13				(1<<13)
#define PIN14				(1<<14)
#define PIN15				(1<<15)

#define GPIO_Lock            (0)
#define GPIO_UnLockr          (1)

#define AF0 				0
#define AF1 				1
#define AF2 				2
#define AF3 				3
#define AF4 				4
#define AF5 				5
#define AF6 				6
#define AF7 				7
#define AF8 				8
#define AF9 				9
#define AF10 				10
#define AF11 				11
#define AF12 				12
#define AF13				13
#define AF14				14
#define AF15				15
#define AF_NUll             16//复用模式为空作为普通I/O口使用。

//O_开头的是输出需要初始化的   I_  开头的是输入需要初始化化的
typedef struct PIN
{
   //GPIO_A....
   u32 PORT;
   //如PIN1....
   u32 Pinx;
   // GPIO 端口模式寄存器如 GPIO_MODE_IN
   u32 MODER;
   //输出模式开漏或者 推挽输出
   u32 O_TYPER;
   //频率
   u32 O_SPEEDR;
   //上拉下拉等
   u32 PUPD;
   //AF mode
   u32 AF;
}Pin;

#define PIO_LISTSIZE(pPins)    (sizeof(pPins) / sizeof( Pin))


bool_t GPIO_CfgPinFunc(u32 port,u32 Pinx,u32 Mode,
						u32 OutType,u32 OutSpeed,u32 PUPD);
bool_t GPIO_AFSet(u32 port,u32 pinnum,u32 af_no);
u32 GPIO_GetData(u32 port);
void GPIO_OutData(u32 port,u32 data);
void GPIO_SettoHigh(u32 port,u32 msk);
void GPIO_SettoLow(u32 port,u32 msk);
void GPIO_PowerOn(u32 port);
void GPIO_PowerOff(u32 port);
bool_t GPIO_SetLckr(u32 port,u32 Lckk);




bool_t PIO_Configure(const Pin *Pin, u32 num);
void PIO_Clear(const Pin *Pin);
void PIO_Set(const Pin *Pin);
unsigned char PIO_Get( const Pin *pin );

#ifdef __cplusplus
}
#endif

#endif /*__cpu_peri_gpio_h_*/
