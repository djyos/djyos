//-----------------------------------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//
//  源码改自ut2416开发板
//

#include <string.h>
#include <stdlib.h>
#include <driver/card/card.h>
//
// 寄存器
//
#define  rHM1_BLKSIZE    	(*(volatile unsigned short*)0x4A800004)	// Host buffer boundary and transfer block size register
#define  rHM1_BLKCNT	    (*(volatile unsigned short*)0x4A800006)	// Block count for current transfer
#define  rHM1_ARGUMENT   	(*(volatile unsigned int*)  0x4A800008)	// Command Argument
#define  rHM1_TRNMOD	    (*(volatile unsigned short*)0x4A80000C)	// Transfer Mode setting register
#define  rHM1_CMDREG	    (*(volatile unsigned short*)0x4A80000E)	// Command register
#define  rHM1_RSPREG0    	(*(volatile unsigned int*)  0x4A800010)	// Response 0
#define  rHM1_RSPREG1    	(*(volatile unsigned int*)  0x4A800014)	// Response 1
#define  rHM1_RSPREG2    	(*(volatile unsigned int*)  0x4A800018)	// Response 2
#define  rHM1_RSPREG3    	(*(volatile unsigned int*)  0x4A80001C)	// Response 3
#define  rHM1_BDATA      	(*(volatile unsigned int*)  0x4A800020)	// Buffer Data register
#define  rHM1_PRNSTS     	(*(volatile unsigned int*)  0x4A800024)	// Present state
#define  rHM1_HOSTCTL    	(*(volatile unsigned char*) 0x4A800028)	// Host control
#define  rHM1_CLKCON     	(*(volatile unsigned short*)0x4A80002C)	// Clock control
#define  rHM1_SWRST      	(*(volatile unsigned char*) 0x4A80002F)	// Software reset
#define  rHM1_TIMEOUTCON    (*(volatile unsigned char*) 0x4A80002E)	// Time out control
#define  rHM1_NORINTSTS  	(*(volatile unsigned short*)0x4A800030)	// Normal interrupt status
#define  rHM1_ERRINTSTS  	(*(volatile unsigned short*)0x4A800032)	// Error interrupt status
#define  rHM1_NORINTSTSEN   (*(volatile unsigned short*)0x4A800034)	// Normal interrupt status enable
#define  rHM1_ERRINTSTSEN   (*(volatile unsigned short*)0x4A800036)	// Error interrupt status enable
#define  rHM1_CONTROL2  	(*(volatile unsigned int*)  0x4A800080)	// Control 2
#define  rHM1_CONTROL3  	(*(volatile unsigned int*)  0x4A800084)	// Control 3
#define  rLOCKCON1          (*(volatile unsigned *)     0x4C000004) // EPLL lock time count
#define  rEPLLCON           (*(volatile unsigned *)     0x4C000018) // EPLL configuration
#define  rCLKSRC            (*(volatile unsigned *)     0x4C000020) // Clock source control
#define  rCLKDIV1           (*(volatile unsigned *)     0x4C000028) // Clock divider ratio control
#define  rSCLKCON     		(*(volatile unsigned *)     0x4C000038) // Special clock enable
#define  rGPLCON			(*(volatile unsigned *)     0x560000F0)
#define  rGPHCON			(*(volatile unsigned *)     0x56000070)
#define  rMISCCR			(*(volatile unsigned *)     0x56000080)


struct CardDescription Card;
s32 __IssueCommand_CH1( u16 Cmd, u32 Arg, u32 IsACmd);
s32 __WaitForTransferComplete_CH1(void);
void __ClockConfig_CH1(u32 ClkSrc, u32 Divisior, struct CardDescription Card);
s32 __IsCardInProgrammingState_CH1(struct CardDescription Card);
s32 S3c2416_HsmmcInit(u8 Channel);
s32 __WaitForCommandComplete_CH1(void);
s32 __WaitForBufferWriteReady_CH1(void);
s32 __WaitForBufferReadReady_CH1(void);
void __GetCSD_CH1(struct CardDescription *Card);
s32 __GetSCR_CH1(struct CardDescription *Card);
s32 __SetDataTransferWidth_CH1(struct CardDescription Card);
void __SetCommandReg_CH1(u16 Cmd,u32 IsACmd);
s32 __SetMMC_OCR_CH1(void);
s32 __SetSD_OCR_CH1(void);
void __SetSDSpeedMode_CH1(u32 SpeedMode);
void __SetMMCSpeedMode_CH1(u32 SpeedMode);
void __SetClock_CH1(u32 ClkSrc, u16 Divisor);
void __SetTransferModeReg_CH1(u32 MultiBlk, u32 DataDirection, u32 AutoCmd12En, u32 BlockCntEn, u32 DmaEn);
void __ClearCommandCompleteStatus_CH1(void);
void __ClearErrInterruptStatus_CH1(void);
void __ClearTransferCompleteStatus_CH1(void);
void __ClearBufferReadReadyStatus_CH1(void);
void __ClearBufferWriteReadyStatus_CH1(void);


