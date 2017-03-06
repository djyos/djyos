#include "IOT.h"
#include "cpuflash.h"
#include    "stdlib.h"
#include "verify.h"
#include "cpu_peri.h"



static struct tagDjyDevice *hdevUART_Host=NULL;

extern void reset(void);
extern s32 Ecc_Corect256(u8 *data, const u8 *old_ecc);
extern void Ecc_Make256(const u8 *data,u8 *ecc);

static	u8 cur_net_id,cur_user_id_h,cur_user_id_l;

extern volatile u32 UpdateFlashWriteAddr;
extern volatile u32 UpdateFlashReadAddr;
extern u16 g_UpdatePkgNum;
extern u16 g_UpdatePkgIndex;
extern u8  g_UpdateLastPkgSize;
extern u8  g_MonitorNum;
extern u8 g_UpdateSuccessNetId[MONITOR_NUM_MAX];
extern u8 g_UpdateFailedInfor[2*MONITOR_NUM_MAX];
extern u8 g_UpdateFailedNum;
extern u8 g_UpdateSuccessNum;

volatile u16 reSendPkgIndex[UPDATE_PKG_NUM_MAX];
volatile u16 reSendPkgCount=0;
extern u8 reUpdateTimes;


/*=====================================================================*/

static void TransferSetNetId(u8 net_id)
{
	SX1278_SetNetId(net_id);
}

/*=====================================================================*/

void	IOT_TransferInit(void)
{
	u32 dev;

	cur_net_id =0xFF;
	cur_user_id_h =0xFF;
	cur_user_id_l =0xFF;

	////通讯口(与Android中继通讯): USART3(PB10,PB11)
	dev = Driver_FindDevice("UART3");
	hdevUART_Host = Driver_OpenDeviceAlias(dev,O_RDWR,0);
	if(hdevUART_Host == NULL)
	{
		return;
	}

	Driver_CtrlDevice(hdevUART_Host,CN_UART_START,0,0);
	Driver_CtrlDevice(hdevUART_Host,CN_UART_DMA_UNUSED,0,0);
	//Driver_CtrlDevice(hdevUART_Host,CN_UART_DMA_USED,0,0);
	//设置串口波特率为115200，
	Driver_CtrlDevice(hdevUART_Host,CN_UART_SET_BAUD,115200,0);

}

static int	Transfer_ReceiveFromHost(u8 *buf,int size,u32 wait_ms)
{
	int res;
	res=Driver_ReadDevice(hdevUART_Host,buf,size,0,wait_ms*mS);

#if 0 //todo: 测试用
	{
		printf("**** RX From Host(cnt:%d):\r\n",res);
		DebugPutHex(0,res,buf);
		printf("*********************\r\n\r\n");
	}
#endif

	return res;
}

int Transfer_SendToHost(const u8 *buf,int size)
{
	int res;

#if 0 //todo:测试用
	{
		printf("**** TX To Host(cnt:%d):\r\n",size);
		DebugPutHex(0,size,buf);
		printf("*********************\r\n\r\n");
	}
#endif

	res=Driver_WriteDevice(hdevUART_Host,(u8*)buf,size,0,CN_BLOCK_COMPLETE,2000*mS);
	return res;
}

/*=====================================================================*/
//唤醒所有设备

void Transfer_WakeupAll(void)
{
	SX1278_SendWakeupCode();
}

/*=====================================================================*/

bool_t	IsHostPacketValidate(struct tagHOST_PKT_HDR *hdr)
{

	if(hdr->tag != HOST_PKT_TAG)
	{
		return false;
	}

	if(hdr->hdr_len != sizeof(struct tagHOST_PKT_HDR))
	{
		return false;
	}

	return true;
}

bool_t	IsMonitorPacketValidate(struct tagMON_PKT_HDR *hdr,u8 cmd)
{
	if(hdr->tag != MON_PKT_TAG)
	{
		return false;
	}

	if(hdr->cmd != cmd)
	{
		return false;
	}

	return true;
}

/*=====================================================================*/
#pragma	pack(4)

	u8 transfer_buf[256];
