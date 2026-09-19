#include "powerMaster.h"
#include <assert.h>     //assert() is a macro: without this it links as an undefined symbol

struct PowerStatus_t PowerState;

//weak function for mutex lock/unlock, to be overridden by application layer
//0 = ok, non-zero = lock not acquired / release failed
__weak int PowerState_lock(void)   { return 0; }
__weak int PowerState_unlock(void) { return 0; }

/**
 * @brief 
 * 
 * @param _en_sta   output state when init
 * @param _set_vol  output voltage  (V)
 * @param _set_cur  output current  (A)
 * @return int 
 */

int PowerStateInit(int _en_sta,float _set_vol,float _set_cur)
{
    assert(_en_sta == PWR_EN_OFF || _en_sta == PWR_EN_ON);
    assert(_set_vol >= 1.0f && _set_vol < 15.5f);
    assert(_set_cur > 0.0f && _set_cur <= 10.0f);

    PowerState.en_statu = _en_sta;
    PowerState.pgood_statu = 1;  //not updated yet, assume good
    
    PowerState.now_current = 0.0f;
    PowerState.now_voltage = 0.0f;
    PowerState.now_temperature = 0.0f;

    PowerState.set_voltage = _set_vol;
    PowerState.set_current = _set_cur;

    PowerState.last_update_time_voltage = 0;
    PowerState.last_update_time_current = 0;
    PowerState.last_update_time_temperature = 0;

    PowerState.I2C1_state = DEVICE_ONLINE;        //sensor bus is online by default
    PowerState.INA226_state = DEVICE_ONLINE;
    PowerState.TMP112_state = DEVICE_ONLINE;
    return 0;
}

