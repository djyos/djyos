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
// 文件名     ：cpu_peri_iic.c
// 模块描述: IIC模块底层硬件驱动，寄存器级别的操作
// 创建时间: 17/09.2014
// =============================================================================
#include "cpu_peri.h"
#include "djyos.h"
#include "stdint.h"
#include "../../../arch/arm/arm32_stdint.h"
#include "stdio.h"
#include "iicbus.h"
#include "endian.h"
#include "lock.h"
#include "int.h"

// =============================================================================
static I2C_TypeDef volatile * const tg_I2c_Reg[] = {(I2C_TypeDef *)I2C1_BASE,
                                                    (I2C_TypeDef *)I2C2_BASE,
                                                    (I2C_TypeDef *)I2C3_BASE,
                                                    (I2C_TypeDef *)I2C4_BASE};

#define tagI2CReg  I2C_TypeDef
static struct IIC_CB s_IIC_CB[CN_IIC_NUM];
//static struct IIC_CB s_IIC_CB[CN_IIC2];

static u32 IIC_BUF_LEN[CN_IIC_NUM]={128,128,128,128};
//iic缓冲区定数组
static u8 s_IIC1Buf[128];
static u8 s_IIC2Buf[128];
static u8 s_IIC3Buf[128];
static u8 s_IIC4Buf[128];

static u8* s_IICBuf[CN_IIC_NUM]={s_IIC1Buf,
								 s_IIC2Buf,
								 s_IIC3Buf,
								 s_IIC4Buf,};

struct IIC_IntParamSet
{
    struct SemaphoreLCB *pDrvPostSemp;   //信号量
    u32 TransCount;       //传输数据量计数器
    u32 TransTotalLen;//接收的数据总量
};

static struct IIC_IntParamSet IntParamset[CN_IIC_NUM];

const char *IIC_Name[CN_IIC_NUM]=
		 {"IIC1","IIC2","IIC3","IIC4"};


// =============================================================================
// 功能：IIC检查是否有错误
// 参数： reg iic基地址
// 返回：true/false
// =============================================================================
static bool_t  _IIC_Chek(tagI2CReg *reg)
{
    if (reg->ISR & (I2C_ISR_NACKF))       /*如果仲裁丢失*/
		{
			#ifdef DEBUG
			printk("__i2c_wait: nackf\n");
			#endif
			return false;
		}
    return true;
}


// =============================================================================
// 功能: 使能iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __IIC_IntEnable(tagI2CReg *reg)
{
    reg->CR1 |= (I2C_CR1_TXIE|I2C_CR1_RXIE);//发送和接收中断同时使能

}

// =============================================================================
// 功能: 禁止iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __IIC_IntDisable(tagI2CReg *reg)
{
    reg->CR1 &= ~(I2C_CR1_RXIE| I2C_CR1_TXIE);
}


// =============================================================================
// 功能: 选择设置时钟
// 参数: iic_port iic编号，
//		sel:选择时钟 //bit:0- APB1-54MHz 	1-sysclk-216MHz 	2- HSI-108MHz
//	    TIMINGR 寄存器设置 (4<<28)|(9<<20)|(1<<16)|(39<<8)|(50);
//		初始值:0x40912732;
// 返回: 无
// =============================================================================

