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
//所属模块: CAN SJA1000驱动模块
//作者:  zhb.
//版本：V1.0.0
//文件描述: SJA1000底层驱动，适用于我司SR1210板
//时间:2015.06.09
//------------------------------------------------------
/*中断  CAN1---外部中断8 对应CPU引脚G0
        CAN2--外部中断9 对应CPU引脚G1
        CAN3---外部中断10 对应CPU引脚G2
        CAN4--外部中断11对应CPU引脚G3*/
#include "SJA1000Can.h"
#include "2410addr.h"
#include "os.h"
#include "msgqueue.h"
#include "stdlib.h"
#include "stdio.h"
#include "int.h"
#include "string.h"
#include "lock.h"
#include "stdint.h"
#include "int_hard.h"
#include "ring.h"
#include "shell.h"
#include "AT24C128.h"
#include "cpu_peri.h"

#define CN_U32_MAX           0x7FFFFFFF
#define CN_MAX_PKG_NUM_PERI  4000
#define CN_CAN_BUFSIZE (52000)

#define CN_CAN_CONFIG_LEN                       0xA
#define CN_E2PROM_CAN_START_ADDR                0x00
#define CN_E2PROM_CAN_ADDR1_ADDR                0x04

#define CN_E2PROM_CAN_ADDR2_ADDR                0x06
#define CN_E2PROM_CAN_ADDR3_ADDR                0x08
#define CN_CAN_PARA_SUCCESS_FLAG                0xAA      //参数成功配置标识符

#define CN_CAN_ERROR_MAX                        0xA
//CAN配置信息在E2PROM中依次为：Byte1~Byte2 波特率;Byte3:标示是否配置了地址过滤
//Byte4:配置的地址,Byte5:在E2PROM中是否配置了波特率标志位，若为0xAA则表示已配置，
//此时CAN芯片采用E2PROM中值配置芯片，若不为0xAA，则表示未配置，此时将采用默认值配置
//CAN0 波特率250;CAN1 波特率250;CAN2波特率125;CAN3波特率125.

CAN_DevCtrl *CAN_DevCtrlPtr=NULL;
static u8 gs_CANAddrNum=0;
static struct  tagSemaphoreLCB *g_ptCanSemp;
static u8 CAN_RecvStack[0xA000];
static u8 CAN_MonitorStack[0x1000];
static u8 recvbuf[650];
static u32 gs_CANDefaultBaudRate[CN_CAN_CHIP_NUM]={250,250,125,125};
static u32 gs_CANCurBaudRate[CN_CAN_CHIP_NUM]={0,0,0,0};

static u32 ALI_Count=0;
static u8  ALC_Current=0;
static u32 BEI_Count=0;
static u8  BusErrorType=0;
static u32 BusError_BitCount=0;
static u32 BusError_FormCount=0;
static u32 BusError_StuffCount=0;
static u32 BusError_OtherCount=0;
static u32 BusError_Tx=0;
static u32 BusError_Rx=0;
static u8  BusError_Current=0;
static u8  ErrStatusCurrent=0xff;
static u32 BusOff_Count=0;

#define CN_CAN_RECORD_ADDR    0x100
#define CN_CAN_RECORD_LEN     13
#define CN_CAN_RECORD_NUM     1240
static u32 RecordConut=0;
static u8 Hard_Can_Message_Read(u8 byChipNum);
static u32 ISR_CAN( ptu32_t );
ptu32_t CAN_Shell_Module_Install(void);
bool_t Sh_CAN_SetBaudRate(char *param);
bool_t Sh_CAN_SetAddr(char *param);
bool_t  Sh_CAN_GetBaudRate(char *param);
bool_t Sh_CAN_GetAddr(char *param);

bool_t Sh_CAN_GetCANInfor(char *param);
extern int Can2Eth(int interface, u8 *buf, int len);

struct ShellCmdTab const shell_cmd_can_table[]=
{
		{
			"set-baudrate",
			Sh_CAN_SetBaudRate,
			"设置CAN通信波特率",
			"COMMAND:set-baudrate+enter"
		},
		{
			"set-addr",
			Sh_CAN_SetAddr,
			"设置CAN通信地址",
			"COMMAND:set-addr+enter"
		},
		{
				"get-baudrate",
				Sh_CAN_GetBaudRate,
				"获取指定CAN通信波特率",
				"COMMAND:get-baudrate+enter"
		},
		{
				"get-addr",
				Sh_CAN_GetAddr,
				"获取指定CAN通信地址",
				"COMMAND:get-addr+enter"
		},
		{
				"get-infor",
				Sh_CAN_GetCANInfor,
				"获取CAN0通道寄存器值",
				"COMMAND:get-infor+enter"
		},
};

#define CN_CAN_SHELL_NUM  sizeof(shell_cmd_can_table)/sizeof(struct ShellCmdTab)
static struct ShellCmdRsc tg_can_shell_cmd_rsc[CN_CAN_SHELL_NUM];

/*******************************************************************************
功能:CAN控制块内存申请初始化。
参数:空。
返回:true--初始化成功，false--初始化失败。
******************************************************************************/

bool_t CAN_MemInit()
{
    CAN_RxTag* can_Rxtag=NULL;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    u32 i;
    struct RingBuf *RingBuf;
    CAN_DevCtrlPtr=(CAN_DevCtrl *)malloc(sizeof(CAN_DevCtrl)*CN_CAN_CHIP_NUM);
    if(!CAN_DevCtrlPtr)
        return false;
    for(i=0;i<CN_CAN_CHIP_NUM;i++)
    {
       CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
       can_Rxtag =(CAN_RxTag* )malloc(sizeof(CAN_RxTag));
       if(!can_Rxtag)
             goto ERRORDeal;
	   RingBuf=(struct tagRingBuf *)malloc(sizeof(struct RingBuf));
	   if(!RingBuf)
	   {
		goto ERRORDeal;
	   }
	   RingBuf->buf=(u8 *)malloc(CN_CAN_BUFSIZE);
	   if(!RingBuf->buf)
	   {
		goto ERRORDeal;
	   }
	    can_Rxtag->rxBuf=RingBuf;
        CAN_DevCtrlTempptr->can_RxTag=can_Rxtag;
    }
    return true;
ERRORDeal:

    for(i=0;i<CN_CAN_CHIP_NUM;i++)
    {
        CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
        if(CAN_DevCtrlTempptr->can_RxTag)
        {
            if(CAN_DevCtrlTempptr->can_RxTag->rxBuf)
                free(CAN_DevCtrlTempptr->can_RxTag->rxBuf);
            free((CAN_DevCtrlTempptr->can_RxTag));
        }
    }
    free(CAN_DevCtrlPtr);
    return false;

}

