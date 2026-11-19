#!/usr/bin/env python3
"""Configures an Ebyte E220-400T30S modem of the ground station (GS) through a USB-UART.

Source: the Ebyte "E220-xxxTxxx Series User Manual", sections 6.1-6.4 (p. 30-35; see core/radio/e220/README.md).
The same configuration and the same rules as the driver in EC/FC (core/radio/e220), so that the GS and the
on-board modem have identical radio parameters.

NOTE: you set the configuration mode (M0=1, M1=1) BY HAND with the jumpers/switches of the module before starting
the tool, and return to M0=0, M1=0 afterwards. Configuration works only at 9600 8N1 (section 6.1). After
entering the configuration mode wait until AUX is idle (~10 ms).

Usage:
  e220_config.py --port /dev/ttyUSB0 --config gs_e220.json --check   # exit 0 = equal, 1 = different
  e220_config.py --port /dev/ttyUSB0 --config gs_e220.json --apply   # writes only the differing registers

The configuration file = the same JSON as the `e220` object of the driver file: address, channel, air_rate,
power_dbm, uart_baudrate, parity, packet_length, lbt, wor_cycle (missing fields = factory values).
"""
import argparse
import json
import os
import select
import sys
import termios
import time

REG_COUNT = 6  # 00H..05H; CRYPT 06H/07H (write-only) are not used

BAUDS = {1200: 0, 2400: 1, 4800: 2, 9600: 3, 19200: 4, 38400: 5, 57600: 6, 115200: 7}  # REG0 [7:5]
AIR_RATES = {2400: 2, 4800: 3, 9600: 4, 19200: 5, 38400: 6, 62500: 7}  # REG0 [2:0] (000/001/010 = 2.4k)
POWERS = {30: 0, 27: 1, 24: 2, 21: 3}  # REG1 [1:0], E220-400T30S
PACKETS = {200: 0, 128: 1, 64: 2, 32: 3}  # REG1 [7:6]
PARITY = {"8N1": 0, "8O1": 1, "8E1": 2}  # REG0 [4:3]
KEYS = {"address", "channel", "air_rate", "power_dbm", "uart_baudrate", "parity", "packet_length", "lbt",
        "wor_cycle"}
DEFAULTS = {"address": 0, "channel": 23, "air_rate": 2400, "power_dbm": 30, "uart_baudrate": 9600,
            "parity": "8N1", "packet_length": 200, "lbt": False}


class ConfigError(Exception):
    pass


def load_config(path):
    with open(path, encoding="utf-8") as f:
        raw = json.load(f)
    if isinstance(raw, dict) and isinstance(raw.get("e220"), dict):
        raw = raw["e220"]
    if not isinstance(raw, dict):
        raise ConfigError("the configuration must be a JSON object")
    unknown = set(raw) - KEYS
    if unknown:
        raise ConfigError(f"unknown fields: {sorted(unknown)}")
    cfg = dict(DEFAULTS)
    cfg.update(raw)

    def need(key, table):
        if isinstance(cfg[key], bool) or cfg[key] not in table:
            raise ConfigError(f"{key}={cfg[key]!r} not allowed (allowed: {sorted(table)})")

    if isinstance(cfg["address"], bool) or not isinstance(cfg["address"], int) or not 0 <= cfg["address"] <= 65535:
        raise ConfigError("address must be an integer 0..65535")
    if isinstance(cfg["channel"], bool) or not isinstance(cfg["channel"], int) or not 0 <= cfg["channel"] <= 83:
        raise ConfigError("channel must be an integer 0..83 (E220-400: 410.125 + CH MHz)")
    need("air_rate", AIR_RATES)
    need("power_dbm", POWERS)
    need("uart_baudrate", BAUDS)
    need("packet_length", PACKETS)
    if cfg["parity"] not in PARITY:
        raise ConfigError(f"parity={cfg['parity']!r} not allowed (8N1, 8O1, 8E1)")
    if not isinstance(cfg["lbt"], bool):
        raise ConfigError("lbt must be a boolean")
    if "wor_cycle" in cfg and (isinstance(cfg["wor_cycle"], bool) or not isinstance(cfg["wor_cycle"], int)
                               or not 0 <= cfg["wor_cycle"] <= 7):
        raise ConfigError("wor_cycle must be an integer 0..7")
    return cfg


