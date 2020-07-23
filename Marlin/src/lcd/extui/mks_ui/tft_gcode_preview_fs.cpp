#include "tft_gcode_preview_fs.h"
#include "../../sd/cardreader.h"
#include "../../../module/shared_mem/shared_mem.h"

struct work_buf_info mem_buff = {
  .buff = (uint8_t *)shared_mem,
  .index = 0xFFFFFFFF,
  .buff_size = WORK_BUF_SIZE,
  .start_position = 0,
  .end_position = 0
};

static struct gcode_image_info image;

static uint32_t convert_text_to_bin(uint8_t *work_buf,uint32_t work_buf_act_size, uint32_t im_width);

lv_fs_res_t tft_gcode_fs_open(lv_fs_drv_t * drv, void * file_p, const char * fn, lv_fs_mode_t mode){
    char fs_filename[105];

    CardReader::release();
    res = f_mount((FATFS *)&FATFS_Obj, "0", 1);
    
    strcpy(fs_filename,"0:/");
    strcpy(fs_filename+3,fn);
    fs_filename[strlen(fs_filename)-4]=0; //Убрать расширение .bin (нужно для LVGL)
    
    //DEBUG("FS open %s %s",fn,fs_filename);  
    res = f_open(&image.gcode,fs_filename,FA_READ);
    
    if(res != FR_OK){
        ERROR("Failed to open %s error %d",fs_filename,res);
        return LV_FS_RES_FS_ERR;
    }else{
      //  DEBUG("File open Ok");
    }

    image.start_offset = get_image_start(&image.gcode,(char *)";simage:",0);
    
    switch (image.start_offset) {
    case 0xFFFFFFFF:
      ERROR("simage pattern not found\n");
      return LV_FS_RES_FS_ERR;
      break;

    default:
      //DEBUG("simage offset %d \n",image.start_offset);
      break;
      }

    get_image_width(&image.gcode, &image);
    get_image_height(&image.gcode, &image);

    //DEBUG("simage %d x %d",image.im_width/4,image.im_height);

    //Gimage
    image.start_offset = get_image_start(&image.gcode,(char *)";gimage:",image.start_offset+(image.im_width+8+1)*image.im_height);
    switch (image.start_offset) {
    case 0xFFFFFFFF:
      ERROR("gimage pattern not found\n");
      return LV_FS_RES_FS_ERR;
      break;

    default:
      //DEBUG("gimage offset %d \n",image.start_offset);
      break;
      }

    get_image_width(&image.gcode, &image);
    get_image_height(&image.gcode, &image);

    //DEBUG("gimage %d x %d",image.im_width/4,image.im_height);

    image.bin_header = ((image.im_width/4) << 10) | (image.im_height << 21) | 4;
    image.read_position = 0;

    //Размер буфера кратный длине строки
    mem_buff.buff_size = ( (WORK_BUF_SIZE / (image.im_width + 8 + 1)) * (image.im_width + 8 + 1) );

    return LV_FS_RES_OK;
};

lv_fs_res_t tft_gcode_fs_close(lv_fs_drv_t * drv, void * file_p){

    //DEBUG("FS close");
    image.read_position = 0;
    f_close(&image.gcode);
    f_mount(0, "", 0);                   
    CardReader::mount();
    return LV_FS_RES_OK;
};


lv_fs_res_t tft_gcode_fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br){
    UINT bytes_read;
    uint32_t img_position = image.read_position-4;
    uint32_t line_number_start;
    uint32_t binary_bytes;
    uint32_t act_index;
    
    // DEBUG("Read %d bytes from %d (%d)",btr,image.read_position,img_position);

    //Заголовок изображения, 4 байта
    if( (image.read_position == 0) && (btr == 4) ){ 
        memcpy((uint8_t *)buf,(uint8_t *)&image.bin_header,4);
       *br=btr;
        return LV_FS_RES_OK;
    }

    if( (img_position < mem_buff.start_position) || ( (img_position+btr) > mem_buff.end_position) ){
      //Не весь дипазон в буфере (или весь не в буфере)
      // DEBUG("Data not buffered");

      //Номер строки, в которую попадает нужная позиция
      line_number_start = img_position / (image.im_width/2);

      // DEBUG("Line to read %d",line_number_start);

      f_lseek(&image.gcode,image.start_offset+line_number_start*(image.im_width + 8 + 1));
      f_read(&image.gcode,mem_buff.buff,mem_buff.buff_size,&bytes_read);

      binary_bytes=convert_text_to_bin(mem_buff.buff,mem_buff.buff_size,image.im_width);

      mem_buff.start_position = line_number_start * (image.im_width / 2);
      mem_buff.end_position = mem_buff.start_position + binary_bytes;

      // DEBUG("Now in buff from %d to %d",mem_buff.start_position,mem_buff.end_position);
    }

    act_index = img_position - mem_buff.start_position;
    // DEBUG("Copy from %d offset %d bytes",act_index,btr);
    memcpy((uint8_t *)buf, mem_buff.buff+act_index, btr);

    *br=btr;
    return LV_FS_RES_OK;
};


