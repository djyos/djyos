/****************************************************************
 *
 * The author of this software is David M. Gay.
 *
 * Copyright (c) 1991 by AT&T.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR AT&T MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 ***************************************************************/

/* Please send bug reports to
    David M. Gay
    AT&T Bell Laboratories, Room 2C-463
    600 Mountain Avenue
    Murray Hill, NJ 07974-2070
    U.S.A.
    dmg@research.att.com or research!dmg
 */

#ifndef __H_MPREC_H__
#define __H_MPREC_H__

#include "ieeefp.h"
#include "math.h"
#include "float.h"
#include "errno.h"
#include "stdlib.h"

#define _Kmax (sizeof (u64) << 3)

struct _Bigint
{
  struct _Bigint *_next;
  int _k, _maxwds, _sign, _wds;
  u32 _x[1];
};
struct _mprec
{
  /* used by mprec routines */
  struct _Bigint *_result;
  int _result_k;
  struct _Bigint *_p5s;
  struct _Bigint **_freelist;
};
struct _Bigint *_result;
int _result_k;
struct _Bigint *_p5s;
extern struct _mprec g_tMprec;

#define _REENT_CHECK_MP(ptr)    /* nothing */
#define _REENT_MP_RESULT(ptr)   ((ptr)->_result)
#define _REENT_MP_RESULT_K(ptr) ((ptr)->_result_k)
#define _REENT_MP_P5S(ptr)      ((ptr)->_p5s)
#define _REENT_MP_FREELIST(ptr) ((ptr)->_freelist)

#ifdef DEBUG
#include "stdio.h"
#define Bug(x) {fprintf(stderr, "%s\n", x); exit(1);}
#endif

#ifdef Unsigned_Shifts
#define Sign_Extend(a,b) if (b < 0) a |= (__uint32_t)0xffff0000;
#else
#define Sign_Extend(a,b) /*no-op*/
#endif

/* If we are going to examine or modify specific bits in a double using
   the word0 and/or word1 macros, then we must wrap the double inside
   a union.  This is necessary to avoid undefined behavior according to
   the ANSI C spec.  */
union double_union
{
  double d;
  __uint32_t i[2];
};

#ifdef __IEEE_LITTLE_ENDIAN
#define word0(x) (x.i[1])
#define word1(x) (x.i[0])
#else
#define word0(x) (x.i[0])
#define word1(x) (x.i[1])
#endif


/* Unfortunately, because u32 might be a different type than
   __uint32_t, we can't re-use union double_union as-is without
   further edits in strtod.c.  */
typedef union { double d; u32 i[2]; } U;

#define dword0(x) word0(x)
#define dword1(x) word1(x)
#define dval(x) (x.d)

#define SI 0    //此原与Sudden_Underflow宏有关，参看原文件。

#define Storeinc(a,b,c) (*(a)++ = ((b) << 16) | ((c) & 0xffff))

/* #define P DBL_MANT_DIG */
/* Ten_pmax = floor(P*log(2)/log(5)) */
/* Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16 */
/* Quick_max = floor((P-1)*log(FLT_RADIX)/log(10) - 1) */
/* Int_max = floor(P*log(FLT_RADIX)/log(10) - 1) */

#if defined (_DOUBLE_IS_32BITS)
#define Exp_shift   23
#define Exp_shift1  23
#define Exp_msk1    ((__uint32_t)0x00800000L)
#define Exp_msk11   ((__uint32_t)0x00800000L)
#define Exp_mask    ((__uint32_t)0x7f800000L)
#define P           24
#define Bias        127
#define NO_HEX_FP   /* not supported in this case */
#define IEEE_Arith
#define Emin        (-126)
#define Exp_1       ((__uint32_t)0x3f800000L)
#define Exp_11      ((__uint32_t)0x3f800000L)
#define Ebits       8
#define Frac_mask   ((__uint32_t)0x007fffffL)
#define Frac_mask1  ((__uint32_t)0x007fffffL)
#define Ten_pmax    10
#define Sign_bit    ((__uint32_t)0x80000000L)
#define Ten_pmax    10
#define Bletch      2
#define Bndry_mask  ((__uint32_t)0x007fffffL)
#define Bndry_mask1 ((__uint32_t)0x007fffffL)
#define LSB 1
#define Sign_bit    ((__uint32_t)0x80000000L)
#define Log2P       1
#define Tiny0       0
#define Tiny1       1
#define Quick_max   5
#define Int_max     6
#define Infinite(x) (word0(x) == ((__uint32_t)0x7f800000L))
#undef word0
#undef word1
#undef dword0
#undef dword1