//-----------------------------------------------------------------------------
//功能: HSMMC控制器初始化
//参数:
//返回: 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
s32 S3c2416_HsmmcInit(u8 Channel)
{
	u32 Arg;
	
	if(1 != Channel)
		return (-1);// 目前只支持SD1
	
	rGPLCON = (rGPLCON & (~0xFFFFF)) | (0xAAAAA);// I/O配置
	
	rHM1_SWRST = 0x3;// reset
	rHM1_CLKCON &= (~(0x1<<2));// 关时钟
	rSCLKCON |= (1<<13);//
	__SetClock_CH1(1, 0x40);//设置并开启时钟
	rHM1_TIMEOUTCON = 0xE;// 设置timeout
	rHM1_HOSTCTL &= (~(0x1<<2));// Normal Speed(25M)
	
	rHM1_NORINTSTS = rHM1_NORINTSTS;// 清中断状态
	rHM1_ERRINTSTS = rHM1_ERRINTSTS;
	rHM1_NORINTSTSEN = 0xFF;// 使能中断
	rHM1_ERRINTSTSEN = 0xFF;
	
	//将设备重置到Idle State
	__IssueCommand_CH1(0, 0, 0);
	Card.State = IDLE;
	
	//CM8判断是否为v2.0器件
	Arg = (0x1<<8) | (0xaa<<0);
	if(__IssueCommand_CH1(8, Arg, 0))
	{
		Card.SpecVer = V2;
	}
	
	//设置OCR,工作电压
	if((V2 != Card.SpecVer) && (__SetMMC_OCR_CH1()))//判断是不是MMC
	{
		Card.Type = MMC;
		printf("\r\nMMC card is detected\n");
	}
	else
	{
		switch(__SetSD_OCR_CH1())
		{
			case 1: Card.Type = SDSC; break;
			case 2: Card.Type = SDHC; break;
			default: return (-1);
		}
		printf("\r\n SD card is detected\n");
	}
	
	//读CID
	__IssueCommand_CH1(2,0,0);	
	printf("\r\n Product Name : %c%c%c%c%c-v%d.%d",((rHM1_RSPREG2>>24)&0xFF),
			((rHM1_RSPREG2>>16)&0xFF), ((rHM1_RSPREG2>>8)&0xFF), (rHM1_RSPREG2&0xFF),
			((rHM1_RSPREG1>>24)&0xFF), ((rHM1_RSPREG1>>20)&0xF), ((rHM1_RSPREG1>>16)&0xF));
	Card.State = IDENT;
	
	//设置RCA
	// Send RCA(Relative Card Address). It places the card in the STBY state
	Card.RCA = (MMC == Card.Type) ? 0x0001 : 0x0000;
 	__IssueCommand_CH1(3, Card.RCA, 0);			
	if(MMC != Card.Type)
	{
		Card.RCA = (rHM1_RSPREG0 >> 16) & 0xFFFF;
		printf("\r\n RCA = 0x%x\n", Card.RCA);
	}
	Card.State = STANDBY;
	printf("\r\n Enter into the Stand-by State.");
	
	//读CSD
	__GetCSD_CH1(&Card);
	
	//选择SD卡,进入transfer state
	__IssueCommand_CH1(7, Card.RCA, 0);
	Card.State = TRANSFER;
	
	//配置工作频率
	__GetSCR_CH1(&Card);
	__ClockConfig_CH1(2, 8, Card);
	
	while (!__IsCardInProgrammingState_CH1(Card));
	
	Card.BusWidth = 4;
	while (!__SetDataTransferWidth_CH1(Card));

	while (!__IsCardInProgrammingState_CH1(Card));

	Card.BlkLenSettings = 9;
	while(!__IssueCommand_CH1(16, (1<<Card.BlkLenSettings), 0));//Set the One Block size

	//清中断
	rHM1_NORINTSTS = 0xFFFF;
	return (0);
}

