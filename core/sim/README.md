# sim

Wspólny kanał sterowania driverami symulacyjnymi (`--define build_mode=sim`): `core/gpio`, `core/i2c`, `core/temp`.

## TL;DR

Drivery sim nie dotykają sprzętu. Mają stan, który test ustawia plikiem `ctl`, i zapisują dziennik oraz stan do asercji. Pozwala to uruchomić pakiet EC na QEMU i odtwarzać scenariusze, również błędy magistrali i czujników.

## Kanał

Katalog `$SRP_SIM_DIR` (domyślnie `/tmp/srpsim`) na urządzeniu/gościu QEMU.

| Ścieżka | Kierunek | Opis |
|---|---|---|
| `ctl` | test → driver | polecenia, 1 na linię, `#` = komentarz; plik jest czytany ponownie przy każdej zmianie |
| `events.jsonl` | driver → test | dziennik (JSON lines): `{"t":<ms epoch>,"src":"gpio\|i2c\|temp",...}` |
| `state/gpio/<pin>` | driver → test | aktualna wartość pinu (`0`/`1`) |
| `state/i2c/0x<addr>.hex` | driver → test | zrzut pamięci urządzenia (hex) po każdym zapisie wielobajtowym |

Reguły `ctl`:

- Settery (`value`, `adc`, `mem`, `eeprom-cfg`, wartości czujników, `list`, `reset`) działają raz, gdy linia pojawi się pierwszy raz. Zmiana wartości = nowa linia.
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
i2c reset                                   # domyślna zawartość urządzeń i napięcia
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

## QEMU

1. `bazel run @srp_platform//tools/qemu -- --console` (sieć `tap0` wg `@srp_platform//tools/qemu`).
2. Gość na świeżym obrazie ma IP `192.168.10.101`, a `flash_ec` celuje w `192.168.10.51` (to IP ustawia też konfiguracja EC po flashu) — przed pierwszym flashem dodaj na gościu alias: `ip addr add 192.168.10.51/24 dev eth0`.
3. `bazel run //deployment/tools/ecu:flash_ec --config=bbb-platform --define build_mode=sim`.
4. Po reboocie gościa: `deployment/bazel/flash_sim.sh root@192.168.10.51` czeka, aż gość znowu odpowie na ssh.

Uwaga: `flash_ecu` w `srp_platform` (release-2503, `tools/common/flash_ecu.bzl`) nie przekazuje parametru `tool` do reguły, więc `select(//:sim → //deployment/bazel:docker_flash_sh)` w `deployment/tools/ecu/BUILD` nie ma dziś efektu — krok 4 trzeba wywołać ręcznie, dopóki platforma tego nie obsłuży.

## BUILD

- `//core/sim:sim_ctl`
