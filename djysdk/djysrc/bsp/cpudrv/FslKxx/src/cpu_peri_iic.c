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

#include "stdio.h"
#include "cpu_peri.h"
#include "iicbus.h"
#include "endian.h"
#include "lock.h"

// =============================================================================
#define CN_IIC0_BASE        (0x40066000u)
#define CN_IIC1_BASE        (0x40067000u)

#define tagI2CReg            I2C_Type

static struct tagIIC_CB s_IIC0_CB;
static struct tagIIC_CB s_IIC1_CB;

#define IIC0_BUF_LEN  128
#define IIC1_BUF_LEN  128

struct tagIIC_IntParamSet
{
    struct tagSemaphoreLCB *pDrvPostSemp;   //信号量
    u32 TransCount;                     //传输数据量计数器
    u32 TransTotalLen;
};

static struct tagIIC_IntParamSet IntParamset0;
static struct tagIIC_IntParamSet IntParamset1;

// =============================================================================
// 函数：读写数据等待
// 功能：读/写完一个字节的数据后等待时间，超时则强制返回，等待MIF信号置位，若检查到
//       MIF，则判断是否失去仲裁MAL和传输完成标记MCF，最后判断是否收到ACK信号
// 参数： dev,设备指针
//       write,写标志，若为写则需判断是否收到ACK信号
// 返回：-1，超时；0，正确返回
// =============================================================================
static int __iic_wait(volatile tagI2CReg *dev, int write)
{
    u32 csr;
    unsigned timeout = 0;

    do
    {
        Djy_DelayUs(1);

        csr = dev->S;
        if (!(csr & I2C_S_IICIF_MASK))
            continue;
        /* Read again to allow register to stabilise */
        csr = dev->S;

        dev->S |= I2C_S_IICIF_MASK;/*清状态寄存器标志*/

        if (csr & I2C_S_ARBL_MASK)                /*判断是否丢失仲裁*/
        {
            #ifdef DEBUG
            printk("__i2c_wait: ARBL\n");
            #endif
            return -1;
        }

        if (!(csr & I2C_S_TCF_MASK))              /*判断是否传输完成*/
        {
            #ifdef DEBUG
            printk("__i2c_wait: unfinished\n");
            #endif
            return -1;
        }

        if (write == I2C_WRITE_BIT && (csr & I2C_S_RXAK_MASK)) /*写时，判断ack*/
        {
            #ifdef DEBUG
            printk("__i2c_wait: No RXACK\n");
            #endif
            return -1;
        }

        return 0;
    } while (timeout++ < CONFIG_I2C_TIMEOUT);

    #ifdef DEBUG
    printk("__i2c_wait: timed out\n");
    #endif
    return -1;
}

// =============================================================================
// 函数：总线等待
// 功能：等待总线状态，判断MBB判断是否允许操作iic
// 参数： dev,设备指针
// 返回：-1，超时；0，正确返回
// =============================================================================
static int __iic_wait4bus(volatile tagI2CReg *dev)
{
    unsigned timeout = 0;

    while (dev->S & I2C_S_BUSY_MASK)
    {
//      writeb(dev->cr, I2C_CR_MEN | I2C_CR_MSTA | I2C_CR_MTX);
//      readb(dev->dr);
        Djy_DelayUs(1);
        dev->C1 |= I2C_C1_IICEN_MASK;
        if ( timeout++ > CONFIG_I2C_MBB_TIMEOUT)    /*判断是否等待BB超时*/
            return -1;
    }

    return 0;
}

// =============================================================================
// 函数：IIC寻址
// 功能：寻址iic设备，即往总线发iic从设备地址，最后一个bit表示读/写位
// 参数： dev,设备指针
//       devaddr,器件物理地址，最后一bit区分读写
//       dir,读写标记，为0时为写，为1时为读
//       rsta,是否为重新启动
// 返回：1,成功；0,失败
// =============================================================================
static int __iic_write_addr (volatile tagI2CReg *dev,
                            u8 devaddr, u8 dir, int rsta)
{
    dev->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK | I2C_C1_TX_MASK |
            (rsta ? I2C_C1_RSTA_MASK : 0);/*使能iic，start，mtx模式*/

    dev->D = (devaddr << 1) | dir;    /*写地址*/

    if (__iic_wait(dev,I2C_WRITE_BIT) < 0)
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
        dev->D = data[i];

        if (__iic_wait(dev,I2C_WRITE_BIT) < 0)
            break;
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
    reg->C1 |= I2C_C1_IICIE_MASK;
}

// =============================================================================
// 功能: 禁止iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __IIC_IntDisable(volatile tagI2CReg *reg)
{
    reg->C1 &= ~I2C_C1_IICIE_MASK;
}

