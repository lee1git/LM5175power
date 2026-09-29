#include "screen.h"

#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stddef.h"

u8g2_t u8g2;

void screen_init()
{
    u8g2Init(&u8g2);
}

void screen_clear_buffer()
{
    u8g2_ClearBuffer(&u8g2);
}

void screen_set_small_font()        //6x10
{
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
}

int screen_printf(u8g2_uint_t x, u8g2_uint_t y, const char*fmt, ...)
{
    char buf[128];
    int res;
    va_list args;
    va_start(args, fmt);
    res = vsnprintf(buf,sizeof(buf),fmt,args);
    va_end(args);

    res = (int)u8g2_DrawStr(&u8g2,x,y,buf);
    return res;
}

void screen_send_buffer()
{
	u8g2_SendBuffer(&u8g2);
}

//
void screen_set_data_print(struct PowerStatus_t* PowerState)
{
    if(PowerState->control_mode == PM_CONTROL_MODE_VOLTAGE){
      screen_printf(0, 10, "Vset :%05.2fV", PowerState->set_voltage); u8g2_DrawStr(&u8g2, 80, 10, "<");
      screen_printf(0, 18, "Ilmt :%05.2fA", PowerState->set_current);
    }else if(PowerState->control_mode == PM_CONTROL_MODE_CURRENT){
      screen_printf(0, 10, "Vlmt :%05.2fV", PowerState->set_voltage); 
      screen_printf(0, 18, "Iset :%05.2fA", PowerState->set_current); u8g2_DrawStr(&u8g2, 80, 18, "<");
    }
}

void screen_real_data_print(float voltage, float current, float temperature)
{
    screen_printf(0, 30, "Vout :%05.2fV", voltage);
    screen_printf(0, 38, "Iout :%05.2fA", current);
    screen_printf(0, 50, "T :%03.1fC", temperature);
}

