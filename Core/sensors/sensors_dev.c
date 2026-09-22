#include "sensors_dev.h"

#include <assert.h>              //assert() is a macro: without this it links as an undefined symbol
#include "stm32f1xx_hal.h"
#include "i2c.h"                 //MX_I2C1_Init(), for bus recovery

#define I2C_INUSE(i2c)  ((i2c == SENSOR_DEV_I2C1 || i2c == SENSOR_DEV_I2C2) && s_hi2c1 != NULL)

/* Bound by the application layer, see I2C_sensor_dev_init() */
static I2C_HandleTypeDef* s_hi2c1 = NULL;
static I2C_HandleTypeDef* s_hi2c2 = &hi2c2;

void I2C_sensor_dev_init(I2C_HandleTypeDef* hi2c1)
{
    s_hi2c1 = hi2c1;
}

/* Weak by default (no locking); the application overrides them, see freertos.c */
__weak int  I2C_dev_lock(sensor_dev_i2c_t i2c)      { return 0; }
__weak void I2C_dev_unlock(sensor_dev_i2c_t i2c)    { }

__weak int  I2C_sensor_dev_lock(void)   { return 0; }   //0 = locked, non-zero = not acquired
__weak void I2C_sensor_dev_unlock(void) { }

sensor_dev_status_t I2C_sensor_dev_write_reg(sensor_dev_i2c_t i2c, uint8_t addr, uint8_t regaddr, uint8_t* data, uint16_t len)
{
    assert(I2C_INUSE(i2c));  // Ensure the I2C device is in use

    I2C_HandleTypeDef *hi2c;
    HAL_StatusTypeDef state_res;
    switch (i2c)
    {
        case SENSOR_DEV_I2C1: hi2c = s_hi2c1; break;
        case SENSOR_DEV_I2C2: hi2c = NULL;   break;// Not implemented for STM32F103
        case SENSOR_DEV_I2C3: hi2c = NULL;   break;// Not implemented for STM32F103 
        case SENSOR_DEV_I2CMAX: hi2c = NULL; break;// Invalid I2C device
        default: hi2c = NULL; break;
    }
    if(hi2c == NULL) return SENSOR_DEV_ERR_I2C_NOT_IMPLEMENTED;
    if((data == NULL) || (len == 0)) return SENSOR_DEV_ERR_INVALID_INPUT;   //data is the sending buffer here

    if(I2C_sensor_dev_lock() != 0) return SENSOR_DEV_ERR_BUSY;   //no lock, do not touch the bus
    state_res = HAL_I2C_Mem_Write(hi2c, addr, regaddr, I2C_MEMADD_SIZE_8BIT, data, len, SENSOR_DEV_I2C_TIMEOUT); // Perform I2C write operation
    I2C_sensor_dev_unlock();
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
    assert(I2C_INUSE(i2c));  // Ensure the I2C device is in use

    I2C_HandleTypeDef *hi2c;
    HAL_StatusTypeDef state_res;
    switch (i2c)
    {
        case SENSOR_DEV_I2C1: hi2c = s_hi2c1; break;
        case SENSOR_DEV_I2C2: hi2c = NULL;   break;// Not implemented for STM32F103
        case SENSOR_DEV_I2C3: hi2c = NULL;   break;// Not implemented for STM32F103 
        case SENSOR_DEV_I2CMAX: hi2c = NULL; break;// Invalid I2C device
        default: hi2c = NULL; break;
    }
    if(hi2c == NULL) return SENSOR_DEV_ERR_I2C_NOT_IMPLEMENTED;
    if((data == NULL) || (len == 0)) return SENSOR_DEV_ERR_INVALID_INPUT;   //data is the receiving buffer here

    if(I2C_sensor_dev_lock() != 0) return SENSOR_DEV_ERR_BUSY;   //no lock, do not touch the bus
    state_res = HAL_I2C_Mem_Read(hi2c, addr, regaddr, I2C_MEMADD_SIZE_8BIT, data, len, SENSOR_DEV_I2C_TIMEOUT); // Perform I2C read operation
    I2C_sensor_dev_unlock();
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

static int I2C_release_bus(GPIO_TypeDef* GPIOx, uint16_t SCL_Pin, uint16_t SDA_Pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // open drain output mode, pull-up, low speed
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // 如果外部已有上拉，可设为 NOPULL
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = SCL_Pin;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = SDA_Pin;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

    //SDA release
    HAL_GPIO_WritePin(GPIOx, SDA_Pin, GPIO_PIN_SET);

    //SCL 9 clock pulses
    for(int i=0; i<9; i++)
    {
        HAL_GPIO_WritePin(GPIOx, SCL_Pin, GPIO_PIN_RESET);
        HAL_Delay(1);
        HAL_GPIO_WritePin(GPIOx, SCL_Pin, GPIO_PIN_SET);
        HAL_Delay(1);
    }

    // Generate a stop condition
    HAL_GPIO_WritePin(GPIOx, SDA_Pin, GPIO_PIN_RESET);
    for(volatile int j=0; j<10; j++);
    HAL_GPIO_WritePin(GPIOx, SCL_Pin, GPIO_PIN_SET);
    for(volatile int j=0; j<10; j++);
    HAL_GPIO_WritePin(GPIOx, SDA_Pin, GPIO_PIN_SET);
    for(volatile int j=0; j<10; j++);

    return 0;
}

int I2C_Restart(sensor_dev_i2c_t i2c)
{
    //assert(I2C_INUSE(i2c));  // Ensure the I2C device is in use
    switch (i2c)
    {
    case SENSOR_DEV_I2C1:
        if(s_hi2c1 == NULL) return -1;      //bus not bound yet
        if(I2C_sensor_dev_lock() != 0) return -1;   //no lock, do not touch the bus
        HAL_I2C_DeInit(s_hi2c1);
        I2C_release_bus(GPIOB, GPIO_PIN_6, GPIO_PIN_7); // Release the I2C bus
        MX_I2C1_Init();
        I2C_sensor_dev_unlock();
        break;
    case SENSOR_DEV_I2C2:
        if(s_hi2c2 == NULL) return -1;
        HAL_I2C_DeInit(s_hi2c2);
        I2C_release_bus(GPIOB, GPIO_PIN_10, GPIO_PIN_11); // Release the I2C bus
        MX_I2C2_Init();
        break;
    default:
        break;
    }
    return 0;
}


