load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/example_adaptive/ExampleApp:instance"],
    visibility = ["//example_adaptive/ExampleApp/code:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/example_adaptive/ExampleApp:instance"],
    visibility = ["//example_adaptive/ExampleApp/code:__subpackages__"],
)
