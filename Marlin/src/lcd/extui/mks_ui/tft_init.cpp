#include "tft_init.h"

#include "../../../gcode/gcode.h"
#include "../../../HAL/STM32F1/w25q64.h"


volatile uint32_t tft_current_screen=TFT_READY;
volatile uint32_t last_screen=0xFFFFFFFF;

typedef void (*pfn_screen_func)(void);
pfn_screen_func screen_upd_func[LCD_SCREEN_COUNT];
pfn_screen_func screen_init_func[LCD_SCREEN_COUNT];
pfn_screen_func screen_deinit_func[LCD_SCREEN_COUNT];

static lv_disp_buf_t disp_buf;
#ifdef FB_DMA_IRQ
static lv_color_t __attribute__ ((aligned (4))) buf_1[(LV_HOR_RES_MAX * LVGL_BUF_COUNT) / 2];
static lv_color_t __attribute__ ((aligned (4))) buf_2[(LV_HOR_RES_MAX * LVGL_BUF_COUNT) / 2];
volatile lv_disp_drv_t * current_disp;
#else
static lv_color_t __attribute__ ((aligned (4))) lcd_buf[LV_HOR_RES_MAX * LVGL_BUF_COUNT];
#endif

volatile lv_style_t style_btn;
volatile lv_style_t style_screen;

volatile lv_obj_t * screen;

void lcd_disp_write(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p);

void SysTick_Callback(){
     lv_tick_inc(1);
}

void mem_report(void){
    lv_mem_monitor_t mon;

    lv_mem_monitor(&mon);
    DEBUG("Total: %d free: %d max: %d used: %d%%",mon.total_size,mon.free_size,mon.max_used,mon.used_pct);
}


