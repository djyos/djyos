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

#ifndef DM9000A_H_
#define DM9000A_H_

#define DM9000A_INDEX	0x6C000000
#define DM9000A_DATA	0x6C000002

#define DM9000A_VID_PID	0x0A469000	// Vendor ID and Produuct ID
// DM9000的寄存器
#define DM9000A_NCR		0x00
#define DM9000A_NSR		0x01
// TX/RX Control & Status
#define DM9000A_TCR		0x02
#define DM9000A_TSR1	0x03
#define DM9000A_TSR2	0x04
#define DM9000A_RCR		0x05
#define DM9000A_RSR		0x06
#define DM9000A_ROCR	0x07
#define DM9000A_BPTR	0x08
#define DM9000A_FCTR	0x09
#define DM9000A_FCR		0x0A
// EEPROM & PHY
#define DM9000A_EPCR	0x0B
#define DM9000A_EPAR	0x0C
#define DM9000A_EPDRL	0x0D
#define DM9000A_EPDRH	0x0E

#define DM9000A_WCR		0x0F
// Physical Address Control
#define DM9000A_PAR0	0x10
#define DM9000A_PAR1	0x11
#define DM9000A_PAR2	0x12
#define DM9000A_PAR3	0x13
#define DM9000A_PAR4	0x14
#define DM9000A_PAR5	0x15
// Multicast Address Control
#define DM9000A_MAR0	0x16
#define DM9000A_MAR1	0x17
#define DM9000A_MAR2	0x18
#define DM9000A_MAR3	0x19
#define DM9000A_MAR4	0x1A
#define DM9000A_MAR5	0x1B
#define DM9000A_MAR6	0x1C
#define DM9000A_MAR7	0x1D
// General Purpose Control
#define DM9000A_GPCR	0x1E
#define DM9000A_GPR		0x1F
// TX/RX SRAM Read/Write Pointer Address
#define DM9000A_TRPAL	0x22
#define DM9000A_TRPAH	0x23
#define DM9000A_RWPAL	0x24
#define DM9000A_RWPAH	0x25
// Chip ID
#define DM9000A_VIDL	0x28
#define DM9000A_VIDH	0x29
#define DM9000A_PIDL	0x2A
#define DM9000A_PIDH	0x2B
#define DM9000A_CHIPR	0x2C
#define DM9000A_TCR2	0x2D
#define DM9000A_OCR		0x2E
#define DM9000A_SMCR	0x2F
#define DM9000A_ETXCSR	0x30
#define DM9000A_TCSCR	0x31
#define DM9000A_RCSCCR	0x32
#define DM9000A_MPAR	0x33
#define DM9000A_LEDCR	0x34
#define DM9000A_BUSR	0x38
#define DM9000A_INTCR	0x39

#define DM9000A_SCCR	0x50
#define DM9000A_RSCCR	0x51
// Memory Data Read
#define DM9000A_MRCMDX	0xF0
#define DM9000A_MRCMDX1	0xF1
#define DM9000A_MRCMD	0xF2
#define DM9000A_MDRRL	0xF4
#define DM9000A_MDRRH	0xF5
// Memory Data Write
#define DM9000A_MWCMDX	0xF6
#define DM9000A_MWCMD	0xF8
#define DM9000A_MDWRL	0xFA
#define DM9000A_MDWRH	0xFB
#define DM9000A_TXPLL	0xFC
#define DM9000A_TXPLH	0xFD
// Interrupt
#define DM9000A_ISR		0xFE
#define DM9000A_IMR		0xFF

enum DM9000A_IO_mode { ENUM_DM9000A_IO_16BIT = 0, ENUM_DM9000A_IO_32BIT = 1,
	ENUM_DM9000A_IO_8BIT = 2};

#define DM9000A_PKT_RDY		0x01	/* Packet ready to receive */
#define DM9000A_PKT_ERR		0x02
#define DM9000A_PKT_MAX		1536	/* Received packet max size */

extern u8 enet_state_nic_init(struct _enet_state *pt_enet_st);
extern struct enet_rcv_packet* dm9000a_dump_data(u32 len);
extern void dm9000a_reset_to_new(void);


#endif /* DM9000A_H_ */
