#ifndef _POWER_CONFIG_H
#define _POWER_CONFIG_H

#include "powerMaster.h"

//limit
#define POWER_MAX_VOLTAGE           (15.0f)
#define POWER_MIN_VOLTAGE           (1.0f)
//step
#define POWER_VOLTAGE_UP_STEP       (1.0f)
#define POWER_VOLTAGE_DOWN_STEP     (-1.0f)

//setup
#define POWER_SETUP_OUT_STATE       (PWR_EN_OFF)
#define POWER_SETUP_VOLTAGE_SET     (6.0f)
#define POWER_SETUP_CURRENT_LIMIT   (10.0f)

#endif // !_POWER_CONFIG_H
