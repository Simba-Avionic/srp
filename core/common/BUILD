cc_library(
    name = "core",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//core/common:common_types",
        "//core/common:condition_lib",
        "//core/common:common_converter",
        "//core/common:crc_lib",
        "//core/common:wait_queue",
    ]
)

cc_library(
    name = "common_types",
    hdrs = ["error_code.h"],
    visibility = [
        "//visibility:public",
    ],
)
cc_library(
    name = "condition_lib",
    hdrs = ["condition.h"],
    srcs = ["condition.cc"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "common_converter",
    hdrs = ["endianess_converter.h"],
    visibility = [
        "//core:__subpackages__",
        "//communication-core:__subpackages__",
    ],
)

cc_library(
    name = "crc_lib",
    srcs = ["crc_16.cc"],
    hdrs = ["crc_16.h"],
    visibility = [
        "//core/test:__subpackages__",
        ],
)

cc_library(
    name = "wait_queue",
    hdrs = ["wait_queue.h"],
    visibility = ["//visibility:public"],
)
