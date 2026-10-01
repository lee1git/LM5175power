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
#include "iwdg.h"
#include "pid.h"
#include "screen.h"
#include "buttom_detect.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
  /* u8g2 handle */
  //INA226 init data
  struct sensor_INA226_init INA226_init_data = {INA226_calibration_10A_6mOhm};
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
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for screen */
osThreadId_t screenHandle;
const osThreadAttr_t screen_attributes = {
  .name = "screen",
  .stack_size = 192 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for iwdog_feed */
osThreadId_t iwdog_feedHandle;
const osThreadAttr_t iwdog_feed_attributes = {
  .name = "iwdog_feed",
  .stack_size = 64 * 4,
  .priority = (osPriority_t) osPriorityRealtime7,
};
/* Definitions for buttomDetectTas */
osThreadId_t buttomDetectTasHandle;
const osThreadAttr_t buttomDetectTas_attributes = {
  .name = "buttomDetectTas",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime1,
};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
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
void IWDOG_feed(void *argument);
void buttom_detect(void *argument);

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

  /* Create the queue(s) */
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew (8, sizeof(buttom_msg_pass_t), &myQueue01_attributes);

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

  /* creation of iwdog_feed */
  iwdog_feedHandle = osThreadNew(IWDOG_feed, NULL, &iwdog_feed_attributes);

  /* creation of buttomDetectTas */
  buttomDetectTasHandle = osThreadNew(buttom_detect, NULL, &buttomDetectTas_attributes);

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
  buttom_msg_pass_t process_msg;
  /* Infinite loop */
  for(;;)
  {
    osMessageQueueGet(myQueue01Handle,&process_msg,0,osWaitForever);

    if(PowerState_lock() == 0)
    {
      switch (process_msg.key_f)
      {
      case BUTTOM_KEY_ON_OFF_F:
        if(process_msg.buttom_type == BUTTOM_PRESS){
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
        }
        break;
      case BUTTOM_KEY_UP_F:
        if(PowerState.control_mode == PM_CONTROL_MODE_VOLTAGE)  //cv
        {
          if(process_msg.buttom_type == BUTTOM_PRESS){
            if(PowerState.set_voltage + POWER_VOLTAGE_UP_STEP > POWER_MAX_VOLTAGE)
              PowerState.set_voltage = POWER_MAX_VOLTAGE;
            else
              PowerState.set_voltage += POWER_VOLTAGE_UP_STEP;
          }
        }
        break;
      case BUTTOM_KEY_DOWN_F:
        if(PowerState.control_mode == PM_CONTROL_MODE_VOLTAGE)  //cv
        {
          if(process_msg.buttom_type == BUTTOM_PRESS){
            if(PowerState.set_voltage + POWER_VOLTAGE_DOWN_STEP < POWER_MIN_VOLTAGE)
              PowerState.set_voltage = POWER_MIN_VOLTAGE;
            else
              PowerState.set_voltage += POWER_VOLTAGE_DOWN_STEP;
          }
        }
        break;
      case BUTTOM_KEY_MODESWITCH_F:
        if(process_msg.buttom_type == BUTTOM_PRESS){
          PowerState.control_mode = ((PowerState.control_mode == PM_CONTROL_MODE_CURRENT) ? PM_CONTROL_MODE_VOLTAGE : PM_CONTROL_MODE_CURRENT);
        }
        break;
      
      default:
        break;
      }
      PowerState_unlock();
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

  //time trace
#ifdef CYCLE_DETECT_PID
#if CYCLE_DETECT_PID == ON
  float pid_min = 999999,pid_max = 0, pid_sum = 0;
  uint32_t pid_count = 0;
#endif
#endif

  PI_data_typedef pi_data;
  PI_init(&pi_data, PI_CV_KP, PI_CV_KI, PI_CV_INTEGRAL_LIMIT, PI_CV_INTEGRAL_DEADZONE);

  float set_voltage = POWER_SETUP_VOLTAGE_SET;
  float new_voltage;  //V
  struct PowerStatus_t powerstate_copy;
  int32_t now_pulse = VOLTAGE_TO_PWM_PULSE(POWER_SETUP_VOLTAGE_SET);       //initial PWM pulse width, just a magic number
  int32_t pwm_pulse;
  TickType_t lastWakeTime = xTaskGetTickCount();   //period base, taken once outside the loop
  uint8_t check_over = 1;
  /* Infinite loop */
  for(;;)
  {
    check_over = 1;     //assume all checks pass, cleared on any fault
    
    if(PowerState_lock() == 0){
      if(PowerState.now_temperature > TEMPERATUE_LIMIT)
        PowerState.en_statu = PWR_EN_OFF;   //temperature is higher than the limit,disable the power
      powerstate_copy = PowerState;
      PowerState_unlock();
    }else{
      //lock not acquired: leave PowerState untouched this round
      check_over = 0;
    }

    if(
      powerstate_copy.en_statu == PWR_EN_OFF ||
      powerstate_copy.INA226_state == DEVICE_OFFLINE ||
      new_voltage < 0.0f ||
      check_over != 1
    )// wrong
    {
      PI_clear_integral(&pi_data);
    }
    else
    {
      //time trace
#ifdef CYCLE_DETECT_PID
#if CYCLE_DETECT_PID == ON
      uint32_t start = DWT->CYCCNT;
#endif
#endif

      // if(check_over == 1){          //all checks passed: this round may drive the PWM
      if(powerstate_copy.control_mode == PM_CONTROL_MODE_VOLTAGE)
        pwm_pulse = (int32_t)f_PI_calcu_keep(&pi_data,powerstate_copy.set_voltage,powerstate_copy.now_voltage);
      else if(powerstate_copy.control_mode == PM_CONTROL_MODE_CURRENT)
        pwm_pulse = (int32_t)f_PI_calcu_keep(&pi_data,powerstate_copy.set_current,powerstate_copy.now_current);

        if(now_pulse+pwm_pulse > PWM_VOLTAGE_PULSE_MAX)now_pulse = PWM_VOLTAGE_PULSE_MAX;
        else if(now_pulse+pwm_pulse < PWM_VOLTAGE_PULSE_MIN)now_pulse = PWM_VOLTAGE_PULSE_MIN;
        else now_pulse += pwm_pulse; 
  
        __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,now_pulse);
      // }
      //time trace
#ifdef CYCLE_DETECT_PID
#if CYCLE_DETECT_PID == ON
      uint32_t end = DWT->CYCCNT;
      uint32_t cycles = end-start;
      float us = (float)cycles / (SystemCoreClock / 1000000.0f);

      if(us < pid_min)pid_min = us;
      if(us > pid_max)pid_max = us;
      pid_sum += us;
      pid_count++;
      UART_Printf("PID:%.1f us\r\n",us);
      if(pid_count % 100 == 0)
      UART_Printf("PID min:%.1f, avg:%.1f, max:%.1f us\r\n",pid_min,pid_sum/pid_count,pid_max);
#endif
#endif
    }

  #ifdef DEV_UART_DEBUG
    UART_Printf("voltage_pid:%d\r\n",run_count++);
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
  //time trace
#ifdef CYCLE_DETECT_I2C1
#if CYCLE_DETECT_I2C1 == ON
  float i2c_min = 999999,i2c_max = 0, i2c_sum = 0;
  uint32_t i2c_count = 0;
#endif
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
  INA226_init(INA226_init_data);
  for(;;)
  {
#ifdef DEV_UART_DEBUG
    UART_Printf("sensorRead:%d\r\n",run_count);
    UART_Printf("sensorRead_stack:%d\r\n",uxTaskGetStackHighWaterMark(sensorTaskHandle));
#endif
    //wait first,so continue is the only need to start the next round
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(20));  //20ms delay

    I2C_state = DEVICE_OFFLINE;
    INA226_state = DEVICE_OFFLINE;
    TMP112A_state = DEVICE_OFFLINE;
    state_res_temperature = SENSOR_ERR_DEV;
    state_res_voltage = SENSOR_ERR_DEV;
    state_res_current = SENSOR_ERR_DEV;
    //iic sensor read
    if(PowerState_lock() == 0)
    {
      I2C_state = PowerState.I2C1_state;
      INA226_state = PowerState.INA226_state;
      TMP112A_state = PowerState.TMP112_state;
      PowerState_unlock();
    }

    //time trace
#ifdef CYCLE_DETECT_I2C1
#if CYCLE_DETECT_I2C1 == ON
    uint32_t start = DWT->CYCCNT;
#endif
#endif
    
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

    //time trace
#ifdef CYCLE_DETECT_I2C1
#if CYCLE_DETECT_I2C1 == ON
    uint32_t end = DWT->CYCCNT;
    uint32_t cycles = end-start;
    float us = (float)cycles / (SystemCoreClock / 1000000.0f);

    if(us < i2c_min)i2c_min = us;
    if(us > i2c_max)i2c_max = us;
    i2c_sum += us;
    i2c_count++;
    UART_Printf("I2C:%.1f us\r\n",us);
    if(i2c_count % 100 == 0)
      UART_Printf("I2C min:%.1f, avg:%.1f, max:%.1f us\r\n",i2c_min,i2c_sum/i2c_count,i2c_max);
#endif
#endif

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

//time trace
#ifdef CYCLE_DETECT_SCREEN
#if CYCLE_DETECT_SCREEN == ON
  float screen_min = 999999,screen_max = 0, screen_sum = 0;
  uint32_t screen_count = 0;
#endif
#endif

  struct PowerStatus_t PowerState_copy_last;
  struct PowerStatus_t PowerState_copy;
  TickType_t lastWakeTime = xTaskGetTickCount();  //period base
  int first_flag = 0;
  /* Infinite loop */
  screen_init();
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

    //time trace
#ifdef CYCLE_DETECT_SCREEN
#if CYCLE_DETECT_SCREEN == ON
    uint32_t start = DWT->CYCCNT;
#endif
#endif

    screen_clear_buffer();
    screen_set_small_font();
    screen_set_data_print(&PowerState_copy);
    screen_real_data_print(&PowerState_copy);

    screen_send_buffer();
    PowerState_copy_last = PowerState_copy;

    //time trace
#ifdef CYCLE_DETECT_SCREEN
#if CYCLE_DETECT_SCREEN == ON
    uint32_t end = DWT->CYCCNT;
    uint32_t cycles = end-start;
    float us = (float)cycles / (SystemCoreClock / 1000000.0f);

    if(us < screen_min)screen_min = us;
    if(us > screen_max)screen_max = us;
    screen_sum += us;
    screen_count++;
    UART_Printf("screen:%.1f us\r\n",us);
    if(screen_count % 100 == 0)
      UART_Printf("Screen min:%.1f, avg:%.1f, max:%.1f us\r\n",screen_min,screen_sum/screen_count,screen_max);
#endif
#endif
  }
  /* USER CODE END screen_show */
}

/* USER CODE BEGIN Header_IWDOG_feed */
/**
* @brief Function implementing the iwdog_feed thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IWDOG_feed */
void IWDOG_feed(void *argument)
{
  /* USER CODE BEGIN IWDOG_feed */
  TickType_t lastWakeTime = xTaskGetTickCount();  //period base
  /* Infinite loop */
  for(;;)
  {
    //  set feed time 2s
    #ifndef DEV_UART_DEBUG
      HAL_IWDG_Refresh(&hiwdg);
    #endif
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(IWDOG_FEED_TIME_MS));
  }
  /* USER CODE END IWDOG_feed */
}

/* USER CODE BEGIN Header_buttom_detect */
/**
* @brief Function implementing the buttomDetectTas thread.
* @param argument: Not used
* @retval None
*/
#define key_detect_num  4   //numbers of key on spy
/* USER CODE END Header_buttom_detect */
void buttom_detect(void *argument)
{
  /* USER CODE BEGIN buttom_detect */
  buttom_msg_pass_t msg_pass;
  uint32_t flags;
  buttom_msg_t bmt_key_onoff;
  buttom_msg_t bmt_key_up;
  buttom_msg_t bmt_key_down;
  buttom_msg_t bmt_key_mode_switch;

  buttom_init(&bmt_key_onoff, GPIOA, GPIO_PIN_8, BUTTOM_KEY_ON_OFF_F);
  buttom_init(&bmt_key_up, GPIOB, GPIO_PIN_15, BUTTOM_KEY_UP_F);
  buttom_init(&bmt_key_down, GPIOB, GPIO_PIN_14, BUTTOM_KEY_DOWN_F);
  buttom_init(&bmt_key_mode_switch, GPIOB, GPIO_PIN_13, BUTTOM_KEY_MODESWITCH_F);

  buttom_msg_t *bmt_arr[key_detect_num] = {&bmt_key_onoff, &bmt_key_up, &bmt_key_down, &bmt_key_mode_switch};

  TickType_t lastWakeTime = xTaskGetTickCount();  //period base
  for(;;)
  {
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(BUTTOM_DETECT_CYCLE_MS));
    flags = osThreadFlagsGet();
    if(flags != 0U)
    {
      for(int i=0;i<key_detect_num;i++) //start to detect whitch edge
      {
        if(((flags & bmt_arr[i]->buttom_flag) != 0) && bmt_arr[i]->on_use == 0){
          osThreadFlagsClear(bmt_arr[i]->buttom_flag);
          bmt_arr[i]->on_use = 1;
        }
      }
    }

    for(int i=0;i<key_detect_num;i++)
    {
      if(bmt_arr[i]->on_use == 1){// count how many counts and high levels
        if(HAL_GPIO_ReadPin(bmt_arr[i]->gpio,bmt_arr[i]->gpio_pin) == GPIO_PIN_RESET){
          bmt_arr[i]->count_time++;
        }else{
          bmt_arr[i]->count_time++;
          bmt_arr[i]->high_times++;
        }
      }

      if(bmt_arr[i]->count_time >= BUTTOM_OVER_DETECT_TIMES){
        if(bmt_arr[i]->high_times > BUTTOM_OVER_DETECT_TIMES/2){    //high > count/2 is high
          buttom_state(bmt_arr[i],BUTTOM_ACTION_UP);
        }else{
          buttom_state(bmt_arr[i],BUTTOM_ACTION_DOWN);
        }
        bmt_arr[i]->count_time = 0;   //clear for next detect
        bmt_arr[i]->high_times = 0;
        bmt_arr[i]->on_use = 0; 
      }else{
        msg_pass.buttom_type = buttom_state(bmt_arr[i],BUTTOM_ACTION_NONE);
        msg_pass.key_f = bmt_arr[i]->buttom_flag;
        if(
          msg_pass.buttom_type == BUTTOM_PRESS ||
          msg_pass.buttom_type == BUTTOM_LPRESS ||
          msg_pass.buttom_type == BUTTOM_DPRESS 
        )
        osMessageQueuePut(myQueue01Handle,&msg_pass,0,pdMS_TO_TICKS(1));
      }
    }
  }
  /* USER CODE END buttom_detect */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_8)
  {
    HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    osThreadFlagsSet(buttomDetectTasHandle,BUTTOM_KEY_ON_OFF_F);
  }

  if((GPIO_Pin == GPIO_PIN_13))
  {
    HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    osThreadFlagsSet(buttomDetectTasHandle,BUTTOM_KEY_MODESWITCH_F);
  }

  if(GPIO_Pin == GPIO_PIN_14)
  {
    HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    osThreadFlagsSet(buttomDetectTasHandle,BUTTOM_KEY_DOWN_F);
  }

  if(GPIO_Pin == GPIO_PIN_15)
  {
    HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
    osThreadFlagsSet(buttomDetectTasHandle,BUTTOM_KEY_UP_F);
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

