/*
 * stdlib.h
 *
 * Definitions for common types, variables, and functions.
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include "arch_feature.h"
#include "_ansi.h"
#include "systime.h"
#define __need_size_t
#define __need_wchar_t
#include <stddef.h>
#include "heap.h"
_BEGIN_STD_C

typedef struct
{
  int quot; /* quotient */
  int rem; /* remainder */
} div_t;

typedef struct
{
  long quot; /* quotient */
  long rem; /* remainder */
} ldiv_t;

#if !defined(__STRICT_ANSI__) || \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  (defined(__cplusplus) && __cplusplus >= 201103L)
typedef struct
{
  long long int quot; /* quotient */
  long long int rem; /* remainder */
} lldiv_t;
#endif

#ifndef __compar_fn_t_defined
#define __compar_fn_t_defined
typedef int (*__compar_fn_t) (const _PTR, const _PTR);
#endif

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX __RAND_MAX

int _EXFUN(__locale_mb_cur_max,(_VOID));

#define MB_CUR_MAX __locale_mb_cur_max()

_VOID   _EXFUN(abort,(_VOID) _ATTRIBUTE ((noreturn)));
int _EXFUN(abs,(int));
int _EXFUN(atexit,(_VOID (*__func)(_VOID)));
double  _EXFUN(atof,(const char *__nptr));
#ifndef __STRICT_ANSI__
float   _EXFUN(atoff,(const char *__nptr));
#endif
int _EXFUN(atoi,(const char *__nptr));
long    _EXFUN(atol,(const char *__nptr));
_PTR    _EXFUN(bsearch,(const _PTR __key,
               const _PTR __base,
               size_t __nmemb,
               size_t __size,
               __compar_fn_t _compar));
div_t   _EXFUN(div,(int __numer, int __denom));
_VOID   _EXFUN(exit,(int __status) _ATTRIBUTE ((noreturn)));
char *  _EXFUN(getenv,(const char *__string));
#ifndef __STRICT_ANSI__
extern char *suboptarg;         /* getsubopt(3) external variable */
int _EXFUN(getsubopt,(char **, char * const *, char **));
#endif
long    _EXFUN(labs,(long));
ldiv_t  _EXFUN(ldiv,(long __numer, long __denom));
int _EXFUN(mblen,(const char *, size_t));
int _EXFUN(mbtowc,(wchar_t *__restrict, const char *__restrict, size_t));
int _EXFUN(wctomb,(char *, wchar_t));
size_t  _EXFUN(mbstowcs,(wchar_t *__restrict, const char *__restrict, size_t));
size_t  _EXFUN(wcstombs,(char *__restrict, const wchar_t *__restrict, size_t));
#ifndef __STRICT_ANSI__
char *  _EXFUN(mkdtemp,(char *));
int _EXFUN(mkostemp,(char *, int));
int _EXFUN(mkostemps,(char *, int, int));
int _EXFUN(mkstemp,(char *));
int _EXFUN(mkstemps,(char *, int));
char *  _EXFUN(mktemp,(char *) _ATTRIBUTE ((__warning__ ("the use of `mktemp' is dangerous; use `mkstemp' instead"))));
#endif
_VOID   _EXFUN(qsort,(_PTR __base, size_t __nmemb, size_t __size, __compar_fn_t _compar));
int _EXFUN(rand,(_VOID));
#ifndef __STRICT_ANSI__
_PTR    _EXFUN(reallocf,(_PTR __r, size_t __size));
char *  _EXFUN(realpath, (const char *__restrict path, char *__restrict resolved_path));
#endif
_VOID   _EXFUN(srand,(unsigned __seed));
double  _EXFUN(strtod,(const char *__restrict __n, char **__restrict __end_PTR));
#if !defined(__STRICT_ANSI__) || (__STDC_VERSION__ >= 199901L) || (__cplusplus >= 201103L)
float   _EXFUN(strtof,(const char *__restrict __n, char **__restrict __end_PTR));
#endif
#ifndef __STRICT_ANSI__
/* the following strtodf interface is deprecated...use strtof instead */
# ifndef strtodf
#  define strtodf strtof
# endif
#endif
s32 strtol(const char *__restrict __n, char **__restrict __end_PTR, int __base);
u32 strtoul(const char *__restrict __n, char **__restrict __end_PTR, int __base);

int _EXFUN(system,(const char *__string));

