load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
    ],
    visibility = ["//visibility:public"],
)

adaptive_application(
    name = "test_app",
    bin = "//apps/test_app",
    model_src = ["//deployment/apps/test_app:instance"],
    visibility = ["//visibility:public"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/test_app:instance"],
    visibility = ["//apps/test_app:__subpackages__"],
)