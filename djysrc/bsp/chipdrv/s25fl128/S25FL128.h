/*
 * S25FL128.h
 *
 *  Created on: Sep 6, 2014
 *      Author: Administrator
 */

#ifndef S25FL128_H_
#define S25FL128_H_


//EEPROMÃüÁî×Ö
#define CN_EEP_CMD_READ   			0x03//Read Data Bytes
#define CN_EEP_CMD_FREAD       		0x0B//Fast Read
#define CN_EEP_CMD_RDID       		0x9F//Read ID
#define CN_EEP_CMD_READID       	0x90//Read MID & DID
#define CN_EEP_CMD_WREN       		0x06//WRITE EN
#define CN_EEP_CMD_WRDI       		0x04//WRITE DIS
#define CN_EEP_CMD_SE       		0x20//SECTOR ERASE
#define CN_EEP_CMD_BE       		0x60//CHIP ERASE
#define CN_EEP_CMD_PP       		0x02//PAGE PRO
#define CN_EEP_CMD_RDSR       		0x05//READ FROM STATUS REGISTER
#define CN_EEP_CMD_WRSR       		0x01//WRITE TO STATUS REGISTER
#define CN_EEP_CMD_ENTRY       		0x55//ENTRY PARALLEL MODEL
#define CN_EEP_CMD_EXIT       		0x45//EXIT PARALLEL MODE
#define CN_EEP_CMD_DP       		0xB9
#define CN_EEP_CMD_RES       		0xAB



bool_t EEPROM_SectorErase(u32 Addr);
bool_t EEPROM_ChipErase(void);
bool_t EEPROM_PageProgame(u32 Addr,u8 *buf,u32 len);
u32    EEPROM_Read(u32 Addr, u8 *buf, u32 len);
bool_t EEPROM_ReadID(u8 ManuID,u8 DevID);


#endif /* S25FL128_H_ */
