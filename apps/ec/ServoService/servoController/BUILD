cc_library(
    name = "servo_controller",
    srcs = ["servo_controller.cpp"],
    hdrs = ["servo_controller.hpp"],
    deps = [
        "//core/common:common_types",
        "//core/json:simba_json",
        "//mw/gpio_server/controller:gpio_controller",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "@srp_platform//ara/log",
    ],
    visibility = [
        "//apps:__subpackages__",
    ],
)

