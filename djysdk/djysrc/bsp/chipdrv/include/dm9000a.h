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
// 文件名     ：dm9000c.h
// 模块描述: DM9000cep网卡驱动，按DJYOS的以太网协议实现要求实现的网卡底层驱动
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 18/09.2014
// =============================================================================

#ifndef __DM9000A_H__
#define __DM9000A_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define DM9KS_ID        0x90000A46

#define DM9000_NCR             0x00
#define DM9000_NSR             0x01
#define DM9000_TCR             0x02
#define DM9000_TSR1            0x03
#define DM9000_TSR2            0x04
#define DM9000_RCR             0x05
#define DM9000_RSR             0x06
#define DM9000_ROCR            0x07
#define DM9000_BPTR            0x08
#define DM9000_FCTR            0x09
#define DM9000_FCR             0x0A
#define DM9000_EPCR            0x0B
#define DM9000_EPAR            0x0C
#define DM9000_EPDRL           0x0D
#define DM9000_EPDRH           0x0E
#define DM9000_WCR             0x0F

#define DM9000_PAR             0x10
#define DM9000_MAR             0x16

#define DM9000_GPCR            0x1e
#define DM9000_GPR             0x1f
#define DM9000_TRPAL           0x22
#define DM9000_TRPAH           0x23
#define DM9000_RWPAL           0x24
#define DM9000_RWPAH           0x25

#define DM9000_VIDL            0x28
#define DM9000_VIDH            0x29
#define DM9000_PIDL            0x2A
#define DM9000_PIDH            0x2B

#define DM9000_CHIPR           0x2C
#define DM9000_SMCR            0x2F

#define DM9000_BUSCR           0x38

#define DM9000_MRCMDX          0xF0
#define DM9000_MRCMD           0xF2
#define DM9000_MRRL            0xF4
#define DM9000_MRRH            0xF5
#define DM9000_MWCMDX          0xF6
#define DM9000_MWCMD           0xF8
#define DM9000_MWRL            0xFA
#define DM9000_MWRH            0xFB
#define DM9000_TXPLL           0xFC
#define DM9000_TXPLH           0xFD
#define DM9000_ISR             0xFE
#define DM9000_IMR             0xFF

// NCR (Network Control Register)
#define NCR_EXT_PHY   (1 << 7)     // 1 ==> external PHY, 0 ==> internal
#define NCR_WAKEEN    (1 << 6)     // enable wakeup events
#define NCR_FCOL      (1 << 4)     // force collision mode (test)
#define NCR_FDX       (1 << 3)     // full duplex (read-only for internal phy)
#define NCR_LBK_NOR   (0 << 1)     // loopback off
#define NCR_LBK_MAC   (1 << 1)     // MAC loopback
#define NCR_LBK_PHY   (2 << 1)     // PHY loopback
#define NCR_RST       (1 << 0)     // Reset (auto-clears after 10us)

// NSR (Network Status Register)
#define NSR_SPEED     (1 << 7)     // 0 = 100Mbps, 1 = 10Mbps
#define NSR_LINKST    (1 << 6)     // link status (1 = okay)
#define NSR_WAKEST    (1 << 5)     // wake status (clear by read)
#define NSR_TX2END    (1 << 3)     // TX packet 2 complete
#define NSR_TX1END    (1 << 2)     // TX packet 1 complete
#define NSR_RXOV      (1 << 1)     // RX overflow

// TCR (TX Control Register)
#define TCR_TJDIS     (1 << 6)     // TX jabber disable
#define TCR_EXCECM    (1 << 5)     // 0 = abort after 15 collisions
#define TCR_PAD_DIS2  (1 << 4)
#define TCR_CRC_DIS2  (1 << 3)
#define TCR_PAD_DIS1  (1 << 2)
#define TCR_CRC_DIS1  (1 << 1)
#define TCR_TXREQ     (1 << 0)

// TSR (TX Status Register)
#define TSR_TJTO      (1 << 7)
#define TSR_LC        (1 << 6)
#define TSR_NC        (1 << 5)
#define TSR_LCOL      (1 << 4)
#define TSR_COL       (1 << 3)
#define TSR_EC        (1 << 2)

