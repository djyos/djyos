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
// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：cpu_peri_can.c
// 模块描述: MCAN驱动
// 模块版本: V1.00
// 创建人员: zhb
// 创建时间: 05/01.2017
// =============================================================================
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include "cpu_peri.h"
#include "ring.h"
#include "os.h"
#include "stdlib.h"
#include "shell.h"
#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "string.h"

/*---------------------------------------------------------------------------
 *      Definitions
 *---------------------------------------------------------------------------*/

#define CN_CAN_RX_BUF_SIZE                2048
#define CN_CAN_DEV_MONITOR_TIME           3*1000*1000
#define CN_CAN_NUM                        1
#define CN_DEBUG_CAN_CNT                  10

#define CAN_SJW_1TQ      ((uint32_t)0x00000000U)     /*!< 1 time quantum */
#define CAN_SJW_2TQ      ((uint32_t)CAN_BTR_SJW_0)  /*!< 2 time quantum */
#define CAN_SJW_3TQ      ((uint32_t)CAN_BTR_SJW_1)  /*!< 3 time quantum */
#define CAN_SJW_4TQ      ((uint32_t)CAN_BTR_SJW)    /*!< 4 time quantum */
#define CAN_TS1_1TQ      ((uint32_t)0x00000000U)   /*!< 1 time quantum  */
#define CAN_TS1_2TQ      ((uint32_t)CAN_BTR_TS1_0)                                    /*!< 2 time quantum  */
#define CAN_TS1_3TQ      ((uint32_t)CAN_BTR_TS1_1)                                    /*!< 3 time quantum  */
#define CAN_TS1_4TQ      ((uint32_t)(CAN_BTR_TS1_1 | CAN_BTR_TS1_0))                  /*!< 4 time quantum  */
#define CAN_TS1_5TQ      ((uint32_t)CAN_BTR_TS1_2)                                    /*!< 5 time quantum  */
#define CAN_TS1_6TQ      ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_0))                  /*!< 6 time quantum  */
#define CAN_TS1_7TQ      ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_1))                  /*!< 7 time quantum  */
#define CAN_TS1_8TQ      ((uint32_t)(CAN_BTR_TS1_2 | CAN_BTR_TS1_1 | CAN_BTR_TS1_0))  /*!< 8 time quantum  */
#define CAN_TS1_9TQ      ((uint32_t)CAN_BTR_TS1_3)                                    /*!< 9 time quantum  */
#define CAN_TS1_10TQ     ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_0))                  /*!< 10 time quantum */
#define CAN_TS1_11TQ     ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_1))                  /*!< 11 time quantum */
#define CAN_TS1_12TQ     ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_1 | CAN_BTR_TS1_0))  /*!< 12 time quantum */
#define CAN_TS1_13TQ     ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2))                  /*!< 13 time quantum */
#define CAN_TS1_14TQ     ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2 | CAN_BTR_TS1_0))  /*!< 14 time quantum */
#define CAN_TS1_15TQ     ((uint32_t)(CAN_BTR_TS1_3 | CAN_BTR_TS1_2 | CAN_BTR_TS1_1))  /*!< 15 time quantum */
#define CAN_TS1_16TQ     ((uint32_t)CAN_BTR_TS1) /*!< 16 time quantum */

#define CAN_TS2_1TQ      ((uint32_t)0x00000000U)                       /*!< 1 time quantum */
#define CAN_TS2_2TQ      ((uint32_t)CAN_BTR_TS2_0)                    /*!< 2 time quantum */
#define CAN_TS2_3TQ      ((uint32_t)CAN_BTR_TS2_1)                    /*!< 3 time quantum */
#define CAN_TS2_4TQ      ((uint32_t)(CAN_BTR_TS2_1 | CAN_BTR_TS2_0))  /*!< 4 time quantum */
#define CAN_TS2_5TQ      ((uint32_t)CAN_BTR_TS2_2)                    /*!< 5 time quantum */
#define CAN_TS2_6TQ      ((uint32_t)(CAN_BTR_TS2_2 | CAN_BTR_TS2_0))  /*!< 6 time quantum */
#define CAN_TS2_7TQ      ((uint32_t)(CAN_BTR_TS2_2 | CAN_BTR_TS2_1))  /*!< 7 time quantum */
#define CAN_TS2_8TQ      ((uint32_t)CAN_BTR_TS2)                      /*!< 8 time quantum */

#define CAN_BRP_27       (uint32_t)(0x1A&CAN_BTR_BRP)

#define CAN_ID_STD             ((uint32_t)0x00000000U)  /*!< Standard Id */
#define CAN_ID_EXT             ((uint32_t)0x00000004U)  /*!< Extended Id */

#define CAN_RTR_DATA           ((uint32_t)0x00000000U)  /*!< Data frame */
#define CAN_RTR_REMOTE         ((uint32_t)0x00000002U)  /*!< Remote frame */

#define CN_CAN1    0
#define CN_CAN2    1

#define CAN_FILTERSCALE_16BIT     ((uint8_t)0x00U)  /*!< Two 16-bit filters */
#define CAN_FILTERSCALE_32BIT     ((uint8_t)0x01U)  /*!< One 32-bit filter  */

#define CAN_FILTERMODE_IDMASK     ((uint8_t)0x00U)  /*!< Identifier mask mode */
#define CAN_FILTERMODE_IDLIST     ((uint8_t)0x01U)  /*!< Identifier list mode */

#define CAN_FILTER_FIFO0       ((uint8_t)0x00U)  /*!< Filter FIFO 0 assignment for filter x */
#define CAN_FILTER_FIFO1       ((uint8_t)0x01U)  /*!< Filter FIFO 1 assignment for filter x */

typedef struct {
	bool_t RcvFlag;
	struct RingBuf *RcvRing;
	uint32_t BaudRate;
}CAN_DevCtrl;

/**
  * @brief  CAN Rx message structure definition
  */
typedef struct
{
  uint32_t ExtId;       /*!< Specifies the extended identifier.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF */
  uint32_t IDE;         /*!< Specifies the type of identifier for the message that will be received.
                              This parameter can be a value of @ref CAN_Identifier_Type */

  uint32_t RTR;         /*!< Specifies the type of frame for the received message.
                              This parameter can be a value of @ref CAN_remote_transmission_request */
  uint32_t DLC;         /*!< Specifies the length of the frame that will be received.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 8 */

  uint8_t Data[8];      /*!< Contains the data to be received.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 0xFF */

  uint32_t FMI;         /*!< Specifies the index of the filter the message stored in the mailbox passes through.
                             This parameter must be a number between Min_Data = 0 and Max_Data = 0xFF */

  uint32_t FIFONumber;  /*!< Specifies the receive FIFO number.
                             This parameter can be CAN_FIFO0 or CAN_FIFO1 */
}CanRxMsg;
/*---------------------------------------------------------------------------
 *      Internal variables
 *---------------------------------------------------------------------------*/
