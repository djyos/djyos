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
#include <stdint.h>
#include <stddef.h>
//*******************************PKGMEM***************************************//
const u32 gNetPkgMemSize         = 0x4000 ;

//*******************************IP***************************************//
const u32 gIpRcvTaskPrior        = 125;   ; //ip receive task prior
const u32 gIpRcvTaskStack        = 0x800  ; //ip receive task stack size
const bool_t gEnableIpAssemble   = false  ; //ipv4 could receive the fragment package
const bool_t gEnableIpFragment   = false  ; //ipv4 could send fragment package

//*******************************ARP******************************************//
const u32  gArpItemHashLen       = 32     ;  //determines the arp tab hash len
const u32  gArpTaskPrior         = 200    ;  //the arp processor task
const u32  gArpTaskStack         = 0x400  ;  //the arp task stack size

//*******************************TPL******************************************//
const u32  gTplProtoNum          = 5      ; //the transmit protocol number supported

//*******************************UDP******************************************//
const bool_t   gUseUdp           = true   ; //use udp, and the icmp must be open
const u32  gUdpSockNum           = 10     ; //you could create mostly 10 udpv4 socket at the same time

//*******************************TCP******************************************//
const bool_t   gUseTcp           = true   ; //use the tcp
const bool_t   gEnableTcpReoder  = false  ; //enable the tcp reorder or not,if resource is enough,please enable it

const u32  gTcpClientNum         = 10     ; //the max tcp client num
const u32  gTcpServerNum         = 5      ; // the max tcp server num
const u32  gTcpTickTaskPrior     = 135    ; // the tcp tick task prior
const u32  gTcpTickTaskStack     = 0x400  ; // the tcp tick task stack size
const int  gTcpSndBufSizeDefault = 0x800  ; //the tcp send buf size default
const int  gTcpRcvBufSizeDefault = 0x800  ; //the tcp receive buf size default
const u16  gTcpMssDefault        = 1460   ; //the tcp mss default used to

//*******************************SOCKET***************************************//
const u32  gSockNum              = 10     ;  //Determine how many socket you could create

//*******************************TELNET***************************************//
const bool_t  gUseTelnet         = true   ;  //load the telnet module or not
const bool_t  gUseTelnetServer   = true   ;  //enable the telnet server or not
const bool_t  gUseTelnetClient   = false  ;  //enable the telnet client or not
const u32     gTelnetAcceptStack = 0x800  ;  //telnet accept task stack size
const u32     gTelnetProcessStack= 0x1000 ;  //telnet processor task stack size,if use fs,please configure it more
const u32     gTelnetAcceptPrior = 200    ;  //telnet accept task prior
const u32     gTelnetProcessPrior= 200    ;  //telnet processor task prior

//*******************************FTP******************************************//
const bool_t  gUseFtp            = true   ;  //load the ftp module or not
const bool_t  gUseFtpServer      = true   ;  //enable the ftp server or not
const bool_t  gUseFtpClient      = false  ;  //enable the ftp server or not
const u32     gFtpAcceptStack    = 0x1000  ;  //the ftp accept task stack size
const u32     gFtpProcessStack   = 0x1000 ;  //the ftp processor task stack,use fs,configure more
const u32     gFtpAcceptPrior    = 200    ;  //ftp accept task prior
const u32     gFtpProcessPrior   = 200    ;  //ftp processor task prior

//******************************TFTP******************************************//
const bool_t  gUseTftp           = true   ;  //load the tftp module or not
const bool_t  gUseTftpServer     = true   ;  //enable the tftp server or not
const bool_t  gUseTftpClient     = true   ;  //enable the tftp server or not
const char   *pTftpWSDefault     = "/efs/";  //set the default tftp work space
const u32     gTftpServerStack   = 0x1000 ;  //tftp server stack size,use fs, pay more
const u32     gTftpServerPrior   = 200    ;  //tftp server prior

//*******************************DHCP SERVER**********************************//
const bool_t   gUseDhcp          = true   ;  //load the dhcp module or not
const bool_t   gUseDhcpClient    = true   ;  //enable the dhcp client or not
const bool_t   gUseDhcpServer    = false  ;  //enable the dhcp server or not
const u32      gDhcpRenewTime    = 3600;            //one hour
const u32      gDhcpRebindTime   = 3600;            //one hour
const u32      gDhcpLeaseTime    = 3600;            //one hour
const u32      gDhcpServerIpNum  = 0x40;            //64
const u32      gDhcpIpMapBytes   = 0x08;            //which defines the bitmap lenth
const char    *pDhcpServerIp     = "192.168.0.100"; //DHCP SERVER IP
const char    *pDhcpRouterIp     = "192.168.0.100"; //ROUTER SERVER IP
const char    *pDhcpSubnetMask   = "255.255.255.0"; //USED TO MASK THE IP
const char    *pDhcpDns          = "192.168.0.100"; //DNSSERVER
const char    *pDhcpDomainName   = "www.djyos.com"; //domain name


