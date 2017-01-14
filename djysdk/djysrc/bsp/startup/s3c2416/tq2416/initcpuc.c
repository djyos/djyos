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
//所属模块:  CPU初始化
//作者：     Liuwei
//版本：      V1.0.0
//初始版本完成日期：2015-05-22
//文件描述:  S3C2416 CPU相关初始化
//修订历史:
//-----------------------------------------------------------------------------

#include "arch_feature.h"
#include "stdint.h"
#include <stdio.h>
#include "cpu_peri.h"
#include "hard-exp.h"
#include "sysctl.h"
#include "initcpu.h"
#include "mmu.h"

int IsAddrInDRAM(u32 addr)
{
    if(addr<0x30000000)
    {
        return 0;
    }

    if(addr>0x3FFFFFFF)
    {
        return 0;
    }

    return 1;
}

void CPU_ClkInit(void)
{/*
    register int i;
    register struct SYSCON_REG *sys =SYSCON_REG_BASE;

    sys->LOCKCON0 = 0x7FFF;

    sys->MPLLCON = (0<<24) | MPLL_400MHZ;

    sys->CLKDIV0 = 0;
    sys->CLKDIV0 = (0<<9)   //ARMDIV =1/1
                | (0<<6)    //EXTDEV =1/1
                | (1<<4)    //PREDIV =1/2
                | (1<<3)    //HALFHCLK =HCLK/2
                | (1<<2)    //PCLK =HCLK/2
                | (1<<0);   //HCLKDIV =1/2



    for(i=0;i<1000;i++);
*/
}

#if 0
void MEM_BusInit(void)
{
    struct GPIO_REG  *gpio =GPIO_REG_BASE;
    struct DMC_REG   *dmc =DMC_REG_BASE;
    volatile u32 i;

    gpio->GPKCON    =0xAAAAAAAA;
    dmc->BANKCFG    =BANKCFGVAL;
    dmc->BANKCON1   =BANKCON1VAL;
    dmc->BANKCON2   =BANKCON2VAL;

    ////Issue a PALL command
    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x1<<0);
    dmc->BANKCON1 =i;

    ////Issue a EMRS2 command
    i  = dmc->BANKCON3;
    i &= ~0xFFFF0000;
    i |=  (BA_EMRS2<<30);
    dmc->BANKCON3 =i;

    ////
    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x3<<0);
    dmc->BANKCON1 =i;

    ////Issue a EMRS3 command
    i  = dmc->BANKCON3;
    i &= ~0xFFFF0000;
    i |=  (BA_EMRS3<<30);
    dmc->BANKCON3 =i;

    ////
    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x3<<0);
    dmc->BANKCON1 =i;

    ////Issue a EMRS1 command
    i  = dmc->BANKCON3;
    i &= ~0xFFFF0000;
    i |=  ((BA_EMRS1<<30)+(RDQS_DIS<<27)+(nDQS_DIS<<26)+(OCD_MODE_EXIT<<23)+(DLL_EN<<16));
    dmc->BANKCON3 =i;

    ////
    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x3<<0);
    dmc->BANKCON1 =i;

    ////Issue a MRS command
    i  = dmc->BANKCON3;
    i &= ~0x0000FFFF;
    i |=  ((BA_MRS<<14)+(DLL_RESET_HIGH<<8)+(TM<<7)+(CL_MRS<<4));
    dmc->BANKCON3 =i;

    ////
    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x2<<0);
    dmc->BANKCON1 =i;

    ////Issue a PALL command
    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x1<<0);
    dmc->BANKCON1 =i;

    ////wait auto-clk
    dmc->REFRESH =0x20;

    ////Issue a MRS command
    i  = dmc->BANKCON3;
    i &= ~0x0000FFFF;
    i |=  ((BA_MRS<<14)+(DLL_RESET_HIGH<<8)+(TM<<7)+(CL_MRS<<4));
    dmc->BANKCON3 =i;

    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x2<<0);
    dmc->BANKCON1 =i;

    ////Wait 200 clock
    for(i=0;i<200;i++);

    ////Issue a EMRS1 command For OCD Mode Set to default
    i  = dmc->BANKCON3;
    i &= ~0xFFFF0000;
    i |=  ((BA_EMRS1<<30)+(RDQS_DIS<<27)+(nDQS_DIS<<26)+(OCD_MODE_DEFAULT<<23)+(DLL_EN<<16));
    dmc->BANKCON3 =i;

    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x3<<0);
    dmc->BANKCON1 =i;

    i  = dmc->BANKCON3;
    i &= ~0xFFFF0000;
    i |=  ((BA_EMRS1<<30)+(RDQS_DIS<<27)+(nDQS_DIS<<26)+(OCD_MODE_DEFAULT<<23)+(DLL_EN<<16));
    dmc->BANKCON3 =i;

    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    i |=  (0x3<<0);
    dmc->BANKCON1 =i;

    ////Refresh  Normal
    dmc->REFRESH =REFCYC;

    ////Issue a Normal mode
    i  = dmc->BANKCON1;
    i &= ~(0x3<<0);
    dmc->BANKCON1 =i;

    for(i=0;i<100;i++);

}
#endif

