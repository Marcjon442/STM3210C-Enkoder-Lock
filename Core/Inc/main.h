/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Enkoder_1_Pin GPIO_PIN_4
#define Enkoder_1_GPIO_Port GPIOC
#define Anoda_1_Pin GPIO_PIN_1
#define Anoda_1_GPIO_Port GPIOB
#define LED_1_Pin GPIO_PIN_14
#define LED_1_GPIO_Port GPIOE
#define LED_2_Pin GPIO_PIN_15
#define LED_2_GPIO_Port GPIOE
#define Anoda_4_Pin GPIO_PIN_14
#define Anoda_4_GPIO_Port GPIOB
#define Enkoder_2_Pin GPIO_PIN_15
#define Enkoder_2_GPIO_Port GPIOB
#define Klawisz_Pin GPIO_PIN_12
#define Klawisz_GPIO_Port GPIOC
#define Katoda_A_Pin GPIO_PIN_0
#define Katoda_A_GPIO_Port GPIOD
#define Katoda_B_Pin GPIO_PIN_1
#define Katoda_B_GPIO_Port GPIOD
#define Katoda_C_Pin GPIO_PIN_2
#define Katoda_C_GPIO_Port GPIOD
#define Katoda_D_Pin GPIO_PIN_3
#define Katoda_D_GPIO_Port GPIOD
#define Katoda_E_Pin GPIO_PIN_4
#define Katoda_E_GPIO_Port GPIOD
#define Katoda_F_Pin GPIO_PIN_5
#define Katoda_F_GPIO_Port GPIOD
#define Katoda_G_Pin GPIO_PIN_6
#define Katoda_G_GPIO_Port GPIOD
#define Katoda_H_Pin GPIO_PIN_7
#define Katoda_H_GPIO_Port GPIOD
#define Anoda_2_Pin GPIO_PIN_5
#define Anoda_2_GPIO_Port GPIOB
#define Anoda_3_Pin GPIO_PIN_9
#define Anoda_3_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