void mks_lcd_start(void){
    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;
    lv_fs_drv_t fs_drv;
    lv_fs_drv_t gcode_fs_drv;
    typedef  FILE * spi_file_t;

    

    init_tft();
    w25q_init();
    touch_init();
    lv_init();	
    systick_attach_callback(SysTick_Callback);
    
    lv_log_register_print_cb(my_log_cb);

    /*Initialize the display buffer*/
    #ifdef FB_DMA_IRQ
    lv_disp_buf_init(&disp_buf, buf_1, buf_2, (LV_HOR_RES_MAX * LVGL_BUF_COUNT) / 2);
    #else
    lv_disp_buf_init(&disp_buf, lcd_buf, NULL, LV_HOR_RES_MAX * LVGL_BUF_COUNT);    
    #endif

    //Display driver
    lv_disp_drv_init(&disp_drv);          
    disp_drv.flush_cb = lcd_disp_write;    
    disp_drv.buffer = &disp_buf;          
    lv_disp_drv_register(&disp_drv);     
    
    //Touch driver
    lv_indev_drv_init(&indev_drv);             
    indev_drv.type = LV_INDEV_TYPE_POINTER;    
    indev_drv.read_cb = lcd_touchpad_read;     
    lv_indev_drv_register(&indev_drv);         

    //File system (SPI flash) driver
    lv_fs_drv_init(&fs_drv);                     
    fs_drv.file_size = sizeof(spi_file_t);
    fs_drv.letter = 'S';                         /*An uppercase letter to identify the drive */
    fs_drv.open_cb = tft_fs_open;                 /*Callback to open a file */
    fs_drv.close_cb = tft_fs_close;               /*Callback to close a file */
    fs_drv.read_cb = tft_fs_read;                 /*Callback to read a file */
    fs_drv.seek_cb = tft_fs_seek;                 /*Callback to seek in a file (Move cursor) */
    fs_drv.tell_cb = tft_fs_tell;                 /*Callback to tell the cursor position  */
    lv_fs_drv_register(&fs_drv);                 /*Finally register the drive*/


    //File system (G-code preview) driver
    lv_fs_drv_init(&gcode_fs_drv);                     
    gcode_fs_drv.file_size = sizeof(spi_file_t);
    gcode_fs_drv.letter = 'G';                         /*An uppercase letter to identify the drive */
    gcode_fs_drv.open_cb = tft_gcode_fs_open;                 /*Callback to open a file */
    gcode_fs_drv.close_cb = tft_gcode_fs_close;               /*Callback to close a file */
    gcode_fs_drv.read_cb = tft_gcode_fs_read;                 /*Callback to read a file */
    gcode_fs_drv.seek_cb = tft_gcode_fs_seek;                 /*Callback to seek in a file (Move cursor) */
    gcode_fs_drv.tell_cb = tft_gcode_fs_tell;                 /*Callback to tell the cursor position  */
    lv_fs_drv_register(&gcode_fs_drv);                 /*Finally register the drive*/

    screen = lv_obj_create(NULL, NULL);
    lv_scr_load((lv_obj_t *)screen);
    
    DEBUG("LVGL init");
    mem_report();

    tft_current_screen = TFT_READY;
 
    screen_upd_func[TFT_READY] = screen_ready_update;
    screen_init_func[TFT_READY] = screen_ready_init;
    screen_deinit_func[TFT_READY] = screen_ready_deinit;

    screen_upd_func[TFT_MENU] = NULL;
    screen_init_func[TFT_MENU] = screen_menu_init;
    screen_deinit_func[TFT_MENU] = screen_menu_deinit;

    screen_upd_func[TFT_KILL]  = NULL;
    screen_init_func[TFT_KILL] = screen_kill_init;
    screen_deinit_func[TFT_KILL] = screen_kill_deinit;

    screen_upd_func[TFT_FILELIST]  = NULL;
    screen_init_func[TFT_FILELIST] = screen_filelist_init;
    screen_deinit_func[TFT_FILELIST] = screen_filelist_deinit;

    screen_upd_func[TFT_FILE]  = NULL;
    screen_init_func[TFT_FILE] = screen_file_init;
    screen_deinit_func[TFT_FILE] = screen_file_deinit;

    init_default_styles();

    #ifdef FB_OUT_DMA
        #ifdef FB_DMA_IRQ
        nvic_irq_enable(NVIC_DMA2_CH3);
        #endif
        DMA2_Channel3->CMAR = LCD_DATA;
    #endif
}

void lcd_disp_write(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p){
    uint16_t width,height;

    width  = area->x2 - area->x1 + 1;
    height = area->y2 - area->y1 + 1;

    ili9320_SetWindows((uint16_t)area->x1,(uint16_t)area->y1,width,height);
    LCD_WriteRAM_Prepare;

#ifndef FB_OUT_DMA
    for(uint32_t i=0; i<width*height; i++){
        LCD_WriteData( *(uint16_t*)color_p++);
    }
#else
    #ifdef FB_DMA_IRQ
        DMA2_Channel3->CCR =  DMA_CCR_MEM2MEM | DMA_CCR_PINC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0 | DMA_CCR_TCIE | DMA_CCR_TEIE;
        current_disp = disp;
    #else
        DMA2_Channel3->CCR =  DMA_CCR_MEM2MEM | DMA_CCR_PINC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0;
    #endif

    DMA2_Channel3->CPAR = (uint32_t )color_p;
    DMA2_Channel3->CNDTR = width*height;
    DMA2->IFCR = DMA_IFCR_CGIF3|DMA_IFCR_CTEIF3|DMA_IFCR_CHTIF3|DMA_IFCR_CTCIF3;

    DMA2_Channel3->CCR |= DMA_CCR_EN;

    #ifndef FB_DMA_IRQ
        while (!(DMA2->ISR & (DMA_ISR_TCIF3|DMA_ISR_TEIF3))){};
        
        if(DMA2->ISR & DMA_ISR_TEIF3){
            ERROR("LCD DMA Error");
        }
        DMA2_Channel3->CCR = 0;
        DMA2->IFCR = DMA_IFCR_CGIF3|DMA_IFCR_CTEIF3|DMA_IFCR_CHTIF3|DMA_IFCR_CTCIF3;
    #endif
#endif

    #ifndef FB_DMA_IRQ
    lv_disp_flush_ready((lv_disp_drv_t *)disp);
    #endif

}

