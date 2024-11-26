load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application")

filegroup(
    name = "instance",
    srcs = [
        "example_app.json",
        "//deployment/example_adaptive/ExampleApp:service_instance",
        "//deployment/system_definition/diag/dtc:Example_dtc",
        "//deployment/system_definition/diag/jobs:read_vin_job",
    ],
    visibility = ["//example_adaptive/ExampleApp2/code:__subpackages__"],
)

adaptive_application(
    name = "ExampleApp2",
    bin = "//example_adaptive/ExampleApp2/code/application:ExampleApp",
    model_src = ["//deployment/example_adaptive/ExampleApp2:instance"],
    visibility = ["//visibility:public"],
)
