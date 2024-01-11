# OTA communication

## Założenia
obsługujemy tylko pliki tar (narazie?)

maksymalna wielkość pliku = +-16GB

data_ID = 0x00 oznacza koniec pliku payload zawiera największe data_ID transmisji

## SOME/IP
korzystamy z REQ/RES
Jeśli RES nie nadejdzie nadajemy fragment danych jeszcze raz
po 3 nieudanych próbach zwracamy timeout error i zamykamy transmisje

### RES
| nazwa | długość | opis |
| --- | --- | --- |
| data_ID | 2B | numer odebranej porcji danych (numeracje zaczynamy od 0x01) |
| CRC | 2B | Obliczone przez odbiorce |

### ostatnia ramka
| nazwa | długość | opis |
| --- | --- | --- |
| last_data_ID | 2B |

### payload structure
| nazwa | długość | opis |
| --- | --- | --- |
| data_ID | 2B | numer wysłanej porcji danych (numeracje zaczynamy od 0x01) |
| data | max 253B | wysylany fragment pliku |
| CRC | 2B | sprawdzenie poprawności danych |

