cc_library(
    name = "modbus_driver",
    deps = [
        "//mw/gpio_server/controller:gpio_controller",
        "//core/uart:uart_driver",
        "//core/pd-33x/rs485:rs485_driver",
    ],
    srcs = ["modbus.cpp"],
    hdrs = ["modbus.hpp"],
    visibility = ["//visibility:public"],
)