//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//所属模块: uart驱动程序(公共接口部分)
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:
//其他说明:
//修订历史:
//1. 日期:2013-11-10
//   作者:  罗侍田.
//   新版本号：1.0.0
//   修改说明: 移植字44b0的1.0.1版
//------------------------------------------------------------------------------
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "ring.h"
#include "systime.h"
#include "lock.h"
#include "djyos.h"
#include "multiplex.h"
#include <driver/include/uart.h>

//串口状态控制结构
 struct UartCB
{
    struct RingBuf SendRingBuf;              //环形发送缓冲区.
    struct RingBuf RecvRingBuf;              //环形接收缓冲区.

    struct SemaphoreLCB *SendRingBufSemp;    //发送缓冲区信号量
    struct SemaphoreLCB *RecvRingBufSemp;    //接收缓冲区信号量
    struct SemaphoreLCB *BlockingSemp;       //阻塞信号量
    struct MutexLCB *WriteMutex;             //app多线程互斥写访问
    struct MutexLCB *ReadMutex;          //app多线程互斥读访问

    u32 RecvRingTrigLevel;      //串口Ring接收触发水平,即收到多少数据时释放信号量
    u32 SendRingTrigLevel;      //发送Ring发送触发水平，当发送环形缓冲区满，将
                                //用SendRingBufSemp阻塞当前线程，直到发送缓冲
                                //区数据量降至SendRingTrigLevel时释放之。
    u32 MplReadTrigLevel;       //多路复用可读触发水平
    u32 MplWriteTrigLevel;      //多路复用可写触发水平
    u32 Baud;                   //串口当前波特率
    ptu32_t UartPortTag;        //串口标签
    struct MultiplexObjectCB * pMultiplexUart;      //多路复用目标对象头指针
    u32 MplUartStatus;          //多路复用的当前状态，如可读，可写等
    UartStartSend StartSend;
//    UartDirectSend DirectlySend;
    UartControl UartCtrl;
};

//----串口设备APP写函数----------------------------------------------------------
//功能: 写UART设备,由应用程序调用，UART被注册为设备，调用设备操作函数
//      Driver_WriteDevice，最终会调用本函数，函数调用过程:
//      Driver_WriteDevice() ---> Dev->Write() ---> UART_AppWrite()
//      该函数实现的功能说明如下:
//      1.若未开调度，则采用直接发送方式，直接发送前需先将缓冲区数据发送完成.
//      2.若开始调度，把数据写入串口uart_UCB的环形发送缓冲区中.
//      3.写入数据后，启动发送数据，启动发送的方式可为中断或轮询，由驱动决定.
//      4.若缓冲区满，则需等待缓冲区中数据降低到触发水平后，再继续写缓冲区.
//      5.有关timeout，每次等待信号量，时间都需递减.
//      6.当所有数据写入缓冲区后，判断是否阻塞发送，若为阻塞，则等待阻塞信号量再返回.
//      7.返回前，将该串口所在的多路利用集的写感应位清除
//参数: UCB,被操作的串口tagUartCB结构体指针数值
//      buf，数据源指针.
//      len，数据量(bytes).
//      offset,偏移量，在串口模块中，此变量无效
//      block_option,阻塞选项，非零为阻塞方式.当block_option为true，函数会等待串口
//         将所有数据发送完成后返回，属于阻塞发送方式；当block_option为false时，函数
//         会等待所有数据被填入串口模块的缓冲区中后，立刻返回，非阻塞发送方式在函数
//         返回时，串口可能仍在发送数据。
//      timeout,超时量(us).
//返回: 实际写入环形缓冲区的字符数
//-----------------------------------------------------------------------------
ptu32_t UART_AppWrite(struct UartCB *UCB,u8* src_buf,u32 len,
                    u32 offset,bool_t block_option,u32 timeout)
{
    u32 completed = 0,written;
    uint8_t *buf;
    u32 base_time,rel_timeout = timeout;

    if((len==0) || ((u8*)src_buf == NULL) || (UCB == NULL))
        return 0;

    buf = (uint8_t*)src_buf;
    base_time = (u32)DjyGetSysTime();

    if(Lock_MutexPend(UCB->WriteMutex,timeout)==false)
        return completed;

    while(1)
    {
        written = Ring_Write(&UCB->SendRingBuf,
                            (uint8_t*)buf+completed,len-completed);
        //先PEND一次信号量，防止事先已经被释放过，发送过程中，发送完成会释放这个
        //信号量，无论block_option值是什么，都会释放。
        Lock_SempPend(UCB->BlockingSemp,0);
        UCB->StartSend(UCB->UartPortTag,rel_timeout);

        if(written != len-completed)     //缓冲区满，没有送出全部数据
        {
            completed += written;
            if(false == Lock_SempPend(UCB->SendRingBufSemp,rel_timeout))
                break;
            rel_timeout = (u32)DjyGetSysTime() - base_time;
            if(rel_timeout > timeout)
                break;
            else
                rel_timeout = timeout - rel_timeout;
        }else
        {
            if(block_option == true)
            {
                Lock_SempPend(UCB->BlockingSemp,rel_timeout);
            }
            completed += written;
            break;
        }
    }
    if(Ring_IsFull(&UCB->SendRingBuf))
    {
        UCB->MplUartStatus &= (~CN_MULTIPLEX_SENSINGBIT_WRITE);
        Multiplex_Set(UCB->pMultiplexUart,UCB->MplUartStatus);
    }

    Lock_MutexPost(UCB->WriteMutex);
    return completed;
}

