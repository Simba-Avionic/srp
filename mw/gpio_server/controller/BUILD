cc_library(
    name = "gpio_controller",
    deps = [
        "//mw/gpio_server/data:gpio_hdr",
        "//communication-core/sockets:socket_ipc",
        "//core/gpio:gpio_drivers",
    ],
    srcs = ["gpio_controller.cpp"],
    hdrs = [
        "gpio_controller.hpp",
        "Igpio_controller.h",
        ],
    visibility = [
        "//apps:__subpackages__",
        "//mw:__subpackages__",
        ],
)
