cc_library(
    name = "adc_controller",
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/controller:i2c_controller",
        "//core/json:simba_json",
    ],
    srcs = ["controller.cpp"],
    hdrs = [
        "controller.hpp",
        "Iads7828.h",
        ],
    visibility = [
        "//apps:__subpackages__",
        "//services/ServoService:__subpackages__",
        "//mw/i2c_service/tests:__subpackages__",
        "//mw/i2c_service/controller:__subpackages__"
        ],
)
