cc_library(
    name = "interfaces",
    hdrs = ["iframe.h"],
    visibility = ["//communication-core:__subpackages__"],
    deps = ["//core"],
)

cc_library(
    name = "network_data_structure",
    srcs = ["network_data_structure.cc"],
    hdrs = ["network_data_structure.h"],
     visibility = [
        "//communication-core:__subpackages__",
        "//diag/dtc/data:__subpackages__",
        "//mw/diag/dtc:__subpackages__",
        "//diag:__subpackages__",
        "//mw/tempService:__subpackages__",
        ],
    deps = [":interfaces"],
)

cc_library(
    name = "network_data_type",
    hdrs = ["network_data_type.h"],
    visibility = [
        "//communication-core:__subpackages__",
        "//diag:__subpackages__",
        "//mw/tempService:__subpackages__"
        ],
    deps = [
        ":interfaces",
        # "//core/common:common_converter"
    ],
)
