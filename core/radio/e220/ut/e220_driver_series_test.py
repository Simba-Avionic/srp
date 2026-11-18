"""E2E drivera E220 z atrapa modemu (e220_sim.py z core/radio/e220/sim, wg instrukcji Ebyte rev. 1.8).

Narzedzie `e220_driver_probe` (prawdziwy E220RadioDriver + UART termios na pty atrapy, piny w plikach atrapy):
  * 1. start zapisuje tylko rozniace sie rejestry (C0), 2. start: 0 zapisow, zmiana jednego parametru = zapis jednego rejestru,
  * serie ramek jedna po drugiej (Send/kBusy/OnTxReady): komplet dostarczony, bufor 400 B atrapy nie przepelnia sie
    (`tx_overflow` == 0, `bytes_while_busy` == 0), kontrola negatywna bez AUX/pacingu przepelnia bufor.
Wspolpraca z daemonem i pluginem .so: e220_plugin_e2e_test.py.
"""
import json
import os
import subprocess
import tempfile
import time
import unittest

from e220_sim import Air, E220Sim

PROBE = os.path.abspath(os.environ["E220_PROBE"])


def run_probe(sim, mode, config, seconds=1.0, **extra):
    cfg = tempfile.NamedTemporaryFile("w", suffix=".json", delete=False)
    json.dump(config, cfg)
    cfg.close()
    args = [PROBE, "--mode", mode, "--uart", sim.slave_path, "--ctl-dir", sim.ctl, "--config", cfg.name,
            "--seconds", str(seconds)]
    for k, v in extra.items():
        args += [f"--{k}", str(v)]
    p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    p.cfg_path = cfg.name
    return p


def finish(p, timeout=60):
    out, err = p.communicate(timeout=timeout)
    os.unlink(p.cfg_path)
    assert p.returncode == 0, f"probe rc={p.returncode}\nstdout={out}\nstderr={err}"
    return json.loads(out.strip().splitlines()[-1])


