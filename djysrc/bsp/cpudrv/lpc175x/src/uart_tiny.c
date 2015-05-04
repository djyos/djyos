//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块: uart驱动程序
//作者：lst
//版本：V1.0.0
//文件描述: 驱动44b0的uart
//其他说明:
//修订历史:
//1. 日期:2009-03-10
//   作者:lst
//   新版本号：1.0.0
//   修改说明: 移植字44b0的1.0.1版
//------------------------------------------------------
#include <string.h>
#include "stdint.h"
#include "cpu_peri.h"
#include "uart_tiny.h"

#if(CN_CFG_UART0_USED == 1)
static struct uart_UCB tg_uart0_cb;
uint8_t uart0_drv_send_buf[uart0_buf_len];
uint8_t uart0_drv_recv_buf[uart0_buf_len];
#endif

#if(CN_CFG_UART1_USED == 1)
static struct uart_UCB tg_uart1_cb;
uint8_t uart1_drv_send_buf[uart1_buf_len];
uint8_t uart1_drv_recv_buf[uart1_buf_len];
#endif


#if(CN_CFG_UART2_USED == 1)
static struct uart_UCB tg_uart1_cb;
uint8_t uart1_drv_send_buf[uart2_buf_len];
uint8_t uart1_drv_recv_buf[uart2_buf_len];
#endif

#if(CN_CFG_UART3_USED == 1)
static struct uart_UCB tg_uart1_cb;
uint8_t uart1_drv_send_buf[uart3_buf_len];
uint8_t uart1_drv_recv_buf[uart3_buf_len];
#endif


void __uart_pause_send_int(volatile struct st_nxp_uart_reg * const reg)
{
    reg->DI.IER &=  ~(u32)(1<<bo_uart_ier_thre);          //禁止发送中断，
}
void __uart_resume_send_int(volatile struct st_nxp_uart_reg * const reg)
{
    reg->DI.IER |=  (u32)(1<<bo_uart_ier_thre);           //使能发送中断，
}

bool_t uart_set_baud(u32 uart_no,u32 baud)
{
    struct st_nxp_uart_reg *reg;
    u32 temp,temp1;
    u32 mulval,divval;
    u32 err;
    bool_t result = false;
    switch(uart_no)
    {
        case 0 :
            reg = pg_uart0_reg;
            break;
        case 1 :
            reg = pg_uart1_reg;
            break;
        case 2 :
            reg = pg_uart2_reg;
            break;
        case 3 :
            reg = pg_uart3_reg;
            break;
        default : break;
    }
    temp = CN_CFG_PCLK/16/baud;
    temp1 = CN_CFG_PCLK/16%baud;
    if(temp1 > (baud>>1))   //整数除法的商直取整数，余数大于除数1半，须进位。
    {
        err = baud - temp1;
        temp++;
    }else
    {
        err = temp1;
    }
    //如果误差 < CN_CFG_PCLK/16>>6 = 1/64，说明不用小数波特率误差已经很小。
    //第二个条件是17xx系列uart的要求，分配系数大于2才能用小数波特率
    if((err <= (CN_CFG_PCLK/16>>6)) || (temp <= 2))
    {
        reg->LCR |= 1<<bo_uart_lcr_dlab;
        reg->RTD.DLL= temp;
        reg->DI.DLM= temp>>8;
        reg->FDR= 0;
        reg->LCR &= ~(1<<bo_uart_lcr_dlab);
        if(err > (baud>>6))
            return false;
        else
            return true;
    }

    //对小数分频值得每一个可能值计算误差，直到误差小于1/64
    for(divval = 1; divval < 15; divval++)
    {
        for(mulval = divval+1; mulval < 16; mulval++)
        {
            temp1 = CN_CFG_PCLK * mulval / 16 / (mulval + divval) % baud;
            if(temp1 > (baud >> 1))
            {
                err = baud - temp1;
            }else
            {
                err = temp1;
            }
            if(err <= (CN_CFG_PCLK/16>>6))
            {
                result = true;
                break;
            }
        }
        if(result == true)
            break;
    }
    if(result == true)
    {
        temp = (CN_CFG_PCLK * mulval / 16 / (mulval + divval) + baud -1) / baud;
        reg->LCR |= 1<<bo_uart_lcr_dlab;
        reg->RTD.DLL = temp;
        reg->DI.DLM = temp>>8;
        reg->FDR = divval + (mulval <<4);
        reg->LCR &= ~(1<<bo_uart_lcr_dlab);
        return true;
    }else
        return false;
}

