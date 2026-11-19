"""Ebyte E220-400T30S module simulator: a pty + M0/M1/AUX control through files in a temporary directory.

Source (the only one): the official Ebyte "E220-xxxTxxx Series User Manual"; sections and pages are cited at
each rule below and in `README.md`. The manual has no numbered tables, so we cite section numbers (e.g. 6.3)
and pages. Where the manual is silent the simulator is conservative and says so ("INTERPRETATION", see README.md).

Only what the driver relies on is simulated: configuration mode with the C0/C1 commands and the FF FF FF reply,
AUX around mode changes, power-up and traffic (in either polarity), the 400 B UART buffer with its overflow,
transparent transmission and the optional RSSI byte.

Control (directory `ctl_dir`):
  m0, m1  - text "0"/"1": the M0/M1 pins (written by the driver/test; the simulator reads them every ~1 ms)
  aux     - text "0"/"1": the AUX pin (written atomically by the simulator; the idle level is `aux_idle_level`)
  eeprom.bin - 8 B of registers 00H..07H (persistent like the module EEPROM; delete it for the factory settings)
UART: `sim.slave_path` (a pty). The simulator reads the UART speed from the pty termios (the master sees the slave
settings) and ignores bytes on a mismatch (configuration mode: 9600 8N1 - section 6.1; normal modes: REG0 bits 7:5).
"""
import heapq
import os
import select
import termios
import threading
import time
import tty

# --- constants from the manual ---------------------------------------------------------------------------------
DEFAULT_REGS = bytes([0x00, 0x00, 0x62, 0x00, 0x17, 0x03, 0x00, 0x00])  # 6.4 (p. 35): "C0 00 09 00 00 62 00 17 03 00 00"
# NOTE: 6.4 gives length 09 but lists 8 parameter bytes (00H..07H) - the simulator uses 8 (a mistake in the manual).
UART_BAUDS = {0: 1200, 1: 2400, 2: 4800, 3: 9600, 4: 19200, 5: 38400, 6: 57600, 7: 115200}  # 6.3, REG0 [7:5] (p. 32)
AIR_RATES = {0: 2400, 1: 2400, 2: 2400, 3: 4800, 4: 9600, 5: 19200, 6: 38400, 7: 62500}  # 6.3, REG0 [2:0] (p. 32)
PACKET_LEN = {0: 200, 1: 128, 2: 64, 3: 32}  # 6.3, REG1 [7:6] (p. 32)
TERMIOS_SPEED = {1200: termios.B1200, 2400: termios.B2400, 4800: termios.B4800, 9600: termios.B9600,
                 19200: termios.B19200, 38400: termios.B38400, 57600: termios.B57600, 115200: termios.B115200}
CONFIG_BAUD = 9600  # 6.1 (p. 30): "when setting, the serial port only supports 9600, 8N1"
MODES = {(0, 0): 0, (1, 1): 3}  # (M1, M0) -> mode, 5.1 (p. 17); the WOR modes 01/10 are not simulated (pins ignored)
T_POWER_ON = 0.016  # 5.2.1: T1 = 16 ms (AUX busy after power-up)
T_SWITCH = 0.010  # 5.2.4: mode switch 9-11 ms (AUX busy)
T_RX_AUX = 0.003  # 5.2.2: AUX busy 3 ms before data from the radio goes out on TXD
FORMAT_ERROR = b"\xFF\xFF\xFF"  # 6.1: reply to a wrong format
REG_COUNT = 8
UART_BUFFER = 400  # 5.2.3 (p. 25): "n = 400 Byte" - the internal UART buffer of the module
MAX_CHANNEL_400 = 83  # 6.3 REG2 (p. 33-34): channels 0-83 (the 400 MHz band)


class Air:
    """A shared radio medium for several simulators (channel, air rate). Air time = bytes*8/rate."""

    def __init__(self, time_scale=1.0):
        self.nodes, self.time_scale = [], time_scale

    def join(self, sim):
        self.nodes.append(sim)
        sim.air = self

    def transmit(self, src, payload):
        for n in self.nodes:
            if n is not src and n.accepts(src):
                n.schedule_rx(time.monotonic(), payload)


def set_uart_speed(fd, baud):
    """Sets the UART speed (termios) on fd (the pty slave) - the driver does the same on a real port."""
    a = termios.tcgetattr(fd)
    a[4] = a[5] = TERMIOS_SPEED[baud]
    termios.tcsetattr(fd, termios.TCSANOW, a)


