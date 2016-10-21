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
//所属模块: iic驱动程序(具体驱动部分)
//作者:  zhuhb-cyg.
//版本：V1.0.0
//文件描述:
//其他说明:

//------------------------------------------------------
#include <string.h>
#include "stdint.h"
#include "cpu_peri.h"
#include "iicbus.h"
#include "endian.h"
#include "def21469.h"
#include "Cdef21469.h"

#define CN_IIC_REGISTER_BADDR0   CN_IIC0_BADDR
#define debug(x,...)
#define I2C_Address             (0x14)//本机IIC地址，当读

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


static struct IIC_CB s_IIC0_CB;

//缓存区大小限制：2-254.
//SHARC21469作为主机发送时，理论上不对发送数据长度做限制；
//作为主机接收时，一个IIC时序内可以接收的数据长度限制为254个，如果要接收更多的数据必须重新启动一次IIC时序。
#define IIC0_BUF_LEN  254


#define TWI_ADDR_MASK           (0x7F)
#define TWI_ADDR_GET(n)         ((n>>1)& TWI_ADDR_MASK)


struct IIC_IntParamSet
{
    struct SemaphoreLCB *pDrvPostSemp;   //信号量
    u32 TransCount;                     //传输数据量计数器
    u32 TransTotalLen;
};

static struct IIC_IntParamSet IntParamset0;


//函数声明
 static int __TWI_WaitForTransOver(void);
 static int __TWI_WaitForRxReady(void);
 static int __TWI_WaitForTxReady(void);
 static int __TWI_WaitForTwiFree(void);
 static int __TWI_WaitForAck(int type);
 static int __TWI_WriteAddr(volatile tagI2CReg *dev,u8 *data,int length);
 void _IIC_IntEnable(volatile tagI2CReg *reg);
 void _IIC_IntDisable(volatile tagI2CReg *reg);
 void __IIC_GenerateStop(volatile tagI2CReg *reg);
 void _IIC_GenerateDisable(volatile tagI2CReg *reg);
 static u32 __IIC_ISR(ufast_t i2c_int_line);
 void __iic_set_clk(volatile tagI2CReg *reg,enum_I2C_Clock i2cclk);
 static void __IIC_HardDefaultSet(ptu32_t RegBaseAddr);
 static void __IIC_IntConfig(u32 IntLine,u32 (*isr)(ufast_t));
 static void __IIC_BusCtrl(ptu32_t spceific_flag,u32 cmd,u32 data1,u32 data2);
 static void __IIC_GenerateEnd(ptu32_t specific_flag);
 static bool_t __IIC_GenerateWriteStart(ptu32_t  specific_flag,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct SemaphoreLCB *iic_semp);
 static bool_t __IIC_GenerateReadStart(ptu32_t  specific_flag,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct SemaphoreLCB *iic_semp);



// =====================读写数据等待===========================================
// 功能：读/写完一个字节的数据后等待时间，超时则强制返回，等待MIF信号置位，若检查到
//       MIF，则判断是否失去仲裁MAL和传输完成标记MCF，最后判断是否收到ACK信号
// 参数： dev,设备指针
//       write,写标志，若为写则需判断是否收到ACK信号
// 返回：-1，超时；0，正确返回
// =============================================================================
  static int __TWI_WaitForTransOver(void)
  {
    unsigned timeout = CONFIG_I2C_TIMEOUT;
    while( !(*pTWIIRPTL & TWIMCOM) )
    {
        if( timeout-- < 1 )
        {
            return -1;
        }
    }

    return 0;

  }

 // =================查询Receive buffer是否有数据=================================
 //功能描述  : 等待Rx FIFO有数据，至少一个；超时返回-1.
 //输入参数  : void
 //返 回 值  : TWI FIFO至少有一个数据即返回0，超时返回-1.
 // =============================================================================
  static int __TWI_WaitForRxReady(void)
  {
    unsigned timeout = CONFIG_I2C_TIMEOUT;
    while(!(*pTWIIRPTL&TWIRXINT))
    {
            if( timeout-- <1 )
        {
            return -1;
        }
    }

    return 0;
  }

