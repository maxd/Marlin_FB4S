#include "tft_hal.h"
#include "../lib/lvgl/lvgl.h"
#include <libmaple/fsmc.h>


void init_tft(void){
	uint16_t i;
    uint16_t DeviceCode;

    LCD_IO_Init();
	
	_delay_ms(5);
	
	LCD_WriteReg(0X00D3);				   
	DeviceCode=LCD_IO_ReadRAM;	//dummy read 	
	DeviceCode=LCD_IO_ReadRAM;	
	DeviceCode=LCD_IO_ReadRAM;   								   
	DeviceCode<<=8;
	DeviceCode|=LCD_IO_ReadRAM;  	
	
	DEBUG("LCD device code: %0X",DeviceCode);
    
    if(DeviceCode == 0x9488)
	{
		LCD_WriteReg(0x00E0); 
		LCD_WriteData(0x0000); 
		LCD_WriteData(0x0007); 
		LCD_WriteData(0x000f); 
		LCD_WriteData(0x000D); 
		LCD_WriteData(0x001B); 
		LCD_WriteData(0x000A); 
		LCD_WriteData(0x003c); 
		LCD_WriteData(0x0078); 
		LCD_WriteData(0x004A); 
		LCD_WriteData(0x0007); 
		LCD_WriteData(0x000E); 
		LCD_WriteData(0x0009); 
		LCD_WriteData(0x001B); 
		LCD_WriteData(0x001e); 
		LCD_WriteData(0x000f);  

		LCD_WriteReg(0x00E1); 
		LCD_WriteData(0x0000); 
		LCD_WriteData(0x0022); 
		LCD_WriteData(0x0024); 
		LCD_WriteData(0x0006); 
		LCD_WriteData(0x0012); 
		LCD_WriteData(0x0007); 
		LCD_WriteData(0x0036); 
		LCD_WriteData(0x0047); 
		LCD_WriteData(0x0047); 
		LCD_WriteData(0x0006); 
		LCD_WriteData(0x000a); 
		LCD_WriteData(0x0007); 
		LCD_WriteData(0x0030); 
		LCD_WriteData(0x0037); 
		LCD_WriteData(0x000f); 

		LCD_WriteReg(0x00C0); 
		LCD_WriteData(0x0010); 
		LCD_WriteData(0x0010); 

		LCD_WriteReg(0x00C1); 
		LCD_WriteData(0x0041); 

		LCD_WriteReg(0x00C5); 
		LCD_WriteData(0x0000); 
		LCD_WriteData(0x0022); 
		LCD_WriteData(0x0080); 

		LCD_WriteReg(0x0036); 
        LCD_WriteData(0x0068); 

		LCD_WriteReg(0x003A); //Interface Mode Control
		LCD_WriteData(0x0055);

		LCD_WriteReg(0X00B0);  //Interface Mode Control  
		LCD_WriteData(0x0000); 
		LCD_WriteReg(0x00B1);   //Frame rate 70HZ  
		LCD_WriteData(0x00B0); 
		LCD_WriteData(0x0011); 
		LCD_WriteReg(0x00B4); 
		LCD_WriteData(0x0002);   
		LCD_WriteReg(0x00B6); //RGB/MCU Interface Control
		LCD_WriteData(0x0002); 
		LCD_WriteData(0x0042); 

		LCD_WriteReg(0x00B7); 
		LCD_WriteData(0x00C6); 

		LCD_WriteReg(0x00E9); 
		LCD_WriteData(0x0000);

		LCD_WriteReg(0X00F7);    
		LCD_WriteData(0x00A9); 
		LCD_WriteData(0x0051); 
		LCD_WriteData(0x002C); 
		LCD_WriteData(0x0082);

		LCD_WriteReg(0x0011); 

		for(i=0;i<65535;i++);

		LCD_WriteReg(0x0029); 	

		ili9320_SetWindows(0,0,480,320);
		LCD_Clear(0x0000);
		
        OUT_WRITE(LCD_BACKLIGHT_PIN, HIGH);
	}

}


