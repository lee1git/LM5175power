/**
 * @file INA226AIDGSR.h
 * @brief INA226AIDGSR sensor driver header file
 * @details This file contains the function declarations and type definitions for the INA226AIDGSR sensor driver.
 *          This driver provides functions to initialize the sensor, read current and voltage values, and handle I2C communication.
 *          INA226AIDGSR is a double forward current and power monitor with an I2C interface. 
 *          It can measure both positive and negative current flow.
 *          So cant detect the data rarly through positive and negative current.
 */

#ifndef _INA226AIDGSR_H_
#define _INA226AIDGSR_H_

#include "sensors_dev.h"

#define INA226_ADDR             (0x40 << 1)
#define INA226_CONFIG_REG       (0x00)
#define INA226_VLOTAGE_REG      (0x02)  //Vout
#define INA226_CURRENT_REG      (0x04)
#define INA226_CALIBRATION_REG  (0x05)

#define INA226_reg0_default     (0x4127)
#define INA226_reg_10A_6mOhm    ((uint16_t)0x06aa)        //max current 10A, 500uA/pear, 6mOhm resister

sensor_dev_status_t INA226_dev_init(sensor_dev_i2c_t S_i2c, uint16_t calibration);
sensor_dev_status_t INA226_dev_readCurrent(sensor_dev_i2c_t S_i2c, float LSB, float* currentOut);
sensor_dev_status_t INA226_dev_readVoltage(sensor_dev_i2c_t S_i2c, float LSB, float* voltageOut);

#endif
