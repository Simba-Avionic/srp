cc_library(
    name = "gpio_interface",
    hdrs = ["IGPIO_digital_driver.h"],
    visibility = ["//core:__subpackages__"],
)

cc_library(
    name = "gpio_drivers",
    srcs = ["GPIO_digital_driver.cpp"],
    hdrs = ["GPIO_digital_driver.h"],
    visibility = ["//core:__subpackages__"],
    deps = [
        ":gpio_interface",
    ],
)