void LCD_IO_Init(void){
  static uint8_t fsmc_init_flag = 0;  

  if (fsmc_init_flag) return;
  fsmc_init_flag = 1;
    
  rcc_clk_enable(RCC_FSMC);

  gpio_set_mode(GPIOD, 14, GPIO_AF_OUTPUT_PP);  // FSMC_D00
  gpio_set_mode(GPIOD, 15, GPIO_AF_OUTPUT_PP);  // FSMC_D01
  gpio_set_mode(GPIOD,  0, GPIO_AF_OUTPUT_PP);  // FSMC_D02
  gpio_set_mode(GPIOD,  1, GPIO_AF_OUTPUT_PP);  // FSMC_D03
  gpio_set_mode(GPIOE,  7, GPIO_AF_OUTPUT_PP);  // FSMC_D04
  gpio_set_mode(GPIOE,  8, GPIO_AF_OUTPUT_PP);  // FSMC_D05
  gpio_set_mode(GPIOE,  9, GPIO_AF_OUTPUT_PP);  // FSMC_D06
  gpio_set_mode(GPIOE, 10, GPIO_AF_OUTPUT_PP);  // FSMC_D07
  gpio_set_mode(GPIOE, 11, GPIO_AF_OUTPUT_PP);  // FSMC_D08
  gpio_set_mode(GPIOE, 12, GPIO_AF_OUTPUT_PP);  // FSMC_D09
  gpio_set_mode(GPIOE, 13, GPIO_AF_OUTPUT_PP);  // FSMC_D10
  gpio_set_mode(GPIOE, 14, GPIO_AF_OUTPUT_PP);  // FSMC_D11
  gpio_set_mode(GPIOE, 15, GPIO_AF_OUTPUT_PP);  // FSMC_D12
  gpio_set_mode(GPIOD,  8, GPIO_AF_OUTPUT_PP);  // FSMC_D13
  gpio_set_mode(GPIOD,  9, GPIO_AF_OUTPUT_PP);  // FSMC_D14
  gpio_set_mode(GPIOD, 10, GPIO_AF_OUTPUT_PP);  // FSMC_D15

  gpio_set_mode(GPIOD,  4, GPIO_AF_OUTPUT_PP);  // FSMC_NOE
  gpio_set_mode(GPIOD,  5, GPIO_AF_OUTPUT_PP);  // FSMC_NWE

  gpio_set_mode(PIN_MAP[FSMC_CS_PIN].gpio_device, PIN_MAP[FSMC_CS_PIN].gpio_bit, GPIO_AF_OUTPUT_PP);  //FSMC_CS_NEx
  gpio_set_mode(PIN_MAP[FSMC_RS_PIN].gpio_device, PIN_MAP[FSMC_RS_PIN].gpio_bit, GPIO_AF_OUTPUT_PP);  //FSMC_RS_Ax

  FSMC_NOR_PSRAM1_BASE->BCR = FSMC_BCR_WREN | FSMC_BCR_MTYP_SRAM | FSMC_BCR_MWID_16BITS | FSMC_BCR_MBKEN;
  FSMC_NOR_PSRAM1_BASE->BTR = (FSMC_DATA_SETUP_TIME << 8) | FSMC_ADDRESS_SETUP_TIME;

  afio_remap(AFIO_REMAP_FSMC_NADV);
}




void ili9320_SetWindows(uint16_t StartX,uint16_t StartY,uint16_t width,uint16_t heigh){
	uint16_t s_h,s_l, e_h, e_l;

    s_h = (StartX >> 8) & 0X00ff;
    s_l = StartX & 0X00ff;
    e_h = ((StartX + width - 1) >> 8) & 0X00ff;
    e_l = (StartX + width - 1) & 0X00ff;
    
    LCD_WriteReg(0x002A);
    LCD_WriteData(s_h);
    LCD_WriteData(s_l);
    LCD_WriteData(e_h);
    LCD_WriteData(e_l);

    s_h = (StartY >> 8) & 0X00ff;
    s_l = StartY & 0X00ff;
    e_h = ((StartY + heigh - 1) >> 8) & 0X00ff;
    e_l = (StartY + heigh - 1) & 0X00ff;
    
    LCD_WriteReg(0x002B);
    LCD_WriteData(s_h);
    LCD_WriteData(s_l);
    LCD_WriteData(e_h);
    LCD_WriteData(e_l);		
}

void LCD_Clear(uint16_t Color){
    uint32_t index=0;

    tft_set_cursor(0,0);
    ili9320_SetWindows(0,0,480,320);
    LCD_WriteRAM_Prepare;
    
    for(index=0; index<320*480; index++){
        LCD_WriteData(Color);
    }

}

void tft_set_cursor(uint16_t x,uint16_t y){
    LCD_WriteReg(0X002A); 
    LCD_WriteData(x>>8); 
    LCD_WriteData(x&0X00FF); 
    LCD_WriteData(x>>8); 
    LCD_WriteData(x&0X00FF);			
    LCD_WriteReg(0X002B); 
    LCD_WriteData(y>>8); 
    LCD_WriteData(y&0X00FF);
    LCD_WriteData(y>>8); 
    LCD_WriteData(y&0X00FF);			
}


