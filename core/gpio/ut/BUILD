cc_test(
    name = "gpio_driver_test",
    srcs = ["test_gpio_driver.cpp"],
    deps = [
        "//core/gpio:gpio_drivers",
        "//core/file:mock_file",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "gpio_regression_test",
    srcs = ["test_regression.cpp"],
    deps = [
        "//core/gpio:gpio_drivers",
        "//core/file:mock_file",
        "@com_google_googletest//:gtest_main",
    ],
)
