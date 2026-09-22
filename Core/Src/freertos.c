/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "powerMaster.h"
#include "tim.h"
#include "usart.h"
#include "sensors.h"
#include "sensors_dev.h"
#include "stm32_u8g2.h"
#include "stdio.h"
#include "power_config.h"
#include "uart_debug.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* I2C 总线锁最长等待：约两倍最坏持锁时间（一次事务 5ms HAL 超时） */
#define I2C_LOCK_TIMEOUT_TICKS    (pdMS_TO_TICKS(10))
/* PowerState 锁最长等待：临界区只有几次字段读写，5ms 足够宽松 */
#define POWERSTATE_LOCK_TIMEOUT_TICKS  (pdMS_TO_TICKS(5))
/* 传感器错误计数上限 */
#define SENSOR_ERROR_THRESHOLD  5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* PID 状态在 main.c 的 USER CODE 4 里定义 */
extern float integral;
extern float last_error;

extern u8g2_t u8g2;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for buttom */
osThreadId_t buttomHandle;
const osThreadAttr_t buttom_attributes = {
  .name = "buttom",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime1,
};
/* Definitions for PIDv */
osThreadId_t PIDvHandle;
const osThreadAttr_t PIDv_attributes = {
  .name = "PIDv",
  .stack_size = 160 * 4,
  .priority = (osPriority_t) osPriorityRealtime7,
};
/* Definitions for sensorTask */
osThreadId_t sensorTaskHandle;
const osThreadAttr_t sensorTask_attributes = {
  .name = "sensorTask",
  .stack_size = 160 * 4,
  .priority = (osPriority_t) osPriorityRealtime1,
};
/* Definitions for sensor_err_hand */
osThreadId_t sensor_err_handHandle;
const osThreadAttr_t sensor_err_hand_attributes = {
  .name = "sensor_err_hand",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for screen */
osThreadId_t screenHandle;
const osThreadAttr_t screen_attributes = {
  .name = "screen",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PowerStateAcssess */
osMutexId_t PowerStateAcssessHandle;
const osMutexAttr_t PowerStateAcssess_attributes = {
  .name = "PowerStateAcssess"
};
/* Definitions for I2CAccess */
osMutexId_t I2CAccessHandle;
const osMutexAttr_t I2CAccess_attributes = {
  .name = "I2CAccess"
};
/* Definitions for dev_uart1 */
osMutexId_t dev_uart1Handle;
const osMutexAttr_t dev_uart1_attributes = {
  .name = "dev_uart1"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern int16_t pid_calculate(float target_voltage, float actual_voltage);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void buttomTask(void *argument);
void Vlotage_pid(void *argument);
void sensorRead(void *argument);
void sensor_err_handle(void *argument);
void screen_show(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of PowerStateAcssess */
  PowerStateAcssessHandle = osMutexNew(&PowerStateAcssess_attributes);

  /* creation of I2CAccess */
  I2CAccessHandle = osMutexNew(&I2CAccess_attributes);

  /* creation of dev_uart1 */
  dev_uart1Handle = osMutexNew(&dev_uart1_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of buttom */
  buttomHandle = osThreadNew(buttomTask, NULL, &buttom_attributes);

  /* creation of PIDv */
  PIDvHandle = osThreadNew(Vlotage_pid, NULL, &PIDv_attributes);

  /* creation of sensorTask */
  sensorTaskHandle = osThreadNew(sensorRead, NULL, &sensorTask_attributes);

  /* creation of sensor_err_hand */
  sensor_err_handHandle = osThreadNew(sensor_err_handle, NULL, &sensor_err_hand_attributes);

  /* creation of screen */
  screenHandle = osThreadNew(screen_show, NULL, &screen_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_buttomTask */
/**
* @brief Function implementing the buttom thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_buttomTask */
void buttomTask(void *argument)
{
  /* USER CODE BEGIN buttomTask */
  int temp = 0;
  osStatus_t result;
  /* Infinite loop */
  for(;;)
  {
    osDelay(pdMS_TO_TICKS(120));
    osThreadFlagsWait((uint32_t)0x00000001U,osFlagsWaitAny,osWaitForever);//wait EXIT
    osDelay(pdMS_TO_TICKS(20));   //

    temp = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8);  //power en switch
    if(temp == GPIO_PIN_RESET){
      temp = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9);   //read levels outside the lock

      if(osMutexAcquire(PowerStateAcssessHandle,pdMS_TO_TICKS(5)) == osOK)
      {
        if(PowerState.en_statu == PWR_EN_ON){   //off
          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);
          HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
          PowerState.en_statu = PWR_EN_OFF;
        }
        else{
          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);
          HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
          PowerState.en_statu = PWR_EN_ON;
        }
        result = osMutexRelease(PowerStateAcssessHandle);   //only release what we hold
        (void)result;                                       //kept for the DEBUG build
        #if DEBUG
        if(result != osOK){
          HAL_UART_Transmit(&huart2,"MUTEX:PowerStateAcssess realse ERR\r\n",29,HAL_MAX_DELAY);
        }
        #endif
      }
      else{
        //lock not acquired: leave PowerState untouched this round
      }
      continue;
    }

    temp = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);   //vlotage limit&current limit switch
    if (temp == GPIO_PIN_RESET){

    }

    temp = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);   //dowm,vlotage first
    if (temp == GPIO_PIN_RESET){
      if(PowerState_lock() == 0)
      {
        if(PowerState.set_voltage + POWER_VOLTAGE_DOWN_STEP < POWER_MIN_VOLTAGE)
          PowerState.set_voltage = POWER_MIN_VOLTAGE;
        else
          PowerState.set_voltage += POWER_VOLTAGE_DOWN_STEP;
        PowerState_unlock();
      }
    }

    temp = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);    //up
    if (temp == GPIO_PIN_RESET){
      if(PowerState_lock() == 0)
      {
        if(PowerState.set_voltage + POWER_VOLTAGE_UP_STEP > POWER_MAX_VOLTAGE)
          PowerState.set_voltage = POWER_MAX_VOLTAGE;
        else
          PowerState.set_voltage += POWER_VOLTAGE_UP_STEP;
        PowerState_unlock();
      }
    }

  }
  /* USER CODE END buttomTask */
}

