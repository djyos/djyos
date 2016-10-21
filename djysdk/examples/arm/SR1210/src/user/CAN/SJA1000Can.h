#ifndef _SJA1000_CAN_H_
#define _SJA1000_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "ring.h"

// Define the address of CAN Ports
#define  CN_ADDR_CAN1_BASE        (0x10800000)  // CAN口1
#define  CN_ADDR_CAN2_BASE        (0x10A00000)  // CAN口2
#define  CN_ADDR_CAN3_BASE        (0x10C00000)  // CAN口3
#define  CN_ADDR_CAN4_BASE        (0x10E00000)  // CAN口4
#define  CN_CAN_CHIP_NUM            (4)        //    4个CAN控制器
#define  CN_SIGNAL_PKG_SIZES_PERI         (13)

//============================================
// SJA1000 Registers for both BasicCAN & PeliCAN Mode
#define CN_CAN_CMR           ( 1)        // Command register
#define CN_CAN_SR            ( 2)        // Status  register
#define CN_CAN_IIR           ( 3)        // Interrupt identification register

//============================================
// SJA1000 Registers BasicCAN Mode
#define CN_BCAN_CR           ( 0)        // Control register ( including IER )
#define CN_BCAN_CMR          ( 1)        // Command register
#define CN_BCAN_SR           ( 2)        // Status  register
#define CN_BCAN_IIR          ( 3)        // Interrupt identification register
#define CN_BCAN_ACR          ( 4)        // Acceptance code register
#define CN_BCAN_AMR          ( 5)        // Acceptance mask register
#define CN_BCAN_BTR0         ( 6)        // Bus timing register 0
#define CN_BCAN_BTR1         ( 7)        // Bus timing register 1
#define CN_BCAN_OCR          ( 8)        // Output control register
#define CN_BCAN_TEST         ( 9)        // Test register
#define CN_BCAN_DSCR0        (10)        // Transmit buffer: Descriptor 0 (Identifier)
#define CN_BCAN_DSCR1        (11)        // Transmit buffer: Descriptor 1 (Controller)
#define CN_BCAN_FRAMENO      (12)        // Transmit buffer: FrameNo
#define CN_BCAN_TXD          (13)        // Transmit buffer: Data (13-19)
#define CN_BCAN_RXD          (20)        // Receive buffer: (20-29))
#define CN_BCAN_RX_LEN       (21)        // RX frame Length register
#define CN_BCAN_CDR          (31)        // Clock divider

//============================================
// SJA1000 Register PeliCAN mode
#define CN_PCAN_MODE         ( 0)        // Mode register
#define CN_PCAN_CMR          ( 1)        // Command register
#define CN_PCAN_SR           ( 2)        // Status register
#define CN_PCAN_IIR          ( 3)        // Interrupt identification register
#define CN_PCAN_IER          ( 4)        // Interrupt setting register
#define CN_PCAN_BTR0         ( 6)        // Bus timing register 0
#define CN_PCAN_BTR1         ( 7)        // Bus timing register 1
#define CN_PCAN_OCR          ( 8)        // Output control register
#define CN_PCAN_TEST         ( 9)        // Test register
#define CN_PCAN_ALC          (11)        // Arbitration lost capture register
#define CN_PCAN_ECC          (12)        // Error code capture register
#define CN_PCAN_EWL          (13)        // Error warning limit register
#define CN_PCAN_RXERR        (14)        // RX error counter register
#define CN_PCAN_TXERR        (15)        // TX error counter register
#define CN_PCAN_ACR0         (16)        // Acceptance code 0/RX(TX) frame information SFF(std) or EFF(Ext)
#define CN_PCAN_ACR1         (17)        // Acceptance code 1/RX(TX) identifier 1
#define CN_PCAN_ACR2         (18)        // Acceptance code 2/RX(TX) identifier 2
#define CN_PCAN_ACR3         (19)        // Acceptance code 3/RX(TX) data 1(SFF) or RX(TX) identifier 3(EFF)
#define CN_PCAN_AMR0         (20)        // Acceptance mask 0/RX(TX) data 2(SFF) or RX(TX) identifier 4(EFF)
#define CN_PCAN_AMR1         (21)        // Acceptance mask 1/RX(TX) data 3(SFF) or RX(TX) data 1
#define CN_PCAN_AMR2         (22)        // Acceptance mask 2/RX(TX) data 4(SFF) or RX(TX) data 2
#define CN_PCAN_AMR3         (23)        // Acceptance mask 3/RX(TX) data 5(SFF) or RX(TX) data 3
#define CN_PCAN_RXD          (16)        // Receive buffer [16-26(SFF) or 16-28(EFF)]
#define CN_PCAN_RX_LEN       (16)        // RX frame Length register(SFF)
#define CN_PCAN_DSCR0        (16)        // Transmit buffer: Descriptor 0 (Length)
#define CN_PCAN_DSCR1        (17)        // Transmit buffer: Descriptor 1 (Identifier)
#define CN_PCAN_DSCR2        (18)        // Transmit buffer: Descriptor 2 (controller)
#define CN_PCAN_FRAMENO      (19)        // Transmit buffer: FrameNo
#define CN_PCAN_TXD          (20)        // Transmit buffer: Data [20-26(SFF) or 22-28(EFF)]
#define CN_PCAN_RXMC         (29)        // RX message counter register
#define CN_PCAN_RBSA         (30)        // RX buffer start address register
#define CN_PCAN_CDR          (31)        // Clock divider register

