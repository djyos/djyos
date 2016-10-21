/*
 * glcd.c
 *
 *  Created on: 2015Äê12ÔÂ1ÈÕ
 *      Author: zhangqf
 */


/******************************************************************************/
/* GLCD_8bitIF_SAM4E.c: ATSAM4E-EK low level Graphic LCD (240x320 pixels)     */
/*                      driven with 8-bit parallel interface                  */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2013 Keil - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "stdint.h"
#include "stddef.h"
#include "cpu_peri.h"
#include "GLCD.h"
//#include "Font_6x8_h.h"
//#include "Font_16x24_h.h"

/************************** Orientation  configuration ************************/

#define LANDSCAPE   1                   /* 1 for landscape, 0 for portrait    */


/*********************** Hardware specific configuration **********************/

/*------------------------- Speed dependant settings -------------------------*/

/* If processor works on high frequency delay has to be increased, it can be
   increased by factor 2^N by this constant                                   */
#define DELAY_2N    18

/*---------------------- Graphic LCD size definitions ------------------------*/

#if (LANDSCAPE == 1)
#define WIDTH       320                 /* Screen Width (in pixels)           */
#define HEIGHT      240                 /* Screen Hight (in pixels)           */
#else
#define WIDTH       240                 /* Screen Width (in pixels)           */
#define HEIGHT      320                 /* Screen Hight (in pixels)           */
#endif
#define BPP         16                  /* Bits per pixel                     */
#define BYPP        ((BPP+7)/8)         /* Bytes per pixel                    */

/*--------------- Graphic LCD interface hardware definitions -----------------*/

/* Note: LCD /CS is CCS1                                                      */
#define LCD_BASE   (0x61000000UL)
#define LCD_REG8   (*((volatile unsigned char *)(LCD_BASE  )))
#define LCD_DAT8   (*((volatile unsigned char *)(LCD_BASE+2)))

#define BG_COLOR  0                     /* Background color                   */
#define TXT_COLOR 1                     /* Text color                         */


/*---------------------------- Global variables ------------------------------*/

/******************************************************************************/
static volatile unsigned short Color[2] = {White, Black};


/************************ Local auxiliary functions ***************************/

/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/

static void delay (int cnt) {
  cnt <<= DELAY_2N;
  while (cnt--);
}


/*******************************************************************************
* Write a command the LCD controller                                           *
*   Parameter:    cmd:    command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_cmd (unsigned char cmd) {

  LCD_REG8 = 0;
  LCD_REG8 = cmd;
}


/*******************************************************************************
* Write data to the LCD controller                                             *
*   Parameter:    dat:    data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat (unsigned short dat) {

  LCD_DAT8 = (dat >> 8) & 0xFF;
  LCD_DAT8 =  dat       & 0xFF;
}


/*******************************************************************************
* Start of data writing to the LCD controller                                  *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_start (void) {

  /* only used for SPI interface */
}


/*******************************************************************************
* Stop of data writing to the LCD controller                                   *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_stop (void) {

  /* only used for SPI interface */
}


/*******************************************************************************
* Data writing to the LCD controller                                           *
*   Parameter:    dat:    data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_only (unsigned short dat) {

  LCD_DAT8 = (dat >> 8) & 0xFF;
  LCD_DAT8 =  dat       & 0xFF;
}

#if 0
/*******************************************************************************
* Read data from the LCD controller                                            *
*   Parameter:                                                                 *
*   Return:               read data                                            *
*******************************************************************************/

static __inline unsigned short rd_dat (void) {
  unsigned short val = 0;

  val   = LCD_DAT8;                     /* Read D8..D15                       */
  val <<= 8;
  val  |= LCD_DAT8;                     /* Read D0..D7                        */
  return (val);
}
#endif

/*******************************************************************************
* Write a value to the to LCD register                                         *
*   Parameter:    reg:    register to be written                               *
*                 val:    value to write to the register                       *
*******************************************************************************/

static __inline void wr_reg (unsigned char reg, unsigned short val) {

  wr_cmd(reg);
  wr_dat(val);
}

#if 0
/*******************************************************************************
* Read from the LCD register                                                   *
*   Parameter:    reg:    register to be read                                  *
*   Return:               value read from the register                         *
*******************************************************************************/

static unsigned short rd_reg (unsigned char reg) {

  wr_cmd(reg);
  return(rd_dat());
}
#endif

/************************ Exported functions **********************************/

/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

