cc_library(
    name = "pca9685_controller",
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/data:i2c_hdr",
        "//mw/i2c_service/data:i2c_factory",
        "//core/logger:Logger",
        "//mw/i2c_service/controller:i2c_controller",
        "//mw/gpio_server/controller:gpio_controller",
        "//core/json:simba_json",
    ],
    srcs = ["controller.cpp"],
    hdrs = ["controller.hpp"],
    visibility = [
        "//apps:__subpackages__",
        "//services/ServoService:__subpackages__",
        "//mw/i2c_service/tests:__subpackages__",
        ],
)