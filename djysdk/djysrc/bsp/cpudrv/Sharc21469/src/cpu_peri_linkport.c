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
//所属模块: linkport驱动程序(具体驱动部分)
//作者:  朱海兵.
//版本：V1.0.0
//文件描述:
//其他说明:
//修订历史:
//1. 日期:2014-4-25
//   作者:  朱海兵.
//   新版本号：1.0.0
//   修改说明:
//------------------------------------------------------



#include <string.h>
#include "stdint.h"
#include "cpu_peri.h"

#include "def21469.h"
#include "Cdef21469.h"

volatile tag_LinkPortReg * const pg_linkport0_reg =  (volatile tag_LinkPortReg *)cn_linkport0_baddr;
volatile tag_LinkPortReg * const pg_linkport1_reg =  (volatile tag_LinkPortReg *)cn_linkport1_baddr;
volatile tag_DMA_LinkPortReg * const pg_dma_linkport0_reg =  (volatile tag_DMA_LinkPortReg *)cn_dma_linkport0_baddr;
volatile tag_DMA_LinkPortReg * const pg_dma_linkport1_reg =  (volatile tag_DMA_LinkPortReg *)cn_dma_linkport1_baddr;

#define  DMA_OFFSET 0x80000

#define Rate_2       1
#define Rate_2_5   2
#define Rate_3       3
#define Rate_4        4

bool_t LP0SendFlag=false,LP1SendFlag=false;
bool_t LP0RecvFlag=false,LP1RecvFlag=false;

#define Chained_DMA_Max_Num   10
static  u32 DSP_FPGA_BUF_LEN[Chained_DMA_Max_Num];
static  u32 DSP_FPGA_BUF_ADDR[Chained_DMA_Max_Num];
static  u32 Chained_Send_Index=0x0;

struct t_LinkPortChainedDMA_TCB LinkPort_ChainedDMA_TCB[Chained_DMA_Max_Num]; //10个TCB块
struct t_LinkPortChainedDMA_TCB LinkPort_ChainedDMA_Send_TCB[Chained_DMA_Max_Num];

/*-------InitLinkPort初始化------------------------------------------------**
//  功能：配置LinkPort数据传输模式。
//  参数：LinkPortNum,LinkPort编号，LinkPortNum=0，指使用LinkPort0；LinkPortNum=1指使用LinkPort1。
//        TransMode,LinkPort传输模式，TransMode=0，LinkPort作为发送；TransMode=1,LinkPort作为接收。
//        DMAMode，DMAMode,LinkPort数据传输模式，DMAMode=0表示无DMA，内核传输；DMAMode=1表示Standard DMA；
//        DMAMode=2表示Chained-DMA。
//  返回：成功，返回true，否则返回false。
**--------------------------------------------------------------------*/
bool_t  LinkPort_Init(u32 LinkPortNum,u8 TransMode,u8 DMAMode)
{
      volatile tag_LinkPortReg *tplinkport;
    u32 i=0;
     if(LinkPortNum== 0)
    {
        tplinkport = pg_linkport0_reg;
    }
    else if (LinkPortNum==1)
    {
         tplinkport = pg_linkport1_reg;
    }
    else
    {
        return false;
    }

   if(TransMode==0)
    {
        tplinkport->rLCTL=0;
                if(DMAMode==0)
                    {
                    tplinkport->rLCTL=LTRAN|EXTTXFR_DONE_MSK;     //发送，Core模式，不使用DMA
                    }
                else if(DMAMode==1)
                    {
                    tplinkport->rLCTL=LTRAN | LDEN | EXTTXFR_DONE_MSK ; //LinkPort：发送  DMA使能
                    }
       else if(DMAMode==2)
        {
        tplinkport->rLCTL=LTRAN | LDEN | LCHEN|EXTTXFR_DONE_MSK ;
        }
        else
            {
            return false;
            }

    }
      else if(TransMode==1)
        {
         tplinkport->rLCTL=0;
                    if(DMAMode==0)
                    {
                        }
                else if(DMAMode==1)
                    {
                    tplinkport->rLCTL= LDEN | DMACH_IRPT_MSK ; //LinkPort：发送  DMA使能
                    }
       else if(DMAMode==2)
        {
        tplinkport->rLCTL= LDEN | LCHEN|DMACH_IRPT_MSK;
        }
        else
            {
            return false;
            }
        }

      else
        {
            return false;
        }
      return true;
}


