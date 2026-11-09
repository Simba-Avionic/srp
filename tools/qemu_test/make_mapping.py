#!/usr/bin/env python3
# pylint: disable=line-too-long,missing-function-docstring,consider-using-f-string,broad-exception-caught,subprocess-run-check,unspecified-encoding,import-outside-toplevel,import-error,redefined-outer-name,unused-variable,too-many-locals
"""Builds qemu_test_results/mapping.md: F-xx -> test (file::name) -> result.
Reads gtest XML from <repo>/bazel-testlogs (targets with tag audit_bugs) and the pytest JUnit
qemu_test_results/bugs.q.xml. A FAIL counts as CONFIRMED only when its message names the F-id and is not a
harness problem; anything else is FALSE ALARM (harness/timeout)."""
import glob
import os
import re
import sys
import xml.etree.ElementTree as ET

repo = sys.argv[1] if len(sys.argv) > 1 else "."
res = os.path.join(repo, "qemu_test_results")

# F-xx without a test: reason
NO_TEST = {
    "F-07": "zamierzone (decyzja usera 2026-11-09), testy usuniete", "F-13": "zamierzone (decyzja usera 2026-11-09), testy usuniete", "F-16": "zamierzone (decyzja usera 2026-11-09), testy usuniete", "F-24": "zamierzone (decyzja usera 2026-11-09), testy usuniete",
    "F-10": "wyscig callbacks/subscribed_pins_states: deterministyczny test wymaga TSAN (brak tsan w toolchainie)",
    "F-11": "niska waga, pominiete",
    "F-17": "brak specyfikacji jednostki w naglowku CSV",
    "F-20": "niska waga, pominiete", "F-21": "niska waga, pominiete",
    "F-25": "niska waga, pominiete",
    "F-27": "TempDriver czyta stala sciezke /sys/bus/w1/devices/ - nietestowalne w UT bez zmiany kodu; w sim parser nie jest wykonywany",
    "F-28": "niska waga, kod nieuzywany w EC",
    "F-29": "niska waga, pominiete", "F-31": "niska waga, pominiete (i2c_service ReadWrite, ASAN)",
    "F-32": "niska waga, pominiete",
    "F-33": "literowka danych w configu sec_ec - brak specyfikacji wartosci oczekiwanej",
    "F-36": "blad budowania flash_ecu (:all) w srp_platform - build, nie runtime; repro: bazel build //deployment/tools/ecu:all",
    "F-38": "niska waga: ten sam pin heartbeatu GPIO 2 w RadioApp i FileLoggerApp (konfiguracja), brak oczekiwanej wartosci w sim",
    "F-39": "niska/srednia: stale w PrimerController (kFIRED zakomentowane - czesciowo pokryte przez F-06 na QEMU)",
    "F-41": "niska waga, kod utajony (WriteByte 24LC32 - brak wolajacych)",
    "F-42": "niska waga, prawdopodobne (BME280 dig_H4/H5 - zalezne od chipa)",
    "F-43": "niska waga: wymaga przechwycenia logu ara::log (LogWarn vs LogDebug)",
    "F-44": "niska waga: wymaga niedostepnego /home/root i czasu SHM (logger_service)",
}
ALL_F = ["F-%02d" % i for i in range(1, 47)]

_ul = os.path.join(res, "bugs.ut.log")
UT_LOG = open(_ul, errors="replace").read() if os.path.exists(_ul) else ""
rows = []   # (fid, test, status, detail)


def fid_of(text):
    m = re.search(r"F-?(\d\d)", text or "", re.I)
    return "F-" + m.group(1) if m else None


def add_case(label, name, failure_text, ok, skipped=False):
    fid = fid_of(name) or fid_of(failure_text)
    if fid is None:
        return
    if skipped:
        rows.append((fid, "%s::%s" % (label, name), "SKIP", ""))
    elif ok:
        rows.append((fid, "%s::%s" % (label, name), "PASS (nieoczekiwane: test powinien byc czerwony)", ""))
    else:
        t = failure_text or ""
        lines = [l.strip() for l in t.splitlines() if l.strip()]
        # the verdict is taken from the assertion message only (not from traceback source lines)
        msg = next((l for l in lines if "F-" in l or "harness:" in l), lines[0] if lines else "")
        if "harness:" in msg or not re.search(r"F-\d\d", msg):
            rows.append((fid, "%s::%s" % (label, name), "FALSE ALARM (harness/brak F-xx w komunikacie)", msg[:200]))
        else:
            rows.append((fid, "%s::%s" % (label, name), "RED (potwierdzone)", msg[:200]))


# gtest XML from bazel
for x in sorted(glob.glob(os.path.join(repo, "bazel-testlogs", "**", "test.xml"), recursive=True)):
    d = os.path.dirname(os.path.relpath(x, os.path.join(repo, "bazel-testlogs")))
    label = "//" + os.path.dirname(d) + ":" + os.path.basename(d)
    if "audit_bugs_test" not in d:
        continue
    if UT_LOG and label not in UT_LOG:
        continue
    try:
        root = ET.parse(x).getroot()
    except ET.ParseError:
        continue
    for tc in root.iter("testcase"):
        fail = tc.find("failure")
        err = tc.find("error")
        ftxt = ""
        if fail is not None:
            ftxt = (fail.get("message") or "") + "\n" + (fail.text or "")
        elif err is not None:
            ftxt = (err.get("message") or "") + "\n" + (err.text or "")
        add_case(label, tc.get("name", ""), ftxt, fail is None and err is None, tc.find("skipped") is not None)

# pytest JUnit
q = os.path.join(res, "bugs.q.xml")
if os.path.exists(q):
    for tc in ET.parse(q).getroot().iter("testcase"):
        fail = tc.find("failure")
        err = tc.find("error")
        ftxt = ""
        node = fail if fail is not None else err
        if node is not None:
            ftxt = (node.get("message") or "") + "\n" + (node.text or "")
        add_case("tests/audit/" + tc.get("classname", "").split(".")[-1] + ".py", tc.get("name", ""), ftxt,
                 node is None, tc.find("skipped") is not None)

print("# Mapowanie F-xx -> test -> wynik (audyt SRP, F3)\n")
print("RED = test asercjonuje poprawne zachowanie i pada z komunikatem F-xx = znalezisko POTWIERDZONE.\n")
print("| F-xx | test (plik::nazwa) | wynik | komunikat |")
print("|---|---|---|---|")
confirmed, unconfirmed = set(), set()
for fid, test, status, detail in sorted(rows):
    print("| %s | `%s` | %s | %s |" % (fid, test, status, detail.replace("|", "/")))
    if status.startswith("RED"):
        confirmed.add(fid)
print("\n## F-xx bez testu\n")
print("| F-xx | powod |")
print("|---|---|")
tested = {r[0] for r in rows}
for f in ALL_F:
    if f not in tested:
        reason = NO_TEST.get(f, "brak testu w tym przebiegu (patrz test_plan.md)")
        print("| %s | %s |" % (f, reason))
        unconfirmed.add(f)
print("\nPotwierdzone (czerwony test): %d - %s" % (len(confirmed), ", ".join(sorted(confirmed))))
print("Niepotwierdzone (brak testu/zielony): %d - %s" % (len(unconfirmed | (tested - confirmed)),
                                                           ", ".join(sorted(unconfirmed | (tested - confirmed)))))
