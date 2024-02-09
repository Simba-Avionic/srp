cc_library(
    name = "common_types",
    hdrs = ["error_code.h"],
    visibility = [
        "//visibility:public",
    ],
)

cc_library(
    name = "common_converter",
    hdrs = ["endianess_converter.h"],
    visibility = ["//core:__subpackages__"],
)

cc_library(
    name = "crc_lib",
    srcs = ["crc_16.cc"],
    hdrs = ["crc_16.h"],
    visibility = ["//core/common/test:__subpackages__"],
)

cc_library(
    name = "wait_queue",
    hdrs = ["wait_queue.h"],
    visibility = ["//visibility:public"],
)
