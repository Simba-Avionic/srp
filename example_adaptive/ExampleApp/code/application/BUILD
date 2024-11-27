cc_binary(
    name = "ExampleApp",
    srcs = [
        "ExampleApp.cc",
        "ExampleApp.h",
        "main.cc",
        "example_service.h",
    ],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//example_adaptive/ExampleApp/code:ara",
        "//core/common:condition_lib",
        "//example_adaptive/ExampleApp/code:someip_lib",
    ],
    visibility = ["//deployment/example_adaptive/ExampleApp:__pkg__"],
)

cc_binary(
    name = "test",
    srcs = ["test.cpp"],
    data = ["test.json"],
    deps = [
        "//core/json:simba_json"
    ],
)