# Temp Controller

## Wymagania
Program Bazuje na Application_MW, ipc_socket, network_data_structure
Możliwość Rejestracji klientów ( lista subskrybentów )
Publikowanie danych odczytanych z czujników z maksymalną gęstością
klasa dla klienta która  umożliwi rejestracje danego klienta i ustawienie callbacka dla otrzymywanej temperatury ( subsrcibe)
używamy jthread i stop_token
aplikacja musi posiadać config który określa które fizyczne id czujnika odpowiada któremu sensor_id


## Register
| name | size | Desc |
| --- | --- | --- |
| service_id | 2B | service_id subskrybenta |

Adres IPC tworzymy jako Engine.<service_id> (dec) dla service_id=0x0010 -> Engine.16


## Temp
| name | type | size | desc |
| --- | --- | --- | --- |
| sensor_id | uint16_t | 2B | id czujnika temperatury |
| value | float64 | 8B | wartość temperatury czujnika |


## lifetime temp controller
1. inicjalizacja aplikacji i uruchomienie socketu do odbierania ramki Register
2. Odczytywanie danych z maksymalną szybkością i wysyłanie do wszystkich subskrybentów

## lifetime klient class
### Init
1. ustawienie callbacka dla otrzymywanej temperatury
2. rejestracja w Temp Controller
3. Usypiamy główny wątek w nieskończoność