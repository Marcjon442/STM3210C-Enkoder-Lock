/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define main_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const unsigned char seg7[] = {0xC0, 0xF9, 0xA4, 0xB0,
                              0x99, 0x92, 0x82, 0xF8,
                              0x80, 0x90, 0x5F};

volatile uint8_t LED_buf[4] = {0,0,0,0};
volatile uint8_t LED_ptr;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
xSemaphoreHandle Tim1s;
xSemaphoreHandle Tim100ms;
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
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void vTask1(void *pvParameters);
static void vTask1(void *pvParameters)
{
  static uint8_t znak[4] = {0,0,0,0};
	uint8_t wyswietl = 0;
	
	for( ;; )
	{
		if (xSemaphoreTake(Tim2ms, portMAX_DELAY))
		{
			xQueueReceive(Wys_A, &znak[0], ( TickType_t ) 0 );
			xQueueReceive(Wys_B, &znak[1], ( TickType_t ) 0 );
			xQueueReceive(Wys_C, &znak[2], ( TickType_t ) 0 );
			xQueueReceive(Wys_D, &znak[3], ( TickType_t ) 0 );
      xQueueReceive(ZmianaCyfry2, &wyswietl, (TickType_t)0);
			LED_buf[0] = seg7[znak[0]];
			LED_buf[1] = seg7[znak[1]];
			LED_buf[2] = seg7[znak[2]];
			LED_buf[3] = seg7[znak[3]];
		  // obsluga wyswietlacza siedmiosegmentowego LED
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if ((++LED_ptr) > 3) LED_ptr = 0;
		  // wylaczenie wyswietlaczy
      HAL_GPIO_WritePin(Anoda_1_GPIO_Port, Anoda_4_Pin|Anoda_3_Pin|Anoda_2_Pin
                                          |Anoda_1_Pin, GPIO_PIN_RESET);
		  // ustawienie segmentow
      Katoda_A_GPIO_Port->BSRR = (uint32_t)LED_buf[wyswietl];
		  Katoda_A_GPIO_Port->BSRR = (uint32_t)(~LED_buf[wyswietl]) << 16;
		
		  // wybor wyswietlacza
      switch (wyswietl)
      {
        case 0: HAL_GPIO_WritePin(Anoda_1_GPIO_Port, Anoda_1_Pin, GPIO_PIN_SET);
                break;
        case 1: HAL_GPIO_WritePin(Anoda_2_GPIO_Port, Anoda_2_Pin, GPIO_PIN_SET);
                break;
        case 2: HAL_GPIO_WritePin(Anoda_3_GPIO_Port, Anoda_3_Pin, GPIO_PIN_SET);
                break;
        case 3: HAL_GPIO_WritePin(Anoda_4_GPIO_Port, Anoda_4_Pin, GPIO_PIN_SET);
                break;
      }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
		}
	}
}
static void vTask2(void *pvParameters);
static void vTask2(void *pvParameters)
{
  static uint8_t dane_enkoder = 0;
  static uint8_t klawisz_enkoder = 0;
  static uint8_t dane_wyswietlacz[4] = {0, 0, 0, 0};

  for(;;)
  {
    if(xSemaphoreTake(Tim50ms, portMAX_DELAY))
    {
      xQueueReceive(HasloJendenZnak, &dane_enkoder, (TickType_t)0);
      xQueueReceive(ZmianaCyfry, &klawisz_enkoder, (TickType_t)0);
      dane_wyswietlacz[klawisz_enkoder] = dane_enkoder;
      if(klawisz_enkoder == 4)
      {
        if(dane_wyswietlacz[0] == 0 && dane_wyswietlacz[1] == 0 && dane_wyswietlacz[2] == 0 && dane_wyswietlacz[3]== 0)
        {
          HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
        }
      }
      else
      {
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
      }
			xQueueSend(Wys_A , &dane_wyswietlacz[0], portMAX_DELAY);
			xQueueSend(Wys_B , &dane_wyswietlacz[1], portMAX_DELAY);
			xQueueSend(Wys_C , &dane_wyswietlacz[2], portMAX_DELAY);
			xQueueSend(Wys_D , &dane_wyswietlacz[3], portMAX_DELAY);
    }
  }
}
static void vTask3(void *pvParameters);
static void vTask3(void *pvParameters)
{
  static uint8_t EnkoderWartosc = 0;
	static uint8_t flaga_zmiany = 0;
  static uint8_t klawiszWyswietlacza = 0;
	static uint8_t debouncer = 0, klawisz = 0;
	static uint8_t left = 0, right = 0;

  for(;;)
  {
    if (xSemaphoreTake(Tim2ms, portMAX_DELAY))
		{
			left = !HAL_GPIO_ReadPin(Enkoder_1_GPIO_Port, Enkoder_1_Pin);
			right = !HAL_GPIO_ReadPin(Enkoder_2_GPIO_Port, Enkoder_2_Pin);
			switch(debouncer)
			{
				case 0: //00
					if(left&&!right) debouncer = 1;
					if(!left&&right) debouncer = 4;
					break;
				case 1: //01
					if(left&&!right) debouncer = 1;
					else{ if(left&&right) debouncer = 2;
					else debouncer = 0;}
					break;
				case 2: //11
					if(left&&right) debouncer = 2;
					else{ if(!left&&right) debouncer = 3;
					else debouncer = 1;}
					break;
				case 3: //10
					if(!left&&right) debouncer = 3;
					else{ if(!left&&!right)
					{
						if(flaga_zmiany == 0)
						{
						if(EnkoderWartosc == 0 ) EnkoderWartosc = 9;
						else EnkoderWartosc--;
						flaga_zmiany = 1;
						}
						debouncer = 0;
					}
					else debouncer = 2;}
					break;
				case 4: //10
					if(!left&&right) debouncer = 4;
					else{ if(left&&right) debouncer = 5;
					else debouncer = 0;}
					break;
				case 5: //11
					if(left&&right) debouncer = 5;
					else{ if(left&&!right) debouncer = 6;
					else debouncer = 3;}
					break;
				case 6: //01
					if(left&&!right) debouncer = 6;
					else{ if(!left&&!right)
					{
						if(flaga_zmiany == 0)
						{
						if(EnkoderWartosc == 9) EnkoderWartosc = 0; 
						else EnkoderWartosc++;
						flaga_zmiany = 1;
						}
						debouncer = 0;
					}
					else debouncer = 5;}
					break;

			}
		}
		flaga_zmiany = 0;
		if (xSemaphoreTake(Tim20ms, portMAX_DELAY))
		{
      switch(klawisz)
			{
				case 0:
					if(HAL_GPIO_ReadPin(Klawisz_GPIO_Port, Klawisz_Pin))klawisz=0;
					else klawisz=1;
					break;
				case 1:
					if(HAL_GPIO_ReadPin(Klawisz_GPIO_Port, Klawisz_Pin))klawisz=1;
					else
						{
							if(klawiszWyswietlacza == 4) klawiszWyswietlacza = 0;
							else klawiszWyswietlacza++;
							EnkoderWartosc = 0;
							klawisz=2;
						}
					break;
				case 2:
					if(!HAL_GPIO_ReadPin(Klawisz_GPIO_Port, Klawisz_Pin))klawisz=2;
					else klawisz=3;
					break;
				case 3:
					if(!HAL_GPIO_ReadPin(Klawisz_GPIO_Port, Klawisz_Pin))klawisz=3;
					else klawisz=0;
					break;
			}
		}
			xQueueSend(HasloJendenZnak , &EnkoderWartosc, portMAX_DELAY);
			xQueueSend(ZmianaCyfry , &klawiszWyswietlacza, portMAX_DELAY);
			xQueueSend(ZmianaCyfry2 , &klawiszWyswietlacza, portMAX_DELAY);
  }
}

