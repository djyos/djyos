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
// 创建时间: 29/09.2015
// =============================================================================

#include "stdio.h"
#include "stddef.h"
#include "cpu_peri.h"
#include "iicbus.h"
#include "endian.h"
#include "lock.h"
#include "int.h"
#include "djyos.h"
#include "board-config.h"

// =============================================================================
#define tagI2CReg  I2C_TypeDef
static tagI2CReg volatile * const tg_I2CReg[] = {(tagI2CReg *)I2C1_BASE,
                                                (tagI2CReg *)I2C2_BASE,
                                                (tagI2CReg *)I2C3_BASE};


static struct IIC_CB s_IIC1_CB;
static struct IIC_CB s_IIC2_CB;
static struct IIC_CB s_IIC3_CB;

#define IIC_BUF_LEN  128

// =============================================================================
// ========================IO模拟IIC接口=========================================
// =============================================================================
//#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
//#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
//#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
//
//#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414
//#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410
//
//#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //out
//#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //in

//IO方向设置
//#define SDA_IN()  {GPIOB->MODER&=~(3<<(6*2));GPIOB->MODER|=0<<6*2;}   //PB9输入模式
//#define () {GPIOB->MODER&=~(3<<(6*2));GPIOB->MODER|=1<<6*2;} //PB9输出模式
//IO操作函数
//#define IIC_SCL    PBout(7) //SCL
//#define IIC_SDA    PBout(6) //SDA
//#define READ_SDA   PBin(6)  //输入SDA
extern bool_t Board_IicGpioInit(u8 I2Cx);
extern void IIC_SetDaIn(u8 IICx);
extern void IIC_SetDaOut(u8 IICx);
extern void IIC_Scl(u8 IICx,u8 level);
extern void IIC_Sda(u8 IICx,u8 level);
extern u8 IIC_ReadSda(u8 IICx);

// =============================================================================
// 功能：IIC硬件初始化，主要是GPIO的时钟和配置
// 参数： 无
// 返回：无
// =============================================================================
static void IIC_Init(u8 I2Cx)
{
    Board_IicGpioInit(I2Cx);
    IIC_Scl(I2Cx,1);
    IIC_Sda(I2Cx,1);
}

// =============================================================================
// 功能：产生IIC起始信号
// 参数： 无
// 返回：无
// =============================================================================
static void IIC_Start(u8 I2Cx)
{
    IIC_SetDaOut(I2Cx);     //sda线输出
    IIC_Sda(I2Cx,1);
    IIC_Scl(I2Cx,1);
    Djy_DelayUs(4);
    IIC_Sda(I2Cx,0);//START:when CLK is high,DATA change form high to low
    Djy_DelayUs(4);
    IIC_Scl(I2Cx,0);//钳住I2C总线，准备发送或接收数据
}

// =============================================================================
// 功能：产生IIC停止信号
// 参数： 无
// 返回：无
// =============================================================================
static void IIC_Stop(u8 I2Cx)
{
    IIC_SetDaOut(I2Cx);//sda线输出
    IIC_Scl(I2Cx,0);
    IIC_Sda(I2Cx,0);//STOP:when CLK is high DATA change form low to high
    Djy_DelayUs(4);
    IIC_Scl(I2Cx,1);
    IIC_Sda(I2Cx,1);//发送I2C总线结束信号
    Djy_DelayUs(4);
}

// =============================================================================
// 功能：等待应答信号到来
// 参数： 无
// 返回：1，接收应答失败,0，接收应答成功
// =============================================================================
static u8 IIC_Wait_Ack(u8 I2Cx)
{
    u8 ucErrTime=0;
    IIC_SetDaIn(I2Cx);      //SDA设置为输入
    IIC_Sda(I2Cx,1);
    Djy_DelayUs(1);
    IIC_Scl(I2Cx,1);
    Djy_DelayUs(1);
    while(IIC_ReadSda(I2Cx))
    {
        ucErrTime++;
        Djy_DelayUs(1);
        if(ucErrTime>250)
        {
            IIC_Stop(I2Cx);
            return 1;
        }
    }
    IIC_Scl(I2Cx,0);//时钟输出0
    return 0;
}

// =============================================================================
// 功能：产生应答信号
// 参数： 无
// 返回：无
// =============================================================================
static void IIC_Ack(u8 I2Cx)
{
    IIC_Scl(I2Cx,0);
    IIC_SetDaOut(I2Cx);
    IIC_Sda(I2Cx,0);
    Djy_DelayUs(2);
    IIC_Scl(I2Cx,1);
    Djy_DelayUs(2);
    IIC_Scl(I2Cx,0);
}

// =============================================================================
// 功能：不产生ACK应答
// 参数： 无
// 返回：1，接收应答失败,0，接收应答成功
// =============================================================================
static void IIC_NAck(u8 I2Cx)
{
    IIC_Scl(I2Cx,0);
    IIC_SetDaOut(I2Cx);
    IIC_Sda(I2Cx,1);
    Djy_DelayUs(2);
    IIC_Scl(I2Cx,1);
    Djy_DelayUs(2);
    IIC_Scl(I2Cx,0);
}

// =============================================================================
// 功能：IIC发送一个字节,返回从机有无应答
// 参数： 无
// 返回：1，有应答,0，无应答
// =============================================================================
static void IIC_Send_Byte(u8 I2Cx,u8 txd)
{
    u8 t;
    IIC_SetDaOut(I2Cx);
    IIC_Scl(I2Cx,0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
        IIC_Sda(I2Cx,(txd&0x80)>>7);
        txd<<=1;
        Djy_DelayUs(2);   //对TEA5767这三个延时都是必须的
        IIC_Scl(I2Cx,1);
        Djy_DelayUs(2);
        IIC_Scl(I2Cx,0);
        Djy_DelayUs(2);
    }
}

