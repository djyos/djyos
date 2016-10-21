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
#include "os.h"
#include "cpu_peri.h"
#include "string.h"
#include "driver.h"
#include "uartctrl.h"
#include "ymodem.h"
#include "flashdrv.h"
#include "driver/include/uart.h"
#include "driver.h"
#include "IAP.h"
#include <cfg/iboot_config.h>
static struct DjyDevice * YmodemDev;
extern struct IbootCtrl gc_ptIbootCtrl;

extern bool_t Ymodem_PathSet(const char *Path);
bool_t Ymodem_UpdateProgram(void)
{
	u8 cmd[32];
	u32 crc;
    extern bool_t Sh_DownloadFile(char *param);
	YmodemDev = Driver_OpenDevice("USART0",D_RDONLY,CN_TIMEOUT_FOREVER);
	if(NULL == YmodemDev)
	{
	    return false;
	}
	Ymodem_PathSet("/iboot");
	Driver_CtrlDevice(YmodemDev,CN_UART_SET_BAUD,115200,0);
	Driver_CtrlDevice(YmodemDev,CN_UART_START,0,0);
	//read the cmd
	while(1)
	{
		Driver_ReadDevice(YmodemDev,cmd,2,0,CN_TIMEOUT_FOREVER);
		if(0 ==memcmp("DL",cmd,2))
		{
			if(true == Ymodem_DownloadFile(NULL))
			{
				//作CRC校验
				crc=gc_ptIbootCtrl.Iap_crc;
			}
			else
			{
				continue;
			}
		}
		else if(0==memcmp("CK",cmd,2))
		{
           u8 buf[7];
           buf[0]=0x43;
           buf[1]=0x4B;
           buf[2]=crc;
           buf[3]=crc>>8;
           buf[4]=crc>>16;
           buf[5]=crc>>24;
           buf[6]=0x43;
           Driver_WriteDevice(YmodemDev,buf,7,0,0,CN_TIMEOUT_FOREVER);
           printf("IAP Ymodem tarnsfer completed.\r\n");
           break;
		}
	}

	return true;
}


static const Pin BOOT_MCU_LED[] = {
		{PIO_PA23, PIOA, ID_PIOA, PIO_OUTPUT_0,    PIO_DEFAULT}
};

void LED_On(void)
{
	PIO_Clear(BOOT_MCU_LED);
}

void LED_Off(void)
{
	PIO_Set(BOOT_MCU_LED);
}

void LED_Init(void)
{
	PIO_Configure(BOOT_MCU_LED, PIO_LISTSIZE(BOOT_MCU_LED));
}



//系统升级用
ptu32_t djy_main(void)
{
	bool_t result;
	while(1)
	{
	   LED_On();
	   Ymodem_UpdateProgram();
    }

	return 0;
}

