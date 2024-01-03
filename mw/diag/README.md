# DTC Service

## SIMBA.DIAG.DTC

 Do nadawania nowej wartości dla kodu DTC. Jest to komunikacja typu UDP bez informacji zwrotnej.

Funkcjonalność ta jest dostarczona do aplikacji która będzię zgodna z interfejsem iapplication. Dokładnie dla każdej aplikacji która nie dziedziczy po Application_no_IPC.

 ### Frame
 | Nazwa | rozmiar [bajty] | Opis |
|-------|---------|------|
| DTC_id | 2 | Numer błędu |
| DTC_status | 1 | flagi błędu |
| length | 1 | Długość pola "payload" [bajty] |
| payload | 0-255 | dodatkowe dane opcjonalnie w celu przekazania jakiś danych |

W polu payload przekazujemy tylko i wyłącznie jakieś wartości typu dane z sensora przy których powstał błąd


# OLD

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
| 0x0003 | 0x7FFE | 0x02 | 0x12 | -> 0x02 0xFA |
| 0x0004 | 0x7FFF | 0x00 | | -> 0x0000-0xFFFF |


## Metody

| ID | Opis | Argumenty | response |
| :----: | :------: | :----: | ---: |
| 0x7F00 | Rejestruje błąd | kod_błędu(uint16_t), dane_błędu | ID_błędu(uint16_t) | 
| 0x7F01 | Zwraca dane błędu | ID_błędu (uint16_t) | lenght(uint_16_t), dane błedu (string) |
| 0x7F02 |  Zwraca ilość błędów | | liczba błędów uint16_t |
| 0x7F03 | Resetuje wybrany błąd (endpoint dla aplikacji) | ID_błędu (uint16_t) | ID_błędu (uint16_t) |
| 0x7F04 | Resetuje wybrany błąd ( endpoint do ręcznego resetu) | ID_błędu (uint16_t) | ID_błędu (uint16_t) |
| 0x7FFD | Zwraca status aplikacji ??? | ID_aplikacji (uint16_t) | status (uint8_t) |

## Kody błędów

| Kod błędu 4b | Litera(oznaczenie) | opis | przykład |
| :--- | :---  | --- | ---: |
| 0x1 | E | silnik | zła rezystancja spłonki, zaklinowany zawór |
| 0x2| R | odzysk | zaklinowany zawór |
| 0x3| C | comunication | serwis nie znaleziony, timeout error |
| 0x4| B | batery | zwarcie na lini 12V (stage 2) |
| 0x5| A | avionics | |


### TODO przemyśleć kody błędów
| Kod błędu 12b | opis | Co zrobić ? |
| :--- | --- | ---: |
| 0x000 | serwis nie odpowiada | Jeśli błąd się powtarza zalecany restart aplikacji |
| 0x001 | zła rezystancja spłonki| Zalecane sprawdzenie połączenia spłonki |

## Status błędu
| Nazwa | hex | Opis |
| :--- | --- | ---: |
| kCreate | 0x0 | Zarejestrowano błąd |
| kAutoReset | 0x1 | Zresetowano automatycznie |
| kManReset | 0x2 | Zresetowano ręcznie |


## Baza danych 
| Opis | Typ |
| :--- | --- |
| error_id | uint16_t |
| error_code | uint16_t |
| error_details | std::string |
