//#include "integer.h"
//#include "diskio.h"
#if 0	//todo---hemin modified for compile
#include <stdlib.h>
#include <cpu_peri_sdio.h>/* 来源于正点原子 */
#include <djyfs/fat/port/drivers/fat_drivers.h>
#include <djyfs/fat/ff11/src/integer.h>
#include <djyfs/fat/ff11/src/diskio.h>
//-----------------------------------------------------------------------------
//功能:
//参数:
//输出:
//返回:
//-----------------------------------------------------------------------------
int MMC_disk_status(void)
{
    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注: 可以将设备初始化放在这里;
//-----------------------------------------------------------------------------
int MMC_disk_initialize(void)
{
    static BYTE Done = 0;
    if(1 == Done)
        return (0);

    Done = 1;
    return (0);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
int MMC_disk_read(BYTE *buff, DWORD sector, UINT count)
{
#if 0
    SD_Error Status;
    DWORD Memory_Offset = sector << 9;/* 参数值转换,目前根据页大小为512Byte为基准的。 */
    WORD Transfer_Length = count << 9;

    SD_ReadMultiBlocks((uint8_t*)buff, Memory_Offset, Transfer_Length, 1);
    Status = SD_WaitReadOperation();
    while (SD_GetStatus() != SD_TRANSFER_OK);/* 目前采用死等的方式 */
#else
    SD_Error Status;
    Status = SD_ReadDisk(buff, sector, count);
    if(SD_OK != Status)
        return (-1);
#endif
    return (0);/* 正确返回 */
}
//-----------------------------------------------------------------------------
//功能:
//参数: buff -- 写入内容。
//     sector -- 目标页号。
//     count -- 页数量。
//返回: 0 -- 正确;-1 -- 错误;
//备注:
//-----------------------------------------------------------------------------
int MMC_disk_write(BYTE *buff, DWORD sector, UINT count)
{
#if 0
    SD_Error Status;
    DWORD Memory_Offset = sector << 9;/* 参数值转换,目前根据页大小为512Byte为基准的。 */
    WORD Transfer_Length = count << 9;

    Status = SD_WriteMultiBlocks((uint8_t*)buff, Memory_Offset, Transfer_Length, 1);
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
    if ( Status != SD_OK )
    {
        return -1;/* 错误返回 */
    }
#else
    SD_Error Status;
    Status = SD_WriteDisk(buff, sector, count);
    if(SD_OK != Status)
        return (-1);
#endif
    return (0);/* 正确返回 */
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
int MMC_ioctl( BYTE cmd, void *buff)
{

    extern SD_CardInfo SDCardInfo;
    switch(cmd)
    {
        case GET_SECTOR_SIZE:
            *(DWORD *)(buff) = SDCardInfo.CardBlockSize;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD*)(buff) = SDCardInfo.CardCapacity >> 9;/*  */
            break;
        case GET_BLOCK_SIZE:
            *(DWORD*)(buff) = 1;/*  */
            break;
       default:
           break;
    }
    return (RES_OK);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//功能:
//参数: ChipName --
//      Clean -- 器件格式化;"1"--是;"0"--否。
//返回: "0" -- 成功;
//      "-1" -- 输入参数错误;
//      "-2" -- 内存不足;
//      "-3" -- 操作失败;
//备注: todo：抽象一个sd管理结构体
//-----------------------------------------------------------------------------
struct FatDrvFuns SDDrv =
{
	.DrvStatus     = MMC_disk_status,
	.DrvInitialize = MMC_disk_initialize,
	.DrvRead       = MMC_disk_read,
	.DrvWrite      = MMC_disk_write,
	.DrvIoctl      = MMC_ioctl
};
s32  ModuleInstall_SD(const char *ChipName, s32 Clean)
{
	SD_Error Ret;
	char *Name;
	(void)Clean;

	if(NULL == ChipName)
		return (-1);

	Name = (char*)malloc(strlen(ChipName)+1);
	if(NULL == Name)
		return (-2);

	strcpy(Name, ChipName);

	Ret = SD_Init();
	if(SD_OK == Ret)
	{
		if(Driver_DeviceCreate(NULL, Name, NULL, NULL, NULL,NULL, NULL, NULL, (ptu32_t)&SDDrv))
			return (0);// 成功
	}

	free(Name);
	return (-3);// 操作失败
}
#endif
