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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_gmac.h
// 模块描述: GMAC模块的初始化的操作，包含PHY初始化和MII初始化
//           按DJYOS的以太网协议实现要求实现的网卡底层驱动
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 25/11.2015
// =============================================================================

#ifndef __CPU_PERI_GMAC_H__
#define __CPU_PERI_GMAC_H__
#include "sam4e.h"


/* Definitions */

/* Number of buffer for RX, be carreful: MUST be 2^n */
#define GRX_BUFFERS  16
/* Number of buffer for TX, be carreful: MUST be 2^n */
#define GTX_BUFFERS   8

/* Buffer Size */
#define GMAC_RX_UNITSIZE            128     /* Fixed size for RX buffer */
#define GMAC_TX_UNITSIZE            1518    /* Size for ETH frame length */

/* The MAC can support frame lengths up to 1536 bytes. */
#define GMAC_FRAME_LENTGH_MAX       1536

#define GMAC_DUPLEX_HALF 0
#define GMAC_DUPLEX_FULL 1

#define GMAC_SPEED_10M      0
#define GMAC_SPEED_100M     1
#define GMAC_SPEED_1000M    2

typedef struct _GmacStats
{
	/* TX errors */
	unsigned int tx_packets;   /* Total Number of packets sent */
	unsigned int tx_comp;      /* Packet complete */
	unsigned int tx_errors;    /* TX errors ( Retry Limit Exceed ) */
	unsigned int collisions;   /* Collision */
	unsigned int tx_exausts;   /* Buffer exhausted */
	unsigned int tx_underruns; /* Under Run, not able to read from memory */
	unsigned int tx_hresp;          /* HRESP Not OK */
	unsigned int tx_latecollisions; /* Late Collision Occurred */
	/* RX errors */
	unsigned int rx_packets;   /* Total Number of packets RX */
	unsigned int rx_comp;      /* Total Number of packets RX */
	unsigned int rx_eof;       /* No EOF error */
	unsigned int rx_ovrs;      /* Over Run, not able to store to memory */
	unsigned int rx_bnas;      /* Buffer is not available */
} GmacStats, *PGmacStats;

/* Callback used by send function */
typedef void (*GMAC_TxCallback)(unsigned int status);
typedef void (*GMAC_RxCallback)(unsigned int status);
typedef void (*GMAC_WakeupCallback)(void);

#define GMAC_CAF_DISABLE  0
#define GMAC_CAF_ENABLE   1
#define GMAC_NBC_DISABLE  0
#define GMAC_NBC_ENABLE   1

/* Return for GMAC_Send function */
#define GMAC_TX_OK                     0
#define GMAC_TX_BUFFER_BUSY            1
#define GMAC_TX_INVALID_PACKET         2

/* Return for GMAC_Poll function */
#define GMAC_RX_OK                   0
#define GMAC_RX_NO_DATA              1
#define GMAC_RX_FRAME_SIZE_TOO_SMALL 2

/* The buffer addresses written into the descriptors must be aligned, so the
 *  last few bits are zero.  These bits have special meaning for the GMAC
 *  peripheral and cannot be used as part of the address. */
#define GMAC_RXD_ADDR_MASK      0xFFFFFFFC
#define GMAC_RXD_WRAP         (1ul << 1)  /* Wrap bit */
#define GMAC_RXD_OWNERSHIP    (1ul << 0)  /* Ownership bit */

#define GMAC_RXD_BROADCAST     (1ul << 31) /* Broadcast detected */
#define GMAC_RXD_MULTIHASH     (1ul << 30) /* Multicast hash match */
#define GMAC_RXD_UNIHASH       (1ul << 29) /* Unicast hash match */
#define GMAC_RXD_EXTADDR       (1ul << 28) /* External address match */
#define GMAC_RXD_ADDR1         (1ul << 26) /* Address 1 match */
#define GMAC_RXD_ADDR2         (1ul << 25) /* Address 2 match */
#define GMAC_RXD_ADDR3         (1ul << 24) /* Address 3 match */
#define GMAC_RXD_ADDR4         (1ul << 23) /* Address 4 match */
#define GMAC_RXD_TYPE          (1ul << 22) /* Type ID match */
#define GMAC_RXD_VLAN          (1ul << 21) /* VLAN tag detected */
#define GMAC_RXD_PRIORITY      (1ul << 20) /* Priority tag detected */
#define GMAC_RXD_PRIORITY_MASK (3ul << 17) /* VLAN priority */
#define GMAC_RXD_CFI           (1ul << 16) /* Concatenation Format Indicator only if bit 21 is set */
#define GMAC_RXD_EOF           (1ul << 15) /* End of frame */
#define GMAC_RXD_SOF           (1ul << 14) /* Start of frame */
#define GMAC_RXD_OFFSET_MASK               /* Receive buffer offset */
#define GMAC_RXD_LEN_MASK       (0xFFF)    /* Length of frame including FCS (if selected) */
#define GMAC_RXD_LENJUMBO_MASK  (0x3FFF)   /* Jumbo frame length */

