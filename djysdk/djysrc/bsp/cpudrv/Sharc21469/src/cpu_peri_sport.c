//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   朱海兵.
//所属模块: Sport驱动模块
//作者:  朱海兵.
//版本：V1.0.0
//文件描述: SPORT口驱动文件，SPORT模拟SPI
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2014-04-24
//   作者:  朱海兵.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "string.h"
#include "int.h"
#include "cpu_peri.h"

#include "def21469.h"



bool_t SPORT1_Send_Flag=false;
bool_t SPORT0_Recv_Flag=false;
bool_t SPORT1_Recv_Flag=false;
bool_t SPORT0_Send_Flag=false;

volatile tag_SportReg * const pg_sport_group1_reg =  (volatile tag_SportReg *)cn_sport_group1_baddr;
volatile tag_SportReg * const pg_sport_group2_reg =  (volatile tag_SportReg *)cn_sport_group2_baddr;
volatile tag_SportReg * const pg_sport_group3_reg =  (volatile tag_SportReg *)cn_sport_group3_baddr;
volatile tag_SportReg * const pg_sport_group4_reg =  (volatile tag_SportReg *)cn_sport_group4_baddr;

volatile tag_DMA_SportReg *const  pg_sport0_dma_reg=(volatile tag_DMA_SportReg* )cn_dma_sport0_baddr;
volatile tag_DMA_SportReg *const  pg_sport1_dma_reg=(volatile tag_DMA_SportReg* )cn_dma_sport1_baddr;
volatile tag_DMA_SportReg *const  pg_sport2_dma_reg=(volatile tag_DMA_SportReg* )cn_dma_sport2_baddr;
volatile tag_DMA_SportReg *const  pg_sport3_dma_reg=(volatile tag_DMA_SportReg* )cn_dma_sport3_baddr;
volatile tag_DMA_SportReg *const  pg_sport4_dma_reg=(volatile tag_DMA_SportReg* )cn_dma_sport4_baddr;
volatile tag_DMA_SportReg *const  pg_sport5_dma_reg=(volatile tag_DMA_SportReg* )cn_dma_sport5_baddr;
volatile tag_DMA_SportReg *const  pg_sport6_dma_reg=(volatile tag_DMA_SportReg* )cn_dma_sport6_baddr;
volatile tag_DMA_SportReg *const  pg_sport7_dma_reg=(volatile tag_DMA_SportReg* )cn_dma_sport7_baddr;

volatile tag_TXRX_SportReg *const pg_sport_group1_txrx_reg=(volatile tag_TXRX_SportReg *)cn_sport_txrx_group1_baddr;
volatile tag_TXRX_SportReg *const pg_sport_group2_txrx_reg=(volatile tag_TXRX_SportReg *)cn_sport_txrx_group2_baddr;
volatile tag_TXRX_SportReg *const pg_sport_group3_txrx_reg=(volatile tag_TXRX_SportReg *)cn_sport_txrx_group3_baddr;
volatile tag_TXRX_SportReg *const pg_sport_group4_txrx_reg=(volatile tag_TXRX_SportReg *)cn_sport_txrx_group4_baddr;





// ----SPORT模块模拟SPI发送数据-------------------------------------------
//功能：使能SPORT某个端口
//参数：SportNum，SPORT口编号，SPORT0~SPORT7对应编号分别为1~8；
//返回：true=成功，false=失败
// ------------------------------------------------------------------------


bool_t EnableSport(u32 SportNum)
{
   switch (SportNum)
    {
    case 0:
            *pSPCTL0=SPEN_A;
              break;
            case 1:
            *pSPCTL1=SPEN_A;
        break;
            case 2:
            *pSPCTL2=SPEN_A;
              break;
            case 3:
            *pSPCTL3=SPEN_A;
        break;

            case 4:
            *pSPCTL4=SPEN_A;
              break;
            case 5:
            *pSPCTL5=SPEN_A;
        break;
             case 6:
            *pSPCTL6=SPEN_A;
              break;
            case 7:
            *pSPCTL7=SPEN_A;
        break;

    default :
        return false;

    }

        return true;
}




