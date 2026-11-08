"""Helpers for the QEMU audit tests: ssh to the guest, sim control channel, UDS over DoIP, logger CSV."""
import json
import os
import subprocess
import time

GUEST = os.environ.get("GUEST_IP", "192.168.10.51")
HOST_IP = os.environ.get("HOST_TAP_IP", "192.168.10.4")
SIM = "/tmp/srpsim"
SSH = ["ssh", "-o", "BatchMode=yes", "-o", "StrictHostKeyChecking=no", "-o", "UserKnownHostsFile=/dev/null",
       "-o", "LogLevel=ERROR", "-o", "ConnectTimeout=5", "root@" + GUEST]


def ssh(cmd, timeout=60, check=False):
    r = subprocess.run(SSH + [cmd], capture_output=True, text=True, timeout=timeout)
    if check and r.returncode != 0:
        raise RuntimeError("ssh failed: %s -> %s" % (cmd, r.stderr))
    return r.stdout


def wait_for(pred, timeout=20.0, step=0.5):
    end = time.time() + timeout
    last = None
    while time.time() < end:
        last = pred()
        if last:
            return last
        time.sleep(step)
    return last


def sim_ctl(*lines):
    """Replace the whole ctl file (remove first so every setter is applied again)."""
    ssh("rm -f %s/ctl" % SIM)
    time.sleep(1.2)
    if lines:
        body = "\n".join(lines) + "\n"
        subprocess.run(SSH + ["cat > %s/ctl.tmp && mv %s/ctl.tmp %s/ctl" % (SIM, SIM, SIM)], input=body, text=True, capture_output=True, timeout=30)
    time.sleep(0.8)


def sim_reset():
    ssh(": > %s/events.jsonl" % SIM)
    # the reset lines stay in ctl until the next sim_ctl (a process that does no I2C/temp op in a short
    # window would otherwise never apply them)
    sim_ctl("i2c reset", "temp reset")
    time.sleep(1.2)


def events(src=None, since_ms=0):
    out = ssh("cat %s/events.jsonl" % SIM, timeout=60)
    res = []
    for line in out.splitlines():
        try:
            e = json.loads(line)
        except ValueError:
            continue
        if e.get("t", 0) >= since_ms and (src is None or e.get("src") == src):
            res.append(e)
    return res


def now_ms():
    return int(ssh("date +%s%3N").strip())


_gpio_cache = {}


def gpio_num(pin_id):
    """pin id (EC config) -> kernel gpio number, read from the guest gpio_service config."""
    if not _gpio_cache:
        cfg = json.loads(ssh("cat /srp/opt/gpio_service/etc/config.json"))
        for g in cfg["gpio"]:
            _gpio_cache[g["id"]] = g["num"]
    return _gpio_cache[pin_id]


def gpio_state(pin_id):
    v = ssh("cat %s/state/gpio/%d 2>/dev/null" % (SIM, gpio_num(pin_id))).strip()
    return int(v) if v else None


def gpio_sets(pin_id, since_ms=0):
    """events of the op=set kind for one pin id"""
    n = gpio_num(pin_id)
    return [e for e in events("gpio", since_ms) if e.get("op") == "set" and e.get("pin") == n]


def pca_off_ticks(channel):
    h = ssh("cat %s/state/i2c/0x70.hex" % SIM).strip()
    b = bytes.fromhex(h) if h else b""
    if len(b) < 0x0A + 4 * channel:
        return None
    return b[0x08 + 4 * channel] | (b[0x09 + 4 * channel] << 8)


def processes():
    out = ssh("ps")
    res = {}
    for line in out.splitlines()[1:]:
        p = line.split()
        if len(p) >= 4:
            res.setdefault(p[3], []).append(p[0])
    return res


def uds(hexstr, timeout=15):
    from doipclient import DoIPClient
    c = DoIPClient(GUEST, 0x0A, client_ip_address=HOST_IP)
    try:
        c.send_diagnostic(bytearray.fromhex(hexstr))
        while True:
            r = bytes(c.receive_diagnostic(timeout))
            if r[0] == 0x7F and len(r) > 2 and r[2] == 0x78:
                continue
            return r
    finally:
        try:
            c.close()
        except Exception:
            pass


def logger_start():
    ssh("rm -f /home/root/*_log.csv")
    r = uds("2E001001")
    assert r[0] == 0x6E, "logger start DID refused: %s" % r.hex()
    assert wait_for(lambda: ssh("ls /home/root/*_log.csv 2>/dev/null").strip(), 15), "no CSV created"


def logger_stop():
    try:
        uds("2E001000")
    except Exception:
        pass


