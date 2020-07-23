#include "screen_filelist.h"

#include "../../libs/fatfs/fatfs_shared.h"
#include "../../sd/cardreader.h"

char current_gcode_filename[100];

//Стили
static lv_style_t style_filelist;
static lv_obj_t * file_list;
static lv_obj_t * up_btn;
static lv_obj_t * down_btn;

uint32_t file_list_index=0;
uint32_t total_files=0;

static void up_btn_cb(lv_obj_t * btn, lv_event_t event);
static void down_btn_cb(lv_obj_t * btn, lv_event_t event);
static void ret_btn_cb(lv_obj_t * btn, lv_event_t event);
static void btn_event_cb(lv_obj_t * obj, lv_event_t event);

static void init_styles(void);
static void read_files(uint32_t start, uint32_t count);

void screen_filelist_deinit(void){
    lv_style_reset(&style_filelist);
    lv_obj_clean((lv_obj_t *)screen);
    DEBUG("Filelist deinit");
    mem_report();
}

void screen_filelist_init(void){
       
    init_styles();

    file_list = lv_list_create((lv_obj_t *)screen, NULL);
    lv_obj_add_style(file_list, LV_BTN_PART_MAIN, &style_filelist);
    lv_obj_set_size(file_list, 350, 300);
    lv_obj_align(file_list, NULL, LV_ALIGN_IN_LEFT_MID, 10, 10);
    
    
    up_btn = lv_btn_create((lv_obj_t *)screen, NULL);    
    lv_obj_set_pos(up_btn, 380, 10);
    lv_obj_set_size(up_btn, 100, 70);
    lv_obj_set_event_cb(up_btn, up_btn_cb);                

    lv_obj_t * up_label = lv_label_create(up_btn, NULL);         
    lv_label_set_text(up_label, LV_SYMBOL_UP " Up");                    

    down_btn = lv_btn_create((lv_obj_t *)screen, NULL);
    lv_obj_set_pos(down_btn, 380, 100);
    lv_obj_set_size(down_btn, 100, 70);
    lv_obj_set_event_cb(down_btn, down_btn_cb);

    lv_obj_t * down_label = lv_label_create(down_btn, NULL);         
    lv_label_set_text(down_label, LV_SYMBOL_DOWN " Down");
    lv_obj_align(down_label, NULL, LV_ALIGN_CENTER, 0, 0);


    lv_obj_t * ret_btn = lv_btn_create((lv_obj_t *)screen, NULL);    
    lv_obj_set_pos(ret_btn, 380, 250);
    lv_obj_set_size(ret_btn, 100, 70);
    lv_obj_set_event_cb(ret_btn, ret_btn_cb);                

    lv_obj_t * ret_label = lv_label_create(ret_btn, NULL);
    lv_label_set_text(ret_label, LV_SYMBOL_NEW_LINE " Back");

    lv_obj_reset_style_list(up_btn, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
    lv_obj_add_style(up_btn, LV_BTN_PART_MAIN, (lv_style_t *) &style_btn);
    
    lv_obj_reset_style_list(down_btn, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
    lv_obj_add_style(down_btn, LV_BTN_PART_MAIN, (lv_style_t *) &style_btn);
    
    lv_obj_reset_style_list(ret_btn, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
    lv_obj_add_style(ret_btn, LV_BTN_PART_MAIN, (lv_style_t *) &style_btn);

    lv_btn_set_state(up_btn,LV_BTN_STATE_DISABLED);
    lv_obj_set_click(up_btn,false);

    read_files(0,FILE_LIST_COUNT);

    DEBUG("Filelist init");
    mem_report();
}


static void ret_btn_cb(lv_obj_t * btn, lv_event_t event){
    if(event == LV_EVENT_RELEASED) {
        tft_current_screen = TFT_READY;
    }
}

static void down_btn_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED) {
        //DEBUG("idx %d total %d",(file_list_index + FILE_LIST_COUNT),total_files);
        if((file_list_index + FILE_LIST_COUNT) < total_files){
            file_list_index++;
            //DEBUG("Down idx: %d",file_list_index);
            lv_list_clean(file_list);
            read_files(file_list_index,FILE_LIST_COUNT);

            lv_btn_set_state(up_btn,LV_STATE_DEFAULT);
            lv_obj_set_click(up_btn,true);
        }else{
            lv_btn_set_state(down_btn,LV_BTN_STATE_DISABLED);
            lv_obj_set_click(down_btn,false);
        }
    }
}


static void up_btn_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_RELEASED) {
        if(file_list_index > 0) {
            file_list_index--;
        }else{
            lv_btn_set_state(up_btn,LV_BTN_STATE_DISABLED);
            lv_obj_set_click(up_btn,false);
        }
        
        if((file_list_index + FILE_LIST_COUNT) < total_files){
            lv_btn_set_state(down_btn,LV_STATE_DEFAULT);
            lv_obj_set_click(down_btn,true);
        }
        
        //DEBUG("Up idx: %d",file_list_index);
        lv_list_clean(file_list);
        read_files(file_list_index,FILE_LIST_COUNT);

    }
}

static void btn_event_cb(lv_obj_t * obj, lv_event_t event){
    if(event == LV_EVENT_CLICKED) {
        //DEBUG("Clicked: %s\n", lv_list_get_btn_text(obj));
        strcpy(current_gcode_filename,lv_list_get_btn_text(obj));
        tft_current_screen = TFT_FILE;
    }
}


static void init_styles(void){
    lv_obj_add_style((lv_obj_t *)screen, LV_BTN_PART_MAIN, (lv_style_t *)&style_screen);
    lv_style_init(&style_filelist);
    lv_style_set_text_font(&style_filelist, LV_STATE_DEFAULT, &arial_20);    
}


static void read_files(uint32_t start, uint32_t count){
    FRESULT res;
    uint32_t cnt=0;
    lv_obj_t * list_btn;

    CardReader::release();
    res = f_mount((FATFS *)&FATFS_Obj, "0", 1);
    //DEBUG("SD init result:%d",res);

    res = f_opendir(&dir, "0:");                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
           
            if( (cnt >= start) && ((cnt - start) < count)){
                //DEBUG("Add %s\n", fno.fname);
                list_btn = lv_list_add_btn(file_list, LV_SYMBOL_FILE, fno.fname);
                lv_obj_set_event_cb(list_btn, btn_event_cb);
                
            }
                cnt++;
            }
       }else{
          ERROR("Opendir error %d",res);
      }
    total_files = cnt;
   f_closedir(&dir);

   f_mount(0, "", 0);                   
   CardReader::mount();
}