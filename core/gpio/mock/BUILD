cc_library(
    name = "mock_gpio",
    srcs = [
        "mock_gpio_driver.h",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//core/gpio:gpio_interface",
        "//communication-core/sockets/mock:mock_socket"
    ],
    testonly = True,
)