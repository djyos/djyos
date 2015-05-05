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
//所属模块:shell模块
//作者:  贺敏.
//版本：V1.0.0
//文件描述:为djyos的shell命令中增加下载文件的功能，可以将文件下
//         载到指定文件和指定地址
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-10-06
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
//todo: 把指定位置读写的功能拆出去,本文件保留标准文件系统传输的功能.
//todo: 须在初始化参数中指定通信设备,不能限于标准输入输出设备
#include "config-prj.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "file.h"
#include "char_term.h"
#include "string.h"
#include "djyos.h"
#include "systime.h"
#include "shell.h"
#include "driver.h"
#include "verify.h"
#include "stddev.h"
#include "ymodem.h"

extern const char *gc_pCfgStddevName;  //标准终端名字
                                    //
#define CN_YMODEM_SOH     1         //128字节起始符
#define CN_YMODEM_STX     2         //1028字节起始符
#define CN_YMODEM_EOT     4
#define CN_YMODEM_ACK     6
#define CN_YMODEM_NAK     0x15
#define CN_YMODEM_C       0x43          //大写字母C
#define CN_YMODEM_CAN     0x18
#define CN_YMODEM_ERR     0xff

#define CN_YMODEM_PACK_SIZE  1024       //ymodem的数据包大小
#define CN_YMODEM_HEAD_SIZE  128        //ymodem包关大小为128
#define CN_YMODEM_PACK_ZERO  00         //第一包

#define CN_FILEBUF_SIZE   (33*1024) //定义缓冲区大小，由函数动态分配
#define CN_MAX_PACKNUM    32            //定义最大包个数，超过该数，则需要写入文件
#define CN_DATABUF_SIZE    (32*1024)   //定义缓冲区大小

bool_t Sh_DownloadFile(char *param);
bool_t Sh_DownloadFileAt(char *param);
bool_t Sh_UploadFile(char *param);
bool_t Sh_UploadFileFrom(char *param);
bool_t Sh_ModifyFlash(char *param);

u32 (*fn_pWriteTo)(uint32_t addr, uint8_t *buf, uint32_t len) = NULL;
u32 (*fn_pReadFrom)(uint32_t addr, uint8_t *buf, uint32_t len) = NULL;

struct tagShellCmdTab const ymodem_cmd_table[] =
{
    {
        "download",
        Sh_DownloadFile,
        "下载文件",
        "命令格式: download"
    },
    {
        "download_at",
        Sh_DownloadFileAt,
        "下载文件到指定地址",
        "命令格式: download_at 地址"
    },
    {
        "upload",
        Sh_UploadFile,
        "上传文件",
        "命令格式: upload 文件名"
    },
    {
        "upload_from",
        Sh_UploadFileFrom,
        "读指定地址的数据，并上传为文件",
        "命令格式: upload_from 地址 长度 文件名"
    },
    {
        "flash",
        Sh_ModifyFlash,
        "修改flash中指定地址数据,最大不超16byte",
        "命令格式：flash 地址  长度 数据"
    }
};

static struct tagShellCmdRsc tg_ymodem_cmd_rsc
                        [sizeof(ymodem_cmd_table)/sizeof(struct tagShellCmdTab)];

//----ymodem模型初始化---------------------------------------
//功能: ymodem下载文件模块接口函数，供module_init调用，
//      该函数初始化xmodem，装入shell下载文件命令
//参数: 无
//返回: 无
//-----------------------------------------------------------
ptu32_t Ymodem_ModuleInit(ptu32_t para)
{
    if(Sh_InstallCmd(ymodem_cmd_table,tg_ymodem_cmd_rsc,
            sizeof(ymodem_cmd_table)/sizeof(struct tagShellCmdTab)))
        return true;
    else
        return false;
}

//----获取下载数据-------------------------------------------
//功能：ymodem下载时，获取下载数据函数，与djy_getchar相似，是
//      它的修改版本，以适应本模块应用需求
//参数：buf：缓冲区指针
//返回：实际读到的大小,cn_limit_uint32超时
//-----------------------------------------------------------
u32 __Ymodem_Gets(u8 *buf)
{
    struct tagInputDeviceMsg input_msg;
    struct tagCharTermineralMsg *char_msg;

    if(! Stddev_ReadDefaultMsg(&input_msg,5000*mS))
    {
        return CN_LIMIT_UINT32;
    }
//  input_msg = (struct tagInputDeviceMsg *)Djy_GetEventPara(NULL,NULL);
//  if(input_msg == NULL)
//  {
//      evttid = Djy_MyEvttId();
//      if(Djy_WaitEvttPop(evttid,NULL,5000*mS) == CN_SYNC_TIMEOUT)
//          return CN_LIMIT_UINT32;
//      input_msg = (struct tagInputDeviceMsg *)Djy_GetEventPara(NULL,NULL);
//  }
    char_msg = &(input_msg.input_data.char_termineral);
    memcpy(buf,char_msg->input_char,char_msg->num);
//  Djy_ParaUsed(0);

    return char_msg->num;
}

