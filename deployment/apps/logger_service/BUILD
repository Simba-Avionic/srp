load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/logger_service:instance"],
    visibility = ["//apps/ec/logger_service:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/logger_service:instance"],
    visibility = ["//apps/ec/logger_service:__subpackages__"],
)

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/logger_service:logger_service_someipy",
        "//deployment/apps/env_service:instance",
        "//deployment/apps/system_stat_service:instance",
        "//deployment/system_definition/diag/jobs/file_logger:file_logger_job",
    ],
    visibility = ["//apps/logger_service:__subpackages__"],
)

adaptive_application(
    name = "FileLoggerApp",
    bin = "//apps/ec/logger_service/service:logger_service",
    model_src = ["//deployment/apps/logger_service:instance"],
    visibility = ["//deployment/cpu/ec:__subpackages__"],
)

