load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/env_service:instance"],
    visibility = ["//apps/ec/env_service:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/env_service:instance"],
    visibility = ["//apps/ec/env_service:__subpackages__"],
)

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/env_service:service_someipy",
    ],
    visibility = [
        "//apps/ec:__subpackages__",
        "//apps/env_service:__pkg__",
        "//deployment/apps/logger_service:__subpackages__",
        "//deployment/apps/fc/radio_app:__subpackages__",],
)

adaptive_application(
    name = "EnvApp",
    bin = "//apps/ec/env_service",
    etcs = ["config.json"],
    model_src = ["//deployment/apps/env_service:instance"],
    visibility = ["//deployment/cpu/ec:__subpackages__"],
)
