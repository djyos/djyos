/*
 * Ax88796BdrCfg.c
 *
 *  Created on: 2016年3月24日
 *      Author: zqf
 */
#include <stdint.h>
#include <stddef.h>

#include "2410addr.h"
#include "AX88796Byte.h"
#include <tcpip_cdef.h>
#include <netdb.h>
#include <linkRout.h>
//网卡中断配置
//for the  s3c2410 cpu, the external net device interrupt must linked to
//the multi function io port, so we'd better to configure the io to interrupt
//first
//only specified to the chip0 or 1
void __Ax88796IntCfg(u32 irqno)
{
	u32 data;
    //use the interrupt to notify the application
    //set the triger mode
    SNGS3C_REG_READ(rEXTINT0,data);
    data = data & (~(0x0F<<(4*irqno)));
    data = data | (0x2<<(4*irqno)); //set it to the falling edge
    SNGS3C_REG_WRITE(rEXTINT0,data);

    //configer the port to interrupt
    SNGS3C_REG_READ(rGPFCON,data);  /* PG0-3 TO INT8-11 */
    data = data & (~(0x3<<(2*irqno)));
    data = data |(0x2<<(2*irqno));
    SNGS3C_REG_WRITE(rGPFCON,data);

    return ;
}

ptu32_t Ax88796IntCfg(void)
{
	__Ax88796IntCfg(CN_INT_LINE_EINT0);
	__Ax88796IntCfg(CN_INT_LINE_EINT1);
	return 0;
}

//网卡安装及其配置

//install the linkrout
//install the netdevice
#define CN_INTERNALNETDEV_NAME  "AX88796B_0"
#define CN_EXTERNALNETDEV_NAME  "AX88796B_1"

static u8  Ax88796Mac[2][6] = {
		{0x00,0x01,0x02,0x03,0x04,0x00},   //for chip0
		{0x00,0x01,0x02,0x03,0x04,0x01},   //for chip1
};
extern tagAx88796Pri *Ax88796Install(tagAx88796Para *para);
ptu32_t Ax88796Cfg(void)
{
	tagAx88796Para  para;
	tagLinkRout    *rout;
	tagHostAddrV4   ax88796ipv4;
	u32             routPro;
	u32             tablen;

	//内网卡
	para.devbase = (u8 *)0x8000000;
	para.fnRcvDealer = NULL;
	para.fnRstDealer = NULL;
	para.irqno = CN_INT_LINE_EINT0;
	para.loopcycle = CN_TIMEOUT_FOREVER;
	para.mac=Ax88796Mac[0];
	para.name = CN_INTERNALNETDEV_NAME;
	para.stacksize = 0x1000;  //4KB
	para.useirq = true;
	Ax88796Install(&para);
	//make the internal dev to static ip
	memset((void *)&ax88796ipv4,0,sizeof(ax88796ipv4));
	ax88796ipv4.ip      = inet_addr("192.168.0.178");
	ax88796ipv4.submask = inet_addr("255.255.255.0");
	ax88796ipv4.gatway  = inet_addr("192.168.0.1");
	ax88796ipv4.dns     = inet_addr("192.168.0.1");
	ax88796ipv4.broad   = inet_addr("192.168.0.255");
	tablen = 256;
	routPro = CN_LINKROUT_LBROAD_R|CN_LINKROUT_DBROAD_R|CN_LINKROUT_LBROAD_S|CN_LINKROUT_DBROAD_S;
	rout = LinkRoutCreate(EN_IPV_4,CN_INTERNALNETDEV_NAME,\
								  (void *)&ax88796ipv4,tablen,routPro);
	if(NULL==rout)
	{
		printk("%s:Add %s failed\r\n",__FUNCTION__,CN_INTERNALNETDEV_NAME);
	}

	//外网卡
	//do the external net rout
	para.devbase = (u8 *)0x8200000;
	para.fnRcvDealer = NULL;
	para.fnRstDealer = NULL;
	para.irqno = CN_INT_LINE_EINT1;
	para.loopcycle = CN_TIMEOUT_FOREVER;
	para.mac=Ax88796Mac[1];
	para.name = CN_EXTERNALNETDEV_NAME;
	para.stacksize = 0x1000;  //4KB
	para.useirq = true;
	Ax88796Install(&para);

	memset((void *)&ax88796ipv4,0,sizeof(ax88796ipv4));
//    ax88796ipv4.ip      = inet_addr("192.168.253.178");
//    ax88796ipv4.submask = inet_addr("255.255.255.0");
//    ax88796ipv4.gatway  = inet_addr("192.168.253.1");
//    ax88796ipv4.dns     = inet_addr("192.168.253.1");
//    ax88796ipv4.broad   = inet_addr("192.168.253.255");
	tablen = 256;
	routPro = CN_LINKROUT_LBROAD_R|CN_LINKROUT_DBROAD_R|CN_LINKROUT_LBROAD_S|CN_LINKROUT_DBROAD_S;
	rout = LinkRoutCreate(EN_IPV_4,CN_EXTERNALNETDEV_NAME,\
								  (void *)&ax88796ipv4,tablen,routPro);
	if(NULL==rout)
	{
		printk("%s:Add %s failed\r\n",__FUNCTION__,CN_EXTERNALNETDEV_NAME);
	}
	Dhcp_AddTask(rout);
	LinkRoutSetDefault(EN_IPV_4,rout);
	return 0;
}
