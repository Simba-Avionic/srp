# pylint: disable=line-too-long,missing-function-docstring,consider-using-f-string,broad-exception-caught,subprocess-run-check,unspecified-encoding,import-outside-toplevel,import-error,redefined-outer-name,unused-variable,too-many-locals
"""Functional scenarios (QEMU, sim build): correct behaviour of the EC apps, must be GREEN on the current code."""
import json
import os
import time

import pytest

import audit_lib as L

pytestmark = pytest.mark.functional

EXPECTED = L.EXPECTED
CONSOLE_LOG = os.path.join(os.environ.get("QEMU_TEST_WORK", "/tmp/qemu_test"), "logs", "qemu.log")


def test_fn01_boot_all_apps():
    procs = L.processes()
    missing = [e for e in EXPECTED if e not in procs]
    assert not missing, "FN-01 missing processes: %s" % missing


def _sample():
    procs = L.processes()
    return {k: sorted(procs.get(k, [])) for k in EXPECTED}


def _pair():
    """Two consecutive samples; a process counts as present when it is in either of them."""
    a = _sample()
    time.sleep(1.5)
    return a, _sample()


def test_fn02_no_crash_30s():
    a0, b0 = _pair()
    time.sleep(30)
    a1, b1 = _pair()
    problems = []
    for k in EXPECTED:
        before = set(a0[k]) | set(b0[k])
        after = set(a1[k]) | set(b1[k])
        if not before:
            problems.append("%s absent in both initial samples" % k)
        elif not after:
            problems.append("%s gone (absent in both final samples, was %s)" % (k, sorted(before)))
        elif not (before & after):
            problems.append("%s restarted (pids %s -> %s)" % (k, sorted(before), sorted(after)))
    assert not problems, "FN-02 process set changed in 30 s: %s\nraw samples:\n before %s\n       %s\n after  %s\n       %s" % (
        problems, a0, b0, a1, b1)
    if os.path.exists(CONSOLE_LOG):
        data = open(CONSOLE_LOG, errors="replace").read().splitlines()
        start = max([i for i, l in enumerate(data) if "Linux version" in l] or [0])
        bad = [l for l in data[start:] if "terminate called" in l or "Segmentation fault" in l or "FATAL" in l]
        assert not bad, "FN-02 crash markers: %s" % bad[:3]


def test_fn03_env_pressure_chamber():
    L.sim_ctl("i2c adc 2 1.0")
    L.logger_start()
    v = L.csv_value("CHAMBER_PRESS1", lambda x: abs(x / 100.0 - 17.68) < 0.3)
    assert v is not None and abs(v / 100.0 - 17.68) < 0.1, "FN-03 CHAMBER_PRESS1=%s (expected 17.68 bar x100)" % v


def test_fn04_env_pressure_pfs():
    L.sim_ctl("i2c adc 1 1.0")
    L.logger_start()
    v = L.csv_value("PRESSURE_FEED_PRESS", lambda x: abs(x / 100.0 - 67.67) < 0.5)
    assert v is not None and abs(v / 100.0 - 67.67) < 0.2, "FN-04 PRESSURE_FEED_PRESS=%s (expected 67.67 bar x100)" % v


def test_fn05_env_board_temperature():
    L.sim_ctl("temp 28-0b2554dfd2cc 25.5")
    L.logger_start()
    v = L.csv_value("BOARD_TEMP1", lambda x: abs(x / 10.0 - 25.5) < 0.5)
    assert v is not None and abs(v / 10.0 - 25.5) < 0.2, "FN-05 BOARD_TEMP1=%s (expected 25.5 C x10)" % v


def test_fn06_servo_move_via_someip():
    # ServoService.SetOxidizerDumpValve(1/0) -> aktuator 62 (kanal PCA 4, on_pos 131 deg, off_pos 0 deg).
    # 131 deg -> 1956 us -> 400 ticks, 0 deg -> 102 ticks (xo korelacja 1.0). Asercja na lustrze rejestrow PCA9685.
    rc, out = L.someip_call(*L.SERVO_SERVICE, L.M_SET_OXI_DUMP, bytes([1]))
    assert getattr(rc, "value", rc) == 0 and out[:1] == b"\x01", "FN-06 SetOxidizerDumpValve(1) -> rc=%s payload=%s" % (rc, out.hex())
    assert L.wait_for(lambda: abs((L.pca_off_ticks(4) or 0) - 400) <= 3, 10), \
        "FN-06 PCA channel 4 OFF ticks = %s, expected ~400 (open)" % L.pca_off_ticks(4)
    rc, out = L.someip_call(*L.SERVO_SERVICE, L.M_SET_OXI_DUMP, bytes([0]))
    assert getattr(rc, "value", rc) == 0, "FN-06 SetOxidizerDumpValve(0) rc=%s" % rc
    assert L.wait_for(lambda: L.pca_off_ticks(4) == 102, 10), \
        "FN-06 PCA channel 4 OFF ticks = %s, expected 102 (closed)" % L.pca_off_ticks(4)


