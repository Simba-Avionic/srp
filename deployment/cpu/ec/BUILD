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
        "//deployment/cpu/ec:sm_config",
        "platform_config.json",
    ],
    srp_components = SIMBA_COMPUTER_BASE_APPS + [
        "//deployment/apps/servo_service:servoService",
        "//deployment/apps/engine_app:EngineService",
        "//deployment/apps/env_service:EnvApp",
        "//deployment/apps/logger_service:FileLoggerApp",
        "//deployment/apps/system_stat_service:SysStatService",
        "//deployment/apps/fc/radio_app:RadioApp",
        "//deployment/apps/prim_service:PrimerService",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
