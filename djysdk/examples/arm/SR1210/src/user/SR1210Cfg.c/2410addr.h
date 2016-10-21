//=============================================================================
// File Name : 2410addr.h
// Function  : S3C2410 Define Address Register
// Program   : Shin, On Pil (SOP)
// Date      : May 06, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (February 15,2002) -> SOP
//         INTERRUPT rPRIORITY 0x4a00000a -> 0x4a00000c       (May 02, 2002 SOP)
//         RTC BCD DAY and DATE Register Name Correction      (May 06, 2002 SOP)
//=============================================================================

#ifndef __2410ADDR_H__
#define __2410ADDR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define S3C2410_HDR_BASE             0x08000000
#define HDR_SIZE_TOTAL               0x28000000

#define S3C2410_SC_BASE             0x48000000
#define SC_SIZE_TOTAL               0x18000000

#define S3C2410_SR_BASE             0x40000000

// Memory control
#define rBWSCON    (0x48000000) //Bus width & wait status
#define rBANKCON0  (0x48000004) //Boot ROM control
#define rBANKCON1  (0x48000008) //BANK1 control
#define rBANKCON2  (0x4800000c) //BANK2 cControl
#define rBANKCON3  (0x48000010) //BANK3 control
#define rBANKCON4  (0x48000014) //BANK4 control
#define rBANKCON5  (0x48000018) //BANK5 control
#define rBANKCON6  (0x4800001c) //BANK6 control
#define rBANKCON7  (0x48000020) //BANK7 control
#define rREFRESH   (0x48000024) //DRAM/SDRAM refresh
#define rBANKSIZE  (0x48000028) //Flexible Bank Size
#define rMRSRB6    (0x4800002c) //Mode register set for SDRAM
#define rMRSRB7    (0x48000030) //Mode register set for SDRAM


// USB Host


// INTERRUPT
#define rSRCPND     (0x4a000000) //Interrupt request status
#define rINTMOD     (0x4a000004) //Interrupt mode control
#define rINTMSK     (0x4a000008) //Interrupt mask control
#define rPRIORITY   (0x4a00000c) //IRQ priority control
#define rINTPND     (0x4a000010) //Interrupt request status
#define rINTOFFSET  (0x4a000014) //Interruot request source offset
#define rSUBSRCPND  (0x4a000018) //Sub source pending
#define rINTSUBMSK  (0x4a00001c) //Interrupt sub mask


// DMA
#define rDISRC0     (0x4b000000) //DMA 0 Initial source
#define rDISRCC0    (0x4b000004) //DMA 0 Initial source control
#define rDIDST0     (0x4b000008) //DMA 0 Initial Destination
#define rDIDSTC0    (0x4b00000c) //DMA 0 Initial Destination control
#define rDCON0      (0x4b000010) //DMA 0 Control
#define rDSTAT0     (0x4b000014) //DMA 0 Status
#define rDCSRC0     (0x4b000018) //DMA 0 Current source
#define rDCDST0     (0x4b00001c) //DMA 0 Current destination
#define rDMASKTRIG0 (0x4b000020) //DMA 0 Mask trigger

#define rDISRC1     (0x4b000040) //DMA 1 Initial source
#define rDISRCC1    (0x4b000044) //DMA 1 Initial source control
#define rDIDST1     (0x4b000048) //DMA 1 Initial Destination
#define rDIDSTC1    (0x4b00004c) //DMA 1 Initial Destination control
#define rDCON1      (0x4b000050) //DMA 1 Control
#define rDSTAT1     (0x4b000054) //DMA 1 Status
#define rDCSRC1     (0x4b000058) //DMA 1 Current source
#define rDCDST1     (0x4b00005c) //DMA 1 Current destination
#define rDMASKTRIG1 (0x4b000060) //DMA 1 Mask trigger

#define rDISRC2     (0x4b000080) //DMA 2 Initial source
#define rDISRCC2    (0x4b000084) //DMA 2 Initial source control
#define rDIDST2     (0x4b000088) //DMA 2 Initial Destination
#define rDIDSTC2    (0x4b00008c) //DMA 2 Initial Destination control
#define rDCON2      (0x4b000090) //DMA 2 Control
#define rDSTAT2     (0x4b000094) //DMA 2 Status
#define rDCSRC2     (0x4b000098) //DMA 2 Current source
#define rDCDST2     (0x4b00009c) //DMA 2 Current destination
#define rDMASKTRIG2 (0x4b0000a0) //DMA 2 Mask trigger

