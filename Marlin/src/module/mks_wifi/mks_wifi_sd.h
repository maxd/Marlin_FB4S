#ifndef MKS_WIFI_SD_H
#define MKS_WIFI_SD_H

#define DMA_TIMEOUT 0xFffffff

#include "mks_wifi.h"
#include "../../sd/cardreader.h"
#include "small_cmsis.h"

#include "../shared_mem/shared_mem.h"

#define ESP_PACKET_SIZE     1024
#define ESP_FILE_BUFF_COUNT 4

extern volatile uint8_t *dma_buff1;
extern volatile uint8_t *dma_buff2;

void mks_wifi_sd_ls(void);

void mks_wifi_sd_ls(void);

void mks_wifi_sd_init(void);
void mks_wifi_sd_deinit(void);
void sd_delete_file(char *filename);

void mks_wifi_start_file_upload(ESP_PROTOC_FRAME *packet);

#endif