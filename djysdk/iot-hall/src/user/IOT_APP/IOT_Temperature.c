#include	"IOT.h"
#include 	"stm32f4xx.h"

/*=====================================================================*/

#define R_REF      10000  		//上拉参考电阻
#define V_IN       3.0    		//模拟输入电压
#define V_REF      3.0    		//ADC参考电压
#define ADC_BITS   12     		//ADC位数

//#define NTC_ADDATA(TR) (V_IN*R_REF)*(1<<ADC_BITS)/(R_REF+TR)/V_REF
#define NTC_ADDATA(TR) (V_IN*TR)/(R_REF+TR)*(1<<ADC_BITS)/V_REF

static	const u16 NTC_adc_tbl[]=
{
 NTC_ADDATA(177000), //-30
 NTC_ADDATA(166400), //-29
 NTC_ADDATA(156600), //-28
 NTC_ADDATA(147200), //-27
 NTC_ADDATA(138500), //-26
 NTC_ADDATA(130400), //-25
 NTC_ADDATA(122900), //-24
 NTC_ADDATA(115800), //-23
 NTC_ADDATA(109100), //-22
 NTC_ADDATA(102900), //-21
 NTC_ADDATA(97120),  //-20
 NTC_ADDATA(91600),  //-19
 NTC_ADDATA(86540),  //-18
 NTC_ADDATA(81720),  //-17
 NTC_ADDATA(77220),  //-16
 NTC_ADDATA(72980),  //-15
 NTC_ADDATA(69000),  //-14
 NTC_ADDATA(65260),  //-13
 NTC_ADDATA(61760),  //-12
 NTC_ADDATA(58460),  //-11
 NTC_ADDATA(55340),  //-10
 NTC_ADDATA(52420),  //-9
 NTC_ADDATA(49660),  //-8
 NTC_ADDATA(47080),  //-7
 NTC_ADDATA(44640),  //-6
 NTC_ADDATA(42340),  //-5
 NTC_ADDATA(40160),  //-4
 NTC_ADDATA(38120),  //-3
 NTC_ADDATA(36200),  //-2
 NTC_ADDATA(34380),  //-1
 NTC_ADDATA(32660),  //0
 NTC_ADDATA(31040),  //1
 NTC_ADDATA(29500),  //2
 NTC_ADDATA(28060),  //3
 NTC_ADDATA(26680),  //4
 NTC_ADDATA(25400),  //5
 NTC_ADDATA(24180),  //6
 NTC_ADDATA(23020),  //7
 NTC_ADDATA(21920),  //8
 NTC_ADDATA(20800),  //9
 NTC_ADDATA(19900),  //10
 NTC_ADDATA(18970),  //11
 NTC_ADDATA(18290),  //12
 NTC_ADDATA(17260),  //13
 NTC_ADDATA(16460),  //14
 NTC_ADDATA(15710),  //15
 NTC_ADDATA(15000),  //16
 NTC_ADDATA(14320),  //17
 NTC_ADDATA(13680),  //18
 NTC_ADDATA(13070),  //19
 NTC_ADDATA(12490),  //20
 NTC_ADDATA(11940),  //21
 NTC_ADDATA(11420),  //22
 NTC_ADDATA(10920),  //23
 NTC_ADDATA(10450),  //24
 NTC_ADDATA(10000),  //25
 NTC_ADDATA(9574),   //26
 NTC_ADDATA(9166),   //27
 NTC_ADDATA(8778),   //28
 NTC_ADDATA(8480),   //29
 NTC_ADDATA(8058),   //30
 NTC_ADDATA(7724),   //31
 NTC_ADDATA(7404),   //32
 NTC_ADDATA(7098),   //33
 NTC_ADDATA(6808),   //34
 NTC_ADDATA(6532),   //35
 NTC_ADDATA(6268),   //36
 NTC_ADDATA(6015),   //37
 NTC_ADDATA(5776),   //38
 NTC_ADDATA(5546),   //39
 NTC_ADDATA(5326),   //40
 NTC_ADDATA(5118),   //41
 NTC_ADDATA(4918),   //42
 NTC_ADDATA(4726),   //43
 NTC_ADDATA(4544),   //44
 NTC_ADDATA(4368),   //45
 NTC_ADDATA(4202),   //46
 NTC_ADDATA(4042),   //47
 NTC_ADDATA(3888),   //48
 NTC_ADDATA(3742),   //49
 NTC_ADDATA(3602),   //50
 NTC_ADDATA(3468),   //51
 NTC_ADDATA(3340),   //52
 NTC_ADDATA(3216),   //53
 NTC_ADDATA(3098),   //54
 NTC_ADDATA(2986),   //55
 NTC_ADDATA(2878),   //56
 NTC_ADDATA(2774),   //57
 NTC_ADDATA(2674),   //58
 NTC_ADDATA(2580),   //59
 NTC_ADDATA(2488),   //60
 NTC_ADDATA(2400),   //61
 NTC_ADDATA(2316),   //62
 NTC_ADDATA(2234),   //63
 NTC_ADDATA(2158),   //64
 NTC_ADDATA(2082),   //65
 NTC_ADDATA(2012),   //66
 NTC_ADDATA(1942),   //67
 NTC_ADDATA(1876),   //68

 0x0000,      		 //End

};

