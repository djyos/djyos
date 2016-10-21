
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
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_gmac.c
// 模块描述: GMAC模块底层硬件驱动，为DJYIP的协议栈特定的驱动
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 09/11.2015
// =============================================================================
#include <sys/socket.h>

#include "cpu_peri.h"
#include "board-config.h"
#include "gmac_interface.h"

#ifdef DEBUG
#define GMAC_DEBUG  printk
#else
#define GMAC_DEBUG
#endif
//==================================mac driver module==============================================
//here we defines the ethernet driver we used
//defines the receive buffer descripter
/** Receive buffer descriptor struct */
typedef struct
{
	union rcvaddr
	{
		uint32_t val;
		struct addrstat
		{
			uint32_t bOwnership:1;  /**< User clear, GMAC set this to one once
										 it has successfully written a frame to
										 memory */
			uint32_t bWrap:1;       /**< Marks last descriptor in receive buffer */
			uint32_t addrDW:30;     /**< Address in number of DW */
		} bm;
	} addr;                    /**< Address, Wrap & Ownership */
	union rcvstat
	{
		uint32_t val;
		struct macrcvstatbits
		{
			uint32_t len:12,                /** Length of frame including FCS */
					 offset:2,              /** Receive buffer offset,
												bits 13:12 of frame length for jumbo
												frame */
					 bSof:1,                /** Start of frame */
					 bEof:1,                /** End of frame */
					 bCFI:1,                /** Concatenation Format Indicator */
					 vlanPriority:3,        /** VLAN priority (if VLAN detected) */
					 bPriorityDetected:1,   /** Priority tag detected */
					 bVlanDetected:1,       /**< VLAN tag detected */
					 bTypeIDMatch:1,        /**< Type ID match */
					 bAddr4Match:1,         /**< Address register 4 match */
					 bAddr3Match:1,         /**< Address register 3 match */
					 bAddr2Match:1,         /**< Address register 2 match */
					 bAddr1Match:1,         /**< Address register 1 match */
					 reserved:1,
					 bExtAddrMatch:1,       /**< External address match */
					 bUniHashMatch:1,       /**< Unicast hash match */
					 bMultiHashMatch:1,     /**< Multicast hash match */
					 bBroadcastDetected:1;  /**< Global all ones broadcast
												 address detected */
		} bm;
	} status;
} tagRcvBD ;    /* GCC */

/** Transmit buffer descriptor struct */
typedef struct
{
	uint32_t addr;
	union macsndstatus
	{
		uint32_t val;
		struct macsndstatusbits
		{
			uint32_t len:11,        /**< Length of buffer */
					 reserved:4,
					 bLastBuffer:1, /**< Last buffer (in the current frame) */
					 bNoCRC:1,      /**< No CRC */
					 reserved1:10,
					 bExhausted:1,  /**< Buffer exhausted in mid frame */
					 bUnderrun:1,   /**< Transmit under run */
					 bError:1,      /**< Retry limit exceeded, error detected */
					 bWrap:1,       /**< Marks last descriptor in TD list */
					 bUsed:1;       /**< User clear, GMAC sets this once a frame
										 has been successfully transmitted */
		} bm;
	} status;
} tagSndBD;     /* GCC */

// ====================================mem resource need========================
//config for the que0
#ifdef __GNUC__
#define CN_TXBUF0_LEN    128
#define CN_TXBD0_LEN     8
#define CN_RXBUF0_LEN    1600
#define CN_RXBD0_LEN     8
static vu8 gTxBuffer0[CN_TXBD0_LEN][CN_TXBUF0_LEN] __attribute__((aligned(32)));
static vu8 gRxBuffer0[CN_RXBD0_LEN][CN_RXBUF0_LEN] __attribute__((aligned(32)));
static volatile tagRcvBD gRxDs0[CN_RXBD0_LEN] __attribute__((aligned(32)));
static volatile tagSndBD gTxDs0[CN_TXBD0_LEN] __attribute__((aligned(32)));
//config for the que1
#define CN_TXBUF1_LEN    128
#define CN_TXBD1_LEN     1
#define CN_RXBUF1_LEN    1600
#define CN_RXBD1_LEN     1
static vu8 gTxBuffer1[CN_TXBD1_LEN][CN_TXBUF1_LEN] __attribute__((aligned(32)));
static vu8 gRxBuffer1[CN_RXBD1_LEN][CN_RXBUF1_LEN] __attribute__((aligned(32)));
static volatile tagRcvBD gRxDs1[CN_RXBD1_LEN] __attribute__((aligned(32)));
static volatile tagSndBD gTxDs1[CN_TXBD1_LEN] __attribute__((aligned(32)));
//config for the que2
#define CN_TXBUF2_LEN    128
#define CN_TXBD2_LEN     1
#define CN_RXBUF2_LEN    1600
#define CN_RXBD2_LEN     1
static vu8 gTxBuffer2[CN_TXBD2_LEN][CN_TXBUF2_LEN] __attribute__((aligned(32)));
static vu8 gRxBuffer2[CN_RXBD2_LEN][CN_RXBUF2_LEN] __attribute__((aligned(32)));
static volatile tagRcvBD gRxDs2[CN_RXBD2_LEN] __attribute__((aligned(32)));
static volatile tagSndBD gTxDs2[CN_TXBD2_LEN] __attribute__((aligned(32)));

#else
#error "bd must be 8-byte aligned"
#endif

typedef struct
{
    volatile tagRcvBD   *rcvbdtab;
    vu16                 rcvbdlen;
    vu16                 rcvbuflen;
    vu16                 rcvbdcur;
    vu8                 *rcvmem;
    vu16                 rcvmemlen;
    volatile tagSndBD   *sndbdtab;
    vu16                 sndbdlen;
    vu16                 sndbuflen;
    vu16                 sndbdcur;
    vu8                 *sndmem;
    vu16                 sndmemlen;
    vu16			     sndbdlast;
}tagQueue;

typedef struct
{
    vu32  Ints;                     //the int counter
    vu32  rcvTimes;                 //rcv package times
    vu32  rcvInts;                  //rcv ints counter
    vu32  hnoInts;                  //hno ints counter
    vu32  rxoverInts;               //rcv over ints counter
    vu32  recInts;                  //rec ints counter
    vu32  bnaInts;                  //bna ins counter
    vu32  sndTimes;                 //the snd times counter
    vu32  sndOkTimes;               //the send success counter
    vu32  sndnobdTimes;				//not enough bd rings
    vu32  sndInts;                  //the send ints counter
    vu32  sndErrInts;               //the send err ints counter
    vu32  rxTaskRun;                //the rx task run counter
    vu32  rsttimes;                 //the reset counter
    vu32  badframe;					//the bad gmac frame
}tagMacDebug;

