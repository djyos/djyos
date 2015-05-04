/*******************************************************************************/
/*                                                                             */
/*   (C) Copyright 2008 - Analog Devices, Inc.  All rights reserved.           */
/*                                                                             */
/*    FILE:     m25p16.h                                                       */
/*																			   */
/*    PURPOSE:  This header file defines items specific to the M25P16 flash.   */
/*                                                                             */
/*******************************************************************************/

#ifndef __M25P16_H__
#define __M25P16_H__


#include <drivers\flash\util.h>
#include <drivers\flash\Errors.h>


/* application definitions */
#define COMMON_SPI_SETTINGS 	(SPIEN|SPIMS|SENDZ|CPHASE|CLKPL)
#define COMMON_SPI_DMA_SETTINGS (INTEN|SPIDEN)
#define BAUD_RATE_DIVISOR 		40

/* interface function prototypes */
ERROR_CODE m25p16_Open(void);						/* open the flash device */
ERROR_CODE m25p16_Close(void);						/* close the flas device */
ERROR_CODE m25p16_Read(unsigned short *pusData,		/* read some bytes from flash */
					   unsigned long ulStartAddress,
					   unsigned int uiCount );
ERROR_CODE m25p16_Write(unsigned short *pusData,	/* write some bytes to flash */
					    unsigned long ulStartAddress,
					    unsigned int uiCount );
ERROR_CODE m25p16_Control(unsigned int uiCmd,		/* send a command to device */
						  COMMAND_STRUCT *pCmdStruct);

ERROR_CODE EraseBlock( int nBlock, unsigned long ulAddr );
ERROR_CODE EraseFlash(unsigned long ulAddr);
						  
#endif	/* __M25P16_H__ */
