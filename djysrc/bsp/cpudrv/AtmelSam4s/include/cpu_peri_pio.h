/*
 * cpu_peri_pio.h
 *
 *  Created on: 2014-12-17
 *      Author: Administrator
 */

#ifndef CPU_PERI_PIO_H_
#define CPU_PERI_PIO_H_

#define PIOA       ((Pio    *)0x400E0E00U) /**< \brief (PIOA      ) Base Address */
#define PDC_PIOA   ((Pdc    *)0x400E0F68U) /**< \brief (PDC_PIOA  ) Base Address */
#define PIOB       ((Pio    *)0x400E1000U) /**< \brief (PIOB      ) Base Address */
#define PIOC       ((Pio    *)0x400E1200U) /**< \brief (PIOC      ) Base Address */

#define ID_PIOA   (11) /**< \brief Parallel I/O Controller A (PIOA) */
#define ID_PIOB   (12) /**< \brief Parallel I/O Controller B (PIOB) */
#define ID_PIOC   (13) /**< \brief Parallel I/O Controller C (PIOC) */

#define PIO_PA0            (1u << 0)  /**< \brief Pin Controlled by PA0 */
#define PIO_PA1            (1u << 1)  /**< \brief Pin Controlled by PA1 */
#define PIO_PA2            (1u << 2)  /**< \brief Pin Controlled by PA2 */
#define PIO_PA3            (1u << 3)  /**< \brief Pin Controlled by PA3 */
#define PIO_PA4            (1u << 4)  /**< \brief Pin Controlled by PA4 */
#define PIO_PA5            (1u << 5)  /**< \brief Pin Controlled by PA5 */
#define PIO_PA6            (1u << 6)  /**< \brief Pin Controlled by PA6 */
#define PIO_PA7            (1u << 7)  /**< \brief Pin Controlled by PA7 */
#define PIO_PA8            (1u << 8)  /**< \brief Pin Controlled by PA8 */
#define PIO_PA9            (1u << 9)  /**< \brief Pin Controlled by PA9 */
#define PIO_PA10           (1u << 10) /**< \brief Pin Controlled by PA10 */
#define PIO_PA11           (1u << 11) /**< \brief Pin Controlled by PA11 */
#define PIO_PA12           (1u << 12) /**< \brief Pin Controlled by PA12 */
#define PIO_PA13           (1u << 13) /**< \brief Pin Controlled by PA13 */
#define PIO_PA14           (1u << 14) /**< \brief Pin Controlled by PA14 */
#define PIO_PA15           (1u << 15) /**< \brief Pin Controlled by PA15 */
#define PIO_PA16           (1u << 16) /**< \brief Pin Controlled by PA16 */
#define PIO_PA17           (1u << 17) /**< \brief Pin Controlled by PA17 */
#define PIO_PA18           (1u << 18) /**< \brief Pin Controlled by PA18 */
#define PIO_PA19           (1u << 19) /**< \brief Pin Controlled by PA19 */
#define PIO_PA20           (1u << 20) /**< \brief Pin Controlled by PA20 */
#define PIO_PA21           (1u << 21) /**< \brief Pin Controlled by PA21 */
#define PIO_PA22           (1u << 22) /**< \brief Pin Controlled by PA22 */
#define PIO_PA23           (1u << 23) /**< \brief Pin Controlled by PA23 */
#define PIO_PA24           (1u << 24) /**< \brief Pin Controlled by PA24 */
#define PIO_PA25           (1u << 25) /**< \brief Pin Controlled by PA25 */
#define PIO_PA26           (1u << 26) /**< \brief Pin Controlled by PA26 */
#define PIO_PA27           (1u << 27) /**< \brief Pin Controlled by PA27 */
#define PIO_PA28           (1u << 28) /**< \brief Pin Controlled by PA28 */
#define PIO_PA29           (1u << 29) /**< \brief Pin Controlled by PA29 */
#define PIO_PA30           (1u << 30) /**< \brief Pin Controlled by PA30 */
#define PIO_PA31           (1u << 31) /**< \brief Pin Controlled by PA31 */
#define PIO_PB0            (1u << 0)  /**< \brief Pin Controlled by PB0 */
#define PIO_PB1            (1u << 1)  /**< \brief Pin Controlled by PB1 */
#define PIO_PB2            (1u << 2)  /**< \brief Pin Controlled by PB2 */
#define PIO_PB3            (1u << 3)  /**< \brief Pin Controlled by PB3 */
#define PIO_PB4            (1u << 4)  /**< \brief Pin Controlled by PB4 */
#define PIO_PB5            (1u << 5)  /**< \brief Pin Controlled by PB5 */
#define PIO_PB6            (1u << 6)  /**< \brief Pin Controlled by PB6 */
#define PIO_PB7            (1u << 7)  /**< \brief Pin Controlled by PB7 */
#define PIO_PB8            (1u << 8)  /**< \brief Pin Controlled by PB8 */
#define PIO_PB9            (1u << 9)  /**< \brief Pin Controlled by PB9 */
#define PIO_PB10           (1u << 10) /**< \brief Pin Controlled by PB10 */
#define PIO_PB11           (1u << 11) /**< \brief Pin Controlled by PB11 */
#define PIO_PB12           (1u << 12) /**< \brief Pin Controlled by PB12 */
#define PIO_PB13           (1u << 13) /**< \brief Pin Controlled by PB13 */
#define PIO_PB14           (1u << 14) /**< \brief Pin Controlled by PB14 */
#define PIO_PC0            (1u << 0)  /**< \brief Pin Controlled by PC0 */
#define PIO_PC1            (1u << 1)  /**< \brief Pin Controlled by PC1 */
#define PIO_PC2            (1u << 2)  /**< \brief Pin Controlled by PC2 */
#define PIO_PC3            (1u << 3)  /**< \brief Pin Controlled by PC3 */
#define PIO_PC4            (1u << 4)  /**< \brief Pin Controlled by PC4 */
#define PIO_PC5            (1u << 5)  /**< \brief Pin Controlled by PC5 */
#define PIO_PC6            (1u << 6)  /**< \brief Pin Controlled by PC6 */
#define PIO_PC7            (1u << 7)  /**< \brief Pin Controlled by PC7 */
#define PIO_PC8            (1u << 8)  /**< \brief Pin Controlled by PC8 */
#define PIO_PC9            (1u << 9)  /**< \brief Pin Controlled by PC9 */
#define PIO_PC10           (1u << 10) /**< \brief Pin Controlled by PC10 */
#define PIO_PC11           (1u << 11) /**< \brief Pin Controlled by PC11 */
#define PIO_PC12           (1u << 12) /**< \brief Pin Controlled by PC12 */
#define PIO_PC13           (1u << 13) /**< \brief Pin Controlled by PC13 */
#define PIO_PC14           (1u << 14) /**< \brief Pin Controlled by PC14 */
#define PIO_PC15           (1u << 15) /**< \brief Pin Controlled by PC15 */
#define PIO_PC16           (1u << 16) /**< \brief Pin Controlled by PC16 */
#define PIO_PC17           (1u << 17) /**< \brief Pin Controlled by PC17 */
#define PIO_PC18           (1u << 18) /**< \brief Pin Controlled by PC18 */
#define PIO_PC19           (1u << 19) /**< \brief Pin Controlled by PC19 */
#define PIO_PC20           (1u << 20) /**< \brief Pin Controlled by PC20 */
#define PIO_PC21           (1u << 21) /**< \brief Pin Controlled by PC21 */
#define PIO_PC22           (1u << 22) /**< \brief Pin Controlled by PC22 */
#define PIO_PC23           (1u << 23) /**< \brief Pin Controlled by PC23 */
#define PIO_PC24           (1u << 24) /**< \brief Pin Controlled by PC24 */
#define PIO_PC25           (1u << 25) /**< \brief Pin Controlled by PC25 */
#define PIO_PC26           (1u << 26) /**< \brief Pin Controlled by PC26 */
#define PIO_PC27           (1u << 27) /**< \brief Pin Controlled by PC27 */
#define PIO_PC28           (1u << 28) /**< \brief Pin Controlled by PC28 */
#define PIO_PC29           (1u << 29) /**< \brief Pin Controlled by PC29 */
#define PIO_PC30           (1u << 30) /**< \brief Pin Controlled by PC30 */
#define PIO_PC31           (1u << 31) /**< \brief Pin Controlled by PC31 */
/* ========== Pio definition for ADC peripheral ========== */
#define PIO_PA17X1_AD0     (1u << 17) /**< \brief Adc signal: AD0 */
#define PIO_PA18X1_AD1     (1u << 18) /**< \brief Adc signal: AD1 */
#define PIO_PC13X1_AD10    (1u << 13) /**< \brief Adc signal: AD10 */
#define PIO_PC15X1_AD11    (1u << 15) /**< \brief Adc signal: AD11 */
#define PIO_PC12X1_AD12    (1u << 12) /**< \brief Adc signal: AD12 */
#define PIO_PC29X1_AD13    (1u << 29) /**< \brief Adc signal: AD13 */
#define PIO_PC30X1_AD14    (1u << 30) /**< \brief Adc signal: AD14 */
#define PIO_PA19X1_AD2     (1u << 19) /**< \brief Adc signal: AD2/WKUP9 */
#define PIO_PA19X1_WKUP9   (1u << 19) /**< \brief Adc signal: AD2/WKUP9 */
#define PIO_PA20X1_AD3     (1u << 20) /**< \brief Adc signal: AD3/WKUP10 */
#define PIO_PA20X1_WKUP10  (1u << 20) /**< \brief Adc signal: AD3/WKUP10 */
#define PIO_PB0X1_AD4      (1u << 0)  /**< \brief Adc signal: AD4/RTCOUT0 */
#define PIO_PB0X1_RTCOUT0  (1u << 0)  /**< \brief Adc signal: AD4/RTCOUT0 */
#define PIO_PB1X1_AD5      (1u << 1)  /**< \brief Adc signal: AD5/RTCOUT1 */
#define PIO_PB1X1_RTCOUT1  (1u << 1)  /**< \brief Adc signal: AD5/RTCOUT1 */
#define PIO_PB2X1_AD6      (1u << 2)  /**< \brief Adc signal: AD6/WKUP12 */
#define PIO_PB2X1_WKUP12   (1u << 2)  /**< \brief Adc signal: AD6/WKUP12 */
#define PIO_PB3X1_AD7      (1u << 3)  /**< \brief Adc signal: AD7 */
#define PIO_PA21X1_AD8     (1u << 21) /**< \brief Adc signal: AD8 */
#define PIO_PA22X1_AD9     (1u << 22) /**< \brief Adc signal: AD9 */
#define PIO_PA8B_ADTRG     (1u << 8)  /**< \brief Adc signal: ADTRG */
/* ========== Pio definition for DACC peripheral ========== */
#define PIO_PB13X1_DAC0    (1u << 13) /**< \brief Dacc signal: DAC0 */
#define PIO_PB14X1_DAC1    (1u << 14) /**< \brief Dacc signal: DAC1 */
#define PIO_PA2C_DATRG     (1u << 2)  /**< \brief Dacc signal: DATRG */
/* ========== Pio definition for EBI peripheral ========== */
#define PIO_PC18A_A0       (1u << 18) /**< \brief Ebi signal: A0 */
#define PIO_PC19A_A1       (1u << 19) /**< \brief Ebi signal: A1 */
#define PIO_PC28A_A10      (1u << 28) /**< \brief Ebi signal: A10 */
#define PIO_PC29A_A11      (1u << 29) /**< \brief Ebi signal: A11 */
#define PIO_PC30A_A12      (1u << 30) /**< \brief Ebi signal: A12 */
#define PIO_PC31A_A13      (1u << 31) /**< \brief Ebi signal: A13 */
#define PIO_PA18C_A14      (1u << 18) /**< \brief Ebi signal: A14 */
#define PIO_PA19C_A15      (1u << 19) /**< \brief Ebi signal: A15 */
#define PIO_PA20C_A16      (1u << 20) /**< \brief Ebi signal: A16 */
#define PIO_PA0C_A17       (1u << 0)  /**< \brief Ebi signal: A17 */
#define PIO_PA1C_A18       (1u << 1)  /**< \brief Ebi signal: A18 */
#define PIO_PA23C_A19      (1u << 23) /**< \brief Ebi signal: A19 */
#define PIO_PC20A_A2       (1u << 20) /**< \brief Ebi signal: A2 */
#define PIO_PA24C_A20      (1u << 24) /**< \brief Ebi signal: A20 */
#define PIO_PC16A_A21      (1u << 16) /**< \brief Ebi signal: A21/NANDALE */
#define PIO_PC16A_NANDALE  (1u << 16) /**< \brief Ebi signal: A21/NANDALE */
#define PIO_PC17A_A22      (1u << 17) /**< \brief Ebi signal: A22/NANDCLE */
#define PIO_PC17A_NANDCLE  (1u << 17) /**< \brief Ebi signal: A22/NANDCLE */
#define PIO_PA25C_A23      (1u << 25) /**< \brief Ebi signal: A23 */
#define PIO_PC21A_A3       (1u << 21) /**< \brief Ebi signal: A3 */
#define PIO_PC22A_A4       (1u << 22) /**< \brief Ebi signal: A4 */
#define PIO_PC23A_A5       (1u << 23) /**< \brief Ebi signal: A5 */
#define PIO_PC24A_A6       (1u << 24) /**< \brief Ebi signal: A6 */
#define PIO_PC25A_A7       (1u << 25) /**< \brief Ebi signal: A7 */
#define PIO_PC26A_A8       (1u << 26) /**< \brief Ebi signal: A8 */
#define PIO_PC27A_A9       (1u << 27) /**< \brief Ebi signal: A9 */
#define PIO_PC0A_D0        (1u << 0)  /**< \brief Ebi signal: D0 */
#define PIO_PC1A_D1        (1u << 1)  /**< \brief Ebi signal: D1 */
#define PIO_PC2A_D2        (1u << 2)  /**< \brief Ebi signal: D2 */
#define PIO_PC3A_D3        (1u << 3)  /**< \brief Ebi signal: D3 */
#define PIO_PC4A_D4        (1u << 4)  /**< \brief Ebi signal: D4 */
#define PIO_PC5A_D5        (1u << 5)  /**< \brief Ebi signal: D5 */
#define PIO_PC6A_D6        (1u << 6)  /**< \brief Ebi signal: D6 */
#define PIO_PC7A_D7        (1u << 7)  /**< \brief Ebi signal: D7 */
#define PIO_PC9A_NANDOE    (1u << 9)  /**< \brief Ebi signal: NANDOE */
#define PIO_PC10A_NANDWE   (1u << 10) /**< \brief Ebi signal: NANDWE */
#define PIO_PC14A_NCS0     (1u << 14) /**< \brief Ebi signal: NCS0 */
#define PIO_PC15A_NCS1     (1u << 15) /**< \brief Ebi signal: NCS1 */
#define PIO_PA22C_NCS2     (1u << 22) /**< \brief Ebi signal: NCS2 */
#define PIO_PC12A_NCS3     (1u << 12) /**< \brief Ebi signal: NCS3 */
#define PIO_PC11A_NRD      (1u << 11) /**< \brief Ebi signal: NRD */
#define PIO_PC13A_NWAIT    (1u << 13) /**< \brief Ebi signal: NWAIT */
#define PIO_PC8A_NWE       (1u << 8)  /**< \brief Ebi signal: NWE */
/* ========== Pio definition for HSMCI peripheral ========== */
#define PIO_PA28C_MCCDA    (1u << 28) /**< \brief Hsmci signal: MCCDA */
#define PIO_PA29C_MCCK     (1u << 29) /**< \brief Hsmci signal: MCCK */
#define PIO_PA30C_MCDA0    (1u << 30) /**< \brief Hsmci signal: MCDA0 */
#define PIO_PA31C_MCDA1    (1u << 31) /**< \brief Hsmci signal: MCDA1 */
#define PIO_PA26C_MCDA2    (1u << 26) /**< \brief Hsmci signal: MCDA2 */
#define PIO_PA27C_MCDA3    (1u << 27) /**< \brief Hsmci signal: MCDA3 */
/* ========== Pio definition for PIOA peripheral ========== */
#define PIO_PA24D_PIODC0   (1u << 24) /**< \brief Pioa signal: PIODC0 */
#define PIO_PA25D_PIODC1   (1u << 25) /**< \brief Pioa signal: PIODC1 */
#define PIO_PA26D_PIODC2   (1u << 26) /**< \brief Pioa signal: PIODC2 */
#define PIO_PA27D_PIODC3   (1u << 27) /**< \brief Pioa signal: PIODC3 */
#define PIO_PA28D_PIODC4   (1u << 28) /**< \brief Pioa signal: PIODC4 */
#define PIO_PA29D_PIODC5   (1u << 29) /**< \brief Pioa signal: PIODC5 */
#define PIO_PA30D_PIODC6   (1u << 30) /**< \brief Pioa signal: PIODC6 */
#define PIO_PA31D_PIODC7   (1u << 31) /**< \brief Pioa signal: PIODC7 */
#define PIO_PA23D_PIODCCLK (1u << 23) /**< \brief Pioa signal: PIODCCLK */
#define PIO_PA15D_PIODCEN1 (1u << 15) /**< \brief Pioa signal: PIODCEN1 */
#define PIO_PA16D_PIODCEN2 (1u << 16) /**< \brief Pioa signal: PIODCEN2 */
/* ========== Pio definition for PMC peripheral ========== */
#define PIO_PA6B_PCK0      (1u << 6)  /**< \brief Pmc signal: PCK0 */
#define PIO_PB13B_PCK0     (1u << 13) /**< \brief Pmc signal: PCK0 */
#define PIO_PA17B_PCK1     (1u << 17) /**< \brief Pmc signal: PCK1 */
#define PIO_PA21B_PCK1     (1u << 21) /**< \brief Pmc signal: PCK1 */
#define PIO_PA18B_PCK2     (1u << 18) /**< \brief Pmc signal: PCK2 */
#define PIO_PA31B_PCK2     (1u << 31) /**< \brief Pmc signal: PCK2 */
#define PIO_PB3B_PCK2      (1u << 3)  /**< \brief Pmc signal: PCK2 */
/* ========== Pio definition for PWM peripheral ========== */
#define PIO_PA9C_PWMFI0    (1u << 9)  /**< \brief Pwm signal: PWMFI0 */
#define PIO_PA0A_PWMH0     (1u << 0)  /**< \brief Pwm signal: PWMH0 */
#define PIO_PA11B_PWMH0    (1u << 11) /**< \brief Pwm signal: PWMH0 */
#define PIO_PA23B_PWMH0    (1u << 23) /**< \brief Pwm signal: PWMH0 */
#define PIO_PB0A_PWMH0     (1u << 0)  /**< \brief Pwm signal: PWMH0 */
#define PIO_PC18B_PWMH0    (1u << 18) /**< \brief Pwm signal: PWMH0 */
#define PIO_PA1A_PWMH1     (1u << 1)  /**< \brief Pwm signal: PWMH1 */
#define PIO_PA12B_PWMH1    (1u << 12) /**< \brief Pwm signal: PWMH1 */
#define PIO_PA24B_PWMH1    (1u << 24) /**< \brief Pwm signal: PWMH1 */
#define PIO_PB1A_PWMH1     (1u << 1)  /**< \brief Pwm signal: PWMH1 */
#define PIO_PC19B_PWMH1    (1u << 19) /**< \brief Pwm signal: PWMH1 */
#define PIO_PA2A_PWMH2     (1u << 2)  /**< \brief Pwm signal: PWMH2 */
#define PIO_PA13B_PWMH2    (1u << 13) /**< \brief Pwm signal: PWMH2 */
#define PIO_PA25B_PWMH2    (1u << 25) /**< \brief Pwm signal: PWMH2 */
#define PIO_PB4B_PWMH2     (1u << 4)  /**< \brief Pwm signal: PWMH2 */
#define PIO_PC20B_PWMH2    (1u << 20) /**< \brief Pwm signal: PWMH2 */
#define PIO_PA7B_PWMH3     (1u << 7)  /**< \brief Pwm signal: PWMH3 */
#define PIO_PA14B_PWMH3    (1u << 14) /**< \brief Pwm signal: PWMH3 */
#define PIO_PA17C_PWMH3    (1u << 17) /**< \brief Pwm signal: PWMH3 */
#define PIO_PB14B_PWMH3    (1u << 14) /**< \brief Pwm signal: PWMH3 */
#define PIO_PC21B_PWMH3    (1u << 21) /**< \brief Pwm signal: PWMH3 */
#define PIO_PA19B_PWML0    (1u << 19) /**< \brief Pwm signal: PWML0 */
#define PIO_PB5B_PWML0     (1u << 5)  /**< \brief Pwm signal: PWML0 */
#define PIO_PC0B_PWML0     (1u << 0)  /**< \brief Pwm signal: PWML0 */
#define PIO_PC13B_PWML0    (1u << 13) /**< \brief Pwm signal: PWML0 */
#define PIO_PA20B_PWML1    (1u << 20) /**< \brief Pwm signal: PWML1 */
#define PIO_PB12A_PWML1    (1u << 12) /**< \brief Pwm signal: PWML1 */
#define PIO_PC1B_PWML1     (1u << 1)  /**< \brief Pwm signal: PWML1 */
#define PIO_PC15B_PWML1    (1u << 15) /**< \brief Pwm signal: PWML1 */
#define PIO_PA16C_PWML2    (1u << 16) /**< \brief Pwm signal: PWML2 */
#define PIO_PA30A_PWML2    (1u << 30) /**< \brief Pwm signal: PWML2 */
#define PIO_PB13A_PWML2    (1u << 13) /**< \brief Pwm signal: PWML2 */
#define PIO_PC2B_PWML2     (1u << 2)  /**< \brief Pwm signal: PWML2 */
#define PIO_PA15C_PWML3    (1u << 15) /**< \brief Pwm signal: PWML3 */
#define PIO_PC3B_PWML3     (1u << 3)  /**< \brief Pwm signal: PWML3 */
#define PIO_PC22B_PWML3    (1u << 22) /**< \brief Pwm signal: PWML3 */
/* ========== Pio definition for SPI peripheral ========== */
#define PIO_PA12A_MISO     (1u << 12) /**< \brief Spi signal: MISO */
#define PIO_PA13A_MOSI     (1u << 13) /**< \brief Spi signal: MOSI */
#define PIO_PA11A_NPCS0    (1u << 11) /**< \brief Spi signal: NPCS0 */
#define PIO_PA9B_NPCS1     (1u << 9)  /**< \brief Spi signal: NPCS1 */
#define PIO_PA31A_NPCS1    (1u << 31) /**< \brief Spi signal: NPCS1 */
#define PIO_PB14A_NPCS1    (1u << 14) /**< \brief Spi signal: NPCS1 */
#define PIO_PC4B_NPCS1     (1u << 4)  /**< \brief Spi signal: NPCS1 */
#define PIO_PA10B_NPCS2    (1u << 10) /**< \brief Spi signal: NPCS2 */
#define PIO_PA30B_NPCS2    (1u << 30) /**< \brief Spi signal: NPCS2 */
#define PIO_PB2B_NPCS2     (1u << 2)  /**< \brief Spi signal: NPCS2 */
#define PIO_PA3B_NPCS3     (1u << 3)  /**< \brief Spi signal: NPCS3 */
#define PIO_PA5B_NPCS3     (1u << 5)  /**< \brief Spi signal: NPCS3 */
#define PIO_PA22B_NPCS3    (1u << 22) /**< \brief Spi signal: NPCS3 */
#define PIO_PA14A_SPCK     (1u << 14) /**< \brief Spi signal: SPCK */
/* ========== Pio definition for SSC peripheral ========== */
#define PIO_PA18A_RD       (1u << 18) /**< \brief Ssc signal: RD */
#define PIO_PA20A_RF       (1u << 20) /**< \brief Ssc signal: RF */
#define PIO_PA19A_RK       (1u << 19) /**< \brief Ssc signal: RK */
#define PIO_PA17A_TD       (1u << 17) /**< \brief Ssc signal: TD */
#define PIO_PA15A_TF       (1u << 15) /**< \brief Ssc signal: TF */
#define PIO_PA16A_TK       (1u << 16) /**< \brief Ssc signal: TK */
/* ========== Pio definition for TC0 peripheral ========== */
#define PIO_PA4B_TCLK0     (1u << 4)  /**< \brief Tc0 signal: TCLK0 */
#define PIO_PA28B_TCLK1    (1u << 28) /**< \brief Tc0 signal: TCLK1 */
#define PIO_PA29B_TCLK2    (1u << 29) /**< \brief Tc0 signal: TCLK2 */
#define PIO_PA0B_TIOA0     (1u << 0)  /**< \brief Tc0 signal: TIOA0 */
#define PIO_PA15B_TIOA1    (1u << 15) /**< \brief Tc0 signal: TIOA1 */
#define PIO_PA26B_TIOA2    (1u << 26) /**< \brief Tc0 signal: TIOA2 */
#define PIO_PA1B_TIOB0     (1u << 1)  /**< \brief Tc0 signal: TIOB0 */
#define PIO_PA16B_TIOB1    (1u << 16) /**< \brief Tc0 signal: TIOB1 */
#define PIO_PA27B_TIOB2    (1u << 27) /**< \brief Tc0 signal: TIOB2 */
/* ========== Pio definition for TC1 peripheral ========== */
#define PIO_PC25B_TCLK3    (1u << 25) /**< \brief Tc1 signal: TCLK3 */
#define PIO_PC28B_TCLK4    (1u << 28) /**< \brief Tc1 signal: TCLK4 */
#define PIO_PC31B_TCLK5    (1u << 31) /**< \brief Tc1 signal: TCLK5 */
#define PIO_PC23B_TIOA3    (1u << 23) /**< \brief Tc1 signal: TIOA3 */
#define PIO_PC26B_TIOA4    (1u << 26) /**< \brief Tc1 signal: TIOA4 */
#define PIO_PC29B_TIOA5    (1u << 29) /**< \brief Tc1 signal: TIOA5 */
#define PIO_PC24B_TIOB3    (1u << 24) /**< \brief Tc1 signal: TIOB3 */
#define PIO_PC27B_TIOB4    (1u << 27) /**< \brief Tc1 signal: TIOB4 */
#define PIO_PC30B_TIOB5    (1u << 30) /**< \brief Tc1 signal: TIOB5 */
/* ========== Pio definition for TWI0 peripheral ========== */
#define PIO_PA4A_TWCK0     (1u << 4)  /**< \brief Twi0 signal: TWCK0 */
#define PIO_PA3A_TWD0      (1u << 3)  /**< \brief Twi0 signal: TWD0 */
/* ========== Pio definition for TWI1 peripheral ========== */
#define PIO_PB5A_TWCK1     (1u << 5)  /**< \brief Twi1 signal: TWCK1 */
#define PIO_PB4A_TWD1      (1u << 4)  /**< \brief Twi1 signal: TWD1 */
/* ========== Pio definition for UART0 peripheral ========== */
#define PIO_PA9A_URXD0     (1u << 9)  /**< \brief Uart0 signal: URXD0 */
#define PIO_PA10A_UTXD0    (1u << 10) /**< \brief Uart0 signal: UTXD0 */
/* ========== Pio definition for UART1 peripheral ========== */
#define PIO_PB2A_URXD1     (1u << 2)  /**< \brief Uart1 signal: URXD1 */
#define PIO_PB3A_UTXD1     (1u << 3)  /**< \brief Uart1 signal: UTXD1 */
/* ========== Pio definition for USART0 peripheral ========== */
#define PIO_PA8A_CTS0      (1u << 8)  /**< \brief Usart0 signal: CTS0 */
#define PIO_PA7A_RTS0      (1u << 7)  /**< \brief Usart0 signal: RTS0 */
#define PIO_PA5A_RXD0      (1u << 5)  /**< \brief Usart0 signal: RXD0 */
#define PIO_PA2B_SCK0      (1u << 2)  /**< \brief Usart0 signal: SCK0 */
#define PIO_PA6A_TXD0      (1u << 6)  /**< \brief Usart0 signal: TXD0 */
/* ========== Pio definition for USART1 peripheral ========== */
#define PIO_PA25A_CTS1     (1u << 25) /**< \brief Usart1 signal: CTS1 */
#define PIO_PA26A_DCD1     (1u << 26) /**< \brief Usart1 signal: DCD1 */
#define PIO_PA28A_DSR1     (1u << 28) /**< \brief Usart1 signal: DSR1 */
#define PIO_PA27A_DTR1     (1u << 27) /**< \brief Usart1 signal: DTR1 */
#define PIO_PA29A_RI1      (1u << 29) /**< \brief Usart1 signal: RI1 */
#define PIO_PA24A_RTS1     (1u << 24) /**< \brief Usart1 signal: RTS1 */
#define PIO_PA21A_RXD1     (1u << 21) /**< \brief Usart1 signal: RXD1 */
#define PIO_PA23A_SCK1     (1u << 23) /**< \brief Usart1 signal: SCK1 */
#define PIO_PA22A_TXD1     (1u << 22) /**< \brief Usart1 signal: TXD1 */
/* ========== Pio indexes ========== */
#define PIO_PA0_IDX        0
#define PIO_PA1_IDX        1
#define PIO_PA2_IDX        2
#define PIO_PA3_IDX        3
#define PIO_PA4_IDX        4
#define PIO_PA5_IDX        5
#define PIO_PA6_IDX        6
#define PIO_PA7_IDX        7
#define PIO_PA8_IDX        8
#define PIO_PA9_IDX        9
#define PIO_PA10_IDX       10
#define PIO_PA11_IDX       11
#define PIO_PA12_IDX       12
#define PIO_PA13_IDX       13
#define PIO_PA14_IDX       14
#define PIO_PA15_IDX       15
#define PIO_PA16_IDX       16
#define PIO_PA17_IDX       17
#define PIO_PA18_IDX       18
#define PIO_PA19_IDX       19
#define PIO_PA20_IDX       20
#define PIO_PA21_IDX       21
#define PIO_PA22_IDX       22
#define PIO_PA23_IDX       23
#define PIO_PA24_IDX       24
#define PIO_PA25_IDX       25
#define PIO_PA26_IDX       26
#define PIO_PA27_IDX       27
#define PIO_PA28_IDX       28
#define PIO_PA29_IDX       29
#define PIO_PA30_IDX       30
#define PIO_PA31_IDX       31
#define PIO_PB0_IDX        32
#define PIO_PB1_IDX        33
#define PIO_PB2_IDX        34
#define PIO_PB3_IDX        35
#define PIO_PB4_IDX        36
#define PIO_PB5_IDX        37
#define PIO_PB6_IDX        38
#define PIO_PB7_IDX        39
#define PIO_PB8_IDX        40
#define PIO_PB9_IDX        41
#define PIO_PB10_IDX       42
#define PIO_PB11_IDX       43
#define PIO_PB12_IDX       44
#define PIO_PB13_IDX       45
#define PIO_PB14_IDX       46
#define PIO_PC0_IDX        64
#define PIO_PC1_IDX        65
#define PIO_PC2_IDX        66
#define PIO_PC3_IDX        67
#define PIO_PC4_IDX        68
#define PIO_PC5_IDX        69
#define PIO_PC6_IDX        70
#define PIO_PC7_IDX        71
#define PIO_PC8_IDX        72
#define PIO_PC9_IDX        73
#define PIO_PC10_IDX       74
#define PIO_PC11_IDX       75
#define PIO_PC12_IDX       76
#define PIO_PC13_IDX       77
#define PIO_PC14_IDX       78
#define PIO_PC15_IDX       79
#define PIO_PC16_IDX       80
#define PIO_PC17_IDX       81
#define PIO_PC18_IDX       82
#define PIO_PC19_IDX       83
#define PIO_PC20_IDX       84
#define PIO_PC21_IDX       85
#define PIO_PC22_IDX       86
#define PIO_PC23_IDX       87
#define PIO_PC24_IDX       88
#define PIO_PC25_IDX       89
#define PIO_PC26_IDX       90
#define PIO_PC27_IDX       91
#define PIO_PC28_IDX       92
#define PIO_PC29_IDX       93
#define PIO_PC30_IDX       94
#define PIO_PC31_IDX       95

