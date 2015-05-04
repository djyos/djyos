//----------------------------------------------------
//Copyright (C), 2013-20019,  lst.
//版权所有 (C), 2013-2019,   lst.
//所属模块:板件lvds驱动
//作者：lst
//版本：V1.0.0
//文件描述:fpga lvds驱动程序
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-09-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdlib.h"
#include "sunri_types.h"
#include "lvdsiodrv.h"
#include "fpga_address.h"

const UINT32 * FPGA_32_LVDSIOBASE = (UINT32 *) LVDS_IO_ADDRESS;
#define CN_LVDSIOBASE         FPGA_32_LVDSIOBASE
#define CN_LVDSIO_RFIFO     (LVDS_IO_ADDRESS + 0xE000)
#define CN_LVDSIO_WFIFO     (LVDS_IO_ADDRESS + 0xF000)
#define CN_KOBASE              0x0000
#define CN_STAMSGBASE        0x0000        //for slave sta buff
#define CN_CTLMSGBASE         0x0010        //for slave ctl buff
#define CN_KOBACKBASE         0x0000
#define CN_KIBASE              0x1000
#define CN_REGBASE             0x2000
#define CN_STABASE             0x2100
#define CN_WRLOCKBASE        0x3000
#define CN_BRDNUM            0x3001
#define CN_VERBASE             0xFF00

#define CN_CTLBUFFLG         16
#define CN_CTLBYTELG        12
#define CN_STABUFFLG        16
#define CN_STABYTELG        14
#define CN_MAXBOARD            16

#define CN_LVDSIOERRCNT     0x3F
#define CN_LVDSIOREVBIT     0x40
#define CN_LVDSIOXORBIT     0x80
#define CN_LVDSIOERRSET     5

#define    CN_LVDSCTLLOCK         0x90
#define CN_LVDSCTLVALID     0x40
#define CN_LVDSBOARDVALID     0xa5
#define CN_WRUNLOCK         0xa5


/***************以下为lvds总线数据功能宏定义*见《FPGA接口寄存器定义》***************/
#define CN_CONTROL_REG         0x3100     //reg addrs
#define CN_POLL_REG         0x3101
#define CN_TRANS_REG         0x3102
#define CN_MULTICAST         0x3103
#define CN_TRANS_STATUS     0x3104
#define CN_FIFO_STATUS         0x3105
#define CN_REG_LOCK         0x3106
#define CN_FIFO_FIFO_DONE     0x3107

UINT32   g_dwFifo_num;       //fifo中存储的完整数据包的总个数
UINT32   g_dwFifo_point;     //fifo中存储的完整数据的位置


/*========================================================*/
//* 函数功能：FPGA寄存器访问，写操作
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void LvdsFPGAWrite(UINT32 dwaddroffset,UINT32 dwdata)
{
 *(UINT32 *)(dwaddroffset+CN_LVDSIOBASE) = dwdata;
}
/*========================================================*/
//* 函数功能：LVDs数据总线模块软件复位
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void LvdsFPGAReset()
{
    LvdsFPGAWrite(CN_REG_LOCK,0xAAAA5555);
    LvdsFPGAWrite(CN_CONTROL_REG,LvdsFPGARead(CN_CONTROL_REG)&0xFFFeFFFF);
    g_dwFifo_num = 0;       //fifo中存储的完整数据包的总个数 清零
    g_dwFifo_point = 0;     //fifo中存储的完整数据的位置 清零
    LvdsFPGAWrite(CN_CONTROL_REG,LvdsFPGARead(CN_CONTROL_REG)|0x10000);
    LvdsFPGAWrite(CN_REG_LOCK,0);
}
void LvdsFPGAResetNot(){return; }
/*========================================================*/
//* 函数功能：LVDs数据总线模块初始化
//* 输入参数：板号，主从，组播列表 设置
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void LvdsFPGAInit(UINT8 byBnum,UINT8 byms,UINT16 wMulticastnum)
{
   LvdsFPGAReset();
   LvdsFPGAWrite(CN_REG_LOCK,0xAAAA5555);
   LvdsFPGAWrite(CN_CONTROL_REG,(1<<16) + (byBnum << 8) + byms);//设置本板板号，主从
   LvdsFPGAWrite(CN_MULTICAST,wMulticastnum);//设置组播列表
   LvdsFPGAWrite(CN_REG_LOCK,0);
}

