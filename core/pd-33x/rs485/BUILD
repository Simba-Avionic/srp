cc_library(
    name = "rs485_base",
    deps = [
        "//mw/gpio_server/controller:gpio_controller",
        "//core/uart:uart_driver",
    ],
    hdrs = ["Irs485.hpp"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "rs485_driver",
    deps = [
        "//mw/gpio_server/controller:gpio_controller",
        "//core/uart:uart_driver",
        "//core/pd-33x/rs485:rs485_base",
    ],
    srcs = ["rs485.cpp"],
    hdrs = ["rs485.hpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "mock_rs485",
    srcs = [
        "mock_rs485.hpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//core/pd-33x/rs485:rs485_base",
    ],
    testonly = True,
)