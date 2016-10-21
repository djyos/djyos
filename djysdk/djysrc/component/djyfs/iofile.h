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

#ifndef __IOFILE_H__
#define __IOFILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "vfile.h"


//
// 八进制表示(来自于linux)
//
#ifndef O_ACCMODE
 #define O_ACCMODE					(00000003)/* 模式屏蔽位 */
#else
 #warning "O_ACCMODE" duplicate definition!
#endif
#ifndef O_RDONLY
 #define O_RDONLY					(00000000)
#else
 #warning "O_RDONLY" duplicate definition!
#endif
#ifndef O_WRONLY
 #define O_WRONLY					(00000001)
#else
 #warning "O_WRONLY" duplicate definition!
#endif
#ifndef O_RDWR
 #define O_RDWR						(00000002)
#else
 #warning "O_RDWR" duplicate definition!
#endif
#ifndef O_CREAT
 #define O_CREAT					(00000100)
#else
 #warning "O_CREATE" duplicate definition!
#endif
#ifndef O_EXCL
 #define O_EXCL                 	(00000200)
#else
 #warning "O_EXCL" duplicate definition!
#endif
#ifndef O_NOCTTY
 #define O_NOCTTY               	(00000400)
#else
 #warning "O_NOCTTY" duplicate definition!
#endif
#ifndef O_TRUNC
 #define O_TRUNC	                (00001000)
#else
 #warning "O_TRUNC" duplicate definition!
#endif
#ifndef O_APPEND
 #define O_APPEND                	(00002000)
#else
 #warning "O_APPEND" duplicate definition!
#endif
#ifndef O_NONBLOCK
 #define O_NONBLOCK             	(00004000)
#else
 #warning "O_NONBLOCK" duplicate definition!
#endif
#ifndef O_DSYNC
 #define O_DSYNC         			(00010000)
#else
 #warning "O_DSYNC" duplicate definition!
#endif
#ifndef O_DIRECT
 #define O_DIRECT        			(00040000)
#else
 #warning "O_DIRECT" duplicate definition!
#endif
#ifndef O_LARGEFILE
 #define O_LARGEFILE     			(00100000)
#else
 #warning "O_LARGEFILE" duplicate definition!
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY     			(00200000)
#else
 #warning "O_DIRECTORY" duplicate definition!
#endif
#ifndef O_NOFOLLOW
 #define O_NOFOLLOW      			(00400000)
#else
 #warning "O_NOFOLLOW" duplicate definition!
#endif
#ifndef O_NOATIME
#define O_NOATIME       			(01000000)
#else
 #warning "O_NOATIME" duplicate definition!
#endif
#ifndef O_CLOEXEC
 #define O_CLOEXEC       			(02000000)
#else
 #warning "O_CLOEXEC" duplicate definition!
#endif
#ifndef S_IFMT
 #define S_IFMT                  	(00170000)// 1111-0000-0000-0000 文件类型屏蔽位
#else
 #warning "S_IFMT" duplicate definition!
#endif
#ifndef S_IFSOCK
 #define S_IFSOCK					(00140000)// 1100-0000-0000-0000
#else
 #warning "S_IFSOCK" duplicate definition!
#endif
#ifndef S_IFLNK
 #define S_IFLNK					(00120000)// 1010-0000-0000-0000
#else
 #warning "S_IFlINK" duplicate definition!
#endif
#ifndef S_IFREG
 #define S_IFREG                 	(00100000)// 1000-0000-0000-0000
#else
 #warning "S_IFREG" duplicate definition!
#endif
#ifndef S_IFBLK
 #define S_IFBLK                 	(00060000)// 0110-0000-0000-0000
#else
 #warning "S_IFBLK" duplicate definition!
#endif
#ifndef S_IFDIR
 #define S_IFDIR                 	(00040000)// 0100-0000-0000-0000
#else
 #warning "S_IFDIR" duplicate definition!
#endif
#ifndef S_IFCHR
 #define S_IFCHR                 	(00020000)// 0010-0000-0000-0000
#else
 #warning "S_IFCHR" duplicate definition!
#endif
#ifndef S_IFIFO
 #define S_IFIFO                 	(00010000)// 0001-0000-0000-0000
#else
 #warning "S_IFIFO" duplicate definition!
#endif


