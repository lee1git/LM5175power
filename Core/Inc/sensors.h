#ifndef _SENSORS_H_

//include
#include "main.h"
#include "float.h"

//define method
#define HIGH8IN16(u16)           ((uint8_t)(u16>>8))
#define LOW8IN16(u16)           ((uint8_t)(u16))


//defines
#define TMP112A_ADDR            (0x48 << 1)
#define TMP112A_TMP_REG         (0x00)

#define INA226_ADDR             (0x40 << 1)
#define INA226_CONFIG_REG       (0x00)
#define INA226_VLOTAGE_REG      (0x02)  //Vout
#define INA226_CURRENT_REG      (0x04)
#define INA226_CALIBRATION_REG  (0x05)

#define INA226_reg0_default     (0x4127)
#define INA226_reg_10A_6mOhm    ((uint16_t)0x06aa)        //max current 10A, 500uA/pear, 6mOhm resister

#define SENSOR_I2C_TIMEOUT      (5U)       //5ms

//structure
struct INA226_init_t
{
    uint16_t calibration;
};


struct Sensors_init_t
{
    struct INA226_init_t INA226;
};

//enum
typedef enum{
    SENSOR_OK = 0,
    SENSOR_ERR_BUSY,        //timeout or block
    SENSOR_ERR_I2C,
    SENSOR_ERR_INVALID_DATA //invalid data/ null ptr
}sensor_fstate_t;


// function
sensor_fstate_t TMP112_ReadTemperature(I2C_HandleTypeDef* hi2c, float* tempOut);
sensor_fstate_t INA226_init(I2C_HandleTypeDef* hi2c, struct INA226_init_t init_data);
float INA226_readCuttent(float LSB);
float INA226_readVoltage(float LSB);

int Sensors_init(struct Sensors_init_t init_data);
int I2C_Restart();

#endif // !_SENSORS_H_