#ifndef _SENSORS_H_
#define _SENSORS_H_

//defines
#define INVALID_DATA_FLOAT      (0xFFFFFFFF)

#define MAX_RETRY_COUNT           (3U)       //max retry count for i2c busy

//typedef
typedef enum{
    SENSOR_SUCCESS = 0,
    SENSOR_ERR_DEV,
    SENSOR_ERR_INVALID_INPUT
}sensor_state_t;

    //INA226
typedef enum{
    INA226_calibration_10A_6mOhm = 0
}INA226_calibration_t;


//structure
struct sensor_INA226_init
{
    INA226_calibration_t calibration;
};


// function
sensor_state_t TMP112_ReadTemperature(float* tempOut);
sensor_state_t INA226_init(struct sensor_INA226_init init_data);
sensor_state_t INA226_readCuttent(float LSB, float* currentOut);
sensor_state_t INA226_readVoltage(float LSB, float* voltageOut);

// int Sensors_init(struct Sensors_init_t init_data);
int I2C_Restart(void);

#endif // !_SENSORS_H_
