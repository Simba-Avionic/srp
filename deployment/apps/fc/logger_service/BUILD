load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/fc/logger_service:instance"],
    visibility = ["//apps/fc/logger_service:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/fc/logger_service:instance"],
    visibility = ["//apps/fc/logger_service:__subpackages__"],
)

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/fc/logger_service:service_someipy",
        "//deployment/apps/fc/env_service:instance",
        "//deployment/apps/fc/system_stat_service:instance",
    ],
    visibility = ["//visibility:public"],
)

adaptive_application(
    name = "FcFileLoggerApp",
    bin = "//apps/fc/logger_service/service:logger_service_fc",
    model_src = ["//deployment/apps/fc/logger_service:instance"],
    visibility = ["//deployment/cpu/fc:__subpackages__"],
)