static bool_t __IIC_ClkSet(u8 iic_port,u8 sel,u32 TIMINGR)
{
	if(iic_port>=CN_IIC_NUM)
		return false;
	RCC->DCKCFGR2 &=~(3<<((2*iic_port)+16));//清除原来的设置
	RCC->DCKCFGR2 |=(sel<<((2*iic_port)+16));
	tg_I2c_Reg[iic_port]->CR1 &=~(1);
	tg_I2c_Reg[iic_port]->TIMINGR=TIMINGR;
	return true;
}
// =============================================================================
// 功能: 硬件GPIO初始化，包括时钟选择和iic初始化参数
// 参数: IIC_NO,编号
// 返回: 无
// =============================================================================
static void __IIC_GpioConfig(u8 iic_port)
{
    u8 tout,gpio;
    u16 pin;
	RCC->APB1ENR |=(1<<(21+iic_port));//外设时钟使能
    //0- APB1-54	1-sysclk-216 	2- HSI-108
    if(__IIC_ClkSet(iic_port,1,0x40912732)==false)//默认时钟源为APB1时钟
    	return;
	switch(iic_port)
    {
		case CN_IIC1:
			gpio=GPIO_B;
			pin=PIN8|PIN9;break;

		case CN_IIC2:
			gpio=GPIO_B;
			pin=PIN10|PIN11;break;

		case CN_IIC3:
			gpio=GPIO_H;
			pin=PIN7|PIN8;	break;

		case CN_IIC4:
			gpio=GPIO_H;
			pin=PIN11|PIN12;break;

		default:  return;
    }
	GPIO_PowerOn(gpio);//GPIO时钟使能
	GPIO_AFSet( gpio,pin, AF4);//设置复用功能
	GPIO_CfgPinFunc(gpio,pin,GPIO_MODE_AF,
			GPIO_OTYPE_OD,GPIO_SPEED_100M,GPIO_PUPD_PU);
	//时序初始化
	tg_I2c_Reg[iic_port]->CR1 &=~I2C_CR1_ANFOFF;//时钟50ns以下时需要写1禁止模拟滤波器减小抑制
	tg_I2c_Reg[iic_port]->CR1 |=I2C_CR1_DNF_4BIT(0);//可滤除的噪声尖峰脉宽 0~15 tI2CCLK
	tg_I2c_Reg[iic_port]->CR2 |=I2C_CR2_AUTOEND;//自动结束模式
	tg_I2c_Reg[iic_port]->CR2 &=~I2C_CR2_ADD10;//7位寻址模式
	tg_I2c_Reg[iic_port]->OAR1=0x8000;//自身地址
	tg_I2c_Reg[iic_port]->CR1 |=I2C_CR1_PE;//PE使能

}
// RELOAD CR2_24 位置 1。在该模式下，完成 NBYTES
//中所编程字节数的数据传输之后，TCR 标志将置 1，并且 TCIE 置 1 时将生成中断。只要
//TCR 标志置 1，SCL 便会延长。当往 NBYTES 写入一个非零值时，TCR 由软件清零。
//在往 NBYTE 中设置最后一次传输的字节数前，必须把 RELOAD 位清零
// =============================================================================
// 功能: IIC中断配置函数
// 参数: IntLine,中断线
//       isr,中断服务函数指针
// 返回: 无
// =============================================================================
static void __IIC_IntConfig(u8 iic_port,u32 (*isr)(ptu32_t))
{
		ufast_t IntLine;
		switch (iic_port)
		{
		case CN_IIC1:
			IntLine=CN_INT_LINE_I2C1_EV;
			break;
		case CN_IIC2:
			IntLine=CN_INT_LINE_I2C2_EV;
			break;
		case CN_IIC3:
			IntLine=CN_INT_LINE_I2C3_EV;
			break;
		case CN_IIC4:
			IntLine=CN_INT_LINE_I2C4_EV;
			break;
			default:return;
		}
		Int_Register(IntLine);
        Int_IsrConnect(IntLine,isr);
        Int_SettoAsynSignal(IntLine);
        Int_ClearLine(IntLine);     //清掉初始化产生的发送fifo空的中断
        Int_RestoreAsynLine(IntLine);
}

// =============================================================================
// 功能: 产生停止时序，结束本次与外器件之间的通信
// 参数: reg,iic外设寄存器基址
// 返回: 无
// =============================================================================
static void __IIC_GenerateStop(tagI2CReg *reg)
{
	reg->CR2 |=I2C_CR2_STOP;//产生停止位

}

