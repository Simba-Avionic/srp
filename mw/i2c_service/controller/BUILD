cc_library(
    name = "i2c_controller",
    deps = [
        "//core/i2c:i2cdriver",
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/data:i2c_hdr",
        "//mw/i2c_service/data:i2c_factory",
        "//core/logger:Logger",
    ],
    srcs = ["i2c_controller.cpp"],
    hdrs = [
        "Ii2c_controller.h",
        "i2c_controller.h",
        ],
    visibility = [
        "//apps:__subpackages__",
        "//mw/i2c_service/controller:__subpackages__",
        ],
)