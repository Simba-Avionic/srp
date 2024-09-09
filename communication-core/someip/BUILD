cc_library(
    name = "someip_types",
    hdrs = [
        "message_code.h",
        "message_type.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
)

cc_library(
    name = "someip_header",
    srcs = ["someip_header.cc"],
    hdrs = [
        "someip_header.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
    deps = [
        "//communication-core/network-data:network_data_structure",
        "//communication-core/network-data:network_data_type",
        "//core/common:common_converter",
    ],
)
alias(
    name = "bench",
    actual = "//communication-core/someip:someip_header",
    visibility = ["//visibility:public"],
)
cc_library(
    name = "someip_objects",
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/someip:someip_header",
        "//communication-core/someip:someip_types",
        "//communication-core/someip/factory:someip_factory_interface",
        "//communication-core/someip/factory:someip_header_factory",
        "//communication-core/someip/factory:someip_message_factory",
    ],
)
