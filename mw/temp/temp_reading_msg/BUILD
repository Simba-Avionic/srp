cc_library(
    name = "temp_reading_msg",
    srcs = ["temp_reading_msg.cpp"],
    hdrs = ["temp_reading_msg.h"],
    visibility = [
        "//mw/temp:__subpackages__",
    ],

    deps = ["//communication-core/network-data:network_data_structure",
        "//communication-core/network-data:network_data_type"
    ]
)

cc_library(
    name = "temp_reading_msg_factory",
    srcs = ["temp_reading_msg_factory.cpp"],
    hdrs = ["temp_reading_msg_factory.h"],

    visibility = [
        "//mw/temp:__subpackages__",
    ],

    deps = [":temp_reading_msg"],
)