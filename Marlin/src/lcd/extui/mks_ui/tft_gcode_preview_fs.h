#ifndef TFT_GCODE_FS_H
#define TFT_GCODE_FS_H

#include "tft_init.h"
#include "../lib/lvgl/lvgl.h"
#include "../../libs/fatfs/fatfs_shared.h"

#define WORK_BUF_SIZE   (uint32_t)4*1024

struct gcode_image_info{
	uint32_t start_offset;
	uint32_t im_width;
    uint32_t im_height;
    FIL      gcode;
    uint32_t bin_header;
    uint32_t read_position;
};

struct work_buf_info{
	uint8_t *buff;
    uint32_t index;
	uint32_t buff_size;
    uint32_t start_position;
    uint32_t end_position;
};


lv_fs_res_t tft_gcode_fs_open(lv_fs_drv_t * drv, void * file_p, const char * fn, lv_fs_mode_t mode);
lv_fs_res_t tft_gcode_fs_close(lv_fs_drv_t * drv, void * file_p);
lv_fs_res_t tft_gcode_fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
lv_fs_res_t tft_gcode_fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos);
lv_fs_res_t tft_gcode_fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

int ascii2dec_test(char *ascii);
uint32_t get_image_start(FIL *gcode,char *pattern,uint32_t offset);
void get_image_width(FIL *gcode, struct gcode_image_info *image );
void get_image_height(FIL *gcode, struct gcode_image_info *image );

#endif