//this is the mac receive hook, if any hook rcv,then the data will goto the hook
//the receive data goto the stack default
typedef bool_t (*fnRcvDeal)(u8 *buf, u16 len);
#define CN_QUE_NUM  3
#define CN_QUE_0    0
#define CN_QUE_1    1
#define CN_QUE_2    2

#define CN_DEVNAME_LEN  32
typedef struct
{
	//os member
	struct SemaphoreLCB   *rcvsync;          //activate the receive task
	struct MutexLCB       *protect;          //protect the device
	ptu32_t                devhandle;        //returned by the tcpip stack
	char                   devname[CN_DEVNAME_LEN];
	//hardware
    Gmac         *pHw;                        //hardware register group
    vu8           bId;                        //hardware id
    tagQueue      queueList[CN_QUE_NUM];      //queue info
    u8            macaddr[CN_MACADDR_LEN];    //storage the mac address
    //which used to descript the device or control the device
    struct devstat
    {
    	vu32 caf:1;       //1 copy all the frame while 0 only copy the valid
    	vu32 nbc:1;       //1 no broadcast receive while 0 perimit receive broadcast frames
    	vu32 duplex:1;    //1 full while 0 half
    	vu32 speed:1;     //1 means 100Mb while 0 10 Mb
    	vu32 mii:1;       //1 means mii mode while 0 rmii mode
    }macstat;
    //dev method
	vu32 loop:1;      //1 use the loop mode while 0 interrupt mode
	vu32 loopcycle:31;//unit:ms
    fnRcvDeal     fnrcvhook;                  //rcv hook function
    //debug info
    tagMacDebug   debuginfo;                  //debug info
}tagMacDriver;
static tagMacDriver   gMacDriver;

///////////////////////////THE FOLLOWING USED FOR THE FILTER LOGIC//////////////
static void __macbitsset(vu32 *reg,u32 bits)
{
	vu32 value;
	value = *reg;
	value |=bits;
	*reg = value;
	return;
}
static void __macbitsclear(vu32 *reg,u32 bits)
{
	vu32 value;
	value = *reg;
	value &=(~bits);
	*reg = value;
	return;
}
///////////////////////////////////////////////////////////////////////////////
// =============================================================================
//Function :Initialize necessary allocated buffer lists for GMAC Driver to transfer
//          data.Must be invoked after GMACD_Init() but before RX/TX start.
//Paramters:pDrive, Pointer to GMAC Driver instance.
//      pInit,  Pointer to sGmacInit.
//      queIdx,  Pointer to gmacQueList_t for different queue.
//Returns  :void
//Notification:If input address is not 8-byte aligned the address is automatically
//        adjusted and the list size is reduced by one.
// =============================================================================
//this function to do the bd initialize,create the buffer describe with the memory
static void __MacBdSndInit(u32 qindex)
{
    tagQueue *queue;
    volatile tagSndBD *sndbd;
    u32       i;
    u32       address;
    u32       status;

	queue = &gMacDriver.queueList[qindex];

	//initialize the send bd
	status = GMAC_TX_USED_BIT;
	for(i =0;i < queue->sndbdlen;i++)
	{
		sndbd = queue->sndbdtab + i;
		address = (u32)(queue->sndmem + queue->sndbuflen*i);
		sndbd->addr = address&GMAC_ADDRESS_MASK;
		sndbd->status.val = status;
	}
	sndbd->status.val = status|GMAC_TX_WRAP_BIT;   //the last one must be wrap
	queue->sndbdcur = 0;
	queue->sndbdlast = 0;

	//set the address to the register
    GMAC_SetTxQueue(gMacDriver.pHw, (u32)queue->sndbdtab, qindex);
    return;
}
static void __MacBdRcvInit(u32 qindex)
{
    tagQueue *queue;
    volatile tagRcvBD *rcvbd;
    u32       i;
    u32       address;
    u32       status;

	queue = &gMacDriver.queueList[qindex];
	//initialize the receive bd
	status = 0;
	for(i =0;i < queue->rcvbdlen;i++)
	{
		rcvbd = queue->rcvbdtab + i;
		address = (u32)(queue->rcvmem + queue->rcvbuflen*i);
		rcvbd->addr.val = address&GMAC_ADDRESS_MASK;
		rcvbd->status.val = status;
	}
	rcvbd->addr.val |= GMAC_RX_WRAP_BIT;   //the last one must be wrap
	queue->rcvbdcur = 0;

	//set the address to the register
	GMAC_SetRxQueue(gMacDriver.pHw, (u32)queue->rcvbdtab, qindex);
    return;
}

extern void PMC_EnablePeripheral(vu8 uID);
static void __MacInitialize(tagMacDriver *pDrive)
{
	Gmac      *pHw;
	tagQueue  *que;
	u32        index;
	u32        value;

	pHw = pDrive->pHw;
    GMAC_DEBUG("Mac Initialize start...\n\r");
	//make it power on
    PMC_EnablePeripheral(pDrive->bId);
	//first,we must stop the device to receive or send
    GMAC_NetworkControl(pHw, 0);
    //disable all the interrupts
    GMAC_DisableAllQueueIt(pHw, ~0u);
    //do the stat clearing
    GMAC_ClearStatistics(pHw);
    /* Clear all status bits in the receive status register. */
    GMAC_ClearRxStatus(pHw, GMAC_RSR_RXOVR | GMAC_RSR_REC
                    | GMAC_RSR_BNA |GMAC_RSR_HNO);
    /* Clear all status bits in the transmit status register */
    GMAC_ClearTxStatus(pHw, GMAC_TSR_UBR | GMAC_TSR_COL | GMAC_TSR_RLE
                            | GMAC_TSR_TXGO | GMAC_TSR_TFC | GMAC_TSR_TXCOMP
                            | GMAC_TSR_HRESP );

    //here we begin to configure the mac device
    for(index =0;index < CN_QUE_NUM;index ++)
    {
        que = &pDrive->queueList[CN_QUE_0];
        GMAC_GetItStatus(pHw, index);   //read for clear
        /*initialize the bd*/
        __MacBdSndInit(index);
        __MacBdRcvInit(index);
        /*set the dma configuration*/
        if(index == CN_QUE_0)
        {
        	value = (GMAC_DCFGR_DRBS(que->rcvbuflen >> 6) )
                | GMAC_DCFGR_RXBMS(3) | GMAC_DCFGR_TXPBMS |GMAC_DCFGR_DDRP|GMAC_DCFGR_FBLDO_INCR4;
        }
        else
        {
        	value = (GMAC_RBSRPQ_RBS(que->rcvbuflen >> 6) );
        }
        GMAC_SetDMAConfig(pHw, value, index);
        //anyway, we open the interrupt,the interrupt could goto the core depends on the
        //interrupt line
        GMAC_EnableIt(pHw,
                  GMAC_INT_RX_BITS |
                  GMAC_INT_TX_BITS |
                  GMAC_INT_TX_ERR_BITS, index);
    }
    /*set the mac address*/
    GMAC_SetAddress(pDrive->pHw,0,pDrive->macaddr);
    /*set the speed and work mode*/
    u32 dwNcfgr;
    dwNcfgr = GMAC_NCFGR_DBW(0) | GMAC_NCFGR_CLK_MCK_64 |
              GMAC_NCFGR_MAXFS | GMAC_NCFGR_PEN | GMAC_NCFGR_RFCS;
    pHw->GMAC_NCFGR = dwNcfgr;
    if (pDrive->macstat.duplex)
	{
		pHw->GMAC_NCFGR |= GMAC_NCFGR_FD;
	}
	else
	{
		pHw->GMAC_NCFGR &= ~GMAC_NCFGR_FD;
	}
	if (pDrive->macstat.speed)
	{
		pHw->GMAC_NCFGR |= GMAC_NCFGR_SPD;
	}
	else
	{
		pHw->GMAC_NCFGR &= ~GMAC_NCFGR_SPD;
	}



	if(pDrive->macstat.mii)
	{
		pHw->GMAC_UR |= GMAC_UR_RMII;
	}
	else
	{
		pHw->GMAC_UR &= ~GMAC_UR_RMII;
	}
	//now,we finished  configure the mac, so enable the receive and send
	pHw->GMAC_NCR |=  (GMAC_NCR_RXEN | GMAC_NCR_TXEN);

	//now return for the call back
	return;
}