// ----SPORT模块模拟SPI发送数据-------------------------------------------
//功能：禁能SPORT某个端口
//参数：SportNum，SPORT口编号，SPORT0~SPORT7对应编号分别为1~8；
//返回：成功返回true，失败返回false
// ------------------------------------------------------------------------


bool_t DisableSport(u32 SportNum)
{
   switch (SportNum)
    {
    case 0:
            *pSPCTL0&=(~SPEN_A);
              break;
            case 1:
            *pSPCTL1&=(~SPEN_A);
        break;
            case 2:
            *pSPCTL2&=(~SPEN_A);
              break;
            case 3:
            *pSPCTL3&=(~SPEN_A);
        break;

            case 4:
            *pSPCTL4&=(~SPEN_A);
              break;
            case 5:
            *pSPCTL5&=(~SPEN_A);
        break;
             case 6:
            *pSPCTL6&=(~SPEN_A);
              break;
            case 7:
            *pSPCTL7&=(~SPEN_A);
        break;

    default :
        return false;

    }

        return true;
}


// ----SPORT Init------------------------------------------
//功能：配置SPORT0/SPORT1中断线及初始化，配置SPORT时钟及初始化SPORT控制寄存器
//参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//      Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//      里SPORT口，Index=0表示SPORT编号大的那一个，Index=1表示SPORT编号小的那一个。
//      TransMode，TransMode=0,SPORT配置为发送端，TransMode=1,SPORT配置为接收端
//      Baud,SPORT传输速率，有40,000000、25,000000、10,000000、5,000000四个可选择
//      DMAMode,SPORT数据传输模式，DMAMode=0表示无DMA，内核传输；DMAMode=1表示Standard DMA；
//      DMAMode=2表示Chained-DMA。
//返回：成功返回true，失败返回false
// --------------------------------------------------------------
/*
bool_t SportInit(u32 SportGroupNum,u32 Index,u8 TransMode,u32 Baud,u32 DMAMode)
{
     volatile  tag_SportReg *tpsport;
     u16 temp=0;
     switch(SportGroupNum)
        {
        case 1:
        tpsport=pg_sport_group1_reg ;
        break;
    case 2:
        tpsport=pg_sport_group2_reg ;
        break;
    case 3:
        tpsport=pg_sport_group3_reg ;
        break;
    case 4:
                tpsport=pg_sport_group1_reg ;
        break;
    default :
        return false;
        }
     if(Index==0)
        {
           temp=CN_CFG_PCLK1/(4*Baud)-1;
           tpsport->rDIV_H|=temp;
           //tpsport->rSPCTL_H=0;
           //tpsport->rSPMCTL_H=0;

        if(TransMode==0)
            {

        if(DMAMode==0)
            {
            tpsport->rSPCTL_H=SPTRAN;
            }
        else if(DMAMode==1)
            {
            tpsport->rSPMCTL_H|=ETDINTEN;
            tpsport->rSPCTL_H|=(CKRE |LFS| LAFS | SPTRAN|IFS| SLEN8 | FSR);
            }
        else if(DMAMode==2)
            {
            }
            }
    else if (TransMode==1)
        {
        if(DMAMode==0)
            {
            }
        else if (DMAMode==1)
            {
            tpsport->rSPCTL_H=( SLEN8 | LAFS| FSR | DITFS| LFS );
            }
        else if (DMAMode==2)
            {
            }
        else
            {
            return false;
            }
        }
        }
     else if (Index==1)
        {
         temp=CN_CFG_PCLK1/(4*Baud)-1;
     tpsport->rDIV_L|=temp;
     //tpsport->rSPCTL_L=0;
     //tpsport->rSPMCTL_L=0;

        if(TransMode==0)
            {

        if(DMAMode==0)
            {
            tpsport->rSPCTL_L=SPTRAN;
            }
        else if(DMAMode==1)
            {
            tpsport->rSPMCTL_L|=ETDINTEN;
            tpsport->rSPCTL_L|=(CKRE |LFS| LAFS | SPTRAN  | SLEN8 | FSR);
            }
        else if(DMAMode==2)
            {
            }
            }
    else if (TransMode==1)
        {
        if(DMAMode==0)
            {
            }
        else if (DMAMode==1)
            {
            tpsport->rSPCTL_L=( SLEN8 | LAFS| FSR | DITFS| LFS );
            }
        else if (DMAMode==2)
            {
            }
        else
            {
            return false;
            }
        }
        }
     else
        {
        return false;
        }
        return true;
}
*/


 void SportInit(u32 Baud)
 {
    volatile  tag_SportReg *tpsport;
    u16 temp=0;
    tpsport=pg_sport_group1_reg ;
    temp=CN_CFG_PCLK1/(4*Baud)-1;
    tpsport->rDIV_H|=temp;

    tpsport->rSPCTL_L|=FSR|LAFS|LFS|SLEN8|CKRE;  //SPORT0 收
    tpsport->rSPCTL_H|=FSR|LAFS|IFS|LFS|SLEN8|ICLK|SPTRAN;  //SPORT1发


    //tpsport->rSPCTL_L|=FSR|LAFS|LFS|CKRE|SLEN8|SPTRAN;
    //tpsport->rSPCTL_H|=FSR|LAFS|IFS|SLEN8|ICLK;

 }




