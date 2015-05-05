
#ifndef __CM0_CORE_H__
#define __CM0_CORE_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#if defined ( __CC_ARM   )
  #define __ASM            __asm           //!< asm keyword for ARM Compiler
  #define __INLINE         __inline        //!< inline keyword for ARM Compiler

#elif defined ( __ICCARM__ )
  #define __ASM           __asm            //!< asm keyword for IAR Compiler
  #define __INLINE        inline           //!< inline keyword for IAR Compiler. 
                                    //Only avaiable in High optimization mode!

#elif defined   (  __GNUC__  )
  #define __ASM            __asm           //!< asm keyword for GNU Compiler
  #define __INLINE         inline          //!< inline keyword for GNU Compiler

#elif defined   (  __TASKING__  )
  #define __ASM            __asm           //!< asm keyword for TASKING Compiler
  #define __INLINE         inline       //!< inline keyword for TASKING Compiler

#endif

#ifdef __cplusplus
  #define     __I     volatile                /*!< defines 'read only' permissions      */
#else
  #define     __I     volatile const          /*!< defines 'read only' permissions      */
#endif
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */

#define _BIT_SHIFT(IRQn)         (  (((u32)(IRQn)       )    &  0x03) * 8 )
#define _SHP_IDX(IRQn)           ( ((((u32)(IRQn) & 0x0F)-8) >>    2)     )
#define _IP_IDX(IRQn)            (   ((u32)(IRQn)            >>    2)     )

//extern void __enable_irq(void);
//extern void __disable_irq(void);
extern void __NOP(void);
extern void __WFI(void);
extern void __WFE(void);
extern void __SEV(void);
extern void __ISB(void);
extern void __DSB(void);
extern void __DMB(void);
extern u32 __get_PSP(void);
extern void __set_PSP(u32 topOfProcStack);
extern u32 __get_MSP(void);
extern void __set_MSP(u32 topOfMainStack);
extern u32 __get_PRIMASK(void);
extern void __set_PRIMASK(u32 priMask);
extern u32 __get_CONTROL(void);
extern void __set_CONTROL(u32 control);
extern u32 __REV(u32 value);
extern u32 __REV16(uint16_t value);
extern int32_t __REVSH(int16_t value);
extern void NVIC_SystemReset(void);
extern u32 SysTick_Config(u32 ticks);
extern u32 NVIC_GetPriority(IRQn_Type IRQn);
extern void NVIC_SetPriority(IRQn_Type IRQn, u32 priority);
extern void NVIC_SetPendingIRQ(IRQn_Type IRQn);
extern void NVIC_SetPendingIRQ(IRQn_Type IRQn);
extern u32 NVIC_GetPendingIRQ(IRQn_Type IRQn);
extern void NVIC_DisableIRQ(IRQn_Type IRQn);
extern void NVIC_EnableIRQ(IRQn_Type IRQn);



#ifdef __cplusplus
}
#endif

#endif /* __CM0_CORE_H__ */

