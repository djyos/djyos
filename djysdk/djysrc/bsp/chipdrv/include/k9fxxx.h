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
//所属模块: 文件系统芯片驱动
//作者：lst
//版本：V1.0.0
//文件描述: 用于三星nand flash的文件系统驱动模块
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2009-03-10
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 移植自yf44b0
//------------------------------------------------------
#ifndef __k9fxxx_h__
#define __k9fxxx_h__

#ifdef __cplusplus
extern "C" {
#endif

//#include "os_inc.h"
//nand flash芯片大小配置
#define cn_nand_little_block    0
#define cn_nand_big_block       1
#define cn_nand_block           cn_nand_big_block

#if (cn_nand_little_block == cn_nand_block)

#define cn_oob_size     16
#define cn_reserve_blocks   0                   //定义一个保留区，紧跟MDR表

//下面定义各操作等待时间，单位微秒
#define cn_wait_address_ready   10
#define cn_wait_page_write      200
#define cn_wait_block_erase     2000
#define cn_wait_reset           500

//ecc校验操作结果
#define cn_all_right_verify     0               //完全正确
#define cn_ecc_right_verify     1               //经ecc纠正正确
#define cn_ecc_error_verify     2               //错误，不能纠正
#define cn_other_error_verify   CN_LIMIT_UINT32 //其他错误，一般是非法参数


#define nand_ce_bit     (0x40)
#define nand_busy_bit   (0x20)
#define nand_cle_bit    (0x80)
#define nand_ale_bit    (0x100)

// HCLK=100Mhz
#define cn_talcs        1  //1clk(10ns)
#define cn_twrph0       4  //3clk(30ns)
#define cn_twrph1       0  //1clk(10ns)  //cn_talcs+cn_twrph0+cn_twrph1>=50ns


#define ce_active()     (pg_nand_reg->NFCONT &= ~(1<<1))
#define ce_inactive()   (pg_nand_reg->NFCONT |= (1<<1))

#define CN_FS_BUF_TICKS (1000)      //文件自动写入时间,降低掉电损失
#define CN_FS_BUF_BLOCKS (16)       //文件缓冲块数,

// Flash commands:
#define cn_nand_select_page0        0x00
#define cn_nand_select_page1        0x01
#define cn_nand_select_oob          0x50
#define cn_nand_reset               0xff
#define cn_nand_page_program        0x80
#define cn_nand_startup_write       0x10
#define cn_nand_block_erase         0x60
#define cn_nand_startup_erase       0xd0
#define cn_nand_read_status         0x70
#define cn_nand_read_id             0x90

#define cn_nand_failure             0x01
#define RB                          0x40
#endif

#if (cn_nand_big_block==1)

#define cn_oob_size     64
#define cn_reserve_blocks   0                   //定义一个保留区，紧跟MDR表

//下面定义各操作等待时间，单位微秒
#define cn_wait_address_ready   10
#define cn_wait_page_write      200
#define cn_wait_block_erase     2000
#define cn_wait_reset           500

//ecc校验操作结果
#define cn_all_right_verify     0               //完全正确
#define cn_ecc_right_verify     1               //经ecc纠正正确
#define cn_ecc_error_verify     2               //错误，不能纠正
#define cn_other_error_verify   CN_LIMIT_UINT32 //其他错误，一般是非法参数

#define nand_ce_bit     (0x40)
#define nand_busy_bit   (0x20)
#define nand_cle_bit    (0x80)
#define nand_ale_bit    (0x100)

// HCLK=100Mhz
#define cn_talcs        1  //1clk(10ns)
#define cn_twrph0       4  //3clk(30ns)
#define cn_twrph1       0  //1clk(10ns)  //cn_talcs+cn_twrph0+cn_twrph1>=50ns
// HCLK=50Mhz
//#define cn_talcs      0  //1clk(20ns)
//#define cn_twrph0     1  //2clk(40ns)
//#define cn_twrph1     0  //1clk(20ns)

#define ce_active()     (pg_nand_reg->NFCONT &= ~(1<<1))
#define ce_inactive()   (pg_nand_reg->NFCONT |= (1<<1))

#define CN_FS_BUF_TICKS (1000)      //文件自动写入时间,降低掉电损失
#define CN_FS_BUF_BLOCKS (16)       //文件缓冲块数,

// Flash commands:
#define cn_nand_page_read        0x00
#define cn_nand_startup_read         0x30
#define cn_nand_reset               0xff
#define cn_nand_page_program        0x80
#define cn_nand_startup_write       0x10
#define cn_nand_block_erase         0x60
#define cn_nand_startup_erase       0xd0
#define cn_nand_read_status         0x70
#define cn_nand_read_id             0x90

#define cn_nand_failure             0x01
#define RB                          0x40


#endif

struct nand_table
{
    uint16_t vendor_chip_id;
    u16 oob_size;
    uint16_t pages_per_block;
    u32 blocks_sum;
    u32 block_size;
    char *chip_name;
};
void __write_address_nand(uint32_t addr);
void __read_sector_and_oob(u32 sector,u8 *data);
bool_t write_PCRB_nand(u32 PCRB_block,
                       u32 protected_block,u8 *buf);
bool_t restore_PCRB_nand(u32 PCRB_block,u32 *restored);
bool_t __wait_ready_nand(void);
bool_t __wait_ready_nand_slow(uint16_t wait_time);
void __write_command_nand(u8 val);
u32 __read_sector_nand_no_ecc(u32 sector,u32 offset,
                                 u8 *data,u32 size);
u32 read_block_ss_no_ecc(u32 block,u32 offset,
                         u8 *buf,u32 size);
u32 __correct_sector(u8 *data,const u8 *old_ecc);
u32 __read_sector_nand_with_ecc(u32 sector,u32 offset,
                                 u8 *data,u32 size);
u32 read_block_ss_with_ecc(u32 block,u32 offset,
                         u8 *buf,u32 size);
u8 __read_status_nand(void);
u32 __write_sector_nand_no_ecc(u32 sector,u32 offset,
                                 u8 *data,u32 size);
u32 write_block_ss_no_ecc(u32 block,u32 offset,
                          u8 *buf,u32 size);
u32 __write_sector_nand_with_ecc(u32 sector,u32 offset,
                                 u8 *data,u32 size);
u32 write_block_ss_with_ecc(u32 block,u32 offset,
                          u8 *buf,u32 size);
bool_t erase_block_nand(u32 block_no);
bool_t __erase_all_nand(void);
bool_t query_block_ready_ss_with_ecc(u32 block,u32 offset,
                              u8 *buf,u32 size);
bool_t query_block_ready_nand_no_ecc(u32 block,u32 offset,
                              u8 *buf,u32 size);
bool_t query_ready_with_data_nand(u8 *new_data,u8 *org_data,
                                        u32 size);
bool_t check_block_nand(u32 block_no);
bool_t __mark_invalid_block(u32 block);
u32 __check_all_ss(void);
void __make_sector_ecc(const u8 *data,u8 *ecc);
uint16_t __read_chip_id (void);
bool_t __parse_chip(uint16_t id,char **name);
void __reset_nand(void);
ptu32_t ModuleInstall_FsNandFlash(ptu32_t para);

#ifdef __cplusplus
}
#endif

#endif // __k9fxxx_h__