//----串口设备APP读函数----------------------------------------------------------
//功能: 读UART设备,由应用程序调用，UART被注册为设备，调用设备操作函数Driver_ReadDevice,
//      实质就是调用了该函数，函数调用过程:
//      Driver_ReadDevice() ---> Dev->Read() ---> UART_AppRead()
//      该函数实现的功能说明如下:
//      1.读出缓冲区中数据，若缓冲区中数据足够，则直接返回
//      2.若数据不够，则以缓冲容量的一半为单位，分批次接收数据
//      3.在等待数据前，需先将信号量清除，每次等待时间需从总超时时间扣除
//      4.返回前，将该串口所在的多路利用集的读感应位清除
//参数: UCB,被操作的串口tagUartCB结构体指针数值
//      dst_buf,应用程序提供的接收缓冲区
//      len,读入长度,字节
//      offset,偏移量,在串口模块中，此变量无效
//      timeout,读超时参数,微秒
//返回: 实际读出长度
//------------------------------------------------------------------------------
ptu32_t UART_AppRead(struct UartCB *UCB,u8* dst_buf,u32 len,
                    u32 offset,u32 timeout)
{
    uint32_t completed = 0;
    uint32_t ReadLen,TrigBak,triglevel;
    u32 base_time,rel_timeout=timeout;

    if((len==0) || ((u8*)dst_buf == NULL) || (UCB == NULL))
        return 0;

    base_time = (u32)DjyGetSysTime();
    if(Lock_MutexPend(UCB->ReadMutex,timeout)==false)
        return completed;

    completed = Ring_Read(&UCB->RecvRingBuf,(uint8_t*)dst_buf,len);
    if(completed < len)    //缓冲区中数据不够，则等待接收
    {
        //为提高效率，临时提高读缓冲区信号量触发水平，最多为半缓冲容积
        TrigBak = UCB->RecvRingTrigLevel;
        triglevel = Ring_Capacity(&UCB->RecvRingBuf)/2;
        ReadLen = len - completed;
        if(triglevel > ReadLen)
            triglevel = ReadLen;
        while(1)
        {
            UCB->RecvRingTrigLevel = triglevel;
            if(Ring_Check(&UCB->RecvRingBuf) < triglevel)
            {
                Lock_SempPend(UCB->RecvRingBufSemp,rel_timeout);
            }
            completed += Ring_Read(&UCB->RecvRingBuf,
                               ((u8*)dst_buf) + completed,
                               ReadLen);
            if(completed < len)
            {
                //每次pend的时间要递减
                rel_timeout = (u32)DjyGetSysTime() - base_time;
                if(rel_timeout > timeout)
                    break;
                else
                {
                    rel_timeout = timeout - rel_timeout;
                    ReadLen = len - completed;
                    if(triglevel > ReadLen)
                        triglevel = ReadLen;
                }
            }
            else
                break;
        }
        UCB->RecvRingTrigLevel = TrigBak;
    }
    //若缓冲区中不再有数据，清掉多路复用触发状态。
    if(Ring_Check(&UCB->RecvRingBuf) == 0)
    {
        UCB->MplUartStatus &= (~CN_MULTIPLEX_SENSINGBIT_READ);
        Multiplex_Set(UCB->pMultiplexUart,UCB->MplUartStatus);
    }

    Lock_MutexPost(UCB->ReadMutex);
    return completed;
}

