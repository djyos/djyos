//----------------------------------------------------
//Copyright (C), 2013-2020,  lst.
//版权所有 (C), 2013-2020,   lst.
//所属模块:板件FPGA驱动
//作者：lst
//版本：V1.0.0
//文件描述:fpga timer驱动程序
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-09-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdlib.h"
#include "fpga_timer.h"
#include "sunri_types.h"

#include "fpga_address.h"

const UINT32 * FPGA_32_FPGATIMERBASE = (UINT32 *)FPGA_TIMER_ADDRESS;//FPGA TIMER基址，由FPGA提供
#define CN_FPGATIMERBASE         FPGA_32_FPGATIMERBASE
#define CN_FPGACINTCNT            (0x00)
#define CN_FPGA10MCNT             (0x02)
#define CN_FPGA100MCNT             (0x03)
#define CN_FPGASECCNT             (0x04)
#define CN_FPGAMILLCNT             (0x05)
#define CN_FPGAMILHCNT             (0x06)
#define CN_FPGACNTLAT            (0x07)
#define CN_FPGAAUTORELOAD         (0x08)
#define CN_FPGACNTADD            (0x0a)

#define CN_FPGACINTSET            (0x10)
#define CN_FPGA10MSET             (0x12)
#define CN_FPGA100MSET             (0x13)
#define CN_FPGASECSET             (0x14)
#define CN_FPGAMILLSET             (0x15)
#define CN_FPGAMILHSET             (0x16)
#define CN_FPGAMANULOAD         (0x17)

#define CN_FPGASAMCMP            (0x18)
#define CN_FPGA10MCMP             (0x1A)
#define CN_FPGA10MCPS             (0x1B)
#define CN_FPGA100MCPS             (0x1C)
#define CN_FPGASECCPS             (0x1D)
#define CN_FPGAREVERR             (0x1E)
#define CN_FPGAMANI             (0x1F)

#define CN_FPGA1588REG            (0x20)
#define CN_FPGA1588STA            (0x24)
#define CN_FPGACINTREG             (0x2A)
#define CN_FPGALVDSERRNUM        (3)

#define CN_FPGA1588MAC            (0x30)

#define CN_FPGACLKFRQ            (25000000)
#define CN_FPGATMRSETVALID         (0xa5)

const UINT32 * FPGA_32_FPGAMSBASE = (UINT32 *) FPGA_MS_ADDRESS;
#define CN_FPGAMSBASE     FPGA_32_FPGAMSBASE

#define CN_FPGAMASTER           (0x02)
#define CN_FPGAMSETVALID        (0xa5)

#define CN_1588_GETFLAG 0xA0010C00      //获取1588新事件标志
#define CN_1588_CLRFLAG 0xA0010C04      //清除1588事件标志
#define CN_1588_MAC1RT0 0xA0011000      //获取MAC1收到的synctime
#define CN_1588_MAC1RT1 0xA0011004      //获取MAC1收到的dreqtime
#define CN_1588_MAC1RT2 0xA0011008      //获取MAC1收到的pdreqtime
#define CN_1588_MAC1RT3 0xA001100C      //获取MAC1收到的pdresptime
#define CN_1588_MAC1ST0 0xA0011010      //获取MAC1发出的synctime
#define CN_1588_MAC1ST1 0xA0011014      //获取MAC1发出的dreqtime
#define CN_1588_MAC1ST2 0xA0011018      //获取MAC1发出的pdreqtime
#define CN_1588_MAC1ST3 0xA001101C      //获取MAC1发出的pdresptime
#define CN_1588_MAC2RT0 0xA0011020      //获取MAC2收到的synctime
#define CN_1588_MAC2RT1 0xA0011024      //获取MAC2收到的dreqtime
#define CN_1588_MAC2RT2 0xA0011028      //获取MAC2收到的pdreqtime
#define CN_1588_MAC2RT3 0xA001102C      //获取MAC2收到的pdresptime
#define CN_1588_MAC2ST0 0xA0011030      //获取MAC2发出的synctime
#define CN_1588_MAC2ST1 0xA0011034      //获取MAC2发出的dreqtime
#define CN_1588_MAC2ST2 0xA0011038      //获取MAC2发出的pdreqtime
#define CN_1588_MAC2ST3 0xA001103C      //获取MAC2发出的pdresptime