// RCR (RX Control Register)
#define RCR_WTDIS     (1 << 6)
#define RCR_DIS_LONG  (1 << 5)
#define RCR_DIS_CRC   (1 << 4)
#define RCR_ALL       (1 << 3)
#define RCR_RUNT      (1 << 2)
#define RCR_PRMSC     (1 << 1)
#define RCR_RXEN      (1 << 0)

// RSR (RX Status Register)
#define RSR_RF        (1 << 7)
#define RSR_MF        (1 << 6)
#define RSR_LCS       (1 << 5)
#define RSR_RWTO      (1 << 4)
#define RSR_PLE       (1 << 3)
#define RSR_AE        (1 << 2)
#define RSR_CE        (1 << 1)
#define RSR_FOE       (1 << 0)

// FCR (Flow Control Register)
#define FCR_TXPO      (1 << 7)
#define FCR_TXPF      (1 << 6)
#define FCR_TXPEN     (1 << 5)
#define FCR_BKPA      (1 << 4)
#define FCR_BKPM      (1 << 3)
#define FCR_RXPS      (1 << 2)
#define FCR_RXPCS     (1 << 1)
#define FCR_FLCE      (1 << 0)

// EPCR (EEPROM & PHY Control Register)
#define EPCR_REEP     (1 << 5)
#define EPCR_WEP      (1 << 4)
#define EPCR_EPOS     (1 << 3)
#define EPCR_ERPRR    (1 << 2)
#define EPCR_ERPRW    (1 << 1)
#define EPCR_ERRE     (1 << 0)

// WCR (Wakeup Control Register)
#define WCR_LINKEN    (1 << 5)
#define WCR_SAMPLEEN  (1 << 4)
#define WCR_MAGICEN   (1 << 3)
#define WCR_LINKST    (1 << 2)
#define WCR_SAMPLEST  (1 << 1)
#define WCR_MAGIGST   (1 << 0)

// SMCR (Special Mode Control Register)
#define SMCR_SM_EN    (1 << 7)
#define SMCR_FLC      (1 << 2)
#define SMCR_FB1      (1 << 1)
#define SMCR_FB0      (1 << 0)

// ISR (Interrupt Status Register)
#define ISR_IOMODE_16 (0 << 6)
#define ISR_IOMODE_32 (1 << 6)
#define ISR_IOMODE_8  (2 << 6)
#define ISR_ROOS      (1 << 3)
#define ISR_ROS       (1 << 2)
#define ISR_PTS       (1 << 1)
#define ISR_PRS       (1 << 0)

// IMR (Interrupt Mask Register)
#define IMR_PAR       (1 << 7)
#define IMR_ROOM      (1 << 3)
#define IMR_ROM       (1 << 2)
#define IMR_PTM       (1 << 1)
#define IMR_PRM       (1 << 0)

//#define DM9000A_FLOW_CONTROL
/* DM9000 REGISTER LIST */
#define DM9000_REG_NCR        0x00
#define DM9000_REG_NSR        0x01
#define DM9000_REG_TCR        0x02
#define DM9000_REG_TSR1       0x03
#define DM9000_REG_TSR2       0x04
#define DM9000_REG_RCR        0x05
#define DM9000_REG_RSR        0x06
#define DM9000_REG_ROCR       0x07
#define DM9000_REG_BPTR       0x08
#define DM9000_REG_FCTR       0x09
#define DM9000_REG_FCR        0x0A
#define DM9000_REG_EPCR       0x0B
#define DM9000_REG_EPAR       0x0C
#define DM9000_REG_EPDRL      0x0D
#define DM9000_REG_EPDRH      0x0E
#define DM9000_REG_WAR        0x0F
#define DM9000_REG_PAR        0x10
#define DM9000_REG_MAR        0x16
#define DM9000_REG_GPCR       0x1E
#define DM9000_REG_GPR        0x1F
#define DM9000_REG_VID_L      0x28
#define DM9000_REG_VID_H      0x29
#define DM9000_REG_PID_L      0x2A
#define DM9000_REG_PID_H      0x2B
#define DM9000_REG_CHIPR      0x2C
#define DM9000_REG_TCR2       0x2D
#define DM9000_REG_OTCR       0x2E
#define DM9000_REG_SMCR       0x2F
#define DM9000_REG_ETXCSR     0x30
#define DM9000_REG_TCSCR      0x31
#define DM9000_REG_RCSCSR     0x32
#define DM9000_REG_MRCMDX     0xF0
#define DM9000_REG_MRCMD      0xF2
#define DM9000_REG_MRRL       0xF4
#define DM9000_REG_MRRH       0xF5
#define DM9000_REG_MWCMDX     0xF6
#define DM9000_REG_MWCMD      0xF8
#define DM9000_REG_MWRL       0xFA
#define DM9000_REG_MWRH       0xFB
#define DM9000_REG_TXPLL      0xFC
#define DM9000_REG_TXPLH      0xFD
#define DM9000_REG_ISR        0xFE
#define DM9000_REG_IMR        0xFF

