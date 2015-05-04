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
//所属模块: linkport驱动程序(具体驱动部分)
//作者:  朱海兵.
//版本：V1.0.0
//文件描述:
//其他说明:
//修订历史:
//1. 日期:2014-4-25
//   作者:  朱海兵.
//   新版本号：1.0.0
//   修改说明: 
//------------------------------------------------------
#ifndef __CPU_PERI_LINKPORT_H__
#define __CPU_PERI_LINKPORT_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct         //offset
{
    vu32 rLCTL;       		//0x00
    vu32 rLSTAT;      		//0x01
    vu32 RESERVED0;
    vu32 rLSTAT_SHADOW;		//0x03
    vu32 RESERVED1[4];
    vu32 rTXLB;       		//0x08
    vu32 rRXLB;       		//0x09
    vu32 rRXLB_OUT_SHADOW;	//0x0A
    vu32 rRXLB_IN_SHADOW;	//0x0B
    vu32 rTXLB_OUT_SHADOW;	//0x0C
    vu32 rTXLB_IN_SHADOW;	//0x0D
}tag_LinkPortReg;


typedef enum
{
    LinkPort1,
    LinkPort2
}enum_linkportx;


typedef enum
{
    SendMode,
    RecvMode
}enum_TransMode;



typedef struct         //offset
{
    u32 rIILB;				//0x00
    u32 rIMLB;				//0x01
    u32 rCLB;				//0x02
    u32 rCPLB;				//0x03
}tag_DMA_LinkPortReg;


struct t_LinkPortChainedDMA_TCB
{
	int CPLB;
	int CLB;
	int IMLB;
	int IILB;
};


bool_t  LinkPort_Init(u32 LinkPortNum,u8 TransMode,u8 DMAMode);
bool_t  LinkPort_Enable(u32 LinkPortNum);
bool_t  LinkPort_Disable(u32 LinkPortNum) ;
bool_t  LinkPort_EnableDMA(u32 LinkPortNum);
bool_t LinkPort_DisableDMA(u32 LinkPortNum);
bool_t LinkPort_InitDMA(u32 LinkPortNum,u32* pData,u32 DataLen);
u32  LinkPort_SendData(u32 LinkPortNum,u32 Val);
u32 LinkPort_RecvData(u32 LinkPortNum);
u32 LinkPort0_RecvISR(ufast_t linkport0_int_line);
u32 LinkPort0_SendISR(ufast_t linkport0_int_line);
u32  LinkPort1_RecvISR(ufast_t linkport1_int_line);
u32 LinkPort1_SendISR(ufast_t linkport1_int_line);
bool_t LinkPort_InterruptInit(u32 LinkPortNum,u32 TransMode);
void LinkPort_InitChainedDMA(int InitTCB_Addr);
bool_t  LinkPort_SetLinkPortChainedTCB(int Chained_Num);
bool_t LinkPort_SetRecvDataPoolLen(u8 Data_Pool_Num, u32 Len);
bool_t LinkPort_SetRecvDataPoolAddr(u8 Data_Pool_Num,u32 addr);
void  LinkPort_DefaultIntInit(void);
void LinkPort_EnableDMA_Send(u32 LinkPortNum);
bool_t SetDMAChainedTCB_Send(u32 addr,u32 Len);
void EnableLinkPortDMA_Recv(u32 LinkPortNum);



#ifdef __cplusplus
}
#endif

#endif //__CPU_PERI_LINKPORT_H__

