cc_binary(
    name = "ExampleApp",
    srcs = [
        "ExampleApp.cc",
        "ExampleApp.h",
        "main.cc",
    ],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//example_adaptive/ExampleApp/code:ara",
        "//core/common:condition_lib",
        "//example_adaptive/ExampleApp/code:someip_lib",
    ],
    visibility = ["//deployment/example_adaptive/ExampleApp:__pkg__"],
)
