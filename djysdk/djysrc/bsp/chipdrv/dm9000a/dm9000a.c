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
// 文件名     ：dm9000c.c
// 模块描述: DM9000cep网卡驱动，按DJYOS的以太网协议实现要求实现的网卡底层驱动
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 18/09.2014
// =============================================================================
#include <sys/socket.h>

#include "dm9000a.h"

#ifndef DM9000_DBG
#define DM9000_DBG       printk
#endif

#define CN_PKG_MAX_LEN   1500
#define CN_DM9000NAME_LEN  32
typedef struct
{
	char   devname[CN_DM9000NAME_LEN];
	u8     devmac[CN_MACADDR_LEN];
    struct SemaphoreLCB rcvsync;
    struct MutexLCB     devsync;
    ptu32_t handle;
    u32 irqno;
    bool_t (*clearextint)(u32 irqno);
    u16 *cmdaddr;
    u16 *dataddr;
}tagDm9000Dev;

u8  regread(tagDm9000Dev *dm9000,u16 reg)
{
	u16  data;
	*dm9000->cmdaddr = reg;
	data = *dm9000->dataddr;
	return (u8)data;
}
void  regwrite(tagDm9000Dev *dm9000,u16 reg,u16 data)
{
	*dm9000->cmdaddr = reg;
	*dm9000->dataddr = data;
	return;
}

static u8  regTab[]={0,1,2,3,4,5,6,7,8,9,10,254,255};
#define CN_REGLEN  sizeof(regTab)

void __showDm9000Reg(tagDm9000Dev *dm9000)
{
	int i;
	u8  value;
	for(i =0;i < CN_REGLEN;i++ )
	{
		value = regread(dm9000,regTab[i]);
		printk("reg:0x%02x:value:0x%02x\n\r",regTab[i],value);
	}

	return;
}

static bool_t __dm9000Probe(tagDm9000Dev *dm9000)
{
    u32 id_val;

    id_val  = regread(dm9000,DM9KS_VID_L);
    id_val |= regread(dm9000,DM9KS_VID_H) << 8;
    id_val |= regread(dm9000,DM9KS_PID_L) << 16;
    id_val |= regread(dm9000,DM9KS_PID_H) << 24;
    if (id_val == DM9KS_ID)
    {
        DM9000_DBG("dm9000 i/o:  id: 0x%x \r\n",id_val);
        return true;
    }
    else
    {
        DM9000_DBG("dm9000 not found !\r\n");
        return false;
    }
}
// =============================================================================
// 功  能: 写dm9000 PHY 寄存器
// 参  数: phy_reg ： 寄存器地址
//       writedata ： 写入的数据
// 返  回: 无
// =============================================================================
static void __dm9000PhyWrite(tagDm9000Dev *dm9000,u8 phy_reg, u16 writedata)
{
    /* 设置写入 PHY 寄存器的位置 */
    regwrite(dm9000,DM9000_REG_EPAR, phy_reg | DM9000_PHY);

    /* 设置写入 PHY 寄存器的值 */
    regwrite(dm9000,DM9000_REG_EPDRH, ( writedata >> 8 ) & 0xff);
    regwrite(dm9000,DM9000_REG_EPDRL, writedata & 0xff);

    regwrite(dm9000,DM9000_REG_EPCR, 0x0a);                       /* 将资料写入 PHY 寄存器 */
    while(regread(dm9000,DM9000_REG_EPCR) & 0x01); //todo 加退出条件  /* 查寻是否执行结束 */
    regwrite(dm9000,DM9000_REG_EPCR, 0x08);                       /* 清除写入命令 */
}
// =============================================================================
// 功  能: 对dm9000E进行软件复位
// 参数：无
// 返回：无
// =============================================================================
static  void __dm9000Reset(tagDm9000Dev *dm9000)
{

	if(Lock_MutexPend(&dm9000->devsync,CN_TIMEOUT_FOREVER))
	{
	    regwrite(dm9000,DM9000_REG_NCR, DM9000_REG_RESET);            /* 对 dm9000 进行软件重置 */
	    Djy_DelayUs(10);                                /* delay 10us */
	    regwrite(dm9000,DM9000_REG_NCR, DM9000_REG_RESET);            /* 对 dm9000 进行软件重置 */
	    Djy_DelayUs(10);                                /* delay 10us */

	    /* 基本记存器相关设置 */
	    regwrite(dm9000,DM9000_REG_IMR, DM9000_IMR_OFF);          /* 开启内存自环模式 */
	    regwrite(dm9000,DM9000_REG_TCR2, DM9000_TCR2_SET);        /* 设置 LED 显示模式1:全双工亮，半双工灭 */

	    /* 清除多余资讯 */
	    regwrite(dm9000,DM9000_REG_NSR, 0x2c);
	    regwrite(dm9000,DM9000_REG_TCR, 0x00);
	    regwrite(dm9000,DM9000_REG_ISR, 0x3f);

	#ifdef dm9000_FLOW_CONTROL
	    regwrite(dm9000,DM9000_REG_BPTR, DM9000_BPTR_SET);            /* 半双工流控设置 */
	    regwrite(dm9000,DM9000_REG_FCTR, DM9000_FCTR_SET);            /* 全双工流控设置 */
	    regwrite(dm9000,DM9000_REG_FCR, DM9000_FCR_SET);          /* 开启流控设置 */
	#endif

	#ifdef dm9000_UPTO_100M
	    /* dm9000无此寄存器 */
	    regwrite(dm9000,DM9000_REG_OTCR, DM9000_OTCR_SET);            /* 工作频率到 100Mhz 设置 */
	#endif

	    regwrite(dm9000,DM9000_REG_IMR, DM9000_IMR_SET);          /* 开启 中断模式 */

	    regwrite(dm9000,DM9000_REG_RCR, 0x39/*DM9000_RCR_SET*/);

    	Lock_MutexPost(&dm9000->devsync);
	}
         /* 开启 接收工能 */
}


