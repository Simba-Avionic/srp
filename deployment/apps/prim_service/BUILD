load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")


filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/prim_service:service_someipy",
    ],
    visibility = [
        "//apps/prim_service:__subpackages__",
        "//deployment/apps/engine_app:__subpackages__",
    ],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/prim_service:instance"],
    visibility = ["//apps/ec/primer_service:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/prim_service:instance"],
    visibility = ["//apps/ec/primer_service:__subpackages__"],
)

adaptive_application(
    name = "PrimerService",
    bin = "//apps/ec/primer_service:primer",
    model_src = ["//deployment/apps/prim_service:instance"],
    etcs = ["config.json"],
    visibility = ["//deployment/cpu/ec:__subpackages__"],
)