#define GMAC_TXD_USED         (1ul << 31) /* Frame is transmitted */
#define GMAC_TXD_WRAP         (1ul << 30) /* Last descriptor */
#define GMAC_TXD_ERROR        (1ul << 29) /* Retry limit exceeded, error */
#define GMAC_TXD_UNDERRUN     (1ul << 28) /* Transmit underrun */
#define GMAC_TXD_EXHAUSTED    (1ul << 27) /* Buffer exhausted */
#define GMAC_TXD_NOCRC        (1ul << 16) /* No CRC */
#define GMAC_TXD_LAST         (1ul << 15) /* Last buffer in frame */
#define GMAC_TXD_LEN_MASK       (0x7FF)     /* Length of buffer */

/* GMAC clock speed */
#define GMAC_CLOCK_SPEED_540MHZ        (540 * 1000 * 1000)
#define GMAC_CLOCK_SPEED_320MHZ        (320 * 1000 * 1000)
#define GMAC_CLOCK_SPEED_240MHZ        (240 * 1000 * 1000)
#define GMAC_CLOCK_SPEED_160MHZ        (160 * 1000 * 1000)
#define GMAC_CLOCK_SPEED_120MHZ        (120 * 1000 * 1000)
#define GMAC_CLOCK_SPEED_80MHZ          (80 * 1000 * 1000)
#define GMAC_CLOCK_SPEED_40MHZ          (40 * 1000 * 1000)
#define GMAC_CLOCK_SPEED_20MHZ          (20 * 1000 * 1000)

/* GMAC maintain code default value*/
#define GMAC_MAN_CODE_VALUE    (10)

/* GMAC maintain start of frame default value*/
#define GMAC_MAN_SOF_VALUE     (1)

/* GMAC maintain read/write*/
#define GMAC_MAN_RW_TYPE       (2)

/* GMAC maintain read only*/
#define GMAC_MAN_READ_ONLY     (1)

/* GMAC address length */
#define GMAC_ADDR_LENGTH       (6)

/*
 * \brief Return codes for GMAC APIs.
 */
typedef enum
{
	GMAC_OK = 0,         /* Operation OK */
	GMAC_TIMEOUT = 1,    /* Operation timeout */
	GMAC_TX_BUSY,        /* TX in progress */
	GMAC_RX_NULL,        /* No data received */
	GMAC_SIZE_TOO_SMALL, /* Buffer size not enough */
	GMAC_PARAM,          /* Parameter error, TX packet invalid or RX size too small */
	GMAC_INVALID = 0xFF, /* Invalid */
} gmac_status_t;



/* Receive buffer descriptor struct */
typedef struct gmac_rx_descriptor
{
	union gmac_rx_addr
	{
		uint32_t val;
		struct gmac_rx_addr_bm
		{
			uint32_t b_ownership : 1, /* User clear, GMAC sets this to 1 once it has
			                           *successfully written a frame to memory */
					b_wrap : 1, /* Marks last descriptor in receive buffer */
					addr_dw : 30; /* Address in number of DW */
		} bm;
	} addr; /* Address, Wrap & Ownership */
	union gmac_rx_status
	{
		uint32_t val;
		struct gmac_rx_status_bm
		{
			uint32_t len:12,       /* Length of frame including FCS */
			offset:2,              /* Receive buffer offset,  bits 13:12 of frame length for jumbo frame */
			b_sof:1,               /* Start of frame */
			b_eof:1,               /* End of frame */
			b_cfi:1,               /* Concatenation Format Indicator */
			vlan_priority:3,       /* VLAN priority (if VLAN detected) */
			b_priority_detected:1, /* Priority tag detected */
			b_vlan_detected:1,     /* VLAN tag detected */
			b_type_id_match:1,     /* Type ID match */
			b_addr4match:1,        /* Address register 4 match */
			b_addr3match:1,        /* Address register 3 match */
			b_addr2match:1,        /* Address register 2 match */
			b_addr1match:1,        /* Address register 1 match */
			reserved:1,
			b_ext_addr_match:1,    /* External address match */
			b_uni_hash_match:1,    /* Unicast hash match */
			b_multi_hash_match:1,  /* Multicast hash match */
			b_boardcast_detect:1;  /* Global broadcast address detected */
		} bm;
	} status;
} __attribute__ ((packed, aligned(8))) gmac_rx_descriptor_t; /* GCC */

