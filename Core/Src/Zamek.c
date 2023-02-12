/**
  ******************************************************************************
  * @file    Zamek.c
  * @brief   Definicje funkcji Task
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

#include "Zamek.h"

const unsigned char seg7[] = {0xC0, 0xF9, 0xA4, 0xB0,
                              0x99, 0x92, 0x82, 0xF8,
                              0x80, 0x90, 0x5F};

uint8_t LED_buf[4];
uint8_t LED_ptr;
															
extern xSemaphoreHandle Tim50ms;
extern xSemaphoreHandle Tim20ms;
extern xSemaphoreHandle Tim2ms;

extern xQueueHandle Wys_A;
extern xQueueHandle Wys_B;
extern xQueueHandle Wys_C;
extern xQueueHandle Wys_D;

extern xQueueHandle HasloJendenZnak;
extern xQueueHandle ZmianaCyfry;
extern xQueueHandle ZmianaCyfry2;

/* USER CODE BEGIN Header_vTask1 */
/**
* @brief Funkcja obslugi wyswietlacza siedmio-segmentowego
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTask1 */
															
/**
*<pre>
*void vTask1();
*</pre>
*
*\brief Obsluga wyswietlacza
*
*Odpowiedzialne za:
*odczytanie danych z kolejki,
*zapisanie danych do tablicy znakow (numer komorek tablicy odpowiada numerowi wyswietlacza,
*wysylanie odpowiedniej zmiennej na dany wysiwetlacz,
*uaktywnianie tylko jednego wyswietlacza
*/
void vTask1(void *pvParameters)
{
  static uint8_t znak[4] = {0,0,0,0};
	uint8_t wyswietl = 0;
	
	for( ;; )
	{
		/* Odswierzanie wyswietlaczy z czestoscia 2ms */
		if (xSemaphoreTake(Tim2ms, portMAX_DELAY))
		{
			/* Pobieranie danych z kolejek */
			xQueueReceive(Wys_A, &znak[0], ( TickType_t ) 0 );
			xQueueReceive(Wys_B, &znak[1], ( TickType_t ) 0 );
			xQueueReceive(Wys_C, &znak[2], ( TickType_t ) 0 );
			xQueueReceive(Wys_D, &znak[3], ( TickType_t ) 0 );
			xQueueReceive(ZmianaCyfry2, &wyswietl, (TickType_t)0);
			LED_buf[0] = seg7[znak[0]];
			LED_buf[1] = seg7[znak[1]];
			LED_buf[2] = seg7[znak[2]];
			LED_buf[3] = seg7[znak[3]];
		  /* obsluga wyswietlacza siedmiosegmentowego LED */
			if ((++LED_ptr) > 3) LED_ptr = 0;
			
			/* Resetowanie wyswietlaczy */
      HAL_GPIO_WritePin(Anoda_1_GPIO_Port, Anoda_4_Pin|Anoda_3_Pin|Anoda_2_Pin
                                          |Anoda_1_Pin, GPIO_PIN_RESET);
		  /* Ustawienie segmentow */
      Katoda_A_GPIO_Port->BSRR = (uint32_t)LED_buf[wyswietl];
		  Katoda_A_GPIO_Port->BSRR = (uint32_t)(~LED_buf[wyswietl]) << 16;
		
		  /* Wybor wyswietlacza */
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
		}
	}
}

	/* USER CODE BEGIN Header_vTask2 */
/**
* @brief Sprawdzanie poprawnosci wprowadzonego hasla
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTask2 */
															
/**
*<pre>
*void vTask2();
*</pre>
*
*\brief Sprawdzanie hasla
*
*Odpowiedzialne za:
*odczytanie danych z kolejki,
*zapisanie danych do tablicy znakow (numer komorek tablicy odpowiada numerowi wyswietlacza,
*porównanie wprowadzonego hasla z haslem wzorcowym
*uaktywnienie diod led imitujacych otwarcie zamka
*/
void vTask2(void *pvParameters)
{
  static uint8_t dane_enkoder = 0;
  static uint8_t klawisz_enkoder = 0;
  static uint8_t dane_wyswietlacz[4] = {0, 0, 0, 0};

  for(;;)
  {
		/* Odswierzanie wyswietlaczy z czestoscia 50ms */
    if(xSemaphoreTake(Tim50ms, portMAX_DELAY))
    {
			/* Pobieranie danych z kolejek */
      xQueueReceive(HasloJendenZnak, &dane_enkoder, (TickType_t)0);
      xQueueReceive(ZmianaCyfry, &klawisz_enkoder, (TickType_t)0);
      dane_wyswietlacz[klawisz_enkoder] = dane_enkoder;
      if(klawisz_enkoder == 4)
      {
				/* Sprawdzanie poprawnosci wprowadzonego hasla */
        if(dane_wyswietlacz[0] == 1 && dane_wyswietlacz[1] == 2 && dane_wyswietlacz[2] == 3 && dane_wyswietlacz[3]== 4)
        {
					/* uruchamianie diod LED imitujacych otwarcie zamka */
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

	/* USER CODE BEGIN Header_vTask3 */
/**
* @brief Funkcja obslugujaca enkoder obrotowy
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_vTask3 */
															
/**
*<pre>
*void vTask3();
*</pre>
*
*\brief Enkoder obrotowy
*
*Odpowiedzialne za:
*pobieranie danych z enkodera,
*interpretowanie kierunku obracania enkodera,
*zmiana wartosci zmiennej przechowujacej aktualna wartosc
*wysylanie kolejka danych
*/
void vTask3(void *pvParameters)
{
	
  static uint8_t EnkoderWartosc = 0;
	static uint8_t flaga_zmiany = 0;
  static uint8_t klawiszWyswietlacza = 0;
	static uint8_t debouncer = 0, klawisz = 0;
	static uint8_t left = 0, right = 0;

  for(;;)
  {
		/* Odswierzanie wyswietlaczy z czestoscia 2ms */
    if (xSemaphoreTake(Tim2ms, portMAX_DELAY))
		{
			/* Pobieranie danych z Enkodera */
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
		/* Odswierzanie wyswietlaczy z czestoscia 20ms */
		if (xSemaphoreTake(Tim20ms, portMAX_DELAY))
		{
			/* Debouncer przycisku potwierdzajacego zapisanie hasla */
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
		/* Wysylanie danych stosowna kolejka */
			xQueueSend(HasloJendenZnak , &EnkoderWartosc, portMAX_DELAY);
			xQueueSend(ZmianaCyfry , &klawiszWyswietlacza, portMAX_DELAY);
			xQueueSend(ZmianaCyfry2 , &klawiszWyswietlacza, portMAX_DELAY);
  }
}

