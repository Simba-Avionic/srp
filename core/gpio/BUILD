cc_library(
    name = "gpio_interface",
    hdrs = [
        "Igpio_driver.hpp",
        "gpio_driver.hpp",
    ],
    visibility = ["//core:__subpackages__"],
)

cc_library(
    name = "gpio_drivers",
    srcs = ["gpio_driver.cpp"],
    visibility = [
        "//core:__subpackages__",
        "//mw:__subpackages__",
        "//apps:__subpackages__"],
    deps = [
        ":gpio_interface",
        "//core/common:common_types",
        "@srp_platform//ara/log",
        "//core/file:file_lib",
    ],
)

cc_library(
    name = "sim_gpio_drivers",
    srcs = ["sim_gpio_driver.cpp"],
    visibility = [
        "//core:__subpackages__",
        "//mw:__subpackages__",
    ],
    deps = [
        ":gpio_interface",
        "//core/common:common_types",
        "@srp_platform//ara/log",
        "//core/file:file_lib",
    ],
)