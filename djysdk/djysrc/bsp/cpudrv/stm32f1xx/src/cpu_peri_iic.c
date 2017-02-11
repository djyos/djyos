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
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 17/09.2014
// =============================================================================
#include "cpu_peri_gpio.h"
#include "djyos.h"
#include "stdint.h"
#include "../../../arch/arm/arm32_stdint.h"
#include "stdio.h"
#include "cpu_peri.h"
#include "iicbus.h"
#include "endian.h"
#include "lock.h"
#include "int.h"
#include "cpu_peri_iic.h"
#include "cpu_peri_rcc.h"
// =============================================================================
#define CN_IIC1_BASE        (0x40005400u)//无符号地址
#define CN_IIC2_BASE        (0x40005800u)//无符号地址

static struct IIC_CB s_IIC1_CB;
static struct IIC_CB s_IIC2_CB;

#define IIC1_BUF_LEN  128
#define IIC2_BUF_LEN  128

struct IIC_IntParamSet
{
    struct SemaphoreLCB *pDrvPostSemp;   //信号量
    u32 TransCount;       //传输数据量计数器
    u32 TransTotalLen;//接收的数据总量
};

static struct IIC_IntParamSet IntParamset0;
static struct IIC_IntParamSet IntParamset1;

static int __iic_wait_addr(volatile tagI2CReg *dev, int write)
{
    uint16_t csr;
    unsigned timeout = 0;
    do
    {
        Djy_DelayUs(1);
        csr = dev->SR1;
        csr|=(u16)(dev->SR2<<16);
        if (!(csr & I2C_SR1_ADDR_MASK))//地址没有发送发送
            continue;

        csr=dev->SR1;   /*清状态寄存器标志*/
        if (csr & I2C_SR1_ARLO_MASK)       /*如果仲裁丢失*/
            {
            #ifdef DEBUG
            printk("__i2c_wait: ARLO\n");
            #endif
            return -1;
            }

        if (write == I2C_WRITE_BIT) /*写时*/
            {
            if( (csr & I2C_SR1_AF_MASK))/*判断ack*/
              {
                #ifdef DEBUG
                printk("__i2c_wait: No RXACK\n");
                #endif
                return -1;
              }
            }

        return 0;
    } while (timeout++ < CONFIG_I2C_TIMEOUT);//0.01s超时

    #ifdef DEBUG
    printk("__i2c_wait: timed out\n");
    #endif
    return -1;
}

// =============================================================================
// 函数：IIC寻址写器件地址

// 功能：寻址iic设备，即往总线发iic从设备地址，最后一个bit表示读/写位
// 参数： dev,设备指针
//     devaddr,器件物理地址，最后一bit区分读写
//     dir,读写标记，为0时为写，为1时为读
//     rsta,是否为重新启动
// 返回：1,成功；0,失败
// =============================================================================

static int __iic_write_addr (volatile tagI2CReg *dev,
                            u8 devaddr, u8 dir)
{
    unsigned time = 0;
    dev->CR1 |=  I2C_CR1_PE_MASK;/*使能 IIC start*/
    dev->CR1 |=  I2C_CR1_START_MASK;
    while(!(dev->SR1&I2C_SR1_SB_MASK))//没有发送起始条件条件//
    {
        Djy_DelayUs(1);
        if(time++>(CONFIG_I2C_MBB_TIMEOUT))//????
            return 0;   //超时返回
    }

    dev->DR = (devaddr << 1) | dir;    /*写 地 址*/

    if (__iic_wait_addr(dev,I2C_WRITE_BIT) < 0)
        return 0;

    return 1;
}

// =============================================================================
// 函数：发送数据
// 功能：发送数据到iic从器件，已经完成了iic总线寻址和内部地址，写有效数据
// 参数： dev,器件指针
//       data,数据缓冲区指针
//       length,data的数据量大小，字节单位
// 返回：发送数据量，字节单位
// =============================================================================
static int __iic_write(volatile tagI2CReg *dev, u8 *data, int length)
{
    int i;

    //将所有数据循环发送，每次发送一字节数据，须等待ack等完成信息
    for (i = 0; i < length; i++)
    {
        dev->DR = data[i];
        while(dev->SR1&I2C_SR1_BTF_MASK);
    }
    return i;
}

