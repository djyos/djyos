

#ifndef __S3C2450_SSMC_H
#define __S3C2450_SSMC_H

#if __cplusplus
extern "C" {
#endif

struct SMC_REG
{
    vu32 SMBIDCYR0;             // 0x00
    vu32 SMBWSTRDR0;             // 0x04
    vu32 SMBWSTWRR0;             // 0x08
    vu32 SMBWSTOENR0;             // 0x0C
    vu32 SMBWSTWENR0;             // 0x10
    vu32 SMBCR0;                  // 0x14
    vu32 SMBSR0;                  // 0x18
    vu32 SMBWSTBRDR0;             // 0x1C

    vu32 SMBIDCYR1;              // 0x20
    vu32 SMBWSTRDR1;             // 0x24
    vu32 SMBWSTWRR1;             // 0x28
    vu32 SMBWSTOENR1;             // 0x2C
    vu32 SMBWSTWENR1;             // 0x30
    vu32 SMBCR1;                  // 0x34
    vu32 SMBSR1;                  // 0x38
    vu32 SMBWSTBRDR1;             // 0x3C

    vu32 SMBIDCYR2;              // 0x40
    vu32 SMBWSTRDR2;             // 0x44
    vu32 SMBWSTWRR2;             // 0x48
    vu32 SMBWSTOENR2;             // 0x4C
    vu32 SMBWSTWENR2;             // 0x50
    vu32 SMBCR2;                  // 0x54
    vu32 SMBSR2;                  // 0x58
    vu32 SMBWSTBRDR2;             // 0x5C

    vu32 SMBIDCYR3;              // 0x60
    vu32 SMBWSTRDR3;             // 0x64
    vu32 SMBWSTWRR3;             // 0x68
    vu32 SMBWSTOENR3;             // 0x6C
    vu32 SMBWSTWENR3;             // 0x70
    vu32 SMBCR3;                  // 0x74
    vu32 SMBSR3;                  // 0x78
    vu32 SMBWSTBRDR3;             // 0x7C

    vu32 SMBIDCYR4;              // 0x80
    vu32 SMBWSTRDR4;             // 0x84
    vu32 SMBWSTWRR4;             // 0x88
    vu32 SMBWSTOENR4;             // 0x8C
    vu32 SMBWSTWENR4;             // 0x90
    vu32 SMBCR4;                  // 0x94
    vu32 SMBSR4;                  // 0x98
    vu32 SMBWSTBRDR4;             // 0x9C

    vu32 SMBIDCYR5;              // 0xA0
    vu32 SMBWSTRDR5;             // 0xA4
    vu32 SMBWSTWRR5;             // 0xA8
    vu32 SMBWSTOENR5;             // 0xAC
    vu32 SMBWSTWENR5;             // 0xB0
    vu32 SMBCR5;                  // 0xB4
    vu32 SMBSR5;                  // 0xB8
    vu32 SMBWSTBRDR5;             // 0xBC

    vu32 PAD[80];                 // 0xC0 ~ 0x1FC

    vu32 SSMCSR;                  // 0x200
    vu32 SSMCCR;                  // 0x204
};

#define SMC_REG_BASE    ((struct SMC_REG*)0x4F000000)


#if __cplusplus
    }
#endif

#endif 
