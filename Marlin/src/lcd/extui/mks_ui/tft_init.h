#ifndef TFT_INIT_H
#define TFT_INIT_H

#include "../lib/lvgl/lvgl.h"
#include "../../../MarlinCore.h"
#include "tft_hal.h"
#include "tft_touch.h"
#include "tft_fs.h"
#include "tft_gcode_preview_fs.h"

#include "screen_ready.h"
#include "screen_menu.h"
#include "screen_kill.h"
#include "screen_filelist.h"
#include "screen_file.h"

#define LVGL_BUF_COUNT     22

#define FB_OUT_DMA
//#define FB_DMA_IRQ  1

extern volatile lv_style_t style_btn;
extern volatile lv_style_t style_screen;
extern volatile lv_obj_t * screen;

enum tft_screen{
    TFT_READY, 
    TFT_MENU,
    TFT_KILL,
    TFT_FILELIST,
    TFT_FILE,
    LIST_END
};

#define LCD_SCREEN_COUNT    LIST_END

extern volatile uint32_t tft_current_screen;

void SysTick_Callback();
void mks_lcd_start(void);
void tft_kill_screen(void);

void tft_update(void);

void init_default_styles(void);
void mem_report(void);

void my_log_cb(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char *dsc);

uint32_t getTickDiff(uint32_t curTick, uint32_t lastTick);


void M0_M1(void);
//void lcd_pause_show_message(const PauseMessage message, const PauseMode mode=PAUSE_MODE_SAME, const uint8_t extruder=active_extruder);

#endif