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
//所属模块:ymodem模块
//作者:  贺敏.
//版本：V2.0.0
//文件描述:ymodem模块，利用ymodem协议上传或下载文件
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-10-06
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "os.h"
#include "shell.h"
#include "driver.h"
#include "verify.h"

#include "ymodem.h"
#include <cfg/ymodemcfg.h>

//#define CN_MAX_PACKNUM    32            //定义最大包个数，超过该数，则需要写入文件
//#define CN_YMODEM_FILEBUF_SIZE   ((CN_MAX_PACKNUM + 1)*1024) //定义缓冲区大小，由函数动态分配

#define CN_YMODEM_PKGBUF_SIZE         	(1029)
#define CN_YMODEM_NAME_LENGTH  			(256)

struct ShellCmdTab const ymodem_cmd_table[] =
{
    {
        "download",
		Ymodem_DownloadFile,
        "下载文件",
        "命令格式: download"
    },
    {
        "upload",
		Ymodem_UploadFile,
        "上传文件",
        "命令格式: upload 文件名"
    }
};

static struct ShellCmdRsc tg_ymodem_cmd_rsc
                        [sizeof(ymodem_cmd_table)/sizeof(struct ShellCmdTab)];
static struct DjyDevice *s_ptYmodemDevice;

static tagYmodem *pYmodem = NULL;
//----ymodem模型初始化---------------------------------------
//功能: ymodem下载文件模块接口函数，在module_trim中调用
//参数: 无
//返回: 无
//-----------------------------------------------------------
ptu32_t ModuleInstall_Ymodem(struct DjyDevice *para)
{
    if(para == NULL)
        s_ptYmodemDevice = ToDev(stdin);
    else
        s_ptYmodemDevice = para;

    pYmodem = (tagYmodem *)malloc(sizeof(tagYmodem));
    if(NULL != pYmodem)
    {
    	pYmodem->FileName = (char *)malloc(CN_YMODEM_NAME_LENGTH);
    	if(NULL == pYmodem->FileName)
    	{
    		free(pYmodem);
    		return false;
    	}
    	pYmodem->pYmodemMutex = Lock_MutexCreate("YMODEM_MUTEX");
    	if(NULL != pYmodem->pYmodemMutex)
    	{
    		pYmodem->Path = NULL;
			if(Sh_InstallCmd(ymodem_cmd_table,tg_ymodem_cmd_rsc,
					sizeof(ymodem_cmd_table)/sizeof(struct ShellCmdTab)))
				return true;
    	}
    	else
    	{
    		free(pYmodem->FileName);
    		free(pYmodem);
    	}
    }
    return false;
}

// ============================================================================
// 功能：配置ymodem下载文件的绝对路径，该函数选择性调用，
//      1.若应用需要变更下载路径，则不调用该函数，通过其他方式（如shell）修改路径;
//      2.调用该函数后，通过Ymodem下载的路径则不可变换;
//      3.举例说明：如利用iboot升级，下载路径不需变，因此Path = "/iboot"
// 参数：Path，通过Ymodem存放的绝对路径
// 返回：true,成功，否则，失败
// ============================================================================
bool_t Ymodem_PathSet(const char *Path)
{
	bool_t Ret = false;

	if( (pYmodem != NULL) && (NULL != Path) )
	{
		Lock_MutexPend(pYmodem->pYmodemMutex,CN_TIMEOUT_FOREVER);
		pYmodem->Path = Path;

		Lock_MutexPost(pYmodem->pYmodemMutex);
		Ret = true;
	}
	return Ret;
}

//----获取下载数据-------------------------------------------
//函数：__Ymodem_Gets，获取多字节数据
//     __Ymodem_Get，获取一单字数据
//     __Ymodem_Puts，传输多字节数据
//     __Ymodem_Put，传输一字节数据
//功能：YMODEM数据接口通道，通过这两对通道获取和传送数据
//参数：
//返回：
//-----------------------------------------------------------
static u32 __Ymodem_Gets(u8 *buf,u32 len, u32 timeout)
{
   return Driver_ReadDevice(s_ptYmodemDevice,buf,len,0,timeout);
}
static u32 __Ymodem_Get(u8 *buf)
{
   return __Ymodem_Gets(buf,1,gYmodemPkgTimeOut);
}