#ifdef FB_DMA_IRQ
extern "C" void __irq_dma2_channel3(void) {
    if ((DMA2->ISR & DMA_ISR_GIF3)) {
        DMA2_Channel3->CCR = 0;
        if(DMA2->ISR & DMA_ISR_TEIF3){
            ERROR("LCD DMA Error");
        }
        DMA2->IFCR = DMA_IFCR_CGIF3|DMA_IFCR_CTEIF3|DMA_IFCR_CHTIF3|DMA_IFCR_CTCIF3;
        lv_disp_flush_ready((lv_disp_drv_t *)current_disp);       
    }
}
#endif

void GcodeSuite::M0_M1(){

}

void tft_update(void){

    if(last_screen != tft_current_screen){
       //DEBUG("Change screen");
        //deinit current screen
        if(last_screen != 0xFFFFFFFF){
            if(screen_deinit_func[last_screen] != NULL){
                //DEBUG("Call deinit for %d",last_screen);
                screen_deinit_func[last_screen]();
            }
        }
        //init new screen
        if(screen_init_func[tft_current_screen] != NULL){
            //DEBUG("Call init for %d",tft_current_screen);
            screen_init_func[tft_current_screen]();
        }

        last_screen = tft_current_screen;
    }else{
        if(screen_upd_func[tft_current_screen] != NULL){
            screen_upd_func[tft_current_screen]();
        }
    }
    
    lv_task_handler();
}

void my_log_cb(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char *dsc){
  /*Send the logs via serial port*/
  if(level == LV_LOG_LEVEL_ERROR) {
    ERROR("%s:%d %s %s",file,line,fn_name,dsc);
      };
  if(level == LV_LOG_LEVEL_WARN){
    WARNING("%s:%d %s %s",file,line,fn_name,dsc);
  } 
  if(level == LV_LOG_LEVEL_INFO) {
    INFO("%s:%d %s %s",file,line,fn_name,dsc);
  }
  if(level == LV_LOG_LEVEL_TRACE){
    DEBUG("%s:%d %s %s",file,line,fn_name,dsc);
  }

}

void tft_kill_screen(void){

    tft_current_screen = TFT_KILL;
    tft_update();

    while(1){
        lv_task_handler();
        delay_us(10);
        lv_tick_inc(10);
    }
}


void init_default_styles(void){

    lv_style_init((lv_style_t *) &style_screen);
    lv_style_set_bg_color((lv_style_t *)&style_screen, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    lv_style_init((lv_style_t *)&style_btn);
    lv_style_set_radius((lv_style_t *)&style_btn, LV_STATE_DEFAULT, 5);
    lv_style_set_bg_color((lv_style_t *)&style_btn, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_opa((lv_style_t *)&style_btn, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_border_color((lv_style_t *)&style_btn, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_border_color((lv_style_t *)&style_btn, LV_STATE_DISABLED, LV_COLOR_GRAY);
    lv_style_set_border_opa((lv_style_t *)&style_btn, LV_STATE_DEFAULT, LV_OPA_70);
    lv_style_set_border_width((lv_style_t *)&style_btn, LV_STATE_DEFAULT, 2);
   
    lv_style_set_text_color((lv_style_t *)&style_btn, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_color((lv_style_t *)&style_btn, LV_STATE_DISABLED, LV_COLOR_GRAY);
    
    lv_style_set_transform_height((lv_style_t *)&style_btn, LV_STATE_PRESSED, -5);
    lv_style_set_transform_width((lv_style_t *)&style_btn, LV_STATE_PRESSED, -10);

    lv_style_set_text_font((lv_style_t *)&style_btn, LV_STATE_DEFAULT, &lv_font_montserrat_16);
}