// =============================================================================
// Copyright (C) 2012-2020 长园继保自动化有限公司 All Rights Reserved
// 文件名     ：LCD.c
// 模块描述: LCD驱动程序，使用了SMC外设
// 模块版本: V1.00
// 创建人员: HM
// 创建时间: 07/03.2016
// =============================================================================

#ifndef __LCD_H__
#define __LCD_H__


void ModuleInstall_LCD(void);
void LCD_Clr(void);

void LCD_WriteByte(uint8_t dat,uint8_t lcd_write_mode);
void LCD_SetBackLight(uint8_t byCtrl);
void Lcd_wrDISPram(uint32_t dwleft,uint32_t dwtop,uint32_t dwsize,uint8_t * pbysrcadd);

#endif
