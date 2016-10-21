#include "os.h"
#include "cpu_peri.h"
#include "string.h"
//#include "translate.h"

#include "iicbus.h"
#include "spibus.h"
#include "shell.h"

#include "timer.h"

#if 1
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
void create_led_task(void)
{
    uint16_t evtt_led;
    evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                Led1Task,NULL,0x100,"led1 blink");

    Djy_EventPop(evtt_led,NULL,0,0,0,0);
    evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                Led2Task,NULL,0x100,"led2 blink");

    Djy_EventPop(evtt_led,NULL,0,0,0,0);
}
#endif
uint32_t BOARD_SdramValidation(uint32_t baseAddr, uint32_t size)
{
    uint32_t i;
    uint32_t ret = 1;
    uint32_t *ptr32 = (uint32_t *) baseAddr;
    uint16_t *ptr16 = (uint16_t *) baseAddr;
    uint8_t *ptr8 = (uint8_t *) baseAddr;

//    SDRAM_Init();             //SDRAM初始化

    /* Test for 55AA55AA/AA55AA55 pattern */
//  printf(" Test for 55AA55AA/AA55AA55 pattern ... \n\r");
    for (i = 0; i < size ; i ++)
    {
        if (i & 1)
        {
            ptr32[i] = 0x55AA55AA ;
        } else
        {
            ptr32[i] = 0xAA55AA55 ;
        }
        __DMB();
    }
    for (i = 0; i <  size ; i++)
    {
        if (i & 1)
        {
            if (ptr32[i] != 0x55AA55AA )
            {
                printf("-E- Expected:%x, read %x @ %x \n\r" ,
                    0xAA55AA55, (unsigned)ptr32[i], (unsigned)(baseAddr + i));
                while(1);
                ret = 0;

            }
        } else
        {
            if (ptr32[i] != 0xAA55AA55 )
            {
                printf("-E- Expected:%x, read %x @ %x \n\r" ,
                        0xAA55AA55 , (unsigned)ptr32[i], (unsigned)(baseAddr + i));
                while(1);
                ret = 0;
            }
        }
    }
    if (!ret) return ret;
    printf(" Test for BYTE accessing... \n\r");
    /* Test for BYTE accessing */
    for (i = 0; i < size ; i ++)
    {
        ptr8[i] = (uint8_t)( i & 0xFF) ;
    }

    for (i = 0; i <  size ; i++)
    {
        if (ptr8[i] != (uint8_t)(i & 0xFF))
        {
            printf("-E- Expected:%x, read %x @ %x \n\r" ,
                (unsigned)(i & 0xFF), ptr8[i],(unsigned)(baseAddr + i));
            while(1);
            ret = 0;
        }
    }
    if (!ret) return ret;

    printf(" Test for WORD accessing... \n\r");
    /* Test for WORD accessing */
    for (i = 0; i < size / 2 ; i ++)
    {
        ptr16[i] = (uint16_t)( i & 0xFFFF) ;
    }

    for (i = 0; i <  size / 2 ; i++) {
        if (ptr16[i] != (uint16_t)(i & 0xFFFF))
        {
            printf("-E- Expected:%x, read %x @ %x \n\r" ,
                (unsigned)(i & 0xFFFF), ptr16[i],(unsigned)(baseAddr + i));
            while(1);
            ret = 0;
        }
    }
    if (!ret) return ret;
    printf(" Test for DWORD accessing... \n\r");
    /* Test for DWORD accessing */
    for (i = 0; i < size / 4 ; i ++)
    {
        ptr32[i] = (uint32_t)( i & 0xFFFFFFFF) ;
        __DMB();
    }

    for (i = 0; i <  size / 4 ; i++) {
        if (ptr32[i] != (uint32_t)(i & 0xFFFFFFFF))
        {
            printf("-E- Expected:%x, read %x @ %x \n\r" ,
                (unsigned)(i & 0xFFFFFFFF), (unsigned)ptr32[i], (unsigned)(baseAddr + i));
            while(1);
            ret = 0;
        }
    }
    return ret;
}
#include "iicbus.h"
static struct IIC_Device *ps_AT24_Dev = NULL;
#define AT45_ADDRESS 0x5F
u32 at45drvtest(char *param)
{
    u8 sno[16],i;
    u8 iaddr = 0x80;
    static struct IIC_Device s_AT24_Dev;

    //添加AT24到IIC0总线
    if(NULL != IIC_DevAdd_s(&s_AT24_Dev,"TWI0","IIC_Dev_AT24",
            AT45_ADDRESS,0,8))
    {
        ps_AT24_Dev = &s_AT24_Dev;
        //单线程访问配置
//      event = Djy_GetEvttId("shell");
//      IIC_BusCtrl(ps_AT24_Dev,CN_IIC_SET_BUSEVENT,event,0);

//      IIC_BusCtrl(ps_AT24_Dev,CN_IIC_SET_CLK,AT24C_CLK_FRE,0);
        IIC_BusCtrl(ps_AT24_Dev,CN_IIC_DMA_USED,0,0);
//      result = true;
        IIC_BusCtrl(ps_AT24_Dev,CN_IIC_SET_POLL,0,0);
    }

    IIC_Read(ps_AT24_Dev,iaddr,sno,16,0xffffff);

    IIC_Read(ps_AT24_Dev,0x00,sno,16,0xffffff);

    IIC_Write(ps_AT24_Dev,0x00,sno,16,1,0xffffff);

    IIC_Read(ps_AT24_Dev,0x00,sno,16,0xffffff);

    return 0;
}

