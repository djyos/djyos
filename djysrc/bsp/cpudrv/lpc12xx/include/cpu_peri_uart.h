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


#ifndef __CPU_PERI_UART__
#define __CPU_PERI_UART__

#ifdef __cplusplus
extern "C" {
#endif
//寄存器位偏移定义
//中断使能寄存器位ier偏移定义
#define bo_uart_ier_rbr     0   //1=允许接收中断和接收超时中断
#define bo_uart_ier_thre    1   //1=允许发送中断
#define bo_uart_ier_rxl     2   //1=允许接收线状态中断，状态可从lsr4:1读出

//中断标识寄存器iir位偏移定义
#define bo_uart_iir_all     0   //0=至少有一个中断挂起,1=无中断挂起
#define bm_uart_iir_id      0xe0//1=发送中断，2=接收中断，3=线状态中断，6=超时
#define bo_uart_iir_id      1   //标识挂起的中断

//fifo控制寄存器fcr位偏移定义
#define bo_uart_fcr_ffen    0   //1=使能fifo，并使本寄存器其他位可访问。
#define bo_uart_fcr_rffrs   1   //1=复位rx fifo，复位后由硬件清零本位
#define bo_uart_fcr_tffrs   2   //1=复位tx fifo，复位后由硬件清零本位
#define bm_uart_fcr_fflevel 0xc0//fifo触发水平，0=1字符，1=4字符，2=8字符，3=14
#define bo_uart_fcr_fflevel 6   //收发fifo触发水平选择

//线状态控制寄存器lcr位偏移定义
#define bm_uart_lcr_bw      0x3 //字符长度，0~3=5~8位
#define bo_uart_lcr_bw      0   //字符长度位偏移
#define bo_uart_lcr_st      2   //0=1位停止位，1=2位停止位，若5位字符则1.5位
#define bo_uart_lcr_pe      3   //0=禁止奇偶校验，1=使能奇偶校验
#define bm_uart_lcr_p       0x30//奇偶校验，0=奇，1=偶，2=强制1，3=强制0
#define bo_uart_lcr_p       4   //奇偶校验选择
#define bo_uart_lcr_lbc     6   //间断发送控制，0=正常发送，1=强制发送0
#define bo_uart_lcr_dlab    7   //1=选择baud寄存器(baud与其他寄存器地址相同)

//线状态寄存器lsr寄存器位偏移定义
#define bo_uart_lsr_rdr     0   //1=接收就绪
#define bo_uart_lsr_oe      1   //1=接收溢出，读lsr则清除
#define bo_uart_lsr_pe      2   //1=产生奇偶错误
#define bo_uart_lsr_fe      3   //1=发生帧错误
#define bo_uart_lsr_bi      4   //1=检测到间断发送信号
#define bo_uart_lsr_thre    5   //1=发送fifo空
#define bo_uart_lsr_temt    6   //1=发送移位器空
#define bo_uart_lsr_rsfe    7   //1=rx fifo中有一个接收错误字符

//发送使能寄存器ter位偏移定义
#define bo_uart_ter_txen    7   //1=允许发送，0=停止发送

//注：1.不同的CPU，uart寄存器基址不同。
//    2.不是所有cpu都支持所有寄存器，参看所用的器件。
struct st_nxp_uart_reg
{
  union {                   //0x00
      vu32  RBR;                //dlab=0 接收fifo地址，thr是，
      vu32  THR;                //dlab=0 发送fifo地址
      vu32  DLL;                //dlab=1 baud除数低字节
  }RTD;
  union {                   //0x04
      vu32  DLM;                //dlab=1 baud除数高字节
      vu32  IER;                //dlab=0 中断使能寄存器
  }DI;
  union {                   //offset = 0x08
      vu32  IIR;                //中断id寄存器
      vu32  FCR;                //fifo控制寄存器
  }IF;
  vu32  LCR;                //0x0c，线控制寄存器
  vu32  MCR;                //0x010 Modem control Register (R/W)
  vu32  LSR;                //0x014 线状态寄存器
  vu32  MSR;                //0x018 Modem status Register (R/ )
  vu32  SCR;                //0x01C 高速缓存
  vu32  ACR;                //0x020 Auto-baud Control Register (R/W)
  vu32  ICR;                //0x024 IrDA Control Register (R/W) */      
  vu32  FDR;                //0x028 Fractional Divider Register (R/W)
  vu32  RESERVED0;
  vu32  TER;                //0x030 发送使能
  vu32  RESERVED1[6];
  vu32  RS485CTRL;          //0x04C RS-485/EIA-485 Control Register (R/W)
  vu32  ADRMATCH;           //0x050 RS-485/EIA-485 address match Register (R/W)
  vu32  RS485DLY;           //0x054 RS-485/EIA-485 direction control delay Register (R/W)
  vu32  FIFOLVL;            //0x058 FIFO Level Register (R/ )
};

/* @brief UART_databit_type definitions
  * @{
  */
#define UART_CFG_DATABIT_5   ((u8)(0))       /* UART 5 bit data mode */
#define UART_CFG_DATABIT_6   ((u8)(1<<0))    /* UART 6 bit data mode */
#define UART_CFG_DATABIT_7   ((u8)(2<<0))    /* UART 7 bit data mode */
#define UART_CFG_DATABIT_8   ((u8)(3<<0))    /* UART 8 bit data mode */
#define PARAM_UART_DATABIT(databit) ((databit==UART_CFG_DATABIT_5) || \
                                     (databit==UART_CFG_DATABIT_6) || \
                                     (databit==UART_CFG_DATABIT_7) || \
                                     (databit==UART_CFG_DATABIT_8))