/*-------使能LinkPort--------------------------------------------------**
//  功能：使能LinkPort
//  参数：Linkport编号，0/1/2分别表示Linkport0,Linkport1,Linkport0&Linkport1
//  返回：成功，返回true，否则返回false
**--------------------------------------------------------------------*/
bool_t  LinkPort_Enable(u32 LinkPortNum)
{
    u32 temp1;
   u32 temp2;
   temp1=*pLSTAT0_SHADOW;
   temp2=*pLSTAT1_SHADOW;
    if (LinkPortNum==0)
    {
        *pLCTL0 |= LEN ;
    }
    else if (LinkPortNum==1)
    {
        *pLCTL1 |= LEN ;
    }
    else if (LinkPortNum==2)
    {
        *pLCTL0 |= LEN ;
        *pLCTL1 |= LEN ;
    }
    else
    {
         return false;
    }
    return true;

}


/*------禁能LinkPort---------------------------------------------**
//  功能：禁能LinkPort
//  参数：Linkport编号，0/1/2分别表示Linkport0,Linkport1,Linkport0&Linkport1。
//  返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
bool_t  LinkPort_Disable(u32 LinkPortNum)
{
    if (LinkPortNum==0)
    {
        *pLCTL0 &= (~LEN) ;
    }
    else if (LinkPortNum==1)
    {
        *pLCTL1 &= (~LEN) ;
    }
    else if (LinkPortNum==2)
    {
        *pLCTL0 &= (~LEN) ;
        *pLCTL1 &= (~LEN) ;
    }
    else   //参数有误
    {
                return false;
    }
    return true;


}

/*--------使能LinkPortDMA，即启动DMA-----------------------------------------------**
//  功能：使能DMA，即启动DMA，通过DMA发送数据时，步骤为：首先使能LinkPort，然后配置
//        DMA传输参数，最后使能DMA，即可启动一次DMA传输。
//  参数：LinkPortNum,LinkPort编号，LinkPortNum=0，指使用LinkPort0；LinkPortNum=1指使用LinkPort1。
//  返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
bool_t  LinkPort_EnableDMA(u32 LinkPortNum)
{


    if (LinkPortNum==0)
    {
        *pLCTL0 |= LDEN ;
    }
    else if (LinkPortNum==1)
    {
        *pLCTL1 |= LDEN ;
    }
    else if (LinkPortNum==2)
    {
        *pLCTL0 |= LDEN ;
        *pLCTL1 |= LDEN ;
    }
    else//参数有误
    {
         return false;
    }
    return true;
}

/*-------禁能LinkPortDMA----------------------------------------**
//  功能：禁能LinkPortDMA
//  参数：Linkport编号，0/1/2分别表示Linkport0,Linkport1,Linkport0&Linkport1。
//  返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
bool_t LinkPort_DisableDMA(u32 LinkPortNum)
{
    if (LinkPortNum==0)
    {
        *pLCTL0 &= (~LDEN) ;
    }
    else if (LinkPortNum==1)
    {
        *pLCTL1 &= (~LDEN) ;
    }
    else if (LinkPortNum==2)
    {
        *pLCTL0 &= (~LDEN) ;
        *pLCTL1 &= (~LDEN) ;
    }
    else //参数有误
    {
           return false;
    }
    return true;
}

/*------LinkPort DMA传输参数配置-------------------------------------------**
//  功能：配置LinkPort DMA传输参数，包含三个参数，分别为IILB(DMA传输起始地址)、CLB(DMA传输长度)、
//        IMLB(DMA每次传输访问地址步进长度，默认都设为1)。
//  参数：LinkPortNum,LinkPort编号，LinkPortNum=0，指使用LinkPort0；LinkPortNum=1指使用LinkPort1。
//        *pData，被传输buffer的起始地址。
//        DataLen，传输长度，即字节数。
//  返回：成功，返回true，否则返回false。
**--------------------------------------------------------------------*/
bool_t LinkPort_InitDMA(u32 LinkPortNum,u32* pData,u32 DataLen)
{
    if (LinkPortNum==0)
    {
        *pIILB0=((int)pData)-DMA_OFFSET; //取地址低19位
        *pCLB0=DataLen;
        *pIMLB0=1;
    }
    else if (LinkPortNum==1)
    {
        *pIILB1=((int)pData)-DMA_OFFSET; //取地址低19位
        *pCLB1=DataLen;
        *pIMLB1=1;
    }
    else //参数有误
    {
           return false;
    }
    return true;

}



