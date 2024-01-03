cc_library(
    name = "someip_interface",
    hdrs = [
        "Isomeip_controller.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
    deps = [
        "//communication-core/someip:someip_header",
        "//communication-core/someip:someip_types",
        "//communication-core/someip/factory:someip_message_factory",
        "//core",
    ],
)

cc_library(
    name = "transfer_lib",
    hdrs = ["transfer.h"],
    visibility = [
        "//core:__subpackages__",
        "//diag:__subpackages__",
    ],
    deps = ["//core/results:results_lib"],
)

cc_library(
    name = "someip_controller",
    srcs = ["someip_controller.cc"],
    hdrs = [
        "someip_controller.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
    deps = [
        ":someip_interface",
        "//communication-core/database:DatabaseLib",
        # "//communication-core/json-parser:JsonParser",
        "//communication-core/sockets:socket_interface",
        "//communication-core/someip:someip_header",
        "//communication-core/someip:someip_types",
        "//communication-core/someip/factory:someip_header_factory",
        "//communication-core/someip/factory:someip_message_factory",
        "//core",
        "//communication-core/someip-controller:transfer_lib",
    ],
)
