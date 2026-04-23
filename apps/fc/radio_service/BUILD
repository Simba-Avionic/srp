cc_binary(
    name = "radio_service",
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//apps/fc/radio_service:telemetry_provider",
        "//apps/fc/radio_service:someip_controller",
        "//deployment/apps/fc/radio_app:ara",
        "//apps/fc/radio_service:radio_controller",
        "//core/timestamp:timestamp_controller",
        "@srp_platform//ara/log",
        "//apps/fc/main_service:MainService",
    ],
    srcs = [
        "radio_app.cc",
        "main.cc",
        "gs_hb_guider.cpp",
        "radio_app.h",
        "gs_hb_guider.hpp",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "someip_controller",
    srcs = [
        "someip_controller.cpp",
    ],
    hdrs = [
        "someip_controller.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@srp_platform//ara/log",
        "//apps/fc/radio_service:event_data",
        "//deployment/apps/fc/radio_app:someip_lib",
        "//apps/fc/recovery_service:ParachuteController",
        "//apps/ec/primer_service/controller:primer_controller",
    ],
)

cc_library(
    name = "radio_controller",
    srcs = [
        "radio_controller.cpp",
    ],
    hdrs = [
        "radio_controller.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@srp_platform//ara/log",
        "//apps/fc/radio_service:event_data",
        "@srp_mavlink//:simba_mavlink",
        "//apps/fc/radio_service:uart_driver",
        "//core/common:wait_queue",
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