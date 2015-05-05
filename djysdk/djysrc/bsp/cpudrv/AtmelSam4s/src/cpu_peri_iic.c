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
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_iic.c
// 模块描述: IIC模块底层硬件驱动，寄存器级别的操作
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 05/12.2014
// =============================================================================
#include "cpu_peri.h"
#include <string.h>
#include "endian.h"
#include "iicbus.h"

//==============================================================================
#define CN_IIC_REGISTER_BADDR0        0x40018000
#define CN_IIC_REGISTER_BADDR1        0x4001C000

//中断中使用的数据类型结构体
struct tagIIC_IntParamSet
{
	struct tagSemaphoreLCB *pDrvPostSemp;	//信号量
	u32 TransCount;						//传输数据量计数器
	u32 TransTotalLen;
};

//定义IIC的GPIO引脚数组
static const Pin IIC_GPIO_PIN[]={
		{PIO_PA3A_TWD0,PIOA,ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT},
		{PIO_PA4A_TWCK0,PIOA,ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT}
};

//定义静态变量
static struct tagIIC_CB s_tIIC0_CB;
#define IIC0_BUF_LEN  		128
#define IIC0_DMA_BUF_LEN 	64
static u8 s_IIC0Buf[IIC0_BUF_LEN];
static u8 s_IIC0DmaBuf[IIC0_DMA_BUF_LEN];
static struct tagIIC_IntParamSet IntParamset0;

static struct tagIIC_CB s_tIIC1_CB;
#define IIC1_BUF_LEN  		128
#define IIC1_DMA_BUF_LEN 	64
static u8 s_IIC1DmaBuf[IIC1_DMA_BUF_LEN];
static u8 s_IIC1Buf[IIC1_BUF_LEN];
static struct tagIIC_IntParamSet IntParamset1;

//静态变量，是否使用DMA方式发送接收，默认为否
static volatile bool_t s_IIC_DmaUsed[2] = {false,false};

// =============================================================================
// 功能：对相应的I2C外设进行软件复位
// 参数：iic相应外设寄存器的基址
// 返回：无
// =============================================================================
static void __IIC_Reset(volatile tagI2CReg *reg)
{
    reg->TWI_CR = TWI_CR_SWRST;
}

// =============================================================================
// 功能: 使能iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
//       IntSrc,中断源
// 返回: 无
// =============================================================================
static void __IIC_IntDisable(volatile tagI2CReg *reg,u32 IntSrc)
{
    reg->TWI_IDR = IntSrc;
}

// =============================================================================
// 功能: 禁止iic中断
// 参数: reg,被操作的寄存器组指针
//       IntSrc,操作的中断源
// 返回: 无
// =============================================================================
static void __IIC_IntEnable(volatile tagI2CReg *reg,u32 IntSrc)
{
    reg->TWI_IER = IntSrc;
}

// =============================================================================
// 功能: 禁止iic中断,接收与发送共用一个中断源。
// 参数: reg,被操作的寄存器组指针
// 返回: 无
// =============================================================================
static void __IIC_GenerateStop(volatile tagI2CReg *reg)
{
	reg->TWI_CR = TWI_CR_STOP;
}

// =============================================================================
// 功能：IIC时钟配置函数，时钟来源为MCK，计算方法可参考技术手册
// 参数：reg，设备句柄
//      iicclk，欲配置的时钟速度，标准的时钟如50KHz,100KHz,200KHz
// 返回：true=成功，false=失败
// =============================================================================
static void __IIC_SetClk(volatile tagI2CReg *reg,u32 iicclk)
{
    uint32_t dwCkDiv = 0 ;
    uint32_t dwClDiv ;
    uint32_t dwOk = 0 ;

    /* Configure clock */
    while ( !dwOk )
    {
        dwClDiv = ((CN_CFG_MCLK/ (2 * iicclk)) - 4) / (1<<dwCkDiv) ;

        if ( dwClDiv <= 255 )
        {
            dwOk = 1 ;
        }
        else
        {
            dwCkDiv++ ;
        }
    }

    reg->TWI_CWGR = 0 ;
    reg->TWI_CWGR = (dwCkDiv << 16) | (dwClDiv << 8) | dwClDiv ;
}

