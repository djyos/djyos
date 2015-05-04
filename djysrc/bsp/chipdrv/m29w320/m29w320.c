/*******************************************************************************/
/*                                                                             */
/*   (C) Copyright 2008 - Analog Devices, Inc.  All rights reserved.           */
/*                                                                             */
/*    FILE:     m29w320.c                                                      */
/*                                                                             */
/*    PURPOSE:  Performs operations specific to the M29W320 flash device.      */
/*                                                                             */
/*******************************************************************************/


/* includes */
#include <cdef21469.h>
#include <def21469.h>

#include <stdio.h>
#include <drivers\flash\util.h>
#include <drivers\flash\Errors.h>

#include <sru.h>
#include <sysreg.h>

#include "stdint.h"
#include "string.h"
#include "board-cfg.h"
#include "board_config.h"

#include "m29w320.h"
#include "easynorflash\easynorflash.h"


static struct tagEasynorFlashChip norflash_chip;
static u8 *chip_sector_buf;

#define cn_flash_size                   0x280000    //20Mbits / 2.5MByte (专门分配给文件系统使用的空间:block31~block70)
#define cn_flash_sector_length          0x10000     //64KByte/sector 一个扇区有多少个字节
#define cn_flash_sector_sum             (cn_flash_size/cn_flash_sector_length)
#define cn_flash_base_addr              (cn_sys_nor_flash_base+0x00180000)  //这块FLASH的block0-7是8k的，而其余block是64k的，
                                                    //为使用方便跳过前面的block，从实际的block31开始当作第一个block
#define cn_flash_start_addr             cn_sys_nor_flash_base   //实际的block0的起始地址
//#define cmd_erase_sector 0x30



/* 21469 EZ-Board rev 0.1 was configured for x16 mode, rev 0.2 and greater
    used x8 mode.  Yet in all cases only the lower 8 bits were used so you
    will see throughout this driver only the lower 8 data bits are checked */
#ifdef _21469_EZBOARD_REV_0_1_
#define X16MODE                 /* define for x16 (word) mode */
#endif


/* constants, if you have a custom target you may need to change these */
#ifdef __ADSP21469__
static char *pBoardDesc =       "ADSP-21469 EZ-Board";  /* description of supported platform */
#define FLASH_START_ADDRESS     0x4000000               /* location of the start of flash */
#define SW_LOCKED_SECTOR        70                      /* sector protected by this driver */
static char *pFlashDesc =       "M29W320EB";            /* flash description */
static char *pDeviceCompany =   "STMicroelectronics";   /* flash manufacturer */
#elif __ADSP21479__
static char *pBoardDesc =       "ADSP-21479 EZ-Board";  /* description of supported platform */
#define FLASH_START_ADDRESS     0x4000000               /* location of the start of flash */
#define SW_LOCKED_SECTOR        70                      /* sector protected by this driver */
static char *pFlashDesc =       "M29W320EB";            /* flash description */
static char *pDeviceCompany =   "STMicroelectronics";   /* flash manufacturer */
#elif __ADSP21489__
static char *pBoardDesc =       "ADSP-21489 EZ-Board";  /* description of supported platform */
#define FLASH_START_ADDRESS     0x4000000               /* location of the start of flash */
#define SW_LOCKED_SECTOR        70                      /* sector protected by this driver */
static char *pFlashDesc =       "M29W320EB";            /* flash description */
static char *pDeviceCompany =   "STMicroelectronics";   /* flash manufacturer */
#else
/* we should not fall in here */
#error "*** The flash driver does not yet support this processor ***"
#endif

static int      gNumSectors;        /* number of sectors */
static int      g_ManId = 0x0;      /* flash manufacturer id */
static int      g_DevId = 0x0;      /* flash device id */

