/*
 * AX88796.c
 *
 *  Created on: 2015年10月27日
 *      Author: zqf
 */

/*
 * AX88796_Module1.c
 *
 *  Created on: 2015年6月4日
 *      Author: zhangqf
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <tcpip/pkg.h>
#include <tcpip/netdev.h>
#include <tcpip/tcpip_cdef.h>

#include "os.h"

#include "Ax88796Byte.h"

#define CN_DEV_MONITOR_CYCLE    (5*1000*mS)  //if 5s no pkg comes. reset the dev
#define CN_TSND_TIMEWAIT        1000      //单位：us,发送最多最多等待时间

//****************************************************************************/
//作者laogu  http://www.laogu.com
//程序的功能是根据多播地址multicast_address[6]的值，计算出MAR0-MAR7，就是multicast_filter[8];
//本程序指计算一个多播地址。如果有多个多播地址，将每个多播地址生成的multicast_filter[8]相或就可以了，
//例如根据多播地址 01:00:5e:00:00:01生成的 value1=multicast_filter[8];
//根据多播地址     01:00:5e:00:00:02生成的 value2=multicast_filter[8];
//那么对这两个多播地址生成的multicast_filter[8]=value1  |  value2 ;将两个值相或
//很容易从这里得到如果要接收所有多播地址的数据包，MAR0--MAR7必须设置为0xff,就是说
//multicast_filter[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

