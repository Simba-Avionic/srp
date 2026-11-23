load("@rules_python//python:defs.bzl", "py_test")
load("@srp_platform//platform/common/mavlink_daemon:defs.bzl", "mavlink_daemon_binary")
load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application")

# MavlinkDaemon of the EC: the only owner of the radio links radio_a (main) and radio_b (long range, events);
# applications with `on: mavlink` are its clients. The binary is built here (mavlink_daemon_binary) with the
# radio drivers linked in statically: no *.so, no driver directory. The application model (ipc service
# MavlinkLinkService, functional groups, logger) is the platform one.
# The EC package gets /srp/opt/mavlink_daemon/{bin/mavlink_daemon, mavlink_daemon_config.json,
# serial_radio_a.json, serial_radio_b.json} (the files are also in .../etc). See README.md.
#
# Drivers linked into the daemon (`serial_radio` is always there). The links of mavlink_daemon_config.json use
# `serial_radio` (plain UART, placeholder until the real radio driver is added in srp); E220 stays linked in, but the
# config does not use it (the E220 module of the EC UART /dev/ttyS1 is not touched by the daemon).
MAVLINK_DRIVERS = ["//core/radio/e220:e220_driver"]

mavlink_daemon_binary(
    name = "mavlink_daemon_bin",
    drivers = MAVLINK_DRIVERS,
)

adaptive_application(
    name = "mavlink_daemon",
    bin = ":mavlink_daemon_bin",
    model_src = ["@srp_platform//platform/common/mavlink_daemon/model:instance"],
    etcs = [
        "serial_radio_a.json",
        "serial_radio_b.json",
        "mavlink_daemon_config.json",
    ],
    visibility = ["//deployment/cpu:__subpackages__"],
)

# The same daemon core and the same drivers without Execution Management (stdin/stdout JSON lines): for tests on a PC.
mavlink_daemon_binary(
    name = "mavlink_daemon_host",
    testonly = True,
    drivers = MAVLINK_DRIVERS,
    host = True,
)

# The real deployment config goes through the strict config parser of the daemon (links: serial_radio on a pty);
# a removed key (heartbeat_hz, version, driver_dir, restart_delay_ms) is a start error.
py_test(
    name = "config_test",
    srcs = ["ut/config_test.py"],
    data = [
        "mavlink_daemon_config.json",
        ":mavlink_daemon_host",
    ],
    env = {
        "DAEMON_HOST": "$(rootpath :mavlink_daemon_host)",
        "DEPLOY_CONFIG": "$(rootpath mavlink_daemon_config.json)",
    },
    main = "ut/config_test.py",
    size = "small",
)
