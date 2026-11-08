# system_stat_service (EC)

Publikacja obciążenia CPU / RAM / dysku.

## TL;DR

`SysStatService` (id **522**) co 1 s wysyła `NewSystemUsage` z `core::stat::SystemStats` (wartości w %). Pola `SysStatType` są `float32`, ale EC rzutuje je wcześniej do **uint8** (część całkowita); FC ma osobny serwis (`FcSysStatType`) z pełnym float.

## Opis funkcjonalności

Pętla 1 Hz: `get_cpu_usage` (% z próbkowania `/proc/stat` co 100 ms, nie średnia 1 min), `get_ram_usage` (`/proc/meminfo`: (MemTotal−MemAvailable)/MemTotal), `get_disk_space` (zajęte % partycji `/`) → event SOME/IP; gdy brak próbki CPU/RAM, cykl jest pomijany. Logger EC subskrybuje ten event.

## Architektura

```mermaid
flowchart LR
  Proc["/proc + /"] --> SS[SystemStats]
  SS --> App[system_stat_service]
  App -->|1 Hz| EV[NewSystemUsage]
```

## API SOME/IP

**Service:** `srp.apps.SysStatService`, **id 522**

| Event | ID | Payload |
|-------|---:|---------|
| NewSystemUsage | 32769 | `srp.apps.SysStatType` {cpu_usage, mem_usage, disk_utilization} (float32) |

## Zależności

`//core/sys:system_status`.

## BUILD

- `//apps/ec/system_stat_service:system_stat_service`
