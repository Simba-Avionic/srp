load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "ara_runtime_lib", "adaptive_application")
load("//platforms:board.bzl", "board_config")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/diag/jobs/gpio_mw:gpio_mw_job",
    ],
    visibility = ["//mw/gpio_server:__subpackages__"],
)
ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/mw/gpio:instance"],
    visibility = ["//mw/gpio_server:__subpackages__"],
)

board_config(name = "board_config")

adaptive_application(
    name = "gpio_service",
    bin = "//mw/gpio_server:gpio_service",
    model_src = ["//deployment/mw/gpio:instance"],
    etcs = [":board_config"],
    visibility = [
        "//deployment/cpu/ec:__subpackages__",
        "//deployment/cpu/fc:__subpackages__",
        "//deployment/cpu/sec_ec:__subpackages__",
    ],
)
