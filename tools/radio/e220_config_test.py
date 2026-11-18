"""Testy tools/radio/e220_config.py na atrapie modemu (e220_sim.py, instrukcja Ebyte rev. 1.8)."""
import json
import os
import tempfile
import time
import unittest

import e220_config as tool
from e220_sim import E220Sim

DEFAULT = bytes([0x00, 0x00, 0x62, 0x00, 0x17, 0x03])


def to_config_mode(sim):
    """Tryb konfiguracji ustawiany "zworkami" (tak jak robi to uzytkownik GS): M0=M1=1."""
    for name in ("m0", "m1"):
        tmp = os.path.join(sim.ctl, f".{name}.w")
        with open(tmp, "w") as f:
            f.write("1")
        os.replace(tmp, os.path.join(sim.ctl, name))
    end = time.monotonic() + 10
    while time.monotonic() < end:
        aux = open(os.path.join(sim.ctl, "aux")).read().strip()
        if sim.mode == 3 and aux == "1":
            return
        time.sleep(0.005)
    raise AssertionError("atrapa nie weszla w tryb konfiguracji")


class ToolTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        os.makedirs(os.path.join(self.tmp.name, "ctl"))
        self.sim = E220Sim(os.path.join(self.tmp.name, "ctl"))
        self.sim.start()
        to_config_mode(self.sim)

    def tearDown(self):
        self.sim.stop()
        self.tmp.cleanup()

    def cfg(self, **kw):
        p = os.path.join(self.tmp.name, "cfg.json")
        with open(p, "w") as f:
            json.dump(kw, f)
        return p

    def run_tool(self, mode, **kw):
        return tool.main(["--port", self.sim.slave_path, "--config", self.cfg(**kw), mode])

    def test_check_on_factory_module_with_default_config_is_ok(self):
        self.assertEqual(self.run_tool("--check"), 0)
        self.assertEqual(self.sim.stats["eeprom_writes"], 0)

    def test_check_reports_difference_without_writing(self):
        self.assertEqual(self.run_tool("--check", channel=30), 1)
        self.assertEqual(self.sim.stats["eeprom_writes"], 0)

    def test_apply_writes_only_differing_registers_then_is_idempotent(self):
        kw = dict(address=0x1234, channel=30, air_rate=9600, power_dbm=27, uart_baudrate=57600)
        self.assertEqual(self.run_tool("--apply", **kw), 0)
        first = self.sim.stats["eeprom_writes"]
        regs = self.sim.eeprom_regs()
        self.assertEqual(regs[0:2], bytes([0x12, 0x34]))
        self.assertEqual(regs[4], 30)
        self.assertEqual(regs[2] >> 5, 6)  # 57600
        self.assertEqual(self.run_tool("--check", **kw), 0)
        self.assertEqual(self.run_tool("--apply", **kw), 0)
        self.assertEqual(self.sim.stats["eeprom_writes"], first)  # drugi apply: 0 zapisow

    def test_one_parameter_change_writes_one_register(self):
        self.assertEqual(self.run_tool("--apply", channel=10), 0)
        self.assertEqual(self.sim.stats["eeprom_writes"], 1)

    def test_reg3_forced_transparent_without_rssi_byte(self):
        self.assertEqual(self.run_tool("--apply"), 0)
        self.assertEqual(self.sim.eeprom_regs()[5] & 0xC0, 0)

    def test_invalid_config_is_rejected_before_touching_the_module(self):
        for bad in (dict(channel=84), dict(power_dbm=22), dict(air_rate=1200), dict(uart_baudrate=230400),
                    dict(parity="8N2"), dict(chanel=5), dict(lbt=1), dict(address=70000)):
            self.assertEqual(self.run_tool("--check", **bad), 2, bad)
        self.assertEqual(self.sim.stats["eeprom_writes"], 0)

    def test_module_not_in_config_mode_gives_clear_error(self):
        for name in ("m0", "m1"):
            tmp = os.path.join(self.sim.ctl, f".{name}.w")
            with open(tmp, "w") as f:
                f.write("0")
            os.replace(tmp, os.path.join(self.sim.ctl, name))
        time.sleep(0.2)
        self.assertEqual(self.run_tool("--check"), 2)


class PureLogicTest(unittest.TestCase):
    def test_desired_matches_c_driver_tables(self):
        cfg = dict(tool.DEFAULTS, channel=18, uart_baudrate=57600, power_dbm=27)
        d = tool.desired_registers(cfg, DEFAULT)
        self.assertEqual(d, bytes([0x00, 0x00, 0xC2, 0x01, 0x12, 0x03]))

    def test_equivalent_codes_are_not_differences(self):
        cfg = dict(tool.DEFAULTS)
        self.assertEqual(tool.differing(bytes([0, 0, 0x60, 0, 0x17, 3]), DEFAULT, cfg), [])  # air 000 == 010
        self.assertEqual(tool.differing(bytes([0, 0, 0x7A, 0, 0x17, 3]), bytes([0, 0, 0x62, 0, 0x17, 3]), cfg), [])


if __name__ == "__main__":
    unittest.main()
