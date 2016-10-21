#include <driver/card/card.h>
#include <stdlib.h>
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
	u32 *Buf = (u32*)buff;
	extern s32 S3c2416_BlkRead(const u32 *Buf, u32 BlkNo, u32 Count);

	if(S3c2416_BlkRead(Buf, sector, count))
    	return (-1);// 失败

    return (0);// 正确
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

	u32 *Buf = (u32*)buff;
	extern s32 S3c2416_BlkWrite(const u32 *Buf, u32 BlkNo, u32 Count);

	if(S3c2416_BlkWrite(Buf, sector, count))
		return (-1);// 失败
    return (0);// 正确
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
int MMC_ioctl( BYTE cmd, void *buff)
{

	extern struct CardDescription Card;
    switch(cmd)
    {
        case GET_SECTOR_SIZE:
            *(DWORD *)(buff) = (1 << Card.BlkLenSettings);//
            break;
        case GET_SECTOR_COUNT:
            *(DWORD*)(buff) = Card.CapacityInBytes >> Card.BlkLenSettings;//
            break;
        case GET_BLOCK_SIZE:
            *(DWORD*)(buff) = 1;// todo
            break;
       default:
           break;
    }
    return (RES_OK);
}

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
extern struct CardDescription Card;
s32  ModuleInstall_SD(const char *ChipName, s32 Clean)
{
	s32 Ret;
	char *Name;
	(void)Clean;

	if(NULL == ChipName)
		return (-1);

	Name = (char*)malloc(strlen(ChipName)+1);
	if(NULL == Name)
		return (-2);

	strcpy(Name, ChipName);

	Ret = S3c2416_HsmmcInit(1);
	if(0 == Ret)
	{
		if(Driver_DeviceCreate(NULL, Name, NULL, NULL, NULL,NULL, NULL, NULL, ((ptu32_t)&SDDrv)))
		{
			if((0 == Clean) || ((Clean) &&
			  (0 == S3c2416_BlkErase(0, ((Card.CapacityInBytes >> Card.BlkLenSettings)-1)))))
			  	  return (0);// 成功
		}

	}

	free(Name);
	return (-3);// 操作失败
}
