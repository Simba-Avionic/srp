# sim

Wspólny kanał sterowania driverami symulacyjnymi (`--define build_mode=sim`): `core/gpio`, `core/i2c`, `core/temp`, `core/uart`.

## TL;DR

Drivery sim nie dotykają sprzętu. Mają stan, który test ustawia plikiem `ctl`, i zapisują dziennik oraz stan do asercji. Pozwala to uruchomić pakiet EC na QEMU i odtwarzać scenariusze, również błędy magistrali i czujników.

## Kanał

Katalog `$SRP_SIM_DIR` (domyślnie `/tmp/srpsim`) na urządzeniu/gościu QEMU.

| Ścieżka | Kierunek | Opis |
|---|---|---|
| `ctl` | test → driver | polecenia, 1 na linię, `#` = komentarz; plik jest czytany ponownie przy każdej zmianie |
| `events.jsonl` | driver → test | dziennik (JSON lines): `{"t":<ms epoch>,"src":"gpio\|i2c\|temp\|uart",...}` |
| `state/gpio/<pin>` | driver → test | aktualna wartość pinu (`0`/`1`) |
| `state/i2c/0x<addr>.hex` | driver → test | zrzut pamięci urządzenia (hex) po każdym zapisie wielobajtowym |
| `state/uart/<port>.tx` | driver → test | bajty zapisane do portu, hex, 1 linia na `Write` (`<port>` bez `/`, `/` → `_`, np. `dev_ttyS1.tx`) |

Reguły `ctl`:

- Settery (`value`, `adc`, `mem`, `eeprom-cfg`, wartości czujników, `list`, `rx`, `rx-file`, `reset`) działają raz, gdy linia pojawi się pierwszy raz. Zmiana wartości = nowa linia.
- Usterki (`fault`) działają, dopóki linia jest w pliku; `count N` ogranicza liczbę zadziałań.
- Prefiks `after <ms>` opóźnia linię o podaną liczbę ms.
- Deterministycznie: brak losowości, jedyny czas to `after`.
- Usunięcie `ctl` kasuje usterki; ustawione wartości zostają (`reset` je przywraca).
- `events.jsonl` rośnie bez limitu — test czyści go przed uruchomieniem.

## GPIO

Domyślnie wszystkie piny = 0, kierunek IN.

```
gpio <pin> value <0|1>                   # wartość wejścia (getValue)
gpio <pin> fault set|get|any [count N]   # setValue -> kError; getValue -> 0xFF
gpio <pin> delay <ms>                    # każda operacja na pinie trwa ms
```

## I2C

Model magistrali z urządzeniami: `0x50` EEPROM 24LC32 (4 KiB, adres 16-bit), `0x48` ADS7828, `0x70` PCA9685, `0x6A` LSM6DSOX (WHO_AM_I=0x6C), `0x76` BME280 (id 0x60). Inny adres = NACK. `Write(vector)` ma semantykę prawdziwego drivera: pary `{reg, wartość}`.

```
i2c adc <kanał 0-7> <wolty>                 # ADS7828, 12 bit, Vref 3.3 V; poza zakresem = nasycenie 0/4095
i2c mem <addr> <offset> <hex...>            # zapis pamięci urządzenia (rejestry, bajty EEPROM)
i2c eeprom-cfg <xo|nan> <id1> <id2> <id3>   # EEPROM_config: pca9685_XO_corelation + 3 id czujników temp
i2c fault <addr> nack|timeout [count N]     # nack: błąd od razu; timeout: 1.5 s, potem błąd
i2c reset                                   # tylko stan wstrzykiwany: napięcia ADC = 0, EEPROM 0x50 = domyślna konfiguracja
i2c reset-all                               # cały model (także rejestry PCA/LSM/BME) do stanu początkowego
```

Domyślnie EEPROM ma poprawny `EEPROM_config` (bez niego `servo_driver` kończy proces), napięcia ADC = 0 V.

## Temp (1-Wire)

Domyślne czujniki: `28-0b2554dfd2cc`, `28-0b25550c2889`, `28-0b2554e84c21`, 25.0 °C.

```
temp <id> <C|err|timeout>   # wartość (także 85.0 i -127.0), błąd, albo 2 s i błąd
temp list <id>...           # lista czujników (GetAllAccessibleSensor)
temp remove <id>            # czujnik znika
temp reset                  # domyślna lista
```

Jak prawdziwy driver: 85.0 / -127.0 są zwracane bez walidacji.

## UART