static const u32  CRC_32_Tbl[260]=
{0x00000000,0x077073096,0x0EE0E612C,0x0990951BA,0x0076DC419,0x0706AF48F,0x0E963A535,0x09E6495A3,0x00EDB8832,0x079DCB8A4,\
0x0E0D5E91E,0x097D2D988,0x009B64C2B,0x07EB17CBD,0x0E7B82D07,0x090BF1D91,0x01DB71064,0x06AB020F2,0x0F3B97148,0x084BE41DE,\
0x01ADAD47D,0x06DDDE4EB,0x0F4D4B551,0x083D385C7,0x0136C9856,0x0646BA8C0,0x0FD62F97A,0x08A65C9EC,0x014015C4F,0x063066CD9,\
0x0FA0F3D63,0x08D080DF5,0x03B6E20C8,0x04C69105E,0x0D56041E4,0x0A2677172,0x03C03E4D1,0x04B04D447,0x0D20D85FD,0x0A50AB56B,\
0x035B5A8FA,0x042B2986C,0x0DBBBC9D6,0x0ACBCF940,0x032D86CE3,0x045DF5C75,0x0DCD60DCF,0x0ABD13D59,0x026D930AC,0x051DE003A,\
0x0C8D75180,0x0BFD06116,0x021B4F4B5,0x056B3C423,0x0CFBA9599,0x0B8BDA50F,0x02802B89E,0x05F058808,0x0C60CD9B2,0x0B10BE924,\
0x02F6F7C87,0x058684C11,0x0C1611DAB,0x0B6662D3D,0x076DC4190,0x001DB7106,0x098D220BC,0x0EFD5102A,0x071B18589,0x006B6B51F,\
0x09FBFE4A5,0x0E8B8D433,0x07807C9A2,0x00F00F934,0x09609A88E,0x0E10E9818,0x07F6A0DBB,0x0086D3D2D,0x091646C97,0x0E6635C01,\
0x06B6B51F4,0x01C6C6162,0x0856530D8,0x0F262004E,0x06C0695ED,0x01B01A57B,0x08208F4C1,0x0F50FC457,0x065B0D9C6,0x012B7E950,\
0x08BBEB8EA,0x0FCB9887C,0x062DD1DDF,0x015DA2D49,0x08CD37CF3,0x0FBD44C65,0x04DB26158,0x03AB551CE,0x0A3BC0074,0x0D4BB30E2,\
0x04ADFA541,0x03DD895D7,0x0A4D1C46D,0x0D3D6F4FB,0x04369E96A,0x0346ED9FC,0x0AD678846,0x0DA60B8D0,0x044042D73,0x033031DE5,\
0x0AA0A4C5F,0x0DD0D7CC9,0x05005713C,0x0270241AA,0x0BE0B1010,0x0C90C2086,0x05768B525,0x0206F85B3,0x0B966D409,0x0CE61E49F,\
0x05EDEF90E,0x029D9C998,0x0B0D09822,0x0C7D7A8B4,0x059B33D17,0x02EB40D81,0x0B7BD5C3B,0x0C0BA6CAD,0x0EDB88320,0x09ABFB3B6,\
0x003B6E20C,0x074B1D29A,0x0EAD54739,0x09DD277AF,0x004DB2615,0x073DC1683,0x0E3630B12,0x094643B84,0x00D6D6A3E,0x07A6A5AA8,\
0x0E40ECF0B,0x09309FF9D,0x00A00AE27,0x07D079EB1,0x0F00F9344,0x08708A3D2,0x01E01F268,0x06906C2FE,0x0F762575D,0x0806567CB,\
0x0196C3671,0x06E6B06E7,0x0FED41B76,0x089D32BE0,0x010DA7A5A,0x067DD4ACC,0x0F9B9DF6F,0x08EBEEFF9,0x017B7BE43,0x060B08ED5,\
0x0D6D6A3E8,0x0A1D1937E,0x038D8C2C4,0x04FDFF252,0x0D1BB67F1,0x0A6BC5767,0x03FB506DD,0x048B2364B,0x0D80D2BDA,0x0AF0A1B4C,\
0x036034AF6,0x041047A60,0x0DF60EFC3,0x0A867DF55,0x0316E8EEF,0x04669BE79,0x0CB61B38C,0x0BC66831A,0x0256FD2A0,0x05268E236,\
0x0CC0C7795,0x0BB0B4703,0x0220216B9,0x05505262F,0x0C5BA3BBE,0x0B2BD0B28,0x02BB45A92,0x05CB36A04,0x0C2D7FFA7,0x0B5D0CF31,\
0x02CD99E8B,0x05BDEAE1D,0x09B64C2B0,0x0EC63F226,0x0756AA39C,0x0026D930A,0x09C0906A9,0x0EB0E363F,0x072076785,0x005005713,\
0x095BF4A82,0x0E2B87A14,0x07BB12BAE,0x00CB61B38,0x092D28E9B,0x0E5D5BE0D,0x07CDCEFB7,0x00BDBDF21,0x086D3D2D4,0x0F1D4E242,\
0x068DDB3F8,0x01FDA836E,0x081BE16CD,0x0F6B9265B,0x06FB077E1,0x018B74777,0x088085AE6,0x0FF0F6A70,0x066063BCA,0x011010B5C,\
0x08F659EFF,0x0F862AE69,0x0616BFFD3,0x0166CCF45,0x0A00AE278,0x0D70DD2EE,0x04E048354,0x03903B3C2,0x0A7672661,0x0D06016F7,\
0x04969474D,0x03E6E77DB,0x0AED16A4A,0x0D9D65ADC,0x040DF0B66,0x037D83BF0,0x0A9BCAE53,0x0DEBB9EC5,0x047B2CF7F,0x030B5FFE9,\
0x0BDBDF21C,0x0CABAC28A,0x053B39330,0x024B4A3A6,0x0BAD03605,0x0CDD70693,0x054DE5729,0x023D967BF,0x0B3667A2E,0x0C4614AB8,\
0x05D681B02,0x02A6F2B94,0x0B40BBE37,0x0C30C8EA1,0x05A05DF1B,0x02D02EF8D};

u16 CalcCRC32(u16 *temp)
{
    u32 CRC32=0xFFFFFFFF;
    u16 value;
    u8 DataBuff[6];
    int i;

    DataBuff[0] = temp[0]&0xff;
    DataBuff[1] = temp[1]&0xff;
    DataBuff[2] = temp[2]&0xff;
    DataBuff[3] = temp[3]&0xff;
    DataBuff[4] = temp[4]&0xff;
    DataBuff[5] = temp[5]&0xff;

    value = 0;

    CRC32=0xFFFFFFFF;
    for( i=0; i<6; ++i)
        CRC32 = CRC_32_Tbl[(CRC32^((u8 *)DataBuff)[i]) & 0xff] ^ (CRC32>>8);

    CRC32 = CRC32&0x00003F;

    if(CRC32&0x01)   value = value + 32;
    if(CRC32&0x02)   value = value + 16;
    if(CRC32&0x04)   value = value +  8;
    if(CRC32&0x08)   value = value +  4;
    if(CRC32&0x10)   value = value +  2;
    if(CRC32&0x20)   value = value +  1;

    return value;
}


