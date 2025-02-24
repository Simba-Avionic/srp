cc_test(
    name = "modbus_test",
    srcs = [
        "modbus_test.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/pd-33x/modbus:modbus_driver",
        "@com_google_googletest//:gtest_main",
        "//core/pd-33x/rs485:mock_rs485",
    ],
)