/*========================================================*/
//* 函数功能：LVDs数据总线模块轮询控制
//* 输入参数：
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void LvdsFPGApoll(tagLvdsEnable *ptpoll)
{
    LvdsFPGAWrite(CN_REG_LOCK,0xAAAA5555);
    LvdsFPGAWrite(CN_POLL_REG,(ptpoll->blist << 16) + (ptpoll->tx_en << 2)
                    + (ptpoll->rx_en << 1) + ptpoll->poll_en);
    LvdsFPGAWrite(CN_REG_LOCK,0);
}

/*========================================================*/
//* 函数功能：LVDs数据总线模块发送控制
//* 输入参数：
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void LvdsFPGAtrans(tagLvdsTrans *pttrans)
{
    LvdsFPGAWrite(CN_REG_LOCK,0xAAAA5555);
    LvdsFPGAWrite(CN_TRANS_REG,(pttrans->desboard << 20)
                                + (pttrans->transmode << 16)
                                + pttrans->datalength
                                + (pttrans->ready << 19));
    LvdsFPGAWrite(CN_REG_LOCK,0);
}

/*========================================================*/
//* 函数功能：FPGA寄存器访问，读操作
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
UINT32 LvdsFPGARead(UINT32 dwaddroffset)
{
    return *(UINT32*)(dwaddroffset + CN_LVDSIOBASE);
}

/*========================================================*/
//* 函数功能：LVDs数据总线模块传输状态
//* 输入参数：
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
UINT32 LvdsFPGATransR()
{
    return LvdsFPGARead(CN_TRANS_STATUS);
}

/*========================================================*/
//* 函数功能：LVDs数据总线模块fifo状态
//* 输入参数：
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
UINT32 LvdsFPGAfifoSR()
{
    return LvdsFPGARead(CN_FIFO_STATUS);
}

/*========================================================*/
//* 函数功能：LVDs数据总线模块写数据
//* 输入参数：数据源，数据长度
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void LvdsFPGADataW(UINT32* dwdata, UINT32 wlength)
{
    UINT32 dwlgh;
    UINT8 sa,da,xorcheck;
	UINT32 times,loop;
	UINT64 LenChk;

    dwlgh = *(UINT32 *) dwdata;
    da = (dwlgh&0xff) + ((dwlgh>>10)&0xc0);         // 目标地址
    sa = (dwlgh>>8)&0xff;                           // 源地址
    xorcheck = 0xaa ^ wlength ^ sa ^ da;            // 效验码

    LenChk = (sa<<24) + (da<<8) + 0xaa00eb;
    LenChk = (LenChk<<32) + (xorcheck<<16)+wlength;
    dmacpy((unsigned long)CN_LVDSIO_WFIFO, (unsigned long) &LenChk, 8);

//    for(j=0; j < wlength; j++)
//        *(UINT32 *)(CN_LVDSIO_WFIFO) = *(UINT32 *) dwdata++;

	times = (wlength * 4)/128 + ((wlength * 4)%128 ? 1:0);
	for(loop = 0; loop < times; loop++)
        dmacpy((unsigned long)CN_LVDSIO_WFIFO,
              (unsigned long) dwdata + loop *128, 128);

    dwlgh = (LvdsFPGARead(CN_TRANS_REG)&0xffff0000) + wlength;
    LvdsFPGAWrite(CN_REG_LOCK,0xAAAA5555);
    LvdsFPGAWrite(CN_TRANS_REG,dwlgh | 0x80000);     // 数据准备完毕
    LvdsFPGAWrite(CN_REG_LOCK,0);
}

