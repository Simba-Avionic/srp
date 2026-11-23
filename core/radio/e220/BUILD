load("@rules_python//python:defs.bzl", "py_test")
load("@srp_platform//platform/common/mavlink_daemon:defs.bzl", "mavlink_daemon_binary")

# Ebyte E220 modem as the `ebyte_e220` driver of MavlinkDaemon (platform: IRadioDriver).
# Driver = RadioDriver (core/radio/common) + E220Modem; it is linked statically into the daemon binary
# (mavlink_daemon_binary(drivers = ["//core/radio/e220:e220_driver"])) and registers itself with
# SRP_MAVLINK_REGISTER_DRIVER("ebyte_e220", ...), hence `alwayslink`.

# Registers, radio configuration and the driver configuration file (parser, validation).
cc_library(
    name = "e220_registers",
    srcs = ["e220_registers.cpp"],
    hdrs = ["e220_registers.hpp"],
    visibility = ["//visibility:public"],
    deps = ["@com_json//:json"],
)

# All knowledge about the module: IRadioModem on IUartDriver + IGpioDriver.
cc_library(
    name = "e220_modem",
    srcs = ["e220_modem.cpp"],
    hdrs = ["e220_modem.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":e220_registers",
        "//core/common:common_types",
        "//core/file:file_lib",
        "//core/gpio:gpio_interface",
        "//core/radio/common:i_radio_modem",
        "//core/uart:Iuart_driver",
        "@srp_platform//ara/com/mavlink:mavlink_core",
        "@srp_platform//ara/com/mavlink:radio_driver",
        "@srp_platform//ara/log",
    ],
)

# Driver file -> RadioDriver + E220Modem on the real UartDriver / GpioDriver (the simulations in the sim build).
cc_library(
    name = "e220_driver_factory",
    srcs = ["e220_driver_factory.cpp"],
    hdrs = ["e220_driver_factory.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":e220_modem",
        ":e220_registers",
        "//core/common:common_types",
        "//core/file:file_lib",
        "//core/gpio:gpio_interface",
        "//core/radio/common:radio_driver",
        "//core/uart:Iuart_driver",
        "//core/uart:uart_driver",
        "@srp_platform//ara/com/mavlink:radio_driver",
    ] + select({
        "//:sim": ["//core/gpio:sim_gpio_drivers"],
        "//conditions:default": ["//core/gpio:gpio_drivers"],
    }),
)

# The driver for mavlink_daemon_binary(drivers = [...]): static registration of `ebyte_e220`.
cc_library(
    name = "e220_driver",
    srcs = ["e220_driver.cc"],
    alwayslink = True,
    visibility = ["//visibility:public"],
    deps = [
        ":e220_driver_factory",
        "@srp_platform//ara/com/mavlink:radio_driver",
    ],
)

# TEST ONLY: like e220_driver, but `pins.ctl_dir` is allowed (pins in the files of the module simulator).
cc_library(
    name = "e220_test_driver",
    testonly = True,
    srcs = ["e220_test_driver.cc"],
    alwayslink = True,
    visibility = ["//visibility:public"],
    deps = [
        ":e220_driver_factory",
        ":file_gpio_driver",
        "//core/uart:uart_driver",
        "@srp_platform//ara/com/mavlink:radio_driver",
    ],
)

cc_library(
    name = "fake_e220_module",
    testonly = True,
    hdrs = ["testing/fake_e220_module.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":e220_registers",
        "//core/file:file_lib",
        "//core/gpio:gpio_interface",
        "//core/uart:Iuart_driver",
    ],
)

cc_library(
    name = "file_gpio_driver",
    testonly = True,
    hdrs = ["testing/file_gpio_driver.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        "//core/file:file_lib",
        "//core/gpio:gpio_interface",
    ],
)

cc_test(
    name = "test_e220_registers",
    srcs = ["ut/test_e220_registers.cpp"],
    deps = [
        ":e220_registers",
        "@com_google_googletest//:gtest_main",
    ],
)

# E220Modem on the fake module: Configure() sequence, idempotency, AUX gating of Transmit(), errors.
cc_test(
    name = "test_e220_modem",
    srcs = ["ut/test_e220_modem.cpp"],
    deps = [
        ":e220_modem",
        ":fake_e220_module",
        "@com_google_googletest//:gtest_main",
    ],
)

# RadioDriver + E220Modem (what the daemon runs): Start/Send/Stop, fatal errors, the factory.
cc_test(
    name = "test_e220_driver",
    srcs = ["ut/test_e220_driver.cpp"],
    deps = [
        ":e220_driver_factory",
        ":e220_modem",
        ":fake_e220_module",
        "//core/radio/common:radio_driver",
        "@com_google_googletest//:gtest_main",
        "@srp_platform//ara/com/mavlink:radio_driver_contract_test",
    ],
)

# The optional RSSI/noise statistics (stats_period_ms): the driver file, REG1 bit 5, the read command.
cc_test(
    name = "test_e220_stats",
    srcs = ["ut/test_e220_stats.cpp"],
    deps = [
        ":e220_modem",
        ":fake_e220_module",
        "//core/radio/common:radio_driver",
        "@com_google_googletest//:gtest_main",
        "@srp_platform//ara/com/mavlink:radio_driver_contract_test",
    ],
)

# The platform IRadioDriver contract suite run on RadioDriver + E220Modem (fake module).
cc_test(
    name = "test_e220_contract",
    srcs = ["ut/test_e220_contract.cpp"],
    deps = [
        ":e220_modem",
        ":fake_e220_module",
        "//core/radio/common:radio_driver",
        "@com_google_googletest//:gtest_main",
        "@srp_platform//ara/com/mavlink:radio_driver_contract_test",
    ],
)

# The MavlinkDaemon (host variant, driven through stdin/stdout) with the test E220 driver linked in.
mavlink_daemon_binary(
    name = "e220_daemon_host",
    testonly = True,
    drivers = [":e220_test_driver"],
    host = True,
)

# E2E: the platform MavlinkDaemon (statically linked E220 driver, test pins: the simulator files) + two IPC
# clients + pymavlink on the "station" side (a second module simulator). Module simulator: sim/e220_sim.py.
py_test(
    name = "e220_driver_e2e_test",
    srcs = [
        "ut/e220_dialect.py",
        "ut/e220_driver_e2e_test.py",
    ],
    main = "ut/e220_driver_e2e_test.py",
    imports = ["ut"],
    data = [
        ":e220_daemon_host",
    ],
    env = {
        "DAEMON_HOST": "$(rootpath :e220_daemon_host)",
    },
    size = "large",
    deps = [
        "//core/radio/e220/sim:e220_sim_lib",
        "@srp_platform//tests/mavlink:daemon_client",
        "@srp_platform//tests/mavlink:mav_helpers",
        "@srp_platform//third_party/python:pymavlink",
        "@srp_platform//third_party/python:pytest",
    ],
)
