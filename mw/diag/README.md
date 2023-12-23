# DTC Service

## ramka IPC
| Pole | długość w Bajtach |
| :--- | :---: |
| Service_ID | 2 |
| Method_ID |2 |  
| Lenght | 2 |
| Payload |  |

## Przykładowa ramka
| Service_ID | Method_ID | Lenght | Payload | Response|
| :--- | ---| :--- | ---: | :--- |
| 0x0001 | 0x7FFC | 0x02 | 0x11 | -> 0x0/0x1 |
| 0x0002 | 0x7FFB | 0x03 | 0x1000 0x01 | -> 0x0/0x1 |
| 0x0001 | 0x7FFD | 0x02 | 0x0001 | -> 0x0-0x8 |
| 0x0003 | 0x7FFE | 0x02 | 0x12 | -> 0x02 0xfa |
| 0x0004 | 0x7FFF | 0x00 | | -> 0x00-0xFFFFFFFF |


## Metody

| ID | Opis | Argumenty | response |
| :----: | :------: | :----: | ---: | 
| 0x7FFF  |  Zwraca ilość błędów | | liczba błędów uint32_t |
| 0x7FFE | Zwraca dane błędu | ID_błędu (uint16_t) | lenght(uint_16_t), dane błedu (string) |
| 0x7FFD | Zwraca status aplikacji | ID_aplikacji (uint16_t) | status (uint8_t) |
| 0x7FFC | Resetuje wybrany błąd | ID_błędu (uint16_t) | OK? (uint8_t) |
| 0x7FFB | Rejestruje błąd | kod_błędu(uint16_t), dane_błędu | OK? (uint8_t) |

## Kody błędów

| Kod błędu 4b | Litera(oznaczenie) | opis | przykład |
| :--- | :---  | --- | ---: |
| 0x1 | E | silnik | zła rezystancja spłonki, zaklinowany zawór |
| 0x2| R | odzysk | zaklinowany zawór |
| 0x3| C | comunication | serwis nie znaleziony, timeout error |
| 0x4| A | avionics | zwarcie na lini 12V (stage 2) |


### TODO przemyśleć kody błędów
| Kod błędu 12b | opis | Co zrobić ? |
| :--- | --- | ---: |
| 0x000 | serwis nie odpowiada | Jeśli błąd się powtarza zalecany restart aplikacji |
| 0x001 | zła rezystancja spłonki| Zalecane sprawdzenie połączenia spłonki |
