#ifndef _BUTTOM_DETECT_H_
#define _BUTTOM_DETECT_H_

#include "stm32f1xx_hal.h"

#define BUTTOM_DETECT_CYCLE_MS              (10)
#define BUTTOM_OVER_DETECT_TIMES            (3)

#define BUTTOM_LONG_PRESS_TIME_MS           (1000)  //1000ms = 1s      
#define BUTTOM_DOUBLE_PRESS_MAX_DELAY_MS    (100)

#define BUTTOM_ACTION_UP            (0x00)
#define BUTTOM_ACTION_DOWN          (0x01)
#define BUTTOM_ACTION_NONE          (0x02)
// flag for detect
#define BUTTOM_KEY_ON_OFF_F         ((uint32_t)0x0001)
#define BUTTOM_KEY_UP_F             ((uint32_t)0x0002)
#define BUTTOM_KEY_DOWN_F           ((uint32_t)0x0004)
#define BUTTOM_KEY_MODESWITCH_F     ((uint32_t)0x0008)

// offset
#define BUTTOM_F_OFFSET_LENTH       ((uint32_t)0x4)
#define BUTTOM_F_OFFSET_P           ((uint32_t)0x0)
#define BUTTOM_F_OFFSET_LP          ((uint32_t)0x1)
#define BUTTOM_F_OFFSET_DP          ((uint32_t)0x2)

// flag for porcess
#define BUTTOM_KEY_ON_OFF_PRESS_F   ((uint32_t)0x0001)   
#define BUTTOM_KEY_UP_PRESS_F       ((uint32_t)0x0002)
#define BUTTOM_KEY_DOWN_PRESS_F     ((uint32_t)0x0004)      

#define BUTTOM_SM_IS_STATE(st)      ((st)>=buttom_sm_start && (st)<buttom_sm_end)

typedef enum {
    buttom_sm_start = 0,
    buttom_sm_key_dowm_1,
    buttom_sm_key_up_1,        //up_1 + delay = BUTTOM_PRESS   or  up_1 + long down = BUTTOM_LPRESS
    buttom_sm_key_down_2,
    buttom_sm_key_up_2,         //up_2 = BUTTOM_DPRESS
    buttom_sm_end
}buttom_state_machine_t;

typedef enum {
    BUTTOM_PRESS = 0,
    BUTTOM_DPRESS,              //double press
    BUTTOM_LPRESS,              //long press
    BUTTOM_LPRESS_OVER,
    //on way state
    BUTTOM_NOTSURE,             //need more information
    BUTTOM_ERR                  //error state
}buttom_detect_t;

typedef struct {
    GPIO_TypeDef* gpio  ;
    uint16_t gpio_pin   ;

    buttom_state_machine_t mid_state;
    uint32_t buttom_flag        ;
    uint32_t state_start_tick   ;
    
    char on_use     ;
    char count_time ;
    char high_times ;
}buttom_msg_t;

typedef struct{
    uint32_t    key_f;
    buttom_detect_t buttom_type;
}buttom_msg_pass_t;


void buttom_init(buttom_msg_t* buttom, GPIO_TypeDef* gpio, uint16_t gpio_pin, uint32_t buttom_flag);
buttom_detect_t buttom_state(buttom_msg_t* buttom, int action);

#endif
