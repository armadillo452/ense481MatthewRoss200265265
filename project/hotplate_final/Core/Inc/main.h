/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BlueButton_Pin GPIO_PIN_13
#define BlueButton_GPIO_Port GPIOC
#define BlueButton_EXTI_IRQn EXTI15_10_IRQn
#define RightButton_Pin GPIO_PIN_1
#define RightButton_GPIO_Port GPIOA
#define RightButton_EXTI_IRQn EXTI1_IRQn
#define LeftButton_Pin GPIO_PIN_4
#define LeftButton_GPIO_Port GPIOA
#define LeftButton_EXTI_IRQn EXTI4_IRQn
#define HotplatePWM_Pin GPIO_PIN_0
#define HotplatePWM_GPIO_Port GPIOB
#define EPAPER_CS_Pin GPIO_PIN_1
#define EPAPER_CS_GPIO_Port GPIOB
#define MAX6675_CS_Pin GPIO_PIN_9
#define MAX6675_CS_GPIO_Port GPIOA
#define EPAPER_D_C_Pin GPIO_PIN_3
#define EPAPER_D_C_GPIO_Port GPIOB
#define EPAPER_BUSY_Pin GPIO_PIN_4
#define EPAPER_BUSY_GPIO_Port GPIOB
#define EPAPER_RES_Pin GPIO_PIN_5
#define EPAPER_RES_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