static uint32_t gs_CANBaudRate[CN_CAN_NUM]={125};
static CAN_DevCtrl *CAN_DevCtrlPtr=NULL;
static CanFilterConfPara *gs_pCanFilterPara=NULL;
static u64 gs_u64AppSndCnt[CN_CAN_NUM]={0};
static u64 gs_u64HardSndCnt[CN_CAN_NUM]={0};
static u64 gs_u64HardRcvCnt[CN_CAN_NUM]={0};
static u64 gs_u64AppRcvCnt[CN_CAN_NUM]={0};
static u64 gs_u64BusOffCnt[CN_CAN_NUM]={0};     //Bus off err
static u64 gs_u64PassiveErrCnt[CN_CAN_NUM]={0}; //Error passive flag
static u64 gs_u64CrcErrCnt[CN_CAN_NUM]={0};     //Crc err
static u64 gs_u64PasBitErrCnt[CN_CAN_NUM]={0};  //隐性位错误
static u64 gs_u64DomBitErrCnt[CN_CAN_NUM]={0};  //显性位错误
static u64 gs_u64FoErrCnt[CN_CAN_NUM]={0};      //format err
static u64 gs_u64StErrCnt[CN_CAN_NUM]={0};      //bit stuff err
static u64 gs_u64ACKErrCnt[CN_CAN_NUM]={0};     //ACK err
static u64 gs_TxErrRstCnt[CN_CAN_NUM]={0};
static u64 gs_RxErrRstCnt[CN_CAN_NUM]={0};
static u64 gs_u64RcvPkgBadCnt[CN_CAN_NUM]={0};
static u64 gs_u64SndPkgBadCnt[CN_CAN_NUM]={0};//数据包不完整，有效数据不为8个字节
static uint8_t CAN_MonitorStack[0x400];
static uint8_t gs_SndPkg[13*CN_DEBUG_CAN_CNT];
static uint8_t gs_RcvPkg[13*CN_DEBUG_CAN_CNT];
static uint8_t gs_SndDebugCnt=0,gs_RcvDebugCnt=0;
static bool_t gs_CanDebugFlag=false;
static bool_t gs_bLoopBackFlag=false;
/*---------------------------------------------------------------------------
 * 函数声明
 *---------------------------------------------------------------------------*/
static void CAN_ISR_Handler(ptu32_t IntLine);
bool_t CAN_Hard_Init(uint8_t byCanNo,uint32_t baudrate,CanFilterConfPara \
		* pFilterConfPara);
static bool_t Sh_CAN_Reset(char *param);
static void Sh_CAN_Stat(void);
static void Sh_Read_CAN_Reg(void);
static void Sh_CAN_Pkg(void);
static void Sh_CAN_LoopBack(char *param);
static void Sh_CAN_FilterConfig(char *param);


struct ShellCmdTab const shell_cmd_can_table[]=
{
	{
			"canrst",
			Sh_CAN_Reset,
			"复位CAN控制器",
			"COMMAND:canrst+CAN控制器编号+enter"
	},
	{
			"canstat",
			Sh_CAN_Stat,
			"CAN通信统计",
			"COMMAND:canstat+enter"
	},
	{
			"canreg",
			Sh_Read_CAN_Reg,
			"读取CAN寄存器值",
			"COMMAND:canreg+enter"
	},
	{
			"canpkg",
			Sh_CAN_Pkg,
			"获取CAN最近10次发送/接收报文内容",
			"COMMAND:canpkg+enter"
	},
	{
			"canlb",
			Sh_CAN_LoopBack,
			"测试CAN控制器的回环模式",
			"COMMAND:canlb+CAN控制器编号+enter"
	},
	{
			"canfc",
			Sh_CAN_FilterConfig,
			"配置CAN过滤器",
			"COMMAND:canf+配置参数列表+enter"
	}

};

#define CN_CAN_SHELL_NUM  sizeof(shell_cmd_can_table)/sizeof(struct ShellCmdTab)
static struct ShellCmdRsc tg_can_shell_cmd_rsc[CN_CAN_SHELL_NUM];


/*******************************************************************************
功能:CAN控制器操作shell模块
参数:无.
返回值:1。
*********************************************************************************/
ptu32_t CAN_Shell_Module_Install(void)
{
	Sh_InstallCmd(shell_cmd_can_table,tg_can_shell_cmd_rsc,CN_CAN_SHELL_NUM);
	return 1;
}

/*******************************************************************************
功能:复位CAN控制器。
参数:无。
输出:无。
*********************************************************************************/
static bool_t Sh_CAN_Reset(char *param)
{
	char *word_ChipNum,*word_trail,*next_param;
	uint8_t byCanNo;
	bool_t ret;
	//提取1个参数
	extern char *Sh_GetWord(char *buf,char **next);
	word_ChipNum = Sh_GetWord(param,&next_param);
	word_trail = Sh_GetWord(next_param,&next_param);
	if((word_ChipNum == NULL)||(word_trail != NULL))
	{
	  printf("\r\n格式错误，正确格式是：\r\n>d CAN控制器编号. \r\n");
	  return false;
	}
	byCanNo = strtol(word_ChipNum, (char **)NULL, 0);
	if(byCanNo>=2)
	{
		printf("The Chip Num must be one of 0/1!\r\n");
		return false;
	}
	ret=CAN_Hard_Init(byCanNo,gs_CANBaudRate[byCanNo],gs_pCanFilterPara);
	if(!ret)
		printf("CAN reset failed.\r\n");
	else
		printf("CAN reset success.\r\n");
	return true;
}