// =============================================================================
// 功能：IIC写
// 参数： reg,寄存器基址
//		devaddr,器件物理地址，最后一bit区分读写
//		 buf_addr地址写缓冲
//       len_addr 地址长度
//		 buf写缓冲
//       len 长度
// 返回：true/false
// =============================================================================
static bool_t __IIC_Write(tagI2CReg *reg,u8 devaddr, u8 *adder_nuf, u32 addr_len,u8 *buf, u32 len)
{
    u32 timeout=0;
    u32 i=0;
    reg->CR2 &=~I2C_CR2_SADD_10BIT(0x7f<<1);
	reg->CR2 |=I2C_CR2_SADD_10BIT(devaddr<<1);//从器件地址
	reg->CR2 &=~I2C_CR2_RD_WRN;//写控制
	 if(reg->ISR|=I2C_ISR_STOPF)
	    	reg->ICR|=I2C_ICR_STOPCF;
	 if(reg->ISR|=I2C_ISR_NACKF)
	    	reg->ICR|=I2C_ICR_NACKCF;
	 len=len+addr_len;
    if(len>255)
    {
    	reg->CR2 |=I2C_CR2_RELOAD;//传输字节大于255
        reg->CR2 |=I2C_CR2_NBYTES_8BIT(0xFF);
		reg->CR2 &=~I2C_CR2_AUTOEND;//非自动结束方式
		reg->CR2 |=I2C_CR2_START;//start

	    while(timeout<CONFIG_I2C_MBB_TIMEOUT)
	    {
			if(reg->ISR&I2C_ISR_TXIS)//传输字节
			{
				if(i<addr_len)
					reg->TXDR=adder_nuf[i];
				else
					reg->TXDR=buf[i-addr_len];
				i++;
			}
			else//判断出错或者超时
			{
				if(_IIC_Chek==false)
					return false;
				timeout++;
				Djy_EventDelay(1);
			}

			if(reg->ISR&I2C_ISR_TCR)//255字节传输完成
			{
				len-=255;
				if(len>255)
				{
					reg->CR2 |=I2C_CR2_NBYTES_8BIT(0xFF);
					reg->CR2 |=I2C_CR2_RELOAD;//传输字节大于255
				}
				else
				{
			    	reg->CR2 &=~I2C_CR2_NBYTES_8BIT(0xff);//清零
					reg->CR2 |=I2C_CR2_NBYTES_8BIT(len);
					reg->CR2 &=~I2C_CR2_RELOAD;//传输字节
//					reg->CR2 |=I2C_CR2_AUTOEND;//自读发送停止位

				}
			}
			//传输是结束
			if((reg->ISR&I2C_ISR_TC)||(reg->ISR&I2C_ISR_STOPF))//传输完成
			{
				__IIC_GenerateStop(reg);
				return true;
			}
	    }
	    return false;

    }
    //写字节小于255字节
    else
    {
    	reg->CR2 &=~I2C_CR2_RELOAD;//小于255字节
    	reg->CR2 &=~I2C_CR2_NBYTES_8BIT(0xff);//清零
        reg->CR2 |=I2C_CR2_NBYTES_8BIT(len);
		reg->CR2 &=~I2C_CR2_AUTOEND;//非自动结束方式
		reg->CR2 |=I2C_CR2_START;//start
	    while(timeout<CONFIG_I2C_MBB_TIMEOUT)
	    {
			if(reg->ISR&I2C_ISR_TXIS)//传输字节
			{
				if(i<addr_len)
					reg->TXDR=adder_nuf[i];
				else
					reg->TXDR=buf[i-addr_len];
				i++;
			}
			else//判断出错或者超时
			{
				if(_IIC_Chek(reg)==false)
					return false;
				timeout++;
				Djy_EventDelay(1);
			}
			//判断是否传输完成
			if((reg->ISR&I2C_ISR_TC)||(reg->ISR&I2C_ISR_STOPF))//传输完成
			{
				__IIC_GenerateStop(reg);
				if(_IIC_Chek(reg)==false)
					return false;
				return true;
			}
	    }
	    return false;
    }
}
// =============================================================================
// 功能：IIC写地址
// 参数： reg,寄存器基址
//		devaddr,器件物理地址，最后一bit区分读写
//		 mem_addr写缓冲
//       maddrlen 长度
//		Start_falg发送起始停止标着为
// 返回：true/false
// =============================================================================
static bool_t __IIC_WriteAddr(tagI2CReg *reg,u8 devaddr, u8 *mem_addr, u32 maddrlen)
{
    u32 timeout=0;
    u32 i=0;
    reg->CR2 &=~I2C_CR2_SADD_10BIT(0x7f<<1);
	reg->CR2 |=I2C_CR2_SADD_10BIT(devaddr<<1);//从器件地址
	reg->CR2 &=~I2C_CR2_RD_WRN;//写控制
	 if(reg->ISR|=I2C_ISR_STOPF)
	    	reg->ICR|=I2C_ICR_STOPCF;
	 if(reg->ISR|=I2C_ISR_NACKF)
	    	reg->ICR|=I2C_ICR_NACKCF;

	reg->CR2 &=~I2C_CR2_RELOAD;//小于255字节
	reg->CR2 &=~I2C_CR2_NBYTES_8BIT(0xff);//清零
	reg->CR2 |=I2C_CR2_NBYTES_8BIT(maddrlen);
	reg->CR2 &=~I2C_CR2_AUTOEND;//非自动结束方式
	reg->CR2 |=I2C_CR2_START;//start
	while(timeout<CONFIG_I2C_MBB_TIMEOUT)
	{
		if(reg->ISR&I2C_ISR_TXIS)//传输字节
		{
			reg->TXDR=mem_addr[i++];
		}
		else//判断出错或者超时
		{
			if(_IIC_Chek(reg)==false)
				return false;
			timeout++;
			Djy_EventDelay(1);
		}
		//判断是否传输完成
		if((reg->ISR&I2C_ISR_TC)||(reg->ISR&I2C_ISR_STOPF))//传输完成
		{
				__IIC_GenerateStop(reg);
				return(_IIC_Chek(reg));
		}
	}
	return false;

}

