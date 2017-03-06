#include    "IOT.h"
#include     "stm32f4xx.h"

/*=====================================================================*/
static struct tagDjyDevice *hdevUART_SX1278=NULL;

#pragma pack(4)
static u8 sx1278_buf[256];
#pragma pack()


void    SX1278_Reset(void)
{
	/*
    GPIO_ResetBits(GPIOB,GPIO_Pin_13);
    Djy_EventDelay(100*mS);
    GPIO_SetBits(GPIOB,GPIO_Pin_13);
    */
}

/*=====================================================================*/

int    SX1278_Send(const u8 *buf,u32 size)
{
    int res;
    res=Driver_WriteDevice(hdevUART_SX1278,buf,size,0,CN_BLOCK_COMPLETE,5000*mS);

    return res;
}

int    SX1278_Receive(u8 *buf,u32 size,u32 wait_ms)
{
    int res;
    res=Driver_ReadDevice(hdevUART_SX1278,buf,size,0,wait_ms*mS);

    return res;
}
/*=====================================================================*/

void    SX1278_SendWakeupCode(void)
{
    const u8 code[]={0xA3,0xF0,0xFE,0x00,0x91,0x00};    //SX1278 发送唤醒帧的命令代码.
    int i;
    SX1278_Send(code,6);

    i=22; //唤醒帧要发送20S,至少要等待大于这个时间才返回．//TODO
    while(i-->0)
    {
        Djy_EventDelay(1000*mS);
    }

    //GPIO_SetBits(GPIOB,GPIO_Pin_12);
}

/*=====================================================================*/

void    SX1278_EnterSleep(void)
{
    const u8 code={0xA2,0xF0,0xFE,0x00,0x90,0x00};    //SX1278 进入休眠的命令代码.

    SX1278_Send(code,6);
    Djy_EventDelay(100*mS);

    //GPIO_SetBits(GPIOB,GPIO_Pin_12);
}

/*=====================================================================*/