#if(CN_CFG_UART0_USED == 1)

u32  Uart0_Int(ufast_t uart_int_line);

//----初始化uart0模块----------------------------------------------------------
//功能：初始化uart0模块，初始化状态为不使用dma传送的方式。
//参数：para，在本函数中无效
//返回：true = 成功初始化，false = 初始化失败
//-----------------------------------------------------------------------------
ptu32_t module_init_uart0(ptu32_t para)
{
    //保护缓冲区的信号量，使缓冲区中数据量为0时阻塞写入线程，读取线程使缓冲区中
    //数据降至trigger_level以下时释放信号量，使写入线程解除阻塞
    tg_uart0_cb.send_buf_semp = Lock_SempCreate(1,0,"uart0 tx buf");
    if(tg_uart0_cb.send_buf_semp == NULL)
        goto exit_from_left_buf_semp;
    tg_uart0_cb.recv_buf_semp = Lock_SempCreate(1,0,"uart0 rx buf");
    if(tg_uart0_cb.recv_buf_semp == NULL)
        goto exit_from_right_buf_semp;
    //初始化IO端口位uart功能
    pg_sysctrl_reg->pconp |= 1<<4;      //使能uart0模块
    GPIO_CfgPinFunc(0,2,cn_p0_2_txd0);
    GPIO_CfgPinFunc(0,3,cn_p0_3_rxd0);
    pg_uart0_reg->IF.IIR;     //读一下清除所有中断标志
    pg_uart0_reg->LCR |= 1<<bo_uart_lcr_dlab;
    pg_uart0_reg->IF.FCR = 1<< bo_uart_fcr_ffen;   //使能ifo。
    pg_uart0_reg->IF.FCR = 0x47;  //接收4字节触发中断，清收发fifo。
    tg_uart0_cb.baud = 115200;
    uart_set_baud(0,115200);
    pg_uart0_reg->LCR = (3<<bo_uart_lcr_bw)     //8位字符
                        | (0<<bo_uart_lcr_st)   //1个停止位
                        | (0<<bo_uart_lcr_pe)   //禁止奇偶校验
                        | (0<<bo_uart_lcr_lbc)  //禁止间断发送
                        | (0<<bo_uart_lcr_dlab);

    pg_uart0_reg->MCR = 0;
    pg_uart0_reg->TER = 1<<bo_uart_ter_txen;
    pg_uart0_reg->DI.IER =  1<< bo_uart_ier_rbr;       //使能接收中断，


    tg_uart0_cb.sendding = false;
    Ring_Init(  &tg_uart0_cb.send_ring_buf,
                uart0_drv_send_buf,
                uart0_buf_len);
    Ring_Init(  &tg_uart0_cb.recv_ring_buf,
                uart0_drv_recv_buf,
                uart0_buf_len);

    Int_IsrConnect(cn_int_line_uart0,Uart0_Int);
    Int_SettoAsynSignal(cn_int_line_uart0);

    Int_ClearLine(cn_int_line_uart0);
    Int_RestoreAsynLine(cn_int_line_uart0);
    return true;

exit_from_right_buf_semp:
    Lock_SempDelete(tg_uart0_cb.send_buf_semp);
exit_from_left_buf_semp:
    return false;
}


