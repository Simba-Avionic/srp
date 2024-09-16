load("//deployment/tools/ara:adaptive_application.bzl", "ara_runtime_lib", "adaptive_application")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/mw/i2c:instance"]
)

cc_binary(
    name = "i2cService",
    deps = [
        "//mw/i2c_service/service:i2c_lib",
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//mw/i2c_service:ara",
        ],
    srcs = ["main.cpp"],
    visibility = [
        "//deployment:__subpackages__",
    ],
)


adaptive_application(
    name = "I2CMWService",
    bin = "//mw/i2c_service:i2cService",
    model_src = ["//deployment/apps/servo_service:instance"],
    visibility = ["//visibility:public"],
)