static u32 __Ymodem_Puts(u8 *buf,u32 len,u32 timeout)
{
    return Driver_WriteDevice(s_ptYmodemDevice,buf,len,0,
                                CN_BLOCK_BUFFER,timeout);
}
static u32 __Ymodem_Put(u8 Char)
{
    u8 ch = Char;
    return __Ymodem_Puts(&ch,1,500*mS);
}

//----提取单词----------------------------------------------
//功能: 从buf中提取一个由空格或行结束符隔开的单词，next用于
//      返回下一个单词首地址，如果没有下一个单词，则next=NULL。
//参数: buf，待分析的字符串
//      next，返回下一个单词指针
//返回: 提取的单词指针，已将单词后面的分隔符换成串结束符'\0'
//-----------------------------------------------------------
//todo:shell.c中Sh_GetWord函数与此函数雷同。
static char *__Ymodem_GetWord(char *buf,char **next)
{
    uint32_t i=0;
    *next = NULL;
    if(buf == NULL)
        return NULL;
    while(1)
    {
        if((buf[i] == ' ') || (buf[i] == 0))
        {
            buf[i] = '\0';
            break;
        }
        i++;
    }
    i++;
    while(buf[i] != 0)
    {
        if(buf[i]!=' ')
        {
            if((buf[i] == '\n') || (buf[i] == '\r'))
                *next = NULL;
            else
                *next = &buf[i];
            break;
        }
        i++;
    }
    return buf;
}
//------ymodem取消传输---------------------------------------
//功能：ymodem模块通知对端取消本次传输
//参数：无
//返回：无
//-----------------------------------------------------------
static void __Ymodem_CancelTrans(void)
{
    __Ymodem_Put(CN_YMODEM_CAN);
    __Ymodem_Put(CN_YMODEM_CAN);
    __Ymodem_Put(CN_YMODEM_CAN);
}

//------校验ymodem数据包-------------------------------------
//功能：ymodem模块校验接收到的数据包
//参数：buf：数据包缓冲
//      pack_len: ymodem数据包长度
//返回：true:  数据包校验正确
//      false: 数据包校验错误
//----------------------------------------------------------
static bool_t __Ymodem_PackCheck(u8* buf, u32 pack_len)
{
    u16 checksum,check;
    if((buf[1] + buf[2]) != 0xff)               //校验包号正反码
    {
        __Ymodem_Put(CN_YMODEM_NAK);                    //应答nak，请求重发
        return false;
    }
    checksum = crc16(buf+3, pack_len);
    if(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)
    {
        check = (buf[pack_len+3]<<8) + buf[pack_len+4];
    }
    else
    {
        check = *(u16 *)&buf[pack_len+3];
    }
    if(checksum != check)    //CRC校验错误
    {
        __Ymodem_Put(CN_YMODEM_NAK);                    //应答nak，请求重发
        return false;
    }

    return true;
}

static void __Ymodem_WriteCrc16(u8 *package, u32 pack_len)
{
	u16 checksum;

	checksum = crc16(package+3, pack_len);
	if(CN_CFG_BYTE_ORDER == CN_CFG_LITTLE_ENDIAN)
	{
		package[3+pack_len] = (checksum >> 8) & 0xFF;
		package[3+pack_len + 1] = (checksum) & 0xFF;

	}
	else
	{
		package[3+pack_len] = (checksum) & 0xFF;
		package[3+pack_len + 1] = (checksum >> 8) & 0xFF;
	}
}

// 获取完整的YMODEM数据包，包括数据包和ACK、CAN等包
static YMRESULT __Ymodem_GetPkg(tagYmodem *ym)
{
	YMRESULT Ret = YMODEM_OK;
	u32 bytes;

	if(ym->PkgBufCnt == 0)
	{
		if(1 != __Ymodem_Get(ym->PkgBuf) )
			Ret = YMODEM_TIMEOUT;
	}

	switch(ym->PkgBuf[0])
	{
	case CN_YMODEM_SOH:
		ym->PkgSize = CN_YMODEM_SOH_SIZE;
		break;
	case CN_YMODEM_STX:
		ym->PkgSize = CN_YMODEM_STX_SIZE;
		break;
	case CN_YMODEM_ACK:
	case CN_YMODEM_NAK:
	case CN_YMODEM_C:
	case CN_YMODEM_CAN:
	case CN_YMODEM_EOT:
		ym->PkgSize = 1;
		break;
	default:
		ym->PkgSize = 1;
		Ret = YMODEM_UNKNOW_ERR;
		break;
	}

	if(ym->PkgSize > 1)
	{
		bytes = __Ymodem_Gets(ym->PkgBuf + 1,ym->PkgSize + 4,gYmodemPkgTimeOut);
		if(bytes != ym->PkgSize + 4)
			Ret = YMODEM_TIMEOUT;
	}

	return Ret;
}