// =================查询Transmit buffer是否为非满=================================
//功能描述  : 等待Tx FIFO空出(两个FIFO均空出)；超时返回-1.
//输入参数  : void
//返 回 值  : TWI FIFO空返回0，超时返回-1.
// =============================================================================
  static int __TWI_WaitForTxReady(void)
  {
    unsigned timeout = CONFIG_I2C_TIMEOUT;
    //TWITXS：Transfer FIFO Status,00表示FIFO empty;01表示FIFO包含一个字节数据；
    //10 Reserved；11,表示FIFO full。
    while(*pTWIIRPTL&TWITXINT)
    {
        if( timeout-- <1 )
        {
            return -1;
        }
    }

    return 0;
  }


// =====================IIC总线等待===========================================
// 功能：等待总线状态，超时返回-1
// 参数： void
// 返回：-1，超时；0，空闲返回
// =============================================================================
static int __TWI_WaitForTwiFree(void)
{
    unsigned timeout = CONFIG_I2C_MBB_TIMEOUT;

    while(*pTWIMSTAT & TWIBUSY)
    {
        if(timeout--<1)
        {
            return -1;
        }
    }
    return 0;
}

// =====================查询是否收到ACK信号====================================
// 功能：I2C作为主机发送地址或数据时，从机会发送ACK信号确认，此函数查询主机
// 是否收到ACK信号.
// 参数： type，主机发送数据类型，0表示地址，1表示数据.
// 返回：收到ACK信号，返回0；否则返回-1.
// =============================================================================