//****************************************************************************/
// 获取网卡寄存器状态值，总线为8位操作
//****************************************************************************/
u8 Net_GetNetReg(tagAx88796Pri  *pri,u8 iReg)
{
    u16  *iAddr;
    u8    Result;
    u16   value;

    if(iReg&0x01)//奇地址
    {
        iAddr = (u16*)((ptu32_t)pri->devbase + (iReg&0xfe));
        value =*(u16 *)iAddr;
        Result = (u8)((value&0xFF00)>>8);
    }
    else
    {
        iAddr = (u16*)((u8 *)pri->devbase + iReg);
        value =*(u16 *)iAddr;
        Result = (u8)(value&0xFF);
    }
    return Result;
}


//****************************************************************************/
// 写寄存器，总线为8位操作
//****************************************************************************/
void Net_SetNetRegA(tagAx88796Pri  *pri,u8 iReg,u8 iValue)
{
    u8  *iAddr;
    iAddr = (u8*)((ptu32_t)pri->devbase + iReg);
    *(iAddr) = iValue;

    return ;
}

//****************************************************************************/
// 设置寄存器值，此8位寄存器的读取和写入状态值相同
//****************************************************************************/
void SetRegMAC(tagAx88796Pri  *pri,u8 iReg,u8 iValue)
{
    u8  *iAddr;
    iAddr = (u8*)((ptu32_t)pri->devbase + iReg);
    *(iAddr) = iValue;

    return ;
}

//****************************************************************************/
// 复位网卡芯片，通过读取其0x1F地址，即可复位网卡芯片
//****************************************************************************/
void Net_ChipRST(tagAx88796Pri  *pri)
{
    Net_GetNetReg(pri,0x1f);
    Djy_DelayUs(5000);
}
//****************************************************************************/
// 清网卡中断
//****************************************************************************/
void Net_ClrIntStatus(tagAx88796Pri  *pri)
{
    Net_SetNetRegA(pri,PORT_INTSTAT,0xFF);     // 清中断标志
    return;
}

//****************************************************************************/
// 初始化网卡芯片的Mac地址
//****************************************************************************/
void Net_ChipConfig(tagAx88796Pri  *pri)
{
    u8 data;
    Net_ChipRST(pri);                      // 复位芯片
    Net_ClrIntStatus(pri);                 // 清中断标志

    //开启中断:couter over,over write,transmit error,receive error, receive fram
    data = IM_CNTE|IM_OVWE|IM_TXEE|IM_RXEE|IM_PRXE;
    Net_SetNetRegA(pri,PORT_INTMASK,data);

    // 转到页面0，同时停止网卡工作，对寄存器进行配置
    Net_SetNetRegA(pri,PORT_CMD,CMD_NODMA|CMD_PAGE0|CMD_STOP); //page0/stop/complete remote DMA

    Net_SetNetRegA(pri,PORT_RBCR0,0x00);    // 清计数器0 clear Remote u8 Count 0
    Net_SetNetRegA(pri,PORT_RBCR1,0x00);    // 清计数器1 clear Remote u8 Count 1

    // 网卡初始化时，需要柚贸勺曰纺Ｊ?
    Net_SetNetRegA(pri,0x0c,0xe0);         // 接收控制寄存器monitor mode,no receive
    Net_SetNetRegA(pri,0x0d,0xe2);         // 发送控制寄存器loop back mode

}