//----uart0中断---------------------------------------------------------------
//功能: 如果是接收信号触发的中断:
//      1.检查接收fifo中的数据量.把数据从接收寄存器全部copy到物理层缓冲区中.
//      2.如果物理层缓冲区满.置串口控制块状态字的溢出位,并发出uart错误事件.
//      3.发送串口接收事件.两个事件都是状态量事件
//      如果是发送信号引发的中断:
//      1.检查发送fifo有多少空位
//      2.从设备右手接口读出适量数据，
//      3.把读出的数据写入发送fifo
//      如果是出错信号引发中断:弹出串口出错事件
//参数: 中断函数没有参数.
//返回: 中断函数没有返回值.
//-----------------------------------------------------------------------------
uint32_t Uart0_Int(ufast_t uart_int_line)
{
    uint32_t recv_trans,num;
    uint32_t IIR;
    uint8_t ch[16];
    while(1)
    {
        IIR = pg_uart0_reg->IF.IIR;
        if(IIR & 1)
            break;
        IIR = (IIR>>1)&0x7;
        switch(IIR)
        {
            case 1:     //发送fifo寄存器空
            {
                recv_trans = Ring_Read(&tg_uart0_cb.send_ring_buf,ch,16);
                if(recv_trans != 0)
                {
                    for(num = 0; num < recv_trans; num++)
                    {
                        pg_uart0_reg->RTD.THR = ch[num];
                    }
                }
                if(recv_trans < 16)
                    __uart_pause_send_int(pg_uart0_reg);
                if(Ring_Check(&tg_uart0_cb.send_ring_buf) < 16)
                    Lock_SempPost(tg_uart0_cb.send_buf_semp);
            }break;
            case 2:     //接收触发水平到
            case 6:     //接收超时，fifo中至少有一个数据，连续4字符时间无新数据
            {
                for(num = 0; num<16; num++)
                {
                    if(pg_uart0_reg->LSR & 1)
                        ch[num] = pg_uart0_reg->RTD.THR;
                    else
                        break;
                }
                Ring_Write(&tg_uart0_cb.recv_ring_buf,ch,num);
                Lock_SempPost(tg_uart0_cb.recv_buf_semp);
            }break;
            case 3:     //出错标志
            {
            }break;
            default: return 0;
        }
    }
    return 0;
}

void uart0_send(u8 *buf,u32 len)
{
    u32 completed = 0,sended;
    if((len==0) || (buf == NULL))
        return;
    __uart_pause_send_int(pg_uart0_reg);
    if(Ring_IsEmpty(&tg_uart0_cb.send_ring_buf))
    {
        while((pg_uart0_reg->FIFOLVL &0x0f00) != 0x0f00)
        {
            pg_uart0_reg->RTD.THR = buf[completed];
            completed++;
            if(completed == len)
                break;
        }
    }
    if(completed < len)
    {
        while(1)
        {
            sended = Ring_Write(&tg_uart0_cb.send_ring_buf,
                                (uint8_t*)buf+completed,len-completed);
            __uart_resume_send_int(pg_uart0_reg);
            if(sended != len-completed)     //缓冲区满，没有送出全部数据
            {
                completed += sended;
                Lock_SempPend(tg_uart0_cb.send_buf_semp,CN_TIMEOUT_FOREVER);
                __uart_pause_send_int(pg_uart0_reg);
            }else
            {
                break;
            }
        }
    }else
    {
        __uart_resume_send_int(pg_uart0_reg);
    }
}


//========================================================
// 函数功能：串口0接收数据
// 输入参数：无
// 输出参数：无
// 返回值  ：无
//========================================================
u32 uart0_read(u8 *buf,u32 len,u32 timeout)
{
    u32 readed,completed=0,start_time,time=0;
    start_time = DjyGetTime();
    while(1)
    {

        readed = Ring_Read(&tg_uart0_cb.recv_ring_buf,buf+completed,len-completed);

        if(readed != len-completed)
        {
            completed += readed;
            if(Lock_SempPend(tg_uart0_cb.recv_buf_semp,timeout-time) == false)
            {
                break;
            }else
            {
                time = DjyGetTime() - start_time;     //回到while处再次读串口。
            }
        }else
        {
            completed = len;                            //已经完成，
            break;
        }
    }
    return completed;
}