//typedef struct
//{
//  vu32 PIO_PER;        /**< \brief (Pio Offset: 0x0000) PIO Enable Register */
//  vu32 PIO_PDR;        /**< \brief (Pio Offset: 0x0004) PIO Disable Register */
//  vu32 PIO_PSR;        /**< \brief (Pio Offset: 0x0008) PIO Status Register */
//  vu32 Reserved1[1];
//  vu32 PIO_OER;        /**< \brief (Pio Offset: 0x0010) Output Enable Register */
//  vu32 PIO_ODR;        /**< \brief (Pio Offset: 0x0014) Output Disable Register */
//  vu32 PIO_OSR;        /**< \brief (Pio Offset: 0x0018) Output Status Register */
//  vu32 Reserved2[1];
//  vu32 PIO_IFER;       /**< \brief (Pio Offset: 0x0020) Glitch Input Filter Enable Register */
//  vu32 PIO_IFDR;       /**< \brief (Pio Offset: 0x0024) Glitch Input Filter Disable Register */
//  vu32 PIO_IFSR;       /**< \brief (Pio Offset: 0x0028) Glitch Input Filter Status Register */
//  vu32 Reserved3[1];
//  vu32 PIO_SODR;       /**< \brief (Pio Offset: 0x0030) Set Output Data Register */
//  vu32 PIO_CODR;       /**< \brief (Pio Offset: 0x0034) Clear Output Data Register */
//  vu32 PIO_ODSR;       /**< \brief (Pio Offset: 0x0038) Output Data Status Register */
//  vu32 PIO_PDSR;       /**< \brief (Pio Offset: 0x003C) Pin Data Status Register */
//  vu32 PIO_IER;        /**< \brief (Pio Offset: 0x0040) Interrupt Enable Register */
//  vu32 PIO_IDR;        /**< \brief (Pio Offset: 0x0044) Interrupt Disable Register */
//  vu32 PIO_IMR;        /**< \brief (Pio Offset: 0x0048) Interrupt Mask Register */
//  vu32 PIO_ISR;        /**< \brief (Pio Offset: 0x004C) Interrupt Status Register */
//  vu32 PIO_MDER;       /**< \brief (Pio Offset: 0x0050) Multi-driver Enable Register */
//  vu32 PIO_MDDR;       /**< \brief (Pio Offset: 0x0054) Multi-driver Disable Register */
//  vu32 PIO_MDSR;       /**< \brief (Pio Offset: 0x0058) Multi-driver Status Register */
//  vu32 Reserved4[1];
//  vu32 PIO_PUDR;       /**< \brief (Pio Offset: 0x0060) Pull-up Disable Register */
//  vu32 PIO_PUER;       /**< \brief (Pio Offset: 0x0064) Pull-up Enable Register */
//  vu32 PIO_PUSR;       /**< \brief (Pio Offset: 0x0068) Pad Pull-up Status Register */
//  vu32 Reserved5[1];
//  vu32 PIO_ABCDSR[2];  /**< \brief (Pio Offset: 0x0070) Peripheral Select Register */
//  vu32 Reserved6[2];
//  vu32 PIO_IFSCDR;     /**< \brief (Pio Offset: 0x0080) Input Filter Slow Clock Disable Register */
//  vu32 PIO_IFSCER;     /**< \brief (Pio Offset: 0x0084) Input Filter Slow Clock Enable Register */
//  vu32 PIO_IFSCSR;     /**< \brief (Pio Offset: 0x0088) Input Filter Slow Clock Status Register */
//  vu32 PIO_SCDR;       /**< \brief (Pio Offset: 0x008C) Slow Clock Divider Debouncing Register */
//  vu32 PIO_PPDDR;      /**< \brief (Pio Offset: 0x0090) Pad Pull-down Disable Register */
//  vu32 PIO_PPDER;      /**< \brief (Pio Offset: 0x0094) Pad Pull-down Enable Register */
//  vu32 PIO_PPDSR;      /**< \brief (Pio Offset: 0x0098) Pad Pull-down Status Register */
//  vu32 Reserved7[1];
//  vu32 PIO_OWER;       /**< \brief (Pio Offset: 0x00A0) Output Write Enable */
//  vu32 PIO_OWDR;       /**< \brief (Pio Offset: 0x00A4) Output Write Disable */
//  vu32 PIO_OWSR;       /**< \brief (Pio Offset: 0x00A8) Output Write Status Register */
//  vu32 Reserved8[1];
//  vu32 PIO_AIMER;      /**< \brief (Pio Offset: 0x00B0) Additional Interrupt Modes Enable Register */
//  vu32 PIO_AIMDR;      /**< \brief (Pio Offset: 0x00B4) Additional Interrupt Modes Disables Register */
//  vu32 PIO_AIMMR;      /**< \brief (Pio Offset: 0x00B8) Additional Interrupt Modes Mask Register */
//  vu32 Reserved9[1];
//  vu32 PIO_ESR;        /**< \brief (Pio Offset: 0x00C0) Edge Select Register */
//  vu32 PIO_LSR;        /**< \brief (Pio Offset: 0x00C4) Level Select Register */
//  vu32 PIO_ELSR;       /**< \brief (Pio Offset: 0x00C8) Edge/Level Status Register */
//  vu32 Reserved10[1];
//  vu32 PIO_FELLSR;     /**< \brief (Pio Offset: 0x00D0) Falling Edge/Low Level Select Register */
//  vu32 PIO_REHLSR;     /**< \brief (Pio Offset: 0x00D4) Rising Edge/ High Level Select Register */
//  vu32 PIO_FRLHSR;     /**< \brief (Pio Offset: 0x00D8) Fall/Rise - Low/High Status Register */
//  vu32 Reserved11[1];
//  vu32 PIO_LOCKSR;     /**< \brief (Pio Offset: 0x00E0) Lock Status */
//  vu32 PIO_WPMR;       /**< \brief (Pio Offset: 0x00E4) Write Protect Mode Register */
//  vu32 PIO_WPSR;       /**< \brief (Pio Offset: 0x00E8) Write Protect Status Register */
//  vu32 Reserved12[5];
//  vu32 PIO_SCHMITT;    /**< \brief (Pio Offset: 0x0100) Schmitt Trigger Register */
//  vu32 Reserved13[19];
//  vu32 PIO_PCMR;       /**< \brief (Pio Offset: 0x150) Parallel Capture Mode Register */
//  vu32 PIO_PCIER;      /**< \brief (Pio Offset: 0x154) Parallel Capture Interrupt Enable Register */
//  vu32 PIO_PCIDR;      /**< \brief (Pio Offset: 0x158) Parallel Capture Interrupt Disable Register */
//  vu32 PIO_PCIMR;      /**< \brief (Pio Offset: 0x15C) Parallel Capture Interrupt Mask Register */
//  vu32 PIO_PCISR;      /**< \brief (Pio Offset: 0x160) Parallel Capture Interrupt Status Register */
//  vu32 PIO_PCRHR;      /**< \brief (Pio Offset: 0x164) Parallel Capture Reception Holding Register */
//  vu32 PIO_RPR;        /**< \brief (Pio Offset: 0x168) Receive Pointer Register */
//  vu32 PIO_RCR;        /**< \brief (Pio Offset: 0x16C) Receive Counter Register */
//  vu32 PIO_TPR;        /**< \brief (Pio Offset: 0x170) Transmit Pointer Register */
//  vu32 PIO_TCR;        /**< \brief (Pio Offset: 0x174) Transmit Counter Register */
//  vu32 PIO_RNPR;       /**< \brief (Pio Offset: 0x178) Receive Next Pointer Register */
//  vu32 PIO_RNCR;       /**< \brief (Pio Offset: 0x17C) Receive Next Counter Register */
//  vu32 PIO_TNPR;       /**< \brief (Pio Offset: 0x180) Transmit Next Pointer Register */
//  vu32 PIO_TNCR;       /**< \brief (Pio Offset: 0x184) Transmit Next Counter Register */
//  vu32 PIO_PTCR;       /**< \brief (Pio Offset: 0x188) Transfer Control Register */
//  vu32 PIO_PTSR;       /**< \brief (Pio Offset: 0x18C) Transfer Status Register */
//}Pio;

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

#endif /* CPU_PERI_PIO_H_ */
