#ifndef _POWERMASTER_H_

#include "main.h"

#define PWR_EN_ON       0x00
#define PWR_EN_OFF      0x01

struct PowerStatus_t
{
    float set_voltage;
    float set_current;
    float now_voltaga;
    float now_current;

    float now_temperature;

    int en_statu;
    int pgood_statu;
};

extern struct PowerStatus_t PowerState;

int PowerStateInit(int _en_sta,float _set_vol,float _set_cur);

#endif // !_POWERMASTER_H_