/*-------使能SPORT DMA----------------------------------------------**
//  功能：使能SPORT DMA，即启动DMA，通过DMA发送数据时，步骤为：首先使能SPORT，然后配置
//        DMA传输参数，最后使能DMA，即可启动一次DMA传输。
//  参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//        Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//        里SPORT口，Index=0表示SPORT编号大的那一个，Index=1表示SPORT编号小的那一个。
//  返回：成功返回true，失败返回false。

**--------------------------------------------------------------------*/
bool_t EnableSportDMA(u32 SportGroupNum,u32 Index )
{
    volatile  tag_SportReg *tpsport;
     switch(SportGroupNum)
        {
        case 1:
        tpsport=pg_sport_group1_reg ;
        break;
    case 2:
        tpsport=pg_sport_group2_reg ;
        break;
    case 3:
        tpsport=pg_sport_group3_reg ;
        break;
    case 4:
                tpsport=pg_sport_group1_reg ;
        break;
    default :
        return false;
        }
     if(Index==0)
        {
        tpsport->rSPCTL_H|=SDEN_A;
        }
     else if(Index==1)
        {
        tpsport->rSPCTL_L|=SDEN_A;
        }
        return true;
}


/*--------禁能SPORT DMA---------------------------------------------**
//  功能：禁能DMA。
//  参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//        Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//        里SPORT口，Index=0表示SPORT编号大的那一个，Index=1表示SPORT编号小的那一个。
    返回：成功返回true，失败返回false。

**--------------------------------------------------------------------*/
bool_t DisableSportDMA(u32 SportGroupNum,u32 Index )
{
    volatile  tag_SportReg *tpsport;
     switch(SportGroupNum)
        {
        case 1:
        tpsport=pg_sport_group1_reg ;
        break;
    case 2:
        tpsport=pg_sport_group2_reg ;
        break;
    case 3:
        tpsport=pg_sport_group3_reg ;
        break;
    case 4:
                tpsport=pg_sport_group1_reg ;
        break;
    default :
        return false;
        }
     if(Index==0)
        {
        tpsport->rSPCTL_H|=(~SDEN_A);
        }
     else if(Index==1)
        {
        tpsport->rSPCTL_L|=(~SDEN_A);
        }
        return true;
}



/*-------配置SPORT DMA参数----------------------------------------------**
    功能：配置SPORT DMA传输参数，包含三个参数，分别为IISP(DMA传输起始地址)、CSP(DMA传输长度)、
//        IMSP(DMA每次传输访问地址步进长度，默认都设为1)。
    参数：SportNum，SPORT口编号，SPORT0~SPORT7对应编号分别为1~8；
//        *pData，要传输的数据buffer指针；
//        DataLen,要传输的数据长度(字节数)
    返回：成功返回true，失败返回false。

**--------------------------------------------------------------------*/
bool_t InitSportDMA(u32 SportNum,u8* pData,u32 DataLen)
{
    volatile tag_DMA_SportReg *tgdmasport;
     switch(SportNum)
        {
        case 1:
        tgdmasport=pg_sport0_dma_reg;
        break;
    case 2:
        tgdmasport=pg_sport1_dma_reg;
        break;
    case 3:
        tgdmasport=pg_sport2_dma_reg;
        break;
    case 4:
                tgdmasport=pg_sport3_dma_reg;
        break;
    case 5:
         tgdmasport=pg_sport4_dma_reg;
         break;
            case 6:
        tgdmasport=pg_sport5_dma_reg;
        break;
    case 7:
                tgdmasport=pg_sport6_dma_reg;
        break;
    case 8:
         tgdmasport=pg_sport7_dma_reg;
         break;

    default :
        return false;
        }
     tgdmasport->rIISPA=(int)pData;
     tgdmasport->rIMSPA=1;
     tgdmasport->rCSPA=DataLen;
     return true;
}



