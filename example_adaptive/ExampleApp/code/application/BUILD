cc_binary(
    name = "ExampleApp",
    srcs = [
        "ExampleApp.cc",
        "ExampleApp.h",
        "example_did.h",
        "example_service.h",
        "main.cc",
    ],
    visibility = ["//deployment/example_adaptive/ExampleApp:__pkg__"],
    deps = [
        "@srp_platform//ara/diag:uds_lib",
        "//core/uart:uart_driver",
        "@srp_platform//ara/exec:adaptive_application_lib",
        "@srp_platform//ara/log",
        "//core/common:condition_lib",
        "//example_adaptive/ExampleApp/code:ara",
        "//example_adaptive/ExampleApp/code:someip_lib",
        "@srp_platform//ara/diag:monitor",
        "//core/gps:gps_data_parser",
    ],
)
