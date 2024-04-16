cc_library(
    name = "bmp280_controller",
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//core/logger:Logger",
        "//mw/i2c_service/controller:i2c_controller",
        "//core/json:simba_json",
    ],
    srcs = ["controller.cpp"],
    hdrs = ["controller.hpp"],
    visibility = [
        "//apps:__subpackages__",
        ],
)