// ----SPORT1模块发送中断服务函数-------------------------------------------
//功能：置发送成功标志位，同时禁能SPORT DMA。
//参数：中断号。
//返回：成功返回true，失败返回false。
// ------------------------------------------------------------------------

u32 SPORT1_TX_IRQ(ptu32_t ufl_line)
{

    SPORT1_Send_Flag=true;
    //DisableSport(1);
    DisableSportDMA(1, 0);
    return true;
}

// ----SPORT1模块接收中断服务函数-------------------------------------------
//功能：置接收成功标志位，同时禁能SPORT DMA。
//参数：中断号。
//返回：成功返回true，失败返回false。
// ------------------------------------------------------------------------

u32 SPORT1_RX_IRQ(ptu32_t ufl_line)
{
    SPORT1_Recv_Flag=true;
    //DisableSport(1);
    DisableSportDMA(1, 0);
    return true;
}

// ----SPORT0模块接收中断服务函数-------------------------------------------
//功能：置接收成功标志位，同时禁能SPORT DMA。
//参数：中断号。
//返回：成功返回true，失败返回false。
// ------------------------------------------------------------------------
u32 SPORT0_RX_IRQ(ptu32_t ufl_line)
{
    SPORT0_Recv_Flag=true;
    //DisableSport(0);
    DisableSportDMA(1, 1);
    return true;
}


// ----SPORT1模块发送中断服务函数-------------------------------------------
//功能：置发送成功标志位，同时禁能SPORT DMA。
//参数：中断号。
//返回：成功返回true，失败返回false。
// ------------------------------------------------------------------------
u32 SPORT0_TX_IRQ(ptu32_t ufl_line)
{
    SPORT0_Send_Flag=true;
    //DisableSport(0);
    DisableSportDMA(1, 1);
    return true;
}


// ----SPORT Init------------------------------------------
//功能：配置SPORT0/SPORT1中断线及初始化，配置SPORT时钟及初始化SPORT控制寄存器
//参数：SportNum，SPORT口编号，SPORT0~SPORT7对应编号分别为1~8；
//      TransMode，TransMode=0,SPORT配置为发送端，TransMode=1,SPORT配置为接收端
//返回：成功返回true，失败返回false。
// --------------------------------------------------------------
bool_t Sport_Interrupt_Init(u32 SportNum,u32 TransMode)
{
     if(SportNum==0)
        {
        Int_Register(cn_int_line_SP0I);
        if(TransMode==0)
            {
             Int_IsrConnect(cn_int_line_SP0I,SPORT0_TX_IRQ);

            }
    else if(TransMode==1)
        {
        Int_IsrConnect(cn_int_line_SP0I,SPORT0_RX_IRQ);
        }
    else
        {
        return false;
        }
        }
     else if (SportNum==1)
        {
        Int_Register(cn_int_line_SP1I);
        if(TransMode==0)
            {
             Int_IsrConnect(cn_int_line_SP1I,SPORT1_TX_IRQ);

            }
    else if(TransMode==1)
        {
        Int_IsrConnect(cn_int_line_SP1I,SPORT1_RX_IRQ);
        }
    else
        {
        return false;
        }
        }
     else
        {
        }
    return true;
}





//------通过DMA发送一个长度为Len buffer----------------------------------
  //  功能：通过DMA发送一个长度为Len buffer。
  //  参数：SportNum，SPORT口编号，SPORT0~SPORT7对应编号分别为1~8；
   //       *pData,发送数据buffer起始地址；
   //       Len,要发送数据长度。