#define READCMD0    0x00
#define READCMD1    0x30
#define READCMD2    0x50

#define ERASECMD0   0x60
#define ERASECMD1   0xd0

#define PROGCMD0    0x80
#define PROGCMD1    0x10

#define copy_backCMD0   0x00
#define copy_backCMD1   0x8a
#define copy_backCMD2   0x10

#define RDSTATUS0CMD    0x70
#define RdIDCMD         0x90

#define EnNandFlash()   (nfc->NFCONT |=  1)
#define DsNandFlash()   (nfc->NFCONT &= ~1)
#define NFChipEn()      (nfc->NFCONT &= ~(1<<1))
#define NFChipDs()      (nfc->NFCONT |=  (1<<1))
#define InitEcc()       (nfc->NFCONT |= (1<<4))
#define MEccUnlock()    (nfc->NFCONT &= ~(1<<5))
#define MEccLock()      (nfc->NFCONT |= (1<<5))
#define SEccUnlock()    (nfc->NFCONT &= ~(1<<6))
#define SEccLock()      (nfc->NFCONT |= (1<<6))

#define WNFDat8(dat)    (nfc->NFDATA = (dat))
#define WNFDat32(dat)   (nfc->NFDATA = (dat))
#define RdNFDat8()      (nfc->NFDATA)   //byte access
#define RdNFDat32()     (nfc->NFDATA)   //word access

#define WNFCmd(cmd)     (nfc->NFCMD = (cmd))
#define WNFAddr(addr)   (nfc->NFADDR = (addr))
#define WNFDat(dat)     WNFDat8(dat)
#define RdNFDat()       RdNFDat8()      //for 8 bit nand flash, use byte access

#define RdNFMEcc()      (nfc->NFMECC0)  //for 8 bit nand flash, only use NFMECC0
#define RdNFSEcc()      (nfc->NFSECC)   //for 8 bit nand flash, only use low 16 bits

#define RdNFStat()      (nfc->NFSTAT)
#define NFIsBusy()      (!(nfc->NFSTAT&1))
#define NFIsReady()     (nfc->NFSTAT&1)

static u32 nf_wait_busy(void)
{
    struct NFC_REG *nfc =NFC_REG_BASE;
    u8 stat;
    ////

    WNFCmd(RDSTATUS0CMD);
    do
    {
        stat = RdNFDat();
    }while(!(stat&(1<<6)));
    WNFCmd(READCMD0);
    return stat&1;
}

static u32 nf_read_chip_id(void)
{
    struct NFC_REG *nfc =NFC_REG_BASE;
    u32 id;

    NFChipEn();
    WNFCmd(RdIDCMD);
    WNFAddr(0);
    while(NFIsBusy());
    id  = RdNFDat()<<8;
    id |= RdNFDat();
    NFChipDs();
    return id;
}

static void nf_read_page(u32 addr, char *buf)
{
    int i;
    struct NFC_REG *nfc =NFC_REG_BASE;

    NFChipEn();
    WNFCmd(READCMD0);   // 1'st command

    WNFAddr(0);
    WNFAddr(0);
    WNFAddr((addr)&0xff);
    WNFAddr((addr>>8)&0xff);
#if 1
    WNFAddr((addr>>16)&0xff);
#endif

    WNFCmd(READCMD1);   // 2'nd command
    nf_wait_busy();
    for(i=0; i<2048; i++)
    {
        buf[i] = RdNFDat();
    }
    NFChipDs();
}

static u32 nf_write_page(u32 addr, char *buf)
{
    struct NFC_REG *nfc =NFC_REG_BASE;
    volatile int i;
    int mecc;
    u8 stat, tmp[7];

    NFChipEn();
    WNFCmd(PROGCMD0);
    WNFAddr(0);
    WNFAddr(0);
    WNFAddr((addr)&0xff);
    WNFAddr((addr>>8)&0xff);
#if 1
    WNFAddr((addr>>16)&0xff);
#endif

    for(i=0; i<2048; i++)
    {
        WNFDat(buf[i]);
    }
    mecc = 0;

    tmp[0] = mecc&0xff;
    tmp[1] = (mecc>>8)&0xff;
    tmp[2] = (mecc>>16)&0xff;
    tmp[3] = (mecc>>24)&0xff;
    tmp[5] = 0xff;  //mark good block

    WNFDat(tmp[0]);
    WNFDat(tmp[1]);
    WNFDat(tmp[2]);
    WNFDat(tmp[3]);

    WNFDat(tmp[4]);
    WNFDat(tmp[5]);

    WNFCmd(PROGCMD1);
    stat = nf_wait_busy();
    NFChipDs();

    return stat;
}

