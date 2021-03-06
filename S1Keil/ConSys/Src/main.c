/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart5_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

osThreadId UART1TaskHandle;
osThreadId UART2TaskHandle;
osThreadId UART3TaskHandle;
osThreadId UART4TaskHandle;
osThreadId DisplayTaskHandle;
/* USER CODE BEGIN PV */

u8 SystemBegin = 0;

//Printf指向的指针变量
volatile UART_HandleTypeDef* UartHandle = &huart1;

//串口1RX口
volatile u8 U1_RX_Len = 0;
volatile u8 U1_RX_EndFlag = 0;
u8 U1_RX_Buffer[70];
char U1_RX_BufferSize = 70;
u8* U1_RX_Position = NULL;
//串口2RX口
volatile u8 U2_RX_Len = 0;
volatile u8 U2_RX_EndFlag = 0;
u8 U2_RX_Buffer[12];
char U2_RX_BufferSize = 12;
u8* U2_RX_Position = NULL;
//串口3RX口
volatile u8 U3_RX_Len = 0;
volatile u8 U3_RX_EndFlag = 0;
u8 U3_RX_Buffer[50];
char U3_RX_BufferSize = 50;
u8* U3_RX_Position = NULL;
//串口4RX口
volatile u8 U4_RX_Len = 0;
volatile u8 U4_RX_EndFlag = 0;
u8 U4_RX_Buffer[40];
char U4_RX_BufferSize = 40;
u8* U4_RX_Position = 0;

u8 U2_TX_Begin[30] = "$9:100:100:100:1100:1500:1500%";

float ControlTemperture = 0;
u16 Pitch = 0;
u16 Yaw = 0;
u16 Roll = 0;
u16 GeomagneticAngle = 0;
uint32_t WaterTemperture = 0;
uint32_t Depth = 0;
uint32_t ADCCache[4] = { 0 };
u16 ADCResult[4] = { 0 };
u16 Light = 0;
u16 PTZ = 0;
u16 Crab = 0;
int ControlWater[2] = { 6, 0 };
int PowerWater[4] = { 6, 1, 6, 1 };
u8 ScreenFinish[3] = { 0 };
u8 SendBuffer[5] = { 0x55, 0, 0, 0, 0x00 };
u8 SendByte = 0;

