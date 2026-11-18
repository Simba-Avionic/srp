"""E2E: MavlinkDaemon z platformy (host bez EM) ladujacy plugin `ebyte_e220` (.so) z katalogu driverow, lacze `lora` na
atrapie modemu E220 (e220_sim.py), po drugiej stronie drugi modul (atrapa) i pymavlink („stacja"); po stronie aplikacji
dwa niezalezne klienty IPC (klient Pythona z platformy).

Scenariusze (AC2 briefu D3):
  * 2 aplikacje: event w obu kierunkach i metoda w obu kierunkach przez modem,
  * drugi start daemona = 0 zapisow rejestrow (C0) w atrapie,
  * zly plik configu E220 = czytelny blad startu lacza w logu daemona; po poprawce lacze wstaje po `restart_delay_ms`.
"""
import json
import os
import re
import shutil
import struct
import subprocess
import tempfile
import threading
import time
import tty

import pytest

import daemon_client as dc
import mav_helpers
from e220_sim import Air, E220Sim, set_uart_speed

HOST = os.environ["DAEMON_HOST"]
PLUGIN = os.environ["E220_PLUGIN"]
SYS, COMP = 10, 3          # system/component daemona
PEER_SYS, PEER_COMP = 7, 1  # stacja (pymavlink)
UART_BAUD = 57600
# rejestry docelowe drivera dla konfiguracji ponizej (ADDH ADDL REG0 REG1 REG2 REG3 + CRYPT): kanal 18, 9600 air, 57600 UART
STATION_EEPROM = bytes([0x00, 0x00, 0xC4, 0x00, 0x12, 0x03, 0x00, 0x00])
RADIO = {"address": 0, "channel": 18, "air_rate": 9600, "power_dbm": 30, "uart_baudrate": UART_BAUD, "parity": "8N1"}


# ------------------------------------------------------------------ uklad wiadomosci z pymavlink (wyrocznia)

def layout(cls):
    fmt = cls.native_format
    fmt = fmt.decode() if isinstance(fmt, (bytes, bytearray)) else fmt
    tokens = re.findall(r"(\d*)([a-zA-Z])", fmt.lstrip("<>=!@"))
    out, pos = {}, 0
    for name, (cnt, ch) in zip(cls.ordered_fieldnames, tokens):
        f = (cnt or "") + ch
        out[name] = (pos, f)
        pos += struct.calcsize("<" + f)
    return out, pos


def payload(cls, **fields):
    lay, n = layout(cls)
    buf = bytearray(n)
    for name, val in fields.items():
        off, f = lay[name]
        struct.pack_into("<" + f, buf, off, val)
    return bytes(buf)


def field(cls, data, name):
    lay, _ = layout(cls)
    off, f = lay[name]
    return struct.unpack_from("<" + f, data, off)[0]


def desc(cls, tsys=None, tcomp=None, status=None):
    lay, n = layout(cls)
    off = lambda k: lay[k][0] if k in lay else dc.NO_OFFSET  # noqa: E731
    return dc.Desc(cls.id, cls.crc_extra, n, cls.msgname, seq=off("seq"), instance=off("instance"),
                   tsys=off("target_system") if tsys else dc.NO_OFFSET,
                   tcomp=off("target_component") if tcomp else dc.NO_OFFSET,
                   status=off("status") if status else dc.NO_OFFSET)


class Svc:
    """Usluga Example z fikstury mav_helpers: event Status + metoda SetMode."""

    def __init__(self, mod):
        self.mod = mod
        self.Status = mod.MAVLink_srp_example_status_message
        self.Req = mod.MAVLink_srp_example_set_mode_req_message
        self.Resp = mod.MAVLink_srp_example_set_mode_resp_message
        self.EV, self.M = 0x8001, 1

    def elements(self):
        return [
            dc.Element(self.EV, desc(self.Status), is_event=True),
            dc.Element(self.M, desc(self.Req, tsys=True, tcomp=True),
                       resp=desc(self.Resp, tsys=True, tcomp=True, status=True), timeout_ms=20000),
        ]

    def status(self, seq, instance, counter):
        return payload(self.Status, seq=seq, instance=instance, state=1, temp=1.5, counter=counter, info_offset=-3)

    def req(self, seq, instance, tsys, tcomp, mode):
        return payload(self.Req, seq=seq, instance=instance, target_system=tsys, target_component=tcomp, mode=mode)

    def resp(self, seq, instance, tsys, tcomp, status, mode):
        return payload(self.Resp, seq=seq, instance=instance, target_system=tsys, target_component=tcomp,
                       status=status, mode=mode)


# ------------------------------------------------------------------ stacja: pymavlink na UART drugiej atrapy

