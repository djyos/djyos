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
// 文件名     ：cpu_peri_iic.h
// 模块描述: IIC模块底层驱动头文件
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 09/17.2014
// =============================================================================

#ifndef CPU_PERI_IIC_H_
#define CPU_PERI_IIC_H_
#ifdef __cplusplus
extern "C" {
#endif
//IIC控制器编号宏定义
#define CN_IIC1     0
#define CN_IIC2     1

#define CN_IIC_SPEED_50KHZ       (50*1000)
#define CN_IIC_SPEED_100KHZ      (100*1000)
#define CN_IIC_SPEED_200KHZ      (200*1000)
#define CN_IIC_SPEED_300KHZ      (300*1000)
#define CN_IIC_SPEED_400KHZ      (400*1000)

//定义总线等待超时时间
#ifndef CONFIG_I2C_MBB_TIMEOUT
#define CONFIG_I2C_MBB_TIMEOUT	10000
#endif

//定义读写超时时间
#ifndef CONFIG_I2C_TIMEOUT
#define CONFIG_I2C_TIMEOUT	1000
#endif

//定义读写操作比特位
#define I2C_READ_BIT  1
#define I2C_WRITE_BIT 0

#ifndef __IO
#define __IO  volatile
#endif

typedef struct I2C_StReg
{
	  __IO uint16_t CR1;
	  uint16_t  RESERVED0;
	  __IO uint16_t CR2;
	  uint16_t  RESERVED1;
	  __IO uint16_t OAR1;
	  uint16_t  RESERVED2;
	  __IO uint16_t OAR2;
	  uint16_t  RESERVED3;
	  __IO uint16_t DR;
	  uint16_t  RESERVED4;
	  __IO uint16_t SR1;
	  uint16_t  RESERVED5;
	  __IO uint16_t SR2;
	  uint16_t  RESERVED6;
	  __IO uint16_t CCR;
	  uint16_t  RESERVED7;
	  __IO uint16_t TRISE;
	  uint16_t  RESERVED8;
}tagI2CReg;


extern struct I2C_StReg   volatile * const pg_i2c1_reg;
extern struct I2C_StReg   volatile * const pg_i2c2_reg;

//===================CR1==================//
#define I2C_CR1_SWRST_MASK (1<<15)

#define I2C_CR1_ALERRT_MASK    (1<<13)
#define I2C_CR1_PEC_MASK       (1<<12)
#define I2C_CR1_POS_MASK       (1<<11)
#define I2C_CR1_ACK_MASK       (1<<10)
#define I2C_CR1_STOP_MASK      (1<<9)
#define I2C_CR1_START_MASK     (1<<8)
#define I2C_CR1_NOSTRETCH_MASK (1<<7)
#define I2C_CR1_ENGC_MASK (1<<6)
#define I2C_CR1_ENPEC_MASK     (1<<5)
#define I2C_CR1_ENARP_MASK     (1<<4)
#define I2C_CR1_SMBTYPE_MASK   (1<<3)

#define I2C_CR1_SMBUS_MASK     (1<<1)
#define I2C_CR1_PE_MASK        (1)

//===================CR2==================//
#define I2C_CR2_LAST_MASK     (1<<12)
#define I2C_CR2_DMAEN_MASK    (1<<11)
#define I2C_CR2_ITBUFEN_MASK  (1<<10)
#define I2C_CR2_ITEVTEN_MASK  (1<<9)
#define I2C_CR2_ITERREN_MASK  (1<<8)
/*I2C_CR2_FREQ[5:0]_MASK
 外设时钟频率必须设置正确的输入时钟频率以产生正确的时序，允许的范围在2～50MHz之间：
000000：禁止
000001：禁止
000010：2 MHz
...
110010：50 MHz
大于110010：禁止*/

////===================OAR1==================//
#define I2C_OAR1_ADDMODE_MASK (1<<15)
/*ADD[9:8]：接口地址
7位地址模式时不用关心
10位地址模式时为地址的9~8位
ADD[7:1]：接口地址地址的7~1位
*/
#define I2C_OAR1_ADD0_MASK    (1)//ADD0：接口地址10位地址模式时为地址第0位

//===================OAR2==================//
/*ADD2[7:1]：接口地址在双地址模式下地址的7~1位。*/
#define I2C_OAR2_ENDUAL_MASK  (1)//1：在7位地址模式下，OAR1和OAR2都被识别。双地址模式使能位
//===================DR==================//
/*DR[7:0]：8位数据寄存用于存放接收到的数据或放置用于发送到总线的数据 */

//===================SR1==================//
#define I2C_SR1_SMBALERT_MASK (1<<15)
#define I2C_SR1_TIMEOUT_MASK  (1<<14)

#define I2C_SR1_PECERR_MASK  (1<<12)
#define I2C_SR1_OVR_MASK      (1<<11)
#define I2C_SR1_AF_MASK       (1<<10)
#define I2C_SR1_ARLO_MASK     (1<<9)
#define I2C_SR1_BERR_MASK     (1<<8)
#define I2C_SR1_TxE_MASK      (1<<7)
#define I2C_SR1_RxNE_MASK     (1<<6)

#define I2C_SR1_STOPF_MASK    (1<<4)
#define I2C_SR1_ADD10_MASK    (1<<3)
#define I2C_SR1_BTF_MASK      (1<<2)
#define I2C_SR1_ADDR_MASK     (1<<1)
#define I2C_SR1_SB_MASK       (1)

//===================SR2==================//
/*PEC[7:0]：数据包出错检测
当ENPEC=1时，PEC[7:0]存放内部的PEC的值。*/
#define I2C_SR2_DUALF_MASK         (1<<7)
#define I2C_SR2_SMBHOST_MASK       (1<<6)
#define I2C_SR2_SMBDEFAULT_MASK    (1<<5)
#define I2C_SR2_GENCALL_MASK       (1<<4)

#define I2C_SR2_TRA_MASK           (1<<2)
#define I2C_SR2_BUSY_MASK          (1<<1)
#define I2C_SR2_MSL_MASK           (1)

//===时钟控制寄存器=====CCR==================//
#define I2C_CCR_FS_MASK            (1<<15)
#define I2C_CCR_DUTY_MASK         (1<<14)
/*CCR[11:0]：快速/标准模式下的时钟控制分频系数（主模式）*/

//===================TRISE==================//
/*TRISE[5:0]：在快速/标准模式下的最大上升时间（主模式）
这些位必须使用I2C总线规范里给出的最大的SCL上升时间来填充，增长步幅为1。
例如：标准模式中最大允许SCL上升时间为1000ns。如果在I2C_CR2寄存器中FREQ[5:0]中的
值等于08h且tCK=125ns，故TRISE[5:0]中必须写入09h(1000ns/125 ns = 8+1)。
滤波器的值也可以加到TRISE[5:0]内。
如果结果不是一个整数，则将整数部分写入TRISE[5:0]以确保tHIGH参数。
注：只有当I2C被禁用(PE=0)时，才能设置TRISE[5:0]。*/
bool_t IIC1_Init(void);
bool_t IIC2_Init(void);

#ifdef __cplusplus
}
#endif
#endif /* CPU_PERI_IIC_H_ */
