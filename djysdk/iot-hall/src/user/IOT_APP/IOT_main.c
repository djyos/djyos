
#include "IOT.h"
#include "stm32f4xx.h"
#include "cpuflash.h"
#include "os.h"

bool_t IsTransfer=FALSE;


volatile u32 UpdateFlashWriteAddr=CN_APP_START_ADDRESS;
volatile u32 UpdateFlashReadAddr=CN_APP_START_ADDRESS;
volatile u16 err_index_list[90];
volatile u8 err_cur_count=0;
volatile u16 re_ok_index_list[90];
volatile u8 re_ok_count;

u16 g_UpdatePkgNum,g_UpdatePkgIndex,reUpdatePkgIndex,g_UpdateLastPkgIndex;
u8  g_MonitorNum,g_NetId,reUpdatePkgNum,g_UpdateSuccessNum,g_UpdateFailedNum,g_UpdateLastPkgSize;
bool_t bWriteFlashErrorFlag=false;
u8 g_UpdateSuccessNetId[MONITOR_NUM_MAX];
u8 g_UpdateFailedInfor[MONITOR_NUM_MAX];
u8 reUpdateTimes;

static ptu32_t iot_update_thread(void)
{
    u32 i;
    u8 buf[12];
	printf("IOT: update thread.\r\n");
	IOT_CommonInit();
	err_cur_count=0;
	re_ok_count=0;
	for(i=0;i<90;i++)
	{
		err_index_list[i]=0xFFFF;
		re_ok_index_list[i]=0xFFFF;
	}

	for(i=0;i<MONITOR_NUM_MAX;i++)
	{
		g_UpdateSuccessNetId[i]=0;
		g_UpdateFailedInfor[i]=0;
	}

	UpdateFlashWriteAddr=CN_APP_START_ADDRESS;
	UpdateFlashReadAddr=CN_APP_START_ADDRESS;
	g_UpdatePkgNum=0;
	g_UpdatePkgIndex=0;
	g_UpdateLastPkgSize=0;
	g_MonitorNum=0;
	g_UpdateLastPkgIndex=0;
	bWriteFlashErrorFlag=false;
	g_UpdateSuccessNum=0;
	g_UpdateFailedNum=0;
	g_NetId=0;
	reUpdatePkgIndex=0;
	reUpdatePkgNum=0;
	reUpdateTimes=0;

	//获取无线模块SX1278的参数信息.
	if(SX1278_ReadCfg(buf))
	{
		printf("SX1278 OK.\r\n");
		for(i=0;i<12;i++)
		{
			printf("%02X ",(u8)buf[i]);
		}
		printf("\r\n");
		g_NetId=buf[9];
	}
	else
	{
		printf("SX1278 Error.\r\n");
	}






	while(1)
	{
		if(IsTransferBoard())
		{
           IOT_TransferUpdate();
		}
		else
		{
			IOT_MonitorUpdate();
		}
	}

	return 1;
}

/*=====================================================================*/
//IOT 主线程
static ptu32_t iot_main_thread(void *p)
{
	bool_t enter_sleep=false;

	printf("IOT:: main thread.\r\n");

	IOT_CommonInit();

	if(1)
	{
		int a;

		IOT_LED_ON();
		Djy_EventDelay(50*mS);
		IOT_LED_OFF();
		Djy_EventDelay(50*mS);


		a =IOT_GetTemperature();
		printf("IOT_TempVal: %d\r\n",a);

		a=IOT_GetVoiceVoltage();
		printf("IOT_VoiceVoltage: %dmV\r\n",a*VOICE_VOL_STEP);

		a =IOT_GetExternVoltage();
		printf("IOT_ExternVoltage: %dmV\r\n",a*BATT_VOL_STEP);


	}


	if(1)
	{
		u8 buf[12];

		if(SX1278_ReadCfg(buf))
		{
			int i;

			enter_sleep=false;

			printf("SX1278 OK.\r\n");
			for(i=0;i<12;i++)
			{
				printf("%02X ",(u8)buf[i]);
			}
			printf("\r\n");
		}
		else
		{
			enter_sleep=true;
			printf("SX1278 Error.\r\n");
		}

	}



	while(1)
	{
		if(IsTransferBoard())
		{
			IsTransfer =TRUE;
			IOT_TransferMain();		//中继端主程序
		}
		else
		{
			IsTransfer =FALSE;
			IOT_MonitorMain(enter_sleep);	//监测端主程序
		}
	}

    return 0;
}


static ptu32_t iot_wdt_thread(void *p)
{
	while(1)
	{
	   IWDG_ReloadCounter();
	   Djy_EventDelay(200*mS);
	}
	return 0;
}
/*=====================================================================*/


int IsIWDT_Reset=0;
int iSlPRST_Reset = 0;


void WDT_Init(void)
{
	iSlPRST_Reset = RCC->CSR;
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)==SET)
	{
		RCC_ClearFlag();
		IsIWDT_Reset=1;
	}
	else
	{
		IsIWDT_Reset=0;
	}
	//printf("The Reset is %8x",iSlPRST_Reset);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256); //32K/256 =6.4ms  (1/40)*256
	IWDG_SetReload(3000/8); //3S
	IWDG_ReloadCounter();
	IWDG_Enable();
}


// =============================================================================
// 函数功能:模块正常APP运行状态
// 输入参数:无
// 输出参数:无
// 返回值  :无
// =============================================================================
void IOT_Startup(void)
{
	u16 evt;

	WDT_Init();
	//创建主线程
	evt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
						iot_main_thread,NULL,8192,"iot main thread");
	Djy_EventPop(evt,NULL,0,NULL,0,CN_PRIO_RRS);

	evt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_WDT,0,0,
						iot_wdt_thread,NULL,1024,"monitor-wdt");
	Djy_EventPop(evt,NULL,0,NULL,0,CN_PRIO_WDT);
}


// =============================================================================
// 函数功能:模块升级
// 输入参数:无
// 输出参数:无
// 返回值  :无
// =============================================================================
void IOT_Update(void)
{
	u16 evt;

	WDT_Init();
	//创建升级线程
	evt=Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
			iot_update_thread,NULL,10240,"iot update thread");
    Djy_EventPop(evt,NULL,0,NULL,0,CN_PRIO_RRS);
    evt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_WDT,0,0,
    						iot_wdt_thread,NULL,1024,"iot wdt thread");
    Djy_EventPop(evt,NULL,0,NULL,0,CN_PRIO_WDT);
}
