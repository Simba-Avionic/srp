cc_library(
    name = "radio_app_lib",
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "@srp_mavlink//:simba_mavlink",
        "//deployment/apps/fc/radio_app:someip_lib",
        "//deployment/apps/fc/radio_app:ara",
        "//core/timestamp:timestamp_controller",
        "@srp_platform//ara/log",
        "//apps/fc/main_service:MainService",
        "//apps/fc/radio_service:uart_driver",
        "//core/common:wait_queue",
        "//apps/fc/recovery_service:ParachuteController",
        "//apps/ec/primer_service/controller:primer_controller",
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
        "//apps/fc/radio_service:radio_app_lib",
    ],
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