//----串口设备端口写函数-------------------------------------------------------
//功能: 从端口写UART,由底层驱动调用,该函数实现的功能说明如下:
//      1.把数据写入串口uart_UCB的环形接收缓冲区中.
//      2.如果写入设备的数据未达到触发水平，则继续收集数据
//      3.达到出发水平后，发送信号量，通知上层取取数
//      4.若缓冲区溢出，则pop错误处理线程，错误处理事件由应用程序注册
//      5.发生错误时，置位多路复用集错误感应位，达到读触发水平时，置位多路复用集读
//参数: UCB,被操作的串口tagUartCB结构体指针.
//      buf，数据源指针。
//      len，数据量(bytes)
//返回: 实际写入环形缓冲区的字符数
//-----------------------------------------------------------------------------
ptu32_t UART_PortWrite(struct UartCB *UCB,u8* buf,u32 len,u32 res)
{
    uint16_t recv_bytes;
    uint32_t check;

    if(NULL == UCB)
        return 0;

    recv_bytes = Ring_Write(&UCB->RecvRingBuf, (u8*)buf,len);
    if(recv_bytes != len)
    {
        UCB->MplUartStatus |= CN_UART_BUF_OVER_ERR;
        Multiplex_Set(UCB->pMultiplexUart,UCB->MplUartStatus);
    }
    check = Ring_Check(&UCB->RecvRingBuf);
    if((check >= UCB->RecvRingTrigLevel) &&
                (check - recv_bytes < UCB->RecvRingTrigLevel))
    {
        Lock_SempPost(UCB->RecvRingBufSemp);
    }
    if((check >= UCB->MplReadTrigLevel) &&
                (check - recv_bytes < UCB->MplReadTrigLevel))
    {
        UCB->MplUartStatus |= CN_MULTIPLEX_SENSINGBIT_READ;
        Multiplex_Set(UCB->pMultiplexUart,UCB->MplUartStatus);
    }
    return (ptu32_t)recv_bytes;
}

//----串口设备端口读函数---------------------------------------------------------
//功能: 从端口读取UART设备,由底层驱动调用,该函数实现的功能说明如下:
//      1.从uart_UCB缓冲区中读取数据到底层驱动提供的dst_buf.
//      2.检查本次从缓冲区中读出数据前后是否跨过发送触发水平，若跨过了发送触发水平，
//        则发送信号量，通知应用层，可以继续写数据到缓冲区.
//      3.达到写触发水平时，置位多路复用集读标志感觉位
//      4.当读不到数据时，即缓冲区中数据为空时，释放阻塞信号量.
//参数: UCB,被操作的串口tagUartCB结构体指针.
//      dst_buf,读缓冲区
//      len,读入长度,
//返回: 实际读出长度
//------------------------------------------------------------------------------
ptu32_t UART_PortRead(struct UartCB *UCB,u8* dst_buf,u32 len,u32 res)
{
    uint32_t check,result=0;

    if(NULL == UCB)
        return result;

    result = (ptu32_t)Ring_Read(&UCB->SendRingBuf,(u8 *)dst_buf,len);
    check = Ring_Check(&UCB->SendRingBuf);
    if((check <= UCB->SendRingTrigLevel)
            && (check + result > UCB->SendRingTrigLevel))
    {
        Lock_SempPost(UCB->SendRingBufSemp);
    }
    if((check <= UCB->MplWriteTrigLevel)
            && (check + result > UCB->MplWriteTrigLevel))
    {
        UCB->MplUartStatus |= CN_MULTIPLEX_SENSINGBIT_WRITE;
        Multiplex_Set(UCB->pMultiplexUart,UCB->MplUartStatus);
    }
    if(result == 0)
    {
        Lock_SempPost(UCB->BlockingSemp);
    }

    return result;
}

