/*
 * IOT_Misc.c
 *
 *  Created on: 2015年10月19日
 *      Author: liuwei
 */


#include	"IOT.h"
#include 	"stm32f4xx.h"
#include    "cpuflash.h"
#include    "verify.h"



extern u16 g_UpdatePkgNum;
extern u16 g_UpdatePkgIndex;
extern u8  g_UpdateLastPkgSize;
extern u16 g_UpdateLastPkgIndex;
extern volatile u16 err_index_list[90];
extern volatile u8 err_cur_count;
extern volatile u32 UpdateFlashWriteAddr;
extern 	u8 transfer_buf[256];
extern bool_t bWriteFlashErrorFlag;
//extern u8 reUpdateTimes;

/*=====================================================================*/
//----配置全部IO口--------------------------------------------------------------
//功能：除必须的外，全部配置成初始态，各功能口由相应的功能模块自行定义。
//参数：无
//返回：无
//------------------------------------------------------------------------------

void HandleAfterUpdate(void)
{
    u8 i;
    u8 buf[16];
    volatile u8 *pAddr;
    //擦除RAM中升级标志
	pAddr=(u8 *)(0x2001fff0);
	for(i=0;i<16;i++)
	{
		*(pAddr+i)=0x00;
	}
	//监测端升级完毕，这时需要对Flash的最后16bytes做一个升级标签
	for(i=0;i<16;i++)
	{
		buf[i]=0xA0+i;
    }
	Flash_Write(CN_APP_END_ADDRESS-16,buf,16);
	Djy_EventDelay(200*mS);
	__disable_irq();
	__disable_fault_irq();
	Flash_Flush();
	reset();

}




//----配置全部IO口--------------------------------------------------------------
//功能：除必须的外，全部配置成初始态，各功能口由相应的功能模块自行定义。
//参数：无
//返回：1,成功接收并写入Flash一包;0,ECC校验错误;-1,发生写Flash错误;-2,升级失败，结束升级.
//------------------------------------------------------------------------------
s32 DealECCandWriteFlash(u8 *pdata,u16 pkgindex,u8 len,u32 ecc,u8 role)
{
	u8 ecc_code[3],dat[180];
	s32 result;
	u8 buf[256];
	u8 count,i,j;
	//先判断ECC校验是否正确
	memcpy(buf,pdata,len);
	count=256-len;
	for(i=0;i<count;i++)
	{
	  buf[len+i]=0x00;
	}
	ecc_code[0]=ecc;
	ecc_code[1]=ecc>>8;
	ecc_code[2]=ecc>>16;
	result=Ecc_Corect256(buf,ecc_code);
	//如果校验结果为：数据无法恢复，则需要记录下该包编号，同时该包有效数据无需写入Flash
	if(result==-1)
	{
		printf("%d pkg ecc error.\r\n",pkgindex);
		err_index_list[err_cur_count]=pkgindex;
		err_cur_count++;
		return 0;
	}
	//校验结果如果是其他三种情形：0 : 正确/1 : 数据被修正/2 : 之前效验的ecc是错误的
	//则将有效数据写进Flash相应地址
	UpdateFlashWriteAddr=CN_APP_START_ADDRESS+SINGLE_UPDATE_PKG_SIZE_MAX*pkgindex;
	Flash_Write(UpdateFlashWriteAddr,buf,len);
	result=CorrectFlashWrite(UpdateFlashWriteAddr,buf,len);
	if(!result)
	{
		  //如果发生写Flash错误，则需要擦除整片Flash，然后直接退出本次传输，并发送UPDATE_ACK
		  //命令给中继器，告知需要重新完整升级.
		   printf("%d pkg write flash error.\r\n",pkgindex);

		   if(role==CN_MONITOR_ROLE)
		   {
			   dat[0]=ACK_CODE_UPDATE_FAILED;
			   for(i=0;i<2;i++)
			   {
				  IOT_SendPacket(UPDATE_ACK,dat,1,NULL);
				  Djy_EventDelay(500*mS);
			   }
		   }
		   else
		   {
			   i=IOT_MakePacket(transfer_buf,UPDATE_ACK,dat,1,NULL);
			   Transfer_SendToHost(transfer_buf,i);
		   }

		   bWriteFlashErrorFlag=true;
		   g_UpdateLastPkgIndex=0;
		   g_UpdatePkgNum=0;
		   for(j=0;j<err_cur_count;j++)
		   {
			  err_index_list[j]=0;
		   }
		   err_cur_count=0;
		   Flash_Erase();    //擦除整片Flash.
		   return -1;
	}

	else
	{
		printf("Recv %d pkg.\r\n",pkgindex);
		return 1;
	}

}



