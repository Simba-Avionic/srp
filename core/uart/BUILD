
cc_library(
    name = "uart_driver",
    deps = [
        "//ara/core:Result",
        "//ara/log",
        "//ara/com:com_error_domain",
    ],
    hdrs = [
        "uart_driver.hpp",
        "Iuart_driver.hpp",
    ],
    srcs = ["uart_driver.cpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "mock_uart",
    srcs = [
        "mock_uart_driver.hpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//core/uart:uart_driver",
    ],
    testonly = True,
)