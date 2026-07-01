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
        "//deployment/cpu/sec_ec:sm_config",
        "platform_config.json",
    ],
    srp_components = SIMBA_COMPUTER_BASE_APPS + [
        "//deployment/apps/sec_ec/servo_service:secServoService",
        "//deployment/apps/sec_ec/env_service:secEnvApp",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
