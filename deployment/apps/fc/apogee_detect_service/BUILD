load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/fc/apogee_detect_service:instance"],
    visibility = ["//apps/fc/apogee_service_fc:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/fc/apogee_detect_service:instance"],
    visibility = ["//apps/fc/apogee_detect_service:__subpackages__"],
)

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/fc/apogee_detect_service:service_someipy",
        "//deployment/system_definition/someip/env_service:service_someipy",
    ],
    visibility = [
        "//apps/fc:__subpackages__",
    ],
)

adaptive_application(
    name = "ApogeeDetectAppFc",
    bin = "//apps/fc/apogee_service:apogee_service_fc",
    etcs = ["config.json"],
    model_src = ["//deployment/apps/fc/apogee_detect_service:instance"],
    visibility = ["//deployment/cpu/fc:__subpackages__"],
)
