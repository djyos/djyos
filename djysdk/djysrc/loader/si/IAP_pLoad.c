//----------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2014 著作权由深圳鹏瑞软件有限公司所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//所属模块: Iboot
//作者:  罗侍田.
//版本：V1.0.0
//文件描述: 在应用编程的核心文件
//其他说明:
//修订历史:
//1. 日期: 2016-06-17
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include "stdint.h"
#include "stdio.h"
#include "driver.h"
#include "IAP.h"
#include "djyos.h"
#include <cfg/Iboot_config.h>
#include "verify.h"
#include <stdlib.h>
#include "string.h"
#include "cpu_peri.h"
#include "cpu-optional.h"
#include "IAP_Inner.h"

#if(CN_CPU_OPTIONAL_CACHE==1)
#include "set-cache.h"
#endif

extern void __AppStart(void);
extern struct IbootCtrl gc_ptIbootCtrl;
extern struct AppInfo   gc_ptAppInfo;
extern const u8 g_IbootCRC;
extern u32 gc_AppAddr;

extern void Load_Preload(void);
extern bool_t IAP_IsForceIboot(void);

extern const u8 g_IbootType;
tagIapVar pg_IapVar __attribute__ ((section(".IapBootFlgRam")));


static const u32 g_CRC32_tab[] = {
 0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
 0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
 0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
 0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
 0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
 0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
 0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
 };


static u32 __IAP_crc32( char *buf, int len)
 {
     u32 ret = 0xFFFFFFFF;
     int   i;
     for(i = 0; i < len;i++)
     {
          ret = g_CRC32_tab[(ret ^buf[i] ) &0xFF] ^ (ret >> 8);
     }
      ret = ~ret;
     return ret;
 }
//-----------------------------------------------------------------
//功能：获取APP在存储器的首地址
//参数：无
//返回：32位的地址.
//------------------------------------------------------------------
static u32 __IAP_GetAPPStartAddr(void)
{
     return (u32)(&gc_ptAppInfo.RomStartAddr);
}

//-----------------------------------------------------------------
//功能：获取APP的大小(字节数)
//参数：无
//返回：APP bin文件的大小.
//------------------------------------------------------------------
 static u32 __IAP_GetAPPSize(void)
{
    return (gc_ptIbootCtrl.AppSize);
}

 const char bootflag[]="RunIboot";//要弄成const，若是局部变量，编译器将其放在ROM
//-----------------------------------------------------------------
//功能：检查该跑Iboot还是APP。设备运行APP还是Iboot，可以通过shell切换，切换的方
//      法是，在内存特定位置做标志，然后重启。
//参数：无
//返回：无。
//说明：TRUE = run APP，false = run Iboot。
//-----------------------------------------------------------------
 bool_t IAP_IsRamIbootFlag(void)
 {
 	u8 i;
 	for(i=0;i<8;i++)
 	{
 		if(pg_IapVar.IbootFlag[i]!=bootflag[i])
 		{
 			return false;
 		}
 	}
 	return true;
 }

//----选择加载项目代码-----------------------------------------------------------
//功能：选择加载存储存储器中哪个项目代码,Iboot或APP。
//参数: 无。
//返回: 无。
//----------------------------------------------------------------------------
void IAP_SelectLoadProgam(void)
{
	bool_t result=true;
	u32 crc,len;
	u32 addr;
	pg_IapVar.IbootStatus=EN_NO_ERR;

	result = IAP_IsForceIboot();
	if(result)
	{
		pg_IapVar.IbootStatus=EN_FORCE_IBOOT;
		pg_IapVar.RunMode = CN_IAP_MODE_IBOOT;
		Load_Preload();   //运行Iboot
	}
	else
	{
		result=IAP_IsRamIbootFlag();
		if(result)
		{
			pg_IapVar.IbootStatus=EN_RAM_IBOOT_FLAG;
			pg_IapVar.RunMode = CN_IAP_MODE_IBOOT;
			Load_Preload();   //运行Iboot
		}
		else
	    {
    	     //如果是运行APP且为DirectRun模式,则
    	     if(g_IbootType==EN_DIRECT_RUN)
    	     {
                 //gc_AppAddr的地址是lds中直接赋值的
                 //RomStartAddr是在app.bin中的
                 //如果iboot的memory.lds中和APP的memory.lds中IbootSize定义不一致，
                 //将直接运行Iboot，并且在Iboot启动后，shell中输出相应信息
				if((u32)(&gc_AppAddr)==gc_ptAppInfo.RomStartAddr)
				{
					if(gc_ptIbootCtrl.flag==CN_APP_CTRL_APP_FLAG)
					{
                        //len是IAP下载后写入的。
                        //AppSize是包含在APP.bin中的
                        //如果下载文件不完整，if将不成立，强制运行Iboot
						len=__IAP_GetAPPSize();
						if(gc_ptAppInfo.AppSize==len)
						{
                            //有些高可靠性特别是航天应用中，需要校验flash中的代码
                            //是否被篡改。不要以为flash绝对安全，宇宙射线无处不在。
                            //但是，校验需要消耗很多时间，对于需要快速启动的应用，
                            //或者可靠性要求不那么高的应用，可以在Iboot_Config.c
                            //中关闭CRC校验。
							if(g_IbootCRC==EN_USE_CRC)   //配置为需要校验
							{
								addr=__IAP_GetAPPStartAddr();
								char *buf;
								buf=(char *)addr;
								crc=__IAP_crc32(buf,len);
								if(crc==gc_ptIbootCtrl.Iap_crc)
								{
									pg_IapVar.RunMode = CN_IAP_MODE_APP;
									__AppStart();
								}
								else
								{
									pg_IapVar.IbootStatus=EN_CRC_ERR;
									pg_IapVar.RunMode = CN_IAP_MODE_IBOOT;
									Load_Preload();
								}
							}
							else
							{
								pg_IapVar.RunMode = CN_IAP_MODE_APP;
								__AppStart();
							}
						}
						else
						{
							pg_IapVar.IbootStatus=EN_BIN_INCOMPLETED_ERR;
							pg_IapVar.RunMode = CN_IAP_MODE_IBOOT;
							Load_Preload();
						}
					}
					else if(gc_ptIbootCtrl.flag==CN_APP_CTRL_DBG_FLAG)
					{
						pg_IapVar.RunMode = CN_IAP_MODE_APP;
						__AppStart();
					}
					else
					{
						pg_IapVar.IbootStatus=EN_APP_FLAG_ERR;
						pg_IapVar.RunMode = CN_IAP_MODE_IBOOT;
						Load_Preload();
					}
				}
				else
				{
					if( (gc_ptAppInfo.RomStartAddr == 0) ||
							(gc_ptAppInfo.RomStartAddr == 0xFFFFFFFF))
						pg_IapVar.IbootStatus=EN_FILE_NO_EXSIT_ERR;
					else
						pg_IapVar.IbootStatus=EN_lDS_MISMATCH;
					pg_IapVar.RunMode = CN_IAP_MODE_IBOOT;
					Load_Preload();
				}
			}
			else
			{
				pg_IapVar.IbootStatus=EN_LOAD_FROM_DATA_MODE;
				pg_IapVar.RunMode = CN_IAP_MODE_IBOOT;
				Load_Preload();
			}
	    }
	}
}
