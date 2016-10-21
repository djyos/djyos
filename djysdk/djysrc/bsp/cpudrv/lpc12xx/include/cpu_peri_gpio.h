/**************************************************************************//**
 * $Id: lpc12xx_gpio.h 519 2010-09-15 01:46:38Z nxp21428 $
 *
 * @file     lpc12xx_gpio.h
 * @brief    Contains all macro definitions and function prototypes
 *              support for GPIO firmware library on lpc12xx.
 * @version  1.0
 * @date     26. Sep. 2010
 * @author   NXP MCU Team
 *
 * @note
 * Copyright (C) 2010 NXP Semiconductors(NXP). All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 ******************************************************************************/

/* Peripheral group ----------------------------------------------------------- */
/** @defgroup GPIO
 * @ingroup LPC1200CMSIS_FwLib_Drivers
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __LPC12xx_GPIO_H
#define __LPC12xx_GPIO_H

#define IOCON_PIO_MODE_NOPULLUP  ((u8)0x00  )   /* no pull resistor enabled */
#define IOCON_PIO_MODE_PULLUP    ((u8)0x10  )   /* Pull-up resistor enabled */


#define IOCON_PIO_INV_NOT        ((u8)0x00  )
#define IOCON_PIO_INV_INVERTED   ((u8)0x40  )


#define IOCON_PIO_AD_ANALOG      ((u8)0x00  )
#define IOCON_PIO_AD_DIGITAL     ((u8)0x80  )


//注意:此处与nxp的库不符，按datasheet修正
#define IOCON_PIO_DRV_2MA_12MA   ((u16)0x0000  )   /* Drive current */
#define IOCON_PIO_DRV_4MA_16MA   ((u16)0x0200  )   /* Drive current */


#define IOCON_PIO_OD_DISABLE     ((u16)0x0000  )
#define IOCON_PIO_OD_ENABLE      ((u16)0x0400  )


//注意:此处与nxp的库不符，按datasheet修正
#define IOCON_PIO_SMODE_BYPASS          ((u16)0x0000  )
#define IOCON_PIO_SMODE_ONE_CLOCK       ((u16)0x0800  )
#define IOCON_PIO_SMODE_TWO_CLOCK       ((u16)0x1000  )
#define IOCON_PIO_SMODE_THREE_CLOCK     ((u16)0x1800  )


#define IOCON_PIO_SEL_FILTER0     ((u16)0x0000  )
#define IOCON_PIO_SEL_FILTER1     ((u16)0x2000  )
#define IOCON_PIO_SEL_FILTER2     ((u16)0x4000  )
#define IOCON_PIO_SEL_FILTER3     ((u16)0x6000  )
#define IOCON_PIO_SEL_FILTER4     ((u16)0x8000  )
#define IOCON_PIO_SEL_FILTER5     ((u16)0xa000  )
#define IOCON_PIO_SEL_FILTER6     ((u16)0xc000  )

union un_iocfg_bit
{
    struct bit
    {
        u32    func:3;          //选择功能，复位=0
        u32    res1:1;
        u32    pu_select:1;     //上拉电阻选择，0=不上拉，1=上拉，复位=1
        u32    res2:1;
        u32    invert:1;        //输入反相选择，0=不反相，1=反相，复位=0
        u32    ad:1;            //模拟和数字模式选择，0=模拟，1=数字，复位=1
        u32    res3:1;
        u32    drv_mode:1;      //输出驱动模式，0=弱，1=强，复位=0
        u32    od:1;            //开漏模式选择，0=普通，1=开漏，复位=0
        u32    filter_node:2;   //抗干扰滤波脉冲数:0~3，复位00
        u32    filter_clk:3;    //抗干扰采样时钟源选择，0~6，复位=000
                                //用于选择IOCONFIGCLKDIV0~IOCONFIGCLKDIV6中的一个
                                //作为抗干扰输入时钟分频数
    }bit;
    u32  dw;
};


struct st_iocon_reg
{
    vu32 PIO2_28;            /*!< Offset: 0x000  (R/W) */
    vu32 PIO2_29;            /*!< Offset: 0x004  (R/W) */
    vu32 PIO0_19;            /*!< Offset: 0x008 Configures pin PIO0_19/ACMP0_I0/CT32B0_1.  (R/W) */
    vu32 PIO0_20;
    vu32 PIO0_21;
    vu32 PIO0_22;
    vu32 PIO0_23;
    vu32 PIO0_24;

    vu32 SWDIO_PIO0_25;     /*!< Offset: 0x020 pin SWDIO/ACMP1_I2/CT32B1_2/CT16B0_RST/PIO0_25.  (R/W) */
    vu32 SWCLK_PIO0_26;
    vu32 PIO0_27;
    vu32 PIO2_12;
    vu32 PIO2_13;
    vu32 PIO2_14;
    vu32 PIO2_15;
    vu32 PIO0_28;