//方便第三方移植，内部逻辑未实现
#define   S_ISUID    0004000   //set-user-ID bit
#define   S_ISGID    0002000   //set-group-ID bit (see below)
#define   S_ISVTX    0001000   //sticky bit (see below)
#define   S_IRWXU    00700     //mask for file owner permissions
#define   S_IRUSR    00400     //owner has read permission
#define   S_IWUSR    00200     //owner has write permission
#define   S_IXUSR    00100     //owner has execute permission
#define   S_IRWXG    00070     //mask for group permissions
#define   S_IRGRP    00040     //group has read permission

#define   S_IWGRP    00020     //group has write permission
#define   S_IXGRP    00010     //group has execute permission
#define   S_IRWXO    00007     //mask for permissions for others (not in group)
#define   S_IROTH    00004     //others have read permission
#define   S_IWOTH    00002     //others have write permission
#define   S_IXOTH    00001     //others have execute permission

#define S_ISREG(x)      ((x&S_IFMT)==S_IFREG?true:false) //is it a regular file
#define S_ISDIR(x)      ((x&S_IFMT)==S_IFDIR?true:false) //directory?
#define S_ISCHR(x)      ((x&S_IFMT)==S_IFCHR?true:false) //character device?
#define S_ISBLK(x)      ((x&S_IFMT)==S_IFBLK?true:false) //block device?
#define S_ISFIFO(x)     ((x&S_IFMT)==S_IFIFO?true:false) //FIFO (named pipe)?
#define S_ISLNK(x)      ((x&S_IFMT)==S_IFLNK?true:false) //symbolic link?  (Not in POSIX.1-1996.)
#define S_ISSOCK(x)      ((x&S_IFMT)==S_IFSOCK?true:false) //socket?  (Not in POSIX.1-1996.)


#ifndef NAME_MAX
#define NAME_MAX  64
#endif
//
// todo
//
struct dirent
{
   long d_ino; /* inode number 索引节点号 */
   off_t d_off; /* offset to this dirent 在目录文件中的偏移 */
   unsigned short d_reclen; /* length of this d_name 文件名长 */
   unsigned char d_type; /* the type of d_name 文件类型 */
   char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长255字符 */
};

//
// todo
//
struct __dirstream
{
	void *__fd; /* `struct hurd_fd' pointer for descriptor.   */
	char *__data; /* Directory block.   */
	int __entry_data; /* Entry number `__data' corresponds to.   */
	char *__ptr; /* Current pointer into the block.   */
	int __entry_ptr; /* Entry number `__ptr' corresponds to.   */
	size_t __allocation; /* Space allocated for the block.   */
	size_t __size; /* Total valid data in the block.   */
};

typedef struct __dirstream DIR;



//
//
//
struct stat
{
    //added by zqf for the comportable for the free software--todo
    mode_t     st_mode;       //文件对应的模式，文件，目录等

    ino_t      st_ino;       //inode节点号

    dev_t      st_dev;        //设备号码

    dev_t      st_rdev;       //特殊设备号码

    nlink_t    st_nlink;      //文件的连接数

    uid_t      st_uid;        //文件所有者

    gid_t      st_gid;        //文件所有者对应的组

    off_t      st_size;       //普通文件，对应的文件字节数

    blksize_t st_blksize;    //文件内容对应的块大小

    blkcnt_t   st_blocks;     //伟建内容对应的块数量

    time_t     st_atime;      //文件最后被访问的时间

    time_t     st_mtime;      //文件内容最后被修改的时间

    time_t     st_ctime;      //文件状态改变时间
};



//
//  其他函数
//
int VFileToHandle(struct VFile *Fp);
struct VFile *HandleToVFile(int Fd);

//
// 已实现的函数申明
//
//int open(const char *pathname, int oflag, unsigned int mode);
int open(const char *pathname, int fmt, ...);
int close(int fd);
int read(int fd, void *buf, size_t count);
int write(int fd, const void *buf, size_t count);
int fstat(int fd, struct stat *buf);
int stat(const char *filename, struct stat *buf);
int ftruncate(int fd, off_t length);
DIR *opendir(const char *name); // todo
struct dirent *readdir(DIR *dir); // todo
int closedir(DIR *dir); // todo
int mkdir (const char *filename, mode_t mode); // todo

#ifdef __cplusplus
}
#endif

#endif/* __IOFILE_H__ */