// =============================================================================
// 功能：IIC读
// 参数： reg,寄存器基址
//		devaddr 器件地址
//		 buf收缓冲区
//       len 长度
// 返回：true/false
// =============================================================================
static bool_t __IIC_Read(tagI2CReg *reg,u8 devaddr,u8 *buf, u32 len)
{
    u32 timeout=0,i=0;
    u32 timedef;
    reg->CR2 &=~I2C_CR2_SADD_10BIT(0x7f<<1);
	reg->CR2 |=I2C_CR2_SADD_10BIT(devaddr<<1);//从器件地址
	reg->CR2 |=I2C_CR2_RD_WRN;//读控制位
	timedef=CONFIG_I2C_TIMEOUT*len;
    if(reg->ISR|=I2C_ISR_STOPF)//清除停止位
    	reg->ICR|=I2C_ICR_STOPCF;
	if(reg->ISR|=I2C_ISR_NACKF)
	    	reg->ICR|=I2C_ICR_NACKCF;
    if(len>255)
    {
		reg->CR2 |=I2C_CR2_RELOAD;//大于255字节
		reg->CR2 |=I2C_CR2_NBYTES_8BIT(0xff);
		reg->CR2 &=~I2C_CR2_AUTOEND;//非自动结束方式
		reg->CR2 |=I2C_CR2_START;//start

	    while((timeout<timedef))
	    {
			if(reg->ISR&I2C_ISR_RXNE)//传输字节
			{
				buf[i++]=reg->RXDR;
			}
			else//判断出错或者超时
			{
				if(_IIC_Chek==false)
					return false;
				timeout++;
				Djy_EventDelay(1);
			}

			if(reg->ISR&I2C_ISR_TCR)//255字节传输完成
			{
				len-=255;
				if(len>255)
				{
					reg->CR2 |=I2C_CR2_NBYTES_8BIT(0xFF);
					reg->CR2 |=I2C_CR2_RELOAD;//传输字节大于255
				}
				else
				{
			    	reg->CR2 &=~I2C_CR2_NBYTES_8BIT(0xff);//清零
					reg->CR2 |=I2C_CR2_NBYTES_8BIT(len);
					reg->CR2 &=~I2C_CR2_RELOAD;//传输字节
//					reg->CR2 |=I2C_CR2_AUTOEND;//自读发送停止位

				}
			}
			//传输是结束
			if((!(reg->ISR&I2C_ISR_RXNE))&&    //最后一位读取完成
			((reg->ISR&I2C_ISR_TC)))
			{
				__IIC_GenerateStop(reg);
				return true;
			}
	    }
	    return false;
    }
    //读小于255字节
    else
    {
    	reg->CR2 &=~I2C_CR2_RELOAD;//小于255字节
    	reg->CR2 &=~I2C_CR2_NBYTES_8BIT(0xff);//清零
        reg->CR2 |=I2C_CR2_NBYTES_8BIT(len);
		reg->CR2 &=~I2C_CR2_AUTOEND;//非自动结束方式
	    reg->CR2 |=I2C_CR2_START;//start
//	    Djy_EventDelay(200);

	    while((timeout<timedef))
	    {
	    	if(reg->ISR&I2C_ISR_RXNE)//传输字节
			{
				buf[i++]=reg->RXDR;
			}
	    	else
	    	{
	    		if(_IIC_Chek==false)
					return false;
				timeout++;
				Djy_DelayUs(1);
	    	}
			//判断是否传输完成传输完成或者已经产生停止位
			if((!(reg->ISR&I2C_ISR_RXNE))&&    //最后一位读取完成
			(reg->ISR&I2C_ISR_TC))//(reg->ISR&I2C_ISR_STOPF)????检测stop不行
			{
				__IIC_GenerateStop(reg);//非自动方式需要手动调用
				return true;
			}
	    }
	    return false;
    }

}

