"""The EC MavlinkDaemon deployment config is accepted by the strict config parser of the daemon.

Runs the host variant of the daemon (the same core as on the EC, without Execution Management)
with the real deployment `mavlink_daemon_config.json`. Only the UART devices differ from the
EC: the driver files of both links (`serial_radio`) point to a pty; every other key is taken as
it is. Then a config with a removed key (`heartbeat_hz`, `version`, `driver_dir`, ...) must fail
at start and name the key.
"""
import json
import os
import subprocess
import tempfile
import unittest

DAEMON = os.environ["DAEMON_HOST"]
CONFIG = os.environ["DEPLOY_CONFIG"]


def _run(config, stdin=None, timeout=20):
    return subprocess.run([DAEMON, config], input=stdin, capture_output=True, text=True,
                          timeout=timeout, check=False)


class DeploymentConfigTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()  # pylint: disable=consider-using-with
        self.ptys = []
        with open(CONFIG, encoding="utf-8") as f:
            self.cfg = json.load(f)
        self.cfg["socket_path"] = os.path.join(self.tmp.name, "mavlink.sock")
        for name, link in self.cfg["links"].items():
            master, slave = os.openpty()
            self.ptys.append((master, slave))
            drv = os.path.join(self.tmp.name, name + ".json")
            with open(drv, "w", encoding="utf-8") as f:
                json.dump({"device": os.ttyname(slave), "baudrate": 9600}, f)
            link["driver"] = "serial_radio"
            link["driver_config"] = drv

    def tearDown(self):
        for master, slave in self.ptys:
            os.close(master)
            os.close(slave)
        self.tmp.cleanup()

    def _write(self, cfg):
        path = os.path.join(self.tmp.name, "cfg.json")
        with open(path, "w", encoding="utf-8") as f:
            json.dump(cfg, f)
        return path

    def test_deployment_config_starts(self):
        r = _run(self._write(self.cfg), stdin='{"id":1,"cmd":"stats"}\n{"cmd":"stop"}\n')
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        events = [json.loads(line) for line in r.stdout.splitlines() if line.strip()]
        self.assertEqual(events[0], {"ev": "ready"})
        stats = [e for e in events if e.get("id") == 1][0]
        self.assertTrue(stats["ok"])
        self.assertEqual(sorted(stats["result"]["links"]), ["radio_a", "radio_b"])

    def test_deployment_file_has_no_removed_keys(self):
        with open(CONFIG, encoding="utf-8") as f:
            cfg = json.load(f)
        for key in ("version", "driver_dir", "heartbeat_hz", "restart_delay_ms"):
            self.assertNotIn(key, cfg)
            for link in cfg["links"].values():
                self.assertNotIn(key, link)
        for link in cfg["links"].values():
            for key in ("air_rate_bps", "max_utilization", "mtu", "method_timeout_ms", "signing"):
                self.assertIn(key, link)

    def _expect_rejected(self, mutate, key):
        cfg = json.loads(json.dumps(self.cfg))
        mutate(cfg)
        r = _run(self._write(cfg))
        self.assertEqual(r.returncode, 2, r.stdout + r.stderr)
        self.assertIn('"ev":"error"', r.stdout)
        self.assertIn(key, r.stdout)

    def test_heartbeat_hz_is_a_start_error(self):
        self._expect_rejected(lambda c: c["links"]["radio_a"].update({"heartbeat_hz": 1}),
                              "heartbeat_hz")

    def test_version_is_a_start_error(self):
        self._expect_rejected(lambda c: c.update({"version": 2}), "version")

    def test_driver_dir_is_a_start_error(self):
        self._expect_rejected(lambda c: c.update({"driver_dir": "/x"}), "driver_dir")

    def test_restart_delay_ms_is_a_start_error(self):
        self._expect_rejected(lambda c: c.update({"restart_delay_ms": 10}), "restart_delay_ms")


if __name__ == "__main__":
    unittest.main()
