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
        "@srp_platform//ara/exec:adaptive_application_lib",
        "@srp_platform//ara/diag:uds_lib",
        "@srp_platform//ara/log",
        "//core/common:condition_lib",
        "//apps/ec/ServoService/servoController:servo_controller",
        "//mw/gpio_server/controller:gpio_controller",
        "//core/common:common_types",
        "//deployment/apps/fc/recovery_service:someip_lib",
        "//deployment/apps/fc/recovery_service:ara",
        "//core/timestamp:timestamp_controller",
    ],
)
