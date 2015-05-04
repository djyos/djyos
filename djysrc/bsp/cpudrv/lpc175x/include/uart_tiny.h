//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块: uart驱动程序
//作者：lst
//版本：V1.0.1
//文件描述: 驱动44b0的uart
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-03-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef __UART_TINY_H__
#define __UART_TINY_H__

#ifdef __cplusplus
extern "C" {
#endif

//串口状态控制结构
struct uart_UCB
{
    struct tagRingBuf send_ring_buf;           //环形发送缓冲区.
    struct tagRingBuf recv_ring_buf;           //环形接收缓冲区.

    struct tagSemaphoreLCB *send_buf_semp;     //发送缓冲区锁
    struct tagSemaphoreLCB *recv_buf_semp;     //接收缓冲区锁

    u32 baud;
    bool_t sendding;                //若为true，则设备从左手写入后，无需重新启动
                                    //发送，否则，需要启动发送
};

#define uart0_buf_len  64
#define uart1_buf_len  64
#define uart2_buf_len  64
#define uart3_buf_len  64

//串口错误类型
enum uart_error_no
{
    enum_recv_hard_over,         //接收硬件溢出
    enum_recv_buf_over,          //接收缓冲区溢出
};

ptu32_t module_init_uart0(ptu32_t para);
ptu32_t module_init_uart1(ptu32_t para);
bool_t uart_set_baud(u32 uart_no,u32 baud);
void uart0_send(u8 *buf,u32 len);
void uart1_send(u8 *buf,u32 len);
u32 uart0_read(u8 *buf,u32 len,u32 timeout);
u32 uart1_read(u8 *buf,u32 len,u32 timeout);


#ifdef __cplusplus
}
#endif

#endif // __UART_TINY_H__