/*-------LinkPort发送一个Byte数据---------------------------------------**
//  功能：LinkPort发送一个Byte数据，不采用DMA方式，而是通过内核方式，先查询Transmit buffer状态，
//        若未满，即将要发送的数据写进Transmit regsiter(TXLB)。
//  参数：LinkPortNum,LinkPort编号，LinkPortNum=0，指使用LinkPort0；LinkPortNum=1指使用LinkPort1。
//          Val,要发送的数据。
//  返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
u32  LinkPort_SendData(u32 LinkPortNum,u32 Val)
{
    u32 Tmp=0;

    if (LinkPortNum==0)
    {
        if ( ((*pLSTAT0_SHADOW)&0x60)!=0x60 )  //发送缓冲未满
        {
            *pTXLB0=Val;
            Tmp=1;
        }
        else //发送缓冲已满
        {
             Tmp=0;
        }
    }
    else if (LinkPortNum==1)
    {
        if ( ((*pLSTAT1_SHADOW)&0x60)!=0x60 ) //发送缓冲未满
        {
            *pTXLB1=Val;
            Tmp=1;
        }
        else //发送缓冲已满
        {
           Tmp=0;
        }//不发送
    }
    else
    {
    Tmp=0;
    }

    return (Tmp);
}


/*-------LinkPort接收一个Byte数据--------------------------------------**
//  功能：LinkPort接收一个Byte数据，不采用DMA方式，而是通过内核方式，先查询receive buffer状态，
//        若非空，即从receiver register中读取一个数据。
//  参数：LinkPortNum,LinkPort编号，LinkPortNum=0，指使用LinkPort0；LinkPortNum=1指使用LinkPort1。
//  返回：读到的数据。

**--------------------------------------------------------------------*/
u32 LinkPort_RecvData(u32 LinkPortNum)
{
    u32 Val=0;

    if (LinkPortNum==0)
    {
        if ( ((*pLSTAT0_SHADOW)&0x60)!=0x0 )  //缓冲区非空
        {
            Val=*pRXLB0;
        }
        else
        {
        return false;
        } //不读取
    }
    else if (LinkPortNum==1)
    {
        if ( ((*pLSTAT1_SHADOW)&0x60)!=0x0 )  //缓冲区非空
        {
            Val=*pRXLB1;
        }
        else
        {
        return false;
        } //不读取
    }
    else
        {
        return false;
        } //不读取

    return (Val);
}


