#include "tft_fs.h"

#include "../../../HAL/STM32F1/w25q64.h"
#include "../../../HAL/STM32F1/small_spi.h"

static volatile uint32_t current_pos = 0;
struct image_info current_image;

lv_fs_res_t tft_fs_open(lv_fs_drv_t * drv, void * file_p, const char * fn, lv_fs_mode_t mode){
    //DEBUG("FS open");

    SPI2->CR1 = SPI_CR1_SSM|\
                SPI_CR1_SSI|\
                (SPI_HI_SPEED << SPI_CR1_BR_Pos)|\
                SPI_CR1_SPE|\
                SPI_CR1_MSTR;

    current_image=(struct image_info)get_img_offset(fn);

    SPI2->CR1 = SPI_CR1_SSM|\
                SPI_CR1_SSI|\
                (SPI_LOW_SPEED << SPI_CR1_BR_Pos)|\
                SPI_CR1_SPE|\
                SPI_CR1_MSTR;


    if(current_image.image_number == 0xFF){
        return LV_FS_RES_NOT_EX;
    }
    
    current_pos=0;
    return LV_FS_RES_OK;
};

lv_fs_res_t tft_fs_close(lv_fs_drv_t * drv, void * file_p){
    current_pos=0;
   

    //DEBUG("FS close");
    return LV_FS_RES_OK;
};

lv_fs_res_t tft_fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br){
    uint32_t img_address = PIC_DATA_ADDR_TFT35 + current_image.image_number * PER_PIC_MAX_SPACE_TFT35;

    img_address+=current_pos;

    // if( (current_pos+btr) > bmp_bin_len){
    //     return LV_FS_RES_UNKNOWN;
    // }
    SPI2->CR1 = SPI_CR1_SSM|\
                SPI_CR1_SSI|\
                (SPI_FREQ_18Mhz << SPI_CR1_BR_Pos)|\
                SPI_CR1_SPE|\
                SPI_CR1_MSTR;


    w25q_read(img_address,(uint8_t *)buf,btr);

    SPI2->CR1 = SPI_CR1_SSM|\
                SPI_CR1_SSI|\
                (SPI_LOW_SPEED << SPI_CR1_BR_Pos)|\
                SPI_CR1_SPE|\
                SPI_CR1_MSTR;

    //memcpy(buf,bmp_bin+current_pos,btr);
    *br=btr;
    return LV_FS_RES_OK;
};

lv_fs_res_t tft_fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos){
    current_pos = pos;
    //DEBUG("FS seek %d",pos);
    return LV_FS_RES_OK;
};

lv_fs_res_t tft_fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p){
    //DEBUG("FS tell");
    *pos_p = current_pos;
    return LV_FS_RES_OK;
};

struct image_info get_img_offset(const char *filename){
    struct image_info img_inf {0xFF,00};
    uint8_t total_images;
    char img_filename[PIC_NAME_MAX_LEN];
    char *char_ptr;
    uint32_t read_offset;
    //uint32_t img_address;
    uint32_t img_size_address;

    //DEBUG("File: %s",filename);

    w25q_read(PIC_COUNTER_ADDR,&total_images,1);

    if(total_images == 0xFF){
        total_images = 0;
    }

	//DEBUG("Total images %d",total_images);
    
    read_offset=0;
    for(uint32_t i = 0; i < total_images; i++){
        char_ptr=img_filename;
        do{
            w25q_read(PIC_NAME_ADDR+read_offset,(uint8_t *)char_ptr,1);
            read_offset++;
        }
        while( *char_ptr++ != '\0');	

        if(!strcmp((char *)img_filename,(char *)filename)){
            //DEBUG("File %s found: %d",filename,i);
            img_inf.image_number = i;
            break;
        }
    }
 
    if(img_inf.image_number == 0xFF){
        ERROR("No image found");
        return img_inf;
    }

    //img_address = PIC_DATA_ADDR_TFT35 + img_inf.image_number * PER_PIC_MAX_SPACE_TFT35;
    
  	img_size_address = PIC_SIZE_ADDR + 4 * img_inf.image_number;
	w25q_read(img_size_address,(uint8_t *)&img_inf.image_size,4);
    
    return img_inf;
}