//----提取单词----------------------------------------------
//功能: 从buf中提取一个由空格或行结束符隔开的单词，next用于
//      返回下一个单词首地址，如果没有下一个单词，则next=NULL。
//参数: buf，待分析的字符串
//      next，返回下一个单词指针
//返回: 提取的单词指针，已将单词后面的分隔符换成串结束符'\0'
//-----------------------------------------------------------
char *__Ymodem_GetWord(char *buf,char **next)
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
void __Ymodem_CancelTrans(void)
{
    Djy_PutChar(CN_YMODEM_CAN);                 //取消传输
    Djy_PutChar(CN_YMODEM_CAN);
    Djy_PutChar(CN_YMODEM_CAN);
}

//------校验ymodem数据包-------------------------------------
//功能：ymodem模块校验接收到的数据包
//参数：buf：数据包缓冲
//      pack_len: ymodem数据包长度
//返回：true:  数据包校验正确
//      false: 数据包校验错误
//----------------------------------------------------------
bool_t __Ymodem_PackCheck(u8* buf, u32 pack_len)
{
    u16 checksum;
    if((buf[1] + buf[2]) != 0xff)               //校验包号正反码
    {
        Djy_PutChar(CN_YMODEM_NAK);                 //应答nak，请求重发
        return false;
    }
    checksum = Crc_16(buf+3, pack_len);
    if(checksum != *(u16 *)&buf[pack_len+3])    //CRC校验错误
    {
        Djy_PutChar(CN_YMODEM_NAK);                 //应答nak，请求重发
        return false;
    }

    return true;
}