/* function prototypes */
static ERROR_CODE EraseFlash(unsigned long ulStartAddr);
static ERROR_CODE EraseBlock( int nBlock, unsigned long ulStartAddr );
static ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulStartAddr);
static ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector );
static ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector );
static ERROR_CODE PollToggleBit(unsigned long ulOffset);
static ERROR_CODE ReadFlash(unsigned long ulOffset, unsigned short *pusValue );
static ERROR_CODE ResetFlash(unsigned long ulStartAddr);
static ERROR_CODE WriteFlash(unsigned long ulOffset, unsigned short usValue );
static unsigned long GetFlashStartAddress( unsigned long ulAddr);


ERROR_CODE m29w320_Open(void)
{
    /* make sure we are connecting to a supported part */
    GetCodes(&g_ManId, &g_DevId, FLASH_START_ADDRESS);

    /* only check lower 8 bits */
    if ( (FLASH_MANUFACTURER_ST != g_ManId) || ((FLASH_DEVCODE_ST_M29W320EB & 0xff) != (0x00ff & g_DevId)) )
    {
        return UNKNOWN_COMMAND;
    }

    /* now that we know what we're connected to, update some variables */
    if ( FLASH_DEVCODE_ST_M29W320DB == g_DevId)
    {
        /* 32 Mbit (4 MB) device */
        gNumSectors = 67;
        pFlashDesc = "STMicro. M29W320DB";
    }

    if ( (FLASH_DEVCODE_ST_M29W320EB & 0xff) == (0x00ff & g_DevId))
    {
        /* 32 Mbit (4 MB) device */
        gNumSectors = 71;
        pFlashDesc = "STMicro. M29W320EB";
    }

    return (NO_ERR);
}


ERROR_CODE m29w320_Close(void)
{
    return (NO_ERR);
}


ERROR_CODE m29w320_Read(unsigned short *pusData,
                        unsigned long ulStartAddress,
                        unsigned int uiCount )
{
    ERROR_CODE Result = NO_ERR;
    unsigned int i = 0;
    unsigned short *pusCurrentData = pusData;
    unsigned long ulCurrentAddress = ulStartAddress;


    for (i = 0; i < uiCount; i++, ulCurrentAddress++, pusCurrentData++)
    {
        Result = ReadFlash( ulCurrentAddress, pusCurrentData );
    }

    return(Result);
}


ERROR_CODE m29w320_Write(unsigned short *pusData,
                         unsigned long ulStartAddress,
                         unsigned int uiCount )
{
    ERROR_CODE Result = NO_ERR;
    unsigned int i = 0;
    unsigned short *pusCurrentData = pusData;
    unsigned long ulCurrentAddress = ulStartAddress;
    unsigned long ulFlashStartAddress;
    unsigned long ulSectStart = 0x0, ulSectEnd = 0x0;

/* if we are protecting a sector we need to know its boundaries */
#ifdef LOCK_CONFIG_FLASH_SECTOR
        GetSectorStartEnd( &ulSectStart, &ulSectEnd, SW_LOCKED_SECTOR );
#endif

    for (i = 0; i < uiCount; i++, ulCurrentAddress++, pusCurrentData++)
    {
        /* get flash start address from absolute address */
        ulFlashStartAddress = GetFlashStartAddress(ulCurrentAddress);

        /* if we are protecting a sector make sure this write doesn't fall in the sector */
#ifdef LOCK_CONFIG_FLASH_SECTOR
        GetSectorStartEnd( &ulSectStart, &ulSectEnd, SW_LOCKED_SECTOR );
        if ( (ulFlashStartAddress < (FLASH_START_ADDRESS + ulSectStart)) ||
             (ulFlashStartAddress > (FLASH_START_ADDRESS + ulSectEnd)) )
#endif
        {
            // unlock flash
#ifdef X16MODE
            WriteFlash( ulFlashStartAddress + 0x0555, 0xaa );
            WriteFlash( ulFlashStartAddress + 0x02aa, 0x55 );
            WriteFlash( ulFlashStartAddress + 0x0555, 0xa0 );
#else
            WriteFlash( ulFlashStartAddress + 0x0aaa, 0xaa );
            WriteFlash( ulFlashStartAddress + 0x0555, 0x55 );
            WriteFlash( ulFlashStartAddress + 0x0aaa, 0xa0 );
#endif

            // program our actual value now
            Result = WriteFlash( ulCurrentAddress, *pusCurrentData );

            // make sure the write was successful
            Result = PollToggleBit(ulCurrentAddress);
        }
    }

    return(Result);
}