static  u8 voice_buf[VOICE_DATA_PKT_NUM*SIZEOF_VOICE_DATA_PKT];

#pragma	pack()


#define CN_RESEND_UPDATE_CMD_COUNT_MAX    5


void __TransferUpdateBin()
{
    u8 k,pkg_len,m;
    u8 buf[256];
    u32 j,ecc,temp;
    u8 ecc_code[4];
    struct tagUpdatePara *pUpdatePara=NULL;
    UpdateFlashReadAddr=CN_APP_START_ADDRESS;
    pUpdatePara=(struct tagUpdatePara *)M_Malloc(sizeof(struct tagUpdatePara),0);
   	if(pUpdatePara==NULL)
   	{
   	   return;
   	}

    //开始传输数据
    for(j=0;j<g_UpdatePkgNum;j++)
    {
	   UpdateFlashReadAddr=CN_APP_START_ADDRESS+SINGLE_UPDATE_PKG_SIZE_MAX*j;
	   if(j!=g_UpdatePkgNum-1)
	   {
		   pkg_len=SINGLE_UPDATE_PKG_SIZE_MAX;
	   }
	   else
	   {
		   pkg_len=g_UpdateLastPkgSize;
	   }
	   Flash_Read(UpdateFlashReadAddr,buf,pkg_len);
	   pUpdatePara->pkgnum=g_UpdatePkgNum;
	   pUpdatePara->index=j;

	   for(k=0;k<4;k++)
	   {
		   ecc_code[k]=0;
	   }
	   ecc=0;
	   //生成ECC码
	   for(m=0;m<256-pkg_len;m++)
	   {
		   buf[pkg_len+m]=0x00;
	   }
	   Ecc_Make256(buf,ecc_code);

	   for(k=0;k<3;k++)
	   {
		  temp=(u32)ecc_code[k];
		  temp=temp<<(8*k);
		  ecc|=temp;
	   }
	   pUpdatePara->ecc_code=ecc;

	   IOT_SendPacket(UPDATE_DATA,buf,pkg_len,pUpdatePara);

	   printf("Send %d pkg.\r\n",j);

	   Djy_EventDelay(500*mS);  //延时500ms.
   }

}


void __DealResendPkg(u8 reSendPkgCount)
{
	u32 j,m,temp;
	u16 index=0;
	u8 k;
	u8 ecc_code[4];
	u32 ecc;
	u8 buf[256];
	struct tagUpdatePara *pUpdatePara=NULL;
	pUpdatePara=(struct tagUpdatePara *)M_Malloc(sizeof(struct tagUpdatePara),0);
    if(pUpdatePara==NULL)
		   return;
    if(reSendPkgCount==0)
    	return;
    for(j=0;j<reSendPkgCount;j++)
	{
	    index=reSendPkgIndex[j];
	    UpdateFlashReadAddr=CN_APP_START_ADDRESS+SINGLE_UPDATE_PKG_SIZE_MAX*index;
	    Flash_Read(UpdateFlashReadAddr,buf,SINGLE_UPDATE_PKG_SIZE_MAX);
	    pUpdatePara->pkgnum=reSendPkgCount;
	    pUpdatePara->index=index;

		for(k=0;k<4;k++)
		{
			ecc_code[k]=0;
		}
		ecc=0;
	   //生成ECC码
	   for(m=0;m<256-SINGLE_UPDATE_PKG_SIZE_MAX;m++)
	   {
		  buf[SINGLE_UPDATE_PKG_SIZE_MAX+m]=0x00;
	   }
	   Ecc_Make256(buf,ecc_code);
       temp=0;
	   for(k=0;k<3;k++)
	   {
		  temp=(u32)ecc_code[k];
		  temp=temp<<(8*k);
		  ecc|=temp;
	   }
	   pUpdatePara->ecc_code=ecc;

	   IOT_SendPacket(UPDATE_DATA_REPEAT,buf,SINGLE_UPDATE_PKG_SIZE_MAX,pUpdatePara);

	   printf("Repeat Send %d pkg.\r\n",index);

	   Djy_EventDelay(500*mS);  //延时500ms.
	}

}