void    SX1278_ExitSleep(void)
{
    //GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

/*=====================================================================*/

typedef    struct{
    u8 hd1;
    u8 hd2;
    u8 id1;
    u8 id2;
    u8 hd3;
    u8 cmd1;
    u8 cmd2;
    u8 len;
}SX1278_HDR;

void SX1278_WriteCfg(const u8 *cfg_dat)
{
    int i;
    u8 buf[32],sum;


    buf[0] =0xAF; //HD1
    buf[1] =0xAF; //HD2
    buf[2] =0x00; //ID1
    buf[3] =0x00; //ID2
    buf[4] =0xAF; //HD3
    buf[5] =0x80; //CMD1
    buf[6] =0x01; //CMD2:写参数命令码
    buf[7] =12;   //LEN
    memcpy(&buf[8],cfg_dat,11); //DATA.

    sum =0;
    for(i=0;i<19;i++) //计算数据CS
    {
        sum += buf[i];
    }
    buf[19] =sum;
    buf[20] =0x0D;
    buf[21] =0x0A;

    SX1278_Send(buf,22);

}

bool_t SX1278_ReadCfg(u8 *cfg_buf)
{
    int i;
    u8 buf[32],sum;

    memset(buf,0,sizeof(buf));

    buf[0] =0xAF; //HD1
    buf[1] =0xAF; //HD2
    buf[2] =0x00; //ID1
    buf[3] =0x00; //ID2
    buf[4] =0xAF; //HD3
    buf[5] =0x80; //CMD1
    buf[6] =0x02; //CMD2:读参数命令码
    buf[7] =12;      //LEN

    sum =0;        //计算数据CS
    for(i=0;i<19;i++)
    {
        sum += buf[i];
    }
    buf[19] =sum;
    buf[20] =0x0D;
    buf[21] =0x0A;

    SX1278_Send(buf,22);
    Djy_EventDelay(50*mS);

    i=SX1278_Receive(buf,22,500);    //正常情况下,模块在500ms之内是可以响应的.
    if(i==22)
    {
        memcpy(cfg_buf,&buf[8],12);
        return true;
    }
    return false;

}


void SX1278_EnterNormalMode(void)
{
    SX1278_HDR *hdr;

    hdr =(SX1278_HDR*)sx1278_buf;

    hdr->hd1 =0xAF;
    hdr->hd2 =0xAF;
    hdr->id1 =0x00;
    hdr->id2 =0x00;
    hdr->hd3 =0xAF;
    hdr->cmd1 =0x80;
    hdr->cmd2 =0x03;

    hdr->len =2;

    sx1278_buf[9] =0x00;
    sx1278_buf[10] =0x92; //CS
    sx1278_buf[11] =0x0D;
    sx1278_buf[12] =0x0A;

    SX1278_Send(sx1278_buf,13);
}

void SX1278_EnterCenterMode(void)
{
    SX1278_HDR *hdr;

    hdr =(SX1278_HDR*)sx1278_buf;

    hdr->hd1 =0xAF;
    hdr->hd2 =0xAF;
    hdr->id1 =0x00;
    hdr->id2 =0x00;
    hdr->hd3 =0xAF;
    hdr->cmd1 =0x80;
    hdr->cmd2 =0x04;

    hdr->len =2;

    sx1278_buf[9] =0x00;
    sx1278_buf[10] =0x93; //CS
    sx1278_buf[11] =0x0D;
    sx1278_buf[12] =0x0A;

    SX1278_Send(sx1278_buf,13);
}

bool_t SX1278_SetNetId(u8 net_id)
{
    u8 buf[8];
    int i;
    u8 cs;

    buf[0] =0xA4;
    buf[1] =0xF0;
    buf[2] =0x81;
    buf[3] =0x01;
    buf[4] =net_id;

    cs=0;
    for(i=0;i<5;i++)
    {
        cs+=buf[i];
    }

    buf[5] =cs;
    buf[6] =0x00;

    SX1278_Send(buf,7);
    Djy_EventDelay(100*mS);

    return true;
}

bool_t xxSX1278_SetId(u8 net_id,u8 user_id_h,u8 user_id_l)
{
    return false;

    /*
    u8 buf[12];


    if(SX1278_ReadCfg(buf))
    {
        buf[7] =user_id_h;    //userID_H
        buf[8] =user_id_l;    //userID_L
        buf[9] =net_id;        //netID

    //    SX1278_WriteCfg(buf);    //for test

        Djy_EventDelay(100*mS);
        memset(buf,0,sizeof(buf));
        SX1278_ReadCfg(buf);

        return true;
    }
    return false;
*/
}

bool_t SX1278_GetId(u8 *net_id,u8 *user_id_h,u8 *user_id_l)
{
    u8 buf[12];

    if(SX1278_ReadCfg(buf))
    {
        *user_id_h =buf[7];    //userID_H
        *user_id_l =buf[8]; //userID_L
        *net_id =buf[9]; //netID

        return true;
    }
    return false;
}

/*=====================================================================*/

void IOT_SX1278_ModuleInit(void)
{
    GPIO_InitTypeDef gpio_init;

    ////休眠控制 -> PB12(0:正常工作; 1:休眠模式)
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB,&gpio_init);

    ////复位控制 -> PB13(0:复位; 1:正常工作)
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_init.GPIO_Mode = GPIO_Mode_OUT;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB,&gpio_init);

    ////通讯口 -> USART2(PA2,PA3)
    if(1)
    {
        u32 dev;

        dev = Driver_FindDevice("UART2");
        hdevUART_SX1278 = Driver_OpenDeviceAlias(dev,O_RDWR,0);
        if(hdevUART_SX1278 != NULL)
        {
            Driver_CtrlDevice(hdevUART_SX1278,CN_UART_START,0,0);

            Driver_CtrlDevice(hdevUART_SX1278,CN_UART_DMA_UNUSED,0,0);

            Driver_CtrlDevice(hdevUART_SX1278,CN_UART_DIS_RTS,0,0);
            Driver_CtrlDevice(hdevUART_SX1278,CN_UART_DIS_CTS,0,0);

            //设置串口波特率为115200，
            Driver_CtrlDevice(hdevUART_SX1278,CN_UART_SET_BAUD,115200,0);

            //Driver_CloseDevice(hdevUART_SX1278);
        }
    }

    SX1278_Reset();
    SX1278_ExitSleep();
}

/*=====================================================================*/
