cc_library(
    name = "radio_app_lib",
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "@srp_mavlink//lib:mavlink_lib",
        "//deployment/apps/fc/radio_app:someip_lib",
        "//deployment/apps/fc/radio_app:ara",
        "//core/timestamp:timestamp_controller",
        "//core/uart:uart_driver"
    ],
    srcs = [
        "radio_app.cc"
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