/* USER CODE BEGIN Header_Vlotage_pid */
/**
* @brief Function implementing the PIDv thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Vlotage_pid */
void Vlotage_pid(void *argument)
{
  /* USER CODE BEGIN Vlotage_pid */
  #ifdef DEV_UART_DEBUG
    int run_count = 0;
  #endif
  float set_voltage = POWER_SETUP_VOLTAGE_SET;
  float new_voltage;  //V
  int16_t voltage_change = 0;
  uint16_t now_pulse = 500;       //initial PWM pulse width, just a magic number
  int16_t pwm_pulse;
  TickType_t lastWakeTime = xTaskGetTickCount();   //period base, taken once outside the loop
  uint8_t check_over = 1;
  /* Infinite loop */
  for(;;)
  {
    check_over = 1;                                //assume all checks pass, cleared on any fault
    voltage_change = 0;                             //reset the voltage change flag
    
    if(PowerState_lock() == 0){

      if(PowerState.en_statu == PWR_EN_OFF){
        integral = 0;
        last_error = 0;
        check_over = 0;
      }

      new_voltage = PowerState.now_voltage;
      if(new_voltage < 0.0f)
      {
        integral = 0;
        last_error = 0;
        check_over = 0;
      }

      set_voltage = PowerState.set_voltage;

      if(PowerState.INA226_state == DEVICE_OFFLINE){
        integral = 0;
        last_error = 0;
        check_over = 0;
      }
      PowerState_unlock();
    }else{
      //lock not acquired: leave PowerState untouched this round
      check_over = 0;
    }

    if(check_over == 1){          //all checks passed: this round may drive the PWM
      pwm_pulse = pid_calculate(set_voltage,new_voltage);
      now_pulse += pwm_pulse;   //update the last known pulse width
      if(now_pulse > 800)now_pulse = 800;
      if(now_pulse < 300)now_pulse = 300;
      __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,now_pulse);
    }

  #ifdef DEV_UART_DEBUG
    UART_Printf("voltage_pid:%d\r\n",run_count);
    UART_Printf("voltage_pid_stack:%d\r\n",uxTaskGetStackHighWaterMark(PIDvHandle));
  #endif

    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(50));  //50ms period
  }
  /* USER CODE END Vlotage_pid */
}

