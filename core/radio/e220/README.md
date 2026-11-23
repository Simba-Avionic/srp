# Ebyte E220-400T30S driver (`core/radio/e220`)

Driver for the **E220-400T30S** LoRa modem as the **`ebyte_e220` driver** of MavlinkDaemon (`ara::com::mavlink::IRadioDriver`
from `srp_platform`). The platform knows nothing about the modem - all knowledge about the registers, the M0/M1/AUX pins and
the TX gating is here.

**Structure:** the driver = `RadioDriver` ([`core/radio/common`](../common): threads, the one-frame TX slot, `kBusy`/`OnTxReady`,
fatal errors, `Stop()` without callbacks - written once for every radio) + **`E220Modem`** (`IRadioModem`: everything about the
module, plain blocking code). The hardware goes through the srp interfaces: the UART through `uart::IUartDriver` (`UartDriver`),
M0/M1/AUX through `gpio::IGpioDriver` (`GpioDriver`, sysfs); in the `sim` build these are the simulation drivers. Logs go through
`ara::log`. The driver is **linked statically** into the daemon binary and registers itself with
`SRP_MAVLINK_REGISTER_DRIVER("ebyte_e220", ...)` (`e220_driver.cc`, library `//core/radio/e220:e220_driver`, `alwayslink`); there is
no plugin `.so`:

```python
mavlink_daemon_binary(name = "mavlink_daemon", drivers = ["//core/radio/e220:e220_driver"])
```

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
| 6.3, REG1 bit 5 (33) | "RSSI Ambient Noise Enable": the `C0 C1 C2 C3 <start> <length>` read of the ambient noise and of the last packet RSSI (the optional statistics, below) |
| 6.4 (35) | factory settings `00 00 62 00 17 03 00 00` |

Known gaps in the manual (the driver is conservative, details: `sim/README.md`): 6.4 gives length `09` but lists 8 parameter bytes;
the REG0 air-rate values `000/001/010` all mean 2.4 kbps (the driver writes `010` and treats all three as equal); the manual gives no
reset time after leaving the configuration mode.
**REG1 bit 2** ("soft switch" in some manual revisions, reserved in older ones) is **unverified: treated as reserved - the driver never
writes it** and leaves it as read. **The driver has not been checked on a real module yet** - the tests use the simulator `sim/e220_sim.py`
built from the same manual.

## What `Start()` does (`E220Modem::Configure()`)

1. Sets up the GPIOs (M0, M1 outputs; AUX input), opens the module UART at **9600 8N1** (required by the configuration mode), waits for AUX idle, sets **M0=1, M1=1** and waits for AUX idle again.
2. `C1 00 06` - reads registers 00H-05H.
3. Computes the target registers from the configuration and compares. `C0` is sent **only for the registers that differ** - one
   `C0 <reg> 01 <value>` per register (the module EEPROM has a limited number of writes). A second start with an unchanged configuration = **zero `C0` bytes**.
4. If anything was written: a `C1` read again and verification.
5. **M0=0, M1=0**, AUX idle, and - when the REG0 baud differs from 9600 - the UART is reopened at the configured speed.

Up to 3 attempts (100 ms apart); after the third failure `Start()` returns `kRetryable` with the cause (AUX, no response, `FF FF FF`, verification...).
An invalid configuration returns `kPersistent` without touching the hardware. A stop token / `Stop()` interrupts `Start()` while it waits for AUX or a reply.

**Forced by the driver** (independent of the configuration; the other reserved/WOR bits stay as the module has them):

- REG3: **transparent transmission** (bit6 = 0 - fixed-point would consume 3 bytes of every frame) and **no RSSI byte** (bit7 = 0 - it would append a byte to every received chunk);
- REG1: ambient RSSI off (bit5 = 0) unless `stats_period_ms` > 0 (see "Statistics"); bit 2 is not written (see above);
- CRYPT 06H/07H are not used (radio encryption is out of scope; MAVLink has its own signing).

## Event model: `Send()` / callbacks (`RadioDriver`)

- **`MaxFrameSize()` = `packet_length`** (200/128/64/32 B): `E220Modem::Caps()` = `{mtu = packet_length, stream, half duplex}`. A frame larger than the
  packet length goes over the air in several LoRa packets, and losing one loses the whole frame, so the platform refuses it (`kInvalidArgument`);
  the generator checks every element against this MTU.
