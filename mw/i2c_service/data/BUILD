cc_library(
    name = "i2c_hdr",
    srcs = [
        "header.cpp",
        ],
    hdrs = [
        "header.h",
        ],
    deps = [
        "//communication-core/network-data:network_data_structure",
        "//communication-core/network-data:network_data_type",
        "//core/logger:Logger",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "i2c_factory",
    srcs = ["i2c_factory.cpp"],
    hdrs = ["i2c_factory.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":i2c_hdr"
    ],
)