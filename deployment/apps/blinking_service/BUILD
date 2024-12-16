load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib")

filegroup(
    name = "instance",
    srcs = [
        "app.json",
    ],
    visibility = [],
)

adaptive_application(
    name = "blinking_service",
    bin = "//apps/blinking_service",
    model_src = ["//deployment/apps/blinking_service:instance"],
    visibility = ["//visibility:public"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/blinking_service:instance"],
    visibility = ["//apps/blinking_service:__subpackages__"],
)