void MacReset(tagMacDriver *pDrive)
{
	if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
	{
		__MacInitialize(pDrive);
		pDrive->debuginfo.rsttimes++;
		Lock_MutexPost(pDrive->protect);
	}
	return ;
}

//we use this function to deal with the send errors
static void __GMAC_TxErrorHandler(tagMacDriver *pDrive, u32 qId)
{
    Gmac                   *pHw = pDrive->pHw;

    u32                     tsr;
    GMAC_TransmitEnable(pHw, 0);                //关闭发送功能

    /* The following step should be optional since this function is called
     * directly by the IRQ handler. Indeed, according to Cadence
     * documentation, the transmission is halted on errors such as
     * too many retries or transmit under run.
     * However it would become mandatory if the call of this function
     * were scheduled as a task by the IRQ handler (this is how Linux
     * driver works). Then this function might compete with GMACD_Send().
     *
     * Setting bit 10, tx_halt, of the Network Control Register is not enough:
     * We should wait for bit 3, tx_go, of the Transmit Status Register to
     * be cleared at transmit completion if a frame is being transmitted.
     */
    GMAC_TransmissionHalt(pHw);
    while (GMAC_GetTxStatus(pHw) & GMAC_TSR_TXGO);

    //here we'd better to check which bd got an error here

    /* Reset TX queue */
    __MacBdSndInit(qId);

    /* Clear status */
    tsr = GMAC_GetTxStatus(pHw);
    GMAC_ClearTxStatus(pHw, tsr);

    /* Now we are ready to start transmission again */
    GMAC_TransmitEnable(pHw, 1);
}

//This is the interrut handler
u32 GMAC_IntHandler(ufast_t IntLine)
{
    tagMacDriver *pDrive;
    Gmac *pHw;
    u32 queIdx;
    u32 isr,rsr,tsr;

    pDrive = &gMacDriver;
    queIdx = 0;
    pHw = pDrive->pHw;

    isr = GMAC_GetItStatus(pHw, queIdx);
    pDrive->debuginfo.Ints++;
    //RX Packet 中断
    if(isr & GMAC_INT_RX_BITS)
    {
        rsr = GMAC_GetRxStatus(pHw);
        pDrive->debuginfo.rcvInts++;
        /* Check OVR */
        if (rsr & GMAC_RSR_RXOVR)
        {
            pDrive->debuginfo.rxoverInts++;
        }
        /* Check BNA */
        if (rsr & GMAC_RSR_BNA)
        {
            pDrive->debuginfo.bnaInts++;
            Lock_SempPost(pDrive->rcvsync); //which means there is some data now
        }
        /* Check HNO */
        if (rsr & GMAC_RSR_HNO)
        {
            pDrive->debuginfo.hnoInts++;
        }
        if(rsr & GMAC_RSR_REC)   //收到数据包
        {
            pDrive->debuginfo.recInts++;
            Lock_SempPost(pDrive->rcvsync);
        }
        /* Clear status */
        GMAC->GMAC_RSR = rsr;
    }

    //TX Packet 中断
    if (isr & GMAC_ISR_TCOMP)
    {
        pDrive->debuginfo.sndInts++;
        tsr = GMAC_GetTxStatus(pHw);
        GMAC->GMAC_TSR = tsr;
    }
    //TX ERROR异常中断
    if(isr & GMAC_INT_TX_ERR_BITS)
    {
        __GMAC_TxErrorHandler(pDrive,queIdx);
        pDrive->debuginfo.sndErrInts++;
    }
    return 0;
}

// =============================================================================
// 功能：以太网模块接收数据包，数据包是字节流。接收数据为如下几个步骤：
//       1.检查是MAC是否有往缓冲区写入数据，即是否有b_ownership == 1；
//       2.查询完整数据帧的首包所在环形BD的位置，即status.bm.b_sof == 1；
//       3.查询结束包的位置status.bm.b_eof，并计算完整数据帧的数据长度；
//       4.分配pkg空间，包大小不超过GMAC_TX_UNITSIZE
//       5.将缓冲区的数据写入pkg，并标记b_ownership，清标志
// 参数：packet,接收到数据的首地址
// 返回：接收到数据包长度，最大不会超过1518字节
// =============================================================================
static tagNetPkg *__GmacRcv(ptu32_t devhandle)

