/*
 * S25FL128.c
 *
 *  Created on: Sep 6, 2014
 *      Author: Administrator
 */

#include "stdint.h"
#include "stddef.h"
#include "S25FL128.h"



bool_t __EEPROM_WriteEnable()
{

}

bool_t __EEPROM_WriteDisable()
{

}



bool_t EEPROM_SectorErase(u32 Addr)
{
    return true;
}

bool_t EEPROM_ChipErase(void)
{
    return true;
}

bool_t EEPROM_PageProgame(u32 Addr,u8 *buf,u32 len)
{
    return true;
}

u32    EEPROM_Read(u32 Addr, u8 *buf, u32 len)
{
    return 0;
}

bool_t EEPROM_ReadID(u8 ManuID,u8 DevID)
{

    return true;
}