#define rDISRC3     (0x4b0000c0) //DMA 3 Initial source
#define rDISRCC3    (0x4b0000c4) //DMA 3 Initial source control
#define rDIDST3     (0x4b0000c8) //DMA 3 Initial Destination
#define rDIDSTC3    (0x4b0000cc) //DMA 3 Initial Destination control
#define rDCON3      (0x4b0000d0) //DMA 3 Control
#define rDSTAT3     (0x4b0000d4) //DMA 3 Status
#define rDCSRC3     (0x4b0000d8) //DMA 3 Current source
#define rDCDST3     (0x4b0000dc) //DMA 3 Current destination
#define rDMASKTRIG3 (0x4b0000e0) //DMA 3 Mask trigger


// CLOCK & POWER MANAGEMENT
#define rLOCKTIME   (0x4c000000) //PLL lock time counter
#define rMPLLCON    (0x4c000004) //MPLL Control
#define rUPLLCON    (0x4c000008) //UPLL Control
#define rCLKCON     (0x4c00000c) //Clock generator control
#define rCLKSLOW    (0x4c000010) //Slow clock control
#define rCLKDIVN    (0x4c000014) //Clock divider control


// LCD CONTROLLER
#define rLCDCON1    (0x4d000000) //LCD control 1
#define rLCDCON2    (0x4d000004) //LCD control 2
#define rLCDCON3    (0x4d000008) //LCD control 3
#define rLCDCON4    (0x4d00000c) //LCD control 4
#define rLCDCON5    (0x4d000010) //LCD control 5
#define rLCDSADDR1  (0x4d000014) //STN/TFT Frame buffer start address 1
#define rLCDSADDR2  (0x4d000018) //STN/TFT Frame buffer start address 2
#define rLCDSADDR3  (0x4d00001c) //STN/TFT Virtual screen address set
#define rREDLUT     (0x4d000020) //STN Red lookup table
#define rGREENLUT   (0x4d000024) //STN Green lookup table
#define rBLUELUT    (0x4d000028) //STN Blue lookup table
#define rDITHMODE   (0x4d00004c) //STN Dithering mode
#define rTPAL       (0x4d000050) //TFT Temporary palette
#define rLCDINTPND  (0x4d000054) //LCD Interrupt pending
#define rLCDSRCPND  (0x4d000058) //LCD Interrupt source
#define rLCDINTMSK  (0x4d00005c) //LCD Interrupt mask
#define rLPCSEL     (0x4d000060) //LPC3600 Control
#define PALETTE     0x4d000400                         //Palette start address


// NAND flash
#define rNFCONF     (0x4e000000)      //NAND Flash configuration
#define rNFCMD      (0x4e000004)            //NADD Flash command
#define rNFADDR     (0x4e000008)            //NAND Flash address
#define rNFDATA     (0x4e00000c)            //NAND Flash data
#define rNFSTAT     (0x4e000010)      //NAND Flash operation status
#define rNFECC      (0x4e000014)      //NAND Flash ECC
#define rNFECC0     (0x4e000014)
#define rNFECC1     (0x4e000015)
#define rNFECC2     (0x4e000016)

// UART
#define SERIAL_A_BASE_ADR  0x50000000
#define SERIAL_B_BASE_ADR  0x50004000
#define SERIAL_C_BASE_ADR  0x50008000
#define UART_REG_ADDR_INTERVAL  4        /* registers 4 bytes apart */

#define SNGS3C_ULCON            (0x00)
#define SNGS3C_UCON             (0x04)
#define SNGS3C_UFCON            (0x08)
#define SNGS3C_UMCON            (0x0C)
#define SNGS3C_UTRSTAT          (0x10)
#define SNGS3C_UERSTAT          (0x14)
#define SNGS3C_UFSTAT           (0x18)
#define SNGS3C_UMSTAT           (0x1C)
#define SNGS3C_UBRDIV           (0x28)
#define SNGS3C_UTXBUF           (0x20)
#define SNGS3C_URXBUF           (0x24)

#define rULCON0     (0x50000000) //UART 0 Line control
#define rUCON0      (0x50000004) //UART 0 Control
#define rUFCON0     (0x50000008) //UART 0 FIFO control
#define rUMCON0     (0x5000000c) //UART 0 Modem control
#define rUTRSTAT0   (0x50000010) //UART 0 Tx/Rx status
#define rUERSTAT0   (0x50000014) //UART 0 Rx error status
#define rUFSTAT0    (0x50000018) //UART 0 FIFO status
#define rUMSTAT0    (0x5000001c) //UART 0 Modem status
#define rUBRDIV0    (0x50000028) //UART 0 Baud rate divisor

