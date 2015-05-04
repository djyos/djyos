#include "os_inc.h"
#include "uart_tiny.h"
#include "cpu_peri.h"
void run_module_init(void);

ptu32_t flash_led(void)
{
    u32 out = 0;
    GPIO_PowerOn(0);
    GPIO_CfgPinFunc(2,0,cn_p2_0_gpio);
    GPIO_CfgPinFunc(2,1,cn_p2_1_gpio);
    GPIO_CfgPinFunc(2,2,cn_p2_2_gpio);
    GPIO_CfgPinFunc(2,3,cn_p2_3_gpio);
    gpio_setto_out(2,0x0f);                 //设置0~3位为输出口
    while(1)
    {
        out++;
        if(out == 4)
            out = 0;
        GPIO_SettoLow(2,1<<out);
        GPIO_SettoHigh(2,0xf & ~(1<<out));

        Djy_EventDelay(500*mS);
    }
}

//本应命名为main的，但调试器总是默认这是整个程序的入口点，罢了
ptu32_t djy_main(void)
{
    uint16_t evtt_flash_led;
    u32 parameter;
    u16 i;
    u16 buf[512];
    run_module_init();

    evtt_flash_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,
                                        1,flash_led,10,NULL);
    parameter = 1;
    Djy_EventPop(evtt_flash_led,NULL,0,&parameter,
                    sizeof(parameter),false,0);

    uart_set_baud(1,6000000);
//    uart0_send("djyos welcome,都江堰操作系统\n\r",30);
#if 1
    while(1)
    {
//        uart1_read((char*)buf,512,CN_TIMEOUT_FOREVER);
        for(i = 0; i<256;i++)
        {
            if(buf[i] != i)
            {
                break;
            }
        }
//        if(i == 256)
//        	uart0_send("ok\n\r",4);
//        else
//            uart0_send("err\n\r",5);
//        uart1_send("end",3);
    }
#endif
#if 1
    while(1)
    {
        for(i = 0; i<256;i++)
        {
            buf[i] = i;
        }
        uart1_send((char*)buf,512);
//        uart1_read((u8*)buf,3,CN_TIMEOUT_FOREVER);
    }

#endif
}