//------ymodem下载文件---------------------------------------
//功能：下载文件的shell命令
//参数：param: 参数字符串
//返回：true:  操作成功
//      false: 操作失败
//----------------------------------------------------------
bool_t Sh_DownloadFile(char *param)
{
    u8 package[1029];
    u32 over=0,readed=0,reads=0;
    u32 write_len=0,file_size=0,completed=0;
    u32 offset=0,pack_no=0,pack_len=0; //包号从0开始计算
    u32 i=0,sum=0;

    char* filebuf;
    FILE *file=NULL;

    char *filename,*nextparam,*str_filesize;

    //缓冲区设为32K
    if((filebuf=(char*)M_MallocLc(CN_FILEBUF_SIZE,0))==NULL)
    {
        printf("申请内存失败!\r\n");
        return false;
    }

    Djy_PutChar(CN_YMODEM_C);

    //等待主机发送数据，超时返回
    printf("下载倒计时：     ");
    while (Djy_WaitEvttPop(Djy_MyEvttId(), NULL,
            1000 * mS) == CN_SYNC_TIMEOUT)
    {
        if (over++ < 30)
        {
            printf("\b\b\b\b\b%2dS ",30-over);
            Djy_PutChar(CN_YMODEM_C); //超时则重新发送C
            continue;
        }
        else
        {
            printf("超时未操作\r\n");
            free(filebuf);
            return false;
        }
    }
    //已经开始传送文件
    while(1)
    {
        reads = __Ymodem_Gets(package + readed);            //读取数
        readed += reads;
        if(reads == CN_LIMIT_UINT32)                        //接收超时
        {
            __Ymodem_CancelTrans();
            printf("\r\n传输超时或选择协议错误！\r\n");
            printf("请选择Ymodem！\r\n");
            fclose(file);
            free(filebuf);
            return false;
        }

        switch(package[0])
        {
        case CN_YMODEM_SOH:                                 //128字节帧（包括起始帧、结束帧、部分数据帧）
            pack_len = CN_YMODEM_HEAD_SIZE;
            if(readed >= pack_len+5)
            {
                readed = 0;
                if(__Ymodem_PackCheck(package,pack_len) == false)
                    break;
                if(package[1] == CN_YMODEM_PACK_ZERO)               //包号为0，则表示为起始包或结束包
                {
                    if(pack_no == 0)    //pack_no为0时，表示该包为起始包,否则为结束包
                    {
                        //获取名称和大小
                        filename = __Ymodem_GetWord((char*)&package[3],&nextparam);         //获取文件名
                        str_filesize = __Ymodem_GetWord(nextparam,&nextparam);
                        file_size = strtol(str_filesize, (char **)NULL, 0);

                        //打开文件
                        file=fopen(filename,"r+b");
                        if(file == NULL)
                        {
                            __Ymodem_CancelTrans();
                            printf("\r\n打开文件失败或文件不存在!\r\n");
                            free(filebuf);
                            return false;
                        }
                        if(file_size > Djyfs_Fcubage(file))
                        {
                            __Ymodem_CancelTrans();
                            printf("\r\n下载的文件超过该文件的最大尺寸!\r\n");
                            free(filebuf);
                            return false;
                        }
                        Djy_PutChar(CN_YMODEM_ACK);                                             //首包回复ACK
                        Djy_PutChar(CN_YMODEM_C);
                    }
                    else
                    {
                        for(i=3; i < 130; i++)
                            sum += package[i];
                        if(sum == 0)                                //传输结束
                        {
                            Djy_PutChar(CN_YMODEM_ACK);
                            printf("\r\n下载完成\r\n");
                            Djyfs_Ftruncate(file, completed);
                            fclose(file);//-----todo
                            free(filebuf);
                            return true;
                        }
                        else                            //既非起始，也非结束，则错误
                        {
                            __Ymodem_CancelTrans();
                            printf("\r\n传输错误或选择协议错误！\r\n");
                            printf("请选择Ymodem！\r\n");
                            free(filebuf);
                            return false;
                        }
                    }
                }
                else                                        //该包为普通128字节数据包
                {
                    if(package[1] != (pack_no & 0xff))
                    {
                        __Ymodem_CancelTrans();
                        printf("\r\n传输错误或选择协议错误！\r\n");
                        printf("请选择Ymodem！\r\n");
                        fclose(file);
                        free(filebuf);
                        return false;
                    }

                    memcpy(&filebuf[offset],(const char*)package+3,pack_len);
                    offset += pack_len;
                    if((offset >= file_size-completed) || (offset >= CN_FILEBUF_SIZE-1024)) //如果足够大，写入flash
                    {
                        if(offset >= file_size-completed)                                   //判断本次写入大小
                            write_len = file_size - completed;
                        else
                            write_len = CN_FILEBUF_SIZE - 1024;
                        offset = 0;
                        fwrite_r(file, filebuf,write_len, completed);
                        completed += write_len;
                    }
                    Djy_PutChar(CN_YMODEM_ACK);
                }
                pack_no++;
            }
            break;
        case CN_YMODEM_STX:                                 //数据帧，1024字节
            pack_len = CN_YMODEM_PACK_SIZE;
            if(readed >= pack_len+5)
            {
                readed = 0;
                if(__Ymodem_PackCheck(package,pack_len) == false)
                    break;
                if(package[1] != (pack_no & 0xff))
                {
                    __Ymodem_CancelTrans();
                    printf("\r\n传输错误！\r\n");
                    fclose(file);
                    free(filebuf);
                    return false;
                }
                memcpy(&filebuf[offset],(const char*)package+3,pack_len);
                offset += pack_len;
                if((offset >= file_size-completed) || (offset >= CN_FILEBUF_SIZE-1024)) //如果足够大，写入flash
                {
                    if(offset >= file_size-completed)                                   //判断本次写入大小
                        write_len = file_size - completed;
                    else
                        write_len = CN_FILEBUF_SIZE-1024;
                    offset = 0;
                    fwrite_r(file, filebuf,write_len, completed);
                    completed += write_len;
                }
                Djy_PutChar(CN_YMODEM_ACK);
                pack_no++;
            }
            break;
        case CN_YMODEM_EOT:                                 //结束符,后面还有全0数据帧
            readed = 0;
            Djy_PutChar(CN_YMODEM_ACK);                         //接收到结束符，回复ACK
            Djy_PutChar(CN_YMODEM_C);                           //接收到结束符，回复C
            break;
        case CN_YMODEM_CAN:                                 //取消传输
            printf("\r\n取消传输成功\r\n");
            fclose(file);
            free(filebuf);
            return true;
        default:
            __Ymodem_CancelTrans();
            printf("\r\n传输错误或选择协议错误！\r\n");
            printf("请选择Ymodem！\r\n");
            fclose(file);
            free(filebuf);
            return false;
        }
    }
    return true;
}

