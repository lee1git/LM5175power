#include "powerMaster.h"

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
    if((_en_sta == PWR_EN_OFF) || (_en_sta == PWR_EN_ON))
        PowerState.en_statu = _en_sta;
    else    
        PowerState.en_statu = PWR_EN_OFF;       //defalt close output
    
    PowerState.now_current = 0.0f;
    PowerState.now_voltage = 0.0f;
    PowerState.now_temperature = 0.0f;

    if(_set_vol >= 1.0f && _set_vol < 15.5f)
        PowerState.set_voltage = _set_vol;
    else    
        PowerState.set_voltage = 2.0f;      //2V

    if(_set_cur >= 0.0f && _set_cur < 10.5f)
        PowerState.set_current = _set_cur;
    else
        PowerState.set_current = 0.5f;      //500mA

    return 0;
}