//------取时钟频率--------------------------------------------
//功能：取FPGA系统时钟
//参数：无
//返回：FPGA系统时钟 UINT32
//-----------------------------------------------------------
UINT32 FpgaTmr_GetFrq(void)
{
    return CN_FPGACLKFRQ;
}

//------初始化补偿寄存器-------------------------------------
//功能：初始化补偿寄存器，
//参数：byCps10m：10ms补偿计数；byCps100m：100ms补偿计数；
//      byCpss：秒补偿计数
//返回：无
//----------------------------------------------------------
void FpgaTmr_SetCps(UINT8 byCps10m,    UINT8 byCps100m, UINT8 byCpss)
{
    UINT32 * pdwptr;
    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGA10MCPS;

    * pdwptr ++    = (UINT32) byCps10m;    //10MS 补偿数
    * pdwptr ++    = (UINT32) byCps100m;    //100MS 补偿数
    * pdwptr ++    = (UINT32) byCpss;        //S 补偿数
}

//------初始化补偿/比较寄存器------------------------------
//功能：初始化比较寄存器和补偿寄存器，
//参数：dwcintfrq：中断频率；byCps10m：10ms补偿计数；
//      byCps100m：100ms补偿计数；byCpss：秒补偿计数
//返回：无
//----------------------------------------------------------
void FpgaTmr_InitReg(UINT32 dwcintfrq, UINT8 byCps10m,
                            UINT8 byCps100m, UINT8 byCpss)
{
    UINT32 * pdwptr;
    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGASAMCMP;

    * pdwptr ++    = (UINT32)((CN_FPGACLKFRQ/dwcintfrq) & 0x0ff);        //中断计数低
    * pdwptr ++    = (UINT32)(((CN_FPGACLKFRQ/dwcintfrq) >>8)  & 0x0ff);//中断计数高
    * pdwptr ++    = (UINT32)(((dwcintfrq/100) & 0x0ff) - 1);        //10ms中断数
    * pdwptr ++    = (UINT32) byCps10m;    //10MS 补偿数
    * pdwptr ++    = (UINT32) byCps100m;    //100MS 补偿数
    * pdwptr ++    = (UINT32) byCpss;        //S 补偿数
}

//------设置比较寄存器--------------------------------------
//功能：设置FPGASAMCMP寄存器
//参数：wSamCmp：主中断计数值
//返回：无
//----------------------------------------------------------
void FpgaTmr_SetSamCmp(UINT16 wSamCmp)
{
    UINT32 * pdwptr;
    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGASAMCMP;
    * pdwptr ++    = (UINT32)(wSamCmp & 0x0ff);        //中断计数低
    * pdwptr ++    = (UINT32)((wSamCmp >>8) & 0x0ff);//中断计数高
}

//------手动设置校时增值------------------------------------
//功能：设置时钟预设寄存器，手动完成校时增值
//参数：wSamCmp：主中断计数值增值
//返回：无
//----------------------------------------------------------
void FpgaTmr_ManuTimeAdd(SINT16 nSamCmp)
{
    UINT32 * pdwptr;
    UINT16 i;

    i = abs(nSamCmp);
    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGACNTADD;
    * pdwptr ++    = (UINT32)(i & 0x0ff);        //中断计数低
    * pdwptr ++    = (UINT32)((i >>8)  & 0x0ff); //中断计数高

    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGAMANULOAD;
    if (nSamCmp>0)
        * pdwptr ++    = 0xAC;        //装载
    else
        * pdwptr ++    = 0xAD;        //装载
}

//------取FPGASAMCMP寄存器----------------------------------
//功能：取FPGASAMCMP寄存器
//参数：无
//返回：主中断计数值
//----------------------------------------------------------
UINT16 FpgaTmr_GetSamCmp(void)
{
    UINT32 dwSamCmpl,dwSamCmph;

    dwSamCmpl = (UINT32)((*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGASAMCMP))&0xff);
    dwSamCmph = (UINT32)((*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGASAMCMP + 1))&0xff);

    return ((UINT16)(dwSamCmpl + (dwSamCmph << 8)));
}

