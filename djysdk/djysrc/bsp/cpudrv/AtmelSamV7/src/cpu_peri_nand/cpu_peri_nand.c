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

//思路:
//   采用一个buf表示页和oob区域,而不是将他们分开表示。原因是读部分数据时,ECC校验
//   却是需要整个页和OOB数据。因此带ECC校验时,每次都需读写的整个页和OOB的数据才行
//

#include <string.h>
#include <stdlib.h>
#include <driver.h>
#include <djyos.h>
#include <driver/flash/flash.h>
#include <cpu_peri.h>// 包含"samv71q21.h"
#include <djyfs/filesystems_configs.h>

//
//
//
#define BOARD_NAND_CS               (3)// NAND 片选管脚
#define NAND_COMMAND_CYCLE(x)       ((*(volatile u8*)0x63400000) = ((u8)(x)))// todo
#define NAND_ADDRESS_CYCLE(x)       ((*(volatile u8*)0x63200000) = ((u8)(x)))
#define NAND_WR_DATA_CYCLE(x)       ((*(volatile u8*)0x63000000) = ((u8)(x)))
#define NAND_RD_DATA_CYCLE()        (*(volatile u8*)0x63100000)
//
//
//
//static struct NandDescr *s_ptNandInfo;
struct NandDescr *s_ptNandInfo;// 临时
static u8 s_u8SizeofHammingCode; // ECC校验数据大小
static u8 *s_pu8HammingCode; // ECC校验结果
static const Pin NAND_PINS[] = {
{PIO_PC0A_D0,           PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND D0 管脚
{PIO_PC1A_D1,           PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND D1 管脚
{PIO_PC2A_D2,           PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND D2 管脚
{PIO_PC3A_D3,           PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND D3 管脚
{PIO_PC4A_D4,           PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND D4 管脚
{PIO_PC5A_D5,           PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND D5 管脚
{PIO_PC6A_D6,           PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND D6 管脚
{PIO_PC7A_D7,           PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND D7 管脚

{PIO_PC16A_NANDALE,     PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND ALE 管脚
{PIO_PC17A_NANDCLE,     PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND CLE 管脚
{PIO_PC9A_NANDOE,       PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND OE 管脚
{PIO_PC10A_NANDWE,      PIOC, ID_PIOC, PIO_PERIPH_A, PIO_PULLUP },  // NAND WE 管脚
{PIO_PD19A_NCS3,        PIOD, ID_PIOD, PIO_OUTPUT_1, PIO_DEFAULT},  // NAND CS 管脚PIO_PD19A_NCS3
{PIO_PE2,               PIOE, ID_PIOE, PIO_INPUT,    PIO_PULLUP},   // NAND ready/busy 管脚

{PIO_PD27,              PIOD, ID_PIOD, PIO_OUTPUT_0, PIO_PULLUP}    // NAND WP管脚
};
//
// 本地函数声明
//
static void __NandOff(void);
static void __NandOn(void);
static void __NandWaitReady(void);
static s32 __NandStatus(void);
static void __NandReset(void);
static u32 __NandID(void);

//-----------------------------------------------------------------------------
//功能: 写NAND某页的OOB区
//参数: PageNo -- 页号;
//      Data --  写缓冲
//输出: ">0" -- 写成功; "-2" -- 写失败;
//返回:
//-----------------------------------------------------------------------------
s32 SAMv7_SpareProgram(u32 PageNo, const u8 *Data)
{
    u32 i;
    s32 Ret;
    u32 SpareOffset = s_ptNandInfo->BytesPerPage;

    __NandOn();

    NAND_COMMAND_CYCLE(PAGE_PROGRAM_CMD_BYTE1);

    NAND_ADDRESS_CYCLE(SpareOffset);
    NAND_ADDRESS_CYCLE((SpareOffset>>8) & 0xF);
    NAND_ADDRESS_CYCLE(PageNo);
    NAND_ADDRESS_CYCLE(PageNo>>8);

    for(i = 0; i < (s_ptNandInfo->OOB_Size); i++)
        NAND_WR_DATA_CYCLE(Data[i]);

    NAND_COMMAND_CYCLE(PAGE_PROGRAM_CMD_BYTE2);

    Djy_EventDelay(700);// 切出
    __NandWaitReady(); // 时序要求

    Ret = __NandStatus();

    __NandOff();

    if(Ret)
        return (-2);

    return (s_ptNandInfo->OOB_Size);
}
//-----------------------------------------------------------------------------
//功能: 读NAND某页的OOB区
//参数: PageNo -- 页号;
//      Data --  读缓冲
//输出: ">0" -- 读成功; "-2" -- 写失败;
//返回:
//-----------------------------------------------------------------------------
s32 SAMv7_SpareRead(u32 PageNo, u8 *Data)
{
    u8 i;
    s32 Ret;
    u32 SpareOffset = s_ptNandInfo->BytesPerPage;

    __NandOn();

    NAND_COMMAND_CYCLE(PAGE_READ_CMD_BYTE1);

    NAND_ADDRESS_CYCLE(SpareOffset);
    NAND_ADDRESS_CYCLE((SpareOffset>>8) & 0xF);
    NAND_ADDRESS_CYCLE(PageNo);
    NAND_ADDRESS_CYCLE(PageNo>>8);
    //NAND_ADDRESS_CYCLE((PageNo>>16) & 0x1);

    NAND_COMMAND_CYCLE(PAGE_READ_CMD_BYTE2);

    Djy_EventDelay(25);
    __NandWaitReady(); // 时序要求

    for(i = 0; i < s_ptNandInfo->OOB_Size; i++)
        Data[i] = NAND_RD_DATA_CYCLE();

    Ret = __NandStatus();

    __NandOff();

    if(Ret)
        return (-2);

    return (s_ptNandInfo->OOB_Size);
}

//-----------------------------------------------------------------------------
//功能: 写NAND某页
//参数: PageNo -- 页号;
//      Data -- 写缓冲;
//      Flags -- 写操作控制位;
//返回: ">0" -- 正常，写入的字节数;
//      "-1" -- 参数错误;
//      "-2" -- 写失败;
//备注:
//-----------------------------------------------------------------------------
s32 SAMv7_PageProgram(u32 PageNo, u8 *Data, u32 Flags)
{
    //逻辑:
    //
    u32 i, EccOffset, WrLen = s_ptNandInfo->BytesPerPage;
    s32 Ret;
    u8 *EccSpace;//指向存放ECC校验数据的空间

    // ECC 功能
    switch (Flags & MASK_ECC)
    {
        case NO_ECC :
        case HW_ECC :
        {
            break;//不支持HW_ECC
        }
        case SW_ECC :
        {
            EccOffset = s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size - s_u8SizeofHammingCode;
            EccSpace = (u8*)Data + EccOffset;
            WrLen = s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size;
            memset(s_pu8HammingCode, 0xFF, s_u8SizeofHammingCode);
            hamming_compute_256x(Data, s_ptNandInfo->BytesPerPage, s_pu8HammingCode);
            for(i=0; i<s_u8SizeofHammingCode; i++)
            {
                EccSpace[i] = s_pu8HammingCode[i];
            }
            break;
        }
        default :return (-1);
    }

    if(SPARE_REQ & Flags)
        WrLen = s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size;

    __NandOn();

    NAND_COMMAND_CYCLE(PAGE_PROGRAM_CMD_BYTE1);

    NAND_ADDRESS_CYCLE(0x0);
    NAND_ADDRESS_CYCLE(0x0);
    NAND_ADDRESS_CYCLE(PageNo);
    NAND_ADDRESS_CYCLE(PageNo>>8);

    for(i = 0; i < WrLen; i++)
        NAND_WR_DATA_CYCLE(Data[i]);

    NAND_COMMAND_CYCLE(PAGE_PROGRAM_CMD_BYTE2);// 写入Main数据完成

    Djy_EventDelay(700);// 切出
    __NandWaitReady();// 时序要求

    Ret = __NandStatus();

    __NandOff();

    if(Ret)
        return (-2);

    return (WrLen);

}
//-----------------------------------------------------------------------------
//功能: 读NAND某页
//参数: PageNo -- 页号;
//      Data -- 读缓冲;
//      Flags -- 读操作控制位;
//返回: ">0" -- 正常，读出的字节数。
//      "-1" -- 输入参数错误;
//      "-2" -- 读失败;
//      "-3" -- ECC纠错失败;
//备注: 不管读写是否正确，都将数据回传
//-----------------------------------------------------------------------------
s32 SAMv7_PageRead(u32 PageNo, u8 *Data, u32 Flags)
{
    //逻辑:
    //
    u32 i, RdLen;
    s32 Ret;
    u32 EccRet, EccOffset;


    if(((SW_ECC & Flags) && (HW_ECC & Flags)) ||
       ((NO_ECC & Flags) && (HW_ECC & Flags)) ||
       ((NO_ECC & Flags) && (SW_ECC & Flags)))
        return (-1);

    if((SPARE_REQ & Flags) || (SW_ECC & Flags))
        RdLen = s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size;
    else
        RdLen = s_ptNandInfo->BytesPerPage;

    __NandWaitReady(); // todo，防止冲突

    __NandOn();

    NAND_COMMAND_CYCLE(PAGE_READ_CMD_BYTE1);

    NAND_ADDRESS_CYCLE(0x0);
    NAND_ADDRESS_CYCLE(0x0);
    NAND_ADDRESS_CYCLE(PageNo);
    NAND_ADDRESS_CYCLE(PageNo>>8);

    NAND_COMMAND_CYCLE(PAGE_READ_CMD_BYTE2);

    Djy_EventDelay(25);
    __NandWaitReady(); // 时序要求

    for(i = 0; i < RdLen; i++)
        Data[i] = NAND_RD_DATA_CYCLE();

    Ret = __NandStatus();

    __NandOff();

    if(Ret)
        return (-2);

    if (!((SPARE_REQ & Flags) || (SW_ECC & Flags)))
        return (s_ptNandInfo->BytesPerPage); // 只读页,结束退出

    if(SW_ECC & Flags)
    {
        EccOffset = s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size - s_u8SizeofHammingCode;
        for(i = 0; i < s_u8SizeofHammingCode; i++)
        {
            s_pu8HammingCode[i] = Data[EccOffset+i];
        }

        EccRet = hamming_verify_256x(Data, s_ptNandInfo->BytesPerPage, s_pu8HammingCode);
        if (EccRet && (EccRet != HAMMING_ERROR_SINGLE_BIT))
        {
            TraceDrv(FLASH_TRACE_DEBUG, "cannot be fixed");
            return (-3);
        }
    }
    return (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size);

}

//-----------------------------------------------------------------------------
//功能: 擦除块
//参数: BlkNo -- 块号;
//返回: "0" -- 成功;"-1" -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 SAMv7_BlockErase(u32 BlkNo)
{
    s32 Ret;

    __NandOn();

    NAND_COMMAND_CYCLE(BLOCK_ERASE_CMD_BYTE1);

    NAND_ADDRESS_CYCLE(BlkNo << 6); // 3个地址周期表示的是页号
    NAND_ADDRESS_CYCLE(BlkNo >> 2);
    //NAND_ADDRESS_CYCLE(BlkNo >> 10);

    NAND_COMMAND_CYCLE(BLOCK_ERASE_CMD_BYTE2);

    Djy_EventDelay(3000); // todo
    __NandWaitReady();

    Ret = __NandStatus();

    __NandOff();

    return (Ret);
}

//-----------------------------------------------------------------------------
//功能: 检查块状况（好坏）
//参数: BlkNo -- 块号;
//返回: "0" -- 好块;
//      "-1" -- 坏块;
//      "-2" -- 内存不足
//      "-3" -- 读失败;
//备注:
//-----------------------------------------------------------------------------
s32 SAMv7_BadChk(u32 BlkNo)
{
    u8 *Spare, i;
    s32 Ret = 0;
    u32 PageNo = BlkNo * s_ptNandInfo->PagesPerBlk;

    Spare = malloc (s_ptNandInfo->OOB_Size);
    if (NULL == Spare)
        return (-2);

    for (i = 0; i < 2; i++)// 只判断每块的首两页
    {
        if(-2 == SAMv7_SpareRead(PageNo + i, Spare))
        {
            Ret = -3;
            break;
        }

        if (0xFF != Spare[s_ptNandInfo->BadMarkOffset])
        {
            Ret = -1;
            break;
        }
    }

    free(Spare);

    return (Ret);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: "0" -- 成功;
//      "-1" -- 失败;
//      "-2" -- 内存不足;
//备注:
//-----------------------------------------------------------------------------
s32 SAMv7_BadMark(u32 BlkNo)
{
    //逻辑:
    //    不管BAD MARK位原来是多少以及是否有ECC校验问题, 只管将该位标志为坏块

    u8 *Spare, i;
    s32 Ret = 0;
    u32 PageNo = BlkNo * s_ptNandInfo->PagesPerBlk;

    Spare = malloc (s_ptNandInfo->OOB_Size);
    if (NULL == Spare)
        return (-2);


    memset(Spare, 0xFF, s_ptNandInfo->OOB_Size);
    Spare[s_ptNandInfo->BadMarkOffset] = 0xAA;

    for (i = 0; i < 2; i++)
    {
        if(-2 == SAMv7_SpareProgram(PageNo + i, Spare))
            Ret = -1;
    }

    free(Spare);

    return (Ret);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: "0" -- 成功; "-1" -- 解析失败; "-2" -- 内存不足;
//备注:
//-----------------------------------------------------------------------------
static s32 SAMv7_GetNandDescr(struct NandDescr *Descr)
{
    u16 i;
    s32 Ret = 0;
    char *OnfiBuf;

    OnfiBuf = (char*)malloc(786);
    if (NULL == OnfiBuf)
        return (-1);

    memset(OnfiBuf, 0, 786);

    __NandOn();

    NAND_COMMAND_CYCLE(PARAMETER_PAGE_READ_CMD_BYTE);

    NAND_ADDRESS_CYCLE(0);

    __NandWaitReady();// 时序要求

    for(i = 0; i < 786; i++)
        OnfiBuf[i] = NAND_RD_DATA_CYCLE();

    __NandOff();

//  PrintBuf(OnfiBuf, 786);//测试
//  {
//      char j;
//      for(i = 0; i < 786; i++)
//          j = OnfiBuf[i];
//  }


    if(DecodeONFI((const char*)OnfiBuf, Descr, 0))
        Ret = -1;

    free (OnfiBuf);

    return (Ret);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
#define NCS_RD_SETUP(x)     (((u32)((x)<<24)) & 0xFF000000)
#define NRD_SETUP(x)        (((u32)((x)<<16)) & 0xFF0000)
#define NCS_WR_SETUP(x)     (((u32)((x)<<8)) & 0xFF00)
#define NWE_SETUP(x)        (((u32)(x)) & 0xFF)
#define NCS_RD_PULSE(x)     (((u32)((x)<<24)) & 0xFF000000)
#define NRD_PULSE(x)        (((u32)((x)<<16)) & 0xFF0000)
#define NCS_WR_PULSE(x)     (((u32)((x)<<8)) & 0xFF00)
#define NWE_PULSE(x)        (((u32)(x)) & 0xFF)
#define NRD_CYCLE(x)        (((u32)((x)<<16)) & 0xFFFF0000)
#define NWE_CYCLE(x)        (((u32)(x)) & 0xFFFF)

static void  SAMv7_NAND_Config(void)
{
    //逻辑:
    //
    PMC_EnablePeripheral(ID_SMC);
    PMC_EnablePeripheral(ID_PIOC);

    MATRIX->CCFG_SMCNFCS = (MATRIX->CCFG_SMCNFCS | (0x1u << BOARD_NAND_CS));
    // MLK为150MHz,同时为时序时钟，精度单位为6.7ns
    SMC->SMC_CS_NUMBER[BOARD_NAND_CS].SMC_SETUP = NCS_RD_SETUP(1) | NRD_SETUP(0) |
                                                  NCS_WR_SETUP(1) | NWE_SETUP(0);
    SMC->SMC_CS_NUMBER[BOARD_NAND_CS].SMC_PULSE = NCS_RD_PULSE(4) | NRD_PULSE(4) |
                                                  NCS_WR_PULSE(3) | NWE_PULSE(2);
    SMC->SMC_CS_NUMBER[BOARD_NAND_CS].SMC_CYCLE = NRD_CYCLE(7) | NWE_CYCLE(4);
    SMC->SMC_CS_NUMBER[BOARD_NAND_CS].SMC_MODE = (SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE | SMC_MODE_DBW_8_BIT);

    PIO_Configure(NAND_PINS, PIO_LISTSIZE(NAND_PINS));

    PIO_Set(&NAND_PINS[14]); // 去除写保护

    __NandReset();

    __NandID();
}

//-----------------------------------------------------------------------------
//功能: 安装"nand"模块
//参数: ChipName -- 器件名
//      Flags -- 器件格式化;"1"--是;"0"--否;
//      StartBlk -- 起始块;
//返回: "0" -- 成功;
//      "-1" -- 输入参数错误;
//      "-2" -- 内存不足;
//      "-3" -- 设备信息获取失败。
//备注:
//-----------------------------------------------------------------------------
s32 ModuleInstall_NAND(const char *ChipName, u32 Flags, u16 StartBlk)
{
    //逻辑:
    //   1.芯片管脚等基本设置;
    //   2.(包括判断逻辑)获取芯片的信息;
    //   3.在dev分支下建立NAND节点;
    //   4.初始化NAND节点,包括FLASH的操作函数;

    u32 Len;
    struct FlashChip *Chip;
    struct NandDescr ChipDesrc = {0};

    if (NULL == ChipName)
    {
        TraceDrv(FLASH_TRACE_ERROR, "unavailable param! \r\n");
        return (-1);
    }

    SAMv7_NAND_Config();// 芯片管脚等基本配置

    // 获取NAND信息
    if(SAMv7_GetNandDescr(&ChipDesrc))
    {
        TraceDrv(FLASH_TRACE_ERROR, "解析NAND信息失败\r\n");
        return (-3);
    }

    if(StartBlk >= ChipDesrc.BlksPerLUN * ChipDesrc.LUNs)
        return (-1);

    ChipDesrc.ReservedBlks = StartBlk;
    ChipDesrc.Controller = SW_ECC_SUPPORTED;

    Len = strlen (ChipName) + 1;

    Chip = (struct FlashChip*)malloc(sizeof(struct FlashChip) + Len);
    if (NULL == Chip)
    {
        TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
        return (-2);
    }

    memset (Chip, 0x00, sizeof(*Chip));

    s_u8SizeofHammingCode = (ChipDesrc.BytesPerPage >> 8) * 3; // ((页大小/256)*3)
    ChipDesrc.BadMarkOffset = ChipDesrc.OOB_Size - s_u8SizeofHammingCode - 1;
    s_pu8HammingCode = malloc(s_u8SizeofHammingCode);
    if(NULL == s_pu8HammingCode)
    {
        TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
        free(Chip);
        return (-2);
    }

    Chip->Type                    = F_NAND;
    Chip->Descr.Nand              = ChipDesrc;
    Chip->Ops.RdPage              = SAMv7_PageRead;
    Chip->Ops.WrPage              = SAMv7_PageProgram;
    Chip->Ops.ErsBlk              = SAMv7_BlockErase;
    Chip->Ops.Special.Nand.ChkBlk = SAMv7_BadChk;
    Chip->Ops.Special.Nand.MrkBad = SAMv7_BadMark;

    strcpy(Chip->Name, ChipName); // 设备名

    s_ptNandInfo = &(Chip->Descr);

    Chip->Buf = (u8*)malloc(s_ptNandInfo->OOB_Size + s_ptNandInfo->BytesPerPage); // NAND底层缓冲
    if(NULL == Chip->Buf)
    {
        TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
        free(s_pu8HammingCode);
        free(Chip);
        return (-2);
    }

    Driver_DeviceCreate(NULL, Chip->Name, NULL, NULL, NULL, NULL, NULL, NULL, (ptu32_t)Chip); // 设备接入"/dev"

    if(Flags & FLASH_ERASE_ALL)
        EarseWholeChip(Chip);

    return (0);// 成功;
}
/******************************************************************************
                         PRIVATE FUNCTION(本地私有函数)
******************************************************************************/
//-----------------------------------------------------------------------------
//功能: 检查NAND操作是否出错
//参数:
//返回: 0 -- 正常; -1 -- 错误;
//备注: 未判断busy等状态
//-----------------------------------------------------------------------------
static s32 __NandStatus(void)
{
    u8 Status = 0x1;
    u32 Retry = 0;

    do
    {
        NAND_COMMAND_CYCLE(STATUS_READ_CMD_BYTE);
        Status = NAND_RD_DATA_CYCLE();
    }
    while((Status & 0x1) && (Retry++ < 10));

    if(Retry > 10)
        __NandReset();// todo: 出错就重置芯片

    return(0);
}
//-----------------------------------------------------------------------------
//功能: NAND时序逻辑
//参数:
//返回:
//备注: 实际上是判断NAND芯片的R/Bn引脚，这种判断是必不可少的。
//-----------------------------------------------------------------------------
static void __NandWaitReady(void)
{
    u32 Retry = 0;
    while(0 == PIO_Get(&NAND_PINS[13]))
    {
        if(Retry++ > 100)
            break;
    }

    while(0 == PIO_Get(&NAND_PINS[13]))
    {
        Djy_EventDelay(25);
        if(Retry++ > 100)
            break;
    }
}
//-----------------------------------------------------------------------------
//功能: 置NAND片选（CS）为低
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static void __NandOn(void)
{
    PIO_Clear(&NAND_PINS[12]);
}
//-----------------------------------------------------------------------------
//功能: 置NAND片选（CS）为高
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static void __NandOff(void)
{
    PIO_Set(&NAND_PINS[12]);
}
//-----------------------------------------------------------------------------
//功能: 重置NAND芯片
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static void __NandReset(void)
{
    __NandOn();

    NAND_COMMAND_CYCLE(RESET_CMD_BYTE);

    __NandWaitReady();

    __NandOff();

}
//-----------------------------------------------------------------------------
//功能: 读NAND芯片ID参数
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static u32 __NandID(void)
{
    u32 NAND_ID;
    volatile u32 delay = 0;

    __NandOn();

    NAND_COMMAND_CYCLE(ID_READ_CMD_BYTE);

    NAND_ADDRESS_CYCLE(0);

    for(;;)
    {
        if(delay++ > 0xFFFF)
            break;
    }
    NAND_ID = NAND_RD_DATA_CYCLE();
    NAND_ID |= (NAND_RD_DATA_CYCLE() << 8);
    NAND_ID |= (NAND_RD_DATA_CYCLE() << 16);
    NAND_ID |= (NAND_RD_DATA_CYCLE() << 24);
    //__NandWaitReady();

    __NandOff();

    return (NAND_ID);
}

/******************************************************************************
                         简易文件系统函数
******************************************************************************/
#ifdef EFS_ON
//-----------------------------------------------------------------------------
//功能: 写小于一页的数据
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
#define __WritePage(a,b,c) SAMv7_PageProgram(a,b,c)
#define __ReadPage(a,b,c) SAMv7_PageRead(a,b,c)
static s32 __WriteFragment(u32 PageNo, u32 Offset, const u8 *Buf, u32 Size)
{
    u32 i;
    s32 Ret;

    __NandOn();

    NAND_COMMAND_CYCLE(PAGE_PROGRAM_CMD_BYTE1);

    NAND_ADDRESS_CYCLE(Offset);
    NAND_ADDRESS_CYCLE(Offset>>8);
    NAND_ADDRESS_CYCLE(PageNo);
    NAND_ADDRESS_CYCLE(PageNo>>8);

    for(i = 0; i < Size; i++)
        NAND_WR_DATA_CYCLE(Buf[i]);

    NAND_COMMAND_CYCLE(PAGE_PROGRAM_CMD_BYTE2);// 写入Main数据完成

    Djy_EventDelay(700);// 切出
    __NandWaitReady();// 时序要求

    Ret = __NandStatus();

    __NandOff();

    if(Ret)
        return (-2);

    return (Size);
}
//-----------------------------------------------------------------------------
//功能: 读小于一页的数据
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static s32 __ReadFragment(u32 PageNo, u32 Offset, u8 *Buf, u32 Size)
{
    u32 i;
    s32 Ret;

    __NandWaitReady(); // todo，防止冲突

    __NandOn();

    NAND_COMMAND_CYCLE(PAGE_READ_CMD_BYTE1);

    NAND_ADDRESS_CYCLE(Offset);
    NAND_ADDRESS_CYCLE(Offset>>8);
    NAND_ADDRESS_CYCLE(PageNo);
    NAND_ADDRESS_CYCLE(PageNo>>8);

    NAND_COMMAND_CYCLE(PAGE_READ_CMD_BYTE2);

    Djy_EventDelay(25);
    __NandWaitReady(); // 时序要求

    for(i = 0; i < Size; i++)
        Buf[i] = NAND_RD_DATA_CYCLE();

    Ret = __NandStatus();

    __NandOff();

    if(Ret)
        return (-2);

    return (Size);
}
//-----------------------------------------------------------------------------
//功能: 写数据
//参数: BlkNo -- 块号;
//      Offset -- 块内偏置;
//      Buf -- 写缓冲;
//      Size -- 写数据大小
//      Flags -- 0x0 -- 无功能; 0x1 - ECC功能开启(数据大小为必须256)
//返回:
//备注:
//-----------------------------------------------------------------------------
u32 EFS_IF_WriteData(u32 BlkNo, u32 Offset, u8 *Buf, u32 Size, u8 Flags)
{
    s32 Ret;
    u32 PageNo = (BlkNo << 6) + (Offset >> 11);
    u32 PageOffset = Offset & 0x7FF;
    u32 WrLen = Size;

    if(!Buf)
        return (0);

    // ECC的数据源为256字节，放入数据后端,3个字节
    if(1 == Flags)
    {
        hamming_compute_256x(Buf, 256, Buf+256);
        WrLen = Size + 3;
        PageOffset += 3 * (PageOffset >> 8);
    }

    for(;;)
    {
        if((!PageOffset) && (WrLen >= s_ptNandInfo->BytesPerPage))
            Ret = __WritePage(PageNo, Buf, NO_ECC);
        else if((WrLen >= s_ptNandInfo->BytesPerPage) || // 写数据大与等于一页,但内容跨页
                (((WrLen+PageOffset) > s_ptNandInfo->BytesPerPage) && (!Flags))) // 写数据小于一页,但内容跨页(只有考虑非ECC的情况)
            Ret = __WriteFragment(PageNo, PageOffset, (const u8*)Buf, (s_ptNandInfo->BytesPerPage - PageOffset));
        else
            Ret = __WriteFragment(PageNo, PageOffset, (const u8*)Buf, WrLen);

		if(Ret <= 0)
			break;// 错误或者已写完
				
		WrLen -= Ret;
		if(WrLen <= 0)
			break;
        Buf += Ret;
        PageNo++;
        if(PageOffset)
            PageOffset = 0;
    }

    if(Ret > 0)
        return (Size);
    else
        return (0);
}

//-----------------------------------------------------------------------------
//功能: 读数据
//参数: BlkNo -- 块号;
//      Offset -- 块内偏置;
//      Buf -- 读缓冲;
//      Size -- 读数据大小
//      Flags -- 0x0 -- 无功能; 0x1 - ECC功能开启(数据大小为必须256)
//返回:
//备注:
//-----------------------------------------------------------------------------
u32 EFS_IF_ReadData(u32 BlkNo, u32 Offset, u8 *Buf, u32 Size, u8 Flags)
{
    s32 Ret;
    u32 PageNo = (BlkNo << 6) + (Offset >> 11);
    u32 PageOffset = Offset & 0x7FF;
    u32 RdLen = Size;

    if(!Buf)
        return (0);

    if(1 == Flags)
    {
        RdLen += 3;
        PageOffset += 3 * (PageOffset >> 8);
    }

    for(;;)
    {
        if((!PageOffset) && (RdLen >= s_ptNandInfo->BytesPerPage))
            Ret = __ReadPage(PageNo, Buf, NO_ECC);
        else if((RdLen >= s_ptNandInfo->BytesPerPage) || // 写数据大与等于一页,但内容跨页
                (((RdLen+PageOffset) > s_ptNandInfo->BytesPerPage) && (!Flags))) // 读数据小于一页,但内容跨页(只有非ECC的情况需要考虑)
            Ret = __ReadFragment(PageNo, PageOffset, (const u8*)Buf, (s_ptNandInfo->BytesPerPage - PageOffset));
        else
            Ret = __ReadFragment(PageNo, PageOffset, (const u8*)Buf, RdLen);

		if(Ret <= 0)
			break;// 错误或者已写完

		RdLen -= Ret;
		if(RdLen <= 0)
			break;
        Buf += Ret;
        PageNo++;
        if(PageOffset)
            PageOffset = 0;
    }


    if(Ret <= 0)
        return (0);

    if((1 == Flags) && (256 == Size))
    {
        u8 *HammingCode;
        u32 EccRet;

        HammingCode = Buf + 256;
        EccRet = hamming_verify_256x(Buf, 256, HammingCode);
        if (EccRet && (EccRet != HAMMING_ERROR_SINGLE_BIT))
        {
            TraceDrv(FLASH_TRACE_DEBUG, "cannot be fixed");
            return (0);
        }
    }

    return (Size);
}

//-----------------------------------------------------------------------------
//功能: 擦除
//参数: BlkNo -- 块号;
//返回:
//备注:
//-----------------------------------------------------------------------------
bool_t EFS_IF_Erase(u32 BlkNo)
{
    if(SAMv7_BlockErase(BlkNo))
        return (FALSE);

    return (TRUE);
}
//-----------------------------------------------------------------------------
//功能: 检查文件是否可写
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static u8 TempBuf[259];
s32 EFS_IF_IsFragmentWritten(u32 BlkNo, u32 Offset)
{
    u16 i;
    u32 PageOffset = Offset & 0x7FF;
    u8 EccOffset = 3 * (PageOffset >> 8);

    if(0 == EFS_IF_ReadData(BlkNo, (Offset+EccOffset), TempBuf, 259, 0))
        return (-1);

    for(i = 0; i < 259; i++)
    {
        if(0xFF != TempBuf[i])
            return (-1);
    }

    return (0);
}
//-----------------------------------------------------------------------------
//功能: 未实现
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
bool_t EFS_IF_CheckBlockReady(u32 block,u32 offset,
        u8 *buf,u32 size)
{
    u32 CurBlock,CurBlockOffset;
    u32 BlockSize;
    bool_t result = TRUE;

    if(NULL == s_ptNandInfo)
    {
        return FALSE;
    }

    CurBlock = block;
    CurBlockOffset = offset;
    BlockSize = s_ptNandInfo->BytesPerPage * s_ptNandInfo->PagesPerBlk;

    while(size)
    {
        if(-1 == EFS_IF_IsFragmentWritten(CurBlock,CurBlockOffset))
        {
            result = FALSE;
            break;
        }
        CurBlockOffset += 256;
        CurBlock += CurBlockOffset / BlockSize;
        CurBlockOffset = CurBlockOffset % BlockSize;
        size = (size > 256) ? (size - 256) :0;
    }
    return result;// 文件写入操作不存在改写逻辑
}


/******************************************************************************
                                 本地测试
******************************************************************************/
void EFS_DriverTest(void)
{
    u8 *Size256, *Size256Check, *SizeBig;
    u32 i, TestSize;
    u32 BlkNo, BlkOffset;

    if(ModuleInstall_NAND("nand", 0, 0))
        while(1);

    Size256 = malloc(256+3);
    if(!Size256)
        while(1);

    Size256Check = malloc(256+3);
    if(!Size256Check)
        while(1);

    printf("\r\n---------EFS DRIVER TEST---------\r\n");

    //
    EFS_IF_Erase(0);

    if(0 == EFS_IF_ReadData(0, 0, Size256, 259, 0))
        printf("\r\nBlock 0 offset 0x0 cannot be read!\r\n");
    else
        PrintBuf(Size256, 259);

    for(i = 0; i < 256; i++)
        Size256[i] = i;

    if(0 == EFS_IF_WriteData(0, 0, Size256, 256, 1))
        printf("\r\nBlock 0 offset 0x0 cannot be write!\r\n");

    if(0 == EFS_IF_ReadData(0, 0, Size256, 256, 1))
        printf("\r\nBlock 0 offset 0x0 cannot be read!\r\n");

    PrintBuf(Size256, 259);


    for(BlkNo = 0; BlkNo < 10; BlkNo++)
    {
        for(BlkOffset = 0; BlkOffset < s_ptNandInfo->BytesPerPage * s_ptNandInfo->PagesPerBlk; BlkOffset += 256)
        {
            if(EFS_IF_IsFragmentWritten(BlkNo, BlkOffset))
            {
                if(FALSE == EFS_IF_Erase(BlkNo))
                    printf("\r\nBlock %d cannot be erased!\r\n", BlkNo);
                break;
            }
        }

        for(BlkOffset = 0; BlkOffset < s_ptNandInfo->BytesPerPage * s_ptNandInfo->PagesPerBlk; BlkOffset += 256)
        {
            if(0 == EFS_IF_WriteData(BlkNo, BlkOffset, Size256, 256, 1))
                printf("\r\nBlock %d offset 0x%x cannot be write!\r\n", BlkNo, BlkOffset);
        }

        for(BlkOffset = 0; BlkOffset < s_ptNandInfo->BytesPerPage * s_ptNandInfo->PagesPerBlk; BlkOffset += 256)
        {
            if(0 == EFS_IF_ReadData(BlkNo, BlkOffset, Size256, 256, 1))
                printf("\r\nBlock %d offset 0x%x cannot be read!\r\n", BlkNo, BlkOffset);

            for(i = 0; i < 256; i++)
            {
                if((u8)i != Size256[i])
                {
                    printf("\r\nBlock %d offset 0x%x read is not right!\r\n", BlkNo, BlkOffset);
                    while(1);
                }
            }
        }
    }

    TestSize = 3000;
    SizeBig = malloc(TestSize);
    if(!SizeBig)
        while(1);

    for(i = 0; i < TestSize; i++)
        SizeBig[i] = (u8)i;

    for(BlkNo = 10; BlkNo < 20; BlkNo++)
    {
        for(BlkOffset = 0; BlkOffset < s_ptNandInfo->BytesPerPage * s_ptNandInfo->PagesPerBlk; BlkOffset += 256)
        {
            if(EFS_IF_IsFragmentWritten(BlkNo, BlkOffset))
            {
                if(FALSE == EFS_IF_Erase(BlkNo))
                    printf("\r\nBlock %d cannot be erased!\r\n", BlkNo);
                break;
            }
        }

        for(BlkOffset = 0; (BlkOffset+TestSize) < (s_ptNandInfo->BytesPerPage*s_ptNandInfo->PagesPerBlk); BlkOffset += TestSize)
        {
            if(0 == EFS_IF_WriteData(BlkNo, BlkOffset, SizeBig, TestSize, 0))
                printf("\r\nBlock %d offset 0x%x cannot be write!\r\n", BlkNo, BlkOffset);
        }

        for(BlkOffset = 0; (BlkOffset+TestSize) < (s_ptNandInfo->BytesPerPage*s_ptNandInfo->PagesPerBlk); BlkOffset += TestSize)
        {
            if(0 == EFS_IF_ReadData(BlkNo, BlkOffset, SizeBig, TestSize, 0))
                printf("\r\nBlock %d offset 0x%x cannot be read!\r\n", BlkNo, BlkOffset);

            for(i = 0; i < TestSize; i++)
            {
                if((u8)i != SizeBig[i])
                {
                    printf("\r\nBlock %d offset 0x%x read is not right!\r\n", BlkNo, BlkOffset);
                    while(1);
                }
            }
        }
    }

    // ECC功能
    for(BlkNo = 0; BlkNo < 1; BlkNo++)
    {
        for(BlkOffset = 0; BlkOffset < s_ptNandInfo->BytesPerPage * s_ptNandInfo->PagesPerBlk; BlkOffset += 259)
        {
            if(0 == EFS_IF_ReadData(BlkNo, BlkOffset, Size256, 259, 0))
                printf("\r\nBlock %d offset 0x%x cannot be read!\r\n", BlkNo, BlkOffset);
            Size256[1] = 0;
            if(0 == EFS_IF_WriteData(BlkNo, BlkOffset, Size256, 259, 0))
                printf("\r\nBlock %d offset 0x%x cannot be write!\r\n", BlkNo, BlkOffset);
        }

        for(BlkOffset = 0; BlkOffset < s_ptNandInfo->BytesPerPage * s_ptNandInfo->PagesPerBlk; BlkOffset += 256)
        {
            if(0 == EFS_IF_ReadData(BlkNo, BlkOffset, Size256, 256, 1))
                printf("\r\nBlock %d offset 0x%x cannot be read!\r\n", BlkNo, BlkOffset);

            for(i = 0; i < 256; i++)
            {
                if((u8)i != Size256[i])
                {
                    printf("\r\nBlock %d offset 0x%x read is not right!\r\n", BlkNo, BlkOffset);
                    while(1);
                }
            }
        }
    }
}
#endif
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void PageTest(const u32 PageNo, const u8 Init)
{
    volatile u8 *Buf;
    u32 i, Ret;
    struct FlashChip *Chip;
    u32 ECC_Flags = 0;

    if(Init)
    {
        if(ModuleInstall_NAND("nand", 0, 0)) //安装nand设备
            while(1);
    }

    if(HW_ECC_SUPPORTED & s_ptNandInfo->Controller)
        ECC_Flags |= HW_ECC;

    if(SW_ECC_SUPPORTED & s_ptNandInfo->Controller)
        ECC_Flags |= SW_ECC;

    if(NO_ECC_SUPPORTED & s_ptNandInfo->Controller)
        ECC_Flags |= NO_ECC;

    Chip = Container(s_ptNandInfo, struct FlashChip, Descr);
    Buf = Chip->Buf;
    for(i = 0; i < s_ptNandInfo->BytesPerPage; i++)
        Buf[i] = (u8)i;

    Chip->Ops.WrPage(PageNo, Buf, ECC_Flags | SPARE_REQ);
    memset(Buf, 0, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));
    Ret = Chip->Ops.RdPage(PageNo, Buf, ECC_Flags | SPARE_REQ);
    for(i=0; i<(s_ptNandInfo->BytesPerPage); i++)
    {
        if(((u8)i) != Buf[i])
            while(1);
    }

    // ECC一位纠错
    Buf[1] = 0;
    Chip->Ops.WrPage(PageNo, Buf, NO_ECC);
    memset(Buf, 0, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));
    Ret = Chip->Ops.RdPage(PageNo, Buf, ECC_Flags | SPARE_REQ);
    for(i=0; i<(s_ptNandInfo->BytesPerPage); i++)
    {
        if(((u8)i) != Buf[i])
            while(1);
    }

}

void ContinuityTest(struct FlashChip *Chip)
{
     u32 i, j, Len;
    u32 Blks, Pages;
    s32 BadBlks = 0;
    u8 *Temp;

    if(NULL == Chip)
        while(1);

    Blks = Chip->Descr.Nand.BlksPerLUN * Chip->Descr.Nand.LUNs;
    for(i = Chip->Descr.Nand.ReservedBlks; i < Blks; i++)
    {
        if(0 != Chip->Ops.ErsBlk(i))
        {
            // 擦除失败
//          TraceDrv(NAND_TRACE_ERROR, "block %d cannot be erased!\r\n", i);
            while(1);
        }
    }

    // 擦除后的校验
    Len = (Chip->Descr.Nand.BytesPerPage + Chip->Descr.Nand.OOB_Size);
    Temp = (u8*)malloc(Len);
    if(NULL == Temp)
        while(1);// 内存不足

    for(i = (Chip->Descr.Nand.ReservedBlks * Chip->Descr.Nand.PagesPerBlk); i < (Blks * Chip->Descr.Nand.PagesPerBlk); i++)
    {
        if(Len != Chip->Ops.RdPage(i, Temp, SPARE_REQ | NO_ECC))
            while(1);

        for(j = 0; j < Len; j++)
        {
            if(0xFF != Temp[j])
                while(1);
        }
    }
}

void ChipRawTest(void)
{
    struct FlashChip *Chip;
    u32 i, Ret;
    u8 *Buf;
    u32 TestPages;
    u32 TestBlocks;
    u32 ECC_Flags = 0;
    u32 ErrorCount = 0;

    if(ModuleInstall_NAND("nand", 0, 0))//安装nand设备
        while(1);

    if(HW_ECC_SUPPORTED & s_ptNandInfo->Controller)
        ECC_Flags |= HW_ECC;

    if(SW_ECC_SUPPORTED & s_ptNandInfo->Controller)
        ECC_Flags |= SW_ECC;

    if(NO_ECC_SUPPORTED & s_ptNandInfo->Controller)
        ECC_Flags |= NO_ECC;

    Chip = Container(s_ptNandInfo, struct FlashChip, Descr);

    Buf = Chip->Buf;

    for(TestBlocks = 0; TestBlocks < s_ptNandInfo->BlksPerLUN; TestBlocks++)
    {
        memset(Buf, 0x0, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

        Chip->Ops.ErsBlk(TestBlocks);

        for(TestPages = 0; TestPages < s_ptNandInfo->PagesPerBlk; TestPages++)
        {
            u32 CurPage = TestPages + (TestBlocks * s_ptNandInfo->PagesPerBlk);
            memset(Buf, 0x0, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));
            Ret = Chip->Ops.RdPage(CurPage, Buf, NO_ECC | SPARE_REQ);
            for(i=0; i<(s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size); i++)
            {
                if(0xFF != Buf[i])
                {
                    ErrorCount += 1;
                    //PrintBuf(Buf, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));
                    while(1);
                }
            }

            PageTest(CurPage, 0);
        }
    }


    for(TestBlocks = 0; TestBlocks < s_ptNandInfo->BlksPerLUN; TestBlocks++)
    {
        if(Chip->Ops.ErsBlk(TestBlocks))
            while(1);
    }

    ContinuityTest(Chip);
    while(1);
}