//****************************************************************************/
// 对网卡芯片寄存器进行配置，源mac地址
//****************************************************************************/
void Net_ChipStart(tagAx88796Pri  *pri)
{
    u16 mac[6],value;
    u8 marH,marL;
 //   Net_PhyWriteReg(pri,0x00,0x2300);
//  Net_DelayUs(1000);
    // 设置接收缓冲区指针寄存器，70到126，环形接收
    // 转到页面0，同时停止网卡工作，对寄存器进行配置
    Net_SetNetRegA(pri,PORT_CMD,CMD_NODMA|CMD_PAGE0|CMD_STOP); //page0/stop/complete remote DMA
    Net_SetNetRegA(pri,0x01,CHIP_PSTART);   // 接收起始页寄存器，定义接收首指针
    Net_SetNetRegA(pri,0x02,CHIP_PSTOP);    // 接收终止页寄存器，定义终止指针
    Net_SetNetRegA(pri,0x03,CHIP_PSTART);   // 接收边界指针寄存器，定义目前接收指针位置
    Net_SetNetRegA(pri,0x04,CHIP_TSTART);   // 传送起始页面寄存器，定义发送起始指针位置
    Net_SetNetRegA(pri,0x05,0x00);          // BNRY
    Net_SetNetRegA(pri,0x06,0x00);          // TPSR
    // 设置总线工作方式16位，关闭loop back模式，初始化指针完毕
    Net_SetNetRegA(pri,0x0e, DCON_BSIZE1 | DCON_BUS_8 | DCON_LOOPBK_OFF);//Set dma 16 bite mode
    // 初始化MAC地址，在页面1寄存器中
    Net_SetNetRegA(pri,PORT_CMD,CMD_NODMA|CMD_PAGE1|CMD_STOP);//page1/stop/complete remote DMA
    Net_SetNetRegA(pri,0x01,pri->mac[0]);
    Net_SetNetRegA(pri,0x02,pri->mac[1]);
    Net_SetNetRegA(pri,0x03,pri->mac[2]);
    Net_SetNetRegA(pri,0x04,pri->mac[3]);
    Net_SetNetRegA(pri,0x05,pri->mac[4]);
    Net_SetNetRegA(pri,0x06,pri->mac[5]);
    // 初始化当前起始指针，指向接收缓冲区的下一页
    Net_SetNetRegA(pri,0x07,CHIP_PSTART+1);//CURR;
// 初始化MAR寄存器，组播地址过滤寄存器
    Net_SetNetRegA(pri,0x08,0x00);//Mar0
    Net_SetNetRegA(pri,0x09,0x00);//Mar1
    Net_SetNetRegA(pri,0x0A,0x00);//Mar2
    Net_SetNetRegA(pri,0x0B,0x00);//Mar3
    Net_SetNetRegA(pri,0x0c,0x00);//Mar4
    Net_SetNetRegA(pri,0x0d,0x00);//Mar5
    Net_SetNetRegA(pri,0x0e,0x00);//Mar6
    Net_SetNetRegA(pri,0x0f,0x00);//Mar7
//设置网卡的MAC的屏蔽码
    mac[0]=pri->mac[0];
    mac[1]=pri->mac[1];
    mac[2]=pri->mac[2];
    mac[3]=pri->mac[3];
    mac[4]=pri->mac[4];
    mac[5]=pri->mac[5];
    value = CalcCRC32((u16*)mac);
    marH = value / 8;
    marL = value % 8;
    marH = marH + 8;
    marL = (0x01 << marL);
    SetRegMAC(pri,marH,marL);       //接收目标地址为本网卡的MAC组播报文
    //SetEhterNetRecMACReg(pri);        //设置以太网口接收MAC屏蔽寄存器,
    mac[0]=0x01;
    mac[1]=0x79;
    mac[2]=0x77;
    mac[3]=0x70;
    mac[4]=0xff;
    mac[5]=0xff;
    value = CalcCRC32((u16*)mac);
    marH = value / 8;
    marL = value % 8;
    marH = marH + 8;
    marL = (0x01 << marL);
    SetRegMAC(pri,marH,marL);       //接收目标地址为本网卡的MAC组播报文
    // 转回页面0，准备设置页面0寄存器
    Net_SetNetRegA(pri,PORT_CMD,CMD_NODMA|CMD_PAGE0|CMD_STOP);  //page0/stop/complete remote DMA
    // 配置接收和发送控制寄存器
    Net_SetNetRegA(pri,0x0c,0x0C);// 只接收组播命令           xuzian  2006-11-29 10:43上午
    Net_SetNetRegA(pri,0x0d,0x00);// 正常发送
    Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);  //start work
    Net_SetNetRegA(pri,0x07,0xFF); // 清中断标志
}

//****************************************************************************/
// 将接收缓冲区数据保存在内存全局变量中
//****************************************************************************/
void Net_NICToPC(tagAx88796Pri *pri,u16 bnry,u16 length,u8 *Buffer,u16 flag)
{
    u16 i;
    u8* ioBufPort;
    u8 *tempbuf;

    Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);
    Net_SetNetRegA(pri,0x09,bnry&0xFF);                        // 置接收边界指针
    Net_SetNetRegA(pri,0x08,0x00);
    Net_SetNetRegA(pri,0x0B,(length >>8) & 0xff);         // 柚檬莅ざ?
    Net_SetNetRegA(pri,0x0a,length & 0xff);
    Net_SetNetRegA(pri,PORT_CMD, CMD_RREAD | CMD_START);   // 设置网卡进入读取数据状态
    ioBufPort=(u8*)(pri->devbase + 0x10);
    tempbuf = Buffer;
    for(i=0;i<length;i++)
    {
        *tempbuf++=*(ioBufPort);  // 读取数据总线上数据
    }

    Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);
}