#define rULCON1     (0x50004000) //UART 1 Line control
#define rUCON1      (0x50004004) //UART 1 Control
#define rUFCON1     (0x50004008) //UART 1 FIFO control
#define rUMCON1     (0x5000400c) //UART 1 Modem control
#define rUTRSTAT1   (0x50004010) //UART 1 Tx/Rx status
#define rUERSTAT1   (0x50004014) //UART 1 Rx error status
#define rUFSTAT1    (0x50004018) //UART 1 FIFO status
#define rUMSTAT1    (0x5000401c) //UART 1 Modem status
#define rUBRDIV1    (0x50004028) //UART 1 Baud rate divisor

#define rULCON2     (0x50008000) //UART 2 Line control
#define rUCON2      (0x50008004) //UART 2 Control
#define rUFCON2     (0x50008008) //UART 2 FIFO control
#define rUMCON2     (0x5000800c) //UART 2 Modem control
#define rUTRSTAT2   (0x50008010) //UART 2 Tx/Rx status
#define rUERSTAT2   (0x50008014) //UART 2 Rx error status
#define rUFSTAT2    (0x50008018) //UART 2 FIFO status
#define rUMSTAT2    (0x5000801c) //UART 2 Modem status
#define rUBRDIV2    (0x50008028) //UART 2 Baud rate divisor

#ifdef __BIG_ENDIAN
#define rUTXH0      (0x50000023) //UART 0 Transmission Hold
#define rURXH0      (0x50000027) //UART 0 Receive buffer
#define rUTXH1      (0x50004023) //UART 1 Transmission Hold
#define rURXH1      (0x50004027) //UART 1 Receive buffer
#define rUTXH2      (0x50008023) //UART 2 Transmission Hold
#define rURXH2      (0x50008027) //UART 2 Receive buffer

#define UTXH0       (0x50000020+3)  //Byte_access address by DMA
#define URXH0       (0x50000024+3)
#define UTXH1       (0x50004020+3)
#define URXH1       (0x50004024+3)
#define UTXH2       (0x50008020+3)
#define URXH2       (0x50008024+3)

#else //Little Endian
#define rUTXH0 (0x50000020) //UART 0 Transmission Hold
#define rURXH0 (0x50000024) //UART 0 Receive buffer
#define rUTXH1 (0x50004020) //UART 1 Transmission Hold
#define rURXH1 (0x50004024) //UART 1 Receive buffer
#define rUTXH2 (0x50008020) //UART 2 Transmission Hold
#define rURXH2 (0x50008024) //UART 2 Receive buffer

#define UTXH0       (0x50000020)    //Byte_access address by DMA
#define URXH0       (0x50000024)
#define UTXH1       (0x50004020)
#define URXH1       (0x50004024)
#define UTXH2       (0x50008020)
#define URXH2       (0x50008024)
#endif

// PWM TIMER
#define rTCFG0  (0x51000000) //Timer 0 configuration
#define rTCFG1  (0x51000004) //Timer 1 configuration
#define rTCON   (0x51000008) //Timer control
#define rTCNTB0 (0x5100000c) //Timer count buffer 0
#define rTCMPB0 (0x51000010) //Timer compare buffer 0
#define rTCNTO0 (0x51000014) //Timer count observation 0
#define rTCNTB1 (0x51000018) //Timer count buffer 1
#define rTCMPB1 (0x5100001c) //Timer compare buffer 1
#define rTCNTO1 (0x51000020) //Timer count observation 1
#define rTCNTB2 (0x51000024) //Timer count buffer 2
#define rTCMPB2 (0x51000028) //Timer compare buffer 2
#define rTCNTO2 (0x5100002c) //Timer count observation 2
#define rTCNTB3 (0x51000030) //Timer count buffer 3
#define rTCMPB3 (0x51000034) //Timer compare buffer 3
#define rTCNTO3 (0x51000038) //Timer count observation 3
#define rTCNTB4 (0x5100003c) //Timer count buffer 4
#define rTCNTO4 (0x51000040) //Timer count observation 4


