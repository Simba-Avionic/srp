load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application")

filegroup(
    name = "instance",
    srcs = [
        "example_app.json",
        "//deployment/system_definition/diag/dtc:Example_dtc",
        "//deployment/system_definition/diag/jobs:read_vin_job",
        "//deployment/example_adaptive/ExampleApp:service_instance",
    ],
    visibility = ["//example_adaptive/ExampleApp/code:__subpackages__"],
)

filegroup(
    name = "service_instance",
    srcs = [
        "example_data_type.json",
        "example_data_type2.json",
        "example_service.json",
    ],
    visibility = ["//visibility:public"],
)

adaptive_application(
    name = "ExampleApp",
    bin = "//example_adaptive/ExampleApp/code/application:ExampleApp",
    model_src = ["//deployment/example_adaptive/ExampleApp:instance"],
    visibility = ["//visibility:public"],
)
