#include	"IOT.h"
#include 	"stm32f4xx.h"

/*=====================================================================*/
// frame header
#define	MA24126_HD1			    (0x4C)
#define	MA24126_HD2			    (0x4E)

// frame length(bytes)
#define	MA24126_PKTLEN		    (16)

// command type
#define WR_MODE                 (0x00)
#define TR_VOICE                (0x01)
#define SET_VOL                 (0x03)
#define REPORT                  (0x04)

// response type
#define RPT_STARTUP             (0x00)
#define RPT_ACK_WRMODE          (0x01)
#define RPT_ACK_SETVOL          (0x02)

/*=====================================================================*/

static struct tagDjyDevice *hdevUART_MA2400=NULL;

/*=====================================================================*/

void IOT_VoicePowerOn(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);	//先关一下电源，保证可靠复位。
	Djy_EventDelay(200*mS);

	//GPIO_SetBits(GPIOB,GPIO_Pin_9);	//模拟输入低增益
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);	//模拟输入高增益

	GPIO_SetBits(GPIOB,GPIO_Pin_8);		//ENC_EN
//	GPIO_ResetBits(GPIOB,GPIO_Pin_8);	//ENC_DI

	GPIO_SetBits(GPIOB,GPIO_Pin_0);		//打开电源控制

	Djy_EventDelay(100*mS);

#if 1
	MA2400_SetWorkingMode(0);

	MA2400_SetBaudrate();
#endif

#if 1
	MA2400_SetEncoderEnable(1);
#endif

#if 1
	MA2400_SetDecodeEnable(1);
#endif

#if 0
	MA2400_SetChecksumEnable(1);
	MA2400_SetVolume();
#endif

}

void IOT_VoicePowerOff(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);	//关闭电源控制
}

void IOT_VoiceEncodeEnable(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
}

void IOT_VoiceEncodeDisable(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
}

u8 IOT_GetVoiceVoltage(void)
{
	int adc_val;
	float vol;

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles );
	ADC_SoftwareStartConv(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	adc_val= ADC_GetConversionValue(ADC1);

	vol =(float)adc_val*3.0*1000/4096;

	vol /= VOICE_VOL_STEP;
	if(vol>255)
	{
		vol=255;
	}

	return (u8)vol;
}

/*=====================================================================*/

int MA2400_Send(const u8 *buf,u32 size)
{
	int res;
	res=Driver_WriteDevice(hdevUART_MA2400,(u8*)buf,size,0,CN_BLOCK_COMPLETE,1000*mS);

	return res;

}

int MA2400_Receive(u8 *buf,u32 size,u32 wait_ms)
{
	int res;
	res=Driver_ReadDevice(hdevUART_MA2400,buf,size,0,wait_ms*mS);

	return res;

}

void uart_get_frame(unsigned char *p_buf)
{
    char find_header, find_header_2, cnt, ch;

    find_header = 0;
    find_header_2 = 0;
    cnt = 0;
    while(1)
    {
        if (MA2400_Receive(&ch,1,10*mS)==1)
        {
            if (ch == 0x4c)
            {
                find_header = 1;
                cnt = 0;
                p_buf[cnt++] = ch;
                continue;
            }

            if (find_header == 1)
            {
                if ((ch == 0x4e) && (cnt == 1))
                {
                    find_header_2 = 1;
                    find_header = 0;
                    p_buf[cnt++] = ch;
                    continue;
                }
                else
                {
                    cnt = 0;
                    find_header = 0;
                }
            }

            if (find_header_2 == 1)
            {
                p_buf[cnt++] = ch;
            }

            if (cnt == 16)
            {
                cnt = 0;
                find_header_2 = 0;
                break;
            }
        }
    }
}

static u8 cal_chksum(u8 *p_buf, int len)
{
	int i, tmp;

	tmp = 0;
	for (i=0; i<len; i++)
	{
		tmp += p_buf[i];
	}
	return tmp;
}

void MA2400_SetWorkingMode(u8 mode)
{
	u8 cmd_buf[16];

    // WR_MODE: set working mode
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = MA24126_HD1;               // header 1
    cmd_buf[1] = MA24126_HD2;               // header 2
    cmd_buf[2] = WR_MODE;                   // cmd type: WR_MODE
    cmd_buf[3] = 11;                        // len
    cmd_buf[4+0] = (1<<1)|(1<<0);                    // cmd mask: set working mode
    cmd_buf[4+1] = 0x00;					// must be 0,2400bps.
    cmd_buf[4+2] = mode;                    // codec loopback
    cmd_buf[15] = cal_chksum(cmd_buf, 15);	// checksum
    MA2400_Send(cmd_buf, 16);
    Djy_EventDelay(100*mS);
   // uart_get_frame(cmd_buf);                // get ack
}

