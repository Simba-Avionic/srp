load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/env_service:instance"],
    visibility = ["//apps/env_service:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/env_service:instance"],
    visibility = ["//apps/env_service:__subpackages__"],
)

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/env_service:service_someipy",
    ],
    visibility = [
        "//apps/env_service:__subpackages__",
        "//deployment/apps/logger_service:__subpackages__"],
)

adaptive_application(
    name = "EnvApp",
    bin = "//apps/env_service",
    model_src = ["//deployment/apps/env_service:instance"],
    visibility = ["//visibility:public"],
)
