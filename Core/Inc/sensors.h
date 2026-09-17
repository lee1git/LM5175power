#ifndef _SENSORS_H_

//include
#include "main.h"
#include "float.h"

//defines
#define TMP112A_ADDR            (0x48 << 1)
#define TMP112A_TMP_REG         (0x00)

#define INA226_ADDR             (0x40 << 1)
#define INA226_CONFIG_REG       (0x00)
#define INA226_VLOTAGE_REG      (0x02)  //Vout
#define INA226_CURRENT_REG      (0x04)
#define INA226_CALIBRATION_REG  (0x05)

#define INA226_reg_10A_6mOhm    ((uint16_t)0x06aa)        //max current 10A, 500uA/pear, 6mOhm resister

#define SENSOR_I2C_TIMEOUT      (10U)       //10ms

//structure
struct INA226_init_t
{
    uint16_t calibration;
};


struct Sensors_init_t
{
    struct INA226_init_t INA226;
};


// function
float TMP112_ReadTemperature(void);
int INA226_init(struct INA226_init_t init_data);
float INA226_readCuttent(float LSB);
float INA226_readVoltage(float LSB);

int Sensors_init(void);

#endif // !_SENSORS_H_