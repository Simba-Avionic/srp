# Ebyte E220-400T30S driver (`core/radio/e220`)

Driver for the **E220-400T30S** LoRa modem as the **`ebyte_e220` plugin** of MavlinkDaemon (`ara::com::mavlink::IRadioDriver`
from `srp_platform`). The platform knows nothing about the modem - all knowledge about the registers, the M0/M1/AUX pins and
the TX gating is here. The daemon loads `libebyte_e220.so` from its driver directory (`driver_dir`); the link
(`links.lora.driver = "ebyte_e220"`) points to the driver file (`driver_config`) - see "Configuration" and "EC deployment".

## Source of knowledge about the module

All register values, modes, timings and limits come from the official Ebyte manual:
**"E220-xxxTxxx Series User Manual"** (page header: "E220-T Series User Manual"), Chengdu Ebyte Electronic Technology Co., Ltd.,
revision **1.8** (last entry of the revision history, p. 54), 54 pages, downloaded from `https://www.cdebyte.com/products/E220-400T30S/4`
(Downloads / Manual). The manual does not number its tables, so sections and pages are cited:

| Section (page) | What it gives the driver |
|---|---|
| 5.1 (17) | modes from the M1/M0 pins: `00` transmission, `11` configuration |
| 5.2.1 (24), 5.2.4 (27-28) | AUX: ~10 ms busy after a mode change; the mode is changed while AUX is idle and we wait for AUX idle again |
| 5.2.3 (25-26) | the module UART buffer is **400 B**, anything above is lost; AUX is busy while the module receives a frame from the UART and transmits it -> one portion per AUX cycle |
| 6.1 (30-31) | configuration commands **only at 9600 8N1**: `C0` persistent write, `C1` read, reply `C1 start len params`, format error `FF FF FF` |
| 6.2-6.3 (31-34) | registers 00H-07H: ADDH/ADDL, REG0 (UART baud [7:5], parity [4:3], air rate [2:0]), REG1 (packet length [7:6], ambient RSSI [5], power [1:0]), REG2 (channel), REG3 (RSSI byte [7], fixed transmission [6], LBT [4], WOR [2:0]); CRYPT 06H/07H are write-only |
| 6.4 (35) | factory settings `00 00 62 00 17 03 00 00` |

Known gaps in the manual (the driver is conservative, details: `sim/README.md`): 6.4 gives length `09` but lists 8 parameter bytes;
the REG0 air-rate values `000/001/010` all mean 2.4 kbps (the driver writes `010` and treats all three as equal); the manual gives no
reset time after leaving the configuration mode.
**REG1 bit 2** ("soft switch" in some manual revisions, reserved in older ones) is **unverified: treated as reserved - the driver never
writes it** and leaves it as read. **The driver has not been checked on a real module yet** - the tests use the simulator `sim/e220_sim.py`
built from the same manual.

## What `Start()` does

1. Opens the module UART at **9600 8N1** (required by the configuration mode), waits for AUX idle, sets **M0=1, M1=1** and waits for AUX idle again.
2. `C1 00 06` - reads registers 00H-05H.
3. Computes the target registers from the configuration and compares. `C0` is sent **only for the registers that differ** - one
   `C0 <reg> 01 <value>` per register (the module EEPROM has a limited number of writes). A second start with an unchanged configuration = **zero `C0` bytes**.
4. If anything was written: a `C1` read again and verification.
5. **M0=0, M1=0**, AUX idle, and - when the REG0 baud differs from 9600 - the UART is reopened at the configured speed.

Up to 3 attempts (100 ms apart); after the third failure `Start()` returns `kRetryable` with the cause (AUX, no response, `FF FF FF`, verification...).
An invalid configuration returns `kPersistent` without touching the hardware. A stop token / `Stop()` interrupts `Start()` while it waits for AUX or a reply.

**Forced by the driver** (independent of the configuration; the other reserved/WOR bits stay as the module has them):

- REG3: **transparent transmission** (bit6 = 0 - fixed-point would consume 3 bytes of every frame) and **no RSSI byte** (bit7 = 0 - it would append a byte to every received chunk);
- REG1: ambient RSSI off (bit5 = 0); bit 2 is not written (see above);
- CRYPT 06H/07H are not used (radio encryption is out of scope; MAVLink has its own signing).