/**
  * @}
  */

/*@brief UART_stopbit_type definitions
  * @{
  */
#define UART_CFG_STOPBIT_1   ((u8)(0))      /* UART 1 Stop Bits Select */
#define UART_CFG_STOPBIT_2   ((u8)(1<<2))   /* UART Two Stop Bits Select */
#define PARAM_UART_STOPBIT(stopbit) ((stopbit==UART_CFG_STOPBIT_1) || \
                                     (stopbit==UART_CFG_STOPBIT_2))
/**
  * @}
  */

/* @brief UART_parity_type definitions
  * @{
  */
#define UART_CFG_PARITY_NONE   ((u8)(0))     /* No parity */
#define UART_CFG_PARITY_ODD    ((u8)(1<<3))  /* Odd parity */
#define UART_CFG_PARITY_EVEN   ((u8)(3<<3))  /* Even parity */
#define UART_CFG_PARITY_MARK   ((u8)(5<<3))  /* Forced "1" stick parity */
#define UART_CFG_PARITY_SPACE  ((u8)(7<<3))  /* Forced "0" stick parity */
#define PARAM_UART_PARITY(parity)   ((parity==UART_CFG_PARITY_NONE) || \
                                     (parity==UART_CFG_PARITY_ODD)  || \
                                     (parity==UART_CFG_PARITY_EVEN) || \
                                     (parity==UART_CFG_PARITY_MARK) || \
                                     (parity==UART_CFG_PARITY_SPACE))
/**
  * @}
  */

/* @brief FIFO_level_type definitions
  * @{
  */
#define UART_CFG_FIFOTRG_1   ((u8)(0))    /* UART FIFO trigger level 0: 1 char */
#define UART_CFG_FIFOTRG_4   ((u8)(1<<6)) /* UART FIFO trigger level 1: 4 char */
#define UART_CFG_FIFOTRG_8   ((u8)(2<<6)) /* UART FIFO trigger level 2: 8 char */
#define UART_CFG_FIFOTRG_14  ((u8)(3<<6)) /* UART FIFO trigger level 3: 14 char */
#define PARAM_UART_FIFO_LEVEL(fifo) ((fifo==UART_CFG_FIFOTRG_1) || \
                                     (fifo==UART_CFG_FIFOTRG_4) || \
                                     (fifo==UART_CFG_FIFOTRG_8) || \
                                     (fifo==UART_CFG_FIFOTRG_14))
