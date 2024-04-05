# Specyfikacja ramki diagnostycznej dtc

 | Nazwa | rozmiar [bajty] | Opis |
|-------|---------|------|
| DTC_id | 2 | Numer błędu [uint16_t] |
| DTC_status | 1 | flagi błędu [uint8_t] |
| length | 1 | Długość pola "payload" [uint8_t] (bajty) |
| payload | 0-255 | dodatkowe dane opcjonalnie w celu przekazania jakiś danych [std::vector<uint8_t>] |


## dtc_status
| status | desc |
| --- | --- |
| 0x00 | idk |


## błędy

| DTC_id | name | desc | payload |
| --- | --- | --- | --- |
| 0x0000 | error Read file |  | uint16_t service_id ( if known, else 0x0000), std::string file_name |