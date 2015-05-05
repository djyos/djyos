// =============================================================================
// Copyright (C) 2012-2020 都江堰操作系统研发团队  All Rights Reserved
// 文件名     ：cpu_peri_iic.c
// 模块描述: IIC模块的底层驱动函数，由具体平台的底层驱动程序员完成
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 18/06.2014
// =============================================================================

#include <string.h>
#include "stdint.h"
#include "cpu_peri.h"
#include "endian.h"
#include "iicbus.h"
#include "lock.h"
#include "int_hard.h"

#define debug(x,...)
#define CN_IIC_REGISTER_BADDR0  cn_i2c1_baddr
#define CN_IIC_REGISTER_BADDR1  cn_i2c2_baddr

#define debug(x,...)
#define I2C_Address             (0x14)//本机IIC地址，当读

#define I2C_Write_Code          (0xfe)//写命令，在从设备地址最后一位配置为0
#define I2C_Read_Code           (0x01)//读命令，在从设备地址最后一位配置为1

#define CHKBIT(dest,mask)       (dest & mask)
#define SETBIT(dest,mask)       (dest |= mask)
#define CLRBIT(dest,mask)       (dest &= ~mask)

//定义总线等待超时时间
#ifndef CONFIG_I2C_MBB_TIMEOUT
#define CONFIG_I2C_MBB_TIMEOUT  100000
#endif

//定义读写超时时间
#ifndef CONFIG_I2C_TIMEOUT
#define CONFIG_I2C_TIMEOUT  10000
#endif

//定义读写操作比特位
#define I2C_READ_BIT  1
#define I2C_WRITE_BIT 0

//操作寄存器
#define writeb(reg, data)       (reg = data)
#define readb(reg)              (reg)

//寄存器位操作
//控制寄存器
#define I2C_CR_MEN                  (1<<7)//使能
#define I2C_CR_MIEN                 (1<<6)//中断使能
#define I2C_CR_MSTA                 (1<<5)//I2C启动停止
#define I2C_CR_MTX                  (1<<4)//发送接收模式
#define I2C_CR_TXAK                 (1<<3)//发送ACK信号
#define I2C_CR_RSTA                 (1<<2)//重新启动