//定义SJA1000模式和控制寄存器MOD位
#define RM_RR_Bit   0x01     //复位模式请求位
#define LOM_Bit     0x02     //仅听模式位
#define STM_Bit     0x04     //自我测试模式位
#define AFM_Bit     0x08     //验收滤波器模式位
#define SM_Bit      0x10     //进入休眠模式位

//命令寄存器CMR位定义
#define TR_Bit    0x01      //发送请求位
#define AT_Bit    0x02      //中止发送位
#define RRB_Bit   0x04      //释放接收缓冲器位
#define CDO_Bit   0x08      //清除数据溢出位
#define SRR_Bit   0x10      //自接收请求位

//状态寄存器SR位定义
#define RBS_Bit   0x01      //接收缓冲器状态位
#define DOS_Bit   0x02      //数据溢出状态位
#define TBS_Bit   0x04      //发送缓冲器状态位
#define TCS_Bit   0x08      //发送完毕状态位
#define RS_Bit    0x10      //接收状态位
#define TS_Bit    0x20      //发送状态位
#define ES_Bit    0x40      //出错状态位
#define BS_Bit    0x80      //总线状态位

//中断寄存器IR位定义
#define RI_Bit   0x01       //接收中断状态位
#define TI_Bit   0x02       //发送中断状态位
#define EI_Bit   0x04       //错误报警中断状态位
#define DOI_Bit  0x08       //数据溢出中断状态位
#define WUI_Bit  0x10       //唤醒中断状态位
#define EPI_Bit  0x20       //错误消极中断状态位
#define ALI_Bit  0x40       //仲裁丢失中断状态位
#define BEI_Bit  0x80       //总线错误中断状态位


//中断使能寄存器IER位定义
#define RIE_Bit   0x01      //接收中断使能位
#define TIE_Bit   0x02      //发送中断使能位
#define EIE_Bit   0x04      //错误报警中断使能位
#define DOIE_Bit  0x08      //数据溢出中断使能位
#define WUIE_Bit  0x10      //唤醒中断使能位
#define EPIE_Bit  0x20      //错误消极中断使能位
#define ALIE_Bit  0x40      //冲裁丢失中断使能位
#define BEIE_Bit  0x80      //总线错误中断使能位

//定义CAN错误状态类型
#define ERR_ACTIVE    0x0   //主动错误
#define ERR_PASSIVE   0x1   //消极错误
#define ERR_BUS_OFF   0x2   //总线关闭



enum _CAN_BAUD_RATE_SET_ {                      // 定义CAN网波特率常量
    EN_CAN_BAUD_5K = 0,                     //    5 Kbps (0)
    EN_CAN_BAUD_10K,                        //   10 Kbps (1)
    EN_CAN_BAUD_20K,                        //   20 Kbps (2)
    EN_CAN_BAUD_40K,                        //   40 Kbps (3)
    EN_CAN_BAUD_50K,                        //   50 Kbps (4)
    EN_CAN_BAUD_80K,                        //   80 Kbps (5)
    EN_CAN_BAUD_100K,                     //   100 Kbps (6)
    EN_CAN_BAUD_125K,                     //   125 Kbps (7)
    EN_CAN_BAUD_200K,                     //   200 Kbps (8)
    EN_CAN_BAUD_250K,                     //   250 Kbps (9)
    EN_CAN_BAUD_400K,                     //   400 Kbps (10)
    EN_CAN_BAUD_500K,                    //    500 Kbps (11)
    EN_CAN_BAUD_666K,                    //    660 Kbps (12)
    EN_CAN_BAUD_800K,                    //    800 Kbps (13)
    EN_CAN_BAUD_1000K                   //  1000 Kbps (14)
};

enum _CAN_MODE_SET_{                           // 定义CAN 运行模式常量
    EN_MODE_PELI_CAN = 0,                   // PeliCAN  模式
    EN_MODE_BASIC_CAN                       // BasicCAN 模式
};


enum _CAN_CTRL_CMD_{
	EN_SET_BAUD,
	EN_SET_ADDR_FILTER,
};

typedef struct {
	struct tagRingBuf *rxBuf;
}CAN_RxTag;



typedef struct {
    CAN_RxTag* can_RxTag;
    u32 CANBaseAddr;
    u32 BaudRate;
    bool_t RecvFlag;
}CAN_DevCtrl;



bool_t CAN_Main(void);
u8 Can_Send(u8 byChipNum,u8 * txbuf,u8 len);


#ifdef __cplusplus
}
#endif

#endif /* _CAN_SJA1000_H_ */
