#ifndef _TMP112A_H_
#define _TMP112A_H_

#include "sensors_dev.h"

#define TMP112A_ADDR            (0x48 << 1)
#define TMP112A_TMP_REG         (0x00)


sensor_dev_status_t TMP112A_dev_readTemperature(sensor_dev_i2c_t S_i2c, float* tempOut);

#endif