/*******************************************************************************
功能:CAN设备中断控制器中断初始化。
参数:无。
返回:无。
备注：四个SJA1000的中断输出依次接到S3C2410的EXT8~EXT11上，这里将EXT8~EXT11设置为下降
沿触发。
*********************************************************************************/
 void InitCANExtInt()
{
    u32 tempp;
    // 设置中断类型，下降沿触发
    SNGS3C_REG_READ(rEXTINT1,tempp);  /* EXT8-11 TO 下降沿中断触发*/
    tempp = tempp & 0xFFFF0000;
    tempp |= 0x2222;
    SNGS3C_REG_WRITE(rEXTINT1,tempp);

    // 设置端口为中断模式
    SNGS3C_REG_READ(rGPGCON,tempp);  /* PG0-3 TO INT8-11 */
    tempp = ( tempp & 0xFFFFFF00) | 0x000000AA;
    SNGS3C_REG_WRITE(rGPGCON,tempp);

    // 设置端口为中断掩码
    SNGS3C_REG_READ(rEINTMASK,tempp);  /* PG0-1 TO INT8-11 */
    tempp = ( tempp & 0xFFFFF0FF) ;
    SNGS3C_REG_WRITE(rEINTMASK,tempp);

    Int_SetClearType(CN_INT_LINE_EINT8_23,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(CN_INT_LINE_EINT8_23,ISR_CAN);
    Int_SettoAsynSignal(CN_INT_LINE_EINT8_23);
    Int_ClearLine(CN_INT_LINE_EINT8_23);
    Int_RestoreAsynLine(CN_INT_LINE_EINT8_23);
}

 /*******************************************************************************
 功能:设置CAN通信波特率。
 参数:para:参数字符串，需要两个参数，用空格隔开
      byChipNum--CAN控制器编号，0~3；
      baudRate--CAN通信波特率，可设波特率有5、10、20、40、50、80、100、125、200、250、
      400、500、666、800、1000.
 输出:true--设置成功，false-设置失败。
 *********************************************************************************/
bool_t Sh_CAN_SetBaudRate(char *param)
{
     char *word_ChipNum,*word_BaudRate,*word_trail,*next_param;
     u8 byChipNum;
     u32 BaudRate;
     u32 BaudRate1;
     u8  byBuf[4];
     u32 result;
	 //提取2个参数
	 word_ChipNum = Sh_GetWord(param,&next_param);
	 word_BaudRate = Sh_GetWord(next_param,&next_param);
	 word_trail = Sh_GetWord(next_param,&next_param);
	 if((word_ChipNum == NULL)||(word_BaudRate == NULL)||(word_trail != NULL))
	 {
		printk("\r\n格式错误，正确格式是：\r\n>d CAN控制器编号 波特率 \r\n");
		return false;
	 }
	 byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	 BaudRate = strtol(word_BaudRate, (char **)NULL, 0);

	 if(byChipNum>=CN_CAN_CHIP_NUM)
	 {
		printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	 }

	 switch(BaudRate)
	 {
	 case 5:
		 BaudRate1=5;
	   break;
	 case 10:
		 BaudRate1=10;
	  break;
	 case 20:
		 BaudRate1=20;
	   break;
	 case 40:
		 BaudRate1=40;
	   break;
	 case 50:
		 BaudRate1=50;
	   break;
	 case 80:
		 BaudRate1=80;
	   break;
	 case 100:
		 BaudRate1=100;
		break;
	 case 125:
		 BaudRate1=125;
		break;
	 case 200:
		 BaudRate1=200;
		break;
	 case 250:
		 BaudRate1=250;
		break;
	 case 400:
		 BaudRate1=400;
		break;
	 case 500:
		 BaudRate1=500;
		break;
	 case 666:
		 BaudRate1=666;
		break;
	 case 800:
		 BaudRate1=800;
		break;
	 case 1000:
		 BaudRate1=1000;
		break;
	 default:
		 printk("The CAN NUM %d :BaudRate is invalid,please set again\r\n",byChipNum);
		 return false;
	 }
	 byBuf[0]=byChipNum;
	 //先写低8位，后写高8位
     byBuf[1]=BaudRate1;
     byBuf[2]=BaudRate1>>8;
     byBuf[3]=CN_CAN_PARA_SUCCESS_FLAG;
     result=AT24_WriteBytes(CN_E2PROM_CAN_START_ADDR+byChipNum*CN_CAN_CONFIG_LEN,byBuf,4);
	if(!result)
	{
		printk("The BaudRate set failed!Please set again!\r\n");
		//波特率设置成功标志位
		return false;
	}
	 printk("The CAN NUM %d BaudRate set success.\r\n",byChipNum);
	  return true;
}

/*******************************************************************************
功能:获取CAN通信波特率。
参数:para:参数字符串，需要1个参数
     byChipNum--CAN控制器编号，0~3
输出:true--获取成功，false-获取失败。
*********************************************************************************/
bool_t  Sh_CAN_GetBaudRate(char *param)
{
	 char *word_ChipNum,*word_trail,*next_param;
	 u8 byChipNum;
	 u32 result;
	 u8 byBuf[4];
	 u32 temp,baudrate=0x0000;
	//提取1个参数
	word_ChipNum = Sh_GetWord(param,&next_param);
    word_trail = Sh_GetWord(next_param,&next_param);
	if((word_ChipNum == NULL)||(word_trail != NULL))
	{
	  printk("\r\n格式错误，正确格式是：\r\n>d CAN控制器编号. \r\n");
	  return false;
	}
	byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	if(byChipNum>=CN_CAN_CHIP_NUM)
	{
	    printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	}
	 result=AT24_ReadBytes(CN_E2PROM_CAN_START_ADDR+byChipNum*CN_CAN_CONFIG_LEN, byBuf,4);
	 if(!result)
	 {
       printk("Get the BaudRate failed!\r\n");
       return false;
	 }
	if(byChipNum!=byBuf[0]) //首先判断CAN编号是否正确，如果这都不正确是否pass吧
	{
		printk("The CAN NUM don't match.\r\n");
		return false;
	}
	else if(CN_CAN_PARA_SUCCESS_FLAG!=byBuf[3])
	{
		printk("The Success Config flag is error.\r\n");
		return false;
	}
	 else
	 {
		if(byBuf[2]==0)
        {
		    baudrate=byBuf[1];
		    printf("The CAN NUM %d:The BaudRate is %d.\r\n",byChipNum,baudrate);
            return true;
        }
        else
        {
		    temp=byBuf[2];
        	baudrate|=(temp<<8);
		    temp=byBuf[1];
        	baudrate|=temp;
		    printf("The CAN NUM %d:The BaudRate is %d.\r\n",byChipNum,baudrate);
        	return true;
        }
	 }
}

/*******************************************************************************
 功能:设置CAN地址过滤功能，设置地址过滤后CAN控制器只接收目标地址为本地址的CAN帧。
 参数: para:参数字符串，需要三个参数，用空格隔开
      byChipNum--CAN控制器编号，0~3；
      Addr1--CAN地址1，范围：0~0xFE；如果地址设为0xFF，则表示不需要地址过滤，接收所有CAN总线的包。
 输出:true--设置成功，false-设置失败。
 说明：针对目前应用需求，只实现扩展帧格式的单滤波过滤.
*********************************************************************************/
bool_t Sh_CAN_SetAddr(char *param)
{
    char *word_ChipNum,*word_Addr,*word_trail,*next_param;
    u8 byChipNum;
    u32 Addr,result;
    u8 byBuf[2];
    u16 wAddr;
   //提取2个参数
    word_ChipNum = Sh_GetWord(param,&next_param);
    word_Addr = Sh_GetWord(next_param,&next_param);
    word_trail = Sh_GetWord(next_param,&next_param);
    if(word_ChipNum==NULL||word_Addr==NULL||word_trail!=NULL)
    {
    	printk("\r\n格式错误，正确格式是：\r\n>d CAN控制器编号 地址.\r\n");
    	return false;
    }
	byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	if(byChipNum>=CN_CAN_CHIP_NUM)
	{
		printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	}
	Addr = strtol(word_Addr, (char **)NULL, 16);
	if(Addr>0xFF)
	{
		printk("The CAN addr is invalid,the addr must be 0x00~0xFF");
		return false;
	}
	byBuf[0]=Addr;
	byBuf[1]=CN_CAN_PARA_SUCCESS_FLAG;
    switch(gs_CANAddrNum)
    {
    case 0:
    	wAddr=CN_E2PROM_CAN_ADDR1_ADDR;
    	break;
    case 1:
    	wAddr=CN_E2PROM_CAN_ADDR2_ADDR;
    	break;
    case 2:
    	wAddr=CN_E2PROM_CAN_ADDR3_ADDR;
    	break;
    default:
    	printk("The Addr Num Supported exceed.\r\n");
    	break;
    }
    result=AT24_WriteBytes(wAddr+byChipNum*CN_CAN_CONFIG_LEN, byBuf, 2);
    if(!result)
    {
    	printk("The addr set failed!Please set again!\r\n");
    	return false;
    }
    else
    {
    	gs_CANAddrNum++;
    	printk("The CAN NUM %d addr set success!\r\n",byChipNum);
    	return true;
    }
}

/*******************************************************************************
功能:获取CAN通信地址。
参数:para:参数字符串，需要1个参数
     byChipNum--CAN控制器编号，0~3
输出:true--获取成功，false-获取失败。
*********************************************************************************/
bool_t  Sh_CAN_GetAddr(char *param)
{
	char *word_ChipNum,*word_trail,*next_param;
	u8 byChipNum;
	u32 result;
	u8 byBuf[6];
	u8 Addr[3]={0,0,0};
	//提取1个参数
	word_ChipNum = Sh_GetWord(param,&next_param);
    word_trail = Sh_GetWord(next_param,&next_param);
	if((word_ChipNum == NULL)||(word_trail != NULL))
	{
	  printk("\r\n格式错误，正确格式是：\r\n>d CAN控制器编号.\r\n");
	  return false;
	}
	byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	if(byChipNum>=CN_CAN_CHIP_NUM)
	{
	    printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	}
	 result=AT24_ReadBytes(CN_E2PROM_CAN_ADDR1_ADDR+byChipNum*CN_CAN_CONFIG_LEN, byBuf,6);
	 if(!result)
	 {
       printk("Get the addr failed!\r\n");
       return false;
	 }
	 if(byBuf[1]!=CN_CAN_PARA_SUCCESS_FLAG)
	 {
		 printk("The CAN has not set the addr.\r\n");
		 return false;
	 }
	 else
	 {
       Addr[0]=byBuf[0];
       if(byBuf[3]!=CN_CAN_PARA_SUCCESS_FLAG)
        {
    	   printk("The CAN Num %d:The Addr is %d.\r\n",byChipNum,Addr[0]);
    	   return true;
        }
       else
       {
    	   Addr[1]=byBuf[2];
    	   if(byBuf[5]!=CN_CAN_PARA_SUCCESS_FLAG)
    	   {
    		   printk("The CAN Num %d:The Addr is %d and %d.\r\n",byChipNum,Addr[0],Addr[1]);
              return true;
            }
           else
           {
    		   Addr[2]=byBuf[4];
    		   printk("The CAN Num %d:The Addr is %d , %d and %d.\r\n",byChipNum,Addr[0],Addr[1],Addr[2]);
    		   return true;
            }
	    }
	 }
}


/*******************************************************************************
功能:获取CAN通信地址。
参数:para:参数字符串，需要1个参数
     byChipNum--CAN控制器编号，0~3
输出:true--获取成功，false-获取失败。
*********************************************************************************/
bool_t  Sh_CAN_GetCANInfor(char *param)
{
	 char *word_ChipNum,*word_trail,*next_param;
	 u8 byChipNum;
	 u32 result;
	 u8 byBuf[260];
     u8 i;
	 word_ChipNum = Sh_GetWord(param,&next_param);
     word_trail = Sh_GetWord(next_param,&next_param);
	if((word_ChipNum == NULL)||(word_trail != NULL))
	{
	  printk("\r\n格式错误，正确格式是：\r\n>d CAN控制器编号. \r\n");
	  return false;
	}
	byChipNum = strtol(word_ChipNum, (char **)NULL, 0);
	if(byChipNum>=CN_CAN_CHIP_NUM)
	{
	    printk("The Chip Num must be one of 0/1/2/3!\r\n");
		return false;
	}
    if(RecordConut>=19)
    {
       result=AT24_ReadBytes(CN_CAN_RECORD_ADDR+(RecordConut-19)*CN_CAN_RECORD_LEN, byBuf,260);
    }
    else
    {
    	i=20-RecordConut-1;
    	result=AT24_ReadBytes(CN_CAN_RECORD_ADDR+0x4000-i*CN_CAN_RECORD_LEN, byBuf,i*CN_CAN_RECORD_LEN);
    	result=AT24_ReadBytes(CN_CAN_RECORD_ADDR, &byBuf[CN_CAN_RECORD_LEN*i],RecordConut*CN_CAN_RECORD_LEN);
    }
    if(result)
    {
      for(i=0;i<20;i++)
      {
    	  printf("Mode:%x,CMR:%x,SR:%x,IIR:%x,IER:%x,D0:%x,D1:%x,D2:%x,D3:%x,D4:%x,D5:%x,D6:%x,D7:%x,\r\n",
    	    byBuf[13*i+0],byBuf[13*i+1],byBuf[13*i+2],byBuf[13*i+3],byBuf[13*i+4],byBuf[13*i+5],byBuf[13*i+6],
			byBuf[13*i+7],byBuf[13*i+8],byBuf[13*i+9],byBuf[13*i+10],byBuf[13*i+11],byBuf[13*i+12]);
      }
    }
    else
    {
       printf("Read fram failed.\r\n");
    }
    return true;
}
/*******************************************************************************
功能:CAN设备的初始化函数，初始化中断控制器，申请内存，初始化器件。
参数:byChipNum--CAN控制器编号，0~3；
     addr--CAN控制器地址；
     baudRate--CAN通信波特率。
输出:true--初始化成功，false-初始化失败。
*********************************************************************************/
bool_t Hard_Can_Init( u8 byChipNum,u32 baudRate)
{
    u8    byBTR0, byBTR1,byTemp;
    u32 i;
    volatile u8*   pAddr;
	static u8 flag=0;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;

    if(byChipNum>=CN_CAN_CHIP_NUM)
        return false;

    if(!flag)
    {  
        if(CAN_MemInit()==false)/*内存申请，只运行一次*/
        {
        	printf("CAN Control Block Malloc Failed!\r\n");
        	return false;
        } 
        InitCANExtInt();/*中断配置，只运行一次*/
        flag++;
    }
    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChipNum;
    CAN_DevCtrlTempptr->BaudRate=baudRate;
    CAN_DevCtrlTempptr->RecvFlag=false;
    switch(byChipNum)
    {
        case 0:
            pAddr = (u8*)(CN_ADDR_CAN1_BASE);
            CAN_DevCtrlTempptr->CANBaseAddr=CN_ADDR_CAN1_BASE;
            break;
        case 1:
            pAddr = (u8*)(CN_ADDR_CAN2_BASE);
            CAN_DevCtrlTempptr->CANBaseAddr=CN_ADDR_CAN2_BASE;
            break;
        case 2:
            pAddr = (u8*)(CN_ADDR_CAN3_BASE);
            CAN_DevCtrlTempptr->CANBaseAddr=CN_ADDR_CAN3_BASE;
            break;
        case 3:
            pAddr = (u8*)(CN_ADDR_CAN4_BASE);
            CAN_DevCtrlTempptr->CANBaseAddr=CN_ADDR_CAN4_BASE;
            break;
        default:
          break;
    }

    switch(baudRate )
    {
        case 5:
            byBTR0 = 0xBF;
            byBTR1 = 0xFF;
            break;      //    5k
        case 10:
            byBTR0 = 0x31;
            byBTR1 = 0x1C;
            break;      //   10k
        case 20:
            byBTR0 = 0x18;
            byBTR1 = 0x1C;
            break;      //   20k
        case 40:
            byBTR0 = 0x87;
            byBTR1 = 0XFF;
            break;      //   40k
        case 50:
            byBTR0 = 0x09;
            byBTR1 = 0X1c;
            break;      //   50k
        case 80:
            byBTR0 = 0x83;
            byBTR1 = 0XFF;
            break;      //  80k
        case 100:
            byBTR0 = 0x04;
            byBTR1 = 0x1C;
            break;      //  100k
        case 125:
            byBTR0 = 0x03;
            byBTR1 = 0x1C;
            break;      //  125k
        case 200:
            byBTR0 = 0x81;
            byBTR1 = 0xFA;
            break;      // 200k
        case 250:
            byBTR0 = 0x01;
            byBTR1 = 0X1C;
            break;      //  250k
        case 400:
            byBTR0 = 0x80;
            byBTR1 = 0XFA;
            break;      //   400k
        case 500:
            byBTR0 = 0x00;
            byBTR1 = 0X1C;
            break;      //  500k
        case 666:
            byBTR0 = 0x80;
            byBTR1 = 0xB6;
            break;      //  666k
        case 800:
            byBTR0 = 0x00;
            byBTR1 = 0x16;
            break;      //  800k
        case 1000:
            byBTR0 = 0x00;
            byBTR1 = 0x14;
            break;      // 1000k
        default:
            byBTR0 = 0x01;
            byBTR1 = 0x1C;
            break;      //   250k
    }
	byTemp = *( pAddr + CN_PCAN_SR );       // Clear Status Register
	byTemp = *( pAddr + CN_PCAN_IIR );      // Clear Interrupt Identify Register
	*( pAddr + CN_PCAN_MODE ) = 0x01;     // Reset mode
	*( pAddr + CN_PCAN_CDR )  = 0x82;     // PeliCAN mode & Clock driver:1000-0101   todo
	*( pAddr + CN_PCAN_CMR )  = 0x0E;     // Command Register
	*( pAddr + CN_PCAN_IER )  = 0x00;     // Interruption disabled
	*( pAddr + CN_PCAN_BTR0 ) = byBTR0;   // Bus timeing register 0
	*( pAddr + CN_PCAN_BTR1 ) = byBTR1;   // Bus timeing register 1
	*( pAddr + CN_PCAN_OCR )  = 0xDA;     // Output Control:110-110-10
	*( pAddr + CN_PCAN_RXERR )= 0x00;     // RX Error counter
	*( pAddr + CN_PCAN_TXERR )= 0x00;     // TX Error counter
	*( pAddr + CN_PCAN_RBSA ) = 0x00;     // RX Buffer Start Address

	//接BMS,该CAN总线地址为0x56(0xE5)
	if(byChipNum<=1)
	{
		*( pAddr + CN_PCAN_ACR0 ) = 0x00;      //
		*( pAddr + CN_PCAN_ACR1 ) = 0x02;     //
		*( pAddr + CN_PCAN_ACR2 ) = 0xB0;     //
		*( pAddr + CN_PCAN_ACR3 ) = 0x00;     //
		*( pAddr + CN_PCAN_AMR0 ) = 0xFF;     //
		*( pAddr + CN_PCAN_AMR1 ) = 0xF8;     //
		*( pAddr + CN_PCAN_AMR2 ) = 0x07;     //
		*( pAddr + CN_PCAN_AMR3 ) = 0xFF;     //
	}
	//地址不过滤
	else
	{
		*( pAddr + CN_PCAN_ACR0 ) = 0x00;      //
		*( pAddr + CN_PCAN_ACR1 ) = 0x00;     //
		*( pAddr + CN_PCAN_ACR2 ) = 0x00;     //
		*( pAddr + CN_PCAN_ACR3 ) = 0x00;     //
		*( pAddr + CN_PCAN_AMR0 ) = 0xFF;     //
		*( pAddr + CN_PCAN_AMR1 ) = 0xFF;     //
		*( pAddr + CN_PCAN_AMR2 ) = 0xFF;     //
		*( pAddr + CN_PCAN_AMR3 ) = 0xFF;     //
	}

//		*( pAddr + CN_PCAN_ACR0 ) = 0x00;      //
//		*( pAddr + CN_PCAN_ACR1 ) = 0x00;     //
//		*( pAddr + CN_PCAN_ACR2 ) = 0x00;     //
//		*( pAddr + CN_PCAN_ACR3 ) = 0x00;     //
//		*( pAddr + CN_PCAN_AMR0 ) = 0xFF;     //
//		*( pAddr + CN_PCAN_AMR1 ) = 0xFF;     //
//		*( pAddr + CN_PCAN_AMR2 ) = 0xFF;     //
//		*( pAddr + CN_PCAN_AMR3 ) = 0xFF;     //

	*( pAddr + CN_PCAN_IER )  = 0x7D;  //Interrupt enabled(接收采用中断模式，发送不使用中断)
	*( pAddr + CN_PCAN_MODE ) = 0x00;     // Normal Operating mode
	while(*( pAddr + CN_PCAN_MODE )&0x01)
	{
		i++;
		if(i>100000)
		{
			break;
		}
	}

    return true;
}

/*******************************************************************************
功能:CAN设备中断处理，该函数被ISR_CAN调用
参数:无。
返回值:无。
备注：关于SJA1000 PeriCAN模式下中断类型说明：
   总线错误中断；
当CAN控制器检测到总线错误且中断使能寄存器（EIR）中的BEIE被置位时此位被置位，当前的错误
代码可以通过错误代码捕捉寄存器(ECC)获得；其中可以分为：位错误、格式错误、填充错误、其它错误；
   仲裁丢失中断；
当CAN控制器丢失仲裁,变为接收器和中断使能寄存器的ALIE为被置位时,此位被置位当前仲裁丢失
的位置可以通过读仲裁丢失捕捉寄存器(ALC)获得；
   错误消极中断；
当CAN控制器到达错误消极状态（Error Passive）(至少一个错误计数器超过协议规定的值127)或
从错误消极状态（Error Passive）又进入错误活动状态(Error Active)以及中断寄存器的EPIE位被
置位时此位被置1；
   错误报警中断；
错误状态位（状态寄存器的SR.6位 ES）和总线状态位（状态寄存器的SR.7位 BS）的改变和中断寄存器
的EIE位被置位时此位被置1影响错误状态位的有：
1，错误计数器至少有一个错误计数器满
2，超过错误报警限制寄存器(EWLR)设置的值时错误状态位被置位；报警中断。EWLR 硬
件复位后的默认值是96
3，当发送错误计数器超过限制255 总线状态位被置为1 总线关闭
4，进入总线关闭后，再次进入错误主动状态（Error Active）时， 也会产生中断；
*********************************************************************************/
static u8 gs_SR=0x00;
void ISR_Deal( u8 byIndex )
{
    u8    byIIR,byTemp;
    volatile u8*    pAddr;
    volatile u8 RxErrCount=0;
    volatile u8 TxErrCount=0;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    if( byIndex >=CN_CAN_CHIP_NUM )
        return ;
    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byIndex;
    if(!CAN_DevCtrlTempptr)
        return;
    pAddr = (u8*)(CAN_DevCtrlTempptr->CANBaseAddr);
    //暂时屏蔽中断，目的是因为在读IIR寄存器清中断的过程中，如果SJA1000再次触发中断，此时由于
    //两次中断时间间隔过短，CPU无法有效采集到，从而导致在CPU无法响应第二个中断，而SJA1000的第
    //二个中断会将SJA1000的INT引脚拉低，导致SJA1000引脚一直处于低电平状态，而CPU端设置成了下降沿
    //触发，此时会导致CPU一直无法响应SJA1000的中断。
    byIIR = *(pAddr+CN_PCAN_IIR);
    gs_SR=*(pAddr+CN_PCAN_SR);
    *(pAddr+CN_PCAN_IER)=0x00;

    //处理SJA1000的8个中断，由于在SJA1000的初始化过程中只使能了7个中断
    //(发送中断没有使能)，下面分别针对7种中断进行相应处理。
    //仲裁丢失中断 ，SJA1000仲裁丢失，SJA1000会自动退出发送，改为接收，等接收完会自动发送。
    if(byIIR&ALI_Bit)
    {
    	ALI_Count++;                      //仲裁丢失计数器累加
    	byTemp=*(pAddr + CN_PCAN_ALC);    //读仲裁丢失寄存器
    	ALC_Current=byTemp&0x1F;          //获得当前仲裁丢失的位置
    	if(ALI_Count>=CN_U32_MAX)
    	{
    		ALI_Count=0;
    	}
    }
    //总线错误中断
    if(byIIR&BEI_Bit)
    {
    	BEI_Count++;                      //总线错误计数器累加
    	byTemp=*(pAddr + CN_PCAN_ECC);    //读仲裁丢失寄存器
        //判断出错方向，发送or接收
    	if(byTemp&0x20)
    	{
    		BusError_Rx++;
    		if(BusError_Rx>=CN_U32_MAX)
    		{
    			BusError_Rx=0;
    		}
    	}
    	else
    	{
    		BusError_Tx++;
    		if(BusError_Tx>=CN_U32_MAX)
			{
				BusError_Tx=0;
			}
    	}
    	//判断出错类型：位错误、格式错误、填充错误、其它错误。
    	BusErrorType=byTemp&0xC0;
    	switch(BusErrorType)
    	{
    	   case 0x00:
    	    	BusError_BitCount++;       //位错误计数器累加
    	    	break;
    	   case 0x40:
    	    	BusError_FormCount++;      //格式出错计数器累加
    	    	break;
    	   case 0x80:
    	    	BusError_StuffCount++;     //填充位计数器累加
    	    	break;
    	   case 0xc0:
    	    	BusError_OtherCount++;     //其他计数器累加
    	    	break;
    	   default:
    	    	break;
    	}
    	BusError_Current=byTemp&0x1F;     //出错的位置，指CAN帧具体的bit位
        if((BusError_BitCount>=CN_CAN_ERROR_MAX)||(BusError_FormCount>=CN_CAN_ERROR_MAX)||\
           (BusError_StuffCount>=CN_CAN_ERROR_MAX)||(BusError_OtherCount>=CN_CAN_ERROR_MAX))
        {
           Hard_Can_Init(byIndex,gs_CANCurBaudRate[byIndex]);
           BusError_BitCount=0;
           BusError_FormCount=0;
           BusError_StuffCount=0;
           BusError_OtherCount=0;
           return;
        }
    }
    //消极错误中断，只需记录错误状态，无需做其他特别处理。
    if(byIIR&EPIE_Bit)
    {
    	RxErrCount=*(pAddr + CN_PCAN_RXERR);
    	TxErrCount=*(pAddr + CN_PCAN_TXERR);
  //接收或发送错误计数超过127时，错误状态变为被动错误
    	if((RxErrCount>127)||(TxErrCount>127))
    	{
    		ErrStatusCurrent=ERR_PASSIVE;
    	}
  //接收或发送错误计数回到小于127时，错误状态变为主动错误
    	if((RxErrCount<127)&&(TxErrCount<127))
    	{
    		ErrStatusCurrent=ERR_ACTIVE;
    	}
    }
   //数据溢出中断,当接收FIFO溢出时，这时直接清除数据溢出、释放接收缓冲器。
   if(byIIR&DOI_Bit)
   {
	   Hard_Can_Init(byIndex,gs_CANCurBaudRate[byIndex]);
	   return;
   }
   //错误报警中断,目前只对总线关闭错误做了处理
   if(byIIR&EI_Bit)
   {
       if(gs_SR&BS_Bit)  //总线关闭
       {
          BusOff_Count++;
          Hard_Can_Init(byIndex,gs_CANCurBaudRate[byIndex]);
          return;
       }
   }

   if(byIIR&RI_Bit)  //接收中断
   {
	   Hard_Can_Message_Read( byIndex );
   }
   //重新使能屏蔽中断
       *(pAddr+CN_PCAN_IER)=0x7D;
}

/*******************************************************************************
功能:CAN设备的中断服务函数
输入:无
输出:无
*********************************************************************************/
u32 ISR_CAN( ptu32_t IsrNo )
{
    u32   dwValReg;
//清CPU中断
    Int_ClearLine(CN_INT_LINE_EINT8_23);
   //获取中断pending状态
    SNGS3C_REG_READ(rEINTPEND,dwValReg);
    if(dwValReg&(1<<8))
    {
    	 SNGS3C_REG_WRITE( rEINTPEND, (1<<8) );
         ISR_Deal( 0 );
    }
     if(dwValReg&(1<<9))
    {
    	SNGS3C_REG_WRITE( rEINTPEND, (1<<9) );
        ISR_Deal( 1 );
    }
     if(dwValReg&(1<<10))
    {
       	SNGS3C_REG_WRITE( rEINTPEND, (1<<10) );
        ISR_Deal( 2 );
    }
     if(dwValReg&(1<<11))
    {
    	SNGS3C_REG_WRITE( rEINTPEND, (1<<11) );
    	ISR_Deal( 3 );
    }
    return 0;
}

/*******************************************************************************
功能:读取CAN接收缓冲区中的数据，为对外接口
输入:byChipNum--CAN控制器选择，当前为0~3
     rxBuf---存储接收到报文的发送缓冲区  len--申请读的数据的长度
输出:读取到的数据的长度
*********************************************************************************/
u32 CAN_ReadData( u8 byChipNum,u8* rxBuf,u32 len)
{
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    CAN_RxTag* can_Rxtag=NULL;
    struct tagRingBuf *ring;
    u32 rdLen=0x0000;
    if( byChipNum >=CN_CAN_CHIP_NUM )
        return 0;
    if(!rxBuf)
    {
        return 0;
    }
    if(len==0)
    {
    	return 0;
    }
    if(len>CN_CAN_BUFSIZE)/*最大读取长度限制*/
          len=CN_CAN_BUFSIZE;
    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChipNum;
    if(!CAN_DevCtrlTempptr)
    {
        return 0;
    }
    can_Rxtag=CAN_DevCtrlTempptr->can_RxTag;/*设备收缓冲的结构体指针*/
    if(!can_Rxtag)
    {
        return 0;
    }
    ring=can_Rxtag->rxBuf;
    rdLen=Ring_Read(ring,recvbuf,len);
    return rdLen;
}

/*******************************************************************************
功能:读取CAN设备中的数据到接收缓冲区中，在中断中调用。
参数:byChipNum--CAN控制器编号，0~3。
输出:实际从硬件中读取的长度。
*********************************************************************************/
static u8 Hard_Can_Message_Read( u8 byChipNum )
{
    volatile u8*            pAddr;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    CAN_RxTag* can_Rxtag=NULL;
    struct tagRingBuf *ring;
    u8 PkgNum=0;
    u8 RecvNum=0;
    u8 i;
    u8 j;
    u8 buf[26];
    if( byChipNum >=CN_CAN_CHIP_NUM )
        return 0;
    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChipNum;
    can_Rxtag=CAN_DevCtrlTempptr->can_RxTag;/*设备收缓冲的结构体指针*/
    ring=can_Rxtag->rxBuf;
    pAddr = (volatile u8*)(CAN_DevCtrlTempptr->CANBaseAddr);/*设备操作的基地址*/
    for(i=0;i<2;i++)
    {
        if(0==(gs_SR&0x01))
    	    break;

		for(j=0;j<13;j++)
		{
		    buf[13*PkgNum+j]=*(pAddr+CN_PCAN_RXD+j);
		}
		PkgNum++;
		*(pAddr+CN_PCAN_CMR)=0x04;  //释放接收缓冲区
		for(j=0;j<=8;j++)   //等待1us
		{

		}
		gs_SR=*(pAddr+CN_CAN_SR);

    }
   RecvNum=13*PkgNum;
   if(PkgNum!=0)
   	{
	   Ring_Write(ring,buf,RecvNum);
   	   CAN_DevCtrlTempptr->RecvFlag=true;
   	   Lock_SempPost(g_ptCanSemp);
   	}
   return RecvNum;
}

/*******************************************************************************
功能:Tcp/Ip调用该函数将主板数据基于单包CAN帧通过指定CAN口发送到指定地址。
参数:byChipNum:CAN控制器编号，0~3
    buf: 要发送的数据指针
    len: 要发送的数据长度
返回值:实际通过CAN总线发送出去的字节数
备注：采用扩展帧格式将单包CAN帧发送，单包CAN帧加上帧头部分共13字节。
*********************************************************************************/
u8 Can_Send(u8 byChipNum,u8 * txbuf,u8 len)
{
    volatile u8*  pAddr;
    u8    bySR, byLen, byLoop=0;
    u32 Id=0x0000;

    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    if( byChipNum >=CN_CAN_CHIP_NUM )
    {
#ifdef DEBUG
    	printf("The CAN Chip Num should not bigger than CN_CAN_CHIP_NUM!\r\n");
#endif
         return 0;
    }
    if(len>13)
    {
#ifdef DEBUG
    	printf("The sizes of one Pkg is smaller than 13 bytes!\r\n");
#endif
        len=13;
    }

    CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChipNum;
    if(!CAN_DevCtrlTempptr)
    {
#ifdef DEBUG
    	printf("CAN Control Block error occurs!\r\n");
#endif
            return 0;
    }

        pAddr = (volatile u8* )(CAN_DevCtrlTempptr->CANBaseAddr);
        bySR = *( pAddr + CN_CAN_SR );/*获取发送缓冲区状态，是否处于release?*/
        if(0==(bySR&0x04))//通过SJA1000状态寄存器的发送缓冲器状态位判断数据是否可用
        {
           //等1ms之后再次判断是否可用
            Djy_EventDelay(10*mS);
            bySR = *( pAddr + CN_CAN_SR );
            if(0==(bySR&0x04))
            {
#ifdef DEBUG
       printf("CAN Transmit Failed because of Tx Buffer status is locked!\r\n");
#endif
              return 0;     // TBS not empty
            }
        }
        byLen=len-5;
        Id|=(u32)txbuf[1]<<24;
        Id|=(u32)txbuf[2]<<16;
        Id|=(u32)txbuf[3]<<8;
        Id|=(u32)txbuf[4];
        Id=Id<<3;
        *( pAddr + CN_PCAN_DSCR0 )   = txbuf[0];
        *( pAddr + CN_PCAN_TXD )   =   Id;
        *( pAddr + CN_PCAN_FRAMENO ) = Id>>8;
        *( pAddr + CN_PCAN_DSCR2 )   = Id>>16;
        *( pAddr + CN_PCAN_DSCR1 )   = Id>>24;
         for( byLoop=0;byLoop<byLen;byLoop++)
                    *(pAddr+(CN_PCAN_TXD+1+byLoop))=txbuf[byLoop+5];
            *(pAddr+CN_PCAN_CMR)=0x01;/*启动发送*/

        return byLen;
}
/*******************************************************************************
功能:从CAN口接收到CAN来自BMS或者充电模块的CAN帧。
参数:byChipNum:CAN控制器编号，0~3；
    buf: 要发送的数据指针；
    len: 要发送的数据长度。
返回值:0.
备注：通过CAN口将接收到的CAN帧调用Can2Eth将帧通过TCP包上传给应用层。
*******************************************************************************/
ptu32_t __Can_Recv(void)
{
    u8 i,j;
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    CAN_RxTag* can_Rxtag=NULL;
    atom_low_t atom;
    u32 PkgNum,temp,byReadBytes=0x0000;
    volatile u32 Id=0x0000;
    struct tagRingBuf *ring;
    while(1)
    {
    	 Lock_SempPend(g_ptCanSemp,CN_TIMEOUT_FOREVER);
    	 for(i=0;i<CN_CAN_CHIP_NUM;i++)
    	 {
    		 CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
    		 can_Rxtag=CAN_DevCtrlTempptr->can_RxTag;
    		 ring=can_Rxtag->rxBuf;
             if((CAN_DevCtrlTempptr->RecvFlag)&&(false==Ring_IsEmpty(ring)))
             {
				 atom = Int_LowAtomStart();
				 byReadBytes=Ring_Check(ring);
				 Int_LowAtomEnd(atom);
				 PkgNum=(u32)(byReadBytes/CN_SIGNAL_PKG_SIZES_PERI);
			  //一次最多读50个CAN帧
				 if(PkgNum>50)
				 {
					 PkgNum=50;
				 }
				 byReadBytes=PkgNum*CN_SIGNAL_PKG_SIZES_PERI;
				 CAN_ReadData(i,recvbuf,byReadBytes);
				 CAN_DevCtrlTempptr->RecvFlag=false;
				 for(j=0;j<PkgNum;j++)
				 {
					 //进行CAN Id挪位转换
					Id|=(u32)recvbuf[1+13*j]<<24;
					Id|=(u32)recvbuf[2+13*j]<<16;
					Id|=(u32)recvbuf[3+13*j]<<8;
					Id|=(u32)recvbuf[4+13*j];
					Id=Id>>3;
					temp=Id;
					recvbuf[4+13*j]=(u8)temp&0xff;
					temp=Id;
					recvbuf[3+13*j]=(u8)((temp>>8)&0xff);
					temp=Id;
					recvbuf[2+13*j]=(u8)((temp>>16)&0xff);
					temp=Id;
					recvbuf[1+13*j]=(u8)((temp>>24)&0xff);
					Id=0x0000;
					temp=0x0000;
				 }
				 Can2Eth(i,recvbuf,byReadBytes);
            }
         }

    }

    return 0;
}

/*******************************************************************************
功能:CAN通信口监控事件。
参数:无。
返回值:0.
备注：每隔100ms读取一次4个CAN口的CAN寄存器.
*******************************************************************************/
ptu32_t __Can_Monitor(void)
{
   u8 RegSr,RegIr,RegRmc;
   volatile u8*    pAddr;
   CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
   CAN_DevCtrlTempptr=CAN_DevCtrlPtr;
   pAddr = (u8*)(CAN_DevCtrlTempptr->CANBaseAddr);
   while(1)
   {
	 RegSr= *(pAddr+CN_PCAN_SR);
	 RegIr= *(pAddr+CN_PCAN_IIR);
	 RegRmc=*(pAddr+CN_PCAN_RXMC);
//	 printf("SR:%x,IIR:%x,RMC:%x\r\n",RegSr,RegIr,RegRmc);
	 Djy_EventDelay(100000);  //100ms
   }
}
/*******************************************************************************
功能:初始化CAN控制器并使能CAN接收中断
参数:空
返回值:成功初始化返回true，失败则返回false。
说明：将4个CAN控制器硬件固定设置为如下：
1.只接收发送扩展帧
2.通讯波特率为250kbps
3.4个CAN控制器地址依次为0xF0/0xF1/0xF2/0xF3

*********************************************************************************/
bool_t CAN_Main(void)
{
   u8 i;
   bool_t result;
   uint16_t evtt;
   CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
   CAN_RxTag* can_Rxtag=NULL;
   struct RingBuf *rxRingBuf=NULL;
   u8 ConfigBuf[CN_CAN_CONFIG_LEN];
   u8 temp=0x0000;
   u32 BaudRate=0x0000;

   //加载CAN-Shell命令
   CAN_Shell_Module_Install();

/*设置四个CAN控制器参数
 * 地址：CAN0/CAN1分别点对点接两个BMS，此时CAN0/CAN1第地址均是0x56(0xE5)，BMS的地址为0xF4
 * (依据国家电动汽车充电技术规范  )  采用扩展帧格式，波特率为250kbps
 * CAN2/CAN3分别接到两个充电模块组中，CAN2/CAN3的地址设置为0xF0/0xF0，充电模块地址依
 * 次为0,1,2,3...(依据为《监控协议》)
 * 采用扩展帧格式，波特率为125kbps   2015.06.04
 */
   //上电后，读取E2PROM中CAN控制器的配置信息
  //读3个bytes，第一个Byte为波特率，第二个及第三个分别为可选地址(一个地址或两个)

   for(i=0;i<CN_CAN_CHIP_NUM;i++)
   {
	   result=AT24_ReadBytes(CN_E2PROM_CAN_START_ADDR+i*CN_CAN_CONFIG_LEN, ConfigBuf, CN_CAN_CONFIG_LEN);
	   if(!result)
	  {
		 gs_CANCurBaudRate[i]=gs_CANDefaultBaudRate[i];
	  	 printk("Read from AT24C128 failed!\r\n");
	//若没有配置CAN芯片的波特率，则采用默认值
	   }
	  else
	  {
		 if(ConfigBuf[0]!=i) //通道编号都不正确，后面就没必要解析了，直接采用默认参数初始化
	    	 {
			gs_CANCurBaudRate[i]=gs_CANDefaultBaudRate[i];
	    	 }
	    	 else
	    	 {
			 if(ConfigBuf[3]!=CN_CAN_PARA_SUCCESS_FLAG)
	    	 {
				 gs_CANCurBaudRate[i]=gs_CANDefaultBaudRate[i];
	    	 }
			 else
	    	 {
	    	 //读取地址配置参数信息
	    	  temp=ConfigBuf[2];
				 if(temp==0)
	    	  {
					 BaudRate=ConfigBuf[1];
	    	  }
	    	  else   //没有配置地址，则采用默认地址
	    	  {
					 BaudRate|=(u32)temp<<8;
					 temp=ConfigBuf[1];
					 BaudRate|=temp;
	    	  }
			   gs_CANCurBaudRate[i]=BaudRate;
			 }
		   }
	    }
       printk("The CAN Num %d:The BaudRate is %d.\r\n",i,gs_CANCurBaudRate[i]);
	   result=Hard_Can_Init(i,gs_CANCurBaudRate[i]);

	   if(!result)
	   {
	      printk("The CAN Num %d init failed.\r\n",i);
		  return false;
	   }
   }
   //初始化环形缓冲区
   for(i=0;i<CN_CAN_CHIP_NUM;i++)
   {
		CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
		can_Rxtag=CAN_DevCtrlTempptr->can_RxTag;
		rxRingBuf=can_Rxtag->rxBuf;
		Ring_Init(rxRingBuf,rxRingBuf->buf,CN_CAN_BUFSIZE);

 //初始化CAN_MonitorReg数组，该数组用于CAN通信出故障后记录SJA1000的32个相关寄存器值
   }
   //创建一个信号量用于四个CAN控制器收到数据后告知用户去处理数据
  g_ptCanSemp=Lock_SempCreate(1,0,CN_SEMP_BLOCK_FIFO,"CAN Semp");
  if(g_ptCanSemp==NULL)
  {
	#ifdef DEBUG
		   printk("CAN Semp create failed!\r\n");
	#endif
	return false;
  }

  evtt = Djy_EvttRegist(EN_CORRELATIVE,130,0,0,__Can_Recv,
		            CAN_RecvStack,sizeof(CAN_RecvStack),"CAN Recv function");
  if(evtt!=CN_EVTT_ID_INVALID)
  {

	  //事件的两个参数暂设为0,如果用shell启动,可用来采集shell命令行参数
	  Djy_EventPop(evtt,NULL,0,NULL,0,130);
  }
  else
  {
	  Djy_EvttUnregist(evtt);
  }
  evtt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,__Can_Monitor,
		  CAN_MonitorStack,sizeof(CAN_MonitorStack),"CAN Monitor function");
   if(evtt!=CN_EVTT_ID_INVALID)
   {
	  Djy_EventPop(evtt,NULL,0,NULL,0,0);
   }
   else
   {
	  Djy_EvttUnregist(evtt);
   }
  return true;
}

/*******************************************************************************
功能:CAN控制器操作
参数:byChipNum--CAN控制器编号，0~3;
    cmd--操作码，详情见enum _CAN_CTRL_CMD_;
    data--每个参数操作指令带的参数。设置波特率，参数格式详见enum _CAN_BAUD_RATE_SET_。
返回值:无。
备注：通过将CAN2、CAN3自环，CAN2发送，CAN3接收，测试SJA1000的发送接收过程。
*********************************************************************************/
ptu32_t CAN_Shell_Module_Install(void)
{

	Sh_InstallCmd(shell_cmd_can_table,tg_can_shell_cmd_rsc,CN_CAN_SHELL_NUM);
	return 1;
}




