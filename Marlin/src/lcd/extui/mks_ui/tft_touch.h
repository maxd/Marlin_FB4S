#ifndef TFT_TOUCH_H
#define TFT_TOUCH_H

#include "../lib/lvgl/lvgl.h"
#include "../../../MarlinCore.h"
#include "tft_hal.h"
#include "../../../HAL/HAL.h"
#include "../../../module/mks_wifi/small_cmsis.h"
#include "../../../module/mks_wifi/dwt.h"
#include "../../../HAL/STM32F1/small_spi.h"


#define TOUCH_COUNT 4
#define ADC_VALID_OFFSET	10

#define	CHX 	0x90
#define	CHY 	0xD0

#ifndef USE_XPT2046
#define USE_XPT2046         1
#endif

#define XPT2046_XY_SWAP  	1
#define XPT2046_X_MIN       201 
#define XPT2046_Y_MIN       164
#define XPT2046_X_MAX       3919
#define XPT2046_Y_MAX       3776
#define XPT2046_HOR_RES     480
#define XPT2046_VER_RES     320
#define XPT2046_X_INV   	0
#define XPT2046_Y_INV   	1

void touch_init(void);

void xpt2046_read(uint16_t *x,uint16_t *y);
bool xpt2046_get_data(lv_coord_t *x, lv_coord_t*y);
uint8_t	xpt2046_data_valid(uint16_t raw_x, uint16_t raw_y);

bool lcd_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data);



#endif