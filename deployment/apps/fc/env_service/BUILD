load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")
load("//platforms:board.bzl", "board_config")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/fc/env_service:instance"],
    visibility = ["//apps/fc/env_service:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/fc/env_service:instance"],
    visibility = ["//apps/fc/env_service:__subpackages__"],
)

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/someip/fc/env_service:service_someipy",
    ],
    visibility = [
        "//apps/fc:__subpackages__",
        "//apps/fc/env_service:__pkg__",
        "//deployment/apps/logger_service:__subpackages__",
        "//deployment/apps/fc/radio_app:__subpackages__",
        "//deployment/apps/radio_app:__subpackages__",
        "//deployment/apps/fc/logger_service:__subpackages__",
    ],
)

board_config(name = "board_config")

adaptive_application(
    name = "EnvAppFc",
    bin = "//apps/fc/env_service:env_service_fc",
    etcs = [":board_config"],
    model_src = ["//deployment/apps/fc/env_service:instance"],
    visibility = ["//deployment/cpu/fc:__subpackages__"],
)
