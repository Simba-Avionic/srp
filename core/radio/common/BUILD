# Common base of the srp radio drivers: IRadioModem (what a radio implements: blocking, sequential code)
# and RadioDriver (the one implementation of the platform IRadioDriver mechanics on top of a modem).
# Drivers live only in srp; the platform provides the IRadioDriver interface (and `serial_radio` for
# plain UART radios).

cc_library(
    name = "i_radio_modem",
    hdrs = ["i_radio_modem.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        "@srp_platform//ara/com/mavlink:radio_driver",
        "@srp_platform//ara/core:Result",
    ],
)

cc_library(
    name = "radio_driver",
    srcs = ["radio_driver.cpp"],
    hdrs = ["radio_driver.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":i_radio_modem",
        "@srp_platform//ara/com/mavlink:mavlink_core",
        "@srp_platform//ara/com/mavlink:radio_driver",
        "@srp_platform//ara/core:Result",
        "@srp_platform//ara/log",
    ],
)

# TEST ONLY: an IRadioModem with an "air" for the other end (see the file comment).
cc_library(
    name = "fake_modem",
    testonly = True,
    hdrs = ["testing/fake_modem.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        ":i_radio_modem",
        "@srp_platform//ara/com/mavlink:mavlink_core",
    ],
)

# The platform IRadioDriver contract suite on RadioDriver + FakeModem (half- and full-duplex).
cc_test(
    name = "test_radio_driver_contract",
    srcs = ["ut/test_radio_driver_contract.cpp"],
    deps = [
        ":fake_modem",
        ":radio_driver",
        "@com_google_googletest//:gtest_main",
        "@srp_platform//ara/com/mavlink:radio_driver_contract_test",
    ],
)

cc_test(
    name = "test_radio_driver",
    srcs = ["ut/test_radio_driver.cpp"],
    deps = [
        ":fake_modem",
        ":radio_driver",
        "@com_google_googletest//:gtest_main",
        "@srp_platform//ara/com/mavlink:radio_driver_contract_test",
    ],
)