/* Transmit buffer descriptor struct */
typedef struct gmac_tx_descriptor
{
	uint32_t addr;
	union gmac_tx_status
	{
		uint32_t val;
		struct gmac_tx_status_bm
		{
			uint32_t len:11, /* Length of buffer */
			reserved:4,
			b_last_buffer:1, /* Last buffer (in the current frame) */
			b_no_crc:1,      /* No CRC */
			reserved1:10,
			b_exhausted:1,   /* Buffer exhausted in mid frame */
			b_underrun:1,    /* Transmit underrun */
			b_error:1,       /* Retry limit exceeded, error detected */
			b_wrap:1,        /* Marks last descriptor in TD list */
			b_used:1;        /* User clear, GMAC sets this to 1 once a frame has been successfully transmitted */
		} bm;
	} status;
} __attribute__ ((packed, aligned(8))) gmac_tx_descriptor_t; /* GCC */


/*
 * GMAC driver structure.
 */
//typedef struct gmac_device {
//	/* Pointer to HW register base */
//	Gmac *p_hw;
//
//	/*
//	 * Pointer to allocated TX buffer.
//	 * Section 3.6 of AMBA 2.0 spec states that burst should not cross
//	 * 1K Boundaries.
//	 * Receive buffer manager writes are burst of 2 words => 3 lsb bits
//	 * of the address shall be set to 0.
//	 */
//	uint8_t *p_tx_buffer;
//	/* Pointer to allocated RX buffer */
//	uint8_t *p_rx_buffer;
//	/* Pointer to Rx TDs (must be 8-byte aligned) */
//	gmac_rx_descriptor_t *p_rx_dscr;
//	/* Pointer to Tx TDs (must be 8-byte aligned) */
//	gmac_tx_descriptor_t *p_tx_dscr;
//	/* Optional callback to be invoked once a frame has been received */
//	gmac_dev_tx_cb_t func_rx_cb;
//	/* Optional callback to be invoked once several TDs have been released */
//	gmac_dev_wakeup_cb_t func_wakeup_cb;
//	/* Optional callback list to be invoked once TD has been processed */
//	gmac_dev_tx_cb_t *func_tx_cb_list;
//	/* RX TD list size */
//	uint16_t us_rx_list_size;
//	/* RX index for current processing TD */
//	uint16_t us_rx_idx;
//	/* TX TD list size */
//	uint16_t us_tx_list_size;
//	/* Circular buffer head pointer by upper layer (buffer to be sent) */
//	uint16_t us_tx_head;
//	/* Circular buffer tail pointer incremented by handlers (buffer sent) */
//	uint16_t us_tx_tail;
//
//	/* Number of free TD before wakeup callback is invoked */
//	uint8_t uc_wakeup_threshold;
//} gmac_device_t;


/* Descriptors for RX (required aligned by 8) */
typedef struct
{
	volatile gmac_rx_descriptor_t td[GRX_BUFFERS];
//	GMAC_RxCallback rxCb; /* Callback function to be invoked once a frame
//	                       * has been received */
	unsigned short idx;
} GRxTd;

/* Descriptors for TX (required aligned by 8) */
typedef struct
{
	volatile gmac_tx_descriptor_t td[GTX_BUFFERS];
	GMAC_TxCallback txCb[GTX_BUFFERS]; /* Callback function to be invoked
	                                    * once TD has been processed */
//	GMAC_WakeupCallback wakeupCb;   /* Callback function to be invoked
//	                                 * once several TD have been released */
	unsigned short wakeupThreshold; /* Number of free TD before wakeupCb
	                                 * is invoked */
	unsigned short head;       /* Circular buffer head pointer incremented
	                            * by the upper layer (buffer to be sent) */
	unsigned short tail;       /* Circular buffer head pointer incremented
	                            * by the IT handler (buffer sent) */
} GTxTd;