void vApplicationTickHook(void)
{
  static uint8_t time1s = 0;
  static uint8_t time100ms = 0;
  static uint8_t time50ms = 0;
  static uint8_t time20ms = 0;
  static uint8_t time2ms = 0;
  signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

		if (time2ms++ > 1)
		{
			time2ms = 0;
			xSemaphoreGiveFromISR(Tim2ms, &xHigherPriorityTaskWoken);
		}
    if (time1s++ > 250)
    {
			time1s = 0;
      xSemaphoreGiveFromISR(Tim1s, &xHigherPriorityTaskWoken);
    }
		if (time100ms++ >50)
		{
			time100ms = 0;
			xSemaphoreGiveFromISR(Tim100ms, &xHigherPriorityTaskWoken);
		}
		if (time50ms++ >25)
		{
			time50ms = 0;
			xSemaphoreGiveFromISR(Tim50ms, &xHigherPriorityTaskWoken);
		}
		if (time20ms++ >10)
		{
			time20ms = 0;
			xSemaphoreGiveFromISR(Tim20ms, &xHigherPriorityTaskWoken);
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
  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);

  vSemaphoreCreateBinary(Tim1s);
  vSemaphoreCreateBinary(Tim100ms);
	vSemaphoreCreateBinary(Tim50ms);
  vSemaphoreCreateBinary(Tim20ms);
  vSemaphoreCreateBinary(Tim2ms);

  Wys_A = xQueueCreate( 16, sizeof(uint8_t) );
	Wys_B = xQueueCreate( 16, sizeof(uint8_t) );
	Wys_C = xQueueCreate( 16, sizeof(uint8_t) );
	Wys_D = xQueueCreate( 16, sizeof(uint8_t) );

  HasloJendenZnak = xQueueCreate( 16, sizeof(uint8_t) );
  ZmianaCyfry = xQueueCreate( 16, sizeof(uint8_t) );
  ZmianaCyfry2 = xQueueCreate( 16, sizeof(uint8_t) );
  
  xTaskCreate(vTask1, "WyswietlaczSiedmioSegmentowy", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);
	xTaskCreate(vTask2, "PorownywanieHasla", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);
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