void __RecordUpdateInfor(u8 type,u8 net_id)
{
	if(type==CN_UPDATE_FAILED)
	{
	   g_UpdateFailedInfor[g_UpdateFailedNum]=net_id;
	   g_UpdateFailedNum++;
	}
	else
	{
	   g_UpdateSuccessNetId[g_UpdateSuccessNum]=net_id;
	   g_UpdateSuccessNum++;
	}
}


void __MakeSureMonitorWakeUp(u8 net_id)
{
	u8 count,rx_cnt;
	struct tagMON_PKT_HDR  *iot_hdr;
	bool_t bMonitorIsSleep=false;
	for(count=0;count<3;count++)
	{
		//发送命令给采集端.
		IOT_SendPacket(GET_STATUS,NULL,0,NULL);
		//等待采集端回复
		rx_cnt =IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT);
		if(rx_cnt > 2)
		{
			iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;
			//检测MON数据包有效性
			if(iot_hdr->tag == MON_PKT_TAG)
			{
			   //确保收到的是GET_STATUS命令的回复
			   if(iot_hdr->cmd==GET_STATUS)
			   {
				   bMonitorIsSleep=false;
				   break;
			   }
			}
		}
		else
		{
			bMonitorIsSleep=true;
		}
	}

	//如果监测端节点处于休眠状态，则唤醒节点
	if(bMonitorIsSleep)
	{
		printf("Monitor %d is Sleep.\r\n",net_id);
		//唤醒节点.
		Transfer_WakeupAll();
		printf("Wakeup Device...\r\n");
	}
	else
	{
	   printf("Monitor %d is Wakeup.\r\n",net_id);
	}
}


