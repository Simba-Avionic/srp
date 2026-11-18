"""Testy atrapy E220-400T30S wzgledem instrukcji Ebyte (E220-xxxTxxx Series User Manual, rev. 1.8, 2026-11-12).
Kazdy test podaje sekcje/strone instrukcji, ktora potwierdza zachowanie; "INTERPRETACJA" = instrukcja milczy
(patrz README.md). Uzywane przez testy drivera E220 w srp (kontrakt zachowania)."""
import os
import select
import sys
import threading
import time
import tty

import pytest

import e220_sim as e2
from e220_sim import Air, E220Sim, set_uart_speed

DEFAULT = bytes([0x00, 0x00, 0x62, 0x00, 0x17, 0x03, 0x00, 0x00])


def write_pins(sim, m1, m0):
    for name, v in (("m1", m1), ("m0", m0)):
        tmp = os.path.join(sim.ctl, f".{name}.w")
        open(tmp, "w").write(str(v))
        os.replace(tmp, os.path.join(sim.ctl, name))


def aux(sim):
    return open(os.path.join(sim.ctl, "aux")).read().strip()


def wait_for(pred, timeout=1.0):
    end = time.monotonic() + timeout
    while time.monotonic() < end:
        if pred():
            return True
        time.sleep(0.0005)
    return False


def to_mode(sim, mode, wait_idle=True):
    write_pins(sim, mode >> 1, mode & 1)
    assert wait_for(lambda: sim.mode == mode, 10.0), "tryb sie nie zmienil"  # limity luzne: zegar zalezy od obciazenia
    if wait_idle:
        assert wait_for(lambda: aux(sim) == "1", 10.0), "AUX nie wrocil na 1"


def uart(sim):
    fd = os.open(sim.slave_path, os.O_RDWR | os.O_NOCTTY)
    tty.setraw(fd)
    return fd


def read_n(fd, n, timeout=0.5):
    got, end = b"", time.monotonic() + timeout
    while len(got) < n and time.monotonic() < end:
        if select.select([fd], [], [], 0.01)[0]:
            got += os.read(fd, 4096)
    return got


def cmd(fd, data, n_resp, timeout=0.5):
    os.write(fd, data)
    return read_n(fd, n_resp, timeout)


@pytest.fixture
def sim(tmp_path):
    s = E220Sim(tmp_path / "ctl", reset_ms=30).start()
    fd = uart(s)
    assert wait_for(lambda: aux(s) == "1")
    yield s, fd
    os.close(fd)
    s.stop()


def low_intervals(sim, since=0):
    """Przedzialy AUX=0 (start, koniec|None) z logu atrapy (deterministyczne, niezalezne od pollingu pliku)."""
    out, start = [], None
    for ts, v in sim.aux_log[since:]:
        if v == 0 and start is None:
            start = ts
        elif v == 1 and start is not None:
            out.append((start, ts))
            start = None
    if start is not None:
        out.append((start, None))
    return out


def low_seen_since(sim, mark):
    """AUX byl nisko po `mark` (w tym: byl juz nisko w chwili mark i nie wrocil)."""
    return any(v == 0 for _, v in sim.aux_log[mark:]) or (mark > 0 and sim.aux_log[mark - 1][1] == 0)


def low_total(sim, since=0):
    now = time.monotonic()
    return sum((e if e is not None else now) - s for s, e in low_intervals(sim, since))


@pytest.fixture
def slow_sim(tmp_path):
    """Okno 'zajety' po przelaczeniu trybu wydluzone do 300 ms: testy zachowania PODCZAS zajetosci sa deterministyczne."""
    s = E220Sim(tmp_path / "ctl", reset_ms=30, switch_ms=300).start()
    fd = uart(s)
    assert wait_for(lambda: aux(s) == "1", 2.0)
    yield s, fd
    os.close(fd)
    s.stop()


def cfg_mode(s):
    to_mode(s, 3)


