#include "buttom_detect.h"

#include "FreeRTOS.h"
#include "task.h"
#include "assert.h"
#include "uart_debug.h"


void buttom_init(buttom_msg_t* buttom)
{
    buttom->mid_state = buttom_sm_start;
    buttom->state_start_tick = 0;
}

buttom_detect_t buttom_state(buttom_msg_t* buttom, int action)
{
    assert(BUTTOM_SM_IS_STATE(buttom->mid_state));

    switch (buttom->mid_state)
    {
    case buttom_sm_start:
        if(action == BUTTOM_ACTION_DOWN){
            buttom->mid_state = buttom_sm_key_dowm_1;
            buttom->state_start_tick = xTaskGetTickCount();
            return BUTTOM_NOTSURE;
        }else if(action == BUTTOM_ACTION_UP){
            buttom->mid_state = buttom_sm_start;
            return BUTTOM_ERR;
        }else{
            return BUTTOM_NOTSURE;
        }
        break;
    case buttom_sm_key_dowm_1:
             UART_Printf("down1\r\n");
        if(action == BUTTOM_ACTION_DOWN){
            buttom->mid_state = buttom_sm_start;
            buttom->state_start_tick = 0;
            return BUTTOM_ERR;
        }else if(action == BUTTOM_ACTION_UP){
            buttom->mid_state = buttom_sm_key_up_1;
            buttom->state_start_tick = xTaskGetTickCount();
            return BUTTOM_NOTSURE;
        }else{
            if((xTaskGetTickCount() - buttom->state_start_tick) >= pdMS_TO_TICKS(BUTTOM_LONG_PRESS_TIME_MS))
            {
                buttom->mid_state = buttom_sm_start;
                buttom->state_start_tick = 0;
                return BUTTOM_LPRESS;               //long press
            }
            return BUTTOM_NOTSURE;
        }
        break;
    case buttom_sm_key_up_1:
        if(action == BUTTOM_ACTION_DOWN){
            // buttom->mid_state = buttom_sm_key_down_2;
            buttom->mid_state = buttom_sm_start;        //double press not realize
            buttom->state_start_tick = xTaskGetTickCount();
            // return BUTTOM_NOTSURE;
            return BUTTOM_ERR;
        }else if(action == BUTTOM_ACTION_UP){
            buttom->mid_state = buttom_sm_start;
            buttom->state_start_tick = 0;
            return BUTTOM_ERR;
        }else{
            if((xTaskGetTickCount() - buttom->state_start_tick) >= pdMS_TO_TICKS(BUTTOM_DOUBLE_PRESS_MAX_DELAY_MS)){
                buttom->mid_state = buttom_sm_start;
                buttom->state_start_tick = 0;
                return BUTTOM_PRESS;                //press once
            }
            return BUTTOM_NOTSURE;
        }
        break;

    default://empty
        break;
    }
}
