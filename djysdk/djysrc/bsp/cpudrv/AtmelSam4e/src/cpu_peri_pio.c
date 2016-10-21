/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/** \file */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "cpu_peri.h"
#include "cpu_peri_pio.h"
#include "pio/sam4e16e.h"
#include "assert.h"
/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/
// =============================================================================
typedef struct _InterruptSource
{
    /* Pointer to the source pin instance. */
    const Pin *pPin;

    /* Interrupt handler. */
    void (*handler)( const Pin* ) ;
} InterruptSource ;

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/
/* List of interrupt sources. */
static InterruptSource sIntSources[MAX_PIO_INTERRUPT_SOURCES] ;

/* Number of currently defined interrupt sources. */
static uint32_t dwNumSources = 0;

static u8 sPIO_Init = false;
/**
 * \brief Configures one or more pin(s) of a PIO controller as being controlled by
 * peripheral A. Optionally, the corresponding internal pull-up(s) can be enabled.
 *
 * \param pio  Pointer to a PIO controller.
 * \param mask  Bitmask of one or more pin(s) to configure.
 * \param enablePullUp  Indicates if the pin(s) internal pull-up shall be
 *                      configured.
 */
static void PIO_SetPeripheralA(
    Pio *pio,
    unsigned int mask,
    unsigned char enablePullUp)
{
    unsigned int abcdsr;
    /* Disable interrupts on the pin(s) */
    pio->PIO_IDR = mask;

    /* Enable the pull-up(s) if necessary */
    if (enablePullUp) {
        pio->PIO_PUER = mask;
    }
    else {

        pio->PIO_PUDR = mask;
    }

    abcdsr = pio->PIO_ABCDSR[0];
    pio->PIO_ABCDSR[0] &= (~mask & abcdsr);
    abcdsr = pio->PIO_ABCDSR[1];
    pio->PIO_ABCDSR[1] &= (~mask & abcdsr);
    pio->PIO_PDR = mask;
}

/**
 * \brief Configures one or more pin(s) of a PIO controller as being controlled by
 * peripheral B. Optionally, the corresponding internal pull-up(s) can be enabled.
 *
 * \param pio  Pointer to a PIO controller.
 * \param mask  Bitmask of one or more pin(s) to configure.
 * \param enablePullUp  Indicates if the pin(s) internal pull-up shall be
 *                      configured.
 */
static void PIO_SetPeripheralB(
    Pio *pio,
    unsigned int mask,
    unsigned char enablePullUp)
{
    unsigned int abcdsr;
    /* Disable interrupts on the pin(s) */
    pio->PIO_IDR = mask;

    /* Enable the pull-up(s) if necessary */
    if (enablePullUp) {

        pio->PIO_PUER = mask;
    }
    else {

        pio->PIO_PUDR = mask;
    }

    abcdsr = pio->PIO_ABCDSR[0];
    pio->PIO_ABCDSR[0] = (mask | abcdsr);
    abcdsr = pio->PIO_ABCDSR[1];
    pio->PIO_ABCDSR[1] &= (~mask & abcdsr);

    pio->PIO_PDR = mask;
}

/**
 * \brief Configures one or more pin(s) of a PIO controller as being controlled by
 * peripheral C. Optionally, the corresponding internal pull-up(s) can be enabled.
 *
 * \param pio  Pointer to a PIO controller.
 * \param mask  Bitmask of one or more pin(s) to configure.
 * \param enablePullUp  Indicates if the pin(s) internal pull-up shall be
 *                      configured.
 */
static void PIO_SetPeripheralC(
    Pio *pio,
    unsigned int mask,
    unsigned char enablePullUp)
{
    unsigned int abcdsr;
    /* Disable interrupts on the pin(s) */
    pio->PIO_IDR = mask;

    /* Enable the pull-up(s) if necessary */
    if (enablePullUp) {

        pio->PIO_PUER = mask;
    }
    else {

        pio->PIO_PUDR = mask;
    }

    abcdsr = pio->PIO_ABCDSR[0];
    pio->PIO_ABCDSR[0] &= (~mask & abcdsr);
    abcdsr = pio->PIO_ABCDSR[1];
    pio->PIO_ABCDSR[1] = (mask | abcdsr);

    pio->PIO_PDR = mask;
}

