//----------------------------------------------------
//Copyright (C), 2013-2020,  luoshitian.
//版权所有 (C), 2013-2020,   罗侍田.
//所属模块:p1020中寄存器读写等接口函数
//作者:  罗侍田.
//版本：V1.0.0
//文件描述:p1020中寄存器读写等接口函数，汇编实现
//其他说明:
//修订历史:
//2. ...
//1. 日期: 2013-11-14
//   作者:  罗侍田.
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#ifndef _PPC_IO_H
#define _PPC_IO_H


#define SIO_CONFIG_RA   0x398
#define SIO_CONFIG_RD   0x399

#ifndef _IO_BASE
#define _IO_BASE 0
#endif

#if !defined(CONFIG_ARCH_SUPPORTS_OPTIMIZED_INLINING) || \
    !defined(CONFIG_OPTIMIZE_INLINING) || (__GNUC__ < 4)
# define inline        inline        __attribute__((always_inline))
# define __inline__    __inline__    __attribute__((always_inline))
# define __inline    __inline    __attribute__((always_inline))
#endif
/*
 * 8, 16 and 32 bit, big and little endian I/O operations, with barrier.
 *
 * Read operations have additional twi & isync to make sure the read
 * is actually performed (i.e. the data has come back) before we start
 * executing any following instructions.
 */
 extern __inline__ unsigned char in_8(const volatile unsigned char *addr)
{
    unsigned char ret;

    __asm__ __volatile__(
        "sync; lbz%U1%X1 %0,%1;\n"
        "twi 0,%0,0;\n"
        "isync" : "=r" (ret) : "m" (*addr));
    return ret;
}

 extern __inline__ void out_8(volatile unsigned char *addr, unsigned char val)
{
    __asm__ __volatile__("sync;\n"
                 "stb%U0%X0 %1,%0;\n"
                 : "=m" (*addr)
                 : "r" (val));
}

 extern __inline__ unsigned short in_le16(const volatile unsigned short *addr)
{
    unsigned short ret;

    __asm__ __volatile__("sync; lhbrx %0,0,%1;\n"
                 "twi 0,%0,0;\n"
                 "isync" : "=r" (ret) :
                  "r" (addr), "m" (*addr));
    return ret;
}

 extern __inline__ unsigned short in_be16(const volatile unsigned short *addr)
{
    unsigned short ret;

    __asm__ __volatile__("sync; lhz%U1%X1 %0,%1;\n"
                 "twi 0,%0,0;\n"
                 "isync" : "=r" (ret) : "m" (*addr));
    return ret;
}

 extern __inline__ void out_le16(volatile unsigned short *addr, unsigned short val)
{
    __asm__ __volatile__("sync; sthbrx %1,0,%2" : "=m" (*addr) :
                  "r" (val), "r" (addr));
}

 extern __inline__ void out_be16(volatile unsigned short *addr, unsigned short val)
{
    __asm__ __volatile__("sync; sth%U0%X0 %1,%0" : "=m" (*addr) : "r" (val));
}

 extern __inline__ unsigned int in_le32(const volatile unsigned *addr)
{
    unsigned int ret;

    __asm__ __volatile__("sync; lwbrx %0,0,%1;\n"
                 "twi 0,%0,0;\n"
                 "isync" : "=r" (ret) :
                 "r" (addr), "m" (*addr));
    return ret;
}

 extern __inline__ unsigned int in_be32(const volatile unsigned *addr)
{
    unsigned int ret;

    __asm__ __volatile__("sync; lwz%U1%X1 %0,%1;\n"
                 "twi 0,%0,0;\n"
                 "isync" : "=r" (ret) : "m" (*addr));
    return ret;
}

 extern __inline__ void out_le32(volatile unsigned *addr, unsigned int val)
{
    __asm__ __volatile__("sync; stwbrx %1,0,%2" : "=m" (*addr) :
                 "r" (val), "r" (addr));
}

 extern __inline__ void out_be32(volatile unsigned *addr, unsigned int val)
{
    __asm__ __volatile__("sync; stw%U0%X0 %1,%0" : "=m" (*addr) : "r" (val));
}
 static __inline__ void eieio(void)
{
    __asm__ __volatile__ ("eieio" : : : "memory");
}

static __inline__ void sync(void)
{
    __asm__ __volatile__ ("sync" : : : "memory");
}

static __inline__ void isync(void)
{
    __asm__ __volatile__ ("isync" : : : "memory");
}

 /* Clear and set bits in one shot. These macros can be used to clear and
  * set multiple bits in a register using a single call. These macros can
  * also be used to set a multiple-bit bit pattern using a mask, by
  * specifying the mask in the 'clear' parameter and the new bit pattern
  * in the 'set' parameter.
  */

 #define clrbits(type, addr, clear) \
     out_##type((addr), in_##type(addr) & ~(clear))

 #define setbits(type, addr, set) \
     out_##type((addr), in_##type(addr) | (set))

 #define clrsetbits(type, addr, clear, set) \
     out_##type((addr), (in_##type(addr) & ~(clear)) | (set))

 #define clrbits_be32(addr, clear) clrbits(be32, addr, clear)
 #define setbits_be32(addr, set) setbits(be32, addr, set)
 #define clrsetbits_be32(addr, clear, set) clrsetbits(be32, addr, clear, set)

 #define clrbits_le32(addr, clear) clrbits(le32, addr, clear)
 #define setbits_le32(addr, set) setbits(le32, addr, set)
 #define clrsetbits_le32(addr, clear, set) clrsetbits(le32, addr, clear, set)

 #define clrbits_be16(addr, clear) clrbits(be16, addr, clear)
 #define setbits_be16(addr, set) setbits(be16, addr, set)
 #define clrsetbits_be16(addr, clear, set) clrsetbits(be16, addr, clear, set)

 #define clrbits_le16(addr, clear) clrbits(le16, addr, clear)
 #define setbits_le16(addr, set) setbits(le16, addr, set)
 #define clrsetbits_le16(addr, clear, set) clrsetbits(le16, addr, clear, set)

 #define clrbits_8(addr, clear) clrbits(8, addr, clear)
 #define setbits_8(addr, set) setbits(8, addr, set)
 #define clrsetbits_8(addr, clear, set) clrsetbits(8, addr, clear, set)

#endif

