cc_library(
    name = "someip_factory_interface",
    hdrs = [
        "Isomeip_header_factory.h",
        "Isomeip_message_factory.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
    deps = [
        "//communication-core/someip:someip_header",
        "//communication-core/someip:someip_types",
    ],
)
alias(
    name = "bench1",
    actual = "//communication-core/someip/factory:someip_message_factory",
    visibility = ["//visibility:public"],
)
cc_library(
    name = "someip_message_factory",
    srcs = ["someip_message_factory.cc"],
    hdrs = [
        "someip_message_factory.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
    deps = [
        ":someip_factory_interface",
        "//communication-core/someip:someip_header",
        "//communication-core/someip:someip_types",
    ],
)
alias(
    name = "bench2",
    actual = "//communication-core/someip/factory:someip_header_factory",
    visibility = ["//visibility:public"],
)
cc_library(
    name = "someip_header_factory",
    srcs = ["someip_header_factory.cc"],
    hdrs = [
        "someip_header_factory.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
    deps = [
        ":someip_factory_interface",
        "//communication-core/someip:someip_header",
        "//communication-core/someip:someip_types",
    ],
)