// =============================================================================
// 功能：轮询方式读IIC从设备的数据
// 参数：reg,寄存器基址
//       devaddr,器件物理地址，最后一bit区分读写
//		 memaddr//寄存器地址
//		 maddrlen的寄存器地址的长度
//       addr,访问的寄存器地址
//       buf, 存储缓冲区
//       len, 存储缓冲区的长度
// 返回：len,读取成功;-1,读取失败
// =============================================================================
static s32 __IIC_ReadPoll(tagI2CReg *reg,u8 devaddr,u32 memaddr,
                        u8 maddrlen, u8 *buf, u32 len)
{

    u8 mem_addr_buf[4];
    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,memaddr);
    // 发送写器件地址和内部寄存器地址
    if(false == __IIC_WriteAddr(reg, devaddr, mem_addr_buf, maddrlen))
        return -1;

    if(false == __IIC_Read(reg, devaddr, buf,len))
    	return -1;
    __IIC_GenerateStop(reg);//非自动方式需要手动调用
    return len;
}

// =============================================================================
// 功能：轮询方式向IIC从设备的写数据
// 参数：reg,寄存器基址
//       devaddr,设备地址
//       memaddr,设备内部地址
//       maddrlen,设备内部地址长度
//       buf,存储数据
//       len,数据长度，字节
// 返回：len,读取成功;-1,读取失败
// =============================================================================
static s32 __IIC_WritePoll(tagI2CReg *reg,u8 devaddr,u32 memaddr,
                        u8 maddrlen, u8 *buf, u32 len)
{
    u8 mem_addr_buf[4];
    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,memaddr);

    if( __IIC_Write(reg, devaddr, mem_addr_buf, maddrlen,buf,len)==false)//写存储器地址
	   return -1;
//	if( __IIC_Write(reg, devaddr, buf, len,stop)==false)//写存储器地址
//		   return -1;
//	__IIC_GenerateStop(reg);//非自动方式需要手动调用
	return len;

}

// =============================================================================
// 功能：轮询方式读写IIC设备
// 参数：reg,寄存器基址
//       DevAddr,设备地址
//       MemAddr,设备内部地址
//       MemAddrLen,设备内部地址长度
//       Buf,存储数据
//       Length,数据长度，字节
//       WrRdFlag,读写标记，为0时写，1时为读
// 返回：
// =============================================================================
static bool_t __IIC_WriteReadPoll(tagI2CReg *reg,u8 DevAddr,u32 MemAddr,\
                                u8 MemAddrLen,u8* Buf, u32 Length,u8 WrRdFlag)
{
    __IIC_IntDisable(reg);
    if(WrRdFlag == CN_IIC_WRITE_FLAG)   //写
    {
        if(Length == __IIC_WritePoll(reg,DevAddr,MemAddr,MemAddrLen,Buf,Length))
            return true;
        else
            return false;
    }
    else                                //读
    {
        if(Length == __IIC_ReadPoll(reg,DevAddr,MemAddr,MemAddrLen,Buf,Length))
            return true;
        else
            return false;
    }
}

