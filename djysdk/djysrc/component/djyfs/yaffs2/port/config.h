#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../yaffs2-583dbd9/yaffs2-583dbd9/yaffs_trace.h"

#ifndef CONFIG_YAFFS_DIRECT
#define CONFIG_YAFFS_DIRECT/* 使用DIRECT模式 */
#endif

#ifndef CONFIG_YAFFS_DEFINES_TYPES
#define CONFIG_YAFFS_DEFINES_TYPES
#endif

#ifndef CONFIG_YAFFS_PROVIDE_DEFS
#define CONFIG_YAFFS_PROVIDE_DEFS /* 使用yaffs提供的宏 */
/* 宏检查（已定义） */
#ifdef DT_UNKNOWN
#warning "DT_UNKNOWN" duplicate definition!
#endif
#ifdef DT_FIFO
#warning "DT_FIFO" duplicate definition!
#endif
#ifdef DT_CHR
#warning "DT_CHR" duplicate definition!
#endif
#ifdef DT_DIR
#warning "DT_DIR" duplicate definition!
#endif
#ifdef DT_BLK
#warning "DT_BLK" duplicate definition!
#endif
#ifdef DT_REG
#warning "DT_REG" duplicate definition!
#endif
#ifdef DT_LNK
#warning "DT_LNK" duplicate definition!
#endif
#ifdef DT_SOCK
#warning "DT_SOCK" duplicate definition!
#endif
#ifdef DT_WHT
#warning "DT_WHT" duplicate definition!
#endif
#ifdef ATTR_MODE
#warning "ATTR_MODE" duplicate definition!
#endif
#ifdef ATTR_UID
#warning "ATTR_UID" duplicate definition!
#endif
#ifdef ATTR_GID
#warning "ATTR_GID" duplicate definition!
#endif
#ifdef ATTR_SIZE
#warning "ATTR_SIZE" duplicate definition!
#endif
#ifdef ATTR_ATIME
#warning "ATTR_ATIME" duplicate definition!
#endif
#ifdef ATTR_MTIME
#warning "ATTR_MTIME" duplicate definition!
#endif
#ifdef ATTR_CTIME
#warning "ATTR_CTIME" duplicate definition!
#endif

#endif/* CONFIG_YAFFS_PROVIDE_DEFS */

#ifndef CONFIG_YAFFSFS_PROVIDE_VALUES
#define CONFIG_YAFFSFS_PROVIDE_VALUES
/* 宏检查(是否已定义) */
#ifdef O_RDONLY
//#warning "O_RDONLY" duplicate definition!//iofile.h中已定义
#endif
#ifdef O_WRONLY
//#warning "O_WRONLY" duplicate definition!//iofile.h中已定义
#endif
#ifdef O_RDWR
//#warning "O_RDWR" duplicate definition! // iofile.h中已定义
#endif
#ifdef O_CREAT
#warning "O_CREAT" duplicate definition!
#endif
#ifdef O_EXCL
//#warning "O_EXCL" duplicate definition! // iofile.h中已定义
#endif
#ifdef O_TRUNC
//#warning "O_TRUNC" duplicate definition! // iofile.h中已定义
#endif
#ifdef O_APPEND
//#warning "O_APPEND" duplicate definition! // iofile.h中已定义
#endif
#ifdef SEEK_SET
//#warning "SEEK_SET" duplicate definition! // stdio.h中已声明,定义一样
#endif
#ifdef SEEK_CUR
//#warning "SEEK_CUR" duplicate definition! // stdio.h中已声明,定义一样
#endif
#ifdef SEEK_END
//#warning "SEEK_END" duplicate definition! // stdio.h中已声明,定义一样
#endif
#ifdef EBUSY
#warning "EBUSY" duplicate definition!
#endif
#ifdef ENODEV
#warning "ENODEV" duplicate definition!
#endif
#ifdef EINVAL
#warning "EINVAL" duplicate definition!
#endif
#ifdef ENFILE
#warning "ENFILE" duplicate definition!
#endif
#ifdef EBADF
#warning "EBADF" duplicate definition!
#endif
#ifdef EACCES
#warning "EACCES" duplicate definition!
#endif
#ifdef EXDEV
#warning "EXDEV" duplicate definition!
#endif
#ifdef ENOENT
#warning "ENOENT" duplicate definition!
#endif
#ifdef ENOSPC
#warning "ENOSPC" duplicate definition!
#endif
#ifdef EROFS
#warning "EROFS" duplicate definition!
#endif
#ifdef ERANGE
#warning "ERANGE" duplicate definition!
#endif
#ifdef ENODATA
#warning "ENODATA" duplicate definition!
#endif
#ifdef ENOTEMPTY
#warning "ENOTEMPTY" duplicate definition!
#endif
#ifdef ENAMETOOLONG
#warning "ENAMETOOLONG" duplicate definition!
#endif
#ifdef ENOMEM
#warning "ENOMEM" duplicate definition!
#endif
#ifdef EFAULT
#warning "EFAULT" duplicate definition!
#endif
#ifdef EEXIST
#warning "EEXIST" duplicate definition!
#endif
#ifdef ENOTDIR
#warning "ENOTDIR" duplicate definition!
#endif
#ifdef EISDIR
#warning "EISDIR" duplicate definition!
#endif
#ifdef ELOOP
#warning "ELOOP" duplicate definition!
#endif
#ifdef S_IFMT
//#warning "S_IFMT" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_IFSOCK
//#warning "S_IFSOCK" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_IFIFO
//#warning "S_IFIFO" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_IFCHR
//#warning "S_IFCHR" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_IFBLK
//#warning "S_IFBLK" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_IFLNK
#warning "S_IFLNK" duplicate definition!
#endif
#ifdef S_IFDIR
//#warning "S_IFDIR" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_IFREG
//#warning "S_IFREG" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_ISSOCK
//#warning "S_ISSOCK" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_ISLNK
//#warning "S_ISLNK" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_ISDIR
//#warning "S_ISDIR" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_ISREG
//#warning "S_ISREG" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_ISBLK
//#warning "S_ISBLK" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_ISCHR
//#warning "S_ISCHR" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_ISFIFO
//#warning "S_ISFIFO" duplicate definition! // iofile.h中已定义
#endif
#ifdef S_IREAD
#warning "S_IREAD" duplicate definition!
#endif
#ifdef S_IWRITE
#warning "S_IWRITE" duplicate definition!
#endif
#ifdef S_IEXEC
#warning "S_IEXEC" duplicate definition!
#endif
#ifdef XATTR_CREATE
#warning "XATTR_CREATE" duplicate definition!
#endif
#ifdef XATTR_REPLACE
#warning "XATTR_REPLACE" duplicate definition!
#endif
#ifdef R_OK
#warning "R_OK" duplicate definition!
#endif
#ifdef W_OK
#warning "W_OK" duplicate definition!
#endif
#ifdef X_OK
#warning "X_OK" duplicate definition!
#endif
#ifdef F_OK
#warning "F_OK" duplicate definition!
#endif

#endif/* CONFIG_YAFFSFS_PROVIDE_VALUES */

#define loff_t s64/* 类型待确定 */

//#define dev_t u32/* linux中用于定义设备编号，高12位为主设备号，低20位为次设备号。 */

#ifdef __cplusplus
}
#endif

#endif/* __CONFIG_H__ */
