#ifndef _SENSORS_H_
#define _SENSORS_H_

//include
#include "main.h"

//defines
#define TMP112A_ADDR            (0x48 << 1)
#define TMP112A_TMP_REG         (0x00)

#define SENSOR_I2C_TIMEOUT      (5U)       //5ms

//typedef
typedef enum{
    SENSOR_STA_OK = 0,
    SENSOR_STA_ERR_DEVI,
    SENSOR_STA_ERR_DATA
}sensor_state_t;

typedef enum{
    SENSOR_OK = 0,
    SENSOR_ERR_BUSY,
    SENSOR_ERR_I2C,
    SENSOR_ERR_INVALID_DATA
}sensor_fstate_t;
typedef enum{
    INA226_calibration_10A_6mOhm = 0
}INA226_calibration_t;


//structure
struct sensor_INA226_init
{
    INA226_calibration_t calibration;
};


// function
sensor_fstate_t TMP112_ReadTemperature(I2C_HandleTypeDef* hi2c, float* tempOut);
sensor_state_t INA226_init(struct sensor_INA226_init init_data);
sensor_fstate_t INA226_readCuttent(I2C_HandleTypeDef* hi2c, float LSB, float* currentOut);
sensor_fstate_t INA226_readVoltage(I2C_HandleTypeDef* hi2c, float LSB, float* voltageOut);

// int Sensors_init(struct Sensors_init_t init_data);
int I2C_Restart(void);

#endif // !_SENSORS_H_
