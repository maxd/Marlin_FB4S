#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <stdio.h>

#define SHARED_MEM_SIZE 6*1024

extern volatile uint8_t shared_mem[SHARED_MEM_SIZE];

#endif


/*
Использование памяти:

Wifi:
*mks_in_buffer  [1024] [end]
*mks_out_buffer [1024] [-1024]
esp_packet     [1024]  [0]

file_buff [4*1024] [0]
dma_buff1 [1024]   [-2048]
dma_buff2 [1024]   [-1024]

Gcode preview
mem_buff  [4]      [0]

spi_eeprom [2048] [0]

*/
