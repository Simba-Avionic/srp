# apogee_service (FC)

Detekcja apogeum i progu głównego spadochronu na podstawie BME + IMU.

## TL;DR

`ApogeeDetectService` (id **555**) subskrybuje `EnvAppFc` (wysokość z BME, `accel_z` z IMU), odpala `RealTimeApogee` co 100 ms. Publikuje `newApogeeDetected` i `newMainParachuteDetected` (wysokość ≤ 400 m). **Nie wywołuje** `MainService.setMode(APOGEE)` — recovery musi dostać APOGEE z GS/radia (albo innego klienta) po FLIGHT.

## Opis funkcjonalności

- Detektor: `RealTimeApogee(15, -0.5, 0.0)`.
- Wysokość: pole `altitude` z eventu BME280.
- Prędkość: obecnie `accel_z` (w kodzie TODO: która oś jest poprawna).
- Po apogeum, gdy `height ≤ 400` m → event głównego spadochronu.

## Architektura

```mermaid
flowchart LR
  Env[EnvAppFc] -->|newBME280Event altitude| Apo[ApogeeService]
  Env -->|newIMUEvent accel_z| Apo
  Apo --> RT[RealTimeApogee]
  RT -->|isApogeeReached| E1[newApogeeDetected]
  Apo -->|h ≤ 400 m| E2[newMainParachuteDetected]
  E1 --> Logger[FcFileLogger]
```

```mermaid
sequenceDiagram
  participant Env as EnvAppFc
  participant A as ApogeeService
  participant M as MainService
  Env-->>A: altitude + accel_z
  A->>A: EvaluateApogee co 100 ms
  A-->>A: Update newApogeeDetected
  Note over M: APOGEE wchodzi tylko przez setMode<br/>(Radio/GS), nie z tego eventa
```

## API SOME/IP

**Service:** `srp.apps.ApogeeDetectService`, **id 555**

| Event | ID | Payload |
|-------|---:|---------|
| newApogeeDetected | 32769 | bool |
| newMainParachuteDetected | 32770 | bool |

Brak metod.

## Zależności

`core/apogee`, proxy `EnvAppFc`.

## BUILD

- `//apps/fc/apogee_service:apogee_service_fc`