    vu32 PIO0_29;            /*!< Offset: 0x040 Configures pin PIO0_29/ROSC/CT16B0_1.  (R/W) */
    vu32 PIO0_0;
    vu32 PIO0_1;
    vu32 PIO0_2;
    vu32 RESERVED0;
    vu32 PIO0_3;
    vu32 PIO0_4;
    vu32 PIO0_5;

    vu32 PIO0_6;             /*!< Offset: 0x060 Configures pin PIO0_6/RI0/CT32B1_0.  (R/W) */
    vu32 PIO0_7;
    vu32 PIO0_8;
    vu32 PIO0_9;
    vu32 PIO2_0;
    vu32 PIO2_1;
    vu32 PIO2_2;
    vu32 PIO2_3;

    vu32 PIO2_4;             /*!< Offset: 0x080 Configures pin PIO2_4/CT32B0_0/CTS0.  (R/W) */
    vu32 PIO2_5;
    vu32 PIO2_6;
    vu32 PIO2_7;
    vu32 PIO0_10;
    vu32 PIO0_11;
    vu32 PIO0_12;
    vu32 RESET_P0_13;

    vu32 PIO0_14;            /*!< Offset: 0x0A0 Configures pin PIO0_14/SSP_CLK.  (R/W) */
    vu32 PIO0_15;
    vu32 PIO0_16;
    vu32 PIO0_17;
    vu32 PIO0_18;
    vu32 PIO0_30;
    vu32 PIO0_31;
    vu32 PIO1_0;

    vu32 PIO1_1;                /*!< Offset: 0x0C0 Configures pin R/PIO1_1/AD3.  (R/W) */
    vu32 PIO1_2;
    vu32 PIO1_3;
    vu32 PIO1_4;
    vu32 PIO1_5;
    vu32 PIO1_6;
    vu32 RESERVED1[2];

    vu32 PIO2_8;             /*!< Offset: 0x0E0 Configures pin PIO2_8/CT32B1_0.  (R/W) */
    vu32 PIO2_9;
    vu32 PIO2_10;
    vu32 PIO2_11;

} ;


/* Includes ------------------------------------------------------------------- */
struct GPIO_StReg
{
    vu32 MASK;       /*!< Offset: 0x000 Pin value mask register (R/W) */
    vu32 PIN;
    vu32 OUT;
    vu32 SET;
    vu32 CLR;        /*!< Offset: 0x010 Pin output value clear register (R/W) */
    vu32 NOT;
    vu32 RESERVE[2];
    vu32 DIR;        /*!< Offset: 0x020 Data direction register (R/W) */
    vu32 IS;
    vu32 IBE;
    vu32 IEV;
    vu32 IE;
    vu32 RIS;
    vu32 MIS;
    vu32 IC;
} ;

#ifdef __cplusplus
extern "C"
{
#endif


/* Public Macros --------------------------------------------------------------- */
/** @defgroup GPIO_Public_Types
 * @{
 */

/** @defgroup GPIO_interrupt_type
  * @{
  */
#define GPIO_INTERRUPT_FALLING     ((u32)(0))
#define GPIO_INTERRUPT_RISING      ((u32)(1))
#define GPIO_INTERRUPT_BOTH_EDGES  ((u32)(2))
#define GPIO_INTERRUPT_LOW         ((u32)(3))
#define GPIO_INTERRUPT_HIGH        ((u32)(4))

#define GPIO_DIR_INPUT    ((u32)(0))  /* INPUT*/
#define GPIO_DIR_OUTPUT   ((u32)(1))  /* OUTPUT */


#define PARAM_GPIO_INTERRUPT(TYPE)  ((  TYPE == GPIO_INTERRUPT_FALLING)   || \
                                     (  TYPE == GPIO_INTERRUPT_RISING)    || \
                                     (  TYPE == GPIO_INTERRUPT_BOTH_EDGES)|| \
                                     (  TYPE == GPIO_INTERRUPT_LOW)       || \
                                     (  TYPE == GPIO_INTERRUPT_HIGH))

    void GPIO_CfgPinFunc(u32 port,u32 pinnum,u32 func_no);
    void gpio_cfg_pin_mode(u32 port,u32 pinnum,u32 mode);
    void gpio_setto_in(u32 port,u32 msk);
    void gpio_setto_out(u32 port,u32 msk);
    u32 GPIO_GetData(u32 port);
    void GPIO_OutData(u32 port,u32 data);
    void GPIO_SettoHigh(u32 port,u32 msk);
    void GPIO_SettoLow(u32 port,u32 msk);
    void GPIO_PowerOn(u32 port);
    void GPIO_PowerOff(u32 port);

#ifdef __cplusplus
}
#endif
#endif /* __LPC12xx_GPIO_H */

/**
 * @}
 */

/* --------------------------------- End Of File ------------------------------ */