/**
 * \brief Configures one or more pin(s) of a PIO controller as being controlled by
 * peripheral D. Optionally, the corresponding internal pull-up(s) can be enabled.
 *
 * \param pio  Pointer to a PIO controller.
 * \param mask  Bitmask of one or more pin(s) to configure.
 * \param enablePullUp  Indicates if the pin(s) internal pull-up shall be
 *                      configured.
 */
static void PIO_SetPeripheralD(
    Pio *pio,
    unsigned int mask,
    unsigned char enablePullUp)
{
    unsigned int abcdsr;
    /* Disable interrupts on the pin(s) */
    pio->PIO_IDR = mask;

    /* Enable the pull-up(s) if necessary */
    if (enablePullUp) {

        pio->PIO_PUER = mask;
    }
    else {

        pio->PIO_PUDR = mask;
    }

    abcdsr = pio->PIO_ABCDSR[0];
    pio->PIO_ABCDSR[0] = (mask | abcdsr);
    abcdsr = pio->PIO_ABCDSR[1];
    pio->PIO_ABCDSR[1] = (mask | abcdsr);

    pio->PIO_PDR = mask;
}

/**
 * \brief Configures one or more pin(s) or a PIO controller as inputs. Optionally,
 * the corresponding internal pull-up(s) and glitch filter(s) can be enabled.
 *
 * \param pio  Pointer to a PIO controller.
 * \param mask  Bitmask indicating which pin(s) to configure as input(s).
 * \param enablePullUp  Indicates if the internal pull-up(s) must be enabled.
 * \param enableFilter  Indicates if the glitch filter(s) must be enabled.
 */
static void PIO_SetInput(
    Pio *pio,
    unsigned int mask,
    unsigned char attribute)
{
    /* Disable interrupts */
    pio->PIO_IDR = mask;

    /* Enable pull-up(s) if necessary */
    if (attribute & PIO_PULLUP)
        pio->PIO_PUER = mask;
    else
        pio->PIO_PUDR = mask;

    /* Enable Input Filter if necessary */
    if (attribute & (PIO_DEGLITCH | PIO_DEBOUNCE))
        pio->PIO_IFER = mask;
    else
        pio->PIO_IFDR = mask;

    /* Enable de-glitch or de-bounce if necessary */
    if (attribute & PIO_DEGLITCH)
    {
        pio->PIO_IFSCDR = mask;
    }
    else
    {
        if (attribute & PIO_DEBOUNCE)
        {
            pio->PIO_IFSCER = mask;
        }
    }

    /* Configure pin as input */
    pio->PIO_ODR = mask;
    pio->PIO_PER = mask;
}

/**
 * \brief Configures one or more pin(s) of a PIO controller as outputs, with the
 * given default value. Optionally, the multi-drive feature can be enabled
 * on the pin(s).
 *
 * \param pio  Pointer to a PIO controller.
 * \param mask  Bitmask indicating which pin(s) to configure.
 * \param defaultValue  Default level on the pin(s).
 * \param enableMultiDrive  Indicates if the pin(s) shall be configured as
 *                          open-drain.
 * \param enablePullUp  Indicates if the pin shall have its pull-up activated.
 */
static void PIO_SetOutput(
    Pio *pio,
    unsigned int mask,
    unsigned char defaultValue,
    unsigned char enableMultiDrive,
    unsigned char enablePullUp)
{
    /* Disable interrupts */
    pio->PIO_IDR = mask;

    /* Enable pull-up(s) if necessary */
    if (enablePullUp) {

        pio->PIO_PUER = mask;
    }
    else {

        pio->PIO_PUDR = mask;
    }

    /* Enable multi-drive if necessary */
    if (enableMultiDrive) {

        pio->PIO_MDER = mask;
    }
    else {

        pio->PIO_MDDR = mask;
    }

    /* Set default value */
    if (defaultValue) {

        pio->PIO_SODR = mask;
    }
    else {

        pio->PIO_CODR = mask;
    }

    /* Configure pin(s) as output(s) */
    pio->PIO_OER = mask;
    pio->PIO_PER = mask;
}