/**
  * @}
  */

/*@brief FIFO_dmamode_type definitions
  * @{
  */
#define UART_CFG_DMAMODE_DISABLE  ((u8)(0))     /* UART DMA mode disable */
#define UART_CFG_DMAMODE_ENABLE   ((u8)(1<<3))  /* UART DMA mode enable  */
#define PARAM_UART_FIFO_DMA(fifo)   ((fifo==UART_CFG_DMAMODE_DISABLE) ||\
                                     (fifo==UART_CFG_DMAMODE_ENABLE))
/**
  * @}
  */

/*@brief TXD_break_forced_type definitions
  * @{
  */
#define UART_CFG_TXDBREAK_DISABLE  ((u8)(0))      /* TXD Break disable */
#define UART_CFG_TXDBREAK_ENABLE   ((u8)(1<<6))   /* TXD Break enable */
#define PARAM_UART_TXDBREAK(type)   ((type==UART_CFG_TXDBREAK_DISABLE) ||\
                                     (type==UART_CFG_TXDBREAK_ENABLE))
/**
  * @}
  */

/*@brief UART_interrupt_type definitions
  * @{
  */
#define UART_INTCFG_RBR  ((u16)(1<<0)) /* RBR Interrupt enable*/
#define UART_INTCFG_THRE ((u16)(1<<1)) /* THR Interrupt enable*/
#define UART_INTCFG_RLS  ((u16)(1<<2)) /* RX line status interrupt enable*/
#define UART_MSINT_EN    ((u32)(1<<3)) /*!< Modem status interrupt enable */
#define UART_CTSINT_EN   ((u32)(1<<7)) /*!< CTS1 signal transition interrupt enable */
#define UART_INTCFG_ABEO ((u16)(1<<8)) /* the end of auto-baud interrupt */
#define UART_INTCFG_ABTO ((u16)(1<<9)) /* the auto-baud time-out interrupt */
#define PARAM_UART_INTCFG(intcfg)   ((intcfg == UART_INTCFG_RBR)  ||\
                                     (intcfg == UART_INTCFG_THRE) ||\
                                     (intcfg == UART_INTCFG_RLS)  ||\
                                     (intcfg == UART_MSINT_EN)    ||\
                                     (intcfg == UART_CTSINT_EN)   ||\
                                     (intcfg == UART_INTCFG_ABEO) ||\
                                     (intcfg == UART_INTCFG_ABTO))
/**
  * @}
  */

/*@brief UART_int_status_type definitions
  * @{
  */

#define UART_INTSTAT_PEND              ((u16)(1<<0))  /* Active low */
#define UART_INTSTAT_RX_LINE_STAT      ((u16)(3<<1))  /* Receive line status*/
#define UART_INTSTAT_RX_DATA_AVAILABLE ((u16)(2<<1))  /* Receive data available*/
#define UART_INTSTAT_RX_TIMEOUT        ((u16)(6<<1))  /* Character time-out indicator*/
#define UART_INTSTAT_TX_READY          ((u16)(1<<1))  /* THRE interrupt*/
#define UART_INTSTAT_MODEM             ((u16)(0<<1))  /* Modem interrupt*/
#define UART_INTSTAT_AUTO_END          ((u16)(1<<8))  /* End of auto-baud interrupt */
#define UART_INTSTAT_AUTO_TIMOUT       ((u16)(1<<9))  /* Autobaud time-out interrupt */

/**
  * @}
  */

/*@brief UART_line_status_type definitions
  * @{
  */