{
    tagNetPkg         *pkg;
    tagQueue          *que;
    tagMacDriver      *pDrive;
    volatile tagRcvBD *pRcvBD;
    u16  len;
    u8   *dst;
    u8   *src;
    u16   offset;
    u16   i;

    u32 type = EN_NETDEV_FRAME_NOPKG;
    pkg = NULL;
    pDrive = &gMacDriver;
    que = &pDrive->queueList[CN_QUE_0];
//    GMAC_CACHE_COHERENCE
    SCB_CleanInvalidateDCache_by_Addr((u32*)gRxDs0,sizeof(gRxDs0));
    //each time try your best to find an valid bd to receive the data
    //check from the receive current buffer descriptor will reduce the check time
    offset=que->rcvbdcur;
    for(i=0;i<que->rcvbdlen;i++)
    {
    	pRcvBD = que->rcvbdtab + offset;
        if(pRcvBD->addr.bm.bOwnership) //this bd GOT AN NEW FRAME
        {
            if(pRcvBD->status.bm.bSof&&pRcvBD->status.bm.bEof)
            {
                que->rcvbdcur = offset;
                break;
            }
            else
            {
            	pRcvBD->addr.bm.bOwnership = 0;
            	pRcvBD->status.val = 0;
            }
        }
        offset++;
        if(offset >= que->rcvbdlen)
        {
            offset =0;
        }
    }

    pRcvBD = que->rcvbdtab + que->rcvbdcur;
    SCB_CleanInvalidateDCache_by_Addr((u32*)gRxBuffer0[que->rcvbdcur],CN_RXBUF0_LEN);
    if(pRcvBD->addr.bm.bOwnership) //this buffer descriptor GOT AN NEW FRAME
    {
        if(pRcvBD->status.bm.bSof&&pRcvBD->status.bm.bEof)
        {
            //all the data in the same bd
            len = pRcvBD->status.bm.len;
            pkg =PkgMalloc(len,CN_PKLGLST_END);
            if(NULL != pkg)
            {
                dst = (u8 *)(pkg->buf +pkg->offset);
                src = (u8 *)(pRcvBD->addr.val&GMAC_ADDRESS_MASK);
                memcpy((void *)dst,(void *)src,len);
                pkg->datalen = len;
            }
        }
        else
        {
        	pDrive->debuginfo.badframe ++;
        }
        que->rcvbdcur++;
        if(que->rcvbdcur >= que->rcvbdlen)
        {
            que->rcvbdcur=0;
        }

        //i want to know it is broad or not
        if(pRcvBD->status.bm.bBroadcastDetected)
        {
        	//it is broad
        	type = EN_NETDEV_FRAME_BROAD;
        }
        else if(pRcvBD->status.bm.bMultiHashMatch)
        {
        	//it is broad
        	type = EN_NETDEV_FRAME_MULTI;
        }
        else
        {
        	type = EN_NETDEV_FRAME_POINT;
        }
        pRcvBD->addr.bm.bOwnership = 0;
        pRcvBD->status.val = 0;
    }
//     GMAC_CACHE_COHERENCE
	SCB_CleanInvalidateDCache_by_Addr((u32*)gRxDs0,sizeof(gRxDs0));
	NetDevFilterCounter(pDrive->devhandle,type);

    return pkg;
}

//mac control function
bool_t  GmacCtrl(ptu32_t devhandle,u8 cmd,ptu32_t para)
{
    bool_t result = false;
    tagMacDriver   *pDrive;

    pDrive = &gMacDriver;
    if((pDrive->devhandle == devhandle)&&\
    	(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER)))
    {
        switch(cmd)
        {
        	case EN_NETDEV_SETNOPKG:
            	if(para)
            	{
            		__MacInitialize(pDrive);
                    pDrive->debuginfo.rsttimes++;
            	}
                result = true;
                break;
        	case EN_NETDEV_SETBORAD:
        		if(para)
        		{
        			__macbitsclear(&pDrive->pHw->GMAC_NCFGR,GMAC_NCFGR_NBC);
                    printk("mac broad enable\n\r");
        		}
        		else
        		{
        			__macbitsset(&pDrive->pHw->GMAC_NCFGR,GMAC_NCFGR_NBC);
                    printk("mac broad disable\n\r");
        		}
        		result = true;
        		break;
        	case EN_NETDEV_SETPOINT:
        		if(para)
        		{
        			__macbitsset(&pDrive->pHw->GMAC_NCR,GMAC_NCR_RXEN);
        		}
        		else
        		{
        			__macbitsclear(&pDrive->pHw->GMAC_NCR,GMAC_NCR_RXEN);
        		}
        		result = true;
        		break;
        	case EN_NETDEV_SETMULTI:
        		result = false;
        		break;
        	case EN_NETDEV_SETRECV:
        		if(para)
        		{
        			__macbitsset(&pDrive->pHw->GMAC_NCR,GMAC_NCR_RXEN);
        			__MacInitialize(pDrive);
                    pDrive->debuginfo.rsttimes++;
        		}
        		else
        		{
        			__macbitsclear(&pDrive->pHw->GMAC_NCR,GMAC_NCR_RXEN);
        		}
        		result = true;
        		break;
        	case EN_NETDEV_SETSEND:
        		break;
            case EN_NETDEV_SETMAC:
                memcpy(pDrive->macaddr,(u8 *)para, CN_MACADDR_LEN);
                __MacInitialize(pDrive);
                pDrive->debuginfo.rsttimes++;
                result = true;
                break;
            case EN_NETDEV_SETMULTIMAC:
                break;
            case EN_NETDEV_GTETMAC:
                if((u8 *)para != NULL)
                {
                    memcpy((u8 *)para,pDrive->macaddr, CN_MACADDR_LEN);
                    result = true;
                }
                break;
            case EN_NETDEV_RESET:
            	if(para)
            	{
                    __MacInitialize(pDrive);
                    pDrive->debuginfo.rsttimes++;
                    result = true;
            	}
                break;
            default:
            	result = false;
                break;
        }
        Lock_MutexPost(pDrive->protect);
    }
    return result;
}

//use this function to do the send function
static u8 gTxBuffer[1600];
#define CN_MAC_MAXDELAY 32
static u32 gSndbdDelay[CN_MAC_MAXDELAY];
static u32 gSndbdDelayTotal = 0;

static bool_t MacDelay(char *param)
{
	printf("%-10s%-10s%10d\n\r","Item","Cnt(HEX)",gSndbdDelayTotal);
	u32 i = 0;
	for(i = 0;i <CN_MAC_MAXDELAY;i++)
	{
		printf("%-10d%08x\n\r",i,gSndbdDelay[i]);
	}
	return true;
}

