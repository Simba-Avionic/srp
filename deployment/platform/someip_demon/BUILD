load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application")

filegroup(
    name = "instance",
    srcs = [
        "someip_app.json",
    ],
    visibility = ["//platform/common/someip_demon/code/application:__subpackages__"]
)

adaptive_application(
    name = "someip_demon",
    bin = "//platform/common/someip_demon/code/application:someip_demon",
    model_src = ["//deployment/platform/someip_demon:instance"],
    etcs = ["endpoint_config.json"],
    visibility = ["//visibility:public"],
)
