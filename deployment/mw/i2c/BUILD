load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/diag/jobs/i2c_service/i2c_write:i2c_Write_job",
        "//deployment/system_definition/diag/jobs/i2c_service/i2c_read:i2c_Read_job",
    ],
    visibility = ["//mw/i2c_service:__subpackages__"],
)

adaptive_application(
    name = "i2cMWService",
    bin = "//mw/i2c_service:i2cService",
    model_src = ["//deployment/mw/i2c:instance"],
    visibility = ["//visibility:public"],
)