def test_power_on_aux_low_then_high_after_about_16ms(tmp_path):
    s = E220Sim(tmp_path / "c").start()
    try:
        assert wait_for(lambda: low_intervals(s) and low_intervals(s)[0][1] is not None, 5.0)
        (a, b), = low_intervals(s)[:1]
        assert 0.012 <= b - a <= 0.120, b - a  # 5.2.1 (str. 24): T1 = 16 ms (log atrapy, nie polling)
    finally:
        s.stop()


def test_mode_switch_pulls_aux_low_about_10ms(sim):
    s, fd = sim
    mark = len(s.aux_log)
    write_pins(s, 0, 1)  # M1=0 M0=1 -> tryb 1
    assert wait_for(lambda: s.mode == 1 and aux(s) == "1" and len(low_intervals(s, mark)) == 1
                    and low_intervals(s, mark)[0][1] is not None, 3.0)
    a, b = low_intervals(s, mark)[0]
    assert 0.008 <= b - a <= 0.120, b - a  # 5.2.4 (str. 27): 9-11 ms


def test_leaving_config_mode_keeps_aux_low_longer_for_reset(sim):
    s, fd = sim
    cfg_mode(s)
    mark = len(s.aux_log)
    write_pins(s, 0, 0)
    assert wait_for(lambda: s.mode == 0 and aux(s) == "1" and low_intervals(s, mark)
                    and low_intervals(s, mark)[0][1] is not None, 3.0)
    a, b = low_intervals(s, mark)[0]
    assert b - a >= 0.035  # 10 ms przelaczenia + reset_ms=30 (5.2.4 uwaga 3, str. 28)


def test_pin_change_while_busy_is_applied_after_busy_ends(slow_sim):
    s, fd = slow_sim
    write_pins(s, 1, 1)
    assert wait_for(lambda: s.mode == 3)
    write_pins(s, 0, 0)  # AUX nisko (300 ms zajetosci) -> pin ignorowany do konca zajetosci (5.2.4 uwaga 1, str. 28)
    time.sleep(0.05)
    assert s.mode == 3
    assert wait_for(lambda: s.mode == 0 and aux(s) == "1", 3.0)  # potem stosowany


# --- 6.1 / 6.4: komendy HEX i domyslne rejestry ------------------------------------------------------------------
def test_factory_defaults_read(sim):
    s, fd = sim
    cfg_mode(s)
    assert cmd(fd, bytes([0xC1, 0x00, 0x08]), 11) == bytes([0xC1, 0x00, 0x08]) + DEFAULT  # 6.4 (str. 35)


def test_manual_example_write_address_serial_rate(sim):
    s, fd = sim
    cfg_mode(s)
    w = bytes([0xC0, 0x00, 0x03, 0x12, 0x34, 0x62])  # 6.1 przyklad 2 (str. 30)
    assert cmd(fd, w, 6) == bytes([0xC1, 0x00, 0x03, 0x12, 0x34, 0x62])
    assert cmd(fd, bytes([0xC1, 0x00, 0x03]), 6) == bytes([0xC1, 0x00, 0x03, 0x12, 0x34, 0x62])
    assert s.addr == 0x1234 and s.stats["eeprom_writes"] == 1


def test_manual_example_set_channel(sim):
    s, fd = sim
    cfg_mode(s)
    assert cmd(fd, bytes([0xC0, 0x04, 0x01, 0x09]), 4) == bytes([0xC1, 0x04, 0x01, 0x09])  # 6.1 przyklad 1
    assert cmd(fd, bytes([0xC1, 0x04, 0x01]), 4) == bytes([0xC1, 0x04, 0x01, 0x09])
    assert s.channel == 9


def test_temporary_write_c2_lost_after_leaving_config_mode(sim):
    s, fd = sim
    cfg_mode(s)
    assert cmd(fd, bytes([0xC2, 0x04, 0x01, 0x09]), 4) == bytes([0xC1, 0x04, 0x01, 0x09])
    assert s.stats["temp_writes"] == 1 and s.stats["eeprom_writes"] == 0
    assert s.eeprom_regs() == DEFAULT
    to_mode(s, 0)
    to_mode(s, 3)
    assert cmd(fd, bytes([0xC1, 0x04, 0x01]), 4) == bytes([0xC1, 0x04, 0x01, 0x17])  # INTERPRETACJA 5.2.4 uwaga 3


