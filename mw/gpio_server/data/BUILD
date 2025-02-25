load("@srp_platform//tools/model_generator/ara:data_structure_generator.bzl", "data_structure_generator")
cc_library(
    name = "gpio_hdr",
    srcs = [
        "header.cpp",
        ],
    hdrs = [
        "header.hpp",
        ],
    deps = [
        "//communication-core/network-data:network_data_structure",
        "//communication-core/network-data:network_data_type",
    ],
    visibility = ["//visibility:public"],
    deprecation = "Use: //mw/gpio_server/data:gpio_com_data",
)
data_structure_generator(
    name = "gpio_com_data",
    model = [
        "//deployment/system_definition/platform/data_structure:gpio_data_structure",
    ],
    visibility = ["//visibility:public"],
)