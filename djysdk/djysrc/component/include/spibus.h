// =============================================================================
// Copyright (C) 2012-2020 都江堰操作系统研发团队  All Rights Reserved
// 文件名     ：SPIbus.h
// 模块描述: SPI模块的结构体及API函数
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 02/07.2014
// =============================================================================

#ifndef __SPIBUS_H__
#define __SPIBUS_H__

#include "object.h"

typedef ptu32_t (*TransferFunc)(ptu32_t SpecificFlag,u32 sendlen,u32 recvlen,u32 recvoff);
typedef bool_t (*TransferPoll)(ptu32_t SpecificFlag,u8* srcaddr,u32 sendlen,
        u8* destaddr,u32 recvlen,u32 recvoff,u8 cs);
typedef bool_t (*CsActiveFunc)(ptu32_t SpecificFlag, u8 cs);
typedef bool_t (*CsInActiveFunc)(ptu32_t SpecificFlag, u8 cs);
typedef ptu32_t (*SPIBusCtrlFunc)(ptu32_t SpecificFlag,u32 cmd,ptu32_t data1,ptu32_t data2);

struct SPI_Param
{
    char            *BusName;               //总线名称，如SPI1
    u8              *SPIBuf;                //总线缓冲区指针
    u32             SPIBufLen;              //总线缓冲区大小，字节
    ptu32_t         SpecificFlag;           //SPI私有标签，如控制寄存器基址
    bool_t          MultiCSRegFlag;         //SPI控制寄存器是否有多套CS配置寄存器
    TransferFunc    pTransferTxRx;          //发送接收回调函数，中断方式
    TransferPoll    pTransferPoll;          //发送接收回调函数，轮询方式
    CsActiveFunc    pCsActive;              //片选使能
    CsInActiveFunc  pCsInActive;            //片选失能
    SPIBusCtrlFunc  pBusCtrl;               //控制函数
};

//SPI总线器件结构体
struct SPI_Device
{
    struct Object DevNode;
    u8 Cs;                                  //片选信号
    bool_t AutoCs;                          //自动片选
    u8 CharLen;                             //数据长度
    u8 Mode;                                //模式选择
    u8 ShiftDir;                            //MSB or LSB
    u32 Freq;                               //速度,Hz
};

struct SPI_DataFrame
{
    u8* SendBuf;        //发送数据指针
    u32 SendLen;        //发送数据长度，字节
    u8* RecvBuf;        //接收数据指针
    u32 RecvLen;        //接收数据长度，字节
    u32 RecvOff;        //接收数据偏移
};

struct SPI_Buf
{
    u32    Offset;         //缓冲区指针,指向下一次读的位置
    u32    MaxLen;        //缓冲区最大长度,元素个数.
    u8     *pBuf;             //缓冲区指针,用户自己保证所开辟的缓冲区是否与设定
};
//SPI总线控制块结构体,本模块可见
struct SPI_CB
{
    struct Object       SPI_BusNode;                //总线资源节点
    struct SPI_Buf       SPI_Buf;                    //缓冲区,用于异步发送
    struct SemaphoreLCB *SPI_BusSemp;                //SPI总线保护信号量
    struct SemaphoreLCB *SPI_BlockSemp;              //简易缓冲区保护信号量
    struct SPI_DataFrame Frame;
    struct SPI_Device    *CurrentDev;                //占有当前总线的设备
    u16                     ErrorPopEvtt;               //出错处理事件的类型
    bool_t                  MultiCsReg;                 //是否具有多套CS寄存器
    u8                      BlockOption;                //当前总线操作是否阻塞
    u8                      Flag;                       //轮询中断标记
    ptu32_t                 SpecificFlag;               //个性标记
    TransferFunc            pTransferTxRx;
    TransferPoll            pTransferPoll;
    CsActiveFunc            pCsActive;
    CsInActiveFunc          pCsInActive;
    SPIBusCtrlFunc          pBusCtrl;
};


// SPI mode flags
#define SPI_CPHA    0x01            /* clock phase */
#define SPI_CPOL    0x02            /* clock polarity */
#define SPI_MODE_0  (0|0)           /* (original MicroWire) */
#define SPI_MODE_1  (0|SPI_CPHA)
#define SPI_MODE_2  (SPI_CPOL|0)
#define SPI_MODE_3  (SPI_CPOL|SPI_CPHA)

#define SPI_SHIFT_MSB  0
#define SPI_SHIFT_LSB  1

#define SPI_MODE_SLAVE  0
#define SPI_MODE_MASTER 1

typedef struct SpiConfig
{
   u8 Mode;     //包括CPHA,CPOL
   u8 ShiftDir; //MSB or LSB
   u8 CharLen;  //字符长度,bits
   u32 Freq;    //时钟频率
} tagSpiConfig;

#define CN_INVALID_CS               0xFF    //无效的片选

//定义BusCtrl()的参数cmd,0x80以下由系统定义，0x80以上用户定义
#define CN_SPI_SET_CLK               0
#define CN_SPI_SET_ERROR_POP_EVENT   1
#define CN_SPI_CS_CONFIG             2
#define CN_SPI_SET_AUTO_CS_EN        3
#define CN_SPI_SET_AUTO_CS_DIS       4
#define CN_SPI_SET_POLL              5
#define CN_SPI_SET_INT               6
#define CN_SPI_OS_USED               0x80

//定义参数返回
#define CN_SPI_EXIT_NOERR                   0
#define CN_SPI_EXIT_UNKNOW_ERR              -1
#define CN_SPI_EXIT_TIMEOUT                 -2
#define CN_SPI_EXIT_PARAM_ERR               -3


//定义发生错误弹出事件参数值
#define CN_SPI_POP_UNKNOW_ERR               (0x01 << 16)        //未知错误

//SPI模块API函数
struct Object *ModuleInstall_SPIBus(ptu32_t para);

struct SPI_CB *SPI_BusAdd(struct SPI_Param *NewSPIParam);
struct SPI_CB *SPI_BusAdd_s(struct SPI_CB *NewSPI,struct SPI_Param *NewSPIParam);
bool_t SPI_BusDelete(struct SPI_CB *DelSPI);
bool_t SPI_BusDelete_s(struct SPI_CB *DelSPI);
struct SPI_CB *SPI_BusFind(const char *BusName);

struct SPI_Device *SPI_DevAdd(const char *BusName ,const char *DevName,u8 cs,u8 charlen,
                                u8 mode,u8 shiftdir,u32 freq,u8 autocs);
struct SPI_Device *SPI_DevAdd_s(struct SPI_Device *NewDev,const char *BusName,
                                   const char *DevName);
bool_t SPI_DevDelete(struct SPI_Device *DelDev);
bool_t SPI_DevDelete_s(struct SPI_Device *DelDev);
struct SPI_Device *SPI_DevFind(const char *DevName);

s32 SPI_Transfer(struct SPI_Device *Dev,struct SPI_DataFrame *spidata,
                u8 block_option,u32 timeout);
bool_t SPI_CsActive(struct SPI_Device *Dev,u32 timeout);
bool_t SPI_CsInactive(struct SPI_Device *Dev);
s32  SPI_PortRead( struct SPI_CB *pSPI,u8 *buf,u32 len);
s32  SPI_PortWrite(struct SPI_CB *pSPI,u8 *buf,u32 len);
s32  SPI_BusCtrl(struct SPI_Device *Dev,u32 cmd,ptu32_t data1,ptu32_t data2);
s32  SPI_ErrPop(struct SPI_CB *pSPI, u32 ErrNo);

#endif /* __SPIBUS_H__ */