#define word0(x) (x.i[0])
#define word1(x) 0
#define dword0(x) word0(x)
#define dword1(x) 0
#else

#define Exp_shift  20
#define Exp_shift1 20
#define Exp_msk1    ((__uint32_t)0x100000L)
#define Exp_msk11   ((__uint32_t)0x100000L)
#define Exp_mask  ((__uint32_t)0x7ff00000L)
#define P 53
#define Bias 1023
#define IEEE_Arith
#define Emin (-1022)
#define Exp_1  ((__uint32_t)0x3ff00000L)
#define Exp_11 ((__uint32_t)0x3ff00000L)
#define Ebits 11
#define Frac_mask  ((__uint32_t)0xfffffL)
#define Frac_mask1 ((__uint32_t)0xfffffL)
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask  ((__uint32_t)0xfffffL)
#define Bndry_mask1 ((__uint32_t)0xfffffL)
#define LSB 1
#define Sign_bit ((__uint32_t)0x80000000L)
#define Log2P 1
#define Tiny0 0
#define Tiny1 1
#define Quick_max 14
#define Int_max 14
#define Infinite(x) (word0(x) == ((__uint32_t)0x7ff00000L)) /* sufficient test for here */

#endif /* !_DOUBLE_IS_32BITS */

#ifndef Flt_Rounds
#ifdef FLT_ROUNDS
#define Flt_Rounds FLT_ROUNDS
#else
#define Flt_Rounds 1
#endif
#endif /*Flt_Rounds*/

#ifndef IEEE_Arith
#define ROUND_BIASED
#else
#define Scale_Bit 0x10
#if defined(_DOUBLE_IS_32BITS) && defined(__v800)
#define n_bigtens 2
#else
#define n_bigtens 5
#endif
#endif

#ifdef IBM
#define n_bigtens 3
#endif

#ifdef VAX
#define n_bigtens 2
#endif

#ifndef __NO_INFNAN_CHECK
#define INFNAN_CHECK
#endif

/*
 * NAN_WORD0 and NAN_WORD1 are only referenced in strtod.c.  Prior to
 * 20050115, they used to be hard-wired here (to 0x7ff80000 and 0,
 * respectively), but now are determined by compiling and running
 * qnan.c to generate gd_qnan.h, which specifies d_QNAN0 and d_QNAN1.
 * Formerly gdtoaimp.h recommended supplying suitable -DNAN_WORD0=...
 * and -DNAN_WORD1=...  values if necessary.  This should still work.
 * (On HP Series 700/800 machines, -DNAN_WORD0=0x7ff40000 works.)
 */
#ifdef IEEE_Arith
#ifdef __IEEE_BIG_ENDIAN
#define _0 0
#define _1 1
#ifndef NAN_WORD0
#define NAN_WORD0 d_QNAN0
#endif
#ifndef NAN_WORD1
#define NAN_WORD1 d_QNAN1
#endif
#else
#define _0 1
#define _1 0
#ifndef NAN_WORD0
#define NAN_WORD0 d_QNAN1
#endif
#ifndef NAN_WORD1
#define NAN_WORD1 d_QNAN0
#endif
#endif
#else
#undef INFNAN_CHECK
#endif

#ifdef RND_PRODQUOT
#define rounded_product(a,b) a = rnd_prod(a, b)
#define rounded_quotient(a,b) a = rnd_quot(a, b)
#ifdef KR_headers
extern double rnd_prod(), rnd_quot();
#else
extern double rnd_prod(double, double), rnd_quot(double, double);
#endif
#else
#define rounded_product(a,b) a *= b
#define rounded_quotient(a,b) a /= b
#endif

