#include "tft_touch.h"

uint16_t x_addata[TOUCH_COUNT],y_addata[TOUCH_COUNT];

static void xpt2046_corr(uint16_t * x, uint16_t * y);
static uint16_t xpt2046_cmd_get(uint8_t cmd);


void touch_init(void){
	spi2_init(SPI_FREQ_1_125Mhz);
}


bool lcd_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data){
  static lv_coord_t last_x = 0;
  static lv_coord_t last_y = 0;

    /*Save the state and save the pressed coordinate*/
    data->state = xpt2046_get_data(&last_x,&last_y) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    /*Set the coordinates (if released use the last pressed coordinates)*/
    data->point.x = last_x;
    data->point.y = last_y;

    return false; /*Return `false` because we are not buffering and no more data to read*/
}


bool xpt2046_get_data(lv_coord_t *x, lv_coord_t*y){
	lv_coord_t x_data = 0;
    lv_coord_t y_data = 0;

	xpt2046_read((uint16_t *)&x_data, (uint16_t *)&y_data);

	if(xpt2046_data_valid(x_data, y_data)){
		xpt2046_corr((uint16_t *)&x_data, (uint16_t *)&y_data);
		*x = x_data;
		*y = y_data;
		//DEBUG("Touch: %d %d",x_data,y_data);
		return true;
	}else{
		return false;
	}
};

void xpt2046_read(uint16_t *x,uint16_t *y){
	uint32_t total_x = 0;
	uint32_t total_y = 0;
	uint8_t good_x = 0;
	uint8_t good_y = 0;
	uint16_t val;

	for(uint32_t i=0; i < TOUCH_COUNT; i++){
		val = xpt2046_cmd_get(CHX);
		if(val > 0){
			total_y += val;
			good_y++;
		}
	
		val = xpt2046_cmd_get(CHY);
		if(val > 0){
			total_x += val;
			good_x++;
		}
	}

	if(good_x > 0){
		*x = total_x / good_x;
	}else{
		*x = 0;
	}
	
	if(good_y > 0){
		*y = total_y / good_y;
	}else{
		*y = 0;
	}
}

static uint16_t xpt2046_cmd_get(uint8_t cmd){
	uint16_t ret_val;

	TOUCH_CS_START;
	spi_send(cmd);
	ret_val = spi_send(0xFF);
	ret_val = ret_val << 8;
	ret_val |= spi_send(0xFF);
	ret_val >>= 3;
	
	TOUCH_CS_STOP;
	return ret_val & 0x0FFF;
}

uint8_t	xpt2046_data_valid(uint16_t raw_x, uint16_t raw_y){
	if ((   raw_x <= ADC_VALID_OFFSET) 
	    || (raw_y <= ADC_VALID_OFFSET)
		|| (raw_x >= 4095 - ADC_VALID_OFFSET)
		|| (raw_y >= 4095 - ADC_VALID_OFFSET))
	{
		return 0;
	}
	else{
		return 1;
	}
}

static void xpt2046_corr(uint16_t * x, uint16_t * y){
#if XPT2046_XY_SWAP     
	int16_t swap_tmp;    
	swap_tmp = *x;    
	*x = *y;    
	*y = swap_tmp;
#endif    
	if((*x) > XPT2046_X_MIN)
		(*x) -= XPT2046_X_MIN;    
	else
		(*x) = 0;    
	if((*y) > XPT2046_Y_MIN)
		(*y) -= XPT2046_Y_MIN;    
	else
		(*y) = 0;    
	(*x) = (uint32_t)((uint32_t)(*x) * XPT2046_HOR_RES)/(XPT2046_X_MAX - XPT2046_X_MIN);    
	(*y) = (uint32_t)((uint32_t)(*y) * XPT2046_VER_RES)/(XPT2046_Y_MAX - XPT2046_Y_MIN);
#if XPT2046_X_INV     
	(*x) =  XPT2046_HOR_RES - (*x);
#endif
#if XPT2046_Y_INV     
	(*y) =  XPT2046_VER_RES - (*y);
#endif
}
