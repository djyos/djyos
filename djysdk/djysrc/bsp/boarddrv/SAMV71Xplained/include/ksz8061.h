// ---------------------------------------------
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
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：ksz8051.c
// 模块描述: PHY芯片驱动部分头文件相关的定义
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 26/11.2015
// =============================================================================

#ifndef __GMII_DEFINE_H__
#define __GMII_DEFINE_H__

#include "cpu_peri_gmac.h"

//IEEE defined Registers
#define GMII_BMCR        0x0   // Basic Mode Control Register
#define GMII_BMSR        0x1   // Basic Mode Status Register
#define GMII_PHYID1R     0x2   // PHY Identifier Register 1
#define GMII_PHYID2R     0x3   // PHY Identifier Register 2
#define GMII_ANAR        0x4   // Auto_Negotiation Advertisement Register
#define GMII_ANLPAR      0x5   // Auto_negotiation Link Partner Ability Register
#define GMII_ANER        0x6   // Auto-negotiation Expansion Register
#define GMII_ANNPR       0x7   // Auto-negotiation Next Page Register
#define GMII_ANLPNPAR    0x8   // Auto_negotiation Link Partner Next Page Ability Register
#define GMII_AFEC0R      0x11  // AFE Control 0 Register
#define GMII_AFEC3R      0x14  // AFE Control 3 Register
#define GMII_RXERCR      0x15  // RXER Counter Register
#define GMII_OMSSR       0x17  // Operation Mode Strap Status Register
#define GMII_ECR         0x18  // Expanded Control Register
#define GMII_ICSR        0x1B  // Interrupt Control/Status Register
#define GMII_FC          0x1C  // Function Control
#define GMII_LCSR        0x1D  // LinkMD?Control/Status Register
#define GMII_PC1R        0x1E  // PHY Control 1 Register
#define GMII_PC2R        0x1F  // PHY Control 2 Register


// PHY ID Identifier Register
#define GMII_LSB_MASK           0x0U
// definitions: MII_PHYID1
#define GMII_OUI_MSB            0x0022
// definitions: MII_PHYID2
#define GMII_OUI_LSB            0x1572          // KSZ8061 PHY Id2

// Basic Mode Control Register (BMCR)
// Bit definitions: MII_BMCR
#define GMII_RESET             (1 << 15) // 1= Software Reset; 0=Normal Operation
#define GMII_LOOPBACK          (1 << 14) // 1=loopback Enabled; 0=Normal Operation
#define GMII_SPEED_SELECT_LSB  (1 << 13) // 1,0=1000Mbps 0,1=100Mbps; 0,0=10Mbps
#define GMII_AUTONEG           (1 << 12) // Auto-negotiation Enable
#define GMII_POWER_DOWN        (1 << 11) // 1=Power down 0=Normal operation
#define GMII_ISOLATE           (1 << 10) // 1 = Isolates 0 = Normal operation
#define GMII_RESTART_AUTONEG   (1 << 9)  // 1 = Restart auto-negotiation 0 = Normal operation
#define GMII_DUPLEX_MODE       (1 << 8)  // 1 = Full duplex operation 0 = Normal operation
//      Reserved                7        // Read as 0, ignore on write
#define GMII_SPEED_SELECT_MSB  (1 << 6)  //
//      Reserved                5 to 0   // Read as 0, ignore on write


// Basic Mode Status Register (BMSR)
// Bit definitions: MII_BMSR
#define GMII_100BASE_T4        (1 << 15) // 100BASE-T4 Capable
#define GMII_100BASE_TX_FD     (1 << 14) // 100BASE-TX Full Duplex Capable
#define GMII_100BASE_T4_HD     (1 << 13) // 100BASE-TX Half Duplex Capable
#define GMII_10BASE_T_FD       (1 << 12) // 10BASE-T Full Duplex Capable
#define GMII_10BASE_T_HD       (1 << 11) // 10BASE-T Half Duplex Capable
//      Reserved                10 to 9  // Read as 0, ignore on write
#define GMII_EXTEND_STATUS     (1 << 8)  // 1 = Extend Status Information In Reg 15
//      Reserved                7
#define GMII_MF_PREAMB_SUPPR   (1 << 6)  // MII Frame Preamble Suppression
#define GMII_AUTONEG_COMP      (1 << 5)  // Auto-negotiation Complete
#define GMII_REMOTE_FAULT      (1 << 4)  // Remote Fault
#define GMII_AUTONEG_ABILITY   (1 << 3)  // Auto Configuration Ability
#define GMII_LINK_STATUS       (1 << 2)  // Link Status
#define GMII_JABBER_DETECT     (1 << 1)  // Jabber Detect
#define GMII_EXTEND_CAPAB      (1 << 0)  // Extended Capability

// Auto-negotiation Advertisement Register (ANAR)
// Auto-negotiation Link Partner Ability Register (ANLPAR)
// Bit definitions: MII_ANAR, MII_ANLPAR
#define GMII_NP               (1 << 15) // Next page Indication
//      Reserved               7
#define GMII_RF               (1 << 13) // Remote Fault
//      Reserved               12       // Write as 0, ignore on read
#define GMII_PAUSE_MASK       (3 << 11) // 0,0 = No Pause 1,0 = Asymmetric Pause(link partner)
                                        // 0,1 = Symmetric Pause 1,1 = Symmetric&Asymmetric Pause(local device)
#define GMII_T4               (1 << 9)  // 100BASE-T4 Support
#define GMII_TX_FDX           (1 << 8)  // 100BASE-TX Full Duplex Support
#define GMII_TX_HDX           (1 << 7)  // 100BASE-TX Support
#define GMII_10_FDX           (1 << 6)  // 10BASE-T Full Duplex Support
#define GMII_10_HDX           (1 << 5)  // 10BASE-T Support
//      Selector                 4 to 0   // Protocol Selection Bits
#define GMII_AN_IEEE_802_3      0x00001

/** The reset length setting for external reset configuration */
#define GMACB_RESET_LENGTH         0xD

typedef struct _GMacb
{
	/**< Driver */
	sGmacd *pGmacd;
	/** The retry & timeout settings */
	uint32_t retryMax;
	/** PHY address ( pre-defined by pins on reset ) */
	uint8_t phyAddress;
} GMacb;

/*---------------------------------------------------------------------------
 *         Exported functions
 *---------------------------------------------------------------------------*/
extern void GMACB_SetupTimeout(GMacb *pMacb, uint32_t toMax);

extern void GMACB_Init(GMacb *pMacb, sGmacd *pGmacd, uint8_t phyAddress);

extern uint8_t GMACB_InitPhy(
				GMacb *pMacb,
				uint32_t mck
//				const Pin *pResetPins,
//				uint32_t nbResetPins,
//				const Pin *pEmacPins,
//				uint32_t nbEmacPins
				);

extern uint8_t GMACB_AutoNegotiate(GMacb *pMacb);

extern uint8_t GMACB_GetLinkSpeed(GMacb *pMacb, uint8_t applySettings);

extern uint8_t GMACB_Send(GMacb *pMacb, void *pBuffer, uint32_t size);

extern uint32_t GMACB_Poll(GMacb *pMacb, uint8_t *pBuffer, uint32_t size);

extern void GMACB_DumpRegisters(GMacb *pMacb);

extern uint8_t GMACB_ResetPhy(GMacb *pMacb);

extern bool_t GMACB_PhyIsLinkUp(GMacb *pMacb);

#endif // #ifndef _GMACB_H