//----串口错误处理函数----------------------------------------------------------
//功能: 错误处理函数，发生错误时调用该函数，配置多路复用集相应的错误标识位
//参数: UCB,被操作的串口tagUartCB结构体指针.
//      ErrNo,错误标识的比特位，该比特位必须是多路复用模块未用到感觉位，（3-30比特）
//返回: 0，错误；1，正确
//------------------------------------------------------------------------------
ptu32_t UART_ErrHandle(struct UartCB *UCB,u32 ErrNo)
{
    u32 Status,result = 0;
    if(NULL != UCB)
    {
        Status = UCB->pMultiplexUart->PendingBit | ErrNo;
        Multiplex_Set(UCB->pMultiplexUart,Status);
        result = 1;
    }
    return result;
}

//----串口设备控制函数---------------------------------------------------------
//功能: 串口设备的控制函数,由应用程序调用，UART被注册为设备，调用设备操作函数
//      Driver_CtrlDevice，实质就是调用了该函数，函数调用过程:
//      Driver_CtrlDevice() ---> Dev->dCtrl() ---> UART_Ctrl()
//      该函数实现的功能根据命令字符决定，说明如下:
//      1.启动停止串口，由底层驱动实现
//      2.设置半双工发送或接收状态，由底层驱动实现
//      3.暂停和恢复接收数据，由底层驱动实现
//      4.设置波特率和硬件触发水平，由底层驱动实现
//      5.设置错误弹出事件类型
//      ......
//参数: UCB,被操作的串口tagUartCB结构体指针.
//      cmd,操作类型
//      data,含义依cmd而定
//返回: 无意义.
//-----------------------------------------------------------------------------
ptu32_t UART_Ctrl(struct UartCB *UCB,u32 cmd,ptu32_t data1,ptu32_t data2)
{
    ptu32_t result = 0;

    if(UCB == NULL)
        return 0;
    switch(cmd)
    {
        case CN_UART_START:
        case CN_UART_STOP:
        case CN_UART_COM_SET:
        case CN_UART_RX_PAUSE:      //暂停接收
        case CN_UART_RX_RESUME:     //恢复接收
        case CN_UART_RECV_HARD_LEVEL:    //设置接收fifo触发水平
        case CN_UART_SEND_HARD_LEVEL:    //设置发送fifo触发水平
        case CN_UART_EN_RTS:             //使能rts流控
        case CN_UART_DIS_RTS:            //禁止rts流控
        case CN_UART_EN_CTS:             //使能cts流控
        case CN_UART_DIS_CTS:            //禁止cts流控
        case CN_UART_DMA_USED:           //使用dma传输
        case CN_UART_DMA_UNUSED:         //禁止dma传输
        case CN_UART_HALF_DUPLEX_SEND:
        case CN_UART_HALF_DUPLEX_RECV:
        case CN_UART_SETTO_ALL_DUPLEX:
             UCB->UartCtrl(UCB->UartPortTag,cmd,data1,data2);
             break;
        case CN_UART_SET_BAUD:          //设置Baud
            if(UCB->Baud !=data1)
            {
                UCB->Baud = data1;
                UCB->UartCtrl(UCB->UartPortTag,CN_UART_SET_BAUD,data1,data2);
            }
            break;
        case CN_UART_RECV_SOFT_LEVEL:   //设置接收软件触发水平
            UCB->RecvRingTrigLevel = data1;
            break;
        case CN_UART_SEND_SOFT_LEVEL:   //设置发送软件触发水平
            UCB->SendRingTrigLevel = data1;
            break;
        case CN_DRV_CTRL_SET_LEVEL:
            if((u32*)data1 != NULL)
            {
                if(*(u32*)data1 != 0)
                {
                    result = Ring_Capacity(&UCB->RecvRingBuf);
                    result = (result > *(u32*)data1)?(*(u32*)data1):result;
                    UCB->MplReadTrigLevel = result;
                }
                else
                {
                    *(u32*)data1 = UCB->MplReadTrigLevel;
                }
            }
            if((u32*)data2 != NULL)
            {
                if(*(u32*)data2 != 0)
                {
                    result = Ring_Capacity(&UCB->SendRingBuf);
                    result = (result > *(u32*)data2)?(*(u32*)data2):result;
                    UCB->MplWriteTrigLevel = result;
                }
                else
                {
                    *(u32*)data2 = UCB->MplWriteTrigLevel;
                }
            }
            break;
        default: break;
    }
    return result;
}