//------读比较寄存器----------------------------------------
//功能：取比较寄存器和补偿寄存器，按内存顺序存入指定地址，
//      共6个BYTE
//参数：pbyGetPtr 指针
//返回：无
//----------------------------------------------------------
void FpgaTmr_GetCmpReg(UINT8 * pbyGetPtr)
{
    UINT32 i;
    UINT32 * pdwptr;
    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGASAMCMP;

    for( i = 0; i < 6 ; i++)
        * pbyGetPtr ++ = (UINT8) (* pdwptr ++);
}

//------设置校时主机----------------------------------------
//功能：设置校时主机，设为校时主机，向LVDS总线发时钟
//参数：TRUE 设为主机
//返回：无
//----------------------------------------------------------
void FpgaTmr_SetMain(BOOL bSetMain)
{
    if (TRUE == bSetMain)
        *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAMANI) = CN_FPGATMRSETVALID;
    else
        *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAMANI) = 0;
}

//------取校时主机状态--------------------------------------
//功能：取校时主机状态
//参数：无
//返回：true: 成功
//      false: 失败
//----------------------------------------------------------
BOOL FpgaTmr_GetMainSta(void)
{
    if (CN_FPGATMRSETVALID == (UINT8)*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAMANI))
        return TRUE;
    else
        return FALSE;
}

//------设置自动装载-----------------------------------------
//功能：设置校时从机自动装载，LVDS自动刷新预设寄存器，自动装
//      入计数器，完成自动校时
//参数：TRUE 设为自动装载
//返回：无
//----------------------------------------------------------
void FpgaTmr_SetAutoLoad(BOOL bSetAutoLoad)
{
    if (TRUE == bSetAutoLoad)
        *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAAUTORELOAD) = CN_FPGATMRSETVALID;
    else
        *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAAUTORELOAD) = 0;
}

//------取自动装载状态--------------------------------------
//功能：取自动装载状态
//参数：无
//返回：
//----------------------------------------------------------
BOOL FpgaTmr_GetAutoReloadSta(void)
{
    if (CN_FPGATMRSETVALID == (UINT8)*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAAUTORELOAD))
        return TRUE;
    else
        return FALSE;
}

//------取校时总线状态--------------------------------------
//功能：取校时总线状态，若异常计数器大于CN_FPGAREVERR，返回FALSE
//参数：无
//返回：TRUE: 成功
//      FALSE: 失败
//----------------------------------------------------------
BOOL FpgaTmr_GetLvdsSta(void)
{
    if (*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAREVERR) <= CN_FPGALVDSERRNUM )
        return TRUE;
    else
        return FALSE;
}

//------手动装入计数器-------------------------------------
//功能：设置主中断计数器预设寄存器，手动装入计数器，
//参数：wSetCnt（二进制）:中断；bySet10ms（二进制）：10毫秒；
//      bySet100ms（BCD码）：百/拾；
//返回：无
//----------------------------------------------------------
void FpgaTmr_ManuLoad(UINT16 wSetCnt,UINT8 bySet10ms, UINT8 bySet100ms)
{
    UINT32 * pdwptr;
    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGACINTSET;

    * pdwptr ++    = (UINT32)((wSetCnt) & 0x0ff);
    * pdwptr ++    = (UINT32)((wSetCnt >>8)  & 0x0ff);
    * pdwptr ++    = (UINT32) bySet10ms;    //10ms 中断数
    * pdwptr ++    = (UINT32) bySet100ms;    //10 ms 100ms BCD

    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGAMANULOAD;
    * pdwptr ++    = CN_FPGATMRSETVALID;        //装载
}
//------手动完成校时---------------------------------------
//功能：设置时钟预设寄存器，手动完成校时
//参数：bySetSec（BCD码）：秒  wSetMilSec（BCD码）: 毫秒
//返回：无
//----------------------------------------------------------
void FpgaTmr_ManuTimeLoad(UINT8 bySetSec, UINT8 bySetMilSecL, UINT8 bySetMilSecH)
{
    UINT32 * pdwptr;
    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGASECSET;

    * pdwptr ++    = (UINT32) bySetSec;    //秒
    * pdwptr ++    = (UINT32) bySetMilSecL;
    * pdwptr ++    = (UINT32) bySetMilSecH;

    * pdwptr ++    = 0xA3;        //装载
}
//------取预设寄存器----------------------------------------
//功能：取预设寄存器，按原始内存顺序存入指定地址，共7个BYTE
//参数：pbyGetPtr 指针
//返回：无
//----------------------------------------------------------
void FpgaTmr_GetSetReg(UINT8 * pbyGetPtr)
{
    UINT32 i;
    UINT32 * pdwptr;
    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGACINTSET;

    for( i = 0; i < 7 ; i++)
        * pbyGetPtr ++ =  (UINT8) (* pdwptr ++);
}