void MA2400_SetBaudrate(void)
{
	u8 cmd_buf[16];

    // WR_MODE: set baudrate
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = MA24126_HD1;               // header 1
    cmd_buf[1] = MA24126_HD2;               // header 2
    cmd_buf[2] = WR_MODE;                   // cmd type: WR_MODE
    cmd_buf[3] = 11;                        // len
    cmd_buf[4+0] = 0x04;                    // cmd mask: set baud rate
    cmd_buf[4+3] = 0;                       // 115200
    cmd_buf[15] = cal_chksum(cmd_buf, 15);	// checksum
    MA2400_Send(cmd_buf, 16);
    Djy_EventDelay(10*mS);

}

void MA2400_SetEncoderEnable(bool_t enable)
{
	u8 cmd_buf[16];

    // WR_MODE: set encoder enable
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = MA24126_HD1;               // header 1
    cmd_buf[1] = MA24126_HD2;               // header 2
    cmd_buf[2] = WR_MODE;                   // cmd type: WR_MODE
    cmd_buf[3] = 11;                        // len
    cmd_buf[4+0] = 0x20;                    // cmd mask: set encoder enable
    cmd_buf[4+8] = enable;                  // 1: enable, 0: disable
    cmd_buf[15] = cal_chksum(cmd_buf, 15);	// checksum
    MA2400_Send(cmd_buf, 16);
    Djy_EventDelay(100*mS);
    //uart_get_frame(cmd_buf);                // get ack

}

void MA2400_SetDecodeEnable(bool_t enable)
{
	u8 cmd_buf[16];

    // WR_MODE: set decoder enable
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = MA24126_HD1;               // header 1
    cmd_buf[1] = MA24126_HD2;               // header 2
    cmd_buf[2] = WR_MODE;                   // cmd type: WR_MODE
    cmd_buf[3] = 11;                        // len
    cmd_buf[4+0] = 0x40;                    // cmd mask: set decoder enable
    cmd_buf[4+9] = enable;                  // 1: enable, 0: disable
    cmd_buf[15] = cal_chksum(cmd_buf, 15);	// checksum
    MA2400_Send(cmd_buf, 16);
    Djy_EventDelay(100*mS);
   // uart_get_frame(cmd_buf);                // get ack

}

void MA2400_SetChecksumEnable(bool_t enable)
{
	u8 cmd_buf[16];

    // WR_MODE: set checksum enable
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = MA24126_HD1;               // header 1
    cmd_buf[1] = MA24126_HD2;               // header 2
    cmd_buf[2] = WR_MODE;                   // cmd type: WR_MODE
    cmd_buf[3] = 11;                        // len
    cmd_buf[4+0] = 0x80;                    // cmd mask: set chkecksum
    cmd_buf[4+10] = enable;                       // checksum enable
    cmd_buf[15] = cal_chksum(cmd_buf, 15);	// checksum
    MA2400_Send(cmd_buf, 16);
    Djy_EventDelay(100*mS);
   // uart_get_frame(cmd_buf);                // get ack

}

void MA2400_SetVolume(void)
{
	u8 cmd_buf[16];

    // SET_VOL:
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = MA24126_HD1;               // header 1
    cmd_buf[1] = MA24126_HD2;               // header 2
    cmd_buf[2] = SET_VOL;                   // cmd type: SET_VOL
    cmd_buf[3] = 4;                         // len
    cmd_buf[4+0] = 0x07;                    // cmd mask: set digital-output gain
    cmd_buf[4+1] = 4;                       // 模拟增益调节，0～4,增益依次增大
    cmd_buf[4+2] = 100;                   // 采集端数字增益调节，－128～127：
    cmd_buf[4+3] = 100;                      // 播放端数字增益调节，－128～127：
    cmd_buf[15] = cal_chksum(cmd_buf, 15);	// checksum
    MA2400_Send(cmd_buf, 16);
    Djy_EventDelay(100*mS);
   // uart_get_frame(cmd_buf);                // get ack

}

/*=====================================================================*/

u8	VoiceVolBuf[VOICE_CHECK_COUNT];