#endif  //for (CN_CFG_UART0_USED == 1)

#if(CN_CFG_UART1_USED == 1)

u32  Uart1_Int(ufast_t uart_int_line);

//----初始化uart1模块----------------------------------------------------------
//功能：初始化uart1模块，初始化状态为不使用dma传送的方式。
//参数：para，在本函数中无效
//返回：true = 成功初始化，false = 初始化失败
//-----------------------------------------------------------------------------
ptu32_t module_init_uart1(ptu32_t para)
{
    //保护缓冲区的信号量，使缓冲区中数据量为0时阻塞写入线程，读取线程使缓冲区中
    //数据降至trigger_level以下时释放信号量，使写入线程解除阻塞
    tg_uart1_cb.send_buf_semp = Lock_SempCreate(1,0,"uart1 tx buf");
    if(tg_uart1_cb.send_buf_semp == NULL)
        goto exit_from_left_buf_semp;
    tg_uart1_cb.recv_buf_semp = Lock_SempCreate(1,0,"uart1 rx buf");
    if(tg_uart1_cb.recv_buf_semp == NULL)
        goto exit_from_right_buf_semp;
    //初始化IO端口位uart功能
    pg_sysctrl_reg->pconp |= 1<<3;      //使能uart1模块
    GPIO_CfgPinFunc(0,15,cn_p0_15_txd1);
    GPIO_CfgPinFunc(0,16,cn_p0_16_rxd1);
    pg_uart1_reg->IF.IIR;     //读一下清除所有中断标志
    pg_uart1_reg->LCR |= 1<<bo_uart_lcr_dlab;
    pg_uart1_reg->IF.FCR = 1<< bo_uart_fcr_ffen;   //使能ifo。
    pg_uart1_reg->IF.FCR = 0x47;  //接收4字节触发中断，清收发fifo。
    tg_uart1_cb.baud = 115200;
    uart_set_baud(1,115200);
    pg_uart1_reg->LCR = (3<<bo_uart_lcr_bw)     //8位字符
                        | (0<<bo_uart_lcr_st)   //1个停止位
                        | (0<<bo_uart_lcr_pe)   //禁止奇偶校验
                        | (0<<bo_uart_lcr_lbc)  //禁止间断发送
                        | (0<<bo_uart_lcr_dlab);

    pg_uart1_reg->MCR = 0;
    pg_uart1_reg->TER = 1<<bo_uart_ter_txen;
    pg_uart1_reg->DI.IER =  1<< bo_uart_ier_rbr;       //使能接收中断，


    tg_uart1_cb.sendding = false;
    Ring_Init(  &tg_uart1_cb.send_ring_buf,
                uart1_drv_send_buf,
                uart1_buf_len);
    Ring_Init(  &tg_uart1_cb.recv_ring_buf,
                uart1_drv_recv_buf,
                uart1_buf_len);

    Int_IsrConnect(cn_int_line_uart1,Uart1_Int);
    Int_SettoAsynSignal(cn_int_line_uart1);

    Int_ClearLine(cn_int_line_uart1);
    Int_RestoreAsynLine(cn_int_line_uart1);
    return true;

exit_from_right_buf_semp:
    Lock_SempDelete(tg_uart1_cb.send_buf_semp);
exit_from_left_buf_semp:
    return false;
}


