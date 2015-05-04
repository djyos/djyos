//----------------------------------------------------
//Copyright (C), 2004-2009,  lst.
//版权所有 (C), 2004-2009,   lst.
//所属模块:IO初始化
//作者：lst
//版本：V1.0.0
//文件描述:IO初始化
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-01-04
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "cpu_peri.h"

struct ppc_gpio_reg volatile * const pg_gpio_reg =  (struct ppc_gpio_reg *)cn_gpio_baddr;

//----选择引脚功能-------------------------------------------------------------
//功能: PPC1020中，配置引脚的功能主要包括输入输出，以及输出
//            的方式包括主动输出和开漏输出两种方式，配置寄存器
//            时，要注意大端存储的问题，需要作相应的偏移。如需
//            配置pinnum=3为输出时，要将寄存器GPDIR第4位置位，由于
//            大端存储，需要将0x80000000向右偏移pinnum位,写入寄存器
//参数: pinnum，port内编号，比如GPIO_0
//      func_no，功能编号，见 cn_gpio_mode_out_ad 那一组常量。
//返回: 无
//-----------------------------------------------------------------------------
void GPIO_CfgPinFunc(u32 pinnum,u32 func_no)
{
    if(pinnum > GPIO_MAX)
        return;

    if(func_no == cn_gpio_mode_in)
        pg_gpio_reg->GPDIR &= ~(0x80000000 >> pinnum);//配置IO为输入
    else
    {
        pg_gpio_reg->GPDIR |= (0x80000000 >> pinnum);//配置IO为输出

        //输出模式
        if(func_no == cn_gpio_mode_out_ad)
            pg_gpio_reg->GPODR &= ~(0x80000000 >> pinnum);
        else if(func_no == cn_gpio_mode_out_od)
            pg_gpio_reg->GPODR |= (0x80000000 >> pinnum);
    }
}

//----IO口输出高电平-----------------------------------------------------------
//功能: 在port中msk中是1的bit对应的位输出高电平
//参数: pinnum，需要输出高电平的位
//返回: 无
//example:
//      GPIO_SettoHigh(1,GPIO_3);      //在p1.4输出高电平
//-----------------------------------------------------------------------------
void GPIO_SettoHigh(u32 pinnum)
{
    if(pinnum > GPIO_MAX)
        return;
    pg_gpio_reg->GPDAT |= (0x80000000 >> pinnum);
}

//----IO口输出低电平-----------------------------------------------------------
//功能: 在port中msk中是1的bit对应的位输出低电平
//参数:pinnum, 需要输出高电平的位
//返回: 无
//example:
//      GPIO_SettoLow(1,GPIO_3);      //在p1.4输出低电平
//-----------------------------------------------------------------------------
void GPIO_SettoLow(u32 pinnum)
{
    if(pinnum > GPIO_MAX)
        return;
    pg_gpio_reg->GPDAT &= ~(0x80000000 >> pinnum);
}

//----获取GPIO相应位的状态----------------------------------------------
//功能: 获取GPIOn位的电平状态，为高电平或低电平
//参数: pinnum，中断标志的GPIOn, 例如GPIO_0
//返回: 非0----高电平
//        0----低电平
//-----------------------------------------------------------------------------
u32 gpio_get_pin(u32 pinnum)
{
    if(pinnum > GPIO_MAX)
        return error_pin_para;
    return (pg_gpio_reg->GPDAT & (0x80000000 >> pinnum));
}

//----使能GPIO中断--------------------------------------------------------------
//功能: 屏蔽GPIO中断的相应中断源，使能中断后，CPU能响应中断
//参数: pinnum，使能的中断源，比如使能GPIO_0
//返回: 无
//-----------------------------------------------------------------------------
void gpio_enable_int(u32 pinnum)
{
    if(pinnum > GPIO_MAX)
        return;
    pg_gpio_reg->GPIMR |= (0x80000000 >> pinnum);
}

//----屏蔽GPIO中断--------------------------------------------------------------
//功能: 屏蔽GPIO中断的相应中断源，屏蔽后，中断不会向CPU请
//求中断响应，但是会置位GPIER相应位
//参数: port，被操作的port编号，比如屏蔽GPIO_0
//      intnum，屏蔽的中断源
//返回: 无
//-----------------------------------------------------------------------------
void gpio_disable_int(u32 pinnum)
{
    if(pinnum > GPIO_MAX)
        return;
    pg_gpio_reg->GPIMR &= ~(0x80000000 >> pinnum);
}

//----清GPIO中断寄存器标志位--------------------------------------------
//功能: 清GPIO中断寄存器标志位，由于系统复位后，该寄存器
//            并不复位，因此要使用GPIO作为外部中断源，必须要清
//            寄存器GPIER,清GPER方式是在相应位写1。
//参数: pinnum，需要清理的中断标志位
//返回: 无
//-----------------------------------------------------------------------------
void gpio_clear_int(u32 pinnum)
{
    if(pinnum > GPIO_MAX)
        return;
    //清中断时往相应位写1
    pg_gpio_reg->GPIER |= (0x80000000 >> pinnum);
}

//----查询GPIO中断寄存器标志位--------------------------------------
//功能: 查询GPIO中相应的中断标志位，发生中断时，
//            相应位置位，读取后，需清中断标志，
//            该寄存器并不受屏蔽中断寄存器GPIMR的影响。
//参数: pinnum，需要查询的中断标志位
//返回: 非0----中断标志置位
//         0----中断标志复位
//-----------------------------------------------------------------------------
u32 gpio_get_int(u32 pinnum)
{
    if(pinnum > GPIO_MAX)
        return error_pin_para;
    return (pg_gpio_reg->GPIER & (0x80000000 >> pinnum));
}

//----复位GPIO各寄存器---------------------------------------------------
//功能: 复位GPIO各寄存器，使其为上电时的状态
//参数: 无
//返回: 无
//-----------------------------------------------------------------------------
void gpio_reset (void)
{
    //reset the reg
    pg_gpio_reg->GPDIR = 0x00000000;
    pg_gpio_reg->GPODR = 0x00000000;
    pg_gpio_reg->GPDAT = 0x00000000;
    pg_gpio_reg->GPIMR = 0x00000000;
    pg_gpio_reg->GPIER = 0xffff0000;
    pg_gpio_reg->GPICR = 0x00000000;
}



