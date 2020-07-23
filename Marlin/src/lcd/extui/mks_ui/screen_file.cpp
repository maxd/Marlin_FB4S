#include "screen_file.h"

#include "../../module/shared_mem/check_stack.h"

//Стили

static lv_obj_t * print_btn;
static lv_obj_t * print_label;

static void ret_btn_cb(lv_obj_t * btn, lv_event_t event);
static void init_styles(void);

void screen_file_deinit(void){

    lv_obj_clean((lv_obj_t *)screen);
    current_gcode_filename[0]=0;

    DEBUG("File deinit");
    mem_report();
}

void screen_file_init(void){
    char fs_filename[105];
 
    init_styles();

    print_btn = lv_btn_create((lv_obj_t *)screen, NULL);    
    lv_obj_set_pos(print_btn, 380, 10);
    lv_obj_set_size(print_btn, 100, 70);
    //lv_obj_set_event_cb(up_btn, up_btn_cb);                

    print_label = lv_label_create(print_btn, NULL);         
    lv_label_set_text(print_label, LV_SYMBOL_PLAY " Print");                    


    lv_obj_t * ret_btn = lv_btn_create((lv_obj_t *)screen, NULL);    
    lv_obj_set_pos(ret_btn, 380, 250);
    lv_obj_set_size(ret_btn, 100, 70);
    lv_obj_set_event_cb(ret_btn, ret_btn_cb);                

    lv_obj_t * ret_label = lv_label_create(ret_btn, NULL);
    lv_label_set_text(ret_label, LV_SYMBOL_NEW_LINE " Back");

    lv_obj_reset_style_list(print_btn, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
    lv_obj_add_style(print_btn, LV_BTN_PART_MAIN, (lv_style_t *)&style_btn);
    
    lv_obj_reset_style_list(ret_btn, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
    lv_obj_add_style(ret_btn, LV_BTN_PART_MAIN, (lv_style_t *)&style_btn);

    strcpy(fs_filename,"G:/");
    strcpy(fs_filename+3,current_gcode_filename);
    strcpy(fs_filename+strlen(fs_filename),".bin");
    
    lv_obj_t * gcode_img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(gcode_img, fs_filename);  
    lv_obj_align(gcode_img, NULL, LV_ALIGN_IN_LEFT_MID, 10, 10);
    
    DEBUG("File init");
    mem_report();
}


static void ret_btn_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED) {
        tft_current_screen = TFT_FILELIST;
    }
}


static void init_styles(void){
    lv_obj_add_style((lv_obj_t *)screen, LV_BTN_PART_MAIN, (lv_style_t *) &style_screen);
}