// =============================================================================
// 功能: 使能iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __IIC_IntEnable(volatile tagI2CReg *reg)
{
    reg->CR2 |= I2C_CR2_ITEVTEN_MASK| I2C_CR2_ITBUFEN_MASK;

}

// =============================================================================
// 功能: 禁止iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __IIC_IntDisable(volatile tagI2CReg *reg)
{
    reg->CR2 &= ~(I2C_CR2_ITEVTEN_MASK| I2C_CR2_ITBUFEN_MASK);
}

// =============================================================================
// 功能：初始化时钟配置,ppc1020提供两个独立i2c，其时钟来自CCB/2，再经过i2c时钟分频，
//        配置I2CFDR寄存器，分频系数并无特定的计算方法，本API通过枚举参数确定四种可
//        配置的波特率.
// 参数：reg，寄存器指针
//      i2cclk，枚举量，I2C_CLOCK_RATE_50K，时钟为50K
// 返回：true=成功，false=失败
//Thigh = CCR ×TPCLK1
//Tlow = 2 × CCR × TPCLK1
//如果DUTY = 1： (速度达到400kHz)
//Thigh = 9 × CCR ×TPCLK1
//Tlow = 16 × CCR ×TPCLK1
// =============================================================================
static void __IIC_SetClk(volatile tagI2CReg *reg,u32 i2cclk)
{
    switch (i2cclk)
    {
            case CN_IIC_SPEED_50KHZ:    //24M    Thigh = CCR ×TPCLK1  Tlow = 2 × CCR × TPCLK1
                 pg_iic1_reg->CCR     = 240;//快速/标准模式下的时钟控制分频系数(主模式)
                break;
            case CN_IIC_SPEED_100KHZ:
                 pg_iic1_reg->CCR     = 120;//快速/标准模式下的时钟控制分频系数(主模式)
                break;
            case CN_IIC_SPEED_200KHZ:
                 pg_iic1_reg->CCR     = 60;//快速/标准模式下的时钟控制分频系数(主模式)
                break;
            case CN_IIC_SPEED_300KHZ:
                pg_iic1_reg->CCR     =  40;//快速/标准模式下的时钟控制分频系数(主模式)
                break;
            case CN_IIC_SPEED_400KHZ:
                 pg_iic1_reg->CCR     = 30;//快速/标准模式下的时钟控制分频系数(主模式)
                break;
            default:
            break;
    }
}

// =============================================================================
// 功能: I/O模拟的方式 释放没有复位和使能引脚的IIC器件
// 参数: port I/O口 如 CN_GPIO_B
//      TS_SDA:引脚号
//      TS_SCK:引脚号
// 返回: true/false
// =============================================================================
static bool_t _IIC_Busfree(u32 port,u32 sda_pin,u32 sck_pin)
{
     u32 timeout=0;
     GPIO_CfgPinFunc(port,sda_pin,CN_GPIO_MODE_IN_FLOATING);//TS_SDA
     GPIO_CfgPinFunc(port,sck_pin,CN_GPIO_MODE_GPIO_OUT_OD_50Mhz);//TS_SCK

     while(1)
     {
          timeout++;
          GPIO_SettoLow(port,1<<sck_pin);
          Djy_DelayUs(10);

          GPIO_SettoHigh(port,1<<sck_pin);
          Djy_DelayUs(10);

          if(timeout>=CONFIG_I2C_TIMEOUT)
              return false;
          if( GPIO_GetData(port)&(1<<sda_pin))
              break;
    }

    GPIO_CfgPinFunc(port,sda_pin,CN_GPIO_MODE_GPIO_OUT_OD_50Mhz);//TS_SDA
    //产生停止信号 iic总线释放
    GPIO_SettoLow(port,1<<sda_pin);
    Djy_DelayUs(10);
    GPIO_SettoHigh(port,1<<sda_pin);
    Djy_DelayUs(10);

    return true;
}