def test_c0_write_survives_power_cycle(tmp_path):
    ctl = tmp_path / "c"
    s = E220Sim(ctl, reset_ms=5).start()
    fd = uart(s)
    assert wait_for(lambda: aux(s) == "1")
    cfg_mode(s)
    assert cmd(fd, bytes([0xC0, 0x04, 0x01, 0x2A]), 4)
    os.close(fd)
    s.stop()
    s2 = E220Sim(ctl, reset_ms=5)
    s2.start()
    try:
        assert s2.channel == 0x2A and open(ctl / "eeprom.bin", "rb").read()[4] == 0x2A
    finally:
        s2.stop()


@pytest.mark.parametrize("bad", [
    bytes([0x55]),  # nieznany kod
    bytes([0xC1, 0x07, 0x03]),  # odczyt poza zakresem 00H..08H
    bytes([0xC0, 0x07, 0x02, 0x00, 0x00]),  # zapis poza 00H..07H
    bytes([0xC0, 0x00, 0x00]),  # dlugosc 0
    bytes([0xC0, 0xC1, 0xC2, 0xC3]),  # prefiks komend RSSI nie jest komenda trybu 3
])
def test_format_error_returns_ffffff(sim, bad):
    s, fd = sim
    cfg_mode(s)
    assert cmd(fd, bad, 3) == b"\xFF\xFF\xFF"  # 6.1 (str. 31): "Formatting error FF FF FF"
    assert s.stats["format_errors"] == 1 and s.eeprom_regs() == DEFAULT


def test_crypt_registers_are_write_only(sim):
    s, fd = sim
    cfg_mode(s)
    assert cmd(fd, bytes([0xC0, 0x06, 0x02, 0xAB, 0xCD]), 5) == bytes([0xC1, 0x06, 0x02, 0xAB, 0xCD])
    assert cmd(fd, bytes([0xC1, 0x06, 0x02]), 5) == bytes([0xC1, 0x06, 0x02, 0x00, 0x00])  # 6.3: odczyt = 0


def test_product_info_register_value_not_in_manual(tmp_path):
    s = E220Sim(tmp_path / "c").start()
    fd = uart(s)
    assert wait_for(lambda: aux(s) == "1")
    cfg_mode(s)
    assert cmd(fd, bytes([0xC1, 0x08, 0x01]), 3) == b"\xFF\xFF\xFF"  # instrukcja nie podaje wartosci 08H
    os.close(fd)
    s.stop()
    s2 = E220Sim(tmp_path / "c2", product_info=0x5A).start()
    fd = uart(s2)
    try:
        assert wait_for(lambda: aux(s2) == "1")
        cfg_mode(s2)
        assert cmd(fd, bytes([0xC1, 0x08, 0x01]), 4) == bytes([0xC1, 0x08, 0x01, 0x5A])
    finally:
        os.close(fd)
        s2.stop()


# --- 6.1 / 6.3: predkosc UART --------------------------------------------------------------------------------------
def test_config_mode_requires_9600_8n1(sim):
    s, fd = sim
    cfg_mode(s)
    set_uart_speed(fd, 57600)
    assert cmd(fd, bytes([0xC1, 0x00, 0x01]), 4, timeout=0.3) == b""  # 6.1 (str. 30): konfiguracja tylko 9600
    assert s.stats["baud_mismatch_bytes"] == 3
    set_uart_speed(fd, 9600)
    assert cmd(fd, bytes([0xC1, 0x00, 0x01]), 4) == bytes([0xC1, 0x00, 0x01, 0x00])


def test_commands_ignored_while_aux_low(slow_sim):
    s, fd = slow_sim
    write_pins(s, 1, 1)
    assert wait_for(lambda: s.mode == 3)
    assert aux(s) == "0"  # okno zajetosci 300 ms
    os.write(fd, bytes([0xC1, 0x00, 0x01]))
    assert wait_for(lambda: s.stats["bytes_while_busy"] == 3, 2.0)  # 5.2.4 uwaga 1 (str. 28): zajety = bez komend
    assert read_n(fd, 4, 0.1) == b""


