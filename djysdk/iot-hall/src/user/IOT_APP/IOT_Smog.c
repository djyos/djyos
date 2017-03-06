#include	"IOT.h"
#include 	"stm32f4xx.h"

/*=====================================================================*/

static u8  TIM2CH2_CAPTURE_STA=0;	//
static u32	TIM2CH2_CAPTURE_VAL=0;	//
static u32  TIM2CH2_CAPTURE_TIMES=0;

static ptu32_t TIM2_ISR(ufast_t IniLine)
{

 	if((TIM2CH2_CAPTURE_STA&0X80)==0)//
	{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//
		{
			if(TIM2CH2_CAPTURE_STA&0X40)//
			{
				if((TIM2CH2_CAPTURE_STA&0X3F)==0X3F)//
				{
					TIM2CH2_CAPTURE_STA|=0X80;		//
					TIM2CH2_CAPTURE_VAL=0XFFFFFFFF;
				}
				else
				{
					TIM2CH2_CAPTURE_STA++;
				}
			}
		}
		if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)//
		{
			if(TIM2CH2_CAPTURE_STA&0X40)		//
			{
				TIM2CH2_CAPTURE_STA|=0X80;		//
			    TIM2CH2_CAPTURE_VAL=TIM_GetCapture2(TIM2);//
	 			TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Rising); //

	 			TIM2CH2_CAPTURE_STA =0; //??

	 			if(TIM2CH2_CAPTURE_TIMES<0x0000FFFF)
	 			{
	 				TIM2CH2_CAPTURE_TIMES++;
	 			}
			}
			else  								//
			{
				TIM2CH2_CAPTURE_STA=0;			//
				TIM2CH2_CAPTURE_VAL=0;
				TIM2CH2_CAPTURE_STA|=0X40;		//
				TIM_Cmd(TIM2,ENABLE ); 	//
	 			TIM_SetCounter(TIM2,0);
	 			TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Falling);		//
				TIM_Cmd(TIM2,ENABLE ); 	//
			}
		}
 	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC2|TIM_IT_Update); //

	return 1;
}

TIM_ICInitTypeDef  TIM2_ICInitStructure;

void TIM2_CH2_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  	//TIM2
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //PB3 -> TIM2_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //
	GPIO_Init(GPIOB,&GPIO_InitStructure); //

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2); //PB3


	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //
	TIM_TimeBaseStructure.TIM_Period=arr;   //
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);

	//
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC2S=01
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000
    TIM_ICInit(TIM2, &TIM2_ICInitStructure);

	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC2,ENABLE);//

    TIM_Cmd(TIM2,ENABLE ); 	//


    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//
	NVIC_Init(&NVIC_InitStructure);	//

    Int_Register(CN_INT_LINE_TIM2);
    Int_SetClearType(CN_INT_LINE_TIM2,CN_INT_CLEAR_AUTO);
    Int_IsrConnect(CN_INT_LINE_TIM2,TIM2_ISR);
    Int_SettoAsynSignal(CN_INT_LINE_TIM2);
    Int_ClearLine(CN_INT_LINE_TIM2);
    Int_RestoreAsynLine(CN_INT_LINE_TIM2);
}


void IOT_SmogPowerOn(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
}

void IOT_SmogPowerOff(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
}

static u32 SmogStatus=0;

bool_t	IOT_GetSmogStatus(void)
{
	return SmogStatus;
}

void IOT_SmogCheck_200ms(void)
{
	static int time_cnt=0;

	if(time_cnt >= 5000)	//5000ms检测一次烟雾
	{
		time_cnt=0;

		Int_SaveAsynLine(CN_INT_LINE_TIM2);
		SmogStatus <<= 1;
		if(TIM2CH2_CAPTURE_TIMES>0)
		{
			SmogStatus |=  1;
			TIM2CH2_CAPTURE_TIMES=0;
		}
		Int_RestoreAsynLine(CN_INT_LINE_TIM2);

		if(SmogStatus!=0)
		{
			//printf("Smog!!!\r\n");
		}

	}
	else
	{
		time_cnt+=200;
	}
}

void IOT_SmogModuleInit(void)
{
	GPIO_InitTypeDef gpio_init;

	////电源使能 -> PB4(0:Disable; 1:Enable)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOB,&gpio_init);

	////MC14468状态检测 -> PB3(TIM2_CH2)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB,&gpio_init);

	IOT_SmogPowerOn();

	TIM2_CH2_Cap_Init(0XFFFFFFFF,84-1); //


}
