#include "sensors.h"


extern I2C_HandleTypeDef hi2c1;

float TMP112_ReadTemperature(void)
{
  uint8_t reg = TMP112A_TMP_REG;
  uint8_t data[2] = {0};
  int16_t raw_temp;
  float temp_celsius;

  //send addr
  if(HAL_I2C_Master_Transmit(&hi2c1, TMP112A_ADDR, &reg, 1, SENSOR_I2C_TIMEOUT) != HAL_OK){
    return FLT_MIN;
  }
  //read 2 bytes teamp data
  if(HAL_I2C_Master_Receive(&hi2c1, TMP112A_ADDR, data, 2, SENSOR_I2C_TIMEOUT) != HAL_OK){
    return FLT_MIN;
  }
  raw_temp = (int16_t)((data[0] << 8) | data[1]);
  temp_celsius = (float)(raw_temp >> 4) * 0.0625f;
  return temp_celsius;
}

int INA226_init(struct INA226_init_t init_data)
{
  uint8_t data[3];
  data[0] = INA226_CONFIG_REG;
  data[1] = 0x41;
  data[2] = 0x27;
  if(HAL_I2C_Master_Transmit(&hi2c1, INA226_ADDR, data, 3, SENSOR_I2C_TIMEOUT) != HAL_OK){
    return 1;
  }

  data[0] = INA226_CALIBRATION_REG;
  data[1] = (uint8_t)(init_data.calibration>>8);
  data[2] = (uint8_t)(init_data.calibration);
  if(HAL_I2C_Master_Transmit(&hi2c1, INA226_ADDR, data, 3, SENSOR_I2C_TIMEOUT) != HAL_OK){
    return 1;
  }
  return 0;
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