// USB DEVICE
#ifdef __BIG_ENDIAN
<ERROR IF BIG_ENDIAN>
#define rFUNC_ADDR_REG     (0x52000143) //Function address
#define rPWR_REG           (0x52000147) //Power management
#define rEP_INT_REG        (0x5200014b) //EP Interrupt pending and clear
#define rUSB_INT_REG       (0x5200015b) //USB Interrupt pending and clear
#define rEP_INT_EN_REG     (0x5200015f) //Interrupt enable
#define rUSB_INT_EN_REG    (0x5200016f)
#define rFRAME_NUM1_REG    (0x52000173) //Frame number lower byte
#define rFRAME_NUM2_REG    (0x52000177) //Frame number higher byte
#define rINDEX_REG         (0x5200017b) //Register index
#define rMAXP_REG          (0x52000183) //Endpoint max packet
#define rEP0_CSR           (0x52000187) //Endpoint 0 status
#define rIN_CSR1_REG       (0x52000187) //In endpoint control status
#define rIN_CSR2_REG       (0x5200018b)
#define rOUT_CSR1_REG      (0x52000193) //Out endpoint control status
#define rOUT_CSR2_REG      (0x52000197)
#define rOUT_FIFO_CNT1_REG (0x5200019b) //Endpoint out write count
#define rOUT_FIFO_CNT2_REG (0x5200019f)
#define rEP0_FIFO          (0x520001c3) //Endpoint 0 FIFO
#define rEP1_FIFO          (0x520001c7) //Endpoint 1 FIFO
#define rEP2_FIFO          (0x520001cb) //Endpoint 2 FIFO
#define rEP3_FIFO          (0x520001cf) //Endpoint 3 FIFO
#define rEP4_FIFO          (0x520001d3) //Endpoint 4 FIFO
#define rEP1_DMA_CON       (0x52000203) //EP1 DMA interface control
#define rEP1_DMA_UNIT      (0x52000207) //EP1 DMA Tx unit counter
#define rEP1_DMA_FIFO      (0x5200020b) //EP1 DMA Tx FIFO counter
#define rEP1_DMA_TTC_L     (0x5200020f) //EP1 DMA total Tx counter
#define rEP1_DMA_TTC_M     (0x52000213)
#define rEP1_DMA_TTC_H     (0x52000217)
#define rEP2_DMA_CON       (0x5200021b) //EP2 DMA interface control
#define rEP2_DMA_UNIT      (0x5200021f) //EP2 DMA Tx unit counter
#define rEP2_DMA_FIFO      (0x52000223) //EP2 DMA Tx FIFO counter
#define rEP2_DMA_TTC_L     (0x52000227) //EP2 DMA total Tx counter
#define rEP2_DMA_TTC_M     (0x5200022b)
#define rEP2_DMA_TTC_H     (0x5200022f)
#define rEP3_DMA_CON       (0x52000243) //EP3 DMA interface control
#define rEP3_DMA_UNIT      (0x52000247) //EP3 DMA Tx unit counter
#define rEP3_DMA_FIFO      (0x5200024b) //EP3 DMA Tx FIFO counter
#define rEP3_DMA_TTC_L     (0x5200024f) //EP3 DMA total Tx counter
#define rEP3_DMA_TTC_M     (0x52000253)
#define rEP3_DMA_TTC_H     (0x52000257)
#define rEP4_DMA_CON       (0x5200025b) //EP4 DMA interface control
#define rEP4_DMA_UNIT      (0x5200025f) //EP4 DMA Tx unit counter
#define rEP4_DMA_FIFO      (0x52000263) //EP4 DMA Tx FIFO counter
#define rEP4_DMA_TTC_L     (0x52000267) //EP4 DMA total Tx counter
#define rEP4_DMA_TTC_M     (0x5200026b)
#define rEP4_DMA_TTC_H     (0x5200026f)

