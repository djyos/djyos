/**************************************************************************//**
 * $Id: lpc12xx_comp.h 433 2010-09-02 08:47:34Z cnh20509 $
 *
 * @file     lpc12xx_comp.h
 * @brief    Contains all macro definitions and function prototypes
 *           support for Comparator firmware library on LPC12xx.
 * @version  1.0
 * @date     26. Sep. 2010
 * @author   NXP MCU Team
 *
 * @note
 * Copyright (C) 2010 NXP Semiconductors(NXP). All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 ******************************************************************************/
 
/* Peripheral group --------------------------------------------------------- */
/** @defgroup Comparator
 * @ingroup LPC1200CMSIS_FwLib_Drivers 
 * @{
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LPC12xx_COMP_H
#define __LPC12xx_COMP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "LPC12xx.h"
#include "lpc_types.h"

/* Public Types ------------------------------------------------------------- */

/* Public Macros -------------------------------------------------------------- */
/** @defgroup COMP_Public_Macros
 * @{
 */

/** @defgroup COMP_index
  * @{
  */
  
#define COMP_INDEX_0   0
#define COMP_INDEX_1   1
#define PARAM_COMP_INDEX(INDEX) (((INDEX) == COMP_INDEX_0) || \
                                 ((INDEX) == COMP_INDEX_1))
                                   
/**
  * @}
  */ 
  
/** @defgroup COMP_interrupt_source
  * @{
  */
   
#define COMP_INTSRC_LEVEL           ((uint32_t) 0x00000004)
#define COMP_INTSRC_RISINGEDGE      ((uint32_t) 0x00000008)
#define COMP_INTSRC_FALLINGEDGE     ((uint32_t) 0x00000000)
#define COMP_INTSRC_DOUBLEEDGE      ((uint32_t) 0x00000010)
#define PARAM_COMP_INTSRC(INTSRC)   (((INTSRC) == COMP_INTSRC_LEVEL) || \
                                     ((INTSRC) == COMP_INTSRC_RISINGEDGE) || \
                                     ((INTSRC) == COMP_INTSRC_FALLINGEDGE) || \
                                     ((INTSRC) == COMP_INTSRC_DOUBLEEDGE))
                                   
/**
  * @}
  */ 
  
/** @defgroup COMP_ROSC_orientation
  * @{
  */

/* Orientation of the two comparators in the relaxation oscillator circuit. */
#define COMP_ROSCORIENTATION_A     ((uint32_t) 0x00000000)
#define COMP_ROSCORIENTATION_B     ((uint32_t) 0x00000020)
#define PARAM_COMP_ROSCORIENTATION(ROSCORIENTATION) (((ROSCORIENTATION) == COMP_ROSCORIENTATION_A) || \
                                                     ((ROSCORIENTATION) == COMP_ROSCORIENTATION_B))
                                   
/**
  * @}
  */ 

/** @defgroup COMP_input_port
  * @{
  */
  
/* Comparator input port */
#define COMP_INPORT_POSITIVE     (0x0)
#define COMP_INPORT_NEGATIVE     (0x1)
#define PARAM_COMP_INPORT(INPORT) (((INPORT) == COMP_INPORT_POSITIVE) || \
                                   ((INPORT) == COMP_INPORT_NEGATIVE))
                                   
/**
  * @}
  */       

/** @defgroup COMP_output_config
  * @{
  */
  
/* Comparator output sync/async select */
#define COMP_OUTPUT_ASYNC   (0x0)
#define COMP_OUTPUT_SYNC    (0x1)
#define PARAM_COMP_OUTPUT(OUTPUT) (((OUTPUT) == COMP_OUTPUT_ASYNC) || \
                                   ((OUTPUT) == COMP_OUTPUT_SYNC))
                                   
/**
  * @}
  */
  
/** @defgroup COMP0_input_channel
  * @{
  */

