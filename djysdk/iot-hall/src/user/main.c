
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
// 这份授权条款，在使用者符合以下二条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、此三条件表列，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、此三条件表列，以及下述
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
/*
 * app_main.c
 *
 *  Created on: 2014-5-28
 *      Author: Administrator
 */
#include "os.h"
#include "stdint.h"
#include "systime.h"
#include "cpu_peri.h"
#include "lowpower.h"
#include "shell.h"
#include "IOT.h"

extern ptu32_t MainTestEntry(void);
extern void GDD_Startup(void);
extern void djyip_main(void);
void IOT_Startup(void);
extern u8  g_MonitorNum;
void BootRun(char *param)
{
	volatile u8 *pAddr;
	u8 i;
	pAddr=(u8 *)(0x2001fff0);
	for(i=0;i<16;i++)
	{
		*(pAddr+i)=(0xA0+i);
	}
	reset();
}
void AppRun(char *param)
{
	volatile u8 *pAddr;
	u8 i;
	pAddr=(u8 *)(0x2001fff0);
	for(i=0;i<16;i++)
	{
		*(pAddr+i)=0x00;
	}
	reset();
}
extern u16 g_UpdatePkgNum;
extern u8  g_MonitorNum;
extern u8  g_UpdateLastPkgSize;
void UpdateMonitor(char *param)
{
	g_UpdatePkgNum=800;
	g_MonitorNum=100;
	g_UpdateLastPkgSize=100;
	TransferToMonitorUpdate();
}
static bool_t __osversion(char *param)
{
	printf("DJYOSVERSION:DATE:%s TIME:%s\n\r",__DATE__,__TIME__);
	return true;
}
struct ShellCmdTab  shell_monitordebug_table[] =
{
    {
            "bootrun",
            BootRun,
            "复位cpu并跑Boot程序",
            NULL
    },
    {
        "apprun",
        AppRun,
        "复位cpu并跑App程序",
        NULL
    },
    {
    	"update",
		UpdateMonitor,
		"中继器升级监测端节点",
		NULL
    },
	{
		"version",
		__osversion,
		"usage:version",
		NULL
	}
};
#define cn_monitor_test_num  ((sizeof(shell_monitordebug_table))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc tg_monitorshell_cmd_rsc[cn_monitor_test_num];
void Shell_CmdAdd(void)
{
    Sh_InstallCmd(shell_monitordebug_table,tg_monitorshell_cmd_rsc,
            cn_monitor_test_num);
}

u32 QH_EntrySleepReCall(u32 SleepLevel)
{
    switch(SleepLevel)
    {
       case CN_SLEEP_L0:
           break;
       case CN_SLEEP_L1:
           break;
       case CN_SLEEP_L2:
           break;
       case CN_SLEEP_L3:
           break;
       case CN_SLEEP_L4:
    	   	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
			IWDG_SetPrescaler(IWDG_Prescaler_256); //40K/256 =6.4ms
			IWDG_SetReload(0x0FFF); //4096*6.4ms
			IWDG_ReloadCounter();
			IWDG_Enable();
    	   //进入低功耗前，关SPK,音频电源和烟雾报警电源
    	   IOT_VoicePowerOff();
    	   SX1278_EnterSleep();
//    	   IOT_SmogPowerOff();

    	   RTC_ClearFlag(	RTC_FLAG_TAMP1F|RTC_FLAG_TSOVF|RTC_FLAG_TSF
    	       			   |RTC_FLAG_WUTF|RTC_FLAG_ALRBF|RTC_FLAG_ALRAF|RTC_FLAG_RSF);

    	   PWR_ClearFlag(PWR_FLAG_WU);//清除Wake-up 标志
    	   PWR_WakeUpPinCmd(ENABLE);
           break;
       default:
           break;
    }
    return 0;
}
u32 QH_ExitSleepReCall(u32 SleepLevel)
{
    switch(SleepLevel)
    {
       case CN_SLEEP_L0:
           break;
       case CN_SLEEP_L1:
           break;
       case CN_SLEEP_L2:
           break;
       default:
           break;
    }
    return 0;
}
void Flash_Test(void)
{
	u8 buf[64];
	u32 i;
	u32 address;
	address = 0x080fff80;
	for(i = 0; i < 64; i ++ )
	{
		buf[i] = i;
	}
	Flash_Write(address,buf,64);
	memset(buf,0x00,64);
	Flash_Read(address,buf,64);
	Flash_Erase();
	Flash_Read(address,buf,64);
}
void Flash_Test2(void)
{
	u8 wbuf[64],rbuf[64];
	u32 i;
	u32 address;
	address = 0x08080000;
	for(i = 0; i < 64; i ++ )
	{
		wbuf[i] = i;
	}
	Flash_Erase();
	for(address = 0x08080000; address < 0x080fffff;)
	{
		Flash_Write(address,wbuf,64);
		memset(rbuf,0x00,64);
		Flash_Read(address,rbuf,64);
		if(0!= memcmp(wbuf,rbuf,64))
		{
			printf("error addr = %d \r\n",address);
			break;
		}
		address += 64;
	}
	if(address == 0x08100000)
	{
		printf("write flash ok!\r\n");
	}
}
void ECC_Test(void)
{
	u32 i;
	u8 ecc_test1[256];
	u8 ecc_test2[256];
	u8 ecc_test3[3];
	u32 ecc_result1=0;
	u32 ecc_result2=0;
	u32 ecc_result3=0;
	for(i=0;i<256;i++)
	{
		ecc_test1[i]=i;
	}
	for(i=0;i<10;i++)
	{
		ecc_test2[i]=i;
	}
	for(i=0;i<246;i++)
	{
		ecc_test2[10+i]=0x00;
	}
	for(i=0;i<3;i++)
	{
		ecc_test3[i]=i;
	}
	Ecc_Make256(ecc_test1,&ecc_result1);
	Ecc_Make256(ecc_test2,&ecc_result2);
	Ecc_Make256(ecc_test3,&ecc_result3);
    printf("ECC Test1 Result %x\r\n",ecc_result1);
    printf("ECC Test2 Result %x\r\n",ecc_result2);
    printf("ECC Test3 Result %x\r\n",ecc_result3);
}
ptu32_t djy_main(void)
{
	Shell_CmdAdd();
//	djyip_main();
#ifdef APP_PRO
	printf("Running APP...\r\n");
	IOT_Startup();
#endif
#ifdef OS_BOOT
	printf("Running OS_BOOT...\r\n");
	Flash_Erase();
	IOT_Update();
#endif
	while(1)
	{
		Djy_EventDelay(1000*mS);
	}
	return 0;
}

