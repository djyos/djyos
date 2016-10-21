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
#include <driver/flash/flash.h>
#include <cpu_peri.h>
#include <djyos.h>


#define NAND_CONTROLLER_REG                 (pg_nand_reg)
#define NandChipOn()                        (NAND_CONTROLLER_REG->NFCONT &= ~(1<<1))
#define NandChipOff()                       (NAND_CONTROLLER_REG->NFCONT |= (1<<1))
#define SetNandCommand(cmd)                 (NAND_CONTROLLER_REG->NFCMD = cmd)
#define SetNandAddr(i, addr)                (NAND_CONTROLLER_REG->NFADDR = addr)
#define SetNandData(data)                   (NAND_CONTROLLER_REG->NFDATA = data)
#define GetNandData()                       (NAND_CONTROLLER_REG->NFDATA)


static struct NandDescr *s_ptNandInfo;
static void ResetNand(void);
static s32 StatusOfNand(void);
static void WaitNandReady(void);
//-----------------------------------------------------------------------------
//功能:
//参数:
//输出: ">0" -- 写成功; "-2" -- 写失败;
//返回:
//-----------------------------------------------------------------------------
s32 S3C2416_SpareProgram(u32 PageNo, const u8 *Data)
{

    u32 i;
    s32 Ret;
    u32 SpareOffset = s_ptNandInfo->BytesPerPage;

    NandChipOn();

    SetNandCommand(PAGE_PROGRAM_CMD_BYTE1);

    NAND_CONTROLLER_REG->NFADDR = (u8)SpareOffset;
    NAND_CONTROLLER_REG->NFADDR = (u8)((SpareOffset>>8) & 0xF);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo>>8);
    NAND_CONTROLLER_REG->NFADDR = (u8)((PageNo>>16) & 0x1);

    for(i = 0; i < (s_ptNandInfo->OOB_Size); i++)
        SetNandData(Data[i]);

    SetNandCommand(PAGE_PROGRAM_CMD_BYTE2);// 写入Main数据完成

    Djy_EventDelay(700);// 切出
    WaitNandReady();//时序要求

    Ret = StatusOfNand();

    NandChipOff();

    if(Ret)
        return (-2);

    return (s_ptNandInfo->OOB_Size);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//输出: ">0" -- 读成功; "-2" -- 写失败;
//返回:
//-----------------------------------------------------------------------------
s32 S3C2416_SpareRead(u32 PageNo, u8 *Data)
{
    u8 i;
    s32 Ret;
    u32 SpareOffset = s_ptNandInfo->BytesPerPage;

    NandChipOn();

    SetNandCommand(PAGE_READ_CMD_BYTE1);

    NAND_CONTROLLER_REG->NFADDR = (u8)(SpareOffset);
    NAND_CONTROLLER_REG->NFADDR = (u8)((SpareOffset>>8) & 0xF);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo>>8);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo>>16) & 0x1;

    SetNandCommand(PAGE_READ_CMD_BYTE2);

    Djy_EventDelay(25);// 切出
    WaitNandReady();//时序要求

    for(i = 0; i < s_ptNandInfo->OOB_Size; i++)
        Data[i] = GetNandData();

    Ret = StatusOfNand();

    NandChipOff();

    if(Ret)
        return (-2);

    return (s_ptNandInfo->OOB_Size);
}