#define LCD_D0_D7       (0xFF)
#define LCD_RS          (PIO_PC19)
#define LCD_RD          (PIO_PC11)
#define LCD_WR          (PIO_PC8)
#define LCD_NCS1        (PIO_PD18)
#define LCD_BACKLIGHT   (PIO_PC13)

void GLCD_Init (void) {
  unsigned int mode, i;

/* Configure the LCD Control pins --------------------------------------------*/
  PMC->PMC_WPMR  = 0x504D4300;                  /* Disable write protect      */
  PMC->PMC_PCER1 = (1 << ID_PIOC) |             /* Enable clock for PIOC      */
                   (1 << ID_PIOD)  ;            /* Enable clock for PIOD      */

/* Configure the Data Bus pins -----------------------------------------------*/
  PIOC->PIO_PDR    =  (LCD_D0_D7);              /* Enable Peripheral function */
  PIOC->PIO_IDR    =  (LCD_D0_D7);              /* Disable interrupt          */
  PIOC->PIO_ABCDSR[0]  &= ~(LCD_D0_D7);         /* Select Peripheral A        */
  PIOC->PIO_ABCDSR[1]  &= ~(LCD_D0_D7);         /* Select Peripheral A        */

/* Configure the Control pins ------------------------------------------------*/
  PIOC->PIO_PDR    =  (LCD_RS);                 /* Enable Peripheral function */
  PIOC->PIO_IDR    =  (LCD_RS);                 /* Disable interrupt          */
  PIOC->PIO_ABCDSR[0] &= ~(LCD_RS);             /* Select Peripheral A        */
  PIOC->PIO_ABCDSR[1] &= ~(LCD_RS);             /* Select Peripheral A        */

  PIOC->PIO_PDR    =  (LCD_RD | LCD_WR);        /* Enable Peripheral function */
  PIOC->PIO_IDR    =  (LCD_RD | LCD_WR);        /* Disable interrupt          */
  PIOC->PIO_ABCDSR[0] &= ~(LCD_RD | LCD_WR);    /* Select Peripheral A        */
  PIOC->PIO_ABCDSR[1] &= ~(LCD_RD | LCD_WR);    /* Select Peripheral A        */

  PIOD->PIO_PDR    =  (LCD_NCS1);               /* Enable Peripheral function */
  PIOD->PIO_IDR    =  (LCD_NCS1);               /* Disable interrupt          */
  PIOD->PIO_ABCDSR[0] &= ~(LCD_NCS1);           /* Select Peripheral A        */
  PIOD->PIO_ABCDSR[1] &= ~(LCD_NCS1);           /* Select Peripheral A        */

/* Configure the Backlight pins ----------------------------------------------*/
  PIOC->PIO_IDR  =                              /* Disable interrupt          */
  PIOC->PIO_OER  =                              /* Enable output              */
  PIOC->PIO_PER  = (LCD_BACKLIGHT);             /* Disable Peripheral function*/
  PIOC->PIO_CODR = (LCD_BACKLIGHT);             /* Backlight off              */

/*-- SMC Configuration -------------------------------------------------------*/
/*--------------------------- CS2 --------------------------------------------*/
  PMC->PMC_PCER0 = (1 << ID_SMC);               /* Enable clock for SMC       */

  SMC->SMC_CS_NUMBER[1].SMC_SETUP = 0
                                 | ((  2 <<  0) & SMC_SETUP_NWE_SETUP_Msk)
                                 | ((  2 <<  8) & SMC_SETUP_NCS_WR_SETUP_Msk)
                                 | ((  2 << 16) & SMC_SETUP_NRD_SETUP_Msk)
                                 | ((  2 << 24) & SMC_SETUP_NCS_RD_SETUP_Msk)
                                 ;

  SMC->SMC_CS_NUMBER[1].SMC_PULSE = 0
                                 | (( 4 <<  0) & SMC_PULSE_NWE_PULSE_Msk)
                                 | (( 4 <<  8) & SMC_PULSE_NCS_WR_PULSE_Msk)
                                 | ((10 << 16) & SMC_PULSE_NRD_PULSE_Msk)
                                 | ((10 << 24) & SMC_PULSE_NCS_RD_PULSE_Msk)
                                 ;

  SMC->SMC_CS_NUMBER[1].SMC_CYCLE = 0
                                 | ((10 <<  0) & SMC_CYCLE_NWE_CYCLE_Msk)
                                 | ((22 << 16) & SMC_CYCLE_NRD_CYCLE_Msk)
                                 ;

  mode = SMC->SMC_CS_NUMBER[1].SMC_MODE;
  SMC->SMC_CS_NUMBER[1].SMC_MODE = (mode & ~(SMC_MODE_READ_MODE | SMC_MODE_WRITE_MODE))
                                 | (SMC_MODE_READ_MODE)
                                 | (SMC_MODE_WRITE_MODE) ;

  delay(5);                             /* Delay 50 ms                        */

  /* Start Initial Sequence --------------------------------------------------*/
  wr_reg(0x10, 0x0000);                 /* DSTB = LP = STB = 0                */
  wr_reg(0x00, 0x0001);                 /* Start internal OSC                 */
  wr_reg(0x01, 0x0100);                 /* Set SS and SM bit                  */
  wr_reg(0x02, 0x0700);                 /* Set 1 line inversion               */
  wr_reg(0x03, 0x1038);                 /* Set GRAM write direction and BGR=1 */
  wr_reg(0x04, 0x0000);                 /* Resize register                    */
  wr_reg(0x08, 0x0202);                 /* 2 lines each, back and front porch */
  wr_reg(0x09, 0x0000);                 /* Set non-disp area refresh cyc ISC  */
  wr_reg(0x0A, 0x0000);                 /* FMARK function                     */
  wr_reg(0x0C, 0x0000);                 /* RGB interface setting              */
  wr_reg(0x0D, 0x0000);                 /* Frame marker Position              */
  wr_reg(0x0F, 0x0000);                 /* RGB interface polarity             */

  /* Power On sequence -------------------------------------------------------*/
  wr_reg(0x10, 0x0000);                 /* Reset Power Control 1              */
  wr_reg(0x11, 0x0000);                 /* Reset Power Control 2              */
  wr_reg(0x12, 0x0000);                 /* Reset Power Control 3              */
  wr_reg(0x13, 0x0000);                 /* Reset Power Control 4              */
  delay(20);                            /* Discharge cap power voltage (200ms)*/
  wr_reg(0x10, 0x1290);                 /* SAP, BT[3:0], AP, DSTB, SLP, STB   */
  wr_reg(0x11, 0x0227);                 /* DC1[2:0], DC0[2:0], VC[2:0]        */
  delay(5);                             /* Delay 50 ms                        */
  wr_reg(0x12, 0x001B);                 /* Internal reference voltage= Vci    */
  delay(5);                             /* Delay 50 ms                        */
  wr_reg(0x13, 0x1100);                 /* Set VDV[4:0] for VCOM amplitude    */
  wr_reg(0x29, 0x0019);                 /* Set VCM[5:0] for VCOMH             */
  wr_reg(0x2B, 0x000D);                 /* Set Frame Rate                     */
  delay(5);                             /* Delay 50 ms                        */
  wr_reg(0x20, 0x0000);                 /* GRAM horizontal Address            */
  wr_reg(0x21, 0x0000);                 /* GRAM Vertical Address              */

  /* Adjust the Gamma Curve --------------------------------------------------*/
  wr_reg(0x30, 0x0000);
  wr_reg(0x31, 0x0204);
  wr_reg(0x32, 0x0200);
  wr_reg(0x35, 0x0007);
  wr_reg(0x36, 0x1404);
  wr_reg(0x37, 0x0705);
  wr_reg(0x38, 0x0305);
  wr_reg(0x39, 0x0707);
  wr_reg(0x3C, 0x0701);
  wr_reg(0x3D, 0x000E);

  /* Set GRAM area -----------------------------------------------------------*/
  wr_reg(0x50, 0x0000);                 /* Horizontal GRAM Start Address      */
  wr_reg(0x51, (HEIGHT-1));             /* Horizontal GRAM End   Address      */
  wr_reg(0x52, 0x0000);                 /* Vertical   GRAM Start Address      */
  wr_reg(0x53, (WIDTH-1));              /* Vertical   GRAM End   Address      */

  #if (LANDSCAPE == 1)
    wr_reg(0x60, 0x2700);               /* Gate Scan Line                     */
  #else
    wr_reg(0x60, 0xA700);               /* Gate Scan Line                     */
  #endif

  wr_reg(0x61, 0x0001);                 /* NDL,VLE, REV                       */
  wr_reg(0x6A, 0x0000);                 /* Set scrolling line                 */

  /* Partial Display Control -------------------------------------------------*/
  wr_reg(0x80, 0x0000);
  wr_reg(0x81, 0x0000);
  wr_reg(0x82, 0x0000);
  wr_reg(0x83, 0x0000);
  wr_reg(0x84, 0x0000);
  wr_reg(0x85, 0x0000);

  /* Panel Control -----------------------------------------------------------*/
  wr_reg(0x90, 0x0010);
  wr_reg(0x92, 0x0600);
  wr_reg(0x95, 0x0110);

  /* Set GRAM write direction
     I/D=11 (Horizontal : increment, Vertical : increment)                    */

#if (LANDSCAPE == 1)
  /* AM=1   (address is updated in vertical writing direction)                */
  wr_reg(0x03, 0x1038);
#else
  /* AM=0   (address is updated in horizontal writing direction)              */
  wr_reg(0x03, 0x1030);
#endif

  wr_reg(0x07, 0x0133);                 /* 262K color and display ON          */

  /* Turn backlight on with a certain backlight level                         */
  for (i = 0; i < 25 /* level */; i++) {
    PIOC->PIO_SODR = (LCD_BACKLIGHT);
    PIOC->PIO_SODR = (LCD_BACKLIGHT);
    PIOC->PIO_SODR = (LCD_BACKLIGHT);

    PIOC->PIO_CODR = (LCD_BACKLIGHT);
    PIOC->PIO_CODR = (LCD_BACKLIGHT);
    PIOC->PIO_CODR = (LCD_BACKLIGHT);
  }
  PIOC->PIO_SODR = (LCD_BACKLIGHT);
}


