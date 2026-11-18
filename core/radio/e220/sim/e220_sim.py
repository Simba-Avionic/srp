"""Atrapa modemu Ebyte E220-400T30S (T1b) - pty + sterowanie M0/M1/AUX przez pliki w katalogu tymczasowym.

Zrodlo (JEDYNE): oficjalna instrukcja Ebyte "E220-xxxTxxx Series User Manual" (naglowek stron: "E220-T Series User
Manual"), rev. 1.8 z 2026-11-12, 54 strony; sekcje i strony cytowane przy kazdej regule ponizej oraz w
`README.md`. Instrukcja NIE ma ponumerowanych tabel - cytujemy numery sekcji (np. 6.3) i strony.
Gdzie instrukcja milczy, atrapa jest KONSERWATYWNA i oznacza to jako "INTERPRETACJA" (lista w README.md).

Sterowanie (katalog `ctl_dir`):
  m0, m1  - tekst "0"/"1": piny M0/M1 (zapisuje sterownik/test; atrapa czyta co ~1 ms)
  aux     - tekst "0"/"1": pin AUX (zapisuje atrapa atomowo; 1 = wolny, 0 = zajety)
  eeprom.bin - 8 B rejestrow 00H..07H (trwale, jak EEPROM modulu; kasowac aby wrocic do ustawien fabrycznych)
UART: `sim.slave_path` (pty). Predkosc UART atrapa odczytuje z termios pty (master widzi ustawienia slave'a) i
ignoruje bajty przy niezgodnosci (tryb 3: 9600 8N1 - sekcja 6.1; tryby normalne: REG0 bity 7:5 - sekcja 6.3).
"""
import heapq
import os
import select
import termios
import threading
import time
import tty

# --- stale z instrukcji ---------------------------------------------------------------------------------------
DEFAULT_REGS = bytes([0x00, 0x00, 0x62, 0x00, 0x17, 0x03, 0x00, 0x00])  # 6.4 (str. 35): "C0 00 09 00 00 62 00 17 03 00 00"
# UWAGA: 6.4 podaje dlugosc 09, ale wymienia 8 bajtow parametrow (00H..07H) - atrapa uzywa 8 (06.4 = blad dokumentu).
UART_BAUDS = {0: 1200, 1: 2400, 2: 4800, 3: 9600, 4: 19200, 5: 38400, 6: 57600, 7: 115200}  # 6.3, REG0 [7:5] (str. 32)
AIR_RATES = {0: 2400, 1: 2400, 2: 2400, 3: 4800, 4: 9600, 5: 19200, 6: 38400, 7: 62500}  # 6.3, REG0 [2:0] (str. 32)
PACKET_LEN = {0: 200, 1: 128, 2: 64, 3: 32}  # 6.3, REG1 [7:6] (str. 32)
TERMIOS_SPEED = {1200: termios.B1200, 2400: termios.B2400, 4800: termios.B4800, 9600: termios.B9600,
                 19200: termios.B19200, 38400: termios.B38400, 57600: termios.B57600, 115200: termios.B115200}
CONFIG_BAUD = 9600  # 6.1 (str. 30): "when setting, the serial port only supports 9600, 8N1"
MODES = {(0, 0): 0, (0, 1): 1, (1, 0): 2, (1, 1): 3}  # (M1, M0) -> tryb, 5.1 (str. 17)
T_POWER_ON = 0.016  # 5.2.1: T1 = 16 ms (AUX nisko po wlaczeniu)
T_SWITCH = 0.010  # 5.2.4: czas przelaczenia trybu 9-11 ms (AUX nisko)
T_RX_AUX = 0.003  # 5.2.2: AUX nisko 3 ms przed wyjsciem danych z radia na TXD
FORMAT_ERROR = b"\xFF\xFF\xFF"  # 6.1: odpowiedz na bledny format
REG_COUNT = 8
UART_BUFFER = 400  # 5.2.3 (str. 25): "n = 400 Byte" - wewnetrzny bufor UART modulu (max dlugosc jednorazowej transmisji)
MAX_CHANNEL_400 = 83  # 6.3 REG2 (str. 33-34): kanaly 0-83 (pasmo 400 MHz); wyzej modul nie ma czestotliwosci


