/**@file Zamek.h
*/
#include "main.h"
#include "cmsis_os.h"
#include "gpio.h"

#define main_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

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
void vTask1(void *pvParameters);

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
void vTask2(void *pvParameters);

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
void vTask3(void *pvParameters);
