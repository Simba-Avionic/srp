# pylint: disable=line-too-long,missing-function-docstring,consider-using-f-string,broad-exception-caught,subprocess-run-check,unspecified-encoding,import-outside-toplevel,import-error,redefined-outer-name,unused-variable,too-many-locals
"""Audit findings on QEMU (sim build). Each test asserts the CORRECT behaviour -> RED on the current code.
The failure message names the F-xx id. Harness problems must fail with 'harness:' (never counted as confirmation)."""
import time

import pytest

import audit_lib as L

pytestmark = pytest.mark.bugs


def _primer_connected():
    L.sim_ctl("i2c adc 3 3.0")
    L.logger_start()
    assert L.csv_value("PRIMER_STATUS", lambda x: x == 2, 40) == 2, "harness: primer never reached CONNECTED"


def test_f05_primer_state_stale_after_adc_error():
    """F-05 primer_controller.cpp:75-78. After an I2C/ADC error the primer must not stay CONNECTED.
    Expected PRIMER_STATUS != 2 (UNKNOWN); actual: stays 2."""
    _primer_connected()
    L.sim_ctl("i2c adc 3 3.0", "i2c fault 0x48 nack")
    time.sleep(15)
    v = L.last_csv_value("PRIMER_STATUS")
    assert v is not None, "harness: no CSV rows"
    assert v != 2, "F-05: PRIMER_STATUS still CONNECTED(2) 15 s after ADC NACK (primer_controller.cpp:75-78)"


def test_f06_primer_never_reaches_fired():
    """F-06 primer_controller.cpp:129 (commented out). After a fire the state must become FIRED (4).
    Actual: stays CONNECTED(2), DisablePrimer is always rejected."""
    _primer_connected()
    r = L.uds("2E0017")
    time.sleep(4)
    v = L.last_csv_value("PRIMER_STATUS")
    assert r[0] == 0x6E or r[0] == 0x7F, "harness: unexpected UDS reply %s" % r.hex()
    assert v == 4, "F-06: PRIMER_STATUS=%s after ignition request, expected FIRED(4) (primer_controller.cpp:129)" % v


def _arm_with_closed_main():
    assert not L.main_valve_open(), "harness: main valve not closed before the test (PCA ch3=%s)" % L.pca_off_ticks(3)
    assert L.set_mode(2), "harness: ARM refused"


def test_f01_launch_opens_valves_without_primer():
    """F-01 engine_app.cpp:254-268. OnLaunch ignores the StartPrime() result. Primer NOT connected (ADC ch3 = 0 V)
    -> LAUNCH must not open the oxidizer main valve (no ignition). Expected: PCA ch3 stays closed (512).
    Actual: main valve (ch3) opens ~0.7 s after LAUNCH."""
    try:
        L.sim_ctl("i2c adc 3 0.0")
        time.sleep(3)
        _arm_with_closed_main()
        assert L.set_mode(3), "harness: LAUNCH refused"
        opened = L.wait_for(L.main_valve_open, 4)
        assert not opened, ("F-01: oxidizer main valve opened after LAUNCH with primer NOT_CONNECTED "
                            "(engine_app.cpp:254-268, StartPrime result ignored), PCA ch3=%s" % L.pca_off_ticks(3))
    finally:
        L.reboot_and_wait()


def test_f02_launch_thread_ignores_abort():
    """F-02 engine_app.cpp:251-272 vs 293-309. The detached launch thread does not check the state: ABORT 300 ms after
    LAUNCH must keep the main valve closed. Expected: ch3 never opens. Actual: opens ~0.7 s after LAUNCH although ABORT
    was commanded. (On QEMU without sec_ec OnAbort is also a no-op - F-03 - so the symptom is shared.)"""
    try:
        L.sim_ctl("i2c adc 3 3.0")
        time.sleep(3)
        _arm_with_closed_main()
        assert L.set_mode(3), "harness: LAUNCH refused"
        time.sleep(0.3)
        L.set_mode(64)
        seen_open = False
        end = time.time() + 3.0
        while time.time() < end:
            seen_open = seen_open or L.main_valve_open()
            time.sleep(0.1)
        assert not seen_open, "F-02: main valve opened after ABORT (launch thread keeps running, engine_app.cpp:251-272)"
    finally:
        L.reboot_and_wait()


def test_f03_abort_without_sec_servo_handler_does_nothing():
    """F-03 engine_app.cpp:296-298. OnAbort returns without action when sec_servo_handler_ == nullptr (no sec_ec).
    Expected: ABORT closes the oxidizer main valve (or logs an error and still acts on the EC part).
    Actual: valve stays open."""
    try:
        _arm_with_closed_main()
        rc, out = L.someip_call(*L.SERVO_SERVICE, L.M_SET_OXI_MAIN, bytes([1]))
        assert L.wait_for(L.main_valve_open, 8), "harness: main valve did not open via SOME/IP"
        assert L.set_mode(64), "harness: ABORT refused"
        closed = L.wait_for(lambda: not L.main_valve_open(), 3)
        assert closed, "F-03: ABORT left the main valve open (sec_servo_handler_ == nullptr -> return, engine_app.cpp:296-298)"
    finally:
        L.reboot_and_wait()


def test_f37_servo_did_not_registered():
    """F-37 servo_did.h / servo_service_did.h (handlery DID nie sa tworzone w ServoService::Initialize), powiazane z F-24.
    Poprawny kontrakt: DID 0x0015 (zapis {aktuator, open_hi, open_lo, close_hi, close_lo}) zmienia konfiguracje i
    odpowiada pozytywnie (0x6E). Aktualnie: NRC 7f 2e 10 (generalReject) - handler nie istnieje."""
    r = L.uds("2E0015" + "3D" + "0083" + "005A")
    assert r[0] == 0x6E, "F-37: servo DID 0x0015 refused with %s (handler not registered, servo_did.h)" % r.hex()


def _radio_up():
    return any("RadioApp" in k for k in L.processes())


def test_f35_radioapp_survives_startup():
    """F-35 apps/ec/radio_service (RadioApp, deployment/cpu/ec/BUILD:31). The process must be running and stay up
    30 s after boot. Actual (QEMU, nondeterministic crashes: bad_alloc / SIGSEGV / terminate; without /dev/ttyS1
    Init() also calls exit(1), radio_controller.cpp:56-58): the process is gone."""
    L.wait_for(_radio_up, 40)
    time.sleep(30)
    assert _radio_up(), "F-35: RadioApp is not running 30 s after boot (crash at start / exit(1) without UART)"


def test_f15_env_app_survives_adc_nack():
    """F-15 adcsensor/controller.cpp:122. ADC NACK must not kill EnvApp (std::terminate). Expected: EnvApp alive 20 s
    after the first failed read. Actual: process gone. (Runs last: restores the guest with a reboot.)"""
    try:
        assert "EnvApp" in L.processes(), "harness: EnvApp not running before the fault"
        L.sim_ctl("i2c fault 0x48 nack")
        gone = L.wait_for(lambda: "EnvApp" not in L.processes(), 25)
        assert not gone, "F-15: EnvApp terminated after ADC NACK (adcsensor/controller.cpp:122, std::terminate)"
    finally:
        L.ensure_up()