static bool_t __Ymodem_FilePathMerge(char *dst,const char *path,char *name)
{
	u32 PathLen,NameLen;

	if(NULL != name)
	{
		PathLen = strlen(path);
		NameLen = strlen(name);
		if( NULL != path )
		{
			if(PathLen + NameLen + 1 < CN_YMODEM_NAME_LENGTH)
			{
				strcpy(dst,path);
				dst[PathLen] = '/';
				strcpy(dst + PathLen + 1,name);
				dst[PathLen + NameLen + 1] = '\0';
				return true;
			}
		}
		else
		{
			strcpy(dst,name);
			dst[NameLen] = '\0';
			return true;
		}
	}
	return false;
}
static bool_t __Ymodem_InfoPkg(tagYmodem *ym)
{
	char *NextParam,*strFileSize,*FileName;

	FileName = __Ymodem_GetWord((char*)&ym->PkgBuf[3],&NextParam);
	strFileSize = __Ymodem_GetWord(NextParam,&NextParam);

	if(__Ymodem_FilePathMerge(ym->FileName,ym->Path,FileName))
	{
		ym->FileSize = strtol(strFileSize, (char **)NULL, 0);
		ym->FileOps = YMODEM_FILE_OPEN;
		ym->Status = CN_YMODEM_SOH;
		ym->PkgNo = 1;
		return true;
	}
	return false;
}

static bool_t __Ymodem_IsZeroPkg(tagYmodem *ym)
{
	u8 i;

	if( (ym->PkgBuf[1] == 0x00) && (ym->PkgBuf[2] == 0xFF) )
	{
		for(i = 0; i < 128; i++)
		{
			if(ym->PkgBuf[3+i] != 0x00)
			{
				return false;
			}
		}
	}

	return true;
}


static YMRESULT __Ymodem_FileOps(tagYmodem *ym, u8 flag)
{
	YMRESULT Ret = YMODEM_OK;
    struct stat FpInfo;
	u32 FileOpsLen;

	//对文件进行操作
	switch(ym->FileOps)
	{
	case YMODEM_FILE_OPEN:
		if(flag)
		{
			ym->File = fopen(ym->FileName,"w+");		//打开文件，不存在则创建
			__Ymodem_Put(CN_YMODEM_ACK);
			__Ymodem_Put(CN_YMODEM_C);
		}
		else
		{
			ym->File = fopen(ym->FileName,"r");		//打开文件，不存在则创建
		}
		if(ym->File == NULL)
		{
			Ret = YMODEM_FILE_ERR;
		}
		break;
	case YMODEM_FILE_WRITE:
        memcpy(ym->FileBuf + ym->FileBufCnt,ym->PkgBuf + 3,ym->PkgSize);
        ym->FileBufCnt += ym->PkgSize;
        if((ym->FileBufCnt >= ym->FileSize - ym->FileCnt) ||
        		(ym->FileBufCnt >= gYmodemBufPkgNum*1024)) //如果足够大，写入flash
        {
            if(ym->FileBufCnt >= ym->FileSize - ym->FileCnt)                                   //判断本次写入大小
            	FileOpsLen = ym->FileSize - ym->FileCnt;
            else
            	FileOpsLen = ym->FileBufCnt;
            ym->FileBufCnt = 0;
            if(FileOpsLen != fwrite(ym->FileBuf,FileOpsLen,1,ym->File))
            {
            	Ret = YMODEM_FILE_ERR;
            }
            ym->FileCnt += FileOpsLen;
        }
        __Ymodem_Put(CN_YMODEM_ACK);
		break;
	case YMODEM_FILE_READ:
		memset(ym->PkgBuf,0x00,CN_YMODEM_STX_SIZE);
		FileOpsLen = (ym->FileSize - ym->FileCnt > CN_YMODEM_STX_SIZE) ?
				(CN_YMODEM_STX_SIZE) : ym->FileSize - ym->FileCnt;
		ym->PkgSize = CN_YMODEM_STX_SIZE;
		ym->PkgBuf[0] = CN_YMODEM_STX;
		ym->PkgBuf[1] = ym->PkgNo - 1;
		ym->PkgBuf[2] = 0xFF - ym->PkgBuf[1];

		if(FileOpsLen != fread(ym->PkgBuf + 3,1,FileOpsLen,ym->File))
		{
			Ret = YMODEM_FILE_ERR;
		}
        __Ymodem_WriteCrc16(ym->PkgBuf,ym->PkgSize);
        ym->FileCnt += ym->PkgSize;
		break;
	case YMODEM_FILE_STAT:
		if(0 == stat(ym->FileName,&FpInfo))
		{
			ym->FileSize = (u32)FpInfo.st_size;
		}
		else
		{
			Ret = YMODEM_FILE_ERR;
		}
		break;
	case YMODEM_FILE_CLOSE:
		fclose(ym->File);
		break;
	default:
		break;
	}
	ym->FileOps = YMODEM_FILE_NOOPS;
	return Ret;
}