/*----------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Configures a list of Pin instances, each of which can either hold a single
 * pin or a group of pins, depending on the mask value; all pins are configured
 * by this function. The size of the array must also be provided and is easily
 * computed using PIO_LISTSIZE whenever its length is not known in advance.
 *
 * \param list  Pointer to a list of Pin instances.
 * \param size  Size of the Pin list (calculated using PIO_LISTSIZE).
 *
 * \return 1 if the pins have been configured properly; otherwise 0.
 */
uint8_t PIO_Configure( const Pin *list, uint32_t size )
{
    /* Configure pins */
    while ( size > 0 )
    {
        switch ( list->type )
        {

            case PIO_PERIPH_A:
                PIO_SetPeripheralA(list->pio,
                                   list->mask,
                                   (list->attribute & PIO_PULLUP) ? 1 : 0);
            break;

            case PIO_PERIPH_B:
                PIO_SetPeripheralB(list->pio,
                                   list->mask,
                                   (list->attribute & PIO_PULLUP) ? 1 : 0);
            break;

            case PIO_PERIPH_C:
                PIO_SetPeripheralC(list->pio,
                                   list->mask,
                                   (list->attribute & PIO_PULLUP) ? 1 : 0);
            break;

            case PIO_PERIPH_D:
                PIO_SetPeripheralD(list->pio,
                                   list->mask,
                                   (list->attribute & PIO_PULLUP) ? 1 : 0);
            break;
            case PIO_INPUT:
                PMC_EnablePeripheral(list->id);
                PIO_SetInput(list->pio,
                             list->mask,
                             list->attribute);
                break;

            case PIO_OUTPUT_0:
            case PIO_OUTPUT_1:
                PIO_SetOutput(list->pio,
                              list->mask,
                              (list->type == PIO_OUTPUT_1),
                              (list->attribute & PIO_OPENDRAIN) ? 1 : 0,
                              (list->attribute & PIO_PULLUP) ? 1 : 0);
                break;

            default: return 0;
        }

        list++;
        size--;
    }

    return 1;
}

/**
 * \brief Sets a high output level on all the PIOs defined in the given Pin instance.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will memorize the value they are changed to outputs.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 */
void PIO_Set(const Pin *pin)
{
    pin->pio->PIO_SODR = pin->mask;
}

/**
 * \brief Sets a low output level on all the PIOs defined in the given Pin instance.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will memorize the value they are changed to outputs.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 */
void PIO_Clear(const Pin *pin)
{
    pin->pio->PIO_CODR = pin->mask;
}

/**
 * \brief Returns 1 if one or more PIO of the given Pin instance currently have
 * a high level; otherwise returns 0. This method returns the actual value that
 * is being read on the pin. To return the supposed output value of a pin, use
 * PIO_GetOutputDataStatus() instead.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 *
 * \return 1 if the Pin instance contains at least one PIO that currently has
 * a high level; otherwise 0.
 */
unsigned char PIO_Get( const Pin *pin )
{
    unsigned int reg ;

    if ( (pin->type == PIO_OUTPUT_0) || (pin->type == PIO_OUTPUT_1) )
    {
        reg = pin->pio->PIO_ODSR ;
    }
    else
    {
        reg = pin->pio->PIO_PDSR ;
    }

    if ( (reg & pin->mask) == 0 )
    {
        return 0 ;
    }
    else
    {
        return 1 ;
    }
}

/**
 * \brief Returns 1 if one or more PIO of the given Pin are configured to output a
 * high level (even if they are not output).
 * To get the actual value of the pin, use PIO_Get() instead.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 *
 * \return 1 if the Pin instance contains at least one PIO that is configured
 * to output a high level; otherwise 0.
 */