/*******************************************************************************
* Set draw window region                                                       *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        window width in pixel                            *
*                   h:        window height in pixels                          *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetWindow (unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
 #if (LANDSCAPE == 1)
  wr_reg(0x50, y);                      /* Vertical   GRAM Start Address      */
  wr_reg(0x51, y+h-1);                  /* Vertical   GRAM End   Address (-1) */
  wr_reg(0x52, x);                      /* Horizontal GRAM Start Address      */
  wr_reg(0x53, x+w-1);                  /* Horizontal GRAM End   Address (-1) */
  wr_reg(0x20, y);
  wr_reg(0x21, x);
 #else
  wr_reg(0x50, x);                      /* Horizontal GRAM Start Address      */
  wr_reg(0x51, x+w-1);                  /* Horizontal GRAM End   Address (-1) */
  wr_reg(0x52, y);                      /* Vertical   GRAM Start Address      */
  wr_reg(0x53, y+h-1);                  /* Vertical   GRAM End   Address (-1) */
  wr_reg(0x20, x);
  wr_reg(0x21, y);
 #endif
}


/*******************************************************************************
* Set draw window region to whole screen                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_WindowMax (void) {
  GLCD_SetWindow (0, 0, WIDTH, HEIGHT);
}


/*******************************************************************************
* Draw a pixel in foreground color                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_PutPixel (unsigned int x, unsigned int y) {

 #if (LANDSCAPE == 1)
  wr_reg(0x20, y);
  wr_reg(0x21, x);
 #else
  wr_reg(0x20, x);
  wr_reg(0x21, y);
 #endif

  wr_cmd(0x22);
  wr_dat(Color[TXT_COLOR]);
}


/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:      color:    foreground color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetTextColor (unsigned short color) {

  Color[TXT_COLOR] = color;
}


/*******************************************************************************
* Set background color                                                         *
*   Parameter:      color:    background color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetBackColor (unsigned short color) {

  Color[BG_COLOR] = color;
}


/*******************************************************************************
* Clear display                                                                *
*   Parameter:      color:    display clearing color                           *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Clear (unsigned short color) {
  unsigned int i;

  GLCD_WindowMax();
  wr_cmd(0x22);
  wr_dat_start();

  for(i = 0; i < (WIDTH*HEIGHT); i++)
    wr_dat_only(color);
  wr_dat_stop();
}


/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   cw:       character width in pixel                         *
*                   ch:       character height in pixels                       *
*                   c:        pointer to character bitmap                      *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DrawChar (unsigned int x, unsigned int y, unsigned int cw, unsigned int ch, unsigned char *c) {
  unsigned int i, j, k, pixs;

  GLCD_SetWindow(x, y, cw, ch);

  wr_cmd(0x22);
  wr_dat_start();

  k  = (cw + 7)/8;

  if (k == 1) {
    for (j = 0; j < ch; j++) {
      pixs = *(unsigned char  *)c;
      c += 1;

      for (i = 0; i < cw; i++) {
        wr_dat_only (Color[(pixs >> i) & 1]);
      }
    }
  }
  else if (k == 2) {
    for (j = 0; j < ch; j++) {
      pixs = *(unsigned short *)c;
      c += 2;

      for (i = 0; i < cw; i++) {
        wr_dat_only (Color[(pixs >> i) & 1]);
      }
    }
  }
  wr_dat_stop();
}


/*******************************************************************************
* Disply character on given line                                               *
*   Parameter:      ln:       line number                                      *
*                   col:      column number                                    *
*                   fi:       font index (0 = 6x8, 1 = 16x24)                  *
*                   c:        ascii character                                  *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DisplayChar (unsigned int ln, unsigned int col, unsigned char fi, unsigned char c) {

  c -= 32;
  switch (fi) {
    case 0:  /* Font 6 x 8 */
