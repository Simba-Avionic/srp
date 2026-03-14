load("@srp_platform//tools/common:cpu_def.bzl", "cpu_def")
load("@srp_platform//tools/model_generator/ara:adaptive.bzl", "ara_json2sm_config")
load("//deployment/cpu:defs.bzl", "SIMBA_COMPUTER_BASE_APPS")

ara_json2sm_config(
    name = "sm_config",
    src = "@srp_platform//platform_definition/sm:example_config",
)

cpu_def(
    name = "pkg",
    config = ":config",
    etcs = [
        "diag_config.json",
        "logger_config.json",
    ],
    platform_etcs = [
        "//deployment/cpu/fc:sm_config",
        "platform_config.json",
    ],
    srp_components = SIMBA_COMPUTER_BASE_APPS + [

        # "//deployment/example_adaptive/ExampleApp",
        "//deployment/apps/fc/gps_app:GPSApp",
        "//deployment/apps/fc/recovery_service:RecoveryService",
        "//deployment/apps/fc/radio_app:RadioApp",
        "//deployment/apps/fc/main_app:MainApp",
        "//deployment/apps/fc/system_stat_service:FcSysStatService",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
