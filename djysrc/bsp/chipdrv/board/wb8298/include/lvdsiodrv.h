/* Copyright 2007-2010 ShenZhen Nari, Inc.*/
/*modification history
struct for app...

Writen By zhangji, 2007,10,19
*/
//****************************************************************************************
// LVDS通讯部分
//****************************************************************************************

#ifndef _LVDSIODRV_H
#define _LVDSIODRV_H

#ifdef __cplusplus
extern "C" {
#endif
#include "sunri_types.h"
typedef struct
{
    UINT32          bySourseAddr;
    UINT32         byBoardType;
    UINT32          byCtlByte[12];
    UINT32        byLockByte;
    UINT32        byValidByte;
} tagLvdsioCtlFrm;  //控制帧格式
typedef struct
{
    UINT32       bySourseAddr;
    UINT32        byBoardType;
    UINT32       byStaByte[14];
} tagLvdsioStaFrm;    //状态帧格式
/********************************************************************/
typedef struct
{
    UINT16        blist;//板件列表，主板召唤数据时使用
    UINT8       tx_en;//发送使能，CPU端控制,1 使能，or 0
    UINT8       rx_en;//读取使能，CPU端控制,1 使能，or 0
    UINT8        poll_en;//召唤轮询使能，主板使用，从板不使能,1 使能，or 0
} tagLvdsEnable;  //LVDS 数据总线模块参数

typedef struct
{
    UINT8        desboard;   //目标板件地址
    UINT8        transmode;  //传送模式
    UINT16        datalength; //数据长度，单位 byte
    UINT8        ready;      //数据准备好=1 ；or 0
} tagLvdsTrans;//LVDS数据总线，发送设置参数

extern void LvdsFPGAReset();
extern void LvdsFPGAResetNot();
extern void LvdsFPGAInit(UINT8 byBnum,UINT8 byms,UINT16 wMulticastnum);
extern void LvdsFPGApoll(tagLvdsEnable *ptpoll);
extern void LvdsFPGAtrans(tagLvdsTrans *pttrans);
extern UINT32 LvdsFPGARead(UINT32 dwaddroffset);
extern UINT32 LvdsFPGATransR();
extern UINT32 LvdsFPGAfifoSR();
extern void LvdsFPGADataW(UINT32* dwdata,UINT32 wlength);
extern UINT32 LvdsFPGADataR(UINT32* dwdata);
extern BOOL LvdsFPGAWriteReady();
extern BOOL LvdsFPGAReadReady();

/********************************************************************/


extern  void         Lvdsio_InitReg();
extern  void         Lvdsio_StartBoard(UINT8 byBoardNum,UINT8 byBoardType, UINT8 * pbyCtlByte, BOOL bLock );
extern  void         Lvdsio_UnlockCtl(UINT8 byBoardNum,BOOL bLock );
//extern  UINT8 *     Lvdsio_GetCtlBuff(UINT8 byBoardNum);
extern  UINT8 *     Lvdsio_GetStaBuff(UINT8 byBoardNum);
extern  void        Lvdsio_SetStaMsg(UINT8 byBoardNum, UINT8 * pbyKiByte);
extern  BOOL         Lvdsio_GetRevSta(UINT8 byBoardNum);
extern  BOOL         Lvdsio_GetBoardSta(UINT8 byBoardNum);
extern  BOOL         Lvdsio_GetCtlReg(UINT8 byBoardNum);
extern  void        Lvdsio_SetSlaveBrdNum(UINT8 byBoardNum);
extern  void        Lvdsio_SetSlaveStaMsg(UINT8 * pbyStaByte);
extern  void        Lvdsio_SetSlaveCtlMsg(UINT8 * pbyCtlByte);
extern     void         Lvdsio_SetCtlReg(UINT8 byBoardNum,UINT8 byCtlReg);

#ifdef __cplusplus
}
#endif

#endif /* _LVDSIODRV_H */