//----UART多路复用添加函数------------------------------------------------------
//功能：将UART添加到多路复用集，tagUartCB控制块有成员pMultiplexCB，调用该函数时，
//      未指向任何tagMultiplexSetsCB，因此需赋值为0
//参数：PrivateTag,串口模块的私有标签，此处为UCB控制块
//      MultiplexSets,多路复用集
//      DevAlias,被Multiplex的串口的设备别名
//      SensingBit,对象敏感位标志，31个bit，设为1表示本对象对这个bit标志敏感
//              bit31表示敏感类型，CN_SENSINGBIT_AND，或者CN_SENSINGBIT_OR
//返回：true,添加成功；false,添加失败
//-----------------------------------------------------------------------------
bool_t UART_MultiplexAdd(struct UartCB *UCB,
                         struct MultiplexSetsCB *MultiplexSets,
                         u32 DevAlias,
                         u32 SensingBit)
{
    bool_t result=false;

    if(UCB == NULL)
        return result;

    result = Multiplex_AddObject(MultiplexSets,
                            &UCB->pMultiplexUart,
                            UCB->MplUartStatus,
                            (ptu32_t)DevAlias,SensingBit);
    return result;
}

//----添加UART设备-------------------------------------------------------------
//功能：添加串口设备到系统设备队列
//      a、初始化环形缓冲区
//      b、创建环形缓冲区信号量和设备互斥量
//      c、将串口添加到设备管理模块，并初始化设备函数指针
//参数：UCB，串口控制块
//      Param,包含初始化UART所需参数，具体参数请查看tagUartParam结构体
//返回：串口控制块指针，NULL失败
//-----------------------------------------------------------------------------
struct UartCB * UART_InstallPort(struct UartParam *Param)
{
    struct DjyDevice * uart_dev;
    struct UartCB *UCB;
    struct MutexLCB *uart_mutexR,*uart_mutexT;
    u8 *pRxRingBuf,*pTxRingBuf;

    if(Param == NULL)
        return NULL;

    UCB = (struct UartCB *)M_Malloc(sizeof(struct UartCB),0);
    if(UCB == NULL)
        goto exit_from_ucb;
    memset(UCB,0x00,sizeof(struct UartCB));
    pRxRingBuf = (u8*)M_Malloc(Param->RxRingBufLen,0);
    if(pRxRingBuf == NULL)
        goto exit_from_rx_ring_buf;
    pTxRingBuf = (u8*)M_Malloc(Param->TxRingBufLen,0);
    if(pTxRingBuf == NULL)
        goto exit_from_tx_ring_buf;

