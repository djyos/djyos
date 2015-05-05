//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块:p1020中位操作等接口函数
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:p1020中位操作等接口函数，汇编实现
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef _PPC_BITOPS_H
#define _PPC_BITOPS_H

#if !defined(CONFIG_ARCH_SUPPORTS_OPTIMIZED_INLINING) || \
    !defined(CONFIG_OPTIMIZE_INLINING) || (__GNUC__ < 4)
# define inline        inline        __attribute__((always_inline))
# define __inline__    __inline__    __attribute__((always_inline))
# define __inline    __inline    __attribute__((always_inline))
#endif

extern void set_bit(int nr, volatile void *addr);
extern void clear_bit(int nr, volatile void *addr);
extern void change_bit(int nr, volatile void *addr);
extern int test_and_set_bit(int nr, volatile void *addr);
extern int test_and_clear_bit(int nr, volatile void *addr);
extern int test_and_change_bit(int nr, volatile void *addr);

/*
 *
 * Arguably these bit operations don't imply any memory barrier or
 * SMP ordering, but in fact a lot of drivers expect them to imply
 * both, since they do on x86 cpus.
 */
#ifdef CONFIG_SMP
#define SMP_WMB        "eieio\n"
#define SMP_MB        "\nsync"
#else
#define SMP_WMB
#define SMP_MB
#endif /* CONFIG_SMP */

#define __INLINE_BITOPS    1

#if __INLINE_BITOPS
/*
 * These used to be if'd out here because using : "cc" as a constraint
 * resulted in errors from egcs.  Things may be OK with gcc-2.95.
 */
extern __inline__ void set_bit(int nr, volatile void * addr)
{
    unsigned long old;
    unsigned long mask = 1 << (nr & 0x1f);
    unsigned long *p = ((unsigned long *)addr) + (nr >> 5);

    __asm__ __volatile__(SMP_WMB "\
1:    lwarx    %0,0,%3\n\
    or    %0,%0,%2\n\
    stwcx.    %0,0,%3\n\
    bne    1b"
    SMP_MB
    : "=&r" (old), "=m" (*p)
    : "r" (mask), "r" (p), "m" (*p)
    : "cc" );
}

extern __inline__ void clear_bit(int nr, volatile void *addr)
{
    unsigned long old;
    unsigned long mask = 1 << (nr & 0x1f);
    unsigned long *p = ((unsigned long *)addr) + (nr >> 5);

    __asm__ __volatile__(SMP_WMB "\
1:    lwarx    %0,0,%3\n\
    andc    %0,%0,%2\n\
    stwcx.    %0,0,%3\n\
    bne    1b"
    SMP_MB
    : "=&r" (old), "=m" (*p)
    : "r" (mask), "r" (p), "m" (*p)
    : "cc");
}

extern __inline__ void change_bit(int nr, volatile void *addr)
{
    unsigned long old;
    unsigned long mask = 1 << (nr & 0x1f);
    unsigned long *p = ((unsigned long *)addr) + (nr >> 5);

    __asm__ __volatile__(SMP_WMB "\
1:    lwarx    %0,0,%3\n\
    xor    %0,%0,%2\n\
    stwcx.    %0,0,%3\n\
    bne    1b"
    SMP_MB
    : "=&r" (old), "=m" (*p)
    : "r" (mask), "r" (p), "m" (*p)
    : "cc");
}

extern __inline__ int test_and_set_bit(int nr, volatile void *addr)
{
    unsigned int old, t;
    unsigned int mask = 1 << (nr & 0x1f);
    volatile unsigned int *p = ((volatile unsigned int *)addr) + (nr >> 5);

    __asm__ __volatile__(SMP_WMB "\
1:    lwarx    %0,0,%4\n\
    or    %1,%0,%3\n\
    stwcx.    %1,0,%4\n\
    bne    1b"
    SMP_MB
    : "=&r" (old), "=&r" (t), "=m" (*p)
    : "r" (mask), "r" (p), "m" (*p)
    : "cc");

    return (old & mask) != 0;
}

extern __inline__ int test_and_clear_bit(int nr, volatile void *addr)
{
    unsigned int old, t;
    unsigned int mask = 1 << (nr & 0x1f);
    volatile unsigned int *p = ((volatile unsigned int *)addr) + (nr >> 5);

    __asm__ __volatile__(SMP_WMB "\
1:    lwarx    %0,0,%4\n\
    andc    %1,%0,%3\n\
    stwcx.    %1,0,%4\n\
    bne    1b"
    SMP_MB
    : "=&r" (old), "=&r" (t), "=m" (*p)
    : "r" (mask), "r" (p), "m" (*p)
    : "cc");

    return (old & mask) != 0;
}

extern __inline__ int test_and_change_bit(int nr, volatile void *addr)
{
    unsigned int old, t;
    unsigned int mask = 1 << (nr & 0x1f);
    volatile unsigned int *p = ((volatile unsigned int *)addr) + (nr >> 5);

    __asm__ __volatile__(SMP_WMB "\
1:    lwarx    %0,0,%4\n\
    xor    %1,%0,%3\n\
    stwcx.    %1,0,%4\n\
    bne    1b"
    SMP_MB
    : "=&r" (old), "=&r" (t), "=m" (*p)
    : "r" (mask), "r" (p), "m" (*p)
    : "cc");

    return (old & mask) != 0;
}
#endif /* __INLINE_BITOPS */

extern __inline__ int test_bit(int nr, __const__ volatile void *addr)
{
    __const__ unsigned int *p = (__const__ unsigned int *) addr;

    return ((p[nr >> 5] >> (nr & 0x1f)) & 1) != 0;
}
/* Return the bit position of the most significant 1 bit in a word */
/* - the result is undefined when x == 0 */
extern __inline__ int ilog2(unsigned int x)
{
    int lz;

    __asm__ ("cntlzw %0,%1" : "=r" (lz) : "r" (x));
    return 31 - lz;
}


extern __inline__ int ffz(unsigned int x)
{
    if ((x = ~x) == 0)
        return 32;
    return ilog2(x & -x);
}

/*
 * fls: find last (most-significant) bit set.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 *
 * On powerpc, __ilog2(0) returns -1, but this is not safe in general
 */
static __inline__ int fls(unsigned int x)
{
    return ilog2(x) + 1;
}
#define PLATFORM_FLS
#define BITS_PER_LONG 32
/**
 * fls64 - find last set bit in a 64-bit word
 * @x: the word to search
 *
 * This is defined in a similar way as the libc and compiler builtin
 * ffsll, but returns the position of the most significant set bit.
 *
 * fls64(value) returns 0 if value is 0 or the position of the last
 * set bit if value is nonzero. The last (most significant) bit is
 * at position 64.
 */
#if BITS_PER_LONG == 32
static __inline__ int fls64(unsigned long long x)
{
    unsigned int h = x >> 32;
    if (h)
        return fls(h) + 32;
    return fls(x);
}
#elif BITS_PER_LONG == 64
static inline int fls64(unsigned long long x)
{
    if (x == 0)
        return 0;
    return __ilog2(x) + 1;
}
#else
#error BITS_PER_LONG not 32 or 64
#endif

static __inline__ int __ilog2_u64(unsigned long long n)
{
    return fls64(n) - 1;
}

static __inline__ int ffs64(unsigned long long x)
{
    return __ilog2_u64(x & -x) + 1ull;
}

#endif /* _PPC_BITOPS_H */
