/**
 * @file sensors_dev.h
 * @brief This file contains the definitions and function prototypes for sensor device operations.
 */

#ifndef _SENSORS_DEV_H_
#define _SENSORS_DEV_H_

//include zone
#include "stdint.h"

//define method
#define HIGH8IN16(u16)           ((uint8_t)(u16>>8))
#define LOW8IN16(u16)           ((uint8_t)(u16))

// define zone
#define SENSOR_DEV_I2C_TIMEOUT      (5U)       //5ms

// enum zone
typedef enum{
    SENSOR_DEV_I2C1 = 0,
    SENSOR_DEV_I2C2,
    SENSOR_DEV_I2C3,
    SENSOR_DEV_I2CMAX
} sensor_dev_i2c_t;

typedef enum{
    SENSOR_DEV_OK = 0,
    SENSOR_DEV_ERR_BUSY,                //timeout or block
    SENSOR_DEV_ERR_I2C,
    SENSOR_DEV_ERR_INVALID_DATA,        //invalid data/ null 
    SENSOR_DEV_ERR_I2C_NOT_IMPLEMENTED  //i2c not implemented for this platform
} sensor_dev_status_t;


// function zone
/**
 * @brief Write data to a sensor device register over I2C.
 * 
 * @param i2c       The I2C interface to use (SENSOR_DEV_I2C1, SENSOR_DEV_I2C2, etc.).
 * @param addr      The I2C address of the sensor device.
 * @param regaddr   The register address to write to.
 * @param data      Pointer to the data buffer to write.
 * @param len       Length of the data to write.
 * @return sensor_dev_status_t  Status of the operation (SENSOR_DEV_OK, SENSOR_DEV_ERR_BUSY, etc.).
 */
sensor_dev_status_t I2C_sensor_dev_write_reg(
    sensor_dev_i2c_t i2c, 
    uint8_t addr, 
    uint8_t regaddr, 
    uint8_t* data, 
    uint16_t len);

/**
 * @brief Read data from a sensor device register over I2C.
 * 
 * @param i2c       The I2C interface to use (SENSOR_DEV_I2C1, SENSOR_DEV_I2C2, etc.).
 * @param addr      The I2C address of the sensor device.
 * @param regaddr   The register address to read from.
 * @param data      Pointer to the buffer receiving the data; 16-bit register uses 2 bytes, MSB first.
 * @param len       Number of bytes to read.
 * @return sensor_dev_status_t  Status of the operation (SENSOR_DEV_OK, SENSOR_DEV_ERR_BUSY, etc.).
 */
sensor_dev_status_t I2C_sensor_dev_read_reg(
    sensor_dev_i2c_t i2c, 
    uint8_t addr, 
    uint8_t regaddr, 
    uint8_t* data, 
    uint16_t len);

#endif // !_SENSORS_DEV_H_