#define	NTC_ADC_TBL_COUNT	(sizeof(NTC_adc_tbl)/sizeof(NTC_adc_tbl[0]))

s8 IOT_GetTemperature(void)
{
	int adc_val,st,end,i;

	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//TEMP Enable

	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_480Cycles );
	ADC_SoftwareStartConv(ADC1);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
	adc_val= ADC_GetConversionValue(ADC1);

	GPIO_SetBits(GPIOB,GPIO_Pin_5);		//TEMP Disable

	st=0;
	end=NTC_ADC_TBL_COUNT-1;
	while(1)
	{
		i=st+((end-st)>>1);
		if(adc_val<=NTC_adc_tbl[i])
		{
			st =i;
		}
		else
		{
			end =i;
		}

		if((end-st) <= 4 ) break;
	}

	while(st<end)
	{
		if(adc_val>=NTC_adc_tbl[st])
		{
			break;
		}
		st++;
	}

	i = -30 + st;
	return i;
}

void IOT_TemperatureModuleInit(void)
{
	GPIO_InitTypeDef gpio_init;

	////鐢垫簮鎺у埗-> PB5(0:Enable; 1:Disable)
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB,&gpio_init);

	////娓╁害妫�祴ADC绔彛 -> PC3(ADC123_IN13)
	gpio_init.GPIO_Pin  = GPIO_Pin_3;
	gpio_init.GPIO_Mode = GPIO_Mode_AN;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&gpio_init);

}

/*=====================================================================*/
//鲁玫脢录禄炉ADC
void  Adc_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//脢鹿脛脺GPIOA脢卤脰脫
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //脢鹿脛脺ADC1脢卤脰脫

  //脧脠鲁玫脢录禄炉ADC1脥篓碌脌5 IO驴脷
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA5 脥篓碌脌5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//脛拢脛芒脢盲脠毛
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//虏禄麓酶脡脧脧脗脌颅
  GPIO_Init(GPIOA, &GPIO_InitStructure);//鲁玫脢录禄炉

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1赂麓脦禄
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//赂麓脦禄陆谩脢酶


  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//露脌脕垄脛拢脢陆
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//脕陆赂枚虏脡脩霉陆脳露脦脰庐录盲碌脛脩脫鲁脵5赂枚脢卤脰脫
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA脢搂脛脺
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//脭陇路脰脝碌4路脰脝碌隆拢ADCCLK=PCLK2/4=84/4=21Mhz,ADC脢卤脰脫脳卯潞脙虏禄脪陋鲁卢鹿媒36Mhz
  ADC_CommonInit(&ADC_CommonInitStructure);//鲁玫脢录禄炉

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12脦禄脛拢脢陆
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//路脟脡篓脙猫脛拢脢陆
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//鹿脴卤脮脕卢脨酶脳陋禄禄
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//陆没脰鹿麓楼路垄录矛虏芒拢卢脢鹿脫脙脠铆录镁麓楼路垄
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//脫脪露脭脝毛
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1赂枚脳陋禄禄脭脷鹿忙脭貌脨貌脕脨脰脨 脪虏戮脥脢脟脰禄脳陋禄禄鹿忙脭貌脨貌脕脨1
  ADC_Init(ADC1, &ADC_InitStructure);//ADC鲁玫脢录禄炉


	ADC_Cmd(ADC1, ENABLE);//驴陋脝么AD脳陋禄禄脝梅

}
//禄帽碌脙ADC脰碌
//ch: @ref ADC_channels
//脥篓碌脌脰碌 0~16脠隆脰碌路露脦搂脦陋拢潞ADC_Channel_0~ADC_Channel_16
//路碌禄脴脰碌:脳陋禄禄陆谩鹿没
u16 Get_Adc(u8 ch)
{
	  	//脡猫脰脙脰赂露篓ADC碌脛鹿忙脭貌脳茅脥篓碌脌拢卢脪禄赂枚脨貌脕脨拢卢虏脡脩霉脢卤录盲
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC脥篓碌脌,480赂枚脰脺脝脷,脤谩赂脽虏脡脩霉脢卤录盲驴脡脪脭脤谩赂脽戮芦脠路露脠

	ADC_SoftwareStartConv(ADC1);		//脢鹿脛脺脰赂露篓碌脛ADC1碌脛脠铆录镁脳陋禄禄脝么露炉鹿娄脛脺

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//碌脠麓媒脳陋禄禄陆谩脢酶

	return ADC_GetConversionValue(ADC1);	//路碌禄脴脳卯陆眉脪禄麓脦ADC1鹿忙脭貌脳茅碌脛脳陋禄禄陆谩鹿没
}
