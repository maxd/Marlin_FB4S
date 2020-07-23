#ifndef SCREEN_FILELIST_H
#define SCREEN_FILELIST_H

#include "tft_init.h"

#define FILE_LIST_COUNT 6


extern char current_gcode_filename[100];


void screen_filelist_deinit(void);
void screen_filelist_init(void);


#endif