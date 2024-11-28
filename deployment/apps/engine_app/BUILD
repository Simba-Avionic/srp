load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")


filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/engine_service:service_someipy",
        "//deployment/apps/servo_service:instance",
        "//deployment/apps/prim_service:instance",
    ],
    visibility = [
        "//apps/engine_service:__subpackages__",
    ],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/engine_app:instance"],
    visibility = ["//apps/engine_service:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/engine_app:instance"],
    visibility = ["//apps/engine_service:__subpackages__"],
)

adaptive_application(
    name = "EngineService",
    bin = "//apps/engine_service",
    model_src = ["//deployment/apps/engine_app:instance"],
    visibility = ["//visibility:public"],
)