/*-----LinkPort0接收中断服务函数-----------------------------------------**
//  功能：LinkPort0中断服务函数。
//  参数：linkport0_int_line，中断号。
//  返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
u32 LinkPort0_RecvISR(ufast_t linkport0_int_line)
{
    volatile tag_LinkPortReg *tplinkport;
    int lstat = 0;

    tplinkport = pg_linkport0_reg;
    lstat = tplinkport->rLSTAT;      //读LSTAT寄存器，清中断标志位

    if( (lstat & DMACH_IRPT ) != 0)
    {
        LP0RecvFlag = true;


        LinkPort_DisableDMA(0);


    }
    return 1;
}



/*-------LinkPort0发送中断服务函数------------------------------------------**
//  功能：LinkPort0的中断服务函数
//  参数：linkport0_int_line，中断号。
//  返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
u32 LinkPort0_SendISR(ufast_t linkport0_int_line)
{
    volatile tag_LinkPortReg *tplinkport;
    int lstat = 0;

    tplinkport = pg_linkport0_reg;
    lstat = tplinkport->rLSTAT;      //读LSTAT寄存器，清中断标志位

    if( (lstat & EXTTXFR_DONE ) != 0)
    {
        //tplinkport->rLCTL = 0;
        LP0SendFlag = true;
        //LinkPort_Disable(0);
        LinkPort_DisableDMA(0);
    }
    return 1;

}



/*-----LinkPort1接收中断服务函数-----------------------------------------**
//  功能：LinkPort1中断服务函数。
//  参数：linkport1_int_line，中断号。
//  返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
u32  LinkPort1_RecvISR(ufast_t linkport1_int_line)
{
    volatile tag_LinkPortReg *tplinkport;
    int lstat = 0;

    tplinkport = pg_linkport1_reg;
    lstat = tplinkport->rLSTAT;

    if( (lstat & DMACH_IRPT ) != 0)
    {
        LP1RecvFlag = true;
        //LinkPort_Disable(1);
        LinkPort_DisableDMA(1);
    }
       return 1;
}



/*-------LinkPort1发送中断服务函数------------------------------------------**
//  功能：LinkPort1的中断服务函数
//  参数：linkport1_int_line，中断号。
//  返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
u32 LinkPort1_SendISR(ufast_t linkport1_int_line)
{
     volatile tag_LinkPortReg *tplinkport;
    int lstat = 0;

    tplinkport = pg_linkport1_reg;
    lstat = tplinkport->rLSTAT;

    if( (lstat & EXTTXFR_DONE ) != 0)
    {
        //tplinkport->rLCTL = 0;
        LP1SendFlag = true;
        //LinkPort_Disable(1);
        LinkPort_DisableDMA(1);
    }
    return 1;
}



//----初始化linkport模块中断----------------------------------------------------------
//功能：初始化linkport模块
//参数：linkport模块选择
//返回：成功，返回true，否则返回false。
//-----------------------------------------------------------------------------

bool_t LinkPort_InterruptInit(u32 LinkPortNum,u32 TransMode)
{
    //volatile tag_DMA_LinkPortReg *tpdmalinkport;

     if(LinkPortNum == 0)
    {
        //tpdmalinkport = pg_dma_linkport0_reg;
      if(TransMode==0)
         {
            Int_IsrConnect(cn_int_line_LP0I,LinkPort0_SendISR);
         }
        else if(TransMode==1)
        {
                Int_IsrConnect(cn_int_line_LP0I,LinkPort0_RecvISR);
        }
        else
            return false;
    }
    else if (LinkPortNum == 1)
    {
         //tpdmalinkport = pg_dma_linkport1_reg;
       //中断线初始化
    if(TransMode==0)
        {
             Int_IsrConnect(cn_int_line_LP1I,LinkPort1_SendISR);
        }

        else if(TransMode==1)
            {
             Int_IsrConnect(cn_int_line_LP1I,LinkPort1_RecvISR);
            }

        else
            return false;

    }
    else
    {
        return false;
    }

    return true;

}


/*-------Chained-DMA加载起始地址------------------------------------------------**
//  功能：设置Chained-DMA加载起始地址，将DMA-Chain第一个TCB的IILB的地址传递给CPLB寄存器，
          DSP即可自动将DMA-Chained所有TCB参数加载到DMA-Chained寄存器中。
//  输入：InitTCB_Addr,DMA-Chained TCB模块起始地址，也即第一个TCB中IILB寄存器地址
//  输出：空

**--------------------------------------------------------------------*/
void LinkPort_InitChainedDMA(int InitTCB_Addr)
{
    *pCPLB0=InitTCB_Addr;
}