def test_uart_speed_after_reg0_applies_only_in_normal_mode(tmp_path):
    air = Air(time_scale=0)
    a = E220Sim(tmp_path / "a", air=air, reset_ms=5).start()
    b = E220Sim(tmp_path / "b", air=air, reset_ms=5).start()
    fa, fb = uart(a), uart(b)
    try:
        assert wait_for(lambda: aux(a) == "1" and aux(b) == "1")
        cfg_mode(a)
        assert cmd(fa, bytes([0xC0, 0x02, 0x01, 0xC2]), 4)  # REG0: 57600 8N1, 2.4k (6.3 str. 32)
        to_mode(a, 0)
        os.write(fa, b"x" * 5)  # UART ciagle 9600 -> modul oczekuje 57600: bajty ignorowane
        assert read_n(fb, 5, 0.3) == b"" and a.stats["baud_mismatch_bytes"] == 5
        set_uart_speed(fa, 57600)
        os.write(fa, b"hello")
        assert read_n(fb, 5) == b"hello"
    finally:
        os.close(fa)
        os.close(fb)
        a.stop()
        b.stop()


# --- 5.1 tryby i tor radiowy ---------------------------------------------------------------------------------------
@pytest.fixture
def pair(tmp_path):
    air = Air(time_scale=0)
    a = E220Sim(tmp_path / "a", air=air, reset_ms=5).start()
    b = E220Sim(tmp_path / "b", air=air, reset_ms=5).start()
    fa, fb = uart(a), uart(b)
    assert wait_for(lambda: aux(a) == "1" and aux(b) == "1")
    yield a, b, fa, fb
    for fd in (fa, fb):
        os.close(fd)
    a.stop()
    b.stop()


def configure(s, fd, *writes):
    cfg_mode(s)
    for w in writes:
        assert cmd(fd, bytes([0xC0]) + bytes(w), len(w) + 1, 0.5)[:1] == b"\xC1"
    to_mode(s, 0)


def test_transparent_link_delivers_what_was_sent(pair):  # 5.1.1.1 (str. 17)
    a, b, fa, fb = pair
    os.write(fa, b"hello")
    assert read_n(fb, 5) == b"hello"
    assert a.stats["tx_packets"] == 1 and b.stats["rx_packets"] == 1


def test_different_channel_not_delivered(pair):  # 6.3 REG2 kanal (str. 33-34)
    a, b, fa, fb = pair
    configure(b, fb, [0x04, 0x01, 0x05])
    os.write(fa, b"hello")
    assert read_n(fb, 5, 0.3) == b""


def test_different_air_rate_not_delivered(pair):  # 6.3 REG0 (str. 32): "air speeds of both parties must be the same"
    a, b, fa, fb = pair
    configure(b, fb, [0x02, 0x01, 0x63])  # 9600 8N1, 4.8k
    os.write(fa, b"hello")
    assert read_n(fb, 5, 0.3) == b""


def test_rssi_byte_appended_when_enabled(pair):  # 6.3 REG3 bit7 (str. 34)
    a, b, fa, fb = pair
    configure(b, fb, [0x05, 0x01, 0x83])
    os.write(fa, b"hello")
    assert read_n(fb, 6) == b"hello" + bytes([0xC4])


def test_fixed_mode_consumes_three_byte_prefix_and_routes(pair):  # 5.1.1.2 (str. 18-19), 6.3 REG3 bit6
    a, b, fa, fb = pair
    configure(a, fa, [0x05, 0x01, 0x43])
    configure(b, fb, [0x00, 0x02, 0x01, 0x02], [0x04, 0x01, 0x04])  # adres 0x0102, kanal 4
    os.write(fa, bytes([0x01, 0x02, 0x04]) + b"data")
    assert read_n(fb, 4) == b"data"  # prefiks nie trafia do odbiorcy
    os.write(fa, bytes([0x09, 0x99, 0x04]) + b"nope")  # inny adres docelowy
    assert read_n(fb, 4, 0.3) == b""
    os.write(fa, bytes([0xFF, 0xFF, 0x04]) + b"bcst")  # broadcast (5.1.1.3, str. 19)
    assert read_n(fb, 4) == b"bcst"