#else  // Little Endian
#define rFUNC_ADDR_REG     (0x52000140) //Function address
#define rPWR_REG           (0x52000144) //Power management
#define rEP_INT_REG        (0x52000148) //EP Interrupt pending and clear
#define rUSB_INT_REG       (0x52000158) //USB Interrupt pending and clear
#define rEP_INT_EN_REG     (0x5200015c) //Interrupt enable
#define rUSB_INT_EN_REG    (0x5200016c)
#define rFRAME_NUM1_REG    (0x52000170) //Frame number lower byte
#define rFRAME_NUM2_REG    (0x52000174) //Frame number higher byte
#define rINDEX_REG         (0x52000178) //Register index
#define rMAXP_REG          (0x52000180) //Endpoint max packet
#define rEP0_CSR           (0x52000184) //Endpoint 0 status
#define rIN_CSR1_REG       (0x52000184) //In endpoint control status
#define rIN_CSR2_REG       (0x52000188)
#define rOUT_CSR1_REG      (0x52000190) //Out endpoint control status
#define rOUT_CSR2_REG      (0x52000194)
#define rOUT_FIFO_CNT1_REG (0x52000198) //Endpoint out write count
#define rOUT_FIFO_CNT2_REG (0x5200019c)
#define rEP0_FIFO          (0x520001c0) //Endpoint 0 FIFO
#define rEP1_FIFO          (0x520001c4) //Endpoint 1 FIFO
#define rEP2_FIFO          (0x520001c8) //Endpoint 2 FIFO
#define rEP3_FIFO          (0x520001cc) //Endpoint 3 FIFO
#define rEP4_FIFO          (0x520001d0) //Endpoint 4 FIFO
#define rEP1_DMA_CON       (0x52000200) //EP1 DMA interface control
#define rEP1_DMA_UNIT      (0x52000204) //EP1 DMA Tx unit counter
#define rEP1_DMA_FIFO      (0x52000208) //EP1 DMA Tx FIFO counter
#define rEP1_DMA_TTC_L     (0x5200020c) //EP1 DMA total Tx counter
#define rEP1_DMA_TTC_M     (0x52000210)
#define rEP1_DMA_TTC_H     (0x52000214)
#define rEP2_DMA_CON       (0x52000218) //EP2 DMA interface control
#define rEP2_DMA_UNIT      (0x5200021c) //EP2 DMA Tx unit counter
#define rEP2_DMA_FIFO      (0x52000220) //EP2 DMA Tx FIFO counter
#define rEP2_DMA_TTC_L     (0x52000224) //EP2 DMA total Tx counter
#define rEP2_DMA_TTC_M     (0x52000228)
#define rEP2_DMA_TTC_H     (0x5200022c)
#define rEP3_DMA_CON       (0x52000240) //EP3 DMA interface control
#define rEP3_DMA_UNIT      (0x52000244) //EP3 DMA Tx unit counter
#define rEP3_DMA_FIFO      (0x52000248) //EP3 DMA Tx FIFO counter
#define rEP3_DMA_TTC_L     (0x5200024c) //EP3 DMA total Tx counter
#define rEP3_DMA_TTC_M     (0x52000250)
#define rEP3_DMA_TTC_H     (0x52000254)
#define rEP4_DMA_CON       (0x52000258) //EP4 DMA interface control
#define rEP4_DMA_UNIT      (0x5200025c) //EP4 DMA Tx unit counter
#define rEP4_DMA_FIFO      (0x52000260) //EP4 DMA Tx FIFO counter
#define rEP4_DMA_TTC_L     (0x52000264) //EP4 DMA total Tx counter
#define rEP4_DMA_TTC_M     (0x52000268)
#define rEP4_DMA_TTC_H     (0x5200026c)
#endif   // __BIG_ENDIAN


// WATCH DOG TIMER
#define rWTCON   (0x53000000) //Watch-dog timer mode
#define rWTDAT   (0x53000004) //Watch-dog timer data
#define rWTCNT   (0x53000008) //Eatch-dog timer count


// IIC
#define rIICCON  (0x54000000) //IIC control
#define rIICSTAT (0x54000004) //IIC status
#define rIICADD  (0x54000008) //IIC address
#define rIICDS   (0x5400000c) //IIC data shift


// IIS
#define rIISCON  (0x55000000) //IIS Control
#define rIISMOD  (0x55000004) //IIS Mode
#define rIISPSR  (0x55000008) //IIS Prescaler
#define rIISFCON (0x5500000c) //IIS FIFO control

#ifdef __BIG_ENDIAN
#define IISFIFO  (0x55000012) //IIS FIFO entry

#else //Little Endian
#define IISFIFO  (0x55000010) //IIS FIFO entry

#endif


// I/O PORT
#define rGPACON    (0x56000000) //Port A control
#define rGPADAT    (0x56000004) //Port A data

#define rGPBCON    (0x56000010) //Port B control
#define rGPBDAT    (0x56000014) //Port B data
#define rGPBUP     (0x56000018) //Pull-up control B

#define rGPCCON    (0x56000020) //Port C control
#define rGPCDAT    (0x56000024) //Port C data
#define rGPCUP     (0x56000028) //Pull-up control C

#define rGPDCON    (0x56000030) //Port D control
#define rGPDDAT    (0x56000034) //Port D data
#define rGPDUP     (0x56000038) //Pull-up control D

