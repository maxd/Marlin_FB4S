#include "screen_ready.h"

#include "../lib/lvgl/lvgl.h"
#include "../../../module/temperature.h"
#include "../../../module/mks_wifi/mks_wifi.h"

static uint8_t wifi_state=0xFF;

//Стили
static lv_style_t style_label;
static lv_style_t style_wifi_label;
static lv_style_t style_line;

//Надписи, которые меняются
static lv_obj_t * label_Extr_temp;
static lv_obj_t * label_Bed_temp;
static lv_obj_t * label_Fan_speed;

static lv_obj_t * label_wifi;

static void menu_btn_cb(lv_obj_t * btn, lv_event_t event);
static void file_btn_cb(lv_obj_t * btn, lv_event_t event);
static void init_styles(void);

void screen_ready_deinit(void){
    lv_style_reset(&style_label);
    lv_style_reset(&style_wifi_label);
    lv_style_reset(&style_line);
  
    lv_obj_clean((lv_obj_t *)screen);

    DEBUG("Ready deinit");
    mem_report();
}

void screen_ready_init(void){
    static lv_point_t line_points[] = { {0, 10}, {480, 10} };

    init_styles();

    lv_obj_t * menu_btn = lv_btn_create((lv_obj_t *)screen, NULL);
    lv_obj_set_pos(menu_btn, 10, 240);
    lv_obj_set_size(menu_btn, 120, 50);
    lv_obj_set_event_cb(menu_btn, menu_btn_cb);

    lv_obj_t * menu_label = lv_label_create(menu_btn, NULL);

    lv_obj_reset_style_list(menu_btn, LV_BTN_PART_MAIN);        
    
    lv_obj_add_style(menu_btn, LV_BTN_PART_MAIN, (lv_style_t *) &style_btn);   
    lv_label_set_text(menu_label, LV_SYMBOL_LIST" Menu");

    lv_obj_t * file_btn = lv_btn_create((lv_obj_t *)screen, NULL);    
    lv_obj_set_pos(file_btn, 180, 240);
    lv_obj_set_size(file_btn, 120, 50);
    lv_obj_set_event_cb(file_btn, file_btn_cb);

    lv_obj_t * file_label = lv_label_create(file_btn, NULL);         
    lv_label_set_text(file_label, LV_SYMBOL_SD_CARD" Files");                    

    lv_obj_reset_style_list(file_btn, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
    lv_obj_add_style(file_btn, LV_BTN_PART_MAIN, (lv_style_t*)&style_btn);

    lv_obj_t * img_extruder = lv_img_create((lv_obj_t *)screen, NULL); 
    lv_img_set_src(img_extruder, "S:/bmp_Ext1_state.bin");  
    lv_obj_set_pos(img_extruder, 50, 120);      

    lv_obj_t * img_bed = lv_img_create((lv_obj_t *)screen, NULL); 
    lv_img_set_src(img_bed, "S:/bmp_Bed_state.bin");  
    lv_obj_set_pos(img_bed, 200, 120);      

    lv_obj_t * img_fan_speed = lv_img_create((lv_obj_t *)screen, NULL); 
    lv_img_set_src(img_fan_speed, "S:/bmp_Fan_state.bin");  
    lv_obj_set_pos(img_fan_speed, 350, 120);      

    label_Extr_temp = lv_label_create((lv_obj_t *)screen, NULL);
    lv_obj_set_pos(label_Extr_temp, 30, 50);      
    lv_obj_add_style(label_Extr_temp, LV_LABEL_PART_MAIN, &style_label);
  
    label_Bed_temp = lv_label_create((lv_obj_t *)screen, NULL);
    lv_obj_set_pos(label_Bed_temp, 180, 50);      
    lv_obj_add_style(label_Bed_temp, LV_LABEL_PART_MAIN, &style_label);

    label_Fan_speed = lv_label_create((lv_obj_t *)screen, NULL);
    lv_obj_set_pos(label_Fan_speed, 340, 50);      
    lv_obj_add_style(label_Fan_speed, LV_LABEL_PART_MAIN, &style_label);

    lv_label_set_text_fmt(label_Extr_temp,"%d / %d",(int)thermalManager.temp_hotend[0].celsius, (int)thermalManager.temp_hotend[0].target);
    lv_label_set_text_fmt(label_Bed_temp,"%d / %d",(int)thermalManager.temp_bed.celsius, (int)thermalManager.temp_bed.target);
    lv_label_set_text_fmt(label_Fan_speed,"%d %%",thermalManager.fan_speed[0]*100/255);

    
    /*Create a line and apply the new style*/
    lv_obj_t * line1;
    line1 = lv_line_create((lv_obj_t *)screen, NULL);
    lv_line_set_points(line1, line_points, 2);     /*Set the points*/
    lv_obj_add_style(line1, LV_LINE_PART_MAIN, &style_line);     /*Set the points*/
    lv_obj_align(line1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    label_wifi = lv_label_create((lv_obj_t *)screen, NULL);
    lv_obj_set_pos(label_wifi, 5, 5);      
    lv_obj_add_style(label_wifi, LV_LABEL_PART_MAIN, &style_wifi_label);

    wifi_state=0xFF;

    DEBUG("Ready init");
    mem_report();
}


void screen_ready_update(void){
    static int extr_temp;
    static int bed_temp;
    static int extr_temp_target;
    static int bed_temp_target;
    static int fan_speed;


    if( (extr_temp != (int)thermalManager.temp_hotend[0].celsius) || (extr_temp_target != (int)thermalManager.temp_hotend[0].target)){
        extr_temp = (int)thermalManager.temp_hotend[0].celsius;
        extr_temp_target = (int)thermalManager.temp_hotend[0].target;
        lv_label_set_text_fmt(label_Extr_temp,"%d/%d",(int)thermalManager.temp_hotend[0].celsius, (int)thermalManager.temp_hotend[0].target);
    }

    if( (bed_temp != (int)thermalManager.temp_bed.celsius) || (bed_temp_target != (int)thermalManager.temp_bed.target)){
        bed_temp = (int)thermalManager.temp_bed.celsius;
        bed_temp_target = (int)thermalManager.temp_bed.target;
        lv_label_set_text_fmt(label_Bed_temp,"%d/%d",(int)thermalManager.temp_bed.celsius, (int)thermalManager.temp_bed.target);
    }

    if(fan_speed != thermalManager.fan_speed[0]){
        fan_speed = thermalManager.fan_speed[0];
        lv_label_set_text_fmt(label_Fan_speed,"%d %%",thermalManager.fan_speed[0]*100/255);
    }


    if(wifi_state != wifi_status.status){
        wifi_state = wifi_status.status;

        if(wifi_state == WIFI_READY){
            lv_label_set_text_fmt(label_wifi,"%s %s %s",LV_SYMBOL_WIFI,wifi_status.wifi_net,wifi_status.ip_addr);
        }else{
            lv_label_set_text(label_wifi,"WIFI: waiting");
        }
    }

}

static void menu_btn_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED) {
        DEBUG("Change screen to menu");
        tft_current_screen = TFT_MENU;
    }
}

static void file_btn_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED) {
        DEBUG("Change screen to file list");
        tft_current_screen = TFT_FILELIST;
    }
}


static void init_styles(void){

    lv_obj_add_style((lv_obj_t *)screen, LV_BTN_PART_MAIN, (lv_style_t*) &style_screen);
 
    lv_style_init(&style_label);
    lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&style_label, LV_STATE_DEFAULT, &lv_font_montserrat_40); 

    lv_style_init(&style_wifi_label);
    lv_style_set_text_color(&style_wifi_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&style_wifi_label, LV_STATE_DEFAULT, &lv_font_montserrat_16); 

    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 40);
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
    lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT, false);

}