/* USER CODE BEGIN Header_sensorRead */
/**
* @brief Function implementing the sensorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_sensorRead */
void sensorRead(void *argument)
{
  /* USER CODE BEGIN sensorRead */
#ifdef DEV_UART_DEBUG
  int run_count = 0;
#endif
  float temperature;
  float voltage;
  float current;

  int temperature_err_count = 0;
  int voltage_err_count = 0;
  int current_err_count = 0;

  char I2C_state;
  char INA226_state;
  char TMP112A_state;

  sensor_state_t state_res_temperature;
  sensor_state_t state_res_voltage;
  sensor_state_t state_res_current;
  TickType_t lastWakeTime = xTaskGetTickCount();  //period base
  TickType_t write_time;                  
  /* Infinite loop */
  for(;;)
  {
#ifdef DEV_UART_DEBUG
    UART_Printf("sensorRead:%d\r\n",run_count);
    UART_Printf("sensorRead_stack:%d\r\n",uxTaskGetStackHighWaterMark(sensorTaskHandle));
#endif
    //wait first,so continue is the only need to start the next round
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(20));  //20ms delay

    //iic sensor read
    if(PowerState_lock() == 0)
    {
      I2C_state = PowerState.I2C1_state;
      INA226_state = PowerState.INA226_state;
      TMP112A_state = PowerState.TMP112_state;
      PowerState_unlock();
    }
    //if lcok fail use the last known state, which is not ideal but better than nothing
    if(I2C_state == DEVICE_ONLINE)  //work only when the bus is online
    {
      if(TMP112A_state == DEVICE_ONLINE){
        state_res_temperature = TMP112_ReadTemperature(&temperature);
        if(state_res_temperature != SENSOR_SUCCESS) {// Handle error
          temperature_err_count++;      
        }
      }else{
        state_res_temperature = SENSOR_SKIP;
      }
  
      if(INA226_state == DEVICE_ONLINE)
      {
        state_res_current = INA226_readCuttent(0.0005f,&current);     //0.0005A/per
        if(state_res_current != SENSOR_SUCCESS) {
          current_err_count++;
        }
        state_res_voltage = INA226_readVoltage(0.00125f,&voltage);    //0.00125V/per
        if(state_res_voltage != SENSOR_SUCCESS) {
          voltage_err_count++;
        }
      }else{
        state_res_current = SENSOR_SKIP;
        state_res_voltage = SENSOR_SKIP;
      }
    }

    // write to PowerState struct, reset error counters if they exceed threshold
    if(PowerState_lock() == 0){
      //error count reset
      if(temperature_err_count >= SENSOR_ERROR_THRESHOLD){
        temperature_err_count = 0;
        PowerState.TMP112_state = DEVICE_OFFLINE;
      }
      if(voltage_err_count >= SENSOR_ERROR_THRESHOLD){
        voltage_err_count = 0;
        PowerState.INA226_state = DEVICE_OFFLINE;
      }
      if(current_err_count >= SENSOR_ERROR_THRESHOLD){
        current_err_count = 0;
        PowerState.INA226_state = DEVICE_OFFLINE;
      }
      //data write
      write_time = xTaskGetTickCount();
      if(state_res_temperature == SENSOR_SUCCESS){
        PowerState.now_temperature = temperature;
        PowerState.last_update_time_temperature = write_time;
      }
      if(state_res_voltage == SENSOR_SUCCESS){
        PowerState.now_voltage = voltage;
        PowerState.last_update_time_voltage = write_time;
      }
      if(state_res_current == SENSOR_SUCCESS){
        PowerState.now_current = current;
        PowerState.last_update_time_current = write_time;
      }
      PowerState_unlock();
    }

  }
  /* USER CODE END sensorRead */
}