//-------ymodem下载文件到指定地址----------------------------
//功能：下载文件到指定的地址，地址通过参数指定
//参数：param: 参数字符串
//返回：true:  操作成功
//      false: 操作失败
//----------------------------------------------------------
bool_t Sh_DownloadFileAt(char *param)
{
    u8 package[1029];
    u32 over=0,readed=0,reads=0;
    u32 write_len=0,file_size=0,completed=0;
    u32 offset=0,pack_no=0,pack_len=0; //包号从0开始计算
    u32 i=0,sum=0;
    u32 result= 0;

    char* filebuf;
    char *str_addr,*next_param;
    u32 addr;

    char *nextparam,*str_filesize;

    if(fn_pWriteTo == NULL)
        return false;
    str_addr = Sh_GetWord(param,&next_param);
    addr = strtol(str_addr, (char **)NULL, 0);

    if(addr == 0)
    {
        printf("请输入地址\r\n");
        printf("正确的命令:download_at 地址");
        return false;
    }

    //缓冲区设为32K
    if((filebuf=(char*)M_MallocLc(CN_FILEBUF_SIZE,0))==NULL)
    {
        printf("申请内存失败!\r\n");
        return false;
    }

    Djy_PutChar(CN_YMODEM_C);

    //等待主机发送数据，超时返回
    printf("下载倒计时：     ");
    while (Djy_WaitEvttPop(Djy_MyEvttId(), NULL,
            1000 * mS) == CN_SYNC_TIMEOUT)
    {
        if (over++ < 30)
        {
            printf("\b\b\b\b\b%2ds ",30-over);
            Djy_PutChar(CN_YMODEM_C); //超时则重新发送C
            continue;
        }
        else
        {
            printf("超时未操作\r\n");
            free(filebuf);
            return false;
        }
    }
    //已经开始传送文件
    while(1)
    {
        reads = __Ymodem_Gets(package + readed);            //读取数
        readed += reads;
        if(reads == CN_LIMIT_UINT32)                        //接收超时
        {
            __Ymodem_CancelTrans();
            printf("\r\n传输超时或选择协议错误！\r\n");
            printf("请选择Ymodem！\r\n");
            free(filebuf);
            return false;
        }

        switch(package[0])
        {
        case CN_YMODEM_SOH:                                 //128字节帧（包括起始帧、结束帧、部分数据帧）
            pack_len = CN_YMODEM_HEAD_SIZE;
            if(readed >= pack_len+5)
            {
                readed = 0;
                if(__Ymodem_PackCheck(package,pack_len) == false)
                    break;
                if(package[1] == CN_YMODEM_PACK_ZERO)               //包号为0，则表示为起始包或结束包
                {
                    if(pack_no == 0)    //pack_no为0时，表示该包为起始包,否则为结束包
                    {
                        //获取名称和大小
                        __Ymodem_GetWord((char*)&package[3],&nextparam);            //获取文件名
                        str_filesize = __Ymodem_GetWord(nextparam,&nextparam);
                        file_size = strtol(str_filesize, (char **)NULL, 0);

                        Djy_PutChar(CN_YMODEM_ACK);                                             //首包回复ACK
                        Djy_PutChar(CN_YMODEM_C);
                    }
                    else
                    {
                        for(i=3; i < 130; i++)
                            sum += package[i];
                        if(sum == 0)                                //传输结束
                        {
                            Djy_PutChar(CN_YMODEM_ACK);
                            printf("\r\n下载完成\r\n");
                            free(filebuf);
                            return true;
                        }
                        else                            //既非起始，也非结束，则错误
                        {
                            __Ymodem_CancelTrans();
                            printf("\r\n传输错误或选择协议错误！\r\n");
                            printf("请选择Ymodem！\r\n");
                            free(filebuf);
                            return false;
                        }
                    }
                }
                else                                        //该包为普通128字节数据包
                {
                    if(package[1] != (pack_no & 0xff))
                    {
                        __Ymodem_CancelTrans();
                        printf("\r\n传输错误或选择协议错误！\r\n");
                        printf("请选择Ymodem！\r\n");
                        free(filebuf);
                        return false;
                    }

                    memcpy(&filebuf[offset],(const char*)package+3,pack_len);
                    offset += pack_len;
                    if((offset >= file_size-completed) || (offset >= CN_FILEBUF_SIZE-1024)) //如果足够大，写入flash
                    {
                        if(offset >= file_size-completed)                                   //判断本次写入大小
                            write_len = file_size - completed;
                        else
                            write_len = CN_FILEBUF_SIZE - 1024;
                        offset = 0;
                        result = fn_pWriteTo(addr+completed,(u8 *)filebuf,write_len);
                        if(result != write_len)
                        {
                            __Ymodem_CancelTrans();
                            if(result == 0)
                                printf("\r\n写入不成功\r\n");
                            else if(result == CN_LIMIT_UINT32)
                                printf("\r\n输入的地址不正确\r\n");
                            free(filebuf);
                            return false;
                        }
                        completed += write_len;
                    }
                    Djy_PutChar(CN_YMODEM_ACK);
                }
                pack_no++;
            }
            break;
        case CN_YMODEM_STX:                                 //数据帧，1024字节
            pack_len = CN_YMODEM_PACK_SIZE;
            if(readed >= pack_len+5)
            {
                readed = 0;
                if(__Ymodem_PackCheck(package,pack_len) == false)
                    break;
                if(package[1] != (pack_no & 0xff))
                {
                    __Ymodem_CancelTrans();
                    printf("\r\n传输错误！\r\n");
                    free(filebuf);
                    return false;
                }
                memcpy(&filebuf[offset],(const char*)package+3,pack_len);
                offset += pack_len;
                if((offset >= file_size-completed) || (offset >= CN_FILEBUF_SIZE-1024)) //如果足够大，写入flash
                {
                    if(offset >= file_size-completed)                                   //判断本次写入大小
                        write_len = file_size - completed;
                    else
                        write_len = CN_FILEBUF_SIZE - 1024;
                    offset = 0;
                    result = fn_pWriteTo(addr+completed,(u8 *)filebuf,write_len);
                    if(result != write_len)
                    {
                        __Ymodem_CancelTrans();
                        if(result == 0)
                            printf("\r\n写入不成功\r\n");
                        else if(result == CN_LIMIT_UINT32)
                            printf("\r\n输入的地址不正确\r\n");
                        free(filebuf);
                        return false;
                    }
                    completed += write_len;
                }
                Djy_PutChar(CN_YMODEM_ACK);
                pack_no++;
            }
            break;
        case CN_YMODEM_EOT:                                 //结束符,后面还有全0数据帧
            readed = 0;
            Djy_PutChar(CN_YMODEM_ACK);                         //接收到结束符，回复ACK
            Djy_PutChar(CN_YMODEM_C);                           //接收到结束符，回复C
            break;
        case CN_YMODEM_CAN:                                 //取消传输
            printf("\r\n取消传输成功\r\n");
            free(filebuf);
            return true;
        default:
            __Ymodem_CancelTrans();
            printf("\r\n传输错误或选择协议错误！\r\n");
            printf("请选择Ymodem！\r\n");
            free(filebuf);
            return false;
        }
    }
    return true;
}

