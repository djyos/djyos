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

#include <string.h>
#include <stdlib.h>
#include <cpu_peri.h>
#include <djyos.h>
#include <driver.h>
#include <driver/flash/flash.h>
#include <spibus.h>

// 
//  开发板源码,BITBAND逻辑
//
#define BITBAND(addr, bitnum) 		((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  			*((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   	MEM_ADDR(BITBAND(addr, bitnum)) 
#define GPIOB_ODR_Addr    			(GPIOB_BASE+20) //0x40020414 
#define PBout(n)   					BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
//
//
//
static struct SPI_Device *s_pSpiPort; // 器件使用的SPI端口
static struct FlashChip *s_pChip; // 测试用
//-----------------------------------------------------------------------------
//功能: 片选
//参数: 
//返回: 
//备注:
//-----------------------------------------------------------------------------
static void __CS_On(void)
{
	PBout(14) = 0;
}

//-----------------------------------------------------------------------------
//功能: 片选
//参数: 
//返回: 
//备注:
//-----------------------------------------------------------------------------
static void __CS_Off(void)
{
	PBout(14) = 1;
}

//-----------------------------------------------------------------------------
//功能: Nor专用逻辑，写使能
//参数: 
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static s32 __WriteEnable(void)
{
	struct SPI_DataFrame CommandFrame;
	s32 Ret = 0;
	u8 Command[1] = {0x06};
	
	CommandFrame.RecvBuf = NULL;
	CommandFrame.RecvLen = 0;
	CommandFrame.RecvOff = 0;
	CommandFrame.SendBuf = Command;
	CommandFrame.SendLen = 1;
	
	__CS_On();
	
	if(CN_SPI_EXIT_NOERR != 
			SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER))
	{
		Ret = -1;
	}
	
	__CS_Off();
	
	return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 获取Nor状态
//参数: 
//返回: 0 -- 成功; -1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static s32 __Status1(u8 *Status)
{
	struct SPI_DataFrame CommandFrame;
	s32 Ret = 0;
	u8 Command[1] = {0x05};
	
	CommandFrame.RecvBuf = Status;
	CommandFrame.RecvLen = 1;
	CommandFrame.RecvOff = 1;
	CommandFrame.SendBuf = Command;
	CommandFrame.SendLen = 1;
	
	__CS_On();
	
	if(CN_SPI_EXIT_NOERR != 
			SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER))
	{
		Ret = -1;
	}
	
	__CS_Off();
	
	return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 检查操作是否完成
//参数: 
//返回:  -1 -- 未完成; 0 -- 完成;
//备注:
//-----------------------------------------------------------------------------
static s32 __Done(void)
{
	u8 Status;
	
	if(__Status1(&Status))
		return ( -1);
		
	if(Status & 0x1)	
		return (-1);
		
	return (0);	
}

//-----------------------------------------------------------------------------
//功能: 获取Nor ID信息
//参数: 
//返回: 非零 -- ID信息; 0 -- 失败;
//备注:
//-----------------------------------------------------------------------------
static u16 __ID(void)
{
	struct SPI_DataFrame CommandFrame;
	s32 Ret = 0;
	u16 ID;
	u8 Command[4];

	Command[0] = 0x90;
	Command[1] = 0x0;
	Command[2] = 0x0;
	Command[3] = 0x0;

	CommandFrame.RecvBuf = (u8*)(&ID);
	CommandFrame.RecvLen = 2;
	CommandFrame.RecvOff = 4;
	CommandFrame.SendBuf = Command;
	CommandFrame.SendLen = 4;

	__CS_On();

	Ret = SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER);
	if(CN_SPI_EXIT_NOERR != Ret)
	{
		ID = 0;
	}

	__CS_Off();

	return (ID);
}

