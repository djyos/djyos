



#ifndef __CPU_FLASH_H__
#define __CPU_FLASH_H__

void Flash_CacheMpuEnable(u8 IsEnable);
u32 Flash_Write(u32 addr, u8 *buf, u32 nbytes);
u32 Flash_Read(u32 addr, u8 *buf, u32 nbytes);
void Flash_Erase(void);
void Flash_Unlock(void);
void Flash_Lock(void);


#endif /* __CPU_FLASH_H__ */