struct ShellCmdTab  shell_monitordebug_table[] =
{
    {
            "dtest",
            at45drvtest,
            "AT45测试",
            NULL
    }

};
#define cn_monitor_test_num  ((sizeof(shell_monitordebug_table))/(sizeof(struct ShellCmdTab)))
static struct ShellCmdRsc tg_monitorshell_cmd_rsc[cn_monitor_test_num];

void led_test(void)
{
    PIO_Configure(led_out_pin,PIO_LISTSIZE(led_out_pin));
    create_led_task();
}

//TimerSoft TimeA,TimeB,TimeC,TimeD;
//
//void TimeA_Handler(struct TimerSoft *timer)
//{
//    TimerSoft_Ctrl(&TimeA,EN_TIMER_SOFT_PAUSE,0);
//    printk("TimerA Handler !\r\n");
//    TimerSoft_Ctrl(&TimeB,EN_TIMER_SOFT_START,0);
//}
//void TimeB_Handler(struct TimerSoft *timer)
//{
//    TimerSoft_Ctrl(&TimeB,EN_TIMER_SOFT_PAUSE,0);
//    printk("TimerB Handler !\r\n");
//    TimerSoft_Ctrl(&TimeC,EN_TIMER_SOFT_START,0);
//}
//void TimeC_Handler(struct TimerSoft *timer)
//{
//    TimerSoft_Ctrl(&TimeC,EN_TIMER_SOFT_PAUSE,0);
//    printk("TimerC Handler !\r\n");
//    TimerSoft_Ctrl(&TimeD,EN_TIMER_SOFT_START,0);
//}
//void TimeD_Handler(struct TimerSoft *timer)
//{
//    TimerSoft_Ctrl(&TimeD,EN_TIMER_SOFT_PAUSE,0);
//    printk("TimerD Handler !\r\n");
//    TimerSoft_Ctrl(&TimeA,EN_TIMER_SOFT_START,0);
//}
//void timetctest(void)
//{
//    s64 timenow;
//    timenow = DjyGetTime();
//    printk("GjyGetTime = %d s !\r\n",(u32)timenow);
//    TimerSoft_Create_s(&TimeA,"TIMEA",1000*mS,TimeA_Handler);
//    TimerSoft_Create_s(&TimeB,"TIMEB",1000*mS,TimeB_Handler);
//    TimerSoft_Create_s(&TimeC,"TIMEC",1000*mS,TimeC_Handler);
//    TimerSoft_Create_s(&TimeD,"TIMED",1000*mS,TimeD_Handler);
//    timenow = DjyGetTime();
//    printk("GjyGetTime = %d s !\r\n",(u32)timenow);
//    TimerSoft_Ctrl(&TimeA,EN_TIMER_SOFT_START,0);
//}
void test_main(void)
{
    u8 i = 0;
    double a,b,c;
    float A,B,C;
    led_test();

    Sh_InstallCmd(shell_monitordebug_table,tg_monitorshell_cmd_rsc,
            cn_monitor_test_num);

//  timetctest();
//  BOARD_SdramValidation(0x70000000,0x100000);
}

void djy_main(void)
{
    test_main();
    for ( ; ; )
    {
        Djy_EventDelay(1000*mS);

    }
}


