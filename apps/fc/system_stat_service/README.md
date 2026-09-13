# system_stat_service (FC)

Obciążenie hosta Flight Computer (float).

## TL;DR

`FcSysStatService` (id **523**) — analog EC, ale `FcSysStatType` z polami **float** i oferta startowana w `Run` (nie w `Initialize`). Event `NewSystemUsage` co 1 s.

## API SOME/IP

**Service:** `srp.apps.FcSysStatService`, **id 523**

| Event | ID | Payload |
|-------|---:|---------|
| NewSystemUsage | 32769 | `srp.apps.FcSysStatType` |

## Zależności

`//core/sys:system_status`.

## BUILD

- `//apps/fc/system_stat_service:system_stat_service`
