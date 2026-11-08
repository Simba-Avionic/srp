# i2c_service (mw)

Middleware dostępu do magistrali I2C oraz biblioteki kontrolerów konkretnych chipów.

## TL;DR

Serwis `i2cMWService` jest jedynym procesem trzymającym `/dev/i2c-2`. Aplikacje używają `I2CController` (IPC `SRP.I2C`) albo wrapperów: INA219, ADS7828, PCA9685, 24LC32AT, BME280, LSM6DSOX.

## Opis funkcjonalności

Serwis nie zna chipów — tylko surowe akcje pod mutexem:

1. Parse nagłówka 3 B: `action | address | payload_size`.
2. `Ioctl(address)`.
3. Wykonanie Read / Write / PageWrite / WriteRead.
4. Odpowiedź bajtami.

## Architektura

```mermaid
flowchart TB
  subgraph Aplikacje
    Env[env_service EC]
    EnvFc[env_service FC]
    Servo[ServoService]
    Primer[primer_service]
  end
  subgraph Kontrolery
    BME[BME280]
    IMU[LSM6DSOX]
    PCA[PCA9685]
    ADC[ADS7828]
    EE[24LC32AT]
    INA[INA219]
  end
  Env --> ADC & EE
  EnvFc --> BME & IMU
  Servo --> PCA & EE
  Primer --> ADC
  BME & IMU & PCA & ADC & EE & INA --> IC[I2CController]
  IC -->|SRP.I2C| S[I2CService]
  S --> D[core::I2CDriver]
```

## Cykl życia

1. `Initialize`: tworzy `I2CDriver` i `StreamIpcSocket`; `i2c_->Init()` otwiera `/dev/i2c-2` (`O_RDWR`) — błąd → `kInitializeError` (`Warn`); bind `/run/SRP.I2C` — błąd → `kInitializeError`; rejestruje `RxCallback` i startuje wątek RX gniazda. Brak pliku konfiguracyjnego — serwis nie zna urządzeń.
2. `Run`: tylko czeka na stop token; po nim `StopRXThread`.
3. Obsługa żądań: pętla wątku RX gniazda (`accept` → `read` do 4096 B → `RxCallback` → `write` odpowiedzi → `close`). Połączenia są obsługiwane po kolei, a `RxCallback` dodatkowo trzyma `i2c_mtx`, więc w danej chwili wykonywana jest jedna transakcja I2C.

## Przepływ żądania

```mermaid
sequenceDiagram
  participant K as I2CController
  participant S as I2CService
  participant D as I2CDriver
  K->>S: 3 B nagłówka + payload
  S->>S: GetHeader, lock i2c_mtx
  S->>D: Ioctl address I2C_SLAVE
  S->>S: GetPayload
  S->>D: akcja wg action
  D-->>S: bajty lub blad
  S-->>K: dane, 1 jako OK, pusta jako blad
```

| Akcja | Wywołanie drivera | Odpowiedź |
|-------|-------------------|-----------|
| `kWrite`, payload 1 B | `Write(byte)` — jeden `write()` | `{1}` / pusta |
| `kWrite`, payload ≥ 2 B | `Write(vector)` — wymaga parzystej długości; każda **para** `{reg, val}` to osobny `write()` | `{1}` / pusta |
| `kPageWrite` | `PageWrite(vector)` — jeden `write()` całego bufora | `{1}` / pusta |
| `kRead` `{reg, n}` | `ReadWrite(reg, n)`: `write(reg)` + `read(n)` (dwa wywołania systemowe) | `n` B / pusta |
| `kWriteRead` `{n, b}` | `Write({b})` + `Read(n)` | `n` B / pusta |
| `kWriteReadBuffer` `{n, w…}` | `ReadWrite(w, n)`: jedna transakcja `I2C_RDWR` z powtórzonym START | `n` B / pusta |
| `kRES` (0x04) i inne | brak obsługi | pusta |

## Obsługa błędów

| Sytuacja | Odpowiedź / efekt |
|----------|-------------------|
| ramka < 3 B | `Warn "invalid header"`, pusta odpowiedź |
| `Ioctl` adresu nieudany | `Warn`, pusta odpowiedź (transakcja nie jest wykonywana) |
| ramka bez payloadu (dokładnie 3 B) | `Warn`, pusta odpowiedź |
| `kRead`: payload o nieparzystej długości | `Warn`, pusta odpowiedź |
| `kWriteRead`: payload ≠ 2 B; `kWriteReadBuffer`: payload < 2 B | `Warn`, pusta odpowiedź |
| NACK / błąd `write` lub `read` (w tym krótki odczyt) | `Warn`, pusta odpowiedź |
| zapis o nieparzystej długości > 1 B (`kWrite`) | odrzucony przez driver → pusta odpowiedź |
| brak serwisu w systemie | klient dostaje `nullopt` / `kConnectionError` (połączenie IPC nie powstaje) |

Pusta odpowiedź oznacza dla klienta błąd po stronie magistrali/chipa (nie transportu): `Write`/`PageWrite` zwracają `kConnectionError`, a `Read*` zwracają `optional` z pustym wektorem (nie `nullopt`). Szczegóły i konsekwencje dla kontrolerów chipów: [controller/README.md](controller/README.md). Pole `payload_size` z nagłówka nie jest porównywane z rzeczywistą długością ramki (payload to wszystko po 3 bajtach).