// =============================================================================
// 功能：设置 dm9000 MAC 、 广播 、 多播 寄存器
// 参数：无
// 返回：无
// =============================================================================
static void dm9000_hash_table(tagDm9000Dev *dm9000)
{
    u8 i;
    u8 mac[6],*p;

    if(1)
    {
        p   =mac;
        for(i = 0; i < 6; i++)
        {
            p[i] =regread(dm9000,DM9000_REG_PAR + i);
        }

        DM9000_DBG("DM_MAC:  %02X,%02X,%02X,%02X,%02X,%02X.\r\n",
                    p[0],p[1],p[2],p[3],p[4],p[5]);
    }

    /* 设置 网卡 MAC 位置，来自於 MyHardware */
    for(i = 0; i < 6; i++)
    {
        regwrite(dm9000,DM9000_REG_PAR + i, dm9000->devmac[i]);
    }

    for(i = 0; i < 8; i++)                              /* 清除 网卡多播设置 */
    {
        regwrite(dm9000,DM9000_REG_MAR + i, 0x00);
    }

    regwrite(dm9000,DM9000_REG_MAR + 7, 0x80);                    /* 速设置 广播包 设置 */
}

// =============================================================================
//功  能: 初始化dm9000E网卡，包括哈希表，相关的寄存器配置等
// 参数：无
// 返回：无
// =============================================================================
static void __dm9000HardInit(tagDm9000Dev *dm9000)
{
	u32 timeout = 1000;
	regwrite(dm9000,DM9000_REG_NCR, DM9000_REG_RESET);            /* 对 dm9000 进行软件重置 */
    Djy_DelayUs(10);                                /* delay 10us */

    dm9000_hash_table(dm9000);                               /* 设置 dm9000 MAC 及 多播*/

    __dm9000Reset(dm9000);                                    /* 进行 dm9000 软件设置 */

    regwrite(dm9000,DM9000_REG_GPR, DM9000_PHY_OFF);          /* 关闭 PHY ，进行 PHY 设置*/
    __dm9000PhyWrite(dm9000,0x00, 0x8000);                    /* 重置 PHY 的寄存器 */
#ifdef dm9000_FLOW_CONTROL
    __dm9000PhyWrite(dm9000,0x04, 0x01e1 | 0x0400);           /* 设置 自适应模式相容表 */
#else
    __dm9000PhyWrite(dm9000,0x04, 0x01e1);                    /* 设置 自适应模式相容表 */
#endif
    //__dm9000PhyWrite(dm9000,0x00, 0x1000);                  /* 设置 基本连接模式 */
    /* 连接模式设置
      0x0000 : 固定10M半双工
      0x0100 : 固定10M全双工
      0x2000 : 固定100M半双工
      0x2100 : 固定100M全双工
      0x1000 : 自适应模式
    */
    __dm9000PhyWrite(dm9000,0x00, 0x1000);                   /* 设置 基本连接模式 */

    regwrite(dm9000,DM9000_REG_GPR, DM9000_PHY_ON);  /* 结束 PHY 设置, 开启 PHY */


    while(!(regread(dm9000,DM9000_NSR) & NSR_LINKST))
    {
    	if(timeout == 0)
    	{
    		DM9000_DBG("DM9000 Link failed!\r\n");
    		break;
    	}
    	timeout--;
    	Djy_EventDelay(1000);
    }

    if(timeout)
    	DM9000_DBG("DM9000 Linked!\r\n");
}

