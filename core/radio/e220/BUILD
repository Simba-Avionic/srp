# Ebyte E220 modem (srp): registers + E220Modem (IRadioModem) + the driver factory (RadioDriver + E220Modem).
# The driver is registered statically with MavlinkDaemon in the next layer.

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