/*========================================================*/
//* 函数功能：LVDs数据总线模块读数据
//* 输入参数：数据源
//* 输出参数：无
//* 返回值  ：数据个数
/*========================================================*/
UINT32 LvdsFPGADataR(UINT32* dwdata)
{
    UINT32 i2,i3,dwNum,dwLen;
    UINT32 i1;
    UINT32 times,temp,loop,times2;

    i1 = LvdsFPGAfifoSR();                          // 读取目前FIFO数据状态
    i2 = i1 & 0xffff;                                 // 读取目前FIFO数据包个数
       i3 = i1 >> 16;                                    // 读取目前FIFO数据指针位置

    if (i2 < g_dwFifo_num)
        g_dwFifo_num = g_dwFifo_num - 65536;          // 超过65536自动翻转
    dwNum = i2 - g_dwFifo_num;                      // 计算数据包个数

    if (i3 < g_dwFifo_point)
        g_dwFifo_point = g_dwFifo_point - 65536;      // 超过65536自动翻转
    dwLen = i3 - g_dwFifo_point;                    // 计算报文总长度

    if( dwLen >= 4096 )                                // 报文长度容错
        return 0;

//    for(i1=0; i1<dwLen; i1++)
//        * dwdata ++ = *(UINT32 *)(CN_LVDSIO_RFIFO);
    times = (dwLen*8)/128;
    for(loop = 0;loop < times;loop++)
		dmacpy((unsigned long)dwdata + 128*loop,CN_LVDSIO_RFIFO, 128);

    temp = (dwLen*8)%128;
    if(temp)
    {
    	times2 = temp/4;
    	for(loop = 0;loop < times2;loop++)
    		* (dwdata + times*32 + loop) = *(UINT32 *)(CN_LVDSIO_RFIFO);
    }

    i1 = LvdsFPGARead(CN_FIFO_FIFO_DONE);           // FIFO数据已读结束

    g_dwFifo_num = i2;                              // 记录已读FIFO数据包个数
    g_dwFifo_point = i3;                            // 记录已读FIFO数据指针位置
    return dwNum;
}

/*========================================================*/
//* 函数功能：LVDs数据总线模块读数据准备好
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：BOOL
/*========================================================*/
BOOL LvdsFPGAReadReady()
{
    UINT32 dwVal;
    dwVal = LvdsFPGATransR();
    if ( dwVal&0x80000000 )                             // 是否存储一个完整数据帧
        return TRUE;
    else
        return FALSE;
}

/*========================================================*/
//* 函数功能：LVDs数据总线模块写数据准备好
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：BOOL
/*========================================================*/
BOOL LvdsFPGAWriteReady()
{
    UINT32 dwVal;
    dwVal = LvdsFPGATransR();
    if ( dwVal&0x40000000 )                             // 上一帧未发送
        return FALSE;
    else
        return TRUE;
}

/*========================================================*/
//* 函数功能：初始化LVDSIO 内存和控制寄存器，初始化后处于无效状态
//* 输入参数：无
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void Lvdsio_InitReg()
{
    UINT16 i,j;

    *(UINT32 *) (CN_LVDSIOBASE + CN_WRLOCKBASE) = CN_WRUNLOCK; //unlock the buffer write
    for(j=0;j<CN_MAXBOARD;j++)
    {
        for(i=0;i<CN_CTLBUFFLG;i++)
            *(UINT32 *) (CN_LVDSIOBASE + CN_KOBASE + i + j*CN_CTLBUFFLG) = 0;
        *(UINT32 *) (CN_LVDSIOBASE + CN_REGBASE+j) = 0;
    }
    *(UINT32 *) (CN_LVDSIOBASE + CN_KOBASE) = 0;        //fpga bug must be added

    *(UINT32 *) (CN_LVDSIOBASE + CN_WRLOCKBASE) = 0;    //lock the boffer write
}