//-----------------------------------------------------------------------------
//功能:
//参数: Data -- 数据空间。不能为NULL。
//      Flags --
//返回: ">0" -- 正常，写入的字节数;
//      "-1" -- 参数错误;
//      "-2" -- 写失败;
//备注:
//-----------------------------------------------------------------------------
s32  S3C2416_PageProgram(u32 PageNo, const u8 *Data, u32 Flags)
{
    //逻辑:
    //
    u32 i, EccOffset;
    s32 Ret;
    u8 *Spare;

    // ECC 功能
    Ret = NAND_CONTROLLER_REG->NFMECC0;//测试
    switch (Flags & MASK_ECC)
    {
        case HW_ECC :
        {
            NAND_CONTROLLER_REG->NFCONT &= (~(1<<7));// 开启(解锁)Main ECC
            NAND_CONTROLLER_REG->NFCONT |= (1<<5);// 初始化Main ECC
            break;
        }
        case SW_ECC :
        case NO_ECC :
        {
            NAND_CONTROLLER_REG->NFCONT |= (1<<7);// 关闭Main ECC
            break;
        }
        default :return (-1);
    }

    NandChipOn();

    SetNandCommand(PAGE_PROGRAM_CMD_BYTE1);

    NAND_CONTROLLER_REG->NFADDR = 0x0;
    NAND_CONTROLLER_REG->NFADDR = 0x0;
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo>>8);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo>>16) & 0x1;

    for(i = 0; i < s_ptNandInfo->BytesPerPage; i++)
        SetNandData(Data[i]);

    SetNandCommand(PAGE_PROGRAM_CMD_BYTE2);// 写入Main数据完成

    Djy_EventDelay(700);// 切出
    WaitNandReady();// 时序要求

    Ret = StatusOfNand();

    NandChipOff();

    if(Ret)
        return (-2);

    if (!((SPARE_REQ & Flags) || (HW_ECC & Flags)))
        return (s_ptNandInfo->BytesPerPage);// 只写页,结束退出

    Spare = (u8*)Data + s_ptNandInfo->BytesPerPage;// 注意：这里是基于驱动都有统一的缓冲块逻辑

    if(HW_ECC & Flags)
    {
        NAND_CONTROLLER_REG->NFCONT |= (1<<7);// 关闭(锁)Mian ECC
        EccOffset = s_ptNandInfo->OOB_Size - 4;// 4个字节(1-bit ECC校验)
        Ret = NAND_CONTROLLER_REG->NFMECC0;//测试
        *(u32*)(Spare + EccOffset) = NAND_CONTROLLER_REG->NFMECC0;//
    }

    if(!(SPARE_REQ & Flags))
        memset(Spare, 0xFF, EccOffset);// 未要求写spare,则默认写0xFF

    if(-2 == S3C2416_SpareProgram(PageNo, Spare))
        return (-2);

    return (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size);
}
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: ">0" -- 正常，读出的字节数。
//      "-1" -- 输入参数错误;
//      "-2" -- 读失败;
//      "-3" -- ECC纠错失败;
//备注: 不管读写是否正确，都将数据回传
//-----------------------------------------------------------------------------
s32  S3C2416_PageRead(u32 PageNo, u8 *Data, u32 Flags)
{
    //逻辑:
    //
    u32 i;
    s32 Ret;
    u8 *Spare;

    // ecc 功能
    switch (Flags & MASK_ECC)
    {
        case HW_ECC :
        {
            NAND_CONTROLLER_REG->NFCONT &= (~(1<<7));// 开启(解锁)Main ECC
            NAND_CONTROLLER_REG->NFCONT |= (1<<5);// 初始化Main ECC
            break;
        }
        case SW_ECC :
        case NO_ECC :
        {
            NAND_CONTROLLER_REG->NFCONT |= (1<<7);// 关闭Main ECC
            break;
        }
        default : return (-1);
    }

    NandChipOn();

    SetNandCommand(PAGE_READ_CMD_BYTE1);

    NAND_CONTROLLER_REG->NFADDR = 0x0;
    NAND_CONTROLLER_REG->NFADDR = 0x0;
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo>>8);
    NAND_CONTROLLER_REG->NFADDR = (u8)(PageNo>>16) & 0x1;

    SetNandCommand(PAGE_READ_CMD_BYTE2);

    Djy_EventDelay(25);// 切出
    WaitNandReady();// 时序要求

    for(i = 0; i < s_ptNandInfo->BytesPerPage; i++)
        Data[i] = GetNandData();

    Ret = StatusOfNand();

    NandChipOff();

    if(Ret)
        return (-2);

    if (!((SPARE_REQ & Flags) || (HW_ECC & Flags)))
        return (s_ptNandInfo->BytesPerPage);// 只读页,结束退出

    Spare = Data + s_ptNandInfo->BytesPerPage;// 注意：这里是基于驱动都有统一的缓冲块逻辑

    if(HW_ECC & Flags)
        NAND_CONTROLLER_REG->NFCONT |= (1<<7);// 关闭(锁)Mian ECC

    if(-2 == S3C2416_SpareRead(PageNo, Spare))
        return (-2);

    if(HW_ECC & Flags)
    {
        u32 EccRes, EccOffset, EccStatus;

        EccOffset = s_ptNandInfo->OOB_Size - 4;//
        EccRes = *(u32*)(Spare + EccOffset);
        // 针对YAFFS2的逻辑,HW_ECC读一个新擦除块的页，ECC校验码为0xFFFFFFFF(-1),
        // 代入if里的逻辑会执行纠错，修改读回的数据, 需将屏蔽掉这个错误
        if(EccRes != -1)
        {
            NAND_CONTROLLER_REG->NFMECCD0 = ((u32)((EccRes & 0xFF00) << 8) | (u32)(EccRes & 0xFF));
            NAND_CONTROLLER_REG->NFMECCD1 = ((u32)((EccRes & 0xFF000000) >> 8) | (u32)(EccRes & 0xFF0000) >> 16);

            EccStatus = NAND_CONTROLLER_REG->NFECCERR0;
            switch(EccStatus & 0x3)
            {
                case 0: break;// 无错
                case 1:// 1位错，可纠错
                {
                    Data[(EccStatus >> 7) & 0x7ff] ^= (1 << ((EccStatus >> 4) & 0x7));
                    TraceDrv(FLASH_TRACE_DEBUG, "fix: bit 0x%x in Byte 0x%x\r\n",
                            ((EccStatus >> 4) & 0x7), ((EccStatus >> 7) & 0x7ff));
                    break;
                }
                case 2:// 错误位太多, 不可纠错
                case 3:// ECC计算错误
                    TraceDrv(FLASH_TRACE_DEBUG, "cannot be fixed");
                    return (-3);
            }
        }
    }

    return (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size);

}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: "0" -- 成功;"-1" -- 失败;
//备注:
//-----------------------------------------------------------------------------
s32 S3C2416_BlockErase(u32 BlkNo)
{
    s32 Ret;

    NandChipOn();

    SetNandCommand(BLOCK_ERASE_CMD_BYTE1);

    NAND_CONTROLLER_REG->NFADDR = (u8)(BlkNo << 6);// 3个地址周期表示的是页号
    NAND_CONTROLLER_REG->NFADDR = (u8)(BlkNo >> 2);
    NAND_CONTROLLER_REG->NFADDR = (u8)(BlkNo >> 10);

    SetNandCommand(BLOCK_ERASE_CMD_BYTE2);

    Djy_EventDelay(3000);// 切出
    WaitNandReady();

    Ret = StatusOfNand();

    NandChipOff();

    return (Ret);
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回: "0" -- 好块;
//      "-1" -- 坏块;
//      "-2" -- 内存不足
//      "-3" -- 读失败;
//备注:
//-----------------------------------------------------------------------------
s32 S3C2416_BadChk(u32 BlkNo)
{
    u8 *Spare, i;
    s32 Ret = 0;
    u32 PageNo = BlkNo * s_ptNandInfo->PagesPerBlk;

    Spare = malloc (s_ptNandInfo->OOB_Size);
    if (NULL == Spare)
        return (-2);

    for (i = 0; i < 2; i++)// 只判断每块的首两页
    {
        if(-2 == S3C2416_SpareRead(PageNo + i, Spare))
        {
            Ret = -3;
            break;
        }

        if ((0xAA == Spare[s_ptNandInfo->BadMarkOffset]) &&
            (0xAA == Spare[s_ptNandInfo->BadMarkOffset-1]))
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
s32  S3C2416_BadMark(u32 BlkNo)
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
    Spare[s_ptNandInfo->BadMarkOffset-1] = 0xAA;

    for (i = 0; i < 2; i++)
    {
        if(-2 == S3C2416_SpareProgram(PageNo + i, Spare))
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
static s32 S3C2416_GetNandDescr(struct NandDescr *Descr)
{
    u16 i;
    s32 Ret = 0;
    char *OnfiBuf;

    OnfiBuf = (char*)malloc(786);
    if (NULL == OnfiBuf)
        return (-1);

    memset(OnfiBuf, 0, 786);

    NandChipOn();

    SetNandCommand(PARAMETER_PAGE_READ_CMD_BYTE);

    NAND_CONTROLLER_REG->NFADDR = 0;

    WaitNandReady();// 时序要求

    for(i = 0; i < 786; i++)
        OnfiBuf[i] = GetNandData();

    NandChipOff();

//  PrintBuf(OnfiBuf, 786);//测试

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
static void  S3C2416_NAND_ControllerConfig(void)
{
    //逻辑:
    //

    //nand config register
    NAND_CONTROLLER_REG->NFCONF = (0<<23)|// ECCType       [24:23]    0:1-bit ECC
                                  (1<<12)|// TACLS         [14:12]    CLE&ALE duration = HCLK*TACLS.
                                  (3<<8)| // TWRPH0        [10:8]     TWRPH0 duration = HCLK*(TWRPH0+1)
                                  (1<<4)| // TWRPH1        [6:4]      TWRPH1 duration = HCLK*(TWRPH1+1)
                                  (1<<3)| // AdvFlash(R)   [3]        Advanced NAND, 0:256/512, 1:1024/2048
                                  (0<<2)| // PageSize(R)   [2]        NAND memory page size
                                  (1<<1)| // AddrCycle(R)  [1]        NAND flash addr size
                                  (0<<0); // BusWidth(R/W) [0]        NAND bus width. 0:8-bit, 1:16-bit.

    //nand control register
    NAND_CONTROLLER_REG->NFCONT = (0<<13)| // Lock-tight   [13]    0:Disable lock, 1:Enable lock.
                                  (0<<12)| // Soft Lock    [12]    0:Disable lock, 1:Enable lock.
                                  (0<<10)| // EnablillegalAcINT[10] Illegal access interupt control.0:Disable,1:Enable
                                  (0<<9)|  // EnbRnBINT    [9] RnB interrupt. 0:Disable, 1:Enable
                                  (0<<8)|  // RnB_TrandMode[8] RnB transition detection config.0:Low->High,1:High->Low
                                  (1<<6)|  // SpareECCLock [6]     0:Unlock, 1:Lock
                                  (1<<5)|  // MainECCLock  [5]     0:Unlock, 1:Lock
                                  (1<<4)|  // InitECC(W)   [4]     1:Init ECC decoder/encoder.
                                  (1<<1)|  // Reg_nCE      [1]     0:nFCE=0, 1:nFCE=1.
                                  (1<<0);  // NANDC Enable [0]     operating mode. 0:Disable, 1:Enable.

    ResetNand();
}
//-----------------------------------------------------------------------------
//功能:
//参数: ChipName --
//      Clean -- 器件格式化;"1"--是;"0"--否。
//返回: "0" -- 成功;
//      "-1" -- 输入参数错误;
//      "-2" -- 内存不足;
//      "-3" -- 设备信息获取失败。
//备注:
//-----------------------------------------------------------------------------
s32 ModuleInstall_NAND(const char *ChipName, u32 Flags, u16 StartBlk)
{
    /*逻辑:
       1.芯片管脚等基本设置;
       2.(包括判断逻辑)获取芯片的信息;
       3.在dev分支下建立NAND节点;
       4.初始化NAND节点,包括FLASH的操作函数;
    */
    u32 Len;
    struct FlashChip *Chip;
    struct NandDescr ChipDesrc = {0};

    if (NULL == ChipName)
    {
        TraceDrv(FLASH_TRACE_ERROR, "unavailable param! \r\n");
        return (-1);
    }

    S3C2416_NAND_ControllerConfig();// 芯片管脚等基本配置

    // 获取NAND信息
    if(S3C2416_GetNandDescr(&ChipDesrc))
    {
        TraceDrv(FLASH_TRACE_ERROR, "解析NAND信息失败\r\n");
        return (-3);
    }

    if(StartBlk >= ChipDesrc.BlksPerLUN * ChipDesrc.LUNs)
        return (-1);

    ChipDesrc.ReservedBlks = StartBlk;
    ChipDesrc.Controller = HW_ECC_SUPPORTED;
    ChipDesrc.BadMarkOffset = ChipDesrc.OOB_Size - 4 - 1;
    Len = strlen (ChipName) + 1;

    Chip = (struct FlashChip*)malloc(sizeof(struct FlashChip) + Len);
    if (NULL == Chip)
    {
        TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
        return (-2);
    }

    memset (Chip, 0x00, sizeof(*Chip));

    Chip->Type            = F_NAND;
    Chip->Descr.Nand      = ChipDesrc;
    Chip->Ops.RdPage      = S3C2416_PageRead;
    Chip->Ops.WrPage      = S3C2416_PageProgram;
    Chip->Ops.ErsBlk      = S3C2416_BlockErase;
    Chip->Ops.Special.Nand.ChkBlk = S3C2416_BadChk;
    Chip->Ops.Special.Nand.MrkBad = S3C2416_BadMark;

    strcpy(Chip->Name, ChipName);// 设备名

    s_ptNandInfo = &(Chip->Descr);

    Chip->Buf = (u8*)malloc(s_ptNandInfo->OOB_Size + s_ptNandInfo->BytesPerPage);// NAND底层缓冲
    if(NULL == Chip->Buf)
    {
        TraceDrv(FLASH_TRACE_ERROR, "out of memory!\r\n");
        free(Chip);
        return (-2);
    }

    Driver_DeviceCreate(NULL, Chip->Name, NULL, NULL, NULL, NULL, NULL, NULL, (ptu32_t)Chip);// 设备接入"/dev"

    if(Flags & FLASH_ERASE_ALL)
        EarseWholeChip(Chip);

    return (0);// 成功;
}
/******************************************************************************
                         PRIVATE FUNCTION(本地私有函数)
******************************************************************************/
//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static void ResetNand(void)
{
    NandChipOn();

    SetNandCommand(RESET_CMD_BYTE);

    WaitNandReady();

    NandChipOff();
}
//-----------------------------------------------------------------------------
//功能: 检查NAND操作是否出错
//参数:
//返回: 0 -- 正常; -1 -- 错误;
//备注: 未判断busy等状态
//-----------------------------------------------------------------------------
static s32 StatusOfNand(void)
{
    u8 Status = 0x1;

    do
    {
        SetNandCommand(STATUS_READ_CMD_BYTE);
        Status = GetNandData();
    }
    while(Status & 0x1);

    return(0);
}

//-----------------------------------------------------------------------------
//功能: Nand时序逻辑
//参数:
//返回:
//备注: 实际上是判断NAND芯片的R/Bn引脚，这种判断是必不可少的
//-----------------------------------------------------------------------------
static void WaitNandReady(void)
{
    while(0 == (NAND_CONTROLLER_REG->NFSTAT & 0x1));
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
static void __WaitEccDone(void)
{
    ;//1-bit ECC 没有判断的位
}

//-----------------------------------------------------------------------------
//功能:
//参数:
//返回:
//备注:
//-----------------------------------------------------------------------------
void RawTest(void)
{
    struct FlashChip *Chip;
    u32 i;
    u8 *Buf;
    u32 TestPage = 640;

    ModuleInstall_NAND("nand", 0, 0);//安装nand设备

    Chip = Container(s_ptNandInfo, struct FlashChip, Descr);

    Buf = Chip->Buf;

    memset(Buf, 0x0, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    Chip->Ops.ErsBlk(TestPage/s_ptNandInfo->PagesPerBlk);

    Chip->Ops.RdPage(TestPage, Buf, NO_ECC | SPARE_REQ);

    //PrintBuf(Buf, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    for(i = 0; i < s_ptNandInfo->BytesPerPage; i++)
        Buf[i] = (u8)i;

    Chip->Ops.WrPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    memset(Buf, 0xFF, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    Chip->Ops.RdPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    //PrintBuf(Buf, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    //测试ECC功能,1位出错情况
    // 1次
    Buf[1] = 0;

    Chip->Ops.WrPage(TestPage, Buf, NO_ECC);

    memset(Buf, 0xFF, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    Chip->Ops.RdPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    //PrintBuf(Buf, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));
    // 2次
    TestPage++;
    Chip->Ops.WrPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    Buf[2] = 0;
    Chip->Ops.WrPage(TestPage, Buf, NO_ECC);

    memset(Buf, 0xFF, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    Chip->Ops.RdPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    //PrintBuf(Buf, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    // 3次
    TestPage++;
    Chip->Ops.WrPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    Buf[3] = 1;
    Chip->Ops.WrPage(TestPage, Buf, NO_ECC);

    memset(Buf, 0xFF, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    Chip->Ops.RdPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    //PrintBuf(Buf, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    //测试ECC功能,2位出错情况,是不能纠错了
    TestPage++;
    Chip->Ops.WrPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    Buf[3] = 0;
    Chip->Ops.WrPage(TestPage, Buf, NO_ECC | SPARE_REQ);

    memset(Buf, 0xFF, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    Chip->Ops.RdPage(TestPage, Buf, HW_ECC | SPARE_REQ);

    //PrintBuf(Buf, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));
    while(1);
}
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
    Buf[1] = 0xF0;

    Chip->Ops.WrPage(PageNo, Buf, NO_ECC);

    memset(Buf, 0, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));

    Ret = Chip->Ops.RdPage(PageNo, Buf, ECC_Flags | SPARE_REQ);
    for(i=0; i<(s_ptNandInfo->BytesPerPage); i++)
    {
        if(((u8)i) != Buf[i])
            while(1);
    }

}
void BadMarkFunctionCheck(struct FlashChip *Chip)
{
    u32 i;
    u32 Blks;

    if(NULL == Chip)
        while(1);

    Blks = Chip->Descr.Nand.BlksPerLUN * Chip->Descr.Nand.LUNs;

    for(i = 0; i < Blks; i++)
    {
        if(Chip->Ops.Special.Nand.MrkBad(i))
            while(1);
    }

    for(i = 0; i < Blks; i++)
    {
        if(-1 != Chip->Ops.Special.Nand.ChkBlk(i))
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
            /* 擦除失败 */
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
        Chip->Ops.ErsBlk(TestBlocks);

        for(TestPages = 0; TestPages < s_ptNandInfo->PagesPerBlk; TestPages++)
        {
            u32 CurPage = TestPages + (TestBlocks * s_ptNandInfo->PagesPerBlk);
            memset(Buf, 0xAA, (s_ptNandInfo->BytesPerPage + s_ptNandInfo->OOB_Size));
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


    BadMarkFunctionCheck(Chip);

    ContinuityTest(Chip);

    while(1);
}