/*------配置LinkPort DMA Chained TCB--------------------------------------**
    功能：配置DMA链TCB参数，每一个TCB由四个参数组成，分别是IILB(该DMA节点传输起始地址)、
          CLB(DMA传输长度)、IMLB(DMA每次传输访问地址步进长度，默认都设为1)、CPLB(下一个TCB传输地址)。
          前一个TCB的CPLB值与当前TCB的IILB值相同。最后一个TCB的CPLB等于0。
    参数：DMA链长度，即节点数。
    返回：成功，返回true，否则返回false。
**--------------------------------------------------------------------*/

bool_t  LinkPort_SetLinkPortChainedTCB(int Chained_Num)
{
      u32 i=0;
    if(Chained_Num<=Chained_DMA_Max_Num)
    {
         for(i=0;i<Chained_Num-1;i++)
        {
        LinkPort_ChainedDMA_TCB[i].CPLB=(int)(&LinkPort_ChainedDMA_TCB[i+1].IILB)&0x7FFFF;
    LinkPort_ChainedDMA_TCB[i].CLB=DSP_FPGA_BUF_LEN[i];
    LinkPort_ChainedDMA_TCB[i].IMLB=1;
    LinkPort_ChainedDMA_TCB[i].IILB=DSP_FPGA_BUF_ADDR[i];
        }

    LinkPort_ChainedDMA_TCB[Chained_Num-1].CPLB=0;
    LinkPort_ChainedDMA_TCB[Chained_Num-1].CLB=DSP_FPGA_BUF_LEN[i];
    LinkPort_ChainedDMA_TCB[Chained_Num-1].IMLB=1;
    LinkPort_ChainedDMA_TCB[Chained_Num-1].IILB=DSP_FPGA_BUF_ADDR[i];

    LinkPort_InitChainedDMA((int)(&LinkPort_ChainedDMA_TCB[0].IILB)&0x7FFFF );
     }
      else
        {
        return false;
        }

  return true;
}



/*-------添加DMA链节点---------------------------------------**
//  功能：添加一个DMA链节点，注意当前DMA链尚未传输，此函数用于DSP依次查询5个缓冲区，
          当缓冲区有数据要传输，即调用一次本函数，添加一个DMA链节点，当所有缓冲区
          检查完毕，即完成整个DMA链TCB设置。
    参数：addr,要传输数据buffer的起始地址;
          Len,传输的数据长度。
    返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/
bool_t SetDMAChainedTCB_Send(u32 addr,u32 Len)
{
           if(Chained_Send_Index<10)
            {
            LinkPort_ChainedDMA_Send_TCB[Chained_Send_Index].CPLB=(int)(&LinkPort_ChainedDMA_Send_TCB[Chained_Send_Index+1].IILB)&0x7FFFF;
    LinkPort_ChainedDMA_Send_TCB[Chained_Send_Index].CLB=Len;
    LinkPort_ChainedDMA_Send_TCB[Chained_Send_Index].IMLB=1;
    LinkPort_ChainedDMA_Send_TCB[Chained_Send_Index].IILB=addr;
    Chained_Send_Index++;
            }
     else
        {
        return false;
        }
     return true;
}

/*------启动DMA发送-----------------------------------**
    功能：本函数用于DSP向FPGA发送数据，当DMA链配置完成，启动DMA发送。
    参数：LinkPortNum,LinkPort编号，LinkPortNum=0，指使用LinkPort0；LinkPortNum=1指使用LinkPort1。
    返回：空

**--------------------------------------------------------------------*/
void LinkPort_EnableDMA_Send(u32 LinkPortNum)
{
      //LinkPort_Enable(LinkPortNum);
      LinkPort_ChainedDMA_Send_TCB[Chained_Send_Index-1].CPLB=0;
      LinkPort_InitChainedDMA((int)(&LinkPort_ChainedDMA_Send_TCB[0].IILB)&0x7FFFF );
      LinkPort_EnableDMA(LinkPortNum);

}

