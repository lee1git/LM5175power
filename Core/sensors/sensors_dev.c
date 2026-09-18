#include "sensors_dev.h"

#include "stm32f1xx_hal.h"
extern I2C_HandleTypeDef hi2c1;

sensor_dev_status_t I2C_sensor_dev_write_reg(sensor_dev_i2c_t i2c, uint8_t addr, uint8_t regaddr, uint8_t* data, uint16_t len)
{
    I2C_HandleTypeDef *hi2c;
    HAL_StatusTypeDef state_res;
    switch (i2c)
    {
        case SENSOR_DEV_I2C1: hi2c = &hi2c1; break;
        case SENSOR_DEV_I2C2: hi2c = NULL;   break;// Not implemented for STM32F103
        case SENSOR_DEV_I2C3: hi2c = NULL;   break;// Not implemented for STM32F103 
        case SENSOR_DEV_I2CMAX: hi2c = NULL; break;// Invalid I2C device
        default: hi2c = NULL; break;
    }
    if(hi2c == NULL) return SENSOR_DEV_ERR_I2C_NOT_IMPLEMENTED;
    if((data == NULL) || (len == 0)) return SENSOR_DEV_ERR_INVALID_DATA;   //data is the sending buffer here

    state_res = HAL_I2C_Mem_Write(hi2c, addr, regaddr, I2C_MEMADD_SIZE_8BIT, data, len, SENSOR_DEV_I2C_TIMEOUT); // Perform I2C write operation
    // Perform I2C write operation
    switch (state_res)
    {
        case HAL_ERROR:   return SENSOR_DEV_ERR_I2C;
        case HAL_BUSY:    return SENSOR_DEV_ERR_BUSY;
        case HAL_TIMEOUT: return SENSOR_DEV_ERR_BUSY; // Treat timeout as busy
        case HAL_OK:      break;
        default:          return SENSOR_DEV_ERR_I2C;
    }
    return SENSOR_DEV_OK;
}

sensor_dev_status_t I2C_sensor_dev_read_reg(sensor_dev_i2c_t i2c, uint8_t addr, uint8_t regaddr, uint8_t* data, uint16_t len)
{
    I2C_HandleTypeDef *hi2c;
    HAL_StatusTypeDef state_res;
    switch (i2c)
    {
        case SENSOR_DEV_I2C1: hi2c = &hi2c1; break;
        case SENSOR_DEV_I2C2: hi2c = NULL;   break;// Not implemented for STM32F103
        case SENSOR_DEV_I2C3: hi2c = NULL;   break;// Not implemented for STM32F103 
        case SENSOR_DEV_I2CMAX: hi2c = NULL; break;// Invalid I2C device
        default: hi2c = NULL; break;
    }
    if(hi2c == NULL) return SENSOR_DEV_ERR_I2C_NOT_IMPLEMENTED;
    if((data == NULL) || (len == 0)) return SENSOR_DEV_ERR_INVALID_DATA;   //data is the receiving buffer here

    state_res = HAL_I2C_Mem_Read(hi2c, addr, regaddr, I2C_MEMADD_SIZE_8BIT, data, len, SENSOR_DEV_I2C_TIMEOUT); // Perform I2C read operation
    switch (state_res)
    {
        case HAL_ERROR:   return SENSOR_DEV_ERR_I2C;
        case HAL_BUSY:    return SENSOR_DEV_ERR_BUSY;
        case HAL_TIMEOUT: return SENSOR_DEV_ERR_BUSY; // Treat timeout as busy
        case HAL_OK:      break;
        default:          return SENSOR_DEV_ERR_I2C;
    }
    return SENSOR_DEV_OK;
}
