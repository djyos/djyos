;/*==========================================================*
* 文件模块说明:                                            *
* Nor Flash  Spansion S29GL128 芯片 驱动程序
* 文件版本: v1.00                                          *
* 开发人员:                                      *
* 创建时间: 2011-10-9                              *
* Copyright(c) 2010-2011 Shenzhen NARI Limited Co.        *
***************==========================================================*
* 程序修改记录(最新的放在最前面):                          *
* <版本号><修改日期>, <修改人员>: <修改功能概述>           *
*==========================================================*/

#ifndef  __NORS29GL128_H_
#define  __NORS29GL128_H_

#ifdef __cplusplus
extern "C" {
#endif



#define SA_OFFSET_MASK	0xFFFFF000	// mask off the offset

#define LLD_DB_READ_MASK   0x0000FFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA

// Status bit definition for status register
#define DEV_RDY_MASK			(0x80)	// Device Ready Bit
#define DEV_ERASE_SUSP_MASK		(0x40)	// Erase Suspend Bit
#define DEV_ERASE_MASK			(0x20)	// Erase Status Bit
#define DEV_PROGRAM_MASK		(0x10)	// Program Status Bit
#define DEV_RFU_MASK			(0x08)	// Reserved
#define DEV_PROGRAM_SUSP_MASK	(0x04)	// Program Suspend Bit
#define DEV_SEC_LOCK_MASK		(0x02)	// Sector lock Bit
#define DEV_BANK_MASK			(0x01)	// Operation in current bank
#define NOR_STATUS_REG_READ_CMD			 ((0x70))
#define NOR_STATUS_REG_CLEAR_CMD		 ((0x71))

/* LLD Command Definition */
#define NOR_AUTOSELECT_CMD               ((0x90))
#define NOR_CFI_QUERY_CMD                ((0x98))
#define NOR_CHIP_ERASE_CMD               ((0x10))
#define NOR_ERASE_SETUP_CMD              ((0x80))
#define NOR_PROGRAM_CMD                  ((0xA0))
#define NOR_RESET_CMD                    ((0xF0))
#define NOR_SECSI_SECTOR_ENTRY_CMD       ((0x88))
#define NOR_SECSI_SECTOR_EXIT_SETUP_CMD  ((0x90))
#define NOR_SECSI_SECTOR_EXIT_CMD        ((0x00))
#define NOR_SECTOR_ERASE_CMD             ((0x30))
#define NOR_UNLOCK_BYPASS_ENTRY_CMD      ((0x20))
#define NOR_UNLOCK_BYPASS_PROGRAM_CMD    ((0xA0))
#define NOR_UNLOCK_BYPASS_RESET_CMD1     ((0x90))
#define NOR_UNLOCK_BYPASS_RESET_CMD2     ((0x00))
#define NOR_UNLOCK_DATA1                 ((0xAA))
#define NOR_UNLOCK_DATA2                 ((0x55))
#define NOR_WRITE_BUFFER_ABORT_RESET_CMD ((0xF0))
#define NOR_WRITE_BUFFER_LOAD_CMD        ((0x25))
#define NOR_WRITE_BUFFER_PGM_CONFIRM_CMD ((0x29))
#define NOR_SUSPEND_CMD                  ((0xB0))
#define NOR_RESUME_CMD                   ((0x30))
#define NOR_SET_CONFIG_CMD			     ((0xD0))
#define NOR_READ_CONFIG_CMD			     ((0xC6))


typedef enum {
 DEV_STATUS_UNKNOWN = 0,
 DEV_NOT_BUSY,
 DEV_BUSY,
 DEV_EXCEEDED_TIME_LIMITS,
 DEV_SUSPEND,
 DEV_WRITE_BUFFER_ABORT,
 DEV_STATUS_GET_PROBLEM,
 DEV_VERIFY_ERROR,
 DEV_BYTES_PER_OP_WRONG,
 DEV_ERASE_ERROR,
 DEV_PROGRAM_ERROR,
 DEV_SECTOR_LOCK

} DEVSTATUS;


typedef enum
{
LLD_P_POLL_NONE = 0,			/* pull program status */
LLD_P_POLL_PGM,				    /* pull program status */
LLD_P_POLL_WRT_BUF_PGM,			/* Poll write buffer   */
LLD_P_POLL_SEC_ERS,			    /* Poll sector erase   */
LLD_P_POLL_CHIP_ERS,			/* Poll chip erase     */
LLD_P_POLL_RESUME,
LLD_P_POLL_BLANK			    /* Poll device sector blank check */

}POLLING_TYPE;


#define FLASH_OFFSET(b,o)   (*(( (volatile u16*)(b) ) + (o)))
#define FLASH_WR(b,o,d)     FLASH_OFFSET((b),(o)) = (d)
#define FLASH_RD(b,o)       FLASH_OFFSET((b),(o))

void s29gl_Init(u32   baseAddr);
void s29gl_ResetCmd(u32   baseAddr);
u32 s29gl_GetDeviceId(u32   baseAddr);
int s29gl_WriteData(u32 baseAddr,u32  offset,
                    u8 *dataBuf,u32 len);
DEVSTATUS s29gl_SectorErase(u32   baseAddr,u32 offset);
void  s29gl_ReadData(u32   baseAddr,u32 offset ,u8* data,u32 length);
#endif
