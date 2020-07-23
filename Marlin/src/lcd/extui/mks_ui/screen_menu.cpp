#include "screen_menu.h"
#include "../lib/lvgl/lvgl.h"

static void init_styles(void);
static void btn_event_cb(lv_obj_t * btn, lv_event_t event);

void screen_menu_deinit(void){
    lv_obj_clean((lv_obj_t *)screen);
    DEBUG("Menu deinit");
    mem_report();
};

void screen_menu_init(void){
 
    init_styles();

    lv_obj_t * menu_btn = lv_btn_create((lv_obj_t *)screen, NULL);    
    lv_obj_set_pos(menu_btn, 10, 200);                           
    lv_obj_set_size(menu_btn, 120, 50);                         
    lv_obj_set_event_cb(menu_btn, btn_event_cb);                

    lv_obj_reset_style_list(menu_btn, LV_BTN_PART_MAIN);
    lv_obj_add_style(menu_btn, LV_BTN_PART_MAIN, (lv_style_t *)&style_btn);

    DEBUG("Menu init");
    mem_report();
};


static void btn_event_cb(lv_obj_t * btn, lv_event_t event){
    if(event == LV_EVENT_RELEASED) {
        DEBUG("Change screen to ready");
        tft_current_screen = TFT_READY;
    }
}


static void init_styles(void){
    lv_obj_add_style((lv_obj_t *)screen, LV_BTN_PART_MAIN, (lv_style_t *)&style_screen);
}