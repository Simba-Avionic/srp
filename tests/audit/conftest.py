import os
import subprocess

import pytest

import audit_lib as L

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
WORK = os.environ.get("QEMU_TEST_WORK", "/tmp/qemu_test")
REUSE = (os.environ.get("QEMU_TEST_REUSE_QEMU") == "1"      # use an already running + flashed guest
         or os.environ.get("QEMU_TEST_IN_CONTAINER") == "1")  # docker: tools/qemu_test/docker/entry.sh started qemu + flashed


def _sh(cmd, timeout=1800):
    return subprocess.run(["bash", "-c", cmd], capture_output=True, text=True, timeout=timeout)


def _apps_ok(secs=5):
    return _sh("cd %s && bash tools/qemu_test/check_apps.sh %d" % (REPO, secs), 400).returncode == 0


@pytest.fixture(scope="session")
def ec():
    """The guest is started and flashed by tools/qemu_test (docker); here only wait until all EC apps are up."""
    if not REUSE:
        pytest.exit("harness: run through tools/qemu_test/qemu_test.sh (guest from the docker runner) "
                    "or set QEMU_TEST_REUSE_QEMU=1 with a running, flashed guest", returncode=2)
    L.ensure_up()
    assert _apps_ok(5), "harness: check_apps failed after ensure_up"
    yield True


@pytest.fixture(autouse=True)
def clean(request, ec):
    L.logger_stop()
    L.sim_reset()
    yield
    L.logger_stop()
    L.ssh("rm -f %s/ctl" % L.SIM)