//  返回：成功返回true，失败返回false。

//--------------------------------------------------------------------
/*
bool_t SPORT_TX_SEM(u32 SportNum,u8 *pData,u32 Len)
{
    InitSportDMA( SportNum, pData,Len);
    SportInit( 1,0,0, Baud_10,1);      //SPORT1 发，10MHz, Standard DMA
    EnableSportDMA(1, 0);               //使能DMA
    //EnableSport(1);                         //使能SPORT
     return true;
}


//---------通过DMA接收一个长度为Len buffer----------------------------
   // 功能：通过DMA接收一个长度为Len buffer。
   // 参数：SportNum，SPORT口编号，SPORT0~SPORT7对应编号分别为1~8；
    //      *pData,存放接收数据buffer起始地址；
     //     Len,要发送数据长度。
//  返回：成功返回true，失败返回false。
//--------------------------------------------------------------------
bool_t SPORT_RX_SEM(u32 SportNum,u8 *pData,u32 Len)
{
    InitSportDMA( SportNum, pData,Len);
    SportInit( 1,0,0, Baud_10,1);      //SPORT0 收 10MHz, Standard DMA
    EnableSportDMA(1, 1);               //使能DMA
   // EnableSport(0);                         //使能SPORT
     return true;
}
*/

// ----SPORT模块查写状态-------------------------------------------
//功能：确认写数据处于就绪状态
//参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//      Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//      里SPORT口，Index=0表示SPORT编号大的那一个，Index=1表示SPORT编号小的那一个。
//返回：成功返回true，失败返回false
// ------------------------------------------------------------------------
bool_t SPORT_Check_TX_Ready(u32 SportGroupNum,u32 Index)
{
    volatile  tag_SportReg *tpsport;
    int nTimeout=10000;

     switch(SportGroupNum)
        {
        case 1:
        tpsport=pg_sport_group1_reg ;
        break;
    case 2:
        tpsport=pg_sport_group2_reg ;
        break;
    case 3:
        tpsport=pg_sport_group3_reg ;
        break;
    case 4:
                tpsport=pg_sport_group1_reg ;
        break;
    default :
        return false;
        }
     if(Index==0)   //SPORT编号大的那个SPORT
        {
        while((DA_S_Full&tpsport->rSPCTL_H))
            {
            if(nTimeout--<0)
                {
                return false;
                }
            }
    return true;
        }
     else if(Index==1)
        {
        while((DA_S_Full&tpsport->rSPCTL_L))
            {
            if(nTimeout--<0)
                {
                return false;
                }
            }
    return true;
        }
     else
        {
        return false;
        }
}

// ----SPORT模块查写状态-------------------------------------------
//功能：确认读数据处于就绪状态
//参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//      Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//      里SPORT口，Index=0表示SPORT编号大的那一个，Index=1表示SPORT编号小的那一个。
//返回：成功返回true，失败返回false
// ------------------------------------------------------------------------
bool_t SPORT_Check_RX_Ready(u32 SportGroupNum,u32 Index)
{
   volatile  tag_SportReg *tpsport;
    int nTimeout=10000;

     switch(SportGroupNum)
        {
        case 1:
        tpsport=pg_sport_group1_reg ;
        break;
    case 2:
        tpsport=pg_sport_group2_reg ;
        break;
    case 3:
        tpsport=pg_sport_group3_reg ;
        break;
    case 4:
                tpsport=pg_sport_group1_reg ;
        break;
    default :
        return false;
        }
     if(Index==0)   //SPORT编号大的那个SPORT
        {
        while((DA_S_Empty&tpsport->rSPCTL_H))
            {
            if(nTimeout--<0)
                {
                return false;
                }
            }
    return true;
        }
     else if(Index==1)
        {
        while((DA_S_Empty&tpsport->rSPCTL_L))
            {
            if(nTimeout--<0)
                {
                return false;
                }
            }
    return true;
        }
     else
        {
        return false;
        }
}


