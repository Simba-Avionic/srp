load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

cc_binary(
    name = "system_stat_service",
    srcs = [
        "main.cpp",
        "system_stat_service.cpp",
        "system_stat_service.hpp",
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
        ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//deployment/apps/system_stat_service:ara",
        "//deployment/apps/system_stat_service:someip_lib",
        "//core/sys:system_status",
    ],
)