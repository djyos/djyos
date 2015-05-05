#ifndef __CPU_PERI_LCD_TOUCH_H__
#define __CPU_PERI_LCD_TOUCH_H__

#ifdef __cplusplus
extern "C" {
#endif

//键盘窗口的大小
#define cn_key_board_width  320
#define cn_key_board_height 240
//LCD映射到窗口的大小
#define  X_SCALE             1
#define  Y_SCALE             1
#define cn_lcd_window_xsize  LCD_XSIZE*X_SCALE
#define cn_lcd_window_ysize  LCD_YSIZE*Y_SCALE

#define cn_lcd_line_size    (LCD_XSIZE*2)


#define cn_lcd_pf       CN_SYS_PF_RGB565

void __lcd_envid_of(int );
void __lcd_power_enable(int ,int );
void __lcd_hard_init(void);
bool_t __lcd_set_pixel_screen(s32 x,s32 y,u32 color,u32 rop2_code);
ptu32_t ModuleInstall_Lcd(ptu32_t para);
ptu32_t ModuleInstall_TouchWindows(ptu32_t para);

#ifdef __cplusplus
}
#endif

#endif   //__CPU_PERI_LCD_TOUCH_H__