unsigned char PIO_GetOutputDataStatus(const Pin *pin)
{
    if ((pin->pio->PIO_ODSR & pin->mask) == 0) {

        return 0;
    }
    else {

        return 1;
    }
}

/*
 * \brief Configures Glitch or Debouncing filter for input.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 * \param cuttoff  Cutt off frequency for debounce filter.
 */
void PIO_SetDebounceFilter( const Pin *pin, uint32_t cuttoff )
{
    Pio *pio = pin->pio;

    pio->PIO_IFSCER = pin->mask; /* set Debouncing, 0 bit field no effect */
    pio->PIO_SCDR = ((32678/(2*(cuttoff))) - 1) & 0x3FFF; /* the lowest 14 bits work */
}

/*
 * \brief Enable write protect.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 */
void PIO_EnableWriteProtect( const Pin *pin )
{
   Pio *pio = pin->pio;

   pio->PIO_WPMR = ( PIO_WPMR_WPKEY_VALID | PIO_WPMR_WPEN_EN );
}

/*
 * \brief Disable write protect.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 */

void PIO_DisableWriteProtect( const Pin *pin )
{
   Pio *pio = pin->pio;

   pio->PIO_WPMR = ( PIO_WPMR_WPKEY_VALID | PIO_WPMR_WPEN_DIS );
}

/*
 * \brief Get write protect violation information.
 *
 * \param pin  Pointer to a Pin instance describing one or more pins.
 */

uint32_t PIO_GetWriteProtectViolationInfo( const Pin * pin )
{
    Pio *pio = pin->pio;
    return (pio->PIO_WPSR);
}
/* \brief Set pin type
 * the pin is controlled by the corresponding peripheral (A, B, C, D,E)
 * \param pin      Pointer to a Pin instance describing one or more pins.
 * \param pinType  PIO_PERIPH_A, PIO_PERIPH_B, ...
 */

void PIO_SetPinType( Pin * pin, uint8_t pinType)
{
    pin->type = pinType;
}

// =============================================================================
// 功能：PIO的中断服务函数，所有PIO的外部中断都调用该函数，然后调用handler
// 参数：中断线
// 返回：1,OK,0,not OK
// =============================================================================
static u32 PioInterruptHandler(ptu32_t intline)
{
    u32 status,id,i;
    Pio *pPio ;

    switch(intline)
    {
    case CN_INT_LINE_PIOA:
        pPio = PIOA;
        id = ID_PIOA;
        break;
    case CN_INT_LINE_PIOB:
        pPio = PIOB;
        id = ID_PIOB;
        break;
    case CN_INT_LINE_PIOC:
        pPio = PIOC;
        id = ID_PIOC;
        break;
    case CN_INT_LINE_PIOD:
        pPio = PIOD;
        id = ID_PIOD;
        break;
    case CN_INT_LINE_PIOE:
        pPio = PIOE;
        id = ID_PIOE;
        break;
    default:
        return 0;
    }

    status = pPio->PIO_ISR;
    status &= pPio->PIO_IMR;

    if ( status != 0 )
    {
        i = 0;
        while ( status != 0 )
        {
            /* There cannot be an un-configured source enabled. */
            assert(i < dwNumSources);

            /* Source is configured on the same controller */
            if (sIntSources[i].pPin->id == id)
            {
                /* Source has PIOs whose statuses have changed */
                if ( (status & sIntSources[i].pPin->mask) != 0 )
                {
                    sIntSources[i].handler(sIntSources[i].pPin);
                    status &= ~(sIntSources[i].pPin->mask);
                }
            }
            i++;
        }
    }

    return 1;
}
static void __PIO_IntInit(u32 ID,Pio* pio ,u32 intline)
{
    PMC_EnablePeripheral( ID ) ;
    pio->PIO_ISR ;
    pio->PIO_IDR = 0xFFFFFFFF ;

    Int_Register(intline);
    Int_IsrConnect(intline,PioInterruptHandler);
    Int_SettoAsynSignal(intline);
    Int_ClearLine(intline);
    Int_ContactLine(intline);
}

