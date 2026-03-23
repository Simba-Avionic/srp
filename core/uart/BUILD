
cc_library(
    name = "Iuart_driver",
    hdrs = [
        "Iuart_driver.hpp",
        "uart_driver.hpp",
    ],
    deps = [
        "//core/common:core",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "uart_driver",
    deps = [
        "@srp_platform//ara/core:Result",
        "@srp_platform//ara/log",
        "@srp_platform//ara/com:com_error_domain",
        "//core/uart:Iuart_driver",
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