static int __TWI_WaitForAck(int type)
{
    unsigned timeout=CONFIG_I2C_TIMEOUT;
    if(type==0)
    {
        while(*pTWIMSTAT&TWIANAK)
        {
            if(timeout--<1)
            {
                return -1;
            }
        }
        return 0;
    }
    else if (type==1)
    {
        while(*pTWIMSTAT&TWIDNAK)
        {
            if(timeout--<1)
            {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

// ==========================发送读/写存储地址============================================
// 功能：发送读/写存储地址。
// 参数： dev,设备指针(IIC寄存器组结构体指针)
//      mem_addr,存储器内部地址,该函数需发送低maddr_len字节到总线
//      maddr_len,存储器内部地址的长度，字节单位；
// 返回：若未成功发送存储地址，返回-1，否则返回发送数据量，字节单位.
// =============================================================================
static int __TWI_WriteAddr(volatile tagI2CReg *dev,u8 *data,int length)

{
    unsigned i=0;
    //将所有数据循环发送，每次发送一字节数据，须等待ack等完成信息
    for(i=0;i<length;i++)
    {
        *pTXTWI8=data[i];
        if(__TWI_WaitForAck(0)!=0)
        {
            return -1;
        }

    }
    return i;

}

//----使能中断--------------------------------------------------------
//功能: 使能iic中断,接收与发送共用一个中断源。
//参数: reg,被操作的寄存器组指针
//返回: 无
//--------------------------------------------------------------------
void _IIC_IntEnable(volatile tagI2CReg *reg)
{
    SETBIT(reg->rTWIIMASK, TWIMCOM);
    SETBIT(reg->rTWIIMASK, TWITXINT);   //enable TX FIFO service interrupts
    SETBIT(reg->rTWIIMASK, TWIRXINT);   //enable RX FIFO service interrupts

}
//----禁止中断--------------------------------------------------------
//功能: 禁止iic中断,接收与发送共用一个中断源。
//参数: reg,被操作的寄存器组指针
//返回: 无
//--------------------------------------------------------------------
void _IIC_IntDisable(volatile tagI2CReg *reg)
{
    CLRBIT(reg->rTWIIMASK, TWIMCOM);
    CLRBIT(reg->rTWIIMASK, TWITXINT);   //disable TX FIFO service interrupts
    CLRBIT(reg->rTWIIMASK, TWIRXINT);   //disable RX FIFO service interrupts
}

//----产生停止时序-----------------------------------------------------------
//功能: 产生停止时序，结束本次与外设备之间的通信(对于SHARC来说，只支持MASTER TX MODE下使用,且未使用DCNT)
//参数: reg,iic外设寄存器基址
//返回: 无
//--------------------------------------------------------------------------
void __IIC_GenerateStop(volatile tagI2CReg *reg)
{
    SETBIT(reg->rTWIMCTL, TWISTOP);
}
//----关闭IIC模块-----------------------------------------------------------
//功能: 关闭IIC模块，禁止中断(本函数并不产生停止时序)
//参数: reg,iic外设寄存器基址
//返回: 无
//--------------------------------------------------------------------------
void _IIC_GenerateDisable(volatile tagI2CReg *reg)
{
    CLRBIT(reg->rTWIIMASK, TWITXINT);   //disable TX FIFO service interrupts
    CLRBIT(reg->rTWIIMASK, TWIRXINT);   //disable RX FIFO service interrupts
    CLRBIT(reg->rTWIIMASK, TWIMCOM);
    CLRBIT(reg->rTWIMITR, TWIEN);
}
// =============================================================================
// 功能: 启动写时序，启动写时序的过程为：器件地址（写）、存储地址（写），当存储地址完
//       成时，需打开中断，重新配置寄存器为接收模式，之后将会发生发送中断，在中断服务
//       函数中，每次发送一定的字节数，直到len数据量，post信号量iic_bus_semp，并产生
//       停止时序
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
//      dev_addr,器件地址的前7比特，最后的三个比特位已经更新，该函数内部需将该地址左
//               移一位，并修改最后的读写比特位，最后一比特写0，表示写，最后一比特写
//               1,表示读;
//      mem_addr,存储器内部地址,该函数需发送低maddr_len字节到总线
//      maddr_len,存储器内部地址的长度，字节单位；
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
                                       struct SemaphoreLCB *iic_semp)
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
    else
    {
        return false;
    }
    fill_little_32bit(mem_addr_buf,0,mem_addr);
    reg=(tagI2CReg *)specific_flag;
    _IIC_IntDisable(reg);

    SETBIT(reg->rTWIMITR, TWIEN);                     //generate START
    //step1:首先发送start信号
    reg->rTWIMCTL |= TWIMEN;                //MASTER MODE

    //step2:发送器件地址，最低位写0.

     if(__TWI_WaitForTwiFree()>=0)
     {
        //reg->rTWIMADDR = TWI_ADDR_GET(dev_addr);              //device addr
        reg->rTWIMADDR =dev_addr;
        reg->rTWIMCTL = 0xFF<<6;                //clean MCTL reg

        //step3:判断是否收到ACK.
         if(__TWI_WaitForAck(0)==0)
        {
        //step4:若收到从机发的ACK，则开始发存储地址.
           if(maddr_len==__TWI_WriteAddr(reg, mem_addr_buf,maddr_len-1));
           {
            *pTXTWI8=mem_addr_buf[maddr_len-1];
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
static bool_t __IIC_GenerateReadStart(ptu32_t  specific_flag,
                                       u8 dev_addr,
                                       u32 mem_addr,
                                       u8 maddr_len,
                                       u32 length,
                                       struct SemaphoreLCB *iic_semp)
{
     volatile tagI2CReg *reg;
      u8 mem_addr_buf[4];
      u32 Recv_Times=0;
      u32 Recv_Index=0;
      u32 i=0;
      u32 Single_Length_Max=0;
      Single_Length_Max=0xFE-maddr_len;
      Recv_Times=(u32)(length/Single_Length_Max);
      Recv_Index=Recv_Times;
      if (length % Single_Length_Max!=0)
      {
        Recv_Times++;
      }

      for(i=0;i<Recv_Index;i++)
  {

      if(specific_flag == CN_IIC_REGISTER_BADDR0)
    {

        if(i!=Recv_Index-1)
        {
        IntParamset0.TransTotalLen = Single_Length_Max;
        IntParamset0.TransCount = 0;
        IntParamset0.pDrvPostSemp = iic_semp;           //iic_bus_semp
        mem_addr=mem_addr+i*Single_Length_Max;
        }
        else
        {
        IntParamset0.TransTotalLen = length-(Recv_Index-1)*Single_Length_Max;
        IntParamset0.TransCount = 0;
        IntParamset0.pDrvPostSemp = iic_semp;           //iic_bus_semp

        }

    }
    else
    {
        return false;
    }


    fill_little_32bit(mem_addr_buf,0,mem_addr);
    reg=(tagI2CReg *)specific_flag;
    _IIC_IntDisable(reg);
    SETBIT(reg->rTWIMITR, TWIEN);                     //generate START

    //step1:首先发送start信号
    SETBIT(reg->rTWIMCTL,TWIMEN);                //MASTER MODE

     //step2:发送器件地址，最低位置0.


     if(__TWI_WaitForTwiFree()>=0)
     {
        //reg->rTWIMADDR = TWI_ADDR_GET(dev_addr);              //device addr
        reg->rTWIMADDR =dev_addr;
        reg->rTWIMCTL = 0xFF<<6;                //clean MCTL reg
     //step3:判断是否收到ACK.
         if(__TWI_WaitForAck(0)==0)
        {
      //step4:若收到从机发的ACK，则开始发存储地址.
            if(__TWI_WriteAddr(reg, mem_addr_buf,maddr_len)==0);
            {
      //step5:若成功发送完存储地址，接着发送一个Repeat Start信号
                SETBIT(reg->rTWIMCTL,TWIRSTART);
      //step6:发送器件地址，最低位置1.
                SETBIT(reg->rTWIMCTL,TWIMDIR);
                reg->rTWIMADDR = TWI_ADDR_GET(dev_addr);
      //step7:开中断
                _IIC_IntEnable(reg);

                if(i!=Recv_Index-1)
                {
                   Lock_SempPend(iic_semp,CN_TIMEOUT_FOREVER);
                }
                else
                {
                   return true;
                }

            }
       }
     }
  }
     return false;
}

// =============================================================================
// 功能: 结束本次读写回调函数，区分读写的不同停止时序，当属于发送时，则直接
//       停止时序；若为读，则先停止回复ACK，再停止.
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
// 返回: 无
// =============================================================================
static void __IIC_GenerateEnd(ptu32_t specific_flag)
{
    volatile tagI2CReg *reg;
    reg = (tagI2CReg *)specific_flag;

    /*if(CHKBIT(reg->TWIMCTL,TWIMDIR))
    {
      //接收模式

    }*/

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

    static struct IIC_IntParamSet *IntParam;
    static struct IIC_CB *ICB;
    tagI2CReg *reg;
    u8 ch;
    u32 IicErrorNo;
    u32 irptl_temp=*rTWIIRPTL;//read IRPTL

    reg = (tagI2CReg*)CN_IIC_REGISTER_BADDR0;
    ICB=&s_IIC0_CB;
    IntParam=&IntParamset0;

        //MASTER TX\RX COMPLETE
        if( (irptl_temp & TWITXINT) != 0 )      //发送中断
        {
            if(!(CHKBIT(reg->rTWIMSTAT, TWIANAK)|CHKBIT(reg->rTWIMSTAT, TWIDNAK)))
            {
                //从泛设备读一个字节的数据，并发送
                if(IIC_PortRead(ICB,&ch,1) > 0)
                {
                    *rTXTWI8 = ch;
                    IntParam->TransCount++;
                }
                else if(IntParam->TransCount == IntParam->TransTotalLen)
                {
                    //in Master TX Mode , we need to STOP TWI by ourself
                    Lock_SempPost(IntParam->pDrvPostSemp);
                     __IIC_GenerateStop(reg);
                }
                else
                {
                 IicErrorNo = CN_IIC_NO_ACK_ERR;//调用错处处理API函数
                 IIC_ErrPop(ICB,IicErrorNo);
                }
            }
            else    //TX no ACK
            {
                IicErrorNo = CN_IIC_NO_ACK_ERR;//调用错处处理API函数
                IIC_ErrPop(ICB,IicErrorNo);
                return 1;
            }
            //clear IIC interrupt
            irptl_temp = TWITXINT;
            *rTWIIRPTL = irptl_temp;
        }
        else if( (irptl_temp & TWIRXINT) != 0 )      //接收中断
        {
            if(!(CHKBIT(reg->rTWIMSTAT, TWIANAK)|CHKBIT(reg->rTWIMSTAT, TWIDNAK)))
            {
                ch = *rRXTWI8;
                IIC_PortWrite(ICB,&ch,1);
                IntParam->TransCount ++;
                if(IntParam->TransCount == IntParam->TransTotalLen)
                {
                    __IIC_GenerateStop(reg);
                    Lock_SempPost(IntParam->pDrvPostSemp);//释放总线信号量
                }
            }

            else    //RX no ACK
            {

            }
            //clear IIC interrupt
            irptl_temp = TWIRXINT;
            *rTWIIRPTL = irptl_temp;
        }
        else    //TWIMERR
        {
        }

    irptl_temp==*rTWIIRPTL; //update TWI_IRPTL

        //MASTER TRANS COMPLETE
        if( (irptl_temp & TWIMCOM) != 0 )
        {
            _IIC_GenerateDisable(reg);

            Lock_SempPost(ICB->iic_bus_semp);//释放总线信号量

            //clear STOP
            CLRBIT(reg->rTWIMCTL, TWISTOP);
            //clear IIC interrupt
            irptl_temp |= TWIMCOM;
            *rTWIIRPTL = irptl_temp;
        }

    return 0;
}

// --------I2C时钟初始化-----------------------------------------------------
//功能：I2C初始化函数，根据I2C协议，初始化时只需要配置时钟,
//        ppc1020提供两个独立i2c，其时钟来自CCB/2，再经过i2c
//        时钟分频，配置I2CFDR寄存器，分频系数并无特定的
//        计算方法，本API通过枚举参数确定四种可配置的波特率.
//参数：I2Cn，被操作的i2c设备，为枚举量n可为1或2；
//      tagI2CConfig，枚举量，I2C_CLOCK_RATE_50K，时钟为50K
//返回：true=成功，false=失败
// -------------------------------------------------------------------------
void __iic_set_clk(volatile tagI2CReg *reg,enum_I2C_Clock i2cclk)
{
    //I2C bus clk devider

    reg->rTWIMITR = CN_CFG_PCLK1/(10*Mhz);      // reference clock = 10 MHz

    switch(i2cclk)
    {
    case I2C_CLOCK_RATE_50K:
        reg->rTWIDIV = 0xCDCD;  //clk=10.227 MHz/0xCD=10.227 MHz/205 = 49.9K
        //I2C_clk_cycle = 20;     //一个时钟周期20us
        break;
    case I2C_CLOCK_RATE_100K:
        reg->rTWIDIV = 0x6666;  //clk=10.227 MHz/0x66=10.227 MHz/102 = 102.3K
        //I2C_clk_cycle = 10;     //一个时钟周期10us
        break;
    case I2C_CLOCK_RATE_200K:
        reg->rTWIDIV = 0x3333;  //clk=10.227 MHz/0x33=10.227 MHz/51 = 200.5K
        //I2C_clk_cycle = 5;        //一个时钟周期5us
        break;
    case I2C_CLOCK_RATE_400K:
        reg->rTWIDIV = 0x1A1A;  //clk=10.227 MHz/0x1A=10.227 MHz/26 = 393.3K
        //I2C_clk_cycle = 3;        //一个时钟周期2.5us,向上取整
        break;
    default:
        break;
    }
}



// =============================================================================
// 功能: IIC默认硬件初始化配置，主要是时钟配置及使能IIC模块。
// 参数: RegBaseAddr,寄存器基址。
// 返回: 无
// =============================================================================

static void __IIC_HardDefaultSet(ptu32_t RegBaseAddr)
{
    volatile tagI2CReg *reg;
    reg=(tagI2CReg *)RegBaseAddr;
    SETBIT(reg->rTWIMITR,TWIEN);
    __iic_set_clk(reg,I2C_CLOCK_RATE_100K);

}


// =============================================================================
// 功能: IIC中断配置函数
// 参数: IntLine,中断线
//       isr,中断服务函数指针
// 返回: 无
// =============================================================================
static void __IIC_IntConfig(u32 IntLine,u32 (*isr)(ptu32_t))
{
    Int_Register(cn_int_line_TWII);
    Int_IsrConnect(cn_int_line_TWII,__IIC_ISR);
    Int_SettoAsynSignal(cn_int_line_TWII);
    Int_ClearLine(cn_int_line_TWII);     //清掉初始化产生的发送fifo空的中断
    Int_RestoreAsynLine(cn_int_line_TWII);

}


// =============================================================================
// 功能：IIC总线控制回调函数，被上层调用，目前只实现对IIC时钟频率配置
// 参数：specific_flag,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：无
// =============================================================================
static void __IIC_BusCtrl(ptu32_t spceific_flag,u32 cmd,u32 data1,u32 data2)
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
    struct IIC_Param IIC0_Config;
    IIC0_Config.BusName="IIC0";
    IIC0_Config.SimpleBuf=(u8 *)&s_IIC0Buf;
    IIC0_Config.SimpleBufLen=IIC0_BUF_LEN;
    IIC0_Config.SpecificFlag=CN_IIC0_BADDR;
    IIC0_Config.GenerateWriteStart=__IIC_GenerateWriteStart;
    IIC0_Config.GenerateReadStart  = __IIC_GenerateReadStart;
    IIC0_Config.GenerateEnd        = __IIC_GenerateEnd;
    IIC0_Config.BusCtrl            = __IIC_BusCtrl;

    __IIC_HardDefaultSet(CN_IIC_REGISTER_BADDR0);
    __IIC_IntConfig(cn_int_line_TWII,__IIC_ISR);

    if(NULL==IIC_BusAdd_r(&IIC0_Config,&s_IIC0_CB))
    {
        return 0;
    }
    return 1;

}





