/*
 * MT29Fxxx.h
 *
 *  Created on: 2014-8-2
 *      Author: Administrator
 */

#ifndef MT29FXXX_H_
#define MT29FXXX_H_

#define cn_oob_size             64
#if 0
#define cn_reserve_blocks                      //定义一个保留区，紧跟MDR表
#else
#define cn_reserve_blocks       50//jizhaolin changed this for TEST, because of invalid blocks_
#endif

//下面定义各操作等待时间，单位微秒
#define cn_wait_address_ready   10
#define CN_WAIT_PAGE_WRITE      600
#define CN_WAIT_BLOCK_ERASE     3000
#define CN_WAIT_RESET           500

//ecc校验操作结果
#define cn_all_right_verify     0               //完全正确
#define cn_ecc_right_verify     1               //经ecc纠正正确
#define cn_ecc_error_verify     2               //错误，不能纠正
#define cn_other_error_verify   cn_limit_uint32 //其他错误，一般是非法参数

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

#define FSL_TWR_K60F120M

#ifdef FSL_TWR_K60F120M
#define ce_active()     (NFC_RAR |= (NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK))
#define ce_inactive()   (NFC_RAR &= ~(NFC_RAR_CS0_MASK | NFC_RAR_RB0_MASK))

#define __ResetNand()   (NFC_CMD2 = NFC_CMD2_BYTE1(RESET_CMD_BYTE) \
                                  | NFC_CMD2_CODE(NFC_RESET_CMD_CODE))

#define __ReadChipID()  (NFC_CMD2 = NFC_CMD2_BYTE1(READ_ID_CMD_BYTE)\
                                  | NFC_CMD2_CODE(NFC_READ_ID_CMD_CODE)\
                                  | NFC_CMD2_BUFNO(0))

#define __PagePrograme() (NFC_CMD2 = (NFC_CMD2_BYTE1(PROGRAM_PAGE_CMD_BYTE1)\
                                  | NFC_CMD2_CODE(NFC_PROGRAM_PAGE_CMD_CODE)\
                                  | NFC_CMD2_BUFNO(0));\
                          NFC_CMD1 = NFC_CMD1_BYTE2(PROGRAM_PAGE_CMD_BYTE2)\
                                  | NFC_CMD1_BYTE3(READ_STATUS_CMD_BYTE))

#define __PageRead()    (NFC_CMD2 = (NFC_CMD2_BYTE1(PAGE_READ_CMD_BYTE1)\
                                  | NFC_CMD2_CODE(NFC_READ_PAGE_CMD_CODE)\
                                  | NFC_CMD2_BUFNO(0) );\
                         NFC_CMD1 = NFC_CMD1_BYTE2(PAGE_READ_CMD_BYTE2)\
                                  | NFC_CMD1_BYTE3(READ_STATUS_CMD_BYTE))

#define __WaitReady()  (NFC_CMD2 & NFC_CMD2_BUSY_START_MASK)

#define __CmdStart()    (NFC_CMD2 |= NFC_CMD2_BUSY_START_MASK)
#define __CmdEnd()  (NFC_CMD2 & NFC_CMD2_BUSY_START_MASK)

#define BufferNo     0

#endif

#define CN_FS_BUF_TICKS (1000)      //文件自动写入时间,降低掉电损失
#define CN_FS_BUF_BLOCKS (16)       //文件缓冲块数,

// Flash commands:
#define cn_nand_page_read           0x00
#define cn_nand_startup_read        0x30
#define CN_NAND_RESET               0xFF
#define cn_nand_page_program        0x80
#define cn_nand_startup_write       0x10
#define cn_nand_block_erase         0x60
#define cn_nand_startup_erase       0xd0
#define cn_nand_read_status         0x70
#define cn_nand_read_id             0x90

#define cn_nand_failure             0x01
#define RB                          0x40



struct nand_table
{
    uint16_t vendor_chip_id;
    u16 oob_size;
    uint16_t pages_per_block;
    u32 blocks_sum;
    u32 block_size;
    char *chip_name;
};
void    __write_address_nand(uint32_t addr);
void    __read_sector_and_oob(u32 sector,u8 *data);
bool_t  write_PCRB_nand(u32 PCRB_block,u32 protected_block,u8 *buf);
bool_t  restore_PCRB_nand(u32 PCRB_block,u32 *restored);
bool_t  __wait_ready_nand(void);
bool_t  __wait_ready_nand_slow(uint16_t wait_time);
void    __write_command_nand(u8 val);
u32     __read_sector_nand_no_ecc(u32 sector,u32 offset,u8 *data,u32 size);
u32     read_block_ss_no_ecc(u32 block,u32 offset,u8 *buf,u32 size);
u32     __correct_sector(u8 *data,const u8 *old_ecc);
u32     __read_sector_nand_with_ecc(u32 sector,u32 offset,u8 *data,u32 size);
u32     read_block_ss_with_ecc(u32 block,u32 offset,u8 *buf,u32 size);
u8      __read_status_nand(void);
u32     __write_sector_nand_no_ecc(u32 sector,u32 offset,u8 *data,u32 size);
u32     write_block_ss_no_ecc(u32 block,u32 offset,u8 *buf,u32 size);
u32     __write_sector_nand_with_ecc(u32 sector,u32 offset,u8 *data,u32 size);
u32     write_block_ss_with_ecc(u32 block,u32 offset,u8 *buf,u32 size);
bool_t  erase_block_nand(u32 block_no);
bool_t  __erase_all_nand(void);
bool_t  query_block_ready_ss_with_ecc(u32 block,u32 offset,u8 *buf,u32 size);
bool_t  query_block_ready_nand_no_ecc(u32 block,u32 offset,u8 *buf,u32 size);
bool_t  query_ready_with_data_nand(u8 *new_data,u8 *org_data,u32 size);
bool_t  check_block_nand(u32 block_no);
bool_t  __mark_invalid_block(u32 block);
u32     __check_all_ss(void);
void    __make_sector_ecc(u8 *data,u8 *ecc);
u16      __read_chip_id (void);
bool_t  __parse_chip(uint16_t id,char **name);
void    __reset_nand(void);
ptu32_t ModuleInstall_FsNandFlash(const char *FlashHeapName);

#endif /* MT29FXXX_H_ */