def desired_registers(cfg, current):
    """Target registers; the fields outside the configuration stay as read (REG3: transparent, no RSSI byte)."""
    d = list(current)
    d[0], d[1] = cfg["address"] >> 8, cfg["address"] & 0xFF
    d[2] = (BAUDS[cfg["uart_baudrate"]] << 5) | (PARITY[cfg["parity"]] << 3) | AIR_RATES[cfg["air_rate"]]
    d[3] = (current[3] & 0x1C) | (PACKETS[cfg["packet_length"]] << 6) | POWERS[cfg["power_dbm"]]  # bit 2: see README
    d[4] = cfg["channel"]
    r3 = current[5] & ~(0x80 | 0x40 | 0x10)  # no RSSI byte, transparent transmission, LBT from the configuration
    if cfg["lbt"]:
        r3 |= 0x10
    if "wor_cycle" in cfg:
        r3 = (r3 & ~0x07) | cfg["wor_cycle"]
    d[5] = r3
    return bytes(d)


def matches(reg, actual, desired, cfg):
    if reg == 2:  # parity 11 == 00, air rate 000/001 == 010
        def norm(v):
            par = (v >> 3) & 3
            air = v & 7
            return (v & 0xE0) | ((0 if par == 3 else par) << 3) | max(air, 2)
        return norm(actual) == norm(desired)
    if reg == 3:
        return (actual & 0xE3) == (desired & 0xE3)
    if reg == 5:
        mask = 0x80 | 0x40 | 0x10 | (0x07 if "wor_cycle" in cfg else 0)
        return (actual & mask) == (desired & mask)
    return actual == desired


def differing(actual, desired, cfg):
    return [i for i in range(REG_COUNT) if not matches(i, actual[i], desired[i], cfg)]


def open_port(path):
    fd = os.open(path, os.O_RDWR | os.O_NOCTTY)
    a = termios.tcgetattr(fd)
    a[0] = a[1] = a[3] = 0  # raw
    a[2] = termios.CS8 | termios.CREAD | termios.CLOCAL
    a[4] = a[5] = termios.B9600
    a[6][termios.VMIN] = 0
    a[6][termios.VTIME] = 0
    termios.tcsetattr(fd, termios.TCSANOW, a)
    return fd


def read_exact(fd, n, timeout):
    got, end = b"", time.monotonic() + timeout
    while len(got) < n and time.monotonic() < end:
        if select.select([fd], [], [], 0.02)[0]:
            got += os.read(fd, 256)
        if got[:3] == b"\xFF\xFF\xFF":
            raise ConfigError("the module replied FF FF FF (command format error)")
    return got[:n] if len(got) >= n else got


def exchange(fd, cmd, expected, timeout=1.0):
    while select.select([fd], [], [], 0)[0]:  # drop stale bytes
        os.read(fd, 256)
    os.write(fd, bytes(cmd))
    resp = read_exact(fd, expected, timeout)
    if len(resp) < expected:
        raise ConfigError(f"no response from the module (received {resp.hex(' ')}); is the module in "
                          "the configuration mode (M0=1, M1=1), 9600 8N1?")
    return resp


def read_registers(fd):
    resp = exchange(fd, [0xC1, 0x00, REG_COUNT], 3 + REG_COUNT)
    if resp[:3] != bytes([0xC1, 0x00, REG_COUNT]):
        raise ConfigError(f"unexpected reply: {resp.hex(' ')}")
    return resp[3:]


def describe(regs):
    return (f"addr=0x{regs[0]:02X}{regs[1]:02X} reg0=0x{regs[2]:02X} reg1=0x{regs[3]:02X} "
            f"ch={regs[4]} reg3=0x{regs[5]:02X}")


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--port", required=True, help="the USB-UART port, e.g. /dev/ttyUSB0")
    ap.add_argument("--config", required=True, help="JSON with the radio configuration")
    mode = ap.add_mutually_exclusive_group(required=True)
    mode.add_argument("--check", action="store_true", help="compare with the module (exit code 1 = different)")
    mode.add_argument("--apply", action="store_true", help="write only the differing registers and verify")
    args = ap.parse_args(argv)
    try:
        cfg = load_config(args.config)
        fd = open_port(args.port)
        try:
            current = read_registers(fd)
            desired = desired_registers(cfg, current)
            diff = differing(current, desired, cfg)
            print(f"module:  {describe(current)}")
            print(f"target:  {describe(desired)}")
            if not diff:
                print("equal: nothing to change")
                return 0
            print(f"differing registers: {[f'{i:02X}H' for i in diff]}")
            if args.check:
                return 1
            for reg in diff:
                resp = exchange(fd, [0xC0, reg, 0x01, desired[reg]], 4)
                if resp != bytes([0xC1, reg, 0x01, desired[reg]]):
                    raise ConfigError(f"write acknowledgement of {reg:02X}H does not match: {resp.hex(' ')}")
            after = read_registers(fd)
            still = differing(after, desired, cfg)
            if still:
                raise ConfigError(f"verification after the write failed (registers {still}): {describe(after)}")
            print(f"wrote {len(diff)} register(s), verification OK: {describe(after)}")
            return 0
        finally:
            os.close(fd)
    except (ConfigError, OSError, json.JSONDecodeError) as e:
        print(f"ERROR: {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