//      GLCD_DrawChar(col *  6, ln *  8,  6,  8, (unsigned char *)&Font_6x8_h  [c * 8]);
      break;
    case 1:  /* Font 16 x 24 */
//      GLCD_DrawChar(col * 16, ln * 24, 16, 24, (unsigned char *)&Font_16x24_h[c * 24]);
      break;
  }
}


/*******************************************************************************
* Disply string on given line                                                  *
*   Parameter:      ln:       line number                                      *
*                   col:      column number                                    *
*                   fi:       font index (0 = 6x8, 1 = 16x24)                  *
*                   s:        pointer to string                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_DisplayString (unsigned int ln, unsigned int col, unsigned char fi, unsigned char *s) {

  while (*s) {
    GLCD_DisplayChar(ln, col++, fi, *s++);
  }
}


/*******************************************************************************
* Clear given line                                                             *
*   Parameter:      ln:       line number                                      *
*                   fi:       font index (0 = 6x8, 1 = 16x24)                  *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_ClearLn (unsigned int ln, unsigned char fi) {
  unsigned char i;
  unsigned char buf[60];

  GLCD_WindowMax();
  switch (fi) {
    case 0:  /* Font 6 x 8 */
      for (i = 0; i < (WIDTH+5)/6; i++)
        buf[i] = ' ';
      buf[i+1] = 0;
      break;
    case 1:  /* Font 16 x 24 */
      for (i = 0; i < (WIDTH+15)/16; i++)
        buf[i] = ' ';
      buf[i+1] = 0;
      break;
  }
  GLCD_DisplayString (ln, 0, fi, buf);
}