class Air:
    """Wspolne medium radiowe dla wielu atrap (kanal, szybkosc, adres). Czas antenowy = bajty*8/szybkosc."""

    def __init__(self, time_scale=1.0):
        self.nodes, self.time_scale = [], time_scale

    def join(self, sim):
        self.nodes.append(sim)
        sim.air = self

    def airtime(self, nbytes, rate):
        """Model czasu antenowego: bajty*8/szybkosc radiowa (bez parametrow LoRa SF/BW) * time_scale."""
        return nbytes * 8.0 / rate * self.time_scale

    def transmit(self, src, payload, target, delay=None):
        info = dict(addr=src.addr, channel=src.channel, rate=src.air_rate, target=target)
        if delay is None:
            delay = self.airtime(len(payload), src.air_rate)
        for n in self.nodes:
            if n is not src and n.accepts(info):
                n.schedule_rx(time.monotonic() + delay, payload)


def set_uart_speed(fd, baud):
    """Ustawia predkosc UART (termios) na fd (slave pty) - sterownik robi to samo na prawdziwym porcie."""
    a = termios.tcgetattr(fd)
    a[4] = a[5] = TERMIOS_SPEED[baud]
    termios.tcsetattr(fd, termios.TCSANOW, a)


class E220Sim:
    def __init__(self, ctl_dir, air=None, rssi_byte=0xC4, product_info=None, reset_ms=10.0, time_scale=1.0,
                 aux_indicates_air=False, switch_ms=10.0, frame_break_min_ms=1.0):
        """aux_indicates_air=True: AUX nisko podczas nadawania radiowego (firmware <= 7463-0-14, sekcja 7.1.7, str. 38);
        domyslnie False (fw >= 7463-0-15: AUX nie wskazuje czasu lotu pojedynczego pakietu)."""
        self.aux_indicates_air = aux_indicates_air
        self.frame_break_min_s = frame_break_min_ms / 1000.0  # min. cisza konczaca ramke UART (spec: 3 bajty); testy moga ja wydluzyc
        self.switch_s = switch_ms / 1000.0  # 5.2.4: 9-11 ms; testy zaleznosci czasowych moga je wydluzyc
        self.aux_log = []  # (monotonic, 0/1) kazda zmiana AUX - deterministyczna obserwacja zamiast pollingu pliku
        self.ctl = str(ctl_dir)
        self.rssi_byte, self.product_info = rssi_byte, product_info
        self.reset_s, self.time_scale = reset_ms / 1000.0, time_scale
        self.air = None
        if air:
            air.join(self)
        self._stop = threading.Event()
        self._lock = threading.RLock()
        self.stats = dict(eeprom_writes=0, temp_writes=0, format_errors=0, baud_mismatch_bytes=0, bytes_while_busy=0,
                          tx_packets=0, rx_packets=0, dropped_mode2_tx=0, mode_switches=0, tx_overflow=0,
                          channel_out_of_range=0)
        self.cmd_log = []  # (komenda, odpowiedz)
        self.mode = None

    # --- konfiguracja (rejestry) ---
    @property
    def addr(self):
        return (self.regs[0] << 8) | self.regs[1]

    @property
    def uart_baud(self):
        return UART_BAUDS[self.regs[2] >> 5]

    @property
    def air_rate(self):
        return AIR_RATES[self.regs[2] & 7]

    @property
    def packet_len(self):
        return PACKET_LEN[self.regs[3] >> 6]

    @property
    def channel(self):
        return self.regs[4]

    @property
    def fixed_mode(self):
        return bool(self.regs[5] & 0x40)  # REG3 bit6 (6.3, str. 34)

    @property
    def rssi_enabled(self):
        return bool(self.regs[5] & 0x80)  # REG3 bit7: bajt RSSI po odebranych danych (6.3, str. 34)

    def eeprom_regs(self):
        return bytes(self._eeprom)

    # --- start/stop ---
    def start(self):
        os.makedirs(self.ctl, exist_ok=True)
        for p in ("m0", "m1"):
            if not os.path.exists(os.path.join(self.ctl, p)):
                self._write_file(p, "0")
        ee = os.path.join(self.ctl, "eeprom.bin")
        self._eeprom = bytearray(open(ee, "rb").read() if os.path.exists(ee) else DEFAULT_REGS)
        self.regs = bytearray(self._eeprom)
        self.master, slave = os.openpty()
        tty.setraw(self.master)
        tty.setraw(slave)
        set_uart_speed(slave, CONFIG_BAUD)
        self._slave_keepalive = slave
        self.slave_path = os.ttyname(slave)
        self._heap, self._seq = [], 0
        self._set_aux(0)
        self._busy_until = time.monotonic() + T_POWER_ON  # 5.2.1
        self.mode = self._pins_mode()
        self._txbuf, self._tx_target, self._cmdbuf = bytearray(), None, bytearray()
        self._inflight = None  # (payload, target, done_at) - pakiet w trakcie nadawania
        self._multi = False  # w buforze bylo wiecej niz dlugosc pakietu (scenariusz 2, 5.2.3)
        self._last_rx = self._last_cmd_rx = time.monotonic()
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()
        return self

    def stop(self):
        self._stop.set()
        self._thread.join(timeout=2)
        for fd in (self.master, self._slave_keepalive):
            try:
                os.close(fd)
            except OSError:
                pass

    def __enter__(self):
        return self.start()

    def __exit__(self, *a):
        self.stop()

    # --- pliki sterujace ---
    def _write_file(self, name, text):
        tmp = os.path.join(self.ctl, f".{name}.tmp")
        with open(tmp, "w") as f:
            f.write(text)
        os.replace(tmp, os.path.join(self.ctl, name))

    def _set_aux(self, v):
        ts = time.monotonic()  # czas decyzji (nie konca zapisu pliku - zapis bywa wolny pod obciazeniem I/O)
        self.aux = v
        self._write_file("aux", str(v))  # plik PRZED logiem: log=0 implikuje, ze plik juz pokazuje 0
        self.aux_log.append((ts, v))

    def _read_pin(self, name):
        try:
            return 1 if open(os.path.join(self.ctl, name)).read().strip() == "1" else 0
        except OSError:
            return 0

    def _pins_mode(self):
        return MODES[(self._read_pin("m1"), self._read_pin("m0"))]

    # --- radio ---
    def accepts(self, tx):
        """INTERPRETACJA 5.1.1.1-5.1.1.3 (str. 17-21) + 6.3 (ADDH/ADDL, FFFF = brak filtrowania): ten sam kanal i
        szybkosc; transparent: adresy rowne lub ktorykolwiek = FFFF; fixed: adres docelowy = moj lub FFFF."""
        if self.mode != 0 or self.channel > MAX_CHANNEL_400:  # odbior tylko w trybie 0 (WOR RX nie modelowany)
            return False
        if tx["rate"] != self.air_rate:
            return False
        if tx["target"] is not None:
            taddr, tch = tx["target"]
            return tch == self.channel and (taddr in (self.addr, 0xFFFF) or self.addr == 0xFFFF)
        return tx["channel"] == self.channel and (
            tx["addr"] == self.addr or tx["addr"] == 0xFFFF or self.addr == 0xFFFF)

    def schedule_rx(self, when, payload):
        with self._lock:
            heapq.heappush(self._heap, (when, self._seq, bytes(payload)))
            self._seq += 1

    # --- petla ---
    def _run(self):
        while not self._stop.is_set():
            now = time.monotonic()
            self._service_timers(now)
            r, _, _ = select.select([self.master], [], [], 0.001)
            if r:
                try:
                    data = os.read(self.master, 4096)
                except OSError:
                    break
                self._on_uart(data, time.monotonic())
            self._flush_idle(time.monotonic())

    def _busy(self):
        return time.monotonic() < self._busy_until

    def _aux_wanted(self, now):
        """AUX nisko gdy: zajety (zasilanie/przelaczanie), odbior z radia, UART jeszcze zbiera ramke, albo
        (5.2.3 scenariusz 2) dane przekroczyly dlugosc pakietu i bufor nie jest oproznione, albo fw<=0-14."""
        if now < self._busy_until or self._rx_active:
            return 0
        occ = len(self._txbuf) + (len(self._inflight[0]) if self._inflight else 0)
        if self.mode in (0, 1):
            if self._rx_frame_open:  # UART wciaz zbiera ramke (ciagly strumien bajtow - takze gdy pakiet juz poszedl)
                if not self._frame_broken(now):
                    return 0
                self._rx_frame_open = False
            if self._multi and occ:
                return 0
            if self.aux_indicates_air and (self._inflight or self._txbuf):
                return 0
        return 1

    def _frame_broken(self, now):
        return now - self._last_rx > max(self.frame_break_min_s, 3 * 10.0 / self.uart_baud)  # 5.2.3: 3 bajty ciszy

    def _service_timers(self, now):
        if self.mode in (0, 1):
            self._tx_pump(now)
        want = self._aux_wanted(now)
        if want != self.aux:
            self._set_aux(want)
        if now >= self._busy_until:
            pins = self._pins_mode()
            if pins != self.mode:  # 5.2.4 uwaga 2: AUX wysoko -> przelaczenie natychmiast; nisko -> pin ignorowany
                self._switch(pins, now)
        with self._lock:
            due = []
            while self._heap and self._heap[0][0] <= now:
                due.append(heapq.heappop(self._heap)[2])
        for p in due:
            if self.mode == 0 and not self._busy():
                self._deliver(p)

    _rx_active = False
    _rx_frame_open = False

    def _switch(self, new_mode, now):
        leaving_config = self.mode == 3
        self.stats["mode_switches"] += 1
        self._txbuf.clear()
        self._cmdbuf.clear()
        self._tx_target, self._inflight, self._multi = None, None, False
        dur = self.switch_s
        if leaving_config:  # 5.2.4 uwaga 3 (str. 28): wyjscie z trybu 3 resetuje parametry, AUX nisko
            self.regs = bytearray(self._eeprom)  # INTERPRETACJA: parametry tymczasowe (C2) przepadaja
            dur += self.reset_s
        self._busy_until = now + dur
        self._set_aux(0)  # najpierw AUX nisko, dopiero potem nowy tryb (obserwator nie moze zobaczyc trybu przy AUX=1)
        self.mode = new_mode

    def _deliver(self, payload):
        """5.2.2: AUX nisko 3 ms przed wyjsciem danych, potem dane (+ opcjonalny bajt RSSI) na TXD."""
        self._rx_active = True
        self._set_aux(0)
        time.sleep(T_RX_AUX)
        out = payload + (bytes([self.rssi_byte]) if self.rssi_enabled else b"")
        self.stats["rx_packets"] += 1  # licznik PRZED efektem widocznym na zewnatrz (dane na UART)
        try:
            os.write(self.master, out)
        except OSError:
            pass
        self._rx_active = False
        self._set_aux(1)

    # --- UART ---
    def _speed_ok(self):
        want = CONFIG_BAUD if self.mode == 3 else self.uart_baud
        have = termios.tcgetattr(self.master)[5]
        return have == TERMIOS_SPEED[want]

    def _on_uart(self, data, now):
        if self._busy():
            self.stats["bytes_while_busy"] += len(data)  # modul zajety (AUX nisko): bajty ignorowane
            return
        if not self._speed_ok():
            self.stats["baud_mismatch_bytes"] += len(data)
            return
        if self.mode == 3:
            self._cmdbuf += data
            self._last_cmd_rx = now
            self._parse_cmds()
        elif self.mode == 2:
            self.stats["dropped_mode2_tx"] += len(data)  # 5.1: tryb 2 "WOR send closed"
        elif self.mode in (0, 1):  # tryb 1: WOR Tx - preambula WOR nie jest modelowana
            self._tx_data(data, now)

    def _flush_idle(self, now):
        if self.mode == 3 and self._cmdbuf and now - self._last_cmd_rx > 0.1:
            self._cmdbuf.clear()  # INTERPRETACJA: niepelna komenda porzucana po 100 ms ciszy

    def _occupancy(self):
        return len(self._txbuf) + (len(self._inflight[0]) if self._inflight else 0)

    def _tx_data(self, data, now):
        """5.2.3 (str. 25): bufor UART 400 B; nadmiar jest tracony (tx_overflow) - sterownik MUSI czekac na AUX."""
        self._last_rx = now
        self._rx_frame_open = True
        space = UART_BUFFER - self._occupancy()
        if len(data) > space:
            self.stats["tx_overflow"] += len(data) - max(space, 0)
            data = data[:max(space, 0)]
        self._txbuf += data
        if self._occupancy() > self.packet_len:
            self._multi = True
        self._tx_pump(now)

    def _tx_pump(self, now):
        """Wysyla pakiety co najwyzej packet_len B w tempie szybkosci radiowej (czas antenowy); pakiet ruszy, gdy
        bufor >= dlugosc pakietu albo po przerwie ramki (3 bajty ciszy)."""
        while True:
            if self._inflight:
                payload, target, done_at = self._inflight
                if now < done_at:
                    return
                self._inflight = None
                self.stats["tx_packets"] += 1
                if self.channel > MAX_CHANNEL_400:
                    self.stats["channel_out_of_range"] += 1  # INTERPRETACJA: brak czestotliwosci - nic nie wychodzi
                elif self.air:
                    self.air.transmit(self, payload, target, delay=0.0)
                if not self._txbuf:
                    self._tx_target = None
                    self._multi = False
                continue
            if not self._txbuf:
                return
            if len(self._txbuf) < self.packet_len and not self._frame_broken(now):
                return
            if self.fixed_mode and self._tx_target is None:
                if len(self._txbuf) < 3:
                    if self._frame_broken(now):
                        self._txbuf.clear()
                    return
                self._tx_target = ((self._txbuf[0] << 8) | self._txbuf[1], self._txbuf[2])  # 5.1.1.2
                del self._txbuf[:3]
                if not self._txbuf:
                    return
            if len(self._txbuf) >= self.packet_len and not self._frame_broken(now):
                self._multi = True  # pakiet wyszedl, bo bufor osiagnal dlugosc pakietu, a dane wciaz naplywaja (scenariusz 2)
            chunk = bytes(self._txbuf[:self.packet_len])
            del self._txbuf[:self.packet_len]
            scale = self.air.time_scale if self.air else 1.0
            self._inflight = (chunk, self._tx_target, now + len(chunk) * 8.0 / self.air_rate * scale)

    # --- komendy HEX (tryb 3), 6.1 (str. 30) ---
    def _parse_cmds(self):
        b = self._cmdbuf
        while b:
            op = b[0]
            if op not in (0xC0, 0xC1, 0xC2):
                self.stats["format_errors"] += 1
                self._reply(bytes(b[:1]), FORMAT_ERROR)
                b.clear()
                return
            if len(b) < 3:
                return
            start, ln = b[1], b[2]
            if op == 0xC1:
                end_ok = ln > 0 and start + ln <= REG_COUNT + 1
                need = 3
            else:
                end_ok = ln > 0 and start + ln <= REG_COUNT
                need = 3 + ln
            if not end_ok:
                self.stats["format_errors"] += 1
                self._reply(bytes(b), FORMAT_ERROR)
                b.clear()
                return
            if len(b) < need:
                return
            cmd = bytes(b[:need])
            del b[:need]
            self._exec(op, start, ln, cmd)

    def _exec(self, op, start, ln, cmd):
        if op == 0xC1:
            out = bytearray()
            for a in range(start, start + ln):
                if a == 8:  # 08H: informacja o produkcie - instrukcja nie podaje wartosci (patrz README.md)
                    if self.product_info is None:
                        self.stats["format_errors"] += 1
                        self._reply(cmd, FORMAT_ERROR)
                        return
                    out.append(self.product_info)
                elif a in (6, 7):
                    out.append(0)  # 6.3: CRYPT write-only, odczyt zwraca 0
                else:
                    out.append(self.regs[a])
            self._reply(cmd, bytes([0xC1, start, ln]) + bytes(out))
            return
        params = cmd[3:]
        self.regs[start:start + ln] = params
        if op == 0xC0:  # zapis trwaly (EEPROM)
            self._eeprom[start:start + ln] = params
            with open(os.path.join(self.ctl, "eeprom.bin"), "wb") as f:
                f.write(self._eeprom)
            self.stats["eeprom_writes"] += 1
        else:  # 0xC2: zapis tymczasowy
            self.stats["temp_writes"] += 1
        self._reply(cmd, bytes([0xC1, start, ln]) + bytes(params))

    def _reply(self, cmd, resp):
        self.cmd_log.append((cmd, resp))
        try:
            os.write(self.master, resp)
        except OSError:
            pass