#define UART_LS_RX_DATA_READY ((u8)(1<<0)) /*Receive data ready*/
#define UART_LS_RX_FIFO_FULL  ((u8)(1<<1)) /*Overrun error: RX fifo overflow*/
#define UART_LS_PARITY_ERROR  ((u8)(1<<2)) /*Parity error*/
#define UART_LS_FRAMING_ERROR ((u8)(1<<3)) /*Framing error*/
#define UART_LS_BREAK_INT     ((u8)(1<<4)) /*Break interrupt*/
#define UART_LS_TX_EMPTY      ((u8)(1<<5)) /*Transmit holding register THR empty*/
#define UART_LS_TX_EMPTY_ALL  ((u8)(1<<6)) /*Transmitter THR and TSR all empty*/
#define UART_LS_RX_FIFO_ERROR ((u8)(1<<7)) /*Error in RX FIFO or others*/

/**
  * @}
  */

/*@brief UART_auto_baudrate_mode_type definitions
  * @{
  */
#define UART_CFG_AUTOBAUD_MODE0  ((u8)(0))     /* UART Auto baudrate Mode 0 */
#define UART_CFG_AUTOBAUD_MODE1  ((u8)(1<<1)) /* UART Auto baudrate Mode 1 */

#define PARAM_UART_AB_MODE(ABmode) ((ABmode==UART_CFG_AUTOBAUD_MODE0) ||\
                                    (ABmode==UART_CFG_AUTOBAUD_MODE1))

/**
  * @}
  */

/*@brief UART_auto_baudrate_restart_type definitions
  * @{
  */
#define UART_CFG_ABRESTART_DISABLE  ((u8)(0))     /* Auto baudrate disable */
#define UART_CFG_ABRESTART_ENABLE   ((u8)(1<<2)) /* Auto baudrate enable */

#define PARAM_UART_AB_RESTART(type) ((type==UART_CFG_ABRESTART_DISABLE) ||\
                                     (type==UART_CFG_ABRESTART_ENABLE))


/**
  * @}
  */

/*@brief UART_modem_status_type definitions
  * @{
  */
#define UART_MODEM_STAT_DELTA_CTS  ((u8)(1<<0))   /* State change of CTS */
#define UART_MODEM_STAT_DELTA_DSR  ((u8)(1<<1))   /* State change of DSR */
#define UART_MODEM_STAT_LO2HI_RI   ((u8)(1<<2))   /* Low to high transition of RI */
#define UART_MODEM_STAT_DELTA_DCD  ((u8)(1<<3))   /* State change of input DCD */
#define UART_MODEM_STAT_CTS        ((u8)(1<<4))   /* Clear To Send State */
#define UART_MODEM_STAT_DSR        ((u8)(1<<5))   /* Data Set Ready State */
#define UART_MODEM_STAT_RI         ((u8)(1<<6))   /* Ring Indicator State */
#define UART_MODEM_STAT_DCD        ((u8)(1<<7))   /* Data Carrier Detect State */

/**
  * @}
  */

/*@brief UART_modem_DTR_pin_type definitions
  * @{
  */
#define UART_MODEM_PIN_DTR_DISABLE ((u8)(0))
#define UART_MODEM_PIN_DTR_ENABLE  ((u8)(1<<0)) /* Modem output pin DTR */
#define PARAM_MODEM_PIN_DTR(x) ((x==UART_MODEM_PIN_DTR_DISABLE) || \
                                (x==UART_MODEM_PIN_DTR_ENABLE))

/**
  * @}
  */

/*@brief UART_modem_RTS_pin_type definitions
  * @{
  */

#define UART_MODEM_PIN_RTS_DISABLE ((u8)(0))
#define UART_MODEM_PIN_RTS_ENABLE  ((u8)(1<<1)) /* Modem output pin RTS */
#define PARAM_MODEM_PIN_RTS(x) ((x==UART_MODEM_PIN_RTS_DISABLE) || \
                                (x==UART_MODEM_PIN_RTS_ENABLE))

/**
  * @}
  */

/*@brief UART_modem_loopback_type definitions
  * @{
  */

#define UART_MODEM_MODE_LOOPBACK_DISABLE ((u8)(0))
#define UART_MODEM_MODE_LOOPBACK_ENABLE  ((u8)(1<<4))  /* Loop back mode select */
#define PARAM_MODEM_MODE_LOOPBACK(x) ((x==UART_MODEM_MODE_LOOPBACK_DISABLE) || \
                                      (x==UART_MODEM_MODE_LOOPBACK_ENABLE))