/*******************************************************************************
* Draw bargraph                                                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        maximum width of bargraph (in pixels)            *
*                   h:        bargraph height                                  *
*                   val:      value of active bargraph (in 1/1024)             *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Bargraph (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int val) {
  int i,j;

  val = (val * w) >> 10;                /* Scale value                        */
  GLCD_SetWindow(x, y, w, h);
  wr_cmd(0x22);
  wr_dat_start();
  for (i = 0; i < h; i++) {
    for (j = 0; j <= w-1; j++) {
      if(j >= val) {
        wr_dat_only(Color[BG_COLOR]);
      } else {
        wr_dat_only(Color[TXT_COLOR]);
      }
    }
  }
  wr_dat_stop();
}


/*******************************************************************************
* Display graphical bitmap image at position x horizontally and y vertically   *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        width of bitmap                                  *
*                   h:        height of bitmap                                 *
*                   bitmap:   address at which the bitmap data resides         *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Bitmap (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap) {
  int i, j;
  unsigned short *bitmap_ptr = (unsigned short *)bitmap;

  GLCD_SetWindow (x, y, w, h);

  wr_cmd(0x22);
  wr_dat_start();
  for (i = (h-1)*w; i > -1; i -= w) {
    for (j = 0; j < w; j++) {
      wr_dat_only (bitmap_ptr[i+j]);
    }
  }
  wr_dat_stop();
}



/*******************************************************************************
* Scroll content of the whole display for dy pixels vertically                 *
*   Parameter:      dy:       number of pixels for vertical scroll             *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_ScrollVertical (unsigned int dy) {
#if (LANDSCAPE == 0)
  static unsigned int y = 0;

  y = y + dy;
  while (y >= HEIGHT)
    y -= HEIGHT;

  wr_reg(0x6A, y);
  wr_reg(0x61, 3);
#endif
}


/*******************************************************************************
* Write a command to the LCD controller                                        *
*   Parameter:      cmd:      command to write to the LCD                      *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_WrCmd (unsigned char cmd) {
  wr_cmd (cmd);
}


/*******************************************************************************
* Write a value into LCD controller register                                   *
*   Parameter:      reg:      lcd register address                             *
*                   val:      value to write into reg                          *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_WrReg (unsigned char reg, unsigned short val) {
  wr_reg (reg, val);
}
/******************************************************************************/