//-----------------------------------------------------------------------------
//功能: 擦除一个sector
//参数: SectorNo -- 扇号
//返回: 0 -- 成功; 1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 w25qxx_SectorErase(u32 SectorNo)
{
	struct SPI_DataFrame CommandFrame;
	s32 Ret = 0;
	u8 Command[4];
	
	SectorNo = (SectorNo << 4); // SectorNo转PageNo
	Command[0] = 0x20;
	Command[1] = ((SectorNo >> 8) & 0xFF);
	Command[2] = (SectorNo & 0xFF);
	Command[3] = 0x0;
	
	CommandFrame.RecvBuf = NULL;
	CommandFrame.RecvLen = 0;
	CommandFrame.RecvOff = 0;
	CommandFrame.SendBuf = Command;
	CommandFrame.SendLen = 4;
	
	if(__WriteEnable())
		return (-1);
	
	__CS_On();
	
	if(CN_SPI_EXIT_NOERR !=
			SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER))
	{
		Ret = -1;
	}	

	__CS_Off();
	
	Djy_EventDelay(400000);// 切出//延时切出.

	if((0 == Ret) && (__Done()))
	{
		Ret = -1;
	}
	
	return (Ret);
}
//-----------------------------------------------------------------------------
//功能: 擦除一个block
//参数: BlkNo -- 块号
//返回: 0 -- 成功; 1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 w25qxx_BlkErase(u32 BlkNo)
{
	struct SPI_DataFrame CommandFrame;
	s32 Ret = 0;
	u8 Command[4];

	Command[0] = 0xD8;
	Command[1] = (BlkNo & 0xFF);
	Command[2] = 0x0;
	Command[3] = 0x0;

	CommandFrame.RecvBuf = NULL;
	CommandFrame.RecvLen = 0;
	CommandFrame.RecvOff = 0;
	CommandFrame.SendBuf = Command;
	CommandFrame.SendLen = 4;

	if(__WriteEnable())
		return (-1);

	__CS_On();

	if(CN_SPI_EXIT_NOERR !=
			SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER))
	{
		Ret = -1;
	}

	__CS_Off();

	Djy_EventDelay(1000000);// 切出//延时切出.

	if((0 == Ret) && (__Done()))
	{
		Ret = -1;
	}

	return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 擦除整个芯片
//参数:
//返回: 0 -- 成功; 1 -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 w25qxx_ChipErase(void)
{
	struct SPI_DataFrame CommandFrame;
	s32 Ret = 0;
	u8 Command[1] = {0xC7};

	CommandFrame.RecvBuf = NULL;
	CommandFrame.RecvLen = 0;
	CommandFrame.RecvOff = 0;
	CommandFrame.SendBuf = Command;
	CommandFrame.SendLen = 1;

	if(__WriteEnable())
		return (-1);

	__CS_On();

	if(CN_SPI_EXIT_NOERR !=
			SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER))
	{
		Ret = -1;
	}

	__CS_Off();

	Djy_EventDelay(40000000);// 切出//延时切出.

	if((0 == Ret) && (__Done()))
	{
		Ret = -1;
	}

	return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 读一页
//参数: PageNo -- 页号;
//      Data -- 读缓冲;
//      Flags -- 保留;
//返回: >0 -- 读出字节数; -1 -- 参数错误; -2 -- 读失败
//备注:
//-----------------------------------------------------------------------------
s32 w25qxx_PageRead(u32 PageNo, u8 *Data, u32 Flags)
{
	struct SPI_DataFrame CommandFrame;
	s32 Ret = -2;
	u8 Command[4];
	
	if(NULL == Data)
		return (-1);
	
	Command[0] = 0x03;
	Command[1] = ((PageNo >> 8) & 0xFF);
	Command[2] = (PageNo & 0xFF);
	Command[3] = 0;
	
	CommandFrame.RecvBuf = Data;
	CommandFrame.RecvLen = 256;
	CommandFrame.RecvOff = 4;
	CommandFrame.SendBuf = Command;
	CommandFrame.SendLen = 4;
	
	__CS_On();
	
	if(CN_SPI_EXIT_NOERR == 
			SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER))
	{
		Ret = 256;
	}
	
	__CS_Off();
	
	return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 写一页
