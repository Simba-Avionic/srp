load("//deployment/bazel:cpu_def.bzl", "cpu_def")
load("//deployment/tools/ara:adaptive.bzl", "ara_json2sm_config")

ara_json2sm_config(
    name = "sm_config",
    src = "//deployment/system_definition/sm:example_config",
)

cpu_def(
    name = "pkg",
    config = ":config",
    etcs = [
        "diag_config.json",
        "logger_config.json",
    ],
    platform_etcs = [
        "//deployment/cpu/ec:sm_config",
        "platform_config.json",
    ],
    srp_components = [
        "//deployment/platform/diag_demon",
        "//deployment/platform/em",
        "//deployment/platform/logger",
        "//deployment/platform/someip_demon",
        "//deployment/platform/state_manager",
        "//deployment/example_adaptive/ExampleApp",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