/*
 * \brief Write network control value.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_ncr   Network control value.
 */
static inline void gmac_network_control(Gmac *p_gmac, uint32_t ul_ncr)
{
	p_gmac->GMAC_NCR = ul_ncr;
}

/*
 * \brief Get network control value.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 */

static inline uint32_t gmac_get_network_control(Gmac *p_gmac)
{
	return p_gmac->GMAC_NCR;
}

/*
 * \brief Enable/Disable GMAC receive.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable GMAC receiver, else to enable it.
 */
static inline void gmac_enable_receive(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCR |= GMAC_NCR_RXEN;
	} else {
		p_gmac->GMAC_NCR &= ~GMAC_NCR_RXEN;
	}
}

/*
 * \brief Enable/Disable GMAC transmit.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable GMAC transmit, else to enable it.
 */
static inline void gmac_enable_transmit(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCR |= GMAC_NCR_TXEN;
	} else {
		p_gmac->GMAC_NCR &= ~GMAC_NCR_TXEN;
	}
}

/*
 * \brief Enable/Disable GMAC management.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable GMAC management, else to enable it.
 */
static inline void gmac_enable_management(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCR |= GMAC_NCR_MPE;
	} else {
		p_gmac->GMAC_NCR &= ~GMAC_NCR_MPE;
	}
}

/*
 * \brief Clear all statistics registers.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 */
static inline void gmac_clear_statistics(Gmac *p_gmac)
{
	p_gmac->GMAC_NCR |= GMAC_NCR_CLRSTAT;
}

/*
 * \brief Increase all statistics registers.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 */
static inline void gmac_increase_statistics(Gmac *p_gmac)
{
	p_gmac->GMAC_NCR |= GMAC_NCR_INCSTAT;
}

/*
 * \brief Enable/Disable statistics registers writing.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable the statistics registers writing, else to
 *                    enable it.
 */
static inline void gmac_enable_statistics_write(Gmac *p_gmac,
		uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCR |= GMAC_NCR_WESTAT;
	} else {
		p_gmac->GMAC_NCR &= ~GMAC_NCR_WESTAT;
	}
}

/*
 * \brief In half-duplex mode, forces collisions on all received frames.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable the back pressure, else to enable it.
 */
static inline void gmac_enable_back_pressure(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCR |= GMAC_NCR_BP;
	} else {
		p_gmac->GMAC_NCR &= ~GMAC_NCR_BP;
	}
}

/*
 * \brief Start transmission.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 */
static inline void gmac_start_transmission(Gmac *p_gmac)
{
	p_gmac->GMAC_NCR |= GMAC_NCR_TSTART;
}

/*
 * \brief Halt transmission.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 */
static inline void gmac_halt_transmission(Gmac *p_gmac)
{
	p_gmac->GMAC_NCR |= GMAC_NCR_THALT;
}

/*
 * \brief Set up network configuration register.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_cfg   Network configuration value.
 */
static inline void gmac_set_configure(Gmac *p_gmac, uint32_t ul_cfg)
{
	p_gmac->GMAC_NCFGR = ul_cfg;
}

/*
 * \brief Get network configuration.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return Network configuration.
 */
static inline uint32_t gmac_get_configure(Gmac *p_gmac)
{
	return p_gmac->GMAC_NCFGR;
}

/*
 * \brief Set speed.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_speed 1 to indicate 100Mbps, 0 to 10Mbps.
 */
static inline void gmac_set_speed(Gmac *p_gmac, uint8_t uc_speed)
{
	if (uc_speed) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_SPD;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_SPD;
	}
}

/*
 * \brief Enable/Disable Full-Duplex mode.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable the Full-Duplex mode, else to enable it.
 */
static inline void gmac_enable_full_duplex(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_FD;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_FD;
	}
}

/*
 * \brief Enable/Disable Copy(Receive) All Valid Frames.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable copying all valid frames, else to enable it.
 */
static inline void gmac_enable_copy_all(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_CAF;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_CAF;
	}
}

/*
 * \brief Enable/Disable jumbo frames (up to 10240 bytes).
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable the jumbo frames, else to enable it.
 */
static inline void gmac_enable_jumbo_frames(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_JFRAME;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_JFRAME;
	}
}

/*
 * \brief Disable/Enable broadcast receiving.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   1 to disable the broadcast, else to enable it.
 */
