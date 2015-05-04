#include "os.h"
#include "cpu_peri.h"
#include "string.h"
//#include "translate.h"

//#include "iic.h"
//#include "spi.h"


//static const Pin led_out_pin[]={
//        {GPIO_PORT_A,GPIO_PIN_30,GPIO_OUTPUT_1,GPIO_DEFAULT},
//        {GPIO_PORT_A,GPIO_PIN_31,GPIO_OUTPUT_1,GPIO_DEFAULT}};

//ptu32_t Led1Task(void)
//{
//    while(1)
//    {
//        GPIO_SettoHigh(&led_out_pin[0]);
//        Djy_DelayUs(1000000);
//        GPIO_SettoLow(&led_out_pin[0]);
//        Djy_DelayUs(1000000);
//    }
//}
//ptu32_t Led2Task(void)
//{
//    while(1)
//    {
//        GPIO_SettoHigh(&led_out_pin[1]);
//        Djy_DelayUs(1000000);
//        GPIO_SettoLow(&led_out_pin[1]);
//        Djy_DelayUs(1000000);
//    }
//}
//void create_led_task(void)
//{
//	uint16_t evtt_led;
//	evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
//                                Led1Task,0x100,"led1 blink");
//
//    Djy_EventPop(evtt_led,NULL,0,NULL,0,0,0);
//    evtt_led = Djy_EvttRegist(EN_CORRELATIVE,CN_PRIO_RRS,0,0,2,
//                                Led2Task,0x100,"led2 blink");
//
//    Djy_EventPop(evtt_led,NULL,0,NULL,0,0,0);
//}
//void led_test(void)
//{
//    GPIO_CfgPinFunc(led_out_pin,PIO_LISTSIZE(led_out_pin));
//	create_led_task();
//}
/*
void iic_test(void)
{
    djy_handle_t iic0_lhdl;
    uint8_t iic_wr[128],iic_rd[128];
    uint8_t loop = 0;
    struct iic_param para;

    for(loop = 0;loop < 128; loop ++)
    {
        iic_wr[loop] = loop;
        iic_rd[loop] = 0x64;
    }
    para.data_len = 30;
    para.device_addr = 0x50;
    para.memory_addr = 0x30;
    para.mem_addr_len = 2;

    iic0_lhdl = dev_open_left("iic0", cn_timeout_forever);

    dev_write_left(iic0_lhdl, (ptu32_t*)iic_wr, (ptu32_t)&para, cn_timeout_forever);

    Djy_DelayUs(10*mS);
    dev_read_left(iic0_lhdl, (ptu32_t*)iic_rd, (ptu32_t)&para, cn_timeout_forever);
    Djy_DelayUs(10*mS);
    para.memory_addr = 0x20;
    dev_read_left(iic0_lhdl, (ptu32_t*)iic_rd, (ptu32_t)&para, cn_timeout_forever);

}*/
/*
void spi_test(void)
{
    djy_handle_t spi_lhdl;
    uint8_t spi_wr[8],spi_rd[8],cmd;
    struct spi_param para;

	spi_wr[0] = 0;	//ms
	spi_wr[1] = 0;	//second
	spi_wr[2] = 38;	//minute
	spi_wr[3] = 16;	//hour
	spi_wr[4] = 3;	//byWeek
	spi_wr[5] = 12; //day
	spi_wr[6] = 3;	//month
	spi_wr[7] = 13;	//year

    cmd = 0x88;//写掉电时间
	para.spi_cs = 0;
	para.cmd_len = 1;
	para.data_len = 8;
	para.cmd[0] = cmd;//写时间

    spi_lhdl = dev_open_left("spi",cn_timeout_forever);

//    dev_write_left(spi_lhdl, (ptu32_t*)spi_wr, (ptu32_t *)&para, cn_timeout_forever);

    Djy_DelayUs(10*mS);

    para.cmd[0] = 0x00;//读时间
    para.data_len = 8;
    dev_read_left(spi_lhdl, (ptu32_t*)spi_rd, (ptu32_t *)&para, cn_timeout_forever);

}
*/
void test_main(void)
{
    led_test();
//    iic_test();
//    spi_test();

}
void djy_main(void)
{
//    test_main();
    for ( ; ; )
    {
        Djy_EventDelay(1000*mS);
    }
}
uint8_t Flash_EraseAddr(u32 addr, u32 len)
{
	return 0;
}
uint32_t Flash_ReadData(uint32_t addr, uint8_t *buf, uint32_t len)
{
	return 0;
}
uint32_t Flash_WriteData(uint32_t addr, uint8_t *buf, uint32_t len)
{
	return 0;
}

bool_t Flash_OpetionIsOK(void)
{
	return true;
}

