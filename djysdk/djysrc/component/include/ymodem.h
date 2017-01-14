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


#ifndef __YMODEM_H__
#define __YMODEM_H__
#include <stdint.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

// YMODEM命令
#define CN_YMODEM_SOH     1         //128字节起始符
#define CN_YMODEM_STX     2         //1028字节起始符
#define CN_YMODEM_EOT     4
#define CN_YMODEM_ACK     6
#define CN_YMODEM_NAK     0x15
#define CN_YMODEM_C       0x43          //大写字母C
#define CN_YMODEM_CAN     0x18

// YMODEM数据包长度
#define CN_YMODEM_SOH_SIZE  128        //SOH数据包大小为128字节
#define CN_YMODEM_STX_SIZE  1024       //STX数据包大小为1024

typedef enum
{
	 YMODEM_OK= 0,				//正常返回
	 YMODEM_PARAM_ERR,			//参数错误
	 YMODEM_FILE_ERR,			//文件操作错误
	 YMODEM_TIMEOUT,			//超时返回
	 YMODEM_MEM_ERR,			//内存不够
	 YMODEM_CAN_TRANS,			//取消传输
	 YMODEM_MEDIA_ERR,			//硬件错误
	 YMODEM_UNKNOW_ERR
}YMRESULT;

typedef enum
{
	ENUM_YMODEM_STA_INFO = 0x80,		//YMODEM信息包
	ENUM_YMODEM_STA_SOH,				//128字节的数据包
	ENUM_YMODEM_STA_STX,				//1024字节的数据包
	ENUM_YMODEM_STA_EOT,				//结束符
	ENUM_YMODEM_STA_ZERO,				//全ZERO包
}YMSTATUS;

typedef enum
{
	YMODEM_FILE_NOOPS= 0xF0,			//无文件操作
	YMODEM_FILE_OPEN ,					//打开文件标记
	YMODEM_FILE_WRITE,					//写文件标记
	YMODEM_FILE_READ,					//读文件标记
	YMODEM_FILE_STAT,
	YMODEM_FILE_CLOSE,					//关闭文件标记
}YMFILEOPS;


typedef struct __Ymodem
{
	FILE 	*File;						//file ptr for file op
	const char    *Path;				//文件所在的路径（未含文件名）
	char 	*FileName;				    //文件名称
	struct MutexLCB *pYmodemMutex;		//互斥访问
	YMFILEOPS FileOps;					//文件操作
	u32 	FileSize;					//当前传输文件大小
	u32 	FileCnt;					//写入文件大小计数
	u32 	PkgNo;						//包号
	u32 	PkgSize;					//当前包长度
	s64 	StartTime;					//start time
	u32 	TimeOut;					//time out
	YMSTATUS Status;					//ymodem status
	u8 		*FileBuf;					//file buf
	u32 	FileBufCnt;					//文件缓冲区大小计数
	u8 		*PkgBuf;
	u32 	PkgBufCnt;					//PKG大小计数
}tagYmodem;

ptu32_t ModuleInstall_Ymodem(ptu32_t Param);
bool_t Ymodem_UploadFile(char *Param);
bool_t Ymodem_DownloadFile(char *Param);
bool_t Ymodem_PathSet(const char *Path);

#ifdef __cplusplus
}
#endif

#endif //__YMODEM_H__

