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
