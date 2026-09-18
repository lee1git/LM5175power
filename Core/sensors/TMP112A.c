#include "TMP112A.h"

sensor_dev_status_t TMP112A_dev_readTemperature(sensor_dev_i2c_t S_i2c, float* tempOut)
{
  uint8_t data[2] = {0};
  int16_t raw_temp;
  float temp_celsius;
  sensor_dev_status_t state_res;

  if(tempOut == NULL) return SENSOR_DEV_ERR_INVALID_INPUT;

  state_res = I2C_sensor_dev_read_reg(S_i2c, TMP112A_ADDR, TMP112A_TMP_REG, data, 2);     //read temp
  if(state_res != SENSOR_DEV_OK) return state_res;

  raw_temp = U8S_TO_S16(data[0], data[1]);
  temp_celsius = (float)(raw_temp >> 4) * 0.0625f;
  if(temp_celsius < -40.0 || temp_celsius > 200.0)
    return SENSOR_DEV_ERR_INVALID_DATA;   //cant work in such temperature range, return error
  
  *tempOut = temp_celsius;
  return SENSOR_DEV_OK;
}