static uint32_t convert_text_to_bin(uint8_t *buf, uint32_t buf_size, uint32_t im_width){
  volatile uint8_t pixel;
  volatile uint32_t pixel_num;
  volatile uint32_t byte_num;
  
  volatile char *end;
  
  byte_num=0;
  pixel_num=0;

  end=(char *)((uint32_t)buf+buf_size);

  for(char *p=(char *)buf; p<end; p=p+2){

    if((byte_num >= 8) && (byte_num < ((im_width+8+1)-1))){
      pixel = (ascii2dec_test(p)<<4|ascii2dec_test(p+1));
      buf[pixel_num]=pixel;
      pixel_num++;
    }

    byte_num=byte_num+2;

    if(byte_num >= (im_width+8+1)){
      byte_num = 0;
      p=p-1;  //Сдвиг из-за не кратной 2 длины строки (809)
    }
    
  }

  return pixel_num;
}

lv_fs_res_t tft_gcode_fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos){
    //DEBUG("FS seek %d",pos);
    image.read_position = pos;
    return LV_FS_RES_OK;
};

lv_fs_res_t tft_gcode_fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p){
    //DEBUG("FS tell");
    *pos_p = image.read_position;
    return LV_FS_RES_OK;
};

int ascii2dec_test(char *ascii){
	int result = 0;

	if(ascii == 0)
		return 0;

	if(*(ascii) >= '0' && *(ascii) <= '9')
		result = *(ascii) - '0';
	else if(*(ascii) >= 'a' && *(ascii) <= 'f')
		result = *(ascii) - 'a' + 0x0a;
	else if(*(ascii) >= 'A' && *(ascii) <= 'F')
		result = *(ascii) - 'A' + 0x0a;
	else
		return 0;

	return result;
}

uint32_t get_image_start(FIL *gcode,char *pattern,uint32_t offset){
  char *pos;
  UINT bytes_read;

  f_lseek(gcode,offset);
  f_read(gcode,(uint8_t *)mem_buff.buff,1024,&bytes_read);
  pos=strstr((char *)mem_buff.buff,pattern);

  if(pos == NULL){
    return 0xFFFFFFFF;
  }else{
    return (uint32_t)(((uint8_t *)pos - (uint8_t *)mem_buff.buff))+offset;
  }
}

void get_image_width(FIL *gcode, struct gcode_image_info *image ){
  char *pos;
  UINT bytes_read;

  f_lseek(gcode,image->start_offset);
  memset((uint8_t *)mem_buff.buff,0,1024);
  f_read(gcode,(uint8_t *)mem_buff.buff,1024,&bytes_read);
  
  pos = (char *)mem_buff.buff + 8;

  image->im_width=0;
  while(*pos++ != 0x0D){
    image->im_width++;
    if(image->im_width > 1022){
      image->im_width=0;
      break;
    }
  }

}

void get_image_height(FIL *gcode, struct gcode_image_info *image ){
  UINT bytes_read;

  memset((uint8_t *)mem_buff.buff,0,1024);
  f_lseek(gcode,image->start_offset+image->im_width+8+1); //8 символов simage + 0x0d
  
  image->im_height = 0;
  do{
    f_read(gcode,(uint8_t *)mem_buff.buff,image->im_width+8+1,&bytes_read);
    
    if(!strncmp((char *)mem_buff.buff,"M10086 ;",8)){
      image->im_height++;
    }else{
      break;
    }
  }while(1);
}
