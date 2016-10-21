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
// 文件名     ：iicbus.h
// 模块描述: IIC模块的结构体及API函数
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 17/06.2014
// =============================================================================

#ifndef __IICBUS_H__
#define __IICBUS_H__
#include "object.h"
#include "lock.h"
//IIC 底层硬件驱动接口
// =============================================================================
// 功能: 启动写时序，启动写时序的过程为：器件地址（写）、存储地址（写），当存储地址完
//       成时，需打开中断，重新配置寄存器为接收模式，之后将会发生发送中断，在中断服务
//       函数中，每次发送一定的字节数，直到len数据量，post信号量iic_bus_semp，并产生
//       停止时序
// 参数: SpecificFlag,个性标记，本模块内即IIC寄存器基址
//      DevAddr,器件地址的前7比特，已将内部地址所占的bit位更新，该函数需将该地址左
//               移一位增加增加最后一位读/写比特;
//      MemAddr,存储器内部地址，即发送到总线上的地址，该地址未包含放在器件地址上的
//               比特位；
//      MenAddrLen,存储器内部地址的长度，字节单位，未包含在器件地址里面的比特位；
//      Length,接收的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，
//          当接收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//      IIC_BusSemp,发送完成时驱动需释放的信号量。发送程序读IIC_PortRead时，若读不到数
//          则需释放该信号量，产生结束时序
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
typedef bool_t (*WriteStartFunc)(ptu32_t  SpecificFlag,u8 DevAddr,\
                                u32 MemAddr,u8 MenAddrLen, u32 Length,\
                                struct SemaphoreLCB *IIC_BusSemp);
// =============================================================================
// 功能: 启动读时序，启动读时序的过程为：器件地址（写）、存储地址（写）、器件地址（读）
//       当器件地址（读）完成时，需打开中断，重新配置寄存器为接收模式，之后将会发生
//       接收数据中断，在中断中将接收到的数据调用IIC_PortWrite写入缓冲，接收到len字
//       节数的数据后，释放信号量iic_semp
// 参数: SpecificFlag,个性标记，本模块内即IIC寄存器基址
//      DevAddr,器件地址的前7比特，已将内部地址所占的bit位更新，该函数需将该地址左
//               移一位增加增加最后一位读/写比特;
//     MemAddr,存储器内部地址，即发送到总线上的地址，该地址未包含放在器件地址上的
//               比特位；
//      MenAddrLen,存储器内部地址的长度，字节单位，未包含在器件地址里面的比特位；
//      Length,接收的数据总量，接收数据的倒数第一字节，即count-1，停止产生ACK信号，当接
//          收的字节数为count时，产生停止时序，并释放信号量iic_semp;
//     IIC_BusSemp,读完成时，驱动需释放的信号量（缓冲区信号量）
// 返回: TRUE，启动读时序成功，FALSE失败
// =============================================================================
typedef bool_t (*ReadStartFunc)(ptu32_t  SpecificFlag,u8 DevAddr,\
                                u32 MemAddr,u8 MemAddrLen, u32 Length,\
                                struct SemaphoreLCB *IIC_BusSemp);
// =============================================================================
// 功能: 结束本次读写回调函数，区分读写的不同停止时序，当属于发送时，则直接停止时序，
//      若为读，则先停止回复ACK，再停止。
// 参数: SpecificFlag,个性标记，本模块内即IIC寄存器基址。
// 返回: 无
// =============================================================================
typedef void (*GenerateEndFunc)(ptu32_t  SpecificFlag);
// =============================================================================
// 功能：IIC总线控制回调函数，被上层调用，目前只实现对IIC时钟频率配置
// 参数：SpecificFlag,个性标记，本模块内即IIC寄存器基址
//       cmd,命令
//       data1,data2,数据，与具体命令相关
// 返回：无
// =============================================================================
typedef s32 (*IICBusCtrlFunc)      (ptu32_t SpecificFlag,u32 cmd,\
                                ptu32_t data1,ptu32_t data2);
// =============================================================================
// 功能：轮询方式读写IIC设备
// 参数：SpecificFlag,寄存器基址
//       DevAddr,设备地址
//       MemAddr,设备内部地址
//       MemAddrLen,设备内部地址长度
//       Buf,存储数据
//       Length,数据长度，字节
//       WrRdFlag,读写标记，为0时写，1时为读
// 返回：len,读取成功;-1,读取失败
// =============================================================================
typedef bool_t (*WriteReadPoll)(ptu32_t  SpecificFlag,u8 DevAddr,u32 MemAddr,\
                                u8 MenAddrLen,u8* Buf, u32 Length,u8 WrRdFlag);

//IIC初始化参数结构体
struct IIC_Param
{
    char                *BusName;                   //总线名称，如IIC1
    u8                  *IICBuf;                    //总线缓冲区指针
    u32                 IICBufLen;                  //总线缓冲区大小，字节
    ptu32_t             SpecificFlag;               //指定标记，如IIC寄存器基址
    WriteReadPoll       pWriteReadPoll;             //轮询或中断未开时使用
    WriteStartFunc      pGenerateWriteStart;        //写过程启动
    ReadStartFunc       pGenerateReadStart;         //读过程启动
    GenerateEndFunc     pGenerateEnd;               //结束通信
    IICBusCtrlFunc      pBusCtrl;                   //控制函数
};