- **`Send(frame)` is NON-BLOCKING:** the TX queue holds **one** frame. A second frame -> **`kBusy`**; when the slot is free again the driver calls
  **`OnTxReady()`** (once per `kBusy`).
- **Half duplex:** one driver thread alternates `Receive()` (10 ms slices; `OnReceive(chunk)` with any piece of the stream) and `Transmit()`.
- **`Transmit()` is gated by AUX** (manual 5.2.3): one portion of at most `packet_length` bytes is written only while AUX is idle, then the modem waits
  for AUX **busy -> idle** (the module took the data and sent it) before the next portion. Nothing is ever written while AUX is busy, so the module never
  overflows its buffer. There is no model of the module buffer or of the air time: the throughput is whatever the module reports through AUX (to be
  calibrated on hardware).
- An AUX that does not go idle within `aux_timeout` (2 s) before a write, or does not go busy and idle again after it, a UART write error or a UART read
  error (device lost) = the module is unusable -> `OnError(kPersistent, fatal)`; the daemon calls `Stop()` and `Start()` again after its restart backoff.
- **`Stop()`:** stops the thread; **zero callbacks** after it returns; it also interrupts a running `Start()`. `Start()` after `Stop()` works (the modem is
  configured again; unchanged configuration = zero `C0` writes).
- The `IRadioDriver` contract is checked by the platform suite (`radio_driver_contract_test`, `ut/test_e220_contract.cpp`).

## Statistics: RSSI and noise (OPTIONAL, off by default, UNVERIFIED on a module)

`"stats_period_ms": 1000` in the driver file (0 or absent = off, the default; 0..3600000). When it is > 0:

1. `Configure()` sets **REG1 bit 5** ("RSSI Ambient Noise Enable", manual 6.3, p. 33) - a normal `C0` register write, like the other registers.
2. About every `stats_period_ms`, while the module is idle (AUX idle), the modem writes `C0 C1 C2 C3 00 02` to the UART **in the transparent mode**
   (manual p. 33: with the bit enabled this read works in the transmit mode). The module answers `C1 00 02 <noise> <last packet RSSI>`,
   `dBm = -(256 - value)`.
3. The reply comes in the same UART stream as the received data: the modem cuts the 5 bytes `C1 00 02 xx yy` out of what it reads (a header split
   across two reads is held back, other bytes pass untouched; a reply that does not come within `command_timeout` (500 ms) is logged once as a warning
   and the held bytes are released as data).
4. The values are reported through `OnLinkStats` (`rssi_dbm` = last packet, `noise_floor_dbm`; the platform shows them in `MavlinkLinkService`).

**Unverified on a real module - check before relying on it:**

- whether the module really answers `C0 C1 C2 C3 00 02` in the transparent mode with 2 values (the manual example shows length 1; two values are an
  interpretation of "register 0x00 / 0x01"), and whether the bytes of the reply can **interleave with received data** in the stream (then the 3-byte
  header `C1 00 02` could also match data bytes - the window is limited to the time after a command);
- what "RSSI of the last received data" reads before any packet arrived;
- **outgoing data containing `C0 C1 C2 C3` would be taken by the module for this command** while bit 5 is set (a MAVLink payload may contain any bytes).
  That is why the feature is off by default.

The simulator (`sim/e220_sim.py`) implements the command as described (see `sim/README.md`).

## Configuration (the driver file, `driver_config` of the link)

The driver's own file (only the driver knows the format); the daemon configuration gives the path. The file is read when the link is created;
an error in it makes `Start()` return `kPersistent` with a readable description in the daemon log (file name, field, reason), e.g.
`file '/srp/opt/mavlink_daemon/ebyte_e220_lora.json': unknown field 'bogus'`; the daemon retries `Start()` after its restart backoff (the file is
not read again - fix it and restart the daemon).

```json
{
  "uart": "/dev/ttyS1",
  "pins": { "m0": 17, "m1": 27, "aux": 22 },
  "aux_idle_level": 1,
  "stats_period_ms": 0,
  "e220": { "address": 4660, "channel": 18, "air_rate": 9600, "power_dbm": 27, "uart_baudrate": 57600 }
}
```