bool_t __sh_WriteFile(u8 *pData,u32 length,u32 timeout)
{
    tagDevHandle char_term_hdl;
    u32 stdinout;
    bool_t result = false;

    stdinout = Driver_FindDevice((char*)gc_pCfgStddevName);
    char_term_hdl = Driver_OpenDeviceAlias(stdinout,O_RDWR,0);
    if(NULL != char_term_hdl)
    {
        Driver_WriteDevice(char_term_hdl,pData,length,0,CN_BLOCK_BUFFER,timeout);
        Driver_CloseDevice(char_term_hdl);
        result = true;
    }

    return result;
}
//----------------------------------------------------------
//功能：上传文件的shell命令
//参数：param: 参数字符串
//返回：true:  操作成功
//      false: 操作失败
//----------------------------------------------------------
bool_t Sh_UploadFile(char *param)
{
    char *filebuf = NULL;
    FILE *file=NULL;
    char *filename,*nextparam;

    u32 over = 0,readed = 0,reads = 0,pack_no = 0;
    u32 sendsum = 0,sendlen=0,completed = 0,checklen = 0;
    u32 filenamelen = 0;
    u8 command[32],package[1029];
    u16 checksum = 0;

    filename = Sh_GetWord(param, &nextparam);
    if(filename == NULL)
    {
        printf("命令格式错误!\r\n");
        printf("正确的命令格式:upload 文件名\r\n");
        return false;
    }
    file = fopen(filename,"r+b");//打开方式-------todo
    if(file == NULL)
    {
        printf("文件不存在或打开失败\r\n");
        return false;
    }

    filenamelen = strlen(filename);
    sendsum = (u32)(file->file_size);
    //等待接收发送文件请求
    while (Djy_WaitEvttPop(Djy_MyEvttId(), NULL,
            1000 * mS) == CN_SYNC_TIMEOUT)
    {
        if (over++ < 30)
        {
            printf("\b\b\b\b%2d s",30-over);
            continue;
        }
        else
        {
            printf("超时未操作\r\n");
            free(filebuf);
            return false;
        }
    }

    filebuf = (char *)M_MallocLc(sendsum,0);
    if(filebuf == NULL)
    {
        printf("申请内存失败!\r\n");
        return false;
    }
    fread_r(file, filebuf, sendsum, 0);

    while(1)
    {
        reads = __Ymodem_Gets(command + readed);            //读取数
        readed += reads;
        if(reads == CN_LIMIT_UINT32)                        //接收超时
        {
            __Ymodem_CancelTrans();
            printf("\r\n传输超时或选择协议错误！\r\n");
            printf("请选择Ymodem！\r\n");
            fclose(file);
            free(filebuf);
            return false;
        }
        switch(command[0])
        {
        case CN_YMODEM_C:
            readed = 0;
            if(pack_no == 0)                                //首包
            {
                memset(package,0x00,CN_YMODEM_HEAD_SIZE+5);
                package[0] = CN_YMODEM_SOH;
                package[1] = 0x00;
                package[2] = 0xFF;
                memcpy(package+3,filename,filenamelen);
                package[filenamelen+3] = 0x00;
                itoa(sendsum,(char*)&package[filenamelen+4],10);
                checksum = Crc_16(package+3,CN_YMODEM_HEAD_SIZE);
                memcpy(package+CN_YMODEM_HEAD_SIZE+3,&checksum, 2);

                //send the start package
//                Driver_DevWriteLeft(pg_char_term_lhdl,(ptu32_t)package,
//                              CN_YMODEM_HEAD_SIZE+5,CN_TIMEOUT_FOREVER);
                __sh_WriteFile(package,CN_YMODEM_HEAD_SIZE+5,CN_TIMEOUT_FOREVER);
            }
            pack_no++;
            break;
        case CN_YMODEM_NAK:
            __Ymodem_CancelTrans();
            printf("请选择ymodem\r\n");
            fclose(file);
            free(filebuf);
            return false;
            break;
        case CN_YMODEM_CAN:
            fclose(file);
            free(filebuf);
            return false;
            break;
        case CN_YMODEM_ACK:
            readed = 0;
            if(completed < sendsum)//继续发送数据包
            {
                if(sendsum - completed <= CN_YMODEM_HEAD_SIZE)//不够128，则取128bytes
                {
                    memset(package,0x00,CN_YMODEM_HEAD_SIZE+5);
                    sendlen = sendsum-completed;
                    checklen = CN_YMODEM_HEAD_SIZE;
                    package[0] = CN_YMODEM_SOH;
                }
                else if(sendsum - completed <= CN_YMODEM_PACK_SIZE)//大于128，不够1024
                {
                    memset(package,0x00,CN_YMODEM_PACK_SIZE+5);
                    sendlen = sendsum-completed;
                    checklen = CN_YMODEM_PACK_SIZE;
                    package[0] = CN_YMODEM_STX;
                }
                else    //足够1024，则包大小为1024
                {
                    memset(package,0x00,CN_YMODEM_PACK_SIZE+5);
                    sendlen = CN_YMODEM_PACK_SIZE;
                    checklen = CN_YMODEM_PACK_SIZE;
                    package[0] = CN_YMODEM_STX;
                }
                package[1] = pack_no;
                package[2] = 0xFF - pack_no;
                memcpy(package+3,filebuf+completed,sendlen);
                checksum = Crc_16(package+3,checklen);
                memcpy(package+checklen+3,&checksum,2);
                //send the start package
//                Driver_DevWriteLeft(pg_char_term_lhdl,(ptu32_t)package,
//                              checklen+5,CN_TIMEOUT_FOREVER);
                __sh_WriteFile(package,checklen+5,CN_TIMEOUT_FOREVER);
                completed += sendlen;
            }
            else    //数据包传输完毕，可结束
            {
                Djy_PutChar(CN_YMODEM_EOT);
                memset(package,0x00,CN_YMODEM_HEAD_SIZE+5);
                package[0] = CN_YMODEM_SOH;
                package[1] = 0x00;
                package[2] = 0xFF;
                checksum = Crc_16(package+3,CN_YMODEM_HEAD_SIZE);
                memcpy(package+CN_YMODEM_HEAD_SIZE+3,&checksum, 2);
                //send the start package
//                Driver_DevWriteLeft(pg_char_term_lhdl,(ptu32_t)package,
//                              CN_YMODEM_HEAD_SIZE+5,CN_TIMEOUT_FOREVER);
                __sh_WriteFile(package,CN_YMODEM_HEAD_SIZE+5,CN_TIMEOUT_FOREVER);

                printf("传输结束\r\n");
                fclose(file);
                free(filebuf);
                return true;
            }
            pack_no++;
            break;
        default:
            fclose(file);
            free(filebuf);
            return false;
            break;
        }
    }
}

