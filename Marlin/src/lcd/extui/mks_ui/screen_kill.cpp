#include "screen_kill.h"

static void init_styles(void);
static void btn_event_cb(lv_obj_t * btn, lv_event_t event);

static lv_style_t style_label;

void screen_kill_deinit(void){
    
    lv_style_reset(&style_label);
    lv_obj_clean((lv_obj_t *)screen);
};

void screen_kill_init(void){
    lv_obj_t * label_Error;
    
    DEBUG("Kill init");

    init_styles();

    label_Error = lv_label_create((lv_obj_t *)screen, NULL);
    lv_obj_add_style(label_Error, LV_LABEL_PART_MAIN, &style_label);
    lv_label_set_text(label_Error,"System error");
    lv_label_set_align(label_Error, LV_LABEL_ALIGN_CENTER);   
    
    lv_obj_t * reset_btn = lv_btn_create((lv_obj_t *)screen, NULL);    
    lv_obj_align(reset_btn, NULL, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_event_cb(reset_btn, btn_event_cb);                
    lv_obj_reset_style_list(reset_btn, LV_BTN_PART_MAIN);
    lv_obj_add_style(reset_btn, LV_BTN_PART_MAIN,(lv_style_t *) &style_btn);

    lv_obj_t * reset_label = lv_label_create(reset_btn, NULL);         
    lv_label_set_text(reset_label, "Reboot");                    

};

static void init_styles(void){

    lv_obj_add_style((lv_obj_t *)screen, LV_BTN_PART_MAIN,(lv_style_t *) &style_screen);

    lv_style_init(&style_label);
    lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&style_label, LV_STATE_DEFAULT, &lv_font_montserrat_40); 
}

static void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED) {
        DEBUG("Reboot");
        nvic_sys_reset();
    }
}