// =============================================================================
// 功能：中断初始化
// 参数：无
// 返回：无
// =============================================================================
void PIO_InitInterrupts(const Pin *pPin)
{
    Pio* pio ;
    pio = pPin->pio;
    assert( pPin ) ;

    if(pio == PIOA)
    {
        if( !(sPIO_Init & (1<<0)) )
        {
            __PIO_IntInit(ID_PIOA,PIOA,CN_INT_LINE_PIOA);
            sPIO_Init |= (1<<0);
        }
    }
    else if(pio == PIOB)
    {
        if( !(sPIO_Init & (1<<1)) )
        {
            __PIO_IntInit(ID_PIOB,PIOB,CN_INT_LINE_PIOB);
            sPIO_Init |= (1<<1);
        }
    }
    else if(pio == PIOC)
    {
        if( !(sPIO_Init & (1<<2)) )
        {
            __PIO_IntInit(ID_PIOC,PIOC,CN_INT_LINE_PIOC);
            sPIO_Init |= (1<<2);
        }
    }
    else if(pio == PIOD)
    {
        if( !(sPIO_Init & (1<<3)) )
        {
            __PIO_IntInit(ID_PIOD,PIOD,CN_INT_LINE_PIOD);
            sPIO_Init |= (1<<3);
        }
    }
    else if(pio == PIOE)
    {
        if( !(sPIO_Init & (1<<4)) )
        {
            __PIO_IntInit(ID_PIOE,PIOE,CN_INT_LINE_PIOE);
            sPIO_Init |= (1<<4);
        }
    }
    else
    {

    }
}

// =============================================================================
// 功能：中断功能配置，包括中断触发模式,即水平触发（高/电平）或边沿触发（上/下降沿）
// 参数：pPin
// 返回：无
// =============================================================================
void PIO_ConfigureIt( const Pin *pPin,void (*handler)( const Pin* ))
{
    Pio* pio ;
    pio = pPin->pio;
    InterruptSource* pSource ;

    assert( pPin ) ;
    pio = pPin->pio ;
    assert( dwNumSources < MAX_PIO_INTERRUPT_SOURCES ) ;

    pSource = &(sIntSources[dwNumSources]) ;
    pSource->pPin = pPin ;
    pSource->handler = handler ;
    dwNumSources++ ;

    /* PIO3 with additional interrupt support
     * Configure additional interrupt mode registers */
    if ( pPin->attribute & PIO_IT_AIME )
    {
        // enable additional interrupt mode
        pio->PIO_AIMER  = pPin->mask ;

        // if bit field of selected pin is 1, set as Rising Edge/High level detection event
        if ( pPin->attribute & PIO_IT_RE_OR_HL )
        {
            pio->PIO_REHLSR    = pPin->mask ;
        }
        else
        {
            pio->PIO_FELLSR     = pPin->mask;
        }

        /* if bit field of selected pin is 1, set as edge detection source */
        if (pPin->attribute & PIO_IT_EDGE)
            pio->PIO_ESR     = pPin->mask;
        else
            pio->PIO_LSR     = pPin->mask;
    }
    else
    {
        /* disable additional interrupt mode */
        pio->PIO_AIMDR       = pPin->mask;
    }
}
// =============================================================================
// 功能：使能中断
// 参数：pPin
// 返回：无
// =============================================================================
void PIO_EnableIt(const Pin *pPin )
{
    assert( pPin != NULL ) ;
    pPin->pio->PIO_ISR;
    pPin->pio->PIO_IER = pPin->mask ;
}
// =============================================================================
// 功能：关闭中断
// 参数：pPin
// 返回：无
// =============================================================================
void PIO_DisableIt( const Pin *pPin )
{
    assert( pPin != NULL ) ;

    pPin->pio->PIO_IDR = pPin->mask;
}
