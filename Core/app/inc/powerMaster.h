#ifndef _POWERMASTER_H_
#define _POWERMASTER_H_

#include "main.h"
#include "cmsis_os.h"

#define PWR_EN_ON       0x00
#define PWR_EN_OFF      0x01

#define DEVICE_ONLINE     (0x01)
#define DEVICE_OFFLINE    (0x00)

struct PowerStatus_t
{
    float set_voltage;
    float set_current;
    float now_voltage;
    float now_current;

    float now_temperature;

    TickType_t last_update_time_voltage;
    TickType_t last_update_time_current;
    TickType_t last_update_time_temperature;

    char I2C1_state;
    char INA226_state;
    char TMP112_state;

    char I2C2_state;
    char SH1106_state;

    int en_statu;
    int pgood_statu;
};

extern struct PowerStatus_t PowerState;

int PowerStateInit(int _en_sta,float _set_vol,float _set_cur);

int PowerState_lock(void);
int PowerState_unlock(void);

#endif // !_POWERMASTER_H_
