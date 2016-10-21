
#ifndef __PHY_88E6060_H__
#define __PHY_88E6060_H__

#include "stdint.h"
#include "cpu_peri_gmac.h"
//IEEE defined Registers (8846060)
#define PHY_BMCR        0x0   // Basic Mode Control Register
#define PHY_BMSR        0x1   // Basic Mode Status Register
#define PHY_PHYID1R     0x2   // PHY Identifier Register 1
#define PHY_PHYID2R     0x3   // PHY Identifier Register 2
#define PHY_ANAR        0x4   // Auto_Negotiation Advertisement Register
#define PHY_ANLPAR      0x5   // Auto_negotiation Link Partner Ability Register
#define PHY_ANER        0x6   // Auto-negotiation Expansion Register
#define PHY_ANNPR       0x7   // Auto-negotiation Next Page Register
#define PHY_ANLPNPAR    0x8   // Auto_negotiation Link Partner Next Page Ability Register

#define PHY_SPC1R		0X10  // Specific Control Register 1
#define PHY_SPS1		0x11  // Specific Status Register
#define PHY_IER			0x12  // Interrupt Enable Register
#define PHY_ISR			0x13  // Interrupt Status Register
#define PHY_IPR			0x14  // Interrupt Port Summary
#define PHY_REC			0x15  // Receive Error Counter


// PHY ID Identifier Register
#define PHY_LSB_MASK           (0X3FF)
// definitions: MII_PHYID1
#define PHY_OUI_MSB            0x0141
// definitions: MII_PHYID2
#define PHY_OUI_LSB            (0x3 << 10)

// Basic Mode Control Register (BMCR)
// Bit definitions: MII_BMCR
#define PHY_RESET             (1 << 15) // 1= Software Reset; 0=Normal Operation
#define PHY_LOOPBACK          (1 << 14) // 1=loopback Enabled; 0=Normal Operation
#define PHY_SPEED_SELECT_LSB  (1 << 13) // 0,1=100Mbps; 0,0=10Mbps
#define PHY_AUTONEG           (1 << 12) // Auto-negotiation Enable
#define PHY_POWER_DOWN        (1 << 11) // 1=Power down 0=Normal operation
#define PHY_ISOLATE           (1 << 10) // 1 = Isolates 0 = Normal operation
#define PHY_RESTART_AUTONEG   (1 << 9)  // 1 = Restart auto-negotiation 0 = Normal operation
#define PHY_DUPLEX_MODE       (1 << 8)  // 1 = Full duplex operation 0 = Normal operation
//      Reserved                7        // Read as 0, ignore on write
#define PHY_SPEED_SELECT_MSB  (1 << 6)  //
//      Reserved                5 to 0   // Read as 0, ignore on write


// Basic Mode Status Register (BMSR)
// Bit definitions: MII_BMSR
#define PHY_100BASE_T4        (1 << 15) // 100BASE-T4 Capable
#define PHY_100BASE_TX_FD     (1 << 14) // 100BASE-TX Full Duplex Capable
#define PHY_100BASE_T4_HD     (1 << 13) // 100BASE-TX Half Duplex Capable
#define PHY_10BASE_T_FD       (1 << 12) // 10BASE-T Full Duplex Capable
#define PHY_10BASE_T_HD       (1 << 11) // 10BASE-T Half Duplex Capable
//      Reserved                10 to 9  // Read as 0, ignore on write
#define PHY_EXTEND_STATUS     (1 << 8)  // 1 = Extend Status Information In Reg 15
//      Reserved                7
#define PHY_MF_PREAMB_SUPPR   (1 << 6)  // MII Frame Preamble Suppression
#define PHY_AUTONEG_COMP      (1 << 5)  // Auto-negotiation Complete
#define PHY_REMOTE_FAULT      (1 << 4)  // Remote Fault
#define PHY_AUTONEG_ABILITY   (1 << 3)  // Auto Configuration Ability
#define PHY_LINK_STATUS       (1 << 2)  // Link Status
#define PHY_JABBER_DETECT     (1 << 1)  // Jabber Detect
#define PHY_EXTEND_CAPAB      (1 << 0)  // Extended Capability

// Auto-negotiation Advertisement Register (ANAR)
// Auto-negotiation Link Partner Ability Register (ANLPAR)
// Bit definitions: MII_ANAR, MII_ANLPAR
#define PHY_NP               (1 << 15) // Next page Indication
//      Reserved               7
#define PHY_RF               (1 << 13) // Remote Fault
//      Reserved               12       // Write as 0, ignore on read
#define PHY_PAUSE_MASK       (3 << 11) // 0,0 = No Pause 1,0 = Asymmetric Pause(link partner)
                                        // 0,1 = Symmetric Pause 1,1 = Symmetric&Asymmetric Pause(local device)
#define PHY_T4               (1 << 9)  // 100BASE-T4 Support
#define PHY_TX_FDX           (1 << 8)  // 100BASE-TX Full Duplex Support
#define PHY_TX_HDX           (1 << 7)  // 100BASE-TX Support
#define PHY_10_FDX           (1 << 6)  // 10BASE-T Full Duplex Support
#define PHY_10_HDX           (1 << 5)  // 10BASE-T Support
//      Selector                 4 to 0   // Protocol Selection Bits
#define PHY_AN_IEEE_802_3      0x00001

// Phy Interrupt Enable Register
#define PHY_IER_LINK         (1 << 10)	// Link Status Changed Interrupt Enable

#define PHY_ISR_LINK		 (1 << 10)  // Link Status Changed

uint8_t GMACB_InitPhy(GMacb *pMacb,	uint32_t mck);
uint8_t GMACB_AutoNegotiate(GMacb *pMacb);
void GMACB_Init(GMacb *pMacb, sGmacd *pGmacd, uint8_t phyAddress);
uint8_t GMACB_ResetPhy(GMacb *pMacb);
bool_t GMACB_PhyIsLinkUp(GMacb *pMacb);

#endif /* SRC_USER_PHY_88E6060_H_ */
