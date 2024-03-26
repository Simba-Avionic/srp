
cc_library(
    name = "pca9685_lib",
    srcs = ["pca9685.cpp"],
    hdrs = ["pca9685.hpp"],
    visibility = [
        "//mw/i2c_service:__subpackages__",
        "//apps:__subpackages__"
        ],
    deps = [
        "//core/i2c:i2cdriver",
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/pca9685/data:servo_hdr",
        "//core/json:simba_json",
        "//mw/gpio_server/controller:gpio_controller",
    ],
)