void __UpdateResultInfro(void)
{
	u8 i;
	//将升级结果信息告知主机
	printf("Update Success Monitor Num %d.\r\n",g_UpdateSuccessNum);
	if(g_UpdateSuccessNum!=0)
	{
	    printf("Update Success Monitor Net id:\r\n");
		for(i=0;i<g_UpdateSuccessNum;i++)
		{
			 printf("%d ",g_UpdateSuccessNetId[i]);
		}
		printf("\r\nEnd...\r\n");
	}


	printf("Update Failed Monitor Num %d.\r\n",g_UpdateFailedNum);
	if(g_UpdateFailedNum!=0)
	{
	    printf("Update Failed Monitor Net id:\r\n");
		for(i=0;i<g_UpdateFailedNum;i++)
		{
			printf("%d,Reason:%x ",g_UpdateFailedInfor[2*i],g_UpdateFailedInfor[2*i+1]);
		}
		printf("\r\nEnd...\r\n");
	}

}
// =============================================================================
// 函数功能:中继端对监测端进行升级，只能一对一升级.
// 输入参数:无.
// 输出参数:无
// 返回值  :无.
// =============================================================================
void TransferToMonitorUpdate(void)
{
    u8 i,rx_cnt,k,net_id,pkg_len,reSendUpdateCmdCount;
    u8 RecvUpdateFailedCount,RecvUpdateOKCount,RecvACKCount,RecvUpdateACKCount;
    u8 dat[180];
    u32 j;
    bool_t bRecvAckFlag,bHardFailure,bUpdateEndFlag;
    struct tagUpdatePara *pUpdatePara=NULL;
    struct tagMON_PKT_HDR  *iot_hdr;
    UpdateFlashReadAddr=CN_APP_START_ADDRESS;
    pUpdatePara=(struct tagUpdatePara *)M_Malloc(sizeof(struct tagUpdatePara),0);
	if(pUpdatePara==NULL)
	{
	   return;
	}
    for(i=0;i<g_MonitorNum;i++)
    {
    	for(j=0;j<UPDATE_PKG_NUM_MAX;j++)
    	{
    		reSendPkgIndex[j]=0;
    	}
    	reSendPkgCount=0;
    	reSendUpdateCmdCount=0;
    	RecvUpdateFailedCount=0;
    	RecvUpdateOKCount=0;
    	reUpdateTimes=0;
    	RecvACKCount=0;
    	net_id=i+1;
    	bUpdateEndFlag=false;
    	bHardFailure=false;
    	bRecvAckFlag=false;
    	RecvUpdateACKCount=0;
    	//设置中继端SX1278 ID,以建立与采集端的通讯.
    	TransferSetNetId(net_id);
        //在与节点通信之前，确保节点处于正常工作状态，而非待机状态.
    	__MakeSureMonitorWakeUp(net_id);

		while(1)
		{
			//下面这个while(1)在收到监测端发来的有效数据包时会跳出，否则一直在循环的读包。
			while(1)
			{
				if(!bRecvAckFlag)
				{
					if(reSendUpdateCmdCount<CN_RESEND_UPDATE_CMD_COUNT_MAX)
					{
					   //发送命令给采集端.
					   IOT_SendPacket(UPDATE_CMD,NULL,0,NULL);
					   printf("Send UPDATE CMD to Monitor %d.\r\n",net_id);
					   reSendUpdateCmdCount++;
					   Djy_EventDelay(500*mS);//两个UPDATE_CMD命令之间间隔1s
					}
					else
					{
						printf("Tx/Rx UPDATE CMD to Monitor %d failed,Update Failed.\r\n",net_id);
						bHardFailure=true;
						__RecordUpdateInfor(CN_UPDATE_FAILED,net_id);
						break;
					}
				 }

				rx_cnt =IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT);
				if(rx_cnt > sizeof(struct tagMON_PKT_HDR))
				{
					iot_hdr=(struct tagMON_PKT_HDR*)transfer_buf;
					//检查Monitor数据包有效性
					if(iot_hdr->tag==MON_PKT_TAG)
					{
						break;
					}
				}
			}

			if(bHardFailure)
			{
               break;
			}
			iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;

			switch(iot_hdr->cmd)
			{
			   case ACK:
			   {
				   if(RecvACKCount==0)
				   {
					   RecvACKCount++;
					   printf("Monitor %d ACK Msg Recved...\r\n",net_id);
					   bRecvAckFlag=true;
					   printf("Wait Monitor Device reset and Erase Flash.\r\n");
					   Djy_EventDelay(10000*mS);
					   //开始传输数据
					   __TransferUpdateBin();
					   bUpdateEndFlag=false;
				   }
			   break;
			   ////

               //收到监测端回复的UPDATE_ACK命令
			   case UPDATE_ACK:
			   {
				   printf("Recv UPDATE_ACK.\r\n");
                   u8 result=0;
                   pkg_len=iot_hdr->dat_len;
				   //检查一下监测端升级结果情况
				   if(pkg_len==1)  //说明是升级成功或者升级失败，需要重新传输
				   {
                       result=iot_hdr->dat[0];
                       if(result==ACK_CODE_UPDATE_SUCCESS)
                       {
                          //升级成功
                          if(RecvUpdateOKCount==0)
                          {
                        	printf("Monitor %d Update Success.\r\n",net_id);
                        	__RecordUpdateInfor(CN_UPDATE_SUCCESS,net_id);
                    	    RecvUpdateOKCount++;
                    	    bUpdateEndFlag=true;
                          }
                       }
                       else if(result==ACK_CODE_UPDATE_FAILED)
                       {
                    	  //某个监测端节点重新完整升级只进行一次，若再次升级失败(出现写Flash问题)，则认为该节点
                    	  //Flash出现硬件错误，该节点无法进行升级.
                    	  if(reUpdateTimes<=CN_REPEAT_MAX_NUM)
                    	  {
                    	     if(RecvUpdateFailedCount==0)
                    	     {
                    		     //升级失败,需要重传
                    		     __TransferUpdateBin();
                    		     RecvUpdateFailedCount++;
                    		     reUpdateTimes++;
                    	     }
                    	  }
                    	  else
                    	  {
                    		 __RecordUpdateInfor(CN_UPDATE_FAILED,net_id);
                    		 bUpdateEndFlag=true;
                    	  }

                       }
                       //直接某个节点升级失败.
                       else
                       {
                    	   __RecordUpdateInfor(CN_UPDATE_FAILED,net_id);
                    	   bUpdateEndFlag=true;
                       }

				   }

				   //收到的是丢失了某些包
				   else
				   {
					  if(reUpdateTimes<=CN_REPEAT_MAX_NUM)
					  {
						  if(RecvUpdateACKCount==0)
						  {
							  u16 index,temp0;
							  printf("Recv reSend Request.\r\n");
							  pkg_len=iot_hdr->dat_len;
							  reSendPkgCount=pkg_len/2;
							  printf("reSend Num %d.\r\n",reSendPkgCount);
							  memcpy(dat,iot_hdr->dat,pkg_len);
							  printf("reSend Pkg Index:\r\n");
							  for(k=0;k<reSendPkgCount;k++)
							  {
								  index=0;
								  temp0=0;
								  temp0=dat[2*k];
								  index|=temp0;
								  temp0=dat[2*k+1];
								  temp0=temp0<<8;
								  index|=temp0;
								  reSendPkgIndex[k]=index;
								  printf("%d ",reSendPkgIndex[k]);
							  }
							  printf("\r\n");
							  __DealResendPkg(reSendPkgCount);
							  RecvUpdateACKCount++;
							  reUpdateTimes++;
						  }

					  }
					  else
					  {
						  __RecordUpdateInfor(CN_UPDATE_FAILED,net_id);
						  bUpdateEndFlag=true;

					  }
				   }

			   }

			   break;
			   ////

			   default:
				   break;
				   ////
			   }
			}

			if(bUpdateEndFlag)
			{
				break;
			}

		}

    }

    __UpdateResultInfro();

    HandleAfterUpdate();

}