// ----SPORT模块发送一个Byte数据-------------------------------------------
//功能：SPORT发送一个Byte数据
//参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//      Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//      里SPORT口，Index=0表示SPORT编号大的那一个，Index=1表示SPORT编号小的那一个。
//返回：成功返回true，失败返回false
// ------------------------------------------------------------------------
UINT32 SPORT_txByte(u32 SportGroupNum,u32 Index,u8 Data)
{
   UINT32 nStatus=0;
   volatile tag_TXRX_SportReg *tp_txrx_reg;

   if(SPORT_Check_TX_Ready(SportGroupNum,Index))
    {
    switch(SportGroupNum)
        {
        case 1:
        tp_txrx_reg=pg_sport_group1_txrx_reg;
        break;
        case 2:
        tp_txrx_reg=pg_sport_group2_txrx_reg;
        break;
        case 3:
        tp_txrx_reg=pg_sport_group3_txrx_reg;
        break;
        case 4:
        tp_txrx_reg=pg_sport_group4_txrx_reg;
        break;
        default:
        return false;
        }
    if(Index==0)
        {
        tp_txrx_reg->rTXSPA_H=Data;
        nStatus=0x1;
        }
    else if(Index==1)
        {
        tp_txrx_reg->rTXSPA_L=Data;
        nStatus=0x1;
        }
    else
        {
        return false;
        }
    }
   return nStatus;
}



// ----SPORT接收一个Byte数据-------------------------------------------
//功能：SPORT接收一个Byte数
//参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//      Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//      *pData，存放接收到数据的地址
//返回：成功返回true，失败返回false
// ------------------------------------------------------------------------
UINT32 SPORT_rxByte(u32 SportGroupNum,u32 Index,u8 *pData)
{
   UINT32 nStatus=0;
   volatile tag_TXRX_SportReg *tp_txrx_reg;

   if(SPORT_Check_TX_Ready(SportGroupNum,Index))
    {
    switch(SportGroupNum)
        {
        case 1:
        tp_txrx_reg=pg_sport_group1_txrx_reg;
        break;
        case 2:
        tp_txrx_reg=pg_sport_group2_txrx_reg;
        break;
        case 3:
        tp_txrx_reg=pg_sport_group3_txrx_reg;
        break;
        case 4:
        tp_txrx_reg=pg_sport_group4_txrx_reg;
        break;
        default:
        return false;
        }
    if(Index==0)
        {
        *pData=tp_txrx_reg->rRXSPA_H;
        nStatus=0x1;
        }
    else if(Index==1)
        {
        *pData=tp_txrx_reg->rRXSPA_L;
        nStatus=0x1;
        }
    else
        {
        return false;
        }
    }
   return nStatus;
}


// ----SPORT发送一个Buffer数据-------------------------------------------
//功能：SPORT发送长度为len的Buffer数据
//参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//      Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//      里SPORT口，Index=0表示SPORT编号大的那一个，Index=1表示SPORT编号小的那一个。
//      *send_buf，要发送的buffer首地址；
//      len，要发送的buffer长度(字节数)
//返回：成功返回true，失败返回false
// ------------------------------------------------------------------------
UINT32 SPORT_txArray(u32 SportGroupNum,u32 Index,u8 *send_buf,u32 len)
{
  volatile tag_TXRX_SportReg *tp_txrx_reg;

     u32 i=0;
    for(i=0;i<len;i++)
        {
        SPORT_txByte( SportGroupNum,  Index, *(send_buf+i));
        }
    return true;
}

// ----SPORT接收一个Buffer数据-------------------------------------------
//功能：SPORT接收长度为len的Buffer数据
//参数：SporGrouptNum,SPORT组编号，SPORT0~SPORT7 8个SPORT依次划分为4 groups
//      Index,一个group里包含两个SPORT，例如group1包含SPORT0、SPORT1，Index用于区分1个group
//      里SPORT口，Index=0表示SPORT编号大的那一个，Index=1表示SPORT编号小的那一个。
//      *recv_buf，存放接收数据的首地址；
//      len，要发送的buffer长度(字节数)
//返回：成功返回true，失败返回false
// ------------------------------------------------------------------------
UINT32 SPORT_rxArray(u32 SportGroupNum,u32 Index,u8 *recv_buf,u32 len)
{
     u32 i=0;
    for(i=0;i<len;i++)
        {
        SPORT_rxByte(SportGroupNum, Index, (recv_buf+i));
        }
    return true;
}