static bool_t MacSndBDClear(char *param)
{
	vu16  bdnum;
    tagMacDriver      *pDrive;
    volatile tagSndBD *pSndBD;
    tagQueue          *q;

    pDrive = &gMacDriver;
	bdnum = strtol(param,NULL,0);
    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
    	 q = &gMacDriver.queueList[CN_QUE_0];
    	 if(bdnum < q->sndbdlen)
    	 {
 			SCB_CleanInvalidateDCache_by_Addr((u32*)gTxDs0,sizeof(gTxDs0));
         	pSndBD = q->sndbdtab + bdnum;
         	pSndBD->status.bm.bUsed = 1;
 			SCB_CleanInvalidateDCache_by_Addr((u32*)gTxDs0,sizeof(gTxDs0));
    	 }

    	Lock_MutexPost(pDrive->protect);
    }
	return true;
}
static bool_t MacSndHalt(char *param)
{
	GMAC_TransmissionHalt(GMAC);
	return true;
}
static bool_t MacSndStart(char *param)
{
	GMAC_TransmissionStart(GMAC);
	return true;
}
static bool_t MacSndEn(char *param)
{
    tagMacDriver      *pDrive;


    pDrive = &gMacDriver;
    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
    	pDrive->pHw->GMAC_NCR |= GMAC_NCR_TXEN;

    	Lock_MutexPost(pDrive->protect);
    }
	return true;
}
static bool_t MacSndDis(char *param)
{
    tagMacDriver      *pDrive;

    pDrive = &gMacDriver;
    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
    	pDrive->pHw->GMAC_NCR &= ~GMAC_NCR_TXEN;

    	Lock_MutexPost(pDrive->protect);
    }
	return true;
}



static bool_t GmacSnd(ptu32_t handle,tagNetPkg * pkg,u32 framelen, u32 netdevtask)
{
    bool_t             result;
    tagMacDriver      *pDrive;
    tagNetPkg         *tmppkg;
    volatile tagSndBD *pSndBD;
    tagQueue          *q;
    u8                *dst,*src;
    u16       		   len;
    u8 				   bdnums,bdcnt;
    result = false;
    pDrive = &gMacDriver;
    pDrive->debuginfo.sndTimes++;
    if((0 == handle)||(NULL == pkg))
        return result;

    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
		SCB_CleanInvalidateDCache_by_Addr((u32*)gTxDs0,sizeof(gTxDs0));

        q = &gMacDriver.queueList[CN_QUE_0];
        // 1.set the bd bused flag mannul
        while(q->sndbdlast != q->sndbdcur)
        {
        	pSndBD = q->sndbdtab + q->sndbdlast;
        	if(pSndBD->status.bm.bUsed)
        	{
        		while(!pSndBD->status.bm.bLastBuffer)
        		{
        			pSndBD->status.bm.bUsed = 1;
		            q->sndbdlast++;
		            if(q->sndbdlast >= q->sndbdlen)
		            {
		                q->sndbdlast =0;
		            }
		            pSndBD = q->sndbdtab + q->sndbdlast;
        		}
        		pSndBD->status.bm.bUsed = 1;
        		pSndBD->status.bm.bLastBuffer = 0;
        	}
        	else
        	{
        		break;		//说明还有未发送完成的包
        	}
            q->sndbdlast++;
            if(q->sndbdlast >= q->sndbdlen)
            {
                q->sndbdlast =0;
            }
        }

        // 2.how many bds in the queue0
        pSndBD = q->sndbdtab + q->sndbdcur;
        bdcnt = q->sndbdcur;
        bdnums = 0;
        while(pSndBD->status.bm.bUsed)
        {
        	bdnums ++;
        	bdcnt ++;
            if(bdcnt >= q->sndbdlen)
            {
            	bdcnt =0;
            }
            if(bdcnt == q->sndbdcur)
            	break;
            pSndBD = q->sndbdtab + bdcnt;
        }

        //3.copy datas to static frame buffer
        tmppkg = pkg;
        dst      = &gTxBuffer[0];
        do
        {
            src = (tmppkg->buf + tmppkg->offset);
            len = tmppkg->datalen;
        	memcpy(dst,src,len);
        	dst      += len;
            if(PKG_ISLISTEND(tmppkg))
            {
                tmppkg = NULL;
                break;
            }
            else
            {
                tmppkg = tmppkg->partnext;
            }
        }while(NULL != tmppkg );

		//4.how many bd needed,and fill the bd, and send
		bdcnt = (framelen + q->sndbuflen - 1)/q->sndbuflen;
		vu16 curbd;
		if( (bdcnt <= bdnums) && (bdcnt > 0))
		{
			curbd = q->sndbdcur;
			src = &gTxBuffer[0];
			while(framelen)
			{
				pSndBD = q->sndbdtab + q->sndbdcur;
				dst =(u8 *)(((unsigned int)(pSndBD->addr))&GMAC_ADDRESS_MASK);
				pSndBD->addr = (unsigned int)dst;

				len = (framelen > q->sndbuflen) ? q->sndbuflen : framelen;
				memcpy((void *)dst,(void *)src,len);
				src		 += len;
				framelen -= len;

				pSndBD->status.bm.bUsed = 0;
				pSndBD->status.bm.len = (len) & 0x3FFF;
				if(framelen > 0)
				{
					pSndBD->status.bm.bLastBuffer = 0;
				}
				else
				{
					pSndBD->status.bm.bLastBuffer = 1;//最后一包
					result = true;
					pDrive->debuginfo.sndOkTimes++;
				}
				SCB_CleanInvalidateDCache_by_Addr((u32*)gTxBuffer0[q->sndbdcur],CN_TXBUF0_LEN);
				q->sndbdcur++;
				if(q->sndbdcur >= q->sndbdlen)
				{
					q->sndbdcur =0;
				}
			}
			SCB_CleanInvalidateDCache_by_Addr((u32*)gTxDs0,sizeof(gTxDs0));
			GMAC_TransmissionStart(GMAC);
			//just  wait the mac to finish
		    u32        delay = 0;
		    pSndBD = q->sndbdtab + curbd;
			while(pSndBD->status.bm.bUsed == 0)
			{
				Djy_DelayUs(1);
				delay++;
				if(delay >= CN_MAC_MAXDELAY)
				{
					printf("Send Bd TimeOUT us! \r\n");
					break;
				}
				SCB_CleanInvalidateDCache_by_Addr((u32*)gTxDs0,sizeof(gTxDs0));
			}
			gSndbdDelay[delay]++;
			gSndbdDelayTotal++;
		}
		else
		{
			pDrive->debuginfo.sndnobdTimes++;
		}
        Lock_MutexPost(pDrive->protect);
    }
    return result;
}

//you could use this function to send data as usual
u32 GMAC_SendData(u8 *buf,u32 len)
{
	tagNetPkg          pkg;
    tagMacDriver      *pDrive;

    pDrive = &gMacDriver;

	pkg.partnext = NULL;
	pkg.pkgflag  = (1<<0);	//只有一个包
	pkg.offset   = 0;
	pkg.datalen  = len;
	pkg.buf      = buf;
	if(GmacSnd(pDrive->devhandle,&pkg,len,0))
	{
		return len;
	}
	else
	{
		return 0;
	}
}

