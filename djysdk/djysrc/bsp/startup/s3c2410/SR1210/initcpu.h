
#ifndef __INITCPU_H__
#define __INITCPU_H__

#ifdef __cplusplus
extern "C" {
#endif

//// CP15 R1
#define R1_M    (1<<0)
#define R1_A    (1<<1)
#define R1_C    (1<<2)
#define R1_W    (1<<3)
#define R1_Z    (1<<11)
#define R1_I    (1<<12)
#define R1_V    (1<<13)
#define R1_RR   (1<<14)
#define R1_VE   (1<<24)
#define R1_nF   (1<<30)
#define R1_iA   (1<<31)



#ifdef __cplusplus
}
#endif

#endif	/*__INITCPU_H__*/

