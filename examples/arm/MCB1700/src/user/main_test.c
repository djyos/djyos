#if 1
#include "djyos.h"
#include "gkernel.h"
#include "cpu_peri.h"
#include "printf.h"
//#include "lcd.h"

#if 0
u8 sendbuf[128];
ptu32_t EnetTest(void)
{
    u32 i;
    for(i = 0; i < 128; i ++)
    {
        sendbuf[i] = i;
    }
    while(1)
    {
        Enet_SendPacket(sendbuf,128);
        Djy_EventDelay(1000*mS);
//      Enet_RecvPacket(sendbuf);
//      Djy_EventDelay(1000*mS);
    }
    return 0;
}
void Enet_Task(void)
{
    u16 enettask;
    ModuleInstall_Enet(0);
    enettask = Djy_EvttRegist(EN_CORRELATIVE,180,0,0,2,
                                    EnetTest,0x100,"ENET TASK");
    Djy_EventPop(enettask,NULL,0,NULL,0,0,200);
}

struct stest
{
    u16 A:8;
    u16 B:8;
};
void djy_main(void)
{
    u8 flag =0;
    djy_printk("Enter djy_main()!\r\n");
    Led_Task();
//  structtest();
//  Enet_Task();
//  Lcd_Task();
//  if(flag)
//      GLCD_Task();
//  Timer_Test();
    while(1)
    {
        Djy_EventDelay(500*mS);
    }

}

#endif

#include "timer.h"
tagTimerSoft* timerhl;
void test_timer_isr(void)
{
    printk("timer test\r\n");
}
void test_timer_isr0(void)
{
    printk("timer test0\r\n");
}
void Timer_Test(void)
{
    timerhl = TimerSoft_Create("testtimer",2000*mS,test_timer_isr);
    TimerSoft_Ctrl(timerhl,EN_TIMER_STARTCOUNT,0);
    timerhl = TimerSoft_Create("testtimer0",3000*mS,test_timer_isr0);
    TimerSoft_Ctrl(timerhl,EN_TIMER_STARTCOUNT,0);
}

const unsigned long led_mask[] = { 1UL<<28, 1UL<<29, 1UL<<31, 1UL<< 2,
                                   1UL<< 3, 1UL<< 4, 1UL<< 5, 1UL<< 6 };

u16 ledofftask;
u16 ledontask;
u8 flag = 0;
void LED_Init (void)
{
    LPC_SC->PCONP     |= (1 << 15);            // enable power to GPIO & IOCON

    LPC_GPIO1->FIODIR |= 0xB0000000;           // LEDs on PORT1 are output
    LPC_GPIO2->FIODIR |= 0x0000007C;           // LEDs on PORT2 are output
}

/*----------------------------------------------------------------------------
  Function that turns on requested LED
 *----------------------------------------------------------------------------*/
void LED_On (unsigned int num)
{
  if (num < 3) LPC_GPIO1->FIOPIN |=  led_mask[num];
  else         LPC_GPIO2->FIOPIN |=  led_mask[num];
}

/*----------------------------------------------------------------------------
  Function that turns off requested LED
 *----------------------------------------------------------------------------*/
void LED_Off (unsigned int num)
{
  if (num < 3) LPC_GPIO1->FIOPIN &= ~led_mask[num];
  else         LPC_GPIO2->FIOPIN &= ~led_mask[num];
}

ptu32_t LedAllOn(void)
{
    u8 i;
    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            LED_On(i);
            Djy_DelayUs(100*mS);
        }
        Djy_EventPop(ledofftask,NULL,0,NULL,0,200);
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
    return 0;
}
ptu32_t LedAllOff(void)
{
    u8 i;
    while(1)
    {
        for(i = 0; i < 8; i++)
        {
            LED_Off(i);
            Djy_DelayUs(100*mS);
        }
        Djy_EventPop(ledontask,NULL,0,NULL,0,200);
        Djy_WaitEvttPop(Djy_MyEvttId(),NULL,CN_TIMEOUT_FOREVER);
    }
    return 0;
}

void Led_Task(void)
{
    LED_Init();

    ledontask = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                    LedAllOn,NULL,0x30,"LedAllOn function");
    if(ledontask == CN_EVTT_ID_INVALID)
    {
        printk("ledontask create failed!\r\n");
    }

    ledofftask = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                    LedAllOff,NULL,0x30,"LedAllOff function");
    if(ledofftask == CN_EVTT_ID_INVALID)
    {
        printk("ledofftask create failed!\r\n");
    }
    Djy_EventPop(ledontask,NULL,0,NULL,0,200);

}
#endif
