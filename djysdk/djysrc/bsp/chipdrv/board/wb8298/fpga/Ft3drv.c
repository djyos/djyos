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
#include "sunri_types.h"
#include "Ft3drv.h"
#include "fpga_address.h"

#define FT3_DATA_BASE        (CN_FPGA_BASE_ADDR + 0x50400)
#define FT3_RXPLEN_BASE        (CN_FPGA_BASE_ADDR + 0x57400)
#define FT3_TXSTA            (CN_FPGA_BASE_ADDR + 0x57800)
#define FT3_TXDATA            (CN_FPGA_BASE_ADDR + 0x58000)
#define FT3_TXCTL            (CN_FPGA_BASE_ADDR + 0x58800)
#define FT3_RXKEEP            (CN_FPGA_BASE_ADDR + 0x58804)
#define FT3_RSTCTL            (CN_FPGA_BASE_ADDR + 0x58808)
#define FT3_REGLOCK            (CN_FPGA_BASE_ADDR + 0x5880C)
#define REGLOCK_OFF_VALUE        (0xAAAA6666)
#define REGLOCK_ON_VALUE        (0x6666AAAA)

tagFT3RecMsg   g_tFT3RecMsg[CN_FT3_CHIP_MAX];

// ============================================================================
// 函数功能：FT3控制接收延时，防止数据量过大。接收延时大小为FPGA接收一帧所用时间
// 输入参数：时间，单位为us
// 输出参数：无
// 返回值：  无
// ============================================================================
void FT3_Delaytime(UINT32 dwMicrosecond)
{
    register UINT32     dwDelay;                    // 实际延时ticket数

    if( dwMicrosecond>=200 )
    {
        dwMicrosecond -= CN_FT3_WAIT_IDLE*2/10;     // 空闲位等待延时(一个BIT延时0.2微秒)
    }
    dwDelay = CN_FT3_CRYSTALL_FRQ*dwMicrosecond;    // FT3硬件晶振频率
    (*(volatile UINT32*)(FT3_REGLOCK)) = REGLOCK_OFF_VALUE;    //写寄存器之前，先解锁
    (*(volatile UINT32*)(FT3_RXKEEP)) = dwDelay;
    (*(volatile UINT32*)(FT3_REGLOCK)) = REGLOCK_ON_VALUE;    //写寄存器后上锁
}

// ============================================================================
// 函数功能：FT3复位处理，对所有FT3口统一复位
//           建议在初始化和收发异常时调用
// 输入参数：无
// 输出参数：无
// 返回值：  无
// ============================================================================
void FT3_Init(void)
{
    register UINT32     dwLoop1;                    // 临时变量
    tagFT3RecMsg       *ptFT3RecMsg;                // 报文数据池

    // 置0复位
    (*(volatile UINT32*)(FT3_REGLOCK)) = REGLOCK_OFF_VALUE;    //写寄存器之前，先解锁
    (*(volatile UINT32*)(FT3_RSTCTL)) = 0;
    (*(volatile UINT32*)(FT3_REGLOCK)) = REGLOCK_ON_VALUE;    //写寄存器后上锁

    // 报文数据池
    for (dwLoop1=0; dwLoop1<CN_FT3_CHIP_MAX; dwLoop1++)
    {
        ptFT3RecMsg = &g_tFT3RecMsg[dwLoop1];       // 对每个通道的数据池进行初始化
        ptFT3RecMsg->dwMsgNum = 0;                  // 报文个数
        ptFT3RecMsg->dwMsgPtr = 0;                  // 报文指针
    }

    // 置1工作
    (*(volatile UINT32*)(FT3_REGLOCK)) = REGLOCK_OFF_VALUE;    //写寄存器之前，先解锁
    (*(volatile UINT32*)(FT3_RSTCTL)) = 1;
    (*(volatile UINT32*)(FT3_REGLOCK)) = REGLOCK_ON_VALUE;    //写寄存器后上锁

    // 网络抑制延时接收设置(全接收)
    FT3_Delaytime( 0 );
}

