// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 模块描述:sys_space_config.h/sharc21469
// 模块版本: V1.0.0
// 创建人员: liq
// 创建时间: 3:11:11 PM/Mar 28, 2014
// =============================================================================
// 程序修改记录(最新的放在最前面):
// <版本号> <修改日期>, <修改人员>: <修改功能概述>
// =============================================================================
//maybe this is the most important files,which defines the whole sys space
//

#ifndef SYS_SPACE_CONFIG_H_
#define SYS_SPACE_CONFIG_H_

//----------------------------------------------------------------------------//
#ifdef MB
#undef MB
#endif
#define MB                      0x100000

//we has devided the ddr into several blocks as we need


//we has divided the norflash into several blocks as we need
//nor_flash START(0x04000000) END(0x05FFFFFF)      //32M-8bit
#define cn_sys_nor_flash_base              0x04000000


//defines for the text code in the norflash

//reserved 1MB for the exception record




//define for the system to run



#endif /* SYS_SPACE_CONFIG_H_ */