//-----------------------------------------------------------------------------
//功能: 写块
//参数: 
//返回: 0 -- 成功; -1 -- 参数错误; -2 -- 写错误;
//备注: 
//-----------------------------------------------------------------------------
s32 S3c2416_BlkWrite(const u32 *Buf, u32 BlkNo, u32 Count)
{	
	u32 i, j;
	
	if(NULL == Buf || 0 == Count)
		return (-1);
		
	rHM1_BLKSIZE = (7 << 12) | (1 << Card.BlkLenSettings);
	rHM1_BLKCNT = Count;
	rHM1_ARGUMENT = BlkNo << Card.BlkLenSettings;// StartAddr*512
	
	if(Count == 1)// single block
	{
		__SetTransferModeReg_CH1(0, 0, 1, 1, 0);
		__SetCommandReg_CH1(24, 0);
	}
	else// multi block
	{
		__SetTransferModeReg_CH1(1, 0, 1, 1, 0);
		__SetCommandReg_CH1(25, 0);
	}
	
	if (!__WaitForCommandComplete_CH1())
	{
		printf("\r\ Command is NOT completed");
		return (-2);
	}
	__ClearCommandCompleteStatus_CH1();	
	
	for(j = 0; j < Count; j++)
	{
		if (!__WaitForBufferWriteReady_CH1())
		{
			printf("\r\nWriteBuffer NOT Ready");
			return (-2);
		}
		__ClearBufferWriteReadyStatus_CH1();

		for(i = 0; i < (1<< (Card.BlkLenSettings-2)); i++)//512 byte should be writed.
			rHM1_BDATA = *Buf++;
	}
	
	if(!__WaitForTransferComplete_CH1())
	{
		__ClearErrInterruptStatus_CH1();
		printf(("\r\nTransfer is NOT Complete\n"));
		return (-2);
	}
				
	__ClearTransferCompleteStatus_CH1();
	return (0);
}

//-----------------------------------------------------------------------------
//功能: 读块
//参数:
//返回: 0 -- 成功; -1 -- 参数错误; -2 -- 读失败;
//备注: 
//-----------------------------------------------------------------------------
s32 S3c2416_BlkRead(u32 *Buf, u32 BlkNo, u32 Count)
{
	u32 i, j;
	
	if(NULL == Buf || 0 == Count)
		return (-1);
	
	rHM1_BLKSIZE = (7<<12) | (1 << Card.BlkLenSettings);
	rHM1_BLKCNT = Count;
	rHM1_ARGUMENT = BlkNo << Card.BlkLenSettings;// StartAddr*512
	
	if(Count == 1)// single block
	{
		__SetTransferModeReg_CH1(0, 1, 0, 1, 0);
		__SetCommandReg_CH1(17, 0);
	}
	else// multi block
	{
		__SetTransferModeReg_CH1(1, 1, 1, 1, 0);
		__SetCommandReg_CH1(18, 0);
	}
	
	if (!__WaitForCommandComplete_CH1())
	{
		printf("\r\n Command is NOT completed\n");
		return (-2);
	}
		
	__ClearCommandCompleteStatus_CH1();
	
	for(j = 0; j < Count; j++)
	{
		if (!__WaitForBufferReadReady_CH1())
		{
			printf("\r\nReadBuffer NOT Ready\n");
			return (-2);
		}

		__ClearBufferReadReadyStatus_CH1();

		for(i = 0; i < (1 << (Card.BlkLenSettings-2)); i++)//512 byte should be writed.
			*Buf++ = rHM1_BDATA;
	}
	
	if(!__WaitForTransferComplete_CH1())
	{
		__ClearErrInterruptStatus_CH1();
		printf(("Transfer is NOT Complete\n"));
		return (-2);
	}
				
	__ClearTransferCompleteStatus_CH1();
	return (0);
}

//-----------------------------------------------------------------------------
//功能: 擦除块
//参数:
//返回: 0 -- 成功; -2 -- 参数错误
//备注:
//-----------------------------------------------------------------------------
s32 S3c2416_BlkErase(u32 BlkNo, u32 Count)
{
	if(MMC == Card.Type || 0 == Count)
		return (-1);
	
	rHM1_ARGUMENT = BlkNo << Card.BlkLenSettings;// StartAddr*512
	__SetCommandReg_CH1(32, 0);//开始地址

	while (!__WaitForCommandComplete_CH1());
	{
		//return (-2);
	}
	__ClearCommandCompleteStatus_CH1();
	
	rHM1_ARGUMENT = (BlkNo + Count) << Card.BlkLenSettings;// StartAddr*512
	__SetCommandReg_CH1(33, 0);//结束地址

	while (!__WaitForCommandComplete_CH1());
	{
		//return (-2);
	}
	__ClearCommandCompleteStatus_CH1();
	
	rHM1_ARGUMENT = 0;
	__SetCommandReg_CH1(38, 0);
	
	return (0);
}

