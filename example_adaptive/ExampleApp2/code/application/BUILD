cc_binary(
    name = "ExampleApp",
    srcs = [
        "ExampleApp.cc",
        "ExampleApp.h",
        "main.cc",
    ],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//example_adaptive/ExampleApp2/code:ara",
        "//core/common:condition_lib",
        "//example_adaptive/ExampleApp2/code:someip_lib",
        "//core/ptp/timestamp:global_timestamp_controller",
    ],
    visibility = ["//deployment/example_adaptive/ExampleApp2:__pkg__"],
)