// =============================================================================
// 功能: 启动写时序，启动写时序的过程为：器件地址（写）、存储地址（写），当存储地址完
//       成时，需打开中断，之后将会发生发送中断，在中断服务
//       函数中，每次发送一定的字节数，直到len数据量，post信号量iic_bus_semp，并产生
//       停止时序
// 参数: SpecificFlag,个性标记，本模块内即IIC寄存器基址
//      DevAddr,器件地址的前7比特，已将内部地址所占的bit位更新，该函数需将该地址左
//               移一位增加增加最后一位读/写比特;
//      MemAddr,存储器内部地址，即发送到总线上的地址，该地址未包含放在器件地址上的
//               比特位；
//      MenAddrLen,存储器内部地址的长度，字节单位，未包含在器件地址里面的比特位；
//      Length,接收的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，
//          当接收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//      IIC_BusSemp,发送完成时驱动需释放的信号量。发送程序读IIC_PortRead时，若读不到数
//          则需释放该信号量，产生结束时序
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
static bool_t __IIC_GenerateWriteStart(tagI2CReg *reg,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct SemaphoreLCB *iic_semp)
{
    u8 mem_addr_buf[4];
    u8 iic_port;
    //通过specific_flag区分是哪条总线
    if(reg==tg_I2c_Reg[CN_IIC1])
    	iic_port=CN_IIC1;
    else if (reg==tg_I2c_Reg[CN_IIC2])
    	iic_port=CN_IIC2;
    else if (reg==tg_I2c_Reg[CN_IIC3])
    	iic_port=CN_IIC3;
    else if (reg==tg_I2c_Reg[CN_IIC4])
    	iic_port=CN_IIC4;
    else
    	return false;

    IntParamset[iic_port].TransTotalLen = length;
	IntParamset[iic_port].TransCount = 0;
	IntParamset[iic_port].pDrvPostSemp = iic_semp;           //iic_bus_semp

    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,mem_addr);
    __IIC_IntDisable(reg);                 //关中断
    //先发送器件地址，内部地址，然后发送第一个数据
    if(__IIC_WriteAddr(reg, dev_addr, mem_addr_buf,maddr_len) )
	{
		reg->CR2 &=~I2C_CR2_RD_WRN;//写标志
    	reg->CR2 |=I2C_CR2_RELOAD;//传输字节大于255
        reg->CR2 |=I2C_CR2_NBYTES_8BIT(0xFF);
		reg->CR2 &=~I2C_CR2_AUTOEND;//非自动结束方式
		__IIC_IntEnable(reg);//使能中断
		reg->CR2 |=I2C_CR2_START;//start
		return true;
	}

    return false;
}

// =============================================================================
// 功能: 启动读时序，启动读时序的过程为：器件地址（写）、存储地址（写）、器件地址（读）
//       当器件地址（读）完成时，需打开中断，重新配置寄存器为接收模式，之后将会发生
//       接收数据中断，在中断中将接收到的数据调用IIC_PortWrite写入缓冲，接收到len字
//       节数的数据后，释放信号量iic_semp
// 参数: SpecificFlag,个性标记，本模块内即IIC寄存器基址
//      DevAddr,器件地址的前7比特，已将内部地址所占的bit位更新，该函数需将该地址左
//               移一位增加增加最后一位读/写比特;
//     MemAddr,存储器内部地址，即发送到总线上的地址，该地址未包含放在器件地址上的
//               比特位；
//      MenAddrLen,存储器内部地址的长度，字节单位，未包含在器件地址里面的比特位；
//      Length,接收的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，当接
//             收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//     IIC_BusSemp,读完成时，驱动需释放的信号量（缓冲区信号量）
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
static bool_t __IIC_GenerateReadStart( tagI2CReg *reg,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct SemaphoreLCB *iic_semp)
{
    u8 mem_addr_buf[4];
    u8 iic_port;
    if(reg==tg_I2c_Reg[CN_IIC1])
    	iic_port=CN_IIC1;
    else if(reg==tg_I2c_Reg[CN_IIC2])
    	iic_port=CN_IIC2;
    else if(reg==tg_I2c_Reg[CN_IIC3])
    	iic_port=CN_IIC3;
    else if(reg==tg_I2c_Reg[CN_IIC4])
    	iic_port=CN_IIC4;
    else
    	return false;

	IntParamset[iic_port].TransTotalLen = length;
	IntParamset[iic_port].TransCount = 0;
	IntParamset[iic_port].pDrvPostSemp = iic_semp;               //iic_buf_semp

    fill_little_32bit(mem_addr_buf,0,mem_addr);
    __IIC_IntDisable(reg);//关中断

    //写器件地址和寻址地址
    //写器件地址和寻址地址
    if(__IIC_WriteAddr(reg, dev_addr, mem_addr,maddr_len) )
	{
		reg->CR2 &=~I2C_CR2_AUTOEND;//非自动结束方式
		reg->CR2 |=I2C_CR2_RD_WRN;//读标志
		__IIC_IntEnable(reg);//使能中断
		reg->CR2 |=I2C_CR2_START;//start
		return true;
	}
    return false;
}

