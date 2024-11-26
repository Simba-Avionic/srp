load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/platform/logger:instance"],
    visibility = ["//visibility:public"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/platform/logger:instance"],
)

filegroup(
    name = "instance",
    srcs = [
        "logger_app.json",
    ],
    visibility = ["//example_adaptive/ExampleApp/code:__subpackages__"],
)

adaptive_application(
    name = "logger",
    bin = "//platform/common/logger/code:logger",
    etcs = ["config.json"],
    model_src = ["//deployment/platform/logger:instance"],
    visibility = ["//visibility:public"],
)
