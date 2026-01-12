load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
    ],
    visibility = [
        "//apps/gh_service:__subpackages__"
    ]
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/gh_service:instance"],
    visibility = ["//apps/ec/gh_service:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/gh_service:instance"],
    visibility = ["//apps/ec/gh_service:__subpackages__"],
)

adaptive_application(
    name = "GhApp",
    bin = "//apps/ec/gh_service",
    model_src = ["//deployment/apps/gh_service:instance"],
    visibility = ["//deployment/cpu/ec:__subpackages__"],
)