/*******************************************************************
Analog Devices, Inc. All Rights Reserved.

This software is proprietary and confidential.  By using this software
you agree to the terms of the associated Analog Devices License Agreement.

Project Name:  	Power_On_Self_Test

Hardware:		ADSP-21469 EZ-Board

Description:	This file tests the parallel flash on the EZ-Board.
*******************************************************************/


/*******************************************************************
*  include files
*******************************************************************/
#include <cdef21469.h>
#include <def21469.h>
#include <stdlib.h>
#include <drivers\flash\util.h>     /* library struct includes */
#include <drivers\flash\Errors.h>   /* error type includes */
#include <drivers\flash\m29w320.h>	/* flash includes */
#include <sru.h>
#include <sysreg.h>
#include "post_debug.h"


/*******************************************************************
*  global variables and defines
*******************************************************************/

#define FLASH_START_ADDR 		0x4000000	/* start address on EZ-Board */

#if defined(__DEBUG_FILE__)
extern FILE *pDebugFile;					/* debug file */
#endif


/*******************************************************************
*  function prototypes
*******************************************************************/
int CheckForCFISupport(void);
void Init_ParallelFlash(void);


/*******************************************************************
*   Function:    Init_ParallelFlash
*   Description: This function initializes parallel flash.
*******************************************************************/
void Init_ParallelFlash(void)
{
	/* setup EPCTL to use bank 1 (MS1) core accesses */
	*pEPCTL = (((*pEPCTL) & (~B1SD)) | EPBRCORE);

	/* setup for max waitstates, note the PKDIS bit is set which makes a 1 to 1
		mapping, each 8 bit byte maps to an address */
	*pAMICTL1 = ( PKDIS | WS31 | HC1 | HC2 | RHC1 | RHC2 | IC7 | AMIEN | AMIFLSH);
}


/*******************************************************************
*   Function:    Test_Parallel_Flash
*   Description: performs a test on the parallel flash
*******************************************************************/
int Test_Parallel_Flash(void)
{
	int i, j;							/* indexes */
	unsigned int iPassed = 1;			/* result from test */
	ERROR_CODE Result = NO_ERR;			/* result */
	COMMAND_STRUCT pCmdBuffer;			/* command buffer union */
	unsigned short usRd = 0, usWr = 0;	/* storage for data */
	int nManCode = 0, nDevCode = 0;		/* man and device ids */
	static unsigned int uiSectors = 0;	/* we use different sectors each time */


	/* sectors to be tested: we chose one sector from a few different banks, making
	   sure we don't overwrite any sector of flash where the boot image or data lives */

	int pnTestSectors[] = { 20, 30, 40, -1,		/* each line must end with -1 */
							21, 31, 41, -1,
							22, 32, 42, -1,
							23, 33, 43, -1,
							24, 34, 44, -1,
							25, 35, 45, -1,
							26, 36, 46, -1,
							27, 37, 47, -1,
							28, 38, 48, -1,
							29, 39, 49, -1 };	
	
	/* choose different sectors each time through the test, this makes the test a
		little more robust and limits the writes to certain flash sectors */
	uiSectors++;
	if (10 == uiSectors)
	uiSectors = 0;

	//DEBUG_HEADER( "Parallel Flash Test" );

	/* init parallel flash interface */
	Init_ParallelFlash();

	/* open the parallel flash */
	Result = m29w320_Open();

	if (Result != NO_ERR)
	{
		//DEBUG_STATEMENT( "\nCould not open flash driver" );
		//DEBUG_STATEMENT( "\nTest failed" );
		return 0;
	}

	/* get the codes */
	pCmdBuffer.SGetCodes.pManCode = (unsigned long *)&nManCode;
	pCmdBuffer.SGetCodes.pDevCode = (unsigned long *)&nDevCode;
	pCmdBuffer.SGetCodes.ulFlashStartAddr = FLASH_START_ADDR;
	Result = m29w320_Control( CNTRL_GET_CODES, &pCmdBuffer );

	//DEBUG_PRINT( "\nDetected manufacturer code of 0x%x and device code of 0x%x", nManCode, nDevCode );

	/* if codes don't match what we expect then we should fail, note we're only checking the lower
		byte of the device code on the EZ-Board */
	if( (FLASH_MANUFACTURER_ST != nManCode) || (FLASH_DEVCODE_ST_M29W320EB & 0xff) != (0x00ff & nDevCode) )
	{
		//DEBUG_STATEMENT( "\nFlash codes do not match what we expected" );
		//DEBUG_STATEMENT( "\nTest failed" );
		return 0;
	}

	/* we also show how to determine if this part supports the Common Flash Interface (CFI) and
		if so, we will attempt to get the CFI data */
	if (!CheckForCFISupport())
	{
		/* this means there was a failure not that the device does not support CFI, so
			therefore we return failure */
		//DEBUG_STATEMENT( "\nFailed checking for CFI support" );
		//DEBUG_STATEMENT( "\nTest failed" );
		return 0;
	}

	/* for each test sector */
	for ( usWr = 0x1234, j = uiSectors*4; (pnTestSectors[j] != -1) && (iPassed); j++ )
	{
		/* erase the sector */
		//DEBUG_PRINT( "\nErasing sector %d...", pnTestSectors[j] );
		pCmdBuffer.SEraseSect.ulFlashStartAddr = FLASH_START_ADDR;
		pCmdBuffer.SEraseSect.nSectorNum = pnTestSectors[j];
		Result = m29w320_Control( CNTRL_ERASE_SECT, &pCmdBuffer );

		/* if the erase passed, continue */
		if(NO_ERR == Result)
		{
			//DEBUG_STATEMENT( "passed" );
			//DEBUG_PRINT( "\nTesting sector %d...", pnTestSectors[j] );

			/* write some data */
			for( i = 0; i < 0x8; i+=2, usWr+= 0x1111 )
			{
				unsigned long ulOffset;

				/* calculate offset based on sector */
				if ( pnTestSectors[j] < 8 )
				{
					ulOffset = pnTestSectors[j]*8*1024 + i + FLASH_START_ADDR;
				}
				else
				{
					ulOffset = 0x10000 + + i + FLASH_START_ADDR + ( (64*1024)*(pnTestSectors[j] - 8) );
				}

				/* write a word to the flash */
				Result = m29w320_Write( &usWr, ulOffset, 0x1 );

				if(NO_ERR == Result)
				{
					iPassed = 1;

					/* now do a read */
					Result = m29w320_Read( &usRd, ulOffset, 0x1 );

					if(NO_ERR == Result)
					{
						/* compare the data */
						if( (usRd & 0xff) != (usWr & 0xff) )
						{
						//	DEBUG_STATEMENT( "failed" );
							//DEBUG_PRINT( "\n\nFailure at address 0x%x: expected 0x%x, received 0x%x",
	        								//ulOffset, 0xff & (char)usWr, 0xff & (char)usRd );
							iPassed = 0;
							break;
						}
					}
				}
			}

			/* print status */
			if ( iPassed )
			{
				//DEBUG_STATEMENT( "passed" );
			}
		}

		/* if erase failed, break out */
		else
		{
			//DEBUG_STATEMENT( "failed" );
			break;
		}

		/* at this point the sector test passed, erase it again */
		Result = m29w320_Control( CNTRL_ERASE_SECT, &pCmdBuffer );
	}

	/* close driver and return result */
	m29w320_Close();

	/* print status */
	//DEBUG_RESULT( iPassed, "\nTest passed", "\nTest failed" );

	return iPassed;
}


