#include "check_stack.h"


uint32_t check_stack_size(void){
    uint32_t ret_val = 0;
    volatile uint32_t value;
    volatile uint32_t ptr;
    
    //0x2000AFD4 0x2000FFD8


    for(ptr = 0x2000FFD8; ptr > 0x2000AFD4; ptr=ptr-4 ){
        
        value = *(uint32_t*)ptr;
        
        if( value == 0x6b6b6b6b){
            break;
        }

        ret_val++;
    }

    return ret_val*4;
};