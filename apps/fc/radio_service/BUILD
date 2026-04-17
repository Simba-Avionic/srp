cc_library(
    name = "radio_app_lib",
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//apps/fc/radio_service:telemetry_provider",
        "//deployment/apps/fc/radio_app:someip_lib",
        "//deployment/apps/fc/radio_app:ara",
        "//core/timestamp:timestamp_controller",
        "@srp_platform//ara/log",
        "//apps/fc/main_service:MainService",
        "//apps/fc/radio_service:uart_driver",
        "//core/common:wait_queue",
        "//apps/fc/recovery_service:ParachuteController",
    ],
    srcs = [
        "radio_app.cc",
        "event_data.cc",
    ],
    hdrs = [
        "radio_app.h",
        "event_data.h",
    ],
    visibility = ["//apps/fc/radio_service:__subpackages__",],
)

cc_binary(
    name = "radio_service",
    srcs = [
        "main.cc",
    ],
    visibility = [
        "//deployment:__subpackages__",
    ],
    deps = [
        "@srp_platform//ara/log",
        "//apps/fc/radio_service:radio_app_lib",
    ],
)

cc_library(
    name = "telemetry_provider",
    srcs = [
        "telemetry_provider.cpp",
    ],
    hdrs = [
        "telemetry_provider.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@srp_mavlink//:simba_mavlink",
        "@srp_platform//ara/log",
        "//apps/fc/radio_service:event_data",
        "//core/timestamp:timestamp_controller",
        "//apps/ec/primer_service/controller:primer_controller",
    ],
)

cc_library(
    name = "event_data",
    hdrs = ["event_data.h"],
    srcs = ["event_data.cc"],
    deps = [
        "@srp_mavlink//:simba_mavlink",
        "//core/rocket_machine_state:rocket_state_base",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "uart_driver",
    hdrs = ["mavlink_uart_driver.hpp"],
    srcs = ["mavlink_uart_driver.cpp"],
    deps = [
        "//core/common:common_types"
    ],
    visibility = ["//visibility:public"],
)