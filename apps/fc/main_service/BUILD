cc_binary(
    name = "MainService",
    srcs = [
        "main.cc",
        "mainService.cpp",
        "mainService.hpp",
        "service.hpp",
    ],
    visibility = [
        "//deployment/apps/fc/main_app:__subpackages__",
        "//apps/fc/radio_service:__subpackages__",
        ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "@srp_platform//ara/diag:uds_lib",
        "@srp_platform//ara/log",
        "//core/common:condition_lib",
        "//deployment/apps/fc/main_app:someip_lib",
        "//deployment/apps/fc/main_app:ara",
        "//core/common:common_types",
        "//core/timestamp:timestamp_controller",
        "@srp_mavlink//lib:mavlink_lib",
        "//apps/fc/main_service:rocket_states",
    ],
)

cc_library(
    name = "rocket_states",
    deps = [
        "@srp_mavlink//lib:mavlink_lib",
    ],
    hdrs = [
        "rocket_state.h",
        "rocketController.cpp",
        "rocketController.hpp",
    ],
    visibility = ["//visibility:public"],
)