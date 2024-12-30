load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/platform/em:instance"],
    visibility = ["//platform/common/em/code:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/platform/em:instance"],
    visibility = ["//platform/common/em/code:__subpackages__"],
)

filegroup(
    name = "instance",
    srcs = [
        "em_app.json",
        "//deployment/system_definition/platform/sm_service:SM_service",
    ],
    visibility = ["//visibility:private"],
)

adaptive_application(
    name = "em",
    bin = "//platform/common/em/code:em",
    model_src = ["//deployment/platform/em:instance"],
    tar_path = "platform/",
    visibility = ["//visibility:public"],
)