// =============================================================================
// 功能：IIC传输的DMA配置函数，配置控制器使用DMA方式
// 参数：
// 返回：
// =============================================================================
static void __IIC_DMA_Config(volatile tagI2CReg* Reg,u32 cmd)
{
	u8 Port;
	if((u32)Reg == CN_IIC_REGISTER_BADDR0)
		Port = CN_IIC0;
	else
		Port = CN_IIC1;

	if(cmd == CN_IIC_DMA_USED)				//配置使用DMA方式发送和接收
	{
		s_IIC_DmaUsed[Port] = true;			//标记使用DMA
		//关闭不使用DMA时的中断
		__IIC_IntDisable(Reg,TWI_IDR_RXRDY|TWI_IDR_TXRDY);
		Reg->TWI_PTCR = TWI_PTCR_RXTEN | TWI_PTCR_TXTEN;
	}
	else									//配置使用非DMA方式发送和接收
	{
		s_IIC_DmaUsed[Port] = false;
		__IIC_IntDisable(Reg,TWI_IDR_ENDTX|TWI_IDR_ENDRX);
		Reg->TWI_PTCR = TWI_PTCR_RXTDIS | TWI_PTCR_TXTDIS;
	}
	//配置DMA参数
	Reg->TWI_TCR   = 0;
	Reg->TWI_TNCR  = 0;
    Reg->TWI_RCR   = 0;
	Reg->TWI_RNCR  = 0;
}

// =============================================================================
// 功能：IIC引脚初始化，初始化为外设使用
// 参数：iic_no,IIC控制器号
// 返回：无
// =============================================================================
static void __IIC_GpioInit(u32 iic_no)
{
    if(iic_no == CN_IIC0)
    {
//        GPIO_CfgPinFunc(IIC_GPIO_PIN,PIO_LISTSIZE(IIC_GPIO_PIN));
    	PIO_Configure(IIC_GPIO_PIN,PIO_LISTSIZE(IIC_GPIO_PIN));
    }
    else if(iic_no == CN_IIC1)
    {
    }
}

// =============================================================================
// 功能: IIC默认硬件初始化配置，主要是时钟配置和GPIO写保护引脚配置
// 参数: RegBaseAddr,寄存器基址
//       iicno,IIC控制器号
// 返回: 无
// =============================================================================
static void __IIC_HardConfig(ptu32_t RegBaseAddr,u8 iicno)
{
	volatile tagI2CReg *reg;
	reg = (volatile tagI2CReg *)RegBaseAddr;

    __IIC_GpioInit(iicno);

    reg->TWI_CR = TWI_CR_SWRST;//reset the twi
    reg->TWI_RHR;        //clear the rhr
    //config master mode
    reg->TWI_CR = TWI_CR_MSDIS;
    reg->TWI_CR = TWI_CR_SVDIS;

    /* Set master mode */
    reg->TWI_CR = TWI_CR_MSEN;

    //config the SCK
    reg->TWI_CWGR = 0 ;
    reg->TWI_CWGR = (1 << 16) | (248 << 8) | 248 ;//100KHz
}