static YMRESULT __Ymodem_ReceiveProcess(tagYmodem *ym)
{
	YMRESULT Ret = YMODEM_OK;
	s64 CurrentTime;

	while(1)
	{
		Ret = __Ymodem_GetPkg(ym);
		if(Ret != YMODEM_OK)
		{
			__Ymodem_CancelTrans();
			break;
		}
		CurrentTime = DjyGetSysTime();					//总超时处理
		if(CurrentTime - ym->StartTime >= ym->TimeOut)
		{
			Ret = YMODEM_TIMEOUT;
			__Ymodem_CancelTrans();
			break;
		}
		//数据包处理
		switch(ym->PkgBuf[0])
		{
		case CN_YMODEM_SOH:
			if(!__Ymodem_PackCheck(ym->PkgBuf,ym->PkgSize))	//need retry load
				break;
			if(ym->Status == ENUM_YMODEM_STA_INFO)			//info pkg
			{
				__Ymodem_InfoPkg(ym);
			}
			else if(ym->Status == CN_YMODEM_EOT)
			{
				if(__Ymodem_IsZeroPkg(ym))
				{
					__Ymodem_Put(CN_YMODEM_ACK);//全零包，所有传输结束
					goto YMODEM_RECVEXIT;
				}
				else
				{
					__Ymodem_InfoPkg(ym);		//继续传下一个文件
				}
			}
			else							//收到128字节大小的数据包
			{
	            if(ym->PkgBuf[1] == (ym->PkgNo & 0xff))
	            {
	            	ym->PkgNo ++;
	            	ym->FileOps = YMODEM_FILE_WRITE;
	            }
	            else
	            {
	            	__Ymodem_Put(CN_YMODEM_NAK);	//包号错误，需重传
	            }
	            ym->Status = CN_YMODEM_SOH;
			}
			break;
		case CN_YMODEM_STX:
			if(!__Ymodem_PackCheck(ym->PkgBuf,ym->PkgSize))	//need retry load
				break;
            if(ym->PkgBuf[1] == (ym->PkgNo & 0xff))
            {
            	ym->PkgNo ++;
            	ym->FileOps = YMODEM_FILE_WRITE;
            }
            else
            {
            	__Ymodem_Put(CN_YMODEM_NAK);//包号错误，需重传
            }
            ym->Status = CN_YMODEM_STX;
			break;
		case CN_YMODEM_EOT:
	        if( (ym->Status == CN_YMODEM_SOH) || (ym->Status == CN_YMODEM_STX))	//	第一个EOT
	        {
	        	__Ymodem_Put(CN_YMODEM_NAK);                         //接收到结束符，回复ACK
	        	ym->Status = CN_YMODEM_EOT;
	        }
	        else if(ym->Status == CN_YMODEM_EOT)
	        {
	        	__Ymodem_Put(CN_YMODEM_ACK);                         //接收到结束符，回复ACK
	        	__Ymodem_Put(CN_YMODEM_C);                           //接收到结束符，回复C
	        }
			break;
		case CN_YMODEM_CAN:
        	Ret = YMODEM_CAN_TRANS;
        	goto YMODEM_RECVEXIT;
			break;
		default:
        	Ret = YMODEM_UNKNOW_ERR;
			break;
		}
		ym->PkgBufCnt = 0;

		Ret = __Ymodem_FileOps(ym,1);
		if(Ret != YMODEM_OK)			//可能情况 1.case 2.file ops
		{
			__Ymodem_CancelTrans();
			break;
		}
	}
YMODEM_RECVEXIT:
	ym->FileOps = YMODEM_FILE_CLOSE;
	__Ymodem_FileOps(ym,1);					//close file
	return Ret;
}