//****************************************************************************/
// 网口数据接收函数
//****************************************************************************/
u16 Net_RecvPacket(tagAx88796Pri *pri,u8* buf,u16 buflen)
{
    register        u16 bnry,curr;
    register        u16 length;
    tagAx88796FH    RecFram,*ptRecFrame;

    if(pri->devflag & (CN_DEV_RERR|CN_DEV_ROVR))            // 网卡溢出停止网卡工作
        return 0;
    ptRecFrame = &RecFram;
    Net_SetNetRegA(pri,PORT_CMD,CMD_NODMA | CMD_PAGE0 | CMD_START);
    bnry=Net_GetNetReg(pri,0x03);    // 获取接收缓冲区边界指针，每进入一个报文，指针加一
    Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE1 | CMD_START);
    curr=Net_GetNetReg(pri,0x07);    // 获取接收缓冲区当前指针，每处理一个报文，指针加一
    Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);
    if(curr==0)
        return 0;
    bnry++;
    if(bnry>=CHIP_PSTOP) bnry=CHIP_PSTART;//reset from pstop=pstart,form circuity structure

    if(bnry==curr)
    {
        return 0;   //NO New INPUT
    }
    Net_NICToPC(pri,bnry,4,(u8*)(ptRecFrame),0);  //Read Head INTO Memory

    if(((ptRecFrame->rstat & 0x01)==0)||(ptRecFrame->next>=CHIP_PSTOP)
        ||(ptRecFrame->next<CHIP_PSTART)||(ptRecFrame->uppByteCnt>0x06))//ERROR Message in Head 0x0600=1536 最长帧为1522
    {
        Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE1 | CMD_STOP);   // 切换到page1
        /*错误的情况可能伴随网卡内部环形缓冲区的写指针越界，因此首先要重新初始化写指针，否则后面的代码将写指针赋值给读指针，会导致读指针也越界*/
        Net_SetNetRegA(pri,0x07,CHIP_PSTART+1);
        curr=Net_GetNetReg(pri,0x07);                                 // 获取接收缓冲区当前指针
        Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE0 | CMD_STOP);//切换到page0
        /*网卡内部DMA计数器清0*/
        Net_SetNetRegA(pri,0x0B,0);
        Net_SetNetRegA(pri,0x0A,0);
        bnry=curr-1;
        if(bnry<CHIP_PSTART)bnry=CHIP_PSTOP-1;
        Net_SetNetRegA(pri,0x03,bnry);                 //设置接收缓冲区边界指针
        Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);//切换到page0
        return 0;
    }
    length=(ptRecFrame->uppByteCnt<<8)+ptRecFrame->lowByteCnt;

    if(length <= buflen)
    {
        Net_NICToPC(pri,bnry,length,buf,1);
    }
    else
    {
        length =0;
    }
    bnry=ptRecFrame->next-1;
    if(bnry<CHIP_PSTART)bnry=CHIP_PSTOP-1;
        Net_SetNetRegA(pri,0x03,bnry);                 //设置接收缓冲区边界指针
    return length;
}

