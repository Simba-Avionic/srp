load("//deployment/bazel:cpu_def.bzl", "cpu_def")

cpu_def(
    name = "pkg",
    config = ":config",
    etcs = [
        "diag_config.json",
        "logger_config.json",
    ],
    srp_components = [
        "//deployment/platform/diag_demon",
        "//deployment/platform/em",
        "//deployment/platform/logger",
        "//deployment/mw/i2c:i2cMWService",
        "//deployment/mw/gpio:gpio_service",
        "//deployment/platform/someip_demon",
        "//deployment/apps/fc/gps_app:GPSApp",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
