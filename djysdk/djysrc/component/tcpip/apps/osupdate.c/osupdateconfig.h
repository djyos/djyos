/*
 * osconfig.h
 *
 *  Created on: 2015Äê12ÔÂ23ÈÕ
 *      Author: zqf
 */

#ifndef __OSUPDATECONFIG_H
#define __OSUPDATECONFIG_H

//each project has one config here, if not right, please change it!
#define CN_OSFILE_NAME	      "SR1210.bin"
#define CN_OSFLASH_BASEADDR   0                //flash space base address
#define CN_OSSPACE_OFFSET     0x100000         //the os start off the base address
#define CN_OSSPACE_SIZE       0x100000         //the space we need to store the os file

#define CN_OS_VERSION         "DJYOS:20151224:TFTP"


#endif /* __OSUPDATECONFIG_H */
