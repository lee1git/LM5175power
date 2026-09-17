/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "sensors.h"
#include "powerMaster.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define PWM_VLOTAGE_TOP       (324U)     //when pluse is 324,zhe Vout is 15V
#define PWM_VLOTAGE_buttom    (758U)     //when pluse is 758,zhe Vout is 1V
#define PWM_STEP              (uint16_t)((PWM_VLOTAGE_buttom-PWM_VLOTAGE_TOP)/14) //1V step

#define MY_DEBUG    0x00      //0x00 nodebug ;0x01 debug on
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

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
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void *argument);
void buttomTask(void *argument);
void Vlotage_pid(void *argument);
void Callback01(void *argument);

static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
struct INA226_init_t INA226_init_data={INA226_reg_10A_6mOhm};
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  //500uA/per 10A/max
  if(INA226_init(&hi2c1,INA226_init_data) == SENSOR_OK){ 
    HAL_UART_Transmit(&huart2,(uint8_t*)"INA:initOK\r\n",12,HAL_MAX_DELAY);
  }
  else{
    HAL_UART_Transmit(&huart2,(uint8_t*)"INA:ERR\r\n",9,HAL_MAX_DELAY);
  }

  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);

  PowerStateInit(PWR_EN_OFF,6.0f,10.0f);  
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
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

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI9_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 3;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 937;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 3;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 324;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/*************************************************
                  USER FUNC DEFINES
*************************************************/
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

//pid
float Kp = -30.5;    //比例系数
float Ki = -1.8;   //积分系数
float Kd = -0.0;     //微分系数

// PID 状态
float integral = 0;
float last_error = 0;

// PID 计算函数
// 输入：设定电压 target_voltage，实际电压 actual_voltage
// 输出：PWM pulse 值
uint16_t pid_calculate(float target_voltage, float actual_voltage) {
    // 1. 计算误差
    float error = target_voltage - actual_voltage;

    // 2. 积分项累加
    integral += error;

    // 3. 微分项计算
    float derivative = error - last_error;

    // 4. PID 输出
    float output = Kp * error + Ki * integral + Kd * derivative;

    // 5. 更新上次误差
    last_error = error;

    // 6. 把输出映射到 PWM pulse 范围
    // 假设 PWM 周期为 1000，输出范围为 0~1000
    int pulse = (int)(output);
    // if (pulse < 300) pulse = 300;
    // if (pulse > 800) pulse = 800;

    return (uint16_t)pulse;   //这里希望把pulse变为一个在基准值上改动的值
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
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

  sensor_fstate_t sensor_res;

  //iic sensor read
  sensor_res = TMP112_ReadTemperature(&hi2c1,&temperature);
  //error process (later add)
  sensor_res = INA226_readCuttent(&hi2c1,0.0005f,&current);    //0.0005A/per

  sensor_res = INA226_readVoltage(&hi2c1,0.00125f,&voltage);  //0.00125V/per

  //this function just get data,later will create a new callbcak to use uart report any data in any format

  last_vlotage = voltage;


  int res = osMessageQueuePut(power_infoHandle,&last_vlotage,0,0);


  /* USER CODE END Callback01 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
