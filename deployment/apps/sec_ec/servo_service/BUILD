load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/sec_ec/servo_service:service_someipy",
    ],
    visibility = [
        "//apps/sec_ec/ServoService:__subpackages__",
        "//deployment/apps/sec_ec/servo_service:__subpackages__",
        "//deployment/apps/engine_app:__subpackages__",
        "//deployment/apps/logger_service:__subpackages__",
    ],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/sec_ec/servo_service:instance"],
    visibility = ["//apps/sec_ec/ServoService:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/sec_ec/servo_service:instance"],
    visibility = ["//apps/sec_ec/ServoService:__subpackages__"],
)

adaptive_application(
    name = "secServoService",
    bin = "//apps/sec_ec/ServoService:SecServoService",
    model_src = ["//deployment/apps/sec_ec/servo_service:instance"],
    etcs = ["config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