// ============================================================================
// 功能：Ymodem下载文件，无参数，因为文件名已经在ymodem协议数据名里面
// 参数：无，需通过shell配置当前路径，或者调用Ymodem_PathSet配置路径
// 返回：true,下载成功，否则，失败
// ============================================================================
bool_t Ymodem_DownloadFile(char *Param)
{
	YMRESULT Ret = YMODEM_OK;
    u32 CntOver = 0;

    if(NULL == pYmodem)
    {
    	Ret = YMODEM_MEM_ERR;
    	goto YMODEM_EXIT;
    }
    Lock_MutexPend(pYmodem->pYmodemMutex,CN_TIMEOUT_FOREVER);

    pYmodem->File 	= NULL;
    pYmodem->FileOps  = YMODEM_FILE_NOOPS;
    pYmodem->FileSize = 0;
    pYmodem->FileCnt  = 0;
    pYmodem->PkgNo    = 0;
    pYmodem->PkgSize  = CN_YMODEM_SOH_SIZE;
    pYmodem->TimeOut  = gYmodemTotalTimeOut;
    pYmodem->PkgBufCnt = 0;
    pYmodem->FileBufCnt = 0;
    pYmodem->Status   = ENUM_YMODEM_STA_INFO;
    pYmodem->FileBuf  = (u8*)malloc((gYmodemBufPkgNum + 1)*1024);
    pYmodem->PkgBuf   = (u8*)malloc(CN_YMODEM_PKGBUF_SIZE);

    if( (NULL == pYmodem->FileBuf) || (NULL == pYmodem->PkgBuf)  )
    {
    	Ret = YMODEM_MEM_ERR;
    	goto YMODEM_EXIT;
    }

    __Ymodem_Put(CN_YMODEM_C);

    //等待主机发送数据，超时返回
    printf("下载倒计时：     ");
    while(!__Ymodem_Gets(pYmodem->PkgBuf,1,1000*mS))
    {
        if (CntOver++ < 60)
        {
        	__Ymodem_Put(CN_YMODEM_C); //超时则重新发送C
            printf("\b\b\b\b\b%2dS ",60-CntOver);
            continue;
        }
        else
        {
        	Ret = YMODEM_TIMEOUT;
        	goto YMODEM_EXIT;
        }
    }

    pYmodem->PkgBufCnt = 1;
    pYmodem->StartTime = DjyGetSysTime();
    Ret = __Ymodem_ReceiveProcess(pYmodem);

YMODEM_EXIT:
	if( NULL != pYmodem->FileBuf)
		free(pYmodem->FileBuf);
	if( NULL != pYmodem->PkgBuf)
		free(pYmodem->PkgBuf);

	Lock_MutexPost(pYmodem->pYmodemMutex);
	if(Ret != YMODEM_OK)						//打印输出信息
	{
		if(Ret == YMODEM_PARAM_ERR)
		{
			printf("\r\nYMODEM PARAMETER ERR !\r\n");
		}
		else if(Ret == YMODEM_FILE_ERR)
		{
			printf("\r\nYMODEM FILE OPERATION ERR !\r\n");
		}
		else if(Ret == YMODEM_TIMEOUT)
		{
			printf("\r\nYMODEM OPERATION TIMEOUT ERR !\r\n");
		}
		else if(Ret == YMODEM_MEM_ERR)
		{
			printf("\r\nYMODEM NOT ENOUGH MEMORY !\r\n");
		}
		else if(Ret == YMODEM_CAN_TRANS)
		{
			printf("\r\nYMODEM BE CANCELED !\r\n");
		}
		else
		{
			printf("\r\nYMODEM UNKNOW ERR !\r\n");
		}
		return false;
	}
	else
	{
		printf("\r\nYMODEM SUCCESSED !\r\n");
		return true;
	}
}

