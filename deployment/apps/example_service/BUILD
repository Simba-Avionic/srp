load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")


filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
    ],
    visibility = [
        "//apps/engine_service:__subpackages__",
    ],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/example_service:instance"],
    visibility = ["//apps/ec/example_service:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/example_service:instance"],
    visibility = ["//apps/ec/example_service:__subpackages__"],
)

adaptive_application(
    name = "ExampleService",
    bin = "//apps/ec/example_service",
    model_src = ["//deployment/apps/example_service:instance"],
    visibility = ["//deployment/cpu/fc:__subpackages__"],
)

