# MavlinkDaemon on the EC

The EC runs one `mavlink_daemon` that owns both radio links; applications with `on: mavlink` are its clients
(modelled on `someip_demon`). The binary is built in this directory by `mavlink_daemon_binary`
(`BUILD`): the radio drivers are linked in statically, so the EC package has **no `*.so` and no driver directory**.
The application model (service `MavlinkLinkService` on `ipc`, functional groups, logger) is the platform one
(`@srp_platform//platform/common/mavlink_daemon/model:instance`).

## Package contents (`//deployment/cpu/ec:pkg`)

| path on the EC | file |
|----------------|------|
| `/srp/opt/mavlink_daemon/bin/mavlink_daemon` | the daemon binary built here |
| `/srp/opt/mavlink_daemon/mavlink_daemon_config.json` | daemon config (this directory), copy in `etc/` |
| `/srp/opt/mavlink_daemon/serial_radio_a.json`, `serial_radio_b.json` | driver files of the links (`device`, `baudrate`), copies in `etc/` |

## Config `mavlink_daemon_config.json`

The config is strict: an unknown or removed key (`version`, `driver_dir`, `heartbeat_hz`, `restart_delay_ms`) is a
start error that names the key. `ut/config_test.py` (`bazel test //deployment/platform/mavlink_daemon:config_test`)
starts the host variant of the daemon with this very file (the links on `serial_radio`, devices replaced by a pty) and checks the
rejection of the removed keys. Key reference: `platform/common/mavlink_daemon/README.md` in `srp_platform`.

| link | role | `air_rate_bps` | `mtu` | `method_timeout_ms` |
|------|------|----------------|-------|---------------------|
| `radio_a` | main radio | 9600 | 200 | 1000 |
| `radio_b` | long range, events | 2400 | 64 | 4000 |

`mtu` is the largest frame of the radio (the generator checks the frames against `mtu`); with a real radio driver it must match the packet length of the module.
`signing` is disabled in the example; to enable it set `enabled`, `key_file`, `link_id` (unique per signed link),
`state_file`.

## Radio drivers: placeholder on the EC

The daemon does **not** use `/dev/ttyS1` on the EC. Both links use `serial_radio` (a plain UART, only `device` and
`baudrate`) as a placeholder for tests; the proper radio driver (the E220 module of the EC or another) is added by the
user in srp. Reason: `RadioApp` (`apps/ec/radio_service`) owned `/dev/ttyS1` (57600); it is commented out of
`//deployment/cpu/ec:pkg` (the code stays), so that the daemon is the only owner of the radio links once a driver is chosen.

**To be confirmed on the hardware:** `device` (`/dev/ttyS2`, `/dev/ttyS4`) and `baudrate` in `serial_radio_a.json`,
`serial_radio_b.json` are placeholders, as are all values of `air_rate_bps` and `mtu`; they depend on the EC board
and the modules. The `ebyte_e220` driver stays linked into the binary (`MAVLINK_DRIVERS` in `BUILD`, keys:
`core/radio/e220/README.md`, incl. board dependent `aux_idle_level`), but the config does not use it.

## Adding an element to a radio

An element (an event, a field or a method of a service) goes through the radio only if it is on the white list
`elements` of the `on: mavlink` entry in the `app.json` of the application, with a limit per radio:

```json
{
  "name": "EngineService as Engine",
  "on": "mavlink",
  "instance": 2,
  "elements": {
    "Telemetry": { "radio_a": { "max_rate_hz": 10 }, "radio_b": { "max_rate_hz": 0.2 } },
    "Position":  { "radio_b": { "max_rate_hz": 1 } },
    "SetMode":   { "radio_a": {} }
  }
}
```

* the radio names are the keys of `links` in `mavlink_daemon_config.json` (`radio_a`, `radio_b`); an unknown radio is a generator error;
* an element that is not in `elements` does not go through the radio;
* an event on a radio with `air_rate_bps` needs `max_rate_hz` (otherwise the budget cannot be computed: generator error);
* a method on more than one radio gets a warning: the REQ is sent by both radios and executes twice.

### Radio budget

The model generator writes `<dialect>_links.md` with one table per radio: for every element the worst-case frame
(header 10 B + CRC 2 B + 13 B signature on a signed link + the full payload) times `max_rate_hz`, plus HEARTBEAT, the sum
in B/s and the percentage of `air_rate_bps`. A load above `max_utilization` (0.8) is a warning, above 100% a generator
error; a frame longer than `mtu` is an error too. Check `_links.md` after every change of `elements` or of a radio.
Methods are listed without summing (no rate). The daemon additionally paces the TX of every link to
`air_rate_bps x max_utilization`.

## Radio link state in an srp application: `MavlinkLinkService`

An application that wants to adapt to the radio (rate, mode) adds the platform service as a `require` entry
with `on: ipc`:

```json
"include": ["@(srp_platform)/platform_definition/mavlink_link_service/mavlink_link_service.json"],
...
"require": [
  { "name": "srp.platform.mavlink.MavlinkLinkService as LinkRPort", "on": "ipc", "instance": 1 }
]
```

and uses `MavlinkLinkServiceProxy` from `srp/platform/mavlink/MavlinkLinkService/MavlinkLinkServiceHandler.h`: the
event `LinkStats` (per link, every `stats_period_ms`) and the method `GetLinkStats(link)`. `ipc` is local to the CPU:
the application sees the links of the daemon on its own CPU. See `docs/mavlink/README.md` in `srp_platform`.

## Known gap: FuelService and FcRadioService

`FuelService` (`deployment/apps/fuel_service/app_config.json`) requires `FcRadioService` (ipc, instance 2), whose
only provider is `RadioApp`; `RadioApp` is off the EC package (see `deployment/cpu/ec/BUILD`). Until `FcRadioService`
is moved to MAVLink (`on: mavlink`, planned S5), the `FuelService` controllers get neither the rocket mass
(`GSERocketMassEvent`) nor the oxidizer tank pressure (`GSEOxidizerTankPressEvent`) from the FC.
