#include "sensors.h"
#include "sensors_dev.h"
#include "INA226AIDGSR.h"

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

sensor_state_t INA226_init(struct sensor_INA226_init init_data)
{
  sensor_dev_status_t state_res;
  uint16_t calibration = 0;
  switch (init_data.calibration)
  {
    case INA226_calibration_10A_6mOhm:
      calibration = INA226_reg_10A_6mOhm;
      break;
    default:
      return SENSOR_STA_ERR_DEVI;
  }

  state_res = INA226_dev_init(SENSOR_DEV_I2C1,calibration);

  if(state_res == SENSOR_DEV_ERR_BUSY) //retry
  {
    // Implement retry logic here
    return SENSOR_STA_ERR_DEVI;
  }else if(state_res != SENSOR_DEV_OK)
  {
    return SENSOR_STA_ERR_DEVI;
  }

  return SENSOR_STA_OK;
}

sensor_fstate_t INA226_readCuttent(I2C_HandleTypeDef* hi2c,float LSB,float* currentOut)
{
  uint8_t data[2] = {0};
  int16_t raw_current;
  float current_ampere;
  HAL_StatusTypeDef state_res;

  if(currentOut == NULL) return SENSOR_ERR_INVALID_DATA;
  if(LSB <= 0.0f) return SENSOR_ERR_INVALID_DATA;

  state_res = HAL_I2C_Mem_Read(hi2c, INA226_ADDR, INA226_CURRENT_REG,
    I2C_MEMADD_SIZE_8BIT, data, 2, SENSOR_I2C_TIMEOUT);     //read current
  switch (state_res)
  {
    case HAL_ERROR:   return SENSOR_ERR_I2C;
    case HAL_BUSY:    return SENSOR_ERR_BUSY;
    case HAL_OK:      break;
    default:          return SENSOR_ERR_I2C;
  }

  raw_current = (int16_t)((data[0] << 8) | data[1]);
  current_ampere = (float)raw_current * LSB;
  if(current_ampere < 0.01f) return SENSOR_ERR_INVALID_DATA;
  *currentOut = current_ampere;
  return SENSOR_OK;
}

sensor_fstate_t INA226_readVoltage(I2C_HandleTypeDef* hi2c, float LSB, float* voltageOut)
{
  uint8_t data[2] = {0};
  int16_t raw_vlotage;
  HAL_StatusTypeDef state_res;

  if(voltageOut == NULL) return SENSOR_ERR_INVALID_DATA;
  if(LSB <= 0.0f) return SENSOR_ERR_INVALID_DATA;

  state_res = HAL_I2C_Mem_Read(hi2c, INA226_ADDR, INA226_VLOTAGE_REG, 
    I2C_MEMADD_SIZE_8BIT, data, 2, SENSOR_I2C_TIMEOUT);
  switch (state_res)
  {
    case HAL_ERROR:   return SENSOR_ERR_I2C;
    case HAL_BUSY:    return SENSOR_ERR_BUSY;
    case HAL_OK:      break;
    default:          return SENSOR_ERR_I2C;
  }

  raw_vlotage = (int16_t)((data[0] << 8) | data[1]);
  if(raw_vlotage*LSB < -0.1f) return SENSOR_ERR_INVALID_DATA;    //dead zone
  *voltageOut = raw_vlotage*LSB;
  return SENSOR_OK;
}
