//-----------------------------------------------------------------------------
// Copyright (c) 2014, SHENZHEN PENGRUI SOFT CO LTD. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主义、无论此责任为因合约关系、无过失责任主义或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <systime.h>
#include <driver.h>
#include <object.h>
#include "vfile.h"
#include "mount.h"

extern struct VCommon g_tDevCom; // driver.c中定义

static s32 DOpen(struct PathWalk *PathContext, u32 Mode, u8 IsFound);
static s32 DClose(void *Device);
static s32 DWrite(const void *Buf, u32 Size, u32 Nmemb, void *Device);
static s32 DRead(void *Buf, u32 Size, u32 Nmemb, void *Device);
static s32 DCntrl(u32 Command, void *Param0, void *Param1, void *Device);
//
// "/dev"目录独立一个体系,但是该目录体系下成员当前不可操作
//
static struct VOperations s_tDOps =
{
    .VOpen      = DOpen,
    .VClose     = DClose,
    .VWrite     = DWrite,
    .VRead      = DRead,
    .VCntrl     = DCntrl,
};

//
// "/dev"目录独立一个体系,但是该目录体系下成员当前部分可操作
//
static struct VMount s_tDevMount =
{
    .VOps       = &s_tDOps,
    .OldObj     = NULL,
    .Property   = MOUNT_ON_DIR | MOUNT_DEV,
    .Context    = NULL,
    .Private    = NULL,
};

//-----------------------------------------------------------------------------
//功能: Dev模块转文件系统模式
//参数: 文件系统操作方法
//返回:
//备注:
//-----------------------------------------------------------------------------
void __DevModule2FS(void *FileOps)
{
    g_tDevCom.Context = (void*)(&s_tDevMount);
    g_tDevCom.Obj.Type = OB_MOUNT | OB_DIR;
    s_tDevMount.Name = g_tDevCom.Obj.Name;
}
//-----------------------------------------------------------------------------
//功能: 打开设备
//参数: 
//返回: 1 -- 成功; 
//      -1 -- 参数输入错误; -2 -- 其他错误;
//备注: 
//-----------------------------------------------------------------------------
static s32 DOpen(struct PathWalk *PathContext, u32 Mode, u8 IsFound)
{
    struct DjyDevice *Dev;
    struct FileContext *FileCt;
    u32 DevMode = 0;
#if 0
    if((!IsFound) || (Mode & M_CREATE))
        return (-1);// 当前逻辑,设备是不能通过Open创建的

    // todo: 类型校验
    Mode &= ~M_OPEN;// 设备不支持依靠open创建
#else
    if(!IsFound)
    	return (-1);
#endif
    FileCt = AllocContext();
    if(!FileCt)
        return (-2);// 内存不足

    FileCt->Property |= P_DEV;// 设备文件

    if((Mode & M_READ) && (Mode & M_WRITE))
    {
        DevMode = O_RDWR;
        FileCt->Property |= (P_READ | P_WRITE);
    }
    else if(Mode & M_READ)
    {
        DevMode = O_RDONLY;
        FileCt->Property |= P_READ;
    }
    else if(Mode & M_WRITE)
    {
        DevMode = O_WRONLY;
        FileCt->Property |= P_WRITE;
    }   

#if 0
    if(Mode)    
        return (-1);// 其他模式当前不支持
#endif

    Dev = Driver_OpenDevice(PathContext->End->Name, DevMode, CN_TIMEOUT_FOREVER);
    if(NULL == Dev)
	{
		FreeContext(FileCt);
        return (-2);// 严重错误
	}
    FileCt->Private = (void*)Dev;

    PathContext->Private = (void*)FileCt;// 传递给VFile
    PathContext->PrivateVHost = &(FileCt->FHost);
    return (1);
}

//-----------------------------------------------------------------------------
//功能: 关闭设备
//参数: Device -- 设备上下文;
//返回: 0 -- 成功; -2 -- 错误;  
//备注: 
//-----------------------------------------------------------------------------
static s32 DClose(void *Device)
{
    bool_t Ret;
    struct FileContext *DevCt = ( struct FileContext *)Device;
    struct DjyDevice *Dev = DevCt->Private;
    
    Ret = Driver_CloseDevice(Dev);
    if(FALSE == Ret)
        return (-2);
    
    FreeContext(DevCt);

    return (0);
}

//-----------------------------------------------------------------------------
//功能: 写设备;
//参数: 
//返回: >0 -- 成功; 0 -- 错误;  
//备注: 
//-----------------------------------------------------------------------------
static s32 DWrite(const void *Buf, u32 Size, u32 Nmemb, void *Device)
{
    u32 Ret;
    u32 DSize = Size * Nmemb;
    struct FileContext *DevCt = ( struct FileContext *)Device;
    struct DjyDevice *Dev = DevCt->Private;
    

    Ret = Driver_WriteDevice(Dev, (u8*)Buf, DSize, 0, CN_BLOCK_BUFFER, CN_TIMEOUT_FOREVER);
    if(-1 == Ret)
        return (0);
    return ((s32)Ret);
}
//-----------------------------------------------------------------------------
//功能: 读设备
//参数: 
//返回: >0 -- 成功; 0 -- 错误;  
//备注: 
//-----------------------------------------------------------------------------
static s32 DRead(void *Buf, u32 Size, u32 Nmemb, void *Device)
{
    u32 Ret;
    u32 DSize = Size * Nmemb;
    struct FileContext *DevCt = ( struct FileContext *)Device;
    struct DjyDevice *Dev = DevCt->Private;
    
    Ret = Driver_ReadDevice(Dev, (u8*)Buf, DSize, 0, CN_TIMEOUT_FOREVER);
    if(-1 == Ret)
        return (0);
    return ((s32)Ret);
}
//-----------------------------------------------------------------------------
//功能: 控制设备.
//参数: 
//返回: -1 -- 错误;  
//备注: 
//-----------------------------------------------------------------------------
static s32 DCntrl(u32 Command, void *Param0, void *Param1, void *Device)
{
    u32 Ret;
    struct FileContext *DevCt = ( struct FileContext *)Device;
    struct DjyDevice *Dev = DevCt->Private;
    
    Ret = Driver_CtrlDevice(Dev, Command, (ptu32_t)Param0, (ptu32_t)Param1);
    
    return ((s32)Ret);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
struct DjyDevice *ToDev(struct VFile *Fp)
{
	struct FileContext *DevCt;

	if((NULL == Fp) || (NULL == Fp->Private))
		return (NULL);

	DevCt = (struct FileContext *)Fp->Private;

	return ((struct DjyDevice *)(DevCt->Private));
}