ERROR_CODE m29w320_Control( unsigned int uiCmd, COMMAND_STRUCT *pCmdStruct )
{
    ERROR_CODE ErrorCode = NO_ERR;

    // switch on the command
    switch ( uiCmd )
    {
        // erase all
        case CNTRL_ERASE_ALL:
            ErrorCode = EraseFlash(pCmdStruct->SEraseAll.ulFlashStartAddr);
            break;

        // erase sector
        case CNTRL_ERASE_SECT:
            ErrorCode = EraseBlock( pCmdStruct->SEraseSect.nSectorNum, pCmdStruct->SEraseSect.ulFlashStartAddr );
            break;

        // get manufacturer and device codes
        case CNTRL_GET_CODES:
            ErrorCode = GetCodes((int *)pCmdStruct->SGetCodes.pManCode, (int *)pCmdStruct->SGetCodes.pDevCode, (unsigned long)pCmdStruct->SGetCodes.ulFlashStartAddr);
            break;

        case CNTRL_GET_DESC:
            //Filling the contents with data
            //pCmdStruct->SGetDesc.pTitle = pBoardDesc;
            pCmdStruct->SGetDesc.pDesc  = pFlashDesc;
            pCmdStruct->SGetDesc.pFlashCompany  = pDeviceCompany;
            break;

        // get sector number based on address
        case CNTRL_GET_SECTNUM:
            ErrorCode = GetSectorNumber( pCmdStruct->SGetSectNum.ulOffset, (int *)pCmdStruct->SGetSectNum.pSectorNum );
            break;

        // get sector number start and end offset
        case CNTRL_GET_SECSTARTEND:
            ErrorCode = GetSectorStartEnd( pCmdStruct->SSectStartEnd.pStartOffset, pCmdStruct->SSectStartEnd.pEndOffset, pCmdStruct->SSectStartEnd.nSectorNum );
            break;

        // get the number of sectors
        case CNTRL_GETNUM_SECTORS:
            pCmdStruct->SGetNumSectors.pnNumSectors[0] = gNumSectors;
            break;

        // reset
        case CNTRL_RESET:
            ErrorCode = ResetFlash(pCmdStruct->SReset.ulFlashStartAddr);
            break;

        // no command or unknown command do nothing
        default:
            // set our error
            ErrorCode = UNKNOWN_COMMAND;
            break;
    }

    // return
    return(ErrorCode);
}


//-----  H e l p e r   F u n c t i o n s    ----//

//----------- R e s e t F l a s h  ( ) ----------//
//
//  PURPOSE
//      Sends a "reset" command to the flash.
//
//  INPUTS
//      unsigned long ulStartAddr - flash start address
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise

ERROR_CODE ResetFlash(unsigned long ulAddr)
{
    // send the reset command to the flash
#ifdef X16MODE
    WriteFlash( ulAddr + 0x02aa, 0xf0 );
#else
    WriteFlash( ulAddr + 0x0555, 0xf0 );
#endif

    // reset should be complete
    return NO_ERR;
}


//----------- E r a s e F l a s h  ( ) ----------//
//
//  PURPOSE
//      Sends an "erase all" command to the flash.
//
//  INPUTS
//      unsigned long ulStartAddr - flash start address
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise

ERROR_CODE EraseFlash(unsigned long ulAddr)
{
    ERROR_CODE ErrorCode = NO_ERR;  /* tells us if there was an error erasing flash */
    int nBlock = 0;                 /* index for each block to erase */

/* if we are protecting a sector in software we can't do an erase all, just do one
    at a time and skip the protected sector */
#ifdef LOCK_CONFIG_FLASH_SECTOR

    for ( nBlock = 0; ( nBlock < gNumSectors ) && ( ErrorCode == NO_ERR ); nBlock++ )
    {
        if ( SW_LOCKED_SECTOR != nBlock )
        {
            ErrorCode = EraseBlock( nBlock, ulAddr );
        }
    }

#else

    /* erase contents of flash */
#ifdef X16MODE
    WriteFlash( ulAddr + 0x0555, 0xaa );
    WriteFlash( ulAddr + 0x02aa, 0x55 );
    WriteFlash( ulAddr + 0x0555, 0x80 );
    WriteFlash( ulAddr + 0x0555, 0xaa );
    WriteFlash( ulAddr + 0x02aa, 0x55 );
    WriteFlash( ulAddr + 0x0555, 0x10 );
#else
    WriteFlash( ulAddr + 0x0aaa, 0xaa );
    WriteFlash( ulAddr + 0x0555, 0x55 );
    WriteFlash( ulAddr + 0x0aaa, 0x80 );
    WriteFlash( ulAddr + 0x0aaa, 0xaa );
    WriteFlash( ulAddr + 0x0555, 0x55 );
    WriteFlash( ulAddr + 0x0aaa, 0x10 );
#endif

    /* poll until the command has completed */
    ErrorCode = PollToggleBit(ulAddr + 0x0000);

#endif

    /* erase should be complete */
    return ErrorCode;
}


//----------- E r a s e B l o c k ( ) ----------//
//
//  PURPOSE
//      Sends an "erase block" command to the flash.
//
//  INPUTS
//      int nBlock - block to erase
//      unsigned long ulStartAddr - flash start address
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise

ERROR_CODE EraseBlock( int nBlock, unsigned long ulAddr )
{

    ERROR_CODE    ErrorCode   = NO_ERR;     //tells us if there was an error erasing flash
    unsigned long ulSectStart = 0x0;        //stores the sector start offset
    unsigned long ulSectEnd   = 0x0;        //stores the sector end offset(however we do not use it here)


    if ( (nBlock < 0) || (nBlock > gNumSectors)  )
        return INVALID_BLOCK;

/* if we are protecting a sector in software we can't do an erase all, just do one
    at a time and skip the protected sector */
#ifdef LOCK_CONFIG_FLASH_SECTOR
    if ( SW_LOCKED_SECTOR == nBlock )
        return NO_ACCESS_SECTOR;
#endif

    // Get the sector start offset
    // we get the end offset too however we do not actually use it for Erase sector
    GetSectorStartEnd( &ulSectStart, &ulSectEnd, nBlock );

    // send the erase block command to the flash
#ifdef X16MODE
    WriteFlash( (ulAddr + 0x0555), 0xaa );
    WriteFlash( (ulAddr + 0x02aa), 0x55 );
    WriteFlash( (ulAddr + 0x0555), 0x80 );
    WriteFlash( (ulAddr + 0x0555), 0xaa );
    WriteFlash( (ulAddr + 0x02aa), 0x55 );
#else
    WriteFlash( (ulAddr + 0x0aaa), 0xaa );
    WriteFlash( (ulAddr + 0x0555), 0x55 );
    WriteFlash( (ulAddr + 0x0aaa), 0x80 );
    WriteFlash( (ulAddr + 0x0aaa), 0xaa );
    WriteFlash( (ulAddr + 0x0555), 0x55 );
#endif

    // the last write has to be at an address in the block
    WriteFlash( (ulAddr + ulSectStart), 0x30 );

    // poll until the command has completed
    ErrorCode = PollToggleBit(ulAddr + ulSectStart);

    // block erase should be complete
    return ErrorCode;
}