/**
  * @}
  */

/*@brief UART_modem_autoRTS_type definitions
  * @{
  */
#define UART_MODEM_MODE_AUTO_RTS_DISABLE ((u8)(0))
#define UART_MODEM_MODE_AUTO_RTS_ENABLE  ((u8)(1<<6)) /* Enable AutoRTS flow-control*/
#define PARAM_MODEM_MODE_AUTO_RTS(x) ((x==UART_MODEM_MODE_AUTO_RTS_DISABLE) || \
                                      (x==UART_MODEM_MODE_AUTO_RTS_ENABLE))

/**
  * @}
  */

/*@brief UART_modem_autoCTS_type definitions
  * @{
  */
#define UART_MODEM_MODE_AUTO_CTS_DISABLE ((u8)(0))
#define UART_MODEM_MODE_AUTO_CTS_ENABLE  ((u8)(1<<7)) /* Enable AutoCTS flow-control*/
#define PARAM_MODEM_MODE_AUTO_CTS(x) ((x==UART_MODEM_MODE_AUTO_CTS_DISABLE) || \
                                      (x==UART_MODEM_MODE_AUTO_CTS_ENABLE))

/**
  * @}
  */

/*@brief UART_485_nmm_type definitions
  * @{
  */
#define UART_RS485_NORMAL_MULTIDROP_DISABLE ((u8)(0))
#define UART_RS485_NORMAL_MULTIDROP_ENABLE  ((u8)(1<<0))  /* Normal Multi-drop Mode */
#define PARAM_UART_RS485_NORMAL_MULTIDROP(x) ((x==UART_RS485_NORMAL_MULTIDROP_DISABLE) || \
                                             (x==UART_RS485_NORMAL_MULTIDROP_ENABLE))

/**
  * @}
  */

/*@brief UART_485_receiver_type definitions
  * @{
  */
#define UART_RS485_RECEIVER_ENABLE   ((u8)(0))
#define UART_RS485_RECEIVER_DISABLE  ((u8)(1<<1))  /* The receiver is disabled */
#define PARAM_UART_RS485_RECEIVER(x) ((x==UART_RS485_RECEIVER_ENABLE) || \
                                 (x==UART_RS485_RECEIVER_DISABLE))

/**
  * @}
  */

/*@brief UART_485_add_type definitions
  * @{
  */

#define UART_RS485_AUTO_ADDRRESS_DISABLE ((u8)(0))
#define UART_RS485_AUTO_ADDRRESS_ENABLE  ((u8)(1<<2))  /* Auto Address Detect */
#define PARAM_UART_RS485_AUTO_ADDRRESS(x) ((x==UART_RS485_AUTO_ADDRRESS_DISABLE) || \
                                      (x==UART_RS485_AUTO_ADDRRESS_ENABLE))

/**
  * @}
  */

/*@brief UART_485_dirpin_ctrl_type definitions
  * @{
  */
#define UART_RS485_DIRPIN_RTS  ((u8)(0))
#define UART_RS485_DIRPIN_DTR  ((u8)(1<<3))  /* Direction control */
#define PARAM_UART_RS485_DIRPIN_SELECT(x) ((x==UART_RS485_DIRPIN_RTS) || \
                                      (x==UART_RS485_DIRPIN_DTR))

/**
  * @}
  */

/*@brief UART_485_autodir_type definitions
  * @{
  */
#define UART_RS485_AUTO_DIRECTION_DISABLE ((u8)(0))
#define UART_RS485_AUTO_DIRECTION_ENABLE  ((u8)(1<<4)) /* Auto Direction Control */
#define PARAM_UART_RS485_AUTO_DIRECTION(x) ((x==UART_RS485_AUTO_DIRECTION_DISABLE) || \
                                           (x==UART_RS485_AUTO_DIRECTION_ENABLE))

/**
  * @}
  */

/*@brief UART_485_dirpin_level_type definitions
  * @{
  */