static YMRESULT __Ymodem_SendProcess(tagYmodem *ym)
{
	YMRESULT Ret = YMODEM_OK;
	s64 CurrentTime;
	u8 Cmd[8];
	u32 temp;
	char *FileName;

	while(1)
	{
		if(ym->PkgBufCnt == 1)
		{
			Cmd[0] = ym->PkgBuf[0];
		}
		else if(!__Ymodem_Get(Cmd))
		{
			Ret = YMODEM_TIMEOUT;
			__Ymodem_CancelTrans();
			break;
		}
		CurrentTime = DjyGetSysTime();				//总超时处理
		if(CurrentTime - ym->StartTime >= ym->TimeOut)
		{
			Ret = YMODEM_TIMEOUT;
			__Ymodem_CancelTrans();
			break;
		}
		//开始发送数据包
		switch(Cmd[0])
		{
		case CN_YMODEM_C:
			if(ym->Status == ENUM_YMODEM_STA_INFO )			//写首包
			{
				FileName = strrchr(ym->FileName,'\\');
				if(NULL == FileName)
					FileName = ym->FileName;
				temp = strlen(FileName);
				ym->PkgSize = CN_YMODEM_SOH_SIZE;
                memset(ym->PkgBuf,0x00,ym->PkgSize + 5);
                ym->PkgBuf[0] = CN_YMODEM_SOH;
                ym->PkgBuf[1] = 0x00;
                ym->PkgBuf[2] = 0xFF;
                memcpy(ym->PkgBuf + 3,FileName,temp);
                ym->PkgBuf[temp + 3] = 0x00;
                itoa(ym->FileSize,(char*)(ym->PkgBuf + temp + 4),10);
                __Ymodem_WriteCrc16(ym->PkgBuf,ym->PkgSize);
                ym->Status = CN_YMODEM_C;		//开始发数据
                ym->PkgNo = 1;
			}
            else if(ym->PkgNo == 1)		//发送第一包
            {
            	ym->Status = ENUM_YMODEM_STA_SOH;
            	ym->FileOps = YMODEM_FILE_READ;
            	ym->PkgNo ++;
            }
            else if(ym->Status == CN_YMODEM_EOT)
            {
            	ym->PkgSize = CN_YMODEM_SOH_SIZE;
                memset(ym->PkgBuf,0x00,ym->PkgSize + 5);
                ym->PkgBuf[0] = CN_YMODEM_SOH;
                ym->PkgBuf[1] = 0x00;
                ym->PkgBuf[2] = 0xFF;
                __Ymodem_WriteCrc16(ym->PkgBuf,ym->PkgSize);
            	ym->Status = ENUM_YMODEM_STA_ZERO;
            }
			break;
		case CN_YMODEM_NAK:						//重发数据包
			break;
		case CN_YMODEM_CAN:
			Ret = YMODEM_CAN_TRANS;
			break;
		case CN_YMODEM_ACK:
            if( (ym->PkgNo == 1) )				//刚刚发送完首包信息包
            {
            	continue;
            }
            else if(ym->Status == ENUM_YMODEM_STA_ZERO)	//传输结束
            {
            	Ret = YMODEM_OK;
            	goto YMODEM_SENDEXIT;
            }
            else if(ym->FileCnt >= ym->FileSize)
            {
                __Ymodem_Put(CN_YMODEM_EOT);
                ym->Status = CN_YMODEM_EOT;
                continue;
            }
            else
            {
            	ym->PkgNo ++;
            	ym->Status = ENUM_YMODEM_STA_STX;
            	ym->FileOps = YMODEM_FILE_READ;
            }
			break;
		default:
			Ret = YMODEM_UNKNOW_ERR;
			break;
		}

		ym->PkgBufCnt = 0;
		Ret = __Ymodem_FileOps(ym,0);
		if(Ret == YMODEM_OK)
		{
			temp = __Ymodem_Puts(ym->PkgBuf,ym->PkgSize + 5,CN_TIMEOUT_FOREVER);
			if(temp != ym->PkgSize + 5)
			{
				Ret = YMODEM_MEDIA_ERR;
			}
		}
		if(Ret != YMODEM_OK)		//可能情况1.case 2.file 3.__Ymodem_Puts
		{
			__Ymodem_CancelTrans();
			break;
		}
	}
YMODEM_SENDEXIT:
	return Ret;
}