| Field | Meaning |
|---|---|
| `uart` | the module UART device (required) |
| `pins.m0`, `pins.m1`, `pins.aux` | sysfs GPIO numbers (`/sys/class/gpio`; required, different); `pins.ctl_dir` (pins in the simulator files) only in the test driver |
| `aux_idle_level` | `1` or `0` (default `1`): the AUX pin level while the module is idle; the opposite level means busy. Set it to the level your board shows when the module is idle (an inverting buffer/level shifter swaps it) |
| `stats_period_ms` | the RSSI/noise readout period (see "Statistics"); `0` = off (default) |
| `e220` | the radio parameters below (optional; a missing field = the factory value); an unknown key is an error |

Radio parameters (`e220`); ranges validated for the **E220-400T30S**:

| Field | Values | Register |
|---|---|---|
| `address` | 0-65535 | ADDH/ADDL |
| `channel` | 0-83 (frequency 410.125 + CH MHz) | REG2 |
| `air_rate` | 2400, 4800, 9600, 19200, 38400, 62500 (bps) | REG0 [2:0] |
| `power_dbm` | 30, 27, 24, 21 | REG1 [1:0] |
| `uart_baudrate` | 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 | REG0 [7:5] |
| `parity` | `"8N1"` only | REG0 [4:3] |
| `packet_length` | 200, 128, 64, 32 (B) | REG1 [7:6] |
| `lbt` | `true`/`false` (listen before talk) | REG3 bit4 |
| `wor_cycle` | 0-7 (optional; missing = WOR unchanged) | REG3 [2:0] |

Both ends of the link (EC/FC and the GS) need the same `channel` and `air_rate` and matching `address`.

The GPIO numbers (M0/M1/AUX), `aux_idle_level` and the UART (`/dev/ttyS1`) are board data - the values above are examples, **to be confirmed on the EC
hardware**. AUX is polled every 1 ms (sysfs, no interrupts); `E220Modem::ReadAux` treats a value other than 0/1 (the `GpioDriver` error code) as a pin
error, so a failure is not taken for an idle module. The timings (AUX timeout 2 s, mode switch 30 ms, command timeout 500 ms) are fields of `E220Timing`.
If the pins were exported by an earlier run, `initializePin()` fails and the modem only sets the direction.

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
| `E220Modem`: `Configure()` (idempotency, AUX, retry, baud), AUX-gated `Transmit()`/`Receive()`, both AUX polarities, errors, stop token (fake module) | `ut/test_e220_modem.cpp` | `...:test_e220_modem` |
| the driver = `RadioDriver` + `E220Modem`: `Start`/`Send`/`kBusy`/`OnTxReady`, fatal errors, `Stop`, the factory | `ut/test_e220_driver.cpp` | `...:test_e220_driver` |
| the `IRadioDriver` contract from the platform (incl. Stop while waiting for `OnTxReady`, zero callbacks after Stop) | `ut/test_e220_contract.cpp` | `...:test_e220_contract` |
| the statistics: the driver file, REG1 bit 5, the read command and its reply cut out of the stream, `OnLinkStats`, off by default | `ut/test_e220_stats.cpp` | `...:test_e220_stats` |
| the module simulator (C0/C1, `FF FF FF`, AUX in both polarities, 400 B overflow, RSSI byte, the ambient RSSI read) | `sim/e220_sim_test.py` | `...:e220_sim_test` |
| E2E: the platform MavlinkDaemon with the E220 driver linked in (`mavlink_daemon_binary`), the `lora` link on the simulator, 2 IPC clients: event + method <-> pymavlink, 2nd start = zero `C0` writes, a bad driver file = an error in the log, `stats_period_ms` = LinkStats with rssi/noise | `ut/e220_driver_e2e_test.py` | `...:e220_driver_e2e_test` |
| the GS tool on the simulator | `tools/radio/e220_config_test.py` | `bazel test //tools/radio:e220_config_test` |

The E2E test and the GS tool use the simulator from this repo (`//core/radio/e220/sim:e220_sim_lib`, see `sim/README.md`); the E2E test needs a platform
with MavlinkDaemon and builds the daemon with `//core/radio/e220:e220_test_driver` (like `e220_driver`, but `pins.ctl_dir` is allowed: the pins are
files of the simulator). `FakeE220Module` (`testing/`) is a minimal stand-in for the unit tests (UART + GPIO in one object). Throughput calibration
(frames sent = frames received at the packet length and air rate in use) is a hardware test on a real module.
