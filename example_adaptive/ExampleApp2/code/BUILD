load("//deployment/tools/ara:adaptive_application.bzl", "ara_runtime_lib", "ara_someip_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/example_adaptive/ExampleApp2:instance"],
    visibility = ["//example_adaptive/ExampleApp2/code:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/example_adaptive/ExampleApp2:instance"],
    visibility = ["//example_adaptive/ExampleApp2/code:__subpackages__"],
)
