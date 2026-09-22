#include "sensors.h"

#include "sensors_dev.h"
#include "INA226AIDGSR.h"
#include "TMP112A.h"
#include "i2c.h"        //hi2c1, bound to the bus layer below

//
struct sensor_INA226_init init_data_last = {INA226_calibration_UNDEFINED};



sensor_state_t TMP112_ReadTemperature(float* tempOut)
{
  sensor_dev_status_t state_res;

  for(int retry = 0; retry < MAX_RETRY_COUNT; retry++)
  {
    state_res = TMP112A_dev_readTemperature(SENSOR_DEV_I2C1, tempOut);
    if(state_res != SENSOR_DEV_ERR_BUSY) break; // Exit loop if not busy
  }

  switch (state_res)
  {
    case SENSOR_DEV_OK: return SENSOR_SUCCESS;
    case SENSOR_DEV_ERR_I2C:
    case SENSOR_DEV_ERR_BUSY:
    case SENSOR_DEV_ERR_INVALID_DATA:
    default: return SENSOR_ERR_DEV;
  }
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
      return SENSOR_ERR_INVALID_INPUT;
  }
  init_data_last = init_data;   //save the last calibration setting

  for(int retry = 0; retry < MAX_RETRY_COUNT; retry++)
  {
    state_res = INA226_dev_init(SENSOR_DEV_I2C1,calibration);
    if(state_res != SENSOR_DEV_ERR_BUSY) break; // Exit loop if not busy
  }

  switch (state_res)
  {
    case SENSOR_DEV_OK: return SENSOR_SUCCESS;
    
    case SENSOR_DEV_ERR_I2C: case SENSOR_DEV_ERR_BUSY:  default:          
      return SENSOR_ERR_DEV;
  }
}

sensor_state_t INA226_readCuttent(float LSB, float* currentOut)
{
  sensor_dev_status_t state_res;

  for(int retry = 0; retry < MAX_RETRY_COUNT; retry++)
  {
    state_res = INA226_dev_readCurrent(SENSOR_DEV_I2C1, LSB, currentOut);
    if(state_res != SENSOR_DEV_ERR_BUSY) break; // Exit loop if not busy
  }

  switch (state_res)
  {
    case SENSOR_DEV_OK: return SENSOR_SUCCESS;
    
    case SENSOR_DEV_ERR_I2C: 
    case SENSOR_DEV_ERR_BUSY: 
    case SENSOR_DEV_ERR_INVALID_DATA: 
    default:          
      return SENSOR_ERR_DEV;
  }
}

sensor_state_t INA226_readVoltage(float LSB, float* voltageOut)
{
  sensor_dev_status_t state_res;

  for(int retry = 0; retry < MAX_RETRY_COUNT; retry++)
  {
    state_res = INA226_dev_readVoltage(SENSOR_DEV_I2C1, LSB, voltageOut);
    if(state_res != SENSOR_DEV_ERR_BUSY) break; // Exit loop if not busy
  }

  switch (state_res)
  {
    case SENSOR_DEV_OK: return SENSOR_SUCCESS;
    
    case SENSOR_DEV_ERR_I2C: 
    case SENSOR_DEV_ERR_BUSY: 
    case SENSOR_DEV_ERR_INVALID_DATA: 
    default:          
      return SENSOR_ERR_DEV;
  }
}

int sensors_outerdev_init(void)
{
  if(INA226_init(init_data_last) != SENSOR_SUCCESS) return -1;

  // maybe more outer device init here

  return 0; // Return 0 for success
}

int Sensors_bus_restart(void)
{
  return I2C_Restart(SENSOR_DEV_I2C1);
}

int I2C2_bus_restart(void)
{
  return I2C_Restart(SENSOR_DEV_I2C2);
}

/* Bind the I2C handle to the bus layer.
 * The application calls this once, after MX_I2C1_Init() and before any sensor access. */
int Sensors_bus_init(void)
{
  I2C_sensor_dev_init(&hi2c1);
  return 0;
}

