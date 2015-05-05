/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  @(#)stdio.h 5.3 (Berkeley) 3/15/86
 */

/*
 * NB: to fit things in six character monocase externals, the
 * stdio code uses the prefix `__s' for stdio objects, typically
 * followed by a three-character attempt at a mnemonic.
 */

#ifndef _STDIO_H_
#define _STDIO_H_

#include "_ansi.h"

#include "stddef.h"

#include <stdarg.h>

_BEGIN_STD_C

typedef struct tagFileRsc FILE;
FILE *stdin,*stderr,*stdout;

#define __SLBF  0x0001      /* line buffered */
#define __SNBF  0x0002      /* unbuffered */
#define __SRD   0x0004      /* OK to read */
#define __SWR   0x0008      /* OK to write */
    /* RD and WR are never simultaneously asserted */
#define __SRW   0x0010      /* open for reading & writing */
#define __SEOF  0x0020      /* found EOF */
#define __SERR  0x0040      /* found error */
#define __SMBF  0x0080      /* _buf is from malloc */
#define __SAPP  0x0100      /* fdopen()ed in append mode - so must  write to end */
#define __SSTR  0x0200      /* this is an sprintf/snprintf string */
#define __SOPT  0x0400      /* do fseek() optimisation */
#define __SNPT  0x0800      /* do not do fseek() optimisation */
#define __SOFF  0x1000      /* set iff _offset is in fact correct */
#define __SORD  0x2000      /* true => stream orientation (byte/wide) decided */
#define __SL64  0x8000      /* is 64-bit offset large file */

/* _flags2 flags */
#define __SWID  0x2000      /* true => stream orientation wide, false => byte, only valid if __SORD in _flags is true */

/*
 * The following three definitions are for ANSI C, which took them
 * from System V, which stupidly took internal interface macros and
 * made them official arguments to setvbuf(), without renaming them.
 * Hence, these ugly _IOxxx names are *supposed* to appear in user code.
 *
 * Although these happen to match their counterparts above, the
 * implementation does not rely on that (so these could be renumbered).
 */
#define _IOFBF  0       /* setvbuf should set fully buffered */
#define _IOLBF  1       /* setvbuf should set line buffered */
#define _IONBF  2       /* setvbuf should set unbuffered */

#define EOF (-1)

#ifdef __BUFSIZ__
#define BUFSIZ      __BUFSIZ__
#else
#define BUFSIZ      1024
#endif

#ifdef __FOPEN_MAX__
#define FOPEN_MAX   __FOPEN_MAX__
#else
#define FOPEN_MAX   20
#endif

#ifdef __FILENAME_MAX__
#define FILENAME_MAX    __FILENAME_MAX__
#else
#define FILENAME_MAX    1024
#endif

#ifdef __L_tmpnam__
#define L_tmpnam    __L_tmpnam__
#else
#define L_tmpnam    FILENAME_MAX
#endif

#ifndef __STRICT_ANSI__
#define P_tmpdir        "\tmp"
#endif

#ifndef SEEK_SET
#define SEEK_SET    0   //从0开始计算
#endif
#ifndef SEEK_CUR
#define SEEK_CUR    1   //可读文件从读指针为准，只写文件以写指针为准
#endif
#ifndef SEEK_END
#define SEEK_END    2   //从文件结束计算
#endif

#define TMP_MAX     26

/*
 * Functions defined in ANSI C standard.
 */

#ifndef __VALIST
#ifdef __GNUC__
#define __VALIST __gnuc_va_list
#else
#define __VALIST char*
#endif
#endif

FILE *  _EXFUN(tmpfile, (void));
char *  _EXFUN(tmpnam, (char *));
FILE *fopen(const char *fullname, char *mode);
u32 remove(const char *fullname);
u32 rename(const char *old_fullname,const char *new_filename);
size_t fread(const void *buf,size_t size, size_t nmemb,FILE *fp);
size_t fread_r(FILE *fp,const void *buf,size_t size, size_t offset);
size_t fwrite(const void *buf,size_t size, size_t nmemb,FILE *fp);
size_t fwrite_r( FILE *fp,const void *buf,size_t size, size_t offset);
u32 fclose(FILE *fp);
u32 fflush( FILE *fp);
u32 fseek( FILE *fp, long offset, int whence);
u32 fseekL(FILE *fp, s64 offset, s32 whence);
FILE *  _EXFUN(freopen, (const char *__restrict, const char *__restrict, FILE *__restrict));
void    _EXFUN(setbuf, (FILE *__restrict, char *__restrict));
int _EXFUN(setvbuf, (FILE *__restrict, char *__restrict, int, size_t));
//该打印函数，用于调试内核或者需要即时打印服务
s32 printk (const char *fmt, ...);
s32 printf (const char *fmt, ...);
s32 sprintf(char *buf, const char *fmt, ...);
s32 snprintf(char *buf, s32 size, const char *fmt, ...);    //todo
s32 fprintf(FILE *fp, const char *fmt, ...);
int _EXFUN(fscanf, (FILE *__restrict, const char *__restrict, ...)
               _ATTRIBUTE ((__format__ (__scanf__, 2, 3))));
int _EXFUN(scanf, (const char *__restrict, ...)
               _ATTRIBUTE ((__format__ (__scanf__, 1, 2))));
int _EXFUN(sscanf, (const char *__restrict, const char *__restrict, ...)
               _ATTRIBUTE ((__format__ (__scanf__, 2, 3))));
int _EXFUN(vfprintf, (FILE *__restrict, const char *__restrict, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 2, 0))));
int _EXFUN(vprintf, (const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 1, 0))));
int _EXFUN(vsprintf, (char *__restrict, const char *__restrict, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 2, 0))));
int _EXFUN(fgetc, (FILE *));
char *  _EXFUN(fgets, (char *__restrict, int, FILE *__restrict));
int _EXFUN(fputc, (int, FILE *));
int _EXFUN(fputs, (const char *__restrict, FILE *__restrict));
int _EXFUN(getc, (FILE *));
int _EXFUN(getchar, (void));
char *  _EXFUN(gets, (char *));
int _EXFUN(putc, (int, FILE *));
int _EXFUN(putchar, (int));
int _EXFUN(puts, (const char *));
int _EXFUN(ungetc, (int, FILE *));
long    _EXFUN(ftell, ( FILE *));
void    _EXFUN(rewind, (FILE *));
void    _EXFUN(clearerr, (FILE *));
s32 _EXFUN(feof, (FILE *));
s32 _EXFUN(ferror, (FILE *));
void    _EXFUN(perror, (const char *));