static inline void gmac_disable_broadcast(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_NBC;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_NBC;
	}
}

/*
 * \brief Enable/Disable multicast hash.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable the multicast hash, else to enable it.
 */
static inline void gmac_enable_multicast_hash(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_UNIHEN;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_UNIHEN;
	}
}

/*
 * \brief Enable/Disable big frames (over 1518, up to 1536).
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable big frames else to enable it.
 */
static inline void gmac_enable_big_frame(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_MAXFS;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_MAXFS;
	}
}

/*
 * \brief Set MDC clock divider.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_mck   GMAC MCK.
 *
 * \return GMAC_OK if successfully.
 */
static inline uint8_t gmac_set_clock(Gmac *p_gmac, uint32_t ul_mck)
{
	uint32_t ul_clk;

	if (ul_mck > GMAC_CLOCK_SPEED_540MHZ) {
		return GMAC_INVALID;
	} else if (ul_mck > GMAC_CLOCK_SPEED_160MHZ) {
		ul_clk = GMAC_NCFGR_CLK_MCK_96;
	} else if (ul_mck > GMAC_CLOCK_SPEED_120MHZ) {
		ul_clk = GMAC_NCFGR_CLK_MCK_64;
	} else if (ul_mck > GMAC_CLOCK_SPEED_80MHZ) {
		ul_clk = GMAC_NCFGR_CLK_MCK_48;
	} else if (ul_mck > GMAC_CLOCK_SPEED_40MHZ) {
		ul_clk = GMAC_NCFGR_CLK_MCK_32;
	} else if (ul_mck > GMAC_CLOCK_SPEED_20MHZ) {
		ul_clk = GMAC_NCFGR_CLK_MCK_16;
	} else {
		ul_clk = GMAC_NCFGR_CLK_MCK_8;
	}

	p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_CLK_Msk;
	p_gmac->GMAC_NCFGR |= ul_clk;
	return GMAC_OK;
}

/*
 * \brief Enable/Disable retry test.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable the GMAC receiver, else to enable it.
 */
static inline void gmac_enable_retry_test(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_RTY;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_RTY;
	}
}

/*
 * \brief Enable/Disable pause (when a valid pause frame is received).
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable pause frame, else to enable it.
 */
static inline void gmac_enable_pause_frame(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_PEN;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_PEN;
	}
}

/*
 * \brief Set receive buffer offset to 0 ~ 3.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 */
static inline void gmac_set_rx_buffer_offset(Gmac *p_gmac, uint8_t uc_offset)
{
	p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_RXBUFO_Msk;
	p_gmac->GMAC_NCFGR |= GMAC_NCFGR_RXBUFO(uc_offset);
}

/*
 * \brief Enable/Disable receive length field checking.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable receive length field checking, else to enable
 * it.
 */
static inline void gmac_enable_rx_length_check(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_LFERD;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_LFERD;
	}
}

/*
 * \brief Enable/Disable discarding FCS field of received frames.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable discarding FCS field of received frames, else
 * to enable it.
 */
static inline void gmac_enable_discard_fcs(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_RFCS;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_RFCS;
	}
}

/*
 * \brief Enable/Disable frames to be received in half-duplex mode
 * while transmitting.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable the received in half-duplex mode, else to
 * enable it.
 */
static inline void gmac_enable_efrhd(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_EFRHD;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_EFRHD;
	}
}

/*
 * \brief Enable/Disable ignore RX FCS.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable ignore RX FCS, else to enable it.
 */
static inline void gmac_enable_ignore_rx_fcs(Gmac *p_gmac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_gmac->GMAC_NCFGR |= GMAC_NCFGR_IRXFCS;
	} else {
		p_gmac->GMAC_NCFGR &= ~GMAC_NCFGR_IRXFCS;
	}
}

/*
 * \brief Get Network Status.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return Network status.
 */
static inline uint32_t gmac_get_status(Gmac *p_gmac)
{
	return p_gmac->GMAC_NSR;
}

/*
 * \brief Get MDIO IN pin status.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return MDIO IN pin status.
 */
static inline uint8_t gmac_get_MDIO(Gmac *p_gmac)
{
	return ((p_gmac->GMAC_NSR & GMAC_NSR_MDIO) > 0);
}

/*
 * \brief Check if PHY is idle.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return  1 if PHY is idle.
 */
static inline uint8_t gmac_is_phy_idle(Gmac *p_gmac)
{
	return ((p_gmac->GMAC_NSR & GMAC_NSR_IDLE) > 0);
}