/*========================================================*/
//* 函数功能：初始化并启动相应板件的LVDSIO控制
//* 输入参数：byBoardNum，byBoardType byCtlByte指针 闭锁（TRUE有效）
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void Lvdsio_StartBoard(UINT8 byBoardNum,UINT8 byBoardType, UINT8 * pbyCtlByte, BOOL bLock )
{
    tagLvdsioCtlFrm  * ptLvdsioCtl;
    UINT32 i;

    *(UINT32 *) (CN_LVDSIOBASE + CN_WRLOCKBASE) = CN_WRUNLOCK; //unlock the buffer write
    if (byBoardNum < CN_MAXBOARD)
        {
        ptLvdsioCtl = (tagLvdsioCtlFrm  *) (CN_LVDSIOBASE + CN_KOBASE + byBoardNum * CN_CTLBUFFLG);
        ptLvdsioCtl->bySourseAddr = 0xe0;
        ptLvdsioCtl->byBoardType = byBoardType;
        for(i=0; i<CN_CTLBYTELG; i++)
            ptLvdsioCtl->byCtlByte[i] = * (pbyCtlByte + i);
        if ( bLock == TRUE)
            ptLvdsioCtl->byLockByte = CN_LVDSCTLLOCK;
        else
            ptLvdsioCtl->byLockByte = 0;
        ptLvdsioCtl->byValidByte = CN_LVDSCTLVALID | byBoardNum;
        }
    *(UINT32 *) (CN_LVDSIOBASE + CN_KOBASE) = 0;        //fpga bug must be added
    *(UINT32 *) (CN_LVDSIOBASE + CN_WRLOCKBASE) = 0; //lock the buffer write
}

void         Lvdsio_UnlockCtl(UINT8 byBoardNum,BOOL bLock ){return; }
UINT8   g_byKiByte[16];
/*========================================================*/
//* 函数功能：取得KI缓冲区的指针
//* 输入参数：byBoardNum
//* 输出参数：无
//* 返回值  ：KI缓冲区的指针或空
/*========================================================*/
UINT8 * Lvdsio_GetStaBuff(UINT8 byBoardNum)
{
    UINT32 * pdwptr;
    UINT32 i;

    if (byBoardNum < CN_MAXBOARD)
    {
        pdwptr = (UINT32 *) (CN_LVDSIOBASE + CN_KIBASE + byBoardNum*CN_STABUFFLG);
        for (i=0; i<16; i++)
            g_byKiByte[i] = (UINT8) pdwptr[i];
        return g_byKiByte;
    }
    else
        return NULL;
}

