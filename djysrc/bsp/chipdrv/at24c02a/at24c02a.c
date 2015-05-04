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
// 文件名     ：at24c02a.c
// 模块描述: EEPROM芯片驱动，调用了IICBUS总线驱动模型
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 14/01.2015
// =============================================================================

#include "stdint.h"
#include "os.h"
#include "iicbus.h"
#include "cpu_peri.h"

// =============================================================================
#define CN_AT24_CHIP_SIZE       (256)			//芯片大小256 Bytes
#define CN_AT24_PAGE_SIZE       (8)				//芯片页大小8 Bytes
#define CN_AT24_PAGE_SUM    	(CN_AT24_CHIP_SIZE/CN_AT24_PAGE_SIZE)

#define AT24C_ADDRESS			0x50			//设备地址
#define AT24C_CLK_FRE			(100*1000)		//总线速度，单位Hz


//定义IICBUS架构下的IIC设备结构
static struct tagIIC_Device *ps_AT24_Dev = NULL;
static u32 s_AT24_Timeout = CN_TIMEOUT_FOREVER;

// =============================================================================
// 功能：AT24芯片WP写保护引脚初始化，配置为低时，允许写，为高时，写保护
// 参数：无
// 返回：无
// =============================================================================
void __AT24_GpioInit(void)
{
	//AT24C的WP引脚配置为低，允许写
}

// =============================================================================
// 功能：AT24芯片按页写,地址范围只局限在该页内
// 参数：wAddr:操作芯片的片内地址
//       pbyBuf:缓冲区
//       wBytesNum:数据字节数
// 返回：true,成功;false,失败
// =============================================================================
u16 __AT24_PageProgram(u16 wAddr, u8 *pbyBuf, u16 wBytesNum)
{
	if(((wAddr%CN_AT24_PAGE_SIZE) + wBytesNum) > CN_AT24_PAGE_SIZE)
		return 0;

	IIC_Write(ps_AT24_Dev,wAddr,pbyBuf,wBytesNum,true,s_AT24_Timeout);

	//AT24内部写时间，最大为10mS
	Djy_EventDelay(10*mS);

	return wBytesNum;
}

// =============================================================================
// 功能：AT24芯片按页读,地址范围只局限在该页内
// 参数：wAddr:操作芯片的片内地址
//       pbyBuf:缓冲区
//       wBytesNum:数据字节数
// 返回：true,成功;false,失败
// =============================================================================
u32 __AT24_PageRead(u16 wAddr, u8 *pbyBuf, u16 wBytesNum)
{
	if(((wAddr%CN_AT24_PAGE_SIZE) + wBytesNum) > CN_AT24_PAGE_SIZE)
		return 0;

	IIC_Read(ps_AT24_Dev,wAddr,pbyBuf,wBytesNum,s_AT24_Timeout);

	//AT24内部写时间，最大为10mS
	Djy_EventDelay(10*mS);

	return wBytesNum;
}

// =============================================================================
// 功能：AT24芯片写操作，EEPROM按页操作，每次读写不能超过页大小
// 参数：wAddr:操作芯片的片内地址
//       pbyBuf:缓冲区
//       wBytesNum:数据字节数
// 返回：true,成功;false,失败
// =============================================================================
u32 AT24_WriteBytes(u16 wAddr, u8 *pbyBuf, u16 wBytesNum)
{
	u32 offset = 0,wSize = 0;

	//判断是否超出芯片地址范围
	if((wAddr + wBytesNum) >= CN_AT24_CHIP_SIZE)
		return false;

	//分页操作
	while(wBytesNum)
	{
		offset = wAddr % CN_AT24_PAGE_SIZE;				//本页偏移字节数
		wSize = (wBytesNum > CN_AT24_PAGE_SIZE - offset)?
				(CN_AT24_PAGE_SIZE - offset): wBytesNum;//本页写字节数
		__AT24_PageProgram(wAddr,pbyBuf,wSize);			//写本页
		wAddr += wSize;									//写地址增加
		pbyBuf += wSize;								//缓冲区增加
		wBytesNum -= wSize;								//写长度递减
	}

	return true;
}

// =============================================================================
// 功能：AT24芯片读操作，EEPROM按页操作，每次读写不能超过页大小
// 参数：wAddr:操作芯片的片内地址
//       pbyBuf:缓冲区
//       wBytesNum:数据字节数
// 返回：true,成功;false,失败
// =============================================================================
u16 AT24_ReadBytes(u16 wAddr, u8 *pbyBuf, u16 wBytesNum)
{
	u16 offset = 0,rSize = 0;

	//判断是否超出芯片地址范围
	if((wAddr + wBytesNum) >= CN_AT24_CHIP_SIZE)
		return false;

	//分页操作
	while(wBytesNum)
	{
		offset = wAddr % CN_AT24_PAGE_SIZE;				//本页偏移字节数
		rSize = (wBytesNum > CN_AT24_PAGE_SIZE - offset)?
				(CN_AT24_PAGE_SIZE - offset): wBytesNum;//本页读字节数
		__AT24_PageRead(wAddr,pbyBuf,rSize);			//读本页
		wAddr += rSize;									//读地址增加
		pbyBuf += rSize;								//缓冲区增加
		wBytesNum -= rSize;								//读长度递减
	}

	return true;
}

// =============================================================================
// 功能：写字到指定地址
// 参数：wAddr,地址
// 		 wValue,数值，16比特数值
// 返回：true,成功;false,失败
// =============================================================================
u16 AT24_WriteWord(u16 wAddr,u16 wValue)
{
	u8 pbyBuf[2];
	pbyBuf[0]=wValue;
	pbyBuf[1]=wValue>>8;
	if(true == AT24_WriteBytes(wAddr,pbyBuf,2))
		return true;
	else
		return false;
}

// =============================================================================
// 功能：读字
// 参数：wAddr,地址
// 返回：wValue,成功;-1,失败
// =============================================================================
s16 AT24_ReadWord(u16 wAddr)
{
	u8 pbyBuf[2];
	u16 wValue;

	if(true == AT24_ReadBytes(wAddr, pbyBuf,2))
	{
		wValue=pbyBuf[0]+(pbyBuf[1]<<8);
		return wValue;
	}
	else
		return -1;
}

// =============================================================================
// 功能：AT24芯片初始化，包括WP写保护功能引脚初始化和加载设备到对应的IIC总线
// 参数：无
// 返回：true,成功;false,失败
// =============================================================================
bool_t AT24_ModuleInit(void)
{
	bool_t result = false;
	static struct tagIIC_Device s_AT24_Dev;
	//GPIO初始化，SDA、SCL已经在IIC中初始化了，此处只需初始化WP即可
	__AT24_GpioInit();

	//初始化IIC设备结构体
	s_AT24_Dev.DevAddr 					= AT24C_ADDRESS;
	s_AT24_Dev.BitOfMemAddr 			= 8;
	s_AT24_Dev.BitOfMemAddrInDevAddr 	= 0;

	//添加AT24到IIC0总线
	if(NULL != IIC_DevAdd_r("IIC0","IIC_Dev_AT24",&s_AT24_Dev))
	{
		ps_AT24_Dev = &s_AT24_Dev;
		IIC_BusCtrl(ps_AT24_Dev,CN_IIC_SET_CLK,AT24C_CLK_FRE,0);
		IIC_BusCtrl(ps_AT24_Dev,CN_IIC_DMA_USED,0,0);
		result = true;
	}

	return result;
}