class E220E2ETest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()

    def tearDown(self):
        self.tmp.cleanup()

    def sim(self, name, air=None):
        d = os.path.join(self.tmp.name, name)
        os.makedirs(d)
        return E220Sim(d, air=air)

    CONFIG = {"address": 0x1234, "channel": 18, "air_rate": 9600, "power_dbm": 27, "uart_baudrate": 57600}

    def test_first_start_writes_differences_second_start_writes_nothing(self):
        with self.sim("a") as s:
            finish(run_probe(s, "start", self.CONFIG))
            writes_first = s.stats["eeprom_writes"]
            self.assertGreater(writes_first, 0)
            self.assertLessEqual(writes_first, 5)  # tylko ADDH ADDL REG0 REG1 REG2 (REG3 bez zmian)
            regs = s.eeprom_regs()
            self.assertEqual(regs[0] << 8 | regs[1], 0x1234)
            self.assertEqual(regs[4], 18)
            self.assertEqual(regs[5] & 0xC0, 0)  # bez bajtu RSSI, transmisja transparentna
            r2 = finish(run_probe(s, "start", self.CONFIG))
            self.assertEqual(s.stats["eeprom_writes"], writes_first)  # 0 nowych zapisow
            self.assertEqual(r2["registers"][4], 18)

    def test_one_changed_parameter_writes_one_register(self):
        with self.sim("a") as s:
            finish(run_probe(s, "start", self.CONFIG))
            before = s.stats["eeprom_writes"]
            finish(run_probe(s, "start", dict(self.CONFIG, channel=19)))
            self.assertEqual(s.stats["eeprom_writes"], before + 1)

    def run_mode(self, sim, mode, config, seconds, **extra):
        return run_probe(sim, mode, config, seconds, **extra)

    def run_series(self, ignore_aux):
        """20 x 200 B jedna po drugiej, UART 57600, air 2.4 kbps; time_scale 0.3 => czas antenowy
        ramki (~200 ms) wyraznie dluzszy niz UART (~35 ms), wiec bez kontroli AUX bufor 400 B przepelnia."""
        cfg = dict(self.CONFIG, air_rate=2400, uart_baudrate=57600)
        count, size = 20, 200
        air = Air(time_scale=0.3)
        with self.sim("a", air) as sa, self.sim("b", air) as sb:
            sink = self.run_mode(sb, "sink", cfg, seconds=40)
            time.sleep(2.0)  # sink po konfiguracji zaczyna czytac
            extra = {"ignore-aux-write": 1} if ignore_aux else {}
            burst = self.run_mode(sa, "burst", cfg, seconds=1, count=count, size=size, **extra)
            b_out, b_err = burst.communicate(timeout=120)
            s_out, s_err = sink.communicate(timeout=120)
            os.unlink(burst.cfg_path)
            os.unlink(sink.cfg_path)
            self.assertEqual(burst.returncode, 0, b_err)
            self.assertEqual(sink.returncode, 0, s_err)
            sent = json.loads(b_out.strip().splitlines()[-1])
            got = json.loads(s_out.strip().splitlines()[-1])
            return sa.stats, sent, got, count * size

    def test_series_of_frames_is_delivered_completely_without_buffer_overflow(self):
        # QA S1 #4: kolejne Write() jedna po drugiej - driver czeka na AUX, wiec bufor 400 B nie przepelnia sie.
        stats, sent, got, total = self.run_series(ignore_aux=False)
        expected = bytearray()
        for i in range(20):
            expected += bytes((i * 7 + k) & 0xFF for k in range(200))
        fnv = 1469598103934665603
        for b in expected:
            fnv = ((fnv ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        self.assertEqual(sent["failed_writes"], 0)
        self.assertEqual(sent["sent"], total)
        self.assertEqual(stats["tx_overflow"], 0)
        self.assertEqual(stats["bytes_while_busy"], 0)
        self.assertEqual(got["received"], total)
        self.assertEqual(got["fnv"], f"{fnv:016x}")  # ten sam strumien bajtow, bez utraty i zmian

    def test_random_length_frames_series_loses_no_bytes(self):
        # tester S1: 200 ramek o dlugosci 12..279 B (jak MAVLink v2 z podpisem) jedna po drugiej,
        # UART 57600, air 9600, czas antenowy rzeczywisty (time_scale 1.0) => tx_overflow == 0,
        # wszystkie bajty dostarczone (ta sama suma FNV), Write nie zglasza sukcesu dla utraconych.
        cfg = dict(self.CONFIG, air_rate=9600, uart_baudrate=57600)
        air = Air(time_scale=1.0)
        with self.sim("a", air) as sa, self.sim("b", air) as sb:
            sink = self.run_mode(sb, "sink", cfg, seconds=75)
            time.sleep(2.0)
            burst = self.run_mode(sa, "burst", cfg, seconds=1, count=200, **{"min-size": 12, "max-size": 279})
            b_out, b_err = burst.communicate(timeout=180)
            s_out, s_err = sink.communicate(timeout=180)
            os.unlink(burst.cfg_path)
            os.unlink(sink.cfg_path)
            self.assertEqual(burst.returncode, 0, b_err)
            self.assertEqual(sink.returncode, 0, s_err)
            sent = json.loads(b_out.strip().splitlines()[-1])
            got = json.loads(s_out.strip().splitlines()[-1])
            self.assertEqual(sent["failed_writes"], 0)
            self.assertEqual(sa.stats["tx_overflow"], 0, (sa.stats, sent, got))
            self.assertEqual(sa.stats["bytes_while_busy"], 0)
            self.assertEqual(got["received"], sent["sent"], (sent, got))
            self.assertEqual(got["fnv"], sent["fnv"])

    def run_sizes(self, sizes, time_scale, tag=""):
        cfg = dict(self.CONFIG, air_rate=9600, uart_baudrate=57600)
        air = Air(time_scale=time_scale)
        with self.sim("a" + tag, air) as sa, self.sim("b" + tag, air) as sb:
            sink = self.run_mode(sb, "sink", cfg, seconds=90)
            time.sleep(2.0)
            burst = self.run_mode(sa, "burst", cfg, seconds=1, sizes=sizes)
            b_out, b_err = burst.communicate(timeout=240)
            s_out, s_err = sink.communicate(timeout=240)
            os.unlink(burst.cfg_path)
            os.unlink(sink.cfg_path)
            self.assertEqual(burst.returncode, 0, b_err)
            self.assertEqual(sink.returncode, 0, s_err)
            sent = json.loads(b_out.strip().splitlines()[-1])
            got = json.loads(s_out.strip().splitlines()[-1])
            return sa.stats, sent, got

    def test_tester_overflow_series_seed7_lose_no_bytes(self):
        # tester S1 retest: serie 1 i 5 (200 ramek 12..279 B) gubily 8 i 18 B przy Write == sukces, bo
        # model obloznia drenowal bufor ciagle, a atrapa/modul zwalnia miejsce porcjami po pakiecie.
        here = os.path.dirname(os.path.abspath(__file__))
        for name in ("overflow_series_1.txt", "overflow_series_5.txt"):
            with open(os.path.join(here, name), encoding="utf-8") as f:
                sizes = f.read().strip()
            stats, sent, got = self.run_sizes(sizes, time_scale=1.0, tag=name[:-4])
            self.assertEqual(sent["failed_writes"], 0, name)
            self.assertEqual(stats["tx_overflow"], 0, (name, stats, sent, got))
            self.assertEqual(stats["bytes_while_busy"], 0, name)
            self.assertEqual(got["received"], sent["sent"], (name, sent, got))
            self.assertEqual(got["fnv"], sent["fnv"], name)

    def test_series_negative_control_overflows_when_aux_is_ignored(self):
        # kontrola negatywna: ten sam scenariusz bez czekania na AUX MUSI przepelnic bufor modulu -
        # inaczej test serii nic by nie wykrywal.
        stats, sent, got, total = self.run_series(ignore_aux=True)
        self.assertGreater(stats["tx_overflow"] + stats["bytes_while_busy"], 0, (stats, sent, got))
        self.assertLess(got["received"], total)

if __name__ == "__main__":
    unittest.main()
