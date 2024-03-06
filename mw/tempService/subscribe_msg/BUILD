cc_library(
    name = "subscribe_header",
    srcs = ["subscribe_header.cpp"],
    hdrs = ["subscribe_header.h"],
    visibility = [
        "//mw/tempService:__subpackages__",
    ],

    deps = ["//communication-core/network-data:network_data_structure"]
)

cc_library(
    name = "subscribe_msg_factory",
    srcs = ["subscribe_msg_factory.cpp"],
    hdrs = ["subscribe_msg_factory.h"],

    visibility = [
        "//mw/tempService:__subpackages__",
    ],

    deps = [":subscribe_header"],
)