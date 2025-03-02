cc_test(
    name = "rs485_test",
    srcs = [
        "rs485_test.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/pd-33x/rs485:rs485_driver",
        "@com_google_googletest//:gtest_main",
        "//mw/gpio_server/controller/mock:mock_gpio_controller",
        "//core/uart:mock_uart",
    ],
)