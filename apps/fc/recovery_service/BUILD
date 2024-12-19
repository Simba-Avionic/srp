
cc_binary(
    name = "RecoveryService",
    srcs = [
        "main.cc",
        "recoveryService.cpp",
        "recoveryService.hpp",
        "service.hpp",
    ],
    visibility = [
        "//deployment/apps/fc/recovery_service:__subpackages__"
        ],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "//deployment/apps/fc/recovery_service:someip_lib",
        "//deployment/apps/fc/recovery_service:ara",
        "//mw/gpio_server/controller:gpio_controller",
    ],
)
