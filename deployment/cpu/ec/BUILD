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
        "ntp_config.json",
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
        "//deployment/apps/prim_service:PrimerService",
        "//deployment/apps/fuel_service:FuelService",
        # RadioApp (apps/ec/radio_service, /dev/ttyS1 at 57600) is off the EC package: the MavlinkDaemon replaces it as the
        # owner of the radio links, and two owners of one UART are not possible. The code stays; the radio driver for
        # /dev/ttyS1 comes in srp later. Re-enable only together with a daemon config that does not use /dev/ttyS1.
        # "//deployment/apps/radio_app:RadioApp",
        "//deployment/platform/mavlink_daemon:mavlink_daemon",
    ],
)

filegroup(
    name = "config",
    srcs = ["platform_config.json"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