// =============================================================================
// 函数功能:中继端更新程序
// 输入参数:无.
// 输出参数:无
// 返回值  :无.
// =============================================================================
void IOT_TransferUpdate(void)
{
	struct tagHOST_PKT_HDR *host_hdr;
	struct tagMON_PKT_HDR  *iot_hdr;
	int rx_cnt,i;
	u16 dat[180];
	u8 *pdata;
	struct tagUpdatePara *pUpdatePara=NULL;
	printf(TRANSFER_MODULE_UPDATE_LOG);
	UpdateFlashWriteAddr=CN_APP_START_ADDRESS;
	IOT_TransferInit();
	pUpdatePara=(struct tagUpdatePara *)M_Malloc(sizeof(struct tagUpdatePara),0);
    if(pUpdatePara==NULL)
	      return;

	while(1)
	{
		while(1)
		{
			//等待Host端数据
			rx_cnt=Transfer_ReceiveFromHost(transfer_buf,200,500);
			if(rx_cnt >= sizeof(struct tagHOST_PKT_HDR))
			{
				host_hdr =(struct tagHOST_PKT_HDR*)transfer_buf;
				//检查Host数据包有效性
				if(IsHostPacketValidate(host_hdr))
				{
					break;
				}
			}
			else
			{

			}
		}

	    host_hdr =(struct tagHOST_PKT_HDR*)transfer_buf;
	    iot_hdr  =(struct tagMON_PKT_HDR*)host_hdr->dat;
	    //解析Host传来的命令并执行.
		switch(iot_hdr->cmd)
		{
		   case  UPDATE_DATA:
		   {
			   UpdateRecvData(CN_TRANSFER_ROLE,iot_hdr);
		   }
		  break;
		  ////

		   case UPDATE_MONITOR_NUM:
		   {
			  pdata=iot_hdr->dat;
			  g_MonitorNum=*pdata;
			  dat[0]=ACK_CODE_UPDATE_MINITOR;
			  printf("The monitor num is %d.\r\n",g_MonitorNum);
			  i=IOT_MakePacket(transfer_buf,ACK,dat,1,NULL);
			  Transfer_SendToHost(transfer_buf,i);
		   }
		   break;
		   ////

		  case UPDATE_DATA_REPEAT:
		  {

		  }
		  break;

		  ////

		  default:
			  break;

		 }

   }
}



