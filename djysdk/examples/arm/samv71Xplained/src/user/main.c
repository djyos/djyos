#include "os.h"
#include "cpu_peri.h"
#include "string.h"

static const Pin led_out_pin[]={
        {PIO_PA23, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT},
        {PIO_PC9, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}};

ptu32_t Led1Task(void)
{
    while(1)
    {
        PIO_Set(&led_out_pin[0]);
        Djy_EventDelay(500000);
        PIO_Clear(&led_out_pin[0]);
        Djy_EventDelay(500000);
    }
}
ptu32_t Led2Task(void)
{
    while(1)
    {
        PIO_Set(&led_out_pin[1]);
        Djy_EventDelay(750000);
        PIO_Clear(&led_out_pin[1]);
        Djy_EventDelay(750000);
    }
}
void LedTaskCreate(void)
{
    uint16_t evtt_led;
    PIO_Configure(led_out_pin,PIO_LISTSIZE(led_out_pin));
    evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                Led1Task,NULL,0x100,"led1 blink");

    Djy_EventPop(evtt_led,NULL,0,0,0,0);
    evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                Led2Task,NULL,0x100,"led2 blink");

    Djy_EventPop(evtt_led,NULL,0,0,0,0);
}

void djy_main(void)
{
	LedTaskCreate();
    for ( ; ; )
    {
        Djy_EventDelay(1000*mS);
    }
}