//----uart1中断---------------------------------------------------------------
//功能: 如果是接收信号触发的中断:
//      1.检查接收fifo中的数据量.把数据从接收寄存器全部copy到物理层缓冲区中.
//      2.如果物理层缓冲区满.置串口控制块状态字的溢出位,并发出uart错误事件.
//      3.发送串口接收事件.两个事件都是状态量事件
//      如果是发送信号引发的中断:
//      1.检查发送fifo有多少空位
//      2.从设备右手接口读出适量数据，
//      3.把读出的数据写入发送fifo
//      如果是出错信号引发中断:弹出串口出错事件
//参数: 中断函数没有参数.
//返回: 中断函数没有返回值.
//-----------------------------------------------------------------------------
uint32_t Uart1_Int(ufast_t uart_int_line)
{
    uint32_t recv_trans,num;
    uint32_t IIR;
    uint8_t ch[16];
    while(1)
    {
        IIR = pg_uart1_reg->IF.IIR;
        if(IIR & 1)
            break;
        IIR = (IIR>>1)&0x7;
        switch(IIR)
        {
            case 1:     //发送fifo寄存器空
            {
                recv_trans = Ring_Read(&tg_uart1_cb.send_ring_buf,ch,16);
                if(recv_trans != 0)
                {
                    for(num = 0; num < recv_trans; num++)
                    {
                        pg_uart1_reg->RTD.THR = ch[num];
                    }
                }
                if(recv_trans < 16)
                    __uart_pause_send_int(pg_uart1_reg);
                if(Ring_Check(&tg_uart1_cb.send_ring_buf) < 16)
                    Lock_SempPost(tg_uart1_cb.send_buf_semp);
            }break;
            case 2:     //接收触发水平到
            case 6:     //接收超时，fifo中至少有一个数据，连续4字符时间无新数据
            {
                for(num = 0; num<16; num++)
                {
                    if(pg_uart1_reg->LSR & 1)
                        ch[num] = pg_uart1_reg->RTD.THR;
                    else
                        break;
                }
                Ring_Write(&tg_uart1_cb.recv_ring_buf,ch,num);
                Lock_SempPost(tg_uart1_cb.recv_buf_semp);
            }break;
            case 3:     //出错标志
            {
            }break;
            default: return 0;
        }
    }
    return 0;
}

void uart1_send(u8 *buf,u32 len)
{
    u32 completed = 0,sended;
    if((len==0) || (buf == NULL))
        return;
    __uart_pause_send_int(pg_uart1_reg);
    if(Ring_IsEmpty(&tg_uart1_cb.send_ring_buf))
    {
        while((pg_uart1_reg->FIFOLVL &0x0f00) != 0x0f00)
        {
            pg_uart1_reg->RTD.THR = buf[completed];
            completed++;
            if(completed == len)
                break;
        }
    }
    if(completed < len)
    {
        while(1)
        {
            sended = Ring_Write(&tg_uart1_cb.send_ring_buf,
                                (uint8_t*)buf+completed,len-completed);
            __uart_resume_send_int(pg_uart1_reg);
            if(sended != len-completed)     //缓冲区满，没有送出全部数据
            {
                completed += sended;
                Lock_SempPend(tg_uart1_cb.send_buf_semp,CN_TIMEOUT_FOREVER);
                __uart_pause_send_int(pg_uart1_reg);
            }else
            {
                break;
            }
        }
    }else
    {
        __uart_resume_send_int(pg_uart1_reg);
    }
}


//========================================================
// 函数功能：串口0接收数据
// 输入参数：无
// 输出参数：无
// 返回值  ：无
//========================================================
u32 uart1_read(u8 *buf,u32 len,u32 timeout)
{
    u32 readed,completed=0,start_time,time=0;
    start_time = DjyGetTime();
    while(1)
    {
        readed = Ring_Read(&tg_uart1_cb.recv_ring_buf,buf+completed,len-completed);
        if(readed != len-completed)
        {
            completed += readed;
            if(Lock_SempPend(tg_uart1_cb.recv_buf_semp,timeout-time) == false)
            {
                break;
            }else
            {
                time = DjyGetTime() - start_time;     //回到while处再次读串口。
            }
        }else
        {
            completed = len;                            //已经完成，
            break;
        }
    }
    return completed;
}
#endif  //for (CN_CFG_UART1_USED == 1)