// =============================================================================
// 功能：读1个字节，ack=1时，发送ACK，ack=0，发送nACK
// 参数： 无
// 返回：1，有应答,0，无应答
// =============================================================================
static u8 IIC_Read_Byte(u8 I2Cx,unsigned char ack)
{
    unsigned char i,receive=0;
    IIC_SetDaIn(I2Cx);//SDA设置为输入
    for(i=0;i<8;i++ )
    {
        IIC_Scl(I2Cx,0);
        Djy_DelayUs(2);
        IIC_Scl(I2Cx,1);
        receive<<=1;
        if(IIC_ReadSda(I2Cx))
            receive++;
        Djy_DelayUs(1);
    }
    if (!ack)
        IIC_NAck(I2Cx);//发送nACK
    else
        IIC_Ack(I2Cx); //发送ACK
    return receive;
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
    case CN_I2C1:
        IIC_Init(IIC_NO);
        break;
    case CN_I2C2:
        break;
    case CN_I2C3:
        break;
    default:
        break;
    }
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
    u8 iic;

    for(i = 0; i < CN_I2C_NUM; i++)
    {
        if(reg == tg_I2CReg[i])
            break;
    }
    if(i == CN_I2C_NUM)
        return -1;

    iic = i;

    u8 mem_addr_buf[4];
    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,memaddr);

    u8 temp=0;
    temp = (devaddr<<1)| CN_IIC_WRITE_FLAG;
    IIC_Start(iic);

    IIC_Send_Byte(iic,temp);       //发送写命令
    IIC_Wait_Ack(iic);
    for(i = 0; i < maddrlen; i++)
    {
        IIC_Send_Byte(iic,mem_addr_buf[i]);   //发送低地址
        IIC_Wait_Ack(iic);
    }

    temp = (devaddr<<1)| CN_IIC_READ_FLAG;
    IIC_Start(iic);
    IIC_Send_Byte(iic,temp);           //进入接收模式
    IIC_Wait_Ack(iic);

    for(i = 0; i < len - 1; i ++)
    {
        buf[i]=IIC_Read_Byte(iic,1);
    }
    buf[i] = IIC_Read_Byte(iic,0);
    IIC_Stop(iic);//产生一个停止条件
    Djy_DelayUs(1000);
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
    u8 iic;
    u32 i;

    for(i = 0; i < CN_I2C_NUM; i++)
    {
        if(reg == tg_I2CReg[i])
            break;
    }
    if(i == CN_I2C_NUM)
        return -1;

    iic = i;
    //将地址作大小端变换
    fill_little_32bit(mem_addr_buf,0,memaddr);

    IIC_Start(iic);

    IIC_Send_Byte(iic,0XA0);        //发送写命令
    IIC_Wait_Ack(iic);

    for(i = 0; i < maddrlen; i++)
    {
        IIC_Send_Byte(iic,mem_addr_buf[i]);   //发送低地址
        IIC_Wait_Ack(iic);
    }

    for(i = 0; i < len; i++)
    {
        IIC_Send_Byte(iic,buf[i]);     //发送字节
        IIC_Wait_Ack(iic);
    }
    IIC_Stop(iic);//产生一个停止条件
    Djy_DelayUs(1000);
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
// 功能：IIC底层驱动的初始化，完成整个IIC总线的初始化，其主要工作如下：
//       1.初始化总线控制块IIC_CB，回调函数和缓冲区的初始化赋值；
//       2.默认的硬件初始化，如GPIO或IIC寄存器等；
//       3.中断初始化，完成读写中断配置；
//       4.调用IICBusAdd或IICBusAdd_r增加总线结点；
// 参数：para,无具体意义
// 返回：无
// =============================================================================
bool_t I2C_Initialize(u8 port)
{
    struct IIC_CB *ICB;
    struct IIC_Param IIC_Config;

    switch(port)
    {
    case CN_I2C1:
        IIC_Config.BusName = "IIC1";
        ICB                = &s_IIC1_CB;
        IIC_Config.IICBuf  = NULL;
        break;
    case CN_I2C2:
        IIC_Config.BusName = "IIC2";
        IIC_Config.IICBuf  = NULL;
        ICB                = &s_IIC2_CB;
        break;
    case CN_I2C3:
        IIC_Config.BusName = "IIC3";
        IIC_Config.IICBuf  = NULL;
        ICB                = &s_IIC3_CB;
        break;
    default: return false;
    }

    IIC_Config.IICBufLen           = IIC_BUF_LEN;
    IIC_Config.SpecificFlag        = (ptu32_t)tg_I2CReg[port];
    IIC_Config.pWriteReadPoll      = (WriteReadPoll)__IIC_WriteReadPoll;
    //stm32的IIC存在问题，故采用IO模拟方式，无须实现如下函数
    IIC_Config.pGenerateWriteStart = NULL;
    IIC_Config.pGenerateReadStart  = NULL;
    IIC_Config.pGenerateEnd        = NULL;
    IIC_Config.pBusCtrl            = NULL;

    __IIC_GpioConfig(port);
//    __IIC_HardDefaultSet(IIC_Config.SpecificFlag);
//    __IIC_IntConfig(intline,__IIC_ISR);

    if(NULL == IIC_BusAdd_s(ICB,&IIC_Config))
        return 0;
    return 1;
}