def test_address_ffff_listens_to_everything_on_channel(pair):  # 6.3 ADDH/ADDL (str. 31-32): FFFF = bez filtrowania
    a, b, fa, fb = pair
    configure(b, fb, [0x00, 0x02, 0x77, 0x77], )
    configure(a, fa, [0x00, 0x02, 0xFF, 0xFF])
    os.write(fa, b"abc")
    assert read_n(fb, 3) == b"abc"


def test_packet_length_200_splits_long_data(pair):  # 6.3 REG1 [7:6] (str. 32), 5.2.3 (str. 25)
    a, b, fa, fb = pair
    data = bytes(i % 251 for i in range(450))
    os.write(fa, data[:300])  # > 200 B: AUX nisko do oproznienia bufora (bufor UART to 400 B, wiec nie wolno wyslac 450 naraz)
    assert wait_for(lambda: aux(a) == "0", 0.5) and wait_for(lambda: aux(a) == "1", 2.0)
    os.write(fa, data[300:])
    assert read_n(fb, 450, 1.0) == data
    assert a.stats["tx_packets"] == 3 and a.stats["tx_overflow"] == 0  # 200 + 100 + 150


def test_packet_length_32_option(pair):
    a, b, fa, fb = pair
    configure(a, fa, [0x03, 0x01, 0xC0])
    os.write(fa, b"y" * 100)
    assert read_n(fb, 100, 1.0) == b"y" * 100
    assert a.stats["tx_packets"] == 4  # 32+32+32+4


@pytest.mark.parametrize("mode", [1, 3])
def test_modes_1_and_3_do_not_receive_from_radio(pair, mode):  # E7; 5.1 (str. 17): odbior radiowy tylko w trybie 0
    a, b, fa, fb = pair
    to_mode(b, mode)
    os.write(fa, b"hello")
    assert wait_for(lambda: a.stats["tx_packets"] == 1, 5.0)
    assert read_n(fb, 5, 0.3) == b"" and b.stats["rx_packets"] == 0
    to_mode(b, 0)
    os.write(fa, b"world")
    assert read_n(fb, 5) == b"world" and b.stats["rx_packets"] == 1  # po powrocie do trybu 0 odbior dziala


def test_mode2_wor_receive_drops_uart_data(pair):  # 5.1 (str. 17): tryb 2 - WOR send closed
    a, b, fa, fb = pair
    to_mode(a, 2)
    os.write(fa, b"zzz")
    assert wait_for(lambda: a.stats["dropped_mode2_tx"] == 3, 3.0)
    assert a.stats["tx_packets"] == 0


def test_config_mode_data_is_not_transmitted(pair):  # 5.1 (str. 17): tryb 3 = sleep/konfiguracja
    a, b, fa, fb = pair
    cfg_mode(a)
    os.write(fa, bytes([0xC1, 0x00, 0x01]))
    read_n(fa, 4)
    assert a.stats["tx_packets"] == 0 and read_n(fb, 1, 0.2) == b""


def test_aux_goes_low_before_received_data_and_back_high(pair):  # 5.2.2 (str. 24): AUX nisko ~3 ms przed TXD
    a, b, fa, fb = pair
    mark = len(b.aux_log)
    os.write(fa, b"hi")
    assert read_n(fb, 2) == b"hi"
    assert wait_for(lambda: aux(b) == "1" and low_intervals(b, mark) and low_intervals(b, mark)[0][1] is not None, 3.0)
    s, e = low_intervals(b, mark)[0]
    assert e - s >= 0.002  # 3 ms nisko przed wyjsciem danych (log atrapy)


# --- 5.2.3: bufor UART 400 B, AUX, tempo nadawania ---------------------------------------------------------------
def set_baud_reg0(s, fd, reg0):
    cfg_mode(s)
    assert cmd(fd, bytes([0xC0, 0x02, 0x01, reg0]), 4)
    to_mode(s, 0)
    set_uart_speed(fd, e2.UART_BAUDS[reg0 >> 5])