void	IOT_VoiceVoltageCheck_200ms(void)
{
	u8 vol;

	memmove(&VoiceVolBuf[1],VoiceVolBuf,sizeof(VoiceVolBuf)-1);

	vol =IOT_GetVoiceVoltage();
	VoiceVolBuf[0] =vol;

}

void	IOT_VoiceResetVolume(void)
{
	memcpy(VoiceVolBuf,0,sizeof(VoiceVolBuf));
}

/*=====================================================================*/

void	VoiceLock(void)
{

}

void VoiceUnlock(void)
{

}

u8 voice_rec_buf[VOICE_DATA_PKT_NUM*SIZEOF_VOICE_DATA_SLOT];
u8 voice_play_buf[VOICE_DATA_PKT_NUM*SIZEOF_VOICE_DATA_SLOT];

int voice_rec_count=0;
int voice_play_count=0;

int Voice_GetRecodeCount(void)
{
	return voice_rec_count;
}


static int Voice_Recode(u8* rec_buf,int size)
{
	int offset,cnt;
	u8 chr,buf[16];

/*

	MA2400_SetEncoderEnable(0);
	Driver_CtrlDevice(hdevUART_MA2400,CN_UART_RX_RESUME,0,0);
	Djy_EventDelay(50*mS);
	MA2400_SetEncoderEnable(1);
*/
	offset=0;
	while(offset < size)
	{
		chr=0;

		if(MA2400_Receive(&chr,1,500)==0)	break;
		if(chr==0x4C)
		{
			MA2400_Receive(&chr,1,50);
			if(chr==0x4E)
			{
				MA2400_Receive(&chr,1,50);
				if(chr==TR_VOICE)
				{
					cnt=MA2400_Receive(&buf[3],13,50);
					if(cnt>0)
					{
						buf[0] =0x4C;
						buf[1] =0x4E;
						buf[2] =TR_VOICE;

						memcpy(&rec_buf[offset],buf,16);
						offset += 16;
					}
				}
			}
		}

	}

	return offset;
}
static void Voice_Recode_Loop(void)
{
	int cnt;
	u8 chr,buf[16];

/*

	MA2400_SetEncoderEnable(0);
	Driver_CtrlDevice(hdevUART_MA2400,CN_UART_RX_RESUME,0,0);
	Djy_EventDelay(50*mS);
	MA2400_SetEncoderEnable(1);
*/
	//offset=0;
	printf("VoiceRecode Start(%d)...\r\n",voice_rec_count);

	VoiceSetStatus(VOICE_STATUS_NONE);
	while(1)
	{
		chr=0;

		if(VoiceGetStatus() != VOICE_STATUS_NONE)
		{
			break;
		}

		if(MA2400_Receive(&chr,1,200) > 0)
		if(chr==0x4C)
		{
			MA2400_Receive(&chr,1,50);
			if(chr==0x4E)
			{
				MA2400_Receive(&chr,1,50);
				if(chr==TR_VOICE)
				{
					cnt=MA2400_Receive(&buf[3],13,50);
					if(cnt>0)
					{
						buf[0] =0x4C;
						buf[1] =0x4E;
						buf[2] =TR_VOICE;

						if(voice_rec_count > (sizeof(voice_rec_buf)-16))
						{
							voice_rec_count =sizeof(voice_rec_buf)-16;
							memmove(&voice_rec_buf[0],&voice_rec_buf[16],voice_rec_count);
						}

						memcpy(&voice_rec_buf[voice_rec_count],buf,16);
						voice_rec_count += 16;
					}
				}
			}
		}

	}

	printf("VoiceRecode Abort(%d).\r\n",voice_rec_count);

}

static void VoicePlay(u8 *play_buf,u32 size)
{
	int i;

	for(i=0;i<size;i+=MA24126_PKTLEN)
	{
		MA2400_Send(&play_buf[i],MA24126_PKTLEN);
		Djy_EventDelay(22*mS);
	}

}

static void voice_power_on(void)
{
	//GPIO_SetBits(GPIOB,GPIO_Pin_9);	//模拟输入低增益
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);	//模拟输入高增益

	GPIO_SetBits(GPIOB,GPIO_Pin_8);		//ENC_EN
//	GPIO_ResetBits(GPIOB,GPIO_Pin_8);	//ENC_DI

	//IOT_VoicePowerOff();
	//Djy_EventDelay(200*mS);
	IOT_VoicePowerOn();
	Djy_EventDelay(200*mS);

#if 1
	MA2400_SetWorkingMode(0);

	MA2400_SetBaudrate();
#endif

