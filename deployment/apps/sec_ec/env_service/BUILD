load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/sec_ec/env_service:service_someipy",
    ],
    visibility = [
        "//apps/sec_ec/env_service:__subpackages__",
        "//deployment/apps/sec_ec/env_service:__subpackages__",
    ],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/sec_ec/env_service:instance"],
    visibility = ["//apps/sec_ec/env_service:__subpackages__"],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/sec_ec/env_service:instance"],
    visibility = ["//apps/sec_ec/env_service:__subpackages__"],
)

adaptive_application(
    name = "secEnvApp",
    bin = "//apps/sec_ec/env_service:SecEnvService",
    etcs = ["config.json"],
    model_src = ["//deployment/apps/sec_ec/env_service:instance"],
    visibility = ["//deployment/cpu:__subpackages__"],
)