/*
 * \brief Return transmit status.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return  Transmit status.
 */
static inline uint32_t gmac_get_tx_status(Gmac *p_gmac)
{
	return p_gmac->GMAC_TSR;
}

/*
 * \brief Clear transmit status.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_status   Transmit status.
 */
static inline void gmac_clear_tx_status(Gmac *p_gmac, uint32_t ul_status)
{
	p_gmac->GMAC_TSR = ul_status;
}

/*
 * \brief Return receive status.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 */
static inline uint32_t gmac_get_rx_status(Gmac *p_gmac)
{
	return p_gmac->GMAC_RSR;
}

/*
 * \brief Clear receive status.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_status   Receive status.
 */
static inline void gmac_clear_rx_status(Gmac *p_gmac, uint32_t ul_status)
{
	p_gmac->GMAC_RSR = ul_status;
}

/*
 * \brief Set Rx Queue.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_addr   Rx queue address.
 */
static inline void gmac_set_rx_queue(Gmac *p_gmac, uint32_t ul_addr)
{
	p_gmac->GMAC_RBQB = GMAC_RBQB_ADDR_Msk & ul_addr;
}

/*
 * \brief Get Rx Queue Address.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return  Rx queue address.
 */
static inline uint32_t gmac_get_rx_queue(Gmac *p_gmac)
{
	return p_gmac->GMAC_RBQB;
}

/*
 * \brief Set Tx Queue.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_addr  Tx queue address.
 */
static inline void gmac_set_tx_queue(Gmac *p_gmac, uint32_t ul_addr)
{
	p_gmac->GMAC_TBQB = GMAC_TBQB_ADDR_Msk & ul_addr;
}

/*
 * \brief Get Tx Queue.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return  Rx queue address.
 */
static inline uint32_t gmac_get_tx_queue(Gmac *p_gmac)
{
	return p_gmac->GMAC_TBQB;
}

/*
 * \brief Enable interrupt(s).
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_source   Interrupt source(s) to be enabled.
 */
static inline void gmac_enable_interrupt(Gmac *p_gmac, uint32_t ul_source)
{
	p_gmac->GMAC_IER = ul_source;
}

/*
 * \brief Disable interrupt(s).
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_source   Interrupt source(s) to be disabled.
 */
static inline void gmac_disable_interrupt(Gmac *p_gmac, uint32_t ul_source)
{
	p_gmac->GMAC_IDR = ul_source;
}

/*
 * \brief Return interrupt status.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return Interrupt status.
 */
static inline uint32_t gmac_get_interrupt_status(Gmac *p_gmac)
{
	return p_gmac->GMAC_ISR;
}

/*
 * \brief Return interrupt mask.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return Interrupt mask.
 */
static inline uint32_t gmac_get_interrupt_mask(Gmac *p_gmac)
{
	return p_gmac->GMAC_IMR;
}

/*
 * \brief Execute PHY maintenance command.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_phy_addr   PHY address.
 * \param uc_reg_addr   Register address.
 * \param uc_rw   1 to Read, 0 to write.
 * \param us_data   Data to be performed, write only.
 */
static inline void gmac_maintain_phy(Gmac *p_gmac,
		uint8_t uc_phy_addr, uint8_t uc_reg_addr, uint8_t uc_rw,
		uint16_t us_data)
{
	/* Wait until bus idle */
	while ((p_gmac->GMAC_NSR & GMAC_NSR_IDLE) == 0) {
	}
	/* Write maintain register */
	p_gmac->GMAC_MAN = GMAC_MAN_WTN(GMAC_MAN_CODE_VALUE)
			| GMAC_MAN_CLTTO
			| GMAC_MAN_PHYA(uc_phy_addr)
			| GMAC_MAN_REGA(uc_reg_addr)
			| GMAC_MAN_OP((uc_rw ? GMAC_MAN_RW_TYPE :
			  GMAC_MAN_READ_ONLY))
			| GMAC_MAN_DATA(us_data);
}

/*
 * \brief Get PHY maintenance data returned.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 *
 * \return Get PHY data.
 */
static inline uint16_t gmac_get_phy_data(Gmac *p_gmac)
{
	/* Wait until bus idle */
	while ((p_gmac->GMAC_NSR & GMAC_NSR_IDLE) == 0) {
	}
	/* Return data */
	return (uint16_t)(p_gmac->GMAC_MAN & GMAC_MAN_DATA_Msk);
}