// =============================================================================
// 功能: 结束本次读写回调函数，区分读写的不同停止时序，当属于发送时，则直接停止时序，
//      若为读，则先停止回复ACK，再停止
// 参数: reg,个性标记，本模块内即IIC寄存器基址
// 返回: 无
// =============================================================================
static void __IIC_GenerateEnd(tagI2CReg *reg)
{
    if(reg == NULL)
        return;
    if(reg->CR2 & I2C_CR2_RD_WRN )//发送
    {
    	reg->CR1 |= I2C_CR2_NACK;//停止回复ACK
    	__IIC_GenerateStop(reg);
    }
    else //接收
    {
        __IIC_GenerateStop(reg);
    }
}

//==============================================================================
// 功能：IIC总线控制回调函数，被上层调用，目前只实现对IIC时钟频率配置
// 参数：TIMINGR-->位 31:28 PRESC时序预分频因子
//				   位 23:20 SCLDEL[3:0]：数据建立时间
//				   位 19:16 SDADEL[3:0]：数据保持时间
//				   位 15:8 SCLH[7:0]：SCL 高电平周期
//				   位 7:0 SCLL[7:0]：SCL 低电平周期
//   reg    本模块内即IIC寄存器基址
// 返回：无
//==============================================================================
static void _IIC_ClkSet(tagI2CReg *reg,u32 data)
{
	u32 SCLH,SCLL;
	reg->CR1 &=~I2C_CR1_PE;
	SCLH=(216*1000*1000)/(data*2);
	SCLL=SCLH;
	reg->TIMINGR&=~((I2C_TIMINGE_SCLH_8BIT(0xff)|I2C_TIMINGE_SCLL_8BIT(0xff)));
	reg->TIMINGR|=(I2C_TIMINGE_SCLH_8BIT(SCLH)|I2C_TIMINGE_SCLL_8BIT(SCLL));
	reg->CR1 |=I2C_CR1_PE;//PE使能
}

// =============================================================================
// 功能：IIC总线控制回调函数，被上层调用，目前只实现对IIC时钟频率配置
// 参数：reg,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：无
// =============================================================================
static s32 __IIC_BusCtrl(tagI2CReg *reg,u32 cmd,u32 data1,u32 data2)
{
    if( reg == NULL )
        return 0;
    switch(cmd)
    {
		case CN_IIC_SET_CLK: //设置时钟频率
				_IIC_ClkSet(reg,data1);
			  break;
		case CN_IIC_DMA_USED://使用dma传输

			break;
		case CN_IIC_DMA_UNUSED://禁止dma传输

			break;
		case CN_IIC_SET_POLL:           //使用轮询方式发送接收
			__IIC_IntDisable(reg);
			break;
		default:
			return 0;
    }
    return 1;
}

