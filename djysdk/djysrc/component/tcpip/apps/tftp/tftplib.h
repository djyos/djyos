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

#ifndef __TFTP_H
#define __TFTP_H

#include <sys/socket.h>

enum __EN_TFTP_OPCODE
{
	TFTP_RRQ = 1,
	TFTP_WRQ,
	TFTP_DATA,
	TFTP_ACK,
	TFTP_ERR,
	TFTP_OACK,
};


#define CN_TFTP_SERVERPORT_DEFAULT    69               //socket port
#define CN_TFTP_BLKSIZE_DEFAULT       512              //unit:byte
#define CN_TFTP_TIMEOUT_DEFAULT       2                //unit:second
#define CN_TFTP_TIMEOUTLIMIT_DEFAULT  3                //timesout limit
#define CN_STRING_LEN                 32
#define CN_PATH_LENLIMIT              128
#define CN_TFTPSERVER_BUFLEN          0x80             //unit:byte
#define CN_TFTP_RETRANSLIMIT          20               //unit:times

typedef struct TftpClient
{
    char   filename[CN_STRING_LEN];    //file name to download or upload
    char   mode[CN_STRING_LEN];        //used to decode the option value
    u16    reqmode;          //request mode
    int    timeout;          //the timeout time level
    int    otimeout;         //the timeout time for oack
    int    blksize;          //the blksize,(each block has this length data)
    int    oblksize;         //the blksize for oack
    int    tsize;            //the file size
    int    otsize;           //the file size for oack
    u32    timeoutLimit;     //the receive timeout times limit
    u32    timeoutTimes;     //the receive timeout times

    u32    block;            //u16 maybe got overflow easily, so we use u32
    u16    errcode;
    char   errmsg[CN_STRING_LEN];
    u32    rcvlen;          //the data length has received
    u32    sntlen;          //the data length has sent
    u32    retrans;         //the retrans times
    bool_t lastblock;       //the last block
    bool_t server;
    u32    sndspeed;        //use to record the send speed
    u32    rcvspeed;        //use to record the receive speed
    u32    timeused;        //use to record the timeuse;
    s64    timestart;       //use to record the start time
    s64    timestop;        //use to record the stop time

    int  (*readdata)(struct TftpClient *client,u8 *buf, int len);
    int  (*writedata)(struct TftpClient *client,u8 *buf, int len);

    u8   *buf;              //used as the client buf
    u32   buflen;
    int   fd;               //file handle
    int   sock;             //the socket handle
    struct sockaddr_in netaddr;
    int     addrlen ;
}tagTftpClient;



enum __EN_TFTPERR
{
    EN_TFTPOK =0,
    EN_TFTPERR_FILENOTFOUND ,
    EN_TFTPERR_FILECREATFAILED,
    EN_TFTPERR_FILESTATERR,
    EN_TFTPERR_INVALIDOPERATION,
    EN_TFTPERR_TIMEOUT,
    EN_TFTPERR_SHUTDOWN,
    EN_TFTPERR_LOCALMEM,
    EN_TFTPERR_SOCKET,
    EN_TFTPERR_LAST,
};
char *TftpErrMsg(u16 errcode);
int MakeRequestMsg(u8 *buf, int buflen,u16 opcode,char *filename,char *mode,\
                   int blksize,int timeout,int tsize);
int DecodeRequestMsg(u8 *buf, int buflen, char **filename, char **mode,\
                     int *blksize,int *timeout, int *tsize);
int MakeOAckMsg(u8 *buf, int buflen,int blksize,int timeout,int tsize);
int DecodeOAckMsg(u8 *buf, int buflen,int *blksize,int *timeout, int *tsize);
int MakeAckMsg(u8 *buf, int buflen,u16 block);
int DecodeAckMsg(u8 *buf, int buflen,u16 *block);
int MakeErrMsg(u8 *buf,int buflen,u16 errcode,char *msg);
int DecodeErrMsg(u8 *buf, int buflen,tagTftpClient *client);
int MakeDataMsg(tagTftpClient  *client);
int DecodeDataMsg(u8 *buf,int buflen,u16 *block);
bool_t ClientShow(tagTftpClient *client);
int CreateClient(char *filename,char *mode,u16 reqmod,struct sockaddr_in *netaddr,\
                 int oblksize,int otimeout,int otsize,bool_t server,tagTftpClient **tftpclient);
bool_t DeleteClient(tagTftpClient *client);
int TftpTransEngine(tagTftpClient *client);

bool_t TftpWorkSpaceShow(char *param);
bool_t TftpSetWorkSpace(char *path);



#endif