//----------- P o l l T o g g l e B i t ( ) ----------//
//
//  PURPOSE
//      Polls the toggle bit in the flash to see when the operation
//      is complete.
//
//  INPUTS
//  unsigned long ulAddr - address in flash
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise


ERROR_CODE PollToggleBit(unsigned long ulAddr)
{
    ERROR_CODE ErrorCode = NO_ERR;  // flag to indicate error
    unsigned short sVal1;
    unsigned short sVal2;

    // read flash 1 time
    ReadFlash( ulAddr, &sVal1 );

    while( ErrorCode == NO_ERR )
    {
        // read the value 2 times
        ReadFlash( ulAddr, &sVal1 );
        ReadFlash( ulAddr, &sVal2 );

        // XOR to see if any bits are different
        sVal1 ^= sVal2;

        // see if we are toggling
        if( !(sVal1 & 0x40) )
            break;

        // check error bit
        if( !(sVal2 & 0x20) )
            continue;
        else
        {
            // read the value 2 times
            ReadFlash( ulAddr, &sVal1 );
            ReadFlash( ulAddr, &sVal2 );

            // XOR to see if any bits are different
            sVal1 ^= sVal2;

            // see if we are toggling
            if( !(sVal1 & 0x40) )
                break;
            else
            {
                ErrorCode = POLL_TIMEOUT;
                ResetFlash(ulAddr);
            }
        }
    }

    // we can return
    return ErrorCode;
}


//----------- G e t C o d e s ( ) ----------//
//
//  PURPOSE
//      Sends an "auto select" command to the flash which will allow
//      us to get the manufacturer and device codes.
//
//  INPUTS
//      int *pnManCode - pointer to manufacture code
//      int *pnDevCode - pointer to device code
//      unsigned long ulStartAddr - flash start address
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise

ERROR_CODE GetCodes(int *pnManCode, int *pnDevCode, unsigned long ulAddr)
{
    unsigned long ulFlashStartAddr;     //flash start address

    // get flash start address from absolute address
    // The ulAddr should ideally be pointing to the flash start
    // address. However we just verify it here again.
    ulFlashStartAddr = GetFlashStartAddress(ulAddr);

    // send the auto select command to the flash
#ifdef X16MODE
    WriteFlash( ulAddr + 0x0555, 0xaa );
    WriteFlash( ulAddr + 0x02aa, 0x55 );
    WriteFlash( ulAddr + 0x0555, 0x90 );
    ReadFlash( ulAddr + 0x0200,(unsigned short *)pnManCode );
    ReadFlash( ulAddr + 0x0201, (unsigned short *)pnDevCode );
#else
    WriteFlash( ulAddr + 0x0aaa, 0xaa );
    WriteFlash( ulAddr + 0x0555, 0x55 );
    WriteFlash( ulAddr + 0x0aaa, 0x90 );
    ReadFlash( ulAddr + 0x0200,(unsigned short *)pnManCode );
    ReadFlash( ulAddr + 0x0202, (unsigned short *)pnDevCode );
#endif

    // mask off the valid bits
    *pnManCode &= 0x00FF;
    *pnDevCode &= 0xFFFF;

    // we need to issue another command to get the part out
    // of auto select mode so issue a reset which just puts
    // the device back in read mode
    ResetFlash(ulAddr);

    // ok
    return NO_ERR;
}


//----------- G e t S e c t o r N u m b e r ( ) ----------//
//
//  PURPOSE
//      Gets a sector number based on the offset.
//
//  INPUTS
//      unsigned long ulAddr - absolute address
//      int      *pnSector     - pointer to sector number
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise

ERROR_CODE GetSectorNumber( unsigned long ulAddr, int *pnSector )
{
    int nSector = 0;
    int i;
    int error_code = 1;
    unsigned long ulMask;                   //offset mask
    unsigned long ulOffset;                 //offset
    unsigned long ulStartOff;
    unsigned long ulEndOff;

    ulMask            = 0x3fffff;
    ulOffset          = ulAddr & ulMask;

    for(i = 0; i < gNumSectors; i++)
    {
        GetSectorStartEnd(&ulStartOff, &ulEndOff, i);
        if ( (ulOffset >= ulStartOff)
            && (ulOffset <= ulEndOff) )
        {
            error_code = 0;
            nSector = i;
            break;
        }
    }

    // if it is a valid sector, set it
    if (error_code == 0)
        *pnSector = nSector;
    // else it is an invalid sector
    else
        return INVALID_SECTOR;

    // ok
    return NO_ERR;
}


//----------- G e t S e c t o r S t a r t E n d ( ) ----------//
//
//  PURPOSE
//      Gets a sector start and end address based on the sector number.
//
//  INPUTS
//      unsigned long *ulStartOff - pointer to the start offset
//      unsigned long *ulEndOff - pointer to the end offset
//      int nSector - sector number
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise

ERROR_CODE GetSectorStartEnd( unsigned long *ulStartOff, unsigned long *ulEndOff, int nSector )
{
    long lSectorSize = 0;
    long lMainStart = 0x10000;

    // The blocks in the flash memory are asymmetrically arranged
    // Thus we use block sizes to determine the block addresses
    unsigned long  ulBootBlkSize    = 0x4000;   //size of Boot Block
    unsigned long  ulParamBlkSize   = 0x2000;   //size of Parameter Block
    unsigned long  ulSmMainBlkSize  = 0x8000;   //size of Small Main Block
    unsigned long  ulMainBlkSize    = 0x10000;  //size of Main Block
    int nNumEBParamBlocks = 8;

    // if it is the M29320DB
    if( gNumSectors == 67 )
    {

        // M29W320DB Bottom Boot Block Addresses (x8)
        // Boot Block-used for small initilization code
        if( nSector == 0 )
        {
            *ulStartOff = 0x0000;
            *ulEndOff = ( (*ulStartOff) + (ulBootBlkSize ) );
        }
        else if( nSector == 1 ) //Parameter Block
        {
            *ulStartOff = 0x4000;
            *ulEndOff = ( (*ulStartOff) + (ulParamBlkSize) );
        }
        else if( nSector == 2 ) //Parameter Block
        {
            *ulStartOff = 0x6000;
            *ulEndOff = ( (*ulStartOff) + (ulParamBlkSize) );
        }
        else if( nSector == 3 ) //Small Main Block-where applications may be stored
        {
            *ulStartOff = 0x8000;
            *ulEndOff = ( (*ulStartOff) + (ulSmMainBlkSize) );
        }
        else if( ( nSector >= 4 ) && ( nSector < gNumSectors ) ) // 64 Main Blocks
        {
            *ulStartOff = lMainStart + ( ( nSector - 4 ) * ulMainBlkSize );
            *ulEndOff = ( (*ulStartOff) + ulMainBlkSize );
        }
        // no such sector
        else
            return INVALID_SECTOR;
    }
    // M29W320EB
    else
    {
        if( (nSector >= 0) && (nSector < nNumEBParamBlocks) )
        {
            *ulStartOff = nSector * ulParamBlkSize;
            *ulEndOff = ( (*ulStartOff) + (ulParamBlkSize ) );
        }
        else if( ( nSector >= nNumEBParamBlocks ) && ( nSector < gNumSectors ) ) // 64 Main Blocks
        {
            *ulStartOff = lMainStart + ( ( nSector - nNumEBParamBlocks ) * ulMainBlkSize );
            *ulEndOff = ( (*ulStartOff) + ulMainBlkSize );
        }
        // no such sector
        else
            return INVALID_SECTOR;
    }

    *ulEndOff -= 1;

    // ok
    return NO_ERR;
}


