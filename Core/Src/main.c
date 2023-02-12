/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @brief   Main program body
	* @author  Marcin Jonik
	*					 Michal Watroba
	* @version 2.0
	* @date		 12.02.2023
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Zamek.h"
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
/* USER CODE BEGIN PV */
xSemaphoreHandle Tim50ms;
xSemaphoreHandle Tim20ms;
xSemaphoreHandle Tim2ms;

xQueueHandle Wys_A;
xQueueHandle Wys_B;
xQueueHandle Wys_C;
xQueueHandle Wys_D;

xQueueHandle HasloJendenZnak;
xQueueHandle ZmianaCyfry;
xQueueHandle ZmianaCyfry2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void vApplicationTickHook(void)
{
	/* definiowanie zmiennych lokalnych funckji */
  static uint8_t time50ms = 0;
  static uint8_t time20ms = 0;
  static uint8_t time2ms = 0;
  signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Tworzenie impulsu z czestoscia odswiezania 2ms */
		if (time2ms++ > 1)
		{
			time2ms = 0;
			/* Wysylanie zmiennej time2ms przez Semaphor */
			xSemaphoreGiveFromISR(Tim2ms, &xHigherPriorityTaskWoken);
		}
	/* Tworzenie impulsu z czestoscia odswiezania 2ms */
		if (time20ms++ > 10)
		{
			time20ms = 0;
			/* Wysylanie zmiennej time20ms przez Semaphor */
			xSemaphoreGiveFromISR(Tim20ms, &xHigherPriorityTaskWoken);
		}
	/* Tworzenie impulsu z czestoscia odswiezania 2ms */
		if (time50ms++ >25)
		{
			time50ms = 0;
			/* Wysylanie zmiennej time50ms przez Semaphor */
			xSemaphoreGiveFromISR(Tim50ms, &xHigherPriorityTaskWoken);
		}	
  
  if (xHigherPriorityTaskWoken == pdTRUE) taskYIELD();
}
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
  /* USER CODE BEGIN 2 */
	/* Wstepne ustawianie diod LED */
  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);

/* definiowanie Semaphora imitujacego czas 50ms */
	vSemaphoreCreateBinary(Tim50ms);
	/* definiowanie Semaphora imitujacego czas 20ms */
  vSemaphoreCreateBinary(Tim20ms);
	/* definiowanie Semaphora imitujacego czas 2ms */
  vSemaphoreCreateBinary(Tim2ms);

/* definiowanie kolejek przesylajacych wyswietlane dane */
  Wys_A = xQueueCreate( 16, sizeof(uint8_t) );
	Wys_B = xQueueCreate( 16, sizeof(uint8_t) );
	Wys_C = xQueueCreate( 16, sizeof(uint8_t) );
	Wys_D = xQueueCreate( 16, sizeof(uint8_t) );

/* definiowanie kolejki przesylajacej jeden znak hasla */
  HasloJendenZnak = xQueueCreate( 16, sizeof(uint8_t) );
/* definiowanie kolejki przesylajacej numer wyswietlanej cyfry hasla */
  ZmianaCyfry = xQueueCreate( 16, sizeof(uint8_t) );
/* definiowanie kolejki przesylajacej numer wyswietlanej cyfry hasla */
  ZmianaCyfry2 = xQueueCreate( 16, sizeof(uint8_t) );
  
/* definiowanie Taska1 odpowiadajacego za obsluge wyswietlaczy siedmiosegmentowych */
  xTaskCreate(vTask1, "WyswietlaczSiedmioSegmentowy", configMINIMAL_STACK_SIZE, NULL,
/* definiowanie Taska1 odpowiadajacego za sprawdzanie poprawnosci wprowadzonego hasla */ main_TASK_PRIORITY, NULL);
	xTaskCreate(vTask2, "PorownywanieHasla", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);
/* definiowanie Taska1 odpowiadajacego za obsluge enkodera obrotowego */
	xTaskCreate(vTask3, "ObslugaEnkodera", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
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
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL10;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV2;
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
  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
  __disable_irq();
  while (1)
  {
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

