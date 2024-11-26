load("//deployment/bazel:cpu_def.bzl", "cpu_def")

cpu_def(
    name = "pkg",
    config = ":config",
    etcs = [
        "diag_config.json",
        "logger_config.json",
        "platform_config.json",
    ],
    srp_components = [
        # "//deployment/mw/em",
        # "//deployment/apps/diag_ota/ec:diag_ota",
        # # "//deployment/mw/diag/dtc:dtc_service",
        "//deployment/platform/diag_demon",
        "//deployment/platform/em",
        "//deployment/platform/logger",
        "//deployment/mw/i2c:i2cMWService",
        "//deployment/mw/temp:temp_service",
        "//deployment/mw/gpio:gpio_service",
        "//deployment/example_adaptive/ExampleApp",
        "//deployment/example_adaptive/ExampleApp2",
        "//deployment/apps/env_service:EnvApp",
        "//deployment/apps/servo_service:servoService",
        "//deployment/apps/prim_service:PrimerService",
        "//deployment/apps/engine_app:EngineService",
        "//deployment/apps/logger_service:FileLoggerApp",
        "//deployment/platform/someip_demon",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