#define rGPECON    (0x56000040) //Port E control
#define rGPEDAT    (0x56000044) //Port E data
#define rGPEUP     (0x56000048) //Pull-up control E

#define rGPFCON    (0x56000050) //Port F control
#define rGPFDAT    (0x56000054) //Port F data
#define rGPFUP     (0x56000058) //Pull-up control F

#define rGPGCON    (0x56000060) //Port G control
#define rGPGDAT    (0x56000064) //Port G data
#define rGPGUP     (0x56000068) //Pull-up control G

#define rGPHCON    (0x56000070) //Port H control
#define rGPHDAT    (0x56000074) //Port H data
#define rGPHUP     (0x56000078) //Pull-up control H

#define rMISCCR    (0x56000080) //Miscellaneous control
#define rDCLKCON   (0x56000084) //DCLK0/1 control
#define rEXTINT0   (0x56000088) //External interrupt control register 0
#define rEXTINT1   (0x5600008c) //External interrupt control register 1
#define rEXTINT2   (0x56000090) //External interrupt control register 2
#define rEINTFLT0  (0x56000094) //Reserved
#define rEINTFLT1  (0x56000098) //Reserved
#define rEINTFLT2  (0x5600009c) //External interrupt filter control register 2
#define rEINTFLT3  (0x560000a0) //External interrupt filter control register 3
#define rEINTMASK  (0x560000a4) //External interrupt mask
#define rEINTPEND  (0x560000a8) //External interrupt pending
#define rGSTATUS0  (0x560000ac) //External pin status
#define rGSTATUS1  (0x560000b0) //Chip ID(0x32410000)
#define rGSTATUS2  (0x560000b4) //Reset type
#define rGSTATUS3  (0x560000b8) //Saved data0(32-bit) before entering POWER_OFF mode
#define rGSTATUS4  (0x560000bc) //Saved data0(32-bit) before entering POWER_OFF mode

// RTC
#ifdef __BIG_ENDIAN
#define rRTCCON    (0x57000043) //RTC control
#define rTICNT     (0x57000047) //Tick time count
#define rRTCALM    (0x57000053) //RTC alarm control
#define rALMSEC    (0x57000057) //Alarm second
#define rALMMIN    (0x5700005b) //Alarm minute
#define rALMHOUR   (0x5700005f) //Alarm Hour
#define rALMDATE   (0x57000063) //Alarm day     <-- May 06, 2002 SOP
#define rALMMON    (0x57000067) //Alarm month
#define rALMYEAR   (0x5700006b) //Alarm year
#define rRTCRST    (0x5700006f) //RTC round reset
#define rBCDSEC    (0x57000073) //BCD second
#define rBCDMIN    (0x57000077) //BCD minute
#define rBCDHOUR   (0x5700007b) //BCD hour
#define rBCDDATE   (0x5700007f) //BCD day       <-- May 06, 2002 SOP
#define rBCDDAY    (0x57000083) //BCD date      <-- May 06, 2002 SOP
#define rBCDMON    (0x57000087) //BCD month
#define rBCDYEAR   (0x5700008b) //BCD year

#else //Little Endian
#define rRTCCON    (0x57000040) //RTC control
#define rTICNT     (0x57000044) //Tick time count
#define rRTCALM    (0x57000050) //RTC alarm control
#define rALMSEC    (0x57000054) //Alarm second
#define rALMMIN    (0x57000058) //Alarm minute
#define rALMHOUR   (0x5700005c) //Alarm Hour
#define rALMDATE   (0x57000060) //Alarm day      <-- May 06, 2002 SOP
#define rALMMON    (0x57000064) //Alarm month
#define rALMYEAR   (0x57000068) //Alarm year
#define rRTCRST    (0x5700006c) //RTC round reset
#define rBCDSEC    (0x57000070) //BCD second
#define rBCDMIN    (0x57000074) //BCD minute
#define rBCDHOUR   (0x57000078) //BCD hour
#define rBCDDATE   (0x5700007c) //BCD day        <-- May 06, 2002 SOP
#define rBCDDAY    (0x57000080) //BCD date       <-- May 06, 2002 SOP
#define rBCDMON    (0x57000084) //BCD month
#define rBCDYEAR   (0x57000088) //BCD year
#endif  //RTC


// ADC
#define rADCCON    (0x58000000) //ADC control
#define rADCTSC    (0x58000004) //ADC touch screen control
#define rADCDLY    (0x58000008) //ADC start or Interval Delay
#define rADCDAT0   (0x5800000c) //ADC conversion data 0
#define rADCDAT1   (0x58000010) //ADC conversion data 1

