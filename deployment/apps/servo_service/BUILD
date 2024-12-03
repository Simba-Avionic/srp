load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")


filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/servo_service:service_someipy",
        "//deployment/system_definition/diag/jobs/servo_service:servo_status_job",
        "//deployment/system_definition/diag/jobs/servo_service:vent_servo_status_job",
    ],
    visibility = [
        "//apps/ServoService:__subpackages__",
        "//deployment/apps/servo_service:__subpackages__",
        "//deployment/apps/engine_app:__subpackages__",
        "//mw/i2c_service:__subpackages__",
        ],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/servo_service:instance"],
    visibility = ["//apps/ServoService:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/servo_service:instance"],
    visibility = ["//apps/ServoService:__subpackages__"],
)

adaptive_application(
    name = "servoService",
    bin = "//apps/ServoService",
    model_src = ["//deployment/apps/servo_service:instance"],
    etcs = ["config.json"],
    visibility = ["//visibility:public"],
)