#define CN_PRINT_PREFIX  "   "
/*******************************************************************************
功能:获取CAN总线通信统计数据
参数:无。
输出:无。
*********************************************************************************/
static void Sh_CAN_Stat(void)
{
   uint32_t data[2];//used to print the s64 type
   uint8_t i;
   printf("CAN Stat:\r\n");
   for(i=0;i<CN_CAN_NUM;i++)
   {
	  printf("CAN %d:\r\n",i);
	  memcpy(data,&gs_u64AppSndCnt[i],sizeof(data));
	  printf("%s APP Snd Cnt      :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64HardSndCnt[i],sizeof(data));
	  printf("%s Hard Snd Cnt         :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64HardRcvCnt[i],sizeof(data));
	  printf("%s Hard Rcv Cnt         :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64AppRcvCnt[i],sizeof(data));
	  printf("%s APP Rcv Cnt          :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64SndPkgBadCnt[i],sizeof(data));
	  printf("%s Snd Bad Cnt          :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64RcvPkgBadCnt[i],sizeof(data));
	  printf("%s Rcv Bad Cnt          :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_TxErrRstCnt[i],sizeof(data));
	  printf("%s Snd Err Rst Cnt      :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_RxErrRstCnt[i],sizeof(data));
	  printf("%s Rcv Err Rst Cnt      :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64BusOffCnt[i],sizeof(data));
	  printf("%s Bus Off Cnt          :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64CrcErrCnt[i],sizeof(data));
	  printf("%s CRC Err Cnt          :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64PasBitErrCnt[i],sizeof(data));
	  printf("%s Passive Bit Err Cnt  :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64DomBitErrCnt[i],sizeof(data));
	  printf("%s Dominant Bit Err Cnt :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64FoErrCnt[i],sizeof(data));
	  printf("%s Format Err Cnt       :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64StErrCnt[i],sizeof(data));
	  printf("%s Stuff Err Cnt        :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
	  memcpy(data,&gs_u64ACKErrCnt[i],sizeof(data));
	  printf("%s ACK Err Cnt          :0x%08x 0x%08x \r\n",CN_PRINT_PREFIX,data[1],data[0]);
   }
   printf("The End.\r\n");
}

/*******************************************************************************
功能:获取CAN可读寄存器的值。
参数:无.
输出:无。
*********************************************************************************/
static void Sh_Read_CAN_Reg(void)
{
	  vu32 Reg;
	  uint8_t i,j;
	  tagCanReg *pCan;
	  printf("CAN Reg:\r\n");
	  for(i=0;i<CN_CAN_NUM;i++)
	  {
		  if(i==0)
		  {
			  pCan=(tagCanReg *)CAN1_BASE;
		  }
		  else
		  {
			  pCan=(tagCanReg *)CAN2_BASE;
		  }
		  printf("CAN %d:\r\n",i);
		  Reg=pCan->MCR;
		  printf("%s MCR     :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
		  Reg=pCan->MSR;
		  printf("%s MSR     :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
		  Reg=pCan->TSR;
		  printf("%s TSR     :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
		  Reg=pCan->RF0R;
		  printf("%s RF0R    :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
		  Reg=pCan->RF1R;
		  printf("%s RF0R    :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
		  Reg=pCan->IER;
		  printf("%s IER     :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
		  Reg=pCan->ESR;
		  printf("%s ESR     :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
		  Reg=pCan->BTR;
		  printf("%s BTR     :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
          // tx fifo
		  for(j=0;j<3;j++)
		  {
			  Reg=pCan->sTxMailBox[j].TIR;
			  printf("%s TIR %d  :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
			  Reg=pCan->sTxMailBox[j].TDTR;
			  printf("%s TITR %d :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
			  Reg=pCan->sTxMailBox[j].TDLR;
			  printf("%s TDLR %d :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
			  Reg=pCan->sTxMailBox[j].TDHR;
			  printf("%s TDHR %d :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
		  }
		  //rx fifo
		  for(j=0;j<2;j++)
		  {
			  Reg=pCan->sFIFOMailBox[j].RIR;
			  printf("%s RIR %d  :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
			  Reg=pCan->sFIFOMailBox[j].RDTR;
			  printf("%s RDTR %d :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
			  Reg=pCan->sFIFOMailBox[j].RDLR;
			  printf("%s RDLR %d :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
			  Reg=pCan->sFIFOMailBox[j].RDHR;
			  printf("%s RDHR %d :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
		  }
	  }
	  printf("Common Reg:\r\n");
	  pCan=(tagCanReg *)CAN1_BASE;
	  Reg=pCan->FMR;
	  printf("%s FMR     :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
	  Reg=pCan->FM1R;
	  printf("%s FM1R    :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
	  Reg=pCan->FS1R;
	  printf("%s FS1R    :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
	  Reg=pCan->FFA1R;
	  printf("%s FFA1R   :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
	  Reg=pCan->FA1R;
	  printf("%s FA1R    :0x%08x\r\n",CN_PRINT_PREFIX,Reg);
	  printf("The End.\r\n");
	  //filter
	  for(j=0;j<28;j++)
	  {
		  Reg=pCan->sFilterRegister[j].FR1;
		  printf("%s FR1 %d  :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
		  Reg=pCan->sFilterRegister[j].FR2;
		  printf("%s FR2 %d  :0x%08x\r\n",CN_PRINT_PREFIX,j,Reg);
	  }

}

/*******************************************************************************
功能:获取CAN可读寄存器的值。
参数:无.
输出:无。
*******************************************************************************/
static void Sh_CAN_Pkg(void)
{
	uint8_t i,j;
	gs_CanDebugFlag=true;
	Djy_EventDelay(3*1000*mS);
	printf("CAN Pkg Snd/Rcv:\r\n");
	printf("%s Snd:\r\n",CN_PRINT_PREFIX);
	for(i=0;i<CN_DEBUG_CAN_CNT;i++)
	{
       printf("%s ID:%d ",CN_PRINT_PREFIX,i);
       for(j=0;j<13;j++)
       {
    	   printf("0x%x ",gs_SndPkg[13*i+j]);
       }
       printf("\r\n");
	}
	printf("%s Rcv:\r\n",CN_PRINT_PREFIX);
	for(i=0;i<CN_DEBUG_CAN_CNT;i++)
	{
	   printf("%s ID:%d ",CN_PRINT_PREFIX,i);
	   for(j=0;j<13;j++)
	   {
		   printf("0x%x ",gs_RcvPkg[13*i+j]);
	   }
	   printf("\r\n");
	}
	gs_CanDebugFlag=false;
	gs_SndDebugCnt=0;
	gs_RcvDebugCnt=0;
	memset(gs_SndPkg,0,13*CN_DEBUG_CAN_CNT);
	memset(gs_RcvPkg,0,13*CN_DEBUG_CAN_CNT);
	printf("The End.\r\n");
}

/*******************************************************************************
功能:获取CAN可读寄存器的值。
参数:无.
输出:无。
*******************************************************************************/
static void Sh_CAN_LoopBack(char *param)
{
	char *word_ChipNum,*word_trail,*next_param;
	uint8_t byCanNo;
	//提取1个参数
	extern char *Sh_GetWord(char *buf,char **next);
	word_ChipNum = Sh_GetWord(param,&next_param);
	word_trail = Sh_GetWord(next_param,&next_param);
	if((word_ChipNum == NULL)||(word_trail != NULL))
	{
	  printf("\r\n格式错误，正确格式是：\r\n>d CAN控制器编号. \r\n");
	  return ;
	}
	byCanNo = strtol(word_ChipNum, (char **)NULL, 0);
	if(byCanNo>=2)
	{
		printf("The Chip Num must be one of 0/1!\r\n");
		return ;
	}
	gs_bLoopBackFlag=true;
	CAN_Hard_Init(byCanNo,gs_CANBaudRate[byCanNo],gs_pCanFilterPara);
}

/*******************************************************************************
功能:CAN控制块内存申请初始化。
参数:空。
返回:true--初始化成功，false--初始化失败。
******************************************************************************/
static void Sh_CAN_FilterConfig(char *param)
{
	char *word_FilterIdHigh,*word_FilterIdLow,*word_FilterMaskIdHigh,*word_FilterMaskIdLow;
	char *word_FilterFIFOAssignment,*word_FilterNumber,*word_FilterMode;
	char *word_FilterScale,*word_FilterActivation,*word_BankNumber;
	char *word_trail,*next_param;
	CanFilterConfPara Para;
	uint8_t i;
	extern char *Sh_GetWord(char *buf,char **next);
	word_FilterIdHigh = Sh_GetWord(param,&next_param);
	word_FilterIdLow=Sh_GetWord(next_param,&next_param);
	word_FilterMaskIdHigh=Sh_GetWord(next_param,&next_param);
	word_FilterMaskIdLow=Sh_GetWord(next_param,&next_param);
	word_FilterFIFOAssignment=Sh_GetWord(next_param,&next_param);
	word_FilterNumber=Sh_GetWord(next_param,&next_param);
	word_FilterMode=Sh_GetWord(next_param,&next_param);
	word_FilterScale=Sh_GetWord(next_param,&next_param);
	word_FilterActivation=Sh_GetWord(next_param,&next_param);
	word_BankNumber=Sh_GetWord(next_param,&next_param);
	word_trail = Sh_GetWord(next_param,&next_param);

	if((word_FilterIdHigh == NULL)||(word_FilterIdLow==NULL)||(word_FilterMaskIdHigh==NULL)||\
		(word_FilterMaskIdLow==NULL)||(word_FilterFIFOAssignment==NULL)||\
		(word_FilterNumber==NULL)||(word_FilterMode==NULL)||(word_FilterScale==NULL)||\
		(word_FilterActivation==NULL)||(word_BankNumber==NULL)||(word_trail != NULL))
	{
	  printf("\r\nCAN过滤器参数不对.\r\n");
	  return ;
	}
	Para.FilterIdHigh=strtol(word_FilterIdHigh, (char **)NULL, 0);
	Para.FilterIdLow=strtol(word_FilterIdLow, (char **)NULL, 0);
	Para.FilterMaskIdHigh=strtol(word_FilterMaskIdHigh, (char **)NULL, 0);
	Para.FilterMaskIdLow=strtol(word_FilterMaskIdLow, (char **)NULL, 0);
	Para.FilterFIFOAssignment=strtol(word_FilterFIFOAssignment, (char **)NULL, 0);
	Para.FilterNumber=strtol(word_FilterNumber, (char **)NULL, 0);
	Para.FilterMode=strtol(word_FilterMode, (char **)NULL, 0);
	Para.FilterScale=strtol(word_FilterScale, (char **)NULL, 0);
	Para.FilterActivation=strtol(word_FilterActivation, (char **)NULL, 0);
	Para.BankNumber=strtol(word_BankNumber, (char **)NULL, 0);
    memcpy(gs_pCanFilterPara,&Para,sizeof(CanFilterConfPara));
    for(i=0;i<CN_CAN_NUM;i++)
    {
    	CAN_Hard_Init(i,gs_CANBaudRate[i],gs_pCanFilterPara);
    }
}
/*******************************************************************************
功能:CAN控制块内存申请初始化。
参数:空。
返回:true--初始化成功，false--初始化失败。
******************************************************************************/
bool_t __CAN_MemInit()
{
    CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
    uint8_t i;
    struct RingBuf *RingBuf;
    gs_pCanFilterPara=malloc(sizeof(CanFilterConfPara));
    if(gs_pCanFilterPara==NULL)
    	return false;
    memset(gs_pCanFilterPara,0,sizeof(CanFilterConfPara));
    CAN_DevCtrlPtr=(CAN_DevCtrl *)malloc(sizeof(CAN_DevCtrl)*CN_CAN_NUM);
    if(!CAN_DevCtrlPtr)
        return false;
    for(i=0;i<CN_CAN_NUM;i++)
    {
       CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
	   RingBuf=(struct RingBuf *)malloc(sizeof(struct RingBuf));
	   if(!RingBuf)
	   {
		  goto ERRORDeal;
	   }
	   RingBuf->buf=(uint8_t *)malloc(CN_CAN_RX_BUF_SIZE);
	   if(!RingBuf->buf)
	   {
		  goto ERRORDeal;
	   }

        CAN_DevCtrlTempptr->RcvRing=RingBuf;
    }
    return true;

ERRORDeal:
    for(i=0;i<CN_CAN_NUM;i++)
    {
        CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
        if(CAN_DevCtrlTempptr->RcvRing)
        {
            free(CAN_DevCtrlTempptr->RcvRing);
        }
    }
    free(CAN_DevCtrlPtr);
    return false;
}

//----CAN GPIO引脚设置----------------------------------------------------------
//功能: 初始化CAN中断配置
//参数: pCanPara,CAN控制器配置参数结构体指针
//返回: 成功初始化则返回true,否则则返回false.
//-----------------------------------------------------------------------------
static void __CAN_GpioConfig(uint8_t byCanNo)
{
	switch(byCanNo)
	{
		case CN_CAN1:
		case CN_CAN2:
			RCC->APB1ENR |=(1<<25);//CAN1时钟使能
			break;

		default:
			break;
	}
}
//----CAN滤波器设置------------------------------------------------------------
//功能: 初始化CAN中断配置
//参数: pCanPara,CAN控制器配置参数结构体指针
//返回: 成功初始化则返回true,否则则返回false.
//-----------------------------------------------------------------------------
static bool_t __CAN_ConfigFilter(CanFilterConfPara* pFilterConfig)
{
	uint32_t filternbrbitpos = 0;
	tagCanReg *pCan;
    if(pFilterConfig==NULL)
    	return false;
    filternbrbitpos = ((uint32_t)1) << pFilterConfig->FilterNumber;
    pCan=(tagCanReg *)CAN1_BASE;
    //Init mode for the filter
    pCan->FMR|=(uint32_t)CAN_FMR_FINIT;
    //Select the start slave bank
    pCan->FMR&=~(uint32_t)CAN_FMR_CAN2SB;
    pCan->FMR|=(uint32_t)(pFilterConfig->BankNumber<<8);
    //Filter Deactivation
    pCan->FA1R&=~(uint32_t)filternbrbitpos;
    //Set filter scale
    if(pFilterConfig->FilterScale==CAN_FILTERSCALE_16BIT)
    {
    	//16 bit scale for the filter
    	pCan->FS1R&=~(uint32_t)filternbrbitpos;
    	//First 16 bit identifier and First 16 bit mask (mask mode)
    	//Or First 16 bit identifier and Second 16 bit identifier(list mode)
    	pCan->sFilterRegister[pFilterConfig->FilterNumber].FR1=
    	((0x0000FFFF & (uint32_t)pFilterConfig->FilterMaskIdLow) << 16) |
    	(0x0000FFFF & (uint32_t)pFilterConfig->FilterIdLow);
    	/* Second 16-bit identifier and Second 16-bit mask */
    	    /* Or Third 16-bit identifier and Fourth 16-bit identifier */
    	pCan->sFilterRegister[pFilterConfig->FilterNumber].FR2 =
		 ((0x0000FFFF & (uint32_t)pFilterConfig->FilterMaskIdHigh) << 16) |
		  (0x0000FFFF & (uint32_t)pFilterConfig->FilterIdHigh);
    }
    else
    {
        /* 32-bit scale for the filter */
    	pCan->FS1R |= filternbrbitpos;
        /* 32-bit identifier or First 32-bit identifier */
    	pCan->sFilterRegister[pFilterConfig->FilterNumber].FR1 =
         ((0x0000FFFF & (uint32_t)pFilterConfig->FilterIdHigh) << 16) |
          (0x0000FFFF & (uint32_t)pFilterConfig->FilterIdLow);
        /* 32-bit mask or Second 32-bit identifier */
    	pCan->sFilterRegister[pFilterConfig->FilterNumber].FR2 =
         ((0x0000FFFF & (uint32_t)pFilterConfig->FilterMaskIdHigh) << 16) |
          (0x0000FFFF & (uint32_t)pFilterConfig->FilterMaskIdLow);
    }
    //Filter mode
    if(pFilterConfig->FilterMode==CAN_FILTERMODE_IDMASK)
    {
    	pCan->FM1R&=~(uint32_t)filternbrbitpos;
    }
    else
    {
    	pCan->FM1R |= (uint32_t)filternbrbitpos;
    }
    //Filter FIFO ASSIGNMENT
    if (pFilterConfig->FilterFIFOAssignment == CAN_FILTER_FIFO0)
    {
       /* FIFO 0 assignation for the filter */
    	pCan->FFA1R &= ~(uint32_t)filternbrbitpos;
    }
    if (pFilterConfig->FilterFIFOAssignment == CAN_FILTER_FIFO1)
    {
       /* FIFO 1 assignation for the filter */
    	pCan->FFA1R |= (uint32_t)filternbrbitpos;
    }
    //Filter activation
    if(pFilterConfig->FilterActivation==ENABLE)
    {
    	pCan->FA1R |= filternbrbitpos;
    }
    //Leave the init mode for the filter
    pCan->FMR&=~(uint32_t)CAN_FMR_FINIT;
    return true;
}
//----初始化CAN中断配置---------------------------------------------------------
//功能: 初始化CAN中断配置
//参数: pCanPara,CAN控制器配置参数结构体指针
//返回: 成功初始化则返回true,否则则返回false.
//-----------------------------------------------------------------------------

static void  __CAN_IntInit(uint8_t byCanNo)
{
	//挂中断线
	if(byCanNo==0)
	{
		//发送中断线
		Int_Register(CN_INT_LINE_CAN1_RX0);
		Int_SetClearType(CN_INT_LINE_CAN1_RX0,CN_INT_CLEAR_AUTO);
		Int_IsrConnect(CN_INT_LINE_CAN1_RX0,CAN_ISR_Handler);
		Int_SettoAsynSignal(CN_INT_LINE_CAN1_RX0);
		Int_ClearLine(CN_INT_LINE_CAN1_RX0);
		Int_RestoreAsynLine(CN_INT_LINE_CAN1_RX0);

		//发送中断线
		Int_Register(CN_INT_LINE_CAN1_RX1);
		Int_SetClearType(CN_INT_LINE_CAN1_RX1,CN_INT_CLEAR_AUTO);
		Int_IsrConnect(CN_INT_LINE_CAN1_RX1,CAN_ISR_Handler);
		Int_SettoAsynSignal(CN_INT_LINE_CAN1_RX1);
		Int_ClearLine(CN_INT_LINE_CAN1_RX1);
		Int_RestoreAsynLine(CN_INT_LINE_CAN1_RX1);

		//发送中断线
		Int_Register(CN_INT_LINE_CAN1_SCE);
		Int_SetClearType(CN_INT_LINE_CAN1_SCE,CN_INT_CLEAR_AUTO);
		Int_IsrConnect(CN_INT_LINE_CAN1_SCE,CAN_ISR_Handler);
		Int_SettoAsynSignal(CN_INT_LINE_CAN1_SCE);
		Int_ClearLine(CN_INT_LINE_CAN1_SCE);
		Int_RestoreAsynLine(CN_INT_LINE_CAN1_SCE);
	}
//	else
//	{
//		//发送中断线
//		Int_Register(CN_INT_LINE_CAN2_RX0);
//		Int_SetClearType(CN_INT_LINE_CAN2_RX0,CN_INT_CLEAR_AUTO);
//		Int_IsrConnect(CN_INT_LINE_CAN2_RX0,CAN_ISR_Handler);
//		Int_SettoAsynSignal(CN_INT_LINE_CAN2_RX0);
//		Int_ClearLine(CN_INT_LINE_CAN2_RX0);
//		Int_RestoreAsynLine(CN_INT_LINE_CAN2_RX0);
//
//		//发送中断线
//		Int_Register(CN_INT_LINE_CAN2_RX1);
//		Int_SetClearType(CN_INT_LINE_CAN2_RX1,CN_INT_CLEAR_AUTO);
//		Int_IsrConnect(CN_INT_LINE_CAN2_RX1,CAN_ISR_Handler);
//		Int_SettoAsynSignal(CN_INT_LINE_CAN2_RX1);
//		Int_ClearLine(CN_INT_LINE_CAN2_RX1);
//		Int_RestoreAsynLine(CN_INT_LINE_CAN2_RX1);
//
//		//发送中断线
//		Int_Register(CN_INT_LINE_CAN2_SCE);
//		Int_SetClearType(CN_INT_LINE_CAN2_SCE,CN_INT_CLEAR_AUTO);
//		Int_IsrConnect(CN_INT_LINE_CAN2_SCE,CAN_ISR_Handler);
//		Int_SettoAsynSignal(CN_INT_LINE_CAN2_SCE);
//		Int_ClearLine(CN_INT_LINE_CAN2_SCE);
//		Int_RestoreAsynLine(CN_INT_LINE_CAN2_SCE);
//	}

}

//----配置CAN通信波特率---------------------------------------------------------
//功能: 配置CAN通信波特率。
//参数: pCan,CAN控制器寄存器结构体指针；
//      baudrate,CAN通信波特率。
//返回: 成功返回true,失败返回false.
//说明：在充电桩应用中仅存在250k及125k的通信波特率，目前仅支持这两种波特率.CAN1/CAN2
//      时钟源均来自APB1,在sysinit.c中已将APB1的时钟设置为54MHz.
//      can_bd=(54M)/((BRP+1)*(1+(TS1+1)+(TS2+1))
//-----------------------------------------------------------------------------
static bool_t __CAN_SetBaudRate(tagCanReg * pCan,uint32_t baudrate)
{
    if(pCan==NULL)
    	return false;
	if(baudrate==125)
	{
      pCan->BTR=(uint32_t)CAN_BRP_27|CAN_SJW_1TQ|CAN_TS1_9TQ|CAN_TS2_6TQ;
	}
	else if(baudrate==250)
	{
		pCan->BTR=(uint32_t)CAN_BRP_27|CAN_SJW_1TQ|CAN_TS1_5TQ|CAN_TS2_2TQ;
	}
	else
		return false;
	return true;

}

/*******************************************************************************
功能:CAN设备的初始化函数，初始化中断控制器，申请内存，初始化器件。
参数:byIndex--CAN控制器编号，0~1.
输出:true--初始化成功，false-初始化失败。
说明：
*********************************************************************************/
bool_t CAN_Hard_Init(uint8_t byCanNo,uint32_t baudrate,CanFilterConfPara \
		* pFilterConfPara)
{
	tagCanReg *pCan;
	static uint8_t flag=0;
	uint32_t i=0;
	CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
	struct RingBuf *rxRingBuf=NULL;
	if(byCanNo==0)
	{
      pCan=(tagCanReg *)CAN1_BASE;
	}
	else if(byCanNo==1)
	{
	  pCan=(tagCanReg *)CAN2_BASE;
	}
	else
		return false;
	if(!flag)
	{
		if(__CAN_MemInit()==false)
		{
			printf("CAN Dev Control Block malloc failed.\r\n");
			return false;
		}
		//初始化接收环形缓冲区
		for(i=0;i<CN_CAN_NUM;i++)
		{
			CAN_DevCtrlTempptr=CAN_DevCtrlPtr+i;
			rxRingBuf=CAN_DevCtrlTempptr->RcvRing;
			Ring_Init(rxRingBuf,rxRingBuf->buf,CN_CAN_RX_BUF_SIZE);
		}
		flag++;
	}

	CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byCanNo;
    CAN_DevCtrlTempptr->BaudRate=baudrate;
    CAN_DevCtrlTempptr->RcvFlag=false;

    __CAN_GpioConfig(byCanNo);

	//Exit from sleep mode
	pCan->MCR&=(~(uint32_t)CAN_MCR_SLEEP);
	//Request init
	pCan->MCR|=CAN_MCR_INRQ;
	//Wait the acknowledge
	while((pCan->MSR&CAN_MSR_INAK)!=CAN_MSR_INAK)
	{
		i++;
		if(i>1000000)
		{
			return false;
		}
	}
	//Disable time triggered communication mode
	pCan->MCR&=~(uint32_t)CAN_MCR_TTCM;
	//Set automatic bus-off management
//	pCan->MCR|=CAN_MCR_ABOM;
	pCan->MCR&=~(uint32_t)CAN_MCR_ABOM;
	//Set automatic wake-up mode
//	pCan->MCR|=CAN_MCR_AWUM;
	pCan->MCR&=~(uint32_t)CAN_MCR_AWUM;
	//Set no automatic retransmission
	pCan->MCR&=~(uint32_t)CAN_MCR_NART;      //todo
	//Set transmit FIFO priority (first in first out)
	pCan->MCR|=CAN_MCR_TXFP;
	//Set recv FIFO locked mode
	pCan->MCR&=~(uint32_t)CAN_MCR_RFLM;

	//Set the bit timing register  APB1上 54MHz
	__CAN_SetBaudRate(pCan,baudrate);
   //Set loopback test mode
//	pCan->BTR|=CAN_BTR_LBKM;   //todo debug for test
//
//	if(gs_bLoopBackFlag)
//	{
//		pCan->BTR|=CAN_BTR_LBKM;
//	}
	//Set filter configuration
	if(pFilterConfPara!=NULL)
	{
		__CAN_ConfigFilter(pFilterConfPara);
	}

	 //Init int
	__CAN_IntInit(byCanNo);
	//Enabel Recv Fifo(FMPIE/FFIE/FOVIE) EPVIE BOFIE LECIE ERRIE interrupt
	pCan->IER|=CAN_IER_FMPIE0|CAN_IER_FFIE0|CAN_IER_FOVIE0|CAN_IER_FMPIE1\
		|CAN_IER_FFIE1|CAN_IER_FOVIE1|CAN_IER_BOFIE|CAN_IER_LECIE|CAN_IER_ERRIE;
	//Request leave init
	pCan->MCR&=~(uint32_t)CAN_MCR_INRQ;
	//Wait the acknowledge
	i=0;
	while((pCan->MSR&CAN_MSR_INAK)!=CAN_MSR_INAK)
	{
		i++;
		if(i>1000000)
		{
			return false;
		}
	}
	return true;
}

//----选择一个空闲的发送邮箱-------------------------------------
//功能: 从编号0开始依次查询三个发送邮箱的状态是否为空，直到找到找到一个空邮箱，如果
//      3个发送邮箱均不为空，则返回-1.
//参数: byChip:  CAN控制器编号
//返回: 如果3个发送邮箱均不为空，则返回-1.其他则相应的返回0、1、2.
//-----------------------------------------------------------------------------
static sint8_t __SelectOneEmptyTxMalibox(uint8_t byChip)
{
	tagCanReg *pCan;
	if(byChip==0)
	    pCan=(tagCanReg *)CAN1_BASE;
	else if(byChip==1)
	    pCan=(tagCanReg *)CAN2_BASE;
	else
		return false;
	if((pCan->TSR&CAN_TSR_TME0)==CAN_TSR_TME0)
		return 0;
	else if((pCan->TSR&CAN_TSR_TME1)==CAN_TSR_TME1)
		return 1;
	else if((pCan->TSR&CAN_TSR_TME2)==CAN_TSR_TME2)
		return 2;
	else
		return -1;

}

//----检查某个指定发送邮箱是否成功将数据发送出去-------------------------------------
//功能: 检查某个指定发送邮箱是否成功将数据发送出去。
//参数: TxMailboxNo:  发送邮箱编号，0~2.
//返回: 如果发送成功返回true,失败则返回false.
//-----------------------------------------------------------------------------
static bool_t __IsCanTxOk(uint8_t byChip,uint8_t TxMailboxNo)
{
	tagCanReg *pCan;
	if(byChip==0)
	{
	  pCan=(tagCanReg *)CAN1_BASE;
	}
	else if(byChip==1)
	{
	  pCan=(tagCanReg *)CAN2_BASE;
	}
	else
		return false;
	if(TxMailboxNo==0)
	{
		if((pCan->TSR&(CAN_TSR_RQCP0|CAN_TSR_TXOK0|CAN_TSR_TME0))==\
				(CAN_TSR_RQCP0|CAN_TSR_TXOK0|CAN_TSR_TME0))
		{
			return true;
		}
	}
	else if(TxMailboxNo==1)
	{
		if((pCan->TSR&(CAN_TSR_RQCP1|CAN_TSR_TXOK1|CAN_TSR_TME1))==\
						(CAN_TSR_RQCP1|CAN_TSR_TXOK1|CAN_TSR_TME1))
		{
			return true;
		}
	}
	else if(TxMailboxNo==2)
	{
		if((pCan->TSR&(CAN_TSR_RQCP2|CAN_TSR_TXOK2|CAN_TSR_TME2))==\
						(CAN_TSR_RQCP2|CAN_TSR_TXOK2|CAN_TSR_TME2))
		{
			return true;
		}
	}
	else
		return false;
	return false;
}
//----CAN发送函数-------------------------------------
//功能: CAN发送函数
//参数:
       // byChip:  CAN控制器编号   0~1(为兼容项目组代码，保留此参数，在SR5333中该参数
//        为0，SR5333 V1.01版本中只使用了一个CAN控制器)
       // txBuf:   报文的发送缓冲区
       // len:     发送报文的长度,13的整数倍.
//返回: 成功发送的长度
//-----------------------------------------------------------------------------
uint32_t CAN_WriteData(uint8_t byChip, uint8_t* txBuf, uint32_t len)
{
	uint32_t Id=0,Len=0;
	uint8_t pkgnum,i,j;
	tagCanReg *pCan;
	sint8_t TxMailboxNo;
	//参数检查
	if(txBuf==NULL)
	{
		gs_u64SndPkgBadCnt[byChip]++;
		return 0;
	}
	if(byChip>=2)
	{
		gs_u64SndPkgBadCnt[byChip]++;
		return 0;
	}
	if((len%13!=0)||(len==0))
	{
		gs_u64SndPkgBadCnt[byChip]++;
		return 0;
	}
	if(byChip==0)
	{
	  pCan=(tagCanReg *)CAN1_BASE;
	}
	else if(byChip==1)
	{
	  pCan=(tagCanReg *)CAN2_BASE;
	}
	else
		return false;
    pkgnum=len/13;
    gs_u64AppSndCnt[byChip]+=pkgnum;
    for(i=0;i<pkgnum;i++)
    {
        //check Tx mailbox is or not empty
	    TxMailboxNo=__SelectOneEmptyTxMalibox(byChip);
	    if(TxMailboxNo==-1)
	    {
	    	//Wait for 5mS
	    	Djy_EventDelay(5*mS);
	    	TxMailboxNo=__SelectOneEmptyTxMalibox(byChip);
	    	if(TxMailboxNo==-1)
	    	{
	    		gs_u64SndPkgBadCnt[byChip]++;
	    		continue;
	    	}
	    }
	    Id=0x0000;
		Id|=(uint32_t)txBuf[1+13*i]<<24;
		Id|=(uint32_t)txBuf[2+13*i]<<16;
		Id|=(uint32_t)txBuf[3+13*i]<<8;
		Id|=(uint32_t)txBuf[4+13*i];
		//Request the tx mailbox
	    pCan->sTxMailBox[TxMailboxNo].TIR&=CAN_TI0R_TXRQ;
        //Set ID and RTR/IDE filed  Data frame Extend Frame
	    pCan->sTxMailBox[TxMailboxNo].TIR|=(Id<<3)|(1<<2);
        //Set DLC filed
	    pCan->sTxMailBox[TxMailboxNo].TDTR&=(uint32_t)0xFFFFFFF0U;
	    pCan->sTxMailBox[TxMailboxNo].TDTR|=0x00000008U;
	    //Set data filed
	    pCan->sTxMailBox[TxMailboxNo].TDLR=(uint32_t)(txBuf[13*i+8]<<24)|\
	    		(uint32_t)(txBuf[13*i+7]<<16)|(uint32_t)(txBuf[13*i+6]<<8)|\
				(uint32_t)txBuf[13*i+5];

	    pCan->sTxMailBox[TxMailboxNo].TDHR=(uint32_t)(txBuf[13*i+12]<<24)|\
	   	    	(uint32_t)(txBuf[13*i+11]<<16)|(uint32_t)(txBuf[13*i+10]<<8)\
	   			|(uint32_t)txBuf[13*i+9];
	    //Request transmission
	    pCan->sTxMailBox[TxMailboxNo].TIR|=CAN_TI0R_TXRQ;

		//Wait for 5ms
		Djy_EventDelay(5*mS);
		//查询是否成功发送出去
		if(__IsCanTxOk(byChip,TxMailboxNo))
		{
			gs_u64HardSndCnt[byChip]++;
		    Len+=13;
		}
		//CAN调试时抓取最近10次发送报文
		if(gs_CanDebugFlag)
		{
			if(gs_SndDebugCnt<CN_DEBUG_CAN_CNT)
			{
				for(j=0;j<13;j++)
				{
				   gs_SndPkg[gs_SndDebugCnt*13+j]=txBuf[j];
				}
				gs_SndDebugCnt++;
			}
		}
    }
    return Len;
}


//----CAN接收函数-------------------------------------
//功能: CAN发送函数
//参数:
      // byChip:  CAN控制器选择   0~4(为兼容项目组代码，保留此参数，在SR5333中该参数
      //  为0，SR5333 V1.01版本中只使用了一个CAN控制器)
      // txBuf:    报文的接收缓冲区
      // len:     请求读取的长度
      // pRd:     缓冲区读指针
//返回: 成功读取数据的长度
//-----------------------------------------------------------------------------
uint32_t CAN_ReadData(uint8_t byChip, uint8_t* rxBuf, uint32_t len, uint32_t *pRd)
{
	uint32_t rdLen=0x0000,ringlen,pkgnum,r_len;
	CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
	struct RingBuf *rxRingBuf=NULL;
	atom_low_t atom;
	//先作参数检查
	if(rxBuf==NULL)
		 return 0;
	if(byChip>=2)
		return 0;
	CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChip;
	rxRingBuf=CAN_DevCtrlTempptr->RcvRing;
	atom = Int_LowAtomStart();
	ringlen=Ring_Check(rxRingBuf);
	Int_LowAtomEnd(atom);
	if(len>ringlen)
	{
		r_len=ringlen;
	}
	else
	{
		r_len=len;
	}
	rdLen=Ring_Read(rxRingBuf,rxBuf,r_len);
	pkgnum=rdLen/13;
	gs_u64AppRcvCnt[byChip]+=pkgnum;
	return rdLen;
}

//----将CAN硬件接收到的数据写进环形缓冲区中---------------------------------------------------------
//功能: 将CAN硬件接收到的数据写进环形缓冲区中，每帧向缓冲区写入13bytes.
//参数: rxMsg,CAN接收数据结构体;
//返回: 空
//-----------------------------------------------------------------------------
static void __WriteMsgToRing(uint8_t byChip,CanRxMsg rxMsg)
{
	uint8_t i,j;
	uint8_t buf[13];
	uint32_t can_id,tmp;
	CAN_DevCtrl *CAN_DevCtrlTempptr=NULL;
	struct RingBuf *rxRingBuf=NULL;
	buf[0]=0x88;
	can_id=rxMsg.ExtId;
	tmp=can_id;
	buf[1]=tmp;
	buf[2]=tmp>>8;
	buf[3]=tmp>>16;
	buf[4]=tmp>>24;
	for(i=0;i<8;i++)
	{
		buf[5+i]=rxMsg.Data[i];
	}
	CAN_DevCtrlTempptr=CAN_DevCtrlPtr+byChip;
	rxRingBuf=CAN_DevCtrlTempptr->RcvRing;
	Ring_Write(rxRingBuf,buf,13);
	gs_u64HardRcvCnt[byChip]++;
//CAN调试抓取最近10次接收报文
	if(gs_CanDebugFlag)
	{
		if(gs_RcvDebugCnt<CN_DEBUG_CAN_CNT)
		{
			for(j=0;j<13;j++)
			{
			   gs_RcvPkg[gs_RcvDebugCnt*13+j]=buf[j];
			}
			gs_RcvDebugCnt++;
		}
	}
}
//----接收中断响应函数---------------------------------------------------------
//功能: 依次查询专用接收Buffer(Dedicated RX Buffer)，如果有数据则
//参数: byChip,CAN控制器编号，0、1;
//      byFiFoNo,接收FIFO编号，0、1.
//返回: 空
//-----------------------------------------------------------------------------
static void __CAN_Recv_FiFo_Handle(uint8_t byChip,uint8_t byFiFoNo)
{
	tagCanReg *pCan;
	CanRxMsg RcvMsg;
	uint8_t cnt,i,j,ret;
	if(byChip==0)
	{
	  pCan=(tagCanReg *)CAN1_BASE;
	}
	else if(byChip==1)
	{
	  pCan=(tagCanReg *)CAN2_BASE;
	}
	else
		return ;
	if(byFiFoNo==0)
	    cnt=pCan->RF0R&(uint32_t)0x03;  //check if fifo0 msg pending
	else
		cnt=pCan->RF1R&(uint32_t)0x03;
	if(cnt==0)
		return;
	for(i=0;i<cnt;i++)
	{
		if(byFiFoNo==0)
			ret=pCan->RF0R&CAN_RF0R_FMP0;
		else
			ret=pCan->RF1R&CAN_RF0R_FMP0;
		if(ret)
		{
			RcvMsg.IDE=(uint8_t)0x04&(pCan->sFIFOMailBox[byFiFoNo].RIR);
			RcvMsg.RTR=(uint8_t)0x02&(pCan->sFIFOMailBox[byFiFoNo].RIR);
			if((RcvMsg.IDE==CAN_ID_STD)||(RcvMsg.RTR==CAN_RTR_REMOTE))
			{
				//release the fifo
				pCan->RF0R|=CAN_RF0R_RFOM0;
				continue;
			}
			else
			{
				RcvMsg.ExtId=(uint32_t)0x1FFFFFFF&(pCan->sFIFOMailBox[byFiFoNo].RIR>>3);
				RcvMsg.DLC=(uint8_t)0x0F&(pCan->sFIFOMailBox[byFiFoNo].RDTR);
				//Get the FMI
				RcvMsg.FMI=(uint8_t)0xFF&(pCan->sFIFOMailBox[byFiFoNo].RDTR>>8);
				//Get the data
				for(j=0;j<4;j++)
				{
					RcvMsg.Data[j]=(uint8_t)0xFF&(pCan->sFIFOMailBox[byFiFoNo].RDLR>>(8*j));
				}
				for(j=0;j<4;j++)
				{
					RcvMsg.Data[4+j]=(uint8_t)0xFF&(pCan->sFIFOMailBox[byFiFoNo].RDHR>>(8*j));
				}

				__WriteMsgToRing(byChip,RcvMsg);
				//release the fifo
				if(byFiFoNo==0)
				    pCan->RF0R|=CAN_RF0R_RFOM0;
				else
					pCan->RF1R|=CAN_RF0R_RFOM0;
			}
		}
	}
	if(byFiFoNo==0)
	{
		//All pending mailbox has deal, clear the pending bit
	    if(pCan->RF0R&CAN_RF0R_FULL0)
			pCan->RF0R|=CAN_RF0R_FULL0;
		if(pCan->RF0R&CAN_RF0R_FOVR0)
			pCan->RF0R|=CAN_RF0R_FOVR0;
	}
	else
	{
		if(pCan->RF1R&CAN_RF0R_FULL0)
			pCan->RF1R|=CAN_RF0R_FULL0;
		if(pCan->RF1R&CAN_RF0R_FOVR0)
			pCan->RF1R|=CAN_RF0R_FOVR0;
	}
}

//----CAN总线错误相关的中断处理---------------------------------------------------------
//功能: CAN总线错误相关的中断处理
//参数: pCanPara,CAN控制器配置参数结构体指针;
//      bufid,专用Tx buffer编号
//返回: 空
//-----------------------------------------------------------------------------
static void __CAN_Err_Handle(uint8_t byChip)
{
	tagCanReg *pCan;
	vu32 byEsr;
	uint32_t byLastErrCode;
	if(byChip==0)
	{
	  pCan=(tagCanReg *)CAN1_BASE;
	}
	else if(byChip==1)
	{
	  pCan=(tagCanReg *)CAN2_BASE;
	}
	else
		return ;
	byEsr=pCan->ESR;
	//Bus off
	if(byEsr&CAN_ESR_BOFF)
	{
		gs_u64BusOffCnt[byChip]++;
		printf("Bus Off. \r\n");
		CAN_Hard_Init(byChip,gs_CANBaudRate[byChip],gs_pCanFilterPara);
		return;
	}
	if(byEsr&CAN_ESR_EPVF)
	{
		gs_u64PassiveErrCnt[byChip]++;
	}
	byLastErrCode=byEsr&(uint32_t)CAN_ESR_LEC;
	if(byLastErrCode!=0)
	{
		switch(byLastErrCode)
		{
		   case CAN_ESR_LEC_0:  //
			   gs_u64StErrCnt[byChip]++;
			   break;
		   case CAN_ESR_LEC_1:
			   gs_u64FoErrCnt[byChip]++;
			   break;
		   case (CAN_ESR_LEC_1 | CAN_ESR_LEC_0):
		       gs_u64ACKErrCnt[byChip]++;
		       break;
		   case CAN_ESR_LEC_2:
			   gs_u64DomBitErrCnt[byChip]++;
			   break;
		   case (CAN_ESR_LEC_2 | CAN_ESR_LEC_0):
		       gs_u64PasBitErrCnt[byChip]++;
		       break;
		   case (CAN_ESR_LEC_2 | CAN_ESR_LEC_1):
		       gs_u64CrcErrCnt[byChip]++;
		       break;
		   default:
			   break;
		}
	}
	//清中断
	pCan->MSR|=CAN_MSR_ERRI;


}
//----CAN中断响应函数------------------------------------------------------------
//功能: CAN中断响应函数
//参数: 无
//返回: 空
//-----------------------------------------------------------------------------
static void CAN_ISR_Handler(ptu32_t IntLine)
{
	if(IntLine==CN_INT_LINE_CAN1_RX0)
	{
		__CAN_Recv_FiFo_Handle(0,0);
	}
	if(IntLine==CN_INT_LINE_CAN1_RX1)
	{
		__CAN_Recv_FiFo_Handle(0,1);
	}
	if(IntLine==CN_INT_LINE_CAN2_RX0)
	{
		__CAN_Recv_FiFo_Handle(1,0);
	}
	if(IntLine==CN_INT_LINE_CAN2_RX1)
	{
		__CAN_Recv_FiFo_Handle(1,1);
	}
	if(IntLine==CN_INT_LINE_CAN1_SCE)
	{
		__CAN_Err_Handle(0);
	}
	if(IntLine==CN_INT_LINE_CAN2_SCE)
	{
		__CAN_Err_Handle(1);
	}
}

//----配置CAN过滤器参数---------------------------------------------------------
//功能: 配置CAN过滤器参数.
//参数: pFilterConfPara.
//返回: 成功则返回true,失败则返回false.
//-----------------------------------------------------------------------------
bool_t CAN_SetFilterConfig(CanFilterConfPara *pFilterConfPara)
{
	if(pFilterConfPara==NULL)
		return false;
	memset(gs_pCanFilterPara,pFilterConfPara,sizeof(CanFilterConfPara));
	return true;
}

//----监测CAN通信数据收发情况---------------------------------------------------------
//功能: 监测CAN通信数据收发情况，当2s中CAN控制器收或者发报文数没有变化时，即可认为CAN
//控制器出现故障，此时需要将CAN控制器进行复位操作.
//参数: 无.
//返回: 无.
//-----------------------------------------------------------------------------
ptu32_t __Can_Monitor(void)
{
   s64 sndcnt[4]={0,0,0,0};
   s64 rcvcnt[4]={0,0,0,0};
   uint8_t i;
   bool_t bQiYongFlag[4]={false,false,false,false};
   bool_t bRstFlag[4]={false,false,false,false};

   while(1)
   {
	   for(i=0;i<CN_CAN_NUM;i++)
	   {
		   bRstFlag[i]=false;
		   sndcnt[i]=gs_u64HardSndCnt[i];
		   rcvcnt[i]=gs_u64HardRcvCnt[i];
		   if((sndcnt[i]!=0)||(rcvcnt[i]!=0))
		   {
			  bQiYongFlag[i]=true;  //表示该CAN总线被启动了，没有启动的总线无需监测。
		   }
	   }
	   Djy_EventDelay(CN_CAN_DEV_MONITOR_TIME);
	   for(i=0;i<CN_CAN_NUM;i++)
	   {
		   if(bQiYongFlag[i])
		   {
			 if(sndcnt[i]==gs_u64HardSndCnt[i])
			 {
				 gs_TxErrRstCnt[i]++;
				 CAN_Hard_Init(i,gs_CANBaudRate[i],NULL);
				 bRstFlag[i]=true;
			 }
			 if(!bRstFlag[i])
			 {
				 if( rcvcnt[i]==gs_u64HardRcvCnt[i])
				 {
					 gs_RxErrRstCnt[i]++;
					 CAN_Hard_Init(i,gs_CANBaudRate[i],NULL);
				 }
			  }
		   }
	   }
   }

   return 1;
}

//----初始化CAN配置参数---------------------------------------------------------
//功能: 初始化CAN配置参数
//参数: pCanPara,CAN控制器配置参数结构体指针
//返回: 成功初始化则返回true,否则则返回false.
//-----------------------------------------------------------------------------
bool_t CAN_Main(void)
{
	uint16_t evtt;
	uint8_t i;
	bool_t ret;
	CanFilterConfPara *pFilterConfPara;
	pFilterConfPara=malloc(sizeof(CanFilterConfPara));
	if(pFilterConfPara==NULL)
	{
		printf("Can Init failed.\r\n");
		return false;
	}
	pFilterConfPara->FilterNumber=0;
	pFilterConfPara->FilterMode=CAN_FILTERMODE_IDMASK;
	pFilterConfPara->FilterScale=CAN_FILTERSCALE_32BIT;
	pFilterConfPara->FilterIdHigh=0x0000;
	pFilterConfPara->FilterIdLow=0x0000;
	pFilterConfPara->FilterMaskIdHigh=0x0000;
	pFilterConfPara->FilterMaskIdLow=0x0000;
	pFilterConfPara->FilterFIFOAssignment=0;
	pFilterConfPara->FilterActivation=1;
	pFilterConfPara->BankNumber=14;

	for(i=0;i<CN_CAN_NUM;i++)
	{
		ret=CAN_Hard_Init(i,125,pFilterConfPara);
		if(!ret)
		{
			printf("Can Init failed.\r\n");
			return false;
		}
	}
	CAN_Shell_Module_Install();
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

    printf("CAN install OK.\r\n");
    return true;
}
