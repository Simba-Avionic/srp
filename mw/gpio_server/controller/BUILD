cc_library(
    name = "gpio_controller",
    deps = [
        "//mw/gpio_server/data:gpio_hdr",
        "//communication-core/sockets:socket_ipc",
        "//core/gpio:gpio_drivers",
        "//core/logger:Logger",
    ],
    srcs = ["gpio_controller.cpp"],
    hdrs = ["gpio_controller.hpp"],
    visibility = [
        "//apps:__subpackages__",
        ],
)