//参数: PageNo -- 页号;
//      Data -- 写缓冲;
//      Flags -- 保留;
//返回: >0 -- 写入字节数; -1 -- 参数错误; -2 -- 写入失败;
//备注:
//-----------------------------------------------------------------------------
s32 w25qxx_PageWrite(u32 PageNo, u8 *Data, u32 Flags)
{
	struct SPI_DataFrame CommandFrame;
	s32 Ret;
	u8 Command[4];
	
	if(NULL == Data)
		return (-1);
	
	if(__WriteEnable())
		return (-2);
	
	Command[0] = 0x02;
	Command[1] = ((PageNo >> 8) & 0xFF);
	Command[2] = (PageNo & 0xFF);
	Command[3] = 0;
	
	CommandFrame.RecvBuf = NULL;
	CommandFrame.RecvLen = 0;
	CommandFrame.RecvOff = 0;
	CommandFrame.SendBuf = Command;
	CommandFrame.SendLen = 4;
	
	__CS_On();
	Ret = SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER);
	if(CN_SPI_EXIT_NOERR == Ret)
	{
		CommandFrame.RecvBuf = NULL;
		CommandFrame.RecvLen = 0;
		CommandFrame.RecvOff = 0;
		CommandFrame.SendBuf = Data;
		CommandFrame.SendLen = 256;
		Ret = SPI_Transfer(s_pSpiPort, &CommandFrame, TRUE, CN_TIMEOUT_FOREVER);
		if(CN_SPI_EXIT_NOERR == Ret)
		{
			Ret = 256;
		}
		else
		{
			Ret = -2;
		}	
	}
	else
	{
		Ret = -2;
	}
	
	__CS_Off();
	
	Djy_EventDelay(3000);// 延时切出.

	if(__Done())
		Ret = -2;// 未完成,写失败
				
	return (Ret);
}

//-----------------------------------------------------------------------------
//功能: Nor设备初始化
//参数: Nor -- 设备结构体
//返回: -1 -- 失败; 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------

s32 Nor_Init(struct FlashChip *Nor)
{
    struct SPI_Device *SpiPort;
	
	// SPI接口
	SpiPort = (struct SPI_Device*)malloc(sizeof(*SpiPort));
	if(NULL == SpiPort)
		return (-1);

	SpiPort->AutoCs = FALSE;
	SpiPort->CharLen = 8;
	SpiPort->Cs = 0;
	SpiPort->Freq = 1000000; // 1M
	SpiPort->Mode = SPI_MODE_0;
	SpiPort->ShiftDir = SPI_SHIFT_MSB;
	
	if(NULL != SPI_DevAdd_s(SpiPort, "SPI1", "DevOnSPI1"))
		SPI_BusCtrl(SpiPort, CN_SPI_SET_POLL, 0, 0);
	
	s_pSpiPort = SpiPort;
	Nor->Descr.Nor.PortType = NOR_SPI;
	Nor->Descr.Nor.Port = (void*)s_pSpiPort;
	
	// 硬件配置
	GPIO_CfgPinFunc(GPIO_B, PIN14, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_50M,
					GPIO_PUPD_PU); // GPB14为CS用于NOR

	RCC->AHB1ENR |= 1<<6; // 使能PORTG时钟
	GPIO_CfgPinFunc(GPIOG,PIN7, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M,
					GPIO_PUPD_PU); // GPG7为CS用于无线模式
	GPIOG->ODR |= 1<<7;

	GPIO_SettoHigh(GPIO_B,PIN14);// CS为高
	
	Nor->Descr.Nor.BytesPerPage = 256;
	Nor->Descr.Nor.PagesPerSector = 16;
	Nor->Descr.Nor.SectorsPerBlk = 16;
	Nor->Descr.Nor.Blks = 256;
	
	Nor->Ops.RdPage = w25qxx_PageRead;
	Nor->Ops.WrPage = w25qxx_PageWrite;
	Nor->Ops.ErsBlk = w25qxx_BlkErase;
	Nor->Ops.Special.Nor.ErsChip = w25qxx_ChipErase;
	
	return (0);
}
//-----------------------------------------------------------------------------
//功能: Nor模块安装
//参数: DevName -- 设备名;
//      Flags -- 保留;
//      StartBlk -- 起始块;
//返回: 0 -- 成功; -1 -- 参数失败; -2 -- 内存不足;
//备注:
//-----------------------------------------------------------------------------