u8 SendDepth[5] = { 0 };
u8 SendWaterT[4] = { 0 };
u8 SendGAngle[3] = { 0 };
u8 SendPitch[3] = { 0 };
u8 SendPower[4] = { 0 };
u8 Num[11][2] = 
{
	{ 0xa3, 0xb0 },
	{ 0xa3, 0xb1 },
	{ 0xa3, 0xb2 },
	{ 0xa3, 0xb3 },
	{ 0xa3, 0xb4 },
	{ 0xa3, 0xb5 },
	{ 0xa3, 0xb6 },
	{ 0xa3, 0xb7 },
	{ 0xa3, 0xb8 },
	{ 0xa3, 0xb9 },
	{ 0xa3, 0xae }
};
u8 SendPowerWater[3][6][2] =  
{
{
	{ 0xb5, 0xe7 },
	{ 0xd4, 0xb4 },
	{ 0xb2, 0xd6 },
	{ 0xa3, 0xb1 },
	{ 0xc2, 0xa9 },
	{ 0xcb, 0xae }
},
{
	{ 0xb5, 0xe7 },
	{ 0xd4, 0xb4 },
	{ 0xb2, 0xd6 },
	{ 0xa3, 0xb2 },
	{ 0xc2, 0xa9 },
	{ 0xcb, 0xae } 
},
{
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 }
}
};
u8 SendControlWater[2][5][2] =
{
{
	{ 0xbf, 0xd8 },
	{ 0xd6, 0xc6 },
	{ 0xb2, 0xd6 },
	{ 0xc2, 0xa9 },
	{ 0xcb, 0xae } 
},
{
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 },
	{ 0xa1, 0xa1 }
}
};
u8 ControlWaterOn[25] = { 0x55, 0x07, 0x04, 0x01, 0x00, 0x55, 0x0A, 0x00, 0x00, 0x00, 0x55, 0x31, 0x05, 0x01, 0x00, 0xBF, 0xD8, 0xD6, 0xC6, 0xB2, 0xD6, 0xC2, 0xA9, 0xCB, 0xAE };
u8 ControlWaterOff[55] = { 0x55, 0x07, 0x04, 0x01, 0x00, 0x55, 0x0A, 0x00, 0x00, 0x00, 0x55, 0x31, 0x14, 0x01, 0x00, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1 };
u8 PowerWaterOn[2][27] = {
	{ 0x55, 0x07, 0x05, 0x01, 0x00, 0x55, 0x0A, 0x00, 0x00, 0x00, 0x55, 0x31, 0x06, 0x01, 0x00, 0xB5, 0xE7, 0xD4, 0xB4, 0xB2, 0xD6, 0xA3, 0xB1, 0xC2, 0xA9, 0xCB, 0xAE },
	{ 0x55, 0x07, 0x06, 0x01, 0x00, 0x55, 0x0A, 0x00, 0x00, 0x00, 0x55, 0x31, 0x06, 0x01, 0x00, 0xB5, 0xE7, 0xD4, 0xB4, 0xB2, 0xD6, 0xA3, 0xB2, 0xC2, 0xA9, 0xCB, 0xAE }
};
u8 PowerWaterOff[2][27] = {
	{ 0x55, 0x07, 0x05, 0x01, 0x00, 0x55, 0x0A, 0x00, 0x00, 0x00, 0x55, 0x31, 0x06, 0x01, 0x00, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1 },
	{ 0x55, 0x07, 0x06, 0x01, 0x00, 0x55, 0x0A, 0x00, 0x00, 0x00, 0x55, 0x31, 0x06, 0x01, 0x00, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1, 0xA1 }
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM1_Init(void);
static void MX_UART4_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_UART5_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM2_Init(void);
void U1Task(void const * argument);
void U2Task(void const * argument);
void U3Task(void const * argument);
void U4Task(void const * argument);
void ScreenTask(void const * argument);

/* USER CODE BEGIN PFP */
void Send5byte(u8 a, u8 b, u8 c); 
void Sendbyte(u8 byte);
void SendLetter(u8 Letter[4]);
void SendNum(u8 H, u8 L, u8 Number);
/* USER CODE END PFP */

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
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_IWDG_Init();
  MX_TIM1_Init();
  MX_UART4_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_UART5_Init();
  MX_USART6_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	/*while (!SystemBegin)
	{
		if (U1_RX_EndFlag == 1)
		{
			U1_RX_EndFlag = 0;
			if (((U1_RX_Position[1]) <= '9')&&((U1_RX_Position[1]) >= '0'))
			{
				SystemBegin = 1;
			}
			HAL_UART_Receive_DMA(&huart1, U1_RX_Buffer, U1_RX_BufferSize);
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
		}
		else if (U2_RX_EndFlag == 1)
		{
			U2_RX_EndFlag = 0;
			if ((U2_RX_Position[3]) != 0)
			{
				SystemBegin = 1;
			}
			HAL_UART_Receive_DMA(&huart2, U2_RX_Buffer, U2_RX_BufferSize);
			__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
		}
		HAL_IWDG_Refresh(&hiwdg);
	}*/
	HAL_IWDG_Refresh(&hiwdg);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	TIM1->CCR1 = 1500;
	TIM2->CCR1 = 1500;
	TIM1->CCR3 = 0;
	HAL_ADC_Start_DMA(&hadc1, ADCCache, 4); 
	HAL_UART_Receive_DMA(&huart3, U3_RX_Buffer, U3_RX_BufferSize);   //打开DMA接收，数据存入rx_buffer数组中。
	HAL_UART_Receive_DMA(&huart2, U2_RX_Buffer, U2_RX_BufferSize);   //打开DMA接收，数据存入rx_buffer数组中。
	HAL_UART_Receive_DMA(&huart4, U4_RX_Buffer, U4_RX_BufferSize);   //打开DMA接收，数据存入rx_buffer数组中。
	HAL_UART_Transmit_DMA(&huart2, U2_TX_Begin,30);
  /* USER CODE END 2 */

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
  /* definition and creation of UART1Task */
  osThreadDef(UART1Task, U1Task, osPriorityNormal, 0, 128);
  UART1TaskHandle = osThreadCreate(osThread(UART1Task), NULL);

  /* definition and creation of UART2Task */
  osThreadDef(UART2Task, U2Task, osPriorityNormal, 0, 128);
  UART2TaskHandle = osThreadCreate(osThread(UART2Task), NULL);

  /* definition and creation of UART3Task */
  osThreadDef(UART3Task, U3Task, osPriorityNormal, 0, 128);
  UART3TaskHandle = osThreadCreate(osThread(UART3Task), NULL);

  /* definition and creation of UART4Task */
  osThreadDef(UART4Task, U4Task, osPriorityNormal, 0, 128);
  UART4TaskHandle = osThreadCreate(osThread(UART4Task), NULL);

  /* definition and creation of DisplayTask */
  osThreadDef(DisplayTask, ScreenTask, osPriorityNormal, 0, 128);
  DisplayTaskHandle = osThreadCreate(osThread(DisplayTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

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

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 4;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 2047;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 167;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 50000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1100;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 1500;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 41;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 5000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
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
  sConfigOC.Pulse = 0;
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
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);        //使能idle中断

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);        //使能idle中断
  HAL_UART_Receive_DMA(&huart1, U1_RX_Buffer, U1_RX_BufferSize);  //打开DMA接收，数据存入rx_buffer数组中。
  /* USER CODE END USART1_Init 2 */

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
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);        //使能idle中断
  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);        //使能idle中断
  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */


void Send5byte(u8 a, u8 b, u8 c)
{
	SendBuffer[1] = a;
	SendBuffer[2] = b;
	SendBuffer[3] = c;
	HAL_UART_Transmit_DMA(&huart5, SendBuffer, 5);
}
void Sendbyte(u8 byte)
{
	SendByte = 0;
	HAL_UART_Transmit_DMA(&huart5, &SendByte, 1);
}
void SendLetter(u8 Letter[4])
{
	HAL_UART_Transmit_DMA(&huart5, Letter, 4);
}
void SendNum(u8 Row, u8 Column, u8 Number)
{
	static u8 SendTempBuffer[10] = { 0x55, 0x07, 0x00, 0x00, 0x00, 0x55, 0x08, 0x00, 0x00, 0x00 };
	SendTempBuffer[2] = Row;
	SendTempBuffer[3] = Column;
	SendTempBuffer[7] = Num[Number][0];
	SendTempBuffer[8] = Num[Number][1];
	HAL_UART_Transmit_DMA(&huart5, SendTempBuffer, 10);
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_U1Task */
/**
  * @brief  Function implementing the UART1Task thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_U1Task */
void U1Task(void const * argument)
{
    
    
    
    
    
    
    

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	  if (U1_RX_EndFlag == 1)
	  {
		  U1_RX_EndFlag = 0;
		  Light = (U1_RX_Position[18] - '0') + (U1_RX_Position[17] - '0') * 10 + (U1_RX_Position[16] - '0') * 100 + (U1_RX_Position[15] - '0') * 1000;
		  PTZ = (U1_RX_Position[23] - '0') + (U1_RX_Position[22] - '0') * 10 + (U1_RX_Position[21] - '0') * 100 + (U1_RX_Position[20] - '0') * 1000;
		  //Crab = (U1_RX_Position[28] - '0') + (U1_RX_Position[27] - '0') * 10 + (U1_RX_Position[26] - '0') * 100 + (U1_RX_Position[25] - '0') * 1000;
		  TIM2->CCR1 = Light;
		  TIM1->CCR3 = PTZ;
		  TIM1->CCR1 = Crab;
		  HAL_UART_Transmit_DMA(&huart2, U1_RX_Position, 30);
		  HAL_UART_Receive_DMA(&huart1, U1_RX_Buffer, U1_RX_BufferSize);
		  HAL_IWDG_Refresh(&hiwdg);
		  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	  }
	  osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_U2Task */
/**
* @brief Function implementing the UART2Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_U2Task */
void U2Task(void const * argument)
{
  /* USER CODE BEGIN U2Task */
  /* Infinite loop */
  for(;;)
  {
	  if (U2_RX_EndFlag == 1)
	  {
		  U2_RX_EndFlag = 0;
		  if (U2_RX_Position[1] == '1')
		  {
			  ++PowerWater[0];
		  }
		  else if (U2_RX_Position[1] == '0')
		  {
			  --PowerWater[0];
		  }
		  if (U2_RX_Position[3] == '1')
		  {
			  ++PowerWater[2];
		  }
		  else if (U2_RX_Position[3] == '0')
		  {
			  --PowerWater[2];
		  }

		  HAL_UART_Receive_DMA(&huart2, U2_RX_Buffer, U2_RX_BufferSize);
		  HAL_IWDG_Refresh(&hiwdg);
		  __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	  }
	  osDelay(1);
  }
  /* USER CODE END U2Task */
}

/* USER CODE BEGIN Header_U3Task */
/**
* @brief Function implementing the UART3Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_U3Task */
void U3Task(void const * argument)
{
  /* USER CODE BEGIN U3Task */
  /* Infinite loop */
  for(;;)
  {
	  if (U3_RX_EndFlag == 1)
	  {
		  U3_RX_EndFlag = 0;
			
		  if (!ScreenFinish[2])
		  { 
			  ControlTemperture = (U3_RX_Position[1] - '0') * 10 + (U3_RX_Position[2] - '0') * 1 + (U3_RX_Position[3] - '0') * 0.1;
			  Pitch = (U3_RX_Position[5] - '0') * 100 + (U3_RX_Position[6] - '0') * 10 + (U3_RX_Position[7] - '0');
			  Yaw = (U3_RX_Position[9] - '0') * 100 + (U3_RX_Position[10] - '0') * 10 + (U3_RX_Position[11] - '0');
			  Roll = (U3_RX_Position[13] - '0') * 100 + (U3_RX_Position[14] - '0') * 10 + (U3_RX_Position[15] - '0');
			  GeomagneticAngle = (U3_RX_Position[17] - '0') * 100 + (U3_RX_Position[18] - '0') * 10 + (U3_RX_Position[19] - '0');
			  float testing = ADCCache[3] * 3.3 / 4096;			  
			  ADCResult[3] = ADCCache[3] * 3.3 / 4096 * 500;
			  SendGAngle[0] = GeomagneticAngle / 100;
			  GeomagneticAngle %= 100;
			  SendGAngle[1] = GeomagneticAngle / 10;
			  SendGAngle[2] = GeomagneticAngle % 10;
			  SendPitch[0] = Pitch / 100;
			  Pitch %= 100;
			  SendPitch[1] = Pitch / 10;
			  SendPitch[2] = Pitch % 10;
			  SendPower[0] = ADCResult[3] / 1000;
			  ADCResult[3] %= 1000;
			  SendPower[1] = ADCResult[3] / 100;
			  ADCResult[3] %= 100;
			  SendPower[2] = ADCResult[3] / 10;
			  SendPower[3] = ADCResult[3] % 10;
			  ScreenFinish[2] = 1;
		  }
		  if (U3_RX_Position[21] == '1')
		  {
			  ++ControlWater[0];
		  }
		  else if (U3_RX_Position[21] == '0')
		  {
			  --ControlWater[0];
		  }
		  HAL_UART_Receive_DMA(&huart3, U3_RX_Buffer, U3_RX_BufferSize);
		  HAL_IWDG_Refresh(&hiwdg);
		  __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	  }
	  osDelay(1);
  }
  /* USER CODE END U3Task */
}

/* USER CODE BEGIN Header_U4Task */
/**
* @brief Function implementing the UART4Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_U4Task */
void U4Task(void const * argument)
{
  /* USER CODE BEGIN U4Task */
  /* Infinite loop */
  for(;;)
  {
	  if (U4_RX_EndFlag == 1)
	  {
		  U4_RX_EndFlag = 0;
		  if (U4_RX_Position[4] == '.')
		  {
			  WaterTemperture = (U4_RX_Position[2] - '0') * 1000 + (U4_RX_Position[3] - '0') * 100 + (U4_RX_Position[5] - '0') * 10 + (U4_RX_Position[6] - '0');
			  if (U4_RX_Position[9] == '-')
			  {
				  Depth = 0;
			  }
			  else
			  {
				  if (U4_RX_Position[10] == '.')
				  {
					  Depth = (U4_RX_Position[9] - '0') * 100 + (U4_RX_Position[11] - '0') * 10 + (U4_RX_Position[12] - '0');
				  }
				  else if (U4_RX_Position[11] == '.')
				  {
					  Depth = (U4_RX_Position[9] - '0') * 1000 + (U4_RX_Position[10] - '0') * 100 + (U4_RX_Position[12] - '0') * 10 + (U4_RX_Position[13] - '0');
				  }
				  else if (U4_RX_Position[12] == '.')
				  {
					  Depth = (U4_RX_Position[9] - '0') * 10000 + (U4_RX_Position[10] - '0') * 1000 + (U4_RX_Position[11] - '0') * 100 + (U4_RX_Position[13] - '0') * 10 + (U4_RX_Position[14] - '0');
				  }
			  }
		  }
		  else if (U4_RX_Position[3] == '.')
		  {
			  WaterTemperture = (U4_RX_Position[2] - '0') * 100 + (U4_RX_Position[4] - '0') * 10 + (U4_RX_Position[5] - '0');
			  if (U4_RX_Position[8] == '-')
			  {
				  Depth = 0;
			  }
			  else
			  {
				  if (U4_RX_Position[9] == '.')
				  {
					  Depth = (U4_RX_Position[8] - '0') * 100 + (U4_RX_Position[10] - '0') * 10 + (U4_RX_Position[11] - '0');

				  }
				  else if (U4_RX_Position[10] == '.')
				  {
					  Depth = (U4_RX_Position[8] - '0') * 1000 + (U4_RX_Position[9] - '0') * 100 + (U4_RX_Position[11] - '0') * 10 + (U4_RX_Position[12] - '0');

				  }
				  else if (U4_RX_Position[11] == '.')
				  {
					  Depth = (U4_RX_Position[8] - '0') * 10000 + (U4_RX_Position[9] - '0') * 1000 + (U4_RX_Position[10] - '0') * 100 + (U4_RX_Position[12] - '0') * 10 + (U4_RX_Position[13] - '0');

				  }
			  }
		  }
		  if (!ScreenFinish[0])
		  {
			  SendDepth[0] = Depth / 10000;
			  Depth %= 10000;
			  SendDepth[1] = Depth / 1000;
			  Depth %= 1000;
			  SendDepth[2] = Depth / 100;
			  Depth %= 100;
			  SendDepth[3] = Depth / 10;
			  SendDepth[4] = Depth % 10;
			  SendWaterT[0] = WaterTemperture / 1000;
			  WaterTemperture %= 1000;
			  SendWaterT[1] = WaterTemperture / 100;
			  WaterTemperture %= 100;
			  SendWaterT[2] = WaterTemperture / 10;
			  SendWaterT[3] = WaterTemperture % 10;
			  ScreenFinish[0] = 1;
		  }
			
		  HAL_UART_Receive_DMA(&huart4, U4_RX_Buffer, U4_RX_BufferSize);
		  HAL_IWDG_Refresh(&hiwdg);
		  __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
	  }
	  osDelay(1);
  }
  /* USER CODE END U4Task */
}

/* USER CODE BEGIN Header_ScreenTask */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ScreenTask */
void ScreenTask(void const * argument)
{
  /* USER CODE BEGIN ScreenTask */
  /* Infinite loop */
	
	for (;;)
	{
	  
		static int i = 0;
		static int j = 0;
		static int ScreenTime = 0;
		if ((ScreenFinish[0] == 1)&&(ScreenFinish[2] == 1))
		{
			switch (ScreenTime)
			{
			case 0:
				if (j == 3)
				{
					SendNum(1, 4 + j, 10);
					++j;
				}
				else if (j == 6)
				{
					i = 0;
					j = 0;
					ScreenTime = 1;
				}
				else
				{
					SendNum(1, 4 + j, SendDepth[i]);
					++i;
					++j;
				}
				break;
			case 1:
				if (j == 2)
				{
					SendNum(2, 4 + j, 10);
					++j;
				}
				else if (j == 5)
				{
					i = 0;
					j = 0;
					ScreenTime = 2;
				}
				else
				{
					SendNum(2, 4 + j, SendWaterT[i]);
					++i;
					++j;
				}
				break;
			case 2:
				if (j == 3)
				{
					i = 0;
					j = 0;
					ScreenTime = 3;
				}
				else
				{
					SendNum(1, 16 + j, SendGAngle[i]);
					++i;
					++j;
				}
				break;
			case 3:
				if (j == 3)
				{
					i = 0;
					j = 0;
					ScreenTime = 4;
				}
				else
				{
					SendNum(8, 1 + j, SendPitch[i]);
					++j;
					++i;
				}
				break;
			case 4:
				if (j == 2)
				{
					SendNum(3, 4 + j, 10);
					++j;
				}
				else if (j == 4)
				{
					i = 0;
					j = 0;
					ScreenTime = 5;
				}
				else
				{
					SendNum(3, 4 + j, SendPower[i]);
					++i;
					++j;
				}
				break;
			case 5:
				ScreenTime = 0;
				ScreenFinish[0] = 0;
				ScreenFinish[2] = 0;
				break;
			}
		}
		osDelay(15);
		if (ControlWater[0] >= 10)
		{
			if (1/*ControlWater[1] == 0*/)
			{
				ControlWater[1] = 1;
				HAL_UART_Transmit_DMA(&huart5, ControlWaterOn, 25);
				osDelay(50);
			}
			ControlWater[0] = 5;
		}
		else if (ControlWater[0] <= 0)
		{
			if (1/*ControlWater[1] == 1*/)
			{
				ControlWater[1] = 0;
				HAL_UART_Transmit_DMA(&huart5, ControlWaterOff, 55);
				osDelay(50);
			}
			ControlWater[0] = 5;
		}
		if (PowerWater[0] >= 10)
		{
			if (1/*PowerWater[1] == 0*/)
			{
				PowerWater[1] = 1;
				HAL_UART_Transmit_DMA(&huart5, PowerWaterOn[0], 27);
				osDelay(50);
			}
			PowerWater[0] = 5;
		}
		else if (PowerWater[0] <= 0)
		{
			if (1/*PowerWater[1] == 1*/)
			{
				PowerWater[1] = 0;
				HAL_UART_Transmit_DMA(&huart5, PowerWaterOff[0], 27);
				osDelay(50);
			}
			PowerWater[0] = 5;
		}
		if (PowerWater[2] >= 10)
		{
			if (1/*PowerWater[3] == 0*/)
			{
				PowerWater[3] = 1;
				HAL_UART_Transmit_DMA(&huart5, PowerWaterOn[1], 27);
				osDelay(50);
			}
			PowerWater[2] = 5;
		}
		else if (PowerWater[2] <= 0)
		{
			if (1/*PowerWater[3] == 1*/)
			{
				PowerWater[3] = 0;
				HAL_UART_Transmit_DMA(&huart5, PowerWaterOff[1], 27);
				osDelay(50);
			}
			PowerWater[2] = 5;
		}
		HAL_IWDG_Refresh(&hiwdg);
		osDelay(1);
	}
  /* USER CODE END ScreenTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
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
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
