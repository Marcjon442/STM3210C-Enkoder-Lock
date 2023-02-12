# Enkoder-Lock-STM3210C

## Opis
Przedmiotem projektu jest Zamek szyfrowy. Zamek został zaprojektowany na płycie uruchomieniowej ZL29ARM firmy KAMAMI.
Płyta wyposażona jest w mikrokontroler STM32F107VCT6.
Na mikrokontrolerze uruchomiony został system wbudowany FreeRTOS, dla którego zaprojektowane zostały poszczególne Taski, kolejki oraz semafory.
![alt text](https://github.com/Marcjon442/STM3210C-Enkoder-Lock/blob/main/Schemat.png)
### Opis wyprowadzeń

- Anoda_1 -> PB1
- Anoda_2 -> PB5
- Anoda_3 -> PB9
- Anoda_4 -> PB14

- Katoda_A -> PD0
- Katoda_B -> PD1
- Katoda_C -> PD2
- Katoda_D -> PD3
- Katoda_E -> PD4
- Katoda_F -> PD5
- Katoda_G -> PD6
- Katoda_H -> PD7

- Enkoder_1 -> PC4
- Enkoder_2 -> PB15
- Potwierdzenie -> PC12

- LED_1 -> PE14
- LED_2 -> PE15
