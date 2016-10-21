/*
 * types.h
 *
 *  Created on: 2015Äê4ÔÂ25ÈÕ
 *      Author: zhangqf
 */

#ifndef __TYPES_h
#define __TYPES_h
#ifdef	__cplusplus
extern "C"{
#endif


typedef unsigned int in_addr_t;
#ifndef	__cplusplus
typedef bool_t bool;
#endif

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK  0x7f000001     //127.0.0.1
#endif


typedef int pid_t;
typedef int __uid_t;
typedef int __gid_t;
typedef int           ino_t;
typedef unsigned int  dev_t;
typedef int           nlink_t;
typedef int           uid_t;
typedef int           gid_t;
typedef int           blksize_t;
typedef int           blkcnt_t;


#define CN_SELECT_MAXNUM      20
#define CN_SELECT_TIMEDEFAULT 10
typedef struct
{
	int  mode;
	u32  fd[CN_SELECT_MAXNUM];
}_types_fd_set;

#define fd_set _types_fd_set


#define ARG_MAX 256

#ifdef	__cplusplus
}
#endif
#endif /* __TYPES_h */
