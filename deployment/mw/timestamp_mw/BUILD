load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "ara_runtime_lib", "adaptive_application")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
    ],
    visibility = ["//mw/timestamp_mw:__subpackages__"],
)

adaptive_application(
    name = "timestamp_service",
    bin = "//mw/timestamp_mw/service:timestamp_mw",
    model_src = ["//deployment/mw/timestamp_mw:instance"],
    visibility = ["//visibility:public"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/mw/timestamp_mw:instance"],
    visibility = ["//mw/timestamp_mw/service:__subpackages__"]
)