//----------- G e t F l a s h S t a r t A d d r e s s ( ) ----------//
//
//  PURPOSE
//      Gets flash start address from an absolute address.
//
//  INPUTS
//      unsigned long ulAddr - absolute address
//
//  RETURN VALUE
//      unsigned long - Flash start address

unsigned long GetFlashStartAddress( unsigned long ulAddr)
{

    ERROR_CODE    ErrorCode = NO_ERR;       //tells us if there was an error erasing flash
    unsigned long ulFlashStartAddr;         //flash start address
    unsigned long ulSectStartAddr;          //sector start address
    unsigned long ulSectEndAddr;            //sector end address
    unsigned long ulMask;                   //offset mask

    // get flash start address from absolute address
    GetSectorStartEnd( &ulSectStartAddr, &ulSectEndAddr, 0);
    ulMask            = ~(ulSectEndAddr);
    ulFlashStartAddr  =  ulAddr & ulMask;

    return(ulFlashStartAddr);
}


//----------- R e a d F l a s h ( ) ----------//
//
//  PURPOSE
//      Reads a value from an address in flash.
//
//  INPUTS
//      unsigned long ulAddr - the address to read from
//      int pnValue - pointer to store value read from flash
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise

ERROR_CODE ReadFlash( unsigned long ulAddr, unsigned short *pusValue )
{
    // set our flash address to where we want to read
    volatile unsigned short *pFlashAddr = (volatile unsigned short *)(ulAddr);

    // read the value
    *pusValue = (unsigned short)*pFlashAddr;

    // ok
    return NO_ERR;
}


//----------- W r i t e F l a s h ( ) ----------//
//
//  PURPOSE
//      Write a value to an address in flash.
//
//  INPUTS
//      unsigned long  ulAddr - address to write to
//      unsigned short nValue - value to write
//
//  RETURN VALUE
//      ERROR_CODE - value if any error occurs
//      NO_ERR     - otherwise

ERROR_CODE WriteFlash( unsigned long ulAddr, unsigned short usValue )
{
    // set the address
    volatile unsigned short *pFlashAddr = (volatile unsigned short *)(ulAddr);

    *pFlashAddr = usValue;

    // ok
    return NO_ERR;
}

/*--------------------------------djyfs-------------------------------------*/

void reset_norflash(void)
{
    WriteFlash( cn_flash_start_addr + 0x0555, 0xf0 );
}

bool_t erase_sector(u32 sector)
{
    u16 *sector_addr;
    if(sector >= cn_flash_sector_sum)
        return false;
    sector_addr = (u16*)(sector*cn_flash_sector_length+cn_flash_base_addr);

    WriteFlash( (cn_flash_start_addr + 0x0aaa), 0xaa );
    WriteFlash( (cn_flash_start_addr + 0x0555), 0x55 );
    WriteFlash( (cn_flash_start_addr + 0x0aaa), 0x80 );
    WriteFlash( (cn_flash_start_addr + 0x0aaa), 0xaa );
    WriteFlash( (cn_flash_start_addr + 0x0555), 0x55 );

    // the last write has to be at an address in the block
    WriteFlash( sector_addr, 0x30 );

    // poll until the command has completed
    PollToggleBit(sector_addr);

    return true;
}

bool_t query_sector_ready(u32 sector,u32 offset,u8 *buf,u32 size)
{
    u16 *sector_addr;
    u32 i,length;
    u16 wd,wdn;

    if(sector >= cn_flash_sector_sum)           //参数错误
        return false;
    if(offset +size > cn_flash_sector_length)   //超出本sector
        size = cn_flash_sector_length - offset;

    //计算从offset到size的所需比较的flash中字长度
//    length = ((offset + size)/2 + (offset + size)%2 )-offset/2;
    sector_addr = (u16*)(sector*cn_flash_sector_length+cn_flash_base_addr);
//    __wait_end_s29glxxx();
    PollToggleBit(sector_addr);

    for(i = 0; i < size; i++)
    {
        ReadFlash(sector_addr+i+offset,&wd);
        wdn = buf[i];
        //检查有没有把0改成1的;检查buf为NULL的情况
        if(((wd | wdn) != wd)
                    || ((buf == NULL)&&(wd != 0xFF)))
            return false;
    }
    return true;
}

