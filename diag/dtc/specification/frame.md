# Specyfikacja ramki diagnostycznej dtc

 | Nazwa | rozmiar [bajty] | Opis |
|-------|---------|------|
| DTC_id | 2 | Numer błędu [uint16_t] |
| DTC_status | 1 | flagi błędu [uint8_t] |
| length | 1 | Długość pola "payload" [uint8_t] (bajty) |
| payload | 0-255 | dodatkowe dane opcjonalnie w celu przekazania jakiś danych [std::vector<uint8_t>] |

