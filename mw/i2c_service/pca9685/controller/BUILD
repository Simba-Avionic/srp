cc_library(
    name = "servoController",
    deps = [
        "//core/common:common_types",
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/pca9685/data:servo_hdr",
    ],
    srcs = ["servoController.cpp"],
    hdrs = ["servoController.hpp"],
    visibility = [
        "//apps/example:__subpackages__",
        ],
)