//****************************************************************************/
// 将缓存的数据写入网卡
//****************************************************************************/
bool_t Net_PCToNIC(tagAx88796Pri *pri,u16 length,u8 *Buffer)
{

    bool_t      result = false;
    register    u16 i;
    register    u8 *addr;
    u8          cmdStat;
    u8          tsr;
    u32         timeout =0;

    Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);
    cmdStat=Net_GetNetReg(pri,PORT_CMD);

    if(cmdStat & CMD_TXP)
        return result;

    Net_SetNetRegA(pri,0x08,00);
    Net_SetNetRegA(pri,0x09,CHIP_TSTART);                // 置发送起始指针
    Net_SetNetRegA(pri,0x0a,length & 0xff);
    Net_SetNetRegA(pri,0x0b,(length >>8) & 0xff);        // 设置发送数据包长度

    Net_SetNetRegA(pri,PORT_CMD, CMD_RWRITE | CMD_START); // 网卡进入写数据状态

    addr = (u8 *)(pri->devbase+0x10);
    for(i=0;i<length;i++)
    {
        *addr  = *Buffer++;
    }
    // there are many diffrent codes about this part.
    Net_SetNetRegA(pri,PORT_TSTART, CHIP_TSTART);
    Net_SetNetRegA(pri,PORT_TCNTH,(length >> 8)& 0xff);
    Net_SetNetRegA(pri,PORT_TCNTL, length & 0xff);
    Net_SetNetRegA(pri,PORT_CMD, CMD_TXP | CMD_START);
    Net_SetNetRegA(pri,PORT_CMD, CMD_NODMA | CMD_PAGE0 | CMD_START);

    //等待发送完毕
    while(timeout < CN_TSND_TIMEWAIT)
    {
        tsr = Net_GetNetReg(pri,PORT_TSTAT);
        if(tsr&(TSTAT_OWC|TSTAT_ABT|TSTAT_COL))
        {
            result = true;
            pri->devflag |= CN_DEV_TERR;
            break;
        }
        else if(tsr |TSTAT_PTX)
        {
            result = true;
            break;
        }
        else
        {
            if(timeout == (CN_TSND_TIMEWAIT-1))
            {
                result = false;
                pri->devflag |= CN_DEV_TERR;
                printf("%s:TimeOut:Len:%d\n\r",__FUNCTION__,length);
                break;
            }
            else
            {
                timeout++;
                Djy_DelayUs(1); //每次延时1us
            }

        }
    }

    return result;
}
//****************************************************************************/
// 发送数据包函数
//****************************************************************************/
static bool_t __Ax88796BufSnd(tagAx88796Pri *pri,u8* buf,u16 iLength)
{
    bool_t  result = false;
    if(Lock_MutexPend(pri->devsync,CN_TIMEOUT_FOREVER))
    {
        if((pri->devflag &(CN_DEV_TERR|CN_DEV_TOVR))||\
            (iLength > CN_FRAME_MAX)||(iLength < CN_FRAME_MIN)||(NULL == buf))
        {
            result = false;
        }
        else
        {
            result = Net_PCToNIC(pri,iLength,(u8*)buf);
        }

        Lock_MutexPost(pri->devsync);
    }

    return result;
}
//用于88796的复位
static bool_t __Ax88796Rst(tagAx88796Pri  *pri)
{
    atom_low_t  atom;

    atom = Int_LowAtomStart();
    //先硬件复位
    if(NULL != pri->fnRstDealer)
    {
        pri->fnRstDealer((void *)pri);
    }
    //再软件复位
    Net_ChipRST(pri);
    //重新硬件配置
    Net_ChipConfig(pri);
    Net_ChipStart(pri);
    //状态量的复位
    pri->devflag =CN_DEV_OK;

    Int_LowAtomEnd(atom);

    return true;
}
// =============================================================================
// 函数功能：发送数据包
// 输入参数：netdev,使用的网卡
//        pkglst,待发送的数据包链表
//        netdevtask,网卡需要完成的工作
// 输出参数：
// 返回值  ：true发送成功  false发送失败。
// 说明    ：采用拷贝的方式发送，后续考虑使用链表发送
// =============================================================================
bool_t Ax88796Snd(tagNetDev *dev,tagNetPkg *pkglst,u32 netdevtask)
{
    bool_t  result;
    tagNetPkg *tmp;
    u8 *src;
    u8 *dst;
    u8 *buf;
    u16 sndlen;
    tagAx88796Pri *pri;

    result = false;

    if((NULL == pkglst)||(netdevtask != CN_IPDEV_NONE)||(NULL ==dev))
    {
        printk("%s:Err Send Para!\n\r");
        return false;
    }

    pri = (tagAx88796Pri *)(NetDev_GetDevPrivate(dev));
    buf = pri->sndbuf;
    if(Lock_MutexPend(pri->devsync,CN_TIMEOUT_FOREVER))
    {
        dst = buf;
        //move all the package to the send buf
        tmp = pkglst;
        sndlen = 0;
        while(NULL != tmp)
        {
            src = (u8 *)(tmp->buf + tmp->offset);
            memcpy((void *)dst,(void *)src,tmp->datalen);
            sndlen +=tmp->datalen;
            dst+=tmp->datalen;
            if(PKG_ISLISTEND(tmp))
            {
                tmp = NULL;
            }
            else
            {
                tmp = tmp->partnext;
            }
        }
        //ok, call the api to send the data
        result = Net_PCToNIC(pri,sndlen,buf);
        Lock_MutexPost(pri->devsync);
        result = true;

    }
    return result;
}
// =============================================================================
// 功能：从网卡拷贝数据到设备的缓冲区
// 参数：
// 返回 ：
// =============================================================================
static u16  __DevRcvFunc(tagAx88796Pri  *pri)
{
    u16         len;

    if(Lock_MutexPend(pri->devsync,CN_TIMEOUT_FOREVER))
    {
        len = Net_RecvPacket(pri,pri->rcvbuf,CN_BUF_LEN);
        Lock_MutexPost(pri->devsync);
    }
    return len;
}