#define UART_RS485_DIRPIN_LOW   ((u8)(0))
#define UART_RS485_DIRPIN_HIGH  ((u8)(1<<5))  /* Reverses the polarity */
#define PARAM_UART_RS485_DIRPIN_LEVEL(x) ((x==UART_RS485_DIRPIN_LOW) || \
                                          (x==UART_RS485_DIRPIN_HIGH))

/**
  * @}
  */

/*@brief UART_irda_mode_type definitions
  * @{
  */
#define UART_IRDA_DISABLE       ((u8)(0))    /* IrDA mode disable */
#define UART_IRDA_ENABLE        ((u8)(1<<0)) /* IrDA mode enable */
#define PARAM_UART_IRDA_MODE(x) ((x==UART_IRDA_DISABLE) || \
                                 (x==UART_IRDA_ENABLE))

/**
  * @}
  */

/*@brief UART_irda_input_invert_type definitions
  * @{
  */
#define UART_IRDA_INPUTINVERT_NOT   ((u8)(0))    /* IrDA serial not input inverted */
#define UART_IRDA_INPUTINVERTED     ((u8)(1<<1)) /* IrDA serial input inverted */
#define PARAM_UART_IRDA_INPUT(x) ((x==UART_IRDA_INPUTINVERT_NOT) || \
                                  (x==UART_IRDA_INPUTINVERTED))

/**
  * @}
  */

/*@brief UART_irda_fixed_pulse_type definitions
  * @{
  */
#define UART_IRDA_PULSEDIV_NORMAL     ((u8)(0))
#define UART_IRDA_PULSEDIV_2          ((u8)(0x1<<2))
#define UART_IRDA_PULSEDIV_4          ((u8)(0x3<<2))
#define UART_IRDA_PULSEDIV_8          ((u8)(0x5<<2))
#define UART_IRDA_PULSEDIV_16         ((u8)(0x7<<2))
#define UART_IRDA_PULSEDIV_32         ((u8)(0x9<<2))
#define UART_IRDA_PULSEDIV_64         ((u8)(0xB<<2))
#define UART_IRDA_PULSEDIV_128        ((u8)(0xD<<2))
#define UART_IRDA_PULSEDIV_256        ((u8)(0xF<<2))
#define PARAM_UART_IRDA_PULSEDIV(x) ((x==UART_IRDA_PULSEDIV_NORMAL) || \
                                     (x==UART_IRDA_PULSEDIV_2)      || \
                                     (x==UART_IRDA_PULSEDIV_4)      || \
                                     (x==UART_IRDA_PULSEDIV_8)      || \
                                     (x==UART_IRDA_PULSEDIV_16)     || \
                                     (x==UART_IRDA_PULSEDIV_64)     || \
                                     (x==UART_IRDA_PULSEDIV_128)    || \
                                     (x==UART_IRDA_PULSEDIV_256))
/**
  * @}
  */

/*@brief UART_process_time_delay definitions
  * @{
  */

#define UART_BLOKING_TIMEOUT          (0xFFFFFFFFUL)
#define UART_NO_TIMEOUT               (0x1UL)
#define PARAM_UART_PROCESS_DELAY(x) ((x==UART_BLOKING_TIMEOUT) || \
                                     (x==UART_NO_TIMEOUT))
    /* Private Types -------------------------------------------------------------- */
    /** @defgroup UART_Private_Types
     * @{
     */
    /* Macro to determine if it is valid UART port number */
#define PARAM_UARTx(x)     ((((u32 *)x)==((u32 *)LPC_UART0)) || \
                                (((u32 *)x)==((u32 *)LPC_UART1)))
#define PARAM_UART_MODEM(x) (((u32 *)x)==((u32 *)LPC_UART0))
#define PARAM_UART_485(x)   (((u32 *)x)==((u32 *)LPC_UART0))
#define PARAM_UART_IRDA(x)  (((u32 *)x)==((u32 *)LPC_UART1))

    /* Macro to define bit mask */