@pytest.fixture
def fast_pair(tmp_path):
    """Dwie atrapy, UART 115200, szybkosc radiowa 2.4k; czas antenowy skrocony 20x (time_scale)."""
    air = Air(time_scale=0.05)
    a = E220Sim(tmp_path / "a", air=air, reset_ms=5, time_scale=0.05, frame_break_min_ms=300).start()
    b = E220Sim(tmp_path / "b", air=air, reset_ms=5, time_scale=0.05).start()
    fa, fb = uart(a), uart(b)
    assert wait_for(lambda: aux(a) == "1" and aux(b) == "1")
    set_baud_reg0(a, fa, 0xE2)  # 115200 8N1, 2.4k
    yield a, b, fa, fb
    for fd in (fa, fb):
        os.close(fd)
    a.stop()
    b.stop()


def test_1000_bytes_without_waiting_for_aux_overflows_400_byte_buffer(fast_pair):  # 5.2.3 (str. 25) n = 400 B
    a, b, fa, fb = fast_pair
    os.write(fa, bytes(i % 251 for i in range(1000)))
    got = read_n(fb, 1000, 2.0)
    assert a.stats["tx_overflow"] == 600 and len(got) == 400  # 1000 B wpadlo naraz: miesci sie tylko 400 B


def test_waiting_for_aux_between_chunks_loses_nothing(fast_pair):  # scenariusz 2 (str. 25-26): AUX = bufor pusty
    a, b, fa, fb = fast_pair
    data = bytes(i % 251 for i in range(1000))
    for i in range(0, 1000, 250):
        # stan wyjsciowy: AUX wysoko w pliku I w logu (log dopisywany po pliku), bufor pusty - inaczej mark bylby za wczesny
        assert wait_for(lambda: aux(a) == "1" and a.aux_log[-1][1] == 1 and a._occupancy() == 0, 10.0)
        mark = len(a.aux_log)
        os.write(fa, data[i:i + 250])  # kazdy kawalek > dlugosc pakietu (200): AUX nisko do oproznienia bufora
        assert wait_for(lambda: any(v == 0 for _, v in a.aux_log[mark:]), 10.0)  # log, nie polling (impuls ~300 ms)
        assert wait_for(lambda: aux(a) == "1" and a._occupancy() == 0 and a.aux_log[-1][1] == 1, 10.0)
    assert a.stats["tx_overflow"] == 0
    assert read_n(fb, 1000, 3.0) == data


def test_aux_low_duration_matches_airtime_for_multi_packet_data(tmp_path):  # 5.2.3 scenariusz 2: ~czas antenowy +-20%
    """Pomiar zegarowy (1 s): dolna granica twarda (nigdy krocej niz 0.8*czas antenowy), gorna +-20% z best-of-3, bo
    opoznienia planisty pod obciazeniem CPU wydluzaja pomiar (nie skracaja)."""
    expected = 300 * 8 / 2400.0  # 1.0 s (model: bajty*8/szybkosc radiowa)
    lows = []
    for attempt in range(3):
        air = Air(time_scale=1.0)
        a = E220Sim(tmp_path / f"a{attempt}", air=air, reset_ms=5).start()
        fa = uart(a)
        try:
            assert wait_for(lambda: aux(a) == "1", 10.0)
            mark = len(a.aux_log)
            os.write(fa, b"z" * 300)  # > 200 B (dlugosc pakietu) -> AUX nisko do wyslania calego bufora
            assert wait_for(lambda: a.stats["tx_packets"] == 2 and aux(a) == "1", 10.0)
            lows.append(low_total(a, mark))
        finally:
            os.close(fa)
            a.stop()
        assert lows[-1] >= 0.8 * expected, lows
        if lows[-1] <= 1.2 * expected + 0.05:
            return
    pytest.fail(f"zaden z 3 pomiarow nie miesci sie w +-20%: {lows} (oczekiwane {expected})")