// =============================================================================
// 功能: IIC中断配置函数
// 参数: IntLine,中断线
//       ISR,中断服务函数指针
// 返回: 无
// =============================================================================
static void __IIC_IntConfig(u32 IntLine,u32 (*ISR)(ufast_t))
{
    //中断线的初始化
    Int_IsrConnect(IntLine,ISR);
    Int_SettoAsynSignal(IntLine);
    Int_ClearLine(IntLine);     //清掉初始化产生的发送fifo空的中断
    Int_RestoreAsynLine(IntLine);
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
//      length,发送的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，
//          当接收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//      iic_semp,发送完成时驱动需释放的信号量。发送程序读IIC_PortRead时，若读不到数
//          则需释放该信号量，产生结束时序
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
static bool_t __IIC_GenerateWriteStart(volatile tagI2CReg *Reg,
									   u8 dev_addr,
									   u32 mem_addr,
									   u8 maddr_len,
									   u32 length,
									   struct tagSemaphoreLCB *iic_semp)
{
    struct tagIIC_CB *IIC;
    struct tagIIC_IntParamSet *IntParam;
    u8 ch,*pDmaBuf,Port,DmaBufLen;

    //通过specific_flag区分是哪条总线
    if((u32)Reg == CN_IIC_REGISTER_BADDR0)
    {
    	IntParam = &IntParamset0;
    	IIC = &s_tIIC0_CB;
    	pDmaBuf = s_IIC0DmaBuf;
    	Port = CN_IIC0;
    	DmaBufLen = IIC0_DMA_BUF_LEN;
    }
    else if((u32)Reg == CN_IIC_REGISTER_BADDR1)
    {
    	IntParam = &IntParamset1;
    	IIC = &s_tIIC1_CB;
    	pDmaBuf = s_IIC1DmaBuf;
    	Port = CN_IIC1;
    	DmaBufLen = IIC1_DMA_BUF_LEN;
    }
    else
    {
    	return false;
    }
    IntParam->TransTotalLen = length;
    IntParam->TransCount = 0;
    IntParam->pDrvPostSemp = iic_semp;

    Reg->TWI_MMR = 0;
    Reg->TWI_MMR = (maddr_len<<8)|(dev_addr<<16);
    Reg->TWI_IADR = 0;
    Reg->TWI_IADR = TWI_IADR_IADR(mem_addr);
    Reg->TWI_CR = TWI_CR_START;

    if(s_IIC_DmaUsed[Port] == false)		//使用非DMA方式 发送
    {
		//发送一个字节的数据
		while(!(Reg->TWI_SR & TWI_SR_TXRDY));
		IIC_PortRead(IIC,&ch,1);

		Reg->TWI_THR = ch;
		IntParam->TransCount++;
	    __IIC_IntEnable(Reg,TWI_IER_TXRDY);
    }
    else									//配置DMA发送时的参数
    {
    	while(!(Reg->TWI_SR & TWI_SR_TXRDY));
    	ch = IIC_PortRead(IIC,pDmaBuf,DmaBufLen);
		Reg->TWI_TPR  = (u32)pDmaBuf;
		Reg->TWI_TCR  = ch;
		Reg->TWI_PTCR = TWI_PTCR_TXTEN;
        __IIC_IntEnable(Reg,TWI_IER_ENDTX);
    }

    return true;
}

// =============================================================================
// 功能: 启动读时序，启动读时序的过程为：器件地址（写）、存储地址（写）、器件地址（读）
//       当器件地址（读）完成时，需打开中断，重新配置寄存器为接收模式，之后将会发生
//       接收数据中断，在中断中将接收到的数据调用IIC_PortWrite写入缓冲，接收到len字
//       节数的数据后，释放信号量iic_semp
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
//      dev_addr,器件地址的前7比特，最后的三个比特位已经更新，该函数内部需将该地址左
//               移一位，并修改最后的读写比特位，最后一比特写0，表示写，最后一比特写
//               1,表示读;
//      mem_addr,存储器内部地址,该函数需发送低maddr_len字节到总线
//      maddr_len,存储器内部地址的长度，字节单位；
//      len,接收的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，当接
//          收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//      iic_semp,读完成时，驱动需释放的信号量（缓冲区信号量）
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
static bool_t __IIC_GenerateReadStart( volatile tagI2CReg *Reg,
									   u8 dev_addr,
									   u32 mem_addr,
									   u8 maddr_len,
									   u32 length,
									   struct tagSemaphoreLCB *iic_semp)
{
    u8 DmaRcvLen,*pDmaBuf,Port,DmaBufLen;

    if((u32)Reg == CN_IIC_REGISTER_BADDR0)
    {
    	IntParamset0.TransTotalLen = length;
    	IntParamset0.TransCount = 0;
    	IntParamset0.pDrvPostSemp = iic_semp;				//iic_buf_semp
    	pDmaBuf = s_IIC0DmaBuf;
    	Port = CN_IIC0;
    	DmaBufLen = IIC0_DMA_BUF_LEN;
    }
    else if((u32)Reg == CN_IIC_REGISTER_BADDR1)
    {
    	IntParamset1.TransTotalLen = length;
    	IntParamset1.TransCount = 0;
    	IntParamset1.pDrvPostSemp = iic_semp;
    	pDmaBuf = s_IIC1DmaBuf;
    	Port = CN_IIC1;
    	DmaBufLen = IIC1_DMA_BUF_LEN;
    }
    else
    {
    	return false;
    }
    Reg->TWI_RHR;

    //TWI_MMR_IADRSZ_2_BYTE; len ; device addr
    Reg->TWI_MMR = 0;
    Reg->TWI_MMR = (maddr_len<<8)|(TWI_MMR_MREAD)|(dev_addr<<16);
    Reg->TWI_IADR = 0;
    Reg->TWI_IADR = TWI_IADR_IADR(mem_addr);
    Reg->TWI_CR = TWI_CR_START;

    if(s_IIC_DmaUsed[Port] == false)
    	__IIC_IntEnable(Reg,TWI_IER_RXRDY|TWI_IER_TXRDY);
    else
    {
    	DmaRcvLen = length>DmaBufLen ? DmaBufLen:length;
		Reg->TWI_RPR  = (uint32_t)pDmaBuf;
		Reg->TWI_RCR  = DmaRcvLen;
		Reg->TWI_PTCR = TWI_PTCR_RXTEN;
        __IIC_IntEnable(Reg,TWI_IER_ENDRX);
    }

    return true;
}

// =============================================================================
// 功能: 结束本次读写回调函数，区分读写的不同停止时序，当属于发送时，则直接停止时序，
//      若为读，则先停止回复ACK，再停止
// 参数: specific_flag,个性标记，本模块内即IIC寄存器基址
// 返回: 无
// =============================================================================
static void __IIC_GenerateEnd(volatile tagI2CReg *Reg)
{
	if(((u32)Reg != CN_IIC_REGISTER_BADDR0) &&
			(u32)Reg == CN_IIC_REGISTER_BADDR1)
		return;

//    __IIC_IntDisable(Reg, TWI_IDR_TXRDY|TWI_IDR_RXRDY|TWI_IDR_TXCOMP);
	__IIC_IntDisable(Reg, TWI_IDR_TXRDY|TWI_IDR_RXRDY|
			TWI_IDR_ENDRX|TWI_IDR_ENDTX|TWI_IDR_TXCOMP);
    Djy_EventDelay(100);
    __IIC_GenerateStop(Reg);

    __IIC_Reset(Reg);
}

// =============================================================================
// 功能：IIC总线控制回调函数，被上层调用，目前只实现对IIC时钟频率配置
// 参数：specific_flag,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：无
// =============================================================================
static s32 __IIC_BusCtrl(volatile tagI2CReg *Reg,u32 cmd,u32 data1,u32 data2)
{
	if(((u32)Reg != CN_IIC_REGISTER_BADDR0) &&
			(u32)Reg != CN_IIC_REGISTER_BADDR1)
		return 0;

	switch(cmd)
	{
	case CN_IIC_SET_CLK:
		__IIC_SetClk(Reg,data1);
		break;
	case CN_IIC_DMA_USED:
	case CN_IIC_DMA_UNUSED:
		__IIC_DMA_Config(Reg,cmd);
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
static u32 __IIC_ISR(ufast_t IntLine)
{
    static struct tagIIC_CB *ICB;
    static struct tagIIC_IntParamSet *IntParam;
    tagI2CReg *reg;

    u8 ch,Port,*PDmaBuf;
    u32 twi_sr,twi_imr,DmaBufLen,temp;

    if(IntLine == CN_INT_LINE_TWI0)
    {
        ICB = &s_tIIC0_CB;
        IntParam = &IntParamset0;
        reg = (tagI2CReg *)CN_IIC_REGISTER_BADDR0;
        Port = CN_IIC0;
        PDmaBuf = s_IIC0DmaBuf;
        DmaBufLen = IIC0_DMA_BUF_LEN;
    }
    else if(IntLine == CN_INT_LINE_TWI1)
    {
        ICB = &s_tIIC1_CB;
        IntParam = &IntParamset1;
        reg = (tagI2CReg *)CN_IIC_REGISTER_BADDR1;
        Port = CN_IIC1;
        PDmaBuf = s_IIC1DmaBuf;
        DmaBufLen = IIC1_DMA_BUF_LEN;
    }
    else
    {
        return 0;
    }

    twi_sr = reg->TWI_SR;
    twi_imr = reg->TWI_IMR;

    if(s_IIC_DmaUsed[Port] == false)		//使用非DMA发送和接收
    {
		if((twi_sr & TWI_SR_RXRDY) && (twi_imr & TWI_IMR_RXRDY))//接收中断
		{
			ch = reg->TWI_RHR;
			if(IIC_PortWrite(ICB,&ch,1))
				IntParam->TransCount ++;
			if(IntParam->TransCount == IntParam->TransTotalLen - 1)
			{
				__IIC_GenerateStop(reg);   //倒数第一个时写stop
			}
			else if(IntParam->TransCount == IntParam->TransTotalLen)
			{
				__IIC_GenerateStop(reg); 
				__IIC_IntDisable(reg,TWI_IDR_RXRDY);
				__IIC_IntEnable(reg,TWI_IER_TXCOMP);
			}
		}
		else if((twi_sr & TWI_SR_TXRDY) && (twi_imr & TWI_IMR_TXRDY))//发送
		{
			if(IIC_PortRead(ICB,&ch,1))
			{
				reg->TWI_THR = ch;
				IntParam->TransCount ++;
			}
			else if(IntParam->TransCount == IntParam->TransTotalLen)
			{
				__IIC_IntDisable(reg,TWI_IDR_TXRDY);
				__IIC_IntEnable(reg,TWI_IER_TXCOMP);
				__IIC_GenerateStop(reg);
			}
		}
		else if((twi_sr & TWI_SR_TXCOMP) && (twi_imr & TWI_IMR_TXCOMP))
		{
			__IIC_IntDisable(reg,TWI_IDR_TXCOMP);
			Lock_SempPost(IntParam->pDrvPostSemp);
		}
    }
    else			//使用DMA方式发送和接收数据
    {
    	if((twi_sr & TWI_SR_ENDRX) && (twi_imr & TWI_IMR_ENDRX))//接收中断
    	{
    		reg->TWI_PTCR = TWI_PTCR_RXTDIS;
    		//计算本次缓冲区中有多少数据
    		if(IntParam->TransTotalLen - IntParam->TransCount > DmaBufLen)
    		{
    			temp = DmaBufLen;
    			IIC_PortWrite(ICB,PDmaBuf,temp);
    			IntParam->TransCount += temp;
    			//计算下次DMA接收的数据大小
    			if(IntParam->TransTotalLen - IntParam->TransCount > DmaBufLen)
    				temp = DmaBufLen;
    			else
    				temp = IntParam->TransTotalLen - IntParam->TransCount;
    			//配置下次DMA接收
    			reg->TWI_RPR = (u32)PDmaBuf;
    			reg->TWI_RCR = temp;
    			reg->TWI_PTCR = TWI_PTCR_RXTEN;
    		}
    		else					//表明接收到了全部的数据
    		{
    			temp = IntParam->TransTotalLen - IntParam->TransCount;
    			IIC_PortWrite(ICB,PDmaBuf,temp);
    			__IIC_GenerateStop(reg);

				__IIC_IntDisable(reg,TWI_IDR_ENDRX);
				__IIC_IntEnable(reg,TWI_IER_TXCOMP);
    		}
    	}
    	else if((twi_sr & TWI_SR_ENDTX) && (twi_imr & TWI_IMR_ENDTX))//发送
    	{
    		reg->TWI_PTCR = TWI_PTCR_TXTDIS;
    		//计算发生本次中断，DMA发送的字节数
    		if(IntParam->TransTotalLen - IntParam->TransCount > DmaBufLen)
    		{
    			IntParam->TransCount += DmaBufLen;
    			//计算本次需发送多少数据
				if(IntParam->TransTotalLen - IntParam->TransCount > DmaBufLen)
					temp = DmaBufLen;
				else
					temp = IntParam->TransTotalLen - IntParam->TransCount;
				IIC_PortRead(ICB,PDmaBuf,temp);
				//配置DMA发送
				reg->TWI_TPR = (u32)PDmaBuf;
				reg->TWI_TCR = temp;
    			reg->TWI_PTCR = TWI_PTCR_TXTEN;
    		}
    		else					//表明DMA已经传输结束
    		{
    			IntParam->TransCount = IntParam->TransTotalLen;
				__IIC_IntDisable(reg,TWI_IDR_ENDTX);
				__IIC_IntEnable(reg,TWI_IER_TXCOMP);
				__IIC_GenerateStop(reg);
    		}

    	}
		else if((twi_sr & TWI_SR_TXCOMP) && (twi_imr & TWI_SR_TXCOMP))
		{
			__IIC_IntDisable(reg,TWI_IDR_TXCOMP);
			Lock_SempPost(IntParam->pDrvPostSemp);
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
	struct tagIIC_Param IIC0_Config;

	IIC0_Config.BusName 		   = "IIC0";
	IIC0_Config.IICBuf 		       = (u8*)&s_IIC0Buf;
	IIC0_Config.IICBufLen   	   = IIC0_BUF_LEN;
	IIC0_Config.SpecificFlag 	   = CN_IIC_REGISTER_BADDR0;
	IIC0_Config.pGenerateWriteStart = (WriteStartFunc)__IIC_GenerateWriteStart;
	IIC0_Config.pGenerateReadStart  = (ReadStartFunc)__IIC_GenerateReadStart;
	IIC0_Config.pGenerateEnd        = (GenerateEndFunc)__IIC_GenerateEnd;
	IIC0_Config.pBusCtrl            = (IICBusCtrlFunc)__IIC_BusCtrl;

	__IIC_HardConfig(CN_IIC_REGISTER_BADDR0,0);
	__IIC_IntConfig(CN_INT_LINE_TWI0,__IIC_ISR);

    PMC_EnablePeripheral(CN_INT_LINE_TWI0);

	if(NULL == IIC_BusAdd_r(&IIC0_Config,&s_tIIC0_CB))
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
	struct tagIIC_Param IIC1_Config;

	IIC1_Config.BusName 		   = "IIC1";
	IIC1_Config.IICBuf 		       = (u8*)&s_IIC1Buf;
	IIC1_Config.IICBufLen   	   = IIC1_BUF_LEN;
	IIC1_Config.SpecificFlag 	   = CN_IIC_REGISTER_BADDR1;
	IIC1_Config.pGenerateWriteStart = (WriteStartFunc)__IIC_GenerateWriteStart;
	IIC1_Config.pGenerateReadStart  = (ReadStartFunc)__IIC_GenerateReadStart;
	IIC1_Config.pGenerateEnd        = (GenerateEndFunc)__IIC_GenerateEnd;
	IIC1_Config.pBusCtrl            = (IICBusCtrlFunc)__IIC_BusCtrl;

	__IIC_HardConfig(CN_IIC_REGISTER_BADDR1,0);
	__IIC_IntConfig(CN_INT_LINE_TWI1,__IIC_ISR);

    PMC_EnablePeripheral(CN_INT_LINE_TWI1);

	if(NULL == IIC_BusAdd_r(&IIC1_Config,&s_tIIC1_CB))
		return 0;
	return 1;
}

