/*******************************************************************************/
/*                                                                             */
/*   本温度传感器驱动改编至VDSP ezboard post例程中的adm1032驱动				   */
/*                                                                             */
/*    FILE:     adm1032.h                                                      */
/*                                                                             */
/*    PURPOSE:  This file tests the ADM1032 Temperature Monitor interface	   */
/*				 on the EZ-Board.       									   */
/*                                                                             */
/*******************************************************************************/

#ifndef __ADM1032_H__
#define __ADM1032_H__



#define	MAX_TEMPERATURE_SAMPLES	50	/* number of samples to check */

/* device address */
#define TEMP_SENSOR_ADDRESS		0x4C

/* register addresses for ADM1032 Temperature Monitor */

#define LT_R			0x00  	/* local temperature - READ */
#define LHS_W			0x0B 	/* local temperature HIGH limit - WRITE */
#define LHS_R			0x05 	/* local temperature HIGH limit - READ */
#define LLS_W			0x0C 	/* local temperature LOW limit - WRITE */
#define LLS_R			0x06 	/* local temperature LOW limit - READ */

#define RHSHB_W			0x0D 	/* remote temperature HIGH limit (high byte) - WRITE */
#define RHSHB_R			0x07 	/* remote temperature HIGH limit (high byte) - READ */
#define RHSLB_W			0x13 	/* remote temperature HIGH limit (low byte) - WRITE */
#define RHSLB_R			0x13 	/* remote temperature HIGH limit (low byte) - READ */

#define RLSHB_W			0x0E 	/* remote temperature LOW limit (high byte) - WRITE */
#define RLSHB_R			0x08 	/* remote temperature LOW limit (high byte) - READ */
#define RLSLB_W			0x14 	/* remote temperature LOW limit (low byte) - WRITE */
#define RLSLB_R			0x14 	/* remote temperature LOW limit (low byte) - READ */

#define CONFIG_W		0x09 	/* configuration register - WRITE */
#define CONFIG_R		0x03	/* configuration register - READ */
#define RCS_W			0x19	/* remote THERM limit - WRITE */
#define RCS_R			0x19	/* remote THERM limit - READ */
#define LCS_W			0x20	/* local THERM limit - WRITE */
#define LCS_R			0x20	/* local THERM limit - READ */
#define SR_R			0x02	/* status register - READ */

#define RTHB_R			0x01	/* remote temperature value (high byte) - READ */
#define RTLB_R			0x10	/* remote temperature value (low byte) - READ */
#define RTOHB_R			0x11	/* remote temperature offset (high byte) - READ */
#define RTOHB_W			0x11	/* remote temperature offset (high byte) - WRITE */
#define RTOLB_R			0x12	/* remote temperature offset (low byte) - READ */
#define RTOLB_W			0x12	/* remote temperature offset (low byte) - WRITE */

#define HYSTERISIS_R	0x21	/* therm hysterisis value - READ */
#define HYSTERISIS_W	0x21	/* therm hysterisis value - WRITE */
#define CR_W			0x0A	/* conversion rate - WRITE */
#define CR_R			0x04	/* conversion rate - READ */


u32 ADM1032_Read_Data(u32 Adm1032_DstAddr,u8 *pDstBuf,u32 DstLen);
u32 ADM1032_Write_Data(u32 Adm1032_DstAddr,u8 *pSrcBuf,u32 SrcLen);
bool_t module_init_adm1032(ptu32_t para);


						  
#endif	/* __ADM1032_H__ */
