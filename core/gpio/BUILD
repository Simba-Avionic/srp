cc_library(
    name = "gpio_interface",
    hdrs = ["IGPIO_digital_driver.h"],
    visibility = ["//core:__subpackages__"],
)

cc_library(
    name = "gpio_drivers",
    srcs = ["GPIO_digital_driver.cpp"],
    hdrs = ["GPIO_digital_driver.h"],
    visibility = [
        "//core:__subpackages__",
        "//mw:__subpackages__",
        "//apps:__subpackages__"],
    deps = [
        ":gpio_interface",
        "//core/common:common_types",
        "//ara/log",
    ],
)

cc_library(
    name = "sim_gpio_drivers",
    srcs = ["SIM_GPIO_digital_driver.cpp"],
    hdrs = ["GPIO_digital_driver.h"],
    visibility = [
        "//core:__subpackages__",
        "//mw:__subpackages__",
    ],
    deps = [
        ":gpio_interface",
        "//core/common:common_types",
        "//ara/log",
    ],
)