void UpdateRecvData(u8 role,struct tagMON_PKT_HDR* iot_hdr)
{
	int i,count;
	u32 ecc=0;
	u8 dat[180];
	u8 *pdata;
	u8 len,k=0;
	static u8 flag=0;
	if(iot_hdr==NULL)
		return;
	g_UpdatePkgNum=iot_hdr->pkgnum;
	len=iot_hdr->dat_len;
	if(len>SINGLE_UPDATE_PKG_SIZE_MAX)
		 len=SINGLE_UPDATE_PKG_SIZE_MAX;
	pdata=iot_hdr->dat;
	ecc=iot_hdr->ecc_code;
	g_UpdatePkgIndex=iot_hdr->index;
	if(g_UpdatePkgIndex==0)
	{
		bWriteFlashErrorFlag=false;
	}

	if(bWriteFlashErrorFlag)
	{
	    return;
	}
	if(!flag)
	{
		printf("PkgNum:%d.\r\n",g_UpdatePkgNum);
		flag++;
	}
	//以防丢包
	if(g_UpdatePkgIndex!=g_UpdateLastPkgIndex+1)
	{
		count=g_UpdatePkgIndex-g_UpdateLastPkgIndex;
		for(i=0;i<count-1;i++)
		{
			printf("Lost %d pkg.\r\n",g_UpdateLastPkgIndex+i+1);
			err_index_list[err_cur_count]=g_UpdateLastPkgIndex+i+1;
			err_cur_count++;
		}
	}

	g_UpdateLastPkgIndex=g_UpdatePkgIndex;

	DealECCandWriteFlash(pdata,g_UpdatePkgIndex,len,ecc,role);
	//如果是接收的是最后一包数据,则中继器需要向主机回复UPDATE_ACK命令告知有
	//哪些包接收失败，需要重传
	if(g_UpdatePkgIndex==g_UpdatePkgNum-1)
	{
	   g_UpdateLastPkgSize=len;
	   //传输完全正确
	   if(err_cur_count==0)
	   {
			 dat[0]=ACK_CODE_UPDATE_SUCCESS;
			 if(role==CN_MONITOR_ROLE)
			 {
				 for(k=0;k<2;k++)
				 {
					IOT_SendPacket(UPDATE_ACK,dat,1,NULL);
					printf("Send Success ACK.\r\n");
					Djy_EventDelay(100*mS);
				 }
				 HandleAfterUpdate();
			 }
			 else
			 {
				 i=IOT_MakePacket(transfer_buf,UPDATE_ACK,dat,1,NULL);
				 Transfer_SendToHost(transfer_buf,i);
				 printf("Send Success ACK to Host.\r\n");
				 //给监测端升级
				 TransferToMonitorUpdate();
			 }
		}
		else
		{
		  //默认情况下，在一次升级过程中，出错包总数(丢包及ECC校验错误)不超过90包，若超过则需要重新开始完整传输。
		  if(err_cur_count>SINGLE_UPDATE_PKG_SIZE_MAX/2)
		  {
			  dat[0]=ACK_CODE_UPDATE_FAILED;
			  if(role==CN_MONITOR_ROLE)
			  {
				  for(i=0;i<2;i++)
				  {
					 IOT_SendPacket(UPDATE_ACK,dat,1,NULL);
					 Djy_EventDelay(500*mS);
				  }
			  }
			  else
			  {
				  i=IOT_MakePacket(transfer_buf,UPDATE_ACK,dat,1,NULL);
				  Transfer_SendToHost(transfer_buf,i);
			  }
		  }
		  else
		  {
			 printf("The need reSend pkg count %d.\r\n",err_cur_count);
			 for(k=0;k<err_cur_count;k++)
			 {
				 printf("%d ",err_index_list[k]);
			 }
			 printf("\r\n");

			 len=2*err_cur_count;
			 for(k=0;k<err_cur_count;k++)
			 {
				dat[2*k]=err_index_list[k];
				dat[2*k+1]=err_index_list[k]>>8;
			 }

			 for(count=0;count<2;count++)
			 {

				 if(role==CN_MONITOR_ROLE)
				 {
					 IOT_SendPacket(UPDATE_ACK,dat,len,NULL);
					 printf("Lost Pkg,Send UPDATE_ACK to Transfer.\r\n");
					 Djy_EventDelay(100*mS);
				 }
				 else
				 {
					 i=IOT_MakePacket(transfer_buf,UPDATE_ACK,dat,len,NULL);
					 Transfer_SendToHost(transfer_buf,i);
				 }
			 }

		  }

		}
	}

}



void DebugPutHex(u32 Offset,u32 Count,const void *Buf)
{

	u32	i;
	char *p;

	p	=(char*)Buf;
    for(i=0;i<Count;i++)
    {
    	if((i&0x0f)==0)
    	{
    		printf("\r\n%04X_%04X:  ",((i+Offset)>>16)&0xffff,((i+Offset)&0xffff));
    	}
    	printf("%02X ",p[i]);
    }
    printf("\r\n");
}