/* USER CODE BEGIN Header_sensor_err_handle */
/**
* @brief Function implementing the sensor_err_hand thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_sensor_err_handle */
void sensor_err_handle(void *argument)
{
  /* USER CODE BEGIN sensor_err_handle */
#ifdef DEV_UART_DEBUG
  int run_count = 0;
#endif
  TickType_t lastWakeTime = xTaskGetTickCount();  //period base
  struct PowerStatus_t PowerState_copy;   //local copy to avoid holding the lock too long
  /* Infinite loop */
  for(;;)
  {
  #ifdef DEV_UART_DEBUG
    UART_Printf("sensor_err_handle:%d\r\n",run_count);
    UART_Printf("sensor_err_handle_stack:%d\r\n",uxTaskGetStackHighWaterMark(sensor_err_handHandle));
  #endif

    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(200));  //200ms delay

    if(PowerState_lock() == 0){
      PowerState_copy = PowerState;
      if(PowerState_copy.INA226_state == DEVICE_OFFLINE && PowerState_copy.TMP112_state == DEVICE_OFFLINE){
        PowerState_copy.I2C1_state = DEVICE_OFFLINE;   //if both sensors are offline, the bus is likely offline too
        PowerState.I2C1_state = DEVICE_OFFLINE;   //update the shared struct immediately
      }

      if(PowerState_copy.SH1106_state == DEVICE_OFFLINE){
        PowerState_copy.I2C2_state = DEVICE_OFFLINE;
        PowerState.I2C2_state = DEVICE_OFFLINE;
      }
      PowerState_unlock();
    }else{
      continue;   //lock not acquired: skip this round
    }

    if(PowerState_copy.I2C1_state == DEVICE_OFFLINE){
      if(Sensors_bus_restart() == 0){
        PowerState_copy.I2C1_state = DEVICE_ONLINE;   //try to recover the bus
      }
    }
    if(PowerState_copy.INA226_state == DEVICE_OFFLINE){
      if(INA226_init(INA226_init_data) == SENSOR_SUCCESS){
        PowerState_copy.INA226_state = DEVICE_ONLINE;   //try to recover the INA226
      }
    }

    if(PowerState_copy.TMP112_state == DEVICE_OFFLINE){
      float temp;
      if(TMP112_ReadTemperature(&temp) == SENSOR_SUCCESS){
        PowerState_copy.TMP112_state = DEVICE_ONLINE;   //try to recover the TMP112
      }
    }

    if(PowerState_copy.I2C2_state == DEVICE_OFFLINE){
      if(I2C2_bus_restart() == 0){
        PowerState_copy.I2C2_state = DEVICE_ONLINE;
      }
    }

    if(PowerState_copy.SH1106_state == DEVICE_OFFLINE && PowerState_copy.I2C2_state == DEVICE_ONLINE){
      u8g2Init(&u8g2);
      PowerState_copy.SH1106_state = DEVICE_ONLINE;
    }


    if(PowerState_lock() == 0){
      if(PowerState_copy.I2C1_state == DEVICE_ONLINE){
        PowerState.I2C1_state = PowerState_copy.I2C1_state;
      }
      if(PowerState_copy.INA226_state == DEVICE_ONLINE){
        PowerState.INA226_state = PowerState_copy.INA226_state;
      }
      if(PowerState_copy.TMP112_state == DEVICE_ONLINE){
        PowerState.TMP112_state = PowerState_copy.TMP112_state;
      }
      if(PowerState_copy.I2C2_state == DEVICE_ONLINE){
        PowerState.I2C2_state = PowerState_copy.I2C2_state;
      }
      if(PowerState_copy.SH1106_state == DEVICE_ONLINE){
        PowerState.SH1106_state = PowerState_copy.SH1106_state;
      }
      PowerState_unlock();
    }//lock not acquired: skip this round
  }
  /* USER CODE END sensor_err_handle */
}

