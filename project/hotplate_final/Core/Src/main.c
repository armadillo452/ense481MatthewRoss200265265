/* USER CODE BEGIN Header */
/**
 * @file main.c
 * @brief Main entry point and application control.
 *
 * This file serves as the primary entry point for the application. It is responsible for:
 * - Initializing hardware and peripherals (e.g., UART, ADC, timers, GPIOs).
 * - Setting up the operating system (e.g., thread creation, message queues, semaphores).
 * - Providing callbacks for hardware interrupts and timer events.
 * - Launching the main application loops and managing system tasks such as heartbeat,
 *   e-paper display updates, and UART communication.
 *
 * The application is developed for an STM32 microcontroller using CubeIDE. It integrates
 * multiple modules, including CLI for user interaction, ADC for sensor readings, e-paper display
 * for output, and a simple neural control for managing heater duty cycles.
 *
 * @note Ensure that all peripheral initializations and system tasks are appropriately
 *       configured before entering the main loop.
 *
 * Hardware connections:
 * - **Left Button:** GPIO (PA4)
 * - **Right Button:** GPIO (PA1)
 * - **GND:** Ground
 *
 * @author Matthew Ross 200265265
 *
 * @version 1.0
 * @date    17apr2025
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cli.h"    // console, heartbeat, status, myprintf
#include "string.h" // strlen in UARTprint task
#include "epaper.h" // epaper print
#include "epaper_ctl.h" // epaper print
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

IWDG_HandleTypeDef hiwdg;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* Definitions for UARTprint */
osThreadId_t UARTprintHandle;
const osThreadAttr_t UARTprint_attributes = {
  .name = "UARTprint",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for UARTinput */
osThreadId_t UARTinputHandle;
const osThreadAttr_t UARTinput_attributes = {
  .name = "UARTinput",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Heartbeat */
osThreadId_t HeartbeatHandle;
const osThreadAttr_t Heartbeat_attributes = {
  .name = "Heartbeat",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for EpaperPrint */
osThreadId_t EpaperPrintHandle;
const osThreadAttr_t EpaperPrint_attributes = {
  .name = "EpaperPrint",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for UARTin */
osMessageQueueId_t UARTinHandle;
const osMessageQueueAttr_t UARTin_attributes = {
  .name = "UARTin"
};
/* Definitions for UARTout */
osMessageQueueId_t UARToutHandle;
const osMessageQueueAttr_t UARTout_attributes = {
  .name = "UARTout"
};
/* Definitions for uptime */
osMessageQueueId_t uptimeHandle;
const osMessageQueueAttr_t uptime_attributes = {
  .name = "uptime"
};
/* Definitions for EpaperStatus */
osMessageQueueId_t EpaperStatusHandle;
const osMessageQueueAttr_t EpaperStatus_attributes = {
  .name = "EpaperStatus"
};
/* Definitions for EpaperStatus2 */
osMessageQueueId_t EpaperStatus2Handle;
const osMessageQueueAttr_t EpaperStatus2_attributes = {
  .name = "EpaperStatus2"
};
/* Definitions for EpaperStatus3 */
osMessageQueueId_t EpaperStatus3Handle;
const osMessageQueueAttr_t EpaperStatus3_attributes = {
  .name = "EpaperStatus3"
};
/* Definitions for console */
osSemaphoreId_t consoleHandle;
const osSemaphoreAttr_t console_attributes = {
  .name = "console"
};
/* USER CODE BEGIN PV */

uint8_t input;                // Store received character
uint8_t uptime = 0;           // Store uptime counter
uint8_t minutes = 0;          // Store uptime minutes counter
uint8_t hours = 0;            // Store uptime hours counter
uint16_t days = 0;            // Store uptime days counter
uint8_t target = 0;           // Hotplate target temperature
extern uint8_t image_bw[EPD_W_BUFF_SIZE * EPD_H];
extern uint8_t clearPaper;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
static void MX_IWDG_Init(void);
void StartUARTprint(void *argument);
void StartUARTinput(void *argument);
void StartHeartbeat(void *argument);
void StartEpaperPrint(void *argument);

/* USER CODE BEGIN PFP */

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
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of console */
  consoleHandle = osSemaphoreNew(1, 1, &console_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of UARTin */
  UARTinHandle = osMessageQueueNew (11, sizeof(uint8_t), &UARTin_attributes);

  /* creation of UARTout */
  UARToutHandle = osMessageQueueNew (12, 128, &UARTout_attributes);

  /* creation of uptime */
  uptimeHandle = osMessageQueueNew (1, sizeof(uint32_t), &uptime_attributes);

  /* creation of EpaperStatus */
  EpaperStatusHandle = osMessageQueueNew (1, 64, &EpaperStatus_attributes);

  /* creation of EpaperStatus2 */
  EpaperStatus2Handle = osMessageQueueNew (1, 64, &EpaperStatus2_attributes);

  /* creation of EpaperStatus3 */
  EpaperStatus3Handle = osMessageQueueNew (1, 64, &EpaperStatus3_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of UARTprint */
  UARTprintHandle = osThreadNew(StartUARTprint, NULL, &UARTprint_attributes);

  /* creation of UARTinput */
  UARTinputHandle = osThreadNew(StartUARTinput, NULL, &UARTinput_attributes);

  /* creation of Heartbeat */
  HeartbeatHandle = osThreadNew(StartHeartbeat, NULL, &Heartbeat_attributes);

  /* creation of EpaperPrint */
  EpaperPrintHandle = osThreadNew(StartEpaperPrint, NULL, &EpaperPrint_attributes);

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
  while (1) {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
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
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 2500;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 63999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 499;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END TIM2_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 63999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 499;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
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
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
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
  HAL_UART_Receive_IT(&huart2, &input, 1); // Start listening for interrupt
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
  HAL_GPIO_WritePin(GPIOB, EPAPER_CS_Pin|EPAPER_D_C_Pin|EPAPER_RES_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MAX6675_CS_GPIO_Port, MAX6675_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BlueButton_Pin */
  GPIO_InitStruct.Pin = BlueButton_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BlueButton_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RightButton_Pin LeftButton_Pin */
  GPIO_InitStruct.Pin = RightButton_Pin|LeftButton_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : EPAPER_CS_Pin EPAPER_D_C_Pin EPAPER_RES_Pin */
  GPIO_InitStruct.Pin = EPAPER_CS_Pin|EPAPER_D_C_Pin|EPAPER_RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MAX6675_CS_Pin */
  GPIO_InitStruct.Pin = MAX6675_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MAX6675_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EPAPER_BUSY_Pin */
  GPIO_InitStruct.Pin = EPAPER_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(EPAPER_BUSY_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief UART Reception Complete Callback.
 *
 * This callback is invoked by the HAL when a complete UART reception interrupt occurs.
 * When the function is called with a UART handle corresponding to USART2, it places the
 * received character (stored in a global variable, e.g. `input`) into the UART input
 * message queue, then reinitiates the UART reception interrupt.
 *
 * @param huart Pointer to the UART handle structure.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {
    osMessageQueuePut(UARTinHandle, &input, 0, 0); // Send received character to the queue
    HAL_UART_Receive_IT(&huart2, &input, 1); // Reset interrupt
  }
}

/**
 * @brief GPIO EXTI (External Interrupt) Callback.
 *
 * This callback is triggered when an external GPIO interrupt occurs. This example uses
 * interrupts on specific pins to adjust a target value. If the interrupt is from GPIO_PIN_1
 * the target is increased by 10°C. Conversely, for GPIO_PIN_4, the target is
 * decreased by 10°C.
 *
 * @param GPIO_Pin The pin number that generated the interrupt.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_1) {
    if (target <= 240)
      target += 10;
  } else if (GPIO_Pin == GPIO_PIN_4) {
    if ((target >= 10))
      target -= 10;
  }
//	if (GPIO_Pin == GPIO_PIN_13) { // Button press interrupt
//		myprintf("\r\n  You pressed the blue button.\r\n>> ");
//	}
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartUARTprint */

/**
 * @brief Thread function for UART print.
 *
 * This function implements the UARTprint thread. It continuously waits for a character
 * string from the UART output message queue, then transmits the received buffer over the UART.
 * A short delay is added in each loop iteration to yield control.
 *
 * @param argument Not used.
 * @retval None.
 */
/* USER CODE END Header_StartUARTprint */
void StartUARTprint(void *argument)
{
  /* USER CODE BEGIN 5 */
  char buffer[128]; // Buffer for console print
  /* Infinite loop */
  for (;;) {
    if (osMessageQueueGet(UARToutHandle, &buffer, NULL, osWaitForever) == osOK) // Wait for a character from the queue
      HAL_UART_Transmit(&huart2, (uint8_t*) buffer, strlen(buffer),
      HAL_MAX_DELAY); // Transmit buffer over UART
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartUARTinput */

/**
 * @brief Thread function for UART input.
 *
 * This function implements the UARTinput thread. It continuously waits for a received
 * character from the UART input message queue and then passes that character to the
 * console_UART() function for processing.
 *
 * @param argument Not used.
 * @retval None.
 */
/* USER CODE END Header_StartUARTinput */
void StartUARTinput(void *argument)
{
  /* USER CODE BEGIN StartUARTinput */
  uint8_t input;
  /* Infinite loop */
  for (;;) {
    if (osMessageQueueGet(UARTinHandle, &input, NULL, osWaitForever) == osOK) // Wait for a character from the queue
      console_UART(input);
    osDelay(1);
  }
  /* USER CODE END StartUARTinput */
}

/* USER CODE BEGIN Header_StartHeartbeat */

/**
 * @brief Thread function for system heartbeat.
 *
 * This function implements the Heartbeat thread. On startup, it acquires a semaphore
 * to safely update and display the system status. It then starts the PWM for controlling
 * the hotplate output. Inside the main loop, it waits for an uptime value from a message queue
 * and calls the heartbeat() function to update the system’s heartbeat routine.
 *
 * @param argument Not used.
 * @retval None.
 */
/* USER CODE END Header_StartHeartbeat */
void StartHeartbeat(void *argument)
{
  /* USER CODE BEGIN StartHeartbeat */
  uint32_t uptime;
  if (osSemaphoreAcquire(consoleHandle, osWaitForever) == osOK) {
    status(); // Clear all and draw status window
    myprintf(">> ");
    osSemaphoreRelease(consoleHandle);
  }
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // Hotplate output
  /* Infinite loop */
  for (;;) {
    if (osMessageQueueGet(uptimeHandle, &uptime, NULL, osWaitForever) == osOK) // Wait for uptime from the queue
      heartbeat();
    osDelay(1);
  }
  /* USER CODE END StartHeartbeat */
}

/* USER CODE BEGIN Header_StartEpaperPrint */

/**
 * @brief Thread function for updating the e-paper display.
 *
 * This function implements the EpaperPrint thread. It first checks whether the e-paper needs to
 * be cleared (using the flag `clearPaper`) and initializes it if required. Then, it waits for
 * status messages from three separate message queues to update different sections of the e-paper.
 * After updating the image buffer, it issues a partial update command so that only changed content is redrawn.
 *
 * @param argument Not used.
 * @retval None.
 */
/* USER CODE END Header_StartEpaperPrint */
void StartEpaperPrint(void *argument)
{
  /* USER CODE BEGIN StartEpaperPrint */
//	osDelay(100);
  char displayBuffer[64]; // Buffer for formatted output to epaper
  /* Infinite loop */
  for (;;) {
    if (clearPaper == 1) {
      epaper_start();
      clearPaper = 0;
    }
    if (osMessageQueueGet(EpaperStatusHandle, displayBuffer, NULL, osWaitForever) == osOK)
      epd_paint_showString(4, 42, (uint8_t*) displayBuffer, EPD_FONT_SIZE8x6, EPD_COLOR_BLACK); // Status for VDD, chip temp, and uptime
    if (osMessageQueueGet(EpaperStatus2Handle, displayBuffer, NULL, osWaitForever) == osOK)
      epd_paint_showString(4, 62, (uint8_t*) displayBuffer, EPD_FONT_SIZE24x12, EPD_COLOR_BLACK); // Status for target and now temp
    if (osMessageQueueGet(EpaperStatus3Handle, displayBuffer, NULL, osWaitForever) == osOK)
      epd_paint_showString(4, 86, (uint8_t*) displayBuffer, EPD_FONT_SIZE24x12, EPD_COLOR_BLACK); // Status for heater

    epd_displayBW_partial(image_bw); // Print to the epaper
    osDelay(1);
  }
  /* USER CODE END StartEpaperPrint */
}

/**
 * @brief Timer period elapsed callback.
 *
 * This callback is invoked by the HAL when a timer period elapses.
 * When TIM1 generates an interrupt, the system tick is incremented via HAL_IncTick().
 * Additionally, when TIM2 generates an interrupt, the uptime counter is incremented, and
 * time-related variables (uptime, minutes, hours, days) are updated accordingly. Finally, the updated
 * uptime is sent to a message queue for further processing.
 *
 * @param htim Pointer to the TIM handle structure.
 * @retval None.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM2) {
    uptime++; // Half-second ticks
    if (uptime == 120) {
      uptime = 0;
      minutes++;
    }
    if (minutes == 60) {
      minutes = 0;
      hours++;
    }
    if (hours == 24) {
      hours = 0;
      days++;
    }
    osMessageQueuePut(uptimeHandle, &uptime, 0, 0); // Send uptime to the queue
  }
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
  while (1) {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
