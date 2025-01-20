load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "ara_runtime_lib")


cc_binary(
    name = "ServoService",
    srcs = [
        "main.cc",
        "servoService.cpp",
        "servoService.hpp",
        "servo_service_did.h",
        "service.hpp",
        "servo_did.h",
    ],
    visibility = [
        "//deployment/apps/servo_service:__subpackages__"
        ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "@srp_platform//ara/diag:uds_lib",
        "//deployment/apps/servo_service:ara",
        "//deployment/apps/servo_service:someip_lib",
    ],
)