## Event model: `Start()` / `Send()` / callbacks

After a successful `Start()` the driver has its own threads:

- **RX:** waits for UART data (`poll()`) and calls `OnReceive(chunk)` - any piece of the stream (also half a frame); a UART read error
  (device lost) -> `OnError(kPersistent, fatal)`.
- **`Send(frame)` is NON-BLOCKING:** a frame (<= 400 B) is accepted whole or not at all. An accepted frame goes to a 400 B TX queue.
  Queue full -> **`kBusy`** (nothing accepted); when the queue has room again the driver calls **`OnTxReady()`** (once per `kBusy`).
  A frame > 400 B -> `kInvalidArgument`.
- **TX is gated by AUX** (manual 5.2.3): the TX thread writes **one portion of at most `packet_length` bytes** only while AUX is idle,
  then waits for AUX **busy -> idle** (the module took the data and sent it) before the next portion. A frame longer than `packet_length`
  therefore leaves in several portions. Nothing is ever written while AUX is busy, so the module never overflows its buffer. There is no
  model of the module buffer or of the air time: the throughput is whatever the module reports through AUX (to be calibrated on hardware).
- An AUX that does not go idle within `aux_timeout` (2 s) before a write, or does not go busy and idle again after it, or a UART write
  error = the module is unusable -> `OnError(kPersistent, fatal)`; the daemon calls `Stop()` and `Start()` again after its restart delay.
- **`Stop()`:** stops both threads; **zero callbacks** after it returns (also when the daemon was waiting for `OnTxReady` after `kBusy`);
  it also interrupts a running `Start()`. `Start()` after `Stop()` works (the modem is configured again; unchanged configuration = zero `C0` writes).
- The `IRadioDriver` contract is checked by the platform suite (`radio_driver_contract_test`, `ut/test_e220_contract.cpp`).

**The plugin does not use `ara::log`, the srp `UartDriver` or `IGpioDriver`:** the `.so` has its own copies of the static libraries, so the
host logger singleton is uninitialised inside it, and those classes call it in their constructors (the plugin would crash); in addition the
srp `UartDriver` is replaced by a simulation in the sim build. The UART is therefore the platform `serial::SerialPort`
(`serial_port_uart`: termios + `poll()`, no logger) and the pins are `sysfs_e220_pins` (sysfs GPIO). Consequences of `SerialPort`: **8N1
only** (the `parity` field accepts only `"8N1"`) and the port is opened **exclusively** (`TIOCEXCL` + `flock`), so another user of the same
UART device makes `Start()` fail with a readable error.

## Configuration (the driver file, `driver_config` of the link)

The driver's own file (only the driver knows the format); the daemon configuration gives the path. The file is read when the link is created;
an error in it makes `Start()` return `kPersistent` with a readable description in the daemon log (file name, field, reason), e.g.
`file '/srp/opt/mavlink_daemon/ebyte_e220_lora.json': unknown field 'bogus'`; the daemon retries `Start()` after its restart delay (the file is
not read again - fix it and restart the daemon).

```json
{
  "uart": "/dev/ttyS1",
  "pins": { "m0": 17, "m1": 27, "aux": 22 },
  "aux_idle_level": 1,
  "e220": { "address": 4660, "channel": 18, "air_rate": 9600, "power_dbm": 27, "uart_baudrate": 57600 }
}
```

| Field | Meaning |
|---|---|
| `uart` | the module UART device (required) |
| `pins.m0`, `pins.m1`, `pins.aux` | sysfs GPIO numbers (`/sys/class/gpio`; required, different); `pins.ctl_dir` (pins in the simulator files) only in the test plugin |
| `aux_idle_level` | `1` or `0` (default `1`): the AUX pin level while the module is idle; the opposite level means busy. Set it to the level your board shows when the module is idle (an inverting buffer/level shifter swaps it) |
| `e220` | the radio parameters below (optional; a missing field = the factory value); an unknown key is an error |

Radio parameters (`e220`); ranges validated for the **E220-400T30S**:

| Field | Values | Register |
|---|---|---|
| `address` | 0-65535 | ADDH/ADDL |
| `channel` | 0-83 (frequency 410.125 + CH MHz) | REG2 |
| `air_rate` | 2400, 4800, 9600, 19200, 38400, 62500 (bps) | REG0 [2:0] |
| `power_dbm` | 30, 27, 24, 21 | REG1 [1:0] |
| `uart_baudrate` | 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 | REG0 [7:5] |
| `parity` | `"8N1"` only (see above) | REG0 [4:3] |
| `packet_length` | 200, 128, 64, 32 (B) | REG1 [7:6] |
| `lbt` | `true`/`false` (listen before talk) | REG3 bit4 |
| `wor_cycle` | 0-7 (optional; missing = WOR unchanged) | REG3 [2:0] |

Both ends of the link (EC/FC and the GS) need the same `channel` and `air_rate` and matching `address`.

## EC deployment

`deployment/platform/mavlink_daemon` (like `someip_demon`) puts into the EC package: the daemon configuration
(`/srp/opt/mavlink_daemon/mavlink_daemon_config.json`: `system_id`/`component_id`, `driver_dir`, the `lora` link -> `ebyte_e220`), the driver file
(`ebyte_e220_lora.json`) and the plugin `libebyte_e220.so` (`drivers/`). The plugin is built by the platform rule `mavlink_driver_plugin`
(target `//core/radio/e220:ebyte_e220`). The GPIO numbers (M0/M1/AUX), `aux_idle_level` and the UART (`/dev/ttyS1`) are board data - the values in
the repo are examples, **to be confirmed on the EC hardware**. Applications with `on: mavlink` are clients of the daemon (they create neither a driver nor a link).

AUX is polled every 1 ms (sysfs, no interrupts). `SysfsE220Pins::ReadAux` returns an error (and the driver waits until the timeout) when the value
is not 0/1 - a pin failure is not taken for an idle module. The timings (AUX timeout 2 s, mode switch 30 ms, command timeout 500 ms) are fields of `E220Timing`.

## Tool for the ground station (GS)

`tools/radio/e220_config.py` configures the GS modem through a USB-UART (standard library only), with the same JSON (`e220` object) and rules as the driver:

```
bazel run //tools/radio:e220_config -- --port /dev/ttyUSB0 --config gs_e220.json --check   # 0 = equal, 1 = different, 2 = error
bazel run //tools/radio:e220_config -- --port /dev/ttyUSB0 --config gs_e220.json --apply   # writes only the differing registers + verifies
```

**You set the configuration mode (M0=1, M1=1) by hand** with the module jumpers/switches before starting the tool and return to M0=0, M1=0 afterwards
(the USB-UART does not drive these pins). Configuration works only at 9600 8N1.

## Tests

| What | Where | Run |
|---|---|---|
| registers, JSON, the driver file, validation | `ut/test_e220_registers.cpp` | `bazel test //core/radio/e220:test_e220_registers` |
| `Start()` (idempotency, AUX, retry, baud), AUX-gated `Send`/`kBusy`/`OnTxReady` in both AUX polarities, fatal errors, `Stop`, the factory (fake module) | `ut/test_e220_driver.cpp` | `...:test_e220_driver` |
| the `IRadioDriver` contract from the platform (incl. Stop while waiting for `OnTxReady`, zero callbacks after Stop) | `ut/test_e220_contract.cpp` | `...:test_e220_contract` |
| UART adapter (pty) and sysfs pins (a temporary directory) | `ut/test_e220_io.cpp` | `...:test_e220_io` |
| the module simulator (C0/C1, `FF FF FF`, AUX in both polarities, 400 B overflow, RSSI byte) | `sim/e220_sim_test.py` | `...:e220_sim_test` |
| E2E: the platform MavlinkDaemon loads the `.so` plugin, the `lora` link on the simulator, 2 IPC clients: event + method <-> pymavlink, 2nd start = zero `C0` writes, a bad driver file = an error in the log | `ut/e220_plugin_e2e_test.py` | `...:e220_plugin_e2e_test` |
| the GS tool on the simulator | `tools/radio/e220_config_test.py` | `bazel test //tools/radio:e220_config_test` |

The E2E test and the GS tool use the simulator from this repo (`//core/radio/e220/sim:e220_sim_lib`, see `sim/README.md`); the driver itself needs a platform
with MavlinkDaemon; `FakeE220Module` (`testing/`) is a minimal stand-in for the driver unit tests. Throughput calibration (frames sent = frames received at
the packet length and air rate in use) is a hardware test on a real module.