// SPI
#define rSPCON0    (0x59000000) //SPI0 control
#define rSPSTA0    (0x59000004) //SPI0 status
#define rSPPIN0    (0x59000008) //SPI0 pin control
#define rSPPRE0    (0x5900000c) //SPI0 baud rate prescaler
#define rSPTDAT0   (0x59000010) //SPI0 Tx data
#define rSPRDAT0   (0x59000014) //SPI0 Rx data

#define rSPCON1    (0x59000020) //SPI1 control
#define rSPSTA1    (0x59000024) //SPI1 status
#define rSPPIN1    (0x59000028) //SPI1 pin control
#define rSPPRE1    (0x5900002c) //SPI1 baud rate prescaler
#define rSPTDAT1   (0x59000030) //SPI1 Tx data
#define rSPRDAT1   (0x59000034) //SPI1 Rx data


// SD Interface
#define rSDICON     (0x5a000000) //SDI control
#define rSDIPRE     (0x5a000004) //SDI baud rate prescaler
#define rSDICARG    (0x5a000008) //SDI command argument
#define rSDICCON    (0x5a00000c) //SDI command control
#define rSDICSTA    (0x5a000010) //SDI command status
#define rSDIRSP0    (0x5a000014) //SDI response 0
#define rSDIRSP1    (0x5a000018) //SDI response 1
#define rSDIRSP2    (0x5a00001c) //SDI response 2
#define rSDIRSP3    (0x5a000020) //SDI response 3
#define rSDIDTIMER  (0x5a000024) //SDI data/busy timer
#define rSDIBSIZE   (0x5a000028) //SDI block size
#define rSDIDCON    (0x5a00002c) //SDI data control
#define rSDIDCNT    (0x5a000030) //SDI data remain counter
#define rSDIDSTA    (0x5a000034) //SDI data status
#define rSDIFSTA    (0x5a000038) //SDI FIFO status
#define rSDIIMSK    (0x5a000040) //SDI interrupt mask

#ifdef __BIG_ENDIAN
#define rSDIDAT    (0x5a00003f) //SDI data
#define SDIDAT     (0x5a00003f)
#else  // Little Endian
#define rSDIDAT    (0x5a00003c) //SDI data
#define SDIDAT     (0x5a00003c)
#endif   //SD Interface


// PENDING BIT
#define BIT_EINT0      (0x1)
#define BIT_EINT1      (0x1<<1)
#define BIT_EINT2      (0x1<<2)
#define BIT_EINT3      (0x1<<3)
#define BIT_EINT4_7    (0x1<<4)
#define BIT_EINT8_23   (0x1<<5)
#define BIT_NOTUSED6   (0x1<<6)
#define BIT_BAT_FLT    (0x1<<7)
#define BIT_TICK       (0x1<<8)
#define BIT_WDT        (0x1<<9)
#define BIT_TIMER0     (0x1<<10)
#define BIT_TIMER1     (0x1<<11)
#define BIT_TIMER2     (0x1<<12)
#define BIT_TIMER3     (0x1<<13)
#define BIT_TIMER4     (0x1<<14)
#define BIT_UART2      (0x1<<15)
#define BIT_LCD        (0x1<<16)
#define BIT_DMA0       (0x1<<17)
#define BIT_DMA1       (0x1<<18)
#define BIT_DMA2       (0x1<<19)
#define BIT_DMA3       (0x1<<20)
#define BIT_SDI        (0x1<<21)
#define BIT_SPI0       (0x1<<22)
#define BIT_UART1      (0x1<<23)
#define BIT_NOTUSED24  (0x1<<24)
#define BIT_USBD       (0x1<<25)
#define BIT_USBH       (0x1<<26)
#define BIT_IIC        (0x1<<27)
#define BIT_UART0      (0x1<<28)
#define BIT_SPI1       (0x1<<29)
#define BIT_RTC        (0x1<<30)
#define BIT_ADC        (0x1<<31)
#define BIT_ALLMSK     (0xffffffff)

#define BIT_SUB_ALLMSK (0x7ff)
#define BIT_SUB_ADC    (0x1<<10)
#define BIT_SUB_TC     (0x1<<9)
#define BIT_SUB_ERR2   (0x1<<8)
#define BIT_SUB_TXD2   (0x1<<7)
#define BIT_SUB_RXD2   (0x1<<6)
#define BIT_SUB_ERR1   (0x1<<5)
#define BIT_SUB_TXD1   (0x1<<4)
#define BIT_SUB_RXD1   (0x1<<3)
#define BIT_SUB_ERR0   (0x1<<2)
#define BIT_SUB_TXD0   (0x1<<1)
#define BIT_SUB_RXD0   (0x1<<0)