#if 1
	MA2400_SetEncoderEnable(1);
#endif

#if 1
	MA2400_SetDecodeEnable(1);
#endif

#if 0
	MA2400_SetChecksumEnable(1);
	MA2400_SetVolume();
#endif

}




static int voice_status=VOICE_STATUS_NONE;

int VoiceGetStatus(void)
{
	Djy_EventDelay(5*mS);
	return voice_status;
}

void VoiceSetStatus(int status)
{
	voice_status =status;
}

static ptu32_t voice_thread(void *pdata)
{

	Djy_EventDelay(500*mS);

	VoiceSetStatus(VOICE_STATUS_NONE);

	//VoiceSetStatus(VOICE_STATUS_POWER_ON);
	while(1)
	{
		int sta;

		sta=VoiceGetStatus();

		switch(sta)
		{
			case VOICE_STATUS_POWER_ON:
			{
				voice_power_on();
				printf("VoicePower ON.\r\n");
				Voice_Recode_Loop();
			}
			break;

			case VOICE_STATUS_POWER_OFF:
			{
				IOT_VoicePowerOff();
				printf("VoicePower OFF.\r\n");
			}
			break;


			case VOICE_STATUS_RECODE_RESTART:
			{

				voice_rec_count=0;
				memset(voice_rec_buf,0,sizeof(voice_rec_buf));

				voice_power_on();
				Voice_Recode_Loop();

				//IOT_VoicePowerOff();
			}
			break;

			case VOICE_STATUS_PLAY:
			{
				voice_power_on();
				printf("VoicePlay Start: cnt:%d\r\n",voice_play_count);
				VoicePlay(voice_play_buf,voice_play_count);
				printf("VoicePlay End.\r\n");
				Voice_Recode_Loop();
				//IOT_VoicePowerOff();
			}
			break;

			case VOICE_STATUS_PLAY_SOUND_1:
			{
				voice_power_on();
				printf("VoicePlay Sound1...\r\n");
				//VoicePlay(test_voice_data,3200);
				VoicePlay(voice_data_1,3200);
				Voice_Recode_Loop();
				//IOT_VoicePowerOff();
			}
			break;

			default:
				break;

		}

		Djy_EventDelay(100*mS);

	}

	return NULL;
}


void IOT_VoiceModuleInit(void)
{
	GPIO_InitTypeDef gpio_init;

	memset(VoiceVolBuf,0,sizeof(VoiceVolBuf));

	voice_rec_count=0;
	memset(voice_rec_buf,0,sizeof(voice_rec_buf));

	voice_play_count=0;
	memset(voice_play_buf,0,sizeof(voice_play_buf));

	////电源使能 -> PB0(0:Disable; 1:Enable)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB,&gpio_init);

	////编码器使能 -> PB8(0:Disable; 1:Enable)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB,&gpio_init);

	////模拟输入增益选择 -> PB9 (0:高增益;1:低增益)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB,&gpio_init);

	////话筒音量测量 -> PC0(ADC123_IN10)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_init.GPIO_Mode = GPIO_Mode_AN;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOC,&gpio_init);

	////通讯口 -> USART3(PB10,PB11)
	if(1)
	{
		u32 dev;

		dev = Driver_FindDevice("UART3");
		hdevUART_MA2400 = Driver_OpenDeviceAlias(dev,O_RDWR,0);
		if(hdevUART_MA2400 != NULL)
		{
			Driver_CtrlDevice(hdevUART_MA2400,CN_UART_START,0,0);

			Driver_CtrlDevice(hdevUART_MA2400,CN_UART_DMA_UNUSED,0,0);

			Driver_CtrlDevice(hdevUART_MA2400,CN_UART_DIS_RTS,0,0);
			Driver_CtrlDevice(hdevUART_MA2400,CN_UART_DIS_CTS,0,0);

			//设置串口波特率为115200，
			Driver_CtrlDevice(hdevUART_MA2400,CN_UART_SET_BAUD,115200,0);

			//Driver_CloseDevice(hdevUART_MA2400);
		}
	}

	GPIO_SetBits(GPIOB,GPIO_Pin_9);	//模拟输入低增益
	//GPIO_ResetBits(GPIOB,GPIO_Pin_9);	//模拟输入高增益

	IOT_VoicePowerOff();

	if(1)
	{
		u16 evt;

		evt = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
				voice_thread,NULL,4096,"voice thread");
		Djy_EventPop(evt,NULL,0,NULL,0,200);

	}


}
/*=====================================================================*/