// =============================================================================
// 功能：初始化时钟配置,ppc1020提供两个独立i2c，其时钟来自CCB/2，再经过i2c时钟分频，
//        配置I2CFDR寄存器，分频系数并无特定的计算方法，本API通过枚举参数确定四种可
//        配置的波特率.
// 参数：reg，寄存器指针
//      i2cclk，枚举量，I2C_CLOCK_RATE_50K，时钟为50K
// 返回：true=成功，false=失败
// =============================================================================
static void __IIC_SetClk(volatile tagI2CReg *reg,u32 i2cclk)
{
    //I2C bus clk devider
    if(i2cclk < CN_IIC_SPEED_100KHZ)
    {
        reg->F = 0xA5;  //50kHz
    }
    else if((i2cclk < CN_IIC_SPEED_200KHZ) && (i2cclk >= CN_IIC_SPEED_100KHZ))
    {
        reg->F = 0x9C;  //100kHz
    }
    else if((i2cclk < CN_IIC_SPEED_300KHZ) && (i2cclk >= CN_IIC_SPEED_200KHZ))
    {
        reg->F = 0x60;  //200kHz
    }
    else if((i2cclk < CN_IIC_SPEED_400KHZ) && (i2cclk >= CN_IIC_SPEED_300KHZ))
    {
        reg->F = 0x56;  //300kHz
    }
    else
    {
        reg->F = 0x53;  //400kHz
    }
}

// =============================================================================
// 功能: 硬件GPIO初始化，包括电源等
// 参数: IIC_NO,编号
// 返回: 无
// =============================================================================
void __IIC_GpioConfig(u8 IIC_NO)
{
    switch(IIC_NO)
    {
    case CN_IIC0:
        GPIO_PowerOn(GPIO_PORT_D);
        SIM->SCGC4 |= SIM_SCGC4_IIC0_MASK;
        PORT_MuxConfig(PORT_PORT_E,PORT_PIN(18),PORT_PINMUX_ALT4);
        PORT_MuxConfig(PORT_PORT_E,PORT_PIN(19),PORT_PINMUX_ALT4);
        break;
    case CN_IIC1:
        GPIO_PowerOn(GPIO_PORT_C);
        SIM->SCGC4 |= SIM_SCGC4_IIC1_MASK;
        PORT_MuxConfig(PORT_PORT_C,PORT_PIN(10),PORT_PINMUX_ALT2);
        PORT_MuxConfig(PORT_PORT_C,PORT_PIN(11),PORT_PINMUX_ALT2);
        break;
    default:
        break;
    }
}

// =============================================================================
// 功能: IIC默认硬件初始化配置，主要是时钟配置和GPIO写保护引脚配置
// 参数: RegBaseAddr,寄存器基址
// 返回: 无
// =============================================================================
static void __IIC_HardDefaultSet(ptu32_t RegBaseAddr)
{
    volatile tagI2CReg *reg;
    reg = (tagI2CReg *)RegBaseAddr;

    reg->F = 0x9C;              //default 100KHz =  60M/(4 * 144)
    reg->A1 = 0xA0;             //Address When The IIC As Slaver
    reg->C1 |= I2C_C1_IICEN_MASK;
}

// =============================================================================
// 功能: IIC中断配置函数
// 参数: IntLine,中断线
//       isr,中断服务函数指针
// 返回: 无
// =============================================================================
static void __IIC_IntConfig(u32 IntLine,u32 (*isr)(ufast_t))
{
    //中断线的初始化
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
    reg->C1 &= ~(I2C_C1_MST_MASK | I2C_C1_IICEN_MASK);
}

