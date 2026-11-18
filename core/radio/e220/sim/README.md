# Ebyte E220-400T30S module simulator (`e220_sim.py`)

A simulator for the tests of the modem driver (`E220RadioDriver`) and the E2E tests. Written **independently of the driver**, only from the manufacturer
manual (below). It lives in `srp` (`core/radio/e220/sim`), tests only; the platform `srp_platform` does not know the modem.
Only what the driver relies on is simulated.

## Source document (the only one)

| | |
|---|---|
| Title | **E220-xxxTxxx Series User Manual** (page header: "E220-T Series User Manual"; the cover page reads "E220-xxxTxxx Product Specification Sheet - AT commands 20/30dBm LoRa wireless module") |
| Publisher | Chengdu Ebyte Electronic Technology Co., Ltd. |
| Revision | **1.8** (last entry of the revision history, p. 54), 54 pages |
| Source | `https://www.cdebyte.com/products/E220-400T30S/4` (Downloads, "Manual"); a copy is kept outside the repo (copyright) |

**Table numbers:** the manual does not number its tables (only "Figure N"), so we cite **section numbers and pages** (the footer "... 17").

## What the simulator reproduces (manual section -> behaviour -> test in `e220_sim_test.py`)

| Section (page) | Behaviour |
|---|---|
| 5.1 (17) | modes from the M1/M0 pins: 00 = transmission, 11 = configuration (the WOR modes 01/10 are not simulated: those pin states are ignored) |
| 5.2.1 (24) | after power-up AUX is busy for ~16 ms, then idle |
| 5.2.4 (27-28) | a mode switch: AUX busy for ~10 ms; while AUX is busy the pins are not evaluated; UART bytes during power-up/switch are ignored and counted (`bytes_while_busy`) |
| 5.2.2 (24) | reception from the radio: AUX busy 3 ms before the data goes out on TXD, then idle |
| 5.2.3 (25-26) | a frame ends after ~3 bytes of silence (by the UART baud); AUX is busy from the first UART byte until everything has been transmitted; the UART buffer is **400 B**, the excess is lost (`stats["tx_overflow"]`); packets of at most `packet_length` leave at the air rate |
| 6.1 (30-31) | configuration-mode commands (9600 8N1): `C0` persistent write, `C1` read; reply `C1 start len params`; a format error (any other command, a range outside 00H-07H) -> `FF FF FF` |
| 6.2/6.3 (31-34) | registers 00H-07H; the baud rate of the UART is read from the pty termios and bytes are ignored on a mismatch (`baud_mismatch_bytes`); REG3 bit7 appends an RSSI byte (`rssi_byte`, 0xC4) to the received data; CRYPT 06H/07H read as 0 |
| 6.4 (35) | the factory settings `00 00 62 00 17 03 00 00` |

**AUX polarity:** `E220Sim(..., aux_idle_level=1|0)` selects the level the `aux` file shows while the module is idle (the opposite level = busy);
`aux_log` records every change.

## Control contract (the `ctl_dir` directory)

`m0`, `m1` are written by the driver/test (`"0"`/`"1"`, preferably atomically with a rename); `aux` is written by the simulator;
`eeprom.bin` holds the 8 register bytes (delete it for the factory settings). UART: `sim.slave_path` (a pty). Statistics: `sim.stats`
(`eeprom_writes`, `format_errors`, `baud_mismatch_bytes`, `bytes_while_busy`, `writes_while_aux_busy`, `tx_packets`, `rx_packets`, `tx_overflow`, ...),
the command log `sim.cmd_log`. Driver idempotency: a second start with no changes => `stats["eeprom_writes"] == 0`; a change of one register => only that register is written.
`writes_while_aux_busy` counts a new UART frame that arrives while the previous data is still being handled - an AUX-gated driver must keep it at 0.

## Gaps and errors in the manual (the simulator does NOT guess - conservative behaviour)

1. **6.4 (p. 35):** "C0 00 09 ..." - length 09, but 8 parameter bytes (00H-07H) are listed. The simulator uses 8.
2. **6.3 REG0 [2:0]:** the values 000, 001, 010 are all described as "2.4k" (010 = default). The simulator maps them to 2400 bps.
3. No numbers for the reset time after leaving the configuration mode (5.2.4 note 3 says only "AUX busy").

## INTERPRETATIONS (the manual is silent; to be confirmed on a real module)

- A partial HEX command is dropped after 100 ms of silence.
- Reception: the same channel and air rate; addresses are not filtered.
- Air time = bytes * 8 / air rate (no LoRa SF/BW parameters); `Air(time_scale=...)` shortens it in tests.
- A channel above 83 (400 MHz band, 6.3 REG2): the module neither transmits nor receives.

## Out of scope

UART parity (REG0 [4:3] is only stored), the WOR protocol, fixed-point transmission, address filtering, the temporary-write command `C2`, the product
information register 08H, AT commands (chapter 7), software mode switching, LBT, ambient RSSI, firmware update (5.3).