static ptu32_t __DevRcvMain(void)
{
    u16             len;
    rcvDealer       fnHook;
    u8              *src;
    u8              *dst;
    tagNetPkg       *pkg;
    tagAx88796Pri   *pri = NULL;
    Djy_GetEventPara((ptu32_t *)&pri,NULL);
    if(NULL != pri)
    {
        fnHook = pri->fnRcvDealer;
        while(1)
        {
            if(Lock_SempPend(pri->rcvsync,pri->loopcycle)) //一个信号灯可能有多包数据
            {
                while((len = __DevRcvFunc(pri))>0)
                {
                    if(NULL != fnHook) //数据不由协议栈处理
                    {
                        fnHook(pri,pri->rcvbuf,len);
                    }
                    else  //数据流进协议栈
                    {
                        pkg = PkgMalloc(len,CN_PKLGLST_END);
                        if(NULL != pkg)
                        {
                            dst = (u8 *)(pkg->offset+ pkg->buf);
                            src = (u8 *)pri->rcvbuf;
                            src += 4;  //前面四字节为包头信息
                            memcpy((void *)dst,(void *)src, len);
                            pkg->datalen = len;
                            NetDev_PostPkg(pri->devhandle,pkg);
                            PkgTryFreePart(pkg);
                        }
                    }
                    pri->monitorRcv++;
                }
            }
        }
    }
    return 0;
}

//网卡的接收监视任务，一定时间内没有数据包接收，则复位网卡
static ptu32_t __DevRcvMonitor(void)
{
    tagAx88796Pri  *pri = NULL;
    u32             chippacks;

    Djy_GetEventPara((ptu32_t *)&pri,NULL);
    if(NULL == pri)
    {
        goto MONITOR_EXIT;
    }
    while(1)
    {
        chippacks = pri->monitorRcv;
        Djy_EventDelay(CN_DEV_MONITOR_CYCLE);
        if(Lock_MutexPend(pri->devsync,CN_TIMEOUT_FOREVER))
        {
            if(chippacks == pri->monitorRcv) //no pack rcved any more, reset the dev
            {
                __Ax88796Rst(pri);
            }
            Lock_MutexPost(pri->devsync);
        }
    }
MONITOR_EXIT:
    printk("%s:%s MonitorErr!\n\r",__FUNCTION__,pri->name);
    return 0;
}

static u16 __taskCreate(char *name,u32 prior,void *routine,u32 stacksize,ptu32_t para)
{

    u16 evttID;
    u16 eventID = CN_EVENT_ID_INVALID;
    evttID = Djy_EvttRegist(EN_CORRELATIVE, prior, 0, 1,\
                            (ptu32_t (*)(void))routine,NULL, stacksize, name);

    if (evttID != CN_EVTT_ID_INVALID)
    {
        eventID=Djy_EventPop(evttID, NULL, 0, (ptu32_t)para, 0, 0);
        if(eventID == CN_EVENT_ID_INVALID)
        {
            Djy_EvttUnregist(evttID);
        }
    }
    return eventID;
}
//中断的处理太简陋,应该处理溢出等等中断--TODO
static u32 __Ax88796Isr(ptu32_t data)
{
    u8 isr;
    tagAx88796Pri *pri;

    pri = (tagAx88796Pri *)data;
    isr = Net_GetNetReg(pri,PORT_INTSTAT);
    Net_ClrIntStatus(pri);  //清理中断

    if(isr & ISTAT_CNT)
    {
        pri->cnt++;
        __Ax88796Rst(pri);
    }
    if(isr& ISTAT_OVW)
    {
        pri->ovw++;
        __Ax88796Rst(pri);
    }
    if(isr&ISTAT_TXE)
    {
        pri->txe++;
        __Ax88796Rst(pri);
    }
    if(isr &ISTAT_RXE)
    {
        pri->rxe++;
        __Ax88796Rst(pri);
    }
    if(isr &ISTAT_PTX)
    {
        //发送中断
    }
    if(isr&ISTAT_PRX)  //接收到数据
    {
        Lock_SempPost(pri->rcvsync);
    }

    return 0;
}