#define SNGS3C_INTNUMLEVELS 32

#define INT_LVL_ADC     0
#define INT_LVL_RTC     1
#define INT_LVL_SPI1    2
#define INT_LVL_UART0   3
#define INT_LVL_IIC     4
#define INT_LVL_USBH    5
#define INT_LVL_USBD    6
#define INT_LVL_RES2    7
#define INT_LVL_UART1   8
#define INT_LVL_SPI0    9
#define INT_LVL_SDI     10
#define INT_LVL_DMA3    11
#define INT_LVL_DMA2    12
#define INT_LVL_DMA1    13
#define INT_LVL_DMA0    14
#define INT_LVL_LCD     15
#define INT_LVL_UART2   16
#define INT_LVL_TIMER4  17
#define INT_LVL_TIMER3  18
#define INT_LVL_TIMER2  19
#define INT_LVL_TIMER1  20
#define INT_LVL_TIMER0  21
#define INT_LVL_WDT     22
#define INT_LVL_TICK    23
#define INT_LVL_FLT     24
#define INT_LVL_RES1    25
#define INT_LVL_823     26
#define INT_LVL_EINT4567  27
#define INT_LVL_EINT3   28
#define INT_LVL_EINT2   29
#define INT_LVL_EINT1   30
#define INT_LVL_EINT0   31

#define INT_VEC_EINT0     IVEC_TO_INUM(INT_LVL_EINT0)
#define INT_VEC_EINT1     IVEC_TO_INUM(INT_LVL_EINT1)
#define INT_VEC_EINT2     IVEC_TO_INUM(INT_LVL_EINT2)
#define INT_VEC_EINT3     IVEC_TO_INUM(INT_LVL_EINT3)
#define INT_VEC_EINT4567  IVEC_TO_INUM(INT_LVL_EINT4567)
#define INT_VEC_EINT823   IVEC_TO_INUM(INT_LVL_823)
#define INT_VEC_TICK      IVEC_TO_INUM(INT_LVL_TICK)
#define INT_VEC_UART0     IVEC_TO_INUM(INT_LVL_UART0)
#define INT_VEC_UART1     IVEC_TO_INUM(INT_LVL_UART1)
#define INT_VEC_UART2     IVEC_TO_INUM(INT_LVL_UART2)
#define INT_VEC_TIMER0    IVEC_TO_INUM(INT_LVL_TIMER0)
#define INT_VEC_TIMER1    IVEC_TO_INUM(INT_LVL_TIMER1)
#define INT_VEC_TIMER2    IVEC_TO_INUM(INT_LVL_TIMER2)
#define INT_VEC_TIMER3    IVEC_TO_INUM(INT_LVL_TIMER3)
#define INT_VEC_TIMER4    IVEC_TO_INUM(INT_LVL_TIMER4)

/* hardware access methods */
#define SNGS3C_REG_READ(x,result) \
        ((result) = *(volatile unsigned int *)(x))

#define SNGS3C_REG_WRITE(x,data) \
        (*((volatile unsigned int *)(x)) = (data))

/* define the clk for io used */

/*#define SNGS3C_SYSCLK   152000000		 System bus clock */

#define SNGS3C_SYSCLK   200000000		/* System bus clock */
//#define SNGS3C_SYSCLK   75000000		//debug zhangji 2008-8-20 15:07:34
/* must modify fclk in option.a too*/

#define SNGS3C_HCLK   (SNGS3C_SYSCLK/2)
#define SNGS3C_PCLK   (SNGS3C_SYSCLK/4)

#define SYS_TIMER_CLK	(SNGS3C_PCLK/200)
#define AUX_TIMER_CLK	(SNGS3C_PCLK/200)

#define SYS_CLK_RATE_MIN 10
#define SYS_CLK_RATE_MAX 25000

#define AUX_CLK_RATE_MIN 10
#define AUX_CLK_RATE_MAX 25000


#define SYS_TIMER_INT_VEC (INT_VEC_TIMER0)
#define AUX_TIMER_INT_VEC (INT_VEC_TIMER1)

#ifdef __cplusplus
}
#endif
#endif  //__2410ADDR_H___e
