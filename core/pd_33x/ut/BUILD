
cc_test(
    name = "pd33x_driver_test",
    srcs = [
        "pd33x_test.cc"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/pd_33x:delta_pressure_driver",
        "@com_google_googletest//:gtest_main",
        "//core/uart:mock_uart",
        "//mw/gpio_server/controller/mock:mock_gpio_controller",
    ],
)
