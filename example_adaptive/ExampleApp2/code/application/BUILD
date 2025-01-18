cc_binary(
    name = "ExampleApp",
    srcs = [
        "ExampleApp.cc",
        "ExampleApp.h",
        "main.cc",
    ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//example_adaptive/ExampleApp2/code:ara",
        "//core/common:condition_lib",
        "//example_adaptive/ExampleApp2/code:someip_lib",
    ],
    visibility = ["//deployment/example_adaptive/ExampleApp2:__pkg__"],
)
