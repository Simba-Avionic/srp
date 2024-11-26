load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/platform/em:instance"],
    visibility = ["//visibility:public"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/platform/em:instance"],
)

filegroup(
    name = "instance",
    srcs = [
        "em_app.json",
    ],
    visibility = ["//example_adaptive/ExampleApp/code:__subpackages__"],
)

adaptive_application(
    name = "em",
    bin = "//platform/common/em/code:em",
    model_src = ["//deployment/platform/em:instance"],
    visibility = ["//visibility:public"],
)