// =============================================================================
// 功能: 硬件GPIO初始化，包括电源等
// 参数: IIC_NO,编号
// 返回: 无
// =============================================================================
static bool_t __IIC_GpioConfig(u8 IIC_NO)
{
    u8 tout;
    pg_rcc_reg->APB2ENR |=(1<<3)|(1<<0);             //开时钟B和复用功能
    switch(IIC_NO)
    {
    case CN_IIC1:
        pg_rcc_reg->APB1ENR |=(1<<21);               //开时钟
        pg_iic1_reg->CR1     |=I2C_CR1_SWRST_MASK;       /* 复位IIC外设   */
        for (tout = 100; tout; tout--);
        pg_iic1_reg->CR1     = 0x0000;
        //对于一些 IIC器件它们没有复位或者使能管脚，像stmpe811 在工作过程中，如果在数据端口为低电平时发生cpu复位
        //将导致iic总线被从器件一直占用 在这里采用I/O模拟的方式产生停止信号释放IIC总线
        if(_IIC_Busfree(CN_GPIO_B,9,8)==false)
            return false;

        GPIO_CfgPinFunc(CN_GPIO_B,9,CN_GPIO_MODE_PERI_OUT_OD_50Mhz);//TS_SDA
        GPIO_CfgPinFunc(CN_GPIO_B,8,CN_GPIO_MODE_PERI_OUT_OD_50Mhz);//TS_SCK

        pg_iic1_reg->CR1     = 0x0001;//PE使能
        pg_iic1_reg->CR2     = 24;    //I2C模块时钟频率  24M
        pg_iic1_reg->CR1     = 0x0000;//PE使能
        pg_iic1_reg->TRISE   = 0x0025;//在快速/标准模式下的最大上升时间(主模式)
        pg_iic1_reg->CCR     = 0x00b4;//快速/标准模式下的时钟控制分频系数(主模式)
        pg_iic1_reg->CR1    |= 0x0401;//PE使能&ACK
        pg_iic1_reg->OAR1    = 0x40A0;//4必须始终由软件保持为’1’。A接口地址
        pg_iic1_reg->CCR|=I2C_CCR_FS_MASK;//快速模式
        break;
    case CN_IIC2:
        pg_rcc_reg->APB1ENR |=(1<<22);//开时钟

        if(_IIC_Busfree(CN_GPIO_B,11,10)==false)
            return false;

        GPIO_CfgPinFunc(CN_GPIO_B,11,CN_GPIO_MODE_PERI_OUT_OD_50Mhz);//TS_SDA
        GPIO_CfgPinFunc(CN_GPIO_B,10,CN_GPIO_MODE_PERI_OUT_OD_50Mhz);//TS_SCK

        pg_iic2_reg->CR1     |= I2C_CR1_SWRST_MASK;    /* 复位IIC外设   */
          for (tout = 100; tout; tout--);
        pg_iic2_reg->CR1     = 0x0000;
        pg_iic2_reg->CR1    |= I2C_CR1_PE_MASK;
        pg_iic2_reg->CR2     = 24;
        pg_iic2_reg->CR1     = 0x0000;
        pg_iic2_reg->TRISE   = 0x0025;
        pg_iic2_reg->CCR     = 0x0078;
        pg_iic2_reg->CR1    |= 0x0401;
        pg_iic2_reg->OAR1    = 0x40A0;
        pg_iic2_reg->CCR|=I2C_CCR_FS_MASK;//快速模式
        break;
    default:
        break;
    }
    return true;
}


