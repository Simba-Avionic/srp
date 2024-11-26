load("//deployment/bazel:cpu_def.bzl", "cpu_def")

cpu_def(
    name = "pkg",
    config = ":config",
    etcs = [
        "diag_config.json",
        "logger_config.json",
    ],
    srp_components = [
        # "//deployment/mw/loger/dlt_fc/fc:dlt_server",
        # "//deployment/apps/example2/example",
        # "//deployment/apps/diag_ota/ec:diag_ota",
        # "//deployment/mw/em",
        # "//deployment/mw/gpio",
        "//deployment/example_adaptive/ExampleApp",
        "//deployment/platform/em",
        "//deployment/platform/logger",
        "//deployment/platform/diag_demon",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