//this is the receive task
static ptu32_t __GmacRcvTask(void)
{
    tagNetPkg *pkg;
    ptu32_t    handle;
    u8        *rawbuf;
    u16        len;
    tagMacDriver      *pDrive;
    pDrive = &gMacDriver;

    Djy_GetEventPara(&handle,NULL);
    while(1)
    {
    	Lock_SempPend(pDrive->rcvsync,pDrive->loopcycle);
        pDrive->debuginfo.rxTaskRun++;
        while(1)
        {
            if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
            {
            	pkg = __GmacRcv(handle);
            	Lock_MutexPost(pDrive->protect);
            }
            if(NULL != pkg)
            {
                if(NULL != pDrive->fnrcvhook)
                {
                    rawbuf = pkg->buf + pkg->offset;
                    len = pkg->datalen;
                    pDrive->fnrcvhook(rawbuf,len);
                }
                else
                {
                    LinkPost(handle,pkg);
                }
                PkgTryFreePart(pkg);
                pDrive->debuginfo.rcvTimes++;
                //check the any filter logic matches
                NetDevFilterCheck(pDrive->devhandle);
            }
            else
            {
                //check the any filter logic matches
                NetDevFilterCheck(pDrive->devhandle);
            	break;
            }
        }
    }
    return 0;
}


//create the receive task
static bool_t __CreateRcvTask(ptu32_t handle)
{
    bool_t result = false;
    u16 evttID;
    u16 eventID;

    evttID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_REAL, 0, 1,
        (ptu32_t (*)(void))__GmacRcvTask,NULL, 0x800, "GMACRcvTask");
    if (evttID != CN_EVTT_ID_INVALID)
    {
        eventID=Djy_EventPop(evttID, NULL,  0,(ptu32_t)handle, 0, 0);
        if(eventID != CN_EVENT_ID_INVALID)
        {
            result = true;
        }
        else
        {
            Djy_EvttUnregist(evttID);
        }
    }
    return result;
}


//for shell to do the restart
bool_t GmacReset(char *param)
{
	tagMacDriver   *pDrive = &gMacDriver;

	MacReset(pDrive);
	return true;
}

//show the gmac status
bool_t gmacdebuginfo(char *param)
{
    s64  time;
    u32  timeS;
    tagMacDriver      *pDrive;
    pDrive = &gMacDriver;

    time = DjyGetSysTime();
    timeS = time/(1000*1000);
    if(timeS == 0)
    {
        timeS =1;
    }
    u32 rcvSpeed = pDrive->debuginfo.rcvTimes/timeS;
    u32 intSpeed = pDrive->debuginfo.Ints/timeS;

    printf("*********MAC DEBUG INFO***********\n\r");
    printf("rcv           :%d\n\r",pDrive->debuginfo.rcvTimes);
    printf("Ints          :%d\n\r",pDrive->debuginfo.Ints);
    printf("rcvInts       :%d\n\r",pDrive->debuginfo.rcvInts);
    printf("hnoInrs       :%d\n\r",pDrive->debuginfo.hnoInts);
    printf("rxoInts       :%d\n\r",pDrive->debuginfo.rxoverInts);
    printf("recInts       :%d\n\r",pDrive->debuginfo.recInts);
    printf("bnaInts       :%d\n\r",pDrive->debuginfo.bnaInts);
    printf("snd           :%d\n\r",pDrive->debuginfo.sndTimes);
    printf("sndInts       :%d\n\r",pDrive->debuginfo.sndInts);
    printf("sndErrs       :%d\n\r",pDrive->debuginfo.sndErrInts);
    printf("sndTimes      :%d\n\r",pDrive->debuginfo.sndTimes);
    printf("sndOkTime     :%d\n\r",pDrive->debuginfo.sndOkTimes);
    printf("sndnobdTimes  :%d\n\r",pDrive->debuginfo.sndnobdTimes);
    printf("rxTaskRun     :%d\n\r",pDrive->debuginfo.rxTaskRun);
    printf("rcvSpeed      :%d\n\r",rcvSpeed);
    printf("intSpeed      :%d\n\r",intSpeed);
    printf("rstTimes      :%d\n\r",pDrive->debuginfo.rsttimes);
    printf("**********************************\n\r");
    return true;
}

