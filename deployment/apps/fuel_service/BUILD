load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/fuel_service:instance"],
    visibility = ["//apps/ec/fuel_service:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/fuel_service:instance"],
    visibility = ["//apps/ec/fuel_service:__subpackages__"],
)

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/fuel_service:service_someipy",
    ],
    visibility = [
        "//apps/fuel_service:__pkg__",
        "//deployment/apps/engine_service:__subpackages__",
    ],
)

adaptive_application(
    name = "FuelApp",
    bin = "//apps/ec/fuel_service",
    model_src = ["//deployment/apps/fuel_service:instance"],
    visibility = ["//deployment/cpu/ec:__subpackages__"],
)