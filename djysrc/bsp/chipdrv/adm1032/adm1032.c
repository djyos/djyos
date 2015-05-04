/*******************************************************************************/
/*                                                                             */
/*   本温度传感器驱动改编至VDSP ezboard post例程中的adm1032驱动                */
/*                                                                             */
/*    FILE:     adm1032.c                                                      */
/*                                                                             */
/*    PURPOSE:  This file tests the ADM1032 Temperature Monitor interface      */
/*               on the EZ-Board.                                              */
/*                                                                             */
/*******************************************************************************/

/* includes */
#include <cdef21469.h>
#include <def21469.h>

#include "stdint.h"
#include "cpu_peri.h"
#include "adm1032.h"
#include "iic.h"

#include <sru.h>
#include <sysreg.h>


struct tagMutexLCB *  ptSemID_adm1032;       // IIC操作互斥标志

static djy_handle_t pg_adm1032_lhdl;
static struct tagIIC_Param pg_adm1032_param;

static u32 timeout = 10000;


// ------读铁电--------------------------------------------------------
// 功能：读铁电指定地址开始Dstlen长度的数据
// 参数：Adm1032_DstAddr，目标存储器预读取数据的地址
//         pDstBuf，存放接收到的数据缓存地址
//         DstLen， 预读取数据的长度
// 返回：ret, 实际读到的字节数
// -------------------------------------------------------------------
u32 ADM1032_Read_Data(u32 Adm1032_DstAddr,u8 *pDstBuf,u32 DstLen)
{
    u32 ret = 0;

    pg_adm1032_param.memory_addr = Adm1032_DstAddr;
    pg_adm1032_param.data_len    = 0;

    //发送存储器地址
    Driver_DevWriteLeft(pg_adm1032_lhdl,(ptu32_t)pDstBuf,
                (ptu32_t)&pg_adm1032_param,timeout);

    //读数据
    pg_adm1032_param.data_len    = DstLen;
    ret = Driver_DevReadLeft(pg_adm1032_lhdl,(ptu32_t)pDstBuf,
                    (ptu32_t)&pg_adm1032_param,timeout);
    return ret;
}
// -------写铁电------------------------------------------------------
// 功能：写铁电指定地址开始Dstlen长度的数据
// 参数：Adm1032_DstAddr，目标存储器预写数据地址
//         pSrcBuf，发送数据缓存地址
//         SrcLen， 发送数据长度
// 返回：ret, 实际写入的字节数
// -------------------------------------------------------------------
u32 ADM1032_Write_Data(u32 Adm1032_DstAddr,u8 *pSrcBuf,u32 SrcLen)
{
    u32 ret = 0;


    pg_adm1032_param.memory_addr = Adm1032_DstAddr;
    pg_adm1032_param.data_len    = SrcLen;

    ret = Driver_DevWriteLeft(pg_adm1032_lhdl,(ptu32_t)pSrcBuf,
                        (ptu32_t)&pg_adm1032_param,timeout);

    return ret;
}


//----初始化ADM1032芯片----------------------------------------------------
//功能：初始化ADM1032芯片
//参数: 无
//返回: true= 成功，false=失败
//--------------------------------------------------------------------------
bool_t module_init_adm1032(ptu32_t para)
{
    ptSemID_adm1032 = Lock_MutexCreate(NULL);// adm1032操作互斥标志

    pg_adm1032_param.device_addr  = TEMP_SENSOR_ADDRESS;  //ADM1032的物理地址
    pg_adm1032_param.memory_addr  = 0;
    pg_adm1032_param.mem_addr_len = 1;             //ADM1032的地址空间为两字节
    pg_adm1032_param.data_len     = 0;

    IIC0_Init();
    pg_adm1032_lhdl = Driver_DevOpenLeft("iic1",0);
    if(pg_adm1032_lhdl != NULL)
    {
        //配置传输速度为100KHz
       Driver_DevCtrlLeft(pg_adm1032_lhdl,cn_iic_set_clk,I2C_CLOCK_RATE_100K,0);
       return true;
    }
    else
        return false;
}
