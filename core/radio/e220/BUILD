load("@rules_python//python:defs.bzl", "py_test")
load("@srp_platform//ara/com/mavlink:mavlink_driver_plugin.bzl", "mavlink_driver_plugin")

# Ebyte E220 modem driver as the `ebyte_e220` plugin for MavlinkDaemon (platform: IRadioDriver).
# A plugin .so has its own copies of the static libraries, so it must NOT use ara::log or the srp
# UartDriver/GpioDriver (their constructors call the host logger singleton): the UART is the platform
# SerialPort (serial_port_uart), the pins are sysfs (sysfs_e220_pins).

# Registers, radio configuration and the driver configuration file (parser, validation).
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

# IUartDriver on the platform SerialPort (no ara::log).
cc_library(
    name = "serial_port_uart",
    srcs = ["serial_port_uart.cpp"],
    hdrs = ["serial_port_uart.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        "//core/common:common_types",
        "//core/uart:Iuart_driver",
        "@srp_platform//core/serial:serial_port",
    ],
)

# IE220Pins on sysfs GPIO (no ara::log).
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
        "@srp_platform//ara/com/mavlink:mavlink_core",
    ],
)

cc_library(
    name = "e220_driver_factory",
    srcs = ["e220_driver_factory.cpp"],
    hdrs = ["e220_driver_factory.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":e220_radio_driver",
        ":e220_registers",
        ":file_e220_pins",
        ":serial_port_uart",
        ":sysfs_e220_pins",
        "@srp_platform//ara/com/mavlink:radio_driver",
    ],
)

# libebyte_e220.so (+ filegroup :ebyte_e220) goes to the MavlinkDaemon driver directory (`driver_dir`).
mavlink_driver_plugin(
    name = "ebyte_e220",
    srcs = ["e220_plugin.cc"],
    visibility = ["//visibility:public"],
    deps = [":e220_driver_factory"],
)

# Test plugin: like ebyte_e220, but with pins.ctl_dir (pins in the files of the module simulator).
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
        ":e220_driver_factory",
        ":e220_radio_driver",
        ":fake_e220_module",
        "@com_google_googletest//:gtest_main",
        "@srp_platform//ara/com/mavlink:radio_driver_contract_test",
    ],
)

# The platform IRadioDriver contract suite run on the E220 driver (fake module).
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
    name = "test_e220_io",
    srcs = ["ut/test_e220_io.cpp"],
    deps = [
        ":serial_port_uart",
        ":sysfs_e220_pins",
        "@com_google_googletest//:gtest_main",
    ],
)

# E2E: the platform MavlinkDaemon loading the plugin .so (test version: pins in the simulator files) + two IPC
# clients + pymavlink on the "station" side (a second module simulator). Module simulator: sim/e220_sim.py.
py_test(
    name = "e220_plugin_e2e_test",
    srcs = [
        "ut/e220_dialect.py",
        "ut/e220_plugin_e2e_test.py",
    ],
    main = "ut/e220_plugin_e2e_test.py",
    imports = ["ut"],
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