def test_single_packet_does_not_hold_aux_low_by_default(tmp_path):  # 5.2.3 scenariusz 1 / 7.1.7 (str. 38, fw >= 0-15)
    air = Air(time_scale=1.0)
    a = E220Sim(tmp_path / "a", air=air, reset_ms=5).start()
    fa = uart(a)
    try:
        assert wait_for(lambda: aux(a) == "1")
        mark = len(a.aux_log)
        os.write(fa, b"x" * 100)  # < 200 B; czas lotu 0.33 s
        assert wait_for(lambda: a.stats["tx_packets"] == 1, 3.0)
        assert low_total(a, mark) < 0.15  # AUX nisko tylko na czas zbierania ramki z UART, nie przez caly lot (0.33 s)
    finally:
        os.close(fa)
        a.stop()


def test_aux_indicates_air_option_holds_aux_low_during_transmission(tmp_path):  # 7.1.7 (str. 38): fw <= 7463-0-14
    air = Air(time_scale=1.0)
    a = E220Sim(tmp_path / "a", air=air, reset_ms=5, aux_indicates_air=True).start()
    fa = uart(a)
    try:
        assert wait_for(lambda: aux(a) == "1")
        mark = len(a.aux_log)
        os.write(fa, b"x" * 100)
        assert wait_for(lambda: a.stats["tx_packets"] == 1 and aux(a) == "1", 3.0)
        assert low_total(a, mark) >= 0.8 * (100 * 8 / 2400.0)  # AUX nisko przez caly lot (0.33 s)
    finally:
        os.close(fa)
        a.stop()


def test_channel_above_83_does_not_transmit_or_receive(pair):  # 6.3 REG2 (str. 33-34): 0-83 dla pasma 400 MHz
    a, b, fa, fb = pair
    configure(a, fa, [0x04, 0x01, 0x54])  # kanal 84
    os.write(fa, b"hello")
    assert read_n(fb, 5, 0.3) == b""
    assert wait_for(lambda: a.stats["channel_out_of_range"] == 1)


# --- pliki sterujace ----------------------------------------------------------------------------------------------
def test_control_files_contract(sim):
    s, fd = sim
    assert open(os.path.join(s.ctl, "m0")).read() == "0" and open(os.path.join(s.ctl, "m1")).read() == "0"
    assert open(os.path.join(s.ctl, "aux")).read() == "1"
    cfg_mode(s)
    cmd(fd, bytes([0xC0, 0x04, 0x01, 0x11]), 4)
    assert open(os.path.join(s.ctl, "eeprom.bin"), "rb").read()[4] == 0x11


def test_cli_pair_for_external_contract_harness(tmp_path):  # kontrakt IRadioDriver w srp (sekcja "Para modułów" w README.md)
    import subprocess
    sim_py = os.path.join(os.path.dirname(os.path.abspath(e2.__file__)), "e220_sim.py")
    p = subprocess.Popen([sys.executable, sim_py, "--ctl-a", str(tmp_path / "a"), "--ctl-b", str(tmp_path / "b"),
                          "--air-scale", "0"], stdout=subprocess.PIPE, stdin=subprocess.PIPE, text=True)
    try:
        info = dict(kv.split("=", 1) for kv in p.stdout.readline().split())
        assert set(info) == {"SLAVE_A", "CTL_A", "SLAVE_B", "CTL_B"}
        fa = os.open(info["SLAVE_A"], os.O_RDWR | os.O_NOCTTY)
        fb = os.open(info["SLAVE_B"], os.O_RDWR | os.O_NOCTTY)
        tty.setraw(fa)
        tty.setraw(fb)
        assert wait_for(lambda: open(os.path.join(info["CTL_A"], "aux")).read() == "1"
                        and open(os.path.join(info["CTL_B"], "aux")).read() == "1")
        os.write(fb, b"from-remote")  # peer_write: zdalny modul nadaje, A wypisuje na UART
        assert read_n(fa, 11) == b"from-remote"
        os.write(fa, b"to-remote")  # peer_read: to, co A nadal
        assert read_n(fb, 9) == b"to-remote"
        os.close(fa)
        os.close(fb)
    finally:
        p.terminate()
        p.wait(timeout=5)


if __name__ == "__main__":
    sys.exit(pytest.main([__file__, "-v", "-p", "no:cacheprovider"]))
