#include "sensors.h"


extern I2C_HandleTypeDef hi2c1;

sensor_fstate_t TMP112_ReadTemperature(I2C_HandleTypeDef* hi2c,float* tempOut)
{
  uint8_t data[2] = {0};
  int16_t raw_temp;
  float temp_celsius;
  HAL_StatusTypeDef state_res;

  if(tempOut == NULL) return SENSOR_ERR_INVALID_DATA;

  state_res = HAL_I2C_Mem_Read(hi2c, TMP112A_ADDR, TMP112A_TMP_REG,
    I2C_MEMADD_SIZE_8BIT, data, 2, SENSOR_I2C_TIMEOUT);     //read temp
  switch (state_res)
  {
    case HAL_ERROR:   return SENSOR_ERR_I2C;
    case HAL_BUSY:    return SENSOR_ERR_BUSY;
    case HAL_OK:      break;
    default:          return SENSOR_ERR_I2C;
  }

  raw_temp = (int16_t)((data[0] << 8) | data[1]);
  temp_celsius = (float)(raw_temp >> 4) * 0.0625f;
  if(temp_celsius < -20.0 || temp_celsius > 200.0)return SENSOR_ERR_INVALID_DATA;
  
  *tempOut = temp_celsius;
  return SENSOR_OK;
}

sensor_fstate_t INA226_init(I2C_HandleTypeDef* hi2c, struct INA226_init_t init_data)
{
  HAL_StatusTypeDef state_res;
  uint16_t buf;

  buf = INA226_reg0_default;
  state_res = HAL_I2C_Mem_Write(hi2c,INA226_ADDR,INA226_CONFIG_REG,
    I2C_MEMADD_SIZE_8BIT,(uint8_t*)&buf,2,SENSOR_I2C_TIMEOUT);
  switch (state_res)
  {
    case HAL_ERROR:   return SENSOR_ERR_I2C;
    case HAL_BUSY:    return SENSOR_ERR_BUSY;
    case HAL_OK:      break;
    default:          return SENSOR_ERR_I2C;
  }

  buf = init_data.calibration;
  state_res = HAL_I2C_Mem_Write(hi2c,INA226_ADDR,INA226_CALIBRATION_REG,
    I2C_MEMADD_SIZE_8BIT,(uint8_t*)&buf,2,SENSOR_I2C_TIMEOUT);
  switch (state_res)
  {
    case HAL_ERROR:   return SENSOR_ERR_I2C;
    case HAL_BUSY:    return SENSOR_ERR_BUSY;
    case HAL_OK:      break;
    default:          return SENSOR_ERR_I2C;
  }

  return SENSOR_OK;
}

float INA226_readCuttent(float LSB)
{
  uint8_t reg = INA226_CURRENT_REG;
  uint8_t data[2];
  int16_t raw_current;
  if(HAL_I2C_Master_Transmit(&hi2c1, INA226_ADDR, &reg, 1, SENSOR_I2C_TIMEOUT) != HAL_OK){
    return FLT_MIN;
  }

  if(HAL_I2C_Master_Receive(&hi2c1, INA226_ADDR, data, 2, SENSOR_I2C_TIMEOUT) != HAL_OK){
    return FLT_MIN;
  }
  raw_current = (int16_t)((data[0] << 8) | data[1]);
  return raw_current*LSB;
}

float INA226_readVoltage(float LSB)
{
  uint8_t reg = INA226_VLOTAGE_REG;
  uint8_t data[2];
  int16_t raw_vlotage;
  if(HAL_I2C_Master_Transmit(&hi2c1, INA226_ADDR, &reg, 1, SENSOR_I2C_TIMEOUT) != HAL_OK){
    return FLT_MIN;
  }

  if(HAL_I2C_Master_Receive(&hi2c1, INA226_ADDR, data, 2, SENSOR_I2C_TIMEOUT) != HAL_OK){
    return FLT_MIN;
  }
  raw_vlotage = (int16_t)((data[0] << 8) | data[1]);
  return raw_vlotage*LSB;
}
