



#ifndef __CPU_FLASH_H__
#define __CPU_FLASH_H__

#define CN_FLASH_START_ADDRESS   (0X08000000)
#define CN_FLASH_SIZE			 (0x100000)		//FLASH´óÐ¡Îª1Mbytes
#define CN_APP_START_ADDRESS	 (CN_FLASH_START_ADDRESS + CN_FLASH_SIZE/2)
#define CN_APP_END_ADDRESS		 (CN_FLASH_START_ADDRESS + CN_FLASH_SIZE)


u32 Flash_Write(u32 addr, u8 *buf, u32 nbytes);
u32 Flash_Read(u32 addr, u8 *buf, u32 nbytes);
void Flash_Flush(void);
void Flash_Erase(void);

bool_t CorrectFlashWrite(u32 addr,u8 *wbuf,u8 len);


#endif /* __CPU_FLASH_H__ */
