#include "os.h"
#include "stddef.h"
#include "cpu_peri.h"
#include "string.h"
#include "glcd.h"

static const Pin led_out_pin[]={
        {PIO_PA0,PIOA,ID_PIOA,PIO_OUTPUT_1,PIO_DEFAULT},
        {PIO_PD20,PIOD,ID_PIOD,PIO_OUTPUT_1,PIO_DEFAULT},
		{PIO_PD21,PIOD,ID_PIOD,PIO_OUTPUT_1,PIO_DEFAULT},
		{}};

ptu32_t Led1Task(void)
{
    while(1)
    {
    	PIO_Set(&led_out_pin[2]);
        Djy_DelayUs(500*mS);
        PIO_Clear(&led_out_pin[2]);
        Djy_DelayUs(500*mS);
    }
}
ptu32_t Led2Task(void)
{
    while(1)
    {
        PIO_Set(&led_out_pin[1]);
        Djy_DelayUs(500*mS);
        PIO_Clear(&led_out_pin[1]);
        Djy_DelayUs(500*mS);
    }
}
void create_led_task(void)
{
	uint16_t evtt_led;
	evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                Led1Task,NULL,0x100,"led1 blink");

	Djy_EventPop(evtt_led,NULL,0,NULL,0,0);
    evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                Led2Task,NULL,0x100,"led2 blink");

    Djy_EventPop(evtt_led,NULL,0,NULL,0,0);
}
void LED_Init(void)
{
	PMC_EnablePeripheral(ID_PIOA);
	PMC_EnablePeripheral(ID_PIOD);
	PIO_Configure(led_out_pin,PIO_LISTSIZE(led_out_pin));
	create_led_task();
}

void djy_main(void)
{
	GLCD_Init();
	LED_Init();

	extern void Sh_GetStatus(char *param);
	extern void Sh_GetRunMode(char *param);
	Sh_GetRunMode(NULL);
	Sh_GetStatus(NULL);
    while(1)
    {
    	GLCD_Clear(White);                         /* Clear graphical LCD display   */
    	Djy_DelayUs(500*mS);
    	GLCD_Clear(Blue);
    	Djy_DelayUs(500*mS);
    	GLCD_Clear(Red);
    	Djy_DelayUs(500*mS);
    }
}