class Station:
    def __init__(self, mod, sim, baud):
        self.mod = mod
        self.fd = os.open(sim.slave_path, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
        tty.setraw(self.fd)
        set_uart_speed(self.fd, baud)  # atrapa porownuje predkosc z termios slave'a z REG0
        self.tx, self.sink = mav_helpers.make_mav(mod, PEER_SYS, PEER_COMP, None, 0)
        self.rx, _ = mav_helpers.make_mav(mod, 255, 0)
        self.rx.robust_parsing = True
        self.msgs = []

    def send(self, msg):
        self.tx.send(msg)
        data = bytes(self.sink.buf)
        self.sink.buf.clear()
        end = time.monotonic() + 10
        while data and time.monotonic() < end:
            try:
                n = os.write(self.fd, data)
                data = data[n:]
            except BlockingIOError:
                time.sleep(0.005)

    def pump(self, seconds=0.2):
        end = time.monotonic() + seconds
        while True:
            try:
                data = os.read(self.fd, 65536)
            except (BlockingIOError, OSError):
                data = b""
            if data:
                for m in self.rx.parse_buffer(data) or []:
                    if m.get_type() != "BAD_DATA":
                        self.msgs.append(m)
            elif time.monotonic() >= end:
                return
            else:
                time.sleep(0.005)

    def wait(self, pred, timeout=15.0):
        end = time.monotonic() + timeout
        while True:
            self.pump(0.05)
            for i, m in enumerate(self.msgs):
                if pred(m):
                    return self.msgs.pop(i)
            if time.monotonic() >= end:
                return None

    def close(self):
        os.close(self.fd)


# ------------------------------------------------------------------ srodowisko

class Env:
    def write_driver_config(self, **override):
        cfg = {"uart": self.node.slave_path, "pins": {"ctl_dir": self.node.ctl}, "e220": dict(RADIO)}
        cfg.update(override)
        with open(self.driver_cfg, "w", encoding="utf-8") as f:
            json.dump(cfg, f)

    def start_daemon(self, restart_ms=1000):
        cfg = {"system_id": SYS, "component_id": COMP, "socket_path": self.sock,
               "driver_dir": os.path.join(self.dir, "plugins"), "restart_delay_ms": restart_ms,
               "links": {"lora": {"driver": "ebyte_e220", "driver_config": self.driver_cfg, "heartbeat_hz": 0}}}
        with open(os.path.join(self.dir, "daemon.json"), "w", encoding="utf-8") as f:
            json.dump(cfg, f)
        self.log = open(self.log_path, "w")  # log per start
        # daemon_host z platformy: gotowosc = {"ev":"ready"} na stdout, stop = zamkniecie stdin
        proc = self.proc = subprocess.Popen([HOST, os.path.join(self.dir, "daemon.json")], stdin=subprocess.PIPE,
                                            stdout=subprocess.PIPE, stderr=self.log, text=True)
        ready = threading.Event()

        def watch_stdout():
            for line in proc.stdout:
                if '"ready"' in line:
                    ready.set()

        threading.Thread(target=watch_stdout, daemon=True).start()
        end = time.monotonic() + 15
        while not ready.wait(0.05):
            assert proc.poll() is None, self.log_text()
            assert time.monotonic() < end, "daemon nie wystartowal:\n" + self.log_text()

    def stop_daemon(self):
        for c in self.clients:
            c.close()
        self.clients = []
        try:
            self.proc.stdin.close()  # EOF = zatrzymanie rdzenia i wyjscie
            self.proc.wait(15)
        except subprocess.TimeoutExpired:
            self.proc.kill()
        self.log.close()

    def log_text(self):
        with open(self.log_path) as f:
            return f.read()

    def client(self):
        c = dc.Client(self.sock)
        self.clients.append(c)
        return c


@pytest.fixture(scope="module")
def mod():
    return mav_helpers.load_dialect(2)


@pytest.fixture
def env(mod):
    e = Env()
    e.dir = tempfile.mkdtemp(prefix="e220e2e_", dir="/tmp")
    os.makedirs(os.path.join(e.dir, "plugins"))
    shutil.copy(PLUGIN, os.path.join(e.dir, "plugins", "libebyte_e220_test.so"))
    air = Air(time_scale=0.3)
    node_ctl, station_ctl = os.path.join(e.dir, "node"), os.path.join(e.dir, "station")
    os.makedirs(station_ctl)
    with open(os.path.join(station_ctl, "eeprom.bin"), "wb") as f:
        f.write(STATION_EEPROM)
    e.node = E220Sim(node_ctl, air=air).start()
    e.station_sim = E220Sim(station_ctl, air=air).start()
    e.station = Station(mod, e.station_sim, UART_BAUD)
    e.svc = Svc(mod)
    e.mod = mod
    e.driver_cfg = os.path.join(e.dir, "e220.json")
    e.sock = os.path.join(e.dir, "d.sock")
    e.log_path = os.path.join(e.dir, "daemon.log")
    e.clients = []
    e.write_driver_config()
    e.start_daemon()
    yield e
    if e.proc.poll() is None:
        e.stop_daemon()
    e.station.close()
    e.node.stop()
    e.station_sim.stop()
    log = e.log_text()
    shutil.rmtree(e.dir, ignore_errors=True)
    assert "ThreadSanitizer" not in log


def register_ok(client, handle, role, svc, instance, links=("lora",), com_id="srp.Example"):
    r = client.register(handle, role, com_id, instance, list(links), svc.elements())
    assert r is not None and r.status == dc.OK, r
    return r


# ------------------------------------------------------------------ testy

def test_plugin_is_loaded_and_two_applications_exchange_events_and_methods_through_the_modem(env):
    s = env.svc
    a, b = env.client(), env.client()
    register_ok(a, 1, dc.ROLE_PROVIDE, s, 3)
    register_ok(b, 1, dc.ROLE_REQUIRE, s, 4)
    assert a.wait_link("lora", True, 30) and b.wait_link("lora", True, 30), env.log_text()
    assert "ebyte_e220" in env.log_text()  # daemon zaladowal plugin i utworzyl driver z fabryki pluginu

    # aplikacja A -> modem -> stacja: event
    a.event(1, s.EV, s.status(seq=11, instance=3, counter=77))
    m = env.station.wait(lambda x: x.get_type() == "SRP_EXAMPLE_STATUS")
    assert m is not None, env.log_text()
    assert (m.instance, m.counter) == (3, 77)
    assert (m.get_srcSystem(), m.get_srcComponent()) == (SYS, COMP)

    # stacja -> modem -> aplikacja B (instancja 4): event
    env.station.send(s.Status(seq=5, instance=4, state=2, temp=9.25, counter=88, info_offset=-7))
    ev = b.expect(dc.EVENT_IN, timeout=30)
    assert ev is not None, env.log_text()
    assert field(s.Status, ev.payload, "counter") == 88 and ev.link == "lora"

    # stacja wywoluje metode u A (provide, instancja 3) i dostaje odpowiedz
    env.station.send(s.Req(seq=5, instance=3, target_system=SYS, target_component=COMP, mode=2))
    rq = a.expect(dc.REQUEST_IN, timeout=30)
    assert rq is not None and field(s.Req, rq.payload, "mode") == 2
    a.response(1, s.M, s.resp(5, 3, rq.sender_system, rq.sender_component, 0, 2))
    resp = env.station.wait(lambda x: x.get_type() == "SRP_EXAMPLE_SET_MODE_RESP")
    assert resp is not None and (resp.seq, resp.instance, resp.status, resp.mode) == (5, 3, 0, 2)

    # B (require, instancja 4) wywoluje metode u stacji, stacja odpowiada
    b.request(1, s.M, s.req(9, 4, PEER_SYS, PEER_COMP, 6))
    got = env.station.wait(lambda x: x.get_type() == "SRP_EXAMPLE_SET_MODE_REQ")
    assert got is not None and (got.seq, got.instance, got.mode) == (9, 4, 6)
    env.station.send(s.Resp(seq=9, instance=4, target_system=got.get_srcSystem(), target_component=got.get_srcComponent(),
                            status=0, mode=6))
    r = b.expect(dc.RESPONSE_IN, timeout=30)
    assert r is not None and field(s.Resp, r.payload, "mode") == 6

    # modem skonfigurowany tym, czego chcial plik drivera
    regs = env.node.eeprom_regs()
    assert regs[4] == 18 and regs[2] == 0xC4, regs.hex()


def test_second_daemon_start_writes_no_registers(env):
    a = env.client()
    register_ok(a, 1, dc.ROLE_PROVIDE, env.svc, 3)
    assert a.wait_link("lora", True, 30), env.log_text()
    first = env.node.stats["eeprom_writes"]
    assert first > 0  # fabryczny modul dostal rozniace sie rejestry (C0)
    env.stop_daemon()
    cmds_before = len(env.node.cmd_log)
    env.start_daemon()
    a = env.client()
    register_ok(a, 1, dc.ROLE_PROVIDE, env.svc, 3)
    assert a.wait_link("lora", True, 30), env.log_text()
    assert env.node.stats["eeprom_writes"] == first  # drugi start: 0 zapisow rejestrow
    new = [c for c in env.node.cmd_log[cmds_before:] if c[0][:1] == b"\xc0"]
    assert not new, new


def test_bad_driver_config_is_a_readable_start_error_and_the_link_starts_after_the_fix(env):
    env.stop_daemon()
    env.write_driver_config(bogus=1)  # nieznane pole
    env.start_daemon(restart_ms=1000)
    a = env.client()
    register_ok(a, 1, dc.ROLE_PROVIDE, env.svc, 3)
    end = time.monotonic() + 10
    while time.monotonic() < end and "nieznane pole 'bogus'" not in env.log_text():
        time.sleep(0.1)
    log = env.log_text()
    assert "nieznane pole 'bogus'" in log, log
    assert env.driver_cfg in log  # czytelny blad: nazwa pliku i pole
    assert "lora" in log
    assert not a.wait_link("lora", True, 2.0)  # lacze niegotowe
    # poprawka pliku: daemon ponawia Start() po restart_delay_ms (produkcyjnie 30 s) i lacze wstaje
    env.write_driver_config()
    assert a.wait_link("lora", True, 30), env.log_text()
    a.event(1, env.svc.EV, env.svc.status(seq=1, instance=3, counter=5))
    assert env.station.wait(lambda x: x.get_type() == "SRP_EXAMPLE_STATUS") is not None


if __name__ == "__main__":
    import sys
    sys.exit(pytest.main([__file__, "-v", "-p", "no:cacheprovider"]))