// ============================================================================
// 函数功能：FT3接收函数
// 输入参数：待接收端口，2~13
// 输出参数：
// ============================================================================
BOOL FT3_Rec(UINT8 byChip)
{
    register UINT32     dwLoop1, dwLoop2, dwVal;            // 临时变量
    register UINT32     *pdwptr;                            // 数据池读取地址
    register UINT32     dwMsgNow, dwMsgOld;                 // 帧位置
    register UINT32     dwMsgNum;                            // 报文包数
    register UINT8      *pbyMsgBuf;                         // 报文保存基址
    tagFT3RecMsg           *ptFT3RecMsg;                       // 报文数据池
    tagFT3Packet           *ptFT3Packet;                       // 报文体数据

    // 独立FT3数目容错
    if( byChip>=CN_FT3_CHIP_MAX )
    {
        return FALSE;
    }

    // 指针初始化
    ptFT3RecMsg = &(g_tFT3RecMsg[byChip]);

    // 判断是否有新报文
    dwMsgNow = (*(UINT32*)(FT3_RXPLEN_BASE+(4*byChip)));    // 得到当前帧指针
    dwMsgOld = ptFT3RecMsg->dwMsgPtr;                        // 上一次的帧指针
    ptFT3RecMsg->dwMsgPtr = dwMsgNow;                       // 保存当前帧指针
    if( dwMsgNow == dwMsgOld )
    {
        return FALSE;                                       // 没有新报文
    }
    else
    {
        if( dwMsgNow > dwMsgOld )
        {
            if( (dwMsgNow-dwMsgOld) <= CN_FT3_MAX_INTERVAL )
            {
                dwMsgNum = dwMsgNow - dwMsgOld;
            }
            else
            {
                return FALSE;                               // 接收的帧数不可能太多
            }
        }
        else                                                // 此时已经反转
        {
            if( (dwMsgNow+CN_FT3_MSG_MAX-dwMsgOld) <= CN_FT3_MAX_INTERVAL )
            {
                dwMsgNum = dwMsgNow + CN_FT3_MSG_MAX - dwMsgOld;
            }
            else
            {
                return FALSE;                               // 接收的帧数不可能太多
            }
        }
    }

    // 报文保存
    ptFT3RecMsg->dwMsgNum = dwMsgNum;                       // 帧数
    for(dwLoop1=0; dwLoop1<dwMsgNum; dwLoop1++)
    {
        if( (dwMsgOld+1) >= CN_FT3_MSG_MAX )
        {
            dwMsgOld = 0;                                   // 如果到达边界，需要翻转
        }
        else
        {
            dwMsgOld = dwMsgOld + 1;
        }

        // 把RAM里面的数据都读回来
        pdwptr      =  (UINT32 *)(FT3_DATA_BASE + (byChip << 10) + (dwMsgOld*4*CN_FT3_LEN_RECV_32BIT));
        ptFT3Packet = &ptFT3RecMsg->tFt3Packet[dwLoop1];
        pbyMsgBuf   = &ptFT3Packet->byPacket[0];
        for (dwLoop2=0; dwLoop2<CN_FT3_LEN_RECV_32BIT; dwLoop2++)
        {
            dwVal        = *pdwptr++;
            *pbyMsgBuf++ = (dwVal>>24)&0xff;
            *pbyMsgBuf++ = (dwVal>>16)&0xff;
            *pbyMsgBuf++ = (dwVal>>8)&0xff;
            *pbyMsgBuf++ = dwVal&0xff;
        }
    }
    return TRUE;
}

// ============================================================================
// 函数功能：FT3发送函数
// 输入参数：待发送端口：2;待发送的内容;发送数据长度(一次写固定64字节，长度固定为64，不需要CPU做CRC，底层做)
// 输出参数：
// ============================================================================
BOOL FT3_Send(UINT8 byChip, UINT8 *pbyBuf, UINT32 dwLen)
{
    register UINT32     dwVal_LL, dwVal_LH;         // 临时变量(双字低16位)
    register UINT32     dwVal_HL, dwVal_HH;         // 临时变量(双字高16位)
    register UINT32     *pdwptr;                    // 32位报文发送基址
    register UINT32     dwLoop1;                      // 临时变量

    // 报文长度容错
    if( dwLen>CN_FT3_LEN_SEND_32BIT*4 )
    {
        return FALSE;
    }

    // 独立FT3数目容错
    if( byChip>=CN_FT3_CHIP_MAX )
    {
        return FALSE;
    }

    // 真实FT3序号
    if( byChip>=2 )
    {
        byChip -= 2;                                    // 端口号从2开始
    }
    else
    {
        byChip = 0;
    }

    // 发送使能屏蔽
    (*(volatile UINT32*)(FT3_TXCTL)) = 0;

    // 报文写入FPGA内存
    pdwptr = (UINT32 *)(FT3_TXDATA);
    for(dwLoop1=0; dwLoop1<dwLen; dwLoop1+=4)       // 共循环64/4=16次
    {
        dwVal_LL  = *pbyBuf++;
        dwVal_LH  = *pbyBuf++;
        dwVal_HL  = *pbyBuf++;
        dwVal_HH  = *pbyBuf++;
        *pdwptr++ = dwVal_LL + (dwVal_LH<<8) + (dwVal_HL<<16) + (dwVal_HH<<24);
    }

    (*(volatile UINT32*)(FT3_REGLOCK)) = REGLOCK_OFF_VALUE;    //写寄存器之前，先解锁
    // 发送使能开放
    (*(volatile UINT32*)(FT3_TXCTL)) = 1 << byChip;
    (*(volatile UINT32*)(FT3_REGLOCK)) = REGLOCK_ON_VALUE;    //写寄存器后上锁
    return TRUE;
}
