# Specyfikacja ramki diagnostycznej

| Nazwa | rozmiar [bajty] | Opis |
|-------|---------|------|
| Service_id | 2 | Numer identyfikacyjny serwisu |
| Client_id | 2 | Numer identyfikacyjny nadawcy |
| diag_id | 2 | Numer identyfikacyjny metody diagnostycznej |
| transfer_id | 2 | numer komunikacji |
| length | 1 | Długość pola "payload" [bajty] |
| payload | 0-255 | dodatkowe dane |