//------取计数寄存器----------------------------------------
//功能：取计数寄存器，按原始内存顺序存入指定地址，共7个BYTE
//参数：pbyGetPtr 指针
//返回：无
// 需时在1.8-2.5us
//----------------------------------------------------------
void FpgaTmr_GetCntReg(UINT8 * pbyGetPtr)
{
    UINT32 i;
    UINT32 * pdwptr;

    pdwptr = (UINT32 *) CN_FPGATIMERBASE + CN_FPGACINTCNT;

    for( i = 0; i < 7 ; i++)
        * pbyGetPtr ++ = (UINT8)((* (pdwptr ++))&0xff);

}

//------读时钟-----------------------------------------------
//功能：取时钟（仅到毫秒），共3个BYTE，顺序为MS1，MS2,S
//      采用BCD格式，一般用于存储时间
//参数：pbyGetPtr 指针
//返回：无
// 需时1us
//----------------------------------------------------------
void FpgaTmr_GetTime(UINT8 * pbyGetPtr)
{
    //空读1个锁存,防止恰好进位时读数据
    *(volatile UINT32 *) (CN_FPGATIMERBASE + CN_FPGACINTCNT);

    * pbyGetPtr++ = (UINT8) (*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAMILLCNT));    //毫秒低
    * pbyGetPtr++ = (UINT8) (*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGAMILHCNT));    //毫秒高
    * pbyGetPtr++ = (UINT8) (*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGASECCNT));    //秒
}

//------取中断计数寄存器------------------------------------
//功能：取中断计数寄存器
//参数：无
//返回：UINT16 计数值
//----------------------------------------------------------
UINT16 FpgaTmr_GetCintCnt(void)
{
    UINT32 dwcnl,dwcnh;

    dwcnl = (UINT32)((*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGACINTCNT))&0xff);
    dwcnh = (UINT32)((*(UINT32 *) (CN_FPGATIMERBASE + CN_FPGACINTCNT + 1))&0xff);

    return ((UINT16)(dwcnl + (dwcnh << 8)));
}

//------取中断标志位----------------------------------------
//功能：取中断标志位
//参数：无
//返回：TURE 有中断标志
//----------------------------------------------------------
BOOL FpgaTmr_GetCintFlg(void)
{
    if (0 == *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGACINTREG))
        return FALSE;
    else
        return TRUE;
}

//------清中断标志位----------------------------------------
//功能：清中断标志位
//参数：无
//返回：无
//----------------------------------------------------------
void FpgaTmr_SetCintFlg(void)
{
    *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGACINTREG) = 0;
}

//------取1588标志位----------------------------------------
//功能：取1588标志位
//参数：无
//返回：TURE 有1588报文到
//----------------------------------------------------------
BOOL FpgaTmr_Get1588Flg(void)
{
    if (0 == *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGA1588STA))
        return FALSE;
    else
        return TRUE;
}

//------清1588标志位----------------------------------------
//功能：清1588标志位
//参数：无
// 输出参数：无
//返回：无
//----------------------------------------------------------
void FpgaTmr_Set1588Flg(void)
{
    *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGA1588STA) = 0;
}

//------MAC地址配置--------------------------------------------
//功能：除广播报文外，1588接收节点MAC地址
//参数：MAC地址
//返回：无
//----------------------------------------------------------
void FpgaTmr_Set1588Mac(UINT8 * pbyMacAddr)
{
    *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGA1588MAC) = (UINT32) (* pbyMacAddr);
    *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGA1588MAC + 1) = (UINT32) (* (pbyMacAddr + 1));
    *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGA1588MAC + 2) = (UINT32) (* (pbyMacAddr + 2));
    *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGA1588MAC + 3) = (UINT32) (* (pbyMacAddr + 3));
    *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGA1588MAC + 4) = (UINT32) (* (pbyMacAddr + 4));
    *(UINT32 *) (CN_FPGATIMERBASE + CN_FPGA1588MAC + 5) = (UINT32) (* (pbyMacAddr + 5));
}