SERVO_CONFIG = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "deployment", "apps",
                            "servo_service", "config.json")


def _pulsing_channels():
    """PCA channels of the actuators with pulsing_time_ms in the servo config: their PWM is switched off (0 ticks)
    between pulses, so 0 is a valid OFF value there (and only there)."""
    with open(SERVO_CONFIG) as f:
        return sorted({s["channel"] for s in json.load(f)["servos"] if s.get("pulsing_time_ms")})


def test_fn07_servo_positions_in_range():
    pulsing = _pulsing_channels()
    for ch in range(5):
        t = L.pca_off_ticks(ch)
        ok = t is not None and (102 <= t <= 512 or (ch in pulsing and t == 0))
        assert ok, ("FN-07 channel %d OFF ticks %s outside 102..512 (0 is allowed only on pulsing channels %s, "
                    "pulsing_time_ms in servo config.json) (PCA mirror: %s)" % (ch, t, pulsing, L.pca_raw()))


def test_fn08_logger_writes_file():
    L.logger_start()
    hdr, rows = L.csv_rows()
    assert hdr and hdr[0] == "TIMESTAMP" and "CHAMBER_PRESS1" in hdr and "GPIO_STATE" in hdr, "FN-08 header %s" % hdr
    n1 = len(rows)
    time.sleep(5)
    _, rows2 = L.csv_rows()
    assert len(rows2) > n1 + 3, "FN-08 CSV not growing (%d -> %d rows)" % (n1, len(rows2))


def test_fn09_sys_stat_publishes():
    L.logger_start()
    time.sleep(4)
    for col in ("CPU_USAGE", "MEM_USAGE", "DISK_UTILIZATION"):
        v = L.last_csv_value(col)
        assert v is not None and 0 <= v <= 100, "FN-09 %s=%s outside 0..100" % (col, v)


def test_fn10_gpio_did_set_and_read():
    r = L.uds("2E000C" + "0C01")
    assert r[0] == 0x6E, "FN-10 gpio DID write refused: %s" % r.hex()
    assert L.wait_for(lambda: L.gpio_state(12) == 1, 5), "FN-10 gpio id 12 not 1 after DID write"
    rd = L.uds("22000C")
    assert rd[0] == 0x62 and (rd[4] >> 4) & 1 == 1, "FN-10 DID read does not show pin 12: %s" % rd.hex()
    L.uds("2E000C" + "0C00")
    assert L.wait_for(lambda: L.gpio_state(12) == 0, 5), "FN-10 gpio id 12 not 0 after DID write 0"


def test_fn11_primer_not_connected():
    L.sim_ctl("i2c adc 3 0.0")
    L.logger_start()
    v = L.csv_value("PRIMER_STATUS", lambda x: x == 1, 30)
    assert v == 1, "FN-11 PRIMER_STATUS=%s, expected 1 (NOT_CONNECTED) at 0 V" % L.last_csv_value("PRIMER_STATUS")


def test_fn12_primer_connected():
    L.sim_ctl("i2c adc 3 3.0")
    L.logger_start()
    v = L.csv_value("PRIMER_STATUS", lambda x: x == 2, 40)
    assert v == 2, "FN-12 PRIMER_STATUS=%s, expected 2 (CONNECTED) at 3.0 V" % L.last_csv_value("PRIMER_STATUS")


def test_fn13_timestamp_monotonic():
    L.logger_start()
    time.sleep(6)
    _, rows = L.csv_rows()
    ts = [int(r["TIMESTAMP"]) for r in rows]
    assert len(ts) > 5 and ts == sorted(ts) and ts[-1] > ts[0], "FN-13 TIMESTAMP not monotonic/increasing"


def test_fn14_diag_rejects_invalid_requests():
    for req, what in (("2E000C01", "wrong length"), ("2E000CFF01", "pin id out of range"),
                      ("2E000C6301", "unknown pin id"), ("22005A", "unknown DID")):
        r = L.uds(req)
        assert r[0] == 0x7F, "FN-14 %s (%s) was not rejected: %s" % (req, what, r.hex())
