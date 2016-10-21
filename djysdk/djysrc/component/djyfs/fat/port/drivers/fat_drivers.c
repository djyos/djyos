#include <string.h>
#include "../../ff11/src/ffconf.h"
#include "../../ff11/src/ff.h"
#include "fat_drivers.h"

//
//
//
struct FatDrvFuns g_tFatDrvs[_VOLUMES];
//
// 与ffconf.h中对应
//
PARTITION VolToPart[_VOLUMES] =
{ { 0, 0,}, // 对应"_VOLUME_STRS"中的"RAM"
  { 3, 0,}, // 对应"_VOLUME_STRS"中的"NAND"，"3"表示底层设备号
  { 0, 0,}, // 对应"_VOLUME_STRS"中的"CF"
  { 1, 0,}, // 对应"_VOLUME_STRS"中的"SD"，"1"表示底层设备号
  { 2, 0,}  // 对应"_VOLUME_STRS"中的"USB"，"2"表示底层设备号
};
//-----------------------------------------------------------------------------
//功能:
//参数:
//输出:
//返回:
//-----------------------------------------------------------------------------
s32 FatDrvInitialize(u8 DrvIndex, struct FatDrvFuns *DrvFuns)
{
	if((DrvIndex >= _VOLUMES) || (NULL == DrvFuns))
		return (-1);

	g_tFatDrvs[DrvIndex].DrvInitialize = DrvFuns->DrvInitialize;
	g_tFatDrvs[DrvIndex].DrvIoctl = DrvFuns->DrvIoctl;
    g_tFatDrvs[DrvIndex].DrvRead = DrvFuns->DrvRead;
	g_tFatDrvs[DrvIndex].DrvStatus = DrvFuns->DrvStatus;
	g_tFatDrvs[DrvIndex].DrvWrite = DrvFuns->DrvWrite;

	return (0);
}

