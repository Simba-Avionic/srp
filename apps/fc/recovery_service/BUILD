
cc_binary(
    name = "RecoveryService",
    srcs = [
        "main.cc",
        "recoveryService.cpp",
        "recoveryService.hpp",
        "service.hpp",
        "parachute_controller.cpp",
        "parachute_controller.hpp",
        "recovery_rid.hpp",
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
        "//ara/diag:uds_lib",
    ],
)
