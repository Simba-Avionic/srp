load("//deployment/tools/ara:adaptive_application.bzl", "ara_runtime_lib")


cc_binary(
    name = "ServoService",
    srcs = [
        "main.cc",
        "servoService.cpp",
        "servoService.hpp",
        "servo_service_did.h",
        "service.hpp",
    ],
    visibility = [
        "//deployment/apps/servo_service:__subpackages__"
        ],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "//diag/jobs/skeleton:did_job_lib",
        "//deployment/apps/servo_service:ara",
        "//deployment/apps/servo_service:someip_lib",
    ],
)