// =============================================================================
// 函数功能：NetHard_Send
//          网卡发送数据包
// 输入参数：netdev,使用的网卡
//        pkg,待发送的数据包
//        netdevtask,网卡需要完成的工作
// 输出参数：
// 返回值  ：true发送成功  false发送失败。
// 说明    ：采用拷贝的方式发送，后续考虑使用链表发送
// =============================================================================
static bool_t __dm9000Snd(ptu32_t handle,tagNetPkg *pkg,u32 netdevtask)
{
    bool_t  result;
    tagNetPkg *tmp;
    u16 *mysrc;
    u16 sndlen;
    u16 i;
    tagDm9000Dev  *dm9000;

    result = false;
    if((0 != handle)&&(NULL != pkg))
    {
    	dm9000 = (tagDm9000Dev  *)NetDevPrivate(handle);
        sndlen = 0;
        tmp = pkg;
        //cout the len
        tmp = pkg;
        sndlen = 0;
        while(NULL != tmp)
        {
            sndlen +=tmp->datalen;
            if(PKG_ISLISTEND(tmp))
            {
                tmp = NULL;
            }
            else
            {
                tmp = tmp->partnext;
            }
        }
        if(Lock_MutexPend(&dm9000->devsync,CN_TIMEOUT_FOREVER))
        {
            /* 检查 dm9000 是否还在传送中！若是等待直到传送结束 */
            while(regread(dm9000,DM9000_REG_TCR) & DM9000_TCR_SET)
            {
                Djy_DelayUs (5);
            }

            //snd all the pkg
            tmp = pkg;
            //init the dm9000
            regwrite(dm9000,DM9000_TXPLH, (sndlen>>8) & 0x0ff);//设置发送数据长度
            regwrite(dm9000,DM9000_TXPLL, sndlen & 0x0ff);

            *dm9000->cmdaddr = DM9000_REG_MWCMD;                    //发送数据缓存赋予数据端口

            while(NULL!= tmp)
            {
                sndlen = tmp->datalen;
                mysrc = (u16 *)(tmp->buf + tmp->offset);
                //发送数据
                for(i=0;i<sndlen;i+=2)
                {
                    *dm9000->dataddr = *mysrc++;  //8位数据转换为16位数据输出
                }
                if(PKG_ISLISTEND(tmp))
                {
                    tmp = NULL;
                }
                else
                {
                    tmp = tmp->partnext;
                }
            }
            //ok now start transfer;
            regwrite(dm9000,DM9000_REG_TCR, DM9000_TCR_SET);  /* 进行传送 */

    //        while((dm9000_reg_read(DM9000_NSR) & 0x0c) == 0)
    //        ;                                               //等待数据发送完成
            regwrite(dm9000,DM9000_NSR, 0x2c);                 //清除TX状态

            result = true;

        	Lock_MutexPost(&dm9000->devsync);
        }

    }
    return result;
}
// =============================================================================
// 函数功能：NetHard_Rcv
//          网卡读数据函数
// 输入参数：无
// 输出参数：
// 返回值  ：
// 说明    ：网卡读数据
// =============================================================================
static tagNetPkg *__dm9000RcvPkg(tagDm9000Dev *dm9000)
{
    s32   rx_length,i;
    u8    rx_ready;
    u16 rx_status;
    tagNetPkg *pkg=NULL;
    u16 *dst;

    if(Lock_MutexPend(&dm9000->devsync,CN_TIMEOUT_FOREVER))
    {
        rx_ready = regread(dm9000,DM9000_MRCMDX);      //先读取一个无效的数据,一般读到0
        rx_ready = regread(dm9000,DM9000_MRCMDX);     //真正读取到的数据包首字节

        if(rx_ready == 1)                           //判读首字节是否为1或0
        {
        	*dm9000->cmdaddr = DM9000_MRCMD;            //连续读取数据包内容
            rx_status = *dm9000->dataddr;               //状态字节
            rx_length = *dm9000->dataddr;               //数据长度
            rx_status &= 0xff;

            if((rx_length > 0) && (rx_length <= CN_PKG_MAX_LEN))
            {
                if(rx_length%2)
                    pkg =PkgMalloc(rx_length+1,0);
                else
                    pkg =PkgMalloc(rx_length,0);
                pkg->partnext = NULL;
            }
            if(pkg != NULL)
            {
                dst = (u16 *)(pkg->buf + pkg->offset);
                if(!(rx_status & 0xbf00) && (rx_length < 10000))//判读数据是否符合要求
                {
                    for(i=0; i<rx_length; i+=2)          //16位数据转换为8位数据存储
                    {
                        *dst++ = *dm9000->dataddr;
                    }
                }
                pkg->datalen = rx_length;
                pkg->partnext= NULL;
            }
        }

    	Lock_MutexPost(&dm9000->devsync);
    }


    return pkg;
}