s32 ModuleInstall_NOR(const char *DevName, u32 Flags, u8 StartBlk)
{
	u32 Len;
	struct FlashChip *Chip;
	
	if(NULL == DevName)
		return (-1);

	Len = strlen(DevName) + 1;
	
	Chip = (struct FlashChip*)malloc(sizeof(*Chip) + Len);
	if(NULL == Chip)
	{
		TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
		return (-2);
	}
	memset(Chip, 0x0, sizeof(*Chip));
	s_pChip = Chip;
	strcpy(Chip->Name, DevName); // 设备别名
	Chip->Type = F_NOR;
	Chip->Descr.Nor.ReservedBlks = StartBlk; //
	
	if(Nor_Init(Chip))
	{
		free(Chip);
		return (-2);
	}
	
	Chip->Buf = (u8*)malloc(Chip->Descr.Nor.BytesPerPage);// NAND底层缓冲
    if(NULL == Chip->Buf)
    {
		TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
		free(Chip);
		return (-2);
    }
	
	Driver_DeviceCreate(NULL, Chip->Name, NULL, NULL, NULL, NULL, NULL, NULL, (ptu32_t)Chip);// 设备接入"/dev"
	
	return (0);

}
//-----------------------------------------------------------------------------
//功能: 驱动测试
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void LocalTest(void)
{
	u32 i, j, k;
	u16 temp;
	u8 Test[256];
	s32 Ret;


	ModuleInstall_NOR("nor", 0 ,0);

	temp = __ID();
#if 0
	for(i=0; i<s_pChip->Descr.Nor.BytesPerPage; i++)
		Test[i] = i;

	for(k=0; k<s_pChip->Descr.Nor.Blks; k++)
	{
		Ret = s_pChip->Ops.ErsBlk(k);
		if(Ret)
			while(1);

		for(i=0; i<(s_pChip->Descr.Nor.PagesPerSector*s_pChip->Descr.Nor.SectorsPerBlk); ++i)
		{
			Ret = s_pChip->Ops.RdPage((i+(k*(s_pChip->Descr.Nor.PagesPerSector*s_pChip->Descr.Nor.SectorsPerBlk))),
					                   s_pChip->Buf, 0);
			if(s_pChip->Descr.Nor.BytesPerPage != Ret)
				while(1);

			for(j=0; j<s_pChip->Descr.Nor.BytesPerPage; j++)
				if(s_pChip->Buf[j] != 0xFF)
					while(1);
		}


		for(i=0; i<(s_pChip->Descr.Nor.PagesPerSector*s_pChip->Descr.Nor.SectorsPerBlk); ++i)
		{
			Ret = s_pChip->Ops.WrPage((i+(k*(s_pChip->Descr.Nor.PagesPerSector*s_pChip->Descr.Nor.SectorsPerBlk))), Test, 0);
			if(s_pChip->Descr.Nor.BytesPerPage != Ret)
				while(1);
		}

		for(i=0; i<(s_pChip->Descr.Nor.PagesPerSector*s_pChip->Descr.Nor.SectorsPerBlk); ++i)
		{
			Ret = s_pChip->Ops.RdPage((i+(k*(s_pChip->Descr.Nor.PagesPerSector*s_pChip->Descr.Nor.SectorsPerBlk))),
					                  s_pChip->Buf, 0);
			if(s_pChip->Descr.Nor.BytesPerPage != Ret)
				while(1);

			for(j=0; j<s_pChip->Descr.Nor.BytesPerPage; j++)
				if(s_pChip->Buf[j] != j)
					while(1);
		}
	}
#endif
	if(EarseWholeChip(s_pChip))
		while(1);
	k = (s_pChip->Descr.Nor.PagesPerSector * s_pChip->Descr.Nor.SectorsPerBlk * s_pChip->Descr.Nor.Blks);
	for(i=0; i<k; i++)
	{
		Ret = s_pChip->Ops.RdPage(i, s_pChip->Buf, 0);
		if(s_pChip->Descr.Nor.BytesPerPage != Ret)
			while(1);

		for(j=0; j<s_pChip->Descr.Nor.BytesPerPage; j++)
			if(s_pChip->Buf[j] != 0xFF)
				while(1);
	}

	while(1);
}
