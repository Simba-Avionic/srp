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

## wstepny zamysł

### error_code
| bit    | opis | przykład |
| ----- | --- | --- |
| 0-2 | oznaczenie elementu rakiety w którym występuje błąd | E[0x0]-silnik, R[0x1]-odzysk, B[0x2]-baterie, C[0x3]-komunikacja |
| 3-5 | waga błędu | DEBUG[0x0], INFO[0x1], WARNING[0x2], ERROR[0x3], CRITICAL[0x4] | 
| 6-16 | błąd[0x000-0x7FF] | 0x000-błąd rezystancji |


# OLD

## Metody

| ID | Opis | Argumenty | response |
| :----: | :------: | :----: | ---: |
| 0x7F00 | Rejestruje błąd | kod_błędu(uint16_t), dane_błędu | ID_błędu(uint16_t) | 
| 0x7F01 | Zwraca dane błędu | ID_błędu (uint16_t) | lenght(uint_16_t), dane błedu (string) |
| 0x7F02 |  Zwraca ilość błędów | | liczba błędów uint16_t |
| 0x7F03 | Resetuje wybrany błąd (endpoint dla aplikacji) | ID_błędu (uint16_t) | ID_błędu (uint16_t) |
| 0x7F04 | Resetuje wybrany błąd ( endpoint do ręcznego resetu) | ID_błędu (uint16_t) | ID_błędu (uint16_t) |
| 0x7FFD | Zwraca status aplikacji ??? | ID_aplikacji (uint16_t) | status (uint8_t) |

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
| error_status | uint8_t |
