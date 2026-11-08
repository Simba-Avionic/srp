# logger_service (EC)

Pokładowy rejestrator CSV lotu na Engine Computer.

## TL;DR

`FileLoggerApp` (id **517**): `Start`/`Stop` oraz event `LoggingState`. Zapis rusza dopiero po `Start` (lub DID 16 = 1) i kończy się po `Stop` (DID 16 = 0) albo zamknięciu aplikacji; wtedy co **10 ms** dopisuje wiersz do `/home/root/<czas systemowy>_log.csv` z danymi Env, SysStat, Primer, Servo, Engine, **SecEnv**, **SecServo**, stanem tankowania (Fuel) i GPIO 1–17.

## Opis funkcjonalności

- Subskrypcja eventów SOME/IP (IPC/UDP) i pinów GPIO.
- `LoggingState` (0/1) publikowany co 1 s. Heartbeat GPIO pin **2**.
- Znacznik czasu z `TimestampController` (SHM).
- DID 16: zapis 0/1 = stop/start.
- FC ma osobny logger (`FcFileLoggerApp`) z IMU/BME/GPS — ten plik loguje napęd i zawory.

## Architektura

```mermaid
flowchart TB
  Env[EnvApp] --> L[LoggerService]
  Stat[SysStat] --> L
  Primer --> L
  Servo --> L
  Engine --> L
  SecEnv --> L
  SecServo --> L
  Fuel[Fuel refuelingState] --> L
  GPIO[piny 1–17] --> L
  L -->|10 ms| CSV[CSVDriver]
  CSV --> File[("/home/root/*_log.csv")]
  UDS[DID 16] --> L
```

## API SOME/IP

**Service:** `srp.apps.FileLoggerApp`, **id 517**

| Rodzaj | Nazwa | ID | Typ |
|--------|-------|---:|-----|
| Method | `Start` | 1 | void → bool |
| Method | `Stop` | 2 | void → bool |
| Event | `LoggingState` | 32769 | uint8 |

## Diagnostyka

`file_logger_did`, `sub_service_id` **16** — write 0/1 start/stop.

## Zależności

`core/csvdriver`, `core/timestamp`, GPIO controller, proxy wszystkich serwisów EC + SEC_EC.

## BUILD

- `//apps/ec/logger_service/service:logger_service`
