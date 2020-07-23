#ifndef TFT_FS_H
#define TFT_FS_H
#include <stdint.h>
#include "../lib/lvgl/lvgl.h"
#include "../../../MarlinCore.h"

#define PIC_NAME_ADDR			0x003000	//pic information addr
#define PIC_SIZE_ADDR			0x007000	//pic size information addr
#define PIC_COUNTER_ADDR		0x008000	//pic total number
#define PER_PIC_SAVE_ADDR		0x009000	//Storage address of each picture
#define PIC_LOGO_ADDR			0x00A000	//logo addr


#define DEFAULT_VIEW_ADDR_TFT35  0XC5800
#define BAK_VIEW_ADDR_TFT35		 (DEFAULT_VIEW_ADDR_TFT35+90*1024)
#define PIC_ICON_LOGO_ADDR_TFT35 (BAK_VIEW_ADDR_TFT35+80*1024)
#define PIC_DATA_ADDR_TFT35		 (PIC_ICON_LOGO_ADDR_TFT35+350*1024)

#define PER_PIC_MAX_SPACE_TFT35		(32*1024)	

#define PIC_NAME_MAX_LEN		50	//Picture name maximum length

struct image_info{
	uint8_t image_number;
	uint32_t image_size;
};


lv_fs_res_t tft_fs_open(lv_fs_drv_t * drv, void * file_p, const char * fn, lv_fs_mode_t mode);
lv_fs_res_t tft_fs_close(lv_fs_drv_t * drv, void * file_p);
lv_fs_res_t tft_fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
lv_fs_res_t tft_fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos);
lv_fs_res_t tft_fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

struct image_info get_img_offset(const char *filename);

#endif