// =============================================================================
// 函数功能:中继端更新程序
// 输入参数:无.
// 输出参数:无
// 返回值  :无.
// =============================================================================

void	IOT_TransferMain(void)	//中继端主程序
{
	struct tagHOST_PKT_HDR *host_hdr;
	struct tagMON_PKT_HDR  *iot_hdr;
	int rx_cnt,i;
	u8 net_id,usr_id_h,usr_id_l;

	printf(TRANSFER_MODULE_LOG);

	IOT_TransferInit();

	while(1)
	{
		while(1)
		{
			//等待Host端数据
			rx_cnt=Transfer_ReceiveFromHost(transfer_buf,200,500);
			if(rx_cnt >= sizeof(struct tagHOST_PKT_HDR))
			{
				host_hdr =(struct tagHOST_PKT_HDR*)transfer_buf;

				//检查Host数据包有效性
				if(IsHostPacketValidate(host_hdr))
				{
					break;
				}
			}
			else
			{
				//IDLE

			}
		}

		host_hdr =(struct tagHOST_PKT_HDR*)transfer_buf;

		net_id   =host_hdr->net_id;
		usr_id_h =host_hdr->usr_id_h;
		usr_id_l =host_hdr->usr_id_l;
		iot_hdr  =(struct tagMON_PKT_HDR*)host_hdr->dat;

		//解析Host传来的命令并执行.
		switch(iot_hdr->cmd)
		{

			case	GET_STATUS:
			{
				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				//发送命令给采集端.
				IOT_SendPacket(GET_STATUS,NULL,0,NULL);

				//等待采集端回复
				rx_cnt =IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT);
				if(rx_cnt > 2)
				{
					iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;
					//检测MON数据包有效性
					if(iot_hdr->tag == MON_PKT_TAG)
					{
						struct tagMON_STATUS_DATA *sta;
						sta =(struct tagMON_STATUS_DATA *)iot_hdr->dat;

						//将采集端数据发送给主机
						Transfer_SendToHost(transfer_buf,rx_cnt);

						printf("STATUS(%02X,%02X,%02X): ext_vol:%dmV,temp:%d,smog:%08X.\r\n",
								net_id,usr_id_h,usr_id_l,
								sta->batt_voltage*BATT_VOL_STEP,sta->temperature,sta->smog_status);
					}
				}
			}
			break;
			////

			case	GET_VOICE_VOLUME:
			{
				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				//发送命令给采集端.
				IOT_SendPacket(GET_VOICE_VOLUME,NULL,0,NULL);

				//等待采集端回复
				rx_cnt =IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT);
				if(rx_cnt > 2)
				{
					iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;
					//检测MON数据包有效性
					if(iot_hdr->tag == MON_PKT_TAG)
					{
						struct tagMON_VOICE_VOLUME_DATA *vol;
						vol =(struct tagMON_VOICE_VOLUME_DATA *)iot_hdr->dat;

						Transfer_SendToHost(transfer_buf,rx_cnt);	//将采集端数据发送给主机

						printf("VOICE_VOL(%02X,%02X,%02X): %03d,%03d,%03d,%03d,%03d,%03d,%03d,%03d...\r\n",
								net_id,usr_id_h,usr_id_l,
								vol->vol[0],vol->vol[1],vol->vol[2],vol->vol[3],vol->vol[4],vol->vol[5],vol->vol[6],vol->vol[7]);
					}
				}

			}
			break;
			////

			case	SET_VOICE_ONOFF:
			{
				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				//发送命令给采集端.
				IOT_SendPacket(SET_VOICE_ONOFF,iot_hdr->dat,iot_hdr->dat_len,NULL);

				//等待采集端回复
				rx_cnt=IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT);
				if(rx_cnt > 2)
				{
					iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;
					if(iot_hdr->tag == MON_PKT_TAG)
					{
						Transfer_SendToHost(transfer_buf,i);
					}

					printf("VOICE_ONOFF(%02X,%02X,%02X):\r\n",
													net_id,usr_id_h,usr_id_l);
				}

			}

			break;
			////

			case	RESET_VOICE_VOLUME:
			{
				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				//发送命令给采集端.
				IOT_SendPacket(RESET_VOICE_VOLUME,NULL,0,NULL);		//RESET_VOICE_VOLUME无数据参数

				//等待采集端回复
				rx_cnt=IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT);
				if(rx_cnt > 2)
				{
					iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;
					//检测MON数据包有效性
					if(iot_hdr->tag == MON_PKT_TAG)
					{
						Transfer_SendToHost(transfer_buf,rx_cnt);

						printf("RESET_VOICE_VOLUME(%02X,%02X,%02X).\r\n",
						net_id,usr_id_h,usr_id_l);
					}
				}
			}
			break;
			////

			case	SET_ENTER_SLEEP_TIME:
			{
				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				//发送命令给采集端.
				IOT_SendPacket(SET_ENTER_SLEEP_TIME,iot_hdr->dat,iot_hdr->dat_len,NULL);

				if(1)
				{
					struct tagMON_SET_ENTER_SLEEP_TIME_DATA *sta;
					sta =(struct tagMON_SET_ENTER_SLEEP_TIME_DATA *)iot_hdr->dat;

					printf("SET_ENTER_SLEEP_TIME(%02X,%02X,%02X): %dS.\r\n",
					net_id,usr_id_h,usr_id_l,
					sta->time);
				}

			}
			break;
			////
			case	VOICE_RECODE_RESTART:
			{
				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				//发送命令给采集端.
				IOT_SendPacket(VOICE_RECODE_RESTART,NULL,0,NULL);

				//等待采集端回复
				rx_cnt=IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT);
				if(rx_cnt > 2)
				{
					iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;
					//检测MON数据包有效性
					if(iot_hdr->tag == MON_PKT_TAG)
					{
						Transfer_SendToHost(transfer_buf,rx_cnt);
						printf("VOICE_RECODE_RESTART(%02X,%02X,%02X).\r\n",
									net_id,usr_id_h,usr_id_l);
					}
				}
				else
				{
					printf("VOICE_RECODE NoACK.\r\n");
				}
			}
			break;
			////

			case	GET_VOICE_RECODE:
			{
				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				//发送命令给采集端.
				IOT_SendPacket(GET_VOICE_RECODE,NULL,0,NULL);

				i=0;
				while(1)
				{
					rx_cnt =IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT);
					if(rx_cnt < sizeof(struct tagMON_PKT_HDR))
					{	//超时结束
						break;
					}

					iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;
					//检测MON数据包有效性
					if(IsMonitorPacketValidate(iot_hdr,GET_VOICE_RECODE))
					{
						struct tagVOICE_DATA *vo;
						vo =(struct tagVOICE_DATA *)iot_hdr->dat;

						//将采集端数据发送给主机
						Transfer_SendToHost(transfer_buf,rx_cnt);

						//发送ACK命令给采集端.
						IOT_SendPacket(ACK,NULL,0,NULL);

						if(vo->pkt_cur == vo->pkt_max)
						{//接收结束
							break;
						}
					}
					else
					{
						printf("GET_VOICE_RECODE pkt error!\r\n");
					}
				}

				//VoicePlay(voice_buf,i);
				printf("GET_VOICE(%02X,%02X,%02X).\r\n",
										net_id,usr_id_h,usr_id_l
										);

			}
			break;
			////

			case	VOICE_PLAY_HOST:
			{
				int i,pkt_num;

				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				printf("VOICE_PLAY_HOST: Receive from host...");
				rx_cnt =0;
				pkt_num =0;
				while(1)
				{
					//发送ACK给Host,开始接收数据
					i=IOT_MakePacket(transfer_buf,ACK,NULL,0,NULL);
					Transfer_SendToHost(transfer_buf,i);

					//丛Host端接收声音数据
					i=Transfer_ReceiveFromHost(transfer_buf,200,2000);
					if(i <= sizeof(struct tagMON_PKT_HDR))
					{
						//超时结束
						printf("host timeout.\r\n");
						break;
					}

					iot_hdr =(struct tagMON_PKT_HDR*)transfer_buf;
					//检测MON数据包有效性
					if(IsMonitorPacketValidate(iot_hdr,VOICE_PLAY_HOST))
					{
						u8 max_pkt,cur_pkt;

						max_pkt =iot_hdr->dat[0];
						cur_pkt =iot_hdr->dat[1];
						memcpy(&voice_buf[rx_cnt],&iot_hdr->dat[2],SIZEOF_VOICE_DATA_SLOT);
						rx_cnt +=SIZEOF_VOICE_DATA_SLOT;
						pkt_num++;

						if(cur_pkt == max_pkt)
						{
							//接收完成.
							break;
						}

						if(rx_cnt >= sizeof(voice_buf))
						{
							//达到最大容量值(容错处理).
							break;
						}
					}
					else
					{
						printf("pkt_error.\r\n");
					}
				}

				if(pkt_num > 0)
				{
					printf("VOICE_PLAY_HOST: Send to monitor...");
					IOT_SendPacket(VOICE_PLAY_HOST,NULL,0,NULL);	//同步

					for(i=0;i<pkt_num;i++)
					{
						struct	tagVOICE_DATA *voice_data;

						//等监测应答
						if(IOT_GetPacket(transfer_buf,SX1278_RX_TIMEOUT)<=0)
						{
							//监测没有应答.
							printf("VOICE_PLAY_HOST: Monitor NoACK!\r\n");
							break;
						}

						//封装VOICE数据包发送给监测端.
						voice_data =(struct	tagVOICE_DATA*)transfer_buf;

						voice_data->pkt_max =pkt_num-1;
						voice_data->pkt_cur =i;
						memcpy(voice_data->dat,&voice_buf[i*SIZEOF_VOICE_DATA_SLOT],SIZEOF_VOICE_DATA_SLOT);

						IOT_SendPacket(VOICE_PLAY_HOST,voice_data,sizeof(struct	tagVOICE_DATA),NULL);

						printf("VOICE_PLAY_HOST: send:%d\r\n",i);

					}

				}

				printf("VOICE_PLAY_HOST(%02X,%02X,%02X).\r\n",
										net_id,usr_id_h,usr_id_l
										);

			}
			break;
			////
			case	VOICE_PLAY_SOUND_1:
			{
				//net_id =0xFF; //使用广播消息方式

				//设置中继端SX1278 ID,以建立与采集端的通讯.
				TransferSetNetId(net_id);

				//发送命令给采集端.
				IOT_SendPacket(VOICE_PLAY_SOUND_1,NULL,0,NULL);

			}
			break;
			////

			case	WAKEUP_DEVICE:
			{
				printf("WAKEUP_DEVICE Start...\r\n");

				TransferSetNetId(net_id);
				Transfer_WakeupAll();

				//发送ACK给Host
				i=IOT_MakePacket(transfer_buf,ACK,NULL,0,NULL);
				Transfer_SendToHost(transfer_buf,i);
				printf("WAKEUP_DEVICE End.\r\n");
			}
			break;
			////

			//收到UPDATE_CMD命令，需要升级程序，这时需要重启CPU(热启动)
			case UPDATE_CMD:
			{
				volatile u8 *pAddr;
				u8 i;
				u8 dat=ACK_CODE_UPDATE_CMD;

				pAddr=(u8 *)(0x2001fff0);
				for(i=0;i<16;i++)
				{
					*(pAddr+i)=(0xA0+i);
				}

				//向主机回复一个ACK
				i=IOT_MakePacket(transfer_buf,ACK,&dat,1,NULL);
				Transfer_SendToHost(transfer_buf,i);
				printf("Transfer Need Update,Transfer will reset.\r\n");
				Djy_EventDelay(50*mS);
				reset();
			}
			break;
            ////
			default:
			{
				printf("UNKNOW IOT_CMD: %02XH.\r\n",iot_hdr->cmd);
			}
			break;
			////
		}


	}
}

/*=====================================================================*/
