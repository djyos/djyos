/*
 * types.h
 *
 *  Created on: 2015Äê4ÔÂ25ÈÕ
 *      Author: zhangqf
 */

#ifndef __TYPES_h
#define __TYPES_h
#ifdef  __cplusplus
extern "C"{
#endif


#define _CLOCK_T_   unsigned long       /* clock() */
#define _TIME_T_    long            /* time() */
#define _CLOCKID_T_     unsigned long
#define _TIMER_T_       unsigned long

#ifndef _HAVE_SYSTYPES
typedef long int __off_t;
typedef int __pid_t;
#ifdef __GNUC__
__extension__ typedef long long int __loff_t;
#else
typedef long int __loff_t;
#endif
#endif


typedef unsigned int in_addr_t;

#ifndef bool
#define bool int
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
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

typedef s32 suseconds_t;
typedef u32  useconds_t;
typedef s64  __time_t;
typedef s64  clock_t;
typedef __time_t time_t;



#define fd_set _types_fd_set


#define ARG_MAX 256

#ifdef  __cplusplus
}
#endif
#endif /* __TYPES_h */
