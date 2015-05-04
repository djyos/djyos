#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <_ansi.h>
#include <stddef.h>

int _EXFUN(access,(const char *__path, int __amode ));
unsigned  _EXFUN(alarm, (unsigned __secs ));
int     _EXFUN(chdir, (const char *__path ));
int     _EXFUN(chmod, (const char *__path, mode_t __mode ));
int     _EXFUN(close, (int __fildes ));
char *  _EXFUN(ctermid, (char *__s ));
char *  _EXFUN(cuserid, (char *__s ));
int     _EXFUN(dup, (int __fildes ));
int     _EXFUN(dup2, (int __fildes, int __fildes2 ));
int     _EXFUN(execl, (const char *__path, const char *, ... ));
int     _EXFUN(execle, (const char *__path, const char *, ... ));
int     _EXFUN(execlp, (const char *__file, const char *, ... ));
int     _EXFUN(execv, (const char *__path, char * const __argv[] ));
int     _EXFUN(execve, (const char *__path, char * const __argv[], char * const __envp[] ));
int     _EXFUN(execvp, (const char *__file, char * const __argv[] ));
int     _EXFUN(fchmod, (int __fildes, mode_t __mode ));
long    _EXFUN(fpathconf, (int __fd, int __name ));
int     _EXFUN(fsync, (int __fd));
int     _EXFUN(fdatasync, (int __fd));
char *  _EXFUN(getcwd, (char *__buf, size_t __size ));
char *  _EXFUN(getlogin, (void ));
char *  _EXFUN(getpass, (const char *__prompt));
int _EXFUN(getpagesize, (void));
int     _EXFUN(link, (const char *__path1, const char *__path2 ));
int _EXFUN(nice, (int __nice_value ));
off_t   _EXFUN(lseek, (int __fildes, off_t __offset, int __whence ));

int     _EXFUN(rmdir, (const char *__path ));

void    _EXFUN(swab, (const void *__restrict, void *__restrict, ssize_t));
char *  _EXFUN(ttyname, (int __fildes ));
int     _EXFUN(unlink, (const char *__path ));
int     _EXFUN(vhangup, (void ));

extern char *optarg;            /* getopt(3) external variables */
extern int optind, opterr, optopt;
int  getopt(int, char * const [], const char *);
extern int optreset;            /* getopt(3) external variable */


int     _EXFUN(ftruncate, (int __fd, off_t __length));
int     _EXFUN(truncate, (const char *, off_t __length));



ssize_t _EXFUN(readlink, (const char *__restrict __path,
                          char *__restrict __buf, size_t __buflen));
int     _EXFUN(symlink, (const char *__name1, const char *__name2));

#endif /* _UNISTD_H_ */