BOOL         FpgaTmr_Get1588FlgAB(UINT32 dwPortNum){return FALSE; }
void         FpgaTmr_Set1588FlgAB(UINT32 dwPortNum){ }
UINT16         FpgaTmr_Get1588RegAB(UINT32 dwPortNum,UINT8 * pbyCintCntLatch,UINT8 * pbyMsCntLatch){return 0; }
void         FpgaTmr_Set1588MacAB(UINT32 dwPortNum,UINT8 * pbyMacAddr){ }

//------设置主从FPGA----------------------------------------
//功能：设置主从FPGA
//参数：无
//返回：无
//----------------------------------------------------------
void Fpga_SetMFun( void )
{
    *(UINT32 *) (CN_FPGAMSBASE + CN_FPGAMASTER) = CN_FPGAMSETVALID;
}

//------返回FPGA保存1588标志--------------------------------
//功能：返回FPGA保存1588标志
//参数：无
//返回：1588标志
//----------------------------------------------------------
UINT32 IEEE1588_GETFLAG(UINT8 macip)
{
    if ( macip ==0 )
    {
        return (*(UINT32*)(CN_1588_GETFLAG))&0xff;
    }
    else
    {
        return (*(UINT32*)(CN_1588_GETFLAG))>>8;
    }
}

//------清FPGA已读1588标志----------------------------------
//功能：清FPGA已读1588标志
//参数：1588标志
//返回：无
//----------------------------------------------------------
void IEEE1588_SETFLAG(UINT32 data,UINT8 macip)
{
    if ( macip ==0 )
    {
        *(UINT32 *)(CN_1588_CLRFLAG) = data;
    }
    else
    {
        *(UINT32 *)(CN_1588_CLRFLAG) = ((data<<8) | 0xff);
    }
    *(UINT32 *)(CN_1588_CLRFLAG) = 0xffff;
}

//------返回FPGA保存1588时间--------------------------------
//功能：返回FPGA保存1588时间
//参数：1588时间标号
//返回：1588时间
//----------------------------------------------------------
UINT32 IEEE1588_GETTIME(UINT8 data,UINT8 macip)
{
    if ( macip ==0 )
    {
        switch (data)
        {
            case 0x00:
                return *(UINT32*)(CN_1588_MAC1RT0);
            break;
            case 0x01:
                return *(UINT32*)(CN_1588_MAC1RT1);
            break;
            case 0x02:
                return *(UINT32*)(CN_1588_MAC1RT2);
            break;
            case 0x03:
                return *(UINT32*)(CN_1588_MAC1RT3);
            break;
            case 0x04:
                return *(UINT32*)(CN_1588_MAC1ST0);
            break;
            case 0x05:
                return *(UINT32*)(CN_1588_MAC1ST1);
            break;
            case 0x06:
                return *(UINT32*)(CN_1588_MAC1ST2);
            break;
            case 0x07:
                return *(UINT32*)(CN_1588_MAC1ST3);
            break;
            default:
                return 0;
            break;
        }
    }
    else
    {
        switch (data)
        {
            case 0x00:
                return *(UINT32*)(CN_1588_MAC2RT0);
            break;
            case 0x01:
                return *(UINT32*)(CN_1588_MAC2RT1);
            break;
            case 0x02:
                return *(UINT32*)(CN_1588_MAC2RT2);
            break;
            case 0x03:
                return *(UINT32*)(CN_1588_MAC2RT3);
            break;
            case 0x04:
                return *(UINT32*)(CN_1588_MAC2ST0);
            break;
            case 0x05:
                return *(UINT32*)(CN_1588_MAC2ST1);
            break;
            case 0x06:
                return *(UINT32*)(CN_1588_MAC2ST2);
            break;
            case 0x07:
                return *(UINT32*)(CN_1588_MAC2ST3);
            break;
            default:
                return 0;
            break;
        }
    }
    return 0;
}




















