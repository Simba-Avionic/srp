cc_binary(
    name = "SecServoService",
    srcs = [
        "main.cc",
        "servoService.cpp",
        "servoService.hpp",
        "service.hpp",
    ],
    visibility = [
        "//deployment/apps/sec_ec/servo_service:__subpackages__",
    ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//apps/ec/ServoService/servoController:servo_controller",
        "//deployment/apps/sec_ec/servo_service:ara",
        "//deployment/apps/sec_ec/servo_service:someip_lib",
        "//mw/gpio_server/controller:gpio_controller",
    ],
)