u32 read_data(u32 sector,u32 offset,u8 *buf,u32 size)
{
    u16 *sector_addr;
    u32 i,length;

    if(sector >= cn_flash_sector_sum)           //参数错误
        return CN_LIMIT_UINT32;
    if(offset +size > cn_flash_sector_length)   //超出本sector
        size = cn_flash_sector_length - offset;

    //计算从offset到size的所需比较的flash中字长度
//    length = ((offset + size)/2 + (offset + size)%2 )-offset/2;
    sector_addr = (u16*)(sector*cn_flash_sector_length+cn_flash_base_addr);
//    __wait_end_s29glxxx();
    PollToggleBit(sector_addr);

    for(i = 0; i < size; i++)
    {
            ReadFlash(sector_addr+i+offset,&buf[i]);
    }
    return size;
}

void __byte_write_m29w320(u16 * addr,u8 data)
{
    u8 data_t=0;
    ReadFlash(*addr, &data_t);
    if(data_t == data)
        return;
    WriteFlash( cn_flash_start_addr + 0x0aaa, 0xaa );
    WriteFlash( cn_flash_start_addr + 0x0555, 0x55 );
    WriteFlash( cn_flash_start_addr + 0x0aaa, 0xa0 );

    // program our actual value now
    WriteFlash( *addr, data );

    // make sure the write was successful
    PollToggleBit(*addr);

    return;
}

u32 write_data(u32 sector,u32 offset,u8 *buf,u32 size)
{
    u16 *sector_addr;
    u32 i,length;
    u32 wd;

    if(sector >= cn_flash_sector_sum)       //参数错误
        return CN_LIMIT_UINT32;
    if(offset +size > cn_flash_sector_length)//超出本sector
        size = cn_flash_sector_length - offset;

    sector_addr = (u16*)(sector*cn_flash_sector_length+cn_flash_base_addr);
//    length = ((offset + size)/2 + (offset + size)%2 )-offset/2;

    //把写入的内容填写到起始扇区缓冲区中
    for(i = 0;i < size; i++)
    {
        wd=sector_addr+i+offset;
        __byte_write_m29w320(&wd, buf[i]);
    }

    return size;
}

//----初始化norflash芯片----------------------------------------------------
//功能：初始化norflash芯片用于文件系统
//参数: 无
//返回: 0= 成功，1=失败
//--------------------------------------------------------------------------
ptu32_t module_init_fs_m29w320(ptu32_t para)
{
    reset_norflash();//复位norflash芯片
    chip_sector_buf = (u8*)M_Malloc(cn_flash_sector_length,0);
    if(chip_sector_buf == NULL)
        return 0;

    strcpy(norflash_chip.PTT_name,"easy nor flash");
    norflash_chip.block_size     = cn_flash_sector_length;//块大小
    norflash_chip.start_block     = easynorflash_start_sector;//该文件系统起始sector
    norflash_chip.block_sum     = easynorflash_length_sector;//sector用于存放该文件系统
    norflash_chip.block_buf     = chip_sector_buf;
    norflash_chip.files_sum     = 0;
    norflash_chip.erase_block         = erase_sector;
    norflash_chip.query_block_ready   = query_sector_ready;
    norflash_chip.read_data           = read_data;
    norflash_chip.write_data          = write_data;

    if(Easynor_InstallChip(&norflash_chip) == 0)
        return 0;
    else
    {
        free(chip_sector_buf);
        return 1;
    }
}

