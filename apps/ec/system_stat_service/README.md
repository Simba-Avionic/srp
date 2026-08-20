# system_stat_service (EC)

Publikacja obciążenia CPU / RAM / dysku.

## TL;DR

`SysStatService` (id **522**) co 1 s wysyła `NewSystemUsage` z `core::stat::SystemStats`. Na EC pola są rzutowane do **uint8**; FC ma osobny serwis z float.

## Opis funkcjonalności

Pętla 1 Hz: `get_cpu_usage`, `get_ram_usage`, `get_disk_space` → event SOME/IP. Logger EC subskrybuje ten event.

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
| NewSystemUsage | 32769 | `srp.apps.SysStatType` {cpu, mem, disk} |

## Zależności

`//core/sys:system_status`.

## BUILD

- `//apps/ec/system_stat_service:system_stat_service`