#ifndef __STRICT_ANSI__
long    _EXFUN(a64l,(const char *__input));
char *  _EXFUN(l64a,(long __input));
int _EXFUN(on_exit,(_VOID (*__func)(int, _PTR),_PTR __arg));
_VOID   _EXFUN(_Exit,(int __status) _ATTRIBUTE ((noreturn)));
int _EXFUN(putenv,(char *__string));
int _EXFUN(setenv,(const char *__string, const char *__value, int __overwrite));

char *  _EXFUN(gcvt,(double,int,char *));
char *  _EXFUN(gcvtf,(float,int,char *));
char *  _EXFUN(fcvt,(double,int,int *,int *));
char *  _EXFUN(fcvtf,(float,int,int *,int *));
char *  _EXFUN(ecvt,(double,int,int *,int *));
char *  _EXFUN(ecvtbuf,(double, int, int*, int*, char *));
char *  _EXFUN(fcvtbuf,(double, int, int*, int*, char *));
char *  _EXFUN(ecvtf,(float,int,int *,int *));
char *  _EXFUN(dtoa,(double, int, int, int *, int*, char**));

s32 rand(void);
void srand(u32 seed);
double _EXFUN(drand48,(_VOID));
u32   _EXFUN(lrand48,(_VOID));
_VOID  _EXFUN(srand48,(s32));

long long _EXFUN(atoll,(const char *__nptr));
long long _EXFUN(llabs,(long long));
lldiv_t _EXFUN(lldiv,(long long __numer, long long __denom));
#endif /* ! __STRICT_ANSI__ */
#if !defined(__STRICT_ANSI__) || (__STDC_VERSION__ >= 199901L) || (__cplusplus >= 201103L)
long long _EXFUN(strtoll,(const char *__restrict __n, char **__restrict __end_PTR, int __base));
#endif
#ifndef __STRICT_ANSI__
#endif /* ! __STRICT_ANSI__ */
#if !defined(__STRICT_ANSI__) || (__STDC_VERSION__ >= 199901L) || (__cplusplus >= 201103L)
u64 strtoull(const char *__restrict __n, char **__restrict __end_PTR, int __base);
#endif


/* On platforms where long double equals double.  */
#ifdef _LDBL_EQ_DBL
#if !defined(__STRICT_ANSI__) || (__STDC_VERSION__ >= 199901L) || (__cplusplus >= 201103L)
extern long double strtold (const char *__restrict, char **__restrict);
#endif
#endif /* _LDBL_EQ_DBL */

char * itoa(s32 d, char *buf, s32 scale);

extern void *(*M_Malloc)(ptu32_t size,u32 timeout);
extern void *(*M_Realloc) (void *, ptu32_t,u32 timeout);
extern void  (*free)(void * pl_mem);
extern void *(*M_MallocHeap)(ptu32_t size,pHeap_t Heap,u32 timeout);
extern void *(*M_MallocLc)(ptu32_t size,u32 timeout);
extern void *(*M_MallocLcHeap)(ptu32_t size,pHeap_t Heap, u32 timeout);
extern void  (*M_FreeHeap)(void * pl_mem,pHeap_t Heap);
extern void *(*__MallocStack)(ptu32_t size);
extern ptu32_t (*M_FormatSizeHeap)(ptu32_t size,pHeap_t Heap);
extern ptu32_t (*M_FormatSize)(ptu32_t size);
extern ptu32_t (*M_GetMaxFreeBlockHeap)(pHeap_t Heap);
extern ptu32_t (*M_GetMaxFreeBlock)(void);
extern ptu32_t M_GetHeapSizeHeap(pHeap_t Heap);
extern ptu32_t M_GetHeapSize(void);
pHeap_t M_FindHeap(const char *HeapName);
extern ptu32_t (*M_GetFreeMem)(void);
extern ptu32_t (*M_GetFreeMemHeap)(pHeap_t Heap);
extern ptu32_t (*M_CheckSize)(void * mp);

#define malloc(x) M_Malloc(x,CN_TIMEOUT_FOREVER)
#define realloc(p,size) M_Realloc(p,size,CN_TIMEOUT_FOREVER)
#define malloc_usable_size(ptr) M_CheckSize(ptr)

extern void * calloc (ptu32_t, ptu32_t);

_END_STD_C

#endif /* _STDLIB_H_ */
