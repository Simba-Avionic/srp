cc_library(
    name = "interfaces",
    hdrs = ["iframe.h"],
    visibility = ["//communication-core:__subpackages__"],
    deps = ["//core/common:common_types"],
)

cc_library(
    name = "network_data_structure",
    srcs = ["network_data_structure.cc"],
    hdrs = ["network_data_structure.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [":interfaces"],
)

cc_library(
    name = "network_data_type",
    hdrs = ["network_data_type.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":interfaces",
        "//core/common:common_converter",
    ],
)
