load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application")
load("//platforms:board.bzl", "board_config")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
    ],
    visibility = ["//mw/i2c_service:__subpackages__"],
)

board_config(name = "board_config")

adaptive_application(
    name = "i2cMWService",
    bin = "//mw/i2c_service:i2cService",
    model_src = ["//deployment/mw/i2c:instance"],
    etcs = [":board_config"],
    visibility = ["//visibility:public"],
)