//安装中断处理
static void __Ax88796IntInstall(tagAx88796Pri  *pri)
{
    ufast_t  irqno;
    irqno = pri->irqno;
    //apply the interrupt and bind it to the isr
    if(true == Int_Register(irqno))
    {
        Int_SetClearType(irqno,CN_INT_CLEAR_AUTO);
        Int_IsrConnect(irqno,__Ax88796Isr);
        Int_SettoAsynSignal(irqno);
        Int_ClearLine(irqno);
        Int_RestoreAsynLine(irqno);
        Int_SetIsrPara(irqno,(ptu32_t)pri);
    }
}

//初始化网卡并将网卡添加进协议栈中
tagAx88796Pri *Ax88796Install(tagAx88796Para *para)
{
    tagNetDevPara   devpara;
    tagAx88796Pri  *ax88796 = NULL;

    ax88796 = (tagAx88796Pri *)malloc(sizeof(tagAx88796Pri));
    if(NULL == ax88796)
    {
        goto INSTALL_END;
    }
    memset((void *)ax88796,0,sizeof(tagAx88796Pri));

    //采用配置的初始化
    ax88796->devbase = para->devbase;
    ax88796->stacksize = para->stacksize;
    ax88796->fnRstDealer = para->fnRstDealer;
    ax88796->fnRcvDealer = para->fnRcvDealer;
    ax88796->fnSndDealer = (sndDealer)__Ax88796BufSnd;
    memcpy((void *)ax88796->mac,(void *)para->mac,CN_MACADDR_LEN);
    memcpy((void *)ax88796->name,(void *)para->name,CN_NAME_LEN);
    //进行一些量的申请
    ax88796->devsync = Lock_MutexCreate(NULL);
    if(NULL == ax88796->devsync)
    {
        goto DEVSYNC_FAILED;
    }
    ax88796->rcvsync = Lock_SempCreate(1,0,CN_BLOCK_FIFO,NULL);
    if(NULL == ax88796->rcvsync)
    {
        goto RCVSYNC_FAILED;
    }
    ax88796->taskRcvID = __taskCreate(ax88796->name,CN_PRIO_REAL+1,(void *)__DevRcvMain,\
            ax88796->stacksize, (ptu32_t)ax88796);
    ax88796->taskMonitorID = __taskCreate(NULL,CN_PRIO_RRS+1,(void *)__DevRcvMonitor,\
            ax88796->stacksize, (ptu32_t)ax88796);

    if((CN_EVENT_ID_INVALID == ax88796->taskMonitorID)||\
            (CN_EVENT_ID_INVALID == ax88796->taskRcvID))
    {
        goto TASK_FAILED;
    }
    //INSTALL THE IRQ
    if(para->useirq)
    {
        ax88796->irqno = para->irqno;
        ax88796->loopcycle = CN_TIMEOUT_FOREVER;
        __Ax88796IntInstall(ax88796);
    }
    else
    {
        ax88796->loopcycle = para->loopcycle;
    }

    //初始化完了设备数据，那么重新复位和配置网卡吧
    __Ax88796Rst(ax88796);

    //向协议栈注册网卡吧
    devpara.ifsend = Ax88796Snd;
    devpara.iftype = EN_LINK_INTERFACE_ETHERNET;
    devpara.devfunc = CN_IPDEV_NONE;    //NO FUNC FOR THE DEV
    memcpy(devpara.mac, ax88796->mac,CN_MACADDR_LEN);
    devpara.name = ax88796->name;
    devpara.private = (ptu32_t)ax88796;
    devpara.linklen = 14;
    devpara.pkglen = 1500;
    ax88796->devhandle = (void *) NetDev_InstallDev(&devpara);
    if(NULL == ax88796->devhandle)
    {
        goto DEV_FAILED;
    }
    else
    {
        goto INSTALL_END;
    }
    //安装完成

DEV_FAILED:
    //DEL THE TASK HERE
TASK_FAILED:
    Lock_SempDelete(ax88796->rcvsync);
RCVSYNC_FAILED:
    Lock_MutexDelete(ax88796->devsync);
DEVSYNC_FAILED:
    free((void *)ax88796);
    ax88796 = NULL;
INSTALL_END:
    if(NULL == ax88796)
    {
        printf("%s:Install:%s Install Err\n\r",__FUNCTION__,para->name);
    }
    else
    {
        printf("%s:Install:%s Install OK\n\r",__FUNCTION__,para->name);
    }

    return ax88796;
}