/*=====================================================================*/

bool_t	IsTransferBoard(void)
{
	//// PB15（Inpput)(0:Transfer; 1:Monitor)

	GPIO_InitTypeDef gpio_init;

	gpio_init.GPIO_OType = GPIO_OType_OD;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_IN;
	gpio_init.GPIO_Speed = GPIO_Speed_25MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB,&gpio_init);

	Djy_EventDelay(10*mS);
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)==0)
	{
		return true;
	}
	return false;
}

/*=====================================================================*/

void IOT_LED_ON(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
}

void IOT_LED_OFF(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
}



/*=====================================================================*/

u8 IOT_GetExternVoltage(void)
{
	int adc_val;
	float vol;

	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_480Cycles );
	ADC_SoftwareStartConv(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	adc_val= ADC_GetConversionValue(ADC1);

	vol =(float)adc_val*3.0*2*1000/4096;

	vol /= BATT_VOL_STEP;
	if(vol>255)
	{
		vol=255;
	}

	return (u8)vol;
}

/*=====================================================================*/

int	IOT_MakePacket(u8 *buf,u8 cmd,const void *dat,u8 len,struct tagUpdatePara *pUpdatePara)
{
	u8 i,j;

	buf[0] =MON_PKT_TAG&0xFF;
	buf[1] =(MON_PKT_TAG>>8)&0xFF;
	buf[2] =cmd;
    if(pUpdatePara==NULL)
    {
    	for(i=0;i<4;i++)
    	{
    		buf[3+i]=0;
    	}
    }
    else
    {
    	buf[3]=pUpdatePara->pkgnum;
    	buf[4]=pUpdatePara->pkgnum>>8;
    	buf[5]=pUpdatePara->index;
    	buf[6]=pUpdatePara->index>>8;
    }
    buf[7] =len;
    if((cmd==UPDATE_DATA)||(cmd==UPDATE_DATA_REPEAT))
    {
    	for(i=0;i<4;i++)
    	{
    		buf[8+i]=pUpdatePara->ecc_code>>(8*i);
    	}
    }
    else
    {
    	for(i=0;i<4;i++)
		{
			buf[8+i]=0;
		}
    }
    if(len!=0)
    {
	  memcpy(&buf[12],dat,len);
    }

    j=0;
    for(i=0;i<12+len;i++)
    {
	   j += buf[i];
    }
	i=12+len;
	buf[i]=j;    //CheckSUM8
	i++;
	buf[i]=0x00;   //End


	return 200;
}

bool_t	IOT_SendPacket(u8 cmd,const void *dat,u8 len,struct tagUpdatePara *pUpdatePara)
{
#pragma	pack(4)
	u8 buf[256];
#pragma	pack()

	IOT_MakePacket(buf,cmd,dat,len,pUpdatePara);
	SX1278_Send(buf,256);

	Djy_EventDelay(SX1278_TX_TIME*mS);
	return true;
}

int	IOT_GetPacket(u8 *buf,u32 wait_ms)
{
	int i;
	u8 chr;

	//等待数据接收
	while(1)
	{
		chr=0;
		i=SX1278_Receive(&chr,1,wait_ms);
		if(i<=0) break;
		if(chr==0xA1)
		{
			i=SX1278_Receive(&chr,1,wait_ms);
			if(i<=0) break;
			if(chr==0xF0)
			{
				i=SX1278_Receive(&buf[2],198,wait_ms);
				if(i<=0) break;
				buf[0] =0xA1;
				buf[1] =0xF0;
				return i+2;
			}
		}
	}

	return 0;
}

/*=====================================================================*/

static void ADC1_Init(void)
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

	 // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

/*
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
	  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);
	  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);


	  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	  ADC_CommonInit(&ADC_CommonInitStructure);

	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfConversion = 1;
	  ADC_Init(ADC1, &ADC_InitStructure);

	  ADC_Cmd(ADC1, ENABLE);

}


void IOT_CommonInit(void)
{
	GPIO_InitTypeDef gpio_init;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	RCC_AHB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	////PC13 (GPIO,不作为RTC_AF1)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC,&gpio_init);

#ifdef APP_PRO
	////LED -> PB14(0:ON; 1:OFF)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB,&gpio_init);
	IOT_LED_OFF();

	////外部电源电压检测ADC端口 -> PC2(ADC123_IN12)
	gpio_init.GPIO_Pin  = GPIO_Pin_2;
	gpio_init.GPIO_Mode = GPIO_Mode_AN;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&gpio_init);

	ADC1_Init(); // 公共ADC1初始化
#endif

	IOT_SX1278_ModuleInit();		//SX1278无线模块初始化

}

/*=====================================================================*/