//show the gmac reg
#define CN_GMAC_REG_BASE   0X40050000
#define CN_GMAC_SHOW_NUM   0x20
bool_t gmacreg(char *param)
{
    vu32    i;
    vu32   *addr;
    vu32    value;

    printf("%-10s%-10s\n\r",\
    		"Addr(Hex)","Value(Hex)");
    addr = (u32 *)CN_GMAC_REG_BASE;
    for(i=0;i < CN_GMAC_SHOW_NUM;i++)
    {
        value =*addr;
        printf("%08x  %08x\n\r",(u32)addr,value);
        addr++;
    }
    return true;
}
//post the receive semp
bool_t  gmacpost(char *param)
{
    tagMacDriver      *pDrive;
    pDrive = &gMacDriver;
    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
        Lock_SempPost(pDrive->rcvsync);
    	Lock_MutexPost(pDrive->protect);
    }

    return true;
}
//check the receive bd
bool_t gmacrcvbdcheck(char *param)
{
    tagQueue          *que;
    tagRcvBD          *pRcvBD;
    void              *mem;
    u32                i = 0;
    u32                len;
    tagMacDriver      *pDrive;
    pDrive = &gMacDriver;
    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
        que = &pDrive->queueList[CN_QUE_0];
        len = que->rcvbdlen*sizeof(tagRcvBD);

        SCB_CleanInvalidateDCache_by_Addr((u32*)que->rcvbdtab,len); //cache sync
        //do the copy and print the result
        mem = (void*)malloc(len);
        if(NULL != mem)
        {
        	pRcvBD = (tagRcvBD *)mem;
        	memcpy(mem,(void *)que->rcvbdtab,len);
        }
    	Lock_MutexPost(pDrive->protect);
    	if(NULL != mem)
    	{
        	printf("Rcvbd:total:%08d CurID:%08d Time:0x%08llx\n\r",\
        			que->rcvbdlen,que->rcvbdcur,DjyGetSysTime());
            printf("%-10s%-10s%-10s%-10s%-10s%-10s\n\r",\
            		"RcvBdNum","Addr(hex)","BAddr(hex)","State","Last","BValue");
            for(i =0; i < que->rcvbdlen;i++)
            {
                printf("%-08d  %-08x  %-08x  %-10s%-10s%-08x  \n\r",\
                		i,\
						(u32)(que->rcvbdtab + i),\
						pRcvBD->addr.bm.addrDW<<2,\
						pRcvBD->addr.bm.bOwnership?"busy":"free",\
                		pRcvBD->addr.bm.bWrap?"Yes":"No",\
                		pRcvBD->status.val);
                pRcvBD++;
            }
            free(mem);
    	}
    	else
    	{
    		printf("%s:mem error\n\r",__FUNCTION__);
    	}
    }
    return true;
}
//check the receive bd
bool_t gmacsndbdcheck(char *param)
{
    tagQueue          *que;
    tagSndBD          *pSndBD;
    void              *mem;
    u32                i = 0;
    u32                len;
    tagMacDriver      *pDrive;
    pDrive = &gMacDriver;
    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
        que = &pDrive->queueList[CN_QUE_0];
        len = que->sndbdlen*sizeof(tagSndBD);
        SCB_CleanInvalidateDCache_by_Addr((u32*)que->sndbdtab,len);
        //do the copy and print the result
        mem = (tagSndBD*)malloc(len);
        if(NULL != mem)
        {
        	memcpy(mem,(void *)que->sndbdtab,len);
        }
    	Lock_MutexPost(pDrive->protect);

    	if(NULL != mem)
    	{
        	pSndBD = (tagSndBD *)mem;
        	printf("Sndbd:total:%08d CurID:%08d  LastID:%08d Time:0x%08x\n\r",\
        			que->sndbdlen,que->sndbdcur,que->sndbdlast,DjyGetSysTime());
            printf("%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s%-10s\n\r",\
            		"SndBdNum","State","Addr(hex)","BAddr(hex)","Len(hex)","NoCrc","Exhausted","Error","UnderRun","Last","Wrap");
            for(i =0; i < que->sndbdlen;i++)
            {
                printf("%-08d  %-10s%-08x  %-08x  %-08x  %-10s%-10s%-10s%-10s%-10s%-10s\n\r",\
                		i,pSndBD->status.bm.bUsed?"free":"busy",\
                		(u32)(que->sndbdtab +i),\
                		(u32)pSndBD->addr,\
                		pSndBD->status.bm.len,\
						pSndBD->status.bm.bNoCRC?"Yes":"No",\
						pSndBD->status.bm.bExhausted?"Yes":"No",\
						pSndBD->status.bm.bError?"Yes":"No",\
						pSndBD->status.bm.bUnderrun?"Yes":"No",\
						pSndBD->status.bm.bLastBuffer?"Yes":"No",\
						pSndBD->status.bm.bWrap?"Yes":"No"\
						);
                pSndBD++;
            }
            free(mem);
    	}
    	else
    	{
    		printf("%s:mem error\n\r",__FUNCTION__);
    	}
    }
    return true;
}

#include <shell.h>
static struct ShellCmdTab  gGmacDebug[] =
{
    {
        "mac",
        gmacdebuginfo,
        "usage:gmac",
        NULL
    },
    {
        "macreg",
        gmacreg,
        "usage:gmacreg",
        NULL
    },
    {
        "macpost",
        gmacpost,
        "usage:gmacpost",
        NULL
    },
    {
        "macrcvbd",
        gmacrcvbdcheck,
        "usage:gmacrcvbd",
        NULL
    },
    {
        "macsndbd",
        gmacsndbdcheck,
        "usage:gmacsndbd",
        NULL
    },
    {
        "macreset",
		GmacReset,
        "usage:reset gmac",
        NULL
    },
    {
        "macdelay",
		MacDelay,
        "usage:MacDelay",
        NULL
    },
    {
        "macsndbdclear",
		MacSndBDClear,
        "usage:MacSndBdClear + ndnum",
        NULL
    },
    {
        "macsndhalt",
		MacSndHalt,
        "usage:MacSndHalt",
        NULL
    },
    {
        "macsndstart",
		MacSndStart,
        "usage:MacSndStart",
        NULL
    },
    {
        "macsnden",
		MacSndEn,
        "usage:MacSndEn",
        NULL
    },
    {
        "macsnddis",
		MacSndDis,
        "usage:MacSndDis",
        NULL
    },
};