/*
 * \brief Set pause time.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param us_pause_time   Pause time.
 */
static inline void gmac_set_pause_time(Gmac *p_gmac, uint16_t us_pause_time)
{
//	printf("ERROR"); /* Read only now.... */
	while (1) {
	}
	/* p_gmac->GMAC_RPQ = us_pause_time; */
}

/*
 * \brief Set Hash.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ul_hash_top   Hash top.
 * \param ul_hash_bottom   Hash bottom.
 */
static inline void gmac_set_hash(Gmac *p_gmac, uint32_t ul_hash_top,
		uint32_t ul_hash_bottom)
{
	p_gmac->GMAC_HRB = ul_hash_bottom;
	p_gmac->GMAC_HRT = ul_hash_top;
}

/*
 * \brief Set 64 bits Hash.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param ull_hash   64 bits hash value.
 */
static inline void gmac_set_hash64(Gmac *p_gmac, uint64_t ull_hash)
{
	p_gmac->GMAC_HRB = (uint32_t)ull_hash;
	p_gmac->GMAC_HRT = (uint32_t)(ull_hash >> 32);
}

/*
 * \brief Set MAC Address.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_index  GMAC specific address register index.
 * \param p_mac_addr  GMAC address.
 */
static inline void gmac_set_address(Gmac *p_gmac, uint8_t uc_index,
		uint8_t *p_mac_addr)
{
	p_gmac->GMAC_SA[uc_index].GMAC_SAB = (p_mac_addr[3] << 24)
			| (p_mac_addr[2] << 16)
			| (p_mac_addr[1] << 8)
			| (p_mac_addr[0]);
	p_gmac->GMAC_SA[uc_index].GMAC_SAT = (p_mac_addr[5] << 8)
			| (p_mac_addr[4]);
}

/*
 * \brief Set MAC Address via 2 dword.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_index  GMAC specific address register index.
 * \param ul_mac_top  GMAC top address.
 * \param ul_mac_bottom  GMAC bottom address.
 */
static inline void gmac_set_address32(Gmac *p_gmac, uint8_t uc_index,
		uint32_t ul_mac_top, uint32_t ul_mac_bottom)
{
	p_gmac->GMAC_SA[uc_index].GMAC_SAB = ul_mac_bottom;
	p_gmac->GMAC_SA[uc_index].GMAC_SAT = ul_mac_top;
}

/*
 * \brief Set MAC Address via int64.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_index  GMAC specific address register index.
 * \param ull_mac  64-bit GMAC address.
 */
static inline void gmac_set_address64(Gmac *p_gmac, uint8_t uc_index,
		uint64_t ull_mac)
{
	p_gmac->GMAC_SA[uc_index].GMAC_SAB = (uint32_t)ull_mac;
	p_gmac->GMAC_SA[uc_index].GMAC_SAT = (uint32_t)(ull_mac >> 32);
}

/*
 * \brief Enable/Disable RMII.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable the RMII mode, else to enable it.
 */
static inline void gmac_enable_rmii(Gmac *p_gmac, uint8_t uc_enable)
{
//	if (uc_enable) {
		/* RGMII mode is selected */
		p_gmac->GMAC_UR |= GMAC_UR_RMIIMII;
//	} else {
//		/* GMII/MII mode is selected */
//		p_gmac->GMAC_UR &= ~GMAC_UR_RMIIMII;
//	}
}

/*
 * \brief Enable/Disable transceiver input clock.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_enable   0 to disable transceiver input clock, else to enable it.
 */
static inline void gmac_enable_transceiver_clock(Gmac *p_gmac,
		uint8_t uc_enable)
{
	/*	if (uc_enable) { */
	/*		p_gmac->GMAC_UR |= GMAC_USRIO_CLKEN; */
	/*	} else { */
	/*		p_gmac->GMAC_UR &= ~GMAC_USRIO_CLKEN; */
	/*	} */
}


u32 GMAC_WritePhy(u32 PhyAddress, u32 Address, u32 Value,u32 retry);
u32 GMAC_ReadPhy(u32 PhyAddress, u32 Address, u32 *pValue,u32 retry);
void GMAC_MdioEnable(void);
void GMAC_MdioDisable(void);
void GMAC_EnableMIii(void);
#endif 	/* __CPU_PERI_GMAC_H__ */