/* USER CODE BEGIN Header_screen_show */
/**
* @brief Function implementing the screen thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_screen_show */
void screen_show(void *argument)
{
  /* USER CODE BEGIN screen_show */
#ifdef DEV_UART_DEBUG
  int run_count = 0;
#endif
  char buffer[20] = {0};
  struct PowerStatus_t PowerState_copy_last;
  struct PowerStatus_t PowerState_copy;
  TickType_t lastWakeTime = xTaskGetTickCount();  //period base
  int first_flag = 0;
  /* Infinite loop */
  for(;;)
  {  
  #ifdef DEV_UART_DEBUG
    UART_Printf("screen_show:%d\r\n",run_count);
    UART_Printf("screen_show_stack:%d\r\n",uxTaskGetStackHighWaterMark(screenHandle));
  #endif

    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(400));  //400ms delay
    if(PowerState_lock() == 0){
      PowerState_copy = PowerState;
      PowerState_unlock();
    }else{
      continue;
    }

    if(first_flag == 0){
      u8g2_ClearBuffer(&u8g2);
      u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
	    sprintf(buffer,"Vset :%05.2fV",PowerState_copy.set_voltage);
      u8g2_DrawStr(&u8g2, 0, 10, buffer); 
	    sprintf(buffer,"Iset :%05.2fA",PowerState_copy.set_current);
      u8g2_DrawStr(&u8g2, 0, 18, buffer);
	    sprintf(buffer,"Vout :%05.2fV",PowerState_copy.now_voltage);
      u8g2_DrawStr(&u8g2, 0, 30, buffer);
	    sprintf(buffer,"Iout :%05.2fA",PowerState_copy.now_current);
      u8g2_DrawStr(&u8g2, 0, 38, buffer);
	    sprintf(buffer,"T:%03.1fC",PowerState_copy.now_temperature);
      u8g2_DrawStr(&u8g2, 0, 50, buffer);
      
	    u8g2_SendBuffer(&u8g2);
      PowerState_copy_last = PowerState_copy;
	    first_flag = 1;
      continue;
    }

    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
    sprintf(buffer,"Vset :%05.2fV",PowerState_copy.set_voltage);
    u8g2_DrawStr(&u8g2, 0, 10, buffer); 
    sprintf(buffer,"Iset :%05.2fA",PowerState_copy.set_current);
    u8g2_DrawStr(&u8g2, 0, 18, buffer);
    sprintf(buffer,"Vout :%05.2fV",PowerState_copy.now_voltage);
    u8g2_DrawStr(&u8g2, 0, 30, buffer);
    sprintf(buffer,"Iout :%05.2fA",PowerState_copy.now_current);
    u8g2_DrawStr(&u8g2, 0, 38, buffer);
    sprintf(buffer,"T:%03.1fC",PowerState_copy.now_temperature);
    u8g2_DrawStr(&u8g2, 0, 50, buffer);

    u8g2_SendBuffer(&u8g2);
    PowerState_copy_last = PowerState_copy;
  }
  /* USER CODE END screen_show */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_8)
  {
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    osThreadFlagsSet(buttomHandle,(uint32_t)0x00000001U);
  }

  if(
    (GPIO_Pin == GPIO_PIN_13)||
    (GPIO_Pin == GPIO_PIN_14)||
    (GPIO_Pin == GPIO_PIN_15)
  )
  {
    HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    osThreadFlagsSet(buttomHandle,(uint32_t)0x00000001U);
  }
}
/* I2C 总线互斥：覆盖 sensors_dev.c 里的弱函数实现
 * 返回 0 = 已持锁；非 0 = 没取到，设备层会放弃本次总线访问并返回 ERR_BUSY
 * 互斥量在 MX_FREERTOS_Init 里创建，创建前（内核尚未启动）句柄为空，直接放行 */
int I2C_dev_lock(sensor_dev_i2c_t i2c)
{
  switch (i2c)
  {
  case SENSOR_DEV_I2C1:
    if(I2CAccessHandle == NULL) return 0;
    return (osMutexAcquire(I2CAccessHandle, I2C_LOCK_TIMEOUT_TICKS) == osOK) ? 0 : -1;
  case SENSOR_DEV_I2C2:
    break;
  
  default:
    return -1;
  }
  return -1;
}

void I2C_dev_unlock(sensor_dev_i2c_t i2c)
{
  switch (i2c)
  {
  case SENSOR_DEV_I2C1:
  if(I2CAccessHandle != NULL)
    osMutexRelease(I2CAccessHandle);
    break;
  case SENSOR_DEV_I2C2:
    break;
  
  default:
    break;
  }
}

int I2C_sensor_dev_lock(void)
{
  if(I2CAccessHandle == NULL)
  {
    return 0;                                   //single threaded before the scheduler runs
  }
  return (osMutexAcquire(I2CAccessHandle, I2C_LOCK_TIMEOUT_TICKS) == osOK) ? 0 : -1;
}

void I2C_sensor_dev_unlock(void)
{
  if(I2CAccessHandle != NULL)
  {
    osMutexRelease(I2CAccessHandle);
  }
}

/*powerstate mutex functions*/
/* 与 powerMaster.c 里的弱函数配对：返回 0 = 已持锁，非 0 = 没取到 */
int PowerState_lock(void)
{
  if(PowerStateAcssessHandle == NULL)
  {
    return 0;                                   //single threaded before the scheduler runs
  }
  return (osMutexAcquire(PowerStateAcssessHandle, POWERSTATE_LOCK_TIMEOUT_TICKS) == osOK) ? 0 : -1;
}

int PowerState_unlock(void)
{
  if(PowerStateAcssessHandle == NULL)
  {
    return 0;
  }
  return (osMutexRelease(PowerStateAcssessHandle) == osOK) ? 0 : -1;
}

//uart_dev.h
int UART_lock_simple(void){
  if(dev_uart1Handle == NULL)
    return 0;
  return (osMutexAcquire(dev_uart1Handle,POWERSTATE_LOCK_TIMEOUT_TICKS)==osOK) ? 0 : -1;
}
void UART_unlock_simple(void){
  if(dev_uart1Handle != NULL)
	  osMutexRelease(dev_uart1Handle);
}
/* USER CODE END Application */

