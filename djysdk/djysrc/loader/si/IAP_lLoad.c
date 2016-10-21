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
//所属模块: Iboot
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 在应用编程的核心文件
//其他说明:
//修订历史:
//1. 日期: 2016-06-17
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include "stdint.h"
#include "stdio.h"
#include "driver.h"
#include "IAP.h"
#include "shell.h"
#include "djyos.h"
#include <cfg/Iboot_config.h>
#include "verify.h"
#include <stdlib.h>
#include "string.h"
#include "version.h"
#include "cpu_peri.h"
#include "cpu-optional.h"
#if(CN_CPU_OPTIONAL_CACHE==1)
#include "set-cache.h"
#endif
#define CN_IBOOT_FLAG               "RunIboot"
#define CN_APP_CTRL_APP_FLAG        0x61707000     //字符串"app"
#define CN_APP_CTRL_DBG_FLAG        0x00676264    //字符串"dbg"
#define CN_APP_CTRL_RELEASE_FLAG    0x72656173    //字符串"reas"
extern char g_cIbootFlag[];
extern struct IbootCtrl gc_ptIbootCtrl;
extern void reset(void);

extern const char *g_pcIbootDir;


void  Sh_RunIBoot(char *param);
void  Sh_RunAPP(char *param);
void Sh_RunReleaseAPP(char *param);
void Sh_GetAPPInfor(char *param);
void Sh_GetStatus(char *param);
void Sh_GetIAPVersion(char *param);
void Sh_GetRunMode(char *param);


extern u8 g_IbootStaus;
u32 g_bRunModeFlag;

extern struct AppInfo   gc_ptAppInfo;
extern u32 g_pAppCodeStartRamAddr;
extern u32 g_pAppCodeEndRamAddr;


extern void __AppPreLoad(void);


u32 IAP_GetAPPStartCodeRamAddr(void)
{
	return ((u32)&g_pAppCodeStartRamAddr);
}
u32 IAP_GetAPPCodeRamSize(void)
{
   return ((u32)&g_pAppCodeEndRamAddr - (u32)&g_pAppCodeStartRamAddr);
}
u32 IAP_GetAPPStartAddr(void)
{
	return (u32)(&gc_ptAppInfo.RomStartAddr);
}

bool_t IAP_APPIsDebug(void)
{
	u8 *dbgflg;

	dbgflg = (u8*)&gc_ptIbootCtrl.flag;
	if( (dbgflg[0] == 'd') && (dbgflg[1] == 'b') && (dbgflg[2] == 'g') )
	{
		return true;
	}
	return false;
}
u32 IAP_GetAPPSize(void)
{
   return (gc_ptAppInfo.AppSize);
}
u32 IAP_GetAPPCRC(void)
{
    return (gc_ptIbootCtrl.Iap_crc);
}

void  Sh_RunIBoot(char *param)
{
	 g_cIbootFlag[0]=0x52;   //R
	 g_cIbootFlag[1]=0x75;   //u
	 g_cIbootFlag[2]=0x6E;   //n
	 g_cIbootFlag[3]=0x49;   //i
	 g_cIbootFlag[4]=0x62;   //b
	 g_cIbootFlag[5]=0x6F;   //o
	 g_cIbootFlag[6]=0x6F;   //o
	 g_cIbootFlag[7]=0x74;   //t
	 g_cIbootFlag[8]=0x0;    // \0
#if(CN_CPU_OPTIONAL_CACHE==1)
	 Cache_CleanData();
	 Cache_InvalidInst();
#endif
	 reset();
}

void  Sh_RunAPP(char *param)
{
	g_cIbootFlag[0]=0x52;   //R
	g_cIbootFlag[1]=0x75;   //u
	g_cIbootFlag[2]=0x6E;   //n
	g_cIbootFlag[3]=0x41;   //a
	g_cIbootFlag[4]=0x50;   //p
	g_cIbootFlag[5]=0x50;   //p
	g_cIbootFlag[6]=0x00;   // \0
	g_cIbootFlag[7]=0x00;
#if(CN_CPU_OPTIONAL_CACHE==1)
	 Cache_CleanData();
	 Cache_InvalidInst();
#endif
	reset();
}
void Sh_RunReleaseAPP(char *param)
{
//	gc_ptIbootCtrl.flag=CN_APP_CTRL_RELEASE_FLAG;
//#if(CN_CPU_OPTIONAL_CACHE==1)
//	 Cache_CleanData();
//	 Cache_InvalidInst();
//#endif
//	reset();
}

void Sh_GetAPPInfor(char *param)
{
    u32 size,crc32;
    char *filename;
	size=gc_ptIbootCtrl.AppSize;
	crc32=gc_ptIbootCtrl.Iap_crc;
    filename=gc_ptIbootCtrl.filename;
    printf("size:0x%08x,crc32:0x%08x,filename:%s.\r\n",size,crc32,filename);
}
//-----------------------------------------------------------------
void Sh_GetStatus(char *param)
{
	switch (g_IbootStaus)
	{
		case 0:
			printf("IAP status:No Err.\r\n");
			break;
		case 1:
			printf("IAP status:Force Run Iboot.\r\n");
			break;
		case 2:
			printf("IAP status:RAM Iboot flag.\r\n");
			break;
		case 3:
			printf("IAP status:Load from data mode.\r\n");
			break;
		case 4:
			printf("IAP status:Ecc Err.\r\n");
			break;
		case 5:
			printf("IAP status:APP Flag Err.\r\n");
			break;
		case 6:
			printf("IAP Err:APP File Not Exist Err.\r\n");
			break;
		case 7:
			printf("IAP Err:APP File Size Invalid.\r\n");
			break;
		case 8:
			printf("IAP Err:Iboot/App Ids MisMatch..\r\n");
			break;
		default:
			break;
	}
}

