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


#include <stdio.h>
#include <string.h>
#include <djyos.h>

#define RW_SIZE						512
#define APPLICATION_FILE_NAME		"sr5333.bin"

extern u8 IAP_USB_Ready;
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static u32 USB_IAP_Threat(void)
{
	DIR *Dir;
	struct dirent *DirentInfo;
	char *Path;
	char Buf[RW_SIZE];
	s32 Res;
	u8 Found = 0;
	s32 HandleSrc = -1;
	s32 HandleDes = -1;
	char *Program = APPLICATION_FILE_NAME;

	do{
		if(IAP_USB_Ready)
			break;
	}while(1);


	Dir = opendir("/fat");
	if(!Dir)
	{
		printf("IAP: USB: directory \"fat\" is not exist! FAT is not installed!\r\n");
		return (-1);
	}

	while((DirentInfo = readdir(Dir)) != NULL)
	{
		if(0 == strcmp(DirentInfo->d_name, Program))
		{
			printf("IAP: USB: file \"%s\" will be programmed.\r\n",
					DirentInfo->d_name);
			Found = 1;
			break;
		}
	}

	if(!Found)
	{
		printf("IAP: USB: file \"%s\" is not found.\r\n", Program);
		goto END;
	}

	Path = Buf;
	strcpy(Path, "/fat/");
	strcpy(Path+5, DirentInfo->d_name);
	HandleSrc = open(Path, O_RDONLY);
	if(-1 == HandleSrc)
	{
		printf("IAP: USB: cannot open source file \"%s\".\r\n", Path);
		goto END;
	}

	strcpy(Path, "/iboot/");
	strcpy(Path+7, DirentInfo->d_name);
	HandleDes = open(Path, O_RDWR);
	if(-1 == HandleDes)
	{
		printf("IAP: USB: cannot open destination file \"%s\"!\r\n", Path);
		goto END;
	}

	while(1)
	{
		Res = read(HandleSrc, Buf, RW_SIZE);
		if(!Res)
			break; // 全部读完

		if(Res != write(HandleDes, Buf, Res))
		{
			printf("IAP: USB: write destination file error.\r\n");
			break;
		}

		if(Res != RW_SIZE)
			break; // 全部读完
	}

END:
	closedir(Dir);
	printf("IAP: USB: program end.\r\n");
	if(-1 != HandleSrc)
	{
		Res = close(HandleSrc);
		if(Res)
		{
			printf("IAP: USB: close source file failed.\r\n");
		}
	}

	if(-1 != HandleDes)
	{
		Res = close(HandleDes);
		if(Res)
		{
			printf("IAP: USB: close destination file failed.\r\n");
			return (-1);
		}
	}

	return (0);
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: "0" -- 成功; "-1" -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 ModuleInstall_USB_IAP(void)
{
	u16 USB_IAP_ThreatID;

	USB_IAP_ThreatID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 0,
					USB_IAP_Threat, NULL, 0x1000, "usb iap service");
	//事件的两个参数暂设为0,如果用shell启动,可用来采集shell命令行参数
	if(USB_IAP_ThreatID != CN_EVTT_ID_INVALID)
	{
		Djy_EventPop(USB_IAP_ThreatID, NULL, 0, 0, 0, 0);
		return (0);
	}

	return (-1);
}
