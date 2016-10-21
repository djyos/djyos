/**
 * \file
 *
 * \brief KSZ8051MNL (Ethernet PHY) driver for SAM.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "stdint.h"
#include "stddef.h"
#include "ksz8051mnl.h"
#include "cpu_peri.h"
//#include "conf_eth.h"

/* ----------------------------------------------------------------------------- */
/*         Headers */
/* ----------------------------------------------------------------------------- */
#ifdef DEBUG
#define TRACE_INFO  printk
#define TRACE_ERROR printk
#define TRACE_DEBUG	printk
#else
#define TRACE_INFO(...)
#define TRACE_ERROR(...)
#define TRACE_DEBUG(...)
#endif
/* ----------------------------------------------------------------------------- */
/*         Definitions */
/* ----------------------------------------------------------------------------- */

//void ksz8051mnl_SetLocalLoopBack(GMacb *pMacb);
//void ksz8051mnl_SetRemoteLoopBack(GMacb *pMacb);

/* Default max retry count */
#define ksz8051mnl_RETRY_MAX            300000

/**
 * \brief Dump all the useful registers
 * \param pMacb Pointer to the MACB instance
 */
static void ksz8051mnl_dump_registers(GMacb *pMacb)
{
	unsigned char phyAddress;
	unsigned int retryMax;
	unsigned int value;

	GMAC_MdioEnable();
	phyAddress = pMacb->phyAddress;
	retryMax = pMacb->retryMax;

	GMAC_ReadPhy(phyAddress, GMII_BMCR, &value, retryMax);
	TRACE_INFO(" _GBMCR   : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_BMSR, &value, retryMax);
	TRACE_INFO(" _GBMSR   : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_ANAR, &value, retryMax);
	TRACE_INFO(" _ANAR   : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_ANLPAR, &value, retryMax);
	TRACE_INFO(" _ANLPAR : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_ANER, &value, retryMax);
	TRACE_INFO(" _ANER   : 0x%X\n\r", value);

	GMAC_ReadPhy(phyAddress, GMII_ANNPR, &value, retryMax);
	TRACE_INFO(" _ANNPR  : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_ANLPNPAR, &value, retryMax);
	TRACE_INFO(" _ANLPNPR: 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_AFECR1, &value, retryMax);
	TRACE_INFO(" _AFECR1 : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_RXERCR, &value, retryMax);
	TRACE_INFO(" _RXERCR : 0x%X\n\r", value);

	GMAC_ReadPhy(phyAddress, GMII_OMSOR, &value, retryMax);
	TRACE_INFO(" _OMSOR  : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_OMSSR, &value, retryMax);
	TRACE_INFO(" _OMSSR  : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_ECR, &value, retryMax);
	TRACE_INFO(" __ECR   : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_ICSR, &value, retryMax);
	TRACE_INFO(" _ICSR   : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_LCSR, &value, retryMax);
	TRACE_INFO(" _LCSR   : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_PCR1, &value, retryMax);
	TRACE_INFO(" _PCR1   : 0x%X\n\r", value);
	GMAC_ReadPhy(phyAddress, GMII_PCR2, &value, retryMax);
	TRACE_INFO(" _PCR2   : 0x%X\n\r", value);

	GMAC_MdioDisable();
}

/**
 * \brief Set local loopback
 * \param pMacb Pointer to the MACB instance
 */
void ksz8051mnl_set_local_loopBack(GMacb *pMacb)
{
	unsigned char phyAddress;
	unsigned int retryMax;
	unsigned int value;

	GMAC_MdioEnable();
	phyAddress = pMacb->phyAddress;
	retryMax = pMacb->retryMax;

	GMAC_ReadPhy(phyAddress, GMII_BMCR, &value, retryMax);
	value |= GMII_LOOPBACK;
	GMAC_WritePhy(phyAddress, GMII_BMCR, value, retryMax);
}

/**
 * \brief Set remote loopback
 * \param pMacb Pointer to the MACB instance
 */
void ksz8051mnl_set_remote_loopBack(GMacb *pMacb)
{
	unsigned char phyAddress;
	unsigned int retryMax;
	unsigned int value;

	GMAC_MdioEnable();
	phyAddress = pMacb->phyAddress;
	retryMax = pMacb->retryMax;
	GMAC_ReadPhy(phyAddress, GMII_PCR2, &value, retryMax);
	value |= 1 << 2; /* Remote (analog) loop back is enable */
	GMAC_WritePhy(phyAddress, GMII_PCR2, value, retryMax);
}

/**
 * \brief Find a valid PHY Address ( from 0 to 31 ).
 * Check BMSR register ( not 0 nor 0xFFFF )
 * \param pMacb          Pointer to the MACB instance
 * \return 0xFF when no valid PHY Address found.
 */
static unsigned char ksz8051mnl_find_valid_phy(GMacb *pMacb)
{
	unsigned int retryMax;
	unsigned int value;
	unsigned char phyAddress;
	unsigned char rc = 0xFF;
	unsigned char cnt;

	TRACE_DEBUG("ksz8051mnl_FindValidPhy\n\r");

	GMAC_MdioEnable();
	phyAddress = pMacb->phyAddress;
	retryMax = pMacb->retryMax;

	/* Check current phyAddress */
	if (GMAC_ReadPhy(phyAddress, GMII_PHYID1, &value, retryMax) == 0) {
		TRACE_ERROR("ksz8051mnl PROBLEM\n\r");
	}

	TRACE_DEBUG("_PHYID1  : 0x%X, addr: %d\n\r", value, phyAddress);

	/* Find another one */
	if (value != GMII_OUI_LSB) {
		rc = 0xFF;
		for (cnt = 0; cnt < 32; cnt++) {
			phyAddress = (phyAddress + 1) & 0x1F;
			if (!GMAC_ReadPhy(phyAddress, GMII_PHYID1, &value, retryMax)) {
				TRACE_ERROR("MACB PROBLEM\n\r");
			}

			TRACE_DEBUG("_PHYID1  : 0x%X, addr: %d\n\r", value, phyAddress);
			if (value == GMII_OUI_MSB) {
				rc = phyAddress;
				break;
			}
		}
	}

	GMAC_MdioDisable();
	if (rc != 0xFF) {
		TRACE_INFO("** ksz8051mnl: Valid PHY Found: %d\n\r", rc);
		GMAC_ReadPhy(phyAddress, GMII_BMSR, &value, retryMax);
		TRACE_DEBUG("_BMSR  : 0x%X, addr: %d\n\r", value, phyAddress);
	}

	return rc;
}

/**
 * Setup the maximum timeout count of the driver.
 * \param pMacb   Pointer to the MACB instance
 * \param toMax Timeout maxmum count.
 */
void ksz8051mnl_setup_timeout(GMacb *pMacb, unsigned int toMax)
{
	pMacb->retryMax = toMax;
}

/**
 * \brief Initialize the MACB instance
 * \param pMacb          Pointer to the MACB instance
 * \param pGmac        Pointer to the Gmac instance for the MACB
 * \param phyAddress   The PHY address used to access the PHY
 *                     ( pre-defined by pin status on PHY reset )
 */
void ksz8051mnl_init(GMacb *pMacb, unsigned char phyAddress)
{
	pMacb->phyAddress = phyAddress;

	/* Initialize timeout by default */
	pMacb->retryMax = ksz8051mnl_RETRY_MAX;
}

/**
 * \brief Issue a SW reset to reset all registers of the PHY
 * \param pMacb   Pointer to the MACB instance
 * \return 1 if successfully, 0 if timeout.
 */
unsigned char ksz8051mnl_reset_phy(GMacb *pMacb)
{
	unsigned int retryMax;
	unsigned int bmcr = GMII_RESET;
	unsigned char phyAddress;
	unsigned int timeout = 10;
	unsigned char ret = 1;

	phyAddress = pMacb->phyAddress;
	retryMax = pMacb->retryMax;

	GMAC_MdioEnable();
	bmcr = GMII_RESET;
	GMAC_WritePhy(phyAddress, GMII_BMCR, bmcr, retryMax);

	do {
		GMAC_ReadPhy(phyAddress, GMII_BMCR, &bmcr, retryMax);
		timeout--;
	} while ((bmcr & GMII_RESET) && timeout);

	GMAC_MdioDisable();

	if (!timeout) {
		ret = 0;
	}

	return(ret);
}

/**
 * \brief Do a HW initialize to the PHY ( via RSTC ) and setup clocks & PIOs
 * This should be called only once to initialize the PHY pre-settings.
 * The PHY address is reset status of CRS,RXD[3:0] (the gmacPins' pullups).
 * The COL pin is used to select MII mode on reset (pulled up for Reduced MII)
 * The RXDV pin is used to select test mode on reset (pulled up for test mode)
 * The above pins should be predefined for corresponding settings in resetPins
 * The GMAC peripheral pins are configured after the reset done.
 * \param pMacb         Pointer to the MACB instance
 * \param mck         Main clock setting to initialize clock
 * \return 1 if RESET OK, 0 if timeout.
 */
unsigned char ksz8051mnl_init_phy(GMacb *pMacb, unsigned int mck)
{
	unsigned char rc;
	unsigned char phy;

	/* Perform RESET */
	rc = ksz8051mnl_reset_phy(pMacb);

	/* Configure GMAC runtime pins */
	if (1 == rc) {
		rc = gmac_set_clock(GMAC, mck);
		if (GMAC_INVALID == rc) {
			TRACE_ERROR("No Valid MDC clock\n\r");
			return 0;
		}

		/* Check PHY Address */
		phy = ksz8051mnl_find_valid_phy(pMacb);
		if (phy == 0xFF) {
			TRACE_ERROR("PHY Access fail\n\r");
			return 0;
		}

		if (phy != pMacb->phyAddress) {
			pMacb->phyAddress = phy;
			ksz8051mnl_reset_phy(pMacb);
		}
	} else {
		TRACE_ERROR("PHY Reset Timeout\n\r");
		return 0;
	}

	return 1;
}

/**
 * \brief Issue a Auto Negotiation of the PHY
 * \param pMacb   Pointer to the MACB instance
 * \return 1 if successfully, 0 if timeout
 */
unsigned char ksz8051mnl_auto_negotiate(GMacb *pMacb)
{
	unsigned int retryMax;
	unsigned int value;
	unsigned int phyAnar;
	unsigned int phyAnalpar;
	unsigned int retryCount = 0;
	unsigned char phyAddress;
	unsigned char rc = 1;

	phyAddress = pMacb->phyAddress;
	retryMax = pMacb->retryMax;

	GMAC_MdioEnable();

	if (!GMAC_ReadPhy(phyAddress, GMII_PHYID1, &value, retryMax)) {
		TRACE_ERROR("Pb GGMAC_ReadPhy Id1\n\r");
		rc = 0;
		goto AutoNegotiateExit;
	}

	TRACE_DEBUG("ReadPhy Id1 0x%X, addresse: %d\n\r", value, phyAddress);
	if (!GMAC_ReadPhy(phyAddress, GMII_PHYID2, &phyAnar, retryMax)) {
		TRACE_ERROR("Pb GMAC_ReadPhy Id2\n\r");
		rc = 0;
		goto AutoNegotiateExit;
	}

	TRACE_DEBUG("ReadPhy Id2 0x%X\n\r", (int)phyAnar);

	TRACE_DEBUG("Vendor Number Model = 0x%X\n\r", ((phyAnar >> 4) & 0x3F));
	TRACE_DEBUG("Model Revision Number = 0x%X\n\r", (phyAnar & 0x7));

	if ((value == GMII_OUI_MSB) &&
			(((phyAnar >> 10) & GMII_LSB_MASK) == GMII_OUI_LSB)) {
		TRACE_DEBUG("Vendor Number Model = 0x%X\n\r", ((phyAnar >> 4) & 0x3F));
		TRACE_DEBUG("Model Revision Number = 0x%X\n\r", (phyAnar & 0x7));
	} else {
		TRACE_ERROR("Problem OUI value\n\r");
	}

	/* Setup control register */
	rc  = GMAC_ReadPhy(phyAddress, GMII_BMCR, &value, retryMax);
	if (rc == 0) {
		goto AutoNegotiateExit;
	}

	value &= (unsigned int)(~GMII_AUTONEG); /* Remove autonegotiation enable */
	value &= (unsigned int)(~(GMII_LOOPBACK | GMII_POWER_DOWN));
	value |=  GMII_ISOLATE; /* Electrically isolate PHY */
	rc = GMAC_WritePhy(phyAddress, GMII_BMCR, value, retryMax);
	if (rc == 0) {
		goto AutoNegotiateExit;
	}

	/* Set the Auto_negotiation Advertisement Register */
	/* MII advertising for Next page */
	/* 100BaseTxFD and HD, 10BaseTFD and HD, IEEE 802.3 */
	phyAnar = GMII_100TX_FDX | GMII_100TX_HDX |
			GMII_10_FDX | GMII_10_HDX | GMII_AN_IEEE_802_3;
	rc = GMAC_WritePhy(phyAddress, GMII_ANAR, phyAnar, retryMax);
	if (rc == 0) {
		goto AutoNegotiateExit;
	}

	/* Read & modify control register */
	rc  = GMAC_ReadPhy(phyAddress, GMII_BMCR, &value, retryMax);
	if (rc == 0) {
		goto AutoNegotiateExit;
	}

	value |= GMII_SPEED_SELECT | GMII_AUTONEG | GMII_DUPLEX_MODE;
	rc = GMAC_WritePhy(phyAddress, GMII_BMCR, value, retryMax);
	if (rc == 0) {
		goto AutoNegotiateExit;
	}

	/* Restart Auto_negotiation */
	value |=  GMII_RESTART_AUTONEG;
	value &= (unsigned int)(~GMII_ISOLATE);
	rc = GMAC_WritePhy(phyAddress, GMII_BMCR, value, retryMax);
	if (rc == 0) {
		goto AutoNegotiateExit;
	}

	TRACE_DEBUG(" _BMCR: 0x%X\n\r", value);

	/* Check AutoNegotiate complete */
	while (1) {
		rc  = GMAC_ReadPhy(phyAddress, GMII_BMSR, &value, retryMax);
		if (rc == 0) {
			TRACE_ERROR("rc==0\n\r");
			goto AutoNegotiateExit;
		}

		/* Done successfully */
		if (value & GMII_AUTONEG_COMP) {
			TRACE_INFO("AutoNegotiate complete\n\r");
			break;
		}

		/* Timeout check */
		if (retryMax) {
			if (++retryCount >= retryMax) {
				ksz8051mnl_dump_registers(pMacb);
				TRACE_ERROR("TimeOut\n\r");
				rc = 0;
				goto AutoNegotiateExit;
			}
		}
	}

	/* Get the AutoNeg Link partner base page */
	rc  = GMAC_ReadPhy(phyAddress, GMII_PCR1, &phyAnalpar, retryMax);
	if (rc == 0) {
		goto AutoNegotiateExit;
	}

	phyAnalpar &= 7;

	/* Setup GMAC mode */
#if (ETH_PHY_MODE != 1)
	gmac_enable_mii();
#else
	gmac_enable_rmii();
#endif

AutoNegotiateExit:
	GMAC_MdioDisable();
	return rc;
}

/**
 * \brief Get the Link & speed settings, and automatically set up the GMAC with
 * the settings.
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_phy_addr PHY address.
 * \param uc_apply_setting_flag Set to 0 to not apply the PHY configurations,
 * else to apply.
 * \return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
uint8_t ksz8051mnl_set_link(GMacb *pMacb, uint8_t uc_phy_addr,
		uint8_t uc_apply_setting_flag)
{
	uint32_t ul_stat1;
	uint32_t ul_stat2;
	uint8_t uc_phy_address, uc_speed, uc_fd;
	uint8_t uc_rc = GMAC_TIMEOUT;
	unsigned int retryMax;

	gmac_enable_management(GMAC, true);

	uc_phy_address = pMacb->phyAddress;
	retryMax = pMacb->retryMax;

	uc_phy_address = uc_phy_addr;

	if (!GMAC_ReadPhy(uc_phy_address, GMII_BMSR, &ul_stat1, retryMax)) {
		/* Disable PHY management and start the GMAC transfer */
		gmac_enable_management(GMAC, false);
		TRACE_ERROR("Set link ERROR 1");
		return uc_rc;
	} else {
		uc_rc = GMAC_OK;
	}

	if ((ul_stat1 & GMII_LINK_STATUS) == 0) {
		/* Disable PHY management and start the GMAC transfer */
		gmac_enable_management(GMAC, false);
		TRACE_ERROR("Set link ERROR 2");

		return GMAC_INVALID;
	}

	if (uc_apply_setting_flag == 0) {
		/* Disable PHY management and start the GMAC transfer */
		gmac_enable_management(GMAC, false);
		TRACE_ERROR("Set link ERROR 3");

		return uc_rc;
	}

	/* Re-configure Link speed */
	/* uc_rc = gmac_phy_read(GMAC, uc_phy_address, GMII_ANAR, &ul_stat2); */
	if (!GMAC_ReadPhy(uc_phy_address, GMII_ANAR, &ul_stat2, retryMax))
	{
		/* Disable PHY management and start the GMAC transfer */
		gmac_enable_management(GMAC, false);
		TRACE_ERROR("Set link ERROR 4");

		return uc_rc;
	} else {
		uc_rc = GMAC_OK;
	}

	if ((ul_stat1 & GMII_100BASE_TX_FD) && (ul_stat2 & GMII_100TX_FDX)) {
		/* Set GMAC for 100BaseTX and Full Duplex */
		uc_speed = true;
		uc_fd = true;
	}

	if ((ul_stat1 & GMII_10BASE_T_FD) && (ul_stat2 & GMII_10_FDX)) {
		/* Set MII for 10BaseT and Full Duplex */
		uc_speed = false;
		uc_fd = true;
	}

	if ((ul_stat1 & GMII_100BASE_T4_HD) && (ul_stat2 & GMII_100TX_HDX)) {
		/* Set MII for 100BaseTX and Half Duplex */
		uc_speed = true;
		uc_fd = false;
	}

	if ((ul_stat1 & GMII_10BASE_T_HD) && (ul_stat2 & GMII_10_HDX)) {
		/* Set MII for 10BaseT and Half Duplex */
		uc_speed = false;
		uc_fd = false;
	}

	gmac_set_speed(GMAC, uc_speed);
	gmac_enable_full_duplex(GMAC, uc_fd);

	/* Start the GMAC transfers */
	gmac_enable_management(GMAC, false);
	return uc_rc;
}