#define CN_VERSION_CHAR_LEN   100
void Sh_GetIAPVersion(char *param)
{
   char *str;
   str=malloc(CN_VERSION_CHAR_LEN);
   if(str!=NULL)
   {
	   str=djyos_kernel_version;
	   printf("Version:%s.\r\n",str);
   }
}
void Sh_GetRunMode(char *param)
{
	if(g_bRunModeFlag==0x12345678)
   {
		   printf("Run Mode:Iboot.\r\n");
   }
	else
		printf("Run Mode:APP.\r\n");
}


struct ShellCmdTab const shell_cmd_iap_table[]=
    {
		{
			"runiboot",
			Sh_RunIBoot,
        "切换到Iboot(仅在采取内存标示确定加载项目且APP有加载DJYOS shell模块时有效)",
        "COMMAND:runiboot+enter"
		},

		{
           "runapp",
		   Sh_RunAPP,
       "直接运行APP(仅在采取内存标示确定加载项目时有效)",
		   "COMMAND:runapp+enter"
    },
    {
    	"runreapp",
		Sh_RunReleaseAPP,
		"运行ReleaseAPP",
		"COMMAND:runreapp+enter"
    },
	{
    	"appinfo",
		Sh_GetAPPInfor,
		"获取升级bin文件信息",
		"COMMAND:appinfor+enter"
    },
	{
		"iapstatus",
		Sh_GetStatus,
		"获取Iboot状态信息",
		"COMMAND:iaperr+enter"
	},
	{
		 "iapver",
			  Sh_GetIAPVersion,
			  "获取IAP版本信息",
			  "COMMAND:iapversion+enter"
		},
		{
      "iapmode",
	  Sh_GetRunMode,
	  "获取当前运行模式(Iboot or APP)",
	  "COMMAND:iapsta+enter"
	}
};
#define CN_IAP_SHELL_NUM  sizeof(shell_cmd_iap_table)/sizeof(struct ShellCmdTab)
static struct ShellCmdRsc tg_iap_shell_cmd_rsc[CN_IAP_SHELL_NUM];
ptu32_t IAP_Shell_Module_Install(void)
{
	Sh_InstallCmd(shell_cmd_iap_table,tg_iap_shell_cmd_rsc,CN_IAP_SHELL_NUM);
	return 1;
}

//----安装IAP组件-----------------------------------------------------------
//功能：安装IAP组件。
//参数: 无。
//返回: 无。
//----------------------------------------------------------------------------
ptu32_t ModuleInstall_IAP(void)
{
	IAP_Shell_Module_Install();
	printf("IAP ModuleInstall success.\r\n");
	return 0;
}


//----判断是否需要从文件系统中加载APP并执行---------------------------------------
//功能：从文件中加载APP并运行。
//参数: 无。
//返回: 无。
//说明:当IAP加载模式为LoadFromData时，只有满足以下三个条件：1.运行APP；2.IAP加载模
//      式为LoadFromData；3.APP文件存在.才会从文件中加载到RAM中运行。
//----------------------------------------------------------------------------
bool_t IAP_LoadAPPFromFile(void)
{
	bool_t result;
	char *Buf;
	u32 crc,len,addr;
	FILE *fp;
    struct stat FpInfo;

	//首先判断是否需要进入APP状态
	result=IAP_IsForceIboot();
	if(result)
	{
		g_IbootStaus=EN_FORCE_IBOOT;
		result = false;  //运行Iboot
	}
	else
	{
		result=IAP_IsRamIbootFlag();
		if(result)
		{
			g_IbootStaus=EN_RAM_IBOOT_FLAG;
			result = false;   //运行Iboot
		}
		else
		{
			if(g_IbootType==0)
			{
			   fp=fopen(g_pcIbootDir,"r");
			   if(fp==NULL)
			   {
				   result = false;
				   goto LOAD_FILE_EXIT;
			   }

			   addr=IAP_GetAPPStartAddr() - 0x100;
			   Buf=(char *)addr;

			   fread(Buf,0x100,1,fp);			//read the 256 byte file info

			   fseek(fp,0,0);

			   if(IAP_APPIsDebug())				//dbg not need crc
			   {
					if(0 == stat(g_pcIbootDir,&FpInfo))
					{
						len = (u32)FpInfo.st_size;
					}
					else
					{
						fclose(fp);
						result = false;
						goto LOAD_FILE_EXIT;
					}
					fread(Buf,len,1,fp);
			   }
			   else
			   {
				   len=IAP_GetAPPSize();
				   fread(Buf,len,1,fp);
				   crc=crc32(Buf,len);
				   if(crc!=gc_ptIbootCtrl.Iap_crc)
				   {
					   fclose(fp);
					   result = false;
					   goto LOAD_FILE_EXIT;
				   }
			   }
				__AppPreLoad();
				return true;		//clear compile warning
			}
		}

	}
LOAD_FILE_EXIT:
	return result;
}





