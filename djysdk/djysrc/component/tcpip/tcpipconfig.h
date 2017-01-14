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


#ifndef __TCPIP_CONFIG_H
#define __TCPIP_CONFIG_H

//*******************************PKGMEM***************************************//
extern const u32 gNetPkgMemSize         ; //the stack package memory size

//*******************************IP*******************************************//
extern const u32    gIpRcvTaskPrior        ; //the ip processor task prior
extern const u32    gIpRcvTaskStack        ; //ip receive task stack size
extern const bool_t gEnableIpAssemble      ; //ipv4 could receive the fragment package
extern const bool_t gEnableIpFragment      ; //ipv4 could send fragment package
//*******************************ARP******************************************//
extern const u32  gArpItemHashLen       ;  //determines the arp tab hash len
extern const u32  gArpTaskPrior         ;  //the arp processor task
extern const u32  gArpTaskStack         ;  //the arp task stack size

//*******************************TPL******************************************//
extern const u32  gTplProtoNum          ; //the transmit protocol number supported

//*******************************UDP******************************************//
extern const bool_t   gUseUdp           ; //use udp, and the icmp must be open
extern const u32  gUdpSockNum           ; //you could create mostly 10 udpv4 socket at the same time

//*******************************TCP******************************************//
extern const bool_t   gUseTcp           ; //use tcp
extern const bool_t   gEnableTcpReoder  ; //enable the tcp reorder or not
extern const u32  gTcpClientNum         ; //tcp maxi client
extern const u32  gTcpServerNum         ; //tcp maxi server
extern const u32  gTcpTickTaskPrior     ; //tcp tick task prior
extern const u32  gTcpTickTaskStack     ; //the tcp tick task stack size
extern const int  gTcpSndBufSizeDefault ; //the tcp buf size default
extern const int  gTcpRcvBufSizeDefault ; //the tcp buf size default
extern const u16  gTcpMssDefault        ; //the tcp mss default used to


//*******************************SOCKET***************************************//
extern const u32  gSockNum              ;  //Determine how many socket you could create

//*******************************TELNET***************************************//
extern const bool_t  gUseTelnet         ;  //load the telnet module or not
extern const bool_t  gUseTelnetServer   ;  //enable the telnet server or not
extern const bool_t  gUseTelnetClient   ;  //enable the telnet client or not
extern const u32     gTelnetAcceptStack ;  //telnet accept task stack size
extern const u32     gTelnetProcessStack;  //telnet processor task stack size,if use fs,please configure it more
extern const u32     gTelnetAcceptPrior ;  //telnet accept task prior
extern const u32     gTelnetProcessPrior;  //telnet processor task prior

//*******************************FTP******************************************//
extern const bool_t  gUseFtp;              //load the ftp module or not
extern const bool_t  gUseFtpServer      ;  //enable the ftp server or not
extern const bool_t  gUseFtpClient      ;  //enable the ftp server or not
extern const u32     gFtpAcceptStack    ;  //the ftp accept task stack size
extern const u32     gFtpProcessStack   ;  //the ftp processor task stack,use fs,configure more
extern const u32     gFtpAcceptPrior    ;  //ftp accept task prior
extern const u32     gFtpProcessPrior   ;  //ftp processor task prior

//******************************TFTP******************************************//
extern const bool_t  gUseTftp           ;  //load the tftp module or not
extern const bool_t  gUseTftpServer     ;  //enable the tftp server or not
extern const bool_t  gUseTftpClient     ;  //enable the tftp server or not
extern const char   *pTftpWSDefault     ;  //set the default tftp work space
extern const u32     gTftpServerStack   ;  //tftp server stack size,use fs, pay more
extern const u32     gTftpServerPrior   ;  //tftp server prior


//*******************************DHCP SERVER**********************************//
extern const bool_t   gUseDhcp;             //load the dhcp module or not
extern const bool_t   gUseDhcpClient;       //enable the dhcp client or not
extern const bool_t   gUseDhcpServer;       //enable the dhcp server or not
extern const u32      gDhcpRenewTime ;      //one hour
extern const u32      gDhcpRebindTime ;     //one hour
extern const u32      gDhcpLeaseTime  ;     //one hour
extern const u32      gDhcpServerIpNum ;    //64
extern const char    *pDhcpServerIp   ;     //DHCP SERVER IP
extern const char    *pDhcpRouterIp   ;     //ROUTER SERVER IP
extern const char    *pDhcpSubnetMask ;     //USED TO MASK THE IP
extern const char    *pDhcpDns        ;     //DNSSERVER
extern const char    *pDhcpDomainName ;     //domain name

#endif /* __TCPIP_CONFIG_H */