// =============================================================================
// 功能：轮询方式读IIC从设备的数据
// 参数：reg,寄存器基址
//       devaddr,设备地址
//       addr,访问的寄存器地址
//       buf,存储数据
//       len,数据长度，字节
// 返回：len,读取成功;-1,读取失败
// =============================================================================
static s32 __IIC_ReadPoll(volatile tagI2CReg *reg,u8 devaddr,u32 memaddr,
                        u8 maddrlen, u8 *buf, u32 len)
{
    u32 i;
    u8 mem_addr_buf[4];
    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,memaddr);

    // Write Device Address
    if(0 == __iic_write_addr(reg,devaddr,I2C_WRITE_BIT,0))
        return -1;

    // Write Register
    if(maddrlen != __iic_write(reg,mem_addr_buf,maddrlen))
        return -1;

    //Write Device Address
    if(0 == __iic_write_addr(reg,devaddr,I2C_READ_BIT,1))
        return -1;

   // Start Recving
    reg->C1 &= ~I2C_C1_TX_MASK;

    //接下来的两句非常重要，如果没有会造成读错误
    reg->S;
    reg->D;

    for(i = 0; i < len; i++)
    {
        if(0 == __iic_wait(reg,I2C_READ_BIT))
            buf[i] = reg->D;
        else
            break;
    }

    reg->C1 = I2C_C1_IICEN_MASK;        // STOP
    // stop
    __iic_wait4bus(reg);
    reg->C1 = 0;    // Disable IIC

    return i;
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
    if(0 == __iic_write_addr(reg,devaddr,I2C_WRITE_BIT,0))
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
// 返回：len,读取成功;-1,读取失败
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
// 功能: 详情参见iic_bus.h中关于WriteStartFunc。
// 参数: 详情参见iic_bus.h中关于WriteStartFunc。
// 返回: 详情参见iic_bus.h中关于WriteStartFunc。
// =============================================================================
static bool_t __IIC_GenerateWriteStart(tagI2CReg *reg,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct tagSemaphoreLCB *iic_semp)
{
    u8 mem_addr_buf[4];
    //通过specific_flag区分是哪条总线
    if((u32)reg == CN_IIC0_BASE)
    {
        IntParamset0.TransTotalLen = length;
        IntParamset0.TransCount = 0;
        IntParamset0.pDrvPostSemp = iic_semp;           //iic_bus_semp
    }
    else if((u32)reg == CN_IIC1_BASE)
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
    __IIC_IntDisable(reg);

    //先发送器件地址，内部地址，然后发送第一个数据
    if (__iic_wait4bus(reg) >= 0)
    {
        if(__iic_write_addr(reg, dev_addr, I2C_WRITE_BIT, 0) != 0)
        {
            //发送器件内部地址
            if(maddr_len - 1 == __iic_write(reg,mem_addr_buf,maddr_len-1))
            {
                reg->D = mem_addr_buf[maddr_len-1];
                __IIC_IntEnable(reg);
                return true;
            }
        }
    }
    return false;
}