// =============================================================================
// 功能: IIC中断配置函数
// 参数: IntLine,中断线
//       isr,中断服务函数指针
// 返回: 无
// =============================================================================
static void __IIC_IntConfig(u32 IntLine,u32 (*isr)(ptu32_t))
{
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
static void __IIC_GenerateStop(volatile tagI2CReg *reg)
{
    reg->CR1 |=(I2C_CR1_STOP_MASK);//产生停止条件

}
// =============================================================================
// 功能: 读取相应寄存器的值
// 参数: reg iic外设寄存器基址  buf 读数据缓冲区  len读数据长度
// 返回: 读数据长度单位字节
// =============================================================================

static u16 __iic_read(volatile tagI2CReg *reg, u8 *buf, u32 len)
{
    u8 i=0;
    while(len--)//接收数据
    {  /*  是否进行ACK应答  */
    if (len>=1)                     reg->CR1 |=  I2C_CR1_ACK_MASK;
    else                            reg->CR1 &= ~I2C_CR1_ACK_MASK;
    while (!(reg->SR1 & I2C_SR1_RxNE_MASK));//等待接收完成

        buf[i++] = reg->DR;

    }
    return i;

}

// =============================================================================
// 功能：轮询方式读IIC从设备的数据
// 参数：reg,寄存器基址
//       devaddr,器件物理地址，最后一bit区分读写
//       memaddr//寄存器地址
//       maddrlen的寄存器地址的长度
//       addr,访问的寄存器地址
//       buf, 存储缓冲区
//       len, 存储缓冲区的长度
// 返回：len,读取成功;-1,读取失败
// =============================================================================
static s32 __IIC_ReadPoll(volatile tagI2CReg *reg,u8 devaddr,u32 memaddr,
                        u8 maddrlen, u8 *buf, u32 len)
{

    u8 mem_addr_buf[4];
    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,memaddr);

    // 发送写器件地址
    if(0 == __iic_write_addr(reg,devaddr,I2C_WRITE_BIT))
        return -1;

    // 发送器件寄存器地址

    if(maddrlen != __iic_write(reg,mem_addr_buf,maddrlen))
        return -1;

    //发送读器件地址
    if(0 == __iic_write_addr(reg,devaddr,I2C_READ_BIT))
        return -1;

    //接受寄存器数据
    if(len != __iic_read(reg,buf,len))
            return -1;

    reg->CR1 = I2C_CR1_STOP_MASK;   // STOP
    // stop
//    reg->CR1 &= ~I2C_CR1_PE_MASK;    // Disable IIC

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
static s32 __IIC_WritePoll(volatile tagI2CReg *reg,u8 devaddr,u32 memaddr,
                        u8 maddrlen, u8 *buf, u32 len)
{
    u8 mem_addr_buf[4];
    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,memaddr);

    // Write Device Address
    if(0 == __iic_write_addr(reg,devaddr,I2C_WRITE_BIT))
        return -1;

    // Write Memory Address
    if(maddrlen != __iic_write(reg,mem_addr_buf,maddrlen))
        return -1;

    //Write Data
    if(len != __iic_write(reg,buf,len))
        return -1;

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
//       成时，需打开中断，重新配置寄存器为接收模式，之后将会发生发送中断，在中断服务
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
    //通过specific_flag区分是哪条总线
    if((u32)reg == CN_IIC1_BASE)
    {
        IntParamset0.TransTotalLen = length;
        IntParamset0.TransCount = 0;
        IntParamset0.pDrvPostSemp = iic_semp;           //iic_bus_semp
    }
    else if((u32)reg == CN_IIC2_BASE)
    {
        IntParamset1.TransTotalLen = length;
        IntParamset1.TransCount = 0;
        IntParamset1.pDrvPostSemp = iic_semp;
    }
    else
    {
        return false;
    }

    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,mem_addr);
    __IIC_IntDisable(reg);                 //关中断

    //先发送器件地址，内部地址，然后发送第一个数据

        if(__iic_write_addr(reg, dev_addr, I2C_WRITE_BIT) != 0)
        {
            //发送器件内部地址
            if(maddr_len - 1 == __iic_write(reg,mem_addr_buf,maddr_len-1))
            {
                reg->DR = mem_addr_buf[maddr_len-1];
                __IIC_IntEnable(reg);
                return true;
            }
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

    if((u32)reg == CN_IIC1_BASE)
    {
        IntParamset0.TransTotalLen = length;
        IntParamset0.TransCount = 0;
        IntParamset0.pDrvPostSemp = iic_semp;               //iic_buf_semp
    }
    else if((u32)reg == CN_IIC2_BASE)
    {
        IntParamset1.TransTotalLen = length;
        IntParamset1.TransCount = 0;
        IntParamset1.pDrvPostSemp = iic_semp;
    }
    else
    {
        return false;
    }

    fill_little_32bit(mem_addr_buf,0,mem_addr);
    __IIC_IntDisable(reg);//关中断

    //写器件地址和寻址地址
           if(__iic_write_addr(reg, dev_addr, I2C_WRITE_BIT) != 0)
        {
            if(__iic_write(reg, mem_addr_buf, maddr_len) == maddr_len)//接收完成
            {
                if(length && __iic_write_addr(reg,dev_addr, I2C_READ_BIT) != 0)
                {
                    reg->CR1 |=  I2C_CR1_ACK_MASK;
                    __IIC_IntEnable(reg);//使能中断
                    return true;
                }
            }
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
    if((reg->SR1 & I2C_SR1_TxE_MASK )&& (reg->SR1 & I2C_SR1_BTF_MASK))//发送
    {
        __IIC_GenerateStop(reg);
    }
    else //接收
    {
        reg->CR1 &= ~(I2C_CR1_ACK_MASK);//停止回复ACK
        __IIC_GenerateStop(reg);
    }
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
        __IIC_SetClk(reg,data1);
        break;
    case CN_IIC_DMA_USED://使用dma传输

        break;
    case CN_IIC_DMA_UNUSED://禁止dma传输

        break;
    case CN_IIC_SET_POLL:           //使用轮询方式发送接收
        __IIC_IntDisable(reg);
        break;
    case CN_IIC_SET_INT:         //使用中断方式发送接收
        __IIC_IntEnable(reg);
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
    u8 ch;
    u32 IicErrorNo;
    switch (i2c_int_line)
    {
        case CN_INT_LINE_I2C1_EV:
                reg = (tagI2CReg*)CN_IIC1_BASE;
                ICB = &s_IIC1_CB;
                IntParam = &IntParamset0;
                break;
        case CN_INT_LINE_I2C2_EV:
                reg = (tagI2CReg*)CN_IIC2_BASE;
                ICB = &s_IIC2_CB;
                IntParam = &IntParamset1;
                break;
        default:
                return false;
    }

    if(reg->SR1 & I2C_SR1_BTF_MASK)        //已经启动传输
    {
        if(reg->SR1 & I2C_SR1_TxE_MASK)      //发送中断
        {
            if(!(reg->SR1 & I2C_SR1_RxNE_MASK))
            {
                //从发送缓冲区读一个字节的数据，并发送
                if(IIC_PortRead(ICB,&ch,1) > 0)
                {
                    reg->DR = ch;
                    IntParam->TransCount ++;;
                }
                else if(IntParam->TransCount == IntParam->TransTotalLen)
                {
                  Lock_SempPost(IntParam->pDrvPostSemp);
                  __IIC_IntDisable(reg);//关中断
                  __IIC_GenerateStop(reg);
                }
            }
            else                        //未收到ACK信号
            {
                IicErrorNo = CN_IIC_POP_NO_ACK_ERR;//调用错处处理API函数
                IIC_ErrPop(ICB,IicErrorNo);
                return 1;
            }
        }
        else                            //接收中断
        {
            while((IntParam->TransCount < IntParam->TransTotalLen))
            {
                // 最后一个字节master不发ACK，表示读操作终止
                if(IntParam->TransCount == IntParam->TransTotalLen - 1)
                {
                    reg->CR1 &=~ I2C_CR1_ACK_MASK;
                }
                while (!(reg->SR1 & I2C_SR1_RxNE_MASK));//等待接收完成
                    ch = reg->DR;
                //写数据
                IIC_PortWrite(ICB,&ch,1);
                IntParam->TransCount ++;
            }
            if((IntParam->TransCount == IntParam->TransTotalLen)  &&
                                        (reg->SR1 & I2C_SR1_BTF_MASK))
            {
                __IIC_GenerateStop(reg);
                __IIC_IntDisable(reg);//关中断
                Lock_SempPost(IntParam->pDrvPostSemp);//释放总线信号量
            }
        }
    }
    else//未启动通信
    {
        if(reg->SR1 & I2C_SR1_ARLO_MASK)//仲裁丢失中断
        {
            reg->SR1 &= ~I2C_SR1_ARLO_MASK;//清除仲裁丢失中断标志位
            IicErrorNo = CN_IIC_POP_MAL_LOST_ERR;
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
// 参数：para,无具体意义
// 返回：无
// =============================================================================
bool_t IIC1_Init(void)
{
    static u8 s_IIC1Buf[IIC1_BUF_LEN];
    struct IIC_Param IIC1_Config;

    IIC1_Config.BusName             = "IIC1";
    IIC1_Config.IICBuf              = (u8*)&s_IIC1Buf;
    IIC1_Config.IICBufLen           = IIC1_BUF_LEN;
    IIC1_Config.SpecificFlag        = CN_IIC1_BASE;
    IIC1_Config.pGenerateWriteStart = (WriteStartFunc)__IIC_GenerateWriteStart;
    IIC1_Config.pGenerateReadStart  = (ReadStartFunc)__IIC_GenerateReadStart;
    IIC1_Config.pGenerateEnd        = (GenerateEndFunc)__IIC_GenerateEnd;
    IIC1_Config.pBusCtrl            = (IICBusCtrlFunc)__IIC_BusCtrl;
    IIC1_Config.pWriteReadPoll      = (WriteReadPoll)__IIC_WriteReadPoll;

    __IIC_GpioConfig(CN_IIC1);
    __IIC_IntConfig(CN_INT_LINE_I2C1_EV,__IIC_ISR);

    if(NULL == IIC_BusAdd_s(&s_IIC1_CB,&IIC1_Config))
        return 0;
    return 1;
}

// =============================================================================
// 功能：IIC底层驱动的初始化，完成整个IIC总线的初始化，其主要工作如下：
//       1.初始化总线控制块IIC_CB，回调函数和缓冲区的初始化赋值；
//       2.默认的硬件初始化，如GPIO或IIC寄存器等；
//       3.中断初始化，完成读写中断配置；
//       4.调用IICBusAdd或IICBusAdd_r增加总线结点；
// 参数：para,无具体意义
// 返回：1成功  0失败
// =============================================================================
bool_t IIC2_Init(void)
{
    static u8 s_IIC2Buf[IIC2_BUF_LEN];
    struct IIC_Param IIC2_Config;

    IIC2_Config.BusName             = "IIC2";   //总线名称，如IIC2
    IIC2_Config.IICBuf              = (u8*)&s_IIC2Buf; //总线缓冲区指针
    IIC2_Config.IICBufLen           = IIC2_BUF_LEN; //总线缓冲区大小，字节
    IIC2_Config.SpecificFlag        = CN_IIC2_BASE; //指定标记，如IIC寄存器基址
    IIC2_Config.pGenerateWriteStart = (WriteStartFunc)__IIC_GenerateWriteStart; //写过程启动
    IIC2_Config.pGenerateReadStart  = (ReadStartFunc)__IIC_GenerateReadStart;//读启动过程
    IIC2_Config.pGenerateEnd        = (GenerateEndFunc)__IIC_GenerateEnd;//结束通信
    IIC2_Config.pBusCtrl            = (IICBusCtrlFunc)__IIC_BusCtrl;//控制函数
    IIC2_Config.pWriteReadPoll      = (WriteReadPoll)__IIC_WriteReadPoll;//轮询或中断未开时使用

    if(false==__IIC_GpioConfig(CN_IIC2))
        return 0;
    __IIC_IntConfig(CN_INT_LINE_I2C2_EV,__IIC_ISR);

    if(NULL == IIC_BusAdd_s(&s_IIC2_CB,&IIC2_Config))
        return 0;
    return 1;
}