def csv_rows():
    out = ssh("cat /home/root/*_log.csv 2>/dev/null", timeout=60).splitlines()
    if len(out) < 2:
        return None, []
    hdr = out[0].split(";")
    rows = []
    for line in out[1:]:
        v = line.split(";")
        if len(v) == len(hdr):
            rows.append(dict(zip(hdr, v)))
    return hdr, rows


def csv_value(col, pred=None, timeout=20):
    """last value of a CSV column once pred(value) holds (None on timeout)"""
    def f():
        _, rows = csv_rows()
        if rows:
            val = float(rows[-1][col])
            if pred is None or pred(val):
                return [val]
        return None
    r = wait_for(f, timeout, 1.0)
    return r[0] if r else None


def last_csv_value(col):
    _, rows = csv_rows()
    return float(rows[-1][col]) if rows else None


EXPECTED = ("em state_manager diag_demon logger someip_demon timestamp_servi i2cMWService gpio_service "
            "temp_service servoService EngineService EnvApp FileLoggerApp SysStatService PrimerService").split()


def ensure_up(tries=3, settle=90):
    """Wait until every EC app runs. First-boot race (servoService exit(1) before i2c/EEPROM are ready)
    -> reboot the guest and retry up to `tries` times. Raises if the guest never comes up."""
    for _ in range(tries):
        end = time.time() + settle
        while time.time() < end:
            try:
                procs = processes()
            except Exception:
                procs = {}
            if procs and all(e in procs for e in EXPECTED):
                time.sleep(8)
                try:
                    if all(e in processes() for e in EXPECTED):
                        return True
                except Exception:
                    pass
            time.sleep(4)
        try:
            ssh("reboot", timeout=10)
        except Exception:
            pass
        time.sleep(25)
    raise AssertionError("harness: EC apps did not come up after %d boots" % tries)


def someip_call(service_id, instance_id, method_id, payload, timeout=40, port=3010):
    """Wywolanie metody SOME/IP (UDP) na gosciu przez someipy; host = tap0. Zwraca (return_code, payload)."""
    import asyncio
    import ipaddress
    from someipy import ServiceBuilder, TransportLayerProtocol
    from someipy.client_service_instance import construct_client_service_instance
    from someipy.service_discovery import construct_service_discovery

    async def run():
        sd = await construct_service_discovery("224.224.224.245", 30490, HOST_IP)
        svc = ServiceBuilder().with_service_id(service_id).with_major_version(1).build()
        inst = await construct_client_service_instance(
            service=svc, instance_id=instance_id, endpoint=(ipaddress.IPv4Address(HOST_IP), port), ttl=5,
            sd_sender=sd, protocol=TransportLayerProtocol.UDP)
        sd.attach(inst)
        try:
            end = time.time() + timeout
            while time.time() < end:
                if inst.service_found():
                    try:
                        res = await asyncio.wait_for(inst.call_method(method_id, payload), 3)
                        return res.return_code, bytes(res.payload)
                    except (RuntimeError, asyncio.TimeoutError):
                        pass
                await asyncio.sleep(0.05)
            raise AssertionError("harness: SOME/IP call 0x%04x/%d/0x%x failed within %ds"
                                 % (service_id, instance_id, method_id, timeout))
        finally:
            sd.close()
            await inst.close()

    return asyncio.run(run())


SERVO_SERVICE = (515, 1)          # ServoService, instancja UDP (app_config servo_service)
M_SET_OXI_DUMP = 5                # SetOxidizerDumpValve(uint8) -> bool (aktuator 62, kanal PCA 4)
ENGINE_SERVICE = (518, 1)         # EngineService, instancja UDP
M_SET_MODE = 2                    # SetMode(uint8) -> bool; DISARM=1 ARM=2 LAUNCH=3 ABORT=64
M_SET_OXI_MAIN = 1                # ServoService.SetOxidizerMainValve(uint8) -> bool (aktuator 60, kanal PCA 3)


def set_mode(mode):
    rc, out = someip_call(*ENGINE_SERVICE, M_SET_MODE, bytes([mode]))
    return out[:1] == b"\x01"


def main_valve_open():
    """Main utleniacza (aktuator 60, kanal 3): otwarty = 102 ticks (0 deg), zamkniety = 512 (180 deg)."""
    t = pca_off_ticks(3)
    return t is not None and t < 300


def reboot_and_wait():
    """Po testach, ktore zostawiaja wątki sekwencji w EngineService - czysty start goscia."""
    try:
        ssh("reboot", timeout=10)
    except Exception:
        pass
    time.sleep(25)
    ensure_up()
