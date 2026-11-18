load("@rules_python//python:defs.bzl", "py_test")
load("@srp_platform//ara/com/mavlink:mavlink_driver_plugin.bzl", "mavlink_driver_plugin")

# Driver modemu Ebyte E220 jako plugin `ebyte_e220` dla MavlinkDaemona (platforma: IRadioDriver).
# Plugin .so ma wlasne kopie statycznych bibliotek, wiec NIE uzywa ara::log ani srp UartDriver/GpioDriver
# (ich konstruktory wolaja singleton loggera hosta): UART = termios_uart, piny = sysfs_e220_pins.

cc_library(
    name = "e220_registers",
    srcs = ["e220_registers.cpp"],
    hdrs = ["e220_registers.hpp"],
    visibility = ["//visibility:public"],
    deps = ["@com_json//:json"],
)

cc_library(
    name = "e220_pins",
    hdrs = ["i_e220_pins.hpp"],
    visibility = ["//visibility:public"],
)

# Plik konfiguracji drivera (uart, pins, e220): parser i walidacja.
cc_library(
    name = "e220_driver_config",
    srcs = ["e220_driver_config.cpp"],
    hdrs = ["e220_driver_config.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":e220_registers",
        "@com_json//:json",
    ],
)

# IUartDriver na termios + poll() bez ara::log.
cc_library(
    name = "termios_uart",
    srcs = ["termios_uart.cpp"],
    hdrs = ["termios_uart.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        "//core/common:common_types",
        "//core/uart:Iuart_driver",
    ],
)

# IE220Pins na sysfs GPIO bez ara::log.
cc_library(
    name = "sysfs_e220_pins",
    srcs = ["sysfs_e220_pins.cpp"],
    hdrs = ["sysfs_e220_pins.hpp"],
    visibility = ["//visibility:public"],
    deps = [":e220_pins"],
)

cc_library(
    name = "e220_radio_driver",
    srcs = ["e220_radio_driver.cpp"],
    hdrs = ["e220_radio_driver.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":e220_pins",
        ":e220_registers",
        "//core/common:common_types",
        "//core/uart:Iuart_driver",
        "@srp_platform//ara/com/mavlink:radio_driver",
        "@srp_platform//ara/com/mavlink:transport_error",
    ],
)

cc_library(
    name = "e220_driver_factory",
    srcs = ["e220_driver_factory.cpp"],
    hdrs = ["e220_driver_factory.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":e220_driver_config",
        ":e220_radio_driver",
        ":file_e220_pins",
        ":sysfs_e220_pins",
        ":termios_uart",
        "@srp_platform//ara/com/mavlink:radio_driver",
    ],
)

# Plik libebyte_e220.so (+ filegroup :ebyte_e220) trafia do katalogu driverow MavlinkDaemona (`driver_dir`).
mavlink_driver_plugin(
    name = "ebyte_e220",
    srcs = ["e220_plugin.cc"],
    visibility = ["//visibility:public"],
    deps = [":e220_driver_factory"],
)

# Plugin testowy: jak ebyte_e220, ale z pins.ctl_dir (piny w plikach atrapy modemu).
mavlink_driver_plugin(
    name = "ebyte_e220_test",
    testonly = True,
    srcs = ["e220_test_plugin.cc"],
    visibility = ["//visibility:public"],
    deps = [":e220_driver_factory"],
)

cc_library(
    name = "fake_e220_module",
    testonly = True,
    hdrs = ["testing/fake_e220_module.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":e220_pins",
        ":e220_registers",
        "//core/uart:Iuart_driver",
    ],
)

cc_library(
    name = "recording_listener",
    testonly = True,
    hdrs = ["testing/recording_listener.hpp"],
    visibility = ["//visibility:public"],
    deps = ["@srp_platform//ara/com/mavlink:radio_driver"],
)

cc_library(
    name = "file_e220_pins",
    hdrs = ["testing/file_e220_pins.hpp"],
    visibility = ["//visibility:public"],
    deps = [":e220_pins"],
)

cc_test(
    name = "test_e220_registers",
    srcs = ["ut/test_e220_registers.cpp"],
    deps = [
        ":e220_registers",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_e220_driver",
    srcs = ["ut/test_e220_driver.cpp"],
    deps = [
        ":e220_radio_driver",
        ":fake_e220_module",
        ":recording_listener",
        "@com_google_googletest//:gtest_main",
    ],
)

# Zestaw kontraktu IRadioDriver z platformy na drivera E220 (podstawka modulu).
cc_test(
    name = "test_e220_contract",
    srcs = ["ut/test_e220_contract.cpp"],
    deps = [
        ":e220_radio_driver",
        ":fake_e220_module",
        "@com_google_googletest//:gtest_main",
        "@srp_platform//ara/com/mavlink:radio_driver_contract_test",
    ],
)

cc_test(
    name = "test_e220_driver_config",
    srcs = ["ut/test_e220_driver_config.cpp"],
    deps = [
        ":e220_driver_config",
        ":e220_driver_factory",
        ":recording_listener",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "test_e220_io",
    srcs = ["ut/test_e220_io.cpp"],
    deps = [
        ":sysfs_e220_pins",
        ":termios_uart",
        "@com_google_googletest//:gtest_main",
    ],
)

# E2E: MavlinkDaemon z platformy ladujacy plugin .so (wersja testowa: piny w plikach atrapy) + dwa klienty IPC
# + pymavlink po stronie „stacji” (druga atrapa modulu). Atrapa modemu: sim/e220_sim.py.
py_test(
    name = "e220_plugin_e2e_test",
    srcs = ["ut/e220_plugin_e2e_test.py"],
    main = "ut/e220_plugin_e2e_test.py",
    data = [
        ":ebyte_e220_test",
        "@srp_platform//tests/mavlink:daemon_host",
    ],
    env = {
        "DAEMON_HOST": "$(rootpath @srp_platform//tests/mavlink:daemon_host)",
        "E220_PLUGIN": "$(rootpath :ebyte_e220_test)",
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

# Narzedzie E2E (testonly): E220RadioDriver na prawdziwym UART (pty atrapy) i pinach w plikach atrapy, bez daemona.
cc_binary(
    name = "e220_driver_probe",
    testonly = True,
    srcs = ["e220_driver_probe.cpp"],
    deps = [
        ":e220_radio_driver",
        ":e220_registers",
        ":file_e220_pins",
        ":termios_uart",
        "@srp_platform//ara/com/mavlink:radio_driver",
        "@srp_platform//ara/com/mavlink:transport_error",
    ],
)

# E2E serii ramek i idempotencji rejestrow z atrapa modemu (bez daemona).
py_test(
    name = "e220_driver_series_test",
    srcs = ["ut/e220_driver_series_test.py"],
    main = "ut/e220_driver_series_test.py",
    data = [
        ":e220_driver_probe",
        "ut/overflow_series_1.txt",
        "ut/overflow_series_5.txt",
    ],
    env = {"E220_PROBE": "$(rootpath :e220_driver_probe)"},
    size = "large",
    deps = ["//core/radio/e220/sim:e220_sim_lib"],
)
