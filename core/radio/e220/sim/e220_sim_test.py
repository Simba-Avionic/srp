"""Tests of the E220 module simulator (e220_sim.py): only the behaviour the driver relies on - the C0/C1 commands and
the FF FF FF reply, AUX around mode changes and traffic (both polarities), the 400 B UART buffer overflow and the
transparent/RSSI-byte reception. Section numbers refer to the Ebyte "E220-xxxTxxx Series User Manual"."""
import os
import tempfile
import tty
import time

import pytest

from e220_sim import Air, E220Sim, set_uart_speed

FACTORY = bytes([0x00, 0x00, 0x62, 0x00, 0x17, 0x03])


class Port:
    """The UART side of a simulator (the pty slave) plus the pins."""

    def __init__(self, sim):
        self.sim = sim
        self.fd = os.open(sim.slave_path, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
        tty.setraw(self.fd)

    def pin(self, name, value):
        tmp = os.path.join(self.sim.ctl, f".{name}.w")
        with open(tmp, "w") as f:
            f.write(str(value))
        os.replace(tmp, os.path.join(self.sim.ctl, name))

    def aux_idle(self):
        level = int(open(os.path.join(self.sim.ctl, "aux")).read().strip())
        return level == self.sim.aux_idle_level

    def wait_aux(self, idle, timeout=2.0):
        end = time.monotonic() + timeout
        while time.monotonic() < end:
            if self.aux_idle() == idle:
                return True
            time.sleep(0.0005)
        return False

    def mode(self, m0, m1):
        assert self.wait_aux(True), "AUX not idle before the mode change"
        self.pin("m0", m0)
        self.pin("m1", m1)
        time.sleep(0.003)
        assert self.wait_aux(True), "AUX did not return to idle after the mode change"

    def config_mode(self):
        self.mode(1, 1)
        set_uart_speed(self.fd, 9600)

    def write(self, data):
        os.write(self.fd, data)

    def read(self, want, timeout=1.0):
        out, end = b"", time.monotonic() + timeout
        while len(out) < want and time.monotonic() < end:
            try:
                out += os.read(self.fd, 4096)
            except (BlockingIOError, OSError):
                time.sleep(0.002)
        return out

    def close(self):
        os.close(self.fd)


@pytest.fixture(params=[1, 0], ids=["aux_idle_high", "aux_idle_low"])
def idle_level(request):
    return request.param


@pytest.fixture
def make():
    made = []

    def factory(ctl=None, **kw):
        sim = E220Sim(ctl or tempfile.mkdtemp(prefix="e220sim_", dir="/tmp"), **kw).start()
        port = Port(sim)
        made.append((sim, port))
        return sim, port

    yield factory
    for sim, port in made:
        port.close()
        sim.stop()


def test_c1_reads_registers_and_c0_writes_them_persistently(make):  # 6.1 (p. 30-31), 6.4 (p. 35)
    sim, p = make()
    assert p.wait_aux(True)  # 5.2.1: AUX is busy ~16 ms after power-up
    p.config_mode()
    p.write(bytes([0xC1, 0x00, 0x06]))
    assert p.read(9) == bytes([0xC1, 0x00, 0x06]) + FACTORY
    p.write(bytes([0xC0, 0x00, 0x03, 0x12, 0x34, 0x62]))  # the manual example: address 1234, 9600 8N1, 2.4k
    assert p.read(6) == bytes([0xC1, 0x00, 0x03, 0x12, 0x34, 0x62])
    assert sim.stats["eeprom_writes"] == 1
    assert sim.eeprom_regs()[:3] == bytes([0x12, 0x34, 0x62])
    with open(os.path.join(sim.ctl, "eeprom.bin"), "rb") as f:
        assert f.read()[:3] == bytes([0x12, 0x34, 0x62])  # persistent across restarts
    p.write(bytes([0xC1, 0x06, 0x02]))
    assert p.read(5) == bytes([0xC1, 0x06, 0x02, 0x00, 0x00])  # 6.3: CRYPT is write-only, reads as 0


@pytest.mark.parametrize("cmd", [bytes([0xC3, 0x00, 0x01]), bytes([0xC1, 0x07, 0x02]), bytes([0xC0, 0x00, 0x00])])
def test_bad_command_gets_ff_ff_ff(make, cmd):  # 6.1: a wrong format is answered with FF FF FF
    sim, p = make()
    p.config_mode()
    p.write(cmd)
    assert p.read(3)[:3] == b"\xff\xff\xff"
    assert sim.stats["format_errors"] == 1


def test_commands_are_ignored_when_the_uart_is_not_9600_8n1(make):  # 6.1 (p. 30)
    sim, p = make()
    p.config_mode()
    set_uart_speed(p.fd, 57600)
    p.write(bytes([0xC1, 0x00, 0x06]))
    assert p.read(9, timeout=0.3) == b""
    assert sim.stats["baud_mismatch_bytes"] == 3


def test_aux_is_busy_while_the_mode_changes_and_returns_idle(make, idle_level):  # 5.2.4 (p. 27-28)
    sim, p = make(aux_idle_level=idle_level)
    assert p.wait_aux(True)
    t0 = time.monotonic()
    p.pin("m0", 1)
    p.pin("m1", 1)
    assert p.wait_aux(False, timeout=0.5)  # AUX goes busy for the switch...
    assert p.wait_aux(True, timeout=0.5)  # ...and idle again after ~10 ms
    busy_for = time.monotonic() - t0
    assert 0.005 < busy_for < 0.2
    assert sim.mode == 3 and sim.stats["mode_switches"] == 1
    levels = {lvl for _, lvl in sim.aux_log}
    assert levels == {0, 1}  # both levels were driven; which one means idle follows aux_idle_level
    time.sleep(0.02)  # the log entry follows the file write
    assert sim.aux_log[-1][1] == idle_level


def test_aux_is_busy_right_after_power_up(make, idle_level):  # 5.2.1 (p. 24)
    sim, p = make(aux_idle_level=idle_level)
    assert sim.aux_log[0][1] == 1 - idle_level  # busy first...
    assert p.wait_aux(True, timeout=0.5)  # ...idle after the power-up time


def test_data_crosses_the_air_unchanged_and_aux_frames_the_transmission(make, idle_level):  # 5.1.1.1, 5.2.3
    air = Air(time_scale=0.05)
    a, pa = make(air=air, aux_idle_level=idle_level)
    b, pb = make(air=air, aux_idle_level=idle_level)
    assert pa.wait_aux(True) and pb.wait_aux(True)
    payload = bytes(range(150))
    pa.write(payload)
    assert pa.wait_aux(False, timeout=0.5)  # busy as soon as the data is taken...
    assert pb.read(len(payload)) == payload  # transparent: no prefix, no RSSI byte
    assert pa.wait_aux(True, timeout=2.0)  # ...idle once everything has been transmitted
    assert a.stats["tx_packets"] == 1 and b.stats["rx_packets"] == 1


def test_rssi_byte_is_appended_when_reg3_bit7_is_set(make):  # 6.3 REG3 bit7 (p. 34)
    air = Air(time_scale=0.05)
    ctl_b = tempfile.mkdtemp(prefix="e220sim_", dir="/tmp")
    with open(os.path.join(ctl_b, "eeprom.bin"), "wb") as f:
        f.write(bytes([0x00, 0x00, 0x62, 0x00, 0x17, 0x83, 0x00, 0x00]))
    a, pa = make(air=air)
    b, pb = make(ctl=ctl_b, air=air, rssi_byte=0xC4)
    assert pa.wait_aux(True) and pb.wait_aux(True)  # past the power-up time
    pa.write(b"hello")
    assert pb.read(6) == b"hello" + bytes([0xC4])


def test_other_channel_or_air_rate_is_not_received(make):
    air = Air(time_scale=0.05)
    ctl_b = tempfile.mkdtemp(prefix="e220sim_", dir="/tmp")
    with open(os.path.join(ctl_b, "eeprom.bin"), "wb") as f:
        f.write(bytes([0x00, 0x00, 0x62, 0x00, 0x18, 0x03, 0x00, 0x00]))  # channel 24 instead of 23
    a, pa = make(air=air)
    b, pb = make(ctl=ctl_b, air=air)
    assert pa.wait_aux(True) and pb.wait_aux(True)
    pa.write(b"x" * 20)
    assert pb.read(1, timeout=0.5) == b""


def test_uart_buffer_overflow_loses_the_excess_and_is_counted(make):  # 5.2.3 (p. 25): n = 400 B
    sim, p = make()
    assert p.wait_aux(True)
    time.sleep(0.02)
    p.write(bytes(600))
    deadline = time.monotonic() + 1.0
    while sim.stats["tx_overflow"] == 0 and time.monotonic() < deadline:
        time.sleep(0.005)
    assert sim.stats["tx_overflow"] == 200


def test_a_new_frame_written_while_aux_is_busy_is_counted(make):  # what the AUX-gated driver must never do
    air = Air(time_scale=1.0)
    sim, p = make(air=air)
    assert p.wait_aux(True)
    p.write(bytes(100))
    assert p.wait_aux(False, timeout=0.5)
    time.sleep(0.02)  # the first frame is closed (silence), still being transmitted: AUX is busy
    p.write(bytes(10))
    time.sleep(0.05)
    assert sim.stats["writes_while_aux_busy"] == 1


RSSI_CMD = bytes([0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x02])


def make_with_regs(make, regs, **kw):
    ctl = tempfile.mkdtemp(prefix="e220sim_", dir="/tmp")
    with open(os.path.join(ctl, "eeprom.bin"), "wb") as f:
        f.write(regs)
    sim, p = make(ctl=ctl, **kw)
    assert p.wait_aux(True)
    return sim, p


def test_ambient_rssi_read_answers_noise_and_last_packet_rssi(make):  # 6.3 REG1 bit5 (p. 33)
    air = Air(time_scale=0.05)
    a, pa = make(air=air)
    b, pb = make_with_regs(make, bytes([0x00, 0x00, 0x62, 0x20, 0x17, 0x03, 0x00, 0x00]), air=air,
                           rssi_byte=0xC4, noise_rssi=0xB0)
    assert pa.wait_aux(True)
    pb.write(RSSI_CMD)
    assert pb.read(5) == bytes([0xC1, 0x00, 0x02, 0xB0, 0x00])  # no packet yet: last RSSI 0
    pa.write(b"hello")
    assert pb.read(5) == b"hello"
    pb.write(RSSI_CMD)
    assert pb.read(5) == bytes([0xC1, 0x00, 0x02, 0xB0, 0xC4])  # last packet RSSI = -(256 - 196) = -60 dBm
    assert len(b.rssi_reads) == 2
    b.noise_rssi = 0xA0
    pb.write(bytes([0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x01]))
    assert pb.read(4) == bytes([0xC1, 0x00, 0x01, 0xA0])  # length 1: the noise only


def test_ambient_rssi_read_is_not_sent_over_the_air_and_not_in_the_command_log(make):
    air = Air(time_scale=0.05)
    a, pa = make(air=air)
    b, pb = make_with_regs(make, bytes([0x00, 0x00, 0x62, 0x20, 0x17, 0x03, 0x00, 0x00]), air=air)
    assert pa.wait_aux(True)
    pb.write(RSSI_CMD)
    assert len(pb.read(5)) == 5
    assert pa.read(1, timeout=0.3) == b""
    assert b.stats["tx_packets"] == 0
    assert b.cmd_log == []  # the C0/C1 configuration log is for configuration commands only


def test_without_reg1_bit5_the_rssi_command_is_ordinary_data(make):
    air = Air(time_scale=0.05)
    a, pa = make(air=air)
    b, pb = make(air=air)
    assert pa.wait_aux(True) and pb.wait_aux(True)
    pb.write(RSSI_CMD)
    assert pa.read(6) == RSSI_CMD  # the module passes it on like any data
    assert b.rssi_reads == []


@pytest.mark.parametrize("cmd", [bytes([0xC0, 0xC1, 0xC2, 0xC3, 0x01, 0x01]), bytes([0xC0, 0xC1, 0xC2, 0xC3, 0x00, 0x03])])
def test_ambient_rssi_read_with_a_bad_start_or_length_gets_no_reply(make, cmd):
    sim, p = make_with_regs(make, bytes([0x00, 0x00, 0x62, 0x20, 0x17, 0x03, 0x00, 0x00]))
    p.write(cmd)
    assert p.read(1, timeout=0.3) == b""
    assert sim.rssi_reads == []


if __name__ == "__main__":
    import sys
    sys.exit(pytest.main([__file__, "-v", "-p", "no:cacheprovider"]))