// ============================================================================
// 功能：Ymodem上载API，调用者传入文件名作为参数，读取文件
// 参数：文件名路径或文件名，
//      1.当Ymodem_PathSet被调用时，使用绝对路径；
//      2.否则使用的是相对路径，可通过shell等方式改变路径；
// 返回：true,上载成功，否则，失败
// ============================================================================
bool_t Ymodem_UploadFile(char *Param)
{
	YMRESULT Ret = YMODEM_OK;
    u32 CntOver = 0;
    char *NextParam,*FileName;

    if(NULL == pYmodem)
    {
    	Ret = YMODEM_MEM_ERR;
    	goto YMODEM_EXIT;
    }
    Lock_MutexPend(pYmodem->pYmodemMutex,CN_TIMEOUT_FOREVER);

    FileName = (char *)Sh_GetWord(Param, &NextParam);
    if(NULL == FileName)
    {
    	Ret = YMODEM_PARAM_ERR;
    	goto YMODEM_EXIT;
    }
    __Ymodem_FilePathMerge(pYmodem->FileName,pYmodem->Path,FileName);
    pYmodem->File 	= NULL;
    pYmodem->FileOps  = YMODEM_FILE_OPEN;
    pYmodem->FileSize = 0;
    pYmodem->FileCnt  = 0;
    pYmodem->PkgNo    = 0;
    pYmodem->PkgSize  = CN_YMODEM_SOH_SIZE;
    pYmodem->TimeOut  = gYmodemTotalTimeOut;
    pYmodem->PkgBufCnt = 0;
    pYmodem->FileBufCnt = 0;
    pYmodem->Status   = ENUM_YMODEM_STA_INFO;
    pYmodem->FileBuf  = NULL;
    pYmodem->PkgBuf   = (u8*)malloc(CN_YMODEM_PKGBUF_SIZE);

    if( (NULL == pYmodem->PkgBuf)  )
    {
    	Ret = YMODEM_MEM_ERR;
    	goto YMODEM_EXIT;
    }
    Ret = __Ymodem_FileOps(pYmodem,0);			//open the file,must be exist
    if(Ret != YMODEM_OK)
    {
    	goto YMODEM_EXIT;
    }
    pYmodem->FileOps  = YMODEM_FILE_STAT;
    Ret = __Ymodem_FileOps(pYmodem,0);			//GET THE FILE SIZE
    if(Ret != YMODEM_OK)
    {
    	goto YMODEM_EXIT;
    }

    printf("上载倒计时：     ");
    while(!__Ymodem_Gets(pYmodem->PkgBuf,1,1000*mS))//等待主机发送数据，超时返回
    {
        if (CntOver++ < 60)
        {
            printf("\b\b\b\b%2d s",60-CntOver);
            continue;
        }
        else
        {
        	Ret = YMODEM_TIMEOUT;
        	goto YMODEM_EXIT;
        }
    }

    pYmodem->PkgBufCnt = 1;
    pYmodem->StartTime = DjyGetSysTime();
    Ret = __Ymodem_SendProcess(pYmodem);

    pYmodem->FileOps = YMODEM_FILE_CLOSE;		//关闭文件
    __Ymodem_FileOps(pYmodem,0);

YMODEM_EXIT:
	if( NULL != pYmodem->FileBuf)
		free(pYmodem->FileBuf);
	if( NULL != pYmodem->PkgBuf)
		free(pYmodem->PkgBuf);

	Lock_MutexPost(pYmodem->pYmodemMutex);
	if(Ret != YMODEM_OK)						//打印输出信息
	{
		if(Ret == YMODEM_PARAM_ERR)
		{
			printf("\r\nYMODEM PARAMETER ERR, PLEASE ENTER FILE NAME !\r\n");
		}
		else if(Ret == YMODEM_FILE_ERR)
		{
			printf("\r\nYMODEM FILE OPERATION ERR !\r\n");
		}
		else if(Ret == YMODEM_TIMEOUT)
		{
			printf("\r\nYMODEM OPERATION TIMEOUT ERR !\r\n");
		}
		else if(Ret == YMODEM_MEM_ERR)
		{
			printf("\r\nYMODEM NOT ENOUGH MEMORY !\r\n");
		}
		else if(Ret == YMODEM_CAN_TRANS)
		{
			printf("\r\nYMODEM BE CANCELED !\r\n");
		}
		else
		{
			printf("YMODEM UNKNOW ERR !\r\n");
		}
		return false;
	}
	else
	{
		printf("\r\nYMODEM SUCCESSED !\r\n");
		return true;
	}

	return true;
}