    //建立串口阻塞使用的信号量
    UCB->BlockingSemp = Lock_SempCreate(1,0,CN_BLOCK_FIFO,"uart blocking");
    if(UCB->BlockingSemp == NULL)
        goto exit_from_blocking_semp;
    //保护缓冲区的信号量，使缓冲区中数据量为0时阻塞写入线程，读取线程使缓冲区中
    //数据降至trigger_level以下时释放信号量，使写入线程解除阻塞
    UCB->SendRingBufSemp = Lock_SempCreate(1,0,CN_BLOCK_FIFO,"uart tx buf");
    if(UCB->SendRingBufSemp == NULL)
        goto exit_from_send_buf_semp;
    UCB->RecvRingBufSemp = Lock_SempCreate(1,0,CN_BLOCK_FIFO,"uart rx buf");
    if(UCB->RecvRingBufSemp == NULL)
        goto exit_from_recv_buf_semp;
    //为设备创建互斥量，提供设备的互斥访问
    uart_mutexR = Lock_MutexCreate("uart receive mutex");
    if(uart_mutexR == NULL)
        goto exit_from_mutexR;
    uart_mutexT = Lock_MutexCreate("uart send mutex");
    if(uart_mutexT == NULL)
        goto exit_from_mutexT;

    UCB->WriteMutex = Lock_MutexCreate("UART_WriteMutex");
    if( NULL == UCB->WriteMutex)
        goto exit_from_mutexWite;
    UCB->ReadMutex  = Lock_MutexCreate("UART_ReadMutex");
    if(NULL == UCB->ReadMutex)
        goto exit_from_mutexRead;

    UCB->SendRingTrigLevel  = (Param->TxRingBufLen)>>2;  //默认缓冲触发水平为1/16
    UCB->RecvRingTrigLevel  = (Param->RxRingBufLen)>>4;
    UCB->MplReadTrigLevel   = (Param->TxRingBufLen)>>4;
    UCB->MplWriteTrigLevel  = (Param->RxRingBufLen)>>2;
    UCB->Baud               = Param->Baud;
    UCB->UartPortTag        = Param->UartPortTag;
    UCB->StartSend          = Param->StartSend;
    UCB->UartCtrl           = Param->UartCtrl;
    UCB->pMultiplexUart     = NULL;                     //初始化时为NULL
    UCB->MplUartStatus      = CN_MULTIPLEX_SENSINGBIT_WRITE;//初始时可写不可读
    Ring_Init(&UCB->SendRingBuf,pTxRingBuf,Param->TxRingBufLen);
    Ring_Init(&UCB->RecvRingBuf,pRxRingBuf,Param->RxRingBufLen);

    //添加UART设备
    uart_dev = Driver_DeviceCreate(NULL,(char*)Param->Name,uart_mutexR,uart_mutexT,
                               (fntDevWrite) UART_AppWrite,
                               (fntDevRead ) UART_AppRead,
                               (fntDevCtrl ) UART_Ctrl,
                               (fntDevMultiplexAdd)UART_MultiplexAdd,
                               (ptu32_t)UCB
                               );
    if(uart_dev == NULL)
        goto exit_from_add_device;

    printk("%s Install Succeeded!\r\n",Param->Name);
    return UCB;
    //如果出现错误，则释放创建的资源，并返回空指针
exit_from_add_device:
    Lock_MutexDelete(UCB->ReadMutex);
exit_from_mutexRead:
Lock_MutexDelete(UCB->WriteMutex);
exit_from_mutexWite:
    Lock_MutexDelete(uart_mutexT);
exit_from_mutexT:
    Lock_MutexDelete(uart_mutexR);
exit_from_mutexR:
    Lock_SempDelete(UCB->RecvRingBufSemp);
exit_from_recv_buf_semp:
    Lock_SempDelete(UCB->SendRingBufSemp);
exit_from_send_buf_semp:
    Lock_SempDelete(UCB->BlockingSemp);
exit_from_blocking_semp:
    free(pTxRingBuf);
exit_from_tx_ring_buf:
    free(pRxRingBuf);
exit_from_rx_ring_buf:
    free(UCB);
exit_from_ucb:
    return NULL;
}

