cc_library(
    name = "uart_driver_interface",
    deps = [
        "//ara/core:Result",
        "//ara/com:com_error_domain",
    ],
    hdrs = ["Iuart_driver.hpp"],
)

cc_library(
    name = "uart_driver",
    deps = [
        "//core/uart:uart_driver_interface",
        "//ara/core:Result",
        "//ara/log",
    ],
    srcs = ["uart_driver.cpp"],
    hdrs = ["uart_driver.hpp"],
    visibility = ["//example_adaptive/ExampleApp/code/application:__subpackages__"],
)