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
/* PID 状态在 main.c 的 USER CODE 4 里定义 */
extern float integral;
extern float last_error;
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
  .priority = (osPriority_t) osPriorityRealtime6,
};
/* Definitions for PIDv */
osThreadId_t PIDvHandle;
const osThreadAttr_t PIDv_attributes = {
  .name = "PIDv",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime7,
};
/* Definitions for power_info */
osMessageQueueId_t power_infoHandle;
const osMessageQueueAttr_t power_info_attributes = {
  .name = "power_info"
};
/* Definitions for voltage_set */
osMessageQueueId_t voltage_setHandle;
const osMessageQueueAttr_t voltage_set_attributes = {
  .name = "voltage_set"
};
/* Definitions for dataRead_TIM */
osTimerId_t dataRead_TIMHandle;
const osTimerAttr_t dataRead_TIM_attributes = {
  .name = "dataRead_TIM"
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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
uint16_t pid_calculate(float target_voltage, float actual_voltage);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void buttomTask(void *argument);
void Vlotage_pid(void *argument);
void Callback01(void *argument);

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

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of dataRead_TIM */
  dataRead_TIMHandle = osTimerNew(Callback01, osTimerPeriodic, NULL, &dataRead_TIM_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  osTimerStart(dataRead_TIMHandle,pdMS_TO_TICKS(10));
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of power_info */
  power_infoHandle = osMessageQueueNew (16, sizeof(float), &power_info_attributes);

  /* creation of voltage_set */
  voltage_setHandle = osMessageQueueNew (16, sizeof(int16_t), &voltage_set_attributes);

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
  const int16_t upV = 1;
  const int16_t downV = -1;
  osStatus_t result;
  /* Infinite loop */
  for(;;)
  {
    osThreadFlagsWait((uint32_t)0x00000001U,osFlagsWaitAny,osWaitForever);//wait EXIT
    osDelay(pdMS_TO_TICKS(20));   //
    result = osMutexAcquire(PowerStateAcssessHandle,pdMS_TO_TICKS(5));

    temp = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8);  //power en switch
    if(temp == GPIO_PIN_RESET){
      temp = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9);
      if(result == osOK){
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
      }else{
        //give up
      }
      goto final;
    }

    temp = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13);   //vlotage limit&current limit switch
    if (temp == GPIO_PIN_RESET){

    }

    temp = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);   //dowm,vlotage first
    if (temp == GPIO_PIN_RESET){
      osMessageQueuePut(voltage_setHandle,&downV,0,0);
    }

    temp = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);    //up
    if (temp == GPIO_PIN_RESET){
      osMessageQueuePut(voltage_setHandle,&upV,0,0);
    }

    final:    //release mutex
    result = osMutexRelease(PowerStateAcssessHandle);
    #if DEBUG
    if(result != osOK){
      HAL_UART_Transmit(&huart2,"MUTEX:PowerStateAcssess realse ERR\r\n",29,HAL_MAX_DELAY);
    }
    #endif

    osDelay(pdMS_TO_TICKS(180));
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
  float set_voltage=6.0f;
  float new_voltage;  //V
  uint16_t now_pulse = 500;
  uint16_t pwm_pulse;
  int16_t voltage_change;
  osStatus_t osRes;
  /* Infinite loop */
  for(;;)
  {
    osMessageQueueGet(power_infoHandle,&new_voltage,0,osWaitForever);

    osRes = osMutexAcquire(PowerStateAcssessHandle,0);
    if(osRes == osOK){
      if(PowerState.en_statu == PWR_EN_OFF){  //if close ,clear pid pram
        integral = 0;
        last_error = 0;
      }
    }

    osMutexRelease(PowerStateAcssessHandle);

    if(osMessageQueueGet(voltage_setHandle,&voltage_change,0,0) == osOK)  //set_voltage change
      set_voltage += voltage_change;

    if(new_voltage < 0.0f)
    {
      integral = 0;
      last_error = 0;
      continue;
    }

    // if((set_voltage - new_voltage) > 0.5 || (set_voltage - new_voltage) < -0.5){
    //   osTimerStart(dataRead_TIMHandle,pdMS_TO_TICKS(10));
    // }
    if((set_voltage - new_voltage) < 0.00 && (set_voltage - new_voltage) > -0.03) //if deviation is small enough skip pid
    {
      // count++;
      // if(count >= 5){
      //   count = 0;
      //   osTimerStart(dataRead_TIMHandle,pdMS_TO_TICKS(100));
      // }
      continue;
    }

    pwm_pulse = pid_calculate(set_voltage,new_voltage);
    pwm_pulse += now_pulse;
    if(pwm_pulse > 800)pwm_pulse = 800;
    if(pwm_pulse < 300)pwm_pulse = 300;
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,pwm_pulse);
  }
  /* USER CODE END Vlotage_pid */
}

/* Callback01 function */
void Callback01(void *argument)
{
  /* USER CODE BEGIN Callback01 */
  float temperature;
  float voltage;
  float current;
  float last_vlotage;

  //iic sensor read
  TMP112_ReadTemperature(&temperature);
  //error process (later add)
  INA226_readCuttent(0.0005f,&current);    //0.0005A/per

  INA226_readVoltage(0.00125f,&voltage);  //0.00125V/per

  //this function just get data,later will create a new callbcak to use uart report any data in any format

  last_vlotage = voltage;


  int res = osMessageQueuePut(power_infoHandle,&last_vlotage,0,0);


  /* USER CODE END Callback01 */
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
/* I2C 总线互斥：覆盖 sensors_dev.c 里的弱函数空实现
 * 互斥量在 MX_FREERTOS_Init 里创建，创建前（内核尚未启动）句柄为空，直接放行 */
void I2C_sensor_dev_lock(void)
{
  if(I2CAccessHandle != NULL)
  {
    osMutexAcquire(I2CAccessHandle, osWaitForever);
  }
}

void I2C_sensor_dev_unlock(void)
{
  if(I2CAccessHandle != NULL)
  {
    osMutexRelease(I2CAccessHandle);
  }
}
/* USER CODE END Application */

