/*
 * cpu_peri_pio.h
 *
 *  Created on: 2014-12-17
 *      Author: Administrator
 */

#ifndef CPU_PERI_PIO_H_
#define CPU_PERI_PIO_H_

/*  The pin is controlled by the associated signal of peripheral A. */
#define PIO_PERIPH_A                0
/*  The pin is controlled by the associated signal of peripheral B. */
#define PIO_PERIPH_B                1
/*  The pin is controlled by the associated signal of peripheral C. */
#define PIO_PERIPH_C                2
/*  The pin is controlled by the associated signal of peripheral D. */
#define PIO_PERIPH_D                3
/*  The pin is an input. */
#define PIO_INPUT                   4
/*  The pin is an output and has a default level of 0. */
#define PIO_OUTPUT_0                5
/*  The pin is an output and has a default level of 1. */
#define PIO_OUTPUT_1                6

/*  Default pin configuration (no attribute). */
#define PIO_DEFAULT                 (0 << 0)
/*  The internal pin pull-up is active. */
#define PIO_PULLUP                  (1 << 0)
/*  The internal glitch filter is active. */
#define PIO_DEGLITCH                (1 << 1)
/*  The pin is open-drain. */
#define PIO_OPENDRAIN               (1 << 2)

/*  The internal debouncing filter is active. */
#define PIO_DEBOUNCE                (1 << 3)

/*  Enable additional interrupt modes. */
#define PIO_IT_AIME                 (1 << 4)

/*  Interrupt High Level/Rising Edge detection is active. */
#define PIO_IT_RE_OR_HL             (1 << 5)
/*  Interrupt Edge detection is active. */
#define PIO_IT_EDGE                 (1 << 6)

/*  Low level interrupt is active */
#define PIO_IT_LOW_LEVEL            (0               | 0 | PIO_IT_AIME)
/*  High level interrupt is active */
#define PIO_IT_HIGH_LEVEL           (PIO_IT_RE_OR_HL | 0 | PIO_IT_AIME)
/*  Falling edge interrupt is active */
#define PIO_IT_FALL_EDGE            (0               | PIO_IT_EDGE | PIO_IT_AIME)
/*  Rising edge interrupt is active */
#define PIO_IT_RISE_EDGE            (PIO_IT_RE_OR_HL | PIO_IT_EDGE | PIO_IT_AIME)
/** The WP is enable */
#define PIO_WPMR_WPEN_EN            ( 0x01     << 0 )
 /** The WP is disable */
#define PIO_WPMR_WPEN_DIS           ( 0x00     << 0 )
 /** Valid WP key */
#define PIO_WPMR_WPKEY_VALID        ( 0x50494F << 8 )
#ifdef __cplusplus
 extern "C" {
#endif

/*
 *          Global Macros
 */

/**
 *  Calculates the size of an array of Pin instances. The array must be defined
 *  locally (i.e. not a pointer), otherwise the computation will not be correct.
 *  \param pPins  Local array of Pin instances.
 *  \return Number of elements in array.
 */
#define PIO_LISTSIZE(pPins)    (sizeof(pPins) / sizeof(Pin))

/*
 *         Global Types
 */


/*
 *  Describes the type and attribute of one PIO pin or a group of similar pins.
 *  The #type# field can have the following values:
 *     - PIO_PERIPH_A
 *     - PIO_PERIPH_B
 *     - PIO_OUTPUT_0
 *     - PIO_OUTPUT_1
 *     - PIO_INPUT
 *
 *  The #attribute# field is a bitmask that can either be set to PIO_DEFAULt,
 *  or combine (using bitwise OR '|') any number of the following constants:
 *     - PIO_PULLUP
 *     - PIO_DEGLITCH
 *     - PIO_DEBOUNCE
 *     - PIO_OPENDRAIN
 *     - PIO_IT_LOW_LEVEL
 *     - PIO_IT_HIGH_LEVEL
 *     - PIO_IT_FALL_EDGE
 *     - PIO_IT_RISE_EDGE
 */
typedef struct _Pin
{
    /*  Bitmask indicating which pin(s) to configure. */
    uint32_t mask;
    /*  Pointer to the PIO controller which has the pin(s). */
    Pio    *pio;
    /*  Peripheral ID of the PIO controller which has the pin(s). */
    uint8_t id;
    /*  Pin type. */
    uint8_t type;
    /*  Pin attribute. */
    uint8_t attribute;
} Pin ;

/*
 *         Global Access Macros
 */

/*
 *         Global Functions
 */

extern uint8_t PIO_Configure( const Pin *list, uint32_t size ) ;

extern void PIO_Set( const Pin *pin ) ;

extern void PIO_Clear( const Pin *pin ) ;

extern uint8_t PIO_Get( const Pin *pin ) ;

extern uint8_t PIO_GetOutputDataStatus( const Pin *pin ) ;

extern void PIO_SetDebounceFilter( const Pin *pin, uint32_t cuttoff );

extern void PIO_EnableWriteProtect( const Pin *pin );

extern void PIO_DisableWriteProtect( const Pin *pin );

extern void PIO_SetPinType( Pin * pin, uint8_t pinType);

extern uint32_t PIO_GetWriteProtectViolationInfo( const Pin * pin );
#endif
extern void PIO_InitInterrupts(const Pin *pPin);

extern void PIO_ConfigureIt( const Pin *pPin,void (*handler)( const Pin* ));

extern void PIO_EnableIt(const Pin *pPin );

extern void PIO_DisableIt( const Pin *pPin );

unsigned int PIO_GetPort(Pio *pio);
unsigned int PIO_WritePort(Pio *pio, u32 data);

 /* CPU_PERI_PIO_H_ */