#if !defined(__STRICT_ANSI__) || (__STDC_VERSION__ >= 199901L)
char *  _EXFUN(asniprintf, (char *, size_t *, const char *, ...)
               _ATTRIBUTE ((__format__ (__printf__, 3, 4))));
char *  _EXFUN(asnprintf, (char *__restrict, size_t *__restrict, const char *__restrict, ...)
               _ATTRIBUTE ((__format__ (__printf__, 3, 4))));
int _EXFUN(asprintf, (char **__restrict, const char *__restrict, ...)
               _ATTRIBUTE ((__format__ (__printf__, 2, 3))));
int _EXFUN(fcloseall, (_VOID));
int _EXFUN(fiprintf, (FILE *, const char *, ...)
               _ATTRIBUTE ((__format__ (__printf__, 2, 3))));
int _EXFUN(fiscanf, (FILE *, const char *, ...)
               _ATTRIBUTE ((__format__ (__scanf__, 2, 3))));
int _EXFUN(iprintf, (const char *, ...)
               _ATTRIBUTE ((__format__ (__printf__, 1, 2))));
int _EXFUN(iscanf, (const char *, ...)
               _ATTRIBUTE ((__format__ (__scanf__, 1, 2))));
int _EXFUN(siprintf, (char *, const char *, ...)
               _ATTRIBUTE ((__format__ (__printf__, 2, 3))));
int _EXFUN(siscanf, (const char *, const char *, ...)
               _ATTRIBUTE ((__format__ (__scanf__, 2, 3))));
char *  _EXFUN(tempnam, (const char *, const char *));
int _EXFUN(vasiprintf, (char **, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 2, 0))));
char *  _EXFUN(vasniprintf, (char *, size_t *, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 3, 0))));
char *  _EXFUN(vasnprintf, (char *, size_t *, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 3, 0))));
int _EXFUN(vasprintf, (char **, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 2, 0))));
int _EXFUN(vdiprintf, (int, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 2, 0))));
int _EXFUN(vfiprintf, (FILE *, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 2, 0))));
int _EXFUN(vfiscanf, (FILE *, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__scanf__, 2, 0))));
int _EXFUN(vfscanf, (FILE *__restrict, const char *__restrict, __VALIST)
               _ATTRIBUTE ((__format__ (__scanf__, 2, 0))));
int _EXFUN(viprintf, (const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 1, 0))));
int _EXFUN(viscanf, (const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__scanf__, 1, 0))));
int _EXFUN(vscanf, (const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__scanf__, 1, 0))));
int _EXFUN(vsiprintf, (char *, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 2, 0))));
int _EXFUN(vsiscanf, (const char *, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__scanf__, 2, 0))));
int _EXFUN(vsniprintf, (char *, size_t, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 3, 0))));
int _EXFUN(vsscanf, (const char *__restrict, const char *__restrict, __VALIST)
               _ATTRIBUTE ((__format__ (__scanf__, 2, 0))));
#endif /* !__STRICT_ANSI__ */

/*
 * Routines in POSIX 1003.1:2001.
 */

#ifndef __STRICT_ANSI__
FILE *  _EXFUN(fdopen, (int, const char *));
int _EXFUN(fileno, (FILE *));
int _EXFUN(getw, (FILE *));
int _EXFUN(pclose, (FILE *));
FILE *  _EXFUN(popen, (const char *, const char *));
int _EXFUN(putw, (int, FILE *));
void    _EXFUN(setbuffer, (FILE *, char *, int));
int _EXFUN(setlinebuf, (FILE *));
int _EXFUN(getc_unlocked, (FILE *));
int _EXFUN(getchar_unlocked, (void));
void    _EXFUN(flockfile, (FILE *));
int _EXFUN(ftrylockfile, (FILE *));
void    _EXFUN(funlockfile, (FILE *));
int _EXFUN(putc_unlocked, (int, FILE *));
int _EXFUN(putchar_unlocked, (int));
#endif /* ! __STRICT_ANSI__ */

/*
 * Routines in POSIX 1003.1:200x.
 */

#ifndef __STRICT_ANSI__
#  ifndef dprintf
int _EXFUN(dprintf, (int, const char *__restrict, ...)
               _ATTRIBUTE ((__format__ (__printf__, 2, 3))));
#  endif
FILE *  _EXFUN(fmemopen, (void *__restrict, size_t, const char *__restrict));
/* getdelim - see __getdelim for now */
/* getline - see __getline for now */
FILE *  _EXFUN(open_memstream, (char **, size_t *));
int _EXFUN(vdprintf, (int, const char *__restrict, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 2, 0))));
#endif /*__STRICT_ANSI__*/

/* Other extensions.  */

int _EXFUN(fpurge, (FILE *));

/*
 * Stdio function-access interface.
 */

#define getchar()   getc(stdin)
#define putchar(x)  putc(x, stdout)

_END_STD_C

#endif /* _STDIO_H_ */
