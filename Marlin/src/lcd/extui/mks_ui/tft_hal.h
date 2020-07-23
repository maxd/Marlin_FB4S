#ifndef TFT_HAL_H
#define TFT_HAL_H

#include "../../../MarlinCore.h"

__attribute__((always_inline)) inline void __DSB() {
  __asm volatile ("dsb 0xF":::"memory");
}

#define TICK_CYCLE 1

/* Timing configuration */
#define FSMC_ADDRESS_SETUP_TIME   15  // AddressSetupTime
#define FSMC_DATA_SETUP_TIME      15  // DataSetupTime

//#define LCD_WriteCmd(cmd)   { *(volatile uint16_t *) (LCD_REG) = cmd; delay_us(1); }
//#define LCD_WriteData(data)     { *(volatile uint16_t *) (LCD_DATA) = data; }

#define LCD_REG     (uint32_t)0x6001FFFE
#define LCD_DATA    (uint32_t)0x60020000

#define LCD_WriteReg(Reg)       do{ *(volatile uint16_t *) (LCD_REG) = Reg;  __DSB();}while(0)
#define LCD_WriteData(Data)     do{ *(volatile uint16_t *) (LCD_DATA) = Data;  __DSB();}while(0)
#define LCD_IO_ReadRAM          *(volatile uint16_t *) (LCD_DATA)

#define LCD_WriteRAM_Prepare    LCD_WriteReg(0x002C)


typedef struct {
  __IO uint16_t REG;
  __IO uint16_t RAM;
} LCD_CONTROLLER_TypeDef;

void LCD_IO_Init(void);
void init_tft(void);

void LCD_Clear(uint16_t Color);
void tft_set_cursor(uint16_t x,uint16_t y);
void ili9320_SetWindows(uint16_t StartX,uint16_t StartY,uint16_t width,uint16_t heigh);


#endif