/*========================================================*/
//* 函数功能：将KI缓冲区移入指定的内存
//* 输入参数：byBoardNum 板号
//* 输出参数：UINT32 * kibyte 排列顺序为 本板地址、板类型码、状态码（14BYTE)
//* 返回值  ：无
/*========================================================*/
void Lvdsio_GetStaMsg(UINT8 byBoardNum, UINT8 * pbyKiByte)
{
    UINT32 * pdwptr;
    UINT32 i;

    pdwptr = (UINT32 *)(CN_LVDSIOBASE + CN_KIBASE + byBoardNum*CN_STABUFFLG);
    for (i=0; i<16; i++)
        * pbyKiByte ++ =(UINT8) (* pdwptr ++);
}
/*========================================================*/
//* 函数功能：获取板件接收状态，ERRCNT大于ERRSET 为FAULSE
//* 输入参数：byBoardNum
//* 输出参数：无
//* 返回值  ：true、false：正确接受状态帧
/*========================================================*/
BOOL Lvdsio_GetRevSta(UINT8 byBoardNum)
{
      UINT32 * pdwstaptr;
      UINT32 * pdwregptr;

      pdwstaptr = (UINT32 *)(CN_LVDSIOBASE + CN_STABASE + byBoardNum);
      pdwregptr = (UINT32 *)(CN_LVDSIOBASE + CN_REGBASE + byBoardNum);

    if ((byBoardNum < CN_MAXBOARD) &&
        (((*pdwstaptr) & CN_LVDSIOERRCNT) < CN_LVDSIOERRSET) &&  //2008-8-20 8:36:54
        (((*pdwregptr) & 0xff) == CN_LVDSBOARDVALID))    //ctl open
        return TRUE;
    else
        return FALSE;
}
/*========================================================*/
//* 函数功能：获取板件态
//* 输入参数：byBoardNum
//* 输出参数：无
//* 返回值  ：true、false：在线
/*========================================================*/
BOOL Lvdsio_GetBoardSta(UINT8 byBoardNum)
{
      UINT32 * pdwstaptr;
      UINT32 * pdwregptr;

      pdwstaptr = (UINT32 *)(CN_LVDSIOBASE + CN_STABASE + byBoardNum);
      pdwregptr = (UINT32 *)(CN_LVDSIOBASE + CN_REGBASE + byBoardNum);

    if ((byBoardNum < CN_MAXBOARD) &&
        (((*pdwstaptr) & CN_LVDSIOREVBIT) == 0) &&  //rev bit
        (((*pdwregptr) & 0xff) == CN_LVDSBOARDVALID))    //ctl open
        return TRUE;
    else
        return FALSE;
}
/*========================================================*/
//* 函数功能：设置相应板件的控制寄存器
//* 输入参数：byBoardNum,byCtlReg(TRUE 打开）
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void Lvdsio_SetCtlReg(UINT8 byBoardNum,UINT8 byCtlReg)
{
    if (byBoardNum < CN_MAXBOARD)
    {
        if (byCtlReg == TRUE)
            *(UINT32 *) (CN_LVDSIOBASE + CN_REGBASE + byBoardNum) = CN_LVDSBOARDVALID;
        else
            *(UINT32 *) (CN_LVDSIOBASE + CN_REGBASE + byBoardNum) = 0;
    }
}

/*========================================================*/
//* 函数功能：取得控制寄存器的状态，
//* 输入参数：byBoardNum
//* 输出参数：无
//* 返回值  ：TRUE/FALSE  相关板件LVDS控制总线开放
/*========================================================*/
BOOL Lvdsio_GetCtlReg(UINT8 byBoardNum)
{
    if ((byBoardNum < CN_MAXBOARD) &&
        ( *(UINT32 *) (CN_LVDSIOBASE + CN_REGBASE + byBoardNum) == CN_LVDSBOARDVALID))
        return TRUE;
    else
        return FALSE;
}

/*========================================================*/
//* 函数功能：设置从板板号
//* 输入参数：板号
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void Lvdsio_SetSlaveBrdNum(UINT8 byBoardNum)
{
    if (byBoardNum < CN_MAXBOARD)
        *(UINT32 *) (CN_LVDSIOBASE + CN_BRDNUM) = byBoardNum;
        else
        *(UINT32 *) (CN_LVDSIOBASE + CN_BRDNUM) = 0;
}

/*========================================================*/
//* 函数功能：设置从板状态帧,
//* 输入参数：UINT32 * pbyStaByte 排列顺序为 本板地址、板类型码、状态码（14BYTE)
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/
void Lvdsio_SetSlaveStaMsg(UINT8 * pbyStaByte)
{
    UINT32 * pdwptr;
    UINT32 i;

    pdwptr = (UINT32 *)(CN_LVDSIOBASE + CN_STAMSGBASE);
    for (i=0; i<16; i++)
        * pdwptr ++ =(UINT32) (* pbyStaByte ++);
}

/*========================================================*/
// 函数功能：接收从板控制帧（从主板发来）,
//* 输入参数：无
//* 输出参数：UINT32 * byCtlByte 排列顺序为 本板地址、板类型码、控制码（14BYTE)
//* 返回值  ：无
/*========================================================*/
void Lvdsio_SetSlaveCtlMsg(UINT8 * pbyCtlByte)
{
    UINT32 * pdwptr;
    UINT32 i;

    pdwptr = (UINT32 *)(CN_LVDSIOBASE + CN_CTLMSGBASE);
    for (i=0; i<16; i++)
        * pbyCtlByte ++ =(UINT8) (* pdwptr ++);
}