class E220Sim:
    def __init__(self, ctl_dir, air=None, rssi_byte=0xC4, time_scale=1.0, switch_ms=10.0, aux_idle_level=1):
        self.aux_idle_level = aux_idle_level  # AUX level while the module is idle (the opposite = busy)
        self.switch_s = switch_ms / 1000.0  # 5.2.4: 9-11 ms; timing tests may lengthen it
        self.time_scale = time_scale
        self.rssi_byte = rssi_byte
        self.aux_log = []  # (monotonic, level) every AUX change - deterministic observation instead of file polling
        self.ctl = str(ctl_dir)
        self.air = None
        if air:
            air.join(self)
        self._stop = threading.Event()
        self._lock = threading.RLock()
        self.stats = dict(eeprom_writes=0, format_errors=0, baud_mismatch_bytes=0, bytes_while_busy=0,
                          writes_while_aux_busy=0, tx_packets=0, rx_packets=0, mode_switches=0, tx_overflow=0)
        self.cmd_log = []  # (command, reply)
        self.mode = None

    # --- configuration (registers) ---
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
    def rssi_enabled(self):
        return bool(self.regs[5] & 0x80)  # REG3 bit7: an RSSI byte after the received data (6.3, p. 34)

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
        self.regs = self._eeprom
        self.master, slave = os.openpty()
        tty.setraw(self.master)
        tty.setraw(slave)
        set_uart_speed(slave, CONFIG_BAUD)
        self._slave_keepalive = slave
        self.slave_path = os.ttyname(slave)
        self._heap, self._seq = [], 0
        self._busy_until = time.monotonic() + T_POWER_ON  # 5.2.1
        self.aux = None
        self._set_aux(1)
        self.mode = self._pins_mode(0)
        self._txbuf, self._cmdbuf, self._inflight = bytearray(), bytearray(), None  # inflight = (payload, done_at)
        self._rx_active = self._frame_open = False
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

    # --- control files ---
    def _write_file(self, name, text):
        tmp = os.path.join(self.ctl, f".{name}.tmp")
        with open(tmp, "w") as f:
            f.write(text)
        os.replace(tmp, os.path.join(self.ctl, name))

    def _set_aux(self, busy):
        """Writes the AUX pin: `busy` truthy = the level opposite to aux_idle_level."""
        level = (1 - self.aux_idle_level) if busy else self.aux_idle_level
        ts = time.monotonic()  # the time of the decision (the file write may be slow under I/O load)
        self.aux = 1 if busy else 0
        self._write_file("aux", str(level))  # the file BEFORE the log: a logged level is already visible in the file
        self.aux_log.append((ts, level))

    def _read_pin(self, name):
        try:
            with open(os.path.join(self.ctl, name)) as f:
                return 1 if f.read().strip() == "1" else 0
        except OSError:
            return 0

    def _pins_mode(self, current):
        return MODES.get((self._read_pin("m1"), self._read_pin("m0")), current)

    # --- radio ---
    def accepts(self, tx):
        """INTERPRETATION of 5.1.1.1 (p. 17-21): the same channel and air rate; addresses are not filtered."""
        return self.mode == 0 and tx.channel <= MAX_CHANNEL_400 and tx.channel == self.channel \
            and tx.air_rate == self.air_rate

    def schedule_rx(self, when, payload):
        with self._lock:
            heapq.heappush(self._heap, (when, self._seq, bytes(payload)))
            self._seq += 1

    # --- loop ---
    def _run(self):
        while not self._stop.is_set():
            self._service(time.monotonic())
            r, _, _ = select.select([self.master], [], [], 0.001)
            if r:
                try:
                    data = os.read(self.master, 4096)
                except OSError:
                    break
                self._on_uart(data, time.monotonic())

    def _frame_broken(self, now):
        return now - self._last_rx > max(0.001, 3 * 10.0 / self.uart_baud)  # 5.2.3: 3 bytes of silence

    def _aux_busy(self, now):
        """AUX busy: powering up / switching, receiving from the radio, the UART still collecting a frame or data
        waiting/in flight (5.2.3)."""
        if now < self._busy_until or self._rx_active:
            return True
        return self.mode == 0 and bool(self._txbuf or self._inflight or self._frame_open)

    def _service(self, now):
        if self.mode == 0:
            if self._frame_open and self._frame_broken(now):
                self._frame_open = False
            self._tx_pump(now)
        busy = self._aux_busy(now)
        if busy != bool(self.aux):
            self._set_aux(busy)
        if now >= self._busy_until:
            pins = self._pins_mode(self.mode)
            if pins != self.mode:  # 5.2.4 note 2: AUX idle -> the switch is immediate; busy -> the pins are ignored
                self._switch(pins, now)
        with self._lock:
            due = []
            while self._heap and self._heap[0][0] <= now:
                due.append(heapq.heappop(self._heap)[2])
        for p in due:
            if self.mode == 0 and now >= self._busy_until:
                self._deliver(p)

    def _switch(self, new_mode, now):
        self.stats["mode_switches"] += 1
        self._txbuf.clear()
        self._cmdbuf.clear()
        self._inflight = None
        self._frame_open = False
        self._busy_until = now + self.switch_s
        self._set_aux(True)  # AUX busy first, only then the new mode (an observer must not see the mode at idle)
        self.mode = new_mode

    def _deliver(self, payload):
        """5.2.2: AUX busy 3 ms before the data, then the data (+ the optional RSSI byte) on TXD."""
        self._rx_active = True
        self._set_aux(True)
        time.sleep(T_RX_AUX)
        out = payload + (bytes([self.rssi_byte]) if self.rssi_enabled else b"")
        self.stats["rx_packets"] += 1  # the counter BEFORE the externally visible effect (data on the UART)
        try:
            os.write(self.master, out)
        except OSError:
            pass
        self._rx_active = False
        self._set_aux(False)

    # --- UART ---
    def _speed_ok(self):
        want = CONFIG_BAUD if self.mode == 3 else self.uart_baud
        return termios.tcgetattr(self.master)[5] == TERMIOS_SPEED[want]

    def _on_uart(self, data, now):
        if now < self._busy_until:
            self.stats["bytes_while_busy"] += len(data)  # powering up / switching: the bytes are ignored
            return
        if not self._speed_ok():
            self.stats["baud_mismatch_bytes"] += len(data)
            return
        if self.mode == 3:
            self._cmdbuf += data
            self._last_cmd_rx = now
            self._parse_cmds()
            return
        if not self._frame_open and (self._txbuf or self._inflight):
            self.stats["writes_while_aux_busy"] += 1  # a new frame while the previous data is still being handled
        self._tx_data(data, now)

    def _tx_data(self, data, now):
        """5.2.3 (p. 25): a 400 B UART buffer; the excess is lost (tx_overflow) - the driver MUST wait for AUX."""
        self._last_rx = now
        self._frame_open = True
        occupied = len(self._txbuf) + (len(self._inflight[0]) if self._inflight else 0)
        space = max(UART_BUFFER - occupied, 0)
        if len(data) > space:
            self.stats["tx_overflow"] += len(data) - space
            data = data[:space]
        self._txbuf += data
        self._tx_pump(now)

    def _tx_pump(self, now):
        """Sends packets of at most packet_len bytes at the air rate; a packet starts when the buffer reaches the
        packet length or after the frame gap (3 bytes of silence)."""
        while True:
            if self._inflight:
                payload, done_at = self._inflight
                if now < done_at:
                    return
                self._inflight = None
                self.stats["tx_packets"] += 1
                if self.air and self.channel <= MAX_CHANNEL_400:
                    self.air.transmit(self, payload)
                continue
            if not self._txbuf:
                return
            if len(self._txbuf) < self.packet_len and not self._frame_broken(now):
                return
            chunk = bytes(self._txbuf[:self.packet_len])
            del self._txbuf[:self.packet_len]
            scale = self.air.time_scale if self.air else self.time_scale
            self._inflight = (chunk, now + len(chunk) * 8.0 / self.air_rate * scale)

    # --- HEX commands (mode 3), 6.1 (p. 30) ---
    def _parse_cmds(self):
        b = self._cmdbuf
        while b:
            op = b[0]
            if op not in (0xC0, 0xC1):
                self.stats["format_errors"] += 1
                self._reply(bytes(b[:1]), FORMAT_ERROR)
                b.clear()
                return
            if len(b) < 3:
                return
            start, ln = b[1], b[2]
            if ln == 0 or start + ln > REG_COUNT:
                self.stats["format_errors"] += 1
                self._reply(bytes(b), FORMAT_ERROR)
                b.clear()
                return
            need = 3 if op == 0xC1 else 3 + ln
            if len(b) < need:
                return
            cmd = bytes(b[:need])
            del b[:need]
            self._exec(op, start, ln, cmd)

    def _exec(self, op, start, ln, cmd):
        if op == 0xC1:
            # 6.3: CRYPT 06H/07H are write-only, a read returns 0
            out = bytes(0 if a in (6, 7) else self.regs[a] for a in range(start, start + ln))
            self._reply(cmd, bytes([0xC1, start, ln]) + out)
            return
        params = cmd[3:]
        self._eeprom[start:start + ln] = params
        with open(os.path.join(self.ctl, "eeprom.bin"), "wb") as f:
            f.write(self._eeprom)
        self.stats["eeprom_writes"] += 1
        self._reply(cmd, bytes([0xC1, start, ln]) + bytes(params))

    def _reply(self, cmd, resp):
        self.cmd_log.append((cmd, resp))
        try:
            os.write(self.master, resp)
        except OSError:
            pass