// =============================================================================
// 功能：        IIC接收与发送中断服务函数。该函数实现的功能如下：
//       1.每发送与接收一个或若干字节发生一次中断；
//       2.若有多个中断使用同一个中断号，则需根据具体情况区分使用的是哪个中断；
//       3.清中断标志，并判断ACK信号，每读写字节，计数器都需相应修改；
//       4.接收达到倒数第一个字节时，需配置不发送ACK信号；
//       5.接收或发送完成时，需post信号量IntParam->pDrvPostSemp；
//       6.接收或发送完成时，需产生停止时序。
// 参数：i2c_int_line,中断号，本函数没用到
// 返回：true falst
// =============================================================================
static u32 __IIC_ISR(ufast_t i2c_int_line)
{
    static struct IIC_CB *ICB;
    static struct IIC_IntParamSet *IntParam;
    tagI2CReg *reg;
    u8 iic_port;
    u8 ch;
    u32 IicErrorNo;
    switch (i2c_int_line)
    {
		case CN_INT_LINE_I2C1_EV:
			iic_port=CN_IIC1;
			break;
		case CN_INT_LINE_I2C2_EV:
			iic_port=CN_IIC2;
			break;
		case CN_INT_LINE_I2C3_EV:
			iic_port=CN_IIC3;
			break;
		case CN_INT_LINE_I2C4_EV:
			iic_port=CN_IIC4;
			break;
		default:	return false;
	}
	reg = (tagI2CReg*)tg_I2c_Reg[iic_port];
	ICB = &s_IIC_CB[iic_port];
	IntParam = &IntParamset[iic_port];

	//判断中断方式
	//清除中断
	//发送或者接受数据
	//出错检查
	if(reg->ISR & I2C_ISR_TXIS) //发送中断
	{
		if(IIC_PortRead(ICB,&ch,1) > 0)
		{
			reg->TXDR = ch;
			IntParam->TransCount ++;;
		}
		else if(IntParam->TransCount == IntParam->TransTotalLen)
		{
		  Lock_SempPost(IntParam->pDrvPostSemp);
		  __IIC_IntDisable(reg);//关中断
		  __IIC_GenerateStop(reg);
		}
	}
	else if(reg->ISR & I2C_ISR_RXNE)//接收中断
	{
		while((IntParam->TransCount < IntParam->TransTotalLen))
		{
			// 最后一个字节master不发ACK，表示读操作终止
			if(IntParam->TransCount == IntParam->TransTotalLen - 1)
			{
				reg->CR2 |= I2C_CR2_NACK;
			}

			while (reg->ISR&I2C_ISR_RXNE)//传输字节
				ch = reg->RXDR;
			//写数据
			IIC_PortWrite(ICB,&ch,1);
			IntParam->TransCount ++;
		}
		if((IntParam->TransCount == IntParam->TransTotalLen)  &&
									(!(reg->ISR&I2C_ISR_RXNE)))//最后一位读取完成
		{
			__IIC_GenerateStop(reg);
			__IIC_IntDisable(reg);//关中断
			Lock_SempPost(IntParam->pDrvPostSemp);//释放总线信号量
		}

	}

    else//未启动通信
    {
        if(reg->ISR & (I2C_ISR_ARLO))//仲裁丢失中断
        {
            reg->ICR = I2C_ICR_ARLOCF;//清除仲裁丢失中断标志位
            IicErrorNo = CN_IIC_POP_MAL_LOST_ERR;
            IIC_ErrPop(ICB,IicErrorNo);
        }
        if(reg->ISR & (I2C_ISR_NACKF))
        {
            reg->ICR = I2C_ICR_ARLOCF;//清除仲裁丢失中断标志位
            IicErrorNo = CN_IIC_POP_NO_ACK_ERR;
            IIC_ErrPop(ICB,IicErrorNo);
        }
    }

    return true;
}

// =============================================================================
// 功能：IIC底层驱动的初始化，完成整个IIC总线的初始化，其主要工作如下：
//       1.初始化总线控制块IIC_CB，回调函数和缓冲区的初始化赋值；
//       2.默认的硬件初始化，如GPIO或IIC寄存器等；
//       3.中断初始化，完成读写中断配置；
//       4.调用IICBusAdd或IICBusAdd_r增加总线结点；
// 参数：iic_port  IIC编号 CN_IIC1
//	   Add_mode 寻址模式
// 返回：无
// =============================================================================
bool_t IIC_Init(u8 iic_port)
{

    struct IIC_Param IIC_Config[CN_IIC_NUM];

    IIC_Config[iic_port].BusName             = (char *)IIC_Name[iic_port];
    IIC_Config[iic_port].IICBuf              = (u8*)s_IICBuf[iic_port];
    IIC_Config[iic_port].IICBufLen           = IIC_BUF_LEN[iic_port];
    IIC_Config[iic_port].SpecificFlag        = tg_I2c_Reg[iic_port];
    IIC_Config[iic_port].pGenerateWriteStart = (WriteStartFunc)__IIC_GenerateWriteStart;
    IIC_Config[iic_port].pGenerateReadStart  = (ReadStartFunc)__IIC_GenerateReadStart;
    IIC_Config[iic_port].pGenerateEnd        = (GenerateEndFunc)__IIC_GenerateEnd;
    IIC_Config[iic_port].pBusCtrl            = (IICBusCtrlFunc)__IIC_BusCtrl;
    IIC_Config[iic_port].pWriteReadPoll      = (WriteReadPoll)__IIC_WriteReadPoll;

    __IIC_GpioConfig(iic_port);
    __IIC_IntConfig(iic_port,__IIC_ISR);

    if(NULL == IIC_BusAdd_s(&s_IIC_CB[iic_port],&IIC_Config[iic_port]))
        return 0;
    return 1;
}


