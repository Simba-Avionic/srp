load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/platform/diag_demon:instance"],
    visibility = ["//visibility:public"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/platform/diag_demon:instance"],
)

filegroup(
    name = "instance",
    srcs = [
        "diag_app.json",
        "//deployment/system_definition/diag/jobs:read_vin_job",
    ],
)

adaptive_application(
    name = "diag_demon",
    bin = "//platform/common/diag_demon/code/application:diag_demon",
    model_src = ["//deployment/platform/diag_demon:instance"],
    visibility = ["//visibility:public"],
)