## API IPC

**Endpoint:** `SRP.I2C`

| Akcja | ID | Payload | Zachowanie |
|-------|-----|---------|------------|
| kRead | 0x01 | `{reg, size}` | `ReadWrite(reg, size)` |
| kWrite | 0x02 | bajty | zapis 1 lub N |
| kWriteRead | 0x03 | `{read_size, write_byte}` | 1 B write + N read |
| kRES | 0x04 | — | zarezerwowane |
| kPageWrite | 0x05 | bufor strony | `PageWrite` |
| kWriteReadBuffer | 0x06 | `{read_size, write…}` | `ReadWrite(write, size)` |

### I2CController

```cpp
ErrorCode Init(std::unique_ptr<ISocketStream> socket = ...);
ErrorCode Write(uint8_t address, std::vector<uint8_t> data);
ErrorCode PageWrite(uint8_t address, std::vector<uint8_t> data);
std::optional<std::vector<uint8_t>> Read(uint8_t address, uint8_t reg, uint8_t size = 1);
std::optional<std::vector<uint8_t>> WriteRead(uint8_t address, uint8_t WriteData, uint8_t ReadSize = 1);
std::optional<std::vector<uint8_t>> WriteReadBuffer(uint8_t address,
    const std::vector<uint8_t>& write_data, uint8_t read_size);
```

Brak SOME/IP (`provide: []`).

## Kontrolery chipów

| Chip | Adres | Rola | Kluczowe API |
|------|-------|------|----------------|
| **INA219** | podawany | prąd / napięcie szyny | `GetCurrent`, `GetVoltage` (16 V FSR, shunt 0.01 Ω, max 8 A) |
| **ADS7828** | 0x48 | ADC 12-bit, 8 ch, ref 3.3 V | `GetAdcRawRead`, `GetAdcVoltage` |
| **PCA9685** | 0x70 | PWM 50 Hz (serwomechanizmy) | `SetChannelPosition`, `ReadChannelPosition` |
| **24LC32AT** | 0x50 | EEPROM 4 KB, strony 32 B | `WriteByte/Page/Buffer`, `ReadByte/Sequential` |
| **ConfigManager** | EEPROM @ 0x0000 | 43 B: korekty PCA + 3× ID 1-Wire | `GetConfig` / `SetConfig` |
| **BME280** | 0x76 | T / P / H | `getTemperature/Pressure/Humidity` |
| **LSM6DSOX** | 0x6A | IMU (WHO_AM_I 0x6C) | `ReadGyroData`, `ReadAccelData` |
| **ADCSensorController** | przez ADS7828 | `y = a·V + b` per czujnik | `Init(path)`, `GetValue(sensor_id)` |

Każdy kontroler ma własny README w `controller/<chip>/` (rejestry, konwersje, obsługa błędów); przegląd klienta IPC i mocków: [controller/README.md](controller/README.md).

## Konfiguracja

Serwis I2C nie ma listy urządzeń ani pliku konfiguracyjnego (stałe: magistrala `/dev/i2c-2`, endpoint `SRP.I2C`). ADCSensor: `etc/config.json` z tablicą `sensors` (`sensor_id`, `channel`, `a`/`b` albo rezystancyjny model `R/RES_MIN/…`).

## Diagnostyka

JSON DID `i2c_Read_did` (30) i `i2c_Write_did` (20) **nie są podpięte** do działającego serwisu.

## Zależności

`//core/i2c:i2cdriver`, `//communication-core/sockets:socket_ipc`, `//mw/i2c_service/data` (nagłówek i fabryka ramek), `//core/common` (`condition`).

## Testy

| Target | Zakres |
|--------|--------|
| `//mw/i2c_service/tests:data_structure` | nagłówek 3 B i fabryka ramek |
| `//mw/i2c_service/tests:i2c_controller_test` | `I2CController` na mocku gniazda |
| `//mw/i2c_service/tests:ads7828_test` | tabela komend, napięcie |
| `//mw/i2c_service/tests:adcsensor_test` | `ADCSensorController`, parsowanie configu |

Test PCA9685 (`test_pca9685.cpp`) jest w `tests/BUILD` zakomentowany; brak UT dla `I2CService` (serwis) i pozostałych chipów.

## BUILD

| Target | Rodzaj |
|--------|--------|
| `//mw/i2c_service:i2cService` | binary |
| `//mw/i2c_service/controller:i2c_controller` | klient |
| `//mw/i2c_service/controller/ina219:ina219_controller` | chip |
| `//mw/i2c_service/controller/ads7828:adc_controller` | chip |
| `//mw/i2c_service/controller/pca9685:pca9685_controller` | chip |
| `//mw/i2c_service/controller/24lc32at:controller` | chip |
| `//mw/i2c_service/controller/bme280:bme280_controller` | chip |
| `//mw/i2c_service/controller/LSM6DSOX:LSM6DSOX_controller` | chip |
| `//deployment/mw/i2c:i2cMWService` | adaptive_application |
