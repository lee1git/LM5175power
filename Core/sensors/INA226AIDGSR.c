#include "INA226AIDGSR.h"

sensor_dev_status_t INA226_dev_init(sensor_dev_i2c_t S_i2c, uint16_t calibration)
{
  sensor_dev_status_t state_res;
  uint8_t tx_data[2] = {0};

  tx_data[0] = HIGH8IN16(INA226_reg0_default);
  tx_data[1] = LOW8IN16(INA226_reg0_default);
  state_res = I2C_sensor_dev_write_reg(S_i2c,INA226_ADDR,INA226_CONFIG_REG,tx_data,2);
  if(state_res != SENSOR_DEV_OK) return state_res;

  tx_data[0] = HIGH8IN16(calibration);
  tx_data[1] = LOW8IN16(calibration);
  state_res = I2C_sensor_dev_write_reg(S_i2c,INA226_ADDR,INA226_CALIBRATION_REG,tx_data,2);
  if(state_res != SENSOR_DEV_OK) return state_res;

  return SENSOR_DEV_OK;
}

sensor_dev_status_t INA226_dev_readCurrent(sensor_dev_i2c_t S_i2c, float LSB, float* currentOut)
{
    uint8_t data[2] = {0};
    int16_t raw_current;
    sensor_dev_status_t state_res;
    
    if(currentOut == NULL) return SENSOR_DEV_ERR_INVALID_INPUT;
    if(LSB <= 0.0f) return SENSOR_DEV_ERR_INVALID_INPUT;
    
    state_res = I2C_sensor_dev_read_reg(S_i2c, INA226_ADDR, INA226_CURRENT_REG, data, 2);     //read current
    if(state_res != SENSOR_DEV_OK) return state_res;
    
    raw_current = U8S_TO_S16(data[0], data[1]);
    *currentOut = (float)raw_current * LSB;
    return SENSOR_DEV_OK;
}

sensor_dev_status_t INA226_dev_readVoltage(sensor_dev_i2c_t S_i2c, float LSB, float* voltageOut)
{
    uint8_t data[2] = {0};
    int16_t raw_voltage;
    sensor_dev_status_t state_res;
    
    if(voltageOut == NULL) return SENSOR_DEV_ERR_INVALID_INPUT;
    if(LSB <= 0.0f) return SENSOR_DEV_ERR_INVALID_INPUT;
    
    state_res = I2C_sensor_dev_read_reg(S_i2c, INA226_ADDR, INA226_VLOTAGE_REG, data, 2);     //read voltage
    if(state_res != SENSOR_DEV_OK) return state_res;
    
    raw_voltage = U8S_TO_S16(data[0], data[1]);
    *voltageOut = (float)raw_voltage * LSB;
    return SENSOR_DEV_OK;
}