#define Big0 (Frac_mask1 | Exp_msk1*(DBL_MAX_EXP+Bias-1))
#define Big1 ((__uint32_t)0xffffffffL)

/* When Pack_32 is not defined, we store 16 bits per 32-bit long.
 * This makes some inner loops simpler and sometimes saves work
 * during multiplications, but it often seems to make things slightly
 * slower.  Hence the default is now to store 32 bits per long.
 */

#ifndef Pack_32
#define Pack_32
#endif

#define ULbits 32
#define kshift 5
#define kmask 31
#define ALL_ON 0xffffffff

#ifdef __cplusplus
extern "C" double strtod(const char *s00, char **se);
extern "C" char *dtoa(double d, int mode, int ndigits,
            int *decpt, int *sign, char **rve);
#endif


typedef struct _Bigint _Bigint;
#if !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__) && !defined(_SMALL_HEXDIG)
#define __get_hexdig(x) __hexdig[x] /* NOTE: must evaluate arg only once */
#else /* !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__) && !defined(_SMALL_HEXDIG) */
#define __get_hexdig(x) __hexdig_fun(x)
#endif /* !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__) && !defined(_SMALL_HEXDIG) */

#define tens __mprec_tens
#define bigtens __mprec_bigtens
#define tinytens __mprec_tinytens

struct FPI;
double      _EXFUN(ulp,(double x));
double      _EXFUN(b2d,(_Bigint *a , int *e));
_Bigint *   _EXFUN(Balloc,(struct _mprec *p, int k));
void        _EXFUN(Bfree,(struct _mprec *p, _Bigint *v));
_Bigint *   _EXFUN(multadd,(struct _mprec *p, _Bigint *, int, int));
_Bigint *   _EXFUN(s2b,(struct _mprec *, const char*, int, int, u32));
_Bigint *   _EXFUN(i2b,(struct _mprec *,int));
_Bigint *   _EXFUN(mult, (struct _mprec *, _Bigint *, _Bigint *));
_Bigint *   _EXFUN(pow5mult, (struct _mprec *, _Bigint *, int k));
int         _EXFUN(hi0bits,(u32));
int         _EXFUN(lo0bits,(u32 *));
_Bigint *   _EXFUN(d2b,(struct _mprec *p, double d, int *e, int *bits));
_Bigint *   _EXFUN(lshift,(struct _mprec *p, _Bigint *b, int k));
_Bigint *   _EXFUN(diff,(struct _mprec *p, _Bigint *a, _Bigint *b));
int     _EXFUN(cmp,(_Bigint *a, _Bigint *b));
int     _EXFUN(gethex,(struct _mprec *p, _CONST char **sp, _CONST struct FPI *fpi, s32 *exp, _Bigint **bp, int sign));
double      _EXFUN(ratio,(_Bigint *a, _Bigint *b));
u32     _EXFUN(any_on,(_Bigint *b, int k));
void        _EXFUN(copybits,(u32 *c, int n, _Bigint *b));
#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__) || defined(_SMALL_HEXDIG)
unsigned char _EXFUN(__hexdig_fun,(unsigned char));
#endif /* !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__) && !defined(_SMALL_HEXDIG) */
#ifdef INFNAN_CHECK
int     _EXFUN(hexnan,(_CONST char **sp, _CONST struct FPI *fpi, u32 *x0));
#endif

#define Bcopy(x,y) memcpy((char *)&x->_sign, (char *)&y->_sign, y->_wds*sizeof(s32) + 2*sizeof(int))

extern _CONST double tinytens[];
extern _CONST double bigtens[];
extern _CONST double tens[];
#if !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__) && !defined(_SMALL_HEXDIG)
extern _CONST unsigned char __hexdig[];
#endif /* !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__) && !defined(_SMALL_HEXDIG) */


double _EXFUN(_mprec_log10,(int));

#endif    //for #ifdef __H_MPREC_H__