#define UART_RBR_MASKBIT    ((u8)0xFF) /* Received Buffer mask bit  */
#define UART_THR_MASKBIT    ((u8)0xFF) /* Transmit Holding mask bit  */
#define UART_IER_BITMASK    ((u32)(0x307)) /* IER normal bit mask */
#define UART_IER_BITMASK2   ((u32)(0x38F)) /* IER include modem bit mask */
#define UART_IIR_BITMASK    ((u32)(0x3CF)) /* IIR bit mask */
#define UART_FCR_BITMASK    ((u8)(0xCF))   /* FCR bit mask */
#define UART_LCR_BITMASK    ((u8)(0xFF))   /* LCR bit mask */
#define UART_MCR_BITMASK    ((u8)(0xF3))   /* MCR bit mask */
#define UART_LSR_BITMASK    ((u8)(0xFF))   /* LSR bit mask */
#define UART_MSR_BITMASK    ((u8)(0xFF))   /* MSR register bit mask */
#define UART_SCR_BIMASK     ((u8)(0xFF))   /* UART Scratch Pad bit mask */
#define UART_ACR_BITMASK    ((u32)(0x307)) /* ACR bit mask */
#define UART_ICR_BITMASK    ((u32)(0x3F))  /* IRDA bit mask */
#define UART_FDR_BITMASK    ((u32)(0xFF))  /* FDR bit mask */
#define UART_TER_BITMASK    ((u8)(0x80))   /* TER bit mask */
#define UART_RS485CTRL_BITMASK     ((u32)(0x3F))  /* 485 CTRL bit mask */
#define UART_RS485ADRMATCH_BITMASK ((u8)(0xFF))   /* 485 ADR bit mask */
#define UART_RS485DLY_BITMASK      ((u8)(0xFF))   /* 485 DLY bit mask */
#define UART_FIFOLVL_BITMASK       ((u32)(0x0F0F))/* 485 FIFOLVL bit mask */

    /* Macro to define control bit */
#define UART_LCR_DLAB_EN        ((u8)(1<<7))   /* DivisorLatchesAccess enable*/
#define UART_FCR_FIFO_EN        ((u8)(1<<0))   /* FIFO enable */
#define UART_FCR_RX_RS          ((u8)(1<<1))   /* FIFO RX reset */
#define UART_FCR_TX_RS          ((u8)(1<<2))   /* FIFO TX reset */
#define UART_TER_TXEN           ((u8)(1<<7))   /* Transmit enable bit */
#define UART_ACR_START          ((u32)(1<<0))  /* Auto baudrate Start */
#define UART_ACR_ABEOINT_CLR    ((u32)(1<<8))  /* Auto baudrate end INT clear */
#define UART_ACR_ABTOINT_CLR    ((u32)(1<<9))  /* Auto baudrate Ttime-out INT clear */

    /* Macro to define caculate */
#define UART_LOAD_DLL(div)        ((div) & 0xFF)
#define UART_LOAD_DLM(div)        (((div) >> 8) & 0xFF)
#define UART_FDR_DIVADDVAL(n)     ((u32)(n&0x0F))      /* Pre-scaler divisor */
#define UART_FDR_MULVAL(n)        ((u32)((n<<4)&0xF0)) /* Multiplier value */
#define UART_FIFOLVL_RXFIFOLVL(n) ((u32)(n&0x0F))      /*receiver FIFO */
#define UART_FIFOLVL_TXFIFOLVL(n) ((u32)((n>>8)&0x0F)) /*transmitter FIFO */

    /* Macro to define parameter */
#define UART_ACCEPTED_BAUDRATE_ERROR  (5) /* Acceptable baudrate error, 1/2(n) percent */
#define UART_TX_FIFO_SIZE             (16)
#define UART_DEVICE_NUMBER            (2)

ptu32_t module_init_uart0(ptu32_t para);
ptu32_t module_init_uart1(ptu32_t para);

#ifdef __cplusplus
}
#endif
#endif /* __CPU_PERI_UART__ */

