load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")


filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/fc/sys_stat_service:sys_stat_service_someipy",
    ],
    visibility = [
        "//apps/fc/system_stat_service:__subpackages__",
        "//deployment/apps/logger_service:__subpackages__",
        ],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/fc/system_stat_service:instance"],
    visibility = ["//apps/fc/system_stat_service:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/fc/system_stat_service:instance"],
    visibility = ["//apps/fc/system_stat_service:__subpackages__"],
)

adaptive_application(
    name = "FcSysStatService",
    bin = "//apps/fc/system_stat_service",
    model_src = ["//deployment/apps/fc/system_stat_service:instance"],
    visibility = ["//deployment/cpu/fc:__subpackages__"],
)