/* 相关宏设置 */
#define DM9000A_ID_OK       0x0A469000

#define DM9000_BYTE_MODE      0x01
#define DM9000_WORD_MODE      0x00
#define DM9000_PHY            0x40
#define DM9000_PKT_RDY        0x01
#define DM9000_PKT_NORDY      0x00
#define DM9000_REG_RESET      0x03

#define DM9000_RX_INTR        0x01                /* 接收中断判断 bit */
#define DM9000_TX_INTR        0x02                /* 传送中断判断 bit */
#define DM9000_OVERFLOW_INTR  0x04                /* 内存溢出中断判断 bit */
#define DM9000_LINK_CHANG     0x20                /* 连接变动中断判断 bit */

#define DM9000_PHY_ON        0x00                /* 设定 PHY 开启 */
#define DM9000_PHY_OFF        0x01                /* 设定 PHY 关闭 */
#define DM9000_RCR_SET        0x31                /* 设定 接收功能 (不收 CRC 及 超长包) */
#define DM9000_TCR_SET        0x01                /* 设定 传送功能 */
#define DM9000_RCR_OFF        0x00                /* 设定 接收功能关关闭设置 */
#define DM9000_BPTR_SET       0x37                /* 设定 Back Pressure 条件设置 */
#define DM9000_FCTR_SET       0x38                /* 设定 Flow Control 条件设置 */
#define DM9000_TCR2_SET       0x80                /* 设置 LED 显示模式 */
#define DM9000_OTCR_SET       0x80                /* 设置 DM9000 工作频率 0x80 = 100Mhz */
#define DM9000_ETXCSR_SET     0x83                /* 设置 Early Tramsmit 条件设置 */
#define DM9000_FCR_SET        0x28                /* 开启 网络流控功能设置 */
#define DM9000_TCSCR_SET      0x07                /* 设定 CHECKSUM 传送运算 设置 */
#define DM9000_RCSCSR_SET     0x03                /* 设定 CHECKSUM 接收检查 设置 */
#define DM9000_IMR_SET        0x81                /* 设定 启用中断使能 条件设置 */
#define DM9000_IMR_OFF        0x80                /* 设定 关闭中断使能 条件设置 */
// =============================================================================

#define DM9KS_VID_L 0x28
#define DM9KS_VID_H 0x29
#define DM9KS_PID_L 0x2A
#define DM9KS_PID_H 0x2B

#define EPCR_EPOS_PHY		(1<<3)
#define EPCR_EPOS_EE		(0<<3)
#define EPCR_ERPRR		(1<<2)
#define EPCR_ERPRW		(1<<1)
#define EPCR_ERRE		(1<<0)

#define FCTR_HWOT(ot)	(( ot & 0xf ) << 4 )
#define FCTR_LWOT(ot)	( ot & 0xf )

#define BPTR_BPHW(x)	((x) << 4)
#define BPTR_JPT_200US		(0x07)
#define BPTR_JPT_600US		(0x0f)


typedef struct Dm9000Para
{
	char *devname;
	u8   *mac;
    u16 irqno;
    bool_t (*clearextint)(u32 irqno);
    u16 *cmdaddr;
    u16 *dataddr;
}tagDm9000Para;

bool_t Dm9000Install(tagDm9000Para *para);

#ifdef __cplusplus
}
#endif
#endif /* #ifndef __DM9000A_H__ */