#define CN_GMACDEBUG_NUM  ((sizeof(gGmacDebug))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc gGmacDebugCmdRsc[CN_GMACDEBUG_NUM];
// =============================================================================
// 功能：GMAC网卡和DJYIP驱动初始化函数
// 参数：para
// 返回值  ：true成功  false失败。
// =============================================================================
bool_t ModuleInstall_GMAC(const char *devname, u8 *mac,\
                          bool_t loop,u32 loopcycle,\
                          bool_t (*rcvHook)(u8 *buf, u16 len))
{
    tagMacDriver   *pDrive = &gMacDriver;
    tagNetDevPara   devpara;

    memset((void *)pDrive,0,sizeof(tagMacDriver));
    //copy the config para to the pDrive
    memcpy(pDrive->devname,devname,CN_DEVNAME_LEN-1);
    memcpy((void *)pDrive->macaddr,mac,CN_MACADDR_LEN);
    if(loop)
    {
    	pDrive->loop = 1;
    }
    pDrive->loopcycle = loopcycle;
    pDrive->fnrcvhook = rcvHook;
    //set the work mode and speed
    pDrive->macstat.caf = 0;       //no cpy all the frame
    pDrive->macstat.duplex = 1;    //duplex full
    pDrive->macstat.speed = 1;     //100 Mb
    pDrive->macstat.nbc = 0;       //permit broadcast
    pDrive->macstat.mii = 0;       //use RMII mode
    //hardware register set
    pDrive->bId = ID_GMAC;
    pDrive->pHw = (Gmac *)GMAC;
    //initialize the memory buffer
    //que-0
    pDrive->queueList[0].sndmem = (vu8*)gTxBuffer0;
    pDrive->queueList[0].sndmemlen = sizeof(gTxBuffer0);
    pDrive->queueList[0].sndbuflen = CN_TXBUF0_LEN;
    pDrive->queueList[0].sndbdtab = gTxDs0;
    pDrive->queueList[0].sndbdlen = CN_TXBD0_LEN;
    pDrive->queueList[0].rcvmem = (vu8*)gRxBuffer0;
    pDrive->queueList[0].rcvmemlen = sizeof(gRxBuffer0);
    pDrive->queueList[0].rcvbuflen = CN_RXBUF0_LEN;
    pDrive->queueList[0].rcvbdtab = gRxDs0;
    pDrive->queueList[0].rcvbdlen = CN_RXBD0_LEN;
    //que -1
    pDrive->queueList[1].sndmem = (vu8*)gTxBuffer1;
    pDrive->queueList[1].sndmemlen = sizeof(gTxBuffer1);
    pDrive->queueList[1].sndbuflen = CN_TXBUF1_LEN;
    pDrive->queueList[1].sndbdtab = gTxDs1;
    pDrive->queueList[1].sndbdlen = CN_TXBD1_LEN;
    pDrive->queueList[1].rcvmem = (vu8*)gRxBuffer1;
    pDrive->queueList[1].rcvmemlen = sizeof(gRxBuffer1);
    pDrive->queueList[1].rcvbuflen = CN_RXBUF1_LEN;
    pDrive->queueList[1].rcvbdtab = gRxDs1;
    pDrive->queueList[1].rcvbdlen = CN_RXBD1_LEN;
    //que-2
    pDrive->queueList[2].sndmem = (vu8*)gTxBuffer2;
    pDrive->queueList[2].sndmemlen = sizeof(gTxBuffer2);
    pDrive->queueList[2].sndbuflen = CN_TXBUF2_LEN;
    pDrive->queueList[2].sndbdtab = gTxDs2;
    pDrive->queueList[2].sndbdlen = CN_TXBD2_LEN;
    pDrive->queueList[2].rcvmem = (vu8*)gRxBuffer2;
    pDrive->queueList[2].rcvmemlen = sizeof(gRxBuffer2);
    pDrive->queueList[2].rcvbuflen = CN_RXBUF2_LEN;
    pDrive->queueList[2].rcvbdtab = gRxDs2;
    pDrive->queueList[2].rcvbdlen = CN_RXBD2_LEN;

    //all the configuration has set in the pDrive now,we need some sys assistant
    //application some semphore and mutex
    pDrive->rcvsync = Lock_SempCreate(1,1,CN_SEMP_BLOCK_FIFO,NULL);
    if(NULL == pDrive->rcvsync)
    {
        goto RCVSYNC_FAILED;
    }
    pDrive->protect = Lock_MutexCreate(NULL);
    if(NULL == pDrive->protect)
    {
        goto DEVPROTECT_FAILED;
    }
    //do the hardware initialize
    __MacInitialize(pDrive);
    //install the net device
    devpara.ifctrl = GmacCtrl;
    devpara.ifsend = GmacSnd;
    devpara.iftype = EN_LINK_ETHERNET;
    devpara.devfunc = 0;    //NO FUNC FOR THE DEV
    memcpy(devpara.mac, pDrive->macaddr,6);
    devpara.name = (char *)pDrive->devname;
    devpara.private = 0;
    devpara.mtu = 1522;
    devpara.private = (ptu32_t)pDrive;
    pDrive->devhandle = NetDevInstall(&devpara);
    if(0 == pDrive->devhandle)
    {
        goto NetInstallFailed;
    }
    if(false == __CreateRcvTask(pDrive->devhandle))
    {
        goto RcvTaskFailed;
    }

    if(false == loop)
    {
	    Int_Register(CN_INT_LINE_GMAC);
        Int_SettoAsynSignal(CN_INT_LINE_GMAC);
        Int_ClearLine(CN_INT_LINE_GMAC);
        Int_IsrConnect(CN_INT_LINE_GMAC,GMAC_IntHandler);
        Int_ContactLine(CN_INT_LINE_GMAC);
    }
    Sh_InstallCmd(gGmacDebug,gGmacDebugCmdRsc,CN_GMACDEBUG_NUM);
    printf("%s:Install Net Device %s success\n\r",__FUNCTION__,devname);
    return true;

RcvTaskFailed:
    NetDevUninstall(devname);
NetInstallFailed:
    Lock_MutexDelete(pDrive->protect);
    pDrive->protect = NULL;
DEVPROTECT_FAILED:
    Lock_SempDelete(pDrive->rcvsync);
    pDrive->rcvsync = NULL;
RCVSYNC_FAILED:
    printf("%s:Install Net Device %s failed\n\r",__FUNCTION__,devname);
    return false;
}


//install a new hook
bool_t GmacInstall_Hook(bool_t (*rcvHook)(u8 *buf, u16 len))
{
	tagMacDriver   *pDrive = &gMacDriver;

	if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
	{
		if(rcvHook != NULL)
		{
			 pDrive->fnrcvhook= rcvHook;
		}

		Lock_MutexPost(pDrive->protect);
	}
	return true;
}
//uninstall the hook
bool_t GmacUnInstall_Hook(void)
{
	tagMacDriver   *pDrive = &gMacDriver;

	if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
	{
		if(pDrive->fnrcvhook != NULL)
		{
			pDrive->fnrcvhook = NULL;
		}

		Lock_MutexPost(pDrive->protect);
	}
	return true;
}

//set the mac filter:caf or nbc
void GmacInstall_Cfg(bool_t EnCAF, bool_t EnNBC)
{
	tagMacDriver   *pDrive = &gMacDriver;

	if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
	{
		if(EnCAF)
		{
			pDrive->macstat.caf = 1;
		}
		else
		{
			pDrive->macstat.caf = 0;
		}

		if(EnNBC)
		{
			pDrive->macstat.nbc = 1;
		}
		else
		{
			pDrive->macstat.nbc = 0;
		}

		__MacInitialize(pDrive);
		Lock_MutexPost(pDrive->protect);
	}
	return;
}


/////////////////////////////MDIO interface for external////////////////////////
//interface for other modules
extern void GMAC_EnableMdio( Gmac *pGmac );
extern void GMAC_DisableMdio( Gmac *pGmac );
extern void GMAC_PHYMaintain(Gmac      *pGmac,
							 uint8_t   bPhyAddr,
							 uint8_t   bRegAddr,
							 uint8_t   bRW,
							 uint16_t  wData);
extern uint16_t GMAC_PHYData(Gmac *pGmac);
bool_t GMAC_MdioR(u8 dev,u8 reg, u16 *value)
{
    bool_t    result = false;
	tagMacDriver   *pDrive = &gMacDriver;

    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
        GMAC_EnableMdio(GMAC);

        GMAC_PHYMaintain(GMAC, dev, reg, 1, 0);
        *value = GMAC_PHYData(GMAC);

        GMAC_DisableMdio(GMAC);

    	Lock_MutexPost(pDrive->protect);

    	result = true;
    }
    return result;
}

bool_t GMAC_MdioW(u8 dev,u8 reg, u16 value)
{
    bool_t    result = false;
	tagMacDriver   *pDrive = &gMacDriver;

    if(Lock_MutexPend(pDrive->protect,CN_TIMEOUT_FOREVER))
    {
        GMAC_EnableMdio(GMAC);

        GMAC_PHYMaintain(GMAC, dev, reg, 0, value);
        GMAC_PHYData(GMAC);

        GMAC_DisableMdio(GMAC);

    	Lock_MutexPost(pDrive->protect);
    	result = true;
    }
    return result;
}