//-----------------------------------------------------------------------------
//功能: 发起命令
//参数:
//返回: 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
s32 __IssueCommand_CH1( u16 Cmd, u32 Arg, u32 IsACmd)
{
	u32 Sfr;

	while ((rHM1_PRNSTS & 0x1));// Check CommandInhibit_CMD

	if (!IsACmd)// R1b type commands have to check CommandInhibit_DAT bit
	{
		if((Cmd==6 && (MMC ==Card.Type)) || (Cmd==7) || (Cmd==12) || (Cmd==28) ||
		   (Cmd==29) || (Cmd==38) || (((Cmd==42) || (Cmd==56)) && (MMC !=Card.Type)))
		{
			do	
			{
				Sfr = rHM1_PRNSTS;
			} 
			while((Sfr & 0x2));// Check CommandInhibit_DAT
		}
	}
	// Argument Setting
	if (!IsACmd)// Normal Command (CMD)
	{
		if((Cmd==3) || (Cmd==4) || (Cmd==7) || (Cmd==9) || (Cmd==10) || (Cmd==13) || 
		   (Cmd==15) || (Cmd==55))
		{   
			rHM1_ARGUMENT = (Arg<<16);
		}	
		else
		{
			rHM1_ARGUMENT = Arg;
		}	
	}
	else// APP.Commnad (ACMD)
	{
		rHM1_ARGUMENT = Arg;
	}
	__SetCommandReg_CH1(Cmd, IsACmd);

	if (!__WaitForCommandComplete_CH1())
		printf(("\r\nCommand NOT Complete\n"));
	else
		__ClearCommandCompleteStatus_CH1();//清完成中断位	

	if (!(rHM1_NORINTSTS & 0x8000))
	{
		return 1;
	}
	else//
	{
		printf("\r\nCommand = %d, Error Stat = %x\n",(rHM1_CMDREG>>8), rHM1_ERRINTSTS);
		rHM1_ERRINTSTS = rHM1_ERRINTSTS;
		rHM1_NORINTSTS = rHM1_NORINTSTS;
		return 0;								
	}
}
//-----------------------------------------------------------------------------
//功能: 设置命令寄存器
//参数:
//返回: 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
void __SetCommandReg_CH1(u16 Cmd,u32 IsACmd)
{
	u16 Sfr = 0;
	
	if (!IsACmd)//No ACMD
	{
		/* R2: 136-bits Resp.*/
		if ((Cmd==2) || (Cmd==9) || (Cmd==10))
		{
			Sfr = (Cmd<<8) | (0<<4) | (1<<3) | (1<<0);
		}
		/* R1,R6,R5: 48-bits Resp. */
		else if ((Cmd==3) || (Cmd==8) || (Cmd==13) || (Cmd==16) || (Cmd==27) || 
		         (Cmd==30) || (Cmd==32) || (Cmd==33) || (Cmd==35) || (Cmd==36) || 
				 (Cmd==42) || (Cmd==55) || (Cmd==56))
		{
			Sfr = (Cmd<<8) | (1<<4) | (1<<3) | (2<<0);
		}
		else if ((Cmd==11) || (Cmd==14) || (Cmd==17) || (Cmd==18) || (Cmd==19) || 
		         (Cmd==20) || (Cmd==24) || (Cmd==25))
		{
			Sfr = (Cmd<<8) | (1<<5) | (1<<4) | (1<<3) | (2<<0);
		}
		/* R1b,R5b: 48-bits Resp. */
		else if ((Cmd==6) || (Cmd==7) || (Cmd==12) || (Cmd==28) || (Cmd==29) || (Cmd==38))
		{
			if (Cmd==12)
			{
				Sfr = (Cmd<<8) | (3<<6) | (1<<4) | (1<<3) | (3<<0);
			}
			else if (Cmd==6)
			{
				if((MMC !=Card.Type))// SD card
					Sfr=(Cmd<<8)|(1<<5)|(1<<4)|(1<<3)|(2<<0);
				else// MMC card
					Sfr=(Cmd<<8)|(1<<4)|(1<<3)|(3<<0);
			}
			else
			{
				Sfr = (Cmd<<8) | (1<<4) | (1<<3) | (3<<0);
			}	
		}
		/* R3,R4: 48-bits Resp.  */
		else if (Cmd==1)
		{
			Sfr = (Cmd<<8) | (0<<4) | (0<<3) | (2<<0);
		}
		/* No-Resp. */
		else
		{
			Sfr = (Cmd<<8) | (0<<4) | (0<<3) | (0<<0);
		}	
	}
	else// ACMD
	{
		if ((Cmd==6) || (Cmd==22) || (Cmd==23))// R1
			Sfr = (Cmd<<8) | (1<<4) | (1<<3) | (2<<0);
		else if ((Cmd==13) || (Cmd==51))
			Sfr = (Cmd<<8) | (1<<5) | (1<<4) | (1<<3) | (2<<0);
		else
			Sfr = (Cmd<<8) | (0<<4) | (0<<3) | (2<<0);
	}
	rHM1_CMDREG = Sfr;
}
//-----------------------------------------------------------------------------
//功能: 等待命令完成
//参数:
//返回: 1 -- 成功; 0 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 __WaitForCommandComplete_CH1(void)
{
	u32 Loop = 0;

	while (!(rHM1_NORINTSTS & 0x1))//轮询方式
	{
		if (((Loop % 500000) == 0) && (Loop>0))
			return (0);// 超时
		Loop++;
	}
	return (1);
}
//-----------------------------------------------------------------------------
//功能: 清命令完成中断位
//参数:
//返回: 1 -- 成功;
//备注:
//-----------------------------------------------------------------------------
void __ClearCommandCompleteStatus_CH1(void)
{
	rHM1_NORINTSTS = (1<<0);// 清完成位
	while (rHM1_NORINTSTS & 0x1)// 查看是否清除
		rHM1_NORINTSTS = (1<<0);
}
//-----------------------------------------------------------------------------
//功能: 设置MMC的工作电压
//参数:
//返回: 1 -- 成功; 0 -- 失败;
//备注: 未测试过
//-----------------------------------------------------------------------------
s32 __SetMMC_OCR_CH1(void)
{
	u32 i, OCR;
	
	for (i=0; i<250; i++)
	{
		__IssueCommand_CH1(1, 0x0, 0);
		OCR = rHM1_RSPREG0 | (1<<30);	
		__IssueCommand_CH1(1, OCR, 0);// (Ocr:2.7V~3.6V)
		if (rHM1_RSPREG0 & (0x1<<31))
		{			
			if(rHM1_RSPREG0 & (1<<7))
				printf("\r\nVoltage range : 1.65V ~ 1.95V");
			else if(rHM1_RSPREG0 & (1<<18))
				printf("\r\nVoltage range: 2.7V ~ 3.1V\n");	
			else if(rHM1_RSPREG0 & (1<<19))
				printf("\r\nVoltage range: 2.7V ~ 3.2V\n");	
			else if(rHM1_RSPREG0 & (1<<20))
				printf("\r\nVoltage range: 2.7V ~ 3.3V\n");	
			else if(rHM1_RSPREG0 & (1<<21))
				printf("\r\nVoltage range: 2.7V ~ 3.4V\n");

			if(rHM1_RSPREG0 & (1<<30))
			{
				printf("\r\nHigh Capacity Sector Mode Operation\n");
			}

			printf("\nrHM_RSPREG0 = %x\n", rHM1_RSPREG0);
			return 1;
		}
	}
	__ClearErrInterruptStatus_CH1();
	return 0;
}
//-----------------------------------------------------------------------------
//功能: 清错误中断位
//参数:
//返回: 1 -- 成功;
//备注:
//-----------------------------------------------------------------------------
void __ClearErrInterruptStatus_CH1(void)
{
	while (rHM1_NORINTSTS & (0x1<<15))
	{
		rHM1_NORINTSTS = rHM1_NORINTSTS;
		rHM1_ERRINTSTS = rHM1_ERRINTSTS;
	}
}
//-----------------------------------------------------------------------------
//功能: 设置SD的工作电压
//参数:
//返回: 1 -- 成功, SDSC卡; 2 -- 成功, SDHC卡; 0 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 __SetSD_OCR_CH1(void)
{
	u32 i;
	s32 Ret = 0;
	
	for(i = 0; i < 250*10; i++)
	{
		__IssueCommand_CH1(55, 0x0000, 0); // CMD55 (For ACMD)
		__IssueCommand_CH1(41, 0x40FF8000, 1); // (Ocr:2.7V~3.6V)
		if (rHM1_RSPREG0 & (0x1<<31))// initialization complete
		{
			if(rHM1_RSPREG0 & (1<<7))
				printf("\r\nVoltage range : 1.65V ~ 1.95V");	
			else if(rHM1_RSPREG0 & (1<<20))
				printf("\r\nVoltage range: 2.7V ~ 3.3V\n\n");	
			else if(rHM1_RSPREG0 & (1<<21))
				printf("\r\nVoltage range: 2.7V ~ 3.4V\n\n");
			else if(rHM1_RSPREG0 & (1<<23))
				printf("\r\nVoltage range: 2.7V ~ 3.6V\n\n");

			if(rHM1_RSPREG0 & (0x1<<30))
			{
				printf("\r\nHigh Capacity Card\n");
				Ret = 2;
				break;
			}
			
			Ret = 1;
			break;
		}
		//Delay(10);
	}
	// The current card is MMC card, then there's time out error, need to be cleared.
	__ClearErrInterruptStatus_CH1();
	return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 解析卡的CSD寄存器
//参数:
//返回: 1 -- 成功;
//备注:
//-----------------------------------------------------------------------------
void __GetCSD_CH1(struct CardDescription *Card)
{
	u32 CardSize, OneBlockSize;
	
	__IssueCommand_CH1(9, Card->RCA, 0);// Send CSD命令
	
	if((MMC == Card->Type))
	{
#if 0
		// todo: 为找到MMC手册
		Card->SpecVer = (rHM1_RSPREG3 >> 18) & 0xF;
		printf("\r\n m_ucMMCSpecVer_ch1=%d\n", m_ucMMCSpecVer_ch1);

#endif
	}
	else
	{
		printf("\r\n CSD version: %d", (rHM1_RSPREG3>>30) & 0xFF);
		Card->TransSpeed = ((rHM1_RSPREG2>>24) & 0xFF);
	}

	Card->ReadBlLen = (u8)((rHM1_RSPREG2 >> 8) & 0xf) ;
	Card->ReadBlPartial = (u8)((rHM1_RSPREG2 >> 7) & 0x1) ;
	Card->CSize = (u16)(((rHM1_RSPREG2 & 0x3) << 10) | ((rHM1_RSPREG1 >> 22) & 0x3ff));
	Card->CSizeMult = (u8)((rHM1_RSPREG1>>7) & 0x7);
	Card->CapacityInBytes = (1 << Card->ReadBlLen) * (Card->CSize + 1) * (1 << (Card->CSizeMult + 2));
	
	CardSize = Card->CapacityInBytes / 1048576;// 转为Mb
	OneBlockSize = (1 << Card->ReadBlLen);

	printf("\r\n TRAN_SPEED: 0x%x", Card->TransSpeed);
	printf("\r\n READ_BL_LEN: %d", Card->ReadBlLen);
	printf("\r\n READ_BL_PARTIAL: %d", Card->ReadBlPartial);
	printf("\r\n C_SIZE: %d", Card->CSize);
	printf("\r\n C_SIZE_MULT: %d", Card->CSizeMult);
	printf("\r\n One Block Size: %dByte", OneBlockSize);
	printf("\r\n Total Card Size: %dMByte", (CardSize+1));

}
//-----------------------------------------------------------------------------
//功能: 设置时钟
//参数:
//返回: 1 -- 成功;
//备注:
//-----------------------------------------------------------------------------
void __ClockConfig_CH1(u32 ClkSrc, u32 Divisior, struct CardDescription Card)
{
	u32 SrcFreq, WorkingFreq;
	u32 HCLK = 133000000;

	rLOCKCON1 = 0x800;
	rCLKSRC |= (1<<6);// EPLL Output
	rEPLLCON = ((33<<16) | (1<<8) | (2));
	rEPLLCON &= (~(1<<24));// EPLL ON 
	rCLKDIV1 = (rCLKDIV1 & (~(0x3<<6))) | (0x0<<6);
	rMISCCR = (rMISCCR & (~(0x7<<8))) | (1<<8);
	rGPHCON = (rGPHCON & (~(0x3<<28))) | (1<<29);//
	
	if (ClkSrc == 1)
		SrcFreq = HCLK;
	else if (ClkSrc == 2)// Epll Out 48MHz
		SrcFreq = 100000000;
	else
		ClkSrc = HCLK;

	if (Divisior != 0)
	{
		WorkingFreq = SrcFreq / (Divisior * 2);
		printf("\r\n HCLK = %d, SD WorkingFreq = %dMHz", HCLK, WorkingFreq/(1000000));
	}
	else
	{
		WorkingFreq = SrcFreq;	
		printf("\r\n WorkingFreq = %dMHz", WorkingFreq/(1000000));
	}

	if (MMC == Card.Type)
	{
		if(V4 == Card.SpecVer)
		{
			// It is necessary to enable the high speed mode in the card before 
			// changing the clock freq to a freq higher than 20MHz.
			if (WorkingFreq > 20000000)
			{
				__SetMMCSpeedMode_CH1(1);// 高速
				printf("\r\n Set MMC High speed mode OK!!\n");
			}
			else
			{
				__SetMMCSpeedMode_CH1(0);// 低速
				printf("\r\n Set MMC Normal speed mode OK!!\n");
			}
		}
		else // old version
			printf("\r\n Old version MMC card can not support working frequency higher than 25MHz");
	}
	else// SD卡
	{
		if (Card.SpecVer > V1)
		{
			if (WorkingFreq > 25000000)
			{
				// Higher than 25MHz, should use high speed mode. Max 50MHz and 25MB/sec
				__SetSDSpeedMode_CH1(1);// 高速
				printf("\r\n Set SD High speed mode OK!!");
			}
			else
			{
				__SetSDSpeedMode_CH1(0);// 低速
				printf("\r\n Set SD Normal speed mode OK!!");
			}
		}
		else 
			printf("\r\n Old version SD card can not support working frequency higher than 25MHz");
	}

	// Higher than 25MHz, it is necessary to enable high speed mode of the host controller.
	if (WorkingFreq > 25000000)
	{
		rHM1_HOSTCTL &= (~(0x1<<2));
		rHM1_HOSTCTL |= 1<<2;
	}
	else
	{
		rHM1_HOSTCTL &= (~(0x1<<2));
	}

	rHM1_CLKCON &= (~(0x1<<2)); // when change the sd clock frequency, need to stop sd clock.
	
	__SetClock_CH1(ClkSrc, Divisior); 
}
//-----------------------------------------------------------------------------
//功能: 设置SD卡速度
//参数: SpeedMode -- 1: 高速;0: 低速;
//返回: 1 -- 成功;
//备注:
//-----------------------------------------------------------------------------
void __SetSDSpeedMode_CH1(u32 SpeedMode)
{
	u32 Sfr;
	u32 Arg = 0;
	int i;

	if (!__IssueCommand_CH1(16, 64, 0)) // CMD16
		printf("\r\n CMD16 fail");
	else
	{
		rHM1_BLKSIZE = (7 << 12) | (64);
		rHM1_BLKCNT = 1;
		rHM1_ARGUMENT = 0;
		__SetTransferModeReg_CH1(0, 1, 0, 0, 0);

		Arg = (0x1<<31) | (0xffff<<8) | (SpeedMode<<0);
		if (!__IssueCommand_CH1(6, Arg, 0))
			printf("\r\n CMD6 fail");
		else
		{
			__WaitForBufferReadReady_CH1();
			__ClearBufferReadReadyStatus_CH1();
			
			for(i=0; i<16; i++)
			{
				Sfr = rHM1_BDATA	;
			}			

			__WaitForTransferComplete_CH1();
			__ClearTransferCompleteStatus_CH1();
		}
	}	
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 
//备注:
//-----------------------------------------------------------------------------
void __SetTransferModeReg_CH1(u32 MultiBlk, u32 DataDirection, u32 AutoCmd12En, 
                            u32 BlockCntEn, u32 DmaEn)
{
	rHM1_TRNMOD = (rHM1_TRNMOD & ~(0xffff)) | (MultiBlk<<5) | (DataDirection<<4) | 
	              (AutoCmd12En<<2) | (BlockCntEn<<1) | (DmaEn<<0);
}



//-----------------------------------------------------------------------------
//功能: 获取SCR寄存器
//参数: 
//返回: 1 -- 成功; 0 -- 失败;
//备注: todo:格式待确定
//-----------------------------------------------------------------------------
s32 __GetSCR_CH1(struct CardDescription *Card)
{
	u32 SCR1, SCR2;

	if (!__IssueCommand_CH1(16, 8, 0))
		return 0;// 失败
	
	rHM1_BLKSIZE = (7<<12) | (8);
	rHM1_BLKCNT = 1;
	rHM1_ARGUMENT = 0;
	__SetTransferModeReg_CH1(0, 1, 0, 0, 0);
		
	if (!__IssueCommand_CH1(55, Card->RCA, 0))// CMD55 (For ACMD)
		return 0;
		
	if (!__IssueCommand_CH1(51, 0, 1))// Acmd51
		return 0;
	
	__WaitForBufferReadReady_CH1();
	__ClearBufferReadReadyStatus_CH1();

	SCR1 = rHM1_BDATA;
	SCR2 = rHM1_BDATA;

	__WaitForTransferComplete_CH1();
	__ClearTransferCompleteStatus_CH1();

	printf("\r\n SCR:[%x][%x]", SCR2, SCR1);

	if ((SCR1 & 0xf) == 0x0)
	{
		Card->SpecVer = V1; // Version 1.0 ~ 1.01
		printf("\r\n Card SpecVer: v1.0 ~ v1.01");
	}	
	else if ((SCR1 & 0xf) == 0x1)
	{
		Card->SpecVer = V1_1; // Version 1.10, support cmd6
		printf("\r\n Card SpecVer: v1.10");
	}	
	else if((SCR1 & 0xf) == 0x2)
	{
		Card->SpecVer = V2; // Version 2.0 support cmd6 and cmd8
		printf("\r\n Card SpecVer: v2.0");
	}
	else
	{
		Card->SpecVer = UNKNOWN;
		printf("\r\n Card SpecVer: unknown");
	}
	
	if(SCR1 & 0x100)
		Card->DataStatusAfterErases = 1;
	else
		Card->DataStatusAfterErases = 0;
	return 1;		
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 
//备注:
//-----------------------------------------------------------------------------
void __SetClock_CH1(u32 ClkSrc, u16 Divisor)
{
	rHM1_CONTROL2 = (rHM1_CONTROL2 & ~(0xffffffff)) | (0x1<<15) | (0x1<<14) | 
	                (0x1<<8) | (ClkSrc<<4);
	rHM1_CONTROL3 = (1<<31) | (1<<23) | (1<<15) | (1<<7);
	rHM1_CLKCON = (rHM1_CLKCON & ~((0xff<<8) | (0x1))) | (Divisor<<8) | (1<<0);
	while (!(rHM1_CLKCON & 0x2));// 等待时钟稳定	
	
	rHM1_CLKCON |= (0x1<<2);
	while(!(rHM1_CLKCON & (0x1<<3)));// 等待时钟稳定
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 
//备注:
//-----------------------------------------------------------------------------
s32 __IsCardInProgrammingState_CH1(struct CardDescription Card)
{
	if (!__IssueCommand_CH1(13, Card.RCA, 0))
	{
		printf("\r\nCard status = %x", ((rHM1_RSPREG0>>9) & 0xf));
		return 0;
	}
	else
	{
		if(((rHM1_RSPREG0>>9) & 0xf) == 4)
		{
			printf("\r\nCard is transfer status\n");
			return 1;
		}
		return 0;
	}
}
//-----------------------------------------------------------------------------
//功能:
//参数: 
//返回: 
//备注: 未测试
//-----------------------------------------------------------------------------
void __SetMMCSpeedMode_CH1(u32 SpeedMode)
{
	u32 Arg = 0;
	
	Arg = (3<<24) | (185<<16) | (SpeedMode<<8); // Change to the high-speed mode
	
	while(!__IssueCommand_CH1(6, Arg, 0));
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
s32 __SetDataTransferWidth_CH1(struct CardDescription Card)
{
	u8 BitMode=0;
	u32 Arg=0;
	u8 HostCtrlVal = 0;
	u32 BusWidth;

	switch (Card.BusWidth)
	{
		case 8:
			BusWidth = (MMC == Card.Type) ? 8 : 4;
			break;
		case 4:
			BusWidth = 4;
			break;
		case 1:
			BusWidth = 1;
			break;
		default :
			BusWidth = 4;
			break;
	}

	rHM1_NORINTSTSEN &= 0xFEFF; // 关闭状态中断

	if(MMC != Card.Type)// SD Card Case
	{
		if (!__IssueCommand_CH1(55, Card.RCA, 0))
			return 0;

		if (Card.BusWidth == 1)
		{
			BitMode = 0;
			if (!__IssueCommand_CH1(6, 0, 1)) // 1-bits
				return 0;
		}
		else
		{
			BitMode = 1;
			if (!__IssueCommand_CH1(6, 2, 1)) // 4-bits
				return 0;
		}

	}
	else //  MMC Card Case
	{
		if (V4 == Card.SpecVer) // It is for a newest MMC Card
		{
			if (BusWidth==1)
				BitMode = 0;
			else if (BusWidth==4)
				BitMode = 1;//4            		// 4-bit bus
			else
				BitMode = 2;//8-bit bus

			Arg = ((3<<24)|(183<<16)|(BitMode<<8));
			while(!__IssueCommand_CH1(6, Arg, 0));
		}
		else
			BitMode = 0;
	}

	if (BitMode == 2)
	{
		HostCtrlVal &= 0xdf;
		HostCtrlVal |= 1<<5;
	}
	else
	{
		HostCtrlVal &= 0xfd;
		HostCtrlVal |= BitMode<<1;
	}

	rHM1_HOSTCTL = HostCtrlVal;
	rHM1_NORINTSTSEN |= (1 << 8);// 使能状态中断

	return 1;
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
s32 __WaitForBufferWriteReady_CH1(void)
{
	u32 Loop = 0;

	while (!(rHM1_NORINTSTS & 0x10))
	{
		if (((Loop % 500000) == 0) && Loop>0)
			return 0;
		Loop++;
	}
	return 1;
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
s32 __WaitForBufferReadReady_CH1(void)
{
	u32 Loop = 0;

	while (!(rHM1_NORINTSTS & 0x20))
	{
		if (((Loop % 500000) == 0) && Loop>0)
			return 0;
		Loop++;
	}
	return 1;
}
//-----------------------------------------------------------------------------
//功能: 检查传输结束中断位
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
s32 __WaitForTransferComplete_CH1(void)
{
	u32 Loop = 0;

	while (!(rHM1_NORINTSTS & 0x2))
	{
		if (((Loop % (500000)) == 0) && (Loop > 0))
		{
			u16 Test0 = rHM1_NORINTSTS;
			u16 Test1 = rHM1_ERRINTSTS;
			return 0;
		}
		Loop++;
	}
	return 1;
}
//-----------------------------------------------------------------------------
//功能: 清写缓冲中断位
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void __ClearBufferWriteReadyStatus_CH1(void)
{
	rHM1_NORINTSTS = (1<<4);
	while (rHM1_NORINTSTS & 0x10)
		rHM1_NORINTSTS = (1<<4);
}
//-----------------------------------------------------------------------------
//功能: 清读缓冲中断位
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void __ClearBufferReadReadyStatus_CH1(void)
{
	rHM1_NORINTSTS = (1<<5);
	while (rHM1_NORINTSTS & 0x20)
		rHM1_NORINTSTS = (1<<5);
}


//-----------------------------------------------------------------------------
//功能: 清传输中断位
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void __ClearTransferCompleteStatus_CH1(void)
{

	rHM1_NORINTSTS = (1<<1);
	while (rHM1_NORINTSTS & 0x2)
		rHM1_NORINTSTS = (1<<1);
}

//-----------------------------------------------------------------------------
//功能: 驱动测试
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void SDRawTest(void)
{

	u32 i;
	u32 *Buf = (u32*)malloc(512*4);
	for(i = 0; i < 512; i++)
		Buf[i] = i+2;

	//PrintBuf(Buf, 512*4);
	S3c2416_HsmmcInit(1);

	S3c2416_BlkWrite(Buf, 1, 4);

	memset(Buf, 0, 512*4);

	S3c2416_BlkRead(Buf, 1, 4);

	PrintBuf(Buf, 512*4);
	
	S3c2416_BlkErase(1, 4);
	
	S3c2416_BlkRead(Buf, 1, 4);

	PrintBuf(Buf, 512*4);

}