//-------ymodem从指定地址读取数据并保存到一个文件-----------
//功能：从指定的位置读取指定长度的数据，并以文件的形式传送
//参数：param: 参数字符串，param中包含如下参数
//      1.addr,读取数据的起始地址，如0xffc00000
//      2.len, 读取数据的长度，字节为单位，如128
//      3.filename,文件名，将读取的数据存放的文件，包括后缀
//返回：true:  操作成功
//      false: 操作失败
//----------------------------------------------------------
bool_t Sh_UploadFileFrom(char *param)
{
    u8 *databuf = NULL;
    char *addr_str,*len_str,*filename,*nextparam;

    u32 read_addr,read_len,filename_len;
    u32 loop,read_flash_times = 0,temp_read_len;
    u32 read_cp = 0;

    u32 over = 0,readed = 0,reads = 0,pack_no = 0;
    u32 sendlen=0,completed = 0,checklen = 0;
    u8 command[32],package[1029];
    u16 checksum = 0;

    if(fn_pReadFrom == NULL)
        return false;
    addr_str = Sh_GetWord(param, &nextparam);
    len_str  = Sh_GetWord(nextparam, &nextparam);
    filename = Sh_GetWord(nextparam, &nextparam);
    if((addr_str == NULL) || (len_str == NULL) || (filename == NULL))
    {
        printf("\r\n正确的命令格式: upload_from 地址 长度 文件名\r\n");
        return false;
    }

    read_addr    = strtol(addr_str, (char **)NULL, 0);
    read_len     = strtol(len_str, (char **)NULL, 0);
    filename_len = strlen(filename);
    read_flash_times = read_len/CN_DATABUF_SIZE + (read_len%CN_DATABUF_SIZE?1:0);

    databuf = (u8 *)M_MallocLc(CN_DATABUF_SIZE,0);
    if(databuf == NULL)
    {
        printf("申请内存失败!\r\n");
        return false;
    }
    //等待接收发送文件请求
    while (Djy_WaitEvttPop(Djy_MyEvttId(), NULL,
            1000 * mS) == CN_SYNC_TIMEOUT)
    {
        if (over++ < 30)
        {
            printf("\b\b\b\b%2d s",30-over);
            continue;
        }
        else
        {
            printf("超时未操作\r\n");
            free(databuf);
            return false;
        }
    }

    for(loop = 0; loop < read_flash_times;loop++ )
    {
        completed = 0;
        if(loop == read_flash_times - 1)
            temp_read_len = read_len - loop * CN_DATABUF_SIZE;
        else
            temp_read_len = CN_DATABUF_SIZE;
        read_cp += fn_pReadFrom(read_addr + read_cp,
                                        databuf, temp_read_len);
        if(loop >0)
            goto continue_send;
        while(1)
        {
            reads = __Ymodem_Gets(command + readed);            //读取数
            readed += reads;
            if(reads == CN_LIMIT_UINT32)                        //接收超时
            {
                __Ymodem_CancelTrans();
                printf("\r\n传输超时或选择协议错误！\r\n");
                printf("请选择Ymodem！\r\n");
                free(databuf);
                return false;
            }
            switch(command[0])
            {
            case CN_YMODEM_C:
                readed = 0;
                if(pack_no == 0)                                //首包
                {
                    memset(package,0x00,CN_YMODEM_HEAD_SIZE+5);
                    package[0] = CN_YMODEM_SOH;
                    package[1] = 0x00;
                    package[2] = 0xFF;
                    memcpy(package+3,filename,filename_len);
                    package[filename_len+3] = 0x00;              //文件名
                    itoa(read_len,(char*)&package[filename_len+4],10);
                    checksum = Crc_16(package+3,CN_YMODEM_HEAD_SIZE);
                    memcpy(package+CN_YMODEM_HEAD_SIZE+3,&checksum, 2);

                    //send the start package
//                    Driver_DevWriteLeft(pg_char_term_lhdl,(ptu32_t)package,
//                                  CN_YMODEM_HEAD_SIZE+5,CN_TIMEOUT_FOREVER);
                    __sh_WriteFile(package,CN_YMODEM_HEAD_SIZE+5,CN_TIMEOUT_FOREVER);
                }
                pack_no++;
                break;
            case CN_YMODEM_NAK:       //包重发，还没做-----todo
                __Ymodem_CancelTrans();
                printf("请选择ymodem\r\n");
                free(databuf);
                return false;
                break;
            case CN_YMODEM_CAN:        //取消发送
                free(databuf);
                return false;
                break;
            case CN_YMODEM_ACK:        //应答，继续发送包
continue_send:
                readed = 0;
                if(completed < temp_read_len) //继续发送数据包
                {
                    if(temp_read_len - completed <= CN_YMODEM_HEAD_SIZE)//不足128
                    {
                        memset(package,0x00,CN_YMODEM_HEAD_SIZE+5);
                        sendlen = temp_read_len - completed;
                        checklen = CN_YMODEM_HEAD_SIZE;
                        package[0] = CN_YMODEM_SOH;
                    }
                    else if(temp_read_len - completed <= CN_YMODEM_PACK_SIZE)//不足1024
                    {
                        memset(package,0x00,CN_YMODEM_PACK_SIZE+5);
                        sendlen = temp_read_len-completed;
                        checklen = CN_YMODEM_PACK_SIZE;
                        package[0] = CN_YMODEM_STX;
                    }
                    else        //1024/pack
                    {
                        memset(package,0x00,CN_YMODEM_PACK_SIZE+5);
                        sendlen = CN_YMODEM_PACK_SIZE;
                        checklen = CN_YMODEM_PACK_SIZE;
                        package[0] = CN_YMODEM_STX;
                    }
                    package[1] = pack_no;
                    package[2] = 0xFF - pack_no;
                    memcpy(package+3,databuf+completed,sendlen);
                    checksum = Crc_16(package+3,checklen);
                    memcpy(package+checklen+3,&checksum,2);
                    //send the start package
//                    Driver_DevWriteLeft(pg_char_term_lhdl,(ptu32_t)package,
//                                  checklen+5,CN_TIMEOUT_FOREVER);
                    __sh_WriteFile(package,checklen+5,CN_TIMEOUT_FOREVER);
                    completed += sendlen;
                }
                else if(loop == read_flash_times - 1)//传输已经全部完成，可结束
                {
                    Djy_PutChar(CN_YMODEM_EOT);
                    memset(package,0x00,CN_YMODEM_HEAD_SIZE+5);
                    package[0] = CN_YMODEM_SOH;
                    package[1] = 0x00;
                    package[2] = 0xFF;
                    checksum = Crc_16(package+3,CN_YMODEM_HEAD_SIZE);
                    memcpy(package+CN_YMODEM_HEAD_SIZE+3,&checksum, 2);
                    //send the start package
//                    Driver_DevWriteLeft(pg_char_term_lhdl,(ptu32_t)package,
//                                  CN_YMODEM_HEAD_SIZE+5,CN_TIMEOUT_FOREVER);
                    __sh_WriteFile(package,CN_YMODEM_HEAD_SIZE+5,CN_TIMEOUT_FOREVER);

                    printf("传输结束\r\n");
                    free(databuf);
                    return true;
                }
                else                //databuf中的数据传输完成
                    goto read_loop;
                pack_no++;
                break;
            default:
                free(databuf);
                return false;
                break;
            }
        }
        //for loop
read_loop:
        completed = 0;
    }
    return true;
}

