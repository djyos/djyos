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
// 文件名     ：M24256.c
// 模块描述: E2PROM器件驱动，使用了DJYBUS 的IIC模块
// 模块版本: V1.00
// 创建人员: hm
// 创建时间: 07/10.2014
// =============================================================================


#if (CN_CFG_DJYBUS_IIC_BUS == 1)
#include "cpu_peri.h"
#include "iicbus.h"

#define E2ROM_Address                   (0x50)
#define CN_E2ROM_MAX_SIZE               (0x40000)   //256K BYTES
#define CN_E2ROM_PAGE_SIZE              (64)        // BYTE
#define CN_E2ROM_MAX_PAGE               (CN_E2ROM_MAX_SIZE/CN_E2ROM_PAGE_SIZE)

#define CN_CHIP_WRITE_FINISHED_TIME            (6*mS)
static struct IIC_Device pg_E2ROM_Dev;
#define CN_E2ROM_OP_TIMEOUT             CN_TIMEOUT_FOREVER//2000*mS


// =============================================================================
// 功能：读数据
// 参数：Addr,读数据的地址
//       pDstBuf,读数据的存储地址
//       DstLen,地址长度
// 返回：读取的数据量，字节
// =============================================================================
u32 E2PROM_ReadData(u32 Addr,u8 *pDstBuf,u32 DstLen)
{
    u32 result = 0;
    if((Addr + DstLen > CN_E2ROM_MAX_SIZE) || (DstLen == 0))
        return result;
    
    result = IIC_Read(&pg_E2ROM_Dev,Addr,pDstBuf,DstLen,CN_E2ROM_OP_TIMEOUT);
    
    return result;
}

// =============================================================================
// 功能：在指定地址写入一个字节的数据
// 参数：Addr,写入地址
//       Value,写入的数据
// 返回：写入的数据量
// =============================================================================
u32 E2PROM_WriteByte(u32 Addr,u8 Value)
{
    u32 result = 0;
    if(Addr > CN_E2ROM_MAX_SIZE)
        return result;
    result = IIC_Write(&pg_E2ROM_Dev,Addr,&Value,1,true,CN_E2ROM_OP_TIMEOUT);
    
    return result;
}

// =============================================================================
// 功能：按页写，最大字节不能超过一页大小的数据,WC是芯片的写控制引脚，每次写之前需拉低
//       电平，写完需拉高电平，但是本板件WC一直是低电平，这样导致写完后，芯片需花一定
//       时间去判断是否写完
// 参数：PageNo,页号
//       pSrcBuf,写入的数据
//       SrcLen,写入字节数，必须小于一页数据量
// 返回：写入的字节数
// =============================================================================
u32 E2PROM_WritePage(u32 PageNo,u8 *pSrcBuf,u32 SrcLen)
{
    u32 Addr,result=0;
    
    if((PageNo > CN_E2ROM_MAX_PAGE) || (SrcLen == 0)
            || (SrcLen > CN_E2ROM_PAGE_SIZE) || (NULL == pSrcBuf))
        return result;
    
    Addr = PageNo * CN_E2ROM_PAGE_SIZE;
    result = IIC_Write(&pg_E2ROM_Dev,Addr,pSrcBuf,SrcLen,1,CN_E2ROM_OP_TIMEOUT);
    
    Djy_EventDelay(CN_CHIP_WRITE_FINISHED_TIME);
    return result;
}

// =============================================================================
// 功能：初始化EEPROM芯片，并将其挂接到IIC总线上面，使用了DJYBUS的模型
// 参数：para，暂时没用到
// 返回：true,正确;false,错误
// =============================================================================
ptu32_t M24256_ModuleInit(ptu32_t para)
{
    pg_E2ROM_Dev.DevAddr = E2ROM_Address;
    pg_E2ROM_Dev.BitOfMemAddrInDevAddr = 0;
    pg_E2ROM_Dev.BitOfMemAddr = 16;
    
    IIC0_Init();

    if(NULL == IIC_DevAdd_r("IIC0","IICDev_M24256",&pg_E2ROM_Dev))
        return false;
    else
        return true;
}

#endif