//IIC总线器件结构体
struct IIC_Device
{
    struct Object DevNode;

    u8 DevAddr;                 //七位的器件地址,最低的0~2bit可能是器件内部地址。
    u8 BitOfMemAddrInDevAddr;   //器件地址中内部地址所占比特位数
    u8 BitOfMemAddr;            //器件内部地址寻址总bit数，包含了BitOfMemAddrInDevAddr
};

//IIC缓冲区结构体
struct IICBuf
{
    u32    Offset;              //缓冲区指针,指向下一次读的位置
    u32    MaxLen;              //缓冲区最大长度,元素个数.
    u8     *pBuf;               //缓冲区指针,用户自己保证所开辟的缓冲区是否与设定
};
//IIC总线控制块结构体,本模块可见
struct IIC_CB
{
    struct Object        IIC_BusNode;               //总线资源节点
    struct IICBuf         IIC_Buf;                   //缓冲区,用于异步发送
    struct SemaphoreLCB * IIC_BusSemp;               //IIC总线保护信号量
    struct SemaphoreLCB * IIC_BufSemp;               //简易缓冲区保护信号量
    u16                    ErrorPopEvtt;                //出错处理事件的类型
    u32                    Counter;                     //发送/接收计数器
    u8                    *pBuf;                        //发送/接收缓冲区指针
    u8                     Flag;                        //标记，按位，包括 读/方式和轮询/中断两种
    ptu32_t                SpecificFlag;                //个性标记
    WriteReadPoll          pWriteReadPoll;
    WriteStartFunc         pGenerateWriteStart;
    ReadStartFunc          pGenerateReadStart;
    GenerateEndFunc        pGenerateEnd;
    IICBusCtrlFunc         pBusCtrl;
};

//定义BusCtrl()的参数cmd,0x80以下由系统定义，0x80以上用户定义
#define CN_IIC_SET_CLK               0  //设置时钟频率
#define CN_IIC_DMA_USED              1  //使用dma传输
#define CN_IIC_DMA_UNUSED            2  //禁止dma传输
#define CN_IIC_SET_ERROR_POP_EVENT   3  //弹出错误事件
#define CN_IIC_SET_POLL              4  //使用轮询方式发送接收
#define CN_IIC_SET_INT            5  //使用中断方式发送接收
#define CN_IIC_OS_USED               0x80

//IIC读写标志
#define CN_IIC_WRITE_FLAG           0x00
#define CN_IIC_READ_FLAG            0x01
//定义参数返回
#define CN_IIC_EXIT_UNKNOW_ERR              -1
#define CN_IIC_EXIT_TIMEOUT                 -2
#define CN_IIC_EXIT_PARAM_ERR               -3
#define CN_IIC_EXIT_FUN_NULL                -4

//定义发生错误弹出事件参数值
#define CN_IIC_POP_UNKNOW_ERR           (0x01 << 16)        //未知错误
#define CN_IIC_POP_NO_ACK_ERR           (0x01 << 17)        //未收到从器件的ACK信号
#define CN_IIC_POP_MAL_LOST_ERR         (0x01 << 18)        //仲裁丢失


//IIC模块API函数
struct Object *ModuleInstall_IICBus(ptu32_t para);

struct IIC_CB *IIC_BusAdd(struct IIC_Param *NewIICParam);
struct IIC_CB *IIC_BusAdd_s(struct IIC_CB *NewIIC,struct IIC_Param *NewIICParam);
bool_t IIC_BusDelete(struct IIC_CB *DelIIC);
bool_t IIC_BusDelete_s(struct IIC_CB *DelIIC);
struct IIC_CB *IIC_BusFind(const char *BusName);

struct IIC_Device *IIC_DevAdd(const char *BusName , const char *DevName, u8 DevAddr,
                                u8 BitOfMaddrInDaddr, u8 BitOfMaddr);
struct IIC_Device *IIC_DevAdd_s(struct IIC_Device *NewDev,const char *BusName,
                                   const char *DevName, u8 DevAddr,
                                   u8 BitOfMaddrInDaddr, u8 BitOfMaddr);
bool_t IIC_DevDelete(struct IIC_Device *DelDev);
bool_t IIC_DevDelete_s(struct IIC_Device *DelDev);
struct IIC_Device *IIC_DevFind(const char *DevName);


s32  IIC_Write(struct IIC_Device *DelDev, u32 addr,u8 *buf,u32 len,
                bool_t block_option,u32 timeout);
s32  IIC_Read(struct IIC_Device *DelDev,u32 addr,u8 *buf,u32 len,u32 timeout);
s32  IIC_PortRead( struct IIC_CB *IIC,u8 *buf,u32 len);
s32  IIC_PortWrite(struct IIC_CB *IIC,u8 *buf,u32 len);
s32  IIC_BusCtrl(struct IIC_Device *DelDev,u32 cmd,ptu32_t data1,ptu32_t data2);
s32  IIC_ErrPop(struct IIC_CB *IIC, u32 ErrNo);

#endif /* __IICBUS_H__ */
