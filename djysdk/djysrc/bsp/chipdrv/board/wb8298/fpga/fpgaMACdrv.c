//----------------------------------------------------
//Copyright (C), 2013-2020,  lst.
//版权所有 (C), 2013-2020,   lst.
//所属模块:板级外设驱动SMAC部分
//作者：lst
//版本：V1.0.0
//文件描述:
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-09-28
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include "cpu_peri.h"
#include "string.h"
/******************* SMAC	地址分配***********************/
//0x00_0000						端口0接收长度
//0x00_0004						端口0发送长度
//0x01_0000						端口1接收长度
//0x01_0004						端口1发送长度
//0x02_0000						端口2接收长度
//0x02_0004						端口2发送长度
//0x03_0000						端口3接收长度
//0x03_0004						端口3发送长度
//0x04_0000						端口4接收长度
//0x04_0004						端口4发送长度
//0x05_0000						端口5接收长度
//0x05_0004						端口5发送长度
//0x00_1000 ~ 0x00_1FFF			端口0接收缓存(4k)
//0x00_2000 ~ 0x00_2FFF			端口0发送缓存(4k)
//0x01_1000 ~ 0x01_1FFF			端口1接收缓存(4k)
//0x01_2000 ~ 0x01_2FFF			端口1发送缓存(4k)
//0x02_1000 ~ 0x02_1FFF			端口2接收缓存(4k)
//0x02_2000 ~ 0x02_2FFF			端口2发送缓存(4k)
//0x03_1000 ~ 0x03_1FFF			端口3接收缓存(4k)
//0x03_2000 ~ 0x03_2FFF			端口3发送缓存(4k)
//0x04_1000 ~ 0x04_1FFF			端口4接收缓存(4k)
//0x04_2000 ~ 0x04_2FFF			端口4发送缓存(4k)
//0x05_1000 ~ 0x05_1FFF			端口5接收缓存(4k)
//0x05_2000 ~ 0x05_2FFF			端口5发送缓存(4k)
#define SMAC_BASE_ADDR          (0xA0000000)

#define PORT_RXLEN(x)   (SMAC_BASE_ADDR + 0x010000 * x + 0x000000) //(RO) 端口x接收长度
#define PORT_TXLEN(x)   (SMAC_BASE_ADDR + 0x010000 * x + 0x000004) //(WO) 端口x发送长度
#define PORT_RXBUF(x)   (SMAC_BASE_ADDR + 0x010000 * x + 0x001000) //(RO) 端口x接收缓存(4k)
#define PORT_TXBUF(x)   (SMAC_BASE_ADDR + 0x010000 * x + 0x002000) //(WO) 端口x发送缓存(4k)

#define PORT_MAX_NUM    5               //总port的数量
#define DMA_PACK_LEN    128             //DMA每次搬运数据大小

#define SMAC_BUF_LENTH   0x1000         //缓冲区大小，字节数
uint8_t smac_recvbuf[SMAC_BUF_LENTH];   //接收缓冲区大小
uint8_t smac_sendbuf[SMAC_BUF_LENTH];   //发送缓冲区大小

//-------SMAC接收数据函数----------------------------------------------------
//功能：SMAC接收数据，该函数由应用层调用，FPGA接收到的网络数据都，应用层通过
//      读RXLEN的高16位来判断是否有有数据，数据长度(bytes)= 4*高16bit，如果
//      有数据，则读取之，否则返回0，读取数据，通过DMA搬运的方式
//参数：port:发送的端口号，包括0~5总共六个端口号
//      Buf:接收数据的缓冲区指针，实际的缓冲区由本驱动调用
//返回：有数据则读取数据，返回读取数据大小，否则返回0
//--------------------------------------------------------------------------
uint32_t smac_recv(uint8_t port, uint8_t *buf)
{
    uint32_t src_addr;
    uint32_t recvlen = 0,readtimes = 0;
    uint8_t i;

    if(port > PORT_MAX_NUM)
        return 0;

    recvlen = (uint32_t)(*(uint32_t *)(PORT_RXLEN(port)));  //读取是否有接收到数据
    recvlen = (u16)((recvlen>>16) & 0xFFFF);                //高16bit是接收的数据长度
                                                            //数据长度(bytes)= 4*高16bit
    recvlen = recvlen *4;
    src_addr = PORT_RXBUF(port);
    if((recvlen>0) &&(recvlen < SMAC_BUF_LENTH)) //判断是否有数据，且数据大小不超缓冲区大小
    {
        readtimes = recvlen/DMA_PACK_LEN + (recvlen%DMA_PACK_LEN ? 1:0);
        for(i = 0; i < readtimes; i++)                      //分多次搬运，暂定
        {
            dmacpy((unsigned long)&smac_recvbuf[i*DMA_PACK_LEN],
                    src_addr + i*DMA_PACK_LEN, DMA_PACK_LEN);
        }
        buf = smac_recvbuf;                                 //返回地址赋值
    }
    else
    {
        recvlen = 0;
        buf = NULL;
    }

    return recvlen;
}

//-------SMAC发送数据函数----------------------------------------------------
//功能：SMAC发送数据，该函数由应用层调用，将数据包发送到FPGA,报文长度不超过
//      2048字节，先将发送的数据分多次通过DMA搬运，然后将数据长度写到相应寄存器
//参数：port:发送的端口号，包括0~5总共六个端口号
//      pbyBuf:发送数据的指针
//      len:发送的数据长度，字节为单位
//返回：正确发送返回len，否则返回0
//--------------------------------------------------------------------------
uint32_t smac_send(uint8_t port, uint8_t *pbyBuf, uint32_t len)
{
    uint32_t dst_addr,*writelen;
    uint32_t sendlen = 0,sendtimes = 0;
    uint8_t i;
    int status;

    if((len > SMAC_BUF_LENTH) || (port > PORT_MAX_NUM))
        return 0;

    dst_addr = PORT_TXBUF(port);
    if(len)
    {
        memcpy(smac_sendbuf, pbyBuf, len);
        sendtimes = len/DMA_PACK_LEN + (len%DMA_PACK_LEN ?1:0);
        for(i = 0; i < sendtimes; i++)                  //DMA分多次搬运，暂定
        {
            status = dmacpy(dst_addr + i*DMA_PACK_LEN,
                        (unsigned long)&smac_sendbuf[i*DMA_PACK_LEN],DMA_PACK_LEN);
            if(status == -1)
            {
                sendlen = 0;
                len = 0;
                break;
            }
            else
                sendlen += DMA_PACK_LEN;
        }
        writelen = (uint32_t *)(PORT_TXLEN(port));
        *writelen = (sendlen << 16) + len;            //将长度写到长度地址内
    }

    return len;
}