/*******************************************************************
*   Function:    CheckForCFISupport
*   Description: Checks if the devices supports the Common Flash Interface
*	             (CFI) and if so gets the CFI data.
*******************************************************************/
int CheckForCFISupport(void)
{
	ERROR_CODE Result = NO_ERR;			/* result from driver */
	COMMAND_STRUCT pCmdBuffer;			/* command buffer union */
	bool bSupportsCFI = false;			/* see if this device supports CFI */
	int nFlashWidth = 0x0;				/* flash width */
	char pCFIData[128*2];				/* provide enough for 16-bit wide flashes */
	char *pQRY = NULL;					/* ptr to QRY string within CFI data */
	unsigned short usPrimary = 0x0;		/* primary pointer within CFI data */
	unsigned short usDeviceSize = 0x0;	/* device size (2^n) within CFI data */
	unsigned short usMajVerASCII = 0x0;	/* major version in ASCII within CFI data */
	unsigned short usMinVerASCII = 0x0;	/* minor version in ASCII within CFI data */

	/* see if this device support CFI */
	pCmdBuffer.SSupportsCFI.pbSupportsCFI = &bSupportsCFI;
	Result = m29w320_Control( CNTRL_SUPPORTS_CFI, &pCmdBuffer );
	if (Result == UNKNOWN_COMMAND)
	{
		return 1;	/* not a failure, but the driver doesn't even support this command */
	}
	else if (Result != NO_ERR)
    {
        return 0;
    }

    /* if it supports CFI we can read the CFI data */
	if ( bSupportsCFI )
	{
		/* first check on the device width so we know how large the CFI data will be and
			how to interpret it */
		pCmdBuffer.SGetFlashWidth.pnFlashWidth = &nFlashWidth;
		Result = m29w320_Control( CNTRL_GET_FLASH_WIDTH, &pCmdBuffer );
		if (Result == UNKNOWN_COMMAND)
		{
			return 1;	/* not a failure, but the driver doesn't even support this command */
		}
		else if (Result != NO_ERR)
	    {
	        return 0;
	    }

	    /* now get the CFI data, make sure enough memory is allocated for the flash width
	    	which is 128 bytes per 8-bit width */
	    pCmdBuffer.SGetCFIData.pCFIData = pCFIData;
	    pCmdBuffer.SGetCFIData.ulFlashStartAddr = 0x0;
		Result = m29w320_Control( CNTRL_GET_CFI_DATA, &pCmdBuffer );
		if (Result == UNKNOWN_COMMAND)
		{
			return 1;	/* not a failure, but the driver doesn't even support this command */
		}
		else if (Result != NO_ERR)
	    {
	        return 0;
	    }

	    /* take a look at some of the CFI data, for example according to the CFI spec, the
	    	string "QRY" should be at offset 0x10, note the byte offset always gets multiplied
	    	by nFlashWidth, so for x16 devices the offset 0x10 becomes byte offset 0x20 */

	    /* if not x8 or x16 we won't check the data, we only allocated enough for up to x16 */
	    if ( (_FLASH_WIDTH_8 == nFlashWidth) || (_FLASH_WIDTH_16 == nFlashWidth) )
	    {
	    	/* check QRY string, note x16 devices will have a NULL after each character, check the
	    		CFI spec for details on other architectures */
	    	pQRY = &pCFIData[0x10 * nFlashWidth];

	    	/* check the primary (P) offset, this is used to offset into the extended area */
	    	usPrimary = pCFIData[0x15 * nFlashWidth];

	    	/* versions are some of the things stored in the extended area */
	    	usMajVerASCII = pCFIData[(usPrimary+0x3) * nFlashWidth];
	    	usMinVerASCII = pCFIData[(usPrimary+0x4) * nFlashWidth];
	    }
	}

	return 1;
}