def main(argv=None):
    """CLI dla testow spoza Pythona (np. harness testu kontraktu IRadioDriver w repo srp dla E220RadioDriver):
    uruchamia PARE atrap (A = badany modul, B = zdalny modul) polaczonych radiem i wypisuje sciezki.
        e220_sim.py --ctl-a DIR --ctl-b DIR [--b-regs HEX16] [--air-scale 1.0] [--aux-indicates-air]
    stdout: `SLAVE_A=<pty> CTL_A=<dir> SLAVE_B=<pty> CTL_B=<dir>`; dziala do SIGTERM / zamkniecia stdin.
    Harness: peer_write(bytes) = zapis do SLAVE_B (B nadaje przez radio, A wypisuje na UART -> Read transportu);
    peer_read = odczyt z SLAVE_B (to, co A nadal przez radio). B startuje w trybie 0 z ustawieniami fabrycznymi
    (albo z --b-regs: 8 bajtow rejestrow 00H-07H, hex) - musi miec ten sam kanal i szybkosc radiowa co A."""
    import argparse
    import signal
    import sys

    p = argparse.ArgumentParser(description=main.__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--ctl-a", required=True)
    p.add_argument("--ctl-b", required=True)
    p.add_argument("--b-regs", help="8 B rejestrow 00H-07H dla modulu B (hex, 16 znakow)")
    p.add_argument("--air-scale", type=float, default=1.0)
    p.add_argument("--aux-indicates-air", action="store_true")
    a = p.parse_args(argv)
    os.makedirs(a.ctl_b, exist_ok=True)
    if a.b_regs:
        with open(os.path.join(a.ctl_b, "eeprom.bin"), "wb") as f:
            f.write(bytes.fromhex(a.b_regs))
    air = Air(time_scale=a.air_scale)
    sa = E220Sim(a.ctl_a, air=air, time_scale=a.air_scale, aux_indicates_air=a.aux_indicates_air).start()
    sb = E220Sim(a.ctl_b, air=air, time_scale=a.air_scale).start()
    print(f"SLAVE_A={sa.slave_path} CTL_A={a.ctl_a} SLAVE_B={sb.slave_path} CTL_B={a.ctl_b}", flush=True)
    stop = threading.Event()
    signal.signal(signal.SIGTERM, lambda *_: stop.set())
    try:
        while not stop.wait(0.2):
            if sys.stdin.closed:
                break
    except KeyboardInterrupt:
        pass
    sa.stop()
    sb.stop()
    return 0


if __name__ == "__main__":
    import sys
    sys.exit(main())