static u32 nf_erase_block(u32 addr)
{
    struct NFC_REG *nfc =NFC_REG_BASE;
    u8 stat;

    NFChipEn();
    WNFCmd(ERASECMD0);

    WNFAddr((addr)&0xff);
    WNFAddr((addr>>8)&0xff);
    WNFAddr((addr>>16)&0xff);

    WNFCmd(ERASECMD1);
    stat = nf_wait_busy();
    NFChipDs();

    return stat;
}

void    NandBoot_CopyToRAM(u32 sector,char *dst,int size)
{
    struct NFC_REG *nfc =NFC_REG_BASE;
    volatile int i;

    nfc->NFCONF =(1<<12)|(3<<8)|(1<<4)|(1<<3)|(0<<2)|(1<<1)|(0<<0);
    nfc->NFCONT =(0<<13)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(1<<6)|(1<<5)|(1<<4)|(1<<1)|(1<<0);

    i=nf_read_chip_id();

    i=0;
    while(i<size)//todo-----size从lds文件中获取比较好
    {
        nf_read_page(sector,&dst[i]);
        i+=2048;
        sector++;
    }

}

void    NandBoot_Prog(u32 sector,char *src,int size)
{
    struct NFC_REG *nfc =NFC_REG_BASE;
    volatile int i;

    nfc->NFCONF =(1<<12)|(3<<8)|(1<<4)|(1<<3)|(0<<2)|(1<<1)|(0<<0);
    nfc->NFCONT =(0<<13)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(1<<6)|(1<<5)|(1<<4)|(1<<1)|(1<<0);

    i=nf_read_chip_id();

    i=0;
    while(i<size)//todo-----size从lds文件中获取比较好
    {
        if((sector&0x3F)==0)
        {
            nf_erase_block(sector);
        }
        nf_write_page(sector,&src[i]);
        i+=2048;
        sector++;
    }

}
/*
void    NandTest(void)
{
    struct NFC_REG *nfc =NFC_REG_BASE;
    volatile int i,page;
    char *src;
    char *dst;


    nfc->NFCONF =(7<<12)|(7<<8)|(7<<4)|(1<<3)|(0<<2)|(1<<1)|(0<<0);
    nfc->NFCONT =(0<<13)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(1<<6)|(1<<5)|(1<<4)|(1<<1)|(1<<0);

    src =(char*)0x30000000;
    dst =(char*)0x32000000;
    i=nf_read_chip_id();

    i=0;
    page=0;
    while(1)
    {


        nf_erase_block(page);
        i++;

        nf_write_page(page,src);
        i++;

        nf_read_page(page,dst);
        i++;

    }


}
*/

// ---- 初始化MMU ----
void MMU_Init(void)
{

    MMU_MapSection(mmu_page_table,0x00000000,0x00000000,4096,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB));

    MMU_MapSection(mmu_page_table,0x08000000,0x08000000,4,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //SMC BANK1

    MMU_MapSection(mmu_page_table,0x20000000,0x20000000,4,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //SMC BANK4

    MMU_MapSection(mmu_page_table,0x30000000,0x30000000,8,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CB));   //ROM1

    MMU_MapSection(mmu_page_table,0x30800000,0x30800000,8,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //RAM_nocache

    MMU_MapSection(mmu_page_table,0x31000000,0x31000000,16,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CB));   //RAM1

    MMU_MapSection(mmu_page_table,0x32000000,0x32000000,16,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CNB));  //RAM2

    MMU_MapSection(mmu_page_table,0x48000000,0x48000000,384,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,NCNB)); //SFR

    MMU_MapSection(mmu_page_table,0xFFF00000,0x33F00000,1,
            SECTION_ATTR(AP_USER_RW,DOMAIN_NO_CHECK,CB));   //Last 1MB


    MMU_SetSectionsBase((u32*)mmu_page_table);
    MMU_SetDomainAccess(0xffffffff);          //所有域具有管理者权限

    //Cache_EnableAll();
    //Cache_EnableWriteBuf();
    // MMU_Enable();

}




