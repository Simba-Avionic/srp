cc_library(
    name = "servo_controller",
    srcs = ["servo_controller.cpp"],
    hdrs = [
        "servo_controller.hpp",
        "servo_config_reader.hpp",
        "servo_driver.hpp",
    ],
    deps = [
        "//core/common:core",
        "//mw/gpio_server/controller:gpio_controller",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "//mw/i2c_service/controller/ina219:ina219_controller",
        "@srp_platform//ara/log",
        "//apps/ec/ServoService/servoController:servo_config",
    ],
    visibility = [
        "//apps:__subpackages__",
    ],
)


cc_library(
    name = "servo_config",
    hdrs = ["servo_config_reader.hpp"],
    deps = [
        "//core/json:simba_json",
        "@srp_platform//ara/log",
        "//core/common:common_types",
    ]
)