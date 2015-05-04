//----------------------------------------------------
//Copyright (C), 2013-2020,  都江堰操作系统开发团队
//版权所有 (C), 2013-2020,   都江堰操作系统开发团队
//所属模块: sport 模拟SPI 驱动程序(具体驱动部分)
//作者:  朱海兵.
//版本：V1.0.0
//文件描述:
//其他说明:
//修订历史:
//1. 日期:2014-4-23
//   作者:  朱海兵.
//   新版本号：1.0.0
//   修改说明: 
//------------------------------------------------------
#ifndef __CPU_PERI_SPORT_H__
#define __CPU_PERI_SPORT_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BIT_0   0x00000001
#define BIT_1   0x00000002
#define BIT_2   0x00000004
#define BIT_3   0x00000008
#define BIT_4   0x00000010
#define BIT_5   0x00000020
#define BIT_6   0x00000040
#define BIT_7   0x00000080
#define BIT_8   0x00000100
#define BIT_9   0x00000200
#define BIT_10  0x00000400
#define BIT_11  0x00000800
#define BIT_12  0x00001000
#define BIT_13  0x00002000
#define BIT_14  0x00004000
#define BIT_15  0x00008000
#define BIT_16  0x00010000
#define BIT_17  0x00020000
#define BIT_18  0x00040000
#define BIT_19  0x00080000
#define BIT_20  0x00100000
#define BIT_21  0x00200000
#define BIT_22  0x00400000
#define BIT_23  0x00800000
#define BIT_24  0x01000000
#define BIT_25  0x02000000
#define BIT_26  0x04000000
#define BIT_27  0x08000000
#define BIT_28  0x10000000
#define BIT_29  0x20000000
#define BIT_30  0x40000000
#define BIT_31  0x80000000



#define DB_S_Empty (0x00000000)
#define DB_S_Half     (BIT_28)
#define DB_S_Full      (BIT_27|BIT_28)

#define DA_S_Empty  (0x00000000)
#define DA_S_Half     (BIT_31)
#define DA_S_Full      (BIT_30|BIT_31)


#define Baud_50    50000000
#define Baud_20    20000000
#define Baud_10    10000000
#define Baud_5      5000000

typedef struct         //offset
{
    u32 rSPCTL_L;       		//0x00
    u32 rSPCTL_H;      		//0x01
    u32 rDIV_L;		//0x02
    u32 rDIV_H;                       //0x03
    u32 rSPMCTL_L;       		//0x04
    u32 REVERSED[12];            //0x5~0x16
    u32 rSPMCTL_H;       		//0x17
}tag_SportReg;



typedef struct         //offset
{
    u32 rIISPA;				//0x00
    u32 rIMSPA;		           //0x01
    u32 rCSPA;				//0x02
    u32 rCPSPA;			//0x03
    u32 rIISPB ;                                     //0x04
    u32 rIMSPB;		           //0x05
    u32 rCSPB;				//0x06
    u32 rCPSPB;                                   //0x07
}tag_DMA_SportReg;

typedef struct 
{
   u32 rTXSPA_L;
   u32 rRXSPA_L;
   u32 rTXSPB_L;
   u32 rRXSPB_L;
   u32 rTXSPA_H;
   u32 rRXSPA_H;
   u32 rTXSPB_H;
   u32 rRXSPB_H;
}tag_TXRX_SportReg;


bool_t EnableSport(u32 SportNum);
bool_t DisableSport(u32 SportNum);
//bool_t SportInit(u32 SportGroupNum,u32 Index,u8 TransMode,u32 Baud,u32 DMAMode);
void SportInit(u32 Baud);
bool_t EnableSportDMA(u32 SportGroupNum,u32 Index );
bool_t DisableSportDMA(u32 SportGroupNum,u32 Index );
bool_t InitSportDMA(u32 SportNum,u8* pData,u32 DataLen);


u32 SPORT1_TX_IRQ(ufast_t ufl_line);
u32 SPORT1_RX_IRQ(ufast_t ufl_line);
u32 SPORT0_RX_IRQ(ufast_t ufl_line);
u32 SPORT0_TX_IRQ(ufast_t ufl_line);
bool_t Sport_Interrupt_Init(u32 SportNum,u32 TransMode);

bool_t SPORT_TX_byWord(u32 SportNum,u8 Data);
bool_t SPORT_RX_byWord(u32 SportNum,u8 *pData);
bool_t SPORT_TX_SEM(u32 SportNum,u8 *pData,u32 Len);
bool_t SPORT_RX_SEM(u32 SportNum,u8 *pData,u32 Len);



bool_t SPORT_Check_TX_Ready(u32 SportGroupNum,u32 Index);
bool_t SPORT_Check_RX_Ready(u32 SportGroupNum,u32 Index);
UINT32 SPORT_txByte(u32 SportGroupNum,u32 Index,u8 Data);
UINT32 SPORT_rxByte(u32 SportGroupNum,u32 Index,u8 *pData);
UINT32 SPORT_txArray(u32 SportGroupNum,u32 Index,u8 *send_buf,u32 len);
UINT32 SPORT_rxArray(u32 SportGroupNum,u32 Index,u8 *recv_buf,u32 len);


#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_SPORT_H__

