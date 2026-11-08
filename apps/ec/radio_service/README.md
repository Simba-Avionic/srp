# radio_service (EC)

Most GS ↔ rakieta po MAVLink (UART). Źródła EC; na platformie **w pakiecie FC** leci pełniejszy wariant — ten kod ma zredukowany TX i skeleton `FcRadioService`.

## TL;DR

`FcRadioService` (id **545**) na UART `/dev/ttyS1` @ 57600. RX: heartbeat GS (stan + aktuatory, **4 identyczne ramki**), RADIO_STATUS, komendy tankowania (→ FuelService), masa rakiety i ciśnienia zbiorników GSE (→ eventy). TX: HB i max alt co 5 s, GPS co 2 s, status tankowania co 1 s i przy zmianie (tank/computer telem wyłączone). Timeout: 2 min → CONNECTION_LOST, 15 min → ABORT.

## Opis funkcjonalności

- `MavRadioController` — odczyt/zapis MAVLink.
- `SomeIPController` — proxy Engine, Main, Servo, Fuel, Primer, Env, EnvFc, GPS, Recovery.
- `TelemetryProvider` — składanie ramek TX.
- `GSHeartbeatGuard` — watchdog łączności.
- Przy ARM heartbeat GS może sterować vent/dump.
- Heartbeat GPIO pin **2**.

**Uwaga:** `deployment/cpu/ec/BUILD` pakuje tę aplikację jako `//deployment/apps/radio_app:RadioApp`. Pełniejszy wariant na locie to FC radio.

## Architektura

```mermaid
flowchart LR
  GS[Ground Station] <-->|MAVLink UART| RC[MavRadioController]
  RC --> App[RadioApp]
  App -->|SetMode| Engine
  App -->|setMode| Main
  App -->|vent/dump| Servo
  App -->|RadioStatusEvent| SOMEIP[id 545]
```

## API SOME/IP

**Service:** `srp.apps.FcRadioService`, **id 545**

| Event | ID | Payload |
|-------|---:|---------|
| RadioStatusEvent | 32769 | `RadioDataType` |
| GSEOxidizerTankPressEvent | 32770 | uint16 |
| GSERocketMassEvent | 32771 | int16 |
| GSEPressureFeedSystemTankPressEvent | 32772 | uint16 |

Model deklaruje metody `SetGSEOxidizerFillValve` (1) i `SetGSEPFSFillValve` (2) (`uint8` → `bool`), ale `RadioApp` ich nie implementuje; komendy idą MAVLink → proxy innych usług.

## Timeouty GS

| Czas bez HB | Stan |
|-------------|------|
| 2 min | CONNECTION_LOST |
| 15 min | ABORT |

Oba idą jako `EngineService.SetMode` + `MainService.setMode`.

## Zależności

`core/uart`, `core/onTimerCallback`, GPIO, MAVLink dialect `simba`.

## BUILD

- `//apps/ec/radio_service:radio_service`
