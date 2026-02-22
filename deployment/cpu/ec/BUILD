load("@srp_platform//tools/common:cpu_def.bzl", "cpu_def")
load("@srp_platform//tools/model_generator/ara:adaptive.bzl", "ara_json2sm_config")

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
    srp_components = [
        "@srp_platform//platform/common/diag_demon/model:diag_demon",
        "@srp_platform//platform/common/em/model:em",
        "@srp_platform//platform/common/logger/model:logger",
        "@srp_platform//platform/common/someip_demon/model:someip_demon",
        "@srp_platform//platform/common/state_manager/model:state_manager",
        "//deployment/example_adaptive/ExampleApp",
        "//deployment/mw/i2c:i2cMWService",
        "//deployment/mw/temp:temp_service",
        "//deployment/mw/gpio:gpio_service",
        "//deployment/apps/servo_service:servoService",
        "//deployment/apps/engine_app:EngineService",
        # "//deployment/apps/gh_service:GhApp",
        "//deployment/apps/env_service:EnvApp",
        "//deployment/apps/logger_service:FileLoggerApp",
        "//deployment/apps/prim_service:PrimerService",
        "//deployment/mw/timestamp_mw:timestamp_service",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