//-------ymodem修改flash数据--------------------------------
//功能：修改FLASH中指定地址的数据，只支持修改指定地址1字节、2字节
//      和4字节长度
//参数：param: 参数字符串
//返回：true:  操作成功
//      false: 操作失败
//----------------------------------------------------------
bool_t Sh_ModifyFlash(char *param)
{
    ptu32_t addr;
    uint32_t data,length;
    uint8_t buf[16],i;
    char *word_addr,*len,*word_byte,*next_param;

    if(fn_pWriteTo == NULL)
        return false;
    //提取3个参数
    word_addr = Sh_GetWord(param,&next_param);
    len = Sh_GetWord(next_param,&next_param);
    if((word_addr == NULL)||(len == NULL))
    {
        printf("\r\n格式错误，正确格式是：\r\n>flash 地址 长度 数据\r\n");
        return false;
    }

    addr = strtol(word_addr, (char **)NULL, 0);
    length = strtol(len, (char **)NULL, 0);

    if(length > 16)
    {
        printf("\r\n输入错误，输入长度大于16字节最大长度！\r\n");
        return false;
    }

    for(i = 0; i < length; i++)
    {
        word_byte = Sh_GetWord(next_param,&next_param);
        if(word_byte == NULL)
        {
            printf("\r\n输入错误，数据与长度不一致！\r\n");
            return false;
        }
        data = strtol(word_byte, (char **)NULL, 0);
        if(data > 0xFF)
        {
            printf("\r\n输入错误，字节数据大于255！\r\n");
            return 0;
        }
        buf[i] = (u8)data;
    }

    if(fn_pWriteTo(addr,(u8*)buf,length) != length)
    {
        printf("\r\n写入flash错误\r\n");
    }
    return 0;
}

//----注册读写函数-------------------------------------------------------------
//功能: 往指定的地址都/写的功能,与文件传输不一样,文件操作有标准函数可用.由于芯片
//      型号各异、板子配置不同,无法给出标准的操作函数.于是ymodem模块定义了两个
//      函数指针,由用户实现相关函数后,调用本函数注册到这两个指针里.
//参数: pWriteTo,写数据到指定地址的函数指针
//      pReadFrom, 从制定地址读数据的函数指针。
//返回: 无
//-----------------------------------------------------------------------------
void ymodem_RegisterFn  (
                u32 (*pWriteTo)(uint32_t addr, uint8_t *buf, uint32_t len),
                u32 (*pReadFrom)(uint32_t addr, uint8_t *buf, uint32_t len)
                        )
{
    if(pWriteTo != NULL)
        fn_pWriteTo = pWriteTo;
    if(pReadFrom != NULL)
        fn_pReadFrom = pReadFrom;
    return;
}
