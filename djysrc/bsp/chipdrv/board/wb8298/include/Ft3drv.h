/*================================================================================
 * 元件名称：WB800平台的独立FT3实现
 * 元件描述：简易FT3接收、发送处理
             由于FPGA内存容量限制，注意对收发报文大小的限制
 * 元件版本: V1.00
 * 开发人员: 麦康世
 * 定版时间:
 * 版本修订:
 * 修订人员:
 *================================================================================*/

#ifndef _WB828_FT3_H_
#define _WB828_FT3_H_

#define CN_FT3_CHIP_MAX             (20)                    // FT3接收芯片总数
#define CN_FT3_MSG_MAX              (15)                    // 其实就是RAM缓冲区最大可以放15个帧
#define CN_FT3_LEN_RECV_32BIT       (17)                    // FT3接收最大32位报文长度 64byte(数据)+4byte(时标)
#define CN_FT3_LEN_SEND_32BIT       (16)                    // FT3单口发送缓冲区最大32位数据长度
#define CN_FT3_MAX_INTERVAL            (7)                        // 一次能够接收的最大报文数目
#define CN_FT3_CRYSTALL_FRQ         (100)                   // FT3硬件晶振频率(单位：兆)
#define CN_FT3_WAIT_IDLE            (200)                   // FT3接收空闲等待BIT数目

// 单个报文体
typedef struct
{
    UINT8           byPacket[CN_FT3_LEN_RECV_32BIT*4];      // 报文内容
}tagFT3Packet;

// 报文数据池
typedef struct
{
    tagFT3Packet    tFt3Packet[CN_FT3_MSG_MAX];             // 报文体数据
    UINT32            dwMsgNum;                               // 帧数
    UINT32            dwMsgPtr;                               // 帧指针
}tagFT3RecMsg;

/*================================================================================*/
// 外部函数声明
/*================================================================================*/
extern      tagFT3RecMsg    g_tFT3RecMsg[CN_FT3_CHIP_MAX];
extern        void            FT3_Init(void);
extern        BOOL            FT3_Rec(UINT8 byChip);
extern        BOOL            FT3_Send(UINT8 byChip, UINT8 *pbyBuf, UINT32 dwLen);
extern        void            FT3_Delaytime(UINT32 dwMicrosecond);

/*================================================================================*/
#endif /* _WB828_FT3_H_ */





