#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "stm32_u8g2.h"
#include "powerMaster.h"

extern u8g2_t u8g2;

void screen_init();
void screen_clear_buffer();
void screen_set_small_font();        //6x10
int screen_printf(u8g2_uint_t x, u8g2_uint_t y, const char*fmt, ...);
void screen_send_buffer();

//
void screen_set_data_print(struct PowerStatus_t* PowerState);
void screen_real_data_print(struct PowerStatus_t* PowerState);

#endif