/*-------启动LinkPort DMA接收------------------------------------------**
    功能：启动DMA接收
    参数：LinkPortNum,LinkPort编号，LinkPortNum=0，指使用LinkPort0；LinkPortNum=1指使用LinkPort1。
    返回：成功，返回true，否则返回false。

**--------------------------------------------------------------------*/

void EnableLinkPortDMA_Recv(u32 LinkPortNum)
{
      LinkPort_EnableDMA(LinkPortNum);

}



//----设置数据池长度-------------------------------------------------------------------
//功能：设置某个指定数据区域长度
//参数：数据区域编号，数据区域长度
//返回：成功返回true,否则返回false
//----------------------------------------------------------------------------------

bool_t LinkPort_SetRecvDataPoolLen(u8 Data_Pool_Num, u32 Len)
{

    switch(Data_Pool_Num)
    {
        case 1:
                  DSP_FPGA_BUF_LEN[0]=Len;
                      break;
        case 2:
                 DSP_FPGA_BUF_LEN[1]=Len;
               break;
                case 3:
                 DSP_FPGA_BUF_LEN[2]=Len;
               break;
                case 4:
                 DSP_FPGA_BUF_LEN[3]=Len;
               break;
                case 5:
                 DSP_FPGA_BUF_LEN[4]=Len;
               break;

                        case 6:
                 DSP_FPGA_BUF_LEN[5]=Len;
               break;
                case 7:
                 DSP_FPGA_BUF_LEN[6]=Len;
               break;
                case 8:
                 DSP_FPGA_BUF_LEN[7]=Len;
               break;
                case 9:
                 DSP_FPGA_BUF_LEN[8]=Len;
               break;

                        case 10:
                 DSP_FPGA_BUF_LEN[9]=Len;
               break;

                 default:
             return false;


    }
    return true;
}

//----设置数据池起始地址-------------------------------------------------------------------
//功能：设置某个数据区域起始地址。
//参数：数据区域编号，数据区域起始地址。
//返回：成功返回true,否则返回false。
//----------------------------------------------------------------------------------

bool_t LinkPort_SetRecvDataPoolAddr(u8 Data_Pool_Num,u32 addr)
{
        switch(Data_Pool_Num)
    {
        case 1:
                  DSP_FPGA_BUF_ADDR[0]=addr;
                      break;
        case 2:
                 DSP_FPGA_BUF_ADDR[1]=addr;
               break;
                case 3:
                 DSP_FPGA_BUF_ADDR[2]=addr;
               break;
                case 4:
                 DSP_FPGA_BUF_ADDR[3]=addr;
               break;
                case 5:
                 DSP_FPGA_BUF_ADDR[4]=addr;
               break;

                case 6:
                  DSP_FPGA_BUF_ADDR[5]=addr;
                      break;
        case 7:
                 DSP_FPGA_BUF_ADDR[6]=addr;
               break;
                case 8:
                 DSP_FPGA_BUF_ADDR[7]=addr;
               break;
                case 9:
                 DSP_FPGA_BUF_ADDR[8]=addr;
               break;
                case 10:
                 DSP_FPGA_BUF_ADDR[9]=addr;
               break;

                 default:
             return false;

    }
    return true;
}


//----LinkPort默认初始化设置-------------------------------------------------------------------
//功能：此函数为调试阶段使用。
//参数：空
//返回：空
//----------------------------------------------------------------------------------


void  LinkPort_DefaultIntInit(void)
{
       LinkPort_InterruptInit(0, 1);      //  LinkPort0   Recv
       LinkPort_InterruptInit(1, 0);      //  LinkPort1   Send
       LinkPort_Init(0, 1, 2) ;            //LinkPort0 Recv  Chained-DMA
       LinkPort_Init(1, 0, 1);        //LinkPort1 Send DMA
}