// =============================================================================
// 功能: 详情参见iic_bus.h中关于ReadStartFunc。
// 参数: 详情参见iic_bus.h中关于ReadStartFunc。
// 返回: 详情参见iic_bus.h中关于ReadStartFunc。
// =============================================================================
static bool_t __IIC_GenerateReadStart( tagI2CReg *reg,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct tagSemaphoreLCB *iic_semp)
{
    u8 mem_addr_buf[4];

    if((u32)reg == CN_IIC0_BASE)
    {
        IntParamset0.TransTotalLen = length;
        IntParamset0.TransCount = 0;
        IntParamset0.pDrvPostSemp = iic_semp;               //iic_buf_semp
    }
    else if((u32)reg == CN_IIC1_BASE)
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
    __IIC_IntDisable(reg);

    //写器件地址和寻址地址
    if (__iic_wait4bus(reg) >= 0)
    {
        if(__iic_write_addr(reg, dev_addr, I2C_WRITE_BIT, 0) != 0)
        {
            if(__iic_write(reg, mem_addr_buf, maddr_len) == maddr_len)
            {
                if(length && __iic_write_addr(reg,dev_addr, I2C_READ_BIT,1) != 0)
                {
                    reg->C1 = I2C_C1_IICEN_MASK | I2C_C1_MST_MASK |
                            ((length == 1) ? I2C_C1_TXAK_MASK : 0);
                    //接下来的两句非常重要，如果没有会造成读错误
                    reg->S;
                    reg->D;

                    __IIC_IntEnable(reg);
                    return true;
                }
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
    if(reg->C1 & I2C_C1_TX_MASK)//发送
    {
        __IIC_GenerateStop(reg);
    }
    else //接收
    {
        reg->C1 |= I2C_C1_TXAK_MASK;//停止回复ACK
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
    case CN_IIC_SET_CLK:
        __IIC_SetClk(reg,data1);
        break;
    default:
        break;
    }
    return 1;
}

// =============================================================================
// 功能：IIC接收与发送中断服务函数。该函数实现的功能如下：
//       1.每发送与接收一个或若干字节发生一次中断；
//       2.若有多个中断使用同一个中断号，则需根据具体情况区分使用的是哪个中断；
//       3.清中断标志，并判断ACK信号，每读写字节，计数器都需相应修改；
//       4.接收达到倒数第一个字节时，需配置不发送ACK信号；
//       5.接收或发送完成时，需post信号量IntParam->pDrvPostSemp；
//       6.接收或发送完成时，需产生停止时序。
// 参数：i2c_int_line,中断号，本函数没用到
// 返回：无意义
// =============================================================================
static u32 __IIC_ISR(ufast_t i2c_int_line)
{
    static struct tagIIC_CB *ICB;
    static struct tagIIC_IntParamSet *IntParam;
    tagI2CReg *reg;
    u8 ch;
    u32 IicErrorNo;

    reg = (tagI2CReg*)CN_IIC0_BASE;

    if(reg->S & I2C_S_IICIF_MASK)//iic1中断
    {
        ICB = &s_IIC0_CB;
        IntParam = &IntParamset0;
    }
    else
    {
        reg = (tagI2CReg*)CN_IIC1_BASE;
        if(reg->S & I2C_S_IICIF_MASK)//iic2中断
        {
            ICB = &s_IIC1_CB;
            IntParam = &IntParamset1;
        }
        else
            return 0;
    }

    reg->S |= I2C_S_IICIF_MASK;            //清中断标志位

    if(reg->C1 & I2C_C1_MST_MASK)        //已经启动传输
    {
        if(reg->C1 & I2C_C1_TX_MASK)      //发送中断
        {
            if(!(reg->C1 & I2C_S_RXAK_MASK))
            {
                //从发送缓冲区读一个字节的数据，并发送
                if(IIC_PortRead(ICB,&ch,1) > 0)
                {
                    reg->D = ch;
                    IntParam->TransCount ++;;
                }
                else if(IntParam->TransCount == IntParam->TransTotalLen)
                {
                  Lock_SempPost(IntParam->pDrvPostSemp);
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
            while((IntParam->TransCount < IntParam->TransTotalLen) &&
                                        (reg->S & I2C_S_TCF_MASK))
            {
                // 最后一个字节master不发ACK，表示读操作终止
                if(IntParam->TransCount == IntParam->TransTotalLen - 1)
                {
                    reg->C1 |= I2C_C1_TXAK_MASK;
                }
                ch = reg->D;
                //写数据
                IIC_PortWrite(ICB,&ch,1);
                IntParam->TransCount ++;
            }
            if((IntParam->TransCount == IntParam->TransTotalLen)  &&
                                        (reg->S & I2C_S_TCF_MASK))
            {
                __IIC_GenerateStop(reg);
                Lock_SempPost(IntParam->pDrvPostSemp);//释放总线信号量
            }
        }
    }
    else//未启动通信
    {
        if(reg->S & I2C_S_ARBL_MASK)//仲裁丢失中断
        {
            reg->C1 |= I2C_S_ARBL_MASK;
            IicErrorNo = CN_IIC_POP_MAL_LOST_ERR;
            IIC_ErrPop(ICB,IicErrorNo);
        }
        else//只能是作为I2C从器件时产生的中断
        {
        }
    }

    return 0;
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
bool_t IIC0_Init(void)
{
    static u8 s_IIC0Buf[IIC0_BUF_LEN];
    struct tagIIC_Param IIC0_Config;

    IIC0_Config.BusName             = "IIC0";
    IIC0_Config.IICBuf              = (u8*)&s_IIC0Buf;
    IIC0_Config.IICBufLen           = IIC0_BUF_LEN;
    IIC0_Config.SpecificFlag        = CN_IIC0_BASE;
    IIC0_Config.pGenerateWriteStart = (WriteStartFunc)__IIC_GenerateWriteStart;
    IIC0_Config.pGenerateReadStart  = (ReadStartFunc)__IIC_GenerateReadStart;
    IIC0_Config.pGenerateEnd        = (GenerateEndFunc)__IIC_GenerateEnd;
    IIC0_Config.pBusCtrl            = (IICBusCtrlFunc)__IIC_BusCtrl;
    IIC0_Config.pWriteReadPoll      = (WriteReadPoll)__IIC_WriteReadPoll;

    __IIC_GpioConfig(CN_IIC0);
    __IIC_HardDefaultSet(CN_IIC0_BASE);
    __IIC_IntConfig(CN_INT_LINE_I2C0,__IIC_ISR);

    if(NULL == IIC_BusAdd_s(&IIC0_Config,&s_IIC0_CB))
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
// 返回：无
// =============================================================================
bool_t IIC1_Init(void)
{
    static u8 s_IIC1Buf[IIC1_BUF_LEN];
    struct tagIIC_Param IIC1_Config;

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
    __IIC_HardDefaultSet(CN_IIC1_BASE);
    __IIC_IntConfig(CN_INT_LINE_I2C1,__IIC_ISR);

    if(NULL == IIC_BusAdd_r(&IIC1_Config,&s_IIC1_CB))
        return 0;
    return 1;
}

