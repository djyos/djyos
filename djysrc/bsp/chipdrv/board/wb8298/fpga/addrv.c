/*=======================================================    *
 * 文件模块说明:                                                *
 *     wb82x系列板件AD驱动程序                        *
 * 文件版本: v1.00 (说明本文件的当前版本信息)                   *
 * 开发人员: 李延新                                           *
 * 创建时间:                                                    *
 * Copyright(c) 2007-2008  Shenzhen NARI Limited Co.            *
 *========================================================    *
 * 程序修改记录(最新的放在最前面):                *
 *  <版本号> <修改日期>, <修改人员>: <修改功能概述>        *
 *========================================================    *
 * 110504,王乾刚：增加第三片AD
*/

#include "stdlib.h"
#include "addrv.h"
#include "sunri_types.h"

#define CN_ADADDR1 0x12000000
#define CN_ADADDR2 0x12000002
#define CN_ADSHCTL 0x14800003
#define CN_ADSHSTA2 0x14800006 //add by xg 110504
#define CN_ADSHSTA 0x14800007

#define CN_ADSH1    0x40
#define CN_ADSH2    0x80

#define CN_ADBUSY1    0x01
#define CN_ADBUSY2    0x02
#define CN_ADBUSY3    0x20   //add by xg 110504

static UINT16 wAdSampleBuff[16][6];      //最多16片AD7656
static UINT16 wADChipNum = 1;        //缺省1片

/*========================================================*/
//* 函数功能：初始化AD7656驱动
//* 输入参数：chipnum,AD7656芯片的数目
//* 输出参数：无
//* 返回值  ：无
/*========================================================*/

void Ad_InitReg(UINT16 wchipnum)
{
wADChipNum = wchipnum;    //init chips
}

/*========================================================*/
//* 函数功能：读取AD采样，刷新采样区数据
//* 输入参数：无
//* 输出参数：无
//* 返回值  ： 采样区指针，若为空，则AD异常
/*========================================================*/
UINT16 * Ad_Read()
{
    UINT16 i,j;

    *(UINT8 *)(CN_ADSHCTL) &= ~ (CN_ADSH1 | CN_ADSH2);                    //sample

    if (( 0 == ((CN_ADBUSY1 | CN_ADBUSY2)  & (*(UINT8 *) CN_ADSHSTA)))
        && ( 0 == (CN_ADBUSY3 & (*(UINT8 *) CN_ADSHSTA2))))            //busy?  modify  by xg 110504
    {
        for(j=0;j<wADChipNum;j++)                        //chips
            for(i=0;i<6;i++)                        //6 channels per chip
                wAdSampleBuff[j][i] = *(UINT16 *) (CN_ADADDR1 + j*2);    //get results
        return     (UINT16 *) wAdSampleBuff;                    //return buff
    }
    else
        return NULL;                                //busy return 0

}

/*========================================================*/
//* 函数功能：保持并采样
//* 输入参数：无
//* 输出参数：无
//* 返回值  ： 无
/*========================================================*/
void Ad_SampleHold(void)
{

//    *(UINT8 *)(CN_ADSHCTL) |= (CN_ADSH1 | CN_ADSH2);    //hold
}