/* Comparator 0 input channel */
#define COMP0_INCHANNEL_VLD       ((uint8_t) 0x00)    /* voltage ladder output */
#define COMP0_INCHANNEL_P0IN0     ((uint8_t) 0x01)    /* ACMP0_I0, PIO0_19 */
#define COMP0_INCHANNEL_P0IN1     ((uint8_t) 0x02)    /* ACMP0_I1, PIO0_20 */
#define COMP0_INCHANNEL_P0IN2     ((uint8_t) 0x03)    /* ACMP0_I2, PIO0_21 */
#define COMP0_INCHANNEL_P0IN3     ((uint8_t) 0x04)    /* ACMP0_I3, PIO0_22 */
#define COMP0_INCHANNEL_P1IN0     ((uint8_t) 0x05)    /* ACMP1_I0, PIO0_23 */
#define COMP0_INCHANNEL_P1IN1     ((uint8_t) 0x06)    /* ACMP1_I1, PIO0_24 */
#define COMP0_INCHANNEL_BODBG     ((uint8_t) 0x07)    /* BOD 0.9V band gap */
#define PARAM_COMP0_INCHANNEL(INCHANNEL) (((INCHANNEL) == COMP0_INCHANNEL_VLD) || \
                                          ((INCHANNEL) == COMP0_INCHANNEL_P0IN0) || \
                                          ((INCHANNEL) == COMP0_INCHANNEL_P0IN1) || \
                                          ((INCHANNEL) == COMP0_INCHANNEL_P0IN2) || \
                                          ((INCHANNEL) == COMP0_INCHANNEL_P0IN3) || \
                                          ((INCHANNEL) == COMP0_INCHANNEL_P1IN0) || \
                                          ((INCHANNEL) == COMP0_INCHANNEL_P1IN1) || \
                                          ((INCHANNEL) == COMP0_INCHANNEL_BODBG))
                                   
/**
  * @}
  */

/** @defgroup COMP1_input_channel
  * @{
  */

#define COMP1_INCHANNEL_VLD       ((uint8_t) 0x00)    /* voltage ladder output */
#define COMP1_INCHANNEL_P1IN0     ((uint8_t) 0x01)    /* ACMP1_I0, PIO0_23 */
#define COMP1_INCHANNEL_P1IN1     ((uint8_t) 0x02)    /* ACMP1_I1, PIO0_24 */
#define COMP1_INCHANNEL_P1IN2     ((uint8_t) 0x03)    /* ACMP1_I2, PIO0_25 */
#define COMP1_INCHANNEL_P1IN3     ((uint8_t) 0x04)    /* ACMP1_I3, PIO0_26 */
#define COMP1_INCHANNEL_P0IN0     ((uint8_t) 0x05)    /* ACMP0_I0, PIO0_19 */
#define COMP1_INCHANNEL_P0IN1     ((uint8_t) 0x06)    /* ACMP0_I1, PIO0_20 */
#define COMP1_INCHANNEL_BODBG     ((uint8_t) 0x07)    /* BOD 0.9V band gap */
#define PARAM_COMP1_INCHANNEL(INCHANNEL) (((INCHANNEL) == COMP1_INCHANNEL_VLD) || \
                                          ((INCHANNEL) == COMP1_INCHANNEL_P1IN0) || \
                                          ((INCHANNEL) == COMP1_INCHANNEL_P1IN1) || \
                                          ((INCHANNEL) == COMP1_INCHANNEL_P1IN2) || \
                                          ((INCHANNEL) == COMP1_INCHANNEL_P1IN3) || \
                                          ((INCHANNEL) == COMP1_INCHANNEL_P0IN0) || \
                                          ((INCHANNEL) == COMP1_INCHANNEL_P0IN1) || \
                                          ((INCHANNEL) == COMP1_INCHANNEL_BODBG))
                                   
/**
  * @}
  */

         
/** @defgroup COMP_vlad_reference
  * @{
  */

#define COMP_VLADREF_CMP    ((uint8_t) 0x00)    /* VREF_CMP pin */
#define COMP_VLADREF_VDD    ((uint8_t) 0x40)    /* Vdd(3V3) pin */
#define PARAM_COMP_VLADREF(VLADREF) (((VLADREF) == COMP_VLADREF_CMP) || \
                                     ((VLADREF) == COMP_VLADREF_VDD))
                                   
/**
  * @}
  */         

/**
  * @}
  */ 

/* Public Functions ----------------------------------------------------------- */
/** @defgroup COMP_Public_Functions
 * @{
 */
 
void COMP0_InputConfig (uint32_t COMP_INPORT, uint32_t COMP0_INCHANNEL);
void COMP1_InputConfig (uint32_t COMP_INPORT, uint32_t COMP1_INCHANNEL);
void COMP_ClearIntPendingBit (void);
void COMP_Cmd(uint32_t COMP_INDEX, FunctionalState NewState);
void COMP_DeInit(void);
FlagStatus COMP_GetOutputStatus (uint32_t COMP_INDEX);
void COMP_Init (void);
void COMP_IntConfig (uint32_t COMP_INTSRC);
void COMP_OutputConfig (uint32_t COMP_INDEX, uint32_t COMP_OUTPUT);
void COMP_OutputSyncCmd (uint32_t COMP_INDEX, FunctionalState NewState);
void COMP_VolLadderCmd (FunctionalState NewState);
void COMP_VolLadderConfig (uint8_t COMP_VLADREF, uint8_t MValue);


/**
 * @}
 */
 
#ifdef __cplusplus
}
#endif

#endif /* __LPC12xx_COMP_H */

/**
 * @}
 */
 
/* --------------------------------- End Of File ------------------------------ */