`Open` działa na dowolnej ścieżce (np. `/dev/ttyS1`, którego na gościu QEMU nie ma), chyba że ustawiono usterkę. Dane do odczytu to kolejka FIFO per port, wstrzykiwana z `ctl`. `Read` zachowuje się jak prawdziwy driver (`VMIN=0`, `VTIME`): oddaje to, co jest w kolejce (maks. `size`, dla `size == 0` tyle, ile ustawia `default_read_size` z `UartConfig`: 256 B dla `Legacy`, 512 B dla `Mavlink`), a gdy kolejka jest pusta, śpi do timeoutu (`vtime` z `UartConfig`, dziesiąte części sekundy, domyślnie 1 s) i zwraca `nullopt`. `WaitForData(timeout, stop_token)` czeka tak samo i zwraca `false` po timeoucie albo od razu po `request_stop()` (jak `core::condition::wait_for`). Konfiguracja portu (prędkość, `raw`, `vmin`, `vtime`) trafia do `events.jsonl` (`"op":"config"`). Każdy `Write` trafia do `events.jsonl` (`"src":"uart","op":"write","port":...,"hex":...`) i do `state/uart/<port>.tx`.

```
uart <port> rx <hex...>                   # bajty do odczytu (kolejka FIFO), np. ramka MAVLink / NMEA
uart <port> rx-file <ścieżka>             # zawartość pliku (na gościu) do odczytu, np. strumień MAVLink
uart <port> fault open|read|write [count N]   # Open -> false; Read -> nullopt; Write -> kConnectionError
uart <port> timeout                       # Read/WaitForData ignorują kolejkę i kończą się timeoutem (nullopt/false)
uart reset                                # czyści kolejki RX wszystkich portów
```

Przykład (heartbeat MAVLink v1 w kolejce RX radia): `uart /dev/ttyS1 rx fe 09 00 ff be 00 00 00 00 00 06 08 00 04 03 49 21`.

## QEMU

1. `bazel run @srp_platform//tools/qemu -- --console` (sieć `tap0` wg `@srp_platform//tools/qemu`).
2. Gość na świeżym obrazie ma IP `192.168.10.101`, a `flash_ec` celuje w `192.168.10.51` (to IP ustawia też konfiguracja EC po flashu) — przed pierwszym flashem dodaj na gościu alias: `ip addr add 192.168.10.51/24 dev eth0`.
3. `bazel run //deployment/tools/ecu:flash_ec --config=bbb-platform --define build_mode=sim`.
4. `flash_ec` restartuje gościa — przed testami poczekaj, aż znowu odpowie na ssh (np. `until ssh root@192.168.10.51 true; do sleep 3; done`).

## BUILD

- `//core/sim:sim_ctl` (zawiera też `sim_uart.hpp`)

## Zapis `ctl` i sprzątanie

- `ctl` czytany jest ponownie przy zmianie (mtime, rozmiar, inode). Polecenia dopasowywane są **po treści linii**: wstawienie lub usunięcie linii nie resetuje stanu pozostałych (`applied`, `count`, `after`).
- Ostatnia linia bez `\n` jest ignorowana do czasu dopisania końca linii; pusty plik po niepustym nie kasuje stanu przez 1 s. Mimo to zapisuj atomowo: `cat > ctl.tmp && mv ctl.tmp ctl`, dopisuj przez `>>`. Usunięcie pliku (`rm ctl`) czyści stan natychmiast.
- Nieparzysta liczba cyfr hex albo znak spoza hex w `uart … rx`/`i2c …` powoduje pominięcie linii (z ostrzeżeniem), nic nie jest po cichu obcinane.
- `events.jsonl` i `state/uart/*.tx` rosną bez limitu, a `reset` ich nie czyści: przed testem usuń je (`rm -f $SRP_SIM_DIR/events.jsonl`).

## Różnice względem prawdziwych driverów

- GPIO: usterka `fault … set` jest zużywana przez `initializePin`, `setValue` i `setDirection` (wszystkie zapisują w sysfs); `initializePin` i `setDirection` zwracają wtedy `kError`. Usterka odczytu zwraca `static_cast<uint8_t>(ErrorCode::kError)` (= 2), tak jak realny `getValue`.
- I2C: `i2c reset` przywraca tylko stan wstrzykiwany z testu (napięcia ADC = 0, EEPROM 0x50 = domyślna konfiguracja) i nie kasuje rejestrów zapisanych przez aplikacje (PCA9685 0x70, LSM6DSOX 0x6A, BME280 0x76), jak prawdziwy układ; `i2c reset-all` przywraca cały model i przepisuje pliki stanu.
- I2C: usterka w `Write(vector)` i `PageWrite` daje `kInitializeError`, w `Write(uint8)` — `kConnectionError` (jak `core/i2c/i2c_driver.cpp`). `Read(0)` zwraca pusty wektor.
- UART: sim honoruje `timeout` z `Open` (deterministyczne czasy w testach); realny driver ignoruje ten parametr i ma `VTIME=10` na sztywno — rozbieżność do oceny przez testera.
- Temp: czas konwersji (`conv_ms`) jest stały (250 ms), niezależny od rozdzielczości.