#define CN_DM9000RCV_TIMEOUT   (1*mS)    //5 seconds timeout
static ptu32_t dm9000Rcv(void)
{
    tagNetPkg *pkg;
	tagDm9000Dev *dm9000;

    Djy_GetEventPara((ptu32_t *)&dm9000,NULL);
    while(1)
    {
        Lock_SempPend(&dm9000->rcvsync,CN_DM9000RCV_TIMEOUT);
        while((pkg = __dm9000RcvPkg(dm9000))!= NULL)
        {
            LinkPost(dm9000->handle,pkg);
            PkgTryFreePart(pkg);
        }
    	//rcv overflow will reset the dm9000a
        u8  rcvstat;
    	u32 times = 0;

        rcvstat = regread(dm9000,DM9000_REG_NSR);
        if(rcvstat &NSR_RXOV)
        {
        	times++;
        	rcvstat = regread(dm9000,DM9000_REG_RSR);
        	printk("RSR  :0X%02x\n\r",rcvstat);
        	rcvstat = regread(dm9000,DM9000_REG_ROCR);
        	printk("ROCR :0X%02x\n\r",rcvstat);


    		__dm9000Reset(dm9000);
    		printk("%s:dm9000a reset for rcv overflow--%08d times\n\r",\
    				__FUNCTION__,times++);
        }
    }
    return 0;
}
u32  __dm9000Isr(ptu32_t para)
{
	tagDm9000Dev *dm9000;
    u8 IntStatus;
    dm9000 = (tagDm9000Dev *)para;

    regwrite(dm9000,DM9000_REG_IMR , DM9000_IMR_OFF); //关闭 DM9000A 中断

    IntStatus = regread(dm9000,DM9000_REG_ISR);   //取得中断产生值
    regwrite(dm9000,DM9000_REG_ISR,IntStatus);    //清中断
    if(IntStatus & ISR_PRS)                       //接收中断
    {
        Lock_SempPost(&dm9000->rcvsync);
    }
    dm9000->clearextint(dm9000->irqno);               //清楚CPU中断标志

    regwrite(dm9000,DM9000_REG_IMR , DM9000_IMR_SET); // 开启 DM9000A 中断

    return 0;
}


static bool_t __dm9000CreateDev(tagDm9000Dev *dm9000)
{
	bool_t res = false;
    u16 evttID;
    u16 eventID;
    tagNetDevPara  devpara;

    Lock_MutexCreate_s(&dm9000->devsync,NULL);
    Lock_SempCreate_s(&dm9000->rcvsync,1,0,CN_SEMP_BLOCK_FIFO,NULL);
    evttID = Djy_EvttRegist(EN_CORRELATIVE, CN_PRIO_RRS, 0, 1,
    						(ptu32_t (*)(void))dm9000Rcv,NULL, 0x1000, dm9000->devname);
    if(evttID == CN_EVTT_ID_INVALID)
    {
    	goto EVTT_FAILED;
    }

    eventID=Djy_EventPop(evttID, NULL, 0, (ptu32_t)dm9000, 0, 0);
    if(eventID == CN_EVENT_ID_INVALID)
    {
    	goto EVENT_FAILED;
    }
    devpara.ifsend = __dm9000Snd;
    devpara.iftype = EN_LINK_ETHERNET;
    devpara.devfunc = 0x00;//COULD DO NO CRC
    memcpy(devpara.mac, dm9000->devmac,CN_MACADDR_LEN);
    devpara.name = dm9000->devname;
    devpara.private = 0;
    devpara.mtu = 14;
    devpara.private = (ptu32_t)dm9000;
    dm9000->handle = NetDevInstall(&devpara);

    if(0 == dm9000->handle)
    {
    	goto DEV_FAILED;
    }

    //OK,NOW INSTALL THE INTERRUPT
    Int_Register(dm9000->irqno);
    Int_IsrConnect(dm9000->irqno,__dm9000Isr);
    Int_SetClearType(dm9000->irqno,CN_INT_CLEAR_AUTO);
    Int_SettoAsynSignal(dm9000->irqno);
    Int_ClearLine(dm9000->irqno);
    Int_SetIsrPara(dm9000->irqno,(ptu32_t)dm9000);
    Int_RestoreAsynLine(dm9000->irqno);

    res = true;
    return res;

DEV_FAILED:
    //do the unpop
EVENT_FAILED:
	Djy_EvttUnregist(evttID);
EVTT_FAILED:
	Lock_MutexDelete_s(&dm9000->devsync);
	Lock_SempDelete_s(&dm9000->rcvsync);
    return res;
}


static tagDm9000Dev *pDm9000;
//THIS FUNCTION USED BY USER TO INSTALL AN DM9000 DEV WITH THE SPECIFIED NAME AND MAC
bool_t Dm9000Install(tagDm9000Para *para)
{
	bool_t res = false;
    tagDm9000Dev *dm9000;

    dm9000 = malloc(sizeof(tagDm9000Dev));
    if(NULL == dm9000)
    {
    	DM9000_DBG("%s:mem failed\n\r",__FUNCTION__);
    	goto MEM_FAILED;
    }

    memset((void *)dm9000,0,sizeof(tagDm9000Dev));
    //cpy from the para
    memcpy((void *)dm9000->devname,(void *)para->devname,CN_DM9000NAME_LEN);
    memcpy((void *)dm9000->devmac,(void *)para->mac,CN_MACADDR_LEN);
    dm9000->cmdaddr = para->cmdaddr;
    dm9000->dataddr = para->dataddr;
    dm9000->irqno = para->irqno;
    dm9000->clearextint = para->clearextint;

    //OK, NOW PROBE THE DM9000A DEVICE
    res = __dm9000Probe(dm9000);
    if(false == res)
    {
    	DM9000_DBG("%s:probe failed\n\r",__FUNCTION__);
    	goto PROBE_FAILED;
    }

    //OK,NOW HARD INITIALIZE
    __dm9000HardInit(dm9000);

    //NOW REGISTER IT FOR THE NETSTACK
    res = __dm9000CreateDev(dm9000);
    if(false == res)
    {
    	DM9000_DBG("%s:Creaet Dm9000 Dev failed\n\r",__FUNCTION__);
    	goto DEV_FAILED;
    }
	DM9000_DBG("%s:ISNTALL DM9000 DEV SUCCESS\n\r",__FUNCTION__);
	pDm9000 = dm9000;
	__showDm9000Reg(dm9000);
    return res;


DEV_FAILED:
PROBE_FAILED:
	free((void *)dm9000);
	dm9000 = NULL;

MEM_FAILED:
	return res;
}

bool_t shellDm9000Reg(char *param)
{
	__showDm9000Reg(pDm9000);
	return true;
}