//状态寄存器
#define I2C_SR_MCF                  (1<<7)//I2C传输完成标志
#define I2C_SR_MBB                  (1<<5)//I2C作为从设备
#define I2C_SR_MAL                  (1<<4)//I2C总线忙标志
#define I2C_SR_SRW                  (1<<2)//I2C中断标志位
#define I2C_SR_MIF                  (1<<1)//I2C仲裁丢失
#define I2C_SR_RXAK                 (1<<0)//I2C接收到ACK信号
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

        csr = readb(dev->I2CSR);
        if (!(csr & I2C_SR_MIF))
            continue;
        /* Read again to allow register to stabilise */
        csr = readb(dev->I2CSR);

        writeb(dev->I2CSR, 0x0);/*清状态寄存器标志*/

        if (csr & I2C_SR_MAL)                 /*判断是否丢失仲裁*/
        {
            debug("__i2c_wait: MAL\n");
            return -1;
        }

        if (!(csr & I2C_SR_MCF))              /*判断是否传输完成*/
        {
            debug("__i2c_wait: unfinished\n");
            return -1;
        }

        if (write == I2C_WRITE_BIT && (csr & I2C_SR_RXAK)) /*写时，判断ack*/
        {
            debug("__i2c_wait: No RXACK\n");
            return -1;
        }

        return 0;
    } while (timeout++ < CONFIG_I2C_TIMEOUT);

    debug("__i2c_wait: timed out\n");
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

    while (readb(dev->I2CSR) & I2C_SR_MBB)
    {
//      writeb(dev->cr, I2C_CR_MEN | I2C_CR_MSTA | I2C_CR_MTX);
//      readb(dev->dr);
        Djy_DelayUs(1);
        writeb(dev->I2CCR, I2C_CR_MEN);
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
// 返回：发送数据量，字节单位
// =============================================================================
static int __iic_write_addr (volatile tagI2CReg *dev,
                            u8 devaddr, u8 dir, int rsta)
{
    writeb(dev->I2CCR,
          I2C_CR_MEN | I2C_CR_MSTA | I2C_CR_MTX | (rsta ? I2C_CR_RSTA : 0)
           );/*使能iic，start，mtx模式*/

    writeb(dev->I2CDR, (devaddr << 1) | dir);/*写地址*/

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
        writeb(dev->I2CDR, data[i]);

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
static void _IIC_IntEnable(volatile tagI2CReg *reg)
{
    SETBIT(reg->I2CCR, I2C_CR_MIEN);
}

// =============================================================================
// 功能: 禁止iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void _IIC_IntDisable(volatile tagI2CReg *reg)
{
    CLRBIT(reg->I2CCR, I2C_CR_MIEN);
}

// =============================================================================
// 功能：初始化时钟配置,ppc1020提供两个独立i2c，其时钟来自CCB/2，再经过i2c时钟分频，
//        配置I2CFDR寄存器，分频系数并无特定的计算方法，本API通过枚举参数确定四种可
//        配置的波特率.
// 参数：reg，寄存器指针
//      i2cclk，枚举量，I2C_CLOCK_RATE_50K，时钟为50K
// 返回：true=成功，false=失败
// =============================================================================
static void __iic_set_clk(volatile tagI2CReg *reg,u8 i2cclk)
{
    //I2C bus clk devider
    switch(i2cclk)
    {
    case I2C_CLOCK_RATE_50K:
        reg->I2CFDR = 0x33;    //clk=CCB/2/4096=400M/2/4096 = 48.8K
        break;
    case I2C_CLOCK_RATE_100K:
        reg->I2CFDR = 0x2F;    //clk=CCB/2/2048=400M/2/2048 = 97.6K
        break;
    case I2C_CLOCK_RATE_200K:
        reg->I2CFDR = 0x07;    //clk=CCB/2/1024=400M/2/1024 = 195.3K
        break;
    case I2C_CLOCK_RATE_400K:
        reg->I2CFDR = 0x26;    //clk=CCB/2/512 = 400M/2/512 = 390.6K
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
    GPIO_CfgPinFunc(GPIO_3,cn_gpio_mode_out_ad);
    GPIO_SettoLow(GPIO_3);//拉低FRAM写保护引脚

    volatile tagI2CReg *reg;
    reg = (tagI2CReg *)RegBaseAddr;
    reg->I2CFDR = 0x2F;//default 100KHz
    SETBIT(reg->I2CCR,I2C_CR_MEN);
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
    //set the iic active high
    Int_SetLineTrigerType(IntLine,EN_INT_TRIGER_HIGHLEVEL);
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
    __iic_wait4bus(reg);
}

// =============================================================================
// 功能: 启动写时序，启动写时序的过程为：器件地址（写）、存储地址（写），当存储地址完
//       成时，需打开中断，重新配置寄存器为接收模式，之后将会发生发送中断，在中断服务
//       函数中，每次发送一定的字节数，直到len数据量，post信号量iic_bus_semp，并产生
//       停止时序
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
//      dev_addr,器件地址的前7比特，已将内部地址所占的bit位更新，该函数需将该地址左
//               移一位增加增加最后一位读/写比特;
//      mem_addr,存储器内部地址，即发送到总线上的地址，该地址未包含放在器件地址上的
//               比特位；
//      maddr_len,存储器内部地址的长度，字节单位，未包含在器件地址里面的比特位；
//      length,接收的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，
//          当接收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//      iic_semp,发送完成时驱动需释放的信号量。发送程序读IIC_PortRead时，若读不到数
//          则需释放该信号量，产生结束时序
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
static bool_t __IIC_GenerateWriteStart(ptu32_t  specific_flag,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct tagSemaphoreLCB *iic_semp)
{
    volatile tagI2CReg *reg;
    u8 mem_addr_buf[4];
    //通过specific_flag区分是哪条总线
    if(specific_flag == CN_IIC_REGISTER_BADDR0)
    {
        IntParamset0.TransTotalLen = length;
        IntParamset0.TransCount = 0;
        IntParamset0.pDrvPostSemp = iic_semp;           //iic_bus_semp
    }
    else if(specific_flag == CN_IIC_REGISTER_BADDR1)
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
    reg = (tagI2CReg *)specific_flag;
    _IIC_IntDisable(reg);

    //先发送器件地址，内部地址，然后发送第一个数据
    if (__iic_wait4bus(reg) >= 0)
    {
        if(__iic_write_addr(reg, dev_addr, I2C_WRITE_BIT, 0) != 0)
        {
            //发送器件内部地址
            if(maddr_len - 1 == __iic_write(reg,mem_addr_buf,maddr_len-1))
            {
                reg->I2CDR = mem_addr_buf[maddr_len-1];
                _IIC_IntEnable(reg);
                return true;
            }
        }
    }
    return false;
}

// =============================================================================
// 功能: 启动读时序，启动读时序的过程为：器件地址（写）、存储地址（写）、器件地址（读）
//       当器件地址（读）完成时，需打开中断，重新配置寄存器为接收模式，之后将会发生
//       接收数据中断，在中断中将接收到的数据调用IIC_PortWrite写入缓冲，接收到len字
//       节数的数据后，释放信号量iic_semp
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
//      dev_addr,器件地址的前7比特，已将内部地址所占的bit位更新，该函数需将该地址左
//               移一位增加增加最后一位读/写比特;
//      mem_addr,存储器内部地址，即发送到总线上的地址，该地址未包含放在器件地址上的
//               比特位；
//      maddr_len,存储器内部地址的长度，字节单位，未包含在器件地址里面的比特位；
//      len,接收的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，当接
//          收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//      iic_semp,读完成时，驱动需释放的信号量（缓冲区信号量）
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
static bool_t __IIC_GenerateReadStart( ptu32_t  specific_flag,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct tagSemaphoreLCB *iic_semp)
{
    volatile tagI2CReg *reg;
    u8 mem_addr_buf[4];

    if(specific_flag == CN_IIC_REGISTER_BADDR0)
    {
        IntParamset0.TransTotalLen = length;
        IntParamset0.TransCount = 0;
        IntParamset0.pDrvPostSemp = iic_semp;               //iic_buf_semp
    }
    else if(specific_flag == CN_IIC_REGISTER_BADDR1)
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
    reg = (tagI2CReg *)specific_flag;
    _IIC_IntDisable(reg);

    //写器件地址和寻址地址
    if (__iic_wait4bus(reg) >= 0)
    {
        if(__iic_write_addr(reg, dev_addr, I2C_WRITE_BIT, 0) != 0)
        {
            if(__iic_write(reg, mem_addr_buf, maddr_len) == maddr_len)
            {
                if(length && __iic_write_addr(reg,dev_addr, I2C_READ_BIT,1) != 0)
                {
                    writeb(reg->I2CCR,I2C_CR_MEN |
                        I2C_CR_MSTA | ((length == 1) ? I2C_CR_TXAK : 0));
                    //接下来的两句非常重要，如果没有会造成读错误
                    readb(reg->I2CSR);
                    readb(reg->I2CDR);

                    _IIC_IntEnable(reg);
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
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
// 返回: 无
// =============================================================================
static void __IIC_GenerateEnd(ptu32_t specific_flag)
{
    volatile tagI2CReg *reg;

    reg = (tagI2CReg *)specific_flag;

    if(CHKBIT(reg->I2CCR,I2C_CR_MTX))//发送
    {
        __IIC_GenerateStop(reg);
    }
    else //接收
    {
        SETBIT(reg->I2CCR, I2C_CR_TXAK);//停止回复ACK
        __IIC_GenerateStop(reg);
    }
}

// =============================================================================
// 功能：IIC总线控制回调函数，被上层调用，目前只实现对IIC时钟频率配置
// 参数：specific_flag,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：1
// =============================================================================
static s32 __IIC_BusCtrl(ptu32_t spceific_flag,u32 cmd,u32 data1,u32 data2)
{
    volatile tagI2CReg *reg;
    reg = (tagI2CReg *)spceific_flag;

    switch(cmd)
    {
    case CN_IIC_SET_CLK:
        __iic_set_clk(reg,data1);
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

    reg = (tagI2CReg*)CN_IIC_REGISTER_BADDR0;

    if(CHKBIT(reg->I2CSR, I2C_SR_MIF))//iic1中断
    {
        ICB = &s_IIC0_CB;
        IntParam = &IntParamset0;
    }
    else
    {
        reg = (tagI2CReg*)CN_IIC_REGISTER_BADDR1;
        if(CHKBIT(reg->I2CSR, I2C_SR_MIF))//iic2中断
        {
            ICB = &s_IIC1_CB;
            IntParam = &IntParamset1;
        }
        else
            return 0;
    }

    CLRBIT(reg->I2CSR, I2C_SR_MIF);            //清中断标志位

    if(CHKBIT(reg->I2CCR, I2C_CR_MSTA))        //已经启动传输
    {
        if(CHKBIT(reg->I2CCR,I2C_CR_MTX))      //发送中断
        {
            if(!CHKBIT(reg->I2CSR, I2C_SR_RXAK))
            {
                //从发送缓冲区读一个字节的数据，并发送
                if(IIC_PortRead(ICB,&ch,1) > 0)
                {
                    reg->I2CDR = ch;
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
                                        (CHKBIT(reg->I2CSR, I2C_SR_MCF)))
            {
                // 最后一个字节master不发ACK，表示读操作终止
                if(IntParam->TransCount == IntParam->TransTotalLen - 1)
                {
                    SETBIT(reg->I2CCR, I2C_CR_TXAK);
                }
                ch = reg->I2CDR;
                //写数据
                IIC_PortWrite(ICB,&ch,1);
                IntParam->TransCount ++;
            }
            if((IntParam->TransCount == IntParam->TransTotalLen)  &&
                                        (CHKBIT(reg->I2CSR, I2C_SR_MCF)))
            {
                __IIC_GenerateStop(reg);
                Lock_SempPost(IntParam->pDrvPostSemp);//释放总线信号量
            }
        }
    }
    else//未启动通信
    {
        if(CHKBIT(reg->I2CSR,I2C_SR_MAL))//仲裁丢失中断
        {
            CLRBIT(reg->I2CSR,I2C_SR_MAL);
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
    IIC0_Config.SpecificFlag        = CN_IIC_REGISTER_BADDR0;
    IIC0_Config.pGenerateWriteStart = __IIC_GenerateWriteStart;
    IIC0_Config.pGenerateReadStart  = __IIC_GenerateReadStart;
    IIC0_Config.pGenerateEnd        = __IIC_GenerateEnd;
    IIC0_Config.pBusCtrl            = __IIC_BusCtrl;

    __IIC_HardDefaultSet(CN_IIC_REGISTER_BADDR0);
    __IIC_IntConfig(cn_int_line_iic_controllers,__IIC_ISR);

    if(NULL == IIC_BusAdd_r(&IIC0_Config,&s_IIC0_CB))
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
    IIC1_Config.IICBufLen           = IIC0_BUF_LEN;
    IIC1_Config.SpecificFlag        = CN_IIC_REGISTER_BADDR1;
    IIC1_Config.pGenerateWriteStart = __IIC_GenerateWriteStart;
    IIC1_Config.pGenerateReadStart  = __IIC_GenerateReadStart;
    IIC1_Config.pGenerateEnd        = __IIC_GenerateEnd;
    IIC1_Config.pBusCtrl            = __IIC_BusCtrl;

    __IIC_HardDefaultSet(CN_IIC_REGISTER_BADDR0);
    __IIC_IntConfig(cn_int_line_iic_controllers,__IIC_ISR);

    if(NULL == IIC_BusAdd_r(&IIC1_Config,&s_IIC1_